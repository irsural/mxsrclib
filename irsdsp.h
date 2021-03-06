//! \file
//! \ingroup signal_processing_group
//! \brief �������� ��������� ��������
//!
//! ����: 13.04.2010\n
//! ������ ����: 27.08.2009

#ifndef irsdspH
#define irsdspH

//#define NEW

#include <irsdefs.h>

#include <irscpp.h>
#include <irsstd.h>
#include <irssysutils.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup signal_processing_group
//! @{

#ifdef NEW
template <class T>
struct basic_pid_data_t {
  // ����� ����������� �������� ����������
  T k;
  // ����������� �������� �����������
  T ki;
  // ����������� �������� ���������������
  T kd;
  // ����������� �������� �� ������
  T min;
  // ������������ �������� �� ������
  T max;
  // ���������� �������� ������
  T prev_e;
  // �������������� �������� ������
  T pp_e;
  // ���������� �������� �� ������ ����������
  T prev_out;
  // ���������� �����������
  int block_int;
  // ������� ���������� �����������
  int block_int_ext;
  // ����� �� �����������
  T int_val;
  // ����� �� ������� ���������� �������� ����������, ����� ��������
  // �������������� �������� ������ � ����� ��� ���������� �� ������������.
  // ������������� �������� 0.1
  T k_d_pid;
};

typedef basic_pid_data_t<float> pid_data_float_t;
typedef basic_pid_data_t<double> pid_data_double_t;
typedef pid_data_double_t pid_data_t;

// ��������� � ������� ��� �������������� �����
template <class T>
struct basic_osc_data_t {
  // x[n-1]
  T x1;
  // x[n-2]
  T x2;
  // y[n-1]
  T y1;
  // y[n-2]
  T y2;
  // ����� �������� ��� ������� ���������� ��������� (fading) � e ���
  T nf;
  // ������ ��������� (oscillation) � ��������
  T no;
};
typedef basic_osc_data_t<float> osc_data_float_t;
typedef basic_osc_data_t<double> osc_data_double_t;
typedef osc_data_double_t osc_data_t;

// ��������� � ������� ��� ������������ �����
template <class T>
struct basic_fade_data_t {
  // x[n-1]
  T x1;
  // y[n-1]
  T y1;
  // ���������� �������
  T t;
};
typedef basic_fade_data_t<float> fade_data_float_t;
typedef basic_fade_data_t<double> fade_data_double_t;
typedef fade_data_t fade_data_double_t;

// ��������� � ������� ��� ����������� �����
template <class T>
struct basic_isodr_data_t {
  // k - ����������� �������� ���������������� �����
  T k;
  // ��������� ������ ������������ �����
  fade_data_t fd;
};
typedef basic_isodr_data_t<float> isodr_data_float_t;
typedef basic_isodr_data_t<double> isodr_data_double_t;
typedef isodr_data_t isodr_data_double_t;


// ���-���������
template <class T>
T pid_reg(basic_pid_data_t<T> *pd, T error_in)
{
  T dif_int = pd->prev_e;
  T res = 0;
  if (pd->block_int == 1) if (pd->prev_e > 0) dif_int = 0;
  if (pd->block_int == -1) if (pd->prev_e < 0) dif_int = 0;
  if (pd->block_int_ext == 1) if (pd->prev_e > 0) dif_int = 0;
  if (pd->block_int_ext == -1) if (pd->prev_e < 0) dif_int = 0;
  pd->int_val += dif_int;
  T d_pid = pd->k_d_pid*(pd->max - pd->min);
  if ((pd->k != 0.) && (pd->ki != 0.)) {
    T imin = (pd->min - d_pid)/pd->k/pd->ki;
    T imax = (pd->max + d_pid)/pd->k/pd->ki;
    pd->int_val = bound(pd->int_val, imin, imax);
  }
  T Ires = pd->k*pd->ki*pd->int_val;
  T PDres = pd->k*(error_in + pd->kd*(error_in - pd->prev_e));
  res = Ires + PDres;
  if (res > pd->max + d_pid) {
    if (PDres > pd->max || (pd->k == 0.) || (pd->ki == 0.)) {
      pd->int_val = 0.;
      res = PDres;
    } else {
      pd->int_val = (pd->max - PDres)/pd->k/pd->ki;
      res = pd->max;
    }
  }
  if (PDres < -d_pid) {
    if (pd->int_val < 0.) pd->int_val = 0.;
  }
  if (res > pd->max) {
    res = pd->max;
    pd->block_int = 1;
  } else if (res < pd->min) {
    res = pd->min;
    pd->block_int = -1;
  } else {
    pd->block_int = 0;
  }
  res = bound(res, pd->min, pd->max);
  pd->pp_e = pd->prev_e;
  pd->prev_e = error_in;
  pd->prev_out = res;
  return res;
}
// ������������� ���-���������� �� ������� �������� ������������� ���������
// ��� ����������� ����������
// pd - ��������� ���-����������,
// e - ������ �� ����� ����������
// rp - �������� ���������� �� �������������� �������� �� �������
// �������������� �������������
template <class T>
void pid_reg_sync(basic_pid_data_t<T> *pd, T e, T rp)
{
  if ((pd->k == 0.) || (pd->ki == 0.)) {
    pd->int_val = 0.;
  } else {
    T Pres = pd->k*pd->prev_e;
    T d_pid = pd->k_d_pid*(pd->max - pd->min);
    if (fabs(Pres) < d_pid) {
      pd->int_val = (rp - pd->k*(e/* + pd->kd*(e - pd->prev_e)*/))
        /(pd->k*pd->ki);
    } else {
      pd->int_val = 0.;
    }
  }
  pd->pp_e = e;
  pd->prev_e = e;
  pd->prev_out = rp;
}
// ������������� ���-���������� �� ������� �������� ������������� ���������
// ��� ���������� ����������
// pd - ��������� ���-����������,
template <class T>
void pid_reg_sync(basic_pid_data_t<T> *pd)
{
  if ((pd->k == 0.) || (pd->ki == 0.)) {
    pd->int_val = 0.;
  } else {
    T PDres = pd->k*(pd->prev_e + pd->kd*(pd->prev_e - pd->pp_e));
    T d_pid = pd->k_d_pid*(pd->max - pd->min);
    if ((PDres > -d_pid) && (PDres < pd->max)) {
      pd->int_val = (pd->prev_out - pd->k*(pd->prev_e/* +
        pd->kd*(pd->prev_e - pd->pp_e)*/))
        /(pd->k*pd->ki);
    } else {
      pd->int_val = 0.;
    }
  }
}

