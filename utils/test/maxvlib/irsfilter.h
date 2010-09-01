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

namespace irs {
#define UNK
double MACHEP =  1.11022302462515654042E-16;   /* 2**-53 */


template <class T>
void iir_filter(
  const irs::filter_settings_t& a_filter_settings,
  vector<T>* ap_numerators,
  vector<T>* ap_denumerators);

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

template <class T>
void iir_filter(
  const irs::filter_settings_t& a_filter_settings,
  vector<T>* ap_numerators,
  vector<T>* ap_denumerators)
{
  typedef size_t size_type;
  
  bool fsuccess = true;

  const filter_family_t family = a_filter_settings.family;
  if ((family != ff_butterworth) &&
    (family != ff_chebyshev_ripple_pass) &&
    (family != ff_cauer)) {

    fsuccess = false;
  }

  const filter_bandform_t bandform = a_filter_settings.bandform;
  if ((bandform != fb_low_pass) &&
    (bandform != fb_band_pass) &&
    (bandform != fb_high_pass) &&
    (bandform != fb_band_stop)) {

    fsuccess = false;
  }

  const size_type order = a_filter_settings.order;
  if (order <= 0) {
    fsuccess = false;
  }
  //const int kind = a_filter_settings.family + 1;
  //const int type = a_filter_settings.bandform + 1;
  T rn = a_filter_settings.order;
  const int n = rn;
  rn = n;
  T phi = 0;
  const T dbfac = 10.0 / log(10.0);
  T scale = 0;
  T eps = 0;
  if(family != ff_butterworth) /* not Butterworth */ {
    const T dbr = a_filter_settings.passband_ripple_db;
    const bool is_order_even = (order & 1) == 0;
    if(family == ff_chebyshev_ripple_pass) {
      /* For Chebyshev filter, ripples go from 1.0 to 1/sqrt(1+eps^2) */
      phi = exp( 0.5 * dbr / dbfac); 
      if(is_order_even) {
        scale = phi;
      } else {
        scale = 1.0;
      }
    } else { /* elliptic */
      eps = exp(dbr / dbfac);
      scale = 1.0;
      if(is_order_even) {
        scale = sqrt( eps );
      }
      eps = sqrt( eps - 1.0 );
    }
  }
  const T fs = 1/a_filter_settings.sampling_time_s;
  const T fnyq = fs / 2;
  T f2 = a_filter_settings.low_cutoff_freq_hz;
  T f1 = 0;
  if((bandform & 1) == 0) {
    f1 = a_filter_settings.high_cutoff_freq_hz;
  } else {
    f1 = 0.0;
  }
  T a = 0;
  T bw = 0;
  if( f2 < f1 ) {
    a = f2;
    f2 = f1;
    f1 = a;
  }
  if(bandform == 3) /* high pass */ {
    bw = f2;
    a = fnyq;
  } else {
    bw = f2 - f1;
    a = f2;
  }
  /* Frequency correspondence for bilinear transformation
   *
   *  Wanalog = tan( 2 pi Fdigital T / 2 )
   *
   * where T = 1/fs
   */
  T ang = bw * M_PI / fs;
  T cang = cos(ang);
  const T c = sin(ang) / cang; /* Wanalog */
  T wc = 0;
  if(family != ff_cauer) {
    wc = c;
    /*printf( "cos( 1/2 (Whigh-Wlow) T ) = %.5e, wc = %.5e\n", cang, wc );*/
  }

  T cgam = 0.0;
  T k = 0;
  T wr = 0;
  T cbp = 0;
  const int ARRSIZE = 50;
  vector<T> y_array(ARRSIZE);
  vector<T> aa_array(ARRSIZE);
  vector<T> pp_array(ARRSIZE);
  T m = 0;
  T Kk = 0;
  T u = 0;
  if(family == ff_cauer) { /* elliptic */
    cgam = cos( (a+f1) * M_PI / fs ) / cang;
    T dbd = a_filter_settings.stopband_ripple_db;
    T f3 = 0;
    if(dbd > 0.0) {
      f3 = dbd;
    } else { /* calculate band edge from db down */
      a = exp( -dbd/dbfac );
      T m1 = eps/sqrt( a - 1.0 );
      m1 *= m1;
      const T m1p = 1.0 - m1;
      const T Kk1 = ellpk(m1p);
      const T Kpk1 = ellpk(m1);
      const T q = exp(-M_PI * Kpk1 / (rn * Kk1));
      k = cay(q);
      if(bandform >= 3) {
        wr = k;
      } else {
        wr = 1.0/k;
      }
      if (bandform & 1) {
        f3 = atan( c * wr ) * fs / M_PI;
      } else {
        a = c * wr;
        a *= a;
        T b = a * (1.0 - cgam * cgam) + a * a;
        b = (cgam + sqrt(b))/(1.0 + a);
        f3 = (M_PI / 2.0 - asin(b)) * fs / (2.0 * M_PI);
      }
    }
    switch(bandform) {
      case 1:
        if( f3 <= f2 )
          fsuccess = false;
        break;
      case 2:
        if( (f3 > f2) || (f3 < f1) )
          break;
        fsuccess = false;
      case 3:
        if( f3 >= f2 )
          fsuccess = false;
        break;
      case 4:
        if((f3 <= f1) || (f3 >= f2))
          fsuccess = false;
        break;
    }
    ang = f3 * M_PI / fs;
    cang = cos(ang);
    T sang = sin(ang);

    if(bandform & 1) {
      wr = sang/(cang*c);
    } else {
      const T q = cang * cang  -  sang * sang;
      sang = 2.0 * cang * sang;
      cang = q;
      wr = (cgam - cang)/(sang * c);
    }

    if(bandform >= 3)
      wr = 1.0/wr;
    if(wr < 0.0)
      wr = -wr;
    y_array[0] = 1.0;
    y_array[1] = wr;
    cbp = wr;

    if(bandform >= 3)
      y_array[1] = 1.0 / y_array[1];

    if(bandform & 1) {
      for(size_t i = 1; i <= 2; i++ ) {
        aa_array[i] = atan(c * y_array[i-1]) * fs / M_PI ;
      }
      IRS_LIB_DBG_MSG("pass band " << aa_array[1]);
      IRS_LIB_DBG_MSG("stop band " << aa_array[2]);
    } else {
      for(size_t i = 1; i <= 2; i++) {
        const T a = c * y_array[i-1];
        const T b = atan(a);
        T q = sqrt(1.0 + a * a  -  cgam * cgam);
        #ifdef ANSIC
        q = atan2(q, cgam);
        #else
        q = atan2(cgam, q);
        #endif
        aa_array[i] = (q + b) * fnyq / M_PI;
        pp_array[i] = (q - b) * fnyq / M_PI;
      }
      IRS_LIB_DBG_MSG("pass band " << pp_array[1] << " " << aa_array[1]);
      IRS_LIB_DBG_MSG("stop band " << pp_array[2] << " " << aa_array[2]);
    }

    lampln(eps, wr, rn, &m, &k, &wc, &Kk, &phi, &u); /* find locations in lambda plane */
  }

  /* Transformation from low-pass to band-pass critical frequencies
   *
   * Center frequency
   *                     cos( 1/2 (Whigh+Wlow) T )
   *  cos( Wcenter T ) = ----------------------
   *                     cos( 1/2 (Whigh-Wlow) T )
   *
   *
   * Band edges
   *            cos( Wcenter T) - cos( Wdigital T )
   *  Wanalog = -----------------------------------
   *                        sin( Wdigital T )
   */

  if(family == ff_chebyshev_ripple_pass) { /* Chebyshev */
    a = M_PI * (a+f1) / fs;
    cgam = cos(a) / cang;
    a = 2.0 * M_PI * f2 / fs;
    cbp = (cgam - cos(a))/sin(a);
  }

  if(family == ff_butterworth) { /* Butterworth */
    a = M_PI * (a+f1) / fs ;
    cgam = cos(a) / cang;
    a = 2.0 * M_PI * f2 / fs;
    cbp = (cgam - cos(a))/sin(a);
    scale = 1.0;
  }
  int np = 0;
  int nz = 0;
  const int ir = 0;
  const complex<T> cone;

  vector<T> zs_array(50);
  int jt = 0;
  int zord = 0;
  T an = 0;
  T pn = 0;
  T gain = 0;
  vector<complex<T> > z_array(ARRSIZE);


  spln(family, bandform, n, rn, m, u, k, Kk, wc, phi, &nz, &np, &zs_array);
  zplna(family, bandform, np, nz, c, wc, cbp, ir, cone, cgam, zs_array, &jt, &zord, &z_array); /* convert s plane to z plane */
  zplnb(family, bandform, jt, zord, cgam, &an, &pn, &z_array, &aa_array, &pp_array, &y_array);
  zplnc(family, an, pn, scale, zord, fs, fnyq, z_array, aa_array, pp_array, &gain);
  // tabulate transfer function 
  xfun(fs, zord, gain, fnyq, dbfac, z_array);
}

} // namespace irs


#endif // iirfilterH
