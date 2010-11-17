//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для ARM
//!
//! Дата: 10.11.2010

#ifndef armgpioH
#define armgpioH

#include <irsdefs.h>

#include <irsstd.h>
#include <irsgpio.h>
#include <armioregs.h>

#include <irsfinal.h>

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
    den_idx = 0x51C,
    dir_idx = 0x400,
    data_idx = 0
  };
  enum {
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
  
  const irs_u8 m_mask;
  const p_arm_port_t mp_port;
  
  inline void clock_gating_control(p_arm_port_t ap_port)
  {
    switch(*ap_port) {
      case PORTA_BASE: {
        RCGC2_bit.PORTA = 1;
      } break;
      case PORTB_BASE: {
        RCGC2_bit.PORTB = 1;
      } break;
      case PORTC_BASE: {
        RCGC2_bit.PORTC = 1;
      } break;
      case PORTD_BASE: {
        RCGC2_bit.PORTD = 1;
      } break;
      case PORTE_BASE: {
        RCGC2_bit.PORTE = 1;
      } break;
      case PORTF_BASE: {
        RCGC2_bit.PORTF = 1;
      } break;
      case PORTG_BASE: {
        RCGC2_bit.PORTG = 1;
      } break;
      case PORTH_BASE: {
        RCGC2_bit.PORTH = 1;
      } break;
      case PORTJ_BASE: {
        RCGC2_bit.PORTJ = 1;
      } break;
    }
  }
  inline arm_port_t& den() { return *(mp_port + den_idx);}
  inline arm_port_t& dir() { return *(mp_port + dir_idx);}
  inline arm_port_t& data() { return *(mp_port + data_idx);}
};

} // namespace arm
} // namespace irs

#endif // armgpioH
