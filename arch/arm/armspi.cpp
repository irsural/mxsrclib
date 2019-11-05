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
  m_polarity_def(POSITIVE_POLARITY),
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
  //IRS_DBG_RAW_MSG("SPI_ABORT" << endl);
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
        if (a_polarity == NEGATIVE_POLARITY) {
          m_reg.mp_SSICR0_bit->SPO = 0;
        } else if (a_polarity == POSITIVE_POLARITY) {
          m_reg.mp_SSICR0_bit->SPO = 1;
        }
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
#elif defined(__STM32F100RBT__)
#elif defined(IRS_STM32_F2_F4_F7)

void irs::arm::st_spi_base_t::clean_buf_from_old_data(spi_regs_t*
#ifdef IRS_STM32F7xx
  ap_spi_regs
#endif // !IRS_STM32F7xx
)
{
  #ifdef IRS_STM32F7xx
  while (ap_spi_regs->SPI_SR_bit.FTLVL != 0);
  while (ap_spi_regs->SPI_SR_bit.BSY != 0);

  while (ap_spi_regs->SPI_SR_bit.FRLVL != 0) {
    volatile irs_u16 data = ap_spi_regs->SPI_DR;
  }
  #else // !IRS_STM32F7xx
  // Ничего не делаем
  #endif // !IRS_STM32F7xx
}

void irs::arm::st_spi_base_t::_disable_spi(spi_regs_t* ap_spi_regs)
{
  #ifdef IRS_STM32F7xx
  while (ap_spi_regs->SPI_SR_bit.FTLVL != 0);
  while (ap_spi_regs->SPI_SR_bit.BSY != 0);

  ap_spi_regs->SPI_CR1_bit.SPE = 0;

  while (ap_spi_regs->SPI_SR_bit.FRLVL != 0) {
    volatile irs_u16 data = ap_spi_regs->SPI_DR;
  }
  #elif defined(IRS_STM32F_2_AND_4)
  while (ap_spi_regs->SPI_SR_bit.RXNE != 0) {
    volatile irs_u32 data = ap_spi_regs->SPI_DR;
  }
  while (ap_spi_regs->SPI_SR_bit.TXE != 1);
  while (ap_spi_regs->SPI_SR_bit.BSY != 0);
  ap_spi_regs->SPI_CR1_bit.SPE = 0;
  #else // !defined(IRS_STM32F_2_AND_4)
  #error Тип контроллера не определён
  #endif // #ifdef IRS_STM32F7xx
}

bool irs::arm::st_spi_base_t::_set_bitrate(spi_regs_t* ap_spi_regs,
  irs_u32 a_bitrate)
{
  cpu_traits_t::frequency_type frequency = 0;
  size_t spi_address = reinterpret_cast<size_t>(ap_spi_regs);
  switch (spi_address) {
    case IRS_SPI1_BASE:
    case IRS_SPI4_BASE:
    case IRS_SPI5_BASE:
    case IRS_SPI6_BASE: {
      frequency = cpu_traits_t::periphery_frequency_second();
    } break;
    case IRS_SPI2_I2S2_BASE:
    case IRS_SPI3_I2S3_BASE: {
      frequency = cpu_traits_t::periphery_frequency_first();
    } break;
  }
  irs_u32 divider = frequency/a_bitrate;
  int power = -1;
  while (divider > 0) {
    divider >>= 1;
    power++;
  }
  const int min_power = 0;
  const int max_power = 7;
  power = bound(power, min_power, max_power);
  ap_spi_regs->SPI_CR1_bit.BR = power;
  return true;
}

bool irs::arm::st_spi_base_t::_set_polarity(spi_regs_t* ap_spi_regs,
  polarity_t a_polarity)
{
  irs_u8 spe = ap_spi_regs->SPI_CR1_bit.SPE;
  _disable_spi(ap_spi_regs);

  if (a_polarity == spi_t::NEGATIVE_POLARITY) {
    ap_spi_regs->SPI_CR1_bit.CPOL = 0;
  } else if (a_polarity == spi_t::POSITIVE_POLARITY) {
    ap_spi_regs->SPI_CR1_bit.CPOL = 1;
  }
  ap_spi_regs->SPI_CR1_bit.SPE = spe;
  return true;
}

bool irs::arm::st_spi_base_t::_set_phase(spi_regs_t* ap_spi_regs,
  phase_t a_phase)
{
  //irs_u8 spe = ap_spi_regs->SPI_CR1_bit.SPE;
  //ap_spi_regs->SPI_CR1_bit.SPE = 0;
  irs_u8 spe = ap_spi_regs->SPI_CR1_bit.SPE;
  _disable_spi(ap_spi_regs);

  ap_spi_regs->SPI_CR1_bit.CPHA = a_phase;
  ap_spi_regs->SPI_CR1_bit.SPE = spe;
  return true;
}

bool irs::arm::st_spi_base_t::_set_order(spi_regs_t* ap_spi_regs,
  order_t a_order)
{
  //irs_u8 spe = ap_spi_regs->SPI_CR1_bit.SPE;
  //ap_spi_regs->SPI_CR1_bit.SPE = 0;
  irs_u8 spe = ap_spi_regs->SPI_CR1_bit.SPE;
  _disable_spi(ap_spi_regs);

  if (a_order == spi_t::LSB) {
    ap_spi_regs->SPI_CR1_bit.LSBFIRST = 1;
  } else if (a_order == spi_t::MSB) {
    ap_spi_regs->SPI_CR1_bit.LSBFIRST = 0;
  }
  ap_spi_regs->SPI_CR1_bit.SPE = spe;
  return true;
}

