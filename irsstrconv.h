// Функции конвертирования строк
// Дата: 2.05.2010
// Дата создания: 2.05.2010

#ifndef irsstrconvH
#define irsstrconvH

#include <irsdefs.h>

#if defined(__BORLANDC__)
#include <dstring.h>
#endif // defined(__BORLANDC__)

#include <irsstrdefs.h>
#include <string.h>
#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

template<class T>
struct base_str_type
{
  //typedef irs::string type;
};

template<>
struct base_str_type<irs::string>
{
  typedef irs::string type;
};

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template<>
struct base_str_type<std::wstring>
{
  typedef std::wstring type;
};
#endif // IRS_FULL_STDCPPLIB_SUPPORT

#if defined(__BORLANDC__)
template<>
struct base_str_type<AnsiString>
{
  typedef irs::string type;
};

template<>
struct base_str_type<WideString>
{
  typedef std::wstring type;
};
#endif // __BORLANDC__

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

template<class T>
inline T str_conv(const irs::string& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);
  return T(a_str_in);
}

template<>
inline irs::string str_conv<irs::string>(const irs::string& a_str_in)
{
  return a_str_in;
}

template<>
inline std::wstring str_conv<std::wstring>(const irs::string& a_str_in)
{
  return std::wstring(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
}

#if defined(__BORLANDC__)
template<>
inline AnsiString str_conv<AnsiString>(const irs::string& a_str_in)
{
  return AnsiString(a_str_in.c_str());
}

template<>
inline WideString str_conv<WideString>(const irs::string& a_str_in)
{
  return WideString(AnsiString(a_str_in.c_str()));
}
#endif // defined(__BORLANDC__)

template<class T>
inline T str_conv(const std::wstring& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);
  return T(a_str_in);
}

template<>
inline std::wstring str_conv<std::wstring>(const std::wstring& a_str_in)
{
  return a_str_in;
}

template<>
inline irs::string str_conv<irs::string>(const std::wstring& a_str_in)
{
  return irs::string(irs::convert_str_t<wchar_t, char>(a_str_in.c_str()).get());
}

#if defined(__BORLANDC__)
template<>
inline AnsiString str_conv<AnsiString>(const std::wstring& a_str_in)
{
  return AnsiString(a_str_in.c_str());
}

template<>
inline WideString str_conv<WideString>(const std::wstring& a_str_in)
{
  return WideString(a_str_in.c_str(), a_str_in.size());
}
#endif // defined(__BORLANDC__)

#if defined(__BORLANDC__)
template<class T>
T str_conv(const AnsiString& a_str_in)
{
  if (is_equal_types<T, AnsiString>::value) {
    return a_str_in;  
  } else {
    // Непроверенное преобразование
    IRS_STATIC_ASSERT(false);
    return T(a_str_in);
  }
}

/*template<>
AnsiString str_conv<AnsiString>(const AnsiString& a_str_in)
{
  return a_str_in;
}*/

template<>
inline std::wstring str_conv<std::wstring>(const AnsiString& a_str_in)
{
  return std::wstring(WideString(a_str_in).Copy());
}

template<>
inline irs::string str_conv<irs::string>(const AnsiString& a_str_in)
{
  return irs::string(a_str_in.c_str());
}

template<>
inline WideString str_conv<WideString>(const AnsiString& a_str_in)
{
  return WideString(a_str_in);
}

template<class T>
inline T str_conv(const WideString& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);  
  return T(a_str_in);
}

template<>
inline WideString str_conv<WideString>(const WideString& a_str_in)
{
  return a_str_in;
}

template<>
inline irs::string str_conv<irs::string>(const WideString& a_str_in)
{
  return irs::string(irs::convert_str_t<wchar_t, char>(a_str_in.Copy()).get());
}

template<>
inline std::wstring str_conv<std::wstring>(const WideString& a_str_in)
{
  return std::wstring(a_str_in.Copy());
}

template<>
inline AnsiString str_conv<AnsiString>(const WideString& a_str_in)
{
  return AnsiString(a_str_in);
} 
#endif // defined(__BORLANDC__)

#endif // IRS_FULL_STDCPPLIB_SUPPORT

} // namespace irs

#endif // irsstrconvH


