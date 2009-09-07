// ƒрайвер SPI
// ƒата 17.08.2009
// AVR

#include <ioavr.h>

#include <avrspi.h>


#ifndef __ATmega128__

#include <irsstd.h>

irs::avr::avr_spi_t::avr_spi_t(irs_u8 a_buffer_size, irs_u32 a_f_osc,
  spi_type_t a_spi_type):
  //  переменные состо€ни€
  m_status(SPI_FREE),
  //  буфер
  mp_buf(0),
  mp_target_buf(0),
  m_buffer_size(a_buffer_size),
  m_current_byte(0),
  m_packet_size(0),
  //  параметры передачи
  m_bitrate(100000),
  m_polarity(FALLING_EDGE),
  m_phase(LEAD_EDGE),
  m_order(MSB),
  //  флаги
  m_buf_empty(true),
  m_lock(false),
  //  параметры железа
  m_f_osc(a_f_osc),
  m_spi_type(a_spi_type)
{
  if (m_buffer_size) mp_buf = new irs_u8[m_buffer_size];
  if (mp_buf) memset((void*)mp_buf, 0, m_buffer_size);
  switch (m_spi_type)
  {
  case USART0:
    {
      //  инициализаци€ железа USART0
      UCSR0C_UDORD0 = 0;    //  MSB first
      UBRR0 = 0;            //  Baudrate = Fosc / 2

      DDRE_DDE2 = 1;        //  PE2 = SCLK -> out
      PORTE_PORTE2 = 1;     //  SCLK = 1
      DDRE_DDE1 = 1;        //  PE1 = TXD -> out
      PORTE_PORTE1 = 1;     //  TXD = 1
      DDRE_DDE0 = 0;        //  PD3 = RXD -> in
      PORTE_PORTE0 = 1;     //  RXD pull up

      UCSR0C_UMSEL01 = 1;   //  master SPI
      UCSR0C_UMSEL00 = 1;   //  master SPI
      UCSR0C &= 0x20^0xFF;  //  leading edge sample,через UCSR0C_UCPHA0=0 нельз€
      UCSR0C_UCPOL0 = 1;    //  clock falling edge first

      UCSR0B_RXCIE0 = 0;    //  recieve interrupt disable
      UCSR0B_TXCIE0 = 0;    //  transmite interrupt disable
      UCSR0B_UDRIE0 = 0;    //  data register empty interrupt disable
      UCSR0B_TXEN0 = 1;     //  reciever enable
      UCSR0B_RXEN0 = 1;     //  transmite enable

      UBRR0 = (m_f_osc/(2*100000)) - 1;//baudrate = Fosc / 2(UBRR1+1) = 100 kHz
      break;
    }
  case USART1:
    {
      //  инициализаци€ железа USART1
      UCSR1C_UDORD1 = 0;    //  MSB first
      UBRR1 = 0;            //  Baudrate = Fosc / 2

      DDRD_DDD5 = 1;        //  PD5 = SCLK -> out
      PORTD_PORTD5 = 1;     //  SCLK = 1
      DDRD_DDD3 = 1;        //  PD3 = TXD -> out
      PORTD_PORTD3 = 1;      //  TXD = 1
      DDRD_DDD2 = 0;        //  PD3 = RXD -> in
      PORTD_PORTD2 = 1;      //  RXD pull up

      /*UCSR1C_UMSEL11 = 1;   //  master SPI
      UCSR1C_UMSEL10 = 1;   //  master SPI
      UCSR1C &= 0x02^0xFF;  //  leading edge sample,через UCSR1C_UCPHA1=0 нельз€
      UCSR1C_UCPOL1 = 1;    //  clock falling edge first*/
      UCSR1C = (1 << UMSEL10) | (1 << UMSEL11) | (1 << UCPOL1) | (1 << UCPHA1);

      UCSR1B_RXCIE1 = 0;    //  recieve interrupt disable
      UCSR1B_TXCIE1 = 0;    //  transmite interrupt disable
      UCSR1B_UDRIE1 = 0;    //  data register empty interrupt disable
      UCSR1B_TXEN1 = 1;     //  reciever enable
      UCSR1B_RXEN1 = 1;     //  transmite enable

      UBRR1 = (m_f_osc/(2*100000)) - 1;//baudrate = Fosc / 2(UBRR1+1) = 100 kHz
      break;
    }
  case SPI:
    {
      //  инициализаци€ железа
      //  I/O Ports initialization for SPI
      DDRB_DDB3 = 0;   //  PB3 = MISO -> in
      PORTB_PORTB3 = 1; //  MISO pull-up
      DDRB_DDB0 = 1;    //  PB0 = SPI SS-pin -> out
      PORTB_PORTB0 = 1; //  SS = 1  //  Ahtung!!! должен быть 1!!!
      DDRB_DDB1 = 1;    //  PB1 = SCLK -> out
      PORTB_PORTB1 = 1; //  SCLK = 1
      DDRB_DDB2 = 1;    //  PB2 = MOSI -> out
      PORTB_PORTB2 = 1; //  MOSI = 1
      //  SPI hardware initialization
      SPCR_SPIE = 0;    //SPI interrupt disable
      SPCR_SPE = 1;     //SPI enable
      SPCR_DORD = 0;    //SPI MSB first
      SPCR_MSTR = 1;    //SPI master mode
      SPCR_CPOL = 1;    //SPI clock falling edge first
      SPCR_CPHA = 0;    //SPI clock phase: data sample = falling edge
      SPCR_SPR1 = 1;    //SPI clock rate = Fosc/128 = 125 kHz for Fosc = 16 MHz
      SPCR_SPR0 = 1;    //-|-
      break;
    }
  }
}

