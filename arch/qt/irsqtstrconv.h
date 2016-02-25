#ifndef IRSQTSTRCONV_H
#define IRSQTSTRCONV_H

#include <irsdefs.h>

#include <irsstrdefs.h>
#include <string.h>
#include <irscpp.h>
#include <irsstrconvbase.h>

#ifdef QT_CORE_LIB
# include <QString>
# include <QTextCodec>
#endif //QT_CORE_LIB

#include <irsfinal.h>

namespace irs {

//! \addtogroup string_processing_group
//! @{

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#ifdef QT_CORE_LIB



template<>
struct base_str_type<QString>
{
  typedef irs_wstring_t type;
};

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
  IRS_LIB_ASSERT(s.size() <
    static_cast<std::size_t>(std::numeric_limits<int>::max()));
  return QString::fromUtf8(s.c_str(), static_cast<int>(s.size()));
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

//
//------------------------- str_conv_simple ------------------------------------
//

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

//
//-------------------------str_conv---------------------------------------------
//

//char*
inline QString str_conv(const char* a_str_in)
{
  return a_str_in;
}

//wchar_t*
inline QString str_conv(const wchar_t* a_str_in)
{
  return QString::fromWCharArray(a_str_in);
}

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

//
//---------------------------- Из QString --------------------------------------
//

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
inline std_string_t str_conv<std_string_t>(const QString& a_str_in)
{
  return QStringToStdString(a_str_in);
}

template<>
inline irs_string_t str_conv<irs_string_t>(const QString& a_str_in)
{
  return QStringToStdString(a_str_in);
}

template<>
inline std_wstring_t str_conv<std_wstring_t>(const QString& a_str_in)
{
  return QStringToStdWString(a_str_in);
}

template<>
inline irs_wstring_t str_conv<irs_wstring_t>(const QString& a_str_in)
{
  return QStringToStdWString(a_str_in);
}

//
// Преобразование кодировок
//

inline std::wstring utf_8_to_wstring(const std::string& a_str)
{
  QTextCodec *codec_utf_8 = QTextCodec::codecForName("UTF-8");
  QString Str = codec_utf_8->toUnicode(a_str.c_str());
  return irs::str_conv<std::wstring>(Str);
}

inline std::string wstring_to_utf_8(const std::wstring& a_str)
{
  QString Str = irs::str_conv<QString>(a_str);
  QTextCodec *codec_utf_8 = QTextCodec::codecForName("UTF-8");
  QByteArray str_utf_8 = codec_utf_8->fromUnicode(Str);
  return std::string(str_utf_8.data());
}
#endif //QT_CORE_LIB
#endif // IRS_FULL_STDCPPLIB_SUPPORT

//! @}

} // namespace irs

#endif // IRSQTSTRCONV_H
