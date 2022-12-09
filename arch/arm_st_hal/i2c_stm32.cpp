#include "i2c_stm32.h"

using namespace irs;

i2c_stm32_t::gpio_init_t::gpio_init_t(GPIO_TypeDef *a_port, uint16_t a_pin, uint8_t a_alt_func):
  m_pin(a_pin),
  m_port(a_port),
  m_alt_func(a_alt_func)
{

}

i2c_stm32_t::i2c_stm32_t(I2C_HandleTypeDef *a_i2c, gpio_init_t *a_scl, gpio_init_t *a_sda):
  m_status(in_free),
  m_operation(op_none),
  m_lock(false),
  mp_buffer(nullptr),
  m_buffer_size(0),
  m_device_addr(0),
  m_i2c_setting(*a_i2c)
{
  GPIO_InitTypeDef  gpio_init_struct;

  gpio_init_struct.Pin       = a_scl->m_pin;
  gpio_init_struct.Mode      = GPIO_MODE_AF_OD;
  gpio_init_struct.Pull      = GPIO_PULLUP;
  gpio_init_struct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_struct.Alternate = a_scl->m_alt_func;
  HAL_GPIO_Init(a_scl->m_port, &gpio_init_struct);

  gpio_init_struct.Pin       = a_sda->m_pin;
  gpio_init_struct.Alternate = a_sda->m_alt_func;
  HAL_GPIO_Init(a_scl->m_port, &gpio_init_struct);

  HAL_I2C_Init(&m_i2c_setting);
}

void i2c_stm32_t::tick()
{
  switch(m_status)
  {
    case in_free: {
    } break;

    case in_check_dev: {
      if(HAL_I2C_IsDeviceReady(&m_i2c_setting, m_device_addr, 1, HAL_MAX_DELAY) == HAL_OK){
        m_status = (m_operation == op_read) ? in_read : in_write;
        m_operation = op_none;
      }
    } break;

    case in_write: {
      HAL_I2C_Master_Transmit(&m_i2c_setting, m_device_addr, mp_buffer, m_buffer_size, HAL_MAX_DELAY);
      m_status = in_free;
    } break;

    case in_read: {
      HAL_I2C_Master_Receive(&m_i2c_setting, m_device_addr, mp_buffer, m_buffer_size, HAL_MAX_DELAY);
      m_status = in_free;
    } break;
  }
}

void i2c_stm32_t::read(irs_u8* ap_buffer, size_t a_size)
{
  initialize_io_operation(ap_buffer, a_size, op_read);
}

void i2c_stm32_t::write(irs_u8* ap_buffer, size_t a_size)
{
  initialize_io_operation(ap_buffer, a_size, op_write);
}

void i2c_stm32_t::initialize_io_operation(irs_u8* ap_buffer, size_t a_size, operation_t a_oper)
{
  m_operation = a_oper;
  m_status = in_check_dev;
  mp_buffer = ap_buffer;
  m_buffer_size = a_size;
}

void i2c_stm32_t::abort()
{
  m_status = in_free;
  m_operation = op_none;
  m_lock = false;
  mp_buffer = 0;
  m_buffer_size = 0;
  m_device_addr = 0;
}

i2c_stm32_t::status_t i2c_stm32_t::get_status()
{
  return (m_status == in_free) ? i2c_ready : i2c_busy;
}

void i2c_stm32_t::lock()
{
  m_lock = true;
}

bool i2c_stm32_t::get_lock()
{
  return m_lock;
}

void i2c_stm32_t::unlock()
{
  m_lock = false;
}

void i2c_stm32_t::set_device_address(irs_u16 a_addr_device)
{
  m_device_addr = a_addr_device;
}

bool i2c_stm32_t::is_free()
{
  return m_status == in_free;
}