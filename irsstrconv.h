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

// Из std_string_t
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
# ifndef QT_CORE_LIB
// Из std_wstring_t
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
# else // QT_CORE_LIB
inline std::string utf_8_to_win_1251(const std::string& a_str)
{
  QTextCodec *codec_utf_8 = QTextCodec::codecForName("UTF-8");
  QString Str = codec_utf_8->toUnicode(a_str.c_str());
  QTextCodec *codec_win_1251 = QTextCodec::codecForName("Windows-1251");
  QByteArray str_win_1251 = codec_win_1251->fromUnicode(Str);
  return str_win_1251.data();
}

inline std::string win_1251_to_utf_8(const std::string& a_str)
{
  QTextCodec *codec_win_1251 = QTextCodec::codecForName("Windows-1251");
  QString Str = codec_win_1251->toUnicode(a_str.c_str());
  QTextCodec *codec_utf_8 = QTextCodec::codecForName("UTF-8");
  QByteArray str_utf_8 = codec_utf_8->fromUnicode(Str);
  return str_utf_8.data();
}

inline std::string QStringToStdString(const QString &str)
{
  #if (defined(__GNUC__) && (QT_VERSION < 0x05))
  return str.toStdString();
  #elif defined(__GNUC__)
  return std::string(str.toUtf8().constData());
  #else
  return utf_8_to_win_1251(str.toUtf8().constData());
  #endif
}

inline std::wstring QStringToStdWString(const QString &str)
{
  #ifdef _MSC_VER
  IRS_STATIC_ASSERT(sizeof(wchar_t) == sizeof(ushort));
  return std::wstring(reinterpret_cast<const wchar_t*>(str.utf16()));
  #else
  return str.toStdWString();
  #endif
}

inline QString StdStringToQString(const std::string &str)
{
  #if (defined(__GNUC__) && (QT_VERSION < 0x05))
  return QString::fromStdString(str);
  #elif defined(__GNUC__)
  return QString::fromUtf8(str.c_str(), str.size());
  #else
  const std::string s = win_1251_to_utf_8(str);
  return QString::fromUtf8(s.c_str(), s.size());
  #endif
}

inline QString StdWStringToQString(const std::wstring &str)
{
  #ifdef _MSC_VER
  IRS_STATIC_ASSERT(sizeof(wchar_t) == sizeof(ushort));
  return QString::fromUtf16(
    reinterpret_cast<const unsigned short*>(str.c_str()));
  #else
  return QString::fromStdWString(str);
  #endif
}

inline std_wstring_t str_conv_simple(const std_wstring_t&,
  const std_string_t& a_str_in)
{
  QString Str = StdStringToQString(a_str_in);
  return QStringToStdWString(Str);
}
inline irs_wstring_t str_conv_simple(const irs_wstring_t&,
  const std_string_t& a_str_in)
{
  QString Str = StdStringToQString(a_str_in);
  return QStringToStdWString(Str);
}
# endif // QT_CORE_LIB
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

//char*
#ifdef QT_CORE_LIB
inline QString str_conv(const char* a_str_in)
{
  return a_str_in;
}
#endif //QT_CORE_LIB

//wchar_t*
#ifdef QT_CORE_LIB
inline QString str_conv(const wchar_t* a_str_in)
{
  return QString::fromWCharArray(a_str_in);
}
#endif //QT_CORE_LIB


// Из std_string_t
template<class T>
inline T str_conv(const std_string_t& a_str_in)
{
  return str_conv_simple(T(), a_str_in);
}
#ifdef QT_CORE_LIB
/*template<>
inline std_wstring_t str_conv<std_wstring_t>(const std_string_t& a_str_in)
{
  QString String = QString::fromStdString(a_str_in);
  return String.toStdWString();
}*/
template<>
inline QString str_conv<QString>(const std_string_t& a_str_in)
{
  return StdStringToQString(a_str_in);
}
#endif //QT_CORE_LIB
#if defined(__BORLANDC__)

