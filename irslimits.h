//! \file
//! \ingroup type_processing_group
//! \brief Характеристики чисел
//!
//! Дата: 24.04.2011\n
//! Ранняя дата: 17.09.2009

#ifndef IRSLIMITSH
#define IRSLIMITSH

#include <irsdefs.h>

#if defined(__BORLANDC__)
#include <dstring.h>
#endif // defined(__BORLANDC__)

#include <float.h>
#include <string.h>

#include <irscpp.h>
#include <irsstring.h>
//#include <irsstrdefs.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup type_processing_group
//! @{

// Глобальные константы
struct global_limits_t
{
  enum {
    bits_in_byte = 8,
    high_bit_in_byte = 7
  };
};

//! \brief Преобразование любого типа в bool
template <class T>
bool to_bool(T a_value)
{
  if (a_value) {
    return true;
  } else {
    return false;
  }
}

//! \brief Преобразование любого типа в irs_bool
template <class T>
irs_bool to_irs_bool(T a_value)
{
  if (a_value) {
    return irs_true;
  } else {
    return irs_false;
  }
}

// Преобразование из bool в C-строку
inline const char* bool_to_cstr(bool a_value)
{
  const char* ret = "false";
  if (a_value) {
    ret = "true";
  }
  return ret;
}

// Преобразование из типа в строку
inline const char* type_to_string_helper(bool)
{
  return "bool";
}
inline const char* type_to_string_helper(char)
{
  return "char";
}
inline const char* type_to_string_helper(wchar_t)
{
  return "wchar_t";
}
inline const char* type_to_string_helper(signed char)
{
  return "signed char";
}
inline const char* type_to_string_helper(unsigned char)
{
  return "unsigned char";
}
inline const char* type_to_string_helper(signed short)
{
  return "signed short";
}
inline const char* type_to_string_helper(unsigned short)
{
  return "unsigned int";
}
inline const char* type_to_string_helper(signed int)
{
  return "signed int";
}
inline const char* type_to_string_helper(unsigned int)
{
  return "unsigned int";
}
inline const char* type_to_string_helper(signed long)
{
  return "signed long";
}
inline const char* type_to_string_helper(unsigned long)
{
  return "unsigned long";
}
#ifdef IRSDEFS_I64
inline const char* type_to_string_helper(irs_i64)
{
  return "irs_i64";
}
inline const char* type_to_string_helper(irs_u64)
{
  return "irs_u64";
}
#endif //IRSDEFS_I64
inline const char* type_to_string_helper(float)
{
  return "float";
}
inline const char* type_to_string_helper(double)
{
  return "double";
}
inline const char* type_to_string_helper(long double)
{
  return "long double";
}
template <class T>
const char* type_to_string()
{
  return type_to_string_helper(T());
}
template <class T>
inline const char* type_to_string(T)
{
  return type_to_string<T>();
}

enum {
  float_size = 4,
  double_size = 8,
  #ifdef __MINGW32__
  // Под MinGW по Win32 размер long double 12 байт, но 2 байта не используется
  // и этот тип может интерпретироватся как 10-ти байтный
  long_double_size = 12
  #else //__MINGW32__
  long_double_size = 10
  #endif //__MINGW32__
};
enum {
  int8_size = 1,
  int16_size = 2,
  int32_size = 4,
  int64_size = 8
};
enum signed_t {
  signed_idx,
  unsigned_idx
};
enum std_type_idx_t {
  invalid_idx,
  unknown_idx,
  bool_idx,
  char_idx,
  wchar_idx,
  signed_char_idx,
  unsigned_char_idx,
  signed_short_idx,
  unsigned_short_idx,
  signed_int_idx,
  unsigned_int_idx,
  signed_long_idx,
  unsigned_long_idx,
  #ifdef IRSDEFS_LONG_LONG
  signed_long_long_idx,
  unsigned_long_long_idx,
  #endif // IRSDEFS_LONG_LONG
  float_idx,
  double_idx,
  long_double_idx
};

enum fix_type_idx_t {
  invalid_fix_idx,
  i8_fix_idx,
  u8_fix_idx,
  i16_fix_idx,
  u16_fix_idx,
  i32_fix_idx,
  u32_fix_idx,
  #ifdef IRSDEFS_I64
  i64_fix_idx,
  u64_fix_idx,
  #endif //IRSDEFS_I64
  float32_fix_idx,
  float64_fix_idx
};