// ������������� �����
template <class T>
T osc(basic_osc_data_t<T> *od, T x)
{
  T k = sqr(1/od->nf) + sqr(2*IRS_PI/od->no);
  T y = (k*(x + 2*od->x1 + od->x2)
    - (2*k - 8)*od->y1
    - (k + 4*(1 - 1/od->nf))*od->y2)/(k + 4*(1 + 1/od->nf));
  od->x2 = od->x1; od->x1 = x;
  od->y2 = od->y1; od->y1 = y;
  return y;
}

// ����������� �����
template <class T>
T fade(basic_fade_data_t<T> *fd, T x)
{
  T y = (x + fd->x1 - (1 - 2*fd->t)*fd->y1)/(1 + 2*fd->t);
  fd->x1 = x;
  fd->y1 = y;
  return y;
}

// ���������� �����
// id - ��������� ������ ����������� �����
// x - ������� ��������
template <class T>
T isodr(basic_isodr_data_t<T> *id, T x)
{
  return id->k*x + (1 - id->k)*fade(&id->fd, x);
}




template <class T>
class pid_regulator_t
{
public:
  enum options_t {
    // ������ 1-�� ������� ����� �������
    sensor_filter_unit = 1,
    // ���������� ����� ����� ������� ��� �������
    sensor_isodr_unit = 2,
    // ������������ ����� �������
    ref_rate_limiter_unit = 4,
    // ������������� �������������
    sync_gains = 8,
    // ��������� ���� ������������������ ���������� �� �������
    dead_band = 16,
    // ������������ ����������������� ������������
    norm_prop_gain = 32,
    // ������������� ����������������� ������������
    // ��������� �� �����
    // ��� �������
    // - ��������� ���������� ������������
    // - ��������� �������� ������������ � ��������� �����������������
    adapt_prop_gain = 64,
    // ������� ����� �� �����
    // ���� �� �������������.
    fast_mode = 128
  };

  pid_regulator_t(const &T a_proportional_gain, const &T a_integral_gain,
    const &T a_derivative_gain, const &T a_out_min, const &T a_out_max,
    options_t a_options = sync_gains);
  ~pid_regulator_t();

  // ����������� ������������� �� ������������ ������������
  void start();
  void stop();
  void proportional_gain(const &T a_proportional_gain);
  T proportional_gain();
  // ki = a_integral_gain*dt
  void integral_gain(const &T a_integral_gain);
  T integral_gain();
  // kd = a_integral_gain/dt
  void derivative_gain(const &T a_derivative_gain);
  T derivative_gain();
  void out_min(const &T a_out_min);
  T out_min();
  void out_max(const &T a_out_max);
  T out_max();
  void integrator(const &T a_integrator);
  T integrator();
  T exec(const T& a_error);
private:
};

#else //NEW
// ��������� ��� ���-����������
typedef struct _pid_data {
  // ����� ����������� �������� ����������
  double k;
  // ����������� �������� �����������
  double ki;
  // ����������� �������� ���������������
  double kd;
  // ����������� �������� �� ������
  double min;
  // ������������ �������� �� ������
  double max;
  // ���������� �������� ������
  double prev_e;
  // �������������� �������� ������
  double pp_e;
  // ���������� �������� �� ������ ����������
  double prev_out;
  // ���������� �����������
  int block_int;
  // ������� ���������� �����������
  int block_int_ext;
  // ����� �� �����������
  double int_val;
  // ����� �� ������� ���������� �������� ����������, ����� ��������
  // �������������� �������� ������ � ����� ��� ���������� �� ������������.
  // ������������� �������� 0.1
  double k_d_pid;
  bool operator==(const _pid_data& ap_pid_data) const
  {
    return (k == ap_pid_data.k) &&
      (ki == ap_pid_data.ki) &&
      (kd == ap_pid_data.kd) &&
      (min == ap_pid_data.min) &&
      (max == ap_pid_data.max) &&
      (prev_e == ap_pid_data.prev_e) &&
      (pp_e == ap_pid_data.pp_e) &&
      (prev_out == ap_pid_data.prev_out) &&
      (block_int == ap_pid_data.block_int) &&
      (block_int_ext == ap_pid_data.block_int_ext) &&
      (int_val == ap_pid_data.int_val) &&
      (k_d_pid == ap_pid_data.k_d_pid);
  }
  bool operator!=(const _pid_data& ap_pid_data) const
  {
    return !operator==(ap_pid_data);
  }
} pid_data_t;

