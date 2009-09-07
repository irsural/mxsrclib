// Стандартаная библиотека ИРС локальная часть
// Для AVR
// Дата: 17.08.2009

//#include <iom2561.h>

#include <irsstdl.h>

#ifdef NOP
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
IRS_INT_EVENT_GEN_DEFINE(irs_avr_pcint0_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_pcint1_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_wdt_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer2_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer2_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer2_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_capt_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer1_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer0_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer0_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer0_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_spi_stc_int);
// Неправильный имя прерывания созданного ранее
irs_int_event_gen_t& irs_avr_spi_int = irs_avr_spi_stc_int;
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart0_rx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart0_udre_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart0_tx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_analog_comp_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_adc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_ee_ready_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_capt_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer3_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart1_rx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart1_udre_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_usart1_tx_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_twi_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_spm_ready_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer4_ovf_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_compa_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_compb_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_compc_int);
IRS_INT_EVENT_GEN_DEFINE(irs_avr_timer5_ovf_int);
irs_u8 irs_avr_timer1_compa_int_mem[sizeof(irs_int_event_gen_t)];
irs_int_event_gen_t& irs_avr_timer1_compa_int =
  reinterpret_cast<irs_int_event_gen_t&>(*irs_avr_timer1_compa_int_mem);
irs_int_event_gen_t irs_avr_timer1_compa_int;
irs_int_event_gen_t irs_avr_timer1_ovf_int;
irs_int_event_gen_t irs_avr_timer3_compa_int;
irs_int_event_gen_t irs_avr_timer3_ovf_int;
irs_int_event_gen_t irs_avr_spi_int;
irs_int_event_gen_t irs_avr_pcint0_int;
irs_int_event_gen_t irs_avr_analog_comp_int;

irs::avr_int_event_gen_init_t::count_type
  irs::avr_int_event_gen_init_t::m_count = 0;
vector<irs_int_event_gen_t*> irs::avr_int_event_gen_init_t::m_itps;
irs::avr_int_event_gen_init_t::
  avr_int_event_gen_init_t()
{
  if (m_count != 0) {
    m_count++;
    //new (&irs_avr_timer1_compa_int) irs_int_event_gen_t();
    itp_add(&irs_avr_int0_int);
    itp_add(&irs_avr_int1_int);
    itp_add(&irs_avr_int2_int);
    itp_add(&irs_avr_int3_int);
    itp_add(&irs_avr_int4_int);
    itp_add(&irs_avr_int5_int);
    itp_add(&irs_avr_int6_int);
    itp_add(&irs_avr_int7_int);
    itp_add(&irs_avr_pcint0_int);
    itp_add(&irs_avr_pcint1_int);
    itp_add(&irs_avr_wdt_int);
    itp_add(&irs_avr_timer2_compa_int);
    itp_add(&irs_avr_timer2_compb_int);
    itp_add(&irs_avr_timer2_ovf_int);
    itp_add(&irs_avr_timer1_capt_int);
    itp_add(&irs_avr_timer1_compa_int);
    itp_add(&irs_avr_timer1_compb_int);
    itp_add(&irs_avr_timer1_compc_int);
    itp_add(&irs_avr_timer1_ovf_int);
    itp_add(&irs_avr_timer0_compa_int);
    itp_add(&irs_avr_timer0_compb_int);
    itp_add(&irs_avr_timer0_ovf_int);
    itp_add(&irs_avr_spi_stc_int);
    itp_add(&irs_avr_usart0_rx_int);
    itp_add(&irs_avr_usart0_udre_int);
    itp_add(&irs_avr_usart0_tx_int);
    itp_add(&irs_avr_analog_comp_int);
    itp_add(&irs_avr_adc_int);
    itp_add(&irs_avr_ee_ready_int);
    itp_add(&irs_avr_timer3_capt_int);
    itp_add(&irs_avr_timer3_compa_int);
    itp_add(&irs_avr_timer3_compb_int);
    itp_add(&irs_avr_timer3_compc_int);
    itp_add(&irs_avr_timer3_ovf_int);
    itp_add(&irs_avr_usart1_rx_int);
    itp_add(&irs_avr_usart1_udre_int);
    itp_add(&irs_avr_usart1_tx_int);
    itp_add(&irs_avr_twi_int);
    itp_add(&irs_avr_spm_ready_int);
    itp_add(&irs_avr_timer4_compa_int);
    itp_add(&irs_avr_timer4_compb_int);
    itp_add(&irs_avr_timer4_compc_int);
    itp_add(&irs_avr_timer4_ovf_int);
    itp_add(&irs_avr_timer5_compa_int);
    itp_add(&irs_avr_timer5_compb_int);
    itp_add(&irs_avr_timer5_compc_int);
    itp_add(&irs_avr_timer5_ovf_int);

    for_each(m_itps.begin(), m_itps.end(), itp_init);
    //...
  }
}
irs::avr_int_event_gen_init_t::
  ~avr_int_event_gen_init_t()
{
  m_count--;
  if (m_count == 0) {
    //irs_avr_timer1_compa_int.~irs_int_event_gen_t();

    for_each(m_itps.rbegin(), m_itps.rend(), itp_deinit);
    //...
  }
}
void irs::avr_int_event_gen_init_t::
  itp_add(irs_int_event_gen_t* a_int_gen)
{
  m_itps.push_back(a_int_gen);
}
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
  //#pragma vector=VECT
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
#pragma vector=TIMER0_COMPA_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer0_compa_int);
#pragma vector=TIMER0_COMPB_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer0_compb_int);
#pragma vector=TIMER0_OVF_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_timer0_ovf_int);
#pragma vector=SPI_STC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_spi_stc_int);
#pragma vector=USART0_RX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart0_rx_int);
#pragma vector=USART0_UDRE_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart0_udre_int);
#pragma vector=USART0_TX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart0_tx_int);
#pragma vector=ANALOG_COMP_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_analog_comp_int);
#pragma vector=ADC_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_adc_int);
#pragma vector=EE_READY_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_ee_ready_int);
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
#pragma vector=USART1_RX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart1_rx_int);
#pragma vector=USART1_UDRE_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart1_udre_int);
#pragma vector=USART1_TX_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_usart1_tx_int);
#pragma vector=TWI_vect
IRS_INT_EVENT_GEN_DEFINE_FUNC(irs_avr_twi_int);
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


