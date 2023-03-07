//! \file
//! \ingroup string_processing_group
//! \brief Функции конвертирования строк
//!
//! Дата: 29.07.2010
//! Дата создания: 2.05.2010

#ifndef irsstrconvH
#define irsstrconvH

#include <irsdefs.h>

#if defined(__BORLANDC__)
#include <dstring.h>
#endif // defined(__BORLANDC__)
#ifdef QT_CORE_LIB
#include <QString>
#include <QTextCodec>
#endif //QT_CORE_LIB

#if IRS_USE_UTF8_CPP
#include <utf8.h>
#endif // IRS_USE_UTF8_CPP

#include <irsstrdefs.h>
#include <string.h>
#include <irscpp.h>
//#include <irsstrconv.h>
//#include <irsstring.h>
#include <mxdata.h>
#include <irsstrconvbase.h>
#include <irs_codecvt.h>

#if defined(__BORLANDC__)
# include <irscbstrconv.h>
#endif // defined(__BORLANDC__)

#ifdef QT_CORE_LIB
# include <irsqtstrconv.h>
#endif //QT_CORE_LIB

#include <irsfinal.h>

namespace irs {

//! \addtogroup string_processing_group
//! @{

template <class wcharT = wchar_t>
inline std::basic_string<wcharT> cp1251_to_unicode(const std::string& a_str)
{
  irs::codecvt_cp1251_t<wcharT, char, std::mbstate_t> codecvt_wchar_cp1251;
  mbstate_t state;

  const char* in_str = a_str.c_str();
  const size_t instr_size = strlenu(in_str) + 1;
  const char* in_str_end = in_str + instr_size;
  const char* in_str_next = in_str;

  vector<wcharT> out_str_array(instr_size, 0);
  wcharT* out_str = vector_data(out_str_array);
  wcharT* out_str_end = out_str + instr_size;
  wcharT* out_str_next = out_str;

  std::codecvt_base::result convert_result = codecvt_wchar_cp1251.in(
    state, in_str, in_str_end, in_str_next, out_str, out_str_end, out_str_next);

  #ifdef IRS_NO_EXCEPTIONS
  IRS_LIB_ASSERT(convert_result == std::codecvt_base::ok);
  #else
  if (convert_result != std::codecvt_base::ok) {
    throw runtime_error("Не удалось преобразовать cp1251 в unicode");
  }
  #endif // IRS_NO_EXCEPTIONS
  return std::basic_string<wcharT>(out_str);
}


template <class wcharT = wchar_t>
inline std::string unicode_to_cp1251(const std::basic_string<wcharT>& a_str)
{
  irs::codecvt_cp1251_t<char, wcharT, std::mbstate_t> codecvt_cp1251_wchar;
  mbstate_t state;
  const wcharT* in_str = a_str.c_str();
  const size_t instr_size = a_str.size() + 1;
  const wcharT* in_str_end = in_str + instr_size;
  const wcharT* in_str_next = in_str;

  vector<char> out_str_array(instr_size, 0);
  char* out_str = vector_data(out_str_array);
  char* out_str_end = out_str + instr_size;
  char* out_str_next = out_str;

  std::codecvt_base::result convert_result = codecvt_cp1251_wchar.in(
    state, in_str, in_str_end, in_str_next, out_str, out_str_end, out_str_next);

  #ifdef IRS_NO_EXCEPTIONS
  IRS_LIB_ASSERT(convert_result == std::codecvt_base::ok);
  #else
  if (convert_result != std::codecvt_base::ok) {
    throw runtime_error("Не удалось преобразовать unicode в cp1251");
  }
  #endif // IRS_NO_EXCEPTIONS

  return std::string(out_str);
}


inline std::wstring cp1251_to_wstring(const std::string& a_str)
{
  return cp1251_to_unicode(a_str);
}

inline std::string wstring_to_cp1251(const std::wstring& a_str)
{
  return unicode_to_cp1251(a_str);
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

// Из std_string_t

// Из std_wstring_t
inline std_wstring_t str_conv_simple(const std_wstring_t&,
  const std_wstring_t& a_str_in)
{
  return a_str_in;
}
inline irs_wstring_t str_conv_simple(const irs_wstring_t&,
  const std_wstring_t& a_str_in)
{
  return a_str_in;
}
# ifndef QT_CORE_LIB
inline std_string_t str_conv_simple(const std_string_t&,
  const std_wstring_t& a_str_in)
{
  return convert_str_t<wchar_t, char>(a_str_in.c_str()).get();
}
inline irs_string_t str_conv_simple(const irs_string_t&,
  const std_wstring_t& a_str_in)
{
  return convert_str_t<wchar_t, char>(a_str_in.c_str()).get();
}
# endif // !QT_CORE_LIB

// Из irs_string_t


template<>
inline std_wstring_t str_conv<std_wstring_t>(const irs_string_t& a_str_in)
{
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDERXE))
  String str(a_str_in.c_str(), a_str_in.size());
  return std_wstring_t(str.c_str(), str.Length());
  #else // Если меньше Builder XE
  return str_conv_simple(std_wstring_t(), a_str_in);
  #endif // Если меньше Builder XE
}

