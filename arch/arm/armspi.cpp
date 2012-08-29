//! \file
//! \ingroup drivers_group
//! \brief Драйвер SPI для ARM
//!
//! Дата: 30.04.2012
//! Дата создания: 30.11.2010

#include <irsdefs.h>

#include <armspi.h>
#include <irscpu.h>
#include <irsconfig.h>
#include <irsdsp.h>
#include <armcfg.h>

#include <irsfinal.h>

/**
SSIISR_bit.BSY // идет прием либо передача или буфер записи не пуст
SSIISR_bit.RFF // буфер на чтение заполнен
SSIISR_bit.RNE // появились данные для чтения
SSIISR_bit.TNF // буфер на передачу не заполнен
SSIISR_bit.TFE // буфер на передачу пуст
*/

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)

irs::arm::arm_spi_t::arm_spi_t(
  irs_u32 a_bitrate,
  spi_type_t a_spi_type,
  ssi_type_t a_ssi_type,
  arm_port_t &a_clk_port,
  arm_port_t &a_rx_port,
  arm_port_t &a_tx_port
):
  m_status(SPI_FREE),
  mp_write_buf(IRS_NULL),
  mp_read_buf(IRS_NULL),
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
  m_reg(m_ssi_type),
  m_on_prev(false)
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
          GPIOEDR8R_bit.no0 = 1;
          GPIOEDIR_bit.no0 = 1;
          GPIOEAFSEL_bit.no0 = 1;
          GPIOEPCTL_bit.PMC0 = SSI1Clk_E;
        } break;
        case PORTF_BASE:
        {
          RCGC2_bit.PORTF = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOFDEN_bit.no2 = 1;
          GPIOFDR8R_bit.no2 = 1;
          GPIOFDIR_bit.no2 = 1;
          GPIOFAFSEL_bit.no2 = 1;
          GPIOFPCTL_bit.PMC2 = SSI1Clk_F;
        } break;
        case PORTH_BASE:
        {
          RCGC2_bit.PORTH = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOHDEN_bit.no4 = 1;
          GPIOHDR8R_bit.no4 = 1;
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
          GPIOEDR8R_bit.no3 = 1;
          GPIOEDIR_bit.no3 = 1;
          GPIOEAFSEL_bit.no3 = 1;
          GPIOEPCTL_bit.PMC3 = SSI1Tx_E;
        } break;
        case PORTF_BASE:
        {
          RCGC2_bit.PORTF = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOFDEN_bit.no5 = 1;
          GPIOFDR8R_bit.no5 = 1;
          GPIOFDIR_bit.no5 = 1;
          GPIOFAFSEL_bit.no5 = 1;
          GPIOFPCTL_bit.PMC5 = SSI1Tx_F;
        } break;
        case PORTH_BASE:
        {
          RCGC2_bit.PORTH = 1;
          for (irs_u8 i = 10; i; i--);
          GPIOHDEN_bit.no7 = 1;
          GPIOHDR8R_bit.no7 = 1;
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

  //init_default();

  // Эта строки обязательно должны стоять после init_default
  m_reg.mp_SSICR1_bit->SSE = 0;

  m_reg.mp_SSICR1_bit->LBM = 0;
  m_reg.mp_SSICR1_bit->MS  = 0;
  m_reg.mp_SSICR1_bit->SOD = 0;
  m_reg.mp_SSICR1_bit->EOT = 0;

  m_reg.mp_SSICR0_bit->FRF = m_spi_type;

  init_default();
  //m_reg.mp_SSICR1_bit->SSE = 1;
}

