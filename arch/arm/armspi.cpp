//! \file
//! \ingroup drivers_group
//! \brief Драйвер SPI для ARM
//!
//! Дата: 30.11.2010
//! Дата создания: 30.11.2010

#include <irsdefs.h>

#include <armspi.h>
#include <armioregs.h>

#include <irsfinal.h>

#ifdef NOP

SSIISR_bit.BSY // идет прием либо передача или буфер записи не пуст
SSIISR_bit.RFF // буфер на чтение заполнен
SSIISR_bit.RNE // появились данные для чтения
SSIISR_bit.TNF // буфер на передачу не заполнен
SSIISR_bit.TFE // буфер на передачу пуст

#endif // NOP

irs::arm::arm_spi_t::arm_spi_t(irs_u8 a_buffer_size, irs_u32 a_f_osc,
  spi_type_t a_spi_type):
  m_status(SPI_FREE),
  mp_buf(a_buffer_size),
  mp_target_buf(0),
  m_buf_size(a_buffer_size),
  m_cur_byte(0),
  m_packet_size(0),
  m_bitrate(1000000),
  m_polarity(FALLING_EDGE),
  m_phase(LEAD_EDGE),
  m_order(MSB),
  m_buf_empty(true),
  m_lock(false),
  m_f_osc(a_f_osc),
  m_spi_type(a_spi_type)
{
  mlog() << "mp_buf.size() = " << mp_buf.size() << endl;
  RCGC1_bit.SSI0 = 1;
  RCGC2_bit.PORTA = 1;
  for (irs_u8 i = 10; i; i--);
  
  GPIOADEN_bit.no2 = 1;
  GPIOADIR_bit.no2 = 1;
  GPIOADEN_bit.no4 = 1;
  GPIOADIR_bit.no4 = 0;
  GPIOADEN_bit.no5 = 1;
  GPIOADIR_bit.no5 = 1;
  
  GPIOAAFSEL_bit.no2 = 1; 
  //GPIOAAFSEL_bit.no3 = 1;
  GPIOAAFSEL_bit.no4 = 1;
  GPIOAAFSEL_bit.no5 = 1;
  
  (*((volatile irs_u32*)(PORTA_BASE + GPIO_PCTL))) |= 
    (PORTA_SSI0Clk|/*PORTA_SSI0Fss|*/PORTA_SSI0Rx|PORTA_SSI0Tx);
    
  SSI0CR1_bit.SSE = 0;
  
  SSI0CR1 = 0x0;
  SSI0CPSR = 0x2;
  SSI0CR0_bit.SCR = (m_f_osc/(SSI0CPSR*m_bitrate)) - 1;
  SSI0CR0_bit.SPH = m_phase;
  SSI0CR0_bit.SPO = m_polarity;
  SSI0CR0_bit.FRF = m_spi_type;
  SSI0CR0_bit.DSS = 0x7; // 8-bit data
  
  SSI0CR1_bit.SSE = 1;
}

irs::arm::arm_spi_t::~arm_spi_t()
{
}

void irs::arm::arm_spi_t::abort()
{
  mlog() << "SPI_ABORT" << endl;
  m_status = SPI_FREE;
  m_cur_byte = 0;
  m_packet_size = 0;
  memsetex(mp_buf.data(), mp_buf.size());
}

irs::spi_t::status_t irs::arm::arm_spi_t::get_status()
{
  if (m_status == SPI_FREE) {
    return FREE;
  } else {
    return BUSY;
  }
}

