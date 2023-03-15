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
  #if (defined(__ICCARM__) && __VER__ >= 8000000)
  // Добавлено для совместимости с библиотекой TouchGFX,
  // там используются 2-байтные символы, а в IAR 8+
  // sizeof(wchar_t) == 4
  m_loc = locale(m_loc, new irs_numpunct_t<uint16_t>);
  #endif
  locale::global(m_loc);
  setlocale(LC_ALL, m_locale_name_def);
}

irs::locale_manager_t& irs::loc()
{
  static locale_manager_t loc_manager;
  return loc_manager;
}

#endif //defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

