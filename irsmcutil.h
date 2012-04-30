//! \file
//! \ingroup system_utils_group
//! \brief Утилиты для микроконтроллеров
//!
//! Дата: 26.04.2012\n
//! Дата создания: 26.04.2012

#ifndef irsmcutilH
#define irsmcutilH

#include <irsdefs.h>

#include <irsgpio.h>
#include <timer.h>

#include <irsfinal.h>

//*****************************************************************************
//*********  Ахтунг! Ахтунг! irserror и irsstd сюда включать нельзя!  *********
//*****************************************************************************

#ifdef IRS_MICROCONTROLLER

namespace irs {

//! \addtogroup drivers_group
//! @{

class blink_t
{
private:
  mc::io_pin_t m_pin_led;
  loop_timer_t m_blink_timer;
public:
  #ifdef __ICCAVR__
  // Реализация конструктора сделана в cpp-файле для того чтобы не включать
  // в h-файле irsstd.h
  blink_t(irs_avr_port_t a_port, irs_u8 a_bit, counter_t a_blink_time = 0);
  #endif //__ICCAVR__
  blink_t(mc_port_t& a_port, irs_u8 a_bit, counter_t a_blink_time = 0):
    m_pin_led(a_port, a_bit, gpio_pin_t::dir_out),
    m_blink_timer(a_blink_time)
  {
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

#endif //IRS_MICROCONTROLLER

#endif // irsmcutilH
