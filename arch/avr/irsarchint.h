// Прерывания AVR
// Дата: 11.05.2010
// Ранняя дата: 26.02.2009

#ifndef irsarchintH
#define irsarchintH

#include <irsdefs.h>

#include <irsint.h>
#include <irscpp.h>

#include <irsfinal.h>

namespace irs {
  
namespace avr {

// Номера прерываний AVR
enum {
  // AVR128 и AVR256
  int0_int,
  int1_int,
  int2_int,
  int3_int,
  int4_int,
  int5_int,
  int6_int,
  int7_int,
  timer2_ovf_int,
  timer1_capt_int,
  timer1_compa_int,
  timer1_compb_int,
  timer1_compc_int,
  timer1_ovf_int,
  timer0_ovf_int,
  spi_stc_int,
  usart0_udre_int,
  adc_int,
  timer3_capt_int,
  timer3_compa_int,
  timer3_compb_int,
  timer3_compc_int,
  timer3_ovf_int,
  usart1_udre_int,
  twi_int,
  
  // AVR256
  #ifndef __ATmega128__
  pcint0_int,
  pcint1_int,
  wdt_int,
  timer2_compa_int,
  timer2_compb_int,
  timer0_compa_int,
  timer0_compb_int,
  usart0_rx_int,
  usart0_tx_int,
  analog_comp_int,
  ee_ready_int,
  usart1_rx_int,
  usart1_tx_int,
  spm_ready_int,
  timer4_compa_int,
  timer4_compb_int,
  timer4_compc_int,
  timer4_ovf_int,
  timer5_compa_int,
  timer5_compb_int,
  timer5_compc_int,
  timer5_ovf_int,
  #endif //__ATmega128__
  
  interrupt_count
};

// Возвращает массив прерываний AVR
interrupt_array_base_t* interrupt_array();

} //namespace avr

} //namespace irs

// Старый интерфейс для прерываний AVR
#define irs_avr_int0_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int0_int))
#define irs_avr_int1_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int1_int))
#define irs_avr_int2_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int2_int))
#define irs_avr_int3_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int3_int))
#define irs_avr_int4_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int4_int))
#define irs_avr_int5_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int5_int))
#define irs_avr_int6_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int6_int))
#define irs_avr_int7_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::int7_int))
#define irs_avr_timer2_ovf_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer2_ovf_int))
#define irs_avr_timer1_capt_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer1_capt_int))
#define irs_avr_timer1_compa_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer1_compa_int))
#define irs_avr_timer1_compb_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer1_compb_int))
#define irs_avr_timer1_compc_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer1_compc_int))
#define irs_avr_timer1_ovf_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer1_ovf_int))
#define irs_avr_timer0_ovf_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer0_int0_int))
#define irs_avr_spi_stc_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::spi_stc_int))
// Неправильное имя прерывания созданного ранее
#define irs_avr_spi_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::spi_stc_int))
#define irs_avr_usart0_udre_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::usart0_udre_int))
#define irs_avr_adc_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::adc_int))
#define irs_avr_timer3_capt_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer3_capt_int))
#define irs_avr_timer3_compa_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer3_compa_int))
#define irs_avr_timer3_compb_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer3_compb_int))
#define irs_avr_timer3_compc_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer3_compc_int))
#define irs_avr_timer3_ovf_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer3_ovf_int))
#define irs_avr_usart1_udre_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::usart1_udre_int))
#define irs_avr_twi_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::twi_int))

#ifndef __ATmega128__
#define irs_avr_pcint0_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::pcint0_int))
#define irs_avr_pcint1_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::pcint1_int))
#define irs_avr_wdt_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::wdt_int))
#define irs_avr_timer2_compa_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer2_compa_int))
#define irs_avr_timer2_compb_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer2_compb_int))
#define irs_avr_timer0_compa_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer0_compa_int))
#define irs_avr_timer0_compb_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer0_compb_int))
#define irs_avr_usart0_rx_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::usart0_rx_int))
#define irs_avr_usart0_tx_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::usart0_tx_int))
#define irs_avr_analog_comp_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::analog_comp_int))
#define irs_avr_ee_ready_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::ee_ready_int))
#define irs_avr_usart1_rx_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::usart1_rx_int))
#define irs_avr_usart1_tx_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::usart1_tx_int))
#define irs_avr_spm_ready_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::spm_ready_int))
#define irs_avr_timer4_compa_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer4_compa_int))
#define irs_avr_timer4_compb_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer4_compb_int))
#define irs_avr_timer4_compc_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer4_compc_int))
#define irs_avr_timer4_ovf_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer4_ovf_int))
#define irs_avr_timer5_compa_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer5_compa_int))
#define irs_avr_timer5_compb_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer5_compb_int))
#define irs_avr_timer5_compc_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer5_compc_int))
#define irs_avr_timer5_ovf_int\
  (*irs::avr::interrupt_array()->int_event_gen(irs::avr::timer5_ovf_int))
#endif //__ATmega128__

#endif //irsarchintH
