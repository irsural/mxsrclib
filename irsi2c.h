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

  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool get_lock() = 0;

  virtual ~i2c_t() {}
  virtual void set_device_addr(irs_u16 a_device_addr) = 0;
  virtual void read(irs_u16 a_mem_addr, uint8_t *ap_buf, irs_u16 a_size) = 0;
  virtual void write(irs_u16 a_mem_addr, uint8_t *ap_buf, irs_u16 a_size) = 0;
  virtual status_t get_status() = 0;
  virtual void tick() = 0;
};

namespace arm
{

#if defined (IRS_NIIET_1921)

class arm_i2c_t : public i2c_t
{
public:
  arm_i2c_t();
  ~arm_i2c_t();
  void lock();
  void unlock();
  bool get_lock();
  void set_device_addr(irs_u16 a_device_addr);
  void read(irs_u16 a_mem_addr, uint8_t *ap_buf, irs_u16 a_size);
  void write(irs_u16 a_mem_addr, uint8_t *ap_buf, irs_u16 a_size);
  status_t get_status();
  void tick();
private:
  void i2c_init(void);
  bool m_lock;
  status_t m_status;
  irs_u16 m_device_address;
//  m_mem_addr_size;
};

#else
  #error Тип контроллера не определён
#endif //defined (IRS_NIIET_1921)

} //namespace arm


//! @}

} //namespace irs

#endif //irsi2cH
