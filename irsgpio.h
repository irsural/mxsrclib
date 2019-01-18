//! \file
//! \ingroup drivers_group
//! \brief Базовый класс для дискретного ввода/вывода
//!
//! Дата: 2.02.2009

#ifndef irsgpioH
#define irsgpioH

#include <irsdefs.h>

#ifdef __ICCARM__
#ifndef IRS_STM32H7xx
# include <armioregs.h>
#endif // IRS_STM32H7xx
#endif //__ICCARM__

#ifdef IRS_STM32_F2_F4_F7
# include <armregs_stm32f2xx.h>
#endif // IRS_STM32_F2_F4_F7

#include <irsfinal.h>

//*****************************************************************************
//*********  Ахтунг! Ахтунг! irserror и irsstd сюда включать нельзя!  *********
//*****************************************************************************

namespace irs {

//! \addtogroup drivers_group
//! @{

struct io_t {
  enum dir_t {
    dir_in,
    dir_in_pull_up,
    dir_in_pull_down,
    dir_out,
    dir_open_drain
  };
};

enum io_pin_value_t
{
  io_pin_no_change = -1,
  io_pin_off = 0,
  io_pin_on = 1
};

//typedef io_t::dir_t port_dir_t;

class gpio_pin_t: public io_t
{
public:
  virtual ~gpio_pin_t() {}
  virtual bool pin() = 0;
  virtual void set() = 0;
  virtual void clear() = 0;
  virtual void set_dir(dir_t a_dir) = 0;
  void set_pin(bool a_pin);
  void pin(bool a_pin);
  void set_state(io_pin_value_t a_value);
};

// Класс для подключения gpio_pin_t
class bit_gpio_t
{
public:
  typedef int bit_t;

  inline bit_gpio_t(gpio_pin_t *a_pin = 0);
  inline void connect(gpio_pin_t *a_pin);
  inline bit_t operator=(const bit_t a_elem);
  inline operator bit_t();
private:
  gpio_pin_t *m_pin;
};

class gpio_port_t: public io_t
{
public:
  #ifdef __ICCAVR__
  typedef irs_u8 data_t;
  #else //__ICCAVR__
  typedef irs_u32 data_t;
  #endif //__ICCAVR__

  virtual ~gpio_port_t() {}
  virtual data_t get() = 0;
  virtual void set(data_t a_data) = 0;
  virtual void set_dir(dir_t a_dir) = 0;
};

} //namespace irs

// Класс для подключения gpio_pin_t
inline irs::bit_gpio_t::bit_gpio_t(irs::gpio_pin_t *a_pin):
  m_pin(a_pin)
{
}
inline void irs::bit_gpio_t::connect(irs::gpio_pin_t *a_pin)
{
  m_pin = a_pin;
}
inline irs::bit_gpio_t::bit_t irs::bit_gpio_t::operator=
  (const irs::bit_gpio_t::bit_t a_elem)
{
  if (a_elem) {
    m_pin->set();
  } else {
    m_pin->clear();
  }
  return a_elem;
}
inline irs::bit_gpio_t::operator bit_t()
{
  return m_pin->pin();
}


#ifdef __ICCAVR__

namespace irs
{
namespace avr
{

//! \addtogroup drivers_group
//! @{

class io_aux_port_t
{
public:
  enum {
    out_idx = 0,
    dir_idx = -1,
    in_idx = -2
  };

  io_aux_port_t(avr_port_t &a_port):
    mp_port(&a_port)
  {}

