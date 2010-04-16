// Прерывания AVR
// Дата: 17.03.2010
// Ранняя дата: 26.02.2009

#include <irsdefs.h>

#include <ioavr.h>

#include <irsarchint.h>
#include <irsconfig.h>

#include <irsfinal.h>

#ifdef IRSARCHINT_NEW_20100317

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
#pragma vector=TIMER1_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(timer1_ovf_int);
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

#endif //IRSARCHINT_NEW_20100317


//**************************************************************************
//**************************************************************************
//**************************************************************************


#ifndef IRSARCHINT_NEW_20100317

// Внешний интерфейс для прерывания AVR
#define IRS_INT_EVENT_GEN_DEFINE(INT_GEN)\
  irs_u8 INT_GEN##_mem[sizeof(irs_int_event_gen_t)];\
  irs_int_event_gen_t& INT_GEN = \
    reinterpret_cast<irs_int_event_gen_t&>(*INT_GEN##_mem);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int0_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int1_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int2_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int3_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int4_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int5_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int6_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_int7_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer2_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_capt_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer0_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_spi_stc_int);
// Неправильное имя прерывания созданного ранее
irs_int_event_gen_t& irs_avr_spi_int = irs_avr_spi_stc_int;
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart0_udre_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_adc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_capt_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart1_udre_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_twi_int);

#ifndef __ATmega128__
IRS_INT_EVENT_GEN_DEFINE(irs_avr_pcint0_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_pcint1_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_wdt_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer2_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer2_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer0_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer0_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart0_rx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart0_tx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_analog_comp_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_ee_ready_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart1_rx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart1_tx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_spm_ready_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_ovf_int);
#endif //__ATmega128__

// Инициализация прерываний AVR
// Для правильной инициализации независимо от порядка инициализации модулей
irs::avr_int_event_gen_init_t::count_type
  irs::avr_int_event_gen_init_t::m_count = 0;
#ifndef IRSARCHINT_NEW
vector<irs_int_event_gen_t*> irs::avr_int_event_gen_init_t::m_itps;
#endif //IRSARCHINT_NEW
irs::avr_int_event_gen_init_t::
  avr_int_event_gen_init_t()
{
  m_count++;
  if (m_count == 1) {
    #ifdef IRSARCHINT_NEW
    itp_init(&irs_avr_int0_int);
    itp_init(&irs_avr_int1_int);
    itp_init(&irs_avr_int2_int);
    itp_init(&irs_avr_int3_int);
    itp_init(&irs_avr_int4_int);
    itp_init(&irs_avr_int5_int);
    itp_init(&irs_avr_int6_int);
    itp_init(&irs_avr_int7_int);
    itp_init(&irs_avr_timer2_ovf_int);
    itp_init(&irs_avr_timer1_capt_int);
    itp_init(&irs_avr_timer1_compa_int);
    itp_init(&irs_avr_timer1_compb_int);
    itp_init(&irs_avr_timer1_compc_int);
    itp_init(&irs_avr_timer1_ovf_int);
    itp_init(&irs_avr_timer0_ovf_int);
    itp_init(&irs_avr_spi_stc_int);
    itp_init(&irs_avr_usart0_udre_int);
    itp_init(&irs_avr_adc_int);
    itp_init(&irs_avr_timer3_capt_int);
    itp_init(&irs_avr_timer3_compa_int);
    itp_init(&irs_avr_timer3_compb_int);
    itp_init(&irs_avr_timer3_compc_int);
    itp_init(&irs_avr_timer3_ovf_int);
    itp_init(&irs_avr_usart1_udre_int);
    itp_init(&irs_avr_twi_int);

    #ifndef __ATmega128__
    itp_init(&irs_avr_pcint0_int);
    itp_init(&irs_avr_pcint1_int);
    itp_init(&irs_avr_wdt_int);
    itp_init(&irs_avr_timer2_compa_int);
    itp_init(&irs_avr_timer2_compb_int);
    itp_init(&irs_avr_timer0_compa_int);
    itp_init(&irs_avr_timer0_compb_int);
    itp_init(&irs_avr_usart0_rx_int);
    itp_init(&irs_avr_usart0_tx_int);
    itp_init(&irs_avr_analog_comp_int);
    itp_init(&irs_avr_ee_ready_int);
    itp_init(&irs_avr_usart1_rx_int);
    itp_init(&irs_avr_usart1_tx_int);
    itp_init(&irs_avr_spm_ready_int);
    itp_init(&irs_avr_timer4_compa_int);
    itp_init(&irs_avr_timer4_compb_int);
    itp_init(&irs_avr_timer4_compc_int);
    itp_init(&irs_avr_timer4_ovf_int);
    itp_init(&irs_avr_timer5_compa_int);
    itp_init(&irs_avr_timer5_compb_int);
    itp_init(&irs_avr_timer5_compc_int);
    itp_init(&irs_avr_timer5_ovf_int);
    #endif //__ATmega128__

    #else //IRSARCHINT_NEW
    itp_add(&irs_avr_int0_int);
    itp_add(&irs_avr_int1_int);
    itp_add(&irs_avr_int2_int);
    itp_add(&irs_avr_int3_int);
    itp_add(&irs_avr_int4_int);
    itp_add(&irs_avr_int5_int);
    itp_add(&irs_avr_int6_int);
    itp_add(&irs_avr_int7_int);
    itp_add(&irs_avr_timer2_ovf_int);
    itp_add(&irs_avr_timer1_capt_int);
    itp_add(&irs_avr_timer1_compa_int);
    itp_add(&irs_avr_timer1_compb_int);
    itp_add(&irs_avr_timer1_compc_int);
    itp_add(&irs_avr_timer1_ovf_int);
    itp_add(&irs_avr_timer0_ovf_int);
    itp_add(&irs_avr_spi_stc_int);
    itp_add(&irs_avr_usart0_udre_int);
    itp_add(&irs_avr_adc_int);
    itp_add(&irs_avr_timer3_capt_int);
    itp_add(&irs_avr_timer3_compa_int);
    itp_add(&irs_avr_timer3_compb_int);
    itp_add(&irs_avr_timer3_compc_int);
    itp_add(&irs_avr_timer3_ovf_int);
    itp_add(&irs_avr_usart1_udre_int);
    itp_add(&irs_avr_twi_int);

    #ifndef __ATmega128__
    itp_add(&irs_avr_pcint0_int);
    itp_add(&irs_avr_pcint1_int);
    itp_add(&irs_avr_wdt_int);
    itp_add(&irs_avr_timer2_compa_int);
    itp_add(&irs_avr_timer2_compb_int);
    itp_add(&irs_avr_timer0_compa_int);
    itp_add(&irs_avr_timer0_compb_int);
    itp_add(&irs_avr_usart0_rx_int);
    itp_add(&irs_avr_usart0_tx_int);
    itp_add(&irs_avr_analog_comp_int);
    itp_add(&irs_avr_ee_ready_int);
    itp_add(&irs_avr_usart1_rx_int);
    itp_add(&irs_avr_usart1_tx_int);
    itp_add(&irs_avr_spm_ready_int);
    itp_add(&irs_avr_timer4_compa_int);
    itp_add(&irs_avr_timer4_compb_int);
    itp_add(&irs_avr_timer4_compc_int);
    itp_add(&irs_avr_timer4_ovf_int);
    itp_add(&irs_avr_timer5_compa_int);
    itp_add(&irs_avr_timer5_compb_int);
    itp_add(&irs_avr_timer5_compc_int);
    itp_add(&irs_avr_timer5_ovf_int);
    #endif //__ATmega128__

    for_each(m_itps.begin(), m_itps.end(), itp_init);
    #endif //IRSARCHINT_NEW
  }
}

