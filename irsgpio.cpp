//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для ARM
//!
//! Дата: 22.11.2010
//! Дата создания: 10.11.2010

#include <irspch.h>

#include <irsgpio.h>

#include <irsfinal.h>

// class gpio_pin_t:
void irs::gpio_pin_t::set_state(io_pin_value_t a_value)
{
  if (a_value == io_pin_on) {
    set();
  } else if (a_value == io_pin_off) {
    clear();
  }
}

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

irs::arm::io_pin_t::io_pin_t(arm_port_t &a_port, irs_u8 a_bit, dir_t a_dir,
  io_pin_value_t a_value
):
  m_port(reinterpret_cast<irs_u32>(&a_port)),
  m_bit(a_bit),
  m_data_mask(0x04 << a_bit),
  m_port_mask(1 << m_bit)
{
  port_clock_on(m_port);
  #ifdef __LM3SxBxx__
    HWREG(m_port + GPIO_LOCK) = GPIO_UNLOCK_VALUE;
    HWREG(m_port + GPIO_CR) |= m_port_mask;
  #endif // __LM3SxBxx__
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    HWREG(m_port + GPIO_DEN) |= m_port_mask;
    HWREG(m_port + GPIO_AFSEL) &= ~m_port_mask;
    HWREG(m_port + GPIO_ODR) &= ~m_port_mask;
    if (a_dir == dir_in) {
      HWREG(m_port + GPIO_DIR) &= ~m_port_mask;
    } else if (a_dir == dir_out) {
      HWREG(m_port + GPIO_DIR) |= m_port_mask;
      HWREG(m_port + GPIO_DATA + m_data_mask) = 0;
    }
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32F2xx)
    set_dir(a_dir);
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
  if (a_dir == dir_out) {
    set_state(a_value);
  }
}

irs::arm::io_pin_t::~io_pin_t()
{
  return;
}

bool irs::arm::io_pin_t::pin()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    return HWREG(m_port + GPIO_DATA + m_data_mask);
  #elif defined(__STM32F100RBT__)
    return (HWREG(m_port + IDR) & m_port_mask);
  #elif defined(IRS_STM32F2xx)
    return (HWREG(m_port + GPIO_IDR_S) & m_port_mask);
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::io_pin_t::set()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    HWREG(m_port + GPIO_DATA + m_data_mask) = 0xFF;
  #elif defined(__STM32F100RBT__)
    HWREG(m_port + ODR) |= m_port_mask;
  #elif defined(IRS_STM32F2xx)
    HWREG(m_port + GPIO_ODR_S) |= m_port_mask;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::io_pin_t::clear()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    HWREG(m_port + GPIO_DATA + m_data_mask) = 0;
  #elif defined(__STM32F100RBT__)
    HWREG(m_port + ODR) &= ~m_port_mask;
  #elif defined(IRS_STM32F2xx)
    HWREG(m_port + GPIO_ODR_S) &= ~m_port_mask;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void set_pin_dir(const irs_u32 a_port,  const irs_u8 a_bit,
  const irs::io_t::dir_t a_dir)
{  
  switch (a_dir) {
    case irs::io_t::dir_in: {  
      HWREG(a_port + GPIO_PUPDR_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_PUPDR_S) |= GPIO_PUPDR_FLOAT << 2*a_bit;
      HWREG(a_port + GPIO_MODER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_MODER_S) |= GPIO_MODER_INPUT << 2*a_bit;
      HWREG(a_port + GPIO_OTYPER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_OTYPER_S) |=
        GPIO_OTYPER_OUTPUT_PUSH_PULL << 2*a_bit;
    } break;
    case irs::io_t::dir_in_pull_up: {
      HWREG(a_port + GPIO_PUPDR_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_PUPDR_S) |= GPIO_PUPDR_PULL_UP << 2*a_bit;
      HWREG(a_port + GPIO_MODER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_MODER_S) |= GPIO_MODER_INPUT << 2*a_bit;
      HWREG(a_port + GPIO_OTYPER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_OTYPER_S) |=
        GPIO_OTYPER_OUTPUT_PUSH_PULL << 2*a_bit;
    } break;
    case irs::io_t::dir_in_pull_down: {
      HWREG(a_port + GPIO_PUPDR_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_PUPDR_S) |= GPIO_PUPDR_PULL_DOWN << 2*a_bit;
      HWREG(a_port + GPIO_MODER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_MODER_S) |= GPIO_MODER_INPUT << 2*a_bit;
      HWREG(a_port + GPIO_OTYPER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_OTYPER_S) |=
        GPIO_OTYPER_OUTPUT_PUSH_PULL << 2*a_bit;
    } break;
    case irs::io_t::dir_out: {
      HWREG(a_port + GPIO_PUPDR_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_PUPDR_S) |= GPIO_PUPDR_FLOAT << 2*a_bit;
      HWREG(a_port + GPIO_MODER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_MODER_S) |= GPIO_MODER_OUTPUT << 2*a_bit;
      HWREG(a_port + GPIO_OTYPER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_OTYPER_S) |=
        GPIO_OTYPER_OUTPUT_PUSH_PULL << 2*a_bit;
    } break;
    case irs::io_t::dir_open_drain: {
      HWREG(a_port + GPIO_PUPDR_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_PUPDR_S) |= GPIO_PUPDR_FLOAT << 2*a_bit;
      HWREG(a_port + GPIO_MODER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_MODER_S) |= GPIO_MODER_OUTPUT << 2*a_bit;
      HWREG(a_port + GPIO_OTYPER_S) &= ~(3 << 2*a_bit);
      HWREG(a_port + GPIO_OTYPER_S) |=
        GPIO_OTYPER_OUTPUT_OPEN_DRAIN << 2*a_bit;
    } break;
  }
}

