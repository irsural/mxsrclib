// Локализация ИРС
// Дата: 12.06.2010
// Ранняя дата: 12.04.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irslocale.h>

#include <irsfinal.h>

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

irs::locale_manager_t::locale_manager_t():
  #if defined(IRS_WIN32)
  m_loc("Russian_Russia.1251")
  #else // Compilers
  m_loc("")
  #endif // Compilers
{
  locale::global(m_loc);
}

irs::locale_manager_t& irs::loc()
{
  static locale_manager_t loc_manager;
  return loc_manager;
}

#endif //IRS_FULL_STDCPPLIB_SUPPORT

