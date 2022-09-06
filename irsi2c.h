//! \file
//! \ingroup drivers_group
//! \brief Abstract base class for I2C
//!
//! Date: 19.08.2022
#ifndef irsi2cH
#define irsi2cH

#include <irsdefsbase.h>

#define I2C_CHIP_ADDRESS 0xC8UL //check for correct or not
#define I2C_WRITE 0x00UL
#define I2C_READ 0x01UL

namespace irs
{

//! \addtogroup string_processing_group
//! @{

class i2c_t
{
public:
  enum status_t {READY, BUSY_READ, BUSY_WRITE};

  virtual ~i2c_t() {}
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool get_lock() = 0;
  virtual void set_device_addr(irs_u16 a_device_addr) = 0;
  virtual void read(uint8_t *ap_buf, irs_u16 a_size) = 0;
  virtual void write(uint8_t *ap_buf, irs_u16 a_size) = 0;
  virtual status_t get_status() = 0;
  virtual void tick() = 0;
};

//! @}

} //namespace irs

#endif //irsi2cH