// ��������� � ������� ��� �������������� �����
typedef struct _osc_data {
  // x[n-1]
  double x1;
  // x[n-2]
  double x2;
  // y[n-1]
  double y1;
  // y[n-2]
  double y2;
  // ����� �������� ��� ������� ���������� ��������� (fading) � e ���
  double nf;
  // ������ ��������� (oscillation) � ��������
  double no;
} osc_data_t;

// ��������� � ������� ��� ������������ �����
typedef struct _fade_data {
  // x[n-1]
  double x1;
  // y[n-1]
  double y1;
  // ���������� �������
  double t;
} fade_data_t;

// ��������� � ������� ��� ����������� �����
typedef struct _isodr_data {
  // k - ����������� �������� ���������������� �����
  double k;
  // ��������� ������ ������������ �����
  fade_data_t fd;
} isodr_data_t;
#endif //NEW

// ��������� ��� ������������ �����
template <class T>
struct rate_limiter_t {
  // ������� �������� ���������
  T cur;
  // ������� �������� ���������
  //T ref;
  // ��� ������ �-��� �� �������, �
  T dt;
  // ��������, <��. ���>/c
  T slope;
  // ������������ ��������� �������� �� ��� ������
  T dl;
  // ���������� �������� ���� ������ �-��� �� �������, �
  T dt_prev;
  // ���������� �������� ��������, <��. ���>/���.
  T slope_prev;
  // �������������� �������� dl
  bool recalc;
};

#ifndef NEW
// ���-���������
double pid_reg(pid_data_t *pd, double e);
// ������������� ���-���������� �� ������� �������� ������������� ���������
// ��� ����������� ����������
// pd - ��������� ���-����������,
// e - ������ �� ����� ����������
// rp - �������� ���������� �� �������������� �������� �� �������
// �������������� �������������
void pid_reg_sync(pid_data_t *pd, double e, double rp);
// ������������� ���-���������� �� ������� �������� ������������� ���������
// ��� ���������� ����������
// pd - ��������� ���-����������,
void pid_reg_sync(pid_data_t *pd);
// ������������� �����
double osc(osc_data_t *od, double x);
// ����������� �����
double fade(fade_data_t *fd, double x);
// ���������� �����
// id - ��������� ������ ����������� �����
// x - ������� ��������
double isodr(isodr_data_t *id, double x);
#endif //NEW

// ������������ �����
// otd - ��������� ������ ��� ������������ �����
// in - �������������� ������������������
template <class T>
T rate_limiter(rate_limiter_t<T> *rld, const T &in)
{
  if ((rld->slope != rld->slope_prev) || (rld->dt != rld->dt_prev) ||
    rld->recalc) {
    rld->dl = rld->dt*rld->slope;
    rld->recalc = false;
  }
  T sub = in - rld->cur;
  if (sub > rld->dl) rld->cur += rld->dl;
  else if (sub < -rld->dl) rld->cur -= rld->dl;
  else rld->cur = in;
  rld->slope_prev = rld->slope;
  rld->dt_prev = rld->dt;
  return rld->cur;
}

// ��������� ������ �� ���������� in � �������� min - max � ���� �� ������,
// �������� �� �������� �� ��������� ������� ���������
template <class T>
T bound(T in, T min, T max)
{
  if (in < min) in = min;
  if (in > max) in = max;
  return in;
}

template <class T>
bool is_in_range(T value, T min, T max)
{
  return (value >= min) && (value <= max);
}

template <class T>
bool is_in_range_exclusive(T value, T min, T max)
{
  return (value > min) && (value < max);
}

// ���������� � �������
template <class T>
T sqr(T x)
{
  return x*x;
}

// ��� �� ������������ �������
#ifdef __WATCOMC__
// �� �����-�� ������� Watcom C++ �� ������������ �������� ����������
// ����������� ��������
template <class T, class V = vector<complex<T> > >
void fft(V& signal)
#else //__WATCOMC__
template <class T>
void fft(vector<complex<T> >& signal)
#endif //__WATCOMC__
{
  if (signal.empty()) return;
  int degree = 0;
  for (int i = signal.size(); i >>= 1;) degree++;
  int size = 1 << degree;
  signal.resize(size);
  for (int first_idx = 0; first_idx < size; first_idx++)
  {
    int second_idx = 0;
    for (int create_idx = 0; create_idx < degree; create_idx++) {
      second_idx |= ((first_idx >> (degree - create_idx - 1))&1) << create_idx;
    }
    if (second_idx > first_idx) {
      ::swap(signal[first_idx], signal[second_idx]);
    }
  }
  complex<T> divider = -1.;
  int group_size = size/2;
  int butterfly_size = 1;
  for (int stage_idx = 0; stage_idx < degree; stage_idx++)
  {
    for (int group_idx = 0; group_idx < group_size; group_idx++)
    {
      complex<T> factor = 1.;
      for (int butterfly_idx = 0; butterfly_idx < butterfly_size;
        butterfly_idx++)
      {
        int wing_first = 2*group_idx*butterfly_size + butterfly_idx;
        int wing_second = wing_first + butterfly_size;
        signal[wing_second] *= factor;
        complex<T> signal_add = signal[wing_first] + signal[wing_second];
        complex<T> signal_sub = signal[wing_first] - signal[wing_second];
        signal[wing_first] = signal_add;
        signal[wing_second] = signal_sub;
        factor /= divider;
      }
    }
    divider = sqrt(divider);
    group_size /= 2;
    butterfly_size *= 2;
  }
}

