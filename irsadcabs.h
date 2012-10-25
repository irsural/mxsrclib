//! \file
//! \ingroup drivers_group
//! \brief ����������� ������� ����� ��� ���
//!
//! ����: 28.06.2011
//! ������ ����: 28.06.2011

#ifndef irsadcabsH
#define irsadcabsH

#include <irsdefs.h>

#include <irscpu.h>

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

class adc_dma_t
{
public:
  typedef irs_u32 select_channel_type;
  
  virtual ~adc_dma_t() {};
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual bool status() = 0;
  virtual void set_frequency(cpu_traits_t::frequency_type a_frequency) = 0;
  virtual void set_size(size_t a_size) = 0;
  virtual void set_prescaler(irs_u16 a_psc) = 0;
};

//! @}

} // namespace irs

#endif  //  irsadcabsH
