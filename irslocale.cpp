//! \file
//! \brief ����������� ���
//!
//! ����: 12.06.2010\n
//! ������ ����: 12.04.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irslocale.h>

#include <irsfinal.h>

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

  #ifdef NOP
  //#if defined(IRS_WIN32)
  #ifdef __MINGW32__
  m_loc("UTF-8")
  #else //__MINGW32__
  m_loc("Russian_Russia.1251")
  #endif //__MINGW32__
  #endif //NOP

irs::locale_manager_t::locale_manager_t():
  #if defined(IRS_WIN32) && !defined(__MINGW32__)

  m_loc("Russian_Russia.1251")


  #else // Compilers
  m_loc("")
  #endif // Compilers
{
  m_loc = locale(m_loc, new irs_numpunct_t<char>);
  m_loc = locale(m_loc, new irs_numpunct_t<wchar_t>);
  locale::global(m_loc);
}

irs::locale_manager_t& irs::loc()
{
  static locale_manager_t loc_manager;
  return loc_manager;
}

#endif //IRS_FULL_STDCPPLIB_SUPPORT

