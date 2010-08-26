// Цифровая обработка сигналов
// Дата: 13.04.2010
// Ранняя дата: 27.08.2009

#ifndef irsdspH
#define irsdspH

//#define NEW

#include <irsdefs.h>

#include <irscpp.h>
#include <irsstd.h>

#include <irsfinal.h>

namespace irs {

#ifdef NEW
template <class T>
struct basic_pid_data_t {
  // Общий коэффициент передачи регулятора
  T k;
  // Коэффициент передачи интегратора
  T ki;
  // Коэффициент передачи дифференциатора
  T kd;
  // Минимальное значение на выходе
  T min;
  // Максимальное значение на выходе
  T max;
  // Предыдущее значение ошибки
  T prev_e;
  // Предпредыдущее значение ошибки
  T pp_e;
  // Предыдущее значение на выходе регулятора
  T prev_out;
  // Блокировка интегратора
  int block_int;
  // Внешняя блокировка интегратора
  int block_int_ext;
  // Сумма на интеграторе
  T int_val;
  // Число на которое умножается диапазон регулятора, чтобы получить
  // дополнительный диапазон сверху и снизу где интегратор не сбрасывается.
  // Рекомендуемое значение 0.1
  T k_d_pid;
};
typedef basic_pid_data_t<float> pid_data_float_t;
typedef basic_pid_data_t<double> pid_data_double_t;
typedef pid_data_double_t pid_data_t;

// Структура с данными для колебательного звена
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
  // число отсчетов при котором происходит затухание (fading) в e раз
  T nf;
  // период колебания (oscillation) в отсчетах
  T no;
};
typedef basic_osc_data_t<float> osc_data_float_t;
typedef basic_osc_data_t<double> osc_data_double_t;
typedef osc_data_double_t osc_data_t;

// Структура с данными для инерционного звена
template <class T>
struct basic_fade_data_t {
  // x[n-1]
  T x1;
  // y[n-1]
  T y1;
  // Постоянная времени
  T t;
};
typedef basic_fade_data_t<float> fade_data_float_t;
typedef basic_fade_data_t<double> fade_data_double_t;
typedef fade_data_t fade_data_double_t;

// Структура с данными для изодромного звена
template <class T>
struct basic_isodr_data_t {
  // k - коэффициент передачи пропорциональной части
  T k;
  // Структура данных инерционного звена
  fade_data_t fd;
};
typedef basic_isodr_data_t<float> isodr_data_float_t;
typedef basic_isodr_data_t<double> isodr_data_double_t;
typedef isodr_data_t isodr_data_double_t;


// ПИД-регулятор
template <class T>
T pid_reg(basic_pid_data_t<T> *pd, T e)
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
  T PDres = pd->k*(e + pd->kd*(e - pd->prev_e));
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
  pd->prev_e = e;
  pd->prev_out = res;
  return res;
}
// Синхронизация ПИД-регулятора на текущее значение регулируемого параметра
// при выключенном регуляторе
// pd - Параметры ПИД-регулятора,
// e - ошибка на входе регулятора
// rp - значение выдаваемое на исполнительный механизм на который
// осуществляется синхронизация
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
// Синхронизация ПИД-регулятора на текущее значение регулируемого параметра
// при включенном регуляторе
// pd - Параметры ПИД-регулятора,
template <class T>
void pid_reg_sync(basic_pid_data_t<T> *pd)
{
  if ((pd->k == 0.) || (pd->ki == 0.)) {
    pd->int_val = 0.;
  } else {
    T PDres = pd->k*(pd->prev_e + pd->kd*(pd->prev_e - pd->pp_e));
    T d_pid = pd->k_d_pid*(pd->max - pd->min);
    if ((PDres > -d_pid) && (PDres < pd->max)) {
      pd->int_val = (pd->prev_out - pd->k*(pd->prev_e +
        pd->kd*(pd->prev_e - pd->pp_e)))
        /(pd->k*pd->ki);
    } else {
      pd->int_val = 0.;
    }
  }
}

// Колебательное звено
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

// Инерционное звено
template <class T>
T fade(basic_fade_data_t<T> *fd, T x)
{
  T y = (x + fd->x1 - (1 - 2*fd->t)*fd->y1)/(1 + 2*fd->t);
  fd->x1 = x;
  fd->y1 = y;
  return y;
}

