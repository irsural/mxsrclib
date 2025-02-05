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

#ifdef IRS_STM32_F2_F4_F7
#include <armioregs.h>
#endif // IRS_STM32_F2_F4_F7

#include <irsfinal.h>

#ifdef __ICCAVR__
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 16000000;
#elif __ICCARM__
#if defined(__LM3Sx9xx__)
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 50000000;
#elif defined(__LM3SxBxx__)
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 80000000;
#elif defined(__STM32F100RBT__)
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 8000000;
#elif defined(IRS_STM32F2xx)
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 120000000;
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_periphery_frequency_first = 30000000;
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_periphery_frequency_second = 60000000;
double irs::cpu_traits_t::m_flash_voltage = 3.3;
#elif (defined(IRS_STM32F4xx) || defined(IRS_STM32F7xx))
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 16000000;
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_periphery_frequency_first = 16000000;
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_periphery_frequency_second = 16000000;
double irs::cpu_traits_t::m_flash_voltage = 3.3;
#else  // Другой __ICCARM__
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::m_frequency = 100000000;
#endif // ARM_device
#else  //__ICCAVR
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

#ifdef IRS_STM32_F2_F4_F7
irs::cpu_traits_t::frequency_type irs::cpu_traits_t::periphery_frequency_first()
{
  return m_periphery_frequency_first;
}

void irs::cpu_traits_t::periphery_frequency_first(frequency_type a_frequency)
{
  m_periphery_frequency_first = a_frequency;
}

irs::cpu_traits_t::frequency_type irs::cpu_traits_t::periphery_frequency_second()
{
  return m_periphery_frequency_second;
}

void irs::cpu_traits_t::periphery_frequency_second(frequency_type a_frequency)
{
  m_periphery_frequency_second = a_frequency;
}

irs::cpu_traits_t::frequency_type irs::cpu_traits_t::timer_frequency(size_t a_timer_base)
{
  if ((a_timer_base == IRS_TIM1_PWM1_BASE) || (a_timer_base == IRS_TIM8_PWM2_BASE) ||
      (a_timer_base == IRS_TIM9_BASE) || (a_timer_base == IRS_TIM10_BASE) ||
      (a_timer_base == IRS_TIM11_BASE))
  {
    return m_periphery_frequency_second * 2;
  } else {
    return m_periphery_frequency_first * 2;
  }
}

double irs::cpu_traits_t::flash_voltage()
{
  return m_flash_voltage;
}

void irs::cpu_traits_t::flash_voltage(double a_flash_voltage)
{
  m_flash_voltage = a_flash_voltage;
}

#endif // IRS_STM32_F2_F4_F7

irs::cpu_traits_t::endian_type irs::cpu_traits_t::endian()
{
  static endian_t cpu_endian = detect_cpu_endian();
  return cpu_endian;
}