// Преобразование тип в индекс
inline std_type_idx_t type_to_index_helper(bool)
{
  return bool_idx;
}
inline std_type_idx_t type_to_index_helper(char)
{
  return char_idx;
}
inline std_type_idx_t type_to_index_helper(wchar_t)
{
  return wchar_idx;
}
inline std_type_idx_t type_to_index_helper(signed char)
{
  return signed_char_idx;
}
inline std_type_idx_t type_to_index_helper(unsigned char)
{
  return unsigned_char_idx;
}
inline std_type_idx_t type_to_index_helper(signed short)
{
  return signed_short_idx;
}
inline std_type_idx_t type_to_index_helper(unsigned short)
{
  return unsigned_short_idx;
}
inline std_type_idx_t type_to_index_helper(signed int)
{
  return signed_int_idx;
}
inline std_type_idx_t type_to_index_helper(unsigned int)
{
  return unsigned_int_idx;
}
inline std_type_idx_t type_to_index_helper(signed long)
{
  return signed_long_idx;
}
inline std_type_idx_t type_to_index_helper(unsigned long)
{
  return unsigned_long_idx;
}
#ifdef IRSDEFS_LONG_LONG
inline std_type_idx_t type_to_index_helper(irs_ilong_long)
{
  return signed_long_long_idx;
}
inline std_type_idx_t type_to_index_helper(irs_ulong_long)
{
  return unsigned_long_long_idx;
}
#endif //IRSDEFS_LONG_LONG
inline std_type_idx_t type_to_index_helper(float)
{
  return float_idx;
}
inline std_type_idx_t type_to_index_helper(double)
{
  return double_idx;
}
inline std_type_idx_t type_to_index_helper(long double)
{
  return long_double_idx;
}
template <class T>
std_type_idx_t type_to_index()
{
  return type_to_index_helper(T());
}
template <class T>
std_type_idx_t type_to_index(T)
{
  return type_to_index<T>();
}
template <class T>
struct type_detect_t
{
  enum {
    index = unknown_idx
  };
};
template <>
struct type_detect_t<char>
{
  enum {
    index = char_idx
  };
};
template <>
struct type_detect_t<wchar_t>
{
  enum {
    index = wchar_idx
  };
};
template <>
struct type_detect_t<signed char>
{
  enum {
    index = signed_char_idx
  };
};
template <>
struct type_detect_t<unsigned char>
{
  enum {
    index = unsigned_char_idx
  };
};
template <>
struct type_detect_t<signed short>
{
  enum {
    index = signed_short_idx
  };
};
template <>
struct type_detect_t<unsigned short>
{
  enum {
    index = unsigned_short_idx
  };
};
template <>
struct type_detect_t<signed int>
{
  enum {
    index = signed_int_idx
  };
};
template <>
struct type_detect_t<unsigned int>
{
  enum {
    index = unsigned_int_idx
  };
};
template <>
struct type_detect_t<signed long>
{
  enum {
    index = signed_long_idx
  };
};
template <>
struct type_detect_t<unsigned long>
{
  enum {
    index = unsigned_long_idx
  };
};
#ifdef IRSDEFS_LONG_LONG
template <>
struct type_detect_t<irs_ilong_long>
{
  enum {
    index = signed_long_long_idx
  };
};
template <>
struct type_detect_t<irs_ulong_long>
{
  enum {
    index = unsigned_long_long_idx
  };
};
#endif //IRSDEFS_I64
template <>
struct type_detect_t<float>
{
  enum {
    index = float_idx
  };
};
template <>
struct type_detect_t<double>
{
  enum {
    index = double_idx
  };
};
template <>
struct type_detect_t<long double>
{
  enum {
    index = long_double_idx
  };
};

template<class T>
struct is_void_type
{
  enum {
    value = false
  };
};
template<>
struct is_void_type<void>
{
  enum {
    value = true
  };
};

template<class T>
struct is_bool_type
{
  enum {
    value = false
  };
};
template<>
struct is_bool_type<bool>
{
  enum {
    value = true
  };
};

template<class T>
struct is_integral_type
{
  enum {
    value = false
  };
};
template<>
struct is_integral_type<char>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<signed char>
{
  enum {
    value = true
  };
};

template<>
struct is_integral_type<unsigned char>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<wchar_t>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<short>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<unsigned short>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<int>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<unsigned int>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<long>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<unsigned long>
{
  enum {
    value = true
  };
};
#ifdef IRSDEFS_LONG_LONG
template<>
struct is_integral_type<irs_ilong_long>
{
  enum {
    value = true
  };
};
template<>
struct is_integral_type<irs_ulong_long>
{
  enum {
    value = true
  };
};
#endif //IRSDEFS_I64

template<class T>
struct is_floating_point_type
{
  enum {
    value = false
  };
};
template<>
struct is_floating_point_type<float>
{
  enum {
    value = true
  };
};
template<>
struct is_floating_point_type<double>
{
  enum {
    value = true
  };
};
template<>
struct is_floating_point_type<long double>
{
  enum {
    value = true
  };
};

template<class T>
struct is_number_type
{
  enum {
    value = is_integral_type<T>::value || is_floating_point_type<T>::value
  };
};

