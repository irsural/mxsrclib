// Низкоуровневая передача через SPI
// Дата 18.08.2009

#include <ioavr.h>

#include <hardflow.h>


#ifndef __ATmega128__

#include <irsalg.h>
#include <irsavrutil.h>

irs::avr::spi_master_flow_t::spi_master_flow_t(irs_u8 a_buffer_size):
  //  служебные переменные
  m_status(PREPARATION),
  //  Временные интервалы
  m_byte_counter(0),
  m_preparation_counter(0),
  //  буфера
  mp_read_buffer(0),
  mp_write_buffer(0),
  m_buffer_size(a_buffer_size),
  m_current_byte(0),
  m_rb_channel_id(0),
  m_rb_read_size(0),
  m_rb_packet_size(0),
  m_max_buffer_size(a_buffer_size),
  //  флаги
  m_rb_new_data(false),
  m_wb_new_data(false),
  m_can_write(false),
  //  переменные железа
  m_sclk_div(4),
  m_current_sclk_div(4)
{
  mp_read_buffer = new irs_u8[m_buffer_size];
  mp_write_buffer = new irs_u8[m_buffer_size];
  mp_user_read_buffer = new irs_u8[m_buffer_size];
  mp_user_write_buffer = new irs_u8[m_buffer_size];
  memset(mp_read_buffer, 0, m_buffer_size);
  memset(mp_write_buffer, 0, m_buffer_size);
  memset(mp_user_read_buffer, 0, m_buffer_size);
  memset(mp_user_write_buffer, 0, m_buffer_size);
  //  инициализация железа
  //  I/O Ports initialization for SPI
  DDRB_DDB3 = 0;    //  PB3 = MISO -> in
  PORTB_PORTB3 = 0; //  MISO no pull-up
  DDRB_DDB0 = 1;    //  PB0 = SPI SS-pin -> out
  PORTB_PORTB0 = 1; //  SS = 1
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
  SPCR_SPR1 = 0;    //SPI clock rate = Fosc/4 = 4 MHz
  SPCR_SPR0 = 0;    //-|-
  //
  init_to_cnt();
  m_byte_delay = MCS_TO_CNT(20);
  m_preparation_delay = MS_TO_CNT(5);
  //
  ss_high();
}

irs::avr::spi_master_flow_t::~spi_master_flow_t()
{
  if (mp_read_buffer) delete []mp_read_buffer;
  if (mp_write_buffer) delete []mp_write_buffer;
  if (mp_user_read_buffer) delete []mp_user_read_buffer;
  if (mp_user_write_buffer) delete []mp_user_write_buffer;
  //  деинициализация железа
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
}

irs::string irs::avr::spi_master_flow_t::param(const irs::string &a_name)
{
  irs::string answer = "0";
  if (a_name == "buffer_size") {answer = int(m_buffer_size); return answer;}
  return answer;
}

void irs::avr::spi_master_flow_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{
  string name = a_name;
  if (name == "sclk_div")
  {
    irs_u8 sclk_div = 0;
    if (name.to_number<irs_u8>(m_sclk_div))
      m_sclk_div = sclk_div;
    if (m_sclk_div < 4) m_sclk_div = 4;
    if ((m_sclk_div > 4) && (m_sclk_div < 16)) m_sclk_div = 16;
    if ((m_sclk_div > 16) && (m_sclk_div < 64)) m_sclk_div = 64;
    if (m_sclk_div > 64) m_sclk_div = 128;
    return;
  }
  return;
}

irs_uarc irs::avr::spi_master_flow_t::read(irs_uarc &a_channel_id,
  irs_u8 *ap_buf, irs_uarc a_size)
{
  if (m_rb_new_data)
  {
    irs_u8 size = irs_u8(a_size);
    if (size > m_rb_packet_size - m_rb_read_size)
      size = m_rb_packet_size - m_rb_read_size;
    void *rdbuf = (void*)&mp_user_read_buffer[m_data_pos + m_rb_read_size];
    memcpy((void*)ap_buf, rdbuf, size);
    m_rb_read_size += size;
    if (m_rb_read_size >= m_rb_packet_size)
    {
      memset((void*)mp_user_read_buffer, 0, m_buffer_size);
      m_rb_new_data = false;
      m_rb_read_size = 0;
    }
    return size;
  }
  else return 0;
}

