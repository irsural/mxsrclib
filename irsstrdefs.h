// Определения для автоматического переключения строк между char и wchar_t
// Дата: 15.04.2010
// Дата создания: 17.09.2009

#ifndef IRSSTRDEFSH
#define IRSSTRDEFSH

#include <irsdefs.h>

#include <string.h>

#include <irscpp.h>
#include <irsstring.h>
#include <irslimits.h>
#include <irsexcept.h>
#include <irslocale.h>
#include <irsconfig.h>

#include <irsfinal.h>

#ifdef IRS_UNICODE
#define IRS_WIDE_FROM_TYPE_STR(str) (str)
#define IRS_SIMPLE_FROM_TYPE_STR(str)\
  (irs::convert_str_t<wchar_t, char>(str).get())
#else //IRS_UNICODE
#define IRS_WIDE_FROM_TYPE_STR(str)\
  (irs::convert_str_t<char, wchar_t>(str).get())
#define IRS_SIMPLE_FROM_TYPE_STR(str) (str)
#endif //IRS_UNICODE

// Поддержка экзотических строк типа __flash в IAR и др.
#ifdef IRS_LIB_FLASH_ASSERT
#define IRS_SPEC_CSTR_DECLARE(a_name, a_cstr)\
  static char IRS_ICCAVR_FLASH a_name[] = a_cstr;
#define IRS_SPEC_CSTR_ASSIGN(a_name, a_cstr)\
  {\
    static char IRS_ICCAVR_FLASH irs_spec_cstr_assign_name[] = a_cstr;\
    a_name = irs_spec_cstr_assign_name;\
  }
#else //IRS_LIB_FLASH_ASSERT
#define IRS_SPEC_CSTR_DECLARE(a_name, a_cstr)\
  static const char a_name[] = a_cstr;
#define IRS_SPEC_CSTR_ASSIGN(a_name, a_cstr) a_name = a_cstr;
#endif //IRS_LIB_FLASH_ASSERT

//#define IRSSTRDEFS_DEBUG
#ifdef IRSSTRDEFS_DEBUG
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
    #define IRSSTRDEFS_ASSERT(expr)\
      {\
        if (!(expr)) {\
          throw irs::assert_e(#expr, __FILE__, __LINE__, "", __FUNC__);\
        }\
      }
    #ifdef NOP
    #define IRSSTRDEFS_ASSERT(expr)\
      {\
        if (!(expr)) {\
          irs::string msg = "\nУтверждение: ";\
          msg = msg + #expr + "\nФайл: " + __FILE__ + "\nСтрока: ";\
          irs::string line = __LINE__;\
          msg = msg + line;\
          throw runtime_error(msg.c_str());\
        }\
      }
    #endif //NOP
  #else //IRS_FULL_STDCPPLIB_SUPPORT
    #define IRSSTRDEFS_ASSERT(expr)\
      {\
        if (!(expr)) {\
          for(;;);\
        }\
      }
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
  #ifdef IRS_LINUX
    #define IRSSTRDEFS_LOG(oper) { oper; }
  #else //IRS_LINUX
    #define IRSSTRDEFS_LOG(oper)
  #endif //IRS_LINUX
#else //IRSSTRDEFS_DEBUG
  #define IRSSTRDEFS_ASSERT(expr)
  #define IRSSTRDEFS_LOG(oper)
#endif //IRSSTRDEFS_DEBUG

namespace irs {

#ifdef IRS_UNICODE

#define irst(cstr) L##cstr

typedef wint_t char_int_t;
typedef wchar_t char_t;
typedef wstring string_t;
typedef wostream ostream_t;
typedef wistream istream_t;
typedef wstringstream stringstream_t; 
typedef wistringstream istringstream_t;
typedef wostringstream ostringstream_t;

#else //IRS_UNICODE

#define irst(cstr) cstr

typedef int char_int_t;
typedef char char_t;
typedef irs::string string_t;
typedef ostream ostream_t;
typedef istream istream_t;
#ifndef __ICCAVR__
typedef stringstream stringstream_t;
#endif // __ICCAVR__
typedef istringstream istringstream_t;
typedef ostringstream ostringstream_t;

#endif //IRS_UNICODE

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
inline bool isalnumt(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::alnum,
    static_cast<char_t>(a_ch));
}

inline bool isalphat(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::alpha,
    static_cast<char_t>(a_ch));
}

inline bool iscntrlt(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::cntrl,
    static_cast<char_t>(a_ch));
}

inline bool isdigitt(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::digit,
    static_cast<char_t>(a_ch));
}

inline bool isgrapht(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::graph,
    static_cast<char_t>(a_ch));
}

inline bool islowert(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::lower,
    static_cast<char_t>(a_ch));
}

inline bool isprintt(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::print,
    static_cast<char_t>(a_ch));
}

inline bool ispunctt(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::punct,
    static_cast<char_t>(a_ch));
}

inline bool isspacet(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::space,
    static_cast<char_t>(a_ch));
}

inline bool isuppert(char_int_t a_ch, const locale& a_loc = irs::loc().get())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::upper,
    static_cast<char_t>(a_ch));
}

