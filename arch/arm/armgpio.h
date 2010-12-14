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
  const p_arm_port_t mp_port;
  const irs_u16 m_data_mask;
  const irs_u8 m_port_mask;
  
  
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
    RCGC2 |= (1 << port_number);
    for (irs_u32 i = 10; i; i--);
  }
};

} // namespace arm
} // namespace irs

#endif // armgpioH
