//! \file
//! \ingroup drivers_group
//! \brief Драйвер SPI для ARM
//!
//! Дата: 16.03.2011
//! Дата создания: 30.11.2010

#include <irsdefs.h>

#include <armspi.h>
#include <armioregs.h>
#include <irscpu.h>

#include <irsfinal.h>

/**
SSIISR_bit.BSY // идет прием либо передача или буфер записи не пуст
SSIISR_bit.RFF // буфер на чтение заполнен
SSIISR_bit.RNE // появились данные для чтения
SSIISR_bit.TNF // буфер на передачу не заполнен
SSIISR_bit.TFE // буфер на передачу пуст
*/

irs::arm::arm_spi_t::arm_spi_t(
  irs_u32 a_bitrate,
  irs_u8 a_buffer_size,
  spi_type_t a_spi_type,
  ssi_type_t a_ssi_type,
  arm_port_t &a_clk_port,
  arm_port_t &a_rx_port,
  arm_port_t &a_tx_port
):
  m_status(SPI_FREE),
  mp_buf(a_buffer_size),
  mp_rw_buf(a_buffer_size),
  mp_target_buf(0),
  m_buf_size(a_buffer_size),
  m_cur_byte(0),
  m_packet_size(0),
  m_bitrate_def(a_bitrate),
  m_polarity_def(FALLING_EDGE),
  m_phase_def(LEAD_EDGE),
  m_order_def(MSB),
  m_data_size_def(8),
  m_buf_empty(true),
  m_lock(false),
  m_spi_type(a_spi_type),
  m_ssi_type(a_ssi_type),
  m_reg(m_ssi_type)
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

      GPIOAPCTL_bit.PMC2 = SSI0Clk;
      GPIOAPCTL_bit.PMC4 = SSI0Rx;
      GPIOAPCTL_bit.PMC5 = SSI0Tx;
    } break;
    case SSI1:
    {
      RCGC1_bit.SSI1 = 1;
      switch(reinterpret_cast<irs_u32>(&a_clk_port))
      {
        case PORTE_BASE:
        {
          RCGC2_bit.PORTE = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOEDEN_bit.no0 = 1;
          GPIOEDIR_bit.no0 = 1;
          GPIOEAFSEL_bit.no0 = 1;
          GPIOEPCTL_bit.PMC0 = SSI1Clk_E;
        } break;
        case PORTF_BASE:
        {
          RCGC2_bit.PORTF = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOFDEN_bit.no2 = 1;
          GPIOFDIR_bit.no2 = 1;
          GPIOFAFSEL_bit.no2 = 1;
          GPIOFPCTL_bit.PMC2 = SSI1Clk_F;
        } break;
        case PORTH_BASE:
        {
          RCGC2_bit.PORTH = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOHDEN_bit.no4 = 1;
          GPIOHDIR_bit.no4 = 1;
          GPIOHAFSEL_bit.no4 = 1;
          GPIOHPCTL_bit.PMC4 = SSI1Clk_H;
        } break;
        default:
        {
          IRS_LIB_ERROR(ec_standard,
            "Неверно указан порт при инициализации SPI1");
        } break;
      }
      switch(reinterpret_cast<irs_u32>(&a_rx_port))
      {
        case PORTE_BASE:
        {
          RCGC2_bit.PORTE = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOEDEN_bit.no2 = 1;
          GPIOEDIR_bit.no2 = 0;
          GPIOEAFSEL_bit.no2 = 1;
          GPIOEPCTL_bit.PMC2 = SSI1Rx_E;
        } break;
        case PORTF_BASE:
        {
          RCGC2_bit.PORTF = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOFDEN_bit.no4 = 1;
          GPIOFDIR_bit.no4 = 0;
          GPIOFAFSEL_bit.no4 = 1;
          GPIOFPCTL_bit.PMC4 = SSI1Rx_F;
        } break;
        case PORTH_BASE:
        {
          RCGC2_bit.PORTH = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOHDEN_bit.no6 = 1;
          GPIOHDIR_bit.no6 = 0;
          GPIOHAFSEL_bit.no6 = 1;
          GPIOHPCTL_bit.PMC6 = SSI1Rx_H;
        } break;
        default:
        {
          IRS_LIB_ERROR(ec_standard,
            "Неверно указан порт при инициализации SPI1");
        } break;
      }
      switch(reinterpret_cast<irs_u32>(&a_tx_port))
      {
        case PORTE_BASE:
        {
          RCGC2_bit.PORTE = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOEDEN_bit.no3 = 1;
          GPIOEDIR_bit.no3 = 1;
          GPIOEAFSEL_bit.no3 = 1;
          GPIOEPCTL_bit.PMC3 = SSI1Tx_E;
        } break;
        case PORTF_BASE:
        {
          RCGC2_bit.PORTF = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOFDEN_bit.no5 = 1;
          GPIOFDIR_bit.no5 = 1;
          GPIOFAFSEL_bit.no5 = 1;
          GPIOFPCTL_bit.PMC5 = SSI1Tx_F;
        } break;
        case PORTH_BASE:
        {
          RCGC2_bit.PORTH = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOHDEN_bit.no7 = 1;
          GPIOHDIR_bit.no7 = 1;
          GPIOHAFSEL_bit.no7 = 1;
          GPIOHPCTL_bit.PMC7 = SSI1Tx_H;
        } break;
        default:
        {
          IRS_LIB_ERROR(ec_standard,
            "Неверно указан порт при инициализации SPI1");
        } break;
      }
    } break;
  }
  
  init_default();
  
  // Эта строки обязательно должны стоять после init_default
  m_reg.mp_SSICR1_bit->SSE = 0;

  m_reg.mp_SSICR1_bit->LBM = 0;
  m_reg.mp_SSICR1_bit->MS  = 0;
  m_reg.mp_SSICR1_bit->SOD = 0;
  m_reg.mp_SSICR1_bit->EOT = 0;
  
  m_reg.mp_SSICR0_bit->FRF = m_spi_type;
  
  m_reg.mp_SSICR1_bit->SSE = 1;
}

