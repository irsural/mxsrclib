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

#include <irsfinal.h>

namespace irs
{
namespace arm
{

typedef irs_u32 volatile 

class io_pin_t: public gpio_pin_t
{
public:
  io_pin_t();
  ~io_pin_t();
}

} // namespace arm
} // namespace irs

#endif // armgpioH