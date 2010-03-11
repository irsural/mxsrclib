#include <irskbd.h>

#ifdef IRS_LINUX

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <irscpp.h>
#include <mxdata.h>

#endif //IRS_LINUX

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
  char key_code = 0;
  if(read(m_terminal, static_cast<void*>(&key_code), 1) == 1) {
    return key_code;
  } else {
    return 0;
  }
}

#endif //IRS_LINUX