#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA()
{
  irs_avr_timer1_compa_int.exec();
}

#pragma vector=TIMER1_OVF_vect
__interrupt void TIMER1_OVF()
{
  irs_avr_timer1_ovf_int.exec();
}

#pragma vector=TIMER3_COMPA_vect
__interrupt void TIMER3_COMPA()
{
  irs_avr_timer3_compa_int.exec();
}

#pragma vector=TIMER3_OVF_vect
__interrupt void TIMER3_OVF()
{
  irs_avr_timer3_ovf_int.exec();
}

//  SPI Serial Transfer Complete
#pragma vector=SPI_STC_vect
__interrupt void SPI_STC()
{
  irs_avr_spi_int.exec();
}

//  Pin Change Interrupt Request 0
#pragma vector=PCINT0_vect
__interrupt void PCINT0_INT()
{
  irs_avr_pcint0_int.exec();
}

//  Analog Comparator Interrupt Request
#pragma vector=ANALOG_COMP_vect
__interrupt void ANALOG_COMP()
{
  irs_avr_analog_comp_int.exec();
}
#endif //NOP

// Карта преобразования номеров клавиш в коды
// для вертикального расположения клавиатуры
__flash irskey_t irskbd_table_vertical[] =
{
  irskey_none,      // 0
  irskey_0,         // 1
  irskey_1,         // 2
  irskey_2,         // 3
  irskey_3,         // 4
  irskey_4,         // 5
  irskey_5,         // 6
  irskey_6,         // 7
  irskey_7,         // 8
  irskey_8,         // 9
  irskey_9,         // 10
  irskey_point,     // 11
  irskey_enter,     // 12
  irskey_up,        // 13
  irskey_down,      // 14
  irskey_backspace, // 15
  irskey_escape     // 16
};

// Карта преобразования номеров клавиш в коды
// для горизонтального расположения клавиатуры
__flash irskey_t irskbd_table_horisontal[] =
{
  irskey_none,      // 0
  irskey_enter,     // 1
  irskey_backspace, // 2
  irskey_3,         // 3
  irskey_6,         // 4
  irskey_point,     // 5
  irskey_2,         // 6
  irskey_5,         // 7
  irskey_0,         // 8
  irskey_1,         // 9
  irskey_4,         // 10
  irskey_down,      // 11
  irskey_escape,    // 12
  irskey_8,         // 13
  irskey_9,         // 14
  irskey_up,        // 15
  irskey_7          // 16
};

