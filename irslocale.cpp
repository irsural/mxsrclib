//! \file
//! \brief Локализация ИРС
//!
//! Дата: 12.06.2010\n
//! Ранняя дата: 12.04.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irslocale.h>

#include <irsfinal.h>

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

const char* irs::locale_manager_t::m_locale_name_def =
  #if defined(IRS_WIN32) && !defined(__MINGW32__)
  "Russian_Russia.1251";
  #else // Compilers
  "";
  #endif // Compilers

irs::locale_manager_t::locale_manager_t():
  m_loc(m_locale_name_def)
{
  m_loc = locale(m_loc, new irs_numpunct_t<char>);
  m_loc = locale(m_loc, new irs_numpunct_t<wchar_t>);
  locale::global(m_loc);
  setlocale(LC_ALL, m_locale_name_def);
}

irs::locale_manager_t& irs::loc()
{
  static locale_manager_t loc_manager;
  return loc_manager;
}

#endif //defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

