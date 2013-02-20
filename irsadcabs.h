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

enum adc_param_t {
  adc_mode = 0,
  adc_gain = 1,
  adc_channel = 2,
  adc_freq = 3,
  adc_buf = 4,
  adc_ref_det = 5,
  adc_unipolar = 6,
  adc_burnout = 7,
  adc_offset = 8,
  adc_full_scale = 9
};
  
const int adc_mode_continuous_conversion = 0;
const int adc_mode_single_conversion = 1;
const int adc_mode_idle = 2;
const int adc_mode_power_down = 3;
const int adc_mode_internal_zero_scale = 4;
const int adc_mode_internal_full_scale = 5;
const int adc_mode_system_zero_scale = 6;
const int adc_mode_system_full_scale = 7;
  
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
  virtual void stop() = 0;
  virtual meas_status_t status() const = 0;
  virtual void set_param(adc_param_t a_param, const int a_value) = 0;
  virtual void get_param(adc_param_t a_param, int* ap_value) = 0;
  virtual irs_i32 get_value() = 0;
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
