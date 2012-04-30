//! \file
//! \ingroup system_utils_group
//! \brief Утилиты для ARM
//!
//! Дата: 27.04.2012\n
//! Дата создания: 27.04.2012

#include <irspch.h>

#include <irsmcutil.h>
#include <irsstd.h>

#include <irsfinal.h>

irs::blink_t::blink_t(irs_avr_port_t a_port, irs_u8 a_bit, 
  counter_t a_blink_time
):
  m_pin_led(*(get_avr_port_map()[a_port].set), a_bit, gpio_pin_t::dir_out),
  m_blink_timer(a_blink_time)
{
}
