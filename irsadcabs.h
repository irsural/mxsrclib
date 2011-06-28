//! \file
//! \ingroup drivers_group
//! \brief Абстрактный базовый класс для АЦП
//!
//! Дата: 28.06.2011
//! Ранняя дата: 28.06.2011

#ifndef irsadcabsH
#define irsadcabsH

#include <irsdefs.h>

#include <irsfinal.h>

namespace irs
{

//! \addtogroup string_processing_group
//! @{

class adc_t
{
public:
  typedef irs_u32 select_channel_type;

  virtual ~adc_t() {};
  virtual irs_u16 get_u16_minimum() = 0;
  virtual irs_u16 get_u16_maximum() = 0;
  virtual irs_u16 get_u16_data(irs_u8 a_channel) = 0;
  virtual irs_u32 get_u32_minimum() = 0;
  virtual irs_u32 get_u32_maximum() = 0;
  virtual irs_u32 get_u32_data(irs_u8 a_channel) = 0;
  virtual float get_float_minimum() = 0;
  virtual float get_float_maximum() = 0;
  virtual float get_float_data(irs_u8 a_channel) = 0;
  virtual float get_temperature() = 0;
  virtual void tick() = 0;
};

//! @}

} // namespace irs

#endif  //  irsadcabsH