template<class T>
struct is_fundamental_type
{
  enum {
    value = is_void_type<T>::value ||
      is_bool_type<T>::value ||
      is_integral_type<T>::value ||
      is_floating_point_type<T>::value
  };
};

template<class T>
struct is_string_type
{
  enum {
    value = false
  };
};

template<>
struct is_string_type< ::string>
{
  enum {
    value = true
  };
};

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template<>
struct is_string_type< ::wstring>
{
  enum {
    value = true
  };
};
#endif //IRS_FULL_STDCPPLIB_SUPPORT

template<>
struct is_string_type<irs_string_t>
{
  enum {
    value = true
  };
};

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template<>
struct is_string_type<irs_wstring_t>
{
  enum {
    value = true
  };
};
#endif //IRS_FULL_STDCPPLIB_SUPPORT

#if defined(__BORLANDC__)
template<>
struct is_string_type<AnsiString>
{
  enum {
    value = true
  };
};

template<>
struct is_string_type<WideString>
{
  enum {
    value = true
  };
};
#endif // defined(__BORLANDC__)

// Определение знаковости типа
struct signed_type_tag {};
struct unsigned_type_tag {};
template <bool s>
struct integer_signed_detect_base_t
{
};
template <>
struct integer_signed_detect_base_t<true>
{
  typedef signed_type_tag sign_type;
  enum {
    signed_value = signed_idx
  };
};
template <>
struct integer_signed_detect_base_t<false>
{
  typedef unsigned_type_tag sign_type;
  enum {
    signed_value = unsigned_idx
  };
};
template <class T>
struct integer_sign_detect_t:
  integer_signed_detect_base_t<T(-1) < 0>
{
};
template <class T>
bool is_type_signed()
{
  signed_t signT =
    static_cast<signed_t>(integer_sign_detect_t<T>::signed_value);
  return signT == signed_idx;
}
struct no_type
{
};
struct yes_type
{
};
template<class T1, class T2>
struct is_equal_types
{
  enum { value = 0 };
  typedef no_type type;
};
template<class T>
struct is_equal_types<T, T>
{
  enum { value = 1 };
  typedef yes_type type;
};
// Взятие типов относительно указанного по размеру
template <size_t size_of_type, class sign_of_type>
struct type_relative_by_size_t
{
};
template <>
struct type_relative_by_size_t<int8_size, signed_type_tag>
{
  typedef irs_i16 larger_type;
  typedef irs_i8 smaller_type;
};
template <>
struct type_relative_by_size_t<int8_size, unsigned_type_tag>
{
  typedef irs_u16 larger_type;
  typedef irs_u8 smaller_type;
};
template <>
struct type_relative_by_size_t<int16_size, signed_type_tag>
{
  typedef irs_i32 larger_type;
  typedef irs_i8 smaller_type;
};
template <>
struct type_relative_by_size_t<int16_size, unsigned_type_tag>
{
  typedef irs_u32 larger_type;
  typedef irs_u8 smaller_type;
};
template <>
struct type_relative_by_size_t<int32_size, signed_type_tag>
{
  #ifdef IRSDEFS_I64
  typedef irs_i64 larger_type;
  #else //IRSDEFS_I64
  typedef irs_i32 larger_type;
  #endif //IRSDEFS_I64
  typedef irs_i16 smaller_type;
};
template <>
struct type_relative_by_size_t<int32_size, unsigned_type_tag>
{
  #ifdef IRSDEFS_I64
  typedef irs_u64 larger_type;
  #else //IRSDEFS_I64
  typedef irs_u32 larger_type;
  #endif //IRSDEFS_I64
  typedef irs_u16 smaller_type;
};
#ifdef IRSDEFS_I64
template <>
struct type_relative_by_size_t<int64_size, signed_type_tag>
{
  typedef irs_i64 larger_type;
  typedef irs_i32 smaller_type;
};
template <>
struct type_relative_by_size_t<int64_size, unsigned_type_tag>
{
  typedef irs_u64 larger_type;
  typedef irs_u32 smaller_type;
};
#endif //IRSDEFS_I64