template<>
inline irs_wstring_t str_conv<irs_wstring_t>(const irs_string_t& a_str_in)
{
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDERXE))
  String str(a_str_in.c_str(), a_str_in.size());
  return irs_wstring_t(str.c_str(), str.Length());
  #else // Если меньше Builder XE
  return str_conv_simple(irs_wstring_t(), a_str_in);
  #endif // Если меньше Builder XE

}

#else // !IRS_FULL_STDCPPLIB_SUPPORT
# ifdef __ICCARM__

inline std::wstring str_conv_simple(const std::wstring&,
  const std::string& a_str_in)
{
  return cp1251_to_wstring(a_str_in);
}

inline std::string str_conv_simple(const std::string&,
  const std::wstring& a_str_in)
{
  return wstring_to_cp1251(a_str_in);
}

// Из irs_string_t
template<class T>
inline T str_conv(const irs_string_t& /*a_str_in*/)
{
  // Непроверенное преобразование
  IRS_ASSERT(false);
  return T();
}

template<>
inline std_string_t str_conv<std_string_t>(const irs_string_t& a_str_in)
{
  return a_str_in;
}

template<>
inline irs_string_t str_conv<irs_string_t>(const irs_string_t& a_str_in)
{
  return a_str_in;
}

template<>
inline std::wstring str_conv<std::wstring>(const irs_string_t& a_str_in)
{
  return str_conv_simple(std::wstring(), a_str_in);
}


template<class T>
inline T str_conv(const std::wstring& /*a_str_in*/)
{
  // Непроверенное преобразование
  IRS_ASSERT(false);
  return T();
}

template<>
inline std::wstring str_conv<std::wstring>(const std::wstring& a_str_in)
{
  return a_str_in;
}

template<>
inline std::string str_conv<std::string>(const std::wstring& a_str_in)
{
  return str_conv_simple(std::string(), a_str_in);
}
# endif // __ICCARM__
#endif // !IRS_FULL_STDCPPLIB_SUPPORT

#ifdef IRS_WIN32
# ifndef QT_CORE_LIB
inline std::wstring utf_8_to_wstring(const std::string& a_str)
{
  int size =  MultiByteToWideChar(CP_UTF8 ,0 , a_str.c_str(), a_str.size(),
    NULL, 0);
  wchar_t* wcstr = new wchar_t[size];
  MultiByteToWideChar(CP_UTF8, 0, a_str.c_str(), a_str.size(), wcstr, size);
  std::wstring wstr(wcstr, size);
  delete[] wcstr;
  return wstr;
}

