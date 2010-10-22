//! \file
//! \ingroup drivers_group
//! \brief Порты ввода-вывода для AVR
//!
//! Дата: 19.12.2008

#ifndef avrgpioH
#define avrgpioH

#include <irsdefs.h>
#include <irsstd.h>
#include <irsgpio.h>

namespace irs
{
namespace avr
{

//! \addtogroup drivers_group
//! @{

class io_pin_t : public gpio_pin_t
{
  enum
  {
    out_idx = 0,
    dir_idx = -1,
    in_idx = -2
  };
  const p_avr_port_t mp_port;
  const irs_u8 m_mask;

  inline avr_port_t& out() { return *(mp_port + out_idx); }
  inline avr_port_t& dir() { return *(mp_port + dir_idx); }
  inline avr_port_t& in() { return *(mp_port + in_idx); }
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
  port_dir_t DIR,
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
  inline static void set_dir(port_dir_t a_dir);
  inline static avr_port_t& out(avr_port_t& a_port);
  inline static avr_port_t& dir(avr_port_t& a_port);
  inline static avr_port_t& in(avr_port_t& a_port);
};

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
avr_pin_t<PORT, DIR, BIT, PULL_UP>::avr_pin_t()
{
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
avr_pin_t<PORT, DIR, BIT, PULL_UP>::~avr_pin_t()
{
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::init()
{
  if (DIR == dir_in)
  {
    dir(PORT) &= ((1 << BIT)^0xFF);
    if (PULL_UP) PORT |= (1 << BIT);
  }
  if (DIR == dir_out)
  {
    dir(PORT) |= (1 << BIT);
    PORT &= ((1 << BIT)^0xFF);
  }
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline bool avr_pin_t<PORT, DIR, BIT, PULL_UP>::pin()
{
  return (in(PORT) >> BIT) & 0x01;
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::set()
{
  PORT  |= (1 << BIT);
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::clear()
{
  PORT  &= ((1 << BIT)^0xFF);
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline void avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  set_dir(port_dir_t a_dir)
{
  if (a_dir == dir_out) dir(PORT)  |= (1 << BIT);
  else dir(PORT) &= ((1 << BIT)^0xFF);
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline avr_port_t& avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  out(avr_port_t& a_port)
{
  return *(&a_port + out_idx);
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline avr_port_t& avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  dir(avr_port_t& a_port)
{
  return *(&a_port + dir_idx);
}

template <avr_port_t &PORT, port_dir_t DIR, irs_u8 BIT, bool PULL_UP>
inline avr_port_t& avr_pin_t<PORT, DIR, BIT, PULL_UP>::
  in(avr_port_t& a_port)
{
  return *(&a_port + in_idx);
}

//! @}

} // namespace avr

} // namespace irs

#endif //avrgpioH
