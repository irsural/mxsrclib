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
  virtual ~dac_t() {}
  virtual irs_status_t get_status() const { return irs_st_ready; }
  virtual size_t get_resolution() const = 0;
  virtual inline irs_u32 get_u32_maximum() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data) = 0;
  virtual inline float get_float_maximum() const;
  virtual inline void set_float_data(size_t a_channel, const float a_data);
  virtual void tick() = 0;
private:
  inline irs_u32 get_dac_max_value() const;
};

inline irs_u32 dac_t::get_u32_maximum() const
{
  return static_cast<irs_u32>(get_dac_max_value()) << (32 - get_resolution());
}

inline irs_u32 dac_t::get_dac_max_value() const
{
  return (1 << get_resolution()) - 1;
}

inline float dac_t::get_float_maximum() const
{
  return 1.0f;
}

inline void dac_t::set_float_data(size_t a_channel, const float a_data)
{
  set_u32_data(a_channel, static_cast<irs_u32>(a_data*get_u32_maximum()));
}

//! @}

} // namespace irs

#endif  //  irsdacabsH