void irs::arm::st_spi_base_t::initialize_gpio_channels(spi_regs_t* ap_spi_regs,
  gpio_channel_t a_sck,
  gpio_channel_t a_miso,
  gpio_channel_t a_mosi,
  int a_gpio_speed)
{
  set_moder_alternate_function(a_sck);
  set_moder_alternate_function(a_miso);
  set_moder_alternate_function(a_mosi);
  size_t spi_address = reinterpret_cast<size_t>(ap_spi_regs);
  if (spi_address == IRS_SPI1_BASE) {
    switch (a_sck) {
      case PA5: {
        GPIOA_AFRL_bit.AFRL5 = 5;
        GPIOA_OSPEEDR_bit.OSPEEDR5 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_miso) {
      case PA6: {
        GPIOA_AFRL_bit.AFRL6 = 5;
        GPIOA_OSPEEDR_bit.OSPEEDR6 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_mosi) {
      case PB5: {
        GPIOB_AFRL_bit.AFRL5 = 5;
        GPIOB_OSPEEDR_bit.OSPEEDR5 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
  } else if (spi_address == IRS_SPI2_I2S2_BASE) {
    switch (a_sck) {
      case PB10: {
        GPIOB_AFRH_bit.AFRH10 = 5;
        GPIOB_OSPEEDR_bit.OSPEEDR10 = a_gpio_speed;
      } break;
      case PB13: {
        GPIOB_AFRH_bit.AFRH13 = 5;
        GPIOB_OSPEEDR_bit.OSPEEDR13 = a_gpio_speed;
      } break;
      case PI1: {
        GPIOI_AFRL_bit.AFRL1 = 5;
        GPIOI_OSPEEDR_bit.OSPEEDR1 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_miso) {
      case PB14: {
        GPIOB_AFRH_bit.AFRH14 = 5;
        GPIOB_OSPEEDR_bit.OSPEEDR14 = a_gpio_speed;
      } break;
      case PC2: {
        GPIOC_AFRL_bit.AFRL2 = 5;
        GPIOC_OSPEEDR_bit.OSPEEDR2 = a_gpio_speed;
      } break;
      case PI2: {
        GPIOI_AFRL_bit.AFRL2 = 5;
        GPIOI_OSPEEDR_bit.OSPEEDR2 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_mosi) {
      case PB15: {
        GPIOB_AFRH_bit.AFRH15 = 5;
        GPIOB_OSPEEDR_bit.OSPEEDR15 = a_gpio_speed;
      } break;
      case PC3: {
        GPIOC_AFRL_bit.AFRL3 = 5;
        GPIOC_OSPEEDR_bit.OSPEEDR3 = a_gpio_speed;
      } break;
      case PI3: {
        GPIOI_AFRL_bit.AFRL3 = 5;
        GPIOI_OSPEEDR_bit.OSPEEDR3 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
  } else if (spi_address == IRS_SPI3_I2S3_BASE) {
    switch (a_sck) {
      case PC10: {
        GPIOC_AFRH_bit.AFRH10 = 6;
        GPIOC_OSPEEDR_bit.OSPEEDR10 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_miso) {
      case PC11: {
        GPIOC_AFRH_bit.AFRH11 = 6;
        GPIOC_OSPEEDR_bit.OSPEEDR11 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_mosi) {
      case PC12: {
        GPIOC_AFRH_bit.AFRH12 = 6;
        GPIOC_OSPEEDR_bit.OSPEEDR12 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
  } else if (spi_address == IRS_SPI6_BASE) {
    switch (a_sck) {
      case PG13: {
        GPIOG_AFRH_bit.AFRH13 = 5;
        GPIOG_OSPEEDR_bit.OSPEEDR13 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_miso) {
      case PG12: {
        GPIOG_AFRH_bit.AFRH12 = 5;
        GPIOG_OSPEEDR_bit.OSPEEDR12 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
    switch (a_mosi) {
      case PG14: {
        GPIOG_AFRH_bit.AFRH14 = 5;
        GPIOG_OSPEEDR_bit.OSPEEDR14 = a_gpio_speed;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация Порта и spi");
      }
    }
  } else {
    IRS_LIB_ASSERT_MSG("Недопустимый адрес SPI");
  }
}

void irs::arm::st_spi_base_t::set_moder_alternate_function(
  gpio_channel_t a_channel)
{
  const size_t port_address = get_port_address(a_channel);
  const irs_u32 pin_index = get_pin_index(a_channel);
  clock_enable(port_address);
  const irs_u32 bits_count = 2;
  IRS_SET_BITS(port_address + GPIO_MODER_S, bits_count*pin_index,
    bits_count, GPIO_MODER_ALTERNATE_FUNCTION);
}

#ifdef USE_STDPERIPH_DRIVER

#if (defined(IRS_STM32F4xx) || defined(IRS_STM32F4xx))

// class st_spi_dma_t
irs::arm::st_spi_dma_t::st_spi_dma_t(
  const settings_t& a_settings
):
  m_lock(false),
  m_settings(a_settings),
  mp_spi_regs(reinterpret_cast<spi_regs_t*>(a_settings.spi_address)),
  mp_dma(reinterpret_cast<dma_regs_t*>(a_settings.dma_address)),
  mp_rx_dma_y_stream_x(a_settings.rx_dma_y_stream_x),
  mp_tx_dma_y_stream_x(a_settings.tx_dma_y_stream_x),
  m_rx_buffer(a_settings.rx_buffer),
  m_tx_buffer(a_settings.tx_buffer),
  m_bitrate_default(a_settings.bitrate),
  m_process(process_wait_command),
  mp_write_buf(IRS_NULL),
  mp_read_buf(IRS_NULL),
  m_packet_size(0),
  m_index(0),
  m_data_item_byte_count(1),
  m_byte_count(0),
  m_max_byte_count(calc_max_byte_count())
{
  IRS_LIB_ASSERT(a_settings.spi_address);
  IRS_LIB_ASSERT(a_settings.dma_address);
  IRS_LIB_ASSERT(a_settings.bitrate > 0);
  IRS_LIB_ASSERT(a_settings.sck != PNONE);
  IRS_LIB_ASSERT(a_settings.miso != PNONE);
  IRS_LIB_ASSERT(a_settings.mosi != PNONE);

  IRS_LIB_ASSERT(a_settings.rx_buffer.data());
  IRS_LIB_ASSERT(a_settings.rx_buffer.size() > 0);
  IRS_LIB_ASSERT(IS_DMA_ALL_PERIPH(a_settings.rx_dma_y_stream_x));
  IRS_LIB_ASSERT(IS_DMA_CHANNEL(a_settings.rx_dma_channel));
  IRS_LIB_ASSERT(IS_DMA_PRIORITY(a_settings.rx_dma_priority));

  IRS_LIB_ASSERT(a_settings.tx_buffer.data());
  IRS_LIB_ASSERT(a_settings.tx_buffer.size() > 0);
  IRS_LIB_ASSERT(IS_DMA_ALL_PERIPH(a_settings.tx_dma_y_stream_x));
  IRS_LIB_ASSERT(IS_DMA_CHANNEL(a_settings.tx_dma_channel));
  IRS_LIB_ASSERT(IS_DMA_PRIORITY(a_settings.tx_dma_priority));

  reset_peripheral(a_settings.spi_address);
  initialize_gpio_channels(a_settings.sck, a_settings.miso, a_settings.mosi,
    a_settings.gpio_speed);

  clock_enable(a_settings.spi_address);

  mp_spi_regs->SPI_CR1_bit.SSM = 1;
  mp_spi_regs->SPI_CR1_bit.SSI = 1;
  // 1: Master configuration
  mp_spi_regs->SPI_CR1_bit.MSTR = 1;

  set_default();
  enable_spi();

  irs::clock_enable(a_settings.dma_address);

  reset_dma();
}

void irs::arm::st_spi_dma_t::reset_dma()
{
  DMA_DeInit(m_settings.tx_dma_y_stream_x); //For STM32F4
  while (DMA_GetCmdStatus(m_settings.tx_dma_y_stream_x) != DISABLE) {}
  DMA_DeInit(m_settings.rx_dma_y_stream_x); //For STM32F4
  while (DMA_GetCmdStatus(m_settings.rx_dma_y_stream_x) != DISABLE) {}

  // TX
  DMA_InitTypeDef DMA_Init_SPI_TX;
  DMA_Init_SPI_TX.DMA_Channel = m_settings.tx_dma_channel;
  DMA_Init_SPI_TX.DMA_PeripheralBaseAddr =
    reinterpret_cast<irs_u32>(&mp_spi_regs->SPI_DR);
  DMA_Init_SPI_TX.DMA_Memory0BaseAddr =
    reinterpret_cast<uint32_t>(m_tx_buffer.data());
  DMA_Init_SPI_TX.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_Init_SPI_TX.DMA_BufferSize = 2;
  DMA_Init_SPI_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_Init_SPI_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_Init_SPI_TX.DMA_PeripheralDataSize = (m_data_item_byte_count == 1) ?
    DMA_PeripheralDataSize_Byte : DMA_PeripheralDataSize_HalfWord;
  DMA_Init_SPI_TX.DMA_MemoryDataSize = (m_data_item_byte_count == 1) ?
    DMA_MemoryDataSize_Byte : DMA_MemoryDataSize_HalfWord;
  DMA_Init_SPI_TX.DMA_Mode = DMA_Mode_Normal;
  DMA_Init_SPI_TX.DMA_Priority = m_settings.tx_dma_priority;
  DMA_Init_SPI_TX.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_Init_SPI_TX.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_Init_SPI_TX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_Init_SPI_TX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(m_settings.tx_dma_y_stream_x, &DMA_Init_SPI_TX);

  SPI_I2S_DMACmd(reinterpret_cast<SPI_TypeDef*>(mp_spi_regs),
    SPI_I2S_DMAReq_Tx, ENABLE);

  // RX
  DMA_InitTypeDef DMA_Init_SPI_RX;
  DMA_Init_SPI_RX.DMA_Channel = m_settings.rx_dma_channel;
  DMA_Init_SPI_RX.DMA_PeripheralBaseAddr =
    reinterpret_cast<irs_u32>(&mp_spi_regs->SPI_DR);
  DMA_Init_SPI_RX.DMA_Memory0BaseAddr =
    reinterpret_cast<uint32_t>(m_rx_buffer.data());
  DMA_Init_SPI_RX.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_Init_SPI_RX.DMA_BufferSize = 2;
  DMA_Init_SPI_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_Init_SPI_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_Init_SPI_RX.DMA_PeripheralDataSize = (m_data_item_byte_count == 1) ?
    DMA_PeripheralDataSize_Byte : DMA_PeripheralDataSize_HalfWord;;
  DMA_Init_SPI_RX.DMA_MemoryDataSize = (m_data_item_byte_count == 1) ?
    DMA_MemoryDataSize_Byte : DMA_MemoryDataSize_HalfWord;
  DMA_Init_SPI_RX.DMA_Mode = DMA_Mode_Normal;
  DMA_Init_SPI_RX.DMA_Priority = m_settings.rx_dma_priority;
  DMA_Init_SPI_RX.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_Init_SPI_RX.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_Init_SPI_RX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_Init_SPI_RX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(m_settings.rx_dma_y_stream_x, &DMA_Init_SPI_RX);

  SPI_I2S_DMACmd(reinterpret_cast<SPI_TypeDef*>(mp_spi_regs),
    SPI_I2S_DMAReq_Rx, ENABLE);
}

void irs::arm::st_spi_dma_t::start_spi_dma()
{
  DMA_SetCurrDataCounter(mp_rx_dma_y_stream_x, m_byte_count);
  DMA_SetCurrDataCounter(mp_tx_dma_y_stream_x, m_byte_count);
  DMA_Cmd(mp_rx_dma_y_stream_x, ENABLE);
  DMA_Cmd(mp_tx_dma_y_stream_x, ENABLE);
}

void irs::arm::st_spi_dma_t::stop_spi_dma()
{
  DMA_Cmd(mp_rx_dma_y_stream_x, DISABLE);
  DMA_Cmd(mp_tx_dma_y_stream_x, DISABLE);
}

void irs::arm::st_spi_dma_t::initialize_gpio_channels(gpio_channel_t a_sck,
  gpio_channel_t a_miso,
  gpio_channel_t a_mosi,
  int a_gpio_speed)
{
  st_spi_base_t::initialize_gpio_channels(
    mp_spi_regs, a_sck, a_miso, a_mosi, a_gpio_speed);
}

irs::arm::st_spi_dma_t::~st_spi_dma_t()
{
  disable_spi();
}

void irs::arm::st_spi_dma_t::set_default()
{
  set_bitrate(m_bitrate_default);
  set_polarity(POSITIVE_POLARITY);
  set_phase(FIRST_EDGE);
  set_data_size(data_size_default);
  set_order(MSB);
}

void irs::arm::st_spi_dma_t::enable_spi()
{
  // 1: Peripheral enabled
  mp_spi_regs->SPI_CR1_bit.SPE = 1;
}

void irs::arm::st_spi_dma_t::disable_spi()
{
  st_spi_base_t::_disable_spi(mp_spi_regs);
  /*while (mp_spi_regs->SPI_SR_bit.RXNE != 0);
  while (mp_spi_regs->SPI_SR_bit.TXE != 1);
  while (mp_spi_regs->SPI_SR_bit.BSY != 0);
  // 0: Peripheral disabled
  mp_spi_regs->SPI_CR1_bit.SPE = 0;*/
}

void irs::arm::st_spi_dma_t::abort()
{
  //IRS_DBG_RAW_MSG("SPI_ABORT" << endl);
  stop_spi_dma();
  m_process = process_wait_command;
}

irs::spi_t::status_t irs::arm::st_spi_dma_t::get_status()
{
  if (m_process == process_wait_command) {
    return FREE;
  } else {
    return BUSY;
  }
}

bool irs::arm::st_spi_dma_t::set_bitrate(irs_u32 a_bitrate)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_bitrate(mp_spi_regs, a_bitrate);
}

bool irs::arm::st_spi_dma_t::set_polarity(polarity_t a_polarity)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_polarity(mp_spi_regs, a_polarity);
}

bool irs::arm::st_spi_dma_t::set_phase(phase_t a_phase)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_phase(mp_spi_regs, a_phase);
}

bool irs::arm::st_spi_dma_t::set_order(order_t a_order)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_order(mp_spi_regs, a_order);
}

bool irs::arm::st_spi_dma_t::set_data_size(irs_u16 a_data_size)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  if ((a_data_size != 8) && (a_data_size != 16)) {
    IRS_LIB_ERROR(ec_standard, "Размер данных может быть только 8 или 16");
  }

  #ifdef IRS_STM32F7xx
  irs_u8 spe = ap_spi_regs->SPI_CR1_bit.SPE;
  #endif //IRS_STM32F7xx
  disable_spi();
  if (a_data_size == 8) {
    mp_spi_regs->SPI_CR1_bit.DFF = 0;
    m_data_item_byte_count = 1;
  } else {
    mp_spi_regs->SPI_CR1_bit.DFF = 1;
    m_data_item_byte_count = 2;
  }
  m_max_byte_count = calc_max_byte_count();
  #ifdef IRS_STM32F7xx
  ap_spi_regs->SPI_CR1_bit.SPE = spe;
  #else //IRS_STM32F7xx
  enable_spi();
  #endif //IRS_STM32F7xx
  reset_dma();
  return true;
}

void irs::arm::st_spi_dma_t::write(const irs_u8* ap_buf, irs_uarc a_size)
{
  read_write(IRS_NULL, ap_buf, a_size);
}

void irs::arm::st_spi_dma_t::read(irs_u8* ap_buf, irs_uarc a_size)
{
  read_write(ap_buf, IRS_NULL, a_size);
}

void irs::arm::st_spi_dma_t::lock()
{
  m_lock = true;
}

void irs::arm::st_spi_dma_t::unlock()
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя разблокировать в процессе "
      "передачи данных");
  }
  m_lock = false;
}

bool irs::arm::st_spi_dma_t::get_lock()
{
  return m_lock;
}

void irs::arm::st_spi_dma_t::tick()
{
  if (m_process == process_read_write) {
    if ((DMA_GetFlagStatus(mp_tx_dma_y_stream_x, DMA_FLAG_TCIF4) == SET) &&
        (DMA_GetFlagStatus(mp_rx_dma_y_stream_x, DMA_FLAG_TCIF3) == SET)) {

      if (mp_read_buf) {
        memcpy(mp_read_buf, m_rx_buffer.data(), m_byte_count);
      }
      m_index += m_byte_count/m_data_item_byte_count;

      DMA_ClearFlag(mp_tx_dma_y_stream_x, DMA_FLAG_TCIF4);
      DMA_ClearFlag(mp_rx_dma_y_stream_x, DMA_FLAG_TCIF3);

      if (m_index < m_packet_size) {
        const size_type need_size = (m_packet_size - m_index)*
          m_data_item_byte_count;
        const size_type start_index = m_index/m_data_item_byte_count;
        if (mp_write_buf != IRS_NULL) {
          m_byte_count = min(m_max_byte_count, need_size);
          memcpy(m_tx_buffer.data(), mp_write_buf + start_index, m_byte_count);
        } else {
          memset(m_tx_buffer.data(), 0, m_byte_count);
        }
        start_spi_dma();
      } else {
        m_process = process_wait_command;
      }

      if (m_index >= m_packet_size) {
        m_process = process_wait_command;
      }
    }
  }
}

irs::arm::st_spi_dma_t::size_type
irs::arm::st_spi_dma_t::calc_max_byte_count() const
{
  size_type byte_count = 0;
  byte_count = min(m_rx_buffer.size(), m_tx_buffer.size());
  byte_count = min<size_type>(byte_count, 0xFFFF);
  // Берем четное количество, если необходимо
  byte_count = (byte_count/m_data_item_byte_count)*m_data_item_byte_count;

  return byte_count;
}

void irs::arm::st_spi_dma_t::read_write(irs_u8 *ap_read_buf,
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
  m_byte_count = min(m_max_byte_count, a_size);
  if (ap_write_buf) {
    memcpy(m_tx_buffer.data(), ap_write_buf, m_byte_count);
  } else {
    memset(m_tx_buffer.data(), 0, m_byte_count);
  }
  mp_read_buf = ap_read_buf;
  mp_write_buf = ap_write_buf;
  m_index = 0;

  m_process = process_read_write;
  start_spi_dma();
}
#elif defined(IRS_STM32F7xx)

#endif // defined(IRS_STM32F7xx)

#endif // USE_STDPERIPH_DRIVER

#ifdef USE_HAL_DRIVER

// class st_hal_spi_dma_t
irs::arm::st_hal_spi_dma_t::st_hal_spi_dma_t(
  const settings_t& a_settings
):
  m_lock(false),
  m_settings(a_settings),
  mp_spi_regs(reinterpret_cast<spi_regs_t*>(a_settings.spi_address)),
  mp_dma(reinterpret_cast<dma_regs_t*>(a_settings.dma_address)),

  m_hdma_tx(),
  m_hdma_rx(),
  m_tx_status(false),
  m_rx_status(false),
  m_hdma_init(false),

  m_rx_buffer(a_settings.rx_buffer),
  m_tx_buffer(a_settings.tx_buffer),
  m_bitrate_default(a_settings.bitrate),
  m_process(process_wait_command),
  mp_write_buf(IRS_NULL),
  mp_read_buf(IRS_NULL),
  m_packet_size(0),
  m_index(0),
  m_data_item_byte_count(1),
  m_byte_count(0),
  m_max_byte_count(calc_max_byte_count())
{
  IRS_LIB_ASSERT(a_settings.spi_address);
  IRS_LIB_ASSERT(a_settings.dma_address);
  IRS_LIB_ASSERT(a_settings.bitrate > 0);
  IRS_LIB_ASSERT(a_settings.sck != PNONE);
  IRS_LIB_ASSERT(a_settings.miso != PNONE);
  IRS_LIB_ASSERT(a_settings.mosi != PNONE);

  IRS_LIB_ASSERT(a_settings.rx_buffer.data());
  IRS_LIB_ASSERT(a_settings.rx_buffer.size() > 0);
  IRS_LIB_ASSERT(IS_DMA_STREAM_ALL_INSTANCE(a_settings.rx_dma_y_stream_x));
  IRS_LIB_ASSERT(IS_DMA_CHANNEL(a_settings.rx_dma_channel));
  IRS_LIB_ASSERT(IS_DMA_PRIORITY(a_settings.rx_dma_priority));

  IRS_LIB_ASSERT(a_settings.tx_buffer.data());
  IRS_LIB_ASSERT(a_settings.tx_buffer.size() > 0);
  IRS_LIB_ASSERT(IS_DMA_STREAM_ALL_INSTANCE(a_settings.tx_dma_y_stream_x));
  IRS_LIB_ASSERT(IS_DMA_CHANNEL(a_settings.tx_dma_channel));
  IRS_LIB_ASSERT(IS_DMA_PRIORITY(a_settings.tx_dma_priority));

  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // Ничего не делаем
  #elif IRS_STM32F7xx
  IRS_LIB_ASSERT(reinterpret_cast<size_t>(a_settings.rx_buffer.data())%32 == 0);
  IRS_LIB_ASSERT(a_settings.rx_buffer.size()%32 == 0);
  IRS_LIB_ASSERT(reinterpret_cast<size_t>(a_settings.tx_buffer.data())%32 == 0);
  IRS_LIB_ASSERT(a_settings.tx_buffer.size()%32 == 0);
  #else
  #error Тип контроллера не определён
  #endif // IRS_STM32F7xx

  // Временно, пока нет соотетствующих регистров!!!
  if (a_settings.spi_address == IRS_SPI6_BASE) {
    __HAL_RCC_SPI6_RELEASE_RESET();
  } else {
    reset_peripheral(a_settings.spi_address);
  }
  initialize_gpio_channels(a_settings.sck, a_settings.miso, a_settings.mosi,
    a_settings.gpio_speed);

  // Временно, пока нет соотетствующих регистров!!!
  if (a_settings.spi_address == IRS_SPI6_BASE) {
    __HAL_RCC_SPI6_CLK_ENABLE();
  } else {
    clock_enable(a_settings.spi_address);
  }

  clock_enable(a_settings.dma_address);

  mp_spi_regs->SPI_CR1_bit.SSM = 1;
  mp_spi_regs->SPI_CR1_bit.SSI = 1;
  // 1: Master configuration
  mp_spi_regs->SPI_CR1_bit.MSTR = 1;

  set_default();
  enable_spi();

  reset_dma();
}

void irs::arm::st_hal_spi_dma_t::reset_spi()
{
  abort();
  m_tx_status = false;
  m_rx_status = false;
  
  disable_spi();
  
  reset_peripheral(m_settings.spi_address);
  
  mp_spi_regs->SPI_CR1_bit.SSM = 1;
  mp_spi_regs->SPI_CR1_bit.SSI = 1;
  mp_spi_regs->SPI_CR1_bit.MSTR = 1;
  
  set_default();
  enable_spi();
}

void irs::arm::st_hal_spi_dma_t::reset_dma()
{
  if (m_hdma_init) {
    stop_spi_dma();
    HAL_DMA_DeInit(&m_hdma_tx);
    HAL_DMA_DeInit(&m_hdma_rx);
  }

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  m_hdma_tx.Instance                 = m_settings.tx_dma_y_stream_x;
  m_hdma_tx.Init.Channel             = m_settings.tx_dma_channel;
  m_hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  m_hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_1QUARTERFULL;

  m_hdma_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
  m_hdma_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  m_hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  m_hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  m_hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  if (m_data_item_byte_count == 1) {
    m_hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  } else {
    m_hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
  }
  m_hdma_tx.Init.Mode                = DMA_NORMAL;
  m_hdma_tx.Init.Priority            = m_settings.tx_dma_priority;

  HAL_DMA_Init(&m_hdma_tx);


  /* Configure the DMA handler for Reception process */
  m_hdma_rx.Instance                 = m_settings.rx_dma_y_stream_x;
  m_hdma_rx.Init.Channel             = m_settings.rx_dma_channel;
  m_hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  m_hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_1QUARTERFULL;

  m_hdma_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
  m_hdma_rx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  m_hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  m_hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  m_hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  if (m_data_item_byte_count == 1) {
    m_hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    m_hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  } else {
    m_hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    m_hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
  }
  m_hdma_rx.Init.Mode                = DMA_NORMAL;
  m_hdma_rx.Init.Priority            = m_settings.rx_dma_priority;

  HAL_DMA_Init(&m_hdma_rx);

  mp_spi_regs->SPI_CR2_bit.RXDMAEN = 1;
  mp_spi_regs->SPI_CR2_bit.TXDMAEN = 1;

  m_hdma_init = true;
}

void irs::arm::st_hal_spi_dma_t::start_spi_dma()
{
  if (mp_read_buf) {
    /* Enable the Rx DMA channel */
    HAL_DMA_Start(&m_hdma_rx, reinterpret_cast<irs_u32>(&mp_spi_regs->SPI_DR),
      (uint32_t)m_rx_buffer.data(), m_byte_count/m_data_item_byte_count);
  } else {
    m_rx_status = true;
  }
  cleand_dcache_tx();
  /* Enable the Tx DMA channel */
  HAL_DMA_Start(&m_hdma_tx, (uint32_t)m_tx_buffer.data(),
    reinterpret_cast<irs_u32>(&mp_spi_regs->SPI_DR),
    m_byte_count/m_data_item_byte_count);
}

void irs::arm::st_hal_spi_dma_t::cleand_dcache_tx()
{
  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // Ничего не делаем
  #elif IRS_STM32F7xx
  SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t*>(m_tx_buffer.data()),
    m_byte_count);
  #else
  #error Тип контроллера не определён
  #endif // IRS_STM32F7xx
}

void irs::arm::st_hal_spi_dma_t::stop_spi_dma()
{
  HAL_DMA_Abort(&m_hdma_rx);
  HAL_DMA_Abort(&m_hdma_tx);
}

void irs::arm::st_hal_spi_dma_t::initialize_gpio_channels(gpio_channel_t a_sck,
  gpio_channel_t a_miso,
  gpio_channel_t a_mosi,
  int a_gpio_speed)
{
  st_spi_base_t::initialize_gpio_channels(
    mp_spi_regs, a_sck, a_miso, a_mosi, a_gpio_speed);
}

irs::arm::st_hal_spi_dma_t::~st_hal_spi_dma_t()
{
  disable_spi();
}

void irs::arm::st_hal_spi_dma_t::set_default()
{
  set_bitrate(m_bitrate_default);
  set_polarity(POSITIVE_POLARITY);
  set_phase(FIRST_EDGE);
  set_data_size(data_size_default);
  set_order(MSB);
}

void irs::arm::st_hal_spi_dma_t::enable_spi()
{
  // 1: Peripheral enabled
  mp_spi_regs->SPI_CR1_bit.SPE = 1;
}

void irs::arm::st_hal_spi_dma_t::disable_spi()
{
  st_spi_base_t::_disable_spi(mp_spi_regs);
  /*while (mp_spi_regs->SPI_SR_bit.RXNE != 0) {
    volatile irs_u32 data = mp_spi_regs->SPI_DR;
  }
  while (mp_spi_regs->SPI_SR_bit.TXE != 1);
  while (mp_spi_regs->SPI_SR_bit.BSY != 0);
  // 0: Peripheral disabled
  mp_spi_regs->SPI_CR1_bit.SPE = 0;*/
}

void irs::arm::st_hal_spi_dma_t::abort()
{
  //IRS_DBG_RAW_MSG("SPI_ABORT" << endl);
  stop_spi_dma();
  m_process = process_wait_command;
}

irs::spi_t::status_t irs::arm::st_hal_spi_dma_t::get_status()
{
  if (m_process == process_wait_command) {
    return FREE;
  } else {
    return BUSY;
  }
}

bool irs::arm::st_hal_spi_dma_t::set_bitrate(irs_u32 a_bitrate)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_bitrate(mp_spi_regs, a_bitrate);
}

bool irs::arm::st_hal_spi_dma_t::set_polarity(polarity_t a_polarity)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_polarity(mp_spi_regs, a_polarity);
}

bool irs::arm::st_hal_spi_dma_t::set_phase(phase_t a_phase)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_phase(mp_spi_regs, a_phase);
}

bool irs::arm::st_hal_spi_dma_t::set_order(order_t a_order)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_order(mp_spi_regs, a_order);
}

bool irs::arm::st_hal_spi_dma_t::set_data_size(irs_u16 a_data_size)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  if ((a_data_size != 8) && (a_data_size != 16)) {
    IRS_LIB_ERROR(ec_standard, "Размер данных может быть только 8 или 16");
  }
  stop_spi_dma();

  irs_u8 spe = mp_spi_regs->SPI_CR1_bit.SPE;
  disable_spi();

  if (a_data_size == 8) {
    #ifdef IRS_STM32F7xx
    mp_spi_regs->SPI_CR2_bit.DS = 7;
    mp_spi_regs->SPI_CR2_bit.FRXTH = 1;
    #else // !IRS_STM32F7xx
    mp_spi_regs->SPI_CR1_bit.DFF = 0;
    #endif // !IRS_STM32F7xx
    m_data_item_byte_count = 1;
  } else {
    #ifdef IRS_STM32F7xx
    mp_spi_regs->SPI_CR2_bit.DS = 15;
    mp_spi_regs->SPI_CR2_bit.FRXTH = 0;
    #else // !IRS_STM32F7xx
    mp_spi_regs->SPI_CR1_bit.DFF = 1;
    #endif // !IRS_STM32F7xx
    m_data_item_byte_count = 2;
  }
  m_max_byte_count = calc_max_byte_count();
  //enable_spi();
  mp_spi_regs->SPI_CR1_bit.SPE = spe;
  reset_dma();
  return true;
}

void irs::arm::st_hal_spi_dma_t::write(const irs_u8* ap_buf, irs_uarc a_size)
{
  read_write(IRS_NULL, ap_buf, a_size);
}

void irs::arm::st_hal_spi_dma_t::read(irs_u8* ap_buf, irs_uarc a_size)
{
  read_write(ap_buf, IRS_NULL, a_size);
}

void irs::arm::st_hal_spi_dma_t::lock()
{
  m_lock = true;
}

void irs::arm::st_hal_spi_dma_t::unlock()
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя разблокировать в процессе "
      "передачи данных");
  }
  m_lock = false;
}