irs_uarc irs::avr::spi_master_flow_t::write(irs_uarc a_channel_id,
  const irs_u8 *ap_buf, irs_uarc a_size)
{
  if (!m_wb_new_data) {
    m_wb_new_data = true;
    irs_u8 size = a_size;
    if (a_size > m_buffer_size - m_header_size)
      size = m_buffer_size - m_header_size;

    memset((void*)mp_user_write_buffer, 0, m_buffer_size);
    mp_user_write_buffer[m_size_pos] = size;
    mp_user_write_buffer[m_channel_id_pos] = irs_u8(a_channel_id);
    memcpy((void*)&mp_user_write_buffer[m_data_pos],(void*)ap_buf, size);
    irs_u8 crc = crc8(mp_user_write_buffer, 2, irs_u8(size + 2));
    mp_user_write_buffer[m_crc_pos] = crc;
    return size;
  } else {
    return 0;
  }
}

void irs::avr::spi_master_flow_t::apply_sclk_div(irs_u8 a_sclk_div)
{
  SPCR_SPIE = 0;
  switch (a_sclk_div)
  {
  case 4:
    {
      SPCR_SPR1 = 0;    //SPI clock rate = Fosc/4 = 4 MHz
      SPCR_SPR0 = 0;    //-|-
      break;
    }
  case 16:
    {
      SPCR_SPR1 = 0;    //SPI clock rate = Fosc/16 = 1 MHz
      SPCR_SPR0 = 1;    //-|-
      break;
    }
  case 64:
    {
      SPCR_SPR1 = 1;    //SPI clock rate = Fosc/64 = 250 kHz
      SPCR_SPR0 = 0;    //-|-
      break;
    }
  case 128:
    {
      SPCR_SPR1 = 1;    //SPI clock rate = Fosc/128 = 125 kHz
      SPCR_SPR0 = 1;
      break;
    }
  default:
    {
      SPCR_SPR1 = 1;    //SPI clock rate = Fosc/128 = 125 kHz
      SPCR_SPR0 = 1;
      break;
    }
  }
  SPCR_SPIE = 1;
}


