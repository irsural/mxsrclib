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
  static endian_t endian();
private:
  static frequency_type m_frequency;
};

//! @}

} //namespace irs

#endif //IRSCPUH
