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
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {
#define UNK
#define MACHEP 1.11022302462515654042E-16   /* 2**-53 */

template <class T>
class iir_filter_t
{
public:
  typedef T value_type;
  typedef irs_size_t size_type;
  iir_filter_t();
  typedef deque<T> coef_list_type;
  typedef deque<T> delay_line_type;
  void set_filter_settings(const irs::filter_settings_t& a_filter_setting);
  template <class COEF_LIST_TYPE>
  void set_coefficiens(const COEF_LIST_TYPE& a_num_coef_list,
    const COEF_LIST_TYPE& a_denum_coef_list);
  void sync(T a_value);
  T filt(const T a_sample);
  void set(const T a_sample);
  T get() const;
  void reset();
private:
  coef_list_type m_num_coef_list;
  coef_list_type m_denum_coef_list;

  delay_line_type m_in_delay_line;
  delay_line_type m_out_delay_line;
};

template <class coef_list_type>
bool get_coef_iir_filter(
  const irs::filter_settings_t& a_filter_settings,
  coef_list_type* ap_num_coef_list,
  coef_list_type* ap_denum_coef_list);

template <class T>
int lampln(
  const T a_eps,
  const T a_wr,
  const T a_rn,
  T* ap_m,
  T* ap_k,
  T* ap_wc,
  T* ap_Kk,
  T* ap_phi,
  T* ap_u);

template <class T>
int spln(
  const filter_family_t a_family,
  const filter_bandform_t a_bandform,
  const size_t a_order,
  const T a_m,
  const T a_u,
  const T a_k,
  const T a_Kk,
  const T a_wc,
  const T a_phi,
  size_t* ap_nz,
  size_t* ap_np,
	vector<T>* ap_zs_array
);

template <class T>
int zplna(
  const filter_family_t a_family,
  const filter_bandform_t a_bandform,
  const int a_np,
  const int a_nz,
  const T a_c,
  const T a_wc,
	const T a_cbp,
  const int a_ir,
  const complex<T>& a_cone,
  const T a_cgam,
  const vector<T>& a_zs_array,
  int* ap_jt,
  int* ap_zord,
  vector<complex<T> >* ap_z_array
); /* convert s plane to z plane */

template <class T>
int zplnb (
  const filter_family_t a_family,
  const filter_bandform_t a_bandform,
  const size_t a_jt,
  const size_t a_zord,
  const T a_cgam,
  T* ap_an,
  T* ap_pn,
  vector<complex<T> >* ap_z_array,
  vector<T>* ap_aa_array,
  vector<T>* ap_pp_array,
  vector<T>* ap_y_array
);

template <class T>
int zplnc(
  const filter_family_t a_family,
  const T a_an,
  const T a_pn,
  const T a_scale,
  const size_t a_zord,
  const T a_fs,
  const T a_fnyq,
  const vector<complex<T> >& a_z_array,
  const vector<T>& a_aa_array,
  vector<T>* ap_pp_array,
  T* ap_gain
);

template <class T>
int quadf(
  const T a_x,
  const T a_y,
  const int a_pzflg /* 1 if poles, 0 if zeros */,
  const T a_fs,
  const T a_fnyq
);

template <class T>
int xfun(
  const T a_fs,
  const size_t a_zord,
  const T a_gain,
  const T a_fnyq,
  const T a_dbfac,
  const vector<complex<T> >& a_z_array
);

template <class T>
T response(
  const T a_f,
  const T a_fs,
  const T a_amp,
  const size_t a_zord,
  const vector<complex<T> >& a_z_array);

template <class T>
void csqrt(const complex<T>& z, complex<T>* ap_w);

template <class T>
int ellpj(
  const T a_u,
  const T a_m,
  T* ap_sn,
  T* ap_cn,
  T* ap_dn,
  T* ap_ph
);

template <class T>
T ellpk(const T a_x);

template <class T>
T ellik(const T a_phi, const T a_m);

template <class T>
T polevl(const T a_x, const double* ap_coef, const int a_n);

template <class T>
T cay(const T a_q);

template <class T>
iir_filter_t<T>::iir_filter_t():
  m_num_coef_list(),
  m_denum_coef_list(),
  m_in_delay_line(),
  m_out_delay_line()
{
}

template <class T>
void iir_filter_t<T>::set_filter_settings(
  const irs::filter_settings_t& a_filter_setting)
{
  get_coef_iir_filter(a_filter_setting, &m_num_coef_list, &m_denum_coef_list);
  m_denum_coef_list.pop_front();
  m_in_delay_line.resize(m_num_coef_list.size(), 0);
  m_out_delay_line.resize(m_denum_coef_list.size(), 0);
}

template <class T>
template <class COEF_LIST_TYPE>
void iir_filter_t<T>::set_coefficiens(const COEF_LIST_TYPE& a_num_coef_list,
  const COEF_LIST_TYPE& a_denum_coef_list)
{
  IRS_LIB_ASSERT(a_num_coef_list.size() == a_denum_coef_list.size());
  m_num_coef_list.clear();
  a_denum_coef_list.clear();
  m_num_coef_list.insert(m_num_coef_list.begin(), a_num_coef_list.begin(),
    a_num_coef_list.end());
  a_denum_coef_list.insert(a_denum_coef_list.begin(),
    a_denum_coef_list.begin(), a_denum_coef_list.end());
}

template <class T>
void iir_filter_t<T>::sync(T a_value)
{
  const size_type size = m_in_delay_line.size();
  m_in_delay_line.clear();
  m_out_delay_line.clear();
  m_in_delay_line.resize(m_in_delay_line.size(), a_value);
  m_out_delay_line.resize(m_out_delay_line.size(), a_value);
}

template <class T>
T iir_filter_t<T>::filt(const T a_sample)
{
  set(a_sample);
  return m_out_delay_line.front();
}

template <class T>
void iir_filter_t<T>::set(const T a_sample)
{
  // y[k] = a[0]*x[k] + a[1]*x[k-1] + a[order]*x[k-order] - b[1]*y[k-1] -
  // b[2]*y[k-2] + b[order]*y[k-order], где order - порядок фильтра,
  // k - номер текущего отсчета, a и b - коэффициенты фильтра,
  // x - линия задержки входных отсчетов,
  // y - линия задержки выходных отсчетов
  const size_type delay_line_size = m_in_delay_line.size();
  m_in_delay_line.pop_back();
  m_in_delay_line.push_front(a_sample);
  T sum_in = 0;
  T sub_out = 0;
  for (size_type i = 0; i < delay_line_size - 1; i++) {
    sum_in += m_in_delay_line[i]*m_num_coef_list[i];
    sub_out -= m_out_delay_line[i]*m_denum_coef_list[i];
  }
  sum_in += m_in_delay_line[delay_line_size - 1]*
    m_num_coef_list[delay_line_size - 1];
  m_out_delay_line.pop_back();
  m_out_delay_line.push_front(sum_in + sub_out);
}

template <class T>
T iir_filter_t<T>::get() const
{
  IRS_LIB_ERROR_IF(m_out_delay_line.empty(), irs::ec_standard,
    "Сначало необходимо инициализировать фильтр");
  return m_out_delay_line.front();
}

template <class T>
void iir_filter_t<T>::reset()
{
  sync(0);
}

template <class coef_list_type>
bool get_coef_iir_filter(
  const irs::filter_settings_t& a_filter_settings,
  coef_list_type* ap_num_coef_list,
  coef_list_type* ap_denum_coef_list)
{
  typedef coef_list_type::value_type value_type;
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
  if (order <= 1) {
    fsuccess = false;
  }
  value_type phi = 0;
  const value_type dbfac = 10.0 / log(10.0);
  value_type scale = 0;
  value_type eps = 0;
  if(family != ff_butterworth) /* not Butterworth */ {
    const value_type dbr = a_filter_settings.passband_ripple_db;
    if( dbr <= 0.0 ) {
      fsuccess = false;
    }
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
  const value_type fs = 1/a_filter_settings.sampling_time_s;
  if(fs <= 0.0) {
    fsuccess =false;
  }
  const value_type fnyq = fs / 2;
  value_type f2 = a_filter_settings.low_cutoff_freq_hz;
  if((f2 <= 0.0) || (f2 >= fnyq)) {
    fsuccess = false;
  }
  value_type f1 = 0;
  if((bandform == fb_band_pass) || (bandform == fb_band_stop)) {
    f1 = a_filter_settings.high_cutoff_freq_hz;
    if((f1 <= 0.0) || (f1 >= fnyq)) {
      fsuccess = false;
    }
  } else {
    f1 = 0.0;
  }
  value_type a = 0;
  value_type bw = 0;
  if( f2 < f1 ) {
    a = f2;
    f2 = f1;
    f1 = a;
  }
  if(bandform == fb_high_pass) /* high pass */ {
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
  value_type ang = bw * M_PI / fs;
  value_type cang = cos(ang);
  const value_type c = sin(ang) / cang; /* Wanalog */
  value_type wc = 0;
  if(family != ff_cauer) {
    wc = c;
    IRS_LIB_DBG_MSG("cos( 1/2 (Whigh-Wlow) T ) = " << cang << " wc = " << wc);
  }

  value_type cgam = 0.0;
  value_type k = 0;
  value_type wr = 0;
  value_type cbp = 0;
  const int ARRSIZE = 50;
  vector<value_type> y_array(ARRSIZE);
  //ap_num_coef_list->resize(ARRSIZE);
  vector<value_type> aa_array(ARRSIZE);
  //ap_denum_coef_list->resize(ARRSIZE);
  vector<value_type> pp_array(ARRSIZE);
  value_type m = 0;
  value_type Kk = 0;
  value_type u = 0;
  if(family == ff_cauer) { /* elliptic */
    cgam = cos( (a+f1) * M_PI / fs ) / cang;
    value_type dbd = a_filter_settings.high_cutoff_freq_hz;
    value_type f3 = 0;
    if(dbd > 0.0) {
      f3 = dbd;
    } else { /* calculate band edge from db down */
      a = exp(-dbd/dbfac);
      value_type m1 = eps/sqrt( a - 1.0 );
      m1 *= m1;
      const value_type m1p = 1.0 - m1;
      const value_type Kk1 = ellpk(m1p);
      const value_type Kpk1 = ellpk(m1);
      const value_type q = exp(-M_PI * Kpk1 / (order * Kk1));
      k = cay(q);
      if((bandform == fb_high_pass) || (bandform == fb_band_stop)) {
        wr = k;
      } else {
        wr = 1.0/k;
      }
      if ((bandform == fb_low_pass) || (bandform == fb_high_pass)) {
        f3 = atan( c * wr ) * fs / M_PI;
      } else {
        a = c * wr;
        a *= a;
        value_type b = a * (1.0 - cgam * cgam) + a * a;
        b = (cgam + sqrt(b))/(1.0 + a);
        f3 = (M_PI / 2.0 - asin(b)) * fs / (2.0 * M_PI);
      }
    }
    switch(bandform) {
      case fb_low_pass:
        if(f3 <= f2 )
          fsuccess = false;
        break;
      case fb_band_pass:
        if((f3 > f2) || (f3 < f1))
          break;
        fsuccess = false;
      case fb_high_pass:
        if(f3 >= f2 )
          fsuccess = false;
        break;
      case fb_band_stop:
        if((f3 <= f1) || (f3 >= f2))
          fsuccess = false;
        break;
    }
    ang = f3 * M_PI / fs;
    cang = cos(ang);
    value_type sang = sin(ang);

    if((bandform == fb_low_pass) || (bandform == fb_high_pass)) {
      wr = sang/(cang*c);
    } else {
      const value_type q = cang * cang  -  sang * sang;
      sang = 2.0 * cang * sang;
      cang = q;
      wr = (cgam - cang)/(sang * c);
    }

    if((bandform == fb_high_pass) || (bandform == fb_band_stop)) {
      wr = 1.0/wr;
    }
    if(wr < 0.0)
      wr = -wr;
    y_array[0] = 1.0;
    y_array[1] = wr;
    cbp = wr;

    if((bandform == fb_high_pass) || (bandform == fb_band_stop)) {
      y_array[1] = 1.0/y_array[1];
    }

    if((bandform == fb_low_pass) || (bandform == fb_high_pass)) {
      for(size_t i = 1; i <= 2; i++ ) {
        aa_array[i] = atan(c*y_array[i-1])*fs/M_PI;
      }
      IRS_LIB_DBG_MSG("pass band " << aa_array[1]);
      IRS_LIB_DBG_MSG("stop band " << aa_array[2]);
    } else {
      for(size_t i = 1; i <= 2; i++) {
        const value_type a = c * y_array[i-1];
        const value_type b = atan(a);
        value_type q = sqrt(1.0 + a*a  -  cgam*cgam);
        #ifdef ANSIC
        q = atan2(q, cgam);
        #else
        q = atan2(cgam, q);
        #endif
        aa_array[i] = (q + b) * fnyq / M_PI;
        pp_array[i] = (q - b) * fnyq / M_PI;
      }
      IRS_LIB_DBG_MSG("pass band " << pp_array[1] << " " <<
        aa_array[1]);
      IRS_LIB_DBG_MSG("stop band " << pp_array[2] << " " <<
        aa_array[2]);
    }

    lampln(eps, wr, order, &m, &k, &wc, &Kk, &phi, &u); /* find locations in lambda plane */
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
  size_type np = 0;
  size_type nz = 0;
  const int ir = 0;
  const complex<value_type> cone(1.0, 0.0);

  vector<value_type> zs_array(50);
  int jt = 0;
  int zord = 0;
  value_type an = 0;
  value_type pn = 0;
  value_type gain = 0;
  vector<complex<value_type> > z_array(ARRSIZE);


  spln(family, bandform, order, m, u, k, Kk, wc, phi, &nz,
    &np, &zs_array);
  zplna(family, bandform, np, nz, c, wc, cbp, ir, cone, cgam, zs_array,
    &jt, &zord, &z_array); /* convert s plane to z plane */
  zplnb(family, bandform, jt, zord, cgam, &an, &pn, &z_array,
    &aa_array, &pp_array, &y_array);
  zplnc(family, an, pn, scale, zord, fs, fnyq, z_array,
    aa_array, &pp_array, &gain);
  ap_num_coef_list->clear();
  ap_num_coef_list->insert(ap_num_coef_list->begin(),
    pp_array.begin(), pp_array.begin() + zord + 1);
  ap_denum_coef_list->insert(ap_denum_coef_list->begin(),
    aa_array.begin(), aa_array.begin() + zord + 1);
  // tabulate transfer function
  xfun(fs, zord, gain, fnyq, dbfac, z_array);
  return fsuccess;
}

template <class T>
int lampln(
  const T a_eps,
  const T a_wr,
  const a_rn,
  T* ap_m,
  T* ap_k,
  T* ap_wc,
  T* ap_Kk,
  T* ap_phi,
  T* ap_u)
{
  *ap_wc = 1.0;
  *ap_k = *ap_wc / a_wr;
  *ap_m = *ap_k * *ap_k;
  *ap_Kk = ellpk(1.0 - *ap_m);
  const T Kpk = ellpk(*ap_m);
  const T q = exp(-M_PI * a_rn * Kpk / *ap_Kk);	/* the nome of k1 */
  T m1 = cay(q); /* see below */
  /* Note m1 = eps / sqrt( A*A - 1.0 ) */
  T a = a_eps/m1;
  a =  a * a + 1;
  a = 10.0 * log(a) / log(10.0);
  IRS_LIB_DBG_MSG("dbdown " << a );
  a = 180.0 * asin(*ap_k) / M_PI;
  T b = 1.0/(1.0 + a_eps*a_eps);
  b = sqrt( 1.0 - b );
  IRS_LIB_DBG_MSG("theta " << a << " rho " << b );
  m1 *= m1;
  const T m1p = 1.0 - m1;
  const T Kk1 = ellpk(m1p);
  const T Kpk1 = ellpk(m1);
  const T r = Kpk1 * *ap_Kk / (Kk1 * Kpk);
  IRS_LIB_DBG_MSG("consistency check: n= " << r);
  /*   -1
   * sn   j/eps\m  =  j ellik( atan(1/eps), m )
   */
  b = 1.0 / a_eps;
  *ap_phi = atan(b);
  *ap_u = ellik(*ap_phi, m1p);
  IRS_LIB_DBG_MSG("phi " << *ap_phi <<" m " << m1p << " u " << *ap_u);
  /* consistency check on inverse sn */
  T sn = 0;
  T cn = 0;
  T dn = 0;
  ellpj(*ap_u, m1p, &sn, &cn, &dn, ap_phi);
  a = sn/cn;
  IRS_LIB_DBG_MSG("consistency check: sn/cn =" << a << "=" << b << "= 1/eps");
  *ap_u = *ap_u * *ap_Kk / (a_rn * Kk1);	/* or, u = u * Kpk / Kpk1 */
  return 0;
}

/* calculate s plane poles and zeros, normalized to wc = 1 */
template <class T>
int spln(
  const filter_family_t a_family,
  const filter_bandform_t a_bandform,
  const size_t a_order,
  const T a_m,
  const T a_u,
  const T a_k,
  const T a_Kk,
  const T a_wc,
  const T a_phi,
  size_t* ap_nz,
  size_t* ap_np,
  vector<T>* ap_zs_array)
{
  for(size_t i = 0; i < ap_zs_array->size(); i++) {
    (*ap_zs_array)[i] = 0.0;
  }
  *ap_np = (a_order + 1) / 2;
  *ap_nz = 0;
  switch (a_family) {
    case ff_butterworth: {
      /* Butterworth poles equally spaced around the unit circle
      */
      T m = 0;
      if(a_order & 1) {
        m = 0.0;
      } else {
        m = M_PI / (2.0 * a_order);
      }
      for(size_t i = 0; i < *ap_np; i++) { /* poles */
        const int lr = i + i;
        (*ap_zs_array)[lr] = -cos(m);
        (*ap_zs_array)[lr+1] = sin(m);
        m += M_PI / a_order;
      }
      /* high pass or band reject
      */
      if((a_bandform == fb_high_pass) || (a_bandform == fb_band_stop)) {
        /* map s => 1/s
        */
        int ii = 0;
        for(size_t j = 0; j < *ap_np; j++) {
          const int ir = j + j;
          ii = ir + 1;
          const T b = (*ap_zs_array)[ir] * (*ap_zs_array)[ir] +
            (*ap_zs_array)[ii] * (*ap_zs_array)[ii];
          (*ap_zs_array)[ir] = (*ap_zs_array)[ir] / b;
          (*ap_zs_array)[ii] = (*ap_zs_array)[ii] / b;
        }
        /* The zeros at infinity map to the origin.
         */
        *ap_nz = *ap_np;
        if(a_bandform == fb_band_stop) {
          *ap_nz += a_order/2;
        }
        for(size_t j = 0; j < *ap_nz; j++) {
          const int ir = ii + 1;
          ii = ir + 1;
          (*ap_zs_array)[ir] = 0.0;
          (*ap_zs_array)[ii] = 0.0;
        }
      }
    } break;
    case ff_chebyshev_ripple_pass: {
      /* For Chebyshev, find radii of two Butterworth circles
      * See Gold & Rader, page 60
      */
      const T rho = (a_phi - 1.0) * (a_phi + 1);  /* rho = eps^2 = {sqrt(1+eps^2)}^2 - 1 */
      const T eps = sqrt(rho);
      /* sqrt( 1 + 1/eps^2 ) + 1/eps  = {sqrt(1 + eps^2)  +  1} / eps
      */
      T phi = (a_phi + 1.0) / eps;
      /* raise to the 1/n power */
      phi = pow(phi, static_cast<T>(1.0/a_order));
      const T b = 0.5 * (phi + 1.0 / phi); /* y coordinates are on this circle */
      const T a = 0.5 * (phi - 1.0 / phi); /* x coordinates are on this circle */
      T m = 0;
      if(a_order & 1) {
        m = 0.0;
      } else {
        m = M_PI/(2.0*a_order);
      }
      for(size_t i = 0; i < *ap_np; i++) {/* poles */
        const int lr = i + i;
        (*ap_zs_array)[lr] = -a * cos(m);
        (*ap_zs_array)[lr+1] = b * sin(m);
        m += M_PI/a_order;
      }
      /* high pass or band reject
      */
      if((a_bandform == fb_high_pass) || (a_bandform == fb_band_stop)) {
        /* map s => 1/s
        */
        int ii = 0;
        for(size_t j = 0; j < *ap_np; j++) {
          const int ir = j + j;
          ii = ir + 1;
          const T b = (*ap_zs_array)[ir]*(*ap_zs_array)[ir] + (*ap_zs_array)[ii]*(*ap_zs_array)[ii];
          (*ap_zs_array)[ir] = (*ap_zs_array)[ir] / b;
          (*ap_zs_array)[ii] = (*ap_zs_array)[ii] / b;
        }
        /* The zeros at infinity map to the origin.
         */
        *ap_nz = *ap_np;
        if(a_bandform == fb_band_stop) {
          *ap_nz += a_order/2;
        }
        for(size_t j = 0; j < *ap_nz; j++ ) {
          const int ir = ii + 1;
          ii = ir + 1;
          (*ap_zs_array)[ir] = 0.0;
          (*ap_zs_array)[ii] = 0.0;
        }
      }
    } break;
    case ff_cauer: {
      *ap_nz = a_order/2;
      T sn1 = 0;
      T cn1 = 0;
      T dn1 = 0;
      T phi1 = 0;
      ellpj(a_u, 1.0 - a_m, &sn1, &cn1, &dn1, &phi1);
      for(size_t i = 0; i < ap_zs_array->size(); i++) {
        (*ap_zs_array)[i] = 0.0;
      }
      for(size_t i = 0; i < *ap_nz; i++) {/* zeros */
        T a = static_cast<int>(a_order) - 1 - i - i;
        const T b = (a_Kk * a)/a_order;
        T sn = 0;
        T cn = 0;
        T dn = 0;
        T phi = a_phi;
        ellpj(b, a_m, &sn, &cn, &dn, &phi );
        const int lr = 2 * *ap_np + 2 * i;
        (*ap_zs_array)[ lr ] = 0.0;
        a = a_wc / (a_k * sn);	/* k = sqrt(a_m) */
        (*ap_zs_array)[ lr + 1 ] = a;
      }
      for(size_t i = 0; i < *ap_np; i++) { /* poles */
        T a = static_cast<int>(a_order) - 1 - i - i;
        T b = a * a_Kk/a_order;
        T sn = 0;
        T cn = 0;
        T dn = 0;
        T phi = a_phi;
        ellpj( b, a_m, &sn, &cn, &dn, &phi );
        const T r = a_k * sn * sn1;
        b = cn1*cn1 + r*r;
        a = -a_wc * cn * dn * sn1 * cn1 / b;
        const int lr = i + i;
        (*ap_zs_array)[lr] = a;
        b = a_wc*sn*dn1/b;
        (*ap_zs_array)[lr + 1] = b;
      }
      if((a_bandform == fb_high_pass) || (a_bandform == fb_band_stop)) {
        
        const size_t nt = *ap_np + *ap_nz;
        int ii = 0;
        for(size_t j = 0; j < nt; j++) {
          const int ir = j + j;
          ii = ir + 1;
          const T b = (*ap_zs_array)[ir]*(*ap_zs_array)[ir] + (*ap_zs_array)[ii]*(*ap_zs_array)[ii];
          (*ap_zs_array)[ir] = (*ap_zs_array)[ir] / b;
          (*ap_zs_array)[ii] = (*ap_zs_array)[ii] / b;
        }
        while(*ap_np > *ap_nz) {
          const int ir = ii + 1;
          ii = ir + 1;
          *ap_nz += 1;
          (*ap_zs_array)[ir] = 0.0;
          (*ap_zs_array)[ii] = 0.0;
        }
      }
    } break;
  }
  IRS_LIB_DBG_MSG("s plane poles:" );
  size_t j = 0;
  for(size_t i=0; i < *ap_np + *ap_nz; i++) {
    const T a = (*ap_zs_array)[j];
    ++j;
    const T b = (*ap_zs_array)[j];
    ++j;
    IRS_LIB_DBG_MSG(a << b);
    if(i == *ap_np - 1) {
      IRS_LIB_DBG_MSG("s plane zeros:" );
    }
  }
  return 0;
}

template <class T>
int zplna(
  const filter_family_t a_family,
  const filter_bandform_t a_bandform,
  const int a_np,
  const int a_nz,
  const T a_c,
  const T a_wc,
  const T a_cbp,
  const int a_ir,
  const complex<T>& a_cone,
  const T a_cgam,
  const vector<T>& a_zs_array,
  int* ap_jt,
  int* ap_zord,
  vector<complex<T> >* ap_z_array)
{
  complex<T> r, cnum, cden, cwc, ca, cb, b4ac;
  T C;

  if(a_family == ff_cauer) {
    C = a_c;
  } else {
    C = a_wc;
  }

  for(size_t i = 0; i < ap_z_array->size(); i++) {
    (*ap_z_array)[i].real(0.0);
    (*ap_z_array)[i].imag(0.0);
  }

  int nc = a_np;
  *ap_jt = -1;
  int ii = -1;

  int ir = a_ir;
  for(int icnt = 0; icnt < 2; icnt++) {
  /* The maps from s plane to z plane */
    do {
      ir = ii + 1;
      ii = ir + 1;
      r.real(a_zs_array[ir]);
      r.imag(a_zs_array[ii]);

      switch(a_bandform) {
        case fb_low_pass:
        case fb_high_pass: {
          /* Substitute  s - r  =  s/wc - r = (1/wc)(z-1)/(z+1) - r
					*
          *     1  1 - r wc (       1 + r wc )
          * =  --- -------- ( z  -  -------- )
          *    z+1    wc    (       1 - r wc )
          *
          * giving the root in the z plane.
          */
          cnum.real(1 + C * r.real());
          cnum.imag(C * r.imag());
          cden.real(1 - C * r.real());
          cden.imag(-C * r.imag());
          *ap_jt += 1;
          // cdiv(&cden, &cnum, &ap_z_array[*ap_jt]);
          (*ap_z_array)[*ap_jt] = cnum / cden;
          if(r.imag() != 0.0) { /* fill in complex conjugate root */
            *ap_jt += 1;
            (*ap_z_array)[*ap_jt].real((*ap_z_array)[*ap_jt-1 ].real());
            (*ap_z_array)[*ap_jt].imag(-(*ap_z_array)[*ap_jt-1 ].imag());
          }
        } break;
        case fb_band_pass:
        case fb_band_stop: {
          /* Substitute  s - r  =>  s/wc - r
            *
            *     z^2 - 2 z cgam + 1
            * =>  ------------------  -  r
            *         (z^2 + 1) wc
            *
            *         1
            * =  ------------  [ (1 - r wc) z^2  - 2 cgam z  +  1 + r wc ]
            *    (z^2 + 1) wc
            *
            * and solve for the roots in the z plane.
            */
          if(a_family == ff_chebyshev_ripple_pass) {
            cwc.real(a_cbp);
          } else {
            cwc.real(a_c);
          }
          cwc.imag(0.0);
          cnum = cwc * r;// cmul( &r, &cwc, &cnum );     /* r wc */
          /* a = 1 - r wc */
          ca = a_cone - cnum;
          /* 1 - (r wc)^2 */
          b4ac = cnum * cnum;
          b4ac = a_cone - b4ac;
          b4ac *= 4.0;              // 4ac
          cb.real(-2.0 * a_cgam);   // b
          cb.imag(0.0);
          cnum = cb * cb;           // b^2
          // b^2 - 4 ac
          b4ac = cnum - b4ac;
          csqrt(b4ac, &b4ac );
          cb *= -1;                 // -b
          ca *= 2;                  // 2a
          // -b + sqrt( b^2 - 4ac)
          cnum = cb + b4ac;
          // ... /2a
          cnum = cnum / ca;
          *ap_jt += 1;
          (*ap_z_array)[*ap_jt] = cnum;
          if(cnum.imag() != 0.0) {
            *ap_jt += 1;
            (*ap_z_array)[*ap_jt].real(cnum.real());
            (*ap_z_array)[*ap_jt].imag(-cnum.imag());
          }
          if( (r.imag() != 0.0) || (cnum.imag() == 0) ) {
            // -b - sqrt( b^2 - 4ac)
            cnum = cb - b4ac;
            // ... /2a
            cnum = cnum / ca;
            *ap_jt += 1;
            (*ap_z_array)[*ap_jt] = cnum;
            if(cnum.imag() != 0.0) {
              *ap_jt += 1;
              (*ap_z_array)[*ap_jt].real(cnum.real());
              (*ap_z_array)[*ap_jt].imag(-cnum.imag());
            }
          }
        }
      } // end switch 
    } while(--nc > 0);

    if(icnt == 0) {
      *ap_zord = *ap_jt + 1;
      if(a_nz <= 0) {
        if((a_family == ff_butterworth) ||
          (a_family == ff_chebyshev_ripple_pass)) {
          
          return(0);
        } else {
          break;
        }
      }
    }
    nc = a_nz;
  } // end for() loop 
  return 0;
}

template <class T>
int zplnb(
  const filter_family_t a_family,
  const filter_bandform_t a_bandform,
  const size_t a_jt,
  const size_t a_zord,
  const T a_cgam,
  T* ap_an,
  T* ap_pn,
  vector<complex<T> >* ap_z_array,
  vector<T>* ap_aa_array,
  vector<T>* ap_pp_array,
  vector<T>* ap_y_array)
{
  complex<T> lin[2];

  lin[1].real(1.0);                
  lin[1].imag(0.0);    
  size_t jt = a_jt;
  if ((a_family == ff_butterworth) || (a_family == ff_chebyshev_ripple_pass)) {
    /* Butterworth or Chebyshev */
    /* generate the remaining zeros */
    while(2 * a_zord - 1 > jt) {
      if((a_bandform == fb_low_pass) ||
        (a_bandform == fb_band_pass) ||
        (a_bandform == fb_band_stop)) {
        
        IRS_LIB_DBG_MSG("adding zero at Nyquist frequency");
        jt += 1;
        (*ap_z_array)[jt].real(-1.0); /* zero at Nyquist frequency */
        (*ap_z_array)[jt].imag(0.0);
      }
      if((a_bandform == fb_band_pass) || (a_bandform == fb_high_pass)) {
        IRS_LIB_DBG_MSG("adding zero at 0 Hz");
        jt += 1;
        (*ap_z_array)[jt].real(1.0); /* zero at 0 Hz */
				(*ap_z_array)[jt].imag(0.0);
      }
    }
  } else { /* elliptic */
    while(2 * a_zord - 1 > jt) {
      jt += 1;
      (*ap_z_array)[jt].real(-1.0); /* zero at Nyquist frequency */
      (*ap_z_array)[jt].imag(0.0);
      if((a_bandform == fb_band_pass) || (a_bandform == fb_band_stop) ) {
        jt += 1;
        (*ap_z_array)[jt].real(1.0); /* zero at 0 Hz */
        (*ap_z_array)[jt].imag(0.0);
      }
    }
  }
  IRS_LIB_DBG_MSG("order = " << a_zord);

  /* Expand the poles and zeros into numerator and
  * denominator polynomials
  */
  for(size_t icnt = 0; icnt < 2; icnt++) {
    //IRS_LIB_DBG_MSG("icnt=" << icnt);
    for(size_t j = 0; j < ap_pp_array->size(); j++) {
      (*ap_pp_array)[j] = 0.0;
      (*ap_y_array)[j] = 0.0;
    }
    (*ap_pp_array)[0] = 1.0;
    for(size_t j = 0; j < a_zord; j++) {
      //IRS_LIB_DBG_MSG("j=" << j);
      int jj = j;
      if (icnt)
        jj += a_zord;
      const T a = (*ap_z_array)[jj].real();
      const T b = (*ap_z_array)[jj].imag();
      //IRS_LIB_DBG_MSG(setprecision(15) << "a=" << a << " b=" << b);
      //IRS_LIB_DBG_MSG(hex << "a=" << *reinterpret_cast<const __int64*>(&a) <<
        //" b=" << *reinterpret_cast<const __int64*>(&b) << dec);
      for(size_t i = 0; i <= j; i++) {
        const size_t jh = j - i;
        (*ap_pp_array)[jh+1] = (*ap_pp_array)[jh+1] - a * (*ap_pp_array)[jh] +
          b * (*ap_y_array)[jh];
        (*ap_y_array)[jh+1] =  (*ap_y_array)[jh+1]  - b * (*ap_pp_array)[jh] -
          a * (*ap_y_array)[jh];
        //IRS_LIB_DBG_MSG(setprecision(15) <<
          //"pp[jh+1]=" << (*ap_pp_array)[jh+1] <<
          //" y[jh+1]=" << (*ap_y_array)[jh+1]);
        /*IRS_LIB_DBG_MSG("jh+1=" << jh+1 << hex <<   " pp[jh+1]=" <<
          *reinterpret_cast<const __int64*>(&(*ap_pp_array)[jh+1]) <<
          " y[jh+1]=" <<
          *reinterpret_cast<const __int64*>(&(*ap_y_array)[jh+1]) <<
          dec);*/
      }
    }
    if(icnt == 0) {
      for(size_t j = 0; j <= a_zord; j++) {
        (*ap_aa_array)[j] = (*ap_pp_array)[j];
      }
    }
  }
  /* Scale factors of the pole and zero polynomials */
  T a = 1.0;
  switch(a_bandform) {
    case fb_high_pass:
      a = -1.0;
    case fb_low_pass:
    case fb_band_stop:

      *ap_pn = 1.0;
      *ap_an = 1.0;
      for(size_t j = 1; j <= a_zord; j++) {
        *ap_pn = a*(*ap_pn) + (*ap_pp_array)[j];
        *ap_an = a*(*ap_an) + (*ap_aa_array)[j];
      }
      break;

    case fb_band_pass:
      const T gam = M_PI / 2.0 - asin(a_cgam);  /* = acos( cgam ) */
      const size_t mh = a_zord / 2;
      *ap_pn = (*ap_pp_array)[mh];
      *ap_an = (*ap_aa_array)[mh];
      T ai = 0.0;
      if(mh > ((a_zord / 4) * 2)) {
        ai = 1.0;
        *ap_pn = 0.0;
        *ap_an = 0.0;
      }
      for(size_t j = 1; j <= mh; j++) {
        const T a = gam * j - ai * M_PI / 2.0;
        const T cng = cos(a);
        const size_t jh = mh + j;
        const size_t jl = mh - j;
        *ap_pn = (*ap_pn) + cng*((*ap_pp_array)[jh] +
          (1.0 - 2.0 * ai)*(*ap_pp_array)[jl]);
        *ap_an = (*ap_an) + cng*((*ap_aa_array)[jh] +
          (1.0 - 2.0 * ai)*(*ap_aa_array)[jl]);
      }
  }
  return 0;
}

template <class T>
int zplnc(
  const filter_family_t a_family,
  const T a_an,
  const T a_pn,
  const T a_scale,
  const size_t a_zord,
  const T a_fs,
  const T a_fnyq,
  const vector<complex<T> >& a_z_array,
  const vector<T>& a_aa_array,
  vector<T>* ap_pp_array,
  T* ap_gain)
{
  *ap_gain = a_an/(a_pn * a_scale);
  const bool butterworth_or_chebyshev_ripple_pass =
    (a_family == ff_butterworth) || (a_family == ff_chebyshev_ripple_pass);
  if(butterworth_or_chebyshev_ripple_pass && (a_pn == 0)) {
    *ap_gain = 1.0;
  }
  //vector<T> pp_array = a_pp_array;
  IRS_LIB_DBG_MSG("constant gain factor " << *ap_gain);
  for(size_t j = 0; j <= a_zord; j++) {
    (*ap_pp_array)[j] = (*ap_gain)*(*ap_pp_array)[j];
  }

  IRS_LIB_DBG_MSG("z plane Denominator Numerator");
  for(size_t j = 0; j <= a_zord; j++) {
    IRS_LIB_DBG_MSG(setw(4) << j << scientific << setprecision(15) <<
      setw(25) << a_aa_array[j] << setw(25) << (*ap_pp_array)[j] << fixed);
  }

  IRS_LIB_DBG_MSG("poles and zeros with corresponding quadratic factors");
  for(size_t j = 0; j < a_zord; j++) {
    T a = a_z_array[j].real();
    T b = a_z_array[j].imag();
    if( b >= 0.0 ) {
      IRS_LIB_DBG_MSG("pole " << a << " " << b);
      quadf(a, b, 1, a_fs, a_fnyq);
    }
    const size_t jj = j + a_zord;
    a = a_z_array[jj].real();
    b = a_z_array[jj].imag();
    if( b >= 0.0 ) {
      IRS_LIB_DBG_MSG("zero " << a << " " << b);
      quadf( a, b, 0, a_fs, a_fnyq);
    }
  }
  return 0;
}

/* display quadratic factors
 */
template <class T>
int quadf(
  const T a_x,
  const T a_y,
  const int a_pzflg,
  const T a_fs,
  const T a_fnyq)
{
  T a, b, r, f, g, g0;

  if(a_y > 1.0e-16) {
    a = -2.0 * a_x;
    b = a_x*a_x + a_y*a_y;
  } else {
    a = -a_x;
    b = 0.0;
  }
  IRS_LIB_DBG_MSG("q. f.\nz**2" << b << "\nz**1" << a);
  if(b != 0.0) {
    /* resonant frequency */
    r = sqrt(b);
    f = M_PI / 2.0 - asin( -a / (2.0 * r) );
    f = f * a_fs / (2.0 * M_PI );
    /* gain at resonance */
    g = 1.0 + r;
    g = g*g - (a*a/r);
    g = (1.0 - r) * sqrt(g);
    g0 = 1.0 + a + b;	/* gain at d.c. */
  } else {
    /* It is really a first-order network.
    * Give the gain at fnyq and D.C.
    */
    f = a_fnyq;
    g = 1.0 - a;
    g0 = 1.0 + a;
  }

  if(a_pzflg) {
    if(g != 0.0) {
      g = 1.0/g;
    } else {
      g = std::numeric_limits<T>::max();
    }
    if(g0 != 0.0) {
      g0 = 1.0/g0;
    } else {
      g = std::numeric_limits<T>::max();
    }
  }
  IRS_LIB_DBG_MSG("f0 " << f << " gain " << g << " DC gain " << g0);
  return 0;
}

/* Print table of filter frequency response
 */
template <class T>
int xfun(
  const T a_fs,
  const size_t a_zord,
  const T a_gain,
  const T a_fnyq,
  const T a_dbfac,
  const vector<complex<T> >& a_z_array)
{
  T f = 0.0;
  for(size_t i = 0; i <= 20; i++) {
    T r = response(f, a_fs, a_gain, a_zord, a_z_array);
    if(r <= 0.0) {
      r = -999.99;
    } else {
      r = 2.0 * a_dbfac * log(r);
    }
    IRS_LIB_DBG_MSG(f << " " << r);
    f = f + 0.05 * a_fnyq;
  }
  return 0;
}

template <class T>
T response(
  const T a_f,
  const T a_fs,
  const T a_amp,
  const size_t a_zord,
  const vector<complex<T> >& a_z_array)
{       
  complex<T> j(0.0, 1.0);
  T omega = 2.0*M_PI*a_f;
  T Tn = 1/a_fs;
  complex<T> x = exp(j*omega*Tn);

  complex<T> num = 1.0;
  complex<T> den = 1.0;
	complex<T> w = 0.0;
	// w = (exp(j*omega*T) - z[order]) * (exp(j*omega*T) - z[order+1])... /
	// (exp(j*omega*T) - z[0]) * (exp(j*omega*T) - z[2])...
	for(size_t i = 0; i < a_zord; i++) {
		w = x - a_z_array[i];      
		den *= w;
    w = x - a_z_array[i + a_zord]; 
    num *= w;
	}
  w = num/den;  
  return abs(w*a_amp);
}

template <class T>
void csqrt(const complex<T>& z, complex<T>* ap_w)
{
  complex<T> q, s;
  T x, y, r, t;

  x = z.real();
  y = z.imag();

  if(y == 0.0) {
    if(x < 0.0) {
      ap_w->real(0.0);
      ap_w->imag(sqrt(-x));
      return;
    } else {
      ap_w->real(sqrt(x));
      ap_w->imag(0.0);
      return;
    }
  }

  if(x == 0.0) {
    r = fabs(y);
    r = sqrt(0.5 * r);
    if( y > 0 )
      ap_w->real(r);
    else
      ap_w->real(-r);
    ap_w->imag(r);
    return;
  }

  /* Approximate  sqrt(x^2+y^2) - x  =  y^2/2x - y^4/24x^3 + ... .
  * The relative error in the first term is approximately y^2/12x^2 .
  */
  if( (fabs(y) < 2.e-4 * fabs(x)) && (x > 0)) {
    t = 0.25*y*(y/x);
  } else {
    r = abs(z);
    t = 0.5*(r - x);
  }

  r = sqrt(t);
  q.imag(r);
  q.real(y/(2.0 * r));
  /* Heron iteration in complex arithmetic */
  //cdiv( &q, z, &s );
  s = z/q;
  //cadd( &q, &s, ap_w );
  *ap_w = s + q;
  ap_w->real(ap_w->real() * 0.5) ;
  ap_w->imag(ap_w->imag() * 0.5) ;
}

template <class T>
int ellpj(
  const T a_u,
  const T a_m,
  T* ap_sn,
  T* ap_cn,
  T* ap_dn,
  T* ap_ph)
{
  T ai;
  T b;
  T phi;
  T t;
  T twon;    
  T a[9], c[9];
  int i;


/* Check for special cases */

  if(a_m < 0.0 || a_m > 1.0) {
    IRS_LIB_ERROR(irs::ec_standard, "domain");
    *ap_sn = 0.0;
    *ap_cn = 0.0;
    *ap_ph = 0.0;
    *ap_dn = 0.0;
    return(-1);
  }
  if( a_m < 1.0e-9 ) {
    t = sin(a_u);
    b = cos(a_u);
    ai = 0.25 * a_m * (a_u - t*b);
    *ap_sn = t - ai*b;
    *ap_cn = b + ai*t;
    *ap_ph = a_u - ai;
    *ap_dn = 1.0 - 0.5*a_m*t*t;
    return(0);
  }

  if(a_m >= 0.9999999999) {
    ai = 0.25 * (1.0-a_m);
    b = cosh(a_u);
    t = tanh(a_u);
    phi = 1.0/b;
    twon = b * sinh(a_u);
    *ap_sn = t + ai * (twon - a_u)/(b*b);
    *ap_ph = 2.0*atan(exp(a_u)) - M_PI_2 + ai*(twon - a_u)/b;
    ai *= t * phi;
    *ap_cn = phi - ai * (twon - a_u);
    *ap_dn = phi + ai * (twon + a_u);
    return(0);
  }


  /*	A. G. M. scale		*/
  a[0] = 1.0;
  b = sqrt(1.0 - a_m);
  c[0] = sqrt(a_m);
  twon = 1.0;
  i = 0;

  while(fabs(c[i]/a[i]) > MACHEP)
  {
    if( i > 7 ) {
      IRS_LIB_ERROR(irs::ec_standard, "overflow");
      goto done;
    }
    ai = a[i];
    ++i;
    c[i] = ( ai - b )/2.0;
    t = sqrt( ai * b );
    a[i] = ( ai + b )/2.0;
    b = t;
    twon *= 2.0;
  }

  done:

  /* backward recurrence */
  phi = twon * a[i] * a_u;
  do {
    t = c[i] * sin(phi) / a[i];
    b = phi;
    phi = (asin(t) + phi)/2.0;
  } while( --i );

  *ap_sn = sin(phi);
  t = cos(phi);
  *ap_cn = t;
  *ap_dn = t/cos(phi-b);
  *ap_ph = phi;
  return(0);
}


double* get_p();
double* get_q();
double get_c1();

template <class T>
T irs::ellpk(const T a_x)
{
  if((a_x < 0.0) || (a_x > 1.0)) {
    IRS_LIB_ERROR(irs::ec_standard, "domain");
    return(0.0);
  }
  if(a_x > MACHEP) {
    return(polevl(a_x, get_p(), 10) - log(a_x) * polevl(a_x, get_q(), 10));
  } else {
    if(a_x == 0.0) {
      IRS_LIB_ERROR(irs::ec_standard, "sing");
      return(std::numeric_limits<T>::max());
    } else {
      return(get_c1() - 0.5 * log(a_x));
    }
  }
}

template <class T>
T ellik(const T a_phi, const T a_m)
{ 
  if(a_m == 0.0) {
    return(a_phi);
  }
  T a = 1.0 - a_m;
  if(a == 0.0) {
    if(abs(a_phi) >= M_PI_2 ) {
      IRS_LIB_ERROR(irs::ec_standard, "sing");
      return(std::numeric_limits<T>::max());
    }
    return(log(tan((M_PI_2 + a_phi) / 2.0)));
  }
  int npio2 = static_cast<int>(floor(a_phi / M_PI_2));
  if(npio2 & 1) {
    npio2 += 1;
  }
  T K = 0;
  T phi = a_phi;
  if(npio2) {
    K = ellpk(a);
    phi = phi - npio2 * M_PI_2;
  } else {
    K = 0.0;
  }
  int sign = 0;
  if(phi < 0.0) {
    phi = -phi;
    sign = -1;
  } else {
    sign = 0;
  }
  T b = sqrt(a);
  T t = tan(phi);
  T temp = 0;
  bool done = false;
  if(abs(t) > 10.0) {
    /* Transform the amplitude */
    T e = 1.0/(b*t);
    /* ... but avoid multiple recursions.  */
    if( fabs(e) < 10.0 ) {
      e = atan(e);
      if( npio2 == 0 ) {
        K = ellpk(a);
      }
      temp = K - ellik(e, a_m);
      done = true;
    }
  }
  if (!done) {
    a = 1.0;
    T c = sqrt(a_m);
    int d = 1;
    int mod = 0;  
    while(fabs(c/a) > MACHEP) {
      temp = b/a;
      phi = phi + atan(t*temp) + mod * M_PI;
      mod = static_cast<int>((phi + M_PI_2)/M_PI);
      t = t * ( 1.0 + temp )/( 1.0 - temp * t * t );
      c = ( a - b )/2.0;
      temp = sqrt( a * b );
      a = ( a + b )/2.0;
      b = temp;
      d += d;
    }       
    temp = (atan(t) + mod * M_PI)/(d * a);
  }
  if( sign < 0 ) {
    temp = -temp;
  }
  temp += npio2 * K;
  return( temp );
}

template <class T>                
T polevl(const T a_x, const double* ap_coef, const int a_n)
{
  T ans = *ap_coef++;
  int i = a_n;
  do {
    ans = ans * a_x  +  *ap_coef++;
  } while( --i ); 
  return(ans);
}

template <class T>
T cay(const T a_q)
{
  T t1, t2;

  T a = 1.0;
  T b = 1.0;
  T r = 1.0;
  T p = a_q;

  do {
    r *= p;
    a += 2.0 * r;
    t1 = fabs( r/a );

    r *= p;
    b += r;
    p *= a_q;
    t2 = fabs( r/b );
    if( t2 > t1 )
      t1 = t2;
  } while( t1 > MACHEP );
  a = b / a;
  a = 4.0 * sqrt(a_q) * a * a;	/* see above formulas, solved for m */
  return(a);
}

} // namespace irs


#endif // iirfilterH