// �������� ��� �� ������������ �������
#ifdef __WATCOMC__
// �� �����-�� ������� Watcom C++ �� ������������ �������� ����������
// ����������� ��������
template <class T, class V = vector<complex<T> > >
void ifft(V& freq_charact)
#else //__WATCOMC__
template <class T>
void ifft(vector<complex<T> > &freq_charact)
#endif //__WATCOMC__
{
  typedef typename vector<complex<T> >::size_type vsize_t;

  for (vsize_t i = 0; i < freq_charact.size(); i++) {
    freq_charact[i] = conj(freq_charact[i]);
  }

  fft(freq_charact);

  for (vsize_t i = 0; i < freq_charact.size(); i++) {
    freq_charact[i] = conj(freq_charact[i])/freq_charact.size();
  }
}

//  ���������� ����������� ��� � ��������� ����
#ifdef __WATCOMC__
// �� �����-�� ������� Watcom C++ �� ������������ �������� ����������
// ����������� ��������
template <class T, class V = vector<complex<T> > >
void fft_cast(V& fft_result)
#else //__WATCOMC__
template <class T>
void fft_cast(vector<complex<T> > &fft_result)
#endif //__WATCOMC__
{
  typedef typename vector<complex<T> >::size_type vsize_t;
  vsize_t divider = 0.5 * fft_result.size();

  fft_result[0] /= fft_result.size();
  for (vsize_t i = 1; i < fft_result.size(); i++)
  {
    fft_result[i] /= divider;
  }
}

//  ���������� ������������ ���������� ���������
#ifdef __WATCOMC__
// �� �����-�� ������� Watcom C++ �� ������������ �������� ����������
// ����������� ��������
template <class T, class V = vector<T> >
T thd_calc(V& spectrum, irs_uarc num_of_points)
#else //__WATCOMC__
template <class T>
T thd_calc(vector<T> &spectrum, irs_uarc num_of_points)
#endif //__WATCOMC__
{
  if (num_of_points > spectrum.size()) num_of_points = spectrum.size();
  T thd = 0;
  for (irs_uarc i = 2; i < num_of_points; i++)
  {
    thd += sqr(spectrum[i]);
  }
  thd = sqrt(thd / sqr(spectrum[1]));
  return thd;
}

//  ���������� ���. �� �����.
template <class T, irs_uarc N>
class fft_process_t: public mxdata_t
{
  enum status_t
  {
    FREE,
    START,
    SWAP,
    PROCESS,
    CAST
  };
  static const irs_uarc m_size = N * sizeof(T);
  status_t m_status;
  mxdata_t &m_source_data;
  vector< complex<T> > m_source_vector;
  array_data_t<T> m_source_array;
  irs_u8 mp_buf[m_size];
  T *mp_out_buf;
  irs_uarc m_real_N;
  irs_uarc m_real_size;
  T m_cast_divider;
  irs_uarc m_cast_cnt;
  bool m_ready;
  //
  irs_uarc m_degree;
  irs_uarc m_first_idx;
  irs_uarc m_second_idx;
  irs_uarc m_create_idx;
  complex<T> m_divider;
  complex<T> m_factor;
  irs_uarc m_group_size;
  irs_uarc m_butterfly_size;
  irs_uarc m_stage_idx;
  irs_uarc m_group_idx;
  irs_uarc m_butterfly_idx;
public:
  fft_process_t(mxdata_t &a_source_data);
  ~fft_process_t();
  void start();
  bool ready();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

template <class T, irs_uarc N>
fft_process_t<T, N>::fft_process_t(mxdata_t &a_source_data):
  m_status(FREE),
  m_source_data(a_source_data),
  m_source_vector(N),
  m_source_array(),
  mp_out_buf(reinterpret_cast<T*>(mp_buf)),
  m_real_N(0),
  m_real_size(0),
  m_cast_divider(1.),
  m_cast_cnt(0),
  m_ready(false),
  //
  m_degree(1),
  m_first_idx(0),
  m_second_idx(0),
  m_create_idx(0),
  m_divider(-1.),
  m_factor(1.),
  m_group_size(0),
  m_butterfly_size(0),
  m_stage_idx(0),
  m_group_idx(0),
  m_butterfly_idx(0)
{
  for (irs_uarc i = N - 1; i >>= 1;) m_degree++;
  m_real_N = 1 << m_degree;
  m_real_size = m_real_N * sizeof(T);
  m_cast_divider = 0.5 * m_real_N;
  m_source_vector.resize(m_real_N);
  m_source_array.connect(&m_source_data, 0, m_real_N);
  memset(mp_buf, 0, m_size);
}

template <class T, irs_uarc N>
fft_process_t<T, N>::~fft_process_t()
{
}

template <class T, irs_uarc N>
irs_uarc fft_process_t<T, N>::size()
{
  return m_real_size;
}

template <class T, irs_uarc N>
irs_bool fft_process_t<T, N>::connected()
{
  return true;
}

template <class T, irs_uarc N>
void fft_process_t<T, N>
  ::read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = a_size;
  if (size + a_index > m_size) size = static_cast<irs_uarc>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(ap_buf),
    reinterpret_cast<void*>(mp_buf + a_index), size);
}