irs::avr::avr_spi_t::~avr_spi_t()
{
  delete []mp_buf;
  switch (m_spi_type)
  {
  case USART0:
    {
      //  денициализаци€ железа USART1
      UCSR0C_UDORD0 = 0;    //  MSB first
      UBRR0 = 0xFFF;        //  Baudrate = Fosc / 8192

      DDRE_DDE2 = 0;        //  PE2 = SCLK -> no out
      PORTE_PORTE2 = 0;     //  SCLK no pull-up

      UCSR0C_UMSEL01 = 0;   //  asynchronus USART
      UCSR0C_UMSEL00 = 0;   //  asynchronus USART
      UCSR0C &= 0x20^0xFF;  //  leading edge sample;
      UCSR0C_UCPOL0 = 0;    //  clock rising edge first

      UCSR0B_RXCIE0 = 0;    //  recieve interrupt disable
      UCSR0B_TXCIE0 = 0;    //  transmite interrupt disable
      UCSR0B_UDRIE0 = 0;    //  data register empty interrupt disable
      UCSR0B_TXEN0 = 0;     //  reciever disable
      UCSR0B_RXEN0 = 0;     //  transmite disable
      break;
    }
  case USART1:
    {
      //  денициализаци€ железа USART1
      UCSR1C_UDORD1 = 0;    //  MSB first
      UBRR1 = 0xFFF;        //  Baudrate = Fosc / 8192
      DDRD_DDD5 = 0;        //  PD5 = SCLK -> no out
      PORTD_PORTD5 = 0;     //  SCLK no pull-up

      UCSR1C_UMSEL11 = 0;   //  asynchronus USART
      UCSR1C_UMSEL10 = 0;   //  asynchronus USART
      UCSR1C &= 0x20^0xFF;  //  leading edge sample;
      UCSR1C_UCPOL1 = 0;    //  clock rising edge first

      UCSR1B_RXCIE1 = 0;    //  recieve interrupt disable
      UCSR1B_TXCIE1 = 0;    //  transmite interrupt disable
      UCSR1B_UDRIE1 = 0;    //  data register empty interrupt disable
      UCSR1B_TXEN1 = 0;     //  reciever disable
      UCSR1B_RXEN1 = 0;     //  transmite disable
      break;
    }
  case SPI:
    {
      //  деинициализаци€ железа
      //  I/O Ports initialization for SPI
      DDRB_DDB3 &= 0;   //  PB3 = MISO -> in
      PORTB_PORTB3 = 0; //  MISO no pull-up
      DDRB_DDB0 = 0;    //  PB0 = SPI SS-pin -> no out
      PORTB_PORTB0 = 0; //  SS no pull up
      DDRB_DDB1 = 0;    //  PB1 = SCLK -> no out
      PORTB_PORTB1 = 0; //  SCLK no pull up
      DDRB_DDB2 = 0;    //  PB2 = MOSI -> no out
      PORTB_PORTB2 = 0; //  MOSI no pull-up
      //  SPI hardware initialization
      SPCR_SPIE = 0;    //SPI interrupt disable
      SPCR_SPE = 0;     //SPI disable
      SPCR_DORD = 0;    //SPI MSB first
      SPCR_MSTR = 1;    //SPI master mode
      SPCR_CPOL = 1;    //SPI clock falling edge first
      SPCR_CPHA = 0;    //SPI clock phase: data sample = falling edge
      SPCR_SPR1 = 0;    //SPI clock rate = Fosc/4 = 4 MHz
      SPCR_SPR0 = 0;    //-|-
      break;
    }
  }
}