bool irs::arm::arm_spi_t::set_bitrate(irs_u32 a_bitrate)
{
  if (m_status == SPI_FREE) {
    if (a_bitrate >= 2000000) {
      switch (m_spi_type)
      {
        case SPI:
        {
          SSI0CR1_bit.SSE = 0;
          SSI0CR0_bit.SCR = (m_f_osc/(SSI0CPSR*a_bitrate)) - 1;
          SSI0CR1_bit.SSE = 1;
        } break;
        case TISS:
        {
        } break;
        case MICROWIRE:
        {
        } break;
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool irs::arm::arm_spi_t::set_polarity(polarity_t a_polarity)
{
  if (m_status == SPI_FREE) {
    switch (m_spi_type)
    {
      case SPI:
      {
        SSI0CR0_bit.SPO = a_polarity;
      } break;
      case TISS:
      {
      } break;
      case MICROWIRE:
      {
      } break;
    }
    return true;
  } else {
    return false;
  }
}

bool irs::arm::arm_spi_t::set_phase(phase_t a_phase)
{
  if (m_status == SPI_FREE) {
    switch (m_spi_type)
    {
      case SPI:
      {
        SSI0CR0_bit.SPH = a_phase;
      } break;
      case TISS:
      {
      } break;
      case MICROWIRE:
      {
      } break;
    }
    return true;
  } else {
    return false;
  }
}

bool irs::arm::arm_spi_t::set_order(order_t /*a_order*/)
{
  return true;
}

void irs::arm::arm_spi_t::write(const irs_u8* ap_buf,irs_uarc a_size)
{
  if (mp_buf.size() && SSI0SR_bit.TNF) {
    m_packet_size = irs_u8(a_size);
    if (m_packet_size > m_buf_size) {
      m_packet_size = m_buf_size;
    }
    memcpyex(mp_buf.data(), ap_buf, m_packet_size);
    write_data_register(mp_buf[0]);
    m_cur_byte = 1;
    m_status = SPI_WRITE;
  }
}

void irs::arm::arm_spi_t::read(irs_u8* ap_buf, irs_uarc a_size)
{
  if (mp_buf.size()) {
    m_packet_size = irs_u8(a_size);
    if (m_packet_size > m_buf_size) {
      m_packet_size = m_buf_size;
    }
    mp_target_buf = ap_buf;
    m_cur_byte = 0;
    read_data_register();
    write_data_register(0);
    m_status = SPI_READ;
  }
}

void irs::arm::arm_spi_t::lock()
{
  m_lock = true;
}

void irs::arm::arm_spi_t::unlock()
{
  m_lock = false;
}

bool irs::arm::arm_spi_t::get_lock()
{
  return m_lock;
}

void irs::arm::arm_spi_t::write_data_register(irs_u8 a_data)
{
  switch (m_spi_type)
  {
    case SPI:
    {
      SSI0DR = a_data;
    } break;
    case TISS:
    {
    } break;
    case MICROWIRE:
    {
    } break;
  }
}

irs_u8 irs::arm::arm_spi_t::read_data_register()
{
  switch (m_spi_type)
  {
    case SPI:
    {
      return SSI0DR;
    }
    case TISS:
    {
      return 0;
    }
    case MICROWIRE:
    {
      return 0;
    }
  }
  return 0;
}

bool irs::arm::arm_spi_t::transfer_complete()
{
  switch (m_spi_type)
  {
    case SPI:
    {
      return !SSI0SR_bit.BSY;
    }
    case TISS:
    {
      return false;
    }
    case MICROWIRE:
    {
      return false;
    }
  }
  return false;
}

void irs::arm::arm_spi_t::tick()
{
  switch(m_status)
  {
    case SPI_WRITE:
    {
      if (transfer_complete()) {
        if (m_cur_byte >= m_packet_size) {
          memsetex(mp_buf.data(), mp_buf.size());
          m_status = SPI_FREE;
        } else {
          write_data_register(mp_buf[m_cur_byte]);
          m_cur_byte++;
        }
      }
    } break;
    case SPI_READ:
    {
      if (transfer_complete() && SSI0SR_bit.RNE) {
        if (m_cur_byte >= (m_packet_size - 1)) {
          mp_buf[m_cur_byte] = read_data_register();
          memcpy((void*)mp_target_buf, mp_buf.data(), m_packet_size);
          memsetex(mp_buf.data(), mp_buf.size());
          m_status = SPI_FREE;
        } else {
          mp_buf[m_cur_byte] = read_data_register();
          write_data_register(m_cur_byte + 1);
          m_cur_byte++;
        }
      }
    } break;
  }
}