// Карта преобразования номеров клавиш в коды
// для расположения клавиатуры для ГТЧ rev 0 (макетка edition)
__flash irskey_t irskbd_table_gtch_rev_0[] =
{
  irskey_none,      // 0
  irskey_none,      // 1
  irskey_none,      // 2
  irskey_escape,    // 3
  irskey_6,         // 4
  irskey_none,      // 5
  irskey_2,         // 6
  irskey_5,         // 7
  irskey_none,      // 8
  irskey_1,         // 9
  irskey_4,         // 10
  irskey_enter,     // 11
  irskey_up,        // 12
  irskey_8,         // 13
  irskey_9,         // 14
  irskey_down,      // 15
  irskey_7          // 16
};

// Карта преобразования номеров клавиш в коды
// для расположения клавиатуры для ГТЧ rev 1 (чемодан edition)
__flash irskey_t irskbd_table_gtch_rev_1[] =
{
  irskey_none,      // 0
  irskey_none,      // 1
  irskey_none,      // 2
  irskey_2,         // 3
  irskey_5,         // 4
  irskey_none,      // 5
  irskey_escape,    // 6
  irskey_6,         // 7
  irskey_none,      // 8
  irskey_enter,     // 9
  irskey_down,      // 10
  irskey_1,         // 11
  irskey_7,         // 12
  irskey_9,         // 13
  irskey_8,         // 14
  irskey_4,         // 15
  irskey_up         // 16
};

const irs_avr_port_map_t avr_port_map[] = {
  {&PORTA, &PINA, &DDRA},
  {&PORTB, &PINB, &DDRB},
  {&PORTC, &PINC, &DDRC},
  {&PORTD, &PIND, &DDRD},
  {&PORTE, &PINE, &DDRE},
  {&PORTF, &PINF, &DDRF},
  {&PORTG, &PING, &DDRG}
};
//#define avr_port_map_size IRS_ARRAYOFSIZE(avr_port_map);

//__flash irs_u8 lcd_table[] =

mxkey_drv_avr_t::mxkey_drv_avr_t(irskbd_t type, irskbd_map_t map_type,
  irs_avr_port_t a_port):
  m_high_set_bit(7),
  m_high_get_bit(3),
  mp_set(avr_port_map[a_port].set),
  mp_get(avr_port_map[a_port].get),
  mp_dir(avr_port_map[a_port].dir)
{
  switch (type) {
    case irskbd_high_pull_up: {
      m_high_set_bit = 3;
      m_high_get_bit = 7;
      (*mp_dir) = 0x0F;
    } break;

    default:
    case irskbd_low_pull_up: {
      m_high_set_bit = 7;
      m_high_get_bit = 3;
      (*mp_dir) = 0xF0;
    } break;
  }
  switch(map_type) {
    default:
    case irskbd_map_vertical: {
      mp_key_map = irskbd_table_vertical;
    } break;

    case irskbd_map_horisontal: {
      mp_key_map = irskbd_table_horisontal;
    } break;

    case irskbd_map_gtch_rev_1: {
      mp_key_map = irskbd_table_gtch_rev_1;
    } break;
  }
  (*mp_set) = (*mp_dir)^0xFF;
}
mxkey_drv_avr_t::~mxkey_drv_avr_t()
{
  (*mp_dir) = 0x00;
  (*mp_set) = 0x00;
}
irskey_t mxkey_drv_avr_t::operator()()
{
  reshor(0);
  sethor(1);
  sethor(2);
  sethor(3);
  if (!chkver(0)) return mp_key_map[8];
  if (!chkver(1)) return mp_key_map[9];
  if (!chkver(2)) return mp_key_map[10];
  if (!chkver(3)) return mp_key_map[16];
  sethor(0);
  reshor(1);
  sethor(2);
  sethor(3);
  if (!chkver(0)) return mp_key_map[5];
  if (!chkver(1)) return mp_key_map[6];
  if (!chkver(2)) return mp_key_map[7];
  if (!chkver(3)) return mp_key_map[13];
  sethor(0);
  sethor(1);
  reshor(2);
  sethor(3);
  if (!chkver(0)) return mp_key_map[2];
  if (!chkver(1)) return mp_key_map[3];
  if (!chkver(2)) return mp_key_map[4];
  if (!chkver(3)) return mp_key_map[14];
  sethor(0);
  sethor(1);
  sethor(2);
  reshor(3);
  if (!chkver(0)) return mp_key_map[1];
  if (!chkver(1)) return mp_key_map[11];
  if (!chkver(2)) return mp_key_map[15];
  if (!chkver(3)) return mp_key_map[12];

  return mp_key_map[0];
}

