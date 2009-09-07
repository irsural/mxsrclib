// Прерывания AVR
// Дата: 26.02.2009

#ifndef irsarchintH
#define irsarchintH

#define IRSARCHINT_NEW

#include <irsdefs.h>
#include <irsint.h>
#include <irscpp.h>

// Внешний интерфейс для прерывания AVR
extern irs_int_event_gen_t& irs_avr_int0_int;
extern irs_int_event_gen_t& irs_avr_int1_int;
extern irs_int_event_gen_t& irs_avr_int2_int;
extern irs_int_event_gen_t& irs_avr_int3_int;
extern irs_int_event_gen_t& irs_avr_int4_int;
extern irs_int_event_gen_t& irs_avr_int5_int;
extern irs_int_event_gen_t& irs_avr_int6_int;
extern irs_int_event_gen_t& irs_avr_int7_int;
extern irs_int_event_gen_t& irs_avr_timer2_ovf_int;
extern irs_int_event_gen_t& irs_avr_timer1_capt_int;
extern irs_int_event_gen_t& irs_avr_timer1_compa_int;
extern irs_int_event_gen_t& irs_avr_timer1_compb_int;
extern irs_int_event_gen_t& irs_avr_timer1_compc_int;
extern irs_int_event_gen_t& irs_avr_timer1_ovf_int;
extern irs_int_event_gen_t& irs_avr_timer0_ovf_int;
extern irs_int_event_gen_t& irs_avr_spi_stc_int;
// Неправильный имя прерывания созданного ранее
extern irs_int_event_gen_t& irs_avr_spi_int;
extern irs_int_event_gen_t& irs_avr_usart0_udre_int;
extern irs_int_event_gen_t& irs_avr_adc_int;
extern irs_int_event_gen_t& irs_avr_timer3_capt_int;
extern irs_int_event_gen_t& irs_avr_timer3_compa_int;
extern irs_int_event_gen_t& irs_avr_timer3_compb_int;
extern irs_int_event_gen_t& irs_avr_timer3_compc_int;
extern irs_int_event_gen_t& irs_avr_timer3_ovf_int;
extern irs_int_event_gen_t& irs_avr_usart1_udre_int;
extern irs_int_event_gen_t& irs_avr_twi_int;

#ifndef __ATmega128__
extern irs_int_event_gen_t& irs_avr_pcint0_int;
extern irs_int_event_gen_t& irs_avr_pcint1_int;
extern irs_int_event_gen_t& irs_avr_wdt_int;
extern irs_int_event_gen_t& irs_avr_timer2_compa_int;
extern irs_int_event_gen_t& irs_avr_timer2_compb_int;
extern irs_int_event_gen_t& irs_avr_timer0_compa_int;
extern irs_int_event_gen_t& irs_avr_timer0_compb_int;
extern irs_int_event_gen_t& irs_avr_usart0_rx_int;
extern irs_int_event_gen_t& irs_avr_usart0_tx_int;
extern irs_int_event_gen_t& irs_avr_analog_comp_int;
extern irs_int_event_gen_t& irs_avr_ee_ready_int;
extern irs_int_event_gen_t& irs_avr_usart1_rx_int;
extern irs_int_event_gen_t& irs_avr_usart1_tx_int;
extern irs_int_event_gen_t& irs_avr_spm_ready_int;
extern irs_int_event_gen_t& irs_avr_timer4_compa_int;
extern irs_int_event_gen_t& irs_avr_timer4_compb_int;
extern irs_int_event_gen_t& irs_avr_timer4_compc_int;
extern irs_int_event_gen_t& irs_avr_timer4_ovf_int;
extern irs_int_event_gen_t& irs_avr_timer5_compa_int;
extern irs_int_event_gen_t& irs_avr_timer5_compb_int;
extern irs_int_event_gen_t& irs_avr_timer5_compc_int;
extern irs_int_event_gen_t& irs_avr_timer5_ovf_int;
#endif //__ATmega128__

namespace irs {

// Инициализация прерываний AVR
// Для правильной инициализации независимо от порядка инициализации модулей
class avr_int_event_gen_init_t
{
public:
  typedef irs_u8 count_type;
  
  avr_int_event_gen_init_t();
  ~avr_int_event_gen_init_t();
private:
  static count_type m_count;
  #ifndef IRSARCHINT_NEW
  static vector<irs_int_event_gen_t*> m_itps;
  
  static void itp_add(irs_int_event_gen_t* a_int_gen);
  #endif //IRSARCHINT_NEW
  static void itp_init(irs_int_event_gen_t* a_int_gen);
  static void itp_deinit(irs_int_event_gen_t* a_int_gen);
};
static avr_int_event_gen_init_t avr_int_event_gen_init;

// Динамическая установка прерываний
#ifdef NOP
typedef irs_u8 int_vect_t;
typedef void (__interrupt *int_func_t)();
void set_int_vect(int_vect_t int_vect, int_func_t int_func);
#endif //NOP

} //namespace irs

#endif //irsarchintH