bool irs::arm::st_hal_spi_dma_t::get_lock()
{
  return m_lock;
}

void irs::arm::st_hal_spi_dma_t::tick()
{
  if (m_process == process_read_write) {

    if (!m_tx_status) {
      m_tx_status =
        HAL_DMA_PollForTransfer(&m_hdma_tx, HAL_DMA_FULL_TRANSFER, 0) == HAL_OK;
    }

    if (!m_rx_status) {
      m_rx_status =
        HAL_DMA_PollForTransfer(&m_hdma_rx, HAL_DMA_FULL_TRANSFER, 0) == HAL_OK;
    }

    if (m_tx_status && m_rx_status) {
      m_tx_status = false;
      m_rx_status = false;
      if (mp_read_buf) {
        invalidate_dcache_rx();

        memcpy(mp_read_buf, m_rx_buffer.data(), m_byte_count);
      }
      m_index += m_byte_count/m_data_item_byte_count;

      if (m_index < m_packet_size) {
        const size_type need_size = (m_packet_size - m_index)*
          m_data_item_byte_count;
        const size_type start_index = m_index/m_data_item_byte_count;

        if (mp_write_buf != IRS_NULL) {
          m_byte_count = min(m_max_byte_count, need_size);
          memcpy(m_tx_buffer.data(), mp_write_buf + start_index, m_byte_count);
        } else {
          memset(m_tx_buffer.data(), 0, m_byte_count);
        }
        start_spi_dma();
      } else {
        m_process = process_wait_command;
      }

      if (m_index >= m_packet_size) {
        m_process = process_wait_command;
      }
    }
  }
}

