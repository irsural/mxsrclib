// Локализация ИРС
// Дата 7.10.2009

#ifndef __embedded_cplusplus 

#include <irslocale.h>

irs::locale_manager_t::locale_manager_t():
  m_loc()
{
  #if defined(IRS_WIN32)
  m_loc = locale("Russia_Russian.1251");
  #elif defined(IRS_LINUX)
  m_loc = locale("");
  #endif // Compilers
  locale::global(m_loc);
}

irs::locale_manager_t& irs::loc()
{
  static locale_manager_t loc_manager;
  return loc_manager;
}

#endif // __embedded_cplusplus
