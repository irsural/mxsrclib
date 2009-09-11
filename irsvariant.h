//  Component Variant
//  Data: 9.09.09
//
//  Copyright (c) 2009
//  IRS Company
//
//  Copyright (c) 2009
//  Maksim Lyashchov
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.

#ifndef irsvariantH
#define irsvariantH

#include <irscpp.h>
#include <irsstd.h>
#include <mxdata.h>
#include <irserror.h>

namespace irs {

namespace variant {
//#ifdef NOP

enum var_type_t {
  var_type_unknown,
  var_type_bool,
  var_type_char,
  var_type_schar,
  var_type_uchar,
  var_type_short,
  var_type_ushort,
  var_type_int,
  var_type_uint,
  var_type_long,
  var_type_ulong,
  var_type_float,
  var_type_double,
  var_type_long_double,
  #ifdef IRSDEFS_I64
  var_type_long_long,
  var_type_ulong_long,
  #endif // IRSDEFS_I64
  var_type_string,
  var_type_array
};

enum operation_type_t {
  operation_unknown,
  operation_equal,
  operation_not_equal,
  operation_less,
  operation_more,
  operation_less_or_equal,
  operation_more_or_equal,
  operation_sum,
  operation_difference,
  operation_multiplication,
  operation_division,
  operation_integer_division,
  operation_prefix_increment,
  operation_prefix_decrement,
  operation_postfix_increment,
  operation_postfix_decrement};

inline bool operation_is_compare(
  const operation_type_t a_operation_type)
{
  return ((a_operation_type >= operation_equal) &&
    (a_operation_type <= operation_more_or_equal));
}
inline bool operation_is_arithmetic(
  const operation_type_t a_operation_type)
{
  return ((a_operation_type >= operation_sum) &&
    (a_operation_type <= operation_postfix_decrement));
}
class variant_t;

void binary_operation(
  const operation_type_t a_operation_type,
  const variant_t& a_first_variant,
  const variant_t& a_second_variant,
  variant_t* ap_result_variant,
  bool* ap_result_bool);

variant_t operator+(const variant_t& a_first_variant,
  const variant_t& a_second_variant);
variant_t operator-(const variant_t& a_first_variant,
  const variant_t& a_second_variant);
variant_t operator*(const variant_t& a_first_variant,
  const variant_t& a_second_variant);
variant_t operator/(const variant_t& a_first_variant,
  const variant_t& a_second_variant);
variant_t operator%(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

class variant_t
{
public:
  typedef size_t size_type;
  typedef irs::string string_type;
  typedef vector<variant_t> vector_variant_type;
  #ifdef IRSDEFS_I64
  typedef irs_i64 long_long_type;
  typedef irs_u64 unsigned_long_long_type;
  #endif // IRSDEFS_I64
  variant_t();

  template<class T>
  variant_t(const T& a_value):
    m_type(var_type_int),
    m_value()
  {
    operator=(a_value);
  }

  variant_t(const variant_t& a_variant);

  ~variant_t();
  inline var_type_t type() const;
  void type(const var_type_t a_variant_type);
  variant_t& operator=(const variant_t& a_variant);
  variant_t& operator=(const bool a_value);
  variant_t& operator=(const char a_value);
  variant_t& operator=(const signed char a_value);
  variant_t& operator=(const unsigned char a_value);
  variant_t& operator=(const short a_value);
  variant_t& operator=(const unsigned short a_value);
  variant_t& operator=(const int a_value);
  variant_t& operator=(const unsigned int a_value);
  variant_t& operator=(const long a_value);
  variant_t& operator=(const unsigned long a_value);
  variant_t& operator=(const float a_value);
  variant_t& operator=(const double a_value);
  variant_t& operator=(const long double a_value);
  #ifdef IRSDEFS_I64
  variant_t& operator=(const long_long_type a_value);
  variant_t& operator=(const unsigned_long_long_type a_value);
  #endif // IRSDEFS_I64
  variant_t& operator=(const string_type& a_value);
  variant_t& operator+=(const variant_t& a_variant);
  variant_t& operator-=(const variant_t& a_variant);
  variant_t& operator*=(const variant_t& a_variant);
  variant_t& operator/=(const variant_t& a_variant);
  variant_t& operator%=(const variant_t& a_variant);
  variant_t& operator++();
  variant_t operator++(int);
  variant_t& operator--();
  variant_t operator--(int);
  
  template<class T>
  operator T()const;
  /*{
    T value;
    value_get(&value);
    return value;
  }*/

  template<class T>
  variant_t& assign_no_cast(const T& a_value);  

  inline size_type size() const;
  inline void resize(const size_type a_new_size);
  inline void clear();
  inline variant_t& operator[](const size_type a_index);
  inline const variant_t& operator[](const size_type a_index) const;
  
private:
  union {
    bool val_bool_type;
    char val_char_type;
    signed char val_schar_type;
    unsigned char val_uchar_type;
    short val_short_type;
    unsigned short val_ushort_type;
    int val_int_type;
    unsigned int val_uint_type;
    long val_long_type;
    unsigned long val_ulong_type;
    float val_float_type;
    double val_double_type;
    long double val_long_double_type;
    #ifdef IRSDEFS_I64
    long_long_type val_long_long_type;
    unsigned_long_long_type val_ulong_long_type;
    #endif // IRSDEFS_I64
    string_type* p_val_string_type;
    vector_variant_type* p_val_vector_variant;
  } m_value;
  var_type_t m_type;
  template<class T>
  T value_get() const;

  template <class T>
  void value_get(T* ap_value) const
  {
    *ap_value = value_get<T>();
  }

  friend void binary_operation(
    const operation_type_t a_operation_type,
    const variant_t& a_first_variant,
    const variant_t& a_second_variant,
    variant_t* ap_result_variant,
    bool* ap_result_bool);

  void type_change(const var_type_t a_variant_type);

  template<class T>
  void prefix_operation(const operation_type_t a_unary_operation_type,
    T* ap_value);

  void prefix_operation(const operation_type_t a_unary_operation_type);
};

inline var_type_t variant_t::type() const
{
  return m_type;
}

template<class T>
variant_t::operator T()const
{
  T value;
  value_get(&value);
  return value;
}   

template<class T>
variant_t& variant_t::assign_no_cast(const T& a_value)
{
  switch(m_type) {
    case var_type_bool: {
      m_value.val_bool_type = static_cast<bool>(a_value);
    } break;
    case var_type_char: {
      m_value.val_char_type = static_cast<char>(a_value);
    } break;
    case var_type_schar: {
      m_value.val_schar_type = static_cast<signed char>(a_value);
    } break;
    case var_type_uchar: {
      m_value.val_uchar_type = static_cast<unsigned char>(a_value);
    } break;
    case var_type_short: {
      m_value.val_short_type = static_cast<short>(a_value);
    } break;
    case var_type_ushort: {
      m_value.val_ushort_type = static_cast<unsigned short>(a_value);
    } break;
    case var_type_int: {
      m_value.val_int_type = static_cast<int>(a_value);
    } break;
    case var_type_uint: {
      m_value.val_uint_type = static_cast<unsigned int>(a_value);
    } break;
    case var_type_long: {
      m_value.val_long_type = static_cast<long>(a_value);
    } break;
    case var_type_ulong: {
      m_value.val_ulong_type = static_cast<unsigned long>(a_value);
    } break;
    case var_type_float: {
      m_value.val_float_type = static_cast<float>(a_value);
    } break;
    case var_type_double: {
      m_value.val_double_type = static_cast<double>(a_value);
    } break;
    case var_type_long_double: {
      m_value.val_long_double_type = static_cast<long double>(a_value);
    } break;
#ifdef IRSDEFS_I64
    case var_type_long_long: {
      m_value.val_long_long_type = static_cast<long_long_type>(a_value);
    } break;
    case var_type_ulong_long: {
      m_value.val_ulong_long_type =
          static_cast<unsigned_long_long_type>(a_value);
    } break;
#endif // IRSDEFS_I64
      /*case variant_string_type: {
    m_value.p_val_string_type = static_cast<string_type>(a_value);
  } break;*/
    default : {
      IRS_LIB_ASSERT_MSG("����������� ���");
    }
  }
  return *this;
}

template<>
inline variant_t& variant_t::assign_no_cast<variant_t::string_type>(
  const string_type& a_value)
{
  if (m_type == var_type_string) {
    *m_value.p_val_string_type = a_value;
  } else {
    IRS_LIB_ASSERT_MSG("���������� ������������� string_type � ������ ����");
  }
  return *this;
}

inline variant_t::size_type variant_t::size() const
{
  size_type container_size = 0;
  if (m_type == var_type_array) {
    container_size = m_value.p_val_vector_variant->size();
  } else {
    IRS_LIB_ERROR(ec_user, "������������ �������� ��� ������ �����");
  }
  return container_size;
}

inline void variant_t::resize(const size_type a_new_size)
{
  if (m_type == var_type_array) {
    m_value.p_val_vector_variant->resize(a_new_size, variant_t());
  } else {
    IRS_LIB_ERROR(ec_user, "������������ �������� ��� ������ �����");
  }
}

inline void variant_t::clear()
{
  #ifdef IRS_LIB_DEBUG
  if (m_type ) {
    IRS_LIB_ERROR(ec_user, "������������ �������� ��� ������ �����");   
  } else {
    // ���������� �������� ��� ������� ����
  }
  #endif // IRS_LIB_DEBUG
  m_value.p_val_vector_variant->clear();
}

inline variant_t& variant_t::operator[](const size_type a_index)
{
  #ifdef IRS_LIB_DEBUG
  if (m_type != var_type_array) {
    IRS_LIB_ERROR(ec_standard, "������������ �������� ��� ������ �����");
  } else {
    // ��� ��������
  }
  #endif // IRS_LIB_DEBUG
  return (*m_value.p_val_vector_variant)[a_index];
}

inline const variant_t& variant_t::operator[](const size_type a_index) const
{
  #ifdef IRS_LIB_DEBUG
  if (m_type != var_type_array) {
    IRS_LIB_ERROR(ec_standard, "������������ �������� ��� ������ �����");
  } else {
    // ��� ��������
  }
  #endif // IRS_LIB_DEBUG
  return (*m_value.p_val_vector_variant)[a_index];
}

template<class T>
T variant_t::value_get() const
{
  T value;
  switch(m_type) {
    case var_type_bool: {
      value = static_cast<T>(m_value.val_bool_type);
    } break;
    case var_type_char: {
      value = static_cast<T>(m_value.val_char_type);
    } break;
    case var_type_schar: {
      value = static_cast<T>(m_value.val_schar_type);
    } break;
    case var_type_uchar: {
      value = static_cast<T>(m_value.val_uchar_type);
    } break;
    case var_type_short: {
      value = static_cast<T>(m_value.val_short_type);
    } break;
    case var_type_ushort: {
      value = static_cast<T>(m_value.val_ushort_type);
    } break;
    case var_type_int: {
      value = static_cast<T>(m_value.val_int_type);
    } break;
    case var_type_uint: {
      value = static_cast<T>(m_value.val_uint_type);
    } break;
    case var_type_long: {
      value = static_cast<T>(m_value.val_long_type);
    } break;
    case var_type_ulong: {
      value = static_cast<T>(m_value.val_ulong_type);
    } break;
    case var_type_float: {
      value = static_cast<T>(m_value.val_float_type);
    } break;
    case var_type_double: {
      value = static_cast<T>(m_value.val_double_type);
    } break;
    case var_type_long_double: {
      value = static_cast<T>(m_value.val_long_double_type);
    } break;
    #ifdef IRSDEFS_I64
    case var_type_long_long: {
      value = static_cast<T>(m_value.val_long_long_type);
    } break;
    case var_type_ulong_long: {
      value = static_cast<T>(m_value.val_ulong_long_type);
    } break;
    #endif // IRSDEFS_I64
    /*case variant_string_type: {
      IRS_STATIC_ASSERT(m_type == variant_string_type);
      value = *(m_value.p_val_string_type);
    } break;*/
    default : {
      IRS_LIB_ASSERT_MSG("����������� ���");
    }
  }
  return value;
}

template<>
inline variant_t::string_type
variant_t::value_get<variant_t::string_type>() const
{
  return *(m_value.p_val_string_type);
}

template<>
inline variant_t::vector_variant_type
variant_t::value_get<variant_t::vector_variant_type>() const
{
  return *(m_value.p_val_vector_variant);
}

template<class T>
void variant_t::prefix_operation(const operation_type_t a_unary_operation_type,
  T* a_value)
{
  if (a_unary_operation_type == operation_prefix_increment) {
    ++(*a_value);
  } else if (a_unary_operation_type == operation_prefix_decrement) {
    --(*a_value);
  } else {
    IRS_LIB_ASSERT_MSG("������������ ��������");
  }
}

template<>
inline void variant_t::prefix_operation(
  const operation_type_t a_unary_operation_type, bool* ap_value)
{
  // �������� ���������� ��� ���� bool ���������
  // �� ��������� ANSI.ISO.IEC.14882.2003
  if (a_unary_operation_type == operation_prefix_increment) {
    ++(*ap_value);
  } else {
    IRS_LIB_ASSERT_MSG("������������ ��������");
  }
}

bool operator==(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

bool operator!=(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

bool operator<(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

bool operator>(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

bool operator<=(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

bool operator>=(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

/*variant_t operator+(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

variant_t operator-(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

variant_t operator*(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

variant_t operator/(const variant_t& a_first_variant,
  const variant_t& a_second_variant);

variant_t operator%(const variant_t& a_first_variant,
  const variant_t& a_second_variant);*/


struct var_unknown_type_tag
{
};

struct var_integer_type_tag
{
};

struct var_floating_type_tag
{
};

struct var_container_type_tag
{
};

template <class T>
struct var_type_traits_t
{
  typedef var_unknown_type_tag group_type;
};

template <>
struct var_type_traits_t<variant_t::string_type>
{
  typedef var_container_type_tag group_type;
};

template <>
struct var_type_traits_t<variant_t::vector_variant_type>
{
  typedef var_container_type_tag group_type;
};

template <>
struct var_type_traits_t<bool>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<char>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<signed char>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<unsigned char>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<short>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<unsigned short>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<int>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<unsigned int>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<long>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<unsigned long>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<variant_t::long_long_type>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<variant_t::unsigned_long_long_type>
{
  typedef var_integer_type_tag group_type;
};

template <>
struct var_type_traits_t<float>
{
  typedef var_floating_type_tag group_type;
};

template <>
struct var_type_traits_t<double>
{
  typedef var_floating_type_tag group_type;
};

template <>
struct var_type_traits_t<long double>
{
  typedef var_floating_type_tag group_type;
};

template <class T>
bool variant_compare(const operation_type_t a_var_operation_type,
  const T& a_first_var,
  const T& a_last_var)
{
  bool result = false;
  switch (a_var_operation_type) {
    case operation_equal: {
      result = (a_first_var == a_last_var);
    } break;
    case operation_not_equal: {
      result = (a_first_var != a_last_var);
    } break;
    case operation_less: {
      result = (a_first_var < a_last_var);
    } break;
    case operation_more: {
      result = (a_first_var > a_last_var);
    } break;
    case operation_less_or_equal: {
      result = (a_first_var <= a_last_var);
    } break;
    case operation_more_or_equal: {
      result = (a_first_var >= a_last_var);
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("������������ ��� ��������");
    }
  }
  return result;
}

template <class T>
bool binary_operation_helper(
  const operation_type_t a_var_operation_type,
  const T& a_first_var,
  const T& a_second_var,
  T* ap_result,
  var_container_type_tag)
{
  bool operation_is_executed = false;
  if (a_var_operation_type == operation_sum) {
    *ap_result = a_first_var + a_second_var;
    operation_is_executed = true;
  } else {
    // ������ �������� ����������
  }
  return operation_is_executed;
}

template <class T>
bool binary_operation_helper(
  const operation_type_t a_var_operation_type,
  const T& a_first_var,
  const T& a_second_var,
  T* ap_result,
  var_floating_type_tag)
{
  bool operation_is_executed =
    binary_operation_helper(a_var_operation_type, a_first_var,
      a_second_var, ap_result, var_container_type_tag());
  if (!operation_is_executed) {
    switch (a_var_operation_type) {
      case operation_difference: {
        *ap_result = a_first_var - a_second_var;
      } break;
      case operation_multiplication: {
        *ap_result = a_first_var * a_second_var;
      } break;
      case operation_division: {
        *ap_result = a_first_var / a_second_var;
      } break;
      default : {
        // ������ ���� ����������
        operation_is_executed = false;
      }
    }
  } else {
    // �������� ���������
  }
  return operation_is_executed;
}

template <class T>
bool binary_operation_helper(
  const operation_type_t a_var_operation_type,
  const T& a_first_var,
  const T& a_second_var,
  T* ap_result,
  var_integer_type_tag)
{
  bool operation_is_executed = binary_operation_helper(
    a_var_operation_type, a_first_var, a_second_var, ap_result,
      var_floating_type_tag());
  if (!operation_is_executed) {
    if (a_var_operation_type == operation_integer_division) {
      *ap_result = a_first_var % a_second_var;
      operation_is_executed = true;
    } else {
      // ������ �������� ����������
    }
  } else {
    // �������� ���������
  }
  return operation_is_executed;
}

template<class T>
void operation_helper(
  const operation_type_t a_var_operation_type,
  const T& a_first_variant,
  const T& a_second_variant,
  T* ap_result_variant,
  bool* ap_result_bool)
{
  if ((a_var_operation_type >= operation_equal) &&
      (a_var_operation_type <= operation_more_or_equal))
  {
    IRS_LIB_ASSERT(ap_result_bool != IRS_NULL);
    *ap_result_bool =
      variant_compare(a_var_operation_type, a_first_variant, a_second_variant);
  } else if ((a_var_operation_type >= operation_sum) &&
      (a_var_operation_type <= operation_integer_division))
  {
    IRS_LIB_ASSERT(ap_result_variant != IRS_NULL);
    binary_operation(a_var_operation_type, a_first_variant,
      a_second_variant, ap_result_variant);
  } else {
    IRS_LIB_ASSERT_MSG("���������� ��� ��������");
  }
}

template<class T>
void binary_operation(
  const operation_type_t a_var_operation_type,
  const T& a_first_var,
  const T& a_second_var,
  T* ap_result)
{
  typedef typename var_type_traits_t<T>::group_type group_type;
  binary_operation_helper(
    a_var_operation_type, a_first_var, a_second_var, ap_result,
      group_type());
}

#ifdef IRS_LIB_DEBUG
void test_variant();
#endif // IRS_LIB_DEBUG

//#endif // NOP

} // namespace variant

} // namespace irs
#endif