irs::arm::arm_spi_t::reg_t::reg_t(ssi_type_t a_ssi_type):
  mp_SSICR0_bit(IRS_NULL),
  mp_SSICR1_bit(IRS_NULL),
  mp_SSIDR(IRS_NULL),
  mp_SSISR_bit(IRS_NULL),
  mp_SSICPSR(IRS_NULL),
  mp_SSIIM_bit(IRS_NULL),
  mp_SSIRIS_bit(IRS_NULL),
  mp_SSIMIS_bit(IRS_NULL),
  mp_SSIICR_bit(IRS_NULL)
{
  switch (a_ssi_type) {
    case SSI0: {
      mp_SSICR0_bit = &SSI0CR0_bit;
      mp_SSICR1_bit = &SSI0CR1_bit;
      mp_SSIDR = &SSI0DR;
      mp_SSISR_bit = &SSI0SR_bit;
      mp_SSICPSR = &SSI0CPSR;
      mp_SSIIM_bit = &SSI0IM_bit;
      mp_SSIRIS_bit = &SSI0RIS_bit;
      mp_SSIMIS_bit = &SSI0MIS_bit;
      mp_SSIICR_bit = &SSI0ICR_bit;
    } break;
    case SSI1: {
      mp_SSICR0_bit = &SSI1CR0_bit;
      mp_SSICR1_bit = &SSI1CR1_bit;
      mp_SSIDR = &SSI1DR;
      mp_SSISR_bit = &SSI1SR_bit;
      mp_SSICPSR = &SSI1CPSR;
      mp_SSIIM_bit = &SSI1IM_bit;
      mp_SSIRIS_bit = &SSI1RIS_bit;
      mp_SSIMIS_bit = &SSI1MIS_bit;
      mp_SSIICR_bit = &SSI1ICR_bit;
    } break;
  }
}

irs::arm::arm_spi_t::~arm_spi_t()
{
}