// Взятие типов относительно указанного
template <class T>
struct type_relative_t
{
  typedef void signed_type;
  typedef void unsigned_type;
  typedef void opposite_signed_type;
  typedef void larger_type;
  typedef void smaller_type;
};
template <class T>
struct char_relative_t
{
};
template <>
struct char_relative_t<signed_type_tag>
{
  typedef signed char signed_type;
  typedef unsigned char unsigned_type;
  typedef unsigned char opposite_signed_type;
};
template <>
struct char_relative_t<unsigned_type_tag>
{
  typedef signed char signed_type;
  typedef unsigned char unsigned_type;
  typedef signed char opposite_signed_type;
};
template <>
struct type_relative_t<char>:
  char_relative_t<integer_sign_detect_t<char>::sign_type >,
  type_relative_by_size_t<
    sizeof(char),
    integer_sign_detect_t<char>::sign_type
  >
{
  #ifdef NOP
  typedef char_relative_t<integer_sign_detect_t<char>::sign_type >
    base_type;

  typedef base_type::signed_type signed_type;
  typedef base_type::unsigned_type unsigned_type;
  typedef base_type::opposite_signed_type opposite_signed_type;
  #endif //NOP
};
template <>
struct type_relative_t<signed char>:
  type_relative_by_size_t<
    sizeof(signed char),
    integer_sign_detect_t<signed char>::sign_type
  >
{
  typedef signed char signed_type;
  typedef unsigned char unsigned_type;
  typedef unsigned char opposite_signed_type;
};
template <>
struct type_relative_t<unsigned char>:
  type_relative_by_size_t<
    sizeof(unsigned char),
    integer_sign_detect_t<unsigned char>::sign_type
  >
{
  typedef signed char signed_type;
  typedef unsigned char unsigned_type;
  typedef signed char opposite_signed_type;
};
template <>
struct type_relative_t<signed short>:
  type_relative_by_size_t<
    sizeof(signed short),
    integer_sign_detect_t<signed short>::sign_type
  >
{
  typedef signed short signed_type;
  typedef unsigned short unsigned_type;
  typedef unsigned short opposite_signed_type;
};
template <>
struct type_relative_t<unsigned short>:
  type_relative_by_size_t<
    sizeof(unsigned short),
    integer_sign_detect_t<unsigned short>::sign_type
  >
{
  typedef signed short signed_type;
  typedef unsigned short unsigned_type;
  typedef signed short opposite_signed_type;
};
template <>
struct type_relative_t<signed int>:
  type_relative_by_size_t<
    sizeof(signed int),
    integer_sign_detect_t<signed int>::sign_type
  >
{
  typedef signed int signed_type;
  typedef unsigned int unsigned_type;
  typedef unsigned int opposite_signed_type;
};
template <>
struct type_relative_t<unsigned int>:
  type_relative_by_size_t<
    sizeof(unsigned int),
    integer_sign_detect_t<unsigned int>::sign_type
  >
{
  typedef signed int signed_type;
  typedef unsigned int unsigned_type;
  typedef signed int opposite_signed_type;
};
template <>
struct type_relative_t<signed long>:
  type_relative_by_size_t<
    sizeof(signed long),
    integer_sign_detect_t<signed long>::sign_type
  >
{
  typedef signed long signed_type;
  typedef unsigned long unsigned_type;
  typedef unsigned long opposite_signed_type;
};
template <>
struct type_relative_t<unsigned long>:
  type_relative_by_size_t<
    sizeof(unsigned long),
    integer_sign_detect_t<unsigned long>::sign_type
  >
{
  typedef signed long signed_type;
  typedef unsigned long unsigned_type;
  typedef signed long opposite_signed_type;
};
#ifdef IRSDEFS_I64
template <>
struct type_relative_t<irs_ilong_long>:
  type_relative_by_size_t<
    sizeof(irs_ilong_long),
    integer_sign_detect_t<irs_ilong_long>::sign_type
  >
{
  typedef irs_ilong_long signed_type;
  typedef irs_ulong_long unsigned_type;
  typedef irs_ulong_long opposite_signed_type;
};
template <>
struct type_relative_t<irs_ulong_long>:
  type_relative_by_size_t<
    sizeof(irs_ulong_long),
    integer_sign_detect_t<irs_ulong_long>::sign_type
  >
{
  typedef irs_ilong_long signed_type;
  typedef irs_ulong_long unsigned_type;
  typedef irs_ilong_long opposite_signed_type;
};
#endif //IRSDEFS_I64

// Функция для тестирования type_relative_t
template <class T>
void type_relative_test(ostream& a_strm)
{
  a_strm << "test of type \"irs::type_relative_t\"" << endl;
  a_strm << "original_type: ";
  a_strm << type_to_string<T>();
  a_strm << endl;
  a_strm << "signed_type: ";
  a_strm << type_to_string<typename type_relative_t<T>::signed_type>();
  a_strm << endl;
  a_strm << "unsigned_type: ";
  a_strm << type_to_string<typename type_relative_t<T>::unsigned_type>();
  a_strm << endl;
  a_strm << "opposite_signed_type: ";
  a_strm << type_to_string<typename type_relative_t<T>::
    opposite_signed_type>();
  a_strm << endl;
  a_strm << "larger_type: ";
  a_strm << type_to_string<typename type_relative_t<T>::larger_type>();
  a_strm << endl;
  a_strm << "smaller_type: ";
  a_strm << type_to_string<typename type_relative_t<T>::smaller_type>();
  a_strm << endl;
  a_strm << endl;
}

