/** @file thermodynamics.h Documented includes for thermodynamics module */

#ifndef __THERMODYNAMICS__
#define __THERMODYNAMICS__

#include "background.h"

/**
 * List of possible reionization schemes.
 */

enum reionization_parametrization {
  reio_none, /**< no reionization */
  reio_camb  /**< reionization parameterized like in CAMB */
};

/**
 * Is the input parameter the reionization redshift or optical depth?
 */

enum reionization_z_or_tau {
  reio_z,  /**< input = redshift */
  reio_tau /**< input = tau */
};

/**
 * Two useful smooth step functions, for smoothing transitions in recfast.
 */

#define f1(x) (-0.75*x*(x*x/3.-1.)+0.5)  /* goes from 0 to 1 when x goes from -1 to 1 */
#define f2(x) (x*x*(0.5-x/3.)*6.)        /* goes from 0 to 1 when x goes from  0 to 1 */

/**
 * All thermodynamics parameters and evolution that other modules need to know.
 *
 * Once initialized by thermodynamics_init(), contains all the
 * necessary information on the thermodynamics, and in particular, a
 * table of thermodynamical quantities as a function of the redshift,
 * used for interpolation in other modules.
 */

struct thermo 
{
  /** @name - input parameters initialized by user in input module
   *  (all other quantitites are computed in this module, given these parameters
   *   and the content of the 'precision' and 'background' structures) */
  
  //@{

  double Tcmb; /**< \f$ T_{cmb} \f$ : current CMB temperature in Kelvins */
  double YHe;  /**< \f$ Y_{He} \f$ : primordial helium fraction */

  enum reionization_parametrization reio_parametrization; /**< reionization scheme */
  enum reionization_z_or_tau reio_z_or_tau; /**< is the input parameter the reionization redshift or optical depth? */
  double tau_reio; /**< if above set to tau, input value of reionization optical depth */
  double z_reio;   /**< if above set to z,   input value of reionization redshift */

  //@}

  /** @name - all indices for the vector of thermodynamical (=th) quantities stored in table */

  //@{

  int index_th_xe;            /**< ionization fraction \f$ x_e \f$ */
  int index_th_dkappa;        /**< Thomson scattering rate \f$ d \kappa / d \eta\f$ (units 1/Mpc) */
  int index_th_ddkappa;       /**< scattering rate derivative \f$ d^2 \kappa / d \eta^2 \f$ */
  int index_th_dddkappa;      /**< scattering rate second derivative \f$ d^3 \kappa / d \eta^3 \f$ */
  int index_th_exp_m_kappa;  /**< \f$ exp^{-\kappa} \f$ */
  int index_th_g;             /**< visibility function \f$ g = (d \kappa / d \eta) * exp^{-\kappa} \f$ */
  int index_th_dg;            /**< visibility function derivative \f$ (d g / d \eta) \f$ */
  int index_th_ddg;           /**< visibility function second derivative \f$ (d^2 g / d \eta^2) \f$ */
  int index_th_Tb;            /**< baryon temperature \f$ T_b \f$ */
  int index_th_cb2;           /**< squared baryon sound speed \f$ c_b^2 \f$ */
  int index_th_dacb2;         /**< derivative of squared baryon sound speed divided by (1+z) \f$ d [c_b^2/(1+z)] / d \eta \f$ */
  int index_th_rate;          /**< maximum variation rate of \f$ exp^{-\kappa}, g and (d g / d \eta), used for computing integration step in perturbation module */
  int th_size;                /**< size of thermodynamics vector */ 

  //@}

  /** @name - thermodynamics interpolation tables */

  //@{

  int tt_size; /**< number of lines (redshift steps) in the tables */
  double * z_table; /**< vector z_table[index_z] with values of redshift (vector of size tt_size) */
  double * thermodynamics_table; /**< table thermodynamics_table[index_z*pth->tt_size+pba->index_th] with all other quantities (array of size th_size*tt_size) */

  //@}

  /** @name - table of their second derivatives, used for spline interpolation */

  //@{

  double * d2thermodynamics_dz2_table; /**< table d2thermodynamics_dz2_table[index_z*pth->tt_size+pba->index_th] with values of \f$ d^2 t_i / dz^2 \f$ (array of size th_size*tt_size) */

  //@}

  /** @name - critical values of the redshift (used for switching on/off approximations in perturbations.c) */

  //@{

  double z_visibility_max;            /**< z at which the visibility reaches its maximum (= recombination time) */
  double z_visibility_free_streaming; /**< z below which g is so small that radiation perturbation can be approximated by free-streaming solution */

  //@}


  /** @name - conformal time and sound horizon at recombination */

  //@{

  double eta_rec; /**< conformal time at which the visibility reaches its maximum (= recombination time) */
  double rs_rec;  /**< sound horizon at that time */

  //@}

  /** @name - initial conformal time at which thermodynamical variables have been be integrated */

