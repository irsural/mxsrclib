// Определения для автоматического переключения строк между char и wchar_t
// Дата: 22.09.2009
// Дата создания: 17.09.2009

#ifndef IRSSTRDEFSH
#define IRSSTRDEFSH

#include <irsdefs.h>
#include <irsstring.h>
#include <mxdata.h>

namespace irs {

#ifdef IRS_UNICODE

#define irst(cstr) L##cstr

typedef wint_t char_int_t;
typedef wchar_t char_t;
typedef wstring string_t;
typedef wostream ostream_t;
typedef wistream istream_t;

#else //IRS_UNICODE

#define irst(cstr) cstr

typedef int char_int_t;
typedef char char_t;
typedef irs::string string_t;
typedef ostream ostream_t;
typedef istream istream_t;

#endif //IRS_UNICODE

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

inline bool isalnumt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::alnum,
    static_cast<char_t>(a_ch));
}
                                              
inline bool isalphat(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::alpha,
    static_cast<char_t>(a_ch));
}

inline bool iscntrlt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::cntrl,
    static_cast<char_t>(a_ch));
}

inline bool isdigitt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::digit,
    static_cast<char_t>(a_ch));
}

inline bool isgrapht(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::graph,
    static_cast<char_t>(a_ch));
}

inline bool islowert(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::lower,
    static_cast<char_t>(a_ch));
}

inline bool isprintt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::print,
    static_cast<char_t>(a_ch));
}

inline bool ispunctt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::punct,
    static_cast<char_t>(a_ch));
}

inline bool isspacet(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::space,
    static_cast<char_t>(a_ch));
}

inline bool isuppert(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::upper,
    static_cast<char_t>(a_ch));
}

inline bool isxdigitt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::xdigit,
    static_cast<char_t>(a_ch));
}

inline const char_t* strchrt(const char_t* a_str, char_int_t a_ch)
{
  #ifdef IRS_UNICODE
  return wcschr(a_str, a_ch);
  #else //IRS_UNICODE
  return strchr(a_str, a_ch);
  #endif //IRS_UNICODE
}

#else //IRS_FULL_STDCPPLIB_SUPPORT
inline bool isalnumt(char_int_t a_ch)
{
  return ::isalnum(a_ch);
}

inline bool isalphat(char_int_t a_ch)
{
  return ::isalpha(a_ch);
}

inline bool iscntrlt(char_int_t a_ch)
{
  return ::iscntrl(a_ch);
}

inline bool isdigitt(char_int_t a_ch)
{
  return ::isdigit(a_ch);
}

inline bool isgrapht(char_int_t a_ch)
{
  return ::isgraph(a_ch);
}

inline bool islowert(char_int_t a_ch)
{
  return ::islower(a_ch);
}

inline bool isprintt(char_int_t a_ch)
{
  return ::isprint(a_ch);
}

inline bool ispunctt(char_int_t a_ch)
{
  return ::ispunct(a_ch);
}

inline bool isspacet(char_int_t a_ch)
{
  return ::isspace(a_ch);
}

inline bool isuppert(char_int_t a_ch)
{
  return ::isupper(a_ch);
}

inline bool isxdigitt(char_int_t a_ch)
{
  return ::isxdigit(a_ch);
}

inline const char_t* strchrt(const char_t* a_str, int a_ch)
{
  return strchr(a_str, a_ch);
}

#endif //IRS_FULL_STDCPPLIB_SUPPORT

#if !defined(__WATCOMC__) && !defined(__ICCAVR__)
#ifndef NOP
inline irs::raw_data_t<char> char_from_wchar_str(
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
#endif //NOP
#endif //__WATCOMC__

} //namespace irs

#endif //IRSSTRDEFSH
