// Строки ИРС
// Дата: 24.05.2010
// Дата создания: 17.09.2009

#ifndef IRSSTRINGH
#define IRSSTRINGH

#include <irsdefs.h>

#ifdef __BORLANDC__
#include <SysUtils.hpp>
#endif //__BORLANDC__

#include <irscpp.h>

#include <irsfinal.h>

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
#define IRS_STRING_ASSIGN_HELPER_TEMPLATE template<class T>
#define IRS_STRING_ASSIGN_HELPER_OSTREAM ostream
#define IRS_STRING_ASSIGN_HELPER_SMALL_TEMPLATE
#define IRS_STRING_CHAR_TYPE char
#define IRS_STRING_OTHER_CHAR_TYPE wchar_t
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
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class C> class basic_irs_string_t;
template <class C, class T>
inline basic_irs_string_t<C>& string_assign(basic_irs_string_t<C>& strg,
  const T& val);
#else //IRS_FULL_STDCPPLIB_SUPPORT
class irs_string_t;
template <class T>
inline irs_string_t& string_assign(irs_string_t& strg, const T& val);
#endif //IRS_FULL_STDCPPLIB_SUPPORT
IRS_STRING_TEMPLATE
class IRS_STRING_TYPE: public IRS_STRING_BASE
{
public:
  typedef irs_size_t size_type;
private:
  void other_char_error()
  {
    const char* error_message = "{Попытка инициализация irs_string_t "
      "недопустимым типом}";
    #ifdef IRS_FULL_STDCPPLIB_SUPPORT
    #ifdef __BORLANDC__
    throw Exception(error_message);
    #else //__BORLANDC__
    throw exception(error_message);
    #endif //__BORLANDC__
    #else //IRS_FULL_STDCPPLIB_SUPPORT
    *this = error_message;
    #endif //IRS_FULL_STDCPPLIB_SUPPORT
  }
public:
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
    return string_assign(*this, val);
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
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const unsigned char& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const signed short& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const unsigned short& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const signed int& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const unsigned int& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const signed long& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const unsigned long& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const float& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const double& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const long double& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const irs_i64& val)
  {
    return string_assign(*this, val);
  }
  IRS_STRING_TYPE& operator=(const irs_u64& val)
  {
    return string_assign(*this, val);
  }

  #ifdef NOP
  operator IRS_STRING_BASE&()
  {
    IRS_STRING_BASE& base = ( IRS_STRING_BASE&)(*this);
    return base;
  }
  #endif //NOP

  IRS_STRING_TYPE()
  {
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
  IRS_STRING_TYPE(const irs_i64& val)
  {
    *this = val;
  }
  IRS_STRING_TYPE(const irs_u64& val)
  {
    *this = val;
  }

  template <class V>
  bool to_number(V& val) const
  {
    #ifdef IRS_FULL_STDCPPLIB_SUPPORT  
    basic_irs_string_t<T> str_modif = *this;
    ::replace(str_modif.begin(), str_modif.end(), ',', '.');
    basic_istringstream<T> strm(str_modif);
    strm.imbue(locale::classic());
    strm >> val;
    return strm;
    #else //IRS_FULL_STDCPPLIB_SUPPORT
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    strm >> val;
    return strm.good();
    #endif //IRS_FULL_STDCPPLIB_SUPPORT
  }
  bool to_number(IRS_STRING_OTHER_CHAR_TYPE& /*val*/) const
  {
    const char* error_message = "{Неправильный тип char в to_number}";
    #ifdef IRS_FULL_STDCPPLIB_SUPPORT
    #ifdef __BORLANDC__
    throw Exception(error_message);
    #else //__BORLANDC__
    throw exception(error_message);
    #endif //__BORLANDC__
    #else //IRS_FULL_STDCPPLIB_SUPPORT
    IRS_STRING_BASE& base = ( IRS_STRING_BASE&)(*this);
    base += error_message;
    #endif //IRS_FULL_STDCPPLIB_SUPPORT
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
};

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class C, class T>
inline basic_irs_string_t<C>& string_assign(basic_irs_string_t<C>& strg,
  const T& val)
{
  basic_ostringstream<C> strm;
  string_assign_helper(strm, val);
  strg = strm.str();
  return strg;
}
#else //IRS_FULL_STDCPPLIB_SUPPORT
template <class T>
inline irs_string_t& string_assign(irs_string_t& strg, const T& val)
{
  ostrstream strm;
  string_assign_helper(strm, val);
  strg.assign(strm.str(), strm.pcount());
  strm.rdbuf()->freeze(false);
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
IRS_STRING_TEMPLATE
inline IRS_STRING_OSTREAM& operator<<(IRS_STRING_OSTREAM& strm,
  const IRS_STRING_TYPE_SPEC& strg)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  const IRS_STRING_BASE& strg_base =
    static_cast<const IRS_STRING_TYPE_SPEC&>(strg);
  return strm << strg_base;
  #else //IRS_FULL_STDCPPLIB_SUPPORT
  return strm << strg.c_str();
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
}
IRS_STRING_TEMPLATE
inline IRS_STRING_OSTREAM& operator>>(IRS_STRING_OSTREAM& strm,
  IRS_STRING_TYPE_SPEC& strg)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  IRS_STRING_BASE& strg_base = static_cast<IRS_STRING_TYPE_SPEC&>(strg);
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

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
typedef basic_irs_string_t<char> string;
typedef basic_irs_string_t<wchar_t> wstring;
typedef string irs_string_t;
typedef wstring irs_wstring_t;
#else //IRS_FULL_STDCPPLIB_SUPPORT
typedef irs_string_t string;
#endif //IRS_FULL_STDCPPLIB_SUPPORT

} //namespace irs

#endif //IRSSTRINGH