void irs::avr::spi_master_flow_t::tick()
{
  switch (m_status)
  {
    case PREPARATION:
    {
      if (test_to_cnt(m_preparation_counter))
      {
        set_to_cnt(m_byte_counter, m_byte_delay);
        ss_low();
        if (!m_rb_new_data) mp_write_buffer[m_neg_pos] = 1;
        else mp_write_buffer[m_neg_pos] = 0;
        m_current_byte = 0;
        m_max_buffer_size = mp_write_buffer[m_size_pos] + m_header_size;
        m_status = TRANSLATION;
      }
      break;
    }
    case TRANSLATION:
    {
      if (test_to_cnt(m_byte_counter))
      {
        set_to_cnt(m_byte_counter, m_byte_delay);
        m_status = TRANSLATION;
        write_data_register(mp_write_buffer[m_current_byte]);
        for (;;)
        {
          if (test_to_cnt(m_byte_counter))
          {
            if (transfer_complete())
            {
              set_to_cnt(m_byte_counter, m_byte_delay);
              irs_u8 read_value = read_data_register();

              if (m_current_byte == m_neg_pos)
              {
                //if (read_value == 1)
                  m_can_write = true;
                //else m_can_write = false;
              }

              if (m_current_byte == m_size_pos)
              {
                if (read_value > mp_write_buffer[m_size_pos])
                  m_max_buffer_size = read_value + m_header_size;

                if (m_max_buffer_size > m_buffer_size)
                  m_max_buffer_size = m_buffer_size;
              }

              mp_read_buffer[m_current_byte] = read_value;
              if (m_current_byte < m_max_buffer_size - 1)
              {
                m_current_byte++;
                if (m_can_write)
                  write_data_register(mp_write_buffer[m_current_byte]);
                else write_data_register(0);
              }
              else
              {
                m_status = PROCESSING;
                break;
              }
            }
          }
        }
      }
      break;
    }
    case PROCESSING:
    {
      ss_high();
      set_to_cnt(m_preparation_counter, m_preparation_delay);
      if (!m_rb_new_data)
      {
        if (mp_read_buffer[m_size_pos] > 0)
        {
          irs_u8 crc = mp_read_buffer[m_crc_pos];
          if (crc == crc8(mp_read_buffer, 2, irs_u8(mp_read_buffer[m_size_pos] + 2)))
          {
            m_rb_channel_id = mp_read_buffer[m_channel_id_pos];
            m_rb_packet_size = mp_read_buffer[m_size_pos];
            memcpy((void*)mp_user_read_buffer, (void*)mp_read_buffer, m_buffer_size);
            m_rb_new_data = true;
          }
        }
        memset(mp_read_buffer, 0, m_buffer_size);
      }
      if (m_can_write)
      {
        mp_write_buffer[m_crc_pos] = 0;
        mp_write_buffer[m_size_pos] = 0;
        //memset(mp_write_buffer, m_buffer_size, 0);
        if (m_wb_new_data)
        {
          m_wb_new_data = false;
          memcpy((void*)mp_write_buffer, (void*)mp_user_write_buffer,
            m_buffer_size);
        }
      }
      m_status = PREPARATION;
      break;
    }
  }
}

inline void irs::avr::spi_master_flow_t::write_data_register(irs_u8 a_data)
{
  SPDR = a_data;
}

inline irs_u8 irs::avr::spi_master_flow_t::read_data_register()
{
  return SPDR;
}

inline bool irs::avr::spi_master_flow_t::transfer_complete()
{
  if (SPSR_SPIF) return true;
  else return false;
}

inline void irs::avr::spi_master_flow_t::ss_high()
{
  PORTB_PORTB0 = 1;
}

inline void irs::avr::spi_master_flow_t::ss_low()
{
  PORTB_PORTB0 = 0;
}

//------------------------------------------------------------------------------


irs::avr::spi_slave_flow_t::spi_slave_flow_t(irs_u8 a_buffer_size):
  //  буфера
  mp_read_buffer(IRS_NULL),
  mp_user_read_buffer(IRS_NULL),
  mp_write_buffer(IRS_NULL),
  mp_user_write_buffer(IRS_NULL),
  m_buffer_size(a_buffer_size),
  m_buffer_top(m_buffer_size - 1),
  m_current_byte(0),
  m_rb_channel_id(0),
  m_rb_read_size(0),
  m_rb_packet_size(0),
  m_spi_stc_interrupt(this),
  m_pcint0_interrupt(this),
  //  флаги
  m_rb_new_data(false),
  m_wb_new_data(false),
  m_can_write(false)
{
  mp_read_buffer = new irs_u8[m_buffer_size];
  mp_user_read_buffer = new irs_u8[m_buffer_size];
  mp_write_buffer = new irs_u8[m_buffer_size];
  mp_user_write_buffer = new irs_u8[m_buffer_size];
  memset(mp_read_buffer, 0, m_buffer_size);
  memset(mp_write_buffer, 0, m_buffer_size);
  memset(mp_user_read_buffer, 0, m_buffer_size);
  memset(mp_user_write_buffer, 0, m_buffer_size);
  //  инициализация железа
  //  I/O Ports initialization for SPI
  DDRB_DDB3 = 1;    //  PB3 = MISO -> out
  PORTB_PORTB3 = 0; //  MISO = 1
  DDRB_DDB0 = 0;    //  PB0 = SPI SS-pin -> in
  PORTB_PORTB0 = 0; //  SS = no pull-up
  DDRB_DDB1 = 0;    //  PB1 = SCLK -> in
  PORTB_PORTB1 = 0; //  SCLK no pull-up
  DDRB_DDB2 = 0;    //  PB2 = MOSI -> in
  PORTB_PORTB2 = 0; //  MOSI no pull-up
  //  SPI hardware initialization
  SPCR_SPE = 1;     //SPI enable
  SPCR_DORD = 0;    //SPI MSB first
  SPCR_MSTR = 0;    //SPI slave mode
  SPCR_CPOL = 1;    //SPI clock falling edge first
  SPCR_CPHA = 0;    //SPI clock phase: data sample = falling edge
  SPCR_SPR1 = 1;    //SPI clock rate = Fosc/4 = 4 MHz
  SPCR_SPR0 = 1;    //-|-
  //
  PORTB_PORTB3 = 1; //  MISO = 1
}