template <class T, irs_uarc N>
void fft_process_t<T, N>
  ::write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = a_size;
  if (size + a_index > m_size) size = static_cast<irs_uarc>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
}

template <class T, irs_uarc N>
irs_bool fft_process_t<T, N>::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & (1 << a_bit_index));
}

template <class T, irs_uarc N>
void fft_process_t<T, N>::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= (1 << a_bit_index);
}

template <class T, irs_uarc N>
void fft_process_t<T, N>::clear_bit(irs_uarc a_index,
  irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= (1 << a_bit_index)^0xFF;
}

//  ����������� �������

template <class T, irs_uarc N>
void fft_process_t<T, N>::start()
{
  m_status = START;
  m_ready = false;
}

template <class T, irs_uarc N>
bool fft_process_t<T, N>::ready()
{
  return m_ready;
}

template <class T, irs_uarc N>
void fft_process_t<T, N>::tick()
{
  switch (m_status)
  {
    case START:
    {
      m_first_idx = 0;
      m_second_idx = 0;
      m_create_idx = 0;
      for (irs_uarc i = 0; i < m_real_N; i++)
      {
        m_source_vector[i] = m_source_array[i];
      }
      m_status = SWAP;
      break;
    }
    case SWAP:
    {
      if (m_first_idx < m_real_N)
      {
        for (m_create_idx = 0; m_create_idx < m_degree; m_create_idx++)
        {
          m_second_idx |= ((m_first_idx >> (m_degree - m_create_idx - 1))&1)
            << m_create_idx;
        }
        if (m_second_idx > m_first_idx)
        {
          swap(m_source_vector[m_first_idx], m_source_vector[m_second_idx]);
        }
        m_first_idx++;
        m_second_idx = 0;
      }
      else
      {
        m_status = PROCESS;
        m_divider = -1.;
        m_group_size = m_real_N / 2;
        m_butterfly_size = 1;
        m_stage_idx = 0;
        m_group_idx = 0;
        m_butterfly_idx = 0;
        m_factor = 1.;
      }
      break;
    }
    case PROCESS:
    {
      if (m_stage_idx < m_degree)
      {
        if (m_group_idx < m_group_size)
        {
          if (m_butterfly_idx < m_butterfly_size)
          {
            int wing_first = 2*m_group_idx*m_butterfly_size + m_butterfly_idx;
            int wing_second = wing_first + m_butterfly_size;
            m_source_vector[wing_second] *= m_factor;
            complex<T> signal_add
              = m_source_vector[wing_first] + m_source_vector[wing_second];
            complex<T> signal_sub
              = m_source_vector[wing_first] - m_source_vector[wing_second];
            m_source_vector[wing_first] = signal_add;
            m_source_vector[wing_second] = signal_sub;
            m_factor /= m_divider;
            m_butterfly_idx++;
          }
          else
          {
            m_butterfly_idx = 0;
            m_factor = 1.;
            m_group_idx++;
          }
        }
        else
        {
          m_group_idx = 0;

          m_stage_idx++;
          m_divider = sqrt(m_divider);
          m_group_size /= 2;
          m_butterfly_size *= 2;
        }
      }
      else
      {
        for (irs_uarc i = 0; i < m_real_N; i++)
        {
          T real = m_source_vector[i].real();
          T imag = m_source_vector[i].imag();
          mp_out_buf[i] = sqrt(sqr(real) + sqr(imag));
        }
        m_status = CAST;
        m_cast_cnt = 1;
        mp_out_buf[0] /= m_real_N;
      }
      break;
    }
    case CAST:
    {
      if (m_cast_cnt < m_real_N)
      {
        mp_out_buf[m_cast_cnt] /= m_cast_divider;
        m_cast_cnt++;
      }
      else
      {
        m_ready = true;
        m_status = FREE;
      }
      break;
    }
  }
}

//------------------------------  THD PROCESS  ---------------------------------

template <class T, irs_uarc N>
class thd_process_t
{
  enum status_t
  {
    FREE,
    START,
    PROCESS
  };
  status_t m_status;
  mxdata_t &m_source_data;
  array_data_t<T> m_source_array;
  irs_uarc m_current_point;
  T m_result;
  T m_thd;
  bool m_ready;
public:
  thd_process_t(mxdata_t &a_source_data);
  ~thd_process_t();
  void start();
  bool ready();
  T get_thd();
  void tick();
};

