//! \file
//! \ingroup drivers_group
//! \brief Клавиатура
//!
//! Дата: 15.03.2010\n
//! Дата создания: 11.03.2010

#ifndef IRSKBDH
#define IRSKBDH

#include <irsdefs.h>

#include <irsstd.h>

#ifdef IRS_LINUX

#include <sys/termios.h>

namespace irs {

namespace gcc_linux {

//! \addtogroup drivers_group
//! @{

//! \author Sergeev Sergey
//! \brief Драйвер клавиатуры для Linux
class keyboard_t
{
public:
  enum {
    key_none = 0,
    key_escape = 27
  };
  keyboard_t();
  ~keyboard_t();
  char key();

private:
  int m_terminal;
  struct termios m_old_terminal;
  struct termios m_new_terminal;
};

//! @}

} //namespace gcc_linux

} //namespace irs

#endif //IRS_LINUX

namespace irs {

#if defined(__ICCARM__) || defined(__ICCAVR__)
// Класс драйвера клавиатуры микроконтроллера
class mxkey_drv_mc_t: public mxkey_drv_t
{
public:
  mxkey_drv_mc_t();
  virtual irskey_t operator()();
  void add_key(irskey_t a_irskey);
  void clear_keys();
  void add_horizontal_pin(gpio_pin_t* ap_pin);
  void add_vertical_pin(gpio_pin_t* ap_pin);
  void add_horizontal_pins(vector<irs::handle_t<irs::gpio_pin_t> >* ap_pins);
  void add_vertical_pins(vector<irs::handle_t<irs::gpio_pin_t> >* ap_pins);
  void clear_pins();
private:
  vector<irskey_t> m_keys;
  vector<gpio_pin_t*> m_horizontal_pins;
  vector<gpio_pin_t*> m_vertical_pins;
  loop_timer_t m_timer;
  irskey_t m_current_key;
  size_t m_vertical_pin_index;
};

void set_default_keys(mxkey_drv_mc_t* ap_mxkey_drv_mc);
#endif  //  __ICCARM__ || __ICCAVR__

} // namespace irs

#endif //IRSKBDH
