//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для ARM
//!
//! Дата: 22.11.2010
//! Дата создания: 10.11.2010

#include <armgpio.h>

irs::arm::io_pin_t::io_pin_t(arm_port_t &a_port, irs_u8 a_bit, dir_t a_dir):
  mp_port(&a_port),
  m_data_mask(0x04 << a_bit),
  m_port_mask(1 << a_bit)
{
  clock_gating_control(mp_port);
  #ifdef __LM3SxBxx__
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_LOCK) = GPIO_UNLOCK_VALUE;
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_CR) |= m_port_mask;
  #endif // __LM3SxBxx__
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DEN) |= m_port_mask;
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_AFSEL) &= ~m_port_mask;
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_ODR) &= ~m_port_mask;
  if (a_dir == dir_in) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) &= ~m_port_mask;
  } else if (a_dir == dir_out) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) |= m_port_mask;
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_data_mask) = 0;
  }
}

irs::arm::io_pin_t::~io_pin_t()
{
  return;
}

bool irs::arm::io_pin_t::pin()
{
  return HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_data_mask);
}

void irs::arm::io_pin_t::set()
{
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_data_mask) = 0xFF;
}

void irs::arm::io_pin_t::clear()
{
  HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DATA + m_data_mask) = 0;
}
void irs::arm::io_pin_t::set_dir(dir_t a_dir)
{
  if (a_dir == dir_in) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) &= ~m_port_mask;
  } else if (a_dir == dir_out) {
    HWREG(reinterpret_cast<irs_u32>(mp_port) + GPIO_DIR) |= m_port_mask;
  }
}
