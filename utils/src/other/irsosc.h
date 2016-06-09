//! \file
//! \ingroup signal_processing_group
//! \brief Цифровая обработка сигналов
//!
//! Дата: 27.08.2009

#ifndef irsdspH
#define irsdspH

#include <stdlib.h>

template <class T>
inline T sqr (T x)
{
  return x*x;
}

//  Колебательный контур
template <class T>
class osc_cir_t
{
public:
  osc_cir_t(T a_n_period, T a_n_fade, T a_y1_init, T a_x1_init = T(),
    T a_y2_init = T(),  T a_x2_init = T());
  ~osc_cir_t();
  //  Число точек на период центральной частоты
  void set_num_of_period_points(T a_n_period);
  //  Число периодов центральной частоты, когда колебания затухают в e раз
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



typedef float main_float_t;
inline int example()
{
  main_float_t Fd = 48000;
  main_float_t Td = 1/Fd;
  main_float_t f_center = 125;
  main_float_t n_t_center = Fd/f_center;
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

#endif //irsdspH