void irs::arm::st_hal_spi_dma_t::invalidate_dcache_rx()
{
  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // Ничего не делаем
  #elif IRS_STM32F7xx
  SCB_InvalidateDCache_by_Addr(
    reinterpret_cast<uint32_t*>(m_rx_buffer.data()),
    m_byte_count);
  #else
  #error Тип контроллера не определён
  #endif // IRS_STM32F7xx
}

irs::arm::st_hal_spi_dma_t::size_type
irs::arm::st_hal_spi_dma_t::calc_max_byte_count() const
{
  size_type byte_count = 0;
  byte_count = min(m_rx_buffer.size(), m_tx_buffer.size());
  byte_count = min<size_type>(byte_count, 0xFFFF);
  // Берем четное количество, если необходимо
  byte_count = (byte_count/m_data_item_byte_count)*m_data_item_byte_count;

  return byte_count;
}

void irs::arm::st_hal_spi_dma_t::read_write(irs_u8 *ap_read_buf,
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
  m_byte_count = min(m_max_byte_count, a_size);///m_data_item_byte_count;
  if (ap_write_buf) {
    memcpy(m_tx_buffer.data(), ap_write_buf, m_byte_count);
  } else {
    memset(m_tx_buffer.data(), 0, m_byte_count);
  }
  mp_read_buf = ap_read_buf;
  mp_write_buf = ap_write_buf;
  m_index = 0;

  m_process = process_read_write;
  start_spi_dma();
}