  inline avr_port_t& out() { return *(mp_port + out_idx); }
  inline avr_port_t& dir() { return *(mp_port + dir_idx); }
  inline avr_port_t& in() { return *(mp_port + in_idx); }
private:
  p_avr_port_t mp_port;
};

class io_pin_t: public gpio_pin_t, public io_aux_port_t
{
  irs_u8 m_mask;
public:
  io_pin_t(p_avr_port_t ap_write_port, p_avr_port_t ap_read_port,
    p_avr_port_t ap_dir_port, irs_u8 a_bit, dir_t a_dir, bool a_pull_up);
  io_pin_t(avr_port_t &a_port, irs_u8 a_bit, dir_t a_dir,
    bool a_pull_up = false);
  ~io_pin_t();
  virtual bool pin();
  virtual void set();
  virtual void clear();
  virtual void set_dir(dir_t a_dir);
};

class io_port_t: public gpio_port_t, public io_aux_port_t
{
public:
  // Крашенинников: В io_pin_t используется параметр a_pull_up.
  // В io_port_t для этого следует использовать параметр a_dir
  // со значением dir_in_pull_up
  io_port_t(avr_port_t &a_port, data_t a_mask, dir_t a_dir,
    irs_u8 a_shift = 0);
  virtual ~io_port_t();
  virtual data_t get();
  virtual void set(data_t a_data);
  virtual void set_dir(dir_t a_dir);
private:
  data_t m_mask;
  const irs_u8 m_shift;
};

class mem_out_register_t
{
  irs_u8 m_value;
  irs_u8 *mp_register;
public:
  mem_out_register_t(irs_uarc a_address, irs_u8 a_init_value);
  ~mem_out_register_t();
  inline irs_u8 get_value();
  inline void set_value(irs_u8 a_value);
};

class mem_out_pin_t : public gpio_pin_t
{
  mem_out_register_t *mp_write_register;
  irs_u8 m_bit;
public:
  mem_out_pin_t(mem_out_register_t *ap_write_register, irs_u8 a_bit,
    irs_u8 a_init_value);
  ~mem_out_pin_t();
  virtual bool pin();
  virtual void set();
  virtual void clear();
  virtual void set_dir(dir_t a_dir);
};

template
<
  avr_port_t &PORT,
  io_t::dir_t DIR,
  irs_u8 BIT,
  bool PULL_UP = false
>
class avr_pin_t
{
  enum
  {
    out_idx = 0,
    dir_idx = 1,
    in_idx = 2
  };
  irs_u8 x;
public:
  avr_pin_t();
  ~avr_pin_t();
  inline static void init();
  inline static bool pin();
  inline static void set();
  inline static void clear();
  inline static void set_dir(io_t::dir_t a_dir);
  inline static avr_port_t& out(avr_port_t& a_port);
  inline static avr_port_t& dir(avr_port_t& a_port);
  inline static avr_port_t& in(avr_port_t& a_port);
};

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
avr_pin_t<PORT, DIR, BIT, PULL_UP>::avr_pin_t()
{
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
avr_pin_t<PORT, DIR, BIT, PULL_UP>::~avr_pin_t()
{
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::init()
{
  if (DIR == io_t::dir_in)
  {
    dir(PORT) &= ((1 << BIT)^0xFF);
    if (PULL_UP) PORT |= (1 << BIT);
  }
  if (DIR == io_t::dir_out)
  {
    dir(PORT) |= (1 << BIT);
    PORT &= ((1 << BIT)^0xFF);
  }
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline bool avr_pin_t<PORT, DIR, BIT, PULL_UP>::pin()
{
  return (in(PORT) >> BIT) & 0x01;
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::set()
{
  PORT  |= (1 << BIT);
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::clear()
{
  PORT  &= ((1 << BIT)^0xFF);
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  set_dir(io_t::dir_t a_dir)
{
  if (a_dir == io_t::dir_out) dir(PORT) |= (1 << BIT);
  else dir(PORT) &= ((1 << BIT)^0xFF);
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline avr_port_t& avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  out(avr_port_t& a_port)
{
  return *(&a_port + out_idx);
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline avr_port_t& avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  dir(avr_port_t& a_port)
{
  return *(&a_port + dir_idx);
}

template <avr_port_t &PORT, io_t::dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline avr_port_t& avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  in(avr_port_t& a_port)
{
  return *(&a_port + in_idx);
}

//! @}

} // namespace avr

} // namespace irs

#endif //__ICCAVR__


#ifdef __ICCARM__
#ifndef IRS_STM32H7xx

namespace irs
{
namespace arm
{

class io_pin_t: public gpio_pin_t
{
public:
  io_pin_t(arm_port_t &a_port, irs_u8 a_bit, dir_t a_dir,
    io_pin_value_t a_value = io_pin_no_change);
  #ifdef IRS_STM32_F2_F4_F7
  io_pin_t(gpio_channel_t a_channel, dir_t a_dir,
    io_pin_value_t a_value = io_pin_no_change);
  #endif // IRS_STM32_F2_F4_F7
  ~io_pin_t();
  virtual bool pin();
  virtual void set();
  virtual void clear();
  virtual void set_dir(dir_t a_dir);
private:
  void init(dir_t a_dir, io_pin_value_t a_value);
  const irs_u32 m_port;
  const irs_u8 m_bit;
  const irs_u16 m_data_mask;
  const irs_u32 m_port_mask;
};

class io_port_t: public gpio_port_t
{
public:
  io_port_t(arm_port_t &a_port, data_t a_mask, dir_t a_dir,
    irs_u8 a_shift = 0);
  ~io_port_t();
  virtual data_t get();
  virtual void set(data_t a_data);
  virtual void set_dir(dir_t a_dir);
private:
  const irs_u32 m_port;
  const data_t m_mask;
  const irs_u8 m_shift;
};

inline irs_u8 port_base_to_port_number(irs_u32 a_port)
{
  irs_u8 port_number = 0;
  switch(a_port) {
  #if defined(__LM3SxBxx__)
    case PORTA_BASE: { port_number = 0; } break;
    case PORTB_BASE: { port_number = 1; } break;
    case PORTC_BASE: { port_number = 2; } break;
    case PORTD_BASE: { port_number = 3; } break;
    case PORTE_BASE: { port_number = 4; } break;
    case PORTF_BASE: { port_number = 5; } break;
    case PORTG_BASE: { port_number = 6; } break;
    case PORTH_BASE: { port_number = 7; } break;
    case PORTJ_BASE: { port_number = 8; } break;
  #elif defined(__LM3Sx9xx__)
    case PORTA_BASE: { port_number = 0; } break;
    case PORTB_BASE: { port_number = 1; } break;
    case PORTC_BASE: { port_number = 2; } break;
    case PORTD_BASE: { port_number = 3; } break;
    case PORTE_BASE: { port_number = 4; } break;
    case PORTF_BASE: { port_number = 5; } break;
    case PORTG_BASE: { port_number = 6; } break;
    case PORTH_BASE: { port_number = 7; } break;
  #elif defined(__STM32F100RBT__)
    case PORTA_BASE: { port_number = 0; } break;
    case PORTB_BASE: { port_number = 1; } break;
    case PORTC_BASE: { port_number = 2; } break;
    case PORTD_BASE: { port_number = 3; } break;
    case PORTE_BASE: { port_number = 4; } break;
  #elif defined(IRS_STM32_F2_F4_F7)
    case IRS_PORTA_BASE: { port_number = 0; } break;
    case IRS_PORTB_BASE: { port_number = 1; } break;
    case IRS_PORTC_BASE: { port_number = 2; } break;
    case IRS_PORTD_BASE: { port_number = 3; } break;
    case IRS_PORTE_BASE: { port_number = 4; } break;
    case IRS_PORTF_BASE: { port_number = 5; } break;
    case IRS_PORTG_BASE: { port_number = 6; } break;
    case IRS_PORTH_BASE: { port_number = 7; } break;
    case IRS_PORTI_BASE: { port_number = 8; } break;
    case IRS_PORTJ_BASE: { port_number = 9; } break;
    case IRS_PORTK_BASE: { port_number = 10; } break;
  #else
    // Тип контроллера не определён
    #define IRS_MC_NOT_DEFINED
  #endif  //  mcu type
    default: {
      for (;;) {}; // Неизвестный порт
    }
  }
  
  #pragma diag_suppress=Pe128
  #ifdef IRS_MC_NOT_DEFINED
  for (;;);
  #endif //IRS_MC_NOT_DEFINED
  #pragma diag_default=Pe128
  
  #pragma diag_suppress=Pe111
  return port_number;
  #pragma diag_default=Pe111
}

inline void port_clock_on(irs_u32 a_port)
{
  irs_u8 port_number = port_base_to_port_number(a_port);
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
    RCGC2 |= (1 << port_number);
    for (irs_u32 i = 10; i; i--);
  #elif defined(__STM32F100RBT__)
    RCC_APB2ENR |= (1 << (port_number + 2));
  #elif defined(IRS_STM32_F2_F4_F7)
    RCC_AHB1ENR |= (1 << port_number);
  #else
    // Тип контроллера не определён
    #pragma diag_suppress=Pe128
    for (;;);
    #pragma diag_default=Pe128
  #endif  //  mcu type
}

} // namespace arm
} // namespace irs

#endif // IRS_STM32H7xx
#endif //__ICCARM__

//! @}

#endif //irsgpioH
