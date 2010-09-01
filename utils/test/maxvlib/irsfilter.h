// Реализация БПФ фильтра
// Дата: 01.09.2010
// Дата: 01.09.2010

#ifndef irsfilterH
#define irsfilterH

#include <irsdefs.h>

#include <irscpp.h>
#include <math.h>
#include <irserror.h>
#include <irsdsp.h>

#include <irsfinal.h>

#define UNK
double MACHEP =  1.11022302462515654042E-16;   /* 2**-53 */
using namespace std;

void iir_filter(const irs::filter_settings_t& a_filter_settings);

int lampln(
  const double a_eps,
  const double a_wr,
  const double a_rn,
  double* ap_m,
  double* ap_k,
  double* ap_wc,
  double* ap_Kk,
  double* ap_phi,
  double* ap_u);

int spln(
  const int a_kind,
  const int a_type,
  const int a_n,
  const double a_rn,
  const double a_m,
  const double a_u,
  const double a_k,
  const double a_Kk,
  const double a_wc,
  const double a_phi,
  int* ap_nz,
  int* ap_np,
  vector<double>* ap_zs_array
);

int zplna(
  const int a_kind,
  const int a_type,
  const int a_np,
  const int a_nz,
  const double a_c,
  const double a_wc,
  const double a_cbp,
  const int a_ir,
  const complex<double>& a_cone,
  const double a_cgam,
  const vector<double>& a_zs_array,
  int* ap_jt,
  int* ap_zord,
  vector<complex<double> >* ap_z_array
); /* convert s plane to z plane */

int zplnb (
  const int a_kind,
  const int a_type,
  const int a_jt,
  const int a_zord,
  const double a_cgam,
  double* ap_an,
  double* ap_pn,
  vector<complex<double> >* ap_z_array,
  vector<double>* ap_aa_array,
  vector<double>* ap_pp_array,
  vector<double>* ap_y_array
);

int zplnc(
  const int a_kind,
  const double a_an,
  const double a_pn,
  const double a_scale,
  const int a_zord,
  const double a_fs,
  const double a_fnyq,
  const vector<complex<double> >& a_z_array,
  const vector<double>& a_aa_array,
  const vector<double>& a_pp_array,
  double* ap_gain
);

int quadf(
  const double a_x,
  const double a_y,
  const int a_pzflg /* 1 if poles, 0 if zeros */,
  const double a_fs,
  const double a_fnyq
);

int xfun(
  const double a_fs,
  const double a_zord,
  const double a_gain,
  const double a_fnyq,
  const double a_dbfac,
  const vector<complex<double> >& a_z_array
);

double response(
  const double a_f,
  const double a_fs,
  const double a_amp,
  const int a_zord,
  const vector<complex<double> >& a_z_array);

void csqrt(const complex<double>& z, complex<double>* ap_w);

int ellpj(
  const double a_u,
  const double a_m,
  double* ap_sn,
  double* ap_cn,
  double* ap_dn,
  double* ap_ph
);

double ellpk(const double a_x);

double ellik(const double a_phi, const double a_m);

double polevl(const double a_x, const double* ap_coef, const int a_n);

double cay(const double a_q);

#endif // iirfilterH