// Класс драйвера дисплея AVR

#define LCD_INIT_DELAY_MS 15
#define LCD_TICK_DELAY_US 100 //100
#define LCD_CYCLE_DELAY_US 40

//#define LCD_SIZE 80 //symbols
#define LINE0 0
#define LINE1 20
#define LINE2 40
#define LINE3 60
#define LINE0_ADR 0x00
#define LINE1_ADR 0x40
#define LINE2_ADR 0x14
#define LINE3_ADR 0x54
//#define LINE_LEN 20

#define IMAGE_ON 0x0C   //  Включение изображения, курсоров есть
#define IMAGE_OFF 0x08  //  Выключение изображения
#define BUS_8BIT 0x30   //  Инициализационная команда - выбор ширины шины
                        // данных = 8 бит
#define TWO_LINES 0x38  //  8 бит шина данных, режим отображения 2 строки,
                        // размер матрицы символа 5 х 8 точек
#define ADR_INC 0x06    //  Счетчик адреса инкрементируется
#define LCD_NULL 0      //  Ничо

#define lcd_delay_mcs(delay)\
{\
  set_to_cnt(lcd_timer, MCS_TO_CNT(delay));\
  while (!test_to_cnt(lcd_timer));\
}
#define lcd_delay_ms(delay)\
{\
  set_to_cnt(lcd_timer, MS_TO_CNT(delay));\
  while (!test_to_cnt(lcd_timer));\
}
#define lcd_init_command(command)\
{\
  lcd_IR_enable();\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_enable();\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_out(command);\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_disable();\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_out(LCD_NULL);\
}

