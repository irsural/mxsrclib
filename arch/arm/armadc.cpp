// Класс для работы с АЦП
// Дата: 14.01.2011
// Дата создания: 13.01.2011

#include <irsdefs.h>

#include <armioregs.h>

#include <armadc.h>

#include <mxdata.h>

#include <irsfinal.h>

irs::arm::adc_t::adc_t(
  counter_t a_adc_timer
):
  m_status(begin_ADC_sampling),
  mp_data(m_channel_max_number),
  m_adc_timer(a_adc_timer) // частота обработки АЦП
{
  // ADC0 Module Initialization
  RCGC0_bit.ADC0 = 1;
  RCGC0_bit.MAXADC0SPD = 0x3;  //  = 1 MSPS
  RCGC2_bit.PORTD = 1;
  for (irs_u8 i = 10; i; i--);
    
  GPIODDEN_bit.no4 = 0;
  GPIODAMSEL_bit.no4 = 1;
  GPIODDEN_bit.no5 = 0;
  GPIODAMSEL_bit.no5 = 1;
  GPIODDEN_bit.no6 = 0;
  GPIODAMSEL_bit.no6 = 1;
  
  // Sample Sequencer Configuration
  // SS disable
  ADC0ACTSS_bit.ASEN0 = 0;
  
  // ADC Event Multiplexer Select
  ADC0EMUX_bit.EM0 = 0x0; // PWM0
  //  ADC Sample Averaging Control
  ADC0SAC_bit.AVG = 0;
  
  ADC0CTL |= INTERNAL_VREF;
  
  ADC0SSMUX0_bit.MUX0 = 0x5;//0x5; // AIN5
  ADC0SSMUX0_bit.MUX1 = 0x6;//0x6; // AIN6
  ADC0SSMUX0_bit.MUX2 = 0x7;//0x7; // AIN7
  ADC0SSMUX0_bit.MUX3 = 0; // Temperature 
  
  ADC0SSCTL0_bit.D0 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END0 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE0 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS0 = 0;  //  Sample Temp Sensor Select
  
  ADC0SSCTL0_bit.D1 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END1 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE1 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS1 = 0;  //  Sample Temp Sensor Select
  
  ADC0SSCTL0_bit.D2 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END2 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE2 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS2 = 0;  //  Sample Temp Sensor Select
  
  ADC0SSCTL0_bit.D3 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END3 = 1; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE3 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS3 = 1;  //  Sample Temp Sensor Select
    
  // SS enable
  ADC0ACTSS_bit.ASEN0 = 1;
  ADC0ISC_bit.IN0 = 1;
  
  m_adc_timer.start();
}

irs::arm::adc_t::~adc_t()
{
}

float irs::arm::adc_t::get_data(irs_u8 a_channel)
{
  if (a_channel >= m_channel_max_number) {
    return 0;
  } else if (a_channel == m_internal_temp_idx) {
    return get_temperature();
  } else {
    return static_cast<float>((mp_data[a_channel]*3.f)/1023.f);
  }
}

float irs::arm::adc_t::get_temperature()
{
  return static_cast<float>(
    147.5f - ((225.f*mp_data[m_internal_temp_idx])/1023.f)
  );
}

void irs::arm::adc_t::tick()
{
  switch(m_status)
  {
    case begin_ADC_sampling:
    {
      if (m_adc_timer.check()) {
        ADC0PSSI_bit.SS0 = 1;
        m_status = data_from_channel;
        m_adc_timer.start();
      }
    } break;
    case data_from_channel:
    {
      if (!ADC0SSFSTAT0_bit.EMPTY) {
        for (irs_u8 cnt = 0; cnt < m_channel_max_number; cnt++) {
          mp_data[cnt] = ADC0SSFIFO0;
        }
        if (!ADC0SSFSTAT0_bit.EMPTY) {
          volatile irs_u32 fifo_data = ADC0SSFIFO0;
          while(!ADC0SSFSTAT0_bit.EMPTY) {
            fifo_data = ADC0SSFIFO0;
          }
        }
        ADC0ISC_bit.IN0 = 1;
        m_status = begin_ADC_sampling;
      }
    } break;
  }
}
