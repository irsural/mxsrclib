#ifndef IRSSTRCONVBASE_H
#define IRSSTRCONVBASE_H

#include <irsdefs.h>

#include <irsstrdefs.h>
#include <string.h>
#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup string_processing_group
//! @{

//
//---------------------- Определение базового типа -----------------------------
//
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

template<>
struct base_str_type<std_string_t>
{
  typedef irs_string_t type;
};

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


//
//------------------------- str_conv_simple ------------------------------------
//

//
// Из std_string_t
//
inline std_string_t str_conv_simple(const std_string_t&,
  const std_string_t& a_str_in)
{
  return a_str_in;
}

//
// Из irs_string_t
//
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
# endif // !QT_CORE_LIB
#endif //IRS_FULL_STDCPPLIB_SUPPORT

//
//------------------------- str_conv ------------------------------------
//

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
// Из std_string_t
template<class T>
inline T str_conv(const std_string_t& a_str_in)
{
  return str_conv_simple(T(), a_str_in);
}

// Из irs_string_t
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
inline std_string_t str_conv<std_string_t>(const irs_string_t& a_str_in)
{
  return a_str_in;
}

#ifdef __GNUC__
inline std_string_t str_conv_simple(const std_string_t&,
  const std_wstring_t& a_str_in);
#endif // __GNUC__

template<class T>
inline T str_conv(const std_wstring_t& a_str_in)
{
  return str_conv_simple(T(), a_str_in);
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

//! @}

} // namespace irs



#endif // IRSSTRCONVBASE_H
