//! \file
//! \ingroup configuration_group
//! \brief Свойства процессора
//!
//! Дата: 14.10.2009\n
//! Дата создания: 1.10.2009

#ifndef IRSCPUH
#define IRSCPUH

#include <irsdefs.h>

namespace irs {

//! \addtogroup system_utils_group
//! @{

class cpu_traits_t
{
public:
  #ifdef __ICCAVR__
  typedef irs_u32 frequency_type;
  #else //__ICCAVR__
  typedef irs_umax frequency_type;
  #endif //__ICCAVR__
  typedef endian_t endian_type;


  // Частота процессора, Гц
  static void frequency(frequency_type a_frequency);
  static frequency_type frequency();
  #ifdef IRS_STM32F2xx
  static frequency_type periphery_frequency_first();
  static void periphery_frequency_first(frequency_type a_frequency);
  static frequency_type periphery_frequency_second();
  static void periphery_frequency_second(frequency_type a_frequency);
  #endif // IRS_STM32F2xx
  static endian_t endian();
  #ifdef IRS_STM32F2xx
  static frequency_type timer_frequency(size_t a_timer_base);
  #endif // IRS_STM32F2xx
private:
  static frequency_type m_frequency;
  #ifdef IRS_STM32F2xx
  static frequency_type m_periphery_frequency_first;
  static frequency_type m_periphery_frequency_second;
  #endif // IRS_STM32F2xx
};

//! @}

} //namespace irs

#endif //IRSCPUH