// Изодромное звено
// id - структура данных изодромного звена
// x - входное значение
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
    // Фильтр 1-го порядка после датчика
    sensor_filter_unit = 1,
    // Изодромное звено после фильтра или датчика
    sensor_isodr_unit = 2,
    // Ограничитель темпа уставки
    ref_rate_limiter_unit = 4,
    // Синхронизация коэффициентов
    sync_gains = 8,
    // Включение зоны нечувствительности регулятора по датчику
    dead_band = 16,
    // нормирование пропорционального коэффициента
    norm_prop_gain = 32,
    // адаптирование пропорционального коэффициента
    adapt_prop_gain = 64,
    // быстрый выход на режим
    fast_mode = 128
  };

  pid_regulator_t(const &T a_proportional_gain, const &T a_integral_gain,
    const &T a_derivative_gain, const &T a_out_min, const &T a_out_max,
    options_t a_options = sync_gains);
  ~pid_regulator_t();

  void start(const &T a_integrator = T());
  void stop();
  void proportional_gain(const &T a_proportional_gain);
  T proportional_gain();
  void integral_gain(const &T a_integral_gain);
  T integral_gain();
  void derivative_gain(const &T a_derivative_gain);
  T derivative_gain();
  void out_min(const &T a_out_min);
  T out_min();
  void out_max(const &T a_out_max);
  T out_max();
  void integrator(const &T a_integrator);
  T integrator();
private:
};

#else //NEW
// Структура для ПИД-регулятора
typedef struct _pid_data {
  // Общий коэффициент передачи регулятора
  double k;
  // Коэффициент передачи интегратора
  double ki;
  // Коэффициент передачи дифференциатора
  double kd;
  // Минимальное значение на выходе
  double min;
  // Максимальное значение на выходе
  double max;
  // Предыдущее значение ошибки
  double prev_e;
  // Предпредыдущее значение ошибки
  double pp_e;
  // Предыдущее значение на выходе регулятора
  double prev_out;
  // Блокировка интегратора
  int block_int;
  // Внешняя блокировка интегратора
  int block_int_ext;
  // Сумма на интеграторе
  double int_val;
  // Число на которое умножается диапазон регулятора, чтобы получить
  // дополнительный диапазон сверху и снизу где интегратор не сбрасывается.
  // Рекомендуемое значение 0.1
  double k_d_pid;
} pid_data_t;

// Структура с данными для колебательного звена
typedef struct _osc_data {
  // x[n-1]
  double x1;
  // x[n-2]
  double x2;
  // y[n-1]
  double y1;
  // y[n-2]
  double y2;
  // число отсчетов при котором происходит затухание (fading) в e раз
  double nf;
  // период колебания (oscillation) в отсчетах
  double no;
} osc_data_t;

// Структура с данными для инерционного звена
typedef struct _fade_data {
  // x[n-1]
  double x1;
  // y[n-1]
  double y1;
  // Постоянная времени
  double t;
} fade_data_t;

// Структура с данными для изодромного звена
typedef struct _isodr_data {
  // k - коэффициент передачи пропорциональной части
  double k;
  // Структура данных инерционного звена
  fade_data_t fd;
} isodr_data_t;
#endif //NEW

// Структура для ограничителя темпа
template <class T>
struct rate_limiter_t {
  // Текущее значение параметра
  T cur;
  // Целевое значение параметра
  //T ref;
  // Шаг вызова ф-ции по времени, с
  T dt;
  // Крутизна, <ед. изм>/c
  T slope;
  // Максимальное изменение величины за шаг вызова
  T dl;
  // Предыдущее значение шага вызова ф-ции по времени, с
  T dt_prev;
  // Предыдущее значение крутизны, <ед. изм>/мин.
  T slope_prev;
  // Принудительный пересчет dl
  bool recalc;
};

#ifndef NEW
// ПИД-регулятор
double pid_reg(pid_data_t *pd, double e);
// Синхронизация ПИД-регулятора на текущее значение регулируемого параметра
// при выключенном регуляторе
// pd - Параметры ПИД-регулятора,
// e - ошибка на входе регулятора
// rp - значение выдаваемое на исполнительный механизм на который
// осуществляется синхронизация
void pid_reg_sync(pid_data_t *pd, double e, double rp);
// Синхронизация ПИД-регулятора на текущее значение регулируемого параметра
// при включенном регуляторе
// pd - Параметры ПИД-регулятора,
void pid_reg_sync(pid_data_t *pd);
// Колебательное звено
double osc(osc_data_t *od, double x);
// Инерционное звено
double fade(fade_data_t *fd, double x);
// Изодромное звено
// id - структура данных изодромного звена
// x - входное значение
double isodr(isodr_data_t *id, double x);
#endif //NEW
// Ограничитель темпа
// otd - структура данных для ограничителя темпа
// in - ограничиваемая последовательность
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

// Проверяет входит ли переменная in в диапазон min - max и если не входит,
// заменяет ее значение на ближайшую границу диапазона
template <class T>
T bound(T in, T min, T max)
{
  if (in < min) in = min;
  if (in > max) in = max;
  return in;
}
// Возведение в квадрат
template <class T> T sqr(T x) { return x*x; }

