// Класс для работы с АЦП
// Дата: 17.05.2010
// Ранняя дата: 23.04.2010

#ifndef avradc
#define avradc

#include <irsdefs.h>

#include <irsstd.h>

#include <irsfinal.h>

namespace irs {

namespace avr {

class adc_t
{
public: 
  typedef irs_u16 adc_data_t;
  typedef irs_u8 select_channel_type;
  
  adc_t(select_channel_type a_selected_channels);
  ~adc_t();
  void tick();
  float get_data();
  float get_data(irs_u8 a_channel);
  void reselect_channels(select_channel_type a_selected_channels);
 
private:
  enum status_t {
    init_ADC_channel,
    data_from_channel
  };
  enum { m_channel_max_number = 8 };
  
  select_channel_type m_selected_channels;
  irs_u8 m_cur_channel;
  status_t m_status;
  adc_data_t mp_data[m_channel_max_number];
  irs::timer_t m_sample_timer;
  irs::timer_t m_delay_timer;
  
  bool channel_exists(irs_u8 a_channel_ident);
  void init_channels();
  void deinit_channels();
  void next_channel();
};

} // namespace avr

} // namespace irs

#endif //avradc