// Отображение параметров встроенных типов
inline void display_parametrs_of_built_in_types(ostream &strm)
{
  strm << "char is signed: " << bool_to_cstr(is_type_signed<char>());
  strm << endl;
  strm << "size of char: " << sizeof(char) << endl;
  strm << "wchar_t is signed: " << bool_to_cstr(is_type_signed<wchar_t>());
  strm << endl;
  strm << "size of wchar_t: " << sizeof(wchar_t) << endl;
  strm << "size of short: " << sizeof(short) << endl;
  strm << "size of int: " << sizeof(int) << endl;
  strm << "size of long: " << sizeof(long) << endl;
  #ifdef IRSDEFS_LONG_LONG
  strm << "size of irs_ilong_long: " << sizeof(irs_ilong_long) << endl;
  #endif // IRSDEFS_LONG_LONG
  strm << "size of float: " << sizeof(float) << endl;
  strm << "size of double: " << sizeof(double) << endl;
  strm << "size of long double: " << sizeof(long double) << endl;
  strm << "size_t is " << irs::type_to_string<size_t>() << endl;
  strm << "ptrdiff_t is " << irs::type_to_string<ptrdiff_t>() << endl;
  strm << endl;
}

// ENUM float_denorm_style
enum float_denorm_style
{ // constants for different IEEE float denormalization styles
  denorm_indeterminate = -1,
  denorm_absent = 0,
  denorm_present = 1
};

// ENUM float_round_style
enum float_round_style
{ // constants for different IEEE rounding styles
  round_indeterminate = -1,
  round_toward_zero = 0,
  round_to_nearest = 1,
  round_toward_infinity = 2,
  round_toward_neg_infinity = 3
};

// STRUCT _Num_base
struct numeric_limits_base
{ // base for all types, with common defaults
  static const float_denorm_style has_denorm = denorm_absent;
  static const bool has_denorm_loss = false;
  static const bool has_infinity = false;
  static const bool has_quiet_NaN = false;
  static const bool has_signaling_NaN = false;
  static const bool is_bounded = false;
  static const bool is_exact = false;
  static const bool is_iec559 = false;
  static const bool is_integer = false;
  static const bool is_modulo = false;
  static const bool is_signed = false;
  static const bool is_specialized = false;
  static const bool tinyness_before = false;
  static const bool traps = false;
  static const float_round_style round_style = round_toward_zero;
  static const int digits = 0;
  static const int digits10 = 0;
  static const int max_exponent = 0;
  static const int max_exponent10 = 0;
  static const int min_exponent = 0;
  static const int min_exponent10 = 0;
  static const int radix = 0;

  static const bool is_floating = false;
};

struct numeric_limits_float_base: public numeric_limits_base
{ // base for floating-point types
  static const float_denorm_style has_denorm = denorm_present;
  static const bool has_denorm_loss = true;
  static const bool has_infinity = true;
  static const bool has_quiet_NaN = true;
  static const bool has_signaling_NaN = true;
  static const bool is_bounded = true;
  static const bool is_exact = false;
  static const bool is_iec559 = true;
  static const bool is_integer = false;
  static const bool is_modulo = false;
  static const bool is_signed = true;
  static const bool is_specialized = true;
  static const bool tinyness_before = true;
  static const bool traps = true;
  static const float_round_style round_style = round_to_nearest;
  static const int radix = FLT_RADIX;

  static const bool is_floating = true;
};

template <size_t size>
struct unknown_type {
  irs_u8 val[size];
  unknown_type()
  {
    memset(static_cast<void*>(val), 0, size);
  }
};

template <size_t size_of_type>
struct float_limits_by_size_base
{
  enum { size = size_of_type };

  typedef irs_u8 exponent_type;
  typedef irs_u8 fraction_type;

  struct unknown_value_type: unknown_type<size>
  {
  };

  enum {
    exponent_idx = 0,
    fraction_idx = 0,
    exponent_bitsize = 0,
    fraction_bitsize = 0
  };
  static exponent_type exponent_mask()
  {
    return 0;
  }
  static fraction_type fraction_mask()
  {
    return 0;
  }
  static bool is_intel_spec()
  {
    return false;
  }
};
template <>
struct float_limits_by_size_base<float_size>
{
  enum { size = float_size };

  typedef irs_u16 exponent_type;
  typedef irs_u32 fraction_type;

  struct unknown_value_type: unknown_type<size>
  {
  };

  enum {
    exponent_idx = 2,
    fraction_idx = 0,
    exponent_bitsize = 8,
    fraction_bitsize = 23
  };
  static exponent_type exponent_mask()
  {
    return 0x7F80;
  }
  static fraction_type fraction_mask()
  {
    return 0x007FFFFF;
  }
};
template <>
struct float_limits_by_size_base<double_size>
{
  enum { size = double_size };

  typedef irs_u16 exponent_type;
  typedef irs_u64 fraction_type;