//  Перекодировка символов Windows-1251 в коды контроллера Epson на базе HD44780 с разными прибамбасами
__flash irs_u8 lcd_table[] =
{
  0x20, // '~':0
  0xFF, // '~':1 //Курсор квадратом
  0x20, // '~':2
  0x20, // '~':3
  0x20, // '~':4
  0x20, // '~':5
  0x20, // '~':6
  0x20, // '~':7
  0x20, // '~':8
  0x20, // '~':9
  0x20, // '~':10
  0x20, // '~':11
  0x20, // '~':12
  0x20, // '~':13
  0x20, // '~':14
  0x20, // '~':15
  0x20, // '~':16
  0x20, // '~':17
  0x20, // '~':18
  0x20, // '~':19
  0x20, // '~':20
  0x20, // '~':21
  0x20, // '~':22
  0x20, // '~':23
  0x20, // '~':24
  0x20, // '~':25
  0x20, // '~':26
  0x20, // '~':27
  0x20, // '~':28
  0x20, // '~':29
  0x20, // '~':30
  0x20, // '~':31
  0x20, // ' ':32
  0x21, // '!':33
  0x22, // '"':34
  0xEF,//0x23, // '#':35-------led on
  0xEE, // '$':36--------------led off
  0x25, // '%':37
  0x26, // '&':38
  0x27, // ''':39
  0x28, // '(':40
  0x29, // ')':41
  0x2A, // '*':42
  0x2B, // '+':43
  0x2C, // ',':44
  0x2D, // '-':45
  0x2E, // '.':46
  0x2F, // '/':47
  0x30, // '0':48
  0x31, // '1':49
  0x32, // '2':50
  0x33, // '3':51
  0x34, // '4':52
  0x35, // '5':53
  0x36, // '6':54
  0x37, // '7':55
  0x38, // '8':56
  0x39, // '9':57
  0x3A, // ':':58
  0x3B, // ';':59
  0x3C, // '<':60
  0x3D, // '=':61
  0x3E, // '>':62
  0x3F, // '?':63
  0x40, // '@':64
  0x41, // 'A':65
  0x42, // 'B':66
  0x43, // 'C':67
  0x44, // 'D':68
  0x45, // 'E':69
  0x46, // 'F':70
  0x47, // 'G':71
  0x48, // 'H':72
  0x49, // 'I':73
  0x4A, // 'J':74
  0x4B, // 'K':75
  0x4C, // 'L':76
  0x4D, // 'M':77
  0x4E, // 'N':78
  0x4F, // 'O':79
  0x50, // 'P':80
  0x51, // 'Q':81
  0x52, // 'R':82
  0x53, // 'S':83
  0x54, // 'T':84
  0x55, // 'U':85
  0x56, // 'V':86
  0x57, // 'W':87
  0x58, // 'X':88
  0x59, // 'Y':89
  0x5A, // 'Z':90
  0x5B, // '[':91
  0x5C, // '\':92
  0x5D, // ']':93
  0xD9, // '^':94 стрелка вверх
  0x5F, // '_':95
  0x60, // '`':96
  0x61, // 'a':97
  0x62, // 'b':98
  0x63, // 'c':99
  0x64, // 'd':100
  0x65, // 'e':101
  0x66, // 'f':102
  0x67, // 'g':103
  0x68, // 'h':104
  0x69, // 'i':105
  0x6A, // 'j':106
  0x6B, // 'k':107
  0x6C, // 'l':108
  0x6D, // 'm':109
  0x6E, // 'n':110
  0x6F, // 'o':111
  0x70, // 'p':112
  0x71, // 'q':113
  0x72, // 'r':114
  0x73, // 's':115
  0x74, // 't':116
  0x75, // 'u':117
  0x76, // 'v':118
  0x77, // 'w':119
  0x78, // 'x':120
  0x79, // 'y':121
  0x7A, // 'z':122
  0x20, // '{':123
  0x20, // '|':124
  0x20, // '}':125
  0xDA, // '~':126 стрелка вниз
  0x20, // '':127
  0x20, // 'Ђ':128
  0x20, // 'Ѓ':129
  0x20, // '‚':130
  0x20, // 'ѓ':131
  0xCA, // '„':132
  0x20, // '…':133
  0x20, // '†':134
  0x20, // '‡':135
  0x7E, // '€':136 'enter'
  0xD4, // '‰':137
  0x20, // 'Љ':138
  0x20, // '‹':139
  0x20, // 'Њ':140
  0x20, // 'Ќ':141
  0x20, // 'Ћ':142
  0x20, // 'Џ':143
  0x20, // 'ђ':144
  0x27, // '‘':145
  0x27, // '’':146
  0xCB, // '“':147
  0xCB, // '”':148
  0xDF, // '•':149
  0x2D, // '–':150
  0x2D, // '—':151
  0x20, // '':152
  0x20, // '™':153
  0x20, // 'љ':154
  0x3E, // '›':155
  0x20, // 'њ':156
  0x20, // 'ќ':157
  0x20, // 'ћ':158
  0x20, // 'џ':159
  0x20, // ' ':160
  0x20, // 'Ў':161
  0x20, // 'ў':162
  0x4A, // 'Ј':163
  0x20, // '¤':164
  0x20, // 'Ґ':165
  0x20, // '¦':166
  0xFD, // '§':167
  0xA2, // 'Ё':168
  0x20, // '©':169
  0x20, // 'Є':170
  0xC8, // '«':171
  0x20, // '¬':172
  0x2D, // '­':173
  0x20, // '®':174
  0x20, // 'Ї':175
  0xEF, // '°':176
  0x20, // '±':177
  0x49, // 'І':178
  0x69, // 'і':179
  0xB4, // 'ґ':180
  0x75, // 'µ':181
  0xFE, // '¶':182
  0xDF, // '·':183
  0xB5, // 'ё':184
  0x23, // '№':185
  0x20, // 'є':186
  0xC9, // '»':187
  0x6A, // 'ј':188
  0x53, // 'Ѕ':189
  0x73, // 'ѕ':190
  0x69, // 'ї':191
  0x41, // 'А':192
  0xA0, // 'Б':193
  0x42, // 'В':194
  0xA1, // 'Г':195
  0xE0, // 'Д':196
  0x45, // 'Е':197
  0xA3, // 'Ж':198
  0x33, // 'З':199
  0xA5, // 'И':200
  0xA6, // 'Й':201
  0x4B, // 'К':202
  0xA7, // 'Л':203
  0x4D, // 'М':204
  0x48, // 'Н':205
  0x4F, // 'О':206
  0xA8, // 'П':207
  0x50, // 'Р':208
  0x43, // 'С':209
  0x54, // 'Т':210
  0xA9, // 'У':211
  0xAA, // 'Ф':212
  0x58, // 'Х':213
  0xE1, // 'Ц':214
  0xAB, // 'Ч':215
  0xAC, // 'Ш':216
  0xE2, // 'Щ':217
  0xAD, // 'Ъ':218
  0xAE, // 'Ы':219
  0xAD, // 'Ь':220 = 'Ъ'
  0xAF, // 'Э':221
  0xB0, // 'Ю':222
  0xB1, // 'Я':223
  0x61, // 'а':224
  0xB2, // 'б':225
  0xB3, // 'в':226
  0xB4, // 'г':227
  0xE3, // 'д':228
  0x65, // 'е':229
  0xB6, // 'ж':230
  0xB7, // 'з':231
  0xB8, // 'и':232
  0xB9, // 'й':233
  0xBA, // 'к':234
  0xBB, // 'л':235
  0xBC, // 'м':236
  0xBD, // 'н':237
  0x6F, // 'о':238
  0xBE, // 'п':239
  0x70, // 'р':240
  0x63, // 'с':241
  0xBF, // 'т':242
  0x79, // 'у':243
  0xE4, // 'ф':244
  0x78, // 'х':245
  0xE5, // 'ц':246
  0xC0, // 'ч':247
  0xC1, // 'ш':248
  0xE6, // 'щ':249
  0xC2, // 'ъ':250
  0xC3, // 'ы':251
  0xC4, // 'ь':252
  0xC5, // 'э':253
  0xC6, // 'ю':254
  0xC7  // 'я':255
};

