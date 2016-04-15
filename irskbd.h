//! \file
//! \ingroup drivers_group
//! \brief Клавиатура
//!
//! Дата: 15.03.2010\n
//! Дата создания: 11.03.2010

#ifndef IRSKBDH
#define IRSKBDH

#include <irsdefs.h>

#ifdef __ICCARM__

#include <armcfg.h>

#endif  // __ICCARM__

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

#ifdef IRS_STM32_F2_F4_F7

class encoder_drv_mc_t: public encoder_drv_t
{
public:
  encoder_drv_mc_t(gpio_channel_t a_gpio_channel_1,
    gpio_channel_t a_gpio_channel_2, size_t a_timer_address);
  virtual int get_press_count();
  virtual irskey_t get_key_encoder();
  virtual irskey_t get_key_button();
  void add_key(irskey_t a_irskey);
  void add_press_down_pin(gpio_pin_t* ap_pin);
  void clear_keys();
private:
  vector<irskey_t> m_keys;
  loop_timer_t m_timer;
  irskey_t m_current_key;
  gpio_pin_t* mp_press_down_pin;
  tim_regs_t* mp_timer;
  irs_i16 m_curr_count;
  irs_i16 m_count_min;
  irs_i16 m_count_max;
  irskey_t m_result_key;
  int m_delta;
  int m_press_count;
};

void set_default_keys(encoder_drv_mc_t* ap_encoder_drv_mc);

#endif  // IRS_STM32_F2_F4_F7

#endif  //  __ICCARM__ || __ICCAVR__

#ifdef __BORLANDC__
class mouse_drv_builder_t: public encoder_drv_t
{
public:
  mouse_drv_builder_t();
  virtual int get_press_count();
  virtual irskey_t get_key_encoder();
  virtual irskey_t get_key_button();
  template <class F>
  void connect(F* ap_f)
  {
    ap_f->OnMouseWheel = MouseWheel;
    ap_f->OnMouseDown = MouseDown;
    ap_f->OnMouseUp = MouseUp;
  }
  void set_key_button_left(irskey_t a_irskey);
  void set_key_button_right(irskey_t a_irskey);
  void set_key_button_middle(irskey_t a_irskey);
  void set_key_wheel_rotation_up(irskey_t a_irskey);
  void set_key_wheel_rotation_down(irskey_t a_irskey);
private:
  void __fastcall MouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta,
    TPoint &MousePos, bool &Handled);
  void __fastcall MouseDown(TObject *Sender, TMouseButton Button,
    TShiftState Shift, int X, int Y);
  void __fastcall MouseUp(TObject *Sender, TMouseButton Button,
    TShiftState Shift, int X, int Y);
  loop_timer_t m_timer;
  int m_delta;
  int m_press_count;
  irskey_t m_key_button_left;
  irskey_t m_key_button_right;
  irskey_t m_key_button_middle;
  irskey_t m_key_wheel_rotation_up;
  irskey_t m_key_wheel_rotation_down;
  irskey_t m_key_button;
};
#endif // __BORLANDC__

} // namespace irs

#endif //IRSKBDH
