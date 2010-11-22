//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для ARM
//!
//! Дата: 22.11.2010
//! Дата создания: 10.11.2010

#include <armgpio.h>

irs::arm::io_pin_t::io_pin_t(arm_port_t &a_port, irs_u8 a_bit, dir_t a_dir):
  mp_port(&a_port),
  m_bit(a_bit)
{
  clock_gating_control(mp_port);
  for (irs_u32 i = 10; i > 0; i--);
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DEN) = 1;
  if (a_dir == dir_in) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) = 0;
  } else if (a_dir == dir_out) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) = 1;
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_LOCK) = 0x4C4F434B;
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_bit) = 0;
  }
}

irs::arm::io_pin_t::~io_pin_t()
{
  return;
}

bool irs::arm::io_pin_t::pin()
{
  return HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_bit);
}

void irs::arm::io_pin_t::set()
{
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_bit) = 1;
}

void irs::arm::io_pin_t::clear()
{
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_bit) = 0;
}
void irs::arm::io_pin_t::set_dir(dir_t a_dir)
{
  if (a_dir == dir_in) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) = 0;
  } else if (a_dir == dir_out) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) = 1;
  }
}
