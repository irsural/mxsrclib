//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для ARM
//!
//! Дата: 22.11.2010
//! Дата создания: 10.11.2010

#ifndef armgpioH
#define armgpioH

#include <irsdefs.h>

#include <irsstd.h>
#include <irsgpio.h>
#include <armioregs.h>

#include <irsfinal.h>

#define HWREG(x) (*((volatile irs_u32*)(x)))

namespace irs
{
namespace arm
{

class io_pin_t: public gpio_pin_t
{
public:
  io_pin_t(arm_port_t &a_port, irs_u8 a_bit, dir_t a_dir);
  ~io_pin_t();
  virtual bool pin();
  virtual void set();
  virtual void clear();
  virtual void set_dir(dir_t a_dir);
private:
  enum {
    SYSTEM_CONTROL_BASE = 0x400FE000,
    RCGC2 = 0x108,
  
    PORTA_BASE = 0x40004000,
    PORTB_BASE = 0x40005000,
    PORTC_BASE = 0x40006000,
    PORTD_BASE = 0x40007000,
    PORTE_BASE = 0x40024000,
    PORTF_BASE = 0x40025000,
    PORTG_BASE = 0x40026000,
    PORTH_BASE = 0x40027000,
    PORTJ_BASE = 0x4003D000
  };
  enum gpio_port_bit {
    GPIO_DEN = 0x51C,
    GPIO_DIR = 0x400,
    GPIO_DATA = 0,
    GPIO_LOCK = 0x520
  };
  
  const p_arm_port_t mp_port;
  const irs_u32 m_bit;
  
  
  inline irs_u8 port_base_to_port_number(p_arm_port_t ap_port)
  {
    irs_u8 port_number = 0;
    switch(reinterpret_cast<irs_u32>(ap_port)) {
      case PORTA_BASE: {
        port_number = 0;
      } break;
      case PORTB_BASE: {
        port_number = 1;
      } break;
      case PORTC_BASE: {
        port_number = 2;
      } break;
      case PORTD_BASE: {
        port_number = 3;
      } break;
      case PORTE_BASE: {
        port_number = 4;
      } break;
      case PORTF_BASE: {
        port_number = 5;
      } break;
      case PORTG_BASE: {
        port_number = 6;
      } break;
      case PORTH_BASE: {
        port_number = 7;
      } break;
      case PORTJ_BASE: {
        port_number = 8;
      } break;
    }
    return port_number;
  }
  inline void clock_gating_control(p_arm_port_t ap_port)
  {
    irs_u8 port_number = port_base_to_port_number(ap_port);
    HWREG(SYSTEM_CONTROL_BASE + RCGC2) |= (1 << port_number);
  }
};

} // namespace arm
} // namespace irs

#endif // armgpioH
