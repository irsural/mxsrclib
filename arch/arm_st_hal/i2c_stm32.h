#ifndef I2C_STM32_H
#define I2C_STM32_H

#ifdef STM32H743xx
#include "stm32h7xx_hal.h"
#else
#error Тип контроллера не определён
#endif

#include <string.h>
#include <irsdefs.h>
#include <i2c.h>

namespace irs
{

class i2c_stm32_t : public i2c_t
{
public:
  struct gpio_init_t
  {
    gpio_init_t(GPIO_TypeDef *a_port, uint16_t a_pin, uint8_t a_alt_func);
    GPIO_TypeDef *m_port;
    uint16_t m_pin;
    uint8_t m_alt_func;
  };

  i2c_stm32_t(I2C_HandleTypeDef *a_i2c, gpio_init_t *a_scl, gpio_init_t *a_sda);
  virtual ~i2c_stm32_t() {}
  virtual bool is_free();
  virtual status_t get_status();
  virtual void lock();
  virtual bool get_lock();
  virtual void unlock();
  virtual void set_device_address(irs_u16 a_addr_device);
  virtual void tick();
  virtual void read(irs_u8* ap_buf, size_t a_size);
  virtual void write(irs_u8* ap_buf, size_t a_size);
  virtual void abort();

private:
  enum internal_status_t
  {
    in_free,
    in_read,
    in_write,
    in_check_dev
  };

  enum operation_t
  {
    op_read,
    op_write,
    op_none
  };

  void initialize_io_operation(irs_u8* ap_buffer, size_t a_size,
    operation_t a_oper);

  internal_status_t m_status;
  operation_t m_operation;
  bool m_lock;
  irs_u8 *mp_buffer;
  size_t m_buffer_size;
  irs_u16 m_device_addr;
  I2C_HandleTypeDef m_i2c_setting;
};

}


#endif // I2C_STM32_H
