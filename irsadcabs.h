//! \file
//! \ingroup drivers_group
//! \brief Абстрактный базовый класс для АЦП
//!
//! Дата: 28.06.2011
//! Ранняя дата: 28.06.2011

#ifndef irsadcabsH
#define irsadcabsH

#include <irsdefs.h>

#include <irscpu.h>
#include <measdef.h>

#include <irsfinal.h>

namespace irs
{

//! \addtogroup drivers_group
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

class adc_request_t
{
public:
  virtual ~adc_request_t() {};
  virtual void start() = 0;
  virtual meas_status_t status() const = 0; 
  virtual void set_channel(irs_u8 a_channel) = 0;
  virtual void set_mode(irs_u8 a_mode) = 0;
  virtual void set_freq(irs_u8 a_freq) = 0;
  virtual void set_gain(irs_u8 a_gain) = 0;
  virtual irs_u32 get_value() const = 0;
  virtual void tick() = 0;
};

class adc_dma_t
{
public:
  typedef irs_u32 select_channel_type;
  
  virtual ~adc_dma_t() {};
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual bool status() = 0;
  virtual void set_frequency(cpu_traits_t::frequency_type a_frequency) = 0;
  virtual void set_prescaler(irs_u16 a_psc) = 0;
  virtual void set_buff(irs::c_array_view_t<irs_u16> ap_buff) = 0;
};

//! @}

} // namespace irs

#endif  //  irsadcabsH
