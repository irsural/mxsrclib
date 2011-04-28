//! \file
//! \ingroup system_utils_group
//! \brief Enbkbns для ARM
//!
//! Дата: 16.11.2010\n
//! Дата создания: 16.11.2010

#ifndef irsarmutilH
#define irsarmutilH

#include <irsdefs.h>

#include <armgpio.h>
#include <timer.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup drivers_group
//! @{

class blink_t
{
private:
  arm::io_pin_t m_pin_led;
  loop_timer_t m_blink_timer;
public:
  blink_t(arm_port_t& a_port, irs_u8 a_bit, counter_t a_blink_time = 0):
    m_pin_led(a_port, a_bit, gpio_pin_t::dir_out)
  {
    if (!a_blink_time) {
      m_blink_timer.set();
    } else {
      m_blink_timer.set(a_blink_time);
    }
  }
  ~blink_t() {}
  void set()
  {
    m_pin_led.set();
  }
  void clear()
  {
    m_pin_led.clear();
  }
  void flip()
  {
    if (m_pin_led.pin()) {
      m_pin_led.clear();
    } else {
      m_pin_led.set();
    }
  }
  void operator()()
  {
    if (m_blink_timer.check()) {
      flip();
    }
  }
};

//! @}

} // namespace irs

#endif // irsarmutilH
