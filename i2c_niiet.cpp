#include <i2c_niiet.h>
#include "logging.h"

using namespace irs;

i2c_niiet_t::i2c_niiet_t(GPIO_TypeDef* port_scl, uint32_t pin_scl,
  GPIO_TypeDef* port_sda, uint32_t pin_sda):
  m_status(in_free),
  m_operation(op_none),
  m_lock(false),
  mp_buffer(nullptr),
  m_buffer_size(0),
  m_device_addr(0)
{
  RCU->HCLKCFG_bit.GPIOAEN = 1;
  RCU->HRSTCFG_bit.GPIOAEN = 1;

  GPIO_Init_TypeDef scl_sda_init;
  scl_sda_init.Pin       = pin_scl;
  scl_sda_init.Out       = ENABLE;
  scl_sda_init.AltFunc   = ENABLE;
  scl_sda_init.OutMode   = GPIO_OutMode_OD;
  scl_sda_init.InMode    = GPIO_InMode_Schmitt;
  scl_sda_init.PullMode  = GPIO_PullMode_PU;
  scl_sda_init.DriveMode = GPIO_DriveMode_HighFast;
  scl_sda_init.Digital   = ENABLE;
  GPIO_Init(port_scl, &scl_sda_init);

  scl_sda_init.Pin = pin_sda;
  GPIO_Init(port_sda, &scl_sda_init);

  RCU->PCLKCFG_bit.I2CEN = 1;
  RCU->PRSTCFG_bit.I2CEN = 1;

  I2C->CTL1_bit.ENABLE = 1;
  I2C->CTL1_bit.SCLFRQ = 0x4;
  I2C->CTL3_bit.SCLFRQ = 0x4;

  I2C->CTL0_bit.INTEN = 1;
}

void i2c_niiet_t::tick()
{
  switch(m_status)
  {
    case in_free: {
    } break;

    case in_check_dev: {
      if(is_device_ready()) {
        m_status = (m_operation == op_read) ? in_read : in_write;
        m_operation = op_none;
      }
    } break;

    case in_write: {
      write_buffer(mp_buffer, m_buffer_size);
      m_status = in_free;
    } break;

    case in_read: {
      read_buffer(mp_buffer, m_buffer_size);
      m_status = in_free;
    } break;
  }
}

void i2c_niiet_t::read(irs_u8* ap_buffer, size_t a_size)
{
  initialize_io_operation(ap_buffer, a_size, op_read);
}

void i2c_niiet_t::write(irs_u8* ap_buffer, size_t a_size)
{
  initialize_io_operation(ap_buffer, a_size, op_write);
}

void i2c_niiet_t::initialize_io_operation(irs_u8* ap_buffer,
  size_t a_size, operation_t a_oper)
{
  m_operation = a_oper;
  m_status = in_check_dev;
  mp_buffer = ap_buffer;
  m_buffer_size = a_size;
}

void i2c_niiet_t::abort()
{
  m_status = in_free;
  m_operation = op_none;
  m_lock = false;
  mp_buffer = 0;
  m_buffer_size = 0;
  m_device_addr = 0;
}

irs_status_t i2c_niiet_t::get_status()
{
  return (m_status == in_free) ? irs_st_ready : irs_st_busy;
}

void i2c_niiet_t::lock()
{
  m_lock = true;
}

bool i2c_niiet_t::get_lock()
{
  return m_lock;
}

void i2c_niiet_t::unlock()
{
  m_lock = false;
}

void i2c_niiet_t::set_device_address(irs_u8 a_device_addr)
{
  m_device_addr = a_device_addr;
}

irs_u8 i2c_niiet_t::get_device_address()
{
  return m_device_addr;
}

bool i2c_niiet_t::is_free()
{
  return m_status == in_free;
}

void i2c_niiet_t::write_buffer(uint8_t* buf, size_t buf_size)
{
  I2C->CTL0_bit.START = 1;
  while(I2C->ST_bit.MODE != I2C_ST_MODE_STDONE);

  I2C->SDA_bit.DATA = m_device_addr;
  I2C->CTL0_bit.CLRST = 1;
  while(I2C->ST_bit.MODE != I2C_ST_MODE_MTADPA);

  for(int i = 0; i < buf_size; i++)
  {
    I2C->SDA_bit.DATA = buf[i];
    I2C->CTL0_bit.CLRST = 1;
    while(I2C->ST_bit.MODE != I2C_ST_MODE_MTDAPA);
  }

  I2C->CTL0_bit.STOP = 1;
  I2C->CTL0_bit.CLRST = 1;
}

void i2c_niiet_t::read_buffer(uint8_t* buf, size_t buf_size)
{
  I2C->CTL0_bit.START = 1;
  while(I2C->ST_bit.MODE != I2C_ST_MODE_STDONE);

  I2C->SDA_bit.DATA = m_device_addr + 1;
  I2C->CTL0_bit.CLRST = 1;
  while(I2C->ST_bit.MODE != I2C_ST_MODE_MRADPA);

  for(int i = 0; i < buf_size; i++)
  {
    if(i == buf_size - 1)
      I2C->CTL0_bit.ACK = 1;
    I2C->CTL0_bit.CLRST = 1;
    while((I2C->ST_bit.MODE != I2C_ST_MODE_MRDAPA) &&
          (I2C->ST_bit.MODE != I2C_ST_MODE_MRDANA));
    buf[i] = I2C->SDA_bit.DATA;
  }

  I2C->CTL0_bit.STOP = 1;
  I2C->CTL0_bit.CLRST = 1;
}

bool i2c_niiet_t::is_device_ready()
{
  I2C->CTL0_bit.START = 1;
  while(I2C->ST_bit.MODE != I2C_ST_MODE_STDONE);

  I2C->SDA_bit.DATA = m_device_addr;
  I2C->CTL0_bit.CLRST = 1;

  while((I2C->ST_bit.MODE != I2C_ST_MODE_MTADPA) &&
        (I2C->ST_bit.MODE != I2C_ST_MODE_MTADNA));
  uint32_t mode = I2C->ST_bit.MODE;

  I2C->CTL0_bit.STOP = 1;
  I2C->CTL0_bit.CLRST = 1;

  return (mode == I2C_ST_MODE_MTADPA);
}