// БПФ от комплексного массива
#ifdef __WATCOMC__
// По какой-то причине Watcom C++ не поддерживает передачу аргументов
// контейнеров напрямую
template <class T, class V = vector<complex<T> > >
void fft(V& signal)
#else //__WATCOMC__
template <class T>
void fft(vector<complex<T> >& signal)
#endif //__WATCOMC__
{
  if (signal.empty()) return;
  int degree = 1;
  for (int i = signal.size() - 1; i >>= 1;) degree++;
  int size = 1 << degree;
  signal.resize(size);
  for (int first_idx = 0; first_idx < size; first_idx++)
  {
    int second_idx = 0;
    for (int create_idx = 0; create_idx < degree; create_idx++) {
      second_idx |= ((first_idx >> (degree - create_idx - 1))&1) << create_idx;
    }
    if (second_idx > first_idx) {
      swap(signal[first_idx], signal[second_idx]);
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

// Обратное БПФ от комплексного массива
#ifdef __WATCOMC__
// По какой-то причине Watcom C++ не поддерживает передачу аргументов
// контейнеров напрямую
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
    freq_charact[i] = conj(freq_charact[i])/
      (double)freq_charact.size();
  }
}

//  Приведение результатов БПФ к божескому виду
#ifdef __WATCOMC__
// По какой-то причине Watcom C++ не поддерживает передачу аргументов
// контейнеров напрямую
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

//  Вычисление коэффициента нелинейных искажений
#ifdef __WATCOMC__
// По какой-то причине Watcom C++ не поддерживает передачу аргументов
// контейнеров напрямую
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

//  Вычисление БПФ. Не сразу.
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
  mp_out_buf((T*)mp_buf),
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
  if (size + a_index > m_size) size = irs_uarc(m_size - a_index);
  memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
}

template <class T, irs_uarc N>
void fft_process_t<T, N>
  ::write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = a_size;
  if (size + a_index > m_size) size = irs_uarc(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
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

//  Посторонние функции

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

//  Колебательный контур

template <class T>
class osc_cir_t
{
public:
  osc_cir_t(T a_n_period, T a_n_fade, T a_y_init);
  ~osc_cir_t();
  //  Число точек на период центральной частоты
  void set_num_of_period_points(T a_n_period);
  //  Число периодов центральной частоты, когда колебания затухают в e раз
  void set_num_of_fade_points(T a_n_fade);
  void sync(T a_y);
  T filt(T a_x);
private:
  T m_n_period;
  T m_freq_2;
  T m_fade;
  T m_a;
  T m_b;
  T m_c;
  T m_denom;
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

template <class T>
osc_cir_t<T>::osc_cir_t(T a_n_period, T a_n_fade, T a_y_init):
  m_n_period(a_n_period),
  m_fade(1. / a_n_fade),
  m_freq_2(sqr(2.* tan(IRS_PI / m_n_period)) - sqr(m_fade)),
  m_denom(calc_denom()),
  m_a(m_denom * calc_a()),
  m_b(m_denom * calc_b()),
  m_c(m_denom * calc_c()),
  m_x1(a_y_init),
  m_x2(a_y_init),
  m_y1(a_y_init),
  m_y2(a_y_init)
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
  m_freq_2 = sqr(2.* tan(IRS_PI / m_n_period)) - sqr(m_fade);
  m_denom = calc_denom();
  m_a = m_denom * calc_a();
  m_b = m_denom * calc_b();
  m_c = m_denom * calc_c();
}

template <class T>
void osc_cir_t<T>::set_num_of_fade_points(T a_n_fade)
{
  m_fade = 1. / a_n_fade;
  m_freq_2 = sqr(2.* tan(IRS_PI / m_n_period)) - sqr(m_fade);
  m_denom = calc_denom();
  m_a = m_denom * calc_a();
  m_b = m_denom * calc_b();
  m_c = m_denom * calc_c();
}

template <class T>
void osc_cir_t<T>::sync(T a_y)
{
  m_x1 = a_y;
  m_x2 = a_y;
  m_y1 = 0;
  m_y2 = 0;
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

//  Инерционное звено

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
  T m_t;  // Постоянная времени
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

enum filter_type_t {
  ft_butterworth,
  ft_chebyshev,
  ft_elliptic
};

struct filter_settings_t {
  typedef size_t size_type;
  filter_type_t type;
  size_type order;
  double sampling_time_s;
  double low_cutoff_freq_hz;
  double high_cutoff_freq_hz;
  double stopband_ripple_db;
  double passband_ripple_db;

  filter_settings_t(
    filter_type_t a_type,
    size_type a_order,
    double a_sampling_freq_hz,
    double a_low_cutoff_freq_hz,
    double a_high_cutoff_freq_hz,
    double a_stopband_ripple_db = 0,
    double a_passband_ripple_db = 0
  ):
    type(a_type),
    order(a_order),
    sampling_time_s(a_sampling_freq_hz),
    low_cutoff_freq_hz(a_low_cutoff_freq_hz),
    high_cutoff_freq_hz(a_high_cutoff_freq_hz),
    stopband_ripple_db(a_stopband_ripple_db),
    passband_ripple_db(a_passband_ripple_db)
  {
  }
};

} //  irs

#endif //irsdspH