mxdisplay_drv_avr_t::mxdisplay_drv_avr_t(
  irslcd_t a_type,
  irs_avr_port_t a_data_port,
  irs_avr_port_t a_control_port,
  irs_u8 a_control_pin
):
  mp_set_data(avr_port_map[a_data_port].set),
  mp_get_data(avr_port_map[a_data_port].get),
  mp_dir_data(avr_port_map[a_data_port].dir),
  mp_set_control(avr_port_map[a_control_port].set),
  mp_get_control(avr_port_map[a_control_port].get),
  mp_dir_control(avr_port_map[a_control_port].dir),
  m_LCD_E(a_control_pin + 0),
  m_LCD_RS(a_control_pin + 1),
  LINE_COUNT(4),
  LINE_LEN(20),
  LCD_SIZE(LINE_LEN*LINE_COUNT),
  lcd_status(LCD_BEGIN),
  lcd_ram(IRS_NULL),
  lcd_current_symbol(0),
  lcd_current_symbol_address(0),
  lcd_init_counter(0),
  lcd_timer(0),
  lcd_address_jump(irs_false)
{
  init_to_cnt();

  if (a_type == irslcd_4x20) {
    LINE_COUNT = 4;
    LINE_LEN = 20;
    LCD_SIZE = LINE_LEN*LINE_COUNT;
  }

  lcd_ram = new irs_u8[LCD_SIZE];
  memset(lcd_ram, ' ', LCD_SIZE);

  //IRS_PAUSE(TIME_TO_CNT(1, 1));

  set_to_cnt(lcd_timer, MS_TO_CNT(200));
  while (!test_to_cnt(lcd_timer));

  //  Инициализация портов ввода/вывода
#ifndef DBGREG
  (*mp_dir_data) = 0xFF;  //  Шина данных LCD
  (*mp_set_data) = 0x00;
  (*mp_dir_control) |= ((1 << m_LCD_RS)|(1 << m_LCD_E)); //  Линии E и RS LCD
  (*mp_set_control) &= (((1 << m_LCD_RS)|(1 << m_LCD_E))^0xFF);
#else //DBGREG
  DDRA = 0xFF;  //  Шина данных LCD
  PORTA = 0x00;
  DDRE |= ((1 << m_LCD_RS)|(1 << m_LCD_E)); //  Линии E и RS LCD
  PORTE &= (((1 << m_LCD_RS)|(1 << m_LCD_E))^0xFF);
#endif //DBGREG

  for (irs_u8 i = 0; i < 3; i++)
  {
    //  Выдержка
    lcd_delay_ms(LCD_INIT_DELAY_MS);
    //  Команда
    lcd_init_command(BUS_8BIT);
    //  И так - 3 раза
  }
  lcd_delay_ms(LCD_INIT_DELAY_MS);
  lcd_init_command(TWO_LINES);
  lcd_delay_ms(LCD_INIT_DELAY_MS);
  lcd_init_command(IMAGE_ON);
  lcd_delay_ms(LCD_INIT_DELAY_MS);
  lcd_init_command(ADR_INC);
}
mxdisplay_drv_avr_t::~mxdisplay_drv_avr_t()
{
  //  Гашение индикатора
  lcd_delay_mcs(LCD_TICK_DELAY_US);
  lcd_init_command(IMAGE_OFF);
  //  Деинициализация портов ввода/вывода
  //  Шина данных LCD
  (*mp_dir_data) = 0x0;
  (*mp_set_data) = 0x00;
  //  Линии E и RS LCD
  (*mp_dir_control) &= (((1 << m_LCD_RS)&(1 << m_LCD_E))^0xFF);
  (*mp_set_control) &= (((1 << m_LCD_RS)&(1 << m_LCD_E))^0xFF);

  delete []lcd_ram;

  deinit_to_cnt();
}
mxdisp_pos_t mxdisplay_drv_avr_t::get_height()
{
  return LINE_COUNT;
}
mxdisp_pos_t mxdisplay_drv_avr_t::get_width()
{
  return LINE_LEN;
}
void mxdisplay_drv_avr_t::outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
  const char *text)
{
  irs_u8 begin = (irs_u8)(a_top*LINE_LEN) + a_left;
  irs_u8 delta = LCD_SIZE - begin;
  for (irs_u8 i = 0; (i < strlen(text))&&(i < delta); i++) {
    lcd_ram[begin + i] = lcd_table[(irs_u8)text[i]];
  }
}
void mxdisplay_drv_avr_t::tick()
{
  switch (lcd_status)
  {
  case LCD_BEGIN:
    {
      set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
      lcd_status = LCD_RS;
      break;
    }
  case LCD_RS:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
        if (lcd_current_symbol > LCD_SIZE-1) lcd_current_symbol = 0;
        if (lcd_address_jump == irs_false)
        {
          switch (lcd_current_symbol)
          {
          case LINE0:
            {
              lcd_current_symbol_address = LINE0_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          case /*(irs_u8)(LCD_SIZE/2)*/LINE1:
            {
              //  Переход на вторую строку. Адрес записывается в индикатор
              // только при переходе на новую строку
              lcd_current_symbol_address = LINE1_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          case LINE2:
            {
              //  Переход на третью строку. Адрес записывается в индикатор
              // только при переходе на новую строку
              lcd_current_symbol_address = LINE2_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          case LINE3:
            {
              //  Переход на четвертую строку. Адрес записывается в индикатор
              // только при переходе на новую строку
              lcd_current_symbol_address = LINE3_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          default:
            {
              lcd_status = LCD_DATA_OUT;
              lcd_DR_enable();
              break;
            }
          }
        }
        else
        {
          lcd_status = LCD_DATA_OUT;
          lcd_DR_enable();
          lcd_address_jump = irs_false;
        }
      }
      break;
    }
  case LCD_DATA_OUT:
    {
      if (test_to_cnt(lcd_timer))
      {
        lcd_status = LCD_E_RISE;
        lcd_out(lcd_ram[lcd_current_symbol]);
        lcd_current_symbol++;
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
      }
      break;
    }
  case LCD_ADDRESS_OUT:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
        lcd_status = LCD_E_RISE;
        lcd_out(lcd_current_symbol_address|0x80);
      }
      break;
    }
  case LCD_E_RISE:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
        lcd_status = LCD_E_FALL;
        lcd_enable();
      }
      break;
    }
  case LCD_E_FALL:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_CYCLE_DELAY_US));
        lcd_status = LCD_RS;
        lcd_disable();
      }
      break;
    }
  case LCD_FREE:
    {
      lcd_disable();
      lcd_out(0);
      break;
    }
  }
}
//---------------------------------------------------------------------------
// Конфигурация консоли для AVR
#ifdef NOP
irs::conio_cfg_t& irs::arch_conio_cfg::def()
{
  static mxkey_drv_avr_t key_drv(irskbd_low_pull_up, irskbd_map_horisontal,
    irs_avr_portf);
  static mxdisplay_drv_avr_t display(irslcd_4x20, irs_avr_porta,
    irs_avr_porte, 2);
  static conio_cfg_t conio_cfg_i(key_drv, display);
  return conio_cfg_i;
}
#endif //NOP