inline std::string wstring_to_utf_8(const std::wstring& a_str)
{
  int size = WideCharToMultiByte(CP_UTF8, 0, a_str.c_str(),
    a_str.size(), NULL, 0, NULL, NULL);
  char* cstr = new char[size];
  WideCharToMultiByte(CP_UTF8, 0, a_str.c_str(), a_str.size(), cstr,
    size, NULL, NULL);
  std::string str(cstr, size);
  delete[] cstr;
  return str;
}

/*inline std::string utf_8_to_cp1251(const std::string& a_str)
{
}*/

# endif // !QT_CORE_LIB
#else // !IRS_WIN32
# if IRS_USE_UTF8_CPP
inline std::wstring utf_8_to_wstring(const std::string& a_str)
{
  //vector<wchar_t> wchar_vec;
  std::wstring wstr;
  if (sizeof(wchar_t) == 2) {
    utf8::utf8to16(a_str.begin(), a_str.end(), back_inserter(wstr));
  } else if (sizeof(wchar_t) == 4) {
    utf8::utf8to32(a_str.begin(), a_str.end(), back_inserter(wstr));
  } else {
    // Недопустимый размер
    IRS_STATIC_ASSERT((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4));
  }
  //std::wstring wstr(wchar_vec.front(), wchar_vec.size());
  return wstr;
}

inline std::string wstring_to_utf_8(const std::wstring& a_str)
{
   std::string str_utf8;
   if (sizeof(wchar_t) == 2) {
    utf8::utf16to8(a_str.begin(), a_str.end(), back_inserter(str_utf8));
  } else if (sizeof(wchar_t) == 4) {
    utf8::utf32to8(a_str.begin(), a_str.end(), back_inserter(str_utf8));
  } else {
    // Недопустимый размер
    IRS_STATIC_ASSERT((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4));
  }
  return str_utf8;
}
# endif // IRS_USE_UTF8_CPP
#endif // !IRS_WIN32

#if (defined(IRS_WIN32) || defined(QT_CORE_LIB))
template <class T>
inline std::string encode_utf_8(const T& a_str)
{
  std::wstring wstr = irs::str_conv<std::wstring>(a_str);
  return wstring_to_utf_8(wstr);
}

template <class T>
inline T decode_utf_8(const std::string& a_str)
{
  std::wstring wstr = utf_8_to_wstring(a_str);
  return irs::str_conv<T>(wstr);
}
#elif IRS_USE_UTF8_CPP
template <class T>
inline std::string encode_utf_8(const T& a_str)
{
  std::wstring wstr = irs::str_conv<std::wstring>(a_str);
  return wstring_to_utf_8(wstr);
}

inline bool all_chars_ascii(const irs::string& a_str)
{
  for (size_t i = 0; i < a_str.size(); i++) {
    if (a_str[i] > 0x7f) {
      return false;
    }
  }
  return true;
}

template <>
inline std::string encode_utf_8<std::string>(const std::string& a_str)
{
  if (all_chars_ascii(a_str)) { // Если все символы <=7F, то не кодируем
    return a_str;
  }
  std::wstring wstr = irs::str_conv<std::wstring>(a_str);
  return wstring_to_utf_8(wstr);
}

template <>
inline std::string encode_utf_8<irs::string>(const irs::string& a_str)
{
  return encode_utf_8<std::string>(a_str);
}

template <class T>
inline T decode_utf_8(const std::string& a_str)
{
  std::wstring wstr = utf_8_to_wstring(a_str);
  return irs::str_conv<T>(wstr);
}

template <>
inline std::string decode_utf_8<std::string>(const std::string& a_str)
{
  if (all_chars_ascii(a_str)) { // Если все символы <=7F, то не кодируем
    return a_str;
  }
  std::wstring wstr = utf_8_to_wstring(a_str);
  return irs::str_conv<std::string>(wstr);
}

template <>
inline irs::string decode_utf_8<irs::string>(const std::string& a_str)
{
  return decode_utf_8<std::string>(a_str);
}
#endif // IRS_USE_UTF8_CPP

//! @}

} // namespace irs

#endif // irsstrconvH


