//! \file
//! \ingroup drivers_group
//! \brief Базовый класс для дискретного ввода/вывода
//!
//! Дата: 2.02.2009

#ifndef irsgpioH
#define irsgpioH

#include <irsdefs.h>

namespace irs {

//! \addtogroup drivers_group
//! @{

enum port_dir_t
{
  dir_in,
  dir_out
};

class gpio_pin_t
{
public:
  enum dir_t
  {
    dir_in,
    dir_out
  };
  virtual ~gpio_pin_t() {}
  virtual bool pin() = 0;
  virtual void set() = 0;
  virtual void clear() = 0;
  virtual void set_dir(dir_t a_dir) = 0;
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

//! @}

#endif //irsgpioH