  //@{

  double eta_ini;

  //@}

/** @name - total number density of electrons today (free or not) */

  //@{

  double n_e;

  //@}

  /** @name - technical parameters */

  //@{

  short thermodynamics_verbose; /**< flag regulating the amount of information sent to standard output (none if set to zero) */ 

  ErrorMsg error_message; /**< zone for writing error messages */

  //@}

};

/**
 * Temporary structure where all the recombination history is defined and stored.
 *
 * This structure is used internally by the thermodynamics module, 
 * but never passed to other modules.
 */ 

struct recombination {

  /** @name - indices of vector of thermodynamics variables related to recombination */

  //@{

  int index_re_z;          /**< redshift \f$ z \f$ */
  int index_re_xe;         /**< ionization fraction \f$ x_e \f$ */
  int index_re_Tb;         /**< baryon temperature \f$ T_b \f$ */
  int index_re_cb2;        /**< squared baryon sound speed \f$ c_b^2 \f$ */
  int index_re_dkappadeta; /**< Thomson scattering rate \f$ d \kappa / d \eta\f$ (units 1/Mpc) */
  int re_size;             /**< size of this vector */

  //@}

  /** @name - table of the above variables at each redshift, and number of redshits */

  //@{

  int rt_size; /**< number of lines (redshift steps) in the table */
  double * recombination_table; /**< table recombination_table[index_z*preco->re_size+index_re] with all other quantities (array of size preco->rt_size*preco->re_size) */

  //@}

  /** @name - recfast parameters needing to be passed to
      thermodynamics_derivs_with_recfast() routine */

  //@{

  double CDB;
  double CR;
  double CK;
  double CL;
  double CT;
  double fHe;
  double CDB_He;
  double CK_He;
  double CL_He;
  double fu;
  double H_frac;
  double Tnow;
  double Nnow;
  double Bfact;
  double CB1;
  double CB1_He1;
  double CB1_He2;
  double H0; 

  //@}

};

/**
 * Temporary structure where all the reionization history is defined and stored.
 *
 * This structure is used internally by the thermodynamics module, 
 * but never passed to other modules.
 */ 

struct reionization {

  /** @name - indices of vector of thermodynamics variables related to reionization */

  //@{

  int index_re_z;          /**< redshift \f$ z \f$ */
  int index_re_xe;         /**< ionization fraction \f$ x_e \f$ */
  int index_re_Tb;         /**< baryon temperature \f$ T_b \f$ */
  int index_re_cb2;        /**< squared baryon sound speed \f$ c_b^2 \f$ */
  int index_re_dkappadeta; /**< Thomson scattering rate \f$ d \kappa / d \eta\f$ (units 1/Mpc) */
  int index_re_dkappadz;   /**< Thomson scattering rate with respect to redshift \f$ d \kappa / d z\f$ (units 1/Mpc) */
  int index_re_d3kappadz3; /**< second derivative of previous quantity with respect to redshift */
  int re_size;             /**< size of this vector */

  //@}

  /** @name - table of the above variables at each redshift, and number of redshits */

  //@{

  int rt_size;                 /**< number of lines (redshift steps) in the table */
  double * reionization_table; /**< table reionization_table[index_z*preio->re_size+index_re] with all other quantities (array of size preio->rt_size*preio->re_size) */

  //@}

  /** @name - reionization optical depth infered from reionization history */

  //@{

  double reionization_optical_depth;

  //@}

  /** @name - indices describing input parameters used in the definition of the various possible functions x_e(z) */

  //@{

  int index_reio_redshift;  /**< hydrogen reionization redshift */
  int index_reio_start;     /**< redshift above which hydrogen reionization neglected */
  int index_reio_xe_before; /**< ionization fraction at redshift 'reio_start' */
  int index_reio_xe_after;  /**< ionization fraction after full reionization */
  int index_reio_exponent;  /**< an exponent used in the function x_e(z) in the reio_camb scheme */
  int index_reio_width;     /**< a width defining the duration of hydrogen reionization in the reio_camb scheme */
  int index_helium_fullreio_fraction; /**< helium full reionization fraction infered from primordial helium fraction */
  int index_helium_fullreio_redshift; /**< helium full reionization redshift */
  int index_helium_fullreio_width;    /**< a width defining the duration of helium full reionization in the reio_camb scheme */

  //@}

  /** @name - vector of such parameters, and its size */

  double * reionization_parameters;
  int reio_num_params;

  //@}

  /** @name - index of line in recombination table corresponding to first line of reionization table */

  //@{

  int index_reco_when_reio_start;

  //@}

};

/** 
 * temporary  parameters and workspace passed to the thermodynamics_derivs function 
 */

struct thermodynamics_parameters_and_workspace {

  /* structures containing fixed input parameters (indices, ...) */
  struct background * pba;
  struct precision * ppr;
  struct recombination * preco;

  /* workspace */
  double * pvecback;

};