irs::avr_int_event_gen_init_t::
  ~avr_int_event_gen_init_t()
{
  m_count--;
  if (m_count == 0) {
    #ifdef IRSARCHINT_NEW
    itp_deinit(&irs_avr_twi_int);
    itp_deinit(&irs_avr_usart1_udre_int);
    itp_deinit(&irs_avr_timer3_ovf_int);
    itp_deinit(&irs_avr_timer3_compc_int);
    itp_deinit(&irs_avr_timer3_compb_int);
    itp_deinit(&irs_avr_timer3_compa_int);
    itp_deinit(&irs_avr_timer3_capt_int);
    itp_deinit(&irs_avr_adc_int);
    itp_deinit(&irs_avr_usart0_udre_int);
    itp_deinit(&irs_avr_spi_stc_int);
    itp_deinit(&irs_avr_timer0_ovf_int);
    itp_deinit(&irs_avr_timer1_ovf_int);
    itp_deinit(&irs_avr_timer1_compc_int);
    itp_deinit(&irs_avr_timer1_compb_int);
    itp_deinit(&irs_avr_timer1_compa_int);
    itp_deinit(&irs_avr_timer1_capt_int);
    itp_deinit(&irs_avr_timer2_ovf_int);
    itp_deinit(&irs_avr_int7_int);
    itp_deinit(&irs_avr_int6_int);
    itp_deinit(&irs_avr_int5_int);
    itp_deinit(&irs_avr_int4_int);
    itp_deinit(&irs_avr_int3_int);
    itp_deinit(&irs_avr_int2_int);
    itp_deinit(&irs_avr_int1_int);
    itp_deinit(&irs_avr_int0_int);

    #ifndef __ATmega128__
    itp_deinit(&irs_avr_timer5_ovf_int);
    itp_deinit(&irs_avr_timer5_compc_int);
    itp_deinit(&irs_avr_timer5_compb_int);
    itp_deinit(&irs_avr_timer5_compa_int);
    itp_deinit(&irs_avr_timer4_ovf_int);
    itp_deinit(&irs_avr_timer4_compc_int);
    itp_deinit(&irs_avr_timer4_compb_int);
    itp_deinit(&irs_avr_timer4_compa_int);
    itp_deinit(&irs_avr_spm_ready_int);
    itp_deinit(&irs_avr_usart1_tx_int);
    itp_deinit(&irs_avr_usart1_rx_int);
    itp_deinit(&irs_avr_ee_ready_int);
    itp_deinit(&irs_avr_analog_comp_int);
    itp_deinit(&irs_avr_usart0_tx_int);
    itp_deinit(&irs_avr_usart0_rx_int);
    itp_deinit(&irs_avr_timer0_compb_int);
    itp_deinit(&irs_avr_timer0_compa_int);
    itp_deinit(&irs_avr_timer2_compb_int);
    itp_deinit(&irs_avr_timer2_compa_int);
    itp_deinit(&irs_avr_wdt_int);
    itp_deinit(&irs_avr_pcint1_int);
    itp_deinit(&irs_avr_pcint0_int);
    #endif //__ATmega128__

    #else //IRSARCHINT_NEW
    for_each(m_itps.rbegin(), m_itps.rend(), itp_deinit);
    #endif //IRSARCHINT_NEW
  }
}
#ifndef IRSARCHINT_NEW
void irs::avr_int_event_gen_init_t::
  itp_add(irs_int_event_gen_t* a_int_gen)
{
  m_itps.push_back(a_int_gen);
}
#endif //IRSARCHINT_NEW
void irs::avr_int_event_gen_init_t::
  itp_init(irs_int_event_gen_t* a_int_gen)
{
  new (a_int_gen) irs_int_event_gen_t();
}
void irs::avr_int_event_gen_init_t::
  itp_deinit(irs_int_event_gen_t* a_int_gen)
{
  a_int_gen->~irs_int_event_gen_t();
}

