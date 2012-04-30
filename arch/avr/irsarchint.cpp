//! \file
//! \ingroup event_processing_group
//! \brief Прерывания AVR
//!
//! Дата: 11.05.2010\n
//! Ранняя дата: 26.02.2009

#include <irsdefs.h>

#include <ioavr.h>

#include <irsarchint.h>
#include <irsconfig.h>

#include <irsfinal.h>

namespace irs {
  
namespace avr {

// Работа с прерываниями в AVR
class interrupt_array_t: public interrupt_array_base_t
{
public:
  typedef interrupt_array_base_t base_type;
  typedef base_type::size_type size_type;
  typedef irs_u8 gen_index_type;

  interrupt_array_t();
  
  virtual irs_int_event_gen_t* int_event_gen(size_type a_index);
  virtual void exec_event(size_type a_index);
private:
  enum {
    interrupt_none = 0
  };

  vector<gen_index_type> m_int_event_gen_indexes;
  vector<irs_int_event_gen_t> m_int_event_gens;
  size_type m_int_event_index;
  
  interrupt_array_t(const interrupt_array_t&);
};

} //namespace avr

} //namespace irs

// Работа с прерываниями в AVR
irs::avr::interrupt_array_t::interrupt_array_t():
  m_int_event_gen_indexes(static_cast<size_t>(interrupt_count), 
    static_cast<gen_index_type>(interrupt_none)),
  m_int_event_gens(1),
  m_int_event_index(interrupt_none)
{
}
irs_int_event_gen_t* 
  irs::avr::interrupt_array_t::int_event_gen(size_type a_index)
{
  gen_index_type& gen_index = m_int_event_gen_indexes[a_index];
  if (gen_index == interrupt_none) {
    // Создаем генератор прерываний для номера прерывания "a_index"
    m_int_event_index++;
    m_int_event_gens.resize(m_int_event_index + 1);
    gen_index = m_int_event_index;
  } else {
    // Генератор прерываний для номера прерывания "a_index" уже существует
  }
  return &m_int_event_gens[gen_index];
}
void irs::avr::interrupt_array_t::exec_event(size_type a_index)
{
  gen_index_type& gen_index = m_int_event_gen_indexes[a_index];
  m_int_event_gens[gen_index].exec();
}

// Возвращает массив прерываний AVR
irs::interrupt_array_base_t* irs::avr::interrupt_array()
{
  static auto_ptr<interrupt_array_t> p_interrupt_array(new interrupt_array_t);
  return p_interrupt_array.get();
}

// Прерывания AVR
#define IRS_INT_EVENT_GEN_DEFINE_FUNC(GEN)\
  __interrupt void irs_avr_##GEN##_func()\
  {\
    irs::avr::interrupt_array()->exec_event(irs::avr::GEN);\
  }
#pragma vector=INT0_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int0_int);
#pragma vector=INT1_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int1_int);
#pragma vector=INT2_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int2_int);
#pragma vector=INT3_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int3_int);
#pragma vector=INT4_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int4_int);
#pragma vector=INT5_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int5_int);
#pragma vector=INT6_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int6_int);
#pragma vector=INT7_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(int7_int);
#pragma vector=TIMER2_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer2_ovf_int);
#pragma vector=TIMER1_CAPT_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer1_capt_int);
#pragma vector=TIMER1_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer1_compa_int);
#pragma vector=TIMER1_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer1_compb_int);
#pragma vector=TIMER1_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer1_compc_int);
// Сделано для звукового ГШ (колнки) Сигнал-1
// define-константа IRS_BOOMBOX определена в настройках проекта
// signal (Boombox)
// См. Options->C/C++ Compiler->Preprocessor->Defined Symbols
#ifndef IRS_BOOMBOX
#pragma vector=TIMER1_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer1_ovf_int);
#endif //IRS_BOOMBOX
#pragma vector=TIMER0_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer0_ovf_int);
#pragma vector=SPI_STC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(spi_stc_int);
#pragma vector=USART0_UDRE_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(usart0_udre_int);
#pragma vector=ADC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(adc_int);
#pragma vector=TIMER3_CAPT_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer3_capt_int);
#pragma vector=TIMER3_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer3_compa_int);
#pragma vector=TIMER3_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer3_compb_int);
#pragma vector=TIMER3_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer3_compc_int);
#pragma vector=TIMER3_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer3_ovf_int);
#pragma vector=USART1_UDRE_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(usart1_udre_int);
#pragma vector=TWI_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(twi_int);

#ifndef __ATmega128__
#pragma vector=PCINT0_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(pcint0_int);
#pragma vector=PCINT1_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(pcint1_int);
#pragma vector=WDT_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(wdt_int);
#pragma vector=TIMER2_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer2_compa_int);
#pragma vector=TIMER2_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer2_compb_int);
#pragma vector=TIMER0_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer0_compa_int);
#pragma vector=TIMER0_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer0_compb_int);
#pragma vector=USART0_RX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(usart0_rx_int);
#pragma vector=USART0_TX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(usart0_tx_int);
#pragma vector=ANALOG_COMP_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(analog_comp_int);
#pragma vector=EE_READY_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(ee_ready_int);
#pragma vector=USART1_RX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(usart1_rx_int);
#pragma vector=USART1_TX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(usart1_tx_int);
#pragma vector=SPM_READY_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(spm_ready_int);
#pragma vector=TIMER4_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer4_compa_int);
#pragma vector=TIMER4_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer4_compb_int);
#pragma vector=TIMER4_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer4_compc_int);
#pragma vector=TIMER4_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer4_ovf_int);
#pragma vector=TIMER5_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer5_compa_int);
#pragma vector=TIMER5_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer5_compb_int);
#pragma vector=TIMER5_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer5_compc_int);
#pragma vector=TIMER5_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer5_ovf_int);
#endif //__ATmega128__
