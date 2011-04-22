//! \file
//! \ingroup drivers_group
//! \brief Драйвер SPI для ARM
//!
//! Дата: 16.03.2011
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
  spi_type_t a_spi_type, ssi_type_t a_ssi_type
):
  m_status(SPI_FREE),
  mp_buf(a_buffer_size),
  mp_rw_buf(a_buffer_size),
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
  m_spi_type(a_spi_type),
  m_ssi_type(a_ssi_type)
{
  switch (m_ssi_type) {
    case SSI0:
    {
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
      GPIOAAFSEL_bit.no4 = 1;
      GPIOAAFSEL_bit.no5 = 1;
      
      //(*((volatile irs_u32*)(PORTA_BASE + GPIO_PCTL))) |= 
        //(PORTA_SSI0Clk|PORTA_SSI0Rx|PORTA_SSI0Tx);
      GPIOAPCTL_bit.PMC2 = SSI0Clk;
      GPIOAPCTL_bit.PMC4 = SSI0Rx;
      GPIOAPCTL_bit.PMC5 = SSI0Tx;
        
      SSI0CR1 = 0x0;
      SSI0CPSR = 0x2;
       
      init_default();
    } break;
    case SSI1:
    {
      RCGC1_bit.SSI1 = 1;
      RCGC2_bit.PORTH = 1;
      for (irs_u8 i = 10; i; i--);
      
      GPIOHDEN_bit.no4 = 1;
      GPIOHDIR_bit.no4 = 1;
      GPIOHAFSEL_bit.no4 = 1;
      GPIOHDEN_bit.no6 = 1;
      GPIOHDIR_bit.no6 = 0;
      GPIOHAFSEL_bit.no6 = 1;
      GPIOHDEN_bit.no7 = 1;
      GPIOHDIR_bit.no7 = 1;
      GPIOHAFSEL_bit.no7 = 1;
      
      GPIOHPCTL_bit.PMC4 = SSI1Clk;
      GPIOHPCTL_bit.PMC6 = SSI1Rx;
      GPIOHPCTL_bit.PMC7 = SSI1Tx;
      
      SSI1CR1 = 0x0;
      SSI1CPSR = 0x2;
      
      init_default();
    } break;
  }
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
      switch (m_ssi_type) {
        case SSI0:
        {
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
        } break;
        case SSI1:
        {
          switch (m_spi_type)
          {
            case SPI:
            {
              SSI1CR1_bit.SSE = 0;
              SSI1CR0_bit.SCR = (m_f_osc/(SSI1CPSR*a_bitrate)) - 1;
              SSI1CR1_bit.SSE = 1;
            } break;
            case TISS:
            {
            } break;
            case MICROWIRE:
            {
            } break;
          }
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
    switch (m_ssi_type)
    {
      case SSI0:
      {
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
      } break;
      case SSI1:
      {
        switch (m_spi_type)
        {
          case SPI:
          {
            SSI1CR0_bit.SPO = a_polarity;
          } break;
          case TISS:
          {
          } break;
          case MICROWIRE:
          {
          } break;
        }
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
    switch (m_ssi_type)
    {
      case SSI0:
      {
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
      } break;
      case SSI1:
      {
        switch (m_spi_type)
        {
          case SPI:
          {
            SSI1CR0_bit.SPH = a_phase;
          } break;
          case TISS:
          {
          } break;
          case MICROWIRE:
          {
          } break;
        }
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

bool irs::arm::arm_spi_t::set_data_size(irs_u16 a_data_size)
{
  if ((a_data_size > 16) || (a_data_size < 4)) {
    return false;
  }
  if (m_status == SPI_FREE) {
    switch (m_ssi_type)
    {
      case SSI0:
      {
        switch (m_spi_type)
        {
          case SPI:
          {
            SSI0CR0_bit.DSS = a_data_size - 1;
          } break;
          case TISS:
          {
          } break;
          case MICROWIRE:
          {
          } break;
        }
      } break;
      case SSI1:
      {
        switch (m_spi_type)
        {
          case SPI:
          {
            SSI1CR0_bit.DSS = a_data_size - 1;
          } break;
          case TISS:
          {
          } break;
          case MICROWIRE:
          {
          } break;
        }
      } break;
    }
    return true;
  } else {
    return false;
  }
}

void irs::arm::arm_spi_t::write(const irs_u8* ap_buf, irs_uarc a_size)
{
  if (mp_buf.size()) {
    m_packet_size = irs_u8(a_size);
    if (m_packet_size > m_buf_size) {
      m_packet_size = m_buf_size;
    }
    memcpyex(mp_buf.data(), ap_buf, m_packet_size);
    m_cur_byte = 0;
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
  init_default();
}

bool irs::arm::arm_spi_t::get_lock()
{
  return m_lock;
}

void irs::arm::arm_spi_t::write_data_register(irs_u8 a_data)
{
  switch (m_ssi_type) {
    case SSI0:
    {
      switch (m_spi_type) {
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
    } break;
    case SSI1:
    {
      switch (m_spi_type) {
        case SPI:
        {
          SSI1DR = a_data;
        } break;
        case TISS:
        {
        } break;
        case MICROWIRE:
        {
        } break;
      }
    } break;
  }
}

irs_u8 irs::arm::arm_spi_t::read_data_register()
{
  switch (m_ssi_type) {
    case SSI0:
    {
      switch (m_spi_type) {
        case SPI:
        {
          return SSI0DR;
        };
        case TISS:
        {
          return 0;
        };
        case MICROWIRE:
        {
          return 0;
        };
      }
    } break;
    case SSI1:
    {
      switch (m_spi_type) {
        case SPI:
        {
          return SSI1DR;
        };
        case TISS:
        {
          return 0;
        };
        case MICROWIRE:
        {
          return 0;
        };
      }
    } break;
  }
  return 0;
}

bool irs::arm::arm_spi_t::transfer_complete()
{
  switch (m_ssi_type) {
    case SSI0:
    {
      switch (m_spi_type) {
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
    } break;
    case SSI1:
    {
      switch (m_spi_type) {
        case SPI:
        {
          return !SSI1SR_bit.BSY;
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
    } break;
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
        switch (m_ssi_type) {
          case SSI0:
          {
            if (SSI0SR_bit.TNF) {
              if (m_cur_byte >= m_packet_size) {
                read_data_register();
                memsetex(mp_buf.data(), mp_buf.size());
                m_status = SPI_FREE;
              } else {
                read_data_register();
                write_data_register(mp_buf[m_cur_byte]);
                m_cur_byte++;
              }
            }
          } break;
          case SSI1:
          {
            if (SSI1SR_bit.TNF) {
              if (m_cur_byte >= m_packet_size) {
                read_data_register();
                memsetex(mp_buf.data(), mp_buf.size());
                m_status = SPI_FREE;
              } else {
                read_data_register();
                write_data_register(mp_buf[m_cur_byte]);
                m_cur_byte++;
              }
            }
          } break;
        }
      }
    } break;
    case SPI_READ:
    {
      if (transfer_complete()) {
        switch (m_ssi_type) {
          case SSI0:
          {
            if(SSI0SR_bit.RNE) {
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
          case SSI1:
          {
            if(SSI1SR_bit.RNE) {
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
    } break;
    case SPI_RW_WRITE:
    {
      if (transfer_complete()) {
        switch (m_ssi_type) {
          case SSI0:
          {
            if (SSI0SR_bit.TNF) {
              if (m_cur_byte > (m_packet_size - 1)) {
                memcpyex(mp_target_buf, mp_rw_buf.data(), m_packet_size);
                memsetex(mp_buf.data(), mp_buf.size());
                m_status = SPI_FREE;
              } else {
                write_data_register(mp_buf[m_cur_byte]);
                m_status = SPI_RW_READ;
              }
            }
          } break;
          case SSI1:
          {
            if (SSI1SR_bit.TNF) {
              if (m_cur_byte > (m_packet_size - 1)) {
                memcpyex(mp_target_buf, mp_rw_buf.data(), m_packet_size);
                memsetex(mp_buf.data(), mp_buf.size());
                m_status = SPI_FREE;
              } else {
                write_data_register(mp_buf[m_cur_byte]);
                m_status = SPI_RW_READ;
              }
            }
          } break;
        }
      }
    } break;
    case SPI_RW_READ:
    {
      if (transfer_complete()) {
        switch (m_ssi_type) {
          case SSI0:
          {
            if(SSI0SR_bit.RNE) {
              mp_rw_buf[m_cur_byte] = read_data_register();
              m_cur_byte++;
              m_status = SPI_RW_WRITE;
            }
          } break;
          case SSI1:
          {
            if(SSI1SR_bit.RNE) {
              mp_rw_buf[m_cur_byte] = read_data_register();
              m_cur_byte++;
              m_status = SPI_RW_WRITE;
            }
          } break;
        }
      }
    } break;
  }
}

void irs::arm::arm_spi_t::read_write(irs_u8 *ap_read_buf, 
  const irs_u8 *ap_write_buf, irs_uarc a_size)
{
  if (mp_buf.size() && mp_rw_buf.size()) {
    m_packet_size = irs_u8(a_size);
    if (m_packet_size > m_buf_size) {
      m_packet_size = m_buf_size;
    }
    mp_target_buf = ap_read_buf;
    memcpyex(mp_buf.data(), ap_write_buf, m_packet_size);    
  
    m_cur_byte = 0;
    m_status = SPI_RW_WRITE;
  }
}

void irs::arm::arm_spi_t::init_default()
{
  switch (m_ssi_type) {
    case SSI0:
    {
      SSI0CR1_bit.SSE = 0;

      SSI0CR0_bit.SCR = (m_f_osc/(SSI0CPSR*m_bitrate)) - 1;
      SSI0CR0_bit.SPH = m_phase;
      SSI0CR0_bit.SPO = m_polarity;
      SSI0CR0_bit.FRF = m_spi_type;
      SSI0CR0_bit.DSS = 0x7; // 8-bit data
      
      SSI0CR1_bit.SSE = 1;
    } break;
    case SSI1:
    {
      SSI1CR1_bit.SSE = 0;

      SSI1CR0_bit.SCR = (m_f_osc/(SSI1CPSR*m_bitrate)) - 1;
      SSI1CR0_bit.SPH = m_phase;
      SSI1CR0_bit.SPO = m_polarity;
      SSI1CR0_bit.FRF = m_spi_type;
      SSI1CR0_bit.DSS = 0x7; // 8-bit data
      
      SSI1CR1_bit.SSE = 1;
    } break;
  }
}