// Прерывания AVR
#define IRS_INT_EVENT_GEN_DEFINE_FUNC(GEN)\
  __interrupt void GEN##_func()\
  {\
    GEN.exec();\
  }
#pragma vector=INT0_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int0_int);
#pragma vector=INT1_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int1_int);
#pragma vector=INT2_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int2_int);
#pragma vector=INT3_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int3_int);
#pragma vector=INT4_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int4_int);
#pragma vector=INT5_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int5_int);
#pragma vector=INT6_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int6_int);
#pragma vector=INT7_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_int7_int);
#pragma vector=TIMER2_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer2_ovf_int);
#pragma vector=TIMER1_CAPT_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer1_capt_int);
#pragma vector=TIMER1_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer1_compa_int);
#pragma vector=TIMER1_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer1_compb_int);
#pragma vector=TIMER1_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer1_compc_int);
#pragma vector=TIMER1_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer1_ovf_int);
#pragma vector=TIMER0_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer0_ovf_int);
#pragma vector=SPI_STC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_spi_stc_int);
#pragma vector=USART0_UDRE_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart0_udre_int);
#pragma vector=ADC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_adc_int);
#pragma vector=TIMER3_CAPT_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer3_capt_int);
#pragma vector=TIMER3_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer3_compa_int);
#pragma vector=TIMER3_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer3_compb_int);
#pragma vector=TIMER3_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer3_compc_int);
#pragma vector=TIMER3_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer3_ovf_int);
#pragma vector=USART1_UDRE_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart1_udre_int);
#pragma vector=TWI_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_twi_int);

#ifndef __ATmega128__
#pragma vector=PCINT0_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_pcint0_int);
#pragma vector=PCINT1_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_pcint1_int);
#pragma vector=WDT_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_wdt_int);
#pragma vector=TIMER2_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer2_compa_int);
#pragma vector=TIMER2_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer2_compb_int);
#pragma vector=TIMER0_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer0_compa_int);
#pragma vector=TIMER0_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer0_compb_int);
#pragma vector=USART0_RX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart0_rx_int);
#pragma vector=USART0_TX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart0_tx_int);
#pragma vector=ANALOG_COMP_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_analog_comp_int);
#pragma vector=EE_READY_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_ee_ready_int);
#pragma vector=USART1_RX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart1_rx_int);
#pragma vector=USART1_TX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart1_tx_int);
#pragma vector=SPM_READY_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_spm_ready_int);
#pragma vector=TIMER4_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer4_compa_int);
#pragma vector=TIMER4_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer4_compb_int);
#pragma vector=TIMER4_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer4_compc_int);
#pragma vector=TIMER4_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer4_ovf_int);
#pragma vector=TIMER5_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer5_compa_int);
#pragma vector=TIMER5_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer5_compb_int);
#pragma vector=TIMER5_COMPC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer5_compc_int);
#pragma vector=TIMER5_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer5_ovf_int);
#endif //__ATmega128__

// Динамическая установка прерываний
#ifdef NOP
void irs::set_int_vect(int_vect_t int_vect, int_func_t int_func)
{
  struct int_vect_struct_t
  {
    irs_u16 instruction;
    int_func_t address;
  };
  irs_u16 int_vect_u16 = int_vect;
  int_vect_struct_t& int_vect_struct =
    *reinterpret_cast<int_vect_struct_t*>(int_vect_u16);
  int_vect_struct.instruction = 0x0C94;
  int_vect_struct.address = int_func;
}
#endif //NOP

#endif //IRSARCHINT_NEW_20100317
