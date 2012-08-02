//! \file
//! \ingroup configuration_group
//! \brief Свойства процессора
//!
//! Дата: 12.11.2010\n
//! Дата создания: 1.10.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irscpu.h>

#include <irsfinal.h>

#ifdef __ICCAVR__
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 16000000;
#elif __ICCARM__
#if defined(__LM3Sx9xx__)
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 50000000;
#elif defined (__LM3SxBxx__)
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 80000000;
#elif defined(__STM32F100RBT__)
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 8000000;
#else
  #error Тип контроллера не определён
#endif // ARM_device
#else //__ICCAVR
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
irs::cpu_traits_t::endian_type irs::cpu_traits_t::endian()
{
  static endian_t cpu_endian = detect_cpu_endian();
  return cpu_endian;
}