template <class T, irs_uarc N>
thd_process_t<T, N>::thd_process_t(mxdata_t &a_source_data):
  m_status(FREE),
  m_source_data(a_source_data),
  m_source_array(&m_source_data, 0, N),
  m_current_point(0),
  m_result(0.),
  m_thd(0.),
  m_ready(false)
{
}

template <class T, irs_uarc N>
thd_process_t<T, N>::~thd_process_t()
{
}

template <class T, irs_uarc N>
void thd_process_t<T, N>::start()
{
  m_status = START;
  m_ready = false;
}

template <class T, irs_uarc N>
bool thd_process_t<T, N>::ready()
{
  return m_ready;
}

template <class T, irs_uarc N>
T thd_process_t<T, N>::get_thd()
{
  return m_result;
}

template <class T, irs_uarc N>
void thd_process_t<T, N>::tick()
{
  switch (m_status)
  {
    case START:
    {
      m_thd = 0.;
      m_current_point = 2;
      m_status = PROCESS;
      break;
    }
    case PROCESS:
    {
      if (m_current_point < N)
      {
        T point = m_source_array[m_current_point];
        m_thd += sqr(point);
        m_current_point++;
      }
      else
      {
        T point = m_source_array[1];
        if (point != 0.)
        {
          m_result = sqrt(m_thd) / point;
        }
        else m_result = -1.;
        m_status = FREE;
        m_ready = true;
      }
      break;
    }
  }
}

//  ������������� ������
template <class T>
class osc_cir_t
{
public:
  osc_cir_t(T a_n_period, T a_n_fade, T a_y1_init, T a_x1_init = T(),
    T a_y2_init = T(),  T a_x2_init = T());
  ~osc_cir_t();
  //  ����� ����� �� ������ ����������� �������
  void set_num_of_period_points(T a_n_period);
  //  ����� �������� ����������� �������, ����� ��������� �������� � e ���
  void set_num_of_fade_points(T a_n_fade);
  void sync(T a_y1, T a_y2 = T());
  T filt(T a_x);
private:
  T m_n_period;
  T m_fade;
  T m_freq_2;
  T m_denom;
  T m_a;
  T m_b;
  T m_c;
  T m_y2_init;
  T m_x1_init;
  T m_x2_init;
  T m_x1;
  T m_x2;
  T m_y1;
  T m_y2;
  //
  T calc_a();
  T calc_b();
  T calc_c();
  T calc_denom();
};

// ������������� (14.06.2016): �� �������!!!
// ������ ������������� ������ osc_cir_t
#ifdef NOP
typedef float main_float_t;
inline int osc_cir_example()
{
  main_float_t Fd = 48000;
  main_float_t Td = 1/Fd;
  main_float_t f_center = 125;
  // �� ���� ������ ������� �������� ������� ������������� �� �������:
  main_float_t f_center_osc = 2 / Td * tan(f_center * Td / 2);
  main_float_t n_t_center = Fd/f_center_osc;
  main_float_t tau = 1;
  main_float_t n_tau = tau/Td;
  osc_cir_t<main_float_t> osc_cir(n_t_center, n_tau, 0);

  int example_count = 100;
  main_float_t sum = 0;
  for (int i = 0; i < example_count; i++) {
    main_float_t sample = rand();
    main_float_t out = osc_cir.filt(sample);
    sum += out;
  }

  return static_cast<int>(sum/example_count);
}
#endif //NOP

template <class T>
osc_cir_t<T>::osc_cir_t(T a_n_period, T a_n_fade, T a_y1_init, T a_x1_init,
  T a_y2_init, T a_x2_init):
  m_n_period(a_n_period),
  m_fade(1. / a_n_fade),
  m_freq_2(sqr(2.* tan(IRS_PI / m_n_period))),
  m_denom(calc_denom()),
  m_a(m_denom * calc_a()),
  m_b(m_denom * calc_b()),
  m_c(m_denom * calc_c()),
  m_y2_init((a_y2_init == T()) ? a_y1_init : a_y2_init),
  m_x1_init((a_x1_init == T()) ? a_y1_init : a_x1_init),
  m_x2_init((a_x2_init == T()) ? a_y1_init : a_x2_init),
  m_x1(m_x1_init),
  m_x2(m_x2_init),
  m_y1(a_y1_init),
  m_y2(m_y2_init)
{
}

template <class T>
osc_cir_t<T>::~osc_cir_t()
{
}

template <class T>
void osc_cir_t<T>::set_num_of_period_points(T a_n_period)
{
  if (a_n_period <= 0) return;
  m_n_period = a_n_period;
  m_freq_2 = sqr(2.* tan(IRS_PI / m_n_period));
  m_denom = calc_denom();
  m_a = m_denom * calc_a();
  m_b = m_denom * calc_b();
  m_c = m_denom * calc_c();
}

template <class T>
void osc_cir_t<T>::set_num_of_fade_points(T a_n_fade)
{
  m_fade = 1. / a_n_fade;
  m_freq_2 = sqr(2.* tan(IRS_PI / m_n_period));
  m_denom = calc_denom();
  m_a = m_denom * calc_a();
  m_b = m_denom * calc_b();
  m_c = m_denom * calc_c();
}

