//! \file
//! \ingroup drivers_group
//! \brief Клавиатура
//!
//! Дата: 12.03.2010\n
//! Дата создания: 11.03.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irskbd.h>

#ifdef IRS_LINUX

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <irscpp.h>
#include <mxdata.h>
#include <timer.h>

#endif //IRS_LINUX

#include <irsfinal.h>

//! \addtogroup drivers_group
//! @{

#ifdef IRS_LINUX

irs::gcc_linux::keyboard_t::keyboard_t():
  m_terminal(0),
  m_old_terminal(zero_struct_t<termios>::get()),
  m_new_terminal(zero_struct_t<termios>::get())
{
  m_terminal = open("/dev/tty", O_RDONLY);

  tcgetattr(m_terminal, &m_old_terminal);
  tcgetattr(m_terminal, &m_new_terminal);

  m_new_terminal.c_lflag = 0;
  m_new_terminal.c_iflag &= ~(ICRNL|IGNCR|INLCR|IXON|IXOFF);
  m_new_terminal.c_cc[VMIN] = 1;
  m_new_terminal.c_cc[VTIME] = 0;
  tcsetattr(m_terminal, TCSADRAIN, &m_new_terminal);

  fcntl(m_terminal, F_SETFL, O_NDELAY);
}

irs::gcc_linux::keyboard_t::~keyboard_t()
{
  fcntl(m_terminal, F_SETFL, 0);
  tcsetattr(m_terminal, TCSADRAIN, &m_old_terminal);
}

char irs::gcc_linux::keyboard_t::key()
{
  char key_code = key_none;
  if(read(m_terminal, static_cast<void*>(&key_code), 1) == 1) {
    return key_code;
  } else {
    return key_none;
  }
}

#endif //IRS_LINUX

// class mxkey_drv_mc_t
irs::mxkey_drv_mc_t::mxkey_drv_mc_t():
  m_keys(),
  m_horizontal_pins(),
  m_vertical_pins(),
  m_timer(make_cnt_ms(5)),
  m_current_key(irskey_none)
{
}

irskey_t irs::mxkey_drv_mc_t::operator()()
{
  if (m_timer.check()) {
    for (size_t h = 0; h < m_horizontal_pins.size(); h++) {
      if (!(m_horizontal_pins[h]->pin())) {
        size_t key_index = h*m_vertical_pins.size() + m_vertical_pin_index;
        if (key_index < m_keys.size()) {
          m_current_key = m_keys[key_index];
        } else {
          m_current_key = irskey_none;
        }
        IRS_DBG_MSG(irst("key = ") << (int)m_current_key);
        return m_current_key;
      }
    }
    m_vertical_pins[m_vertical_pin_index]->set();
    m_vertical_pin_index++;
    if (m_vertical_pin_index >= m_vertical_pins.size()) {
      m_vertical_pin_index = 0;
      m_current_key = irskey_none;
    }
    m_vertical_pins[m_vertical_pin_index]->clear();
  }
  return m_current_key;
}

void irs::mxkey_drv_mc_t::add_key(irskey_t a_irskey)
{
  m_keys.push_back(a_irskey);
}

void irs::mxkey_drv_mc_t::clear_keys()
{
  m_keys.clear();
}

void irs::mxkey_drv_mc_t::add_horizontal_pin(gpio_pin_t* ap_pin)
{
  m_horizontal_pins.push_back(ap_pin);
}

void irs::mxkey_drv_mc_t::add_vertical_pin(gpio_pin_t* ap_pin)
{
  m_vertical_pins.push_back(ap_pin);
  ap_pin->set();
}

void irs::mxkey_drv_mc_t::add_horizontal_pins(
  vector<irs::handle_t<irs::gpio_pin_t> >* ap_pins)
{
  for (size_t i = 0; i < ap_pins->size(); i++) {
    m_horizontal_pins.push_back((*ap_pins)[i].get());
  }
}

void irs::mxkey_drv_mc_t::add_vertical_pins(
  vector<irs::handle_t<irs::gpio_pin_t> >* ap_pins)
{
  for (size_t i = 0; i < ap_pins->size(); i++) {
    m_vertical_pins.push_back((*ap_pins)[i].get());
    (*ap_pins)[i]->set();
  }
}

void irs::mxkey_drv_mc_t::clear_pins()
{
  m_horizontal_pins.clear();
  m_vertical_pins.clear();
}

void irs::set_default_keys(mxkey_drv_mc_t* ap_mxkey_drv_mc)
{
  ap_mxkey_drv_mc->clear_keys();
  ap_mxkey_drv_mc->add_key(irskey_7);
  ap_mxkey_drv_mc->add_key(irskey_8);
  ap_mxkey_drv_mc->add_key(irskey_9);
  ap_mxkey_drv_mc->add_key(irskey_escape);
  ap_mxkey_drv_mc->add_key(irskey_4);
  ap_mxkey_drv_mc->add_key(irskey_5);
  ap_mxkey_drv_mc->add_key(irskey_6);
  ap_mxkey_drv_mc->add_key(irskey_up);
  ap_mxkey_drv_mc->add_key(irskey_1);
  ap_mxkey_drv_mc->add_key(irskey_2);
  ap_mxkey_drv_mc->add_key(irskey_3);
  ap_mxkey_drv_mc->add_key(irskey_down);
  ap_mxkey_drv_mc->add_key(irskey_0);
  ap_mxkey_drv_mc->add_key(irskey_point);
  ap_mxkey_drv_mc->add_key(irskey_backspace);
  ap_mxkey_drv_mc->add_key(irskey_enter);
}

//! @}
