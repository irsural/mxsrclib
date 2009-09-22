// Набор тестов
// Дата: 22.09.2009
// Дата создания: 20.09.2009

#ifndef IRSTESTH
#define IRSTESTH

#include <irsdefs.h>
#include <irscpp.h>

namespace irs {

namespace compiler_test {

namespace template_template_inheritance {

// Наследование шаблонов от шаблонов
template <class T>
struct base_t
{
  typedef size_t size_type;

  enum {
    bits_on_byte = 8
  };

  T f(T x)
  {
    return x*x;
  }
};
template <class T, class B = base_t<T> >
struct derived_t: B
{
};
inline void exec(ostream& a_strm)
{
  derived_t<double> derived;
  a_strm << "base_t::f call through derived_t::f: " << derived.f(1.1) << '\n';
  a_strm << "base_t::bits_on_byte call through derived_t::bits_on_byte: ";
  a_strm << derived_t<double>::bits_on_byte << '\n';
  a_strm << "base_t::size_type call through derived_t::size_type: ";
  derived_t<double>::size_type size = 7;
  a_strm << size << '\n';
  a_strm << endl;
}

} //namespace template_template_inheritance

} //namespace compiler_test


namespace cstr_convert {

template <class T>
int to_int(T a_char)
{
  return static_cast<int>(a_char);
}

template <class T>
class convert_to_char_t
{
public:
private:
};

template <class T>
class convert_from_char_t
{
public:
private:
};


inline irs::raw_data_t<char> char_from_wchar_str_dbg(
  const irs::raw_data_t<wchar_t>& ins)
{
  irs::raw_data_t<wchar_t>::const_pointer in_it = ins.data();

  irs::raw_data_t<char> outs(ins.size());
  irs::raw_data_t<char>::pointer out_it = outs.data();

  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  //typedef codecvt<char, wchar_t, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());

  mbstate_t state;
  cdcvt.out(state, ins.data(), ins.data() + ins.size(), in_it,
    outs.data(), outs.data() + outs.size(), out_it);

  return outs;
}

inline irs::raw_data_t<wchar_t> wchar_from_char_str_dbg(
  const irs::raw_data_t<char>& ins)
{
  irs::raw_data_t<char>::const_pointer in_it = ins.data();

  irs::raw_data_t<wchar_t> outs(ins.size());
  irs::raw_data_t<wchar_t>::pointer out_it = outs.data();

  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  //typedef codecvt<char, wchar_t, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());

  mbstate_t state;
  cdcvt.in(state, ins.data(), ins.data() + ins.size(), in_it,
    outs.data(), outs.data() + outs.size(), out_it);

  return outs;
}

inline void exec(ostream& strm)
{
  {
  wchar_t wstr[] = L"hello!?&";
  irs::raw_data_t<wchar_t> wstr_data(sizeof(wstr));
  irs::memcpyex(wstr_data.data(), wstr, sizeof(wstr));
  irs::raw_data_t<char> str_data(sizeof(wstr));
  str_data = char_from_wchar_str_dbg(wstr_data);
  IRS_DBG_MSG("*" << str_data.data() << "*");
  }

  {
  char wstr[] = "yes!?&";
  irs::raw_data_t<char> wstr_data(sizeof(wstr));
  irs::memcpyex(wstr_data.data(), wstr, sizeof(wstr));
  irs::raw_data_t<wchar_t> str_data(sizeof(wstr));
  str_data = wchar_from_char_str_dbg(wstr_data);
  //WideString bwstr = str_data.data();
  WideString bwstr = L"Привет";
  AnsiString bstr = bwstr;
  IRS_DBG_MSG("*" << bstr.c_str() << "*");
  }
}

} //namespace cstr_convert

} //namespace irs

#endif //IRSTESTH
