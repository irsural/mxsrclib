// Строки ИРС
// Дата: 13.04.2010
// Дата создания: 17.09.2009

#ifndef IRSSTRINGH
#define IRSSTRINGH

#include <irsdefs.h>

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

// Добавление операций ввода/вывода для типа string
template <class T>
inline ostrstream& string_assign_helper(ostrstream& strm, const T& val)
{
  const int sizeofT = sizeof(T);
  int prec = 15;
  if (sizeofT <= 4) prec = 6;
  else if (sizeofT <= 8) prec = 15;
  else if (sizeofT <= 10) prec = 19;
  strm.precision(prec);
  strm << val;
  return strm;
}
inline ostrstream& string_assign_helper(ostrstream& strm, const irs_u8& val)
{
  //strm.precision(30);
  int val_int = val;
  strm << val_int;
  return strm;
}
inline ostrstream& string_assign_helper(ostrstream& strm, const irs_i8& val)
{
  //strm.precision(30);
  int val_int = val;
  strm << val_int;
  return strm;
}
template <class S, class T>
inline S& string_assign(S& strg, const T& val)
{
  ostrstream strm;
  string_assign_helper(strm, val);
  strg.assign(strm.str(), strm.pcount());
  strm.rdbuf()->freeze(false);
  return strg;
}
class string: public ::string
{
public:
  string& operator=(const ::string& strg)
  {
    ::string& base = ( ::string&)(*this);
    base = strg;
    return *this;
  }
  string& operator=(const char *cstr)
  {
    ::string& base = ( ::string&)(*this);
    base = cstr;
    return *this;
  }
  string& operator=(const wchar_t*)
  {
    //IRS_LIB_ERROR(irs::ec_standard,
      //"Инициализация irs::string типом wchar_t* недопустима");
    *this = "Инициализация irs::string типом wchar_t* недопустима";
    return *this;
  }
  string& operator=(const bool& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const char& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed char& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned char& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed short& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned short& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed int& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned int& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed long& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned long& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const float& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const double& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const long double& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const irs_i64& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const irs_u64& val)
  {
    return string_assign(*this, val);
  }

  #ifdef NOP
  operator ::string&()
  {
    ::string& base = ( ::string&)(*this);
    return base;
  }
  #endif //NOP

  string()
  {
  }
  string(const ::string& strg)
  {
    ::string& base = ( ::string&)(*this);
    base = strg;
  }
  string(const char *cstr)
  {
    *this = cstr;
  }
  string(const wchar_t*)
  {
    //IRS_LIB_ERROR(irs::ec_standard,
      //"Инициализация irs::string типом wchar_t* недопустима");
    *this = "Инициализация irs::string типом wchar_t* недопустима";
  }

  string(const bool& val)
  {
    *this = val;
  }
  string(const char& val)
  {
    *this = val;
  }
  string(const signed char& val)
  {
    *this = val;
  }
  string(const unsigned char& val)
  {
    *this = val;
  }
  string(const signed short& val)
  {
    *this = val;
  }
  string(const unsigned short& val)
  {
    *this = val;
  }
  string(const signed int& val)
  {
    *this = val;
  }
  string(const unsigned int& val)
  {
    *this = val;
  }
  string(const signed long& val)
  {
    *this = val;
  }
  string(const unsigned long& val)
  {
    *this = val;
  }
  string(const float& val)
  {
    *this = val;
  }
  string(const double& val)
  {
    *this = val;
  }
  string(const long double& val)
  {
    *this = val;
  }
  string(const irs_i64& val)
  {
    *this = val;
  }
  string(const irs_u64& val)
  {
    *this = val;
  }

  template <class T>
  bool to_number(T& val) const
  {
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    strm >> val;
    if (strm) return true;
    else return false;
  }
  bool to_number(irs_u8& val) const
  {
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    int val_int = 0;
    strm >> val_int;
    val = static_cast<irs_u8>(val_int);
    if (strm) return true;
    else return false;
  }
  bool to_number(irs_i8& val) const
  {
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    int val_int = 0;
    strm >> val_int;
    val = static_cast<irs_u8>(val_int);
    if (strm) return true;
    else return false;
  }
};

inline bool operator==(const string& a_first_strg, const string& a_second_strg)
{
  return static_cast<const ::string&>(a_first_strg) ==
    static_cast<const ::string&>(a_second_strg);
}

inline bool operator!=(const string& a_first_strg, const string& a_second_strg)
{
  return static_cast<const ::string&>(a_first_strg) !=
    static_cast<const ::string&>(a_second_strg);
}

inline bool operator<(const string& a_first_strg, const string& a_second_strg)
{
  return static_cast<const ::string&>(a_first_strg) <
    static_cast<const ::string&>(a_second_strg);
}

inline bool operator>(const string& a_first_strg, const string& a_second_strg)
{
  return static_cast<const ::string&>(a_first_strg) >
    static_cast<const ::string&>(a_second_strg);
}

inline bool operator<=(const string& a_first_strg, const string& a_second_strg)
{
  return static_cast<const ::string&>(a_first_strg) <=
    static_cast<const ::string&>(a_second_strg);
}

inline bool operator>=(const string& a_first_strg, const string& a_second_strg)
{
  return static_cast<const ::string&>(a_first_strg) >=
    static_cast<const ::string&>(a_second_strg);
}

inline bool operator==(const string& a_strg, const char* ap_cstr)
{
  return static_cast<const ::string&>(a_strg) == ap_cstr;
}

inline bool operator==(const char* ap_cstr, const string& a_strg)
{
  return ap_cstr == static_cast<const ::string&>(a_strg);
}

inline bool operator!=(const string& a_strg, const char* ap_cstr)
{
  return static_cast<const ::string&>(a_strg) != ap_cstr;
}

inline bool operator!=(const char* ap_cstr, const string& a_strg)
{
  return ap_cstr != static_cast<const ::string&>(a_strg);
}

inline bool operator<(const string& a_strg, const char* ap_cstr)
{
  return static_cast<const ::string&>(a_strg) < ap_cstr;
}

inline bool operator<(const char* ap_cstr, const string& a_strg)
{
  return ap_cstr < static_cast<const ::string&>(a_strg);
}

inline bool operator>(const string& a_strg, const char* ap_cstr)
{
  return static_cast<const ::string&>(a_strg) > ap_cstr;
}

inline bool operator>(const char* ap_cstr, const string& a_strg)
{
  return ap_cstr > static_cast<const ::string&>(a_strg);
}

inline bool operator<=(const string& a_strg, const char* ap_cstr)
{
  return static_cast<const ::string&>(a_strg) <= ap_cstr;
}

inline bool operator<=(const char* ap_cstr, const string& a_strg)
{
  return ap_cstr <= static_cast<const ::string&>(a_strg);
}

inline bool operator>=(const string& a_strg, const char* ap_cstr)
{
  return static_cast<const ::string&>(a_strg) >= ap_cstr;
}

inline bool operator>=(const char* ap_cstr, const string& a_strg)
{
  return ap_cstr >= static_cast<const ::string&>(a_strg);
}

inline ostream& operator<<(ostream& strm, const irs::string& strg)
{
  return strm << strg.c_str();
}
inline istream& operator>>(istream& strm, irs::string& strg)
{
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
}

} //namespace irs

#endif //IRSSTRINGH