  struct unknown_value_type: unknown_type<size>
  {
  };

  enum {
    exponent_idx = 6,
    fraction_idx = 0,
    exponent_bitsize = 11,
    fraction_bitsize = 52
  };
  static exponent_type exponent_mask()
  {
    return 0x7FF0;
  }
  static fraction_type fraction_mask()
  {
    fraction_type mask = 0;
    IRS_HIDWORD(mask) = 0x000FFFFF;
    IRS_LODWORD(mask) = 0xFFFFFFFF;
    return mask;
  }
};
template <>
struct float_limits_by_size_base<long_double_size>
{
  enum { size = long_double_size };

  typedef irs_u16 exponent_type;
  typedef irs_u64 fraction_type;

  struct unknown_value_type: unknown_type<size>
  {
  };

  enum {
    exponent_idx = 8,
    fraction_idx = 0,
    exponent_bitsize = 15,
    fraction_bitsize = 64
  };
  static exponent_type exponent_mask()
  {
    return 0x7FFF;
  }
  static fraction_type fraction_mask()
  {
    fraction_type frac = 0;
    IRS_HIDWORD(frac) = 0xFFFFFFFF;
    IRS_LODWORD(frac) = 0xFFFFFFFF;
    return frac;
  }
  static bool is_intel_spec()
  {
    return true;
  }
};

struct float_limits_intel_spec_t
{
  typedef float_limits_by_size_base<long_double_size>::
    fraction_type fraction_type;

  enum {
    spec_fraction_shift = 62
  };

  static fraction_type spec_fraction_mask()
  {
    fraction_type frac = 0;
    IRS_HIDWORD(frac) = 0xC0000000;
    IRS_LODWORD(frac) = 0x00000000;
    return frac;
  }
  static fraction_type other_fraction_mask()
  {
    fraction_type frac = 0;
    IRS_HIDWORD(frac) = 0x3FFFFFFF;
    IRS_LODWORD(frac) = 0xFFFFFFFF;
    return frac;
  }
};

template <size_t size_of_type>
struct float_limits_by_size:
  float_limits_by_size_base<size_of_type>,
  numeric_limits_float_base
{
  typedef irs_u8 sign_type;
  typedef float_limits_by_size_base<size_of_type> base_type;

  typedef typename base_type::exponent_type exponent_type;
  typedef typename base_type::fraction_type fraction_type;

  typedef float_limits_intel_spec_t intel_spec_type;

  enum {
    sign_idx = 3,
    sign_bitsize = 1
  };

  enum {
    exponent_idx = base_type::exponent_idx,
    fraction_idx = base_type::fraction_idx,
    exponent_bitsize = base_type::exponent_bitsize,
    fraction_bitsize = base_type::fraction_bitsize
  };
  static exponent_type exponent_mask()
  {
    return base_type::exponent_mask();
  }
  static fraction_type fraction_mask()
  {
    return base_type::fraction_mask();
  }
  static bool is_intel_spec()
  {
    return base_type::is_intel_spec();
  }

  static sign_type sign_mask()
  {
    return 0x80;
  }

  static sign_type& sign(irs_u8* ap_val)
  {
    return reinterpret_cast<sign_type&>(ap_val[sign_idx]);
  }
  static exponent_type& exponent(irs_u8* ap_val)
  {
    return reinterpret_cast<exponent_type&>(ap_val[exponent_idx]);
  }
  static const exponent_type& exponent(const irs_u8* ap_val)
  {
    return reinterpret_cast<const exponent_type&>(ap_val[exponent_idx]);
  }
  static fraction_type& fraction(irs_u8* ap_val)
  {
    return reinterpret_cast<fraction_type&>(ap_val[fraction_idx]);
  }

  static bool is_inf_or_nan(const irs_u8* ap_val)
  {
    bool is_inf_or_nan_res = false;
    const exponent_type mask = exponent_mask();
    const exponent_type exp = exponent(ap_val);
    is_inf_or_nan_res = ((exp&mask) == mask);
    return is_inf_or_nan_res;
  }
  static bool is_nan(const irs_u8* ap_val)
  {
    bool is_nan_res = false;
    if (is_inf_or_nan(ap_val)) {
      if (is_intel_spec()) {
        // В процессорах Intel используется 10-байтный вещественный тип
        // В языке C++ это long double
        // Под Borland C++, Visual C++, Watcom C++ он 10 байт
        // Под MinGW он 12 байт, но используются только младшие 10
        // Старшие 2 байта игнорируются
        // Обработка бесконечности и NAN в числах intel устроена по другому
        // Здесь в NAN собрано все, кроме бесконечности. Включая то, что
        // считается у Intel некорректным
        const fraction_type spec_mask = intel_spec_type::spec_fraction_mask();
        const fraction_type spec_bits = fraction(ap_val)&spec_mask;
        const fraction_type other_mask = intel_spec_type::other_fraction_mask();
        const fraction_type other_bits = ((fraction(ap_val)&other_mask) >>
          intel_spec_type::spec_fraction_shift);
        bool is_infinity = ((other_bits == 2) && (spec_bits == 0));
        is_nan_res = !is_infinity;
      } else {
        const fraction_type mask = fraction_mask();
        if ((fraction(ap_val)&mask) != 0) {
          is_nan_res = true;
        }
      }
    }
    return is_nan_res;
  }
  static bool is_inf(const irs_u8* ap_val)
  {
    return is_inf_or_nan(ap_val) && (!is_nan(ap_val));
  }

  #ifdef NOP
  static int unknown_quiet_NaN()
  { // return non-signaling NaN
    unknown_value_type qnan = unknown_value_type();
    irs_u8* p_qnan_u8 = reinterpret_cast<irs_u8*>(&qnan);
    exponent_type& exp_ref =
      reinterpret_cast<exponent_type&>(p_qnan_u8[exponent_idx]);
    fraction_type& frac_ref =
      reinterpret_cast<fraction_type&>(p_qnan_u8[fraction_idx]);
    exp_ref |= exponent_mask();
    frac_ref |= fraction_mask();
    return qnan;
  }
  #endif //NOP
};
template <class T>
struct float_limits_by_type: float_limits_by_size<sizeof(T)>
{
  typedef T value_type;
  typedef float_limits_by_size<sizeof(value_type)> base_type;