inline bool isxdigitt(char_int_t a_ch, const locale& a_loc = irs::loc().get())
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
  return strlenu(a_str);
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
const T* cstr_from_codecvt_result(codecvt_base::result a_result)
{
  return cstr_from_codecvt_result_helper(a_result, T());
}

inline codecvt_base::result code_convert(mbstate_t& state,
  const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
  char* to, char* to_end, char*& to_next)
{
  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  return cdcvt.out(state, from, from_end, from_next,
    to, to_end, to_next);
}
inline codecvt_base::result code_convert(mbstate_t& state,
  const char* from, const char* from_end, const char*& from_next,
  wchar_t* to, wchar_t* to_end, wchar_t*& to_next)
{
  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  return cdcvt.in(state, from, from_end, from_next,
    to, to_end, to_next);
}

#ifdef IRSSTRDEFS_DEBUG
inline ostream& tcout(char)
{
  return cout;
}
inline wostream& tcout(wchar_t)
{
  return wcout;
}
#endif //IRSSTRDEFS_DEBUG

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
  convert_str_t():
    mp_outstr(new out_char_type[1])
  {
    mp_outstr[0] = 0;
  }
  convert_str_t(const convert_str_t& a_convert_str):
    mp_outstr(IRS_NULL)
  {
    IRSSTRDEFS_ASSERT(a_convert_str.mp_outstr != IRS_NULL);
    size_t len = strlenu(a_convert_str.mp_outstr) + 1;
    mp_outstr = new out_char_type[len];
    memcpy(mp_outstr, a_convert_str.mp_outstr, len);
  }
  const convert_str_t& operator=(const convert_str_t& a_convert_str)
  {
    IRSSTRDEFS_ASSERT(a_convert_str.mp_outstr != IRS_NULL);
    if (this == &a_convert_str) {
      convert_str_t convert_str_copy(a_convert_str);
      swap(mp_outstr, convert_str_copy.mp_outstr);
    }
    return *this;
  }
  convert_str_t(const in_char_type* ap_instr,
    size_t a_instr_size = m_size_default
  ):
    mp_outstr(IRS_NULL)
  {
    IRS_STATIC_ASSERT(
      ((static_cast<std_type_idx_t>(type_detect_t<in_char_type>::index) ==
        char_idx) &&
       (static_cast<std_type_idx_t>(type_detect_t<out_char_type>::index) ==
        irs::wchar_idx)) ||
      ((static_cast<std_type_idx_t>(type_detect_t<in_char_type>::index) ==
        irs::wchar_idx) &&
       (static_cast<std_type_idx_t>(type_detect_t<out_char_type>::index) ==
        irs::char_idx))
    );

    //IRSSTRDEFS_ASSERT(ap_instr != IRS_NULL);
    IRSSTRDEFS_LOG(cout << "\nconvert_str_t(const in_char_type* ap_instr,");
    IRSSTRDEFS_LOG(cout << "size_t a_instr_size = m_size_default)\n");
    IRSSTRDEFS_LOG(cout << type_to_string<in_char_type>() << " ap_instr = \"");
    IRSSTRDEFS_LOG(tcout(in_char_type()) << ap_instr);
    IRSSTRDEFS_LOG(cout << "\"" << endl);

    if (a_instr_size == m_size_default) {
      a_instr_size = strlenu(ap_instr) + 1;
    }

    mp_outstr = new out_char_type[a_instr_size];
    memset(static_cast<void*>(mp_outstr), 0,
      a_instr_size*sizeof(out_char_type));

    const in_char_type* p_instr_next = ap_instr;
    out_char_type* p_outstr_next = mp_outstr;

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

     if (convert_result != codecvt_base::ok) {
      stringstream stream;
      stream << "convert_str_t codecvt result: " <<
        cstr_from_codecvt_result<char>(convert_result);
      stream.flush();
      throw runtime_error(stream.str().c_str());
      //IRS_LIB_DBG_MSG("convert_str_t codecvt result: " <<
        //cstr_from_codecvt_result<char>(convert_result));
    }
  }
  const convert_str_t& operator=(const in_char_type* ap_instr)
  {
    IRSSTRDEFS_ASSERT(ap_instr != IRS_NULL);
    convert_str_t convert_str_copy(ap_instr);
    swap(mp_outstr, convert_str_copy.mp_outstr);
    return *this;
  }
  ~convert_str_t()
  {
    IRS_ARDELETE(mp_outstr);
  }
  const out_char_type* get() const
  {
    IRSSTRDEFS_ASSERT(mp_outstr != IRS_NULL);
    return mp_outstr;
  }
  out_char_type* get()
  {
    IRSSTRDEFS_ASSERT(mp_outstr != IRS_NULL);
    return mp_outstr;
  }
};