template <class T>
void osc_cir_t<T>::sync(T a_y1, T a_y2)
{
  if (a_y2 == T()) {
    a_y2 = a_y1;
  }
  m_x1 = a_y1;
  m_x2 = a_y2;
  m_y1 = a_y1;
  m_y2 = a_y2;

}

template <class T>
T osc_cir_t<T>::filt(T a_x)
{
  T y = m_a * (a_x - m_x2) - m_b * m_y2 - m_c * m_y1;
  if (is_inf_or_nan(y))
  {
    return m_y1;
  }
  m_y2 = m_y1;
  m_y1 = y;
  m_x2 = m_x1;
  m_x1 = a_x;
  return y;
}

template <class T>
T osc_cir_t<T>::calc_a()
{
  return 4. * m_fade;
}

template <class T>
T osc_cir_t<T>::calc_b()
{
  return 4. + sqr(m_fade) - 4. * m_fade + m_freq_2;
}

template <class T>
T osc_cir_t<T>::calc_c()
{
  return 2. * (sqr(m_fade) - 4. + m_freq_2);
}

template <class T>
T osc_cir_t<T>::calc_denom()
{
  return 1. / (4. + sqr(m_fade) + 4. * m_fade + m_freq_2);
}

//  ����������� �����
template <class T>
class fade_t
{
public:
  fade_t(T a_t, T a_y_init);
  ~fade_t();
  T filt(T a_x);
  void set_t(T a_t);
private:
  T m_x1; // x[n-1]
  T m_y1; // y[n-1]
  T m_t;  // ���������� �������
};

template <class T>
fade_t<T>::fade_t(T a_t, T a_y_init):
  m_x1(a_y_init),
  m_y1(a_y_init),
  m_t(a_t)
{
}

template <class T>
fade_t<T>::~fade_t()
{
}

template <class T>
T fade_t<T>::filt(T a_x)
{
  T y = (a_x + m_x1 - (1 - 2 * m_t) * m_y1) / (1 + 2 * m_t);
  m_x1 = a_x;
  m_y1 = y;
  return y;
}

template <class T>
void fade_t<T>::set_t(T a_t)
{
  m_t = a_t;
}

enum filter_family_t {
  ff_butterworth = 0,
  ff_chebyshev_ripple_pass = 1,
  ff_chebyshev_ripple_stop = 2,
  ff_cauer = 3
};

enum filter_bandform_t {
  fb_low_pass,
  fb_band_pass,
  fb_high_pass,
  fb_band_stop
};

enum filter_impulse_response_type_t {
  firt_finite,
  firt_infinite
};

struct filter_settings_t {
  typedef size_t size_type;
  filter_family_t family;
  filter_bandform_t bandform;
  size_type order;
  double sampling_time_s;
  double low_cutoff_freq_hz;
  double high_cutoff_freq_hz;
  double passband_ripple_db;
  double stopband_ripple_db;
  filter_settings_t(
    filter_family_t a_family = ff_butterworth,
    filter_bandform_t a_bandform = fb_low_pass,
    size_type a_order = 2,
    double a_sampling_time_s = 10e-6,
    double a_low_cutoff_freq_hz = 1000,
    double a_high_cutoff_freq_hz = 10000,
    double a_passband_ripple_db = 0.1,
    double a_stopband_ripple_db = 80
  ):
    family(a_family),
    bandform(a_bandform),
    order(a_order),
    sampling_time_s(a_sampling_time_s),
    low_cutoff_freq_hz(a_low_cutoff_freq_hz),
    high_cutoff_freq_hz(a_high_cutoff_freq_hz),
    passband_ripple_db(a_passband_ripple_db),
    stopband_ripple_db(a_stopband_ripple_db)
  {
  }
};

inline bool operator==(const filter_settings_t& a_fs_first,
  const filter_settings_t& a_fs_second)
{
  return ((a_fs_first.family == a_fs_second.family) &&
    (a_fs_first.bandform == a_fs_second.bandform) &&
    (a_fs_first.order == a_fs_second.order) &&
    (a_fs_first.sampling_time_s == a_fs_second.sampling_time_s) &&
    (a_fs_first.low_cutoff_freq_hz == a_fs_second.low_cutoff_freq_hz) &&
    (a_fs_first.high_cutoff_freq_hz == a_fs_second.high_cutoff_freq_hz) &&
    (a_fs_first.passband_ripple_db == a_fs_second.passband_ripple_db) &&
    (a_fs_first.stopband_ripple_db == a_fs_second.stopband_ripple_db));
}

inline bool operator!=(const filter_settings_t& a_fs_first,
  const filter_settings_t& a_fs_second)
{
  return !(a_fs_first == a_fs_second);
}

template <class T>
class delay_line_t
{
public:
  typedef T value_type;
  typedef size_t size_type;