  typedef typename base_type::sign_type sign_type;
  typedef typename base_type::exponent_type exponent_type;
  typedef typename base_type::fraction_type fraction_type;

  static sign_type& sign(irs_u8* ap_val)
  {
    return base_type::sign(ap_val);
  }
  static exponent_type& exponent(irs_u8* ap_val)
  {
    return base_type::exponent(ap_val);
  }
  static fraction_type& fraction(irs_u8* ap_val)
  {
    return base_type::fraction(ap_val);
  }
  static bool is_inf_or_nan(const irs_u8* ap_val)
  {
    return base_type::is_inf_or_nan(ap_val);
  }
  static bool is_nan(const irs_u8* ap_val)
  {
    return base_type::is_nan(ap_val);
  }
  static bool is_inf(const irs_u8* ap_val)
  {
    return base_type::is_inf(ap_val);
  }

  static sign_type& sign(value_type& a_val)
  {
    return base_type::sign(reinterpret_cast<irs_u8*>(&a_val));
  }
  static exponent_type& exponent(value_type& a_val)
  {
    return base_type::exponent(reinterpret_cast<irs_u8*>(&a_val));
  }
  static fraction_type& fraction(value_type& a_val)
  {
    return base_type::fraction(reinterpret_cast<irs_u8*>(&a_val));
  }
  static bool is_inf_or_nan(const value_type& a_val)
  {
    return base_type::is_inf_or_nan(reinterpret_cast<const irs_u8*>(&a_val));
  }
  static bool is_nan(const value_type& a_val)
  {
    return base_type::is_nan(reinterpret_cast<const irs_u8*>(&a_val));
  }
  static bool is_inf(const value_type& a_val)
  {
    return base_type::is_inf(reinterpret_cast<const irs_u8*>(&a_val));
  }
  static T mark_bad()
  {
    return static_cast<T>(3e33f);
  }
};

// TEMPLATE CLASS numeric_limits
template<class T>
struct numeric_limits: numeric_limits_base
{ // numeric limits for arbitrary type T (say little or nothing)
  typedef T value_type;
  typedef irs_u8 sign_type;
  typedef irs_u8 exponent_type;
  typedef irs_u8 fraction_type;

  static value_type (min)()
  { // return minimum value
    return (value_type(0));
  }

  static value_type (max)()
  { // return maximum value
    return (value_type(0));
  }

  static value_type epsilon()
  { // return smallest effective increment from 1.0
    return (value_type(0));
  }

  static value_type round_error()
  { // return largest rounding error
    return (value_type(0));
  }

  static value_type denorm_min()
  { // return minimum denormalized value
    return (value_type(0));
  }

  static value_type infinity()
  { // return positive infinity
    return (value_type(0));
  }

  static value_type quiet_NaN()
  { // return non-signaling NaN
    return (value_type(0));
  }

  static value_type signaling_NaN()
  { // return signaling NaN
    return (value_type(0));
  }

  static const sign_type& sign(const irs_u8* ap_val)
  {
    return reinterpret_cast<sign_type&>(*ap_val);
  }
  static const exponent_type& exponent(const irs_u8* ap_val)
  {
    return reinterpret_cast<exponent_type&>(*ap_val);
  }
  static const fraction_type& fraction(const irs_u8* ap_val)
  {
    return reinterpret_cast<fraction_type&>(*ap_val);
  }
  static bool is_inf_or_nan(const irs_u8* /*ap_val*/)
  {
    return false;
  }
  static bool is_nan(const irs_u8* /*ap_val*/)
  {
    return false;
  }
  static bool is_inf(const irs_u8* /*ap_val*/)
  {
    return false;
  }