void irs::arm::io_pin_t::set_dir(dir_t a_dir)
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    if (a_dir == dir_in) {
      HWREG(m_port + GPIO_DIR) &= ~m_port_mask;
    } else if (a_dir == dir_out) {
      HWREG(m_port + GPIO_DIR) |= m_port_mask;
    }
  #elif defined(__STM32F100RBT__)
    irs_u32 cfg_reg_offset = CRL;
    irs_u32 cfg_mask_offset = m_bit * GPIO_MASK_SIZE;
    if (m_bit > 7) {
      cfg_mask_offset = (m_bit - 8) * GPIO_MASK_SIZE;
      cfg_reg_offset = CRH;
    }
    irs_u32 clr_mask = GPIO_FULL_MASK << cfg_mask_offset;
    irs_u32 set_mask = 0;
    switch (a_dir) {
      case dir_in:          set_mask = GPIO_FLOAT_IN_MASK;        break;
      case dir_out:         set_mask = GPIO_PUSHPULL_OUT_MASK;    break;
      case dir_open_drain:  set_mask = GPIO_OPEN_DRAIN_OUT_MASK;  break;
    }
    set_mask <<= cfg_mask_offset;
    HWREG(m_port + cfg_reg_offset) &= ~clr_mask;
    HWREG(m_port + cfg_reg_offset) |= set_mask;
  #elif defined(IRS_STM32F2xx)
    set_pin_dir(m_port, m_bit, a_dir);
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

irs::arm::io_port_t::io_port_t(arm_port_t &a_port, data_t a_mask,
  dir_t a_dir, irs_u8 a_shift):
  m_port(reinterpret_cast<irs_u32>(&a_port)),
  m_mask(a_mask << a_shift),
  m_shift(a_shift)
{
  port_clock_on(m_port);
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    volatile dir_t dir = a_dir;//
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32F2xx)
    set_dir(a_dir);
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

irs::arm::io_port_t::~io_port_t()
{
}

irs::arm::io_port_t::data_t irs::arm::io_port_t::get()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    return 0;
  #elif defined(__STM32F100RBT__)
    return (HWREG(m_port + IDR) & m_mask) >> m_shift;
  #elif defined(IRS_STM32F2xx)
    return (HWREG(m_port + GPIO_IDR_S) & m_mask) >> m_shift;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::io_port_t::set(data_t a_data)
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    volatile data_t data = a_data;//
  #elif defined(__STM32F100RBT__)
    irs_u32 set_data = (a_data << m_shift) & m_mask;
    irs_u32 clr_data = ~(a_data << m_shift) & m_mask;
    HWREG(m_port + BSRR) = set_data;
    HWREG(m_port + BSRR) = clr_data << GPIO_WIDTH;
  #elif defined(IRS_STM32F2xx)
    irs_u32 set_data = (a_data << m_shift) & m_mask;
    irs_u32 clr_data = ~(a_data << m_shift) & m_mask;
    HWREG(m_port + GPIO_BSRR_S) = set_data;
    HWREG(m_port + GPIO_BSRR_S) = clr_data << GPIO_WIDTH;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::io_port_t::set_dir(dir_t a_dir)
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    volatile dir_t dir = a_dir;
  #elif defined(__STM32F100RBT__)
    for (irs_u8 bit = 0; bit < GPIO_WIDTH; bit++) {
      if (m_mask & (1 << bit)) {
        irs_u32 cfg_reg_offset = CRL;
        irs_u32 cfg_mask_offset = bit * GPIO_MASK_SIZE;
        if (bit > 7) {
          cfg_mask_offset = (bit - 8) * GPIO_MASK_SIZE;
          cfg_reg_offset = CRH;
        }
        irs_u32 clr_mask = GPIO_FULL_MASK << cfg_mask_offset;
        irs_u32 set_mask = 0;
        switch (a_dir) {
          case dir_in:          set_mask = GPIO_FLOAT_IN_MASK;        break;
          case dir_out:         set_mask = GPIO_PUSHPULL_OUT_MASK;    break;
          case dir_open_drain:  set_mask = GPIO_OPEN_DRAIN_OUT_MASK;  break;
        }
        set_mask <<= cfg_mask_offset;
        HWREG(m_port + cfg_reg_offset) &= ~clr_mask;
        HWREG(m_port + cfg_reg_offset) |= set_mask;
      }
    }
  #elif defined(IRS_STM32F2xx)
    for (irs_u8 bit = 0; bit < GPIO_WIDTH; bit++) {
      if (m_mask & (1 << bit)) {
        set_pin_dir(m_port, bit, a_dir);
      }
    }
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

#endif //__ICCARM__
