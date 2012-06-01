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
  static const char* m_locale_name_def;

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

template <class tmpl_char_type>
class irs_numpunct_t: public numpunct<tmpl_char_type>
{
public:
  typedef tmpl_char_type char_type;
  typedef basic_string<char_type> string_type;

  explicit irs_numpunct_t(size_t a_no_locale_delete = 0):
    numpunct<char_type>(a_no_locale_delete)
  {
  }
protected:
  char_type do_decimal_point() const
  {
    return ',';
  }
  char_type do_thousands_sep() const
  {
    return ' ';
  }
};

#endif //IRS_FULL_STDCPPLIB_SUPPORT

} // namespace irs

#endif
