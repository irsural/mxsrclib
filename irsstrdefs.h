// Определения для автоматического переключения строк между char и wchar_t
// Дата: 27.09.2009
// Дата создания: 17.09.2009

#ifndef IRSSTRDEFSH
#define IRSSTRDEFSH

#include <irsdefs.h>
#include <irsstring.h>
//#include <mxdata.h>
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#include <stdexcept>
#endif //IRS_FULL_STDCPPLIB_SUPPORT
#include <irscpp.h>

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

#endif //IRS_FULL_STDCPPLIB_SUPPORT

inline const char* strchru(const char* a_str, int a_ch)
{
  return strchr(a_str, a_ch);
}
inline const wchar_t* strchru(const wchar_t* a_str, wint_t a_ch)
{
  return wcschr(a_str, a_ch);
}
inline const char_t* strchrt(const char_t* a_str, char_int_t a_ch)
{
  return strchru(a_str, a_ch);
}

inline size_t strlenu(const char* a_str)
{
  return strlen(a_str);
}
inline size_t strlenu(const wchar_t* a_str)
{
  return wcslen(a_str);
}
inline size_t strlent(const char_t* a_str)
{
  strlenu(a_str);
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
inline const char* cstr_from_codecvt_result_helper(
  codecvt_base::result a_result,
  char
)
{
  switch (a_result) {
    case codecvt_base::ok: {
      return "ok";
    } break;
    case codecvt_base::partial: {
      return "partial";
    } break;
    case codecvt_base::error: {
      return "error";
    } break;
    case codecvt_base::noconv: {
      return "noconv";
    } break;
  }
  return "unknown";
}
inline const wchar_t* cstr_from_codecvt_result_helper(
  codecvt_base::result a_result,
  wchar_t
)
{
  switch (a_result) {
    case codecvt_base::ok: {
      return L"ok";
    } break;
    case codecvt_base::partial: {
      return L"partial";
    } break;
    case codecvt_base::error: {
      return L"error";
    } break;
    case codecvt_base::noconv: {
      return L"noconv";
    } break;
  }
  return L"unknown";
}

//#if !defined(__WATCOMC__) && !defined(__ICCAVR__)
template <class T>
const char* cstr_from_codecvt_result(codecvt_base::result a_result)
{
  return cstr_from_codecvt_result_helper(a_result, T());
}

inline codecvt_base::result code_convert(mbstate_t state,
  const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
  char* to, char* to_end, char*& to_next)
{
  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  return cdcvt.out(state, from, from_end, from_next,
    to, to_end, to_next);
}
inline codecvt_base::result code_convert(mbstate_t state,
  const char* from, const char* from_end, const char*& from_next,
  wchar_t* to, wchar_t* to_end, wchar_t*& to_next)
{
  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  return cdcvt.in(state, from, from_end, from_next,
    to, to_end, to_next);
}

// Преобразование из строки in_char_type* в строку out_char_type*
// Доступны только варианты:
// I = wchar_t, O = char
// I = char, O = wchar_t
template <class I, class O>
class convert_str_t
{
public:
  typedef I in_char_type;
  typedef O out_char_type;
  //typedef irs::raw_data_t<out_char_type> data_type;
private:
  enum {
    m_size_default = static_cast<size_t>(-1)
  };
  out_char_type* mp_outstr;
public:
  convert_str_t(const in_char_type* ap_instr,
    size_t a_instr_size = m_size_default
  ):
    mp_outstr(IRS_NULL)
  {
    IRS_STATIC_ASSERT(
      ((irs::type_detect_t<I>::index == irs::char_idx) &&
       (irs::type_detect_t<O>::index == irs::wchar_idx)) ||
      ((irs::type_detect_t<I>::index == irs::wchar_idx) &&
       (irs::type_detect_t<O>::index == irs::char_idx))
    );

    if (a_instr_size == m_size_default) {
      a_instr_size = strlenu(ap_instr) + 1;
    }
    //mp_outstr.resize(a_instr_size);
    mp_outstr = new out_char_type[a_instr_size];

    const in_char_type* p_instr_next = ap_instr;
    const in_char_type* p_outstr_next = mp_outstr;

    mbstate_t state;
    codecvt_base::result convert_result =
      code_convert(
        state,

        ap_instr,
        ap_instr + a_instr_size,
        p_instr_next,

        mp_outstr,
        mp_outstr + a_instr_size,
        p_outstr_next
      );
    if (convert_result != codecvt_base::ok)
    {
      stringstream stream;
      stream << "convert_str_t codecvt result: " <<
        cstr_from_codecvt_result<char>(convert_result);
      stream.flush();
      throw runtime_error(stream.str().c_str());
      //IRS_LIB_DBG_MSG("convert_str_t codecvt result: " <<
        //cstr_from_codecvt_result<char>(convert_result));
    }
  }
  ~convert_str_t()
  {
    IRS_ARRDELETE(mp_outstr);
  }
  operator const out_char_type*() const
  {
    return mp_outstr;
  }
  operator out_char_type*()
  {
    return mp_outstr;
  }
};
#endif //IRS_FULL_STDCPPLIB_SUPPORT

#ifdef IRS_UNICODE
#define IRS_WIDE_CHAR_FROM_TCHAR_STR(str) (str)
#define IRS_SIMPLE_CHAR_FROM_TCHAR_STR(str)\
  (irs::convert_str_t<wchar_t, char>(str))
#else //IRS_UNICODE
#define IRS_WIDE_CHAR_FROM_TCHAR_STR(str)\
  (irs::convert_str_t<char, wchar_t>(str))
#define IRS_SIMPLE_CHAR_FROM_TCHAR_STR(str) (str)
#endif //IRS_UNICODE

} //namespace irs

#endif //IRSSTRDEFSH
