//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для ARM
//!
//! Дата: 22.11.2010
//! Дата создания: 10.11.2010

#include <irspch.h>

#include <irsgpio.h>

#include <irsfinal.h>

#ifdef __ICCAVR__

//--------------------------  io_pin_t  -----------------------------

irs::avr::io_pin_t::io_pin_t(p_avr_port_t ap_port,
  p_avr_port_t, p_avr_port_t,
  irs_u8 a_bit, dir_t a_dir, bool a_pull_up):
  mp_port(ap_port),
  m_mask(1 << a_bit)
{
  if (a_dir == dir_in)
  {
    dir() &= (m_mask^0xFF);
    if (a_pull_up) out() |= m_mask;
  }
  if (a_dir == dir_out)
  {
    dir() |= m_mask;
    out() &= (m_mask^0xFF);
  }
}

irs::avr::io_pin_t::io_pin_t(avr_port_t &a_port,
  irs_u8 a_bit, dir_t a_dir, bool a_pull_up):
  mp_port(&a_port),
  m_mask(1 << a_bit)
{
  if (a_dir == dir_in)
  {
    dir() &= (m_mask^0xFF);
    if (a_pull_up) out() |= m_mask;
  }
  if (a_dir == dir_out)
  {
    dir() |= m_mask;
    out() &= (m_mask^0xFF);
  }
}

irs::avr::io_pin_t::~io_pin_t()
{
  return;
}

bool irs::avr::io_pin_t::pin()
{
  return in() & m_mask;
}

void irs::avr::io_pin_t::set()
{
  out() |= m_mask;
}

void irs::avr::io_pin_t::clear()
{
  out() &= (m_mask^0xFF);
}
void irs::avr::io_pin_t::set_dir(dir_t a_dir)
{
  if (a_dir == dir_in) dir() &= (m_mask^0xFF);
  if (a_dir == dir_out) dir() |= m_mask;
}

//-----------------------------  mem_out_pin_t  --------------------------------

irs::avr::mem_out_pin_t::mem_out_pin_t(mem_out_register_t *ap_write_register,
  irs_u8 a_bit, irs_u8 a_init_value):
  mp_write_register(ap_write_register),
  m_bit(a_bit)
{
  if (m_bit > 7) m_bit = 7;
  irs_u8 buf_value = mp_write_register->get_value();
  if (a_init_value)
    buf_value |= (1 << m_bit);
  else
    buf_value &= ((1 << m_bit)^0xFF);
  mp_write_register->set_value(buf_value);
}

irs::avr::mem_out_pin_t::~mem_out_pin_t()
{
  irs_u8 buf_value = mp_write_register->get_value();
  buf_value &= ((1 << m_bit)^0xFF);
  mp_write_register->set_value(buf_value);
}

bool irs::avr::mem_out_pin_t::pin()
{
  irs_u8 buf_value = mp_write_register->get_value();
  return buf_value & (1 << m_bit);
}

void irs::avr::mem_out_pin_t::set()
{
  irs_u8 buf_value = mp_write_register->get_value();
  buf_value |= (1 << m_bit);
  mp_write_register->set_value(buf_value);
}

void irs::avr::mem_out_pin_t::clear()
{
  irs_u8 buf_value = mp_write_register->get_value();
  buf_value &= ((1 << m_bit)^0xFF);
  mp_write_register->set_value(buf_value);
}
void irs::avr::mem_out_pin_t::set_dir(dir_t /*a_dir*/)
{
  return;
}

//---------------------------  mem_out_register_t  -----------------------------

irs::avr::mem_out_register_t::mem_out_register_t(irs_uarc a_address,
  irs_u8 a_init_value):
  m_value(a_init_value),
  mp_register((irs_u8*)a_address)
{
  *mp_register = a_init_value;
}

irs::avr::mem_out_register_t::~mem_out_register_t()
{
}

inline irs_u8 irs::avr::mem_out_register_t::get_value()
{
  return m_value;
}

inline void irs::avr::mem_out_register_t::set_value(irs_u8 a_value)
{
  m_value = a_value;
  *mp_register = m_value;
}

#endif //__ICCAVR__

#ifdef __ICCARM__

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

#endif //__ICCARM__