/**************************************************************/

/*
 * Boilerplate for C++ 
 */
#ifdef __cplusplus
extern "C" {
#endif

  int thermodynamics_at_z(
			  struct background * pba,
			  struct thermo * pth,
			  double z,
			  enum interpolation_mode intermode,
			  int * last_index,
			  double * pvecback,
			  double * pvecthermo
			  );

  int thermodynamics_init(
			  struct precision * ppr,
			  struct background * pba,
			  struct thermo * pth
			  );

  int thermodynamics_free(
			  struct thermo * pthermo
			  );

  int thermodynamics_indices(
			     struct thermo * pthermo,
			     struct recombination * preco,
			     struct reionization * preio
			     );

  int thermodynamics_reionization_function(
					   double z,
					   struct thermo * pth,
					   struct reionization * preio,
					   double * xe
					   );

  int thermodynamics_reionization(
				  struct precision * ppr,
				  struct background * pba,
				  struct thermo * pth,
				  struct recombination * preco,
				  struct reionization * preio,
				  double * pvecback
				  );

  int thermodynamics_reionization_sample(
					 struct precision * ppr,
					 struct background * pba,
					 struct thermo * pth,
					 struct recombination * preco,
					 struct reionization * preio,
					 double * pvecback
					 );
  
  int thermodynamics_get_xe_before_reionization(struct precision * ppr,
						struct thermo * pth,
						struct recombination * preco,
						double z,
						double * xe);

  int thermodynamics_recombination(
				   struct precision * ppr,
				   struct background * pba,
				   struct thermo * pth,
				   struct recombination * prec,
				   double * pvecback
				   );

  int thermodynamics_derivs_with_recfast(
					 double z,
					 double * y,
					 double * dy,
					 void * fixed_parameters,
					 ErrorMsg error_message
					 );

  int thermodynamics_merge_reco_and_reio(
					 struct precision * ppr,
					 struct thermo * pth,
					 struct recombination * preco,
					 struct reionization * preio
					 );

#ifdef __cplusplus
}
#endif

/**************************************************************/

/**  
 * @name Some basic constants needed by RECFAST:
 */

//@{

#define _m_e_ 9.10938215e-31  /**< electron mass in Kg */
#define _m_p_ 1.672621637e-27 /**< proton mass in Kg */
#define _m_H_ 1.673575e-27    /**< Hydrogen mass in Kg */
#define _not4_ 3.9715         /**< Helium to Hydrogen mass ratio */
#define _sigma_ 6.6524616e-29 /**< Thomson cross-section in m^2 */

//@}

/**  
 * @name Some specific constants needed by RECFAST:
 */

//@{

#define _RECFAST_INTEG_SIZE_ 3

#define _Lambda_ 8.2245809
#define _Lambda_He_ 51.3
#define _L_H_ion_ 1.096787737e7
#define _L_H_alpha_ 8.225916453e6
#define _L_He1_ion_ 1.98310772e7
#define _L_He2_ion_ 4.389088863e7
#define _L_He_2s_ 1.66277434e7
#define _L_He_2p_ 1.71134891e7
#define	_A2P_s_		1.798287e9     /*updated like in recfast 1.4*/
#define	_A2P_t_		177.58e0       /*updated like in recfast 1.4*/
#define	_L_He_2Pt_	1.690871466e7  /*updated like in recfast 1.4*/
#define	_L_He_2St_	1.5985597526e7 /*updated like in recfast 1.4*/
#define	_L_He2St_ion_	3.8454693845e6 /*updated like in recfast 1.4*/
#define	_sigma_He_2Ps_	1.436289e-22   /*updated like in recfast 1.4*/
#define	_sigma_He_2Pt_	1.484872e-22   /*updated like in recfast 1.4*/

//@}

/**  
 * @name Some specific constants needed by recfast_derivs:
 */

//@{

#define _a_PPB_ 4.309
#define _b_PPB_ -0.6166
#define _c_PPB_ 0.6703
#define _d_PPB_ 0.5300
#define _T_0_ pow(10.,0.477121)   /* from recfast 1.4 */
#define _a_VF_ pow(10.,-16.744)
#define _b_VF_ 0.711
#define _T_1_ pow(10.,5.114)
#define	_a_trip_ pow(10.,-16.306) /* from recfast 1.4 */
#define	_b_trip_ 0.761            /* from recfast 1.4 */

//@}

/**  
 * @name Some limits imposed on cosmological parameter values:
 */

//@{

#define _TCMB_BIG_ 2.8     /**< maximal \f$ T_{cmb} \f$ in K */
#define _TCMB_SMALL_ 2.7   /**< minimal \f$ T_{cmb}  \f$ in K */
#define _YHE_BIG_ 0.5      /**< maximal \f$ Y_{He} \f$ */
#define _YHE_SMALL_ 0.01   /**< minimal \f$ Y_{He} \f$ */

//@}

#endif
