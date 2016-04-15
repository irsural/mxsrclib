#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#ifdef __ICCARM__
#include <armcfg.h>
#endif // __ICCARM__

#include <irsstrm.h>

#include <irsfinal.h>

#ifdef __ICCARM__
// class com_buf

irs::arm::com_buf::com_buf(const com_buf& a_buf):
  m_outbuf_size(a_buf.m_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  #ifdef IRS_STM32_F2_F4_F7
  m_usart(a_buf.m_usart),
  #endif // IRS_STM32_F2_F4_F7
  m_baud_rate(a_buf.m_baud_rate)
{
  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
}
irs::arm::com_buf::com_buf(
  int a_com_index,
  int a_outbuf_size,
  irs_u32 a_baud_rate
):
  m_outbuf_size(a_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  #ifdef IRS_STM32_F2_F4_F7
  m_usart(0),
  #endif // IRS_STM32_F2_F4_F7
  m_baud_rate(a_baud_rate)
{
  volatile int index_supress_warning = a_com_index;
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  const float FOSC = cpu_traits_t::frequency(); //  Частота процессора
  float BRD = FOSC / 16.f / float(m_baud_rate);
  irs_u16 BRDI = irs_u16(BRD);  //  Целая часть делителя
  irs_u16 BRDF = irs_u16((BRD - float(BRDI))*64.f + 0.5f); //  Дробная часть

  RCGC1_bit.UART0 = 1;      //  Подача тактовой частоты на модуль UART0
  RCGC2_bit.PORTA = 1;      //  Подача тактовой частоты на PORTA (нога TX)
  for (irs_u8 i = 10; i; i--);

  UART0CTL_bit.UARTEN = 0;  //  Отключение UART0
  UART0IBRD = BRDI;
  UART0FBRD = BRDF;
  UART0LCRH_bit.SPS = 0;    //  Проверка чётности выключена
  UART0LCRH_bit.WLEN = 0x3; //  8 бит
  UART0LCRH_bit.FEN = 1;    //  FIFO включен
  UART0LCRH_bit.STP2 = 0;   //  2 стоп-бита выключено
  UART0LCRH_bit.EPS = 0;    //  Проверка чётности выключена
  UART0LCRH_bit.PEN = 0;    //  Проверка чётности выключена
  UART0LCRH_bit.BRK = 0;    //  Лень писать что, но тоже выключено
  UART0CTL_bit.RXE = 0;     //  Приём выключен
  UART0CTL_bit.TXE = 1;     //  Передача включена
  UART0CTL_bit.LBE = 0;     //  Loop-back выключен
  #ifdef __LM3SxBxx__
  UART0CTL_bit.LIN = 0;     //  LIN mode off
  UART0CTL_bit.HSE = 0;     //  ClkDiv = 16
  UART0CTL_bit.EOT = 0;     //  The TXRIS bit is set when the transmit FIFO
                            //  condition specified in UARTIFLS is met.
  UART0CTL_bit.SMART = 0;   //  The UART operates in Smart Card mode off
  #endif  //  __LM3SxBxx__
  UART0CTL_bit.SIRLP = 0;   //  Экономный режим ИК-трансивера выключен
  UART0CTL_bit.SIREN = 0;   //  И сам трансивер тоже
  UART0IM = 0;              //  Прерывания выключены
  UART0CTL_bit.UARTEN = 1;  //  UART0 включен

  GPIOADEN_bit.no1 = 1;     //  Нога TX включена
  GPIOADIR_bit.no1 = 1;     //  Нога TX выход
  GPIOAAFSEL_bit.no1 = 1;   //  Альтернативная функция включена
  //
  #ifdef __LM3SxBxx__
  (*((volatile irs_u32*)(PORTA_BASE + GPIO_PCTL))) |= PORTA1_UART0Tx;
  #endif // __LM3SxBxx__
  #elif defined(__STM32F100RBT__)
  #elif defined(IRS_STM32_F2_F4_F7)
  //typedef volatile usart_regs_t usart_regs_v_t;
  m_usart = get_usart(a_com_index);
  switch (a_com_index) {
    case 1: {
      //m_usart = reinterpret_cast<usart_regs_t*>(IRS_USART1_BASE);
      irs::clock_enable(IRS_USART1_BASE);
      irs::clock_enable(IRS_PORTA_BASE);
    } break;
    case 2: {
      //m_usart = reinterpret_cast<usart_regs_t*>(IRS_USART2_BASE);
      irs::clock_enable(IRS_USART2_BASE);
      irs::clock_enable(IRS_PORTA_BASE);
    } break;
    case 3: {
      //m_usart = reinterpret_cast<usart_regs_t*>(IRS_USART3_BASE);
      irs::clock_enable(IRS_USART3_BASE);
      irs::clock_enable(IRS_PORTB_BASE);
    } break;
    case 4: {
      //m_usart = reinterpret_cast<usart_regs_t*>(IRS_UART4_BASE);
      irs::clock_enable(IRS_UART4_BASE);
      irs::clock_enable(IRS_PORTC_BASE);
    } break;
    case 5: {
      //m_usart = reinterpret_cast<usart_regs_t*>(IRS_UART5_BASE);
      irs::clock_enable(IRS_UART5_BASE);
      irs::clock_enable(IRS_PORTC_BASE);
      irs::clock_enable(IRS_PORTD_BASE);
    } break;
    case 6: {
      //m_usart = reinterpret_cast<usart_regs_t*>(IRS_USART6_BASE);
      irs::clock_enable(IRS_USART6_BASE);
      irs::clock_enable(IRS_PORTC_BASE);
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Индекс ком-порта должен быть от 1 до 6");
    }
  }
  set_usart_options(a_com_index);
  /*m_usart->USART_CR1_bit.UE = 1;
  m_usart->USART_CR1_bit.M = 0; // 8 Data bits
  m_usart->USART_CR1_bit.PCE = 0; // Parity control disabled
  m_usart->USART_CR1_bit.PS = 0; // Even parity
  m_usart->USART_CR2_bit.STOP = 0; // 1 stop bit
  m_usart->USART_CR1_bit.OVER8 = 0;
  volatile irs::cpu_traits_t::frequency_type periphery_frequency =
    irs::cpu_traits_t::periphery_frequency_first();
  if ((a_com_index == 1) || (a_com_index == 6)) {
    periphery_frequency = irs::cpu_traits_t::periphery_frequency_second();
  }
  m_usart->USART_BRR_bit.DIV_Mantissa = periphery_frequency/(16*a_baud_rate);
  m_usart->USART_BRR_bit.DIV_Fraction =
    16*(periphery_frequency%(16*a_baud_rate))/(16*a_baud_rate);*/
  switch (a_com_index) {
    case 1: {
      GPIOA_AFRH_bit.AFRH9 = 7; // AF7: USART1_TX
      GPIOA_AFRH_bit.AFRH10 = 7; // AF7: USART1_RX
      GPIOA_MODER_bit.MODER9 = 2; // 10: Alternate function mode
      GPIOA_MODER_bit.MODER10 = 2; // 10: Alternate function mode
    } break;
    case 2: {
      GPIOA_AFRL_bit.AFRL2 = 7; // AF7: USART2_TX
      GPIOA_AFRL_bit.AFRL3 = 7; // AF7: USART2_RX
      GPIOA_MODER_bit.MODER2 = 2; // 10: Alternate function mode
      GPIOA_MODER_bit.MODER3 = 2; // 10: Alternate function mode
    } break;
    case 3: {
      GPIOB_AFRH_bit.AFRH10 = 7; // AF7: USART3_TX
      GPIOB_AFRH_bit.AFRH11 = 7; // AF7: USART3_RX
      GPIOB_MODER_bit.MODER10 = 2; // 10: Alternate function mode
      GPIOB_MODER_bit.MODER11 = 2; // 10: Alternate function mode
    } break;
    case 4: {
      GPIOC_AFRH_bit.AFRH10 = 8; // AF8: USART4_TX
      GPIOC_AFRH_bit.AFRH11 = 8; // AF8: USART4_RX
      GPIOC_MODER_bit.MODER10 = 2; // 10: Alternate function mode
      GPIOC_MODER_bit.MODER11 = 2; // 10: Alternate function mode
    } break;
    case 5: {
      GPIOC_AFRH_bit.AFRH12 = 8; // AF8: USART5_TX
      GPIOD_AFRL_bit.AFRL2 = 8; // AF8: USART5_RX
      GPIOC_MODER_bit.MODER12 = 2; // 10: Alternate function mode
      GPIOD_MODER_bit.MODER2 = 2; // 10: Alternate function mode
    } break;
    case 6: {
      GPIOC_AFRL_bit.AFRL6 = 8; // AF8: USART4_TX
      GPIOC_AFRL_bit.AFRL7 = 8; // AF8: USART4_RX
      GPIOC_MODER_bit.MODER6 = 2; // 10: Alternate function mode
      GPIOC_MODER_bit.MODER7 = 2; // 10: Alternate function mode
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Индекс ком-порта должен быть от 1 до 6");
    }
  }
  // 1: Transmitter is enabled
  m_usart->USART_CR1_bit.TE = 1;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type

  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);

  // Задержка необходимая для того, чтобы COM-порт успел инициализироватся
  //__delay_cycles(4000);
}

void irs::arm::com_buf::set_usart_options(int a_com_index)
{
  m_usart->USART_CR1_bit.UE = 1;
  
  #ifdef IRS_STM32F7xx
  m_usart->USART_CR1_bit.M0 = 0; // 8 Data bits
  #else // F2 F4
  m_usart->USART_CR1_bit.M = 0; // 8 Data bits
  #endif // F2 F4
  m_usart->USART_CR1_bit.PCE = 0; // Parity control disabled
  m_usart->USART_CR1_bit.PS = 0; // Even parity
  m_usart->USART_CR2_bit.STOP = 0; // 1 stop bit
  m_usart->USART_CR1_bit.OVER8 = 0;
  volatile irs::cpu_traits_t::frequency_type periphery_frequency =
    irs::cpu_traits_t::periphery_frequency_first();
  if ((a_com_index == 1) || (a_com_index == 6)) {
    periphery_frequency = irs::cpu_traits_t::periphery_frequency_second();
  }
  m_usart->USART_BRR_bit.DIV_Mantissa = periphery_frequency/(16*m_baud_rate);
  m_usart->USART_BRR_bit.DIV_Fraction =
    16*(periphery_frequency%(16*m_baud_rate))/(16*m_baud_rate);
}

#ifdef IRS_STM32_F2_F4_F7
irs::arm::com_buf::com_buf(
  int a_com_index,
  gpio_channel_t a_tx,
  int a_outbuf_size,
  irs_u32 a_baud_rate
):
  m_outbuf_size(a_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  m_usart(0),
  m_baud_rate(a_baud_rate)
{
  volatile int index_supress_warning = a_com_index;
  IRS_LIB_ASSERT((a_tx != PNONE));

  m_usart = get_usart(a_com_index);
  irs::clock_enable(reinterpret_cast<size_t>(m_usart));

  if (a_tx != PNONE) {
    irs::clock_enable(a_tx);
  }

  if (a_tx != PNONE) {
    gpio_moder_alternate_function_enable(a_tx);
    gpio_usart_alternate_function_select(a_tx, a_com_index);
  }
  set_usart_options(a_com_index);
  if (a_tx != PNONE) {
    m_usart->USART_CR1_bit.TE = 1; // 1: Transmitter is enabled
  }  
}

#endif // IRS_STM32_F2_F4_F7
/*inline irs::com_buf::~com_buf()
{
}*/
void irs::arm::com_buf::trans (char data)
{
  if (data == '\n'){
    trans_simple('\r');
    trans_simple('\n');
  } else {
    trans_simple(data);
  }
}
void irs::arm::com_buf::trans_simple (char data)
{
  #if defined(__LM3Sx9xx__)
    UART0DR = data;
    while (UART0FR_bit.TXFF);
  #elif defined(__LM3SxBxx__)
    UART0DR_bit.DATA = data;
    while (UART0FR_bit.TXFF);
  #elif defined(__STM32F100RBT__)
    volatile char x = data;
    //data = 0;
  #elif defined(IRS_STM32_F2_F4_F7)
    // 1: Transmitter is enabled
    //m_usart->USART_CR1_bit.TE = 1;
    #ifdef IRS_STM32F7xx
    while (m_usart->USART_ISR_bit.TC != 1);
    m_usart->USART_TDR = data;
    #else // F2 F4
    while (m_usart->USART_SR_bit.TC != 1);
    m_usart->USART_DR = data;
    #endif // F2 F4
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

int irs::arm::com_buf::overflow(int c)
{
  int len_s = pptr() - pbase();
  if (len_s > 0) {
    *pptr() = 0;
    char* pend = pptr();
    for(char *message = pbase(); message<pend; message++ ) {
      trans(*message);
    }
  }
  if (c != EOF) {
    trans(c);
  }
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
  return 0;
}
int irs::arm::com_buf::sync()
{
  return overflow();
}

#endif // __ICCARM__
