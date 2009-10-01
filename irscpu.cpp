// Свойства процессора
// Дата: 1.10.2009
// Дата создания: 1.10.2009

#include <irscpu.h>

#ifdef __ICCAVR__
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 16000000;
#else //__ICCAVR__
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 1;
#endif //__ICCAVR__
void irs::cpu_traits_t::frequency(frequency_type a_frequency)
{
  m_frequency = a_frequency;
}
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::frequency()
{
  return m_frequency;
}