irs::avr::spi_slave_flow_t::~spi_slave_flow_t()
{
  if (mp_read_buffer) delete []mp_read_buffer;
  if (mp_user_read_buffer) delete []mp_user_read_buffer;
  if (mp_write_buffer) delete []mp_write_buffer;
  if (mp_user_write_buffer) delete []mp_user_write_buffer;
  //  денициализация железа
  //  I/O Ports initialization for SPI
  DDRB_DDB3 = 0;    //  PB3 = MISO -> no out
  PORTB_PORTB3 = 0; //  MISO no pull-up
  DDRB_DDB0 = 0;    //  PB0 = SPI SS-pin -> in
  PORTB_PORTB0 = 0; //  SS no pull-up
  DDRB_DDB1 = 0;    //  PB1 = SCLK -> no out
  PORTB_PORTB1 = 0; //  SCLK no pull-up
  DDRB_DDB2 = 0;    //  PB2 = MOSI -> no out
  PORTB_PORTB2 = 0; //  MOSI no pull-up
  //  SPI hardware initialization
  SPCR_SPIE = 0;    //SPI interrupt disable
  SPCR_SPE = 0;     //SPI disable
  SPCR_DORD = 0;    //SPI MSB first
  SPCR_MSTR = 0;    //SPI slave mode
  SPCR_CPOL = 1;    //SPI clock falling edge first
  SPCR_CPHA = 0;    //SPI clock phase: data sample = falling edge
  SPCR_SPR1 = 0;    //SPI clock rate = Fosc/4 = 4 MHz
  SPCR_SPR0 = 0;    //-|-
}

irs::string irs::avr::spi_slave_flow_t::param(const irs::string &a_name)
{
  irs::string answer = "0";
  if (a_name == "buffer_size") {answer = int(m_buffer_size); return answer;}
  return answer;
}

void irs::avr::spi_slave_flow_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{
  return;
}

irs_uarc irs::avr::spi_slave_flow_t::read(irs_uarc &a_channel_id,
  irs_u8 *ap_buf, irs_uarc a_size)
{
  if (m_rb_new_data)
  {
    //irs_disable_interrupt();
    irs_u8 size = irs_u8(a_size);
    if (size > m_rb_packet_size - m_rb_read_size)
      size = m_rb_packet_size - m_rb_read_size;
    void *rdbuf = (void*)&mp_user_read_buffer[m_data_pos + m_rb_read_size];
    memcpy((void*)ap_buf, rdbuf, size);
    m_rb_read_size += size;
    if (m_rb_read_size >= m_rb_packet_size)
    {
      memset((void*)mp_user_read_buffer, 0, m_buffer_size);
      m_rb_new_data = false;
      m_rb_read_size = 0;
    }
    //irs_enable_interrupt();
    return size;
  }
  else return 0;
}

