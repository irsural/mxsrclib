// Реализация БПФ фильтра
// Дата: 01.09.2010
// Дата: 01.09.2010

#include <irsdefs.h>

#include <iirfilter.h>

#include <irsfinal.h>

using namespace std;
void iir_filter(const irs::filter_settings_t& a_filter_settings)
{
  bool fsuccess = true;
  const int kind = a_filter_settings.family + 1;
  const int type = a_filter_settings.bandform + 1;
  double rn = a_filter_settings.order;
  const int n = rn;
  rn = n;
  double phi = 0;
  const double dbfac = 10.0 / log(10.0);
  double scale = 0;
  double eps = 0;
  if(kind > 1) /* not Butterworth */ {
    const double dbr = a_filter_settings.passband_ripple_db;
    if(kind == 2) {
      /* For Chebyshev filter, ripples go from 1.0 to 1/sqrt(1+eps^2) */
      phi = exp( 0.5 * dbr / dbfac);

      if((n & 1) == 0)
        scale = phi;
      else
        scale = 1.0;
    } else { /* elliptic */
      eps = exp(dbr / dbfac);
      scale = 1.0;
      if( (n & 1) == 0 )
        scale = sqrt( eps );
        eps = sqrt( eps - 1.0 );
    }
  }
  const double fs = 1/a_filter_settings.sampling_time_s;
  const double fnyq = fs / 2;
  double f2 = a_filter_settings.low_cutoff_freq_hz;
  double f1 = 0;
  if((type & 1) == 0) {
    f1 = a_filter_settings.high_cutoff_freq_hz;
  } else {
    f1 = 0.0;
  }
  double a = 0;
  double bw = 0;
  if( f2 < f1 ) {
    a = f2;
    f2 = f1;
    f1 = a;
  }
  if(type == 3) /* high pass */ {
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
  double ang = bw * M_PI / fs;
  double cang = cos(ang);
  const double c = sin(ang) / cang; /* Wanalog */
  double wc = 0;
  if(kind != 3) {
    wc = c;
    /*printf( "cos( 1/2 (Whigh-Wlow) T ) = %.5e, wc = %.5e\n", cang, wc );*/
  }

  double cgam = 0.0;
  double k = 0;
  double wr = 0;
  double cbp = 0;
  const int ARRSIZE = 50;
  vector<double> y_array(ARRSIZE);
  vector<double> aa_array(ARRSIZE);
  vector<double> pp_array(ARRSIZE);
  double m = 0;
  double Kk = 0;
  double u = 0;
  if(kind == 3) { /* elliptic */
    cgam = cos( (a+f1) * M_PI / fs ) / cang;
    double dbd = a_filter_settings.stopband_ripple_db;
    double f3 = 0;
    if(dbd > 0.0) {
      f3 = dbd;
    } else { /* calculate band edge from db down */
      a = exp( -dbd/dbfac );
      double m1 = eps/sqrt( a - 1.0 );
      m1 *= m1;
      const double m1p = 1.0 - m1;
      const double Kk1 = ellpk(m1p);
      const double Kpk1 = ellpk(m1);
      const double q = exp(-M_PI * Kpk1 / (rn * Kk1));
      k = cay(q);
      if(type >= 3) {
        wr = k;
      } else {
        wr = 1.0/k;
      }
      if( type & 1 ) {
        f3 = atan( c * wr ) * fs / M_PI;
      } else {
        a = c * wr;
        a *= a;
        double b = a * (1.0 - cgam * cgam) + a * a;
        b = (cgam + sqrt(b))/(1.0 + a);
        f3 = (M_PI / 2.0 - asin(b)) * fs / (2.0 * M_PI);
      }
    }
    switch(type) {
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
    double sang = sin(ang);

    if(type & 1) {
      wr = sang/(cang*c);
    } else {
      const double q = cang * cang  -  sang * sang;
      sang = 2.0 * cang * sang;
      cang = q;
      wr = (cgam - cang)/(sang * c);
    }

    if(type >= 3)
      wr = 1.0/wr;
    if(wr < 0.0)
      wr = -wr;
    y_array[0] = 1.0;
    y_array[1] = wr;
    cbp = wr;

    if(type >= 3)
      y_array[1] = 1.0 / y_array[1];

    if( type & 1 ) {
      for(size_t i = 1; i <= 2; i++ ) {
        aa_array[i] = atan(c * y_array[i-1]) * fs / M_PI ;
      }
      IRS_LIB_DBG_MSG("pass band " << aa_array[1]);
      IRS_LIB_DBG_MSG("stop band " << aa_array[2]);
    } else {
      for(size_t i = 1; i <= 2; i++) {
        const double a = c * y_array[i-1];
        const double b = atan(a);
        double q = sqrt(1.0 + a * a  -  cgam * cgam);
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

  if( kind == 2 ) { /* Chebyshev */
    a = M_PI * (a+f1) / fs ;
    cgam = cos(a) / cang;
    a = 2.0 * M_PI * f2 / fs;
    cbp = (cgam - cos(a))/sin(a);
  }

  if( kind == 1 ) { /* Butterworth */
    a = M_PI * (a+f1) / fs ;
    cgam = cos(a) / cang;
    a = 2.0 * M_PI * f2 / fs;
    cbp = (cgam - cos(a))/sin(a);
    scale = 1.0;
  }
  int np = 0;
  int nz = 0;
  const int ir = 0;
  const complex<double> cone;

  vector<double> zs_array(50);
  int jt = 0;
  int zord = 0;
  double an = 0;
  double pn = 0;
  double gain = 0;
  vector<complex<double> > z_array(ARRSIZE);


  spln(kind, type, n, rn, m, u, k, Kk, wc, phi, &nz, &np, &zs_array);
  zplna(kind, type, np, nz, c, wc, cbp, ir, cone, cgam, zs_array, &jt, &zord, &z_array); /* convert s plane to z plane */
  zplnb(kind, type, jt, zord, cgam, &an, &pn, &z_array, &aa_array, &pp_array, &y_array);
  zplnc(kind, an, pn, scale, zord, fs, fnyq, z_array, aa_array, pp_array, &gain);
  // tabulate transfer function 
  xfun(fs, zord, gain, fnyq, dbfac, z_array);
}

int lampln(
  const double a_eps,
  const double a_wr,
  const double a_rn,
  double* ap_m,
  double* ap_k,
  double* ap_wc,
  double* ap_Kk,
  double* ap_phi,
  double* ap_u)
{
  *ap_wc = 1.0;
  *ap_k = *ap_wc / a_wr;
  *ap_m = *ap_k * *ap_k;
  *ap_Kk = ellpk(1.0 - *ap_m);
  const double Kpk = ellpk(*ap_m);
  const double q = exp(-M_PI * a_rn * Kpk / *ap_Kk);	/* the nome of k1 */
  double m1 = cay(q); /* see below */
  /* Note m1 = eps / sqrt( A*A - 1.0 ) */
  double a = a_eps/m1;
  a =  a * a + 1;
  a = 10.0 * log(a) / log(10.0);
  IRS_LIB_DBG_MSG("dbdown " << a );
  a = 180.0 * asin(*ap_k) / M_PI;
  double b = 1.0/(1.0 + a_eps*a_eps);
  b = sqrt( 1.0 - b );
  IRS_LIB_DBG_MSG("theta " << a << " rho " << b );
  m1 *= m1;
  const double m1p = 1.0 - m1;
  const double Kk1 = ellpk(m1p);
  const double Kpk1 = ellpk(m1);
  const double r = Kpk1 * *ap_Kk / (Kk1 * Kpk);
  IRS_LIB_DBG_MSG("consistency check: n= " << r);
  /*   -1
   * sn   j/eps\m  =  j ellik( atan(1/eps), m )
   */
  b = 1.0 / a_eps;
  *ap_phi = atan(b);
  *ap_u = ellik(*ap_phi, m1p);
  IRS_LIB_DBG_MSG("phi " << *ap_phi <<" m " << m1p << " u " << *ap_u);
  /* consistency check on inverse sn */
  double sn = 0;
  double cn = 0;
  double dn = 0;
  ellpj(*ap_u, m1p, &sn, &cn, &dn, ap_phi);
  a = sn/cn;
  IRS_LIB_DBG_MSG("consistency check: sn/cn =" << a << "=" << b << "= 1/eps");
  *ap_u = *ap_u * *ap_Kk / (a_rn * Kk1);	/* or, u = u * Kpk / Kpk1 */
  return 0;
}

/* calculate s plane poles and zeros, normalized to wc = 1 */
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
  vector<double>* ap_zs_array)
{
  for(size_t i = 0; i < ap_zs_array->size(); i++) {
    (*ap_zs_array)[i] = 0.0;
  }
  *ap_np = (a_n + 1) / 2;
  *ap_nz = 0;
  switch (a_kind) {
    case 1: {
      /* Butterworth poles equally spaced around the unit circle
      */
      double m = 0;
      if(a_n & 1) {
        m = 0.0;
      } else {
        m = M_PI / (2.0 * a_n);
      }
      for(size_t i = 0; i < *ap_np; i++) { /* poles */
        const int lr = i + i;
        (*ap_zs_array)[lr] = -cos(m);
        (*ap_zs_array)[lr+1] = sin(m);
        m += M_PI / a_n;
      }
      /* high pass or band reject
      */
      if(a_type >= 3) {
        /* map s => 1/s
        */
        int ii = 0;
        for(size_t j = 0; j < *ap_np; j++) {
          const int ir = j + j;
          ii = ir + 1;
          const double b = (*ap_zs_array)[ir] * (*ap_zs_array)[ir] +
            (*ap_zs_array)[ii] * (*ap_zs_array)[ii];
          (*ap_zs_array)[ir] = (*ap_zs_array)[ir] / b;
          (*ap_zs_array)[ii] = (*ap_zs_array)[ii] / b;
        }
        /* The zeros at infinity map to the origin.
         */
        *ap_nz = *ap_np;
        if(a_type == 4) {
          *ap_nz += a_n/2;
        }
        for(size_t j = 0; j < *ap_nz; j++) {
          const int ir = ii + 1;
          ii = ir + 1;
          (*ap_zs_array)[ir] = 0.0;
          (*ap_zs_array)[ii] = 0.0;
        }
      }
    } break;
    case 2: {
      /* For Chebyshev, find radii of two Butterworth circles
      * See Gold & Rader, page 60
      */
      const double rho = (a_phi - 1.0) * (a_phi + 1);  /* rho = eps^2 = {sqrt(1+eps^2)}^2 - 1 */
      const double eps = sqrt(rho);
      /* sqrt( 1 + 1/eps^2 ) + 1/eps  = {sqrt(1 + eps^2)  +  1} / eps
      */
      double phi = (a_phi + 1.0) / eps;
      phi = pow(phi, 1.0 / a_rn);  /* raise to the 1/n power */
      const double b = 0.5 * (phi + 1.0 / phi); /* y coordinates are on this circle */
      const double a = 0.5 * (phi - 1.0 / phi); /* x coordinates are on this circle */
      double m = 0;
      if(a_n & 1) {
        m = 0.0;
      } else {
        m = M_PI / (2.0 * a_n);
      }
      for(size_t i = 0; i < *ap_np; i++) {/* poles */
        const int lr = i + i;
        (*ap_zs_array)[lr] = -a * cos(m);
        (*ap_zs_array)[lr+1] = b * sin(m);
        m += M_PI / a_n;
      }
      /* high pass or band reject
      */
      if(a_type >= 3) {
        /* map s => 1/s
        */
        int ii = 0;
        for(size_t j = 0; j < *ap_np; j++) {
          const int ir = j + j;
          ii = ir + 1;
          const double b = (*ap_zs_array)[ir]*(*ap_zs_array)[ir] + (*ap_zs_array)[ii]*(*ap_zs_array)[ii];
          (*ap_zs_array)[ir] = (*ap_zs_array)[ir] / b;
          (*ap_zs_array)[ii] = (*ap_zs_array)[ii] / b;
        }
        /* The zeros at infinity map to the origin.
         */
        *ap_nz = *ap_np;
        if(a_type == 4) {
          *ap_nz += a_n/2;
        }
        for(size_t j = 0; j < *ap_nz; j++ ) {
          const int ir = ii + 1;
          ii = ir + 1;
          (*ap_zs_array)[ir] = 0.0;
          (*ap_zs_array)[ii] = 0.0;
        }
      }
    } break;
    case 3: {
      *ap_nz = a_n/2;
      double sn1 = 0;
      double cn1 = 0;
      double dn1 = 0;
      double phi1 = 0;
      ellpj(a_u, 1.0 - a_m, &sn1, &cn1, &dn1, &phi1);
      for(size_t i = 0; i < ap_zs_array->size(); i++) {
        (*ap_zs_array)[i] = 0.0;
      }
      for(size_t i = 0; i < *ap_nz; i++) {/* zeros */
        double a = a_n - 1 - i - i;
        const double b = (a_Kk * a) / a_rn;
        double sn = 0;
        double cn = 0;
        double dn = 0;
        double phi = a_phi;
        ellpj(b, a_m, &sn, &cn, &dn, &phi );
        const int lr = 2 * *ap_np + 2 * i;
        (*ap_zs_array)[ lr ] = 0.0;
        a = a_wc / (a_k * sn);	/* k = sqrt(a_m) */
        (*ap_zs_array)[ lr + 1 ] = a;
      }
      for(size_t i = 0; i < *ap_np; i++) { /* poles */
        double a = a_n - 1 - i - i;
        double b = a * a_Kk / a_rn;
        double sn = 0;
        double cn = 0;
        double dn = 0;
        double phi = a_phi;
        ellpj( b, a_m, &sn, &cn, &dn, &phi );
        const double r = a_k * sn * sn1;
        b = cn1*cn1 + r*r;
        a = -a_wc * cn * dn * sn1 * cn1 / b;
        const int lr = i + i;
        (*ap_zs_array)[lr] = a;
        b = a_wc*sn*dn1/b;
        (*ap_zs_array)[lr + 1] = b;
      }
      if(a_type >= 3) {
        const size_t nt = *ap_np + *ap_nz;
        int ii = 0;
        for(size_t j = 0; j < nt; j++) {
          const int ir = j + j;
          ii = ir + 1;
          const double b = (*ap_zs_array)[ir]*(*ap_zs_array)[ir] + (*ap_zs_array)[ii]*(*ap_zs_array)[ii];
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
    const double a = (*ap_zs_array)[j];
    ++j;
    const double b = (*ap_zs_array)[j];
    ++j;
    IRS_LIB_DBG_MSG(a << b);
    if(i == *ap_np - 1) {
      IRS_LIB_DBG_MSG("s plane zeros:" );
    }
  }
  return 0;
}

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
  vector<complex<double> >* ap_z_array)
{
  complex<double> r, cnum, cden, cwc, ca, cb, b4ac;
  double C;

  if(a_kind == 3) {
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

      switch(a_type) {
        case 1:
        case 3: {
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
        case 2:
        case 4: {
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
          if(a_kind == 2) {
            cwc.real(a_cbp);
          } else {
            cwc.real(a_c);
          }
          cwc.imag(0.0);
          cnum = cwc * r;// cmul( &r, &cwc, &cnum );     /* r wc */
          //csub( &cnum, &cone, &ca );   /* a = 1 - r wc */
          ca = a_cone - cnum;
          //cmul( &cnum, &cnum, &b4ac ); /* 1 - (r wc)^2 */
          b4ac = cnum * cnum;
          //csub( &b4ac, &cone, &b4ac );
          b4ac = a_cone - b4ac;
          b4ac.real(b4ac.real() * 4.0);               /* 4ac */
          b4ac.imag(b4ac.imag() * 4.0);
          cb.real(-2.0 * a_cgam);          /* b */
          cb.imag(0.0);
          //cmul( &cb, &cb, &cnum );     /* b^2 */
          cnum = cb * cb;
          // csub( &b4ac, &cnum, &b4ac ); /* b^2 - 4 ac */
          b4ac = cnum - b4ac;
          csqrt(b4ac, &b4ac );
          cb.real(cb.real());  /* -b */
          cb.imag(-cb.imag());
          ca.real(ca.real() * 2.0); /* 2a */
          ca.imag(ca.imag() * 2.0);
          //cadd( &b4ac, &cb, &cnum );   /* -b + sqrt( b^2 - 4ac) */
          cnum = cb + b4ac;
          // cdiv( &ca, &cnum, &cnum );   /* ... /2a */
          cnum = cnum / ca;
          *ap_jt += 1;
          // cmov( &cnum, &z[*ap_jt] );
          (*ap_z_array)[*ap_jt] = cnum;
          if(cnum.imag() != 0.0) {
            *ap_jt += 1;
            (*ap_z_array)[*ap_jt].real(cnum.real());
            (*ap_z_array)[*ap_jt].imag(-cnum.imag());
          }
          if( (r.imag() != 0.0) || (cnum.imag() == 0) ) {
            // csub( &b4ac, &cb, &cnum );  /* -b - sqrt( b^2 - 4ac) */
            cnum = cb - b4ac;
            //cdiv( &ca, &cnum, &cnum );  /* ... /2a */
            cnum = cnum / ca;
            *ap_jt += 1;
            // cmov( &cnum, &z[*ap_jt] );
            (*ap_z_array)[*ap_jt] = cnum;
            (*ap_z_array)[*ap_jt] = cnum;
            if(cnum.imag() != 0.0) {
              *ap_jt += 1;
              (*ap_z_array)[*ap_jt].real(cnum.real());
              (*ap_z_array)[*ap_jt].imag(-cnum.imag());
            }
          }
        }
      } /* end switch */
    } while(--nc > 0);

    if(icnt == 0) {
      *ap_zord = *ap_jt + 1;
      if(a_nz <= 0) {
        if(a_kind != 3) {
          return(0);
        } else {
          break;
        }
      }
    }
    nc = a_nz;
  } /* end for() loop */
  return 0;
}

int zplnb(
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
  vector<double>* ap_y_array)
{
  complex<double> lin[2];

  lin[1].real(1.0);
  lin[1].imag(0.0);
  int jt = a_jt;
  if (a_kind != 3) { /* Butterworth or Chebyshev */
    /* generate the remaining zeros */
    while( 2 * a_zord - 1 > jt) {
      if( a_type != 3 ) {
        IRS_LIB_DBG_MSG("adding zero at Nyquist frequency");
        jt += 1;
        (*ap_z_array)[jt].real(-1.0); /* zero at Nyquist frequency */
        (*ap_z_array)[jt].imag(0.0);
      }
      if((a_type == 2) || (a_type == 3)) {
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
      if((a_type == 2) || (a_type == 4) ) {
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
    for(size_t j = 0; j < ap_pp_array->size(); j++) {
      (*ap_pp_array)[j] = 0.0;
      (*ap_y_array)[j] = 0.0;
    }
    (*ap_pp_array)[0] = 1.0;
    for(size_t j = 0; j < a_zord; j++) {
      int jj = j;
      if (icnt)
        jj += a_zord;
      const double a = (*ap_z_array)[jj].real();
      const double b = (*ap_z_array)[jj].imag();
      for(size_t i = 0; i <= j; i++) {
        const size_t jh = j - i;
        (*ap_pp_array)[jh+1] = (*ap_pp_array)[jh+1] - a * (*ap_pp_array)[jh] + b * (*ap_y_array)[jh];
        (*ap_y_array)[jh+1] =  (*ap_y_array)[jh+1]  - b * (*ap_pp_array)[jh] - a * (*ap_y_array)[jh];
      }
    }
    if( icnt == 0 ) {
      for(size_t j=0; j <= a_zord; j++ )
      (*ap_aa_array)[j] = (*ap_pp_array)[j];
    }
  }
  /* Scale factors of the pole and zero polynomials */
  double a = 1.0;
  switch(a_type) {
    case 3:
      a = -1.0;

    case 1:
    case 4:

      *ap_pn = 1.0;
      *ap_an = 1.0;
      for(size_t j = 1; j <= a_zord; j++) {
        *ap_pn = a * *ap_pn + (*ap_pp_array)[j];
        *ap_an = a * *ap_an + (*ap_aa_array)[j];
      }
      break;

    case 2:
      const double gam = M_PI / 2.0 - asin(a_cgam);  /* = acos( cgam ) */
      const size_t mh = a_zord / 2;
      *ap_pn = (*ap_pp_array)[mh];
      *ap_an = (*ap_aa_array)[mh];
      double ai = 0.0;
      if(mh > ((a_zord / 4) * 2)) {
        ai = 1.0;
        *ap_pn = 0.0;
        *ap_an = 0.0;
      }
      for(size_t j = 1; j <= mh; j++) {
        const double a = gam * j - ai * M_PI / 2.0;
        const double cng = cos(a);
        const size_t jh = mh + j;
        const size_t jl = mh - j;
        *ap_pn = *ap_pn + cng * ((*ap_pp_array)[jh] + (1.0 - 2.0 * ai) * (*ap_pp_array)[jl]);
        *ap_an = *ap_an + cng * ((*ap_aa_array)[jh] + (1.0 - 2.0 * ai) * (*ap_aa_array)[jl]);
      }
    }
  return 0;
}

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
  double* ap_gain)
{
  *ap_gain = a_an/(a_pn * a_scale);
  if((a_kind != 3) && (a_pn == 0)) {
    *ap_gain = 1.0;
  }
  vector<double> pp_array = a_pp_array;
  IRS_LIB_DBG_MSG("constant gain factor " << *ap_gain);
  for(size_t j = 0; j <= a_zord; j++) {
    pp_array[j] = *ap_gain * pp_array[j];
  }

  IRS_LIB_DBG_MSG("z plane Denominator Numerator");
  for(size_t j = 0; j <= a_zord; j++) {
    IRS_LIB_DBG_MSG(setw(4) << j << scientific << setw(20) <<
      a_aa_array[j] << setw(20) << pp_array[j] << fixed);

  }

  IRS_LIB_DBG_MSG("poles and zeros with corresponding quadratic factors");
  for(size_t j = 0; j < a_zord; j++) {
    double a = a_z_array[j].real();
    double b = a_z_array[j].imag();
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
int quadf(
  const double a_x,
  const double a_y,
  const int a_pzflg,
  const double a_fs,
  const double a_fnyq)
{
  double a, b, r, f, g, g0;

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
      g = numeric_limits<double>::max();
    }
    if(g0 != 0.0) {
      g0 = 1.0/g0;
    } else {
      g = numeric_limits<double>::max();
    }
  }
  IRS_LIB_DBG_MSG("f0 " << f << " gain " << g << " DC gain " << g0);
  return 0;
}

/* Print table of filter frequency response
 */
int xfun(
  const double a_fs,
  const double a_zord,
  const double a_gain,
  const double a_fnyq,
  const double a_dbfac,
  const vector<complex<double> >& a_z_array)
{
  double f = 0.0;
  for(size_t i = 0; i <= 20; i++) {
    double r = response(f, a_fs, a_gain, a_zord, a_z_array);
    if(r <= 0.0) {
      r = -999.99;
    } else {
      r = 2.0 * a_dbfac * log( r );
    }
    IRS_LIB_DBG_MSG(f << " " << r);
    f = f + 0.05 * a_fnyq;
  }
  return 0;
}

double response(
  const double a_f,
  const double a_fs,
  const double a_amp,
  const int a_zord,
  const vector<complex<double> >& a_z_array)
{
  /* exp( j omega T ) */
  //u = 2.0 * M_PI * a_f /a_fs;
  //x.real(cos(u));
  //x.imag(sin(u));
  complex<double> j(0.0, 1.0);
  double omega = 2.0*M_PI*a_f;
  double T = 1/a_fs;
  complex<double> x = exp(j*omega*T);

  complex<double> num = 1.0;
  complex<double> den = 1.0;
  complex<double> w = 0.0;
  for(size_t i = 0; i < a_zord; i++) {
    //csub(&z[i], &x, &w);
    w = x - a_z_array[i];
    double a_real = a_z_array[i].real();
    double a_imag = a_z_array[i].imag();
    //cmul(&w, &den, &den);
    den *= w;
    //csub(&z[i+zord], &x, &w);
    w = x - a_z_array[i + a_zord];
    //cmul(&w, &num, &num);
    num *= w;
  }
  //cdiv( &den, &num, &w );
  w = num / den;
  //w.real(w.real() * a_amp);
  //w.imag(w.imag() * a_amp);
  return abs(w*a_amp);
}

void csqrt(const complex<double>& z, complex<double>* ap_w)
{
  complex<double> q, s;
  double x, y, r, t;

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
  s = z - q;
  //cadd( &q, &s, ap_w );
  *ap_w = s + q;
  ap_w->real(ap_w->real() * 0.5) ;
  ap_w->imag(ap_w->imag() * 0.5) ;
}

int ellpj(
  const double a_u,
  const double a_m,
  double* ap_sn,
  double* ap_cn,
  double* ap_dn,
  double* ap_ph)
{
  double ai;
  double b;
  double phi;
  double t;
  double twon;
  //double sqrt(), fabs(), sin(), cos(), asin(), tanh();
  //double sinh(), cosh(), atan(), exp();
  double a[9], c[9];
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

#ifdef DEC
static unsigned short P[] =
{
0035020,0127576,0040430,0051544,
0036025,0070136,0042703,0153716,
0036402,0122614,0062555,0077777,
0036441,0102130,0072334,0025172,
0036341,0043320,0117242,0172076,
0036312,0146456,0077242,0154141,
0036420,0003467,0013727,0035407,
0036564,0137263,0110651,0020237,
0036775,0001330,0144056,0020305,
0037305,0144137,0157521,0141734,
0040261,0071027,0173721,0147572
};
static unsigned short Q[] =
{
0034366,0130371,0103453,0077633,
0035557,0122745,0173515,0113016,
0036302,0124470,0167304,0074473,
0036575,0132403,0117226,0117576,
0036703,0156271,0047124,0147733,
0036766,0137465,0002053,0157312,
0037031,0014423,0154274,0176515,
0037107,0177747,0143216,0016145,
0037217,0177777,0172621,0074000,
0037377,0177777,0177776,0156435,
0040000,0000000,0000000,0000000
};
static unsigned short ac1[] = {0040261,0071027,0173721,0147572};
#define C1 (*(double *)ac1)
#endif

#ifdef IBMPC
static unsigned short P[] =
{
0x0a6d,0xc823,0x15ef,0x3f22,
0x7afa,0xc8b8,0xae0b,0x3f62,
0xb000,0x8cad,0x54b1,0x3f80,
0x854f,0x0e9b,0x308b,0x3f84,
0x5e88,0x13d4,0x28da,0x3f7c,
0x5b0c,0xcfd4,0x59a5,0x3f79,
0xe761,0xe2fa,0x00e6,0x3f82,
0x2414,0x7235,0x97d6,0x3f8e,
0xc419,0x1905,0xa05b,0x3f9f,
0x387c,0xfbea,0xb90b,0x3fb8,
0x39ef,0xfefa,0x2e42,0x3ff6
};
static unsigned short Q[] =
{
0x6ff3,0x30e5,0xd61f,0x3efe,
0xb2c2,0xbee9,0xf4bc,0x3f4d,
0x8f27,0x1dd8,0x5527,0x3f78,
0xd3f0,0x73d2,0xb6a0,0x3f8f,
0x99fb,0x29ca,0x7b97,0x3f98,
0x7bd9,0xa085,0xd7e6,0x3f9e,
0x9faa,0x7b17,0x2322,0x3fa3,
0xc38d,0xf8d1,0xfffc,0x3fa8,
0x2f00,0xfeb2,0xffff,0x3fb1,
0xdba4,0xffff,0xffff,0x3fbf,
0x0000,0x0000,0x0000,0x3fe0
};
static unsigned short ac1[] = {0x39ef,0xfefa,0x2e42,0x3ff6};
#define C1 (*(double *)ac1)
#endif

#ifdef MIEEE
static unsigned short P[] =
{
0x3f22,0x15ef,0xc823,0x0a6d,
0x3f62,0xae0b,0xc8b8,0x7afa,
0x3f80,0x54b1,0x8cad,0xb000,
0x3f84,0x308b,0x0e9b,0x854f,
0x3f7c,0x28da,0x13d4,0x5e88,
0x3f79,0x59a5,0xcfd4,0x5b0c,
0x3f82,0x00e6,0xe2fa,0xe761,
0x3f8e,0x97d6,0x7235,0x2414,
0x3f9f,0xa05b,0x1905,0xc419,
0x3fb8,0xb90b,0xfbea,0x387c,
0x3ff6,0x2e42,0xfefa,0x39ef
};
static unsigned short Q[] =
{
0x3efe,0xd61f,0x30e5,0x6ff3,
0x3f4d,0xf4bc,0xbee9,0xb2c2,
0x3f78,0x5527,0x1dd8,0x8f27,
0x3f8f,0xb6a0,0x73d2,0xd3f0,
0x3f98,0x7b97,0x29ca,0x99fb,
0x3f9e,0xd7e6,0xa085,0x7bd9,
0x3fa3,0x2322,0x7b17,0x9faa,
0x3fa8,0xfffc,0xf8d1,0xc38d,
0x3fb1,0xffff,0xfeb2,0x2f00,
0x3fbf,0xffff,0xffff,0xdba4,
0x3fe0,0x0000,0x0000,0x0000
};
static unsigned short ac1[] = {
0x3ff6,0x2e42,0xfefa,0x39ef
};
#define C1 (*(double *)ac1)
#endif

#ifdef UNK
static double P[] =
{
 1.37982864606273237150E-4,
 2.28025724005875567385E-3,
 7.97404013220415179367E-3,
 9.85821379021226008714E-3,
 6.87489687449949877925E-3,
 6.18901033637687613229E-3,
 8.79078273952743772254E-3,
 1.49380448916805252718E-2,
 3.08851465246711995998E-2,
 9.65735902811690126535E-2,
 1.38629436111989062502E0
};

static double Q[] =
{
 2.94078955048598507511E-5,
 9.14184723865917226571E-4,
 5.94058303753167793257E-3,
 1.54850516649762399335E-2,
 2.39089602715924892727E-2,
 3.01204715227604046988E-2,
 3.73774314173823228969E-2,
 4.88280347570998239232E-2,
 7.03124996963957469739E-2,
 1.24999999999870820058E-1,
 4.99999999999999999821E-1
};
static double C1 = 1.3862943611198906188E0; /* log(4) */
#endif


double ellpk(const double a_x)
{    
  if((a_x < 0.0) || (a_x > 1.0)) {
    IRS_LIB_ERROR(irs::ec_standard, "domain");
    return(0.0);
  }  
  if(a_x > MACHEP) {
    return(polevl(a_x, P, 10) - log(a_x) * polevl(a_x, Q, 10));
  } else {
    if(a_x == 0.0) {
      IRS_LIB_ERROR(irs::ec_standard, "sing");
      return(numeric_limits<double>::max());
    } else {
      return(C1 - 0.5 * log(a_x));
    }
  }
}

double ellik(const double a_phi, const double a_m)
{ 
  if(a_m == 0.0) {
    return(a_phi);
  }
  double a = 1.0 - a_m;
  if(a == 0.0) {
    if(abs(a_phi) >= M_PI_2 ) {
      IRS_LIB_ERROR(irs::ec_standard, "sing");
      return(numeric_limits<double>::max());
    }
    return(log(tan((M_PI_2 + a_phi) / 2.0)));
  }
  int npio2 = floor(a_phi / M_PI_2);
  if(npio2 & 1) {
    npio2 += 1;
  }
  double K = 0;
  double phi = a_phi;
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
  double b = sqrt(a);
  double t = tan(phi);
  double temp = 0;
  bool done = false;
  if(abs(t) > 10.0) {
    /* Transform the amplitude */
    double e = 1.0/(b*t);
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
    double c = sqrt(a_m);
    int d = 1;
    int mod = 0;  
    while(fabs(c/a) > MACHEP) {
      temp = b/a;
      phi = phi + atan(t*temp) + mod * M_PI;
      mod = (phi + M_PI_2)/M_PI;
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

double polevl(const double a_x, const double* ap_coef, const int a_n)
{
  double ans = *ap_coef++;
  int i = a_n;
  do {
    ans = ans * a_x  +  *ap_coef++;
  } while( --i ); 
  return(ans);
}

double cay(const double a_q)
{
  double t1, t2;

  double a = 1.0;
  double b = 1.0;
  double r = 1.0;
  double p = a_q;

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