void irs::avr::avr_spi_t::abort()
{
  m_status = SPI_FREE;
  m_current_byte = 0;
  m_packet_size = 0;
  m_buf_empty = true;
  memset((void*)mp_buf, 0, m_buffer_size);
  transfer_complete();
}

irs::spi_t::status_t irs::avr::avr_spi_t::get_status()
{
  if (m_status == SPI_FREE) return FREE;
  else return BUSY;
}

bool irs::avr::avr_spi_t::set_bitrate(irs_u32 a_bitrate)
{
  if (m_status == SPI_FREE)
  {
    switch (m_spi_type)
    {
    case USART0:
      {
        if (a_bitrate >= (m_f_osc / 4))
          UBRR0 = 1;
        else
          UBRR0 = (m_f_osc / (a_bitrate * 2)) - 1;
        return true;
      }
    case USART1:
      {
        if (a_bitrate >= (m_f_osc / 4))
          UBRR1 = 1;
        else
          UBRR1 = (m_f_osc / (a_bitrate * 2)) - 1;
        return true;
      }
    case SPI:
      {
        irs_u8 sclk_div = m_f_osc / a_bitrate;
        SPCR_SPIE = 0;
        if (sclk_div <= 4)
        {
          SPCR_SPR1 = 0;    //SPI clock rate = Fosc/4 = 4 MHz
          SPCR_SPR0 = 0;    //-|-
          SPCR_SPIE = 1;
          return true;
        }
        if (sclk_div <= 16)
        {
          SPCR_SPR1 = 0;    //SPI clock rate = Fosc/64 = 250 kHz
          SPCR_SPR0 = 1;    //-|-
          SPCR_SPIE = 1;
          return true;
        }
        if (sclk_div <= 64)
        {
          SPCR_SPR1 = 1;    //SPI clock rate = Fosc/128 = 125 kHz
          SPCR_SPR0 = 0;
          SPCR_SPIE = 1;
          return true;
        }
        SPCR_SPR1 = 1;    //SPI clock rate = Fosc/128 = 125 kHz
        SPCR_SPR0 = 1;
        SPCR_SPIE = 1;
        break;
      }
    }
    return true;
  }
  else return false;
}

bool irs::avr::avr_spi_t::set_polarity(polarity_t a_polarity)
{
  if (m_status == SPI_FREE)
  {
    switch (m_spi_type)
    {
    case USART0:
      {
        if (a_polarity == RISING_EDGE)
          UCSR0C_UCPOL0 = 0;    //  clock rising edge first
        else
          UCSR0C_UCPOL0 = 1;    //  clock falling edge first
        break;
      }
    case USART1:
      {
        if (a_polarity == RISING_EDGE)
          UCSR1C_UCPOL1 = 0;    //  clock rising edge first
        else
          UCSR1C_UCPOL1 = 1;    //  clock falling edge first
        break;
      }
    case SPI:
      {
        if (a_polarity == RISING_EDGE)
          SPCR_CPOL = 0;    //  clock rising edge first
        else
          SPCR_CPOL = 1;    //  clock falling edge first
        break;
      }
    }
    return true;
  }
  else return false;
}

bool irs::avr::avr_spi_t::set_phase(phase_t a_phase)
{
  if (m_status == SPI_FREE)
  {
    switch (m_spi_type)
    {
    case USART0:
      {
        if (a_phase == LEAD_EDGE)
          UCSR0C &= 0x02^0xFF;  //  leading edge sample
        else
          UCSR0C |= 0x02;       //  trailing edge sample
        break;
      }
    case USART1:
      {
        if (a_phase == LEAD_EDGE)
          UCSR1C &= 0x02^0xFF;  //  leading edge sample
        else
          UCSR1C |= 0x02;       //  trailing edge sample
        break;
      }
    case SPI:
      {
        if (a_phase == LEAD_EDGE)
          SPCR_CPHA = 0;  //  leading edge sample
        else
          SPCR_CPHA = 1;  //  trailing edge sample
        break;
      }
    }
    return true;
  }
  else return false;
}