irs::arm::arm_spi_t::reg_t::reg_t(ssi_type_t a_ssi_type):
  mp_SSICR0_bit(IRS_NULL),
  mp_SSICR1_bit(IRS_NULL),
  mp_SSICPSR(IRS_NULL)
{
  switch (a_ssi_type) {
    case SSI0: {
      mp_SSICR0_bit = &SSI0CR0_bit;
      mp_SSICR1_bit = &SSI0CR1_bit;
      mp_SSICPSR = &SSI0CPSR;
    } break;
    case SSI1: {
      mp_SSICR0_bit = &SSI1CR0_bit;
      mp_SSICR1_bit = &SSI1CR1_bit;
      mp_SSICPSR = &SSI1CPSR;
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
    switch (m_spi_type) {
      case SPI: {
        m_reg.mp_SSICR1_bit->SSE = 0;
        
        static const irs_u8 SSICPSR_max = 254;
        const irs_u32 cpu_frequency = cpu_traits_t::frequency();
        
        irs_u32 SSICPSR_prior = 
          2*(cpu_frequency/(2*a_bitrate*(1 + IRS_U8_MAX)) + 1);
        if (SSICPSR_prior <= SSICPSR_max) {
          (*m_reg.mp_SSICPSR) = SSICPSR_prior;
          irs_u32 SCR_prior = (cpu_frequency/(a_bitrate*SSICPSR_prior));
          if (SCR_prior > 0) {
            SCR_prior--;
          }
          m_reg.mp_SSICR0_bit->SCR = SCR_prior;
        } else {
          (*m_reg.mp_SSICPSR) = SSICPSR_max;
          m_reg.mp_SSICR0_bit->SCR = IRS_U8_MAX;
        }
        
        m_reg.mp_SSICR1_bit->SSE = 1;
      } break;
      case TISS: {
      } break;
      case MICROWIRE: {
      } break;
    }
    return true;
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
  return true; // MSB
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
  set_bitrate(m_bitrate_def);
    
  m_reg.mp_SSICR1_bit->SSE = 0;
  
  m_reg.mp_SSICR0_bit->SPH = m_phase_def;
  m_reg.mp_SSICR0_bit->SPO = m_polarity_def;
  m_reg.mp_SSICR0_bit->DSS = m_data_size_def - 1; // 8-bit data
  
  m_reg.mp_SSICR1_bit->SSE = 1;
}

