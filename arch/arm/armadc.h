// Класс для работы с АЦП
// Дата: 14.01.2011
// Ранняя дата: 13.01.2011

#ifndef armadc
#define armadc

#include <irsdefs.h>

#include <irsstd.h>
#include <irsarchint.h>
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {

namespace arm {

class adc_t
{
public: 
  typedef irs_u16 adc_data_t;
  
  adc_t(counter_t a_adc_timer = irs::make_cnt_ms(100));
  ~adc_t();
  void tick();
  float get_data(irs_u8 a_channel);
  float get_temperature();
 
private:
  enum {
    AIN0  = 0x0,
    AIN1  = 0x1,
    AIN2  = 0x2,
    AIN3  = 0x3,
    AIN4  = 0x4,
    AIN5  = 0x5,
    AIN6  = 0x6,
    AIN7  = 0x7,
    AIN8  = 0x8,
    AIN9  = 0x9,
    AIN10 = 0xA,
    AIN11 = 0xB,
    AIN12 = 0xC,
    AIN13 = 0xD,
    AIN14 = 0xE,
    AIN15 = 0xF,
    
    INTERNAL_VREF = 0x0,
    EXTERNAL_REF = 0x1
  };
  enum status_t {
    begin_ADC_sampling,
    data_from_channel
  };
  enum {
    m_SSI0_channel_max_number = 8,
    m_channel_max_number = 11,
    m_internal_temp_idx = 10
  };
  
  status_t m_status;
  irs::raw_data_t<adc_data_t> mp_data;
  irs::timer_t m_adc_timer;
};

#ifdef __LM3SxBxx__

class adc_full_t
{
public: 
  typedef irs_u16 adc_data_t;
  typedef irs_u16 select_channel_type;
  enum adc_module_t
  {
    ADC0 = ADC0_BASE,
    ADC1 = ADC1_BASE
  };
  enum adc_ref_t
  {
    INT_REF = 0,
    EXT_REF = 1
  };
  
  adc_full_t(select_channel_type a_selected_channels, 
    adc_ref_t a_adc_ref = INT_REF,
    adc_module_t a_adc_module = ADC0,
    counter_t a_adc_interval = make_cnt_ms(100));
  ~adc_full_t();
  void set_adc_ref_voltage(float a_ref_voltage);
  void reselect_channels(select_channel_type a_selected_channels);
  float get_temperature();
  float get_data(irs_u8 a_channel);
  void tick();
private:
  enum status_t
  {
    WAIT,
    SEQUENCE_START,
    READ_RESULTS
  };
  status_t m_status;
  loop_timer_t m_adc_timer;
  vector<adc_data_t> m_result_vector;
  irs_u32 m_adc_base_addr;
  float m_ref_voltage;
};

#endif  //  __LM3SxBxx__
  
} // namespace arm

} // namespace irs

#endif //armadc