bool irs::avr::avr_spi_t::set_order(order_t a_order)
{
  if (m_status == SPI_FREE)
  {
    switch (m_spi_type)
    {
    case USART0:
      {
        irs_u16 temp_UBRR0 = UBRR0;
        UBRR0 = 0;
        UCSR0B_TXEN0 = 0;     //  reciever disable
        UCSR0B_RXEN0 = 0;     //  transmite disable
        if (a_order == MSB)
          UCSR1C &= ((1 << UDORD1)^0xFF);
        else
          UCSR1C |= (1 << UDORD1);
        UCSR0B_TXEN0 = 1;     //  reciever enable
        UCSR0B_RXEN0 = 1;     //  transmite enable
        UBRR0 = temp_UBRR0;
        break;
      }
    case USART1:
      {
        irs_u16 temp_UBRR1 = UBRR1;
        UBRR1 = 0;
        UCSR1B_TXEN1 = 0;     //  reciever disable
        UCSR1B_RXEN1 = 0;     //  transmite disable
        if (a_order == MSB)
          UCSR1C &= ((1 << UDORD1)^0xFF);
        else
          UCSR1C |= (1 << UDORD1);
        UCSR1B_TXEN1 = 1;     //  reciever enable
        UCSR1B_RXEN1 = 1;     //  transmite enable
        UBRR1 = temp_UBRR1;
        break;
      }
    case SPI:
      {
        if (a_order == MSB)
          SPCR_DORD = 0;  //  MSB first
        else
          SPCR_DORD = 1;  //  LSB first
        break;
      }
    }
    return true;
  }
  else return false;
}

void irs::avr::avr_spi_t::write(const irs_u8 *ap_buf,irs_uarc a_size)
{
  if (mp_buf && m_buf_empty)
  {
    m_packet_size = irs_u8(a_size);
    if (m_packet_size > m_buffer_size) m_packet_size = m_buffer_size;
    memcpy((void*)mp_buf, (void*)ap_buf, m_packet_size);
    write_data_register(mp_buf[0]);
    m_current_byte = 1;
    m_buf_empty = false;
    m_status = SPI_WRITE;
  }
}

void irs::avr::avr_spi_t::read(irs_u8 *ap_buf,irs_uarc a_size)
{
  if (mp_buf && m_buf_empty)
  {
    m_packet_size = irs_u8(a_size);
    if (m_packet_size > m_buffer_size) m_packet_size = m_buffer_size;
    mp_target_buf = ap_buf;
    m_current_byte = 0;
    read_data_register();
    write_data_register(0);
    m_buf_empty = false;
    m_status = SPI_READ;
  }
}

void irs::avr::avr_spi_t::tick()
{
  static counter_t cnt = 0;
  static counter_t time = MS_TO_CNT(0);
  static bool first = true;
  if (first)
  {
    init_to_cnt();
    first = false;
    counter_t time = MS_TO_CNT(0);
    set_to_cnt(cnt, time);
  }
  switch (m_status)
  {
  case SPI_WRITE:
    {
      if (test_to_cnt(cnt))
      {
        if (transfer_complete())
        {
          set_to_cnt(cnt, time);
          if (m_current_byte >= m_packet_size)
          {
            memset((void*)mp_buf, 0, m_buffer_size);
            m_buf_empty = true;
            m_status = SPI_FREE;
          }
          else
          {
            write_data_register(mp_buf[m_current_byte]);
            m_current_byte++;
          }
        }
      }
      break;
    }
  case SPI_READ:
    {
      if (transfer_complete())
      {
        if (m_current_byte >=m_packet_size-1)
        {
          mp_buf[m_current_byte] = read_data_register();
          memcpy((void*)mp_target_buf, (void*)mp_buf, m_packet_size);
          memset((void*)mp_buf, 0, m_buffer_size);
          m_buf_empty = true;
          m_status = SPI_FREE;
        }
        else
        {
          mp_buf[m_current_byte] = read_data_register();
          write_data_register(m_current_byte+1);
          m_current_byte++;
        }
      }
      break;
    }
  }
}

void irs::avr::avr_spi_t::write_data_register(irs_u8 a_data)
{
  switch (m_spi_type)
  {
  case USART0: UDR0 = a_data; return;
  case USART1: UDR1 = a_data; return;
  case SPI: SPDR = a_data; return;
  }
}

irs_u8 irs::avr::avr_spi_t::read_data_register()
{
  switch (m_spi_type)
  {
  case USART0: return UDR0;
  case USART1: return UDR1;
  case SPI: return SPDR;
  }
  return 0;
}

bool irs::avr::avr_spi_t::transfer_complete()
{
  switch (m_spi_type)
  {
  case USART0:
    {
      if (UCSR0A_TXC0)
      {
        UCSR0A_TXC0 = 1;
        return true;
      }
      else return false;
    }
  case USART1:
    {
      if (UCSR1A_TXC1)
      {
        UCSR1A_TXC1 = 1;
        return true;
      }
      else return false;
    }
  case SPI:
    {
      if (SPSR_SPIF) return true;
      else return false;
    }
  }
  return false;
}

void irs::avr::avr_spi_t::lock()
{
  m_lock = true;
}

void irs::avr::avr_spi_t::unlock()
{
  m_lock = false;
}

bool irs::avr::avr_spi_t::get_lock()
{
  return m_lock;
}

#endif //__ATmega128__
