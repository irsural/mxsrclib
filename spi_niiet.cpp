#include "K1921VK035.h"
#include <spi_niiet.h>
#include <irscpu.h>
#include <plib035_spi.h>
#include <plib035_gpio.h>
#include <logging.h>

using namespace irs;

spi_niiet_t::spi_niiet_t() :
  m_status(inner_state::st_free),
  mp_buf(nullptr),
  m_buf_size(0),
  m_lock(false),
  m_bitrate(100000),
  m_polarity(FALLING_EDGE),
  m_phase(LEAD_EDGE),
  m_order(MSB),
  m_frequency(cpu_traits_t::frequency())
{
  SPI_Init_TypeDef SPI_InitStruct;
  GPIO_Init_TypeDef GPIO_InitStruct;

  RCU_SPIClkConfig(RCU_PeriphClk_PLLClk, 0, DISABLE);
  RCU_SPIClkCmd(ENABLE);
  RCU_SPIRstCmd(ENABLE);
  SPI_StructInit(&SPI_InitStruct);
  SPI_InitStruct.SCKDiv = 9;
  SPI_InitStruct.SCKDivExtra = 1;
  SPI_InitStruct.FrameFormat = SPI_FrameFormat_SPI;
  SPI_InitStruct.DataWidth = SPI_DataWidth_8;

  SPI_SCKConfig(SPI_SCKPhase_CaptureRise, SPI_SCKPolarity_SteadyLow);

//  SPI_ITFIFOLevelRxConfig(3);
//  SPI_ITFIFOLevelTxConfig(3);
//  SPI_SCKConfig(SPI_SCKPhase_CaptureRise, SPI_SCKPolarity_SteadyLow);
//  SPI_SCKConfig(SPI_SCKPhase_CaptureFall, SPI_SCKPolarity_SteadyHigh);
//  SPI_SCKConfig(SPI_SCKPhase_CaptureRise, SPI_SCKPolarity_SteadyHigh);
//  SPI_SCKConfig(SPI_SCKPhase_CaptureFall, SPI_SCKPolarity_SteadyLow);


  SPI_Init(&SPI_InitStruct);
  SPI_Cmd(ENABLE);

  RCU_AHBClkCmd(RCU_AHBClk_GPIOB, ENABLE);
  RCU_AHBRstCmd(RCU_AHBRst_GPIOB, ENABLE);
  GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.AltFunc = ENABLE;
  GPIO_InitStruct.Digital = ENABLE;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_Pin_4;
  GPIO_InitStruct.Out = ENABLE;
  GPIO_InitStruct.AltFunc = DISABLE;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void spi_niiet_t::abort()
{
  m_status = inner_state::st_free;
  mp_buf = nullptr;
  m_buf_size = 0;
}
void spi_niiet_t::read(irs_u8 *ap_buf, irs_uarc a_size)
{
  init_io_oper(ap_buf, a_size, inner_state::st_read);
}

void spi_niiet_t::write(const irs_u8 *ap_buf, irs_uarc a_size)
{
  init_io_oper(const_cast<irs_u8*>(ap_buf), a_size, inner_state::st_write);
}

void spi_niiet_t::read_write(irs_u8 *ap_buf, const irs_u8 *ap_write_buf,
  irs_uarc a_size)
{
  init_io_oper(ap_buf, a_size, inner_state::st_read_write);
}

void spi_niiet_t::init_io_oper(irs_u8 *ap_buf, irs_uarc a_size,
                               inner_state a_status)
{
  m_status = a_status;
  mp_buf = ap_buf;
  m_buf_size = a_size;
}

void spi_niiet_t::setCS()
{
  GPIO_ClearBits(GPIOB, GPIO_Pin_4);
}

void spi_niiet_t::clearCS()
{
  GPIO_SetBits(GPIOB, GPIO_Pin_4);
}

void spi_niiet_t::tick()
{
  switch(m_status)
  {
    case st_free: {
    } break;

    case st_read: {
      while(SPI_FlagStatus(SPI_Flag_RxFIFONotEmpty))
      {
        SPI_RecieveData();
        SPI_SendData(0);
      }

      read_buf();
      m_status = inner_state::st_free;
    } break;

    case st_write: {
      while(SPI_FlagStatus(SPI_Flag_RxFIFONotEmpty))
      {
        SPI_RecieveData();
        SPI_SendData(0);
      }

      write_buf();
      m_status = inner_state::st_free;
    } break;

    case st_read_write: {
    } break;
  }
}

irs_status_t spi_niiet_t::get_irs_status()
{
  return (m_status == inner_state::st_free) ?
                 irs_status_t::irs_st_ready :
                 irs_status_t::irs_st_busy;
}

spi_t::status_t spi_niiet_t::get_status()
{
  return (m_status == inner_state::st_free) ?
                      spi_t::status_t::FREE :
                      spi_t::status_t::BUSY;
}

void spi_niiet_t::lock()
{
  m_lock = true;
}

void spi_niiet_t::unlock()
{
  m_lock = false;
}

bool spi_niiet_t::get_lock()
{
  return m_lock;
}

bool spi_niiet_t::set_bitrate(irs_u32 a_bitrate)
{
  if(m_status == inner_state::st_free)
  {
    m_bitrate = a_bitrate;
    return true;
  }
  else
  {
    return false;
  }
}

bool spi_niiet_t::set_polarity(polarity_t a_polarity)
{
  if(m_status == inner_state::st_free)
  {
    m_polarity = a_polarity;
    return true;
  }
  else
  {
    return false;
  }
}

bool spi_niiet_t::set_phase(phase_t a_phase)
{
  if(m_status == inner_state::st_free)
  {
    m_phase = a_phase;
    return true;
  }
  else
  {
    return false;
  }
}

bool spi_niiet_t::set_order(order_t a_order)
{
  if(m_status == inner_state::st_free)
  {
    m_order = a_order;
    return true;
  }
  else
  {
    return false;
  }
}

bool spi_niiet_t::set_data_size(irs_u16 a_data_size)
{
  return true;
}

void spi_niiet_t::read_buf()
{
  for(int i = 0; i < m_buf_size; i++)
  {
    while (SPI_FlagStatus(SPI_Flag_Busy));

    SPI_SendData(i+1);
    while (SPI_FlagStatus(SPI_Flag_Busy));

    mp_buf[i] = SPI_RecieveData();
    INFO("re: check %d = 0x%x", i, mp_buf[i]);
    INFO("re: send 0x%x", i + 1);
  }
  INFO("\n");
}

void spi_niiet_t::write_buf()
{
  for(int i = 0; i < m_buf_size; i++)
  {
    while (SPI_FlagStatus(SPI_Flag_Busy));

    INFO("wr: send 0x%x", mp_buf[i]);
    SPI_SendData(mp_buf[i]);
    while (SPI_FlagStatus(SPI_Flag_Busy));

    int check = SPI_RecieveData();
    INFO("wr: check_%d = 0x%x", i, check);
  }
  INFO("\n");
}

void spi_niiet_t::read_write_buf()
{

}

