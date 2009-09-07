// Характеристики чисел
// Дата: 4.02.2008

#ifndef IRSLIMITSH
#define IRSLIMITSH

#include <float.h>

#include <irsdefs.h>

namespace irs {

template <class T>
const char* type_to_string()
{
  return "unknown";
}
template <>
inline const char* type_to_string<float>()
{
  return "float";
}
template <>
inline const char* type_to_string<double>()
{
  return "double";
}
template <>
inline const char* type_to_string<long double>()
{
  return "long double";
}

enum {
  float_size = 4,
  double_size = 8,
  long_double_size = 10
};
enum {
  int8_size = 1,
  int16_size = 2,
  int32_size = 4,
  int64_size = 8
};
enum sign_t {
  signed_idx,
  unsigned_idx
};
enum std_type_idx_t {
  char_idx,
  signed_char_idx,
  unsigned_char_idx,
  signed_short_idx,
  unsigned_short_idx,
  signed_int_idx,
  unsigned_int_idx,
  signed_long_idx,
  unsigned_long_idx,
  float_idx,
  double_idx,
  long_double_idx
};

// ENUM float_denorm_style
enum float_denorm_style
{	// constants for different IEEE float denormalization styles
  denorm_indeterminate = -1,
  denorm_absent = 0,
  denorm_present = 1
};

// ENUM float_round_style
enum float_round_style
{	// constants for different IEEE rounding styles
  round_indeterminate = -1,
  round_toward_zero = 0,
  round_to_nearest = 1,
  round_toward_infinity = 2,
  round_toward_neg_infinity = 3
};

// STRUCT _Num_base
struct numeric_limits_base
{	// base for all types, with common defaults
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
{	// base for floating-point types
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
  enum { size = float_size };

  typedef int exponent_type;
  typedef int fraction_type;

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
  enum { size = double_size };

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

  enum {
    sign_idx = 3,
    sign_bitsize = 1,
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

  static sign_type sign_mask()
  {
    return 0x80;
  }

  static const sign_type& sign(const irs_u8* ap_val)
  {
    return reinterpret_cast<const sign_type&>(ap_val[sign_idx]);
  }
  static const exponent_type& exponent(const irs_u8* ap_val)
  {
    return reinterpret_cast<const exponent_type&>(ap_val[exponent_idx]);
  }
  static const fraction_type& fraction(const irs_u8* ap_val)
  {
    return reinterpret_cast<const fraction_type&>(ap_val[fraction_idx]);
  }
  static bool is_inf_or_nan(const irs_u8* ap_val)
  {
    bool is_inf_or_nan_res = false;
    const exponent_type mask = exponent_mask();
    const exponent_type exp = exponent(ap_val);
    if ((exp&mask) == mask)
    {
      is_inf_or_nan_res = true;
    }
    return is_inf_or_nan_res;
  }
  static bool is_nan(const irs_u8* ap_val)
  {
    bool is_nan_res = false;
    if (is_inf_or_nan(ap_val)) {
      const fraction_type mask = fraction_mask();
      if ((fraction(ap_val)&mask) != 0) {
        is_nan_res = true;
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
  {	// return non-signaling NaN
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

  static const sign_type& sign(const irs_u8* ap_val)
  {
    return base_type::sign(ap_val);
  }
  static const exponent_type& exponent(const irs_u8* ap_val)
  {
    return base_type::exponent(ap_val);
  }
  static const fraction_type& fraction(const irs_u8* ap_val)
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
  
  static const sign_type& sign(const value_type& a_val)
  {
    return base_type::sign(reinterpret_cast<const irs_u8*>(&a_val));
  }
  static const exponent_type& exponent(const value_type& a_val)
  {
    return base_type::exponent(reinterpret_cast<const irs_u8*>(&a_val));
  }
  static const fraction_type& fraction(const value_type& a_val)
  {
    return base_type::fraction(reinterpret_cast<const irs_u8*>(&a_val));
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
  {	// return minimum value
    return (value_type(0));
  }

  static value_type (max)()
  {	// return maximum value
    return (value_type(0));
  }

  static value_type epsilon()
  {	// return smallest effective increment from 1.0
    return (value_type(0));
  }

  static value_type round_error()
  {	// return largest rounding error
    return (value_type(0));
  }

  static value_type denorm_min()
  {	// return minimum denormalized value
    return (value_type(0));
  }

  static value_type infinity()
  {	// return positive infinity
    return (value_type(0));
  }

  static value_type quiet_NaN()
  {	// return non-signaling NaN
    return (value_type(0));
  }

  static value_type signaling_NaN()
  {	// return signaling NaN
    return (value_type(0));
  }

  static const sign_type& sign(const irs_u8* ap_val)
  {
    return reinterpret_cast<const sign_type&>(*ap_val);
  }
  static const exponent_type& exponent(const irs_u8* ap_val)
  {
    return reinterpret_cast<const exponent_type&>(*ap_val);
  }
  static const fraction_type& fraction(const irs_u8* ap_val)
  {
    return reinterpret_cast<const fraction_type&>(*ap_val);
  }
  static bool is_inf_or_nan(const irs_u8* ap_val)
  {
    return false;
  }
  static bool is_nan(const irs_u8* ap_val)
  {
    return false;
  }
  static bool is_inf(const irs_u8* ap_val)
  {
    return false;
  }
  
  static const sign_type& sign(const value_type& a_val)
  {
    return reinterpret_cast<const sign_type&>(a_val);
  }
  static const exponent_type& exponent(const value_type& a_val)
  {
    return reinterpret_cast<const sign_type&>(a_val);
  }
  static const fraction_type& fraction(const value_type& a_val)
  {
    return reinterpret_cast<const sign_type&>(a_val);
  }
  static bool is_inf_or_nan(const value_type& a_val)
  {
    return false;
  }
  static bool is_nan(const value_type& a_val)
  {
    return false;
  }
  static bool is_inf(const value_type& a_val)
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
{	// limits for type float
  static value_type (min)()
  {	// return minimum value
    return (FLT_MIN);
  }

  static value_type (max)()
  {	// return maximum value
    return (FLT_MAX);
  }

  static value_type epsilon()
  {	// return smallest effective increment from 1.0
    return (FLT_EPSILON);
  }

  static value_type round_error()
  {	// return largest rounding error
    return (0.5f);
  }

  #ifdef NOP
  static value_type denorm_min()
  {	// return minimum denormalized value
    return (_FDenorm._Float);
  }

  static value_type infinity() 
  {	// return positive infinity
    return (_FInf._Float);
  }

  static value_type quiet_NaN() 
  {	// return non-signaling NaN
    return (_FNan._Float);
  }

  static value_type signaling_NaN()
  {	// return signaling NaN
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
};

// CLASS numeric_limits<long double>
template<>
struct numeric_limits<long double>: float_limits_by_type<long double>
{
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

} //namespace irs

#endif //IRSLIMITSH
