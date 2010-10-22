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

#include <irsstrdefs.h>
#include <string.h>
#include <irscpp.h>
//#include <irsstring.h>
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup string_processing_group
//! @{

template<class T>
struct base_str_type
{
  typedef void type;
};

template<>
struct base_str_type<irs_string_t>
{
  typedef irs_string_t type;
};

//std_string_t
inline std_string_t str_conv_simple(const std_string_t&,
  const std_string_t& a_str_in)
{
  return a_str_in;
}
inline irs_string_t str_conv_simple(const irs_string_t&,
  const std_string_t& a_str_in)
{
  return a_str_in;
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
//std_wstring_t
inline std_wstring_t str_conv_simple(const std_wstring_t&,
  const std_string_t& a_str_in)
{
  return std_wstring_t(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
}
inline irs_wstring_t str_conv_simple(const irs_wstring_t&,
  const std_string_t& a_str_in)
{
  return irs_wstring_t(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template<>
struct base_str_type<irs_wstring_t>
{
  typedef irs_wstring_t type;
};
template<>
struct base_str_type<std_wstring_t>
{
  typedef irs_wstring_t type;
};
#endif // IRS_FULL_STDCPPLIB_SUPPORT

#if defined(__BORLANDC__)
template<>
struct base_str_type<AnsiString>
{
  typedef irs_string_t type;
};

template<>
struct base_str_type<WideString>
{
  typedef irs_wstring_t type;
};

#if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
template<>
struct base_str_type<UnicodeString>
{
  typedef irs_wstring_t type;
};
#endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))

#endif // __BORLANDC__

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

//std_string_t
template<class T>
inline T str_conv(const std_string_t& a_str_in)
{
  return str_conv_simple(T(), a_str_in);
}

//std_wstring_t
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

template<class T>
inline T str_conv(const std_wstring_t& a_str_in)
{
  return str_conv_simple(T(), a_str_in);
}
#if defined(__BORLANDC__)
template<>
inline AnsiString str_conv<AnsiString>(const std_wstring_t& a_str_in)
{
  return AnsiString(a_str_in.c_str());
}
template<>
inline WideString str_conv<WideString>(const std_wstring_t& a_str_in)
{
  return WideString(a_str_in.c_str(), a_str_in.size());
}

#if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
template<>
inline UnicodeString str_conv<UnicodeString>(const std_wstring_t& a_str_in)
{
  return UnicodeString(a_str_in.c_str(), a_str_in.size());
}
#endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))

#endif // defined(__BORLANDC__)

//irs_string_t
template<class T>
inline T str_conv(const irs_string_t& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);
  return T(a_str_in);
}

template<>
inline irs_string_t str_conv<irs_string_t>(const irs_string_t& a_str_in)
{
  return a_str_in;
}

template<>
inline std_wstring_t str_conv<std_wstring_t>(const irs_string_t& a_str_in)
{
  return std_wstring_t(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
}

template<>
inline irs_wstring_t str_conv<irs_wstring_t>(const irs_string_t& a_str_in)
{
  return irs_wstring_t(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
}

#if defined(__BORLANDC__)
template<>
inline AnsiString str_conv<AnsiString>(const irs_string_t& a_str_in)
{
  return AnsiString(a_str_in.c_str());
}

template<>
inline WideString str_conv<WideString>(const irs_string_t& a_str_in)
{
  return WideString(AnsiString(a_str_in.c_str()));
}

#if (__BORLANDC__ >= IRS_CPP_BUILDER2010)
template<>
inline UnicodeString str_conv<UnicodeString>(const irs_string_t& a_str_in)
{
  return UnicodeString(AnsiString(a_str_in.c_str()));
}
#endif // (__BORLANDC__ >= IRS_CPP_BUILDER2010)

#endif // defined(__BORLANDC__)




#ifdef NOP
//std_wstring_t
template<class T>
inline T str_conv(const std::wstring& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);
  return T(a_str_in);
}

template<>
inline std_wstring_t str_conv<std_wstring_t>(const std_wstring_t& a_str_in)
{
  return a_str_in;
}

template<>
inline irs_string_t str_conv<irs_string_t>(const std_wstring_t& a_str_in)
{
  return irs_string_t(irs::convert_str_t<wchar_t, char>(a_str_in.c_str()).get());
}

#if defined(__BORLANDC__)
template<>
inline AnsiString str_conv<AnsiString>(const std_wstring_t& a_str_in)
{
  return AnsiString(a_str_in.c_str());
}

template<>
inline WideString str_conv<WideString>(const std_wstring_t& a_str_in)
{
  return WideString(a_str_in.c_str(), a_str_in.size());
}


#endif // defined(__BORLANDC__)
#endif //NOP




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
inline std_wstring_t str_conv<std_wstring_t>(const AnsiString& a_str_in)
{
  return std::wstring(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
}

template<>
inline irs_string_t str_conv<irs_string_t>(const AnsiString& a_str_in)
{
  return irs_string_t(a_str_in.c_str());
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
  //IRS_STATIC_ASSERT(false);
  return T(a_str_in);
}

template<>
inline WideString str_conv<WideString>(const WideString& a_str_in)
{
  return a_str_in;
}

template<>
inline irs_string_t str_conv<irs_string_t>(const WideString& a_str_in)
{
  return irs_string_t(AnsiString(a_str_in).c_str());
}

template<>
inline std_string_t str_conv<std_string_t>(const WideString& a_str_in)
{
  return std_string_t(AnsiString(a_str_in).c_str());
}

template<>
inline std_wstring_t str_conv<std_wstring_t>(const WideString& a_str_in)
{
  const irs_size_t str_size = a_str_in.Length();
  std::wstring str_out;
  str_out.resize(str_size);
  for (irs_size_t ch_i = 0; ch_i < str_size; ch_i++) {
    str_out[ch_i] = a_str_in[ch_i + 1];
  }
  return str_out;
}

template<>
inline AnsiString str_conv<AnsiString>(const WideString& a_str_in)
{
  return AnsiString(a_str_in);
}

#if (__BORLANDC__ >= IRS_CPP_BUILDER2010)
// UnicodeString
template<class T>
inline T str_conv(const UnicodeString& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);
  return T(a_str_in);
}

template<>
inline UnicodeString str_conv<UnicodeString>(const UnicodeString& a_str_in)
{
  return a_str_in;
}

template<>
inline irs_string_t str_conv<irs_string_t>(const UnicodeString& a_str_in)
{
  return irs_string_t(convert_str_t<wchar_t, char>(a_str_in.c_str()).get());
}

template<>
inline irs_wstring_t str_conv<irs_wstring_t>(const UnicodeString& a_str_in)
{
  return irs_wstring_t(a_str_in.c_str());
}
#endif // (__BORLANDC__ >= IRS_CPP_BUILDER2010)
#endif // defined(__BORLANDC__)

#endif // IRS_FULL_STDCPPLIB_SUPPORT

//! @}

} // namespace irs

#endif // irsstrconvH


