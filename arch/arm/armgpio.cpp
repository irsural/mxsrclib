//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для ARM
//!
//! Дата: 10.11.2010

#include <armgpio.h>

irs::arm::io_pin_t::io_pin_t(arm_port_t &a_port, irs_u8 a_bit, dir_t a_dir):
  mp_port(&a_port),
  m_mask(1 << a_bit)
{
  clock_gating_control(mp_port);
  for (char i = 10; i > 0; i--);
  if (a_dir == dir_in) {
    dir() &= (m_mask^0xFF);
  } else if (a_dir == dir_out) {
    dir() |= m_mask;
    data() &= (m_mask^0xFF);
  }
}

irs::arm::io_pin_t::~io_pin_t()
{
  return;
}

bool irs::arm::io_pin_t::pin()
{
  return data() & m_mask;
}

void irs::arm::io_pin_t::set()
{
  data() |= m_mask;
}

void irs::arm::io_pin_t::clear()
{
  data() &= (m_mask^0xFF);
}
void irs::arm::io_pin_t::set_dir(dir_t a_dir)
{
  if (a_dir == dir_in) dir() &= (m_mask^0xFF);
  if (a_dir == dir_out) dir() |= m_mask;
}
