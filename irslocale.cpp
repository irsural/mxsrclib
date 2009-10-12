// Локализация ИРС
// Дата: 12.10.2009

#include <irslocale.h>

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