  static sign_type& sign(const value_type& a_val)
  {
    return reinterpret_cast<sign_type&>(a_val);
  }
  static exponent_type& exponent(const value_type& a_val)
  {
    return reinterpret_cast<sign_type&>(a_val);
  }
  static fraction_type& fraction(const value_type& a_val)
  {
    return reinterpret_cast<sign_type&>(a_val);
  }
  static bool is_inf_or_nan(const value_type& /*a_val*/)
  {
    return false;
  }
  static bool is_nan(const value_type& /*a_val*/)
  {
    return false;
  }
  static bool is_inf(const value_type& /*a_val*/)
  {
    return false;
  }
  static value_type mark_bad()
  {
    return static_cast<value_type>(-1);
  }
};

// CLASS numeric_limits<float>
template<>
struct numeric_limits<float>: float_limits_by_type<float>
{ // limits for type float
  static value_type (min)()
  { // return minimum value
    return (FLT_MIN);
  }

  static value_type (max)()
  { // return maximum value
    return (FLT_MAX);
  }

  static value_type epsilon()
  { // return smallest effective increment from 1.0
    return (FLT_EPSILON);
  }

  static value_type round_error()
  { // return largest rounding error
    return (0.5f);
  }

  #ifdef NOP
  static value_type denorm_min()
  { // return minimum denormalized value
    return (_FDenorm._Float);
  }

  static value_type infinity()
  { // return positive infinity
    return (_FInf._Float);
  }

  static value_type quiet_NaN()
  { // return non-signaling NaN
    return (_FNan._Float);
  }

  static value_type signaling_NaN()
  { // return signaling NaN
    return (_FSnan._Float);
  }

  static const int digits = FLT_MANT_DIG;
  static const int digits10 = FLT_DIG;
  static const int max_exponent = (int)FLT_MAX_EXP;
  static const int max_exponent10 = (int)FLT_MAX_10_EXP;
  static const int min_exponent = (int)FLT_MIN_EXP;
  static const int min_exponent10 = (int)FLT_MIN_10_EXP;
  #endif //NOP
};

// CLASS numeric_limits<double>
template<>
struct numeric_limits<double>: float_limits_by_type<double>
{
  static value_type (max)()
  { // return maximum value
    return (DBL_MAX);
  }
};

// CLASS numeric_limits<long double>
template<>
struct numeric_limits<long double>: float_limits_by_type<long double>
{
  static value_type (max)()
  { // return maximum value
    return (LDBL_MAX);
  }
};

template <class T>
bool is_inf_or_nan(const irs_u8* a_val)
{
  return numeric_limits<T>::is_inf_or_nan(a_val);
}
template <class T>
bool is_nan(const irs_u8* a_val)
{
  return numeric_limits<T>::is_nan(a_val);
}
template <class T>
bool is_inf(const irs_u8* a_val)
{
  return numeric_limits<T>::is_inf(a_val);
}
template <class T>
bool is_inf_or_nan(const T& a_val)
{
  return numeric_limits<T>::is_inf_or_nan(a_val);
}
template <class T>
bool is_nan(const T& a_val)
{
  return numeric_limits<T>::is_nan(a_val);
}
template <class T>
bool is_inf(const T& a_val)
{
  return numeric_limits<T>::is_inf(a_val);
}

template <class T>
T inf()
{
  //irs_u8 num_in_byte_array[sizeof(T)] = {0};
  //memset(reinterpret_cast<void*>(num_in_byte_array), 0,
    //sizeof(num_in_byte_array));
  //typedef typename numeric_limits<T>::exponent_type exponent_t;
  T inf_ret = T();
  numeric_limits<T>::exponent(inf_ret) =
    numeric_limits<T>::exponent_mask();
  #ifdef NOP
  exponent_t& exponent = reinterpret_cast<exponent_t& >(
    num_in_byte_array[numeric_limits<T>::exponent_idx]);
  exponent = numeric_limits<T>::exponent_mask();
  #endif //NOP
  if (numeric_limits<T>::is_intel_spec()) {
    typedef typename numeric_limits<T>::fraction_type fraction_t;
    const int spec_fraction_shift = static_cast<int>(
      numeric_limits<T>::intel_spec_type::spec_fraction_shift);

    numeric_limits<T>::fraction(inf_ret) =
      (static_cast<fraction_t>(2) << spec_fraction_shift);
  } else {
    // Для не-Intel типов ничего с мантиссой не делаем
  }
  return inf_ret;
}

//! @}

} //namespace irs

#endif //IRSLIMITSH
