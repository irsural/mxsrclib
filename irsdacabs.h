//! \file
//! \ingroup drivers_group
//! \brief Абстрактный базовый класс для ЦАП
//!
//! Дата создания: 14.09.2012

#ifndef irsdacabsH
#define irsdacabsH

#include <irsdefs.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup drivers_group
//! @{

class dac_t
{
public:
  typedef irs_u32 select_channel_type;
  virtual ~dac_t() {};
  virtual irs_status_t get_status() const { return irs_st_ready; }
  virtual size_t get_resolution() const = 0;
  virtual irs_u32 get_u32_maximum() const = 0;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data) = 0;
  virtual float get_float_maximum() const = 0;
  virtual void set_float_data(size_t a_channel, const float a_data) = 0;
  virtual void tick() = 0;
};

//! @}

} // namespace irs

#endif  //  irsdacabsH