  delay_line_t(const value_type& a_delay_time,
    const value_type& a_sampling_time, const value_type& a_fill_value);
  value_type tick(const value_type& a_value);
  void sampling_time(const value_type& a_time);
  value_type sampling_time() const;
  void delay_time(const value_type& a_time);
  value_type delay_time() const;
  void reset(const value_type& a_fill_value);
  void resize(size_type a_size);
  size_type size() const;
private:
  value_type m_delay_time;
  value_type m_sampling_time;
  value_type m_fill_value;
  deque<value_type> m_sample_list;
};
template <class T>
delay_line_t<T>::delay_line_t(const value_type& a_delay_time,
  const value_type& a_sampling_time, const value_type& a_fill_value
):
  m_delay_time(a_delay_time),
  m_sampling_time(a_sampling_time),
  m_fill_value(a_fill_value),
  m_sample_list(round(m_delay_time/m_sampling_time, size_t()), m_fill_value)
{
}
template <class T>
typename delay_line_t<T>::value_type delay_line_t<T>::
  tick(const value_type& a_value)
{
  T sample = m_sample_list.front();
  m_sample_list.pop_front();
  m_sample_list.push_back(a_value);
  return sample;
}
template <class T>
void delay_line_t<T>::sampling_time(const value_type& a_time)
{
  m_sampling_time = a_time;
  m_sample_list.resize(round(m_delay_time/m_sampling_time, size_t()),
    m_fill_value);
}
template <class T>
typename delay_line_t<T>::value_type delay_line_t<T>::sampling_time() const
{
  return m_sampling_time;
}
template <class T>
void delay_line_t<T>::delay_time(const value_type& a_time)
{
  m_delay_time = a_time;
  m_sample_list.resize(round(m_delay_time/m_sampling_time, size_t()),
    m_fill_value);
}
template <class T>
typename delay_line_t<T>::value_type delay_line_t<T>::delay_time() const
{
  return m_delay_time;
}
template <class T>
void delay_line_t<T>::reset(const value_type& a_fill_value)
{
  fill(m_sample_list.begin(), m_sample_list.end(), a_fill_value);
}
template <class T>
void delay_line_t<T>::resize(size_type a_size)
{
  m_sample_list.resize(a_size, m_fill_value);
  m_delay_time = a_size*m_sampling_time;
}
template <class T>
typename delay_line_t<T>::size_type delay_line_t<T>::size() const
{
  return m_sample_list.size();
}

//#define IRS_DELAY_LINE_TEST
#ifdef IRS_DELAY_LINE_TEST
template <class T>
inline void delay_line_test_by_type()
{
  delay_line_t<T> delay_line(10, 1, 0);
  mlog() << delay_line.tick(7) << endl;
  delay_line.sampling_time(1.7);
  mlog() << delay_line.sampling_time() << endl;
  delay_line.delay_time(3.14);
  mlog() << delay_line.delay_time() << endl;
  delay_line.reset(0.75);
  delay_line.resize(10);
  mlog() << delay_line.size() << endl;
}
inline void delay_line_test()
{
  delay_line_test_by_type<float>();
  delay_line_test_by_type<double>();
  delay_line_test_by_type<long double>();
}
#endif //IRS_DELAY_LINE_TEST

#ifndef NOP
// ������� ������� f(x) = k*x + b
template <class T>
class linear_func_t
{
public:
  typedef T value_t;

  linear_func_t(const value_t& a_k, const value_t& a_b);
  linear_func_t(const value_t& a_x1, const value_t& a_y1,
    const value_t& a_x2, const value_t& a_y2);
  value_t get(const value_t& a_value) const;
  value_t get_inverse(const value_t& a_value) const;
  value_t k() const;
  void k(const value_t& a_value);
  value_t b() const;
  void b(const value_t& a_value);
  void def_on_double_point(const value_t& a_x1, const value_t& a_y1,
    const value_t& a_x2, const value_t& a_y2);
private:
  value_t m_k;
  value_t m_b;
};
template <class T>
linear_func_t<T>::linear_func_t(const value_t& a_k,
  const value_t& a_b
):
  m_k(a_k),
  m_b(a_b)
{
}
template <class T>
linear_func_t<T>::linear_func_t(const value_t& a_x1, const value_t& a_y1,
  const value_t& a_x2, const value_t& a_y2
):
  m_k(0),
  m_b(0)
{
  def_on_double_point(a_x1, a_y1, a_x2, a_y2);
}
template <class T>
typename linear_func_t<T>::value_t linear_func_t<T>::
  get(const value_t& a_value) const
{
  return m_k*a_value + m_b;
}
template <class T>
typename linear_func_t<T>::value_t linear_func_t<T>::get_inverse(
  const value_t& a_value) const
{
  return (a_value - m_b)/m_k;
}
template <class T>
typename linear_func_t<T>::value_t linear_func_t<T>::k() const
{
  return m_k;
}
template <class T>
void linear_func_t<T>::k(const value_t& a_value)
{
  m_k = a_value;
}
template <class T>
typename linear_func_t<T>::value_t linear_func_t<T>::b() const
{
  return m_b;
}
template <class T>
void linear_func_t<T>::b(const value_t& a_value)
{
  m_b = a_value;
}
template <class T>
void linear_func_t<T>::def_on_double_point(const value_t& a_x1,
  const value_t& a_y1, const value_t& a_x2, const value_t& a_y2)
{
  m_k = (a_y2 - a_y1)/(a_x2 - a_x1);
  m_b = a_y1 - m_k*a_x1;
}
#endif //NOP

//! @}

} // namespace irs

#endif //irsdspH

