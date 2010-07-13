// Строки ИРС
// Дата: 13.07.2010
// Дата создания: 17.09.2009

#ifndef IRSSTRINGH
#define IRSSTRINGH

#include <irsdefs.h>

#ifdef __BORLANDC__
#include <SysUtils.hpp>
#endif //__BORLANDC__

#include <irscpp.h>
#include <irsconfig.h>
#include <irslocale.h>

#include <irsfinal.h>

// Поддержка экзотических строк типа __flash в IAR и др.
#ifdef IRS_LIB_FLASH_ASSERT
#define IRS_SPEC_CSTR_DECLARE(a_name, a_cstr)\
  static char const IRS_ICCAVR_FLASH a_name[] = a_cstr;
#define IRS_SPEC_CSTR_ASSIGN(a_name, a_cstr)\
  {\
    static char const IRS_ICCAVR_FLASH irs_spec_cstr_assign_name[] = a_cstr;\
    a_name = irs_spec_cstr_assign_name;\
  }
#else //IRS_LIB_FLASH_ASSERT
#define IRS_SPEC_CSTR_DECLARE(a_name, a_cstr)\
  static const char a_name[] = a_cstr;
#define IRS_SPEC_CSTR_ASSIGN(a_name, a_cstr) a_name = a_cstr;
#endif //IRS_LIB_FLASH_ASSERT

