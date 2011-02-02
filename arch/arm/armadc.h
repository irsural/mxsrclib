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
    PORTB_VREFA = 0x01000000,
    PORTD_AIN5 = 0x01000000,
    PORTD_AIN6 = 0x00100000,
    PORTD_AIN7 = 0x00010000,
    
    INTERNAL_VREF = 0x0
  };
  enum status_t {
    begin_ADC_sampling,
    data_from_channel
  };
  enum { m_channel_max_number = 4 };
  enum { m_internal_temp_idx = 3 };
  
  status_t m_status;
  irs::raw_data_t<adc_data_t> mp_data;
  irs::timer_t m_adc_timer;
};

} // namespace arm

} // namespace irs

#endif //armadc
