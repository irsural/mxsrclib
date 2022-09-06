#ifndef armi2cH
#define armi2cH

#include "irsi2c.h"

namespace irs
{

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
  irs_u16 get_device_addr();
  void set_status(status_t a_status);
  void read(irs_u8 *ap_buf, irs_u16 a_size);
  void write(irs_u8 *ap_buf, irs_u16 a_size);
  status_t get_status();
  void tick();
private:
  bool m_lock;
  status_t m_status;
  irs_u16 m_device_address;
};



#else
  #error Тип контроллера не определён
#endif //defined (IRS_NIIET_1921)

} //namespace arm

}//namespace irs

#endif