#ifndef IRSKBDH
#define IRSKBDH

#include <irsdefs.h>

#ifdef IRS_LINUX 

#include <sys/termios.h>

namespace irs {

namespace gcc_linux {

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

} //namespace gcc_linux

} //namespace irs

#endif //IRS_LINUX

#endif //IRSKBDH