void irs::arm::arm_spi_t::abort()
{
  IRS_DBG_RAW_MSG("SPI_ABORT" << endl);
  m_status = SPI_FREE;
  m_cur_byte = 0;
  m_packet_size = 0;
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
    switch (m_spi_type)
    {
      case SPI:
      {
        irs_u8 sse = m_reg.mp_SSICR1_bit->SSE;
        m_reg.mp_SSICR1_bit->SSE = 0;
        m_reg.mp_SSICR0_bit->SPO = a_polarity;
        m_reg.mp_SSICR1_bit->SSE = sse;
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
        irs_u8 sse = m_reg.mp_SSICR1_bit->SSE;
        m_reg.mp_SSICR1_bit->SSE = 0;
        m_reg.mp_SSICR0_bit->SPH = a_phase;
        m_reg.mp_SSICR1_bit->SSE = sse;
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
  return true; // MSB
}

bool irs::arm::arm_spi_t::set_data_size(irs_u16 a_data_size)
{
  if ((a_data_size > 16) || (a_data_size < 4)) {
    return false;
  }
  if (m_status == SPI_FREE) {
    switch (m_spi_type)
    {
      case SPI:
      {
        irs_u8 sse = m_reg.mp_SSICR1_bit->SSE;
        m_reg.mp_SSICR1_bit->SSE = 0;
        m_reg.mp_SSICR0_bit->DSS = a_data_size - 1;
        m_reg.mp_SSICR1_bit->SSE = sse;
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

void irs::arm::arm_spi_t::write(const irs_u8* ap_buf, irs_uarc a_size)
{
  if (ap_buf) {
    if (!m_lock) {
      IRS_DBG_RAW_MSG("Вызов write на разблокированном spi" << endl);
    }
    m_packet_size = a_size;
    mp_write_buf = ap_buf;
    m_cur_byte = 0;
    m_status = SPI_WRITE;
    if (m_ssi_type == SSI1) {
      if ((mp_write_buf[1] & (1 << 1)) == 0) {
        if ((m_on_prev) && ((mp_write_buf[1] & (1 << 2)) == 0)) {
          irs::time_remain_t time = irs::cur_time()->get_remain();
          const tm* date = gmtime(&time.time);
          irs::mlog() << setfill('0');
          irs::mlog() << setw(4) << (date->tm_year + 1900) << ".";
          irs::mlog() << setw(2) << (date->tm_mon + 1) << ".";
          irs::mlog() << setw(2) << date->tm_mday << ' ';
          irs::mlog() << setw(2) << date->tm_hour << ":";
          irs::mlog() << setw(2) << date->tm_min << ":";
          irs::mlog() << setw(2) << date->tm_sec << " ";
          irs::mlog() << time.remain*1000.;
          irs::mlog() << " бит on сброшен" << endl;
        }
        if ((mp_write_buf[1] & (1 << 2))) {
          m_on_prev = true;
        } else {
          m_on_prev = false;
        }
      }
    }
  }
}

void irs::arm::arm_spi_t::read(irs_u8* ap_buf, irs_uarc a_size)
{
  if (ap_buf) {
    if (!m_lock) {
      IRS_DBG_RAW_MSG("Вызов read на разблокированном spi" << endl);
    }
    m_packet_size = a_size;
    mp_read_buf = ap_buf;
    m_cur_byte = 0;
    read_data_register();
    write_data_register(0);
    m_status = SPI_READ;
  }
}

void irs::arm::arm_spi_t::lock()
{
  if (m_lock) {
    IRS_DBG_RAW_MSG("Вызов lock на уже заблокированном spi");
  }
  m_lock = true;
}

void irs::arm::arm_spi_t::unlock()
{
  if (!m_lock) {
    IRS_DBG_RAW_MSG("Вызов unlock на уже разблокированном spi" << endl);
  }
  m_lock = false;
  init_default();
}

bool irs::arm::arm_spi_t::get_lock()
{
  return m_lock;
}

void irs::arm::arm_spi_t::write_data_register(irs_u8 a_data)
{
  switch (m_spi_type) {
    case SPI:
    {
      (*m_reg.mp_SSIDR) = a_data;
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

  switch (m_spi_type) {
    case SPI:
    {
      return (*m_reg.mp_SSIDR);
    } break;
    case TISS:
    {
      return 0;
    } break;
    case MICROWIRE:
    {
      return 0;
    } break;
    default: {
      return 0;
    } break;
  }
}

bool irs::arm::arm_spi_t::transfer_complete()
{
 switch (m_spi_type) {
    case SPI:
    {
      return !(m_reg.mp_SSISR_bit->BSY);
    } break;
    case TISS:
    {
      return false;
    } break;
    case MICROWIRE:
    {
      return false;
    } break;
    default: {
      return false;
    } break;
  }
}

void irs::arm::arm_spi_t::tick()
{
  switch(m_status)
  {
    case SPI_WRITE:
    {
      if (transfer_complete()) {
        if (m_reg.mp_SSISR_bit->TNF) {
          if (m_cur_byte >= m_packet_size) {
            read_data_register();
            m_status = SPI_FREE;
          } else {
            read_data_register();
            write_data_register(mp_write_buf[m_cur_byte]);
            m_cur_byte++;
          }
        }
      }
    } break;
    case SPI_READ:
    {
      if (transfer_complete()) {
        if(m_reg.mp_SSISR_bit->RNE) {
          if (m_cur_byte >= (m_packet_size - 1)) {
            mp_read_buf[m_cur_byte] = read_data_register();
            m_status = SPI_FREE;
          } else {
            mp_read_buf[m_cur_byte] = read_data_register();
            write_data_register(0);
            m_cur_byte++;
          }
        }
      }
    } break;
    case SPI_RW_WRITE:
    {
      if (transfer_complete()) {
        if (m_reg.mp_SSISR_bit->TNF) {
          if (m_cur_byte > (m_packet_size - 1)) {
            m_status = SPI_FREE;
          } else {
            write_data_register(mp_write_buf[m_cur_byte]);
            m_status = SPI_RW_READ;
          }
        }
      }
    } break;
    case SPI_RW_READ:
    {
      if (transfer_complete()) {
        if(m_reg.mp_SSISR_bit->RNE) {
          mp_read_buf[m_cur_byte] = read_data_register();
          m_cur_byte++;
          m_status = SPI_RW_WRITE;
        }
      }
    } break;
  }
}

void irs::arm::arm_spi_t::read_write(irs_u8 *ap_read_buf,
  const irs_u8 *ap_write_buf, irs_uarc a_size)
{
  if (ap_read_buf && ap_write_buf) {
    if (!m_lock) {
      IRS_DBG_RAW_MSG("Вызов read_write на разблокированном spi" << endl);
    }
    m_packet_size = a_size;
    mp_read_buf = ap_read_buf;
    mp_write_buf = ap_write_buf;
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

#elif defined(__STM32F100RBT__) || defined(IRS_STM32F2xx)
irs::arm::arm_spi_t::arm_spi_t(
  size_t a_spi_address,
  irs_u32 a_bitrate,
  gpio_channel_t a_sck,
  gpio_channel_t a_miso,
  gpio_channel_t a_mosi
):
  m_lock(false),
  mp_spi_regs(reinterpret_cast<spi_regs_t*>(a_spi_address)),
  m_bitrate_default(a_bitrate),
  m_process(process_wait_command),
  mp_write_buf(IRS_NULL),
  mp_read_buf(IRS_NULL),
  m_packet_size(0),
  m_read_buf_index(0),
  m_data_item_byte_count(1)

{
  reset_peripheral(a_spi_address);
  initialize_gpio_channels(a_sck, a_miso, a_mosi);
  clock_enable(a_spi_address);

  mp_spi_regs->SPI_CR1_bit.SSM = 1;
  mp_spi_regs->SPI_CR1_bit.SSI = 1;
  // 1: Master configuration
  mp_spi_regs->SPI_CR1_bit.MSTR = 1;

  set_default();
  enable_spi();
}

void irs::arm::arm_spi_t::initialize_gpio_channels(gpio_channel_t a_sck,
  gpio_channel_t a_miso,
  gpio_channel_t a_mosi)
{
  set_moder_alternate_function(a_sck);
  set_moder_alternate_function(a_miso);
  set_moder_alternate_function(a_mosi);
  size_t spi_address = reinterpret_cast<size_t>(mp_spi_regs);
  switch (a_sck) {
    case PC10: {
      if (spi_address == SPI3_I2S3_BASE) {
        GPIOC_AFRH_bit.AFRH10 = 6;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
    }
  }
  switch (a_miso) {
    case PC11: {
      if (spi_address == SPI3_I2S3_BASE) {
        GPIOC_AFRH_bit.AFRH11 = 6;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
    }
  }
  switch (a_mosi) {
    case PC12: {
      if (spi_address == SPI3_I2S3_BASE) {
        GPIOC_AFRH_bit.AFRH12 = 6;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
    }
  }
}

void irs::arm::arm_spi_t::set_moder_alternate_function(gpio_channel_t a_channel)
{
  const size_t port_address = get_port_address(a_channel);
  const irs_u32 pin_index = get_pin_index(a_channel);
  clock_enable(port_address);
  const irs_u32 bits_count = 2;
  IRS_SET_BITS(port_address + GPIO_MODER_S, bits_count*pin_index,
    bits_count, GPIO_MODER_ALTERNATE_FUNCTION);
}

irs::arm::arm_spi_t::~arm_spi_t()
{
  disable_spi();
}

void irs::arm::arm_spi_t::set_default()
{
  set_bitrate(m_bitrate_default);
  set_polarity(FALLING_EDGE);
  set_phase(FIRST_EDGE);
  set_data_size(data_size_default);
  set_order(MSB);
}

void irs::arm::arm_spi_t::enable_spi()
{
  // 1: Peripheral enabled
  mp_spi_regs->SPI_CR1_bit.SPE = 1;
}

void irs::arm::arm_spi_t::disable_spi()
{
  while (mp_spi_regs->SPI_SR_bit.RXNE != 0);
  while (mp_spi_regs->SPI_SR_bit.TXE != 1);
  while (mp_spi_regs->SPI_SR_bit.BSY != 0);
  // 0: Peripheral disabled
  mp_spi_regs->SPI_CR1_bit.SPE = 0;
}

void irs::arm::arm_spi_t::abort()
{
  IRS_DBG_RAW_MSG("SPI_ABORT" << endl);
  m_process = process_wait_command;
}

irs::spi_t::status_t irs::arm::arm_spi_t::get_status()
{
  if (m_process == process_wait_command) {
    return FREE;
  } else {
    return BUSY;
  }
}

bool irs::arm::arm_spi_t::set_bitrate(irs_u32 a_bitrate)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  cpu_traits_t::frequency_type frequency = 0;
  size_t spi_address = reinterpret_cast<size_t>(mp_spi_regs);
  if (spi_address == SPI1_BASE) {
    frequency = cpu_traits_t::periphery_frequency_second();
  } else {
    frequency = cpu_traits_t::periphery_frequency_first();
  }
  irs_u32 divider = frequency/a_bitrate;
  int power = -1;
  while (divider > 0) {
    divider >>= 1;
    power++;
  }
  const int min_power = 0;
  const int max_power = 8;
  power = bound(power, min_power, max_power);
  mp_spi_regs->SPI_CR1_bit.BR = power;
  return true;
}

bool irs::arm::arm_spi_t::set_polarity(polarity_t a_polarity)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  if (a_polarity == NEGATIVE_POLARITY) {
    mp_spi_regs->SPI_CR1_bit.CPOL = 0;
  } else if (a_polarity == POSITIVE_POLARITY) {
    mp_spi_regs->SPI_CR1_bit.CPOL = 1;
  }
  return true;
}

bool irs::arm::arm_spi_t::set_phase(phase_t a_phase)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  mp_spi_regs->SPI_CR1_bit.CPHA = a_phase;
  return true;
}

bool irs::arm::arm_spi_t::set_order(order_t a_order)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  if (a_order == LSB) {
    mp_spi_regs->SPI_CR1_bit.LSBFIRST = 1;
  } else if (a_order == MSB) {
    mp_spi_regs->SPI_CR1_bit.LSBFIRST = 0;
  }
  return true;
}

bool irs::arm::arm_spi_t::set_data_size(irs_u16 a_data_size)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  if ((a_data_size != 8) && (a_data_size != 16)) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  disable_spi();
  if (a_data_size == 8) {
    mp_spi_regs->SPI_CR1_bit.DFF = 0;
    m_data_item_byte_count = 1;
  } else {
    mp_spi_regs->SPI_CR1_bit.DFF = 1;
    m_data_item_byte_count = 2;
  }
  enable_spi();
  return true;
}

void irs::arm::arm_spi_t::write(const irs_u8* ap_buf, irs_uarc a_size)
{
  read_write(IRS_NULL, ap_buf, a_size);
}

void irs::arm::arm_spi_t::read(irs_u8* ap_buf, irs_uarc a_size)
{
  read_write(ap_buf, IRS_NULL, a_size);
}

void irs::arm::arm_spi_t::lock()
{
  m_lock = true;
}

void irs::arm::arm_spi_t::unlock()
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя разблокировать в процессе "
      "передачи данных");
  }
  m_lock = false;
}

bool irs::arm::arm_spi_t::get_lock()
{
  return m_lock;
}

void irs::arm::arm_spi_t::tick()
{
  if (m_process == process_read_write) {
    if ((m_write_buf_index <= (m_read_buf_index + 1)) &&
      (m_write_buf_index < m_packet_size)) {
      if (mp_spi_regs->SPI_SR_bit.TXE == 1) {
        if (mp_write_buf != IRS_NULL) {
          mp_spi_regs->SPI_DR =
            mp_write_buf[m_write_buf_index*m_data_item_byte_count];
        } else {
          mp_spi_regs->SPI_DR = 0;
        }
        m_write_buf_index++;
      }
    }
    if (((m_read_buf_index + 2) == m_write_buf_index) ||
        (m_write_buf_index == m_packet_size)) {
      if (mp_spi_regs->SPI_SR_bit.RXNE == 1) {
        if (mp_read_buf) {
          mp_read_buf[m_read_buf_index*m_data_item_byte_count] =
            mp_spi_regs->SPI_DR;
        }
        m_read_buf_index++;
        if (m_read_buf_index >= m_packet_size) {
          m_process = process_wait_command;
        }
      }
    }
  }
}

void irs::arm::arm_spi_t::read_write(irs_u8 *ap_read_buf,
  const irs_u8 *ap_write_buf, irs_uarc a_size)
{
  if (!ap_read_buf && !ap_write_buf) {
    IRS_LIB_ERROR(ec_standard, "ap_read_buf и ap_write_buf "
      "не могут быть одновременно равны IRS_NULL");
  }
  if (m_data_item_byte_count == 2) {
    if ((a_size % 2) != 0) {
      IRS_LIB_ERROR(ec_standard, "Количество байт должно быть четным");
    }
  }
  if (!m_lock) {
    IRS_LIB_ERROR(ec_standard, "Вызов read_write на разблокированном spi");
  }

  m_packet_size = a_size/m_data_item_byte_count;
  mp_read_buf = ap_read_buf;
  mp_write_buf = ap_write_buf;
  m_write_buf_index = 0;
  m_read_buf_index = 0;
  m_process = process_read_write;
}

#else
  #error Тип контроллера не определён
#endif  //  mcu type
