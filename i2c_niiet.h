#ifndef I2C_NIIET_H
#define I2C_NIIET_H

#ifdef IRS_NIIET_1921
#include "K1921VK035.h"
#include "plib035.h"
#else
#error ��� ����������� �� ��������
#endif

#include <string.h>
#include <irsdefs.h>
#include <i2c.h>

namespace irs
{

class i2c_niiet_t : public i2c_t
{
public:
  i2c_niiet_t(GPIO_TypeDef* port_scl, uint32_t pin_scl, GPIO_TypeDef* port_sda, uint32_t pin_sda);
  virtual ~i2c_niiet_t() {}
  virtual bool is_free();
  virtual irs_status_t get_status();
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
  void write_buffer(irs_u8* buffer, size_t buffer_size);
  void read_buffer(irs_u8* buffer, size_t buffer_size);
  bool is_device_ready();

  internal_status_t m_status;
  operation_t m_operation;
  bool m_lock;
  irs_u8 *mp_buffer;
  size_t m_buffer_size;
  irs_u16 m_device_addr;
};

}


#endif // I2C_NIIET_H