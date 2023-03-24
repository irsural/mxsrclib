#ifndef I2C_T_H
#define I2C_T_H

#include <irsdefs.h>
#include <irsspi.h>


namespace irs
{

class i2c_t
{
public:
  virtual ~i2c_t() {}
  virtual bool is_free() = 0;
  virtual irs_status_t get_status() = 0;
  virtual void lock() = 0;
  virtual bool get_lock() = 0;
  virtual void unlock() = 0;
  virtual void set_device_address(irs_u8 a_addr_device) = 0;
  virtual irs_u8 get_device_address() = 0;
  virtual void tick() = 0;
  virtual void read(irs_u8* ap_buf, size_t a_size) = 0;
  virtual void write(irs_u8* ap_buf, size_t a_size) = 0;
  virtual void abort() = 0;
  virtual bool is_device_ready() = 0;
};

}

#endif /*I2C_T_H*/