irs_uarc irs::avr::spi_slave_flow_t::write(irs_uarc a_channel_id,
  const irs_u8 *ap_buf, irs_uarc a_size)
{
  if (!m_wb_new_data)
  {
    //irs_disable_interrupt();
    m_wb_new_data = true;
    irs_u8 size = a_size;
    if (a_size > m_buffer_size - m_header_size)
      size = m_buffer_size - m_header_size;

    memset((void*)mp_user_write_buffer, 0, m_buffer_size);
    mp_user_write_buffer[m_size_pos] = size;
    mp_user_write_buffer[m_channel_id_pos] = irs_u8(a_channel_id);
    memcpy((void*)&mp_user_write_buffer[m_data_pos],(void*)ap_buf, size);
    irs_u8 crc = crc8(mp_user_write_buffer, 2, irs_u8(size + 2));
    mp_user_write_buffer[m_crc_pos] = crc;
    //irs_enable_interrupt();
    return size;
  }
  else return 0;
}

void irs::avr::spi_slave_flow_t::tick()
{
}

inline void irs::avr::spi_slave_flow_t::write_data_register(irs_u8 a_data)
{
  SPDR = a_data;
}

inline irs_u8 irs::avr::spi_slave_flow_t::read_data_register()
{
  return SPDR;
}

inline void irs::avr::spi_slave_flow_t::byte_interrupt()
{
  if (!m_current_byte)
  {
    //if (SPDR)
      m_can_write = true;
  }
  if (m_current_byte < m_buffer_size)
  {
    mp_read_buffer[m_current_byte++] = SPDR;
    SPDR = mp_write_buffer[m_current_byte];
  }
}

inline void irs::avr::spi_slave_flow_t::packet_interrupt()
{
  if (!m_rb_new_data)
  {
    if (mp_read_buffer[m_size_pos] > 0)
    {
      irs_u8 crc = mp_read_buffer[m_crc_pos];
      if (crc == crc8(mp_read_buffer, 2, mp_read_buffer[m_size_pos] + 2))
      {
        m_rb_channel_id = mp_read_buffer[m_channel_id_pos];
        m_rb_packet_size = mp_read_buffer[m_size_pos];
        memcpy((void*)mp_user_read_buffer, (void*)mp_read_buffer, m_buffer_size);
        m_rb_new_data = true;
      }
    }
    memset(mp_read_buffer, 0, m_buffer_size);
  }

  if (m_can_write)
  {
    mp_write_buffer[m_crc_pos] = 0;
    mp_write_buffer[m_size_pos] = 0;
    //memset(mp_write_buffer, m_buffer_size, 0);
    if (m_wb_new_data)
    {
      m_wb_new_data = false;
      memcpy((void*)mp_write_buffer, (void*)mp_user_write_buffer,
        m_buffer_size);
    }
  }
  if (!m_rb_new_data) mp_write_buffer[m_neg_pos] = 1;
  else mp_write_buffer[m_neg_pos] = 0;
  m_current_byte = 0;
  m_can_write = false;
  write_data_register(mp_write_buffer[m_neg_pos]);
}

irs::avr::spi_slave_flow_t::spi_stc_interrupt_t::spi_stc_interrupt_t
  (spi_slave_flow_t *ap_outer):
    mp_outer(ap_outer)
{
  SPCR_SPIE = 1;
  irs_avr_spi_int.add(this);
}

void irs::avr::spi_slave_flow_t::spi_stc_interrupt_t::exec()
{
  mp_outer->byte_interrupt();
  irs_enable_interrupt();
}

irs::avr::spi_slave_flow_t::pcint0_interrupt_t::pcint0_interrupt_t
  (spi_slave_flow_t *ap_outer):
    mp_outer(ap_outer)
{
  PCICR_PCIE0 = 1;
  PCMSK0_PCINT0 = 1;
  irs_avr_pcint0_int.add(this);
}

void irs::avr::spi_slave_flow_t::pcint0_interrupt_t::exec()
{
  __delay_cycles(10);
  if (PINB_PINB0) mp_outer->packet_interrupt();
}

#endif //__ATmega128__
