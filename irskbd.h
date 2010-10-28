//! \file
//! \ingroup drivers_group
//! \brief Клавиатура
//!
//! Дата: 15.03.2010\n
//! Дата создания: 11.03.2010

#ifndef IRSKBDH
#define IRSKBDH

#include <irsdefs.h>

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

#endif //IRSKBDH
