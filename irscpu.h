//! \file
//! \ingroup configuration_group
//! \brief �������� ����������
//!
//! ����: 14.10.2009\n
//! ���� ��������: 1.10.2009

#ifndef IRSCPUH
#define IRSCPUH

#include <irsdefs.h>

#ifdef IRS_STM32_F2_F4_F7
#include <armioregs.h>
#endif // IRS_STM32_F2_F4_F7

#include <irsfinal.h>

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


  // ������� ����������, ��
  static void frequency(frequency_type a_frequency);
  static frequency_type frequency();
  static endian_t endian();
  #ifdef IRS_STM32_F2_F4_F7
  static frequency_type periphery_frequency_first();
  static void periphery_frequency_first(frequency_type a_frequency);
  static frequency_type periphery_frequency_second();
  static void periphery_frequency_second(frequency_type a_frequency);
  static frequency_type timer_frequency(size_t a_timer_base);
  static double flash_voltage();
  static void flash_voltage(double a_flash_voltage);
  #endif // IRS_STM32_F2_F4_F7
private:
  static frequency_type m_frequency;
  #ifdef IRS_STM32_F2_F4_F7
  static frequency_type m_periphery_frequency_first;
  static frequency_type m_periphery_frequency_second;
  static double m_flash_voltage;
  #endif // IRS_STM32_F2_F4_F7
};

//! @}

} //namespace irs

#endif //IRSCPUH
