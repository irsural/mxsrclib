//! \file
//! \brief Локализация ИРС
//!
//! Дата: 30.06.2010\n
//! Ранняя дата: 12.04.2010

#ifndef irslocaleH
#define irslocaleH

#include <irsdefs.h>

#include <irscpp.h>

#include <irsfinal.h> 

namespace irs {

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

class locale_manager_t
{
public:
  locale_manager_t();
  inline locale& get();
  void set(const locale& a_loc);
private:
  locale m_loc;
};

inline locale& locale_manager_t::get()
{
  return m_loc;
}

inline void locale_manager_t::set(const locale& a_loc)
{
  locale::global(a_loc);
  m_loc = a_loc;
}

locale_manager_t& loc();

template<class T>
class irs_numpunct_t: public numpunct<T>
{
public:
  explicit irs_numpunct_t(size_t r = 0):
    numpunct<T>(r)
  {
  }
protected:
  T do_decimal_point() const
  {
    return ',';
  }
  T do_thousands_sep() const
  {
    return ' ';
  }
};

#endif //IRS_FULL_STDCPPLIB_SUPPORT

} // namespace irs

#endif
