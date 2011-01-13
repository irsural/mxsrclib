// Дата: 18.05.2010
// Ранняя дата: 23.04.2010

#include <irsdefs.h>

#include <ioavr.h>

#include <avradc.h>

#include <mxdata.h>

#include <irsfinal.h>

inline irs_u8 mask(irs_u8 a_index)
{
  irs_u8 right_shift = a_index; // -1
  irs_u8 mask = 1;
  mask <<= right_shift;
  return mask;
}

irs::avr::adc_t::adc_t(
  select_channel_type a_selected_channels
):
  m_selected_channels(a_selected_channels),
  m_cur_channel(0),
  m_status(init_ADC_channel),
  m_sample_timer(irs::make_cnt_ms(90)), // частота обработки АЦП
  m_delay_timer(irs::make_cnt_ms(10))
{
  memsetex(mp_data, m_channel_max_number);
    
  ADMUX_REFS1 = 0;   //  REF = AREF
  ADMUX_REFS0 = 0;
  ADMUX_ADLAR = 0;   //  Right data align
  ADMUX_MUX4 = 0;    //  Channel select
  ADMUX_MUX3 = 0;
  ADMUX_MUX2 = 0;
  ADMUX_MUX1 = 0;
  ADMUX_MUX0 = 1;
  ADCSRA_ADEN = 1;   //  ADC enable
  ADCSRA_ADSC = 0;   //  No start conversion
  ADCSRA_ADATE = 0;  //  Autotriggering off
  ADCSRA_ADIF = 0;   //  Interrupt flag
  ADCSRA_ADIE = 0;   //  Interrupt disable
  ADCSRA_ADPS2 = 1;  //  Clock division factor = 64
  ADCSRA_ADPS1 = 1;
  ADCSRA_ADPS0 = 0;
  ADCSRB_ADTS2 = 0;  // Autotriggering source
  ADCSRB_ADTS1 = 0;
  ADCSRB_ADTS0 = 0;
  
  DIDR0_ADC1D = 0;
  DIDR0_ADC2D = 0;
  DIDR0_ADC3D = 0;
  DIDR0_ADC4D = 0;
  DIDR0_ADC5D = 0;
  DIDR0_ADC6D = 0;
  DIDR0_ADC7D = 0;
  DIDR1_AIN0D = 0;
  DIDR1_AIN1D = 0;
  
  init_channels();
  
  m_delay_timer.start();
}

irs::avr::adc_t::~adc_t()
{
  DIDR0_ADC1D = 0;
  DIDR0_ADC2D = 0;
  DIDR0_ADC3D = 0;
  DIDR0_ADC4D = 0;
  DIDR0_ADC5D = 0;
  DIDR0_ADC6D = 0;
  DIDR0_ADC7D = 0;

  ADCSRA_ADEN = 0;
  ADCSRA_ADSC = 0;
}

void irs::avr::adc_t::reselect_channels(select_channel_type a_selected_channels)
{
  deinit_channels();
  m_selected_channels = a_selected_channels;
  init_channels();
  m_delay_timer.start();
  next_channel();
}

bool irs::avr::adc_t::channel_exists(irs_u8 a_channel_ident)
{
  return (mask(a_channel_ident) & m_selected_channels);
}

void irs::avr::adc_t::init_channels()
{
  DIDR0 |= static_cast<irs_u8>(m_selected_channels);
}

void irs::avr::adc_t::deinit_channels()
{
  DIDR0 &= static_cast<irs_u8>(m_selected_channels);
}

void irs::avr::adc_t::next_channel()
{
  irs_u8 channel = m_cur_channel;
  for(int ch_idx = 0; ch_idx < 8; ch_idx++) {
    channel++;
    if (channel > 7) {
      channel = 0;
    }
    if (channel_exists(channel)) {
      m_cur_channel = channel;
      break;
    }
  }
}

float irs::avr::adc_t::get_data(irs_u8 a_channel)
{
  if(a_channel > 7) {
    return 0;
  } else {
    return static_cast<float>(mp_data[a_channel]);
  }
}

float irs::avr::adc_t::get_data()
{
  return static_cast<float>(mp_data[m_cur_channel]);
}

void irs::avr::adc_t::tick()
{
  if(m_selected_channels) {
    switch(m_status)
    {
      case init_ADC_channel:
      {
        if (channel_exists(m_cur_channel)) {
          if (m_delay_timer.check()) {
            ADMUX = m_cur_channel;
            ADCSRA_ADSC = 1;
            m_sample_timer.start();
            m_status = data_from_channel;
          }
        } else {
          next_channel();
        }
      } break;
      case data_from_channel:
      {
        if ((ADCSRA_ADSC == 0)) {
          if (m_sample_timer.check()) {
            IRS_LOBYTE(mp_data[m_cur_channel]) = ADCL;
            IRS_HIBYTE(mp_data[m_cur_channel]) = ADCH;
            next_channel();
            m_status = init_ADC_channel;
            m_delay_timer.start();
          }
        }
      } break;
    }
  }
}