template<>
inline std_wstring_t str_conv<std_wstring_t>(const std_string_t& a_str_in)
{
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDERXE))
  String str(a_str_in.c_str(), a_str_in.size());
  return std_wstring_t(str.c_str(), str.Length());
  #else
  return str_conv_simple(std_wstring_t(), a_str_in);
  #endif
}

template<>
inline irs_wstring_t str_conv<irs_wstring_t>(const std_string_t& a_str_in)
{
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDERXE))
  String str(a_str_in.c_str(), a_str_in.size());
  return irs_wstring_t(str.c_str(), str.Length());
  #else // Если не Builder XE
  return str_conv_simple(irs_wstring_t(), a_str_in);
  #endif // Если не Builder XE
}

template<>
inline String str_conv<String>(const std_string_t& a_str_in)
{
  irs::irs_string_t str(a_str_in.c_str(), a_str_in.size());
  return str_conv<String>(str);
}
#endif // defined(__BORLANDC__)

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
# else // QT_CORE_LIB
inline std_string_t str_conv_simple(const std_string_t&,
  const std_wstring_t& a_str_in)
{
  QString Str = StdWStringToQString(a_str_in);
  return QStringToStdString(Str);
}
inline irs_string_t str_conv_simple(const irs_string_t&,
  const std_wstring_t& a_str_in)
{
  QString Str = StdWStringToQString(a_str_in);
  return QStringToStdString(Str);
}
# endif // QT_CORE_LIB
template<class T>
inline T str_conv(const std_wstring_t& a_str_in)
{
  return str_conv_simple(T(), a_str_in);
}
#ifdef QT_CORE_LIB
/*template<>
inline std_string_t str_conv<std_string_t>(const std_wstring_t& a_str_in)
{
  QString String = QString::fromStdWString(a_str_in);
  return String.toStdString();
}*/

template<>
inline QString str_conv<QString>(const std_wstring_t& a_str_in)
{
  return StdWStringToQString(a_str_in);
}
#endif // QT_CORE_LIB
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

// Из irs_string_t
template<class T>
inline T str_conv(const irs_string_t& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);
  return T(a_str_in);
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
inline AnsiString str_conv<AnsiString>(const AnsiString& a_str_in)
{
  return a_str_in;
}

template<>
inline irs_string_t str_conv<irs_string_t>(const AnsiString& a_str_in)
{
  return irs_string_t(a_str_in.c_str(), a_str_in.Length());
}

template<>
inline std_string_t str_conv<std_string_t>(const AnsiString& a_str_in)
{
  return std_string_t(a_str_in.c_str(), a_str_in.Length());
}

template<>
inline std_wstring_t str_conv<std_wstring_t>(const AnsiString& a_str_in)
{
  return std::wstring(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
}

template<>
inline irs_wstring_t str_conv<irs_wstring_t>(const AnsiString& a_str_in)
{
  return irs_wstring_t(convert_str_t<char, wchar_t>(a_str_in.c_str()).get());
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

#ifdef QT_CORE_LIB
// QString
template<class T>
inline T str_conv(const QString& a_str_in)
{
  // Непроверенное преобразование
  IRS_STATIC_ASSERT(false);
  return T(a_str_in);
}

template<>
inline QString str_conv<QString>(const QString& a_str_in)
{
  return a_str_in;
}

template<>
inline irs_string_t str_conv<irs_string_t>(const QString& a_str_in)
{
  return QStringToStdString(a_str_in);
}

template<>
inline irs_wstring_t str_conv<irs_wstring_t>(const QString& a_str_in)
{
  return QStringToStdWString(a_str_in);
}
#endif // QT_CORE_LIB

#else // !IRS_FULL_STDCPPLIB_SUPPORT
# ifdef __ICCARM__
// Из irs_string_t
template<class T>
inline T str_conv(const irs_string_t& a_str_in)
{
  // Непроверенное преобразование
  IRS_ASSERT(false);
  return T(a_str_in);
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
# endif // __ICCARM__
#endif // !IRS_FULL_STDCPPLIB_SUPPORT

//! @}

} // namespace irs

#endif // irsstrconvH