namespace irs {

// Работа с массивами по типу auto_ptr
template <class T>
class auto_arr
{
  T* m_arr;
public:
  auto_arr(T* a_arr = IRS_NULL): m_arr(a_arr) {}
  ~auto_arr() { delete []m_arr; }
  T *get() const { return m_arr; }
  T& operator[](int a_index) const { return m_arr[a_index]; }
  T& operator*() const { return *m_arr; }
  T* operator->() const { return m_arr; }
  T* release() { T* arr = m_arr; m_arr = IRS_NULL; return arr; }
  void reset(T* a_arr = IRS_NULL)
  {
    if (a_arr != m_arr) { delete []m_arr; m_arr = a_arr; }
  }
};

typedef ::string std_string_t;
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
typedef ::wstring std_wstring_t;
template <class T>
struct irs_string_other_char_t
{
  typedef void char_type;
};
template <>
struct irs_string_other_char_t<char>
{
  typedef wchar_t char_type;
};
template <>
struct irs_string_other_char_t<wchar_t>
{
  typedef char char_type;
};
#define IRS_STRING_BASE basic_string<T>
#define IRS_STRING_TYPE basic_irs_string_t
#define IRS_STRING_TYPE_SPEC basic_irs_string_t<T>
#define IRS_STRING_OSTREAM basic_ostream<T>
#define IRS_STRING_ISTREAM basic_istream<T>
#define IRS_STRING_TEMPLATE template <class T>
#define IRS_STRING_NUMBER_TEMPLATE template <class T, class V>
#define IRS_STRING_ASSIGN_HELPER_TEMPLATE template<class T, class C>
#define IRS_STRING_ASSIGN_HELPER_SMALL_TEMPLATE template<class C>
#define IRS_STRING_ASSIGN_HELPER_OSTREAM basic_ostream<C>
#define IRS_STRING_CHAR_TYPE T
#define IRS_STRING_OTHER_CHAR_TYPE\
  typename irs_string_other_char_t<T>::char_type
#else //IRS_FULL_STDCPPLIB_SUPPORT
#define IRS_STRING_BASE std_string_t
#define IRS_STRING_TYPE irs_string_t
#define IRS_STRING_TYPE_SPEC irs_string_t
#define IRS_STRING_OSTREAM ostream
#define IRS_STRING_ISTREAM istream
#define IRS_STRING_TEMPLATE
#define IRS_STRING_NUMBER_TEMPLATE template <class V>
#define IRS_STRING_ASSIGN_HELPER_TEMPLATE template<class T>
#define IRS_STRING_ASSIGN_HELPER_OSTREAM ostream
#define IRS_STRING_ASSIGN_HELPER_SMALL_TEMPLATE
#define IRS_STRING_CHAR_TYPE char
#define IRS_STRING_OTHER_CHAR_TYPE wchar_t
#endif //IRS_FULL_STDCPPLIB_SUPPORT

//char const IRS_CSTR_NONVOLATILE*

inline irs_size_t nvstrlen(char const IRS_CSTR_NONVOLATILE* ap_string)
{
  char const IRS_CSTR_NONVOLATILE* ap_string_end = ap_string;
  while ((*ap_string_end) != 0) {
    ap_string_end++;
  }
  return ap_string_end - ap_string;
}
inline std_string_t stdstr_from_nvstr(
  char const IRS_CSTR_NONVOLATILE* ap_string)
{
  irs_size_t len = nvstrlen(ap_string);
  std_string_t std_string(len, ' ');
  copy(ap_string, ap_string + len, std_string.begin());
  return std_string;
}
inline const char* irsstr_error_message(char)
{
  return "{error operation}";
}
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
inline const wchar_t* irsstr_error_message(wchar_t)
{
  return L"{error operation}";
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT
inline void irsstr_error_helper(char const IRS_CSTR_NONVOLATILE* ap_message)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  #ifdef __BORLANDC__
  throw Exception(ap_message);
  #else //__BORLANDC__
  throw runtime_error(ap_message);
  #endif //__BORLANDC__
  #else //IRS_FULL_STDCPPLIB_SUPPORT
  ap_message = ap_message;
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
}
inline void irsstr_error(char const IRS_CSTR_NONVOLATILE* ap_message,
  std_string_t* ap_out_string)
{
  irsstr_error_helper(ap_message);
  (*ap_out_string) += irsstr_error_message(char());
}
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
inline void irsstr_error(char const IRS_CSTR_NONVOLATILE* ap_message,
  std_wstring_t* ap_out_string)
{
  irsstr_error_helper(ap_message);
  (*ap_out_string) += irsstr_error_message(wchar_t());
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

template<class T>
int get_num_precision_def(const T& /*a_t*/)
{
  const int sizeofT = sizeof(T);
  int precision_def = 15;
  if (sizeofT <= 4) {
    precision_def = 6;
  } else if (sizeofT <= 8) {
    precision_def = 15;
  } else {
    precision_def = 19;
  }
  return precision_def;
}

// Добавление операций ввода/вывода для типа string
IRS_STRING_ASSIGN_HELPER_TEMPLATE
inline IRS_STRING_ASSIGN_HELPER_OSTREAM& string_assign_helper(
  IRS_STRING_ASSIGN_HELPER_OSTREAM& strm, const T& val)
{
  strm.precision(get_num_precision_def(val));
  strm << val;
  return strm;
}
IRS_STRING_ASSIGN_HELPER_SMALL_TEMPLATE
inline IRS_STRING_ASSIGN_HELPER_OSTREAM& string_assign_helper(
  IRS_STRING_ASSIGN_HELPER_OSTREAM& strm, const irs_u8& val)
{
  //strm.precision(30);
  int val_int = val;
  strm << val_int;
  return strm;
}
IRS_STRING_ASSIGN_HELPER_SMALL_TEMPLATE
inline IRS_STRING_ASSIGN_HELPER_OSTREAM& string_assign_helper(
  IRS_STRING_ASSIGN_HELPER_OSTREAM& strm, const irs_i8& val)
{
  //strm.precision(30);
  int val_int = val;
  strm << val_int;
  return strm;
}
enum irsstrloc_t {
  irsstrloc_classic,
  irsstrloc_current,
  irsstrloc_russian
};
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class C> class basic_irs_string_t;
template <class C, class T>
inline basic_irs_string_t<C>& string_assign(basic_irs_string_t<C>& strg,
  const T& val, irsstrloc_t a_locale_style);
#else //IRS_FULL_STDCPPLIB_SUPPORT
class irs_string_t;
template <class T>
inline irs_string_t& string_assign(irs_string_t& strg, const T& val,
  irsstrloc_t a_locale_style);
#endif //IRS_FULL_STDCPPLIB_SUPPORT
struct irsstrdata_t
{
  irsstrloc_t locale_style;

  irsstrdata_t():
    locale_style(irsstrloc_classic)
  {
  }
};
IRS_STRING_TEMPLATE
class IRS_STRING_TYPE: public IRS_STRING_BASE
{
public:
  typedef irs_size_t size_type;
private:
  irsstrdata_t m_data;

  void other_char_error()
  {
    IRS_SPEC_CSTR_DECLARE(error_message, "{Попытка инициализация "
      "irs_string_t недопустимым типом}");
    irsstr_error(error_message, this);
  }
public:
  irsstrloc_t locale_style()
  {
    return m_data.locale_style;
  }
  void locale_style(irsstrloc_t a_locale_style)
  {
    m_data.locale_style = a_locale_style;
  }

  IRS_STRING_TYPE& operator=(const IRS_STRING_BASE& strg)
  {
    IRS_STRING_BASE& base = ( IRS_STRING_BASE&)(*this);
    base = strg;
    return *this;
  }
  IRS_STRING_TYPE& operator=(const IRS_STRING_CHAR_TYPE* cstr)
  {
    IRS_STRING_BASE& base = (IRS_STRING_BASE&)(*this);
    base = cstr;
    return *this;
  }
  IRS_STRING_TYPE& operator=(const IRS_STRING_OTHER_CHAR_TYPE*)
  {
    other_char_error();
    return *this;
  }
  IRS_STRING_TYPE& operator=(const bool& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const IRS_STRING_CHAR_TYPE& val)
  {
    IRS_STRING_BASE& base = (IRS_STRING_BASE&)(*this);
    base = val;
    return *this;
  }
  IRS_STRING_TYPE& operator=(const IRS_STRING_OTHER_CHAR_TYPE& /*val*/)
  {
    other_char_error();
    return *this;
  }
  IRS_STRING_TYPE& operator=(const signed char& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const unsigned char& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const signed short& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const unsigned short& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const signed int& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const unsigned int& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const signed long& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const unsigned long& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const float& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const double& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const long double& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  #ifdef IRSDEFS_I64
  IRS_STRING_TYPE& operator=(const irs_i64& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  IRS_STRING_TYPE& operator=(const irs_u64& val)
  {
    return string_assign(*this, val, m_data.locale_style);
  }
  #endif //IRSDEFS_I64
  template <class V>
  void assign_style(const V& val, irsstrloc_t a_locale_style)
  {
    locale_style(a_locale_style);
    *this = val;
  }
  template <class V>
  void assign_classic(const V& val)
  {
    assign_style(val, irsstrloc_classic);
  }
  template <class V>
  void assign_current(const V& val)
  {
    assign_style(val, irsstrloc_current);
  }
  template <class V>
  void assign_russian(const V& val)
  {
    assign_style(val, irsstrloc_russian);
  }
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  template <class V>
  void assign_locale(const V& val, const locale& a_locale)
  {
    locale locale_old = irs::loc().get();
    irs::loc().set(a_locale);
    assign_current(val);
    irs::loc().set(locale_old);
  }
  #endif //IRS_FULL_STDCPPLIB_SUPPORT

  IRS_STRING_TYPE():
    IRS_STRING_BASE(),
    m_data()
  {
  }
  IRS_STRING_TYPE(const IRS_STRING_TYPE& a_string):
    IRS_STRING_BASE(a_string),
    m_data(a_string.m_data)
  {
    IRS_STRING_BASE& base = ( IRS_STRING_BASE&)(*this);
    base = a_string;
  }
  IRS_STRING_TYPE(size_type a_count, IRS_STRING_CHAR_TYPE a_char):
    IRS_STRING_BASE(a_count, a_char)
  {
  }
  IRS_STRING_TYPE(const IRS_STRING_BASE& strg)
  {
    IRS_STRING_BASE& base = ( IRS_STRING_BASE&)(*this);
    base = strg;
  }
  IRS_STRING_TYPE(const IRS_STRING_CHAR_TYPE* cstr)
  {
    *this = cstr;
  }
  IRS_STRING_TYPE(const IRS_STRING_OTHER_CHAR_TYPE* cstr)
  {
    *this = cstr;
  }

  IRS_STRING_TYPE(const bool& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const IRS_STRING_CHAR_TYPE& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const IRS_STRING_OTHER_CHAR_TYPE& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const signed char& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const unsigned char& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const signed short& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const unsigned short& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const signed int& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const unsigned int& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const signed long& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const unsigned long& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const float& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const double& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const long double& val)
  {
    *this = val;
  }
  #ifdef IRSDEFS_I64
  IRS_STRING_TYPE(const irs_i64& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const irs_u64& val)
  {
    *this = val;
  }
  #endif //IRSDEFS_I64

  template <class V>
  bool to_number(V& val) const
  {
    #ifdef IRS_FULL_STDCPPLIB_SUPPORT
    basic_irs_string_t<T> str_modif = *this;
    switch (m_data.locale_style)
    {
      case irsstrloc_russian:
      case irsstrloc_classic: {
        ::replace(str_modif.begin(), str_modif.end(), ',', '.');
      } break;
    }
    basic_istringstream<T> strm(str_modif);
    switch (m_data.locale_style)
    {
      case irsstrloc_russian:
      case irsstrloc_classic: {
        strm.imbue(locale::classic());
      } break;
      case irsstrloc_current: {
        strm.imbue(irs::loc().get());
      } break;
    }
    strm >> val;
    return strm;
    #else //IRS_FULL_STDCPPLIB_SUPPORT
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    strm >> val;
    return strm;
    #endif //IRS_FULL_STDCPPLIB_SUPPORT
  }
  bool to_number(IRS_STRING_OTHER_CHAR_TYPE& /*val*/) const
  {
    IRS_SPEC_CSTR_DECLARE(error_message,
      "{Неправильный тип char в to_number}");
    // Функция const, поэтому нельзя менять this
    std_string_t string_dummy;
    irsstr_error(error_message, &string_dummy);
    return false;
  }
  bool to_number(irs_u8& val) const
  {
    int val_int = 0;
    bool result = to_number(val_int);
    val = static_cast<irs_u8>(val_int);
    return result;
  }
  bool to_number(irs_i8& val) const
  {
    int val_int = 0;
    bool result = to_number(val_int);
    val = static_cast<irs_u8>(val_int);
    return result;
  }
  template <class V>
  bool to_number_style(V& val, irsstrloc_t a_locale_style) const
  {
    IRS_STRING_TYPE string_style = *this;
    string_style.locale_style(a_locale_style);
    return string_style.to_number(val);
  }
  template <class V>
  bool to_number_classic(V& val) const
  {
    return to_number_style(val, irsstrloc_classic);
  }
  template <class V>
  bool to_number_current(V& val) const
  {
    return to_number_style(val, irsstrloc_current);
  }
  template <class V>
  bool to_number_russian(V& val) const
  {
    return to_number_style(val, irsstrloc_russian);
  }
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  template <class V>
  bool to_number_locale(V& val, const locale &a_locale) const
  {
    locale locale_old = irs::loc().get();
    irs::loc().set(a_locale);
    return to_number_style(val, irsstrloc_current);
    irs::loc().set(locale_old);
  }
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
};

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class C, class T>
inline basic_irs_string_t<C>& string_assign(basic_irs_string_t<C>& strg,
  const T& val, irsstrloc_t a_locale_style)
{
  basic_ostringstream<C> strm;

  switch (a_locale_style)
  {
    case irsstrloc_russian:
    case irsstrloc_classic: {
      strm.imbue(locale::classic());
    } break;
    case irsstrloc_current: {
      strm.imbue(irs::loc().get());
    } break;
    default: {
      IRS_SPEC_CSTR_DECLARE(error_message,
        "{Неправильный locale_style в irs_string}");
      irsstr_error(error_message, &strg);
      return strg;
    } //break;
  }

  string_assign_helper(strm, val);
  strg = strm.str();

  if (a_locale_style == irsstrloc_russian) {
    ::replace(strg.begin(), strg.end(), '.', ',');
  }

  return strg;
}
#else //IRS_FULL_STDCPPLIB_SUPPORT
template <class T>
inline irs_string_t& string_assign(irs_string_t& strg, const T& val,
  irsstrloc_t a_locale_style)
{
  ostrstream strm;
  string_assign_helper(strm, val);
  strg.assign(strm.str(), strm.pcount());
  strm.rdbuf()->freeze(false);

  if (a_locale_style == irsstrloc_russian) {
    ::replace(strg.begin(), strg.end(), '.', ',');
  }

  return strg;
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

IRS_STRING_TEMPLATE
inline bool operator==(const IRS_STRING_TYPE_SPEC& a_first_strg,
  const IRS_STRING_TYPE_SPEC& a_second_strg)
{
  return static_cast<const IRS_STRING_BASE&>(a_first_strg) ==
    static_cast<const IRS_STRING_BASE&>(a_second_strg);
}
IRS_STRING_TEMPLATE
inline bool operator!=(const IRS_STRING_TYPE_SPEC& a_first_strg,
  const IRS_STRING_TYPE_SPEC& a_second_strg)
{
  return static_cast<const IRS_STRING_BASE&>(a_first_strg) !=
    static_cast<const IRS_STRING_BASE&>(a_second_strg);
}
IRS_STRING_TEMPLATE
inline bool operator<(const IRS_STRING_TYPE_SPEC& a_first_strg,
  const IRS_STRING_TYPE_SPEC& a_second_strg)
{
  return static_cast<const IRS_STRING_BASE&>(a_first_strg) <
    static_cast<const IRS_STRING_BASE&>(a_second_strg);
}
IRS_STRING_TEMPLATE
inline bool operator>(const IRS_STRING_TYPE_SPEC& a_first_strg,
  const IRS_STRING_TYPE_SPEC& a_second_strg)
{
  return static_cast<const IRS_STRING_BASE&>(a_first_strg) >
    static_cast<const IRS_STRING_BASE&>(a_second_strg);
}
IRS_STRING_TEMPLATE
inline bool operator<=(const IRS_STRING_TYPE_SPEC& a_first_strg,
  const IRS_STRING_TYPE_SPEC& a_second_strg)
{
  return static_cast<const IRS_STRING_BASE&>(a_first_strg) <=
    static_cast<const IRS_STRING_BASE&>(a_second_strg);
}
IRS_STRING_TEMPLATE
inline bool operator>=(const IRS_STRING_TYPE_SPEC& a_first_strg,
  const IRS_STRING_TYPE_SPEC& a_second_strg)
{
  return static_cast<const IRS_STRING_BASE&>(a_first_strg) >=
    static_cast<const IRS_STRING_BASE&>(a_second_strg);
}
IRS_STRING_TEMPLATE
inline bool operator==(const IRS_STRING_TYPE_SPEC& a_strg,
  const IRS_STRING_CHAR_TYPE* ap_cstr)
{
  return static_cast<const IRS_STRING_BASE&>(a_strg) == ap_cstr;
}
IRS_STRING_TEMPLATE
inline bool operator==(const IRS_STRING_CHAR_TYPE* ap_cstr,
  const IRS_STRING_TYPE_SPEC& a_strg)
{
  return ap_cstr == static_cast<const IRS_STRING_BASE&>(a_strg);
}
IRS_STRING_TEMPLATE
inline bool operator!=(const IRS_STRING_TYPE_SPEC& a_strg,
  const IRS_STRING_CHAR_TYPE* ap_cstr)
{
  return static_cast<const IRS_STRING_BASE&>(a_strg) != ap_cstr;
}
IRS_STRING_TEMPLATE
inline bool operator!=(const IRS_STRING_CHAR_TYPE* ap_cstr,
  const IRS_STRING_TYPE_SPEC& a_strg)
{
  return ap_cstr != static_cast<const IRS_STRING_BASE&>(a_strg);
}
IRS_STRING_TEMPLATE
inline bool operator<(const IRS_STRING_TYPE_SPEC& a_strg,
  const IRS_STRING_CHAR_TYPE* ap_cstr)
{
  return static_cast<const IRS_STRING_BASE&>(a_strg) < ap_cstr;
}
IRS_STRING_TEMPLATE
inline bool operator<(const IRS_STRING_CHAR_TYPE* ap_cstr,
  const IRS_STRING_TYPE_SPEC& a_strg)
{
  return ap_cstr < static_cast<const IRS_STRING_BASE&>(a_strg);
}
IRS_STRING_TEMPLATE
inline bool operator>(const IRS_STRING_TYPE_SPEC& a_strg,
  const IRS_STRING_CHAR_TYPE* ap_cstr)
{
  return static_cast<const IRS_STRING_BASE&>(a_strg) > ap_cstr;
}
IRS_STRING_TEMPLATE
inline bool operator>(const IRS_STRING_CHAR_TYPE* ap_cstr,
  const IRS_STRING_TYPE_SPEC& a_strg)
{
  return ap_cstr > static_cast<const IRS_STRING_BASE&>(a_strg);
}
IRS_STRING_TEMPLATE
inline bool operator<=(const IRS_STRING_TYPE_SPEC& a_strg,
  const IRS_STRING_CHAR_TYPE* ap_cstr)
{
  return static_cast<const IRS_STRING_BASE&>(a_strg) <= ap_cstr;
}
IRS_STRING_TEMPLATE
inline bool operator<=(const IRS_STRING_CHAR_TYPE* ap_cstr,
  const IRS_STRING_TYPE_SPEC& a_strg)
{
  return ap_cstr <= static_cast<const IRS_STRING_BASE&>(a_strg);
}
IRS_STRING_TEMPLATE
inline bool operator>=(const IRS_STRING_TYPE_SPEC& a_strg,
  const IRS_STRING_CHAR_TYPE* ap_cstr)
{
  return static_cast<const IRS_STRING_BASE&>(a_strg) >= ap_cstr;
}
IRS_STRING_TEMPLATE
inline bool operator>=(const IRS_STRING_CHAR_TYPE* ap_cstr,
  const IRS_STRING_TYPE_SPEC& a_strg)
{
  return ap_cstr >= static_cast<const IRS_STRING_BASE&>(a_strg);
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
typedef basic_irs_string_t<char> irs_string_t;
typedef basic_irs_string_t<wchar_t> irs_wstring_t;
//typedef basic_irs_string_t<wchar_t> wstring;
#endif //IRS_FULL_STDCPPLIB_SUPPORT
typedef irs_string_t string;

inline irs_string_t irsstr_from_nvstr(
  char const IRS_CSTR_NONVOLATILE* ap_string)
{
  return stdstr_from_nvstr(ap_string);
}

//irsstr_from_number
IRS_STRING_NUMBER_TEMPLATE
inline IRS_STRING_TYPE_SPEC irsstr_from_number(
  IRS_STRING_CHAR_TYPE,
  const V& a_value, irsstrloc_t a_locale_style)
{
  IRS_STRING_TYPE_SPEC string_value;
  string_value.assign_style(a_value, a_locale_style);
  return string_value;
}
IRS_STRING_NUMBER_TEMPLATE
inline IRS_STRING_TYPE_SPEC irsstr_from_number_classic(
  IRS_STRING_CHAR_TYPE,
  const V& a_value)
{
  return irsstr_from_number(a_value, irsstrloc_classic);
}
IRS_STRING_NUMBER_TEMPLATE
inline IRS_STRING_TYPE_SPEC irsstr_from_number_current(
  IRS_STRING_CHAR_TYPE,
  const V& a_value)
{
  return irsstr_from_number(a_value, irsstrloc_current);
}
IRS_STRING_NUMBER_TEMPLATE
inline IRS_STRING_TYPE_SPEC irsstr_from_number_russian(
  IRS_STRING_CHAR_TYPE,
  const V& a_value)
{
  return irsstr_from_number(a_value, irsstrloc_russian);
}
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
IRS_STRING_NUMBER_TEMPLATE
inline IRS_STRING_TYPE_SPEC irsstr_from_number_locale(
  IRS_STRING_CHAR_TYPE,
  const V& a_value, const locale& a_locale)
{
  IRS_STRING_TYPE_SPEC string_value;
  string_value.assign_style(a_value, a_locale);
  return string_value;
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

//cstr_to_number
IRS_STRING_NUMBER_TEMPLATE
inline bool cstr_to_number(
  const IRS_STRING_CHAR_TYPE* a_string,
  V& a_value, irsstrloc_t a_locale_style)
{
  IRS_STRING_TYPE_SPEC string_value = a_string;
  return string_value.to_number_style(a_value, a_locale_style);
}
IRS_STRING_NUMBER_TEMPLATE
inline bool cstr_to_number_classic(
  const IRS_STRING_CHAR_TYPE* a_string,
  V& a_value)
{
  return cstr_to_number(a_string, a_value, irsstrloc_classic);
}
IRS_STRING_NUMBER_TEMPLATE
inline bool cstr_to_number_current(
  const IRS_STRING_CHAR_TYPE* a_string,
  V& a_value)
{
  return cstr_to_number(a_string, a_value, irsstrloc_current);
}
IRS_STRING_NUMBER_TEMPLATE
inline bool cstr_to_number_russian(
  const IRS_STRING_CHAR_TYPE* a_string,
  V& a_value)
{
  return cstr_to_number(a_string, a_value, irsstrloc_russian);
}
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
IRS_STRING_NUMBER_TEMPLATE
inline bool cstr_to_number_locale(
  const IRS_STRING_CHAR_TYPE* a_string,
  V& a_value, const locale& a_locale)
{
  IRS_STRING_TYPE_SPEC string_value = a_string;
  return string_value.to_number_style(a_value, a_locale);
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

} //namespace irs

namespace irs_shift_operator_irsstr {

IRS_STRING_TEMPLATE
inline IRS_STRING_OSTREAM& operator<<(IRS_STRING_OSTREAM& strm,
  const irs::IRS_STRING_TYPE_SPEC& strg)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  const IRS_STRING_BASE& strg_base =
    static_cast<const IRS_STRING_BASE & >(strg);
  return strm << strg_base;
  #else //IRS_FULL_STDCPPLIB_SUPPORT
  return strm << strg.c_str();
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
}
IRS_STRING_TEMPLATE
inline IRS_STRING_ISTREAM& operator>>(IRS_STRING_ISTREAM& strm,
  irs::IRS_STRING_TYPE_SPEC& strg)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  IRS_STRING_BASE& strg_base = static_cast<IRS_STRING_BASE&>(strg);
  return strm >> strg_base;
  #else //IRS_FULL_STDCPPLIB_SUPPORT
  strg = "";
  streambuf &sbuf = *strm.rdbuf();
  // Пропускаем пробельные символы
  int c = sbuf.sgetc();
  for (;;) {
    if (c == EOF) {
      strm.clear(strm.rdstate()|ios::eofbit);
      return strm;
    }
    if (!isspace(c)) break;
    c = sbuf.snextc();
  }
  // Считываем до пробельного символа
  for (;;) {
    if (c == EOF) {
      strm.clear(strm.rdstate()|ios::eofbit);
      break;
    }
    if (isspace(c)) break;
    strg += static_cast<char>(c);
    c = sbuf.snextc();
  }
  return strm;
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
}

} //namespace irs_shift_operator_irsstr

using namespace irs_shift_operator_irsstr;

#endif //IRSSTRINGH