#endif // USE_HAL_DRIVER

irs::arm::arm_spi_t::arm_spi_t(
  size_t a_spi_address,
  irs_u32 a_bitrate,
  gpio_channel_t a_sck,
  gpio_channel_t a_miso,
  gpio_channel_t a_mosi,
  gpio_speed_t a_gpio_speed
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
  // Временно, пока нет соотетствующих регистров!!!
  if (a_spi_address == IRS_SPI6_BASE) {
    #ifdef USE_HAL_DRIVER
    __HAL_RCC_SPI6_RELEASE_RESET();
    #else
    IRS_LIB_ASSERT_MSG("Сброс для указанного устройства не определен");
    #endif
  } else {
    reset_peripheral(a_spi_address);
  }

  initialize_gpio_channels(a_sck, a_miso, a_mosi, a_gpio_speed);

  // Временно, пока нет соотетствующих регистров!!!
  if (a_spi_address == IRS_SPI6_BASE) {
    #ifdef USE_HAL_DRIVER
    __HAL_RCC_SPI6_CLK_ENABLE();
    #else
    IRS_LIB_ASSERT_MSG("Сброс для указанного устройства не определен");
    #endif
  } else {
    clock_enable(a_spi_address);
  }

  mp_spi_regs->SPI_CR1_bit.SSM = 1;
  //mp_spi_regs->SPI_CR1_bit.SSM = 0;
  mp_spi_regs->SPI_CR1_bit.SSI = 1;
  // 1: Master configuration
  mp_spi_regs->SPI_CR1_bit.MSTR = 1;

  //mp_spi_regs->SPI_CR2_bit.NSSP = 1;
  //mp_spi_regs->SPI_CR2_bit.SSOE = 1;
  //mp_spi_regs->SPI_CR2_bit.TXDMAEN = 1;

  set_default();
  enable_spi();
}