// Проверка преобразования строк
template <class F>
inline void convert_str_test_check(ostream& a_strm, F a_func)
{
  try {
    a_func(a_strm);
  } catch (runtime_error& e) {
    a_strm << "\nОшибка преобразования: " << e.what() << endl;
  }
}
inline const char* convert_str_test_str_rus(char)
{
  return "hello!?& Я";
}
inline const wchar_t* convert_str_test_str_rus(wchar_t)
{
  #ifndef __AVR32__
  return L"hello!?& Я";
  #else //__AVR32__
  // На AVR32 недоступна опция преобразования кодировки перед компиляцией,
  // поэтому русские буквы в wchar_t строках вызывают ошибку компилятора
  return L"hello!?& ";
  #endif //__AVR32__
}
inline const char* convert_str_test_str_eng(char)
{
  return "hello";
}
inline const wchar_t* convert_str_test_str_eng(wchar_t)
{
  return L"hello";
}
inline void convert_str_test_wide_to_simple(ostream& a_strm,
  const char* ap_info_str, const wchar_t* ap_test_str)
{
  a_strm << "Преобразование из wchar_t* в char* (" << ap_info_str << "): ";
  a_strm << irs::convert_str_t<wchar_t, char>(ap_test_str).get() << endl;
}
inline void convert_str_test_simple_to_wide(ostream& a_strm,
  const char* ap_info_str, const char* ap_test_str)
{
  a_strm << "Преобразование из char* в wchar_t* (" << ap_info_str << "): ";
  wstring wide_str = irs::convert_str_t<char, wchar_t>(ap_test_str).get();
  a_strm << irs::convert_str_t<wchar_t, char>(wide_str.c_str()).get() << endl;

  //irs::convert_str_t<wchar_t, char> simple_str(wide_str.c_str());
  //a_strm << simple_str.get() << endl;
}
inline void convert_str_test_type_to_simple(ostream& a_strm,
  const char* ap_info_str, const char_t* ap_test_str)
{
  a_strm << "Преобразование из char_t* в char* (" << ap_info_str << "): ";
  a_strm << IRS_SIMPLE_FROM_TYPE_STR(ap_test_str) << endl;
}
inline void convert_str_test_type_to_wide(ostream& a_strm,
  const char* ap_info_str, const char_t* ap_test_str)
{
  a_strm << "Преобразование из char_t* в wchar* (" << ap_info_str << "): ";
  wstring wide_str(IRS_WIDE_FROM_TYPE_STR(ap_test_str));
  //wstring wide_str = IRS_WIDE_FROM_TYPE_STR(ap_test_str);
  //IRSSTRDEFS_LOG(wcout << L"wide_str=" << wide_str.c_str() << endl);
  irs::convert_str_t<wchar_t, char> simple_str(wide_str.c_str());
  //irs::convert_str_t<wchar_t, char> simple_str(L"auto");
  a_strm << simple_str.get() << endl;
}
inline void convert_str_test_wide_to_simple_rus(ostream& a_strm)
{
  convert_str_test_wide_to_simple(a_strm, "русский",
    convert_str_test_str_rus(wchar_t()));
}
inline void convert_str_test_simple_to_wide_rus(ostream& a_strm)
{
  convert_str_test_simple_to_wide(a_strm, "русский",
    convert_str_test_str_rus(char()));
}
inline void convert_str_test_type_to_simple_rus(ostream& a_strm)
{
  convert_str_test_type_to_simple(a_strm, "русский",
    convert_str_test_str_rus(char_t()));
}
inline void convert_str_test_type_to_wide_rus(ostream& a_strm)
{
  convert_str_test_type_to_wide(a_strm, "русский",
    convert_str_test_str_rus(char_t()));
}
inline void convert_str_test_wide_to_simple_eng(ostream& a_strm)
{
  convert_str_test_wide_to_simple(a_strm, "английский",
    convert_str_test_str_eng(wchar_t()));
}
inline void convert_str_test_simple_to_wide_eng(ostream& a_strm)
{
  convert_str_test_simple_to_wide(a_strm, "английский",
    convert_str_test_str_eng(char()));
}
inline void convert_str_test_type_to_simple_eng(ostream& a_strm)
{
  convert_str_test_type_to_simple(a_strm, "английский",
    convert_str_test_str_eng(char_t()));
}
inline void convert_str_test_type_to_wide_eng(ostream& a_strm)
{
  convert_str_test_type_to_wide(a_strm, "английский",
    convert_str_test_str_eng(char_t()));
}
inline void convert_str_test(ostream& a_strm)
{
  convert_str_test_check(a_strm, convert_str_test_wide_to_simple_eng);
  convert_str_test_check(a_strm, convert_str_test_simple_to_wide_eng);
  convert_str_test_check(a_strm, convert_str_test_type_to_simple_eng);
  convert_str_test_check(a_strm, convert_str_test_type_to_wide_eng);
  convert_str_test_check(a_strm, convert_str_test_wide_to_simple_rus);
  convert_str_test_check(a_strm, convert_str_test_simple_to_wide_rus);
  convert_str_test_check(a_strm, convert_str_test_type_to_simple_rus);
  convert_str_test_check(a_strm, convert_str_test_type_to_wide_rus);
  IRSSTRDEFS_LOG(wcout << L"first первый" << endl);
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

} //namespace irs

#endif //IRSSTRDEFSH