void irs::arm::arm_spi_t::initialize_gpio_channels(gpio_channel_t a_sck,
  gpio_channel_t a_miso,
  gpio_channel_t a_mosi,
  int a_gpio_speed)
{
  st_spi_base_t::initialize_gpio_channels(
    mp_spi_regs, a_sck, a_miso, a_mosi, a_gpio_speed);
}

irs::arm::arm_spi_t::~arm_spi_t()
{
  disable_spi();
}

void irs::arm::arm_spi_t::set_default()
{
  set_bitrate(m_bitrate_default);
  set_polarity(POSITIVE_POLARITY);
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
  st_spi_base_t::_disable_spi(mp_spi_regs);
  /*while (mp_spi_regs->SPI_SR_bit.RXNE != 0) {
    volatile irs_u32 data = mp_spi_regs->SPI_DR;
  }
  while (mp_spi_regs->SPI_SR_bit.TXE != 1);
  while (mp_spi_regs->SPI_SR_bit.BSY != 0);
  // 0: Peripheral disabled
  mp_spi_regs->SPI_CR1_bit.SPE = 0;*/
}

void irs::arm::arm_spi_t::abort()
{
  //IRS_DBG_RAW_MSG("SPI_ABORT" << endl);
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
  return st_spi_base_t::_set_bitrate(mp_spi_regs, a_bitrate);
}

bool irs::arm::arm_spi_t::set_polarity(polarity_t a_polarity)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_polarity(mp_spi_regs, a_polarity);
}

bool irs::arm::arm_spi_t::set_phase(phase_t a_phase)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_phase(mp_spi_regs, a_phase);
}

bool irs::arm::arm_spi_t::set_order(order_t a_order)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  return st_spi_base_t::_set_order(mp_spi_regs, a_order);
}

bool irs::arm::arm_spi_t::set_data_size(irs_u16 a_data_size)
{
  if (m_process == process_read_write) {
    IRS_LIB_ERROR(ec_standard, "Нельзя задать значение в процессе "
      "передачи данных");
  }
  if ((a_data_size != 8) && (a_data_size != 16)) {
    IRS_LIB_ERROR(ec_standard, "Размер данных может быть только 8 или 16");
  }
  irs_u8 spe = mp_spi_regs->SPI_CR1_bit.SPE;
  disable_spi();
  if (a_data_size == 8) {
    #ifdef IRS_STM32F7xx
    mp_spi_regs->SPI_CR2_bit.DS = 7;
    mp_spi_regs->SPI_CR2_bit.FRXTH = 1;
    #else // !IRS_STM32F7xx
    mp_spi_regs->SPI_CR1_bit.DFF = 0;
    #endif // !IRS_STM32F7xx
    m_data_item_byte_count = 1;
  } else {
    #ifdef IRS_STM32F7xx
    mp_spi_regs->SPI_CR2_bit.DS = 15;
    #else // !IRS_STM32F7xx
    mp_spi_regs->SPI_CR1_bit.DFF = 1;
    #endif // !IRS_STM32F7xx
    m_data_item_byte_count = 2;
  }
  //enable_spi();
  mp_spi_regs->SPI_CR1_bit.SPE = spe;
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

//#define ARM_SPI_OLD_VERSION

#ifndef ARM_SPI_OLD_VERSION
void irs::arm::arm_spi_t::tick()
{
  if (m_process == process_read_write) {
    if ((m_write_buf_index <= (m_read_buf_index)) &&
      (m_write_buf_index < m_packet_size)) {
      if (mp_spi_regs->SPI_SR_bit.TXE == 1) {
        writeToDR();
      }
    }
    if (((m_read_buf_index + 1) == m_write_buf_index) ||
        (m_write_buf_index == m_packet_size)) {
      if (mp_spi_regs->SPI_SR_bit.RXNE == 1) {
        readFromDR();
        if (m_read_buf_index >= m_packet_size) {
          m_process = process_wait_command;
        }
      }
    }
  }
}
#endif // !ARM_SPI_OLD_VERSION
#ifdef ARM_SPI_OLD_VERSION
void irs::arm::arm_spi_t::tick()
{
  int shift = 0;
  if (m_process == process_read_write) {
    if ((m_write_buf_index <= (m_read_buf_index + shift)) &&
      (m_write_buf_index < m_packet_size)) {
      if (mp_spi_regs->SPI_SR_bit.TXE == 1) {
        if (mp_write_buf != IRS_NULL) {
          *reinterpret_cast<irs_u8*>(&mp_spi_regs->SPI_DR) =
            mp_write_buf[m_write_buf_index*m_data_item_byte_count];
        } else {
          *reinterpret_cast<irs_u8*>(&mp_spi_regs->SPI_DR) = 0;
        }
        m_write_buf_index++;
      }
    }
    if (((m_read_buf_index + shift + 1) == m_write_buf_index) ||
        (m_write_buf_index == m_packet_size)) {
      if (mp_spi_regs->SPI_SR_bit.RXNE == 1) {
        if (mp_read_buf) {
          mp_read_buf[m_read_buf_index*m_data_item_byte_count] =
            *reinterpret_cast<irs_u8*>(&mp_spi_regs->SPI_DR);
        } else {
          volatile irs_u32 data = *reinterpret_cast<irs_u8*>(&mp_spi_regs->SPI_DR);
        }
        m_read_buf_index++;
        if (m_read_buf_index >= m_packet_size) {
          m_process = process_wait_command;
        }
      }
    }
  }
}
#endif // ARM_SPI_OLD_VERSION

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

  // В калибраторе (плата rev. R2, STM32f756, SPI6) при смене полярности в
  // буфере иногда появлялся мусор, который приводил к ошибочному взведению
  // статуса наличия данных для чтения. Причина ошибки до конца не выяснена.
  // Ниже вставлена функция для очистки буфера чтения/записи от мусора, если он
  // там есть
  st_spi_base_t::clean_buf_from_old_data(mp_spi_regs);

  m_packet_size = a_size/m_data_item_byte_count;
  mp_read_buf = ap_read_buf;
  mp_write_buf = ap_write_buf;
  m_write_buf_index = 0;
  m_read_buf_index = 0;
  m_process = process_read_write;
}

#else // Микроконтроллеры
  #error Тип контроллера не определён
#endif  // Микроконтроллеры
