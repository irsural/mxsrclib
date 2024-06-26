//! \file
//! \ingroup single_type_group
//! \brief ��� variant
//!
//! ����: 16.05.2010\n
//! ������ ����: 12.10.2009

#ifndef irsvariantH
#define irsvariantH

#ifndef __WATCOMC__

#include <irsdefs.h>

/*#ifdef __WIN32__
#include <winsock2.h>
#endif // __WIN32__
*/
#ifdef __BORLANDC__
# include <ComCtrls.hpp>
#endif // __BORLANDC__

#include <irscpp.h>
#include <irsstd.h>
#include <mxdata.h>
#include <irserror.h>
#include <irsstrdefs.h>

#include <irsfinal.h>

namespace irs {

namespace variant {

//! \addtogroup single_type_group
//! @{

typedef size_t sizens_t;
typedef char_t charns_t;
typedef string_t stringns_t;

// �������� ���� ������ ���� � ������������ �������,
// ��� ��� ��� ������������ � ��������� ��������
enum var_type_t {
  var_type_first,                     // ������ ���
  var_type_unknown = var_type_first,  // ���, ����������������� �� ���������
  var_type_bool,
  var_type_char,
  var_type_singned_char,
  var_type_unsigned_char,
  var_type_short,
  var_type_unsigned_short,
  var_type_int,
  var_type_unsigned_int,
  var_type_long,
  var_type_unsigned_long,
  var_type_float,
  var_type_double,
  var_type_long_double,
  #ifdef IRSDEFS_LONG_LONG
  var_type_long_long,
  var_type_unsigned_long_long,
  #endif // IRSDEFS_LONG_LONG
  var_type_void_ptr,
  var_type_const_void_ptr,
  //var_type_ptr_char,
  var_type_string,
  var_type_array,
  var_type_last = var_type_array,     // ��������� ���
  var_type_count                      // ���������� �����
};

inline bool is_number_type(const var_type_t a_var_type)
{
  #ifdef IRSDEFS_LONG_LONG
  return ((a_var_type >= var_type_bool) &&
    (a_var_type <= var_type_unsigned_long_long)) ? true : false;
  #else
  return ((a_var_type >= var_type_bool) &&
    (a_var_type <= var_type_long_double)) ? true : false;
  #endif // IRSDEFS_LONG_LONG
}

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
  typedef char_t char_type;
  typedef stringns_t string_type;
  typedef vector<variant_t> vector_variant_type;
  #ifdef IRSDEFS_LONG_LONG
  typedef irs_ilong_long long_long_type;
  typedef irs_ulong_long unsigned_long_long_type;
  #endif // IRSDEFS_LONG_LONG

  variant_t();
  variant_t(const variant_t& a_variant);
  variant_t(const bool a_value);
  variant_t(const char a_value);
  variant_t(const signed char a_value);
  variant_t(const unsigned char a_value);
  variant_t(const short a_value);
  variant_t(const unsigned short a_value);
  variant_t(const int a_value);
  variant_t(const unsigned int a_value);
  variant_t(const long a_value);
  variant_t(const unsigned long a_value);
  variant_t(const float a_value);
  variant_t(const double a_value);
  variant_t(const long double a_value);
  #ifdef IRSDEFS_LONG_LONG
  variant_t(const long_long_type a_value);
  variant_t(const unsigned_long_long_type a_value);
  #endif // IRSDEFS_LONG_LONG
  variant_t(const string_type& a_value);

  #ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
  template<class T>
  variant_t(const T& a_value, const var_type_t a_var_type):
    m_value(),
    m_type(a_var_type)
  {
    assign_no_cast(a_value);
  }
  #endif // IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
  ~variant_t();
  inline var_type_t type() const;
  void type(const var_type_t a_variant_type);
  void swap(variant_t& a_variant);

  template <class T>
  variant_t& operator=(const T& a_value);
  variant_t& operator=(const variant_t& a_variant);

  bool as_bool() const;
  char as_char() const;
  signed char as_signed_char() const;
  unsigned char as_unsigned_char() const;
  short as_short() const;
  unsigned short as_unsigned_short() const;
  int as_int() const;
  unsigned int as_unsigned_int() const;
  long as_long() const;
  unsigned long as_unsigned_long() const;
  float as_float() const;
  double as_double() const;
  long double as_long_double() const;
  #ifdef IRSDEFS_LONG_LONG
  long_long_type as_long_long() const;
  unsigned_long_long_type as_unsigned_long_long() const;
  #endif // IRSDEFS_LONG_LONG
  void* as_void_ptr() const;
  const void* as_const_void_ptr() const;
  string_type as_string() const;
  //signed char is_array() const;
  template <class T>
  T as_type() const;

  operator bool() const;
  operator char() const;
  operator signed char() const;
  operator unsigned char() const;
  operator short() const;
  operator unsigned short() const;
  operator int() const;
  operator unsigned int() const;
  operator long() const;
  operator unsigned long() const;
  operator float() const;
  operator double() const;
  operator long double() const;
  #ifdef IRSDEFS_LONG_LONG
  operator long_long_type() const;
  operator unsigned_long_long_type() const;
  #endif // IRSDEFS_LONG_LONG
  operator void*() const;
  operator const void*() const;
  //operator const char_type*() const;
  operator string_type() const;

  variant_t& operator+=(const variant_t& a_variant);
  variant_t& operator-=(const variant_t& a_variant);
  variant_t& operator*=(const variant_t& a_variant);
  variant_t& operator/=(const variant_t& a_variant);
  variant_t& operator%=(const variant_t& a_variant);
  variant_t& operator++();
  variant_t operator++(int);
  variant_t& operator--();
  variant_t operator--(int);

  template <class T>
  variant_t& assign(const T& a_value);
  #ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
  template <class T>
  variant_t& assign_no_cast(const T& a_value);
  #endif // IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
  template <class T>
  bool try_assign_no_cast(const T& a_value);
  bool convert_to(const var_type_t a_var_type);

  inline size_type size() const;
  inline void resize(const size_type a_new_size);
  inline void clear();
  inline variant_t& operator[](const size_type a_index);
  inline const variant_t& operator[](const size_type a_index) const;
  
private:
  union value_t {
    void* p_void_type;
    const void* p_const_void_type;
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
    #ifdef IRSDEFS_LONG_LONG
    long_long_type val_long_long_type;
    unsigned_long_long_type val_ulong_long_type;
    #endif // IRSDEFS_LONG_LONG
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
  bool is_type_number(var_type_t a_var_type);
};

template <class T>
variant_t& variant_t::operator=(const T& a_value)
{
  variant_t variant(a_value);
  swap(variant);
  return *this;
}

inline var_type_t variant_t::type() const
{
  return m_type;
}

/*template<class T>
variant_t::operator T()const
{
  T value;
  value_get(&value);
  return value;
}*/

template <class T>
T variant_t::as_type() const
{
  return value_get<T>();
}

template <class T>
variant_t& variant_t::assign(const T& a_value)
{
  return operator=(a_value);
}

#ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

template<class T>
variant_t& variant_t::assign_no_cast(const T& a_value)
{
  switch(m_type) {
    #ifdef _MSC_VER
    # pragma warning(disable: 4800)
    #endif // _MSC_VER  
    case var_type_bool: {
      m_value.val_bool_type = static_cast<bool>(a_value);
    } break;
    #ifdef _MSC_VER
    # pragma warning(default: 4800)
    #endif // _MSC_VER  
    case var_type_char: {
      m_value.val_char_type = static_cast<char>(a_value);
    } break;
    case var_type_singned_char: {
      m_value.val_schar_type = static_cast<signed char>(a_value);
    } break;
    case var_type_unsigned_char: {
      m_value.val_uchar_type = static_cast<unsigned char>(a_value);
    } break;
    case var_type_short: {
      m_value.val_short_type = static_cast<short>(a_value);
    } break;
    case var_type_unsigned_short: {
      m_value.val_ushort_type = static_cast<unsigned short>(a_value);
    } break;
    case var_type_int: {
      m_value.val_int_type = static_cast<int>(a_value);
    } break;
    case var_type_unsigned_int: {
      m_value.val_uint_type = static_cast<unsigned int>(a_value);
    } break;
    case var_type_long: {
      m_value.val_long_type = static_cast<long>(a_value);
    } break;
    case var_type_unsigned_long: {
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
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long: {
      m_value.val_long_long_type = static_cast<long_long_type>(a_value);
    } break;
    case var_type_unsigned_long_long: {
      m_value.val_ulong_long_type =
        static_cast<unsigned_long_long_type>(a_value);
    } break;
    #endif // IRSDEFS_LONG_LONG
    case var_type_string: {
      *m_value.p_val_string_type = static_cast<string_type>(a_value);
    } break;
    case var_type_unknown:
    case var_type_array: {
      IRS_LIB_ERROR(ec_standard, "������������ �������� ��� ������� ����");
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("����������� ���");
    }
  }
  return *this;
}

#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

#ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
template<>
inline variant_t& variant_t::assign_no_cast<variant_t::string_type>(
  const string_type& a_value)
{
  bool fsuccess = false;
  switch(m_type) {
    case var_type_bool: {
      fsuccess = a_value.to_number(m_value.val_bool_type);
    } break;
    case var_type_char: { // !!! ������ � �� ����� !!!
      //fsuccess = a_value.to_number(m_value.val_char_type);
      if (m_value.p_val_string_type->size() >= 1) {
        m_value.val_char_type = IRS_SIMPLE_FROM_TYPE_STR(
          m_value.p_val_string_type->c_str())[0];
        fsuccess = true;
      } else {
        fsuccess = false;
      }
    } break;
    case var_type_singned_char: {
      fsuccess = a_value.to_number(m_value.val_schar_type);
    } break;
    case var_type_unsigned_char: {
      fsuccess = a_value.to_number(m_value.val_uchar_type);
    } break;
    case var_type_short: {
      fsuccess = a_value.to_number(m_value.val_short_type);
    } break;
    case var_type_unsigned_short: {
      fsuccess = a_value.to_number(m_value.val_ushort_type);
    } break;
    case var_type_int: {
      fsuccess = a_value.to_number(m_value.val_int_type);
    } break;
    case var_type_unsigned_int: {
      fsuccess = a_value.to_number(m_value.val_uint_type);
    } break;
    case var_type_long: {
      fsuccess = a_value.to_number(m_value.val_long_type);
    } break;
    case var_type_unsigned_long: {
      fsuccess = a_value.to_number(m_value.val_ulong_type);
    } break;
    case var_type_float: {
      fsuccess = a_value.to_number(m_value.val_float_type);
    } break;
    case var_type_double: {
      fsuccess = a_value.to_number(m_value.val_double_type);
    } break;
    case var_type_long_double: {
      fsuccess = a_value.to_number(m_value.val_long_double_type);
    } break;
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long: {
      fsuccess = a_value.to_number(m_value.val_long_long_type);
    } break;
    case var_type_unsigned_long_long: {
      fsuccess = a_value.to_number(m_value.val_ulong_long_type);
    } break;
    #endif // IRSDEFS_LONG_LONG
    case var_type_string: {
      *m_value.p_val_string_type = a_value;
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("����������� ���");
    }
  }
  if (fsuccess) {
    IRS_LIB_ERROR(ec_standard, "�������������� ����������� ��������");
  } else {
    // �������������� �������          
  }
  return *this;
}
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

#ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
template<>
inline variant_t& variant_t::assign_no_cast<variant_t>(
  const variant_t& a_variant)
{
  switch(m_type) {
    case var_type_bool: {
      m_value.val_bool_type = a_variant.as_bool();
    } break;
    case var_type_char: {
      m_value.val_char_type = a_variant.as_char();
    } break;
    case var_type_singned_char: {
      m_value.val_schar_type = a_variant.as_signed_char();
    } break;
    case var_type_unsigned_char: {
      m_value.val_uchar_type = a_variant.as_unsigned_char();
    } break;
    case var_type_short: {
      m_value.val_short_type = a_variant.as_short();
    } break;
    case var_type_unsigned_short: {
      m_value.val_ushort_type = a_variant.as_unsigned_short();
    } break;
    case var_type_int: {
      m_value.val_int_type = a_variant.as_int();
    } break;
    case var_type_unsigned_int: {
      m_value.val_uint_type = a_variant.as_unsigned_int();
    } break;
    case var_type_long: {
      m_value.val_long_type = a_variant.as_long();
    } break;
    case var_type_unsigned_long: {
      m_value.val_ulong_type = a_variant.as_unsigned_long();
    } break;
    case var_type_float: {
      m_value.val_float_type = a_variant.as_float();
    } break;
    case var_type_double: {
      m_value.val_double_type = a_variant.as_double();
    } break;
    case var_type_long_double: {
      m_value.val_long_double_type = a_variant.as_long_double();
    } break;
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long: {
      m_value.val_long_long_type = a_variant.as_long_long();
    } break;
    case var_type_unsigned_long_long: {
      m_value.val_ulong_long_type = a_variant.as_unsigned_long_long();
    } break;
    #endif // IRSDEFS_LONG_LONG
    case var_type_string: {
      *m_value.p_val_string_type = a_variant.as_string();
    } break;
    case var_type_unknown:
      IRS_LIB_ERROR(ec_standard, "������������ �������� ��� ������� ����");
      // fall through
    case var_type_array: {
      if (a_variant.type() == var_type_array) {
        *m_value.p_val_string_type = *a_variant.m_value.p_val_string_type;
      } else {
        IRS_LIB_ERROR(ec_standard, "������������ ��������");
      }
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("����������� ���");
    }
  }
  return *this;
}
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

#ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
template<>
inline variant_t& variant_t::assign_no_cast<void*>(void* const& ap_void)
{
  if (m_type == var_type_void_ptr) {
    m_value.p_void_type = ap_void;
  } else if (m_type == var_type_const_void_ptr) {
    m_value.p_const_void_type = ap_void;
  } else {
    IRS_LIB_ASSERT_MSG("������������ ���");
  }
  return *this;
}
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

#ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
template<>
inline variant_t& variant_t::assign_no_cast<const void*>(
  const void* const& ap_void)
{
  if (m_type == var_type_const_void_ptr) {
    m_value.p_const_void_type = ap_void;
  } else {
    IRS_LIB_ASSERT_MSG("������������ ���");
  }
  return *this;
}
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

template<class T>
bool variant_t::try_assign_no_cast(const T& a_value)
{
  variant_t variant(a_value);
  bool status = variant.convert_to(type());
  if (status) {
    assign(variant);
  }
  return status;
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
  T value = T();
  switch(m_type) {    

    #ifdef _MSC_VER
    # pragma warning(disable: 4800)
    #endif // _MSC_VER
    
    case var_type_bool: {
      value = static_cast<T>(m_value.val_bool_type);
    } break;
    case var_type_char: {
      value = static_cast<T>(m_value.val_char_type);
    } break;
    case var_type_singned_char: {
      value = static_cast<T>(m_value.val_schar_type);
    } break;
    case var_type_unsigned_char: {
      value = static_cast<T>(m_value.val_uchar_type);
    } break;
    case var_type_short: {
      value = static_cast<T>(m_value.val_short_type);
    } break;
    case var_type_unsigned_short: {
      value = static_cast<T>(m_value.val_ushort_type);
    } break;
    case var_type_int: {
      value = static_cast<T>(m_value.val_int_type);
    } break;
    case var_type_unsigned_int: {
      value = static_cast<T>(m_value.val_uint_type);
    } break;
    case var_type_long: {
      value = static_cast<T>(m_value.val_long_type);
    } break;
    case var_type_unsigned_long: {
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

    #ifdef IRSDEFS_LONG_LONG

    case var_type_long_long: {
      value = static_cast<T>(m_value.val_long_long_type);
    } break;
    case var_type_unsigned_long_long: {
      value = static_cast<T>(m_value.val_ulong_long_type);
    } break;

    #endif // IRSDEFS_LONG_LONG

    #ifdef _MSC_VER
    # pragma warning(default: 4800)
    #endif // _MSC_VER  

    case var_type_string: {
      bool fsuccess = m_value.p_val_string_type->to_number(value);
      if (!fsuccess) {
        IRS_LIB_ASSERT_MSG("������������� ������ � ����� �� �������");
      } else {
        // ������ ������� ������������� � �����
      }
    } break;
    default : {      
      IRS_LIB_ASSERT_MSG("����������� ���");
      // ����� ���������� � ����� ������ ���� ����������������
      //value = 0;
    }
  }
  return value;
}

#ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
template<>
inline void* variant_t::value_get<void*>() const
{
  void* p_value = IRS_NULL;
  if (m_type == var_type_void_ptr) {
    p_value = m_value.p_void_type;
  } else {
    IRS_LIB_ASSERT_MSG("������������ ��������");
  }
  return p_value;
}

template<>
inline const void* variant_t::value_get<const void*>() const
{
  const void* p_value = IRS_NULL;
  if (m_type == var_type_void_ptr) {
    p_value = m_value.p_void_type;
  } else if (m_type == var_type_const_void_ptr) {
    p_value = m_value.p_const_void_type;
  } else {
    IRS_LIB_ASSERT_MSG("������������ ��������");
  }
  return p_value;
}

template<>
inline variant_t::string_type
variant_t::value_get<variant_t::string_type>() const
{
  string_type value;
  switch(m_type) {
    case var_type_bool: {
      value = m_value.val_bool_type;
    } break;
    case var_type_char: {
      value = m_value.val_char_type;
    } break;
    case var_type_singned_char: {
      value = m_value.val_schar_type;
    } break;
    case var_type_unsigned_char: {
      value = m_value.val_uchar_type;
    } break;
    case var_type_short: {
      value = m_value.val_short_type;
    } break;
    case var_type_unsigned_short: {
      value = m_value.val_ushort_type;
    } break;
    case var_type_int: {
      value = m_value.val_int_type;
    } break;
    case var_type_unsigned_int: {
      value = m_value.val_uint_type;
    } break;
    case var_type_long: {
      value = m_value.val_long_type;
    } break;
    case var_type_unsigned_long: {
      value = m_value.val_ulong_type;
    } break;
    case var_type_float: {
      value = m_value.val_float_type;
    } break;
    case var_type_double: {
      value = m_value.val_double_type;
    } break;
    case var_type_long_double: {
      value = m_value.val_long_double_type;
    } break;
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long: {
      value = m_value.val_long_long_type;
    } break;
    case var_type_unsigned_long_long: {
      value = m_value.val_ulong_long_type;
    } break;
    #endif // IRSDEFS_LONG_LONG
    case var_type_string: {
      value = *(m_value.p_val_string_type);
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("������������ ��������");
    }
  }
  return value;
}

template<>
inline variant_t::vector_variant_type
variant_t::value_get<variant_t::vector_variant_type>() const
{
  IRS_LIB_ASSERT(m_type == var_type_array);
  return *(m_value.p_val_vector_variant);
}
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

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

#ifdef IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
template<>
inline void variant_t::prefix_operation<bool>(
  const operation_type_t /*a_unary_operation_type*/, bool* /*ap_value*/)
{
  // �� ��������� ANSI.ISO.IEC.14882.2003 �������� ���������� ��� ���� bool
  // ���������. �������� ���������� ��������� ���������� � �� ������������� �
  // ���������� � ����� ����������
  IRS_LIB_ASSERT_MSG("������������ ��������");
}
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

inline void swap(variant_t& a_first, variant_t& a_second)
{
  a_first.swap(a_second);
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

// Comparisons  (type void*)
#ifdef NOP
inline bool operator==(const variant_t& a_variant, const void* ap_value)
{
  return operator==(a_variant, variant_t(ap_value));
}

inline bool operator==(const void* ap_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(ap_value));
}

inline bool operator!=(const variant_t& a_variant, const void* ap_value)
{
  return operator!=(a_variant, variant_t(ap_value));
}

inline bool operator!=(const void* ap_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(ap_value));
}

inline bool operator<(const variant_t& a_variant, const void* ap_value)
{
  return operator<(a_variant, variant_t(ap_value));
}

inline bool operator<(const void* ap_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(ap_value));
}

inline bool operator>(const variant_t& a_variant, const void* ap_value)
{
  return operator>(a_variant, variant_t(ap_value));
}

inline bool operator>(const void* ap_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(ap_value));
}

inline bool operator<=(const variant_t& a_variant, const void* ap_value)
{
  return operator<=(a_variant, variant_t(ap_value));
}

inline bool operator<=(const void* ap_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(ap_value));
}

inline bool operator>=(const variant_t& a_variant, const void* ap_value)
{
  return operator>=(a_variant, variant_t(ap_value));
}

inline bool operator>=(const void* ap_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(ap_value));
}
#endif // NOP

// Comparisons  (type char)
inline bool operator==(const variant_t& a_variant, const char& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const char& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const char& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const char& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const char& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const char& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const char& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const char& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const char& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const char& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const char& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const char& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type unsigned char)
inline bool operator==(const variant_t& a_variant, const unsigned char& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const unsigned char& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const unsigned char& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const unsigned char& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const unsigned char& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const unsigned char& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const unsigned char& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const unsigned char& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const unsigned char& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const unsigned char& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const unsigned char& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const unsigned char& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type signed char)
inline bool operator==(const variant_t& a_variant, const signed char& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const signed char& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const signed char& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const signed char& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const signed char& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const signed char& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const signed char& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const signed char& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const signed char& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const signed char& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const signed char& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const signed char& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type short)
inline bool operator==(const variant_t& a_variant, const short& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const short& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const short& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const short& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const short& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const short& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const short& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const short& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const short& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const short& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const short& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const short& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type unsigned short)
inline bool operator==(const variant_t& a_variant,
  const unsigned short& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const unsigned short& a_value,
  const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant,
  const unsigned short& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const unsigned short& a_value,
  const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const unsigned short& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const unsigned short& a_value,
  const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const unsigned short& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const unsigned short& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant,
  const unsigned short& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const unsigned short& a_value,
  const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant,
  const unsigned short& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const unsigned short& a_value,
  const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type int)
inline bool operator==(const variant_t& a_variant, const int& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const int& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const int& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const int& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const int& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const int& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const int& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const int& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const int& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const int& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const int& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const int& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type unsigned int)
inline bool operator==(const variant_t& a_variant, const unsigned int& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const unsigned int& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const unsigned int& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const unsigned int& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const unsigned int& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const unsigned int& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const unsigned int& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const unsigned int& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const unsigned int& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const unsigned int& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const unsigned int& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const unsigned int& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type long)
inline bool operator==(const variant_t& a_variant, const long& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const long& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const long& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const long& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const long& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const long& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const long& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const long& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const long& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const long& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const long& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const long& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type unsigned long)
inline bool operator==(const variant_t& a_variant, const unsigned long& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const unsigned long& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const unsigned long& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const unsigned long& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const unsigned long& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const unsigned long& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const unsigned long& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const unsigned long& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const unsigned long& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const unsigned long& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const unsigned long& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const unsigned long& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type float)
inline bool operator==(const variant_t& a_variant, const float& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const float& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const float& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const float& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const float& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const float& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const float& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const float& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const float& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const float& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const float& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const float& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type double)
inline bool operator==(const variant_t& a_variant, const double& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const double& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const double& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const double& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const double& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const double& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const double& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const double& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const double& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const double& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const double& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const double& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type long double)
inline bool operator==(const variant_t& a_variant, const long double& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const long double& a_value, const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant, const long double& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const long double& a_value, const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant, const long double& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const long double& a_value, const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant, const long double& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const long double& a_value, const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant, const long double& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const long double& a_value, const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant, const long double& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const long double& a_value, const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

#ifdef IRSDEFS_LONG_LONG

// Comparisons  (type long long)
inline bool operator==(const variant_t& a_variant,
  const variant_t::long_long_type& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const variant_t::long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant,
  const variant_t::long_long_type& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t::long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant,
  const variant_t::long_long_type& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t::long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant,
  const variant_t::long_long_type& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t::long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant,
  const variant_t::long_long_type& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t::long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant,
  const variant_t::long_long_type& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t::long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

// Comparisons  (type unsigned_long_long_type)
inline bool operator==(const variant_t& a_variant,
  const variant_t::unsigned_long_long_type& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const variant_t::unsigned_long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant,
  const variant_t::unsigned_long_long_type& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t::unsigned_long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant,
  const variant_t::unsigned_long_long_type& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t::unsigned_long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant,
  const variant_t::unsigned_long_long_type& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t::unsigned_long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant,
  const variant_t::unsigned_long_long_type& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t::unsigned_long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant,
  const variant_t::unsigned_long_long_type& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t::unsigned_long_long_type& a_value,
  const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}

#endif // IRSDEFS_LONG_LONG

// Comparisons  (type string_type)
inline bool operator==(const variant_t& a_variant,
  const variant_t::string_type& a_value)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator==(const variant_t::string_type& a_value,
  const variant_t& a_variant)
{
  return operator==(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t& a_variant,
  const variant_t::string_type& a_value)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator!=(const variant_t::string_type& a_value,
  const variant_t& a_variant)
{
  return operator!=(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t& a_variant,
  const variant_t::string_type& a_value)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator<(const variant_t::string_type& a_value,
  const variant_t& a_variant)
{
  return operator<(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t& a_variant,
  const variant_t::string_type& a_value)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator>(const variant_t::string_type& a_value,
  const variant_t& a_variant)
{
  return operator>(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t& a_variant,
  const variant_t::string_type& a_value)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator<=(const variant_t::string_type& a_value,
  const variant_t& a_variant)
{
  return operator<=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t& a_variant,
  const variant_t::string_type& a_value)
{
  return operator>=(a_variant, variant_t(a_value));
}

inline bool operator>=(const variant_t::string_type& a_value,
  const variant_t& a_variant)
{
  return operator>=(a_variant, variant_t(a_value));
}



/*template <class T>
bool operator==(const variant_t& a_variant, const T& a_value)
{
  bool result = false;
  binary_operation(operation_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator==(const T& a_value, const variant_t& a_variant)
{
  bool result = false;
  binary_operation(operation_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator!=(const variant_t& a_variant, const T& a_value)
{
  bool result = false;
  binary_operation(operation_not_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator!=(const T& a_value, const variant_t& a_variant)
{
  bool result = false;
  binary_operation(operation_not_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator<(const variant_t& a_variant, const T& a_value)
{
  bool result = false;
  binary_operation(operation_less,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator<(const T& a_value, const variant_t& a_variant)
{
  bool result = false;
  binary_operation(operation_less,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator>(const variant_t& a_variant, const T& a_value)
{
  bool result = false;
  binary_operation(operation_more,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator>(const T& a_value, const variant_t& a_variant)
{
  bool result = false;
  binary_operation(operation_more,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator<=(const variant_t& a_variant, const T& a_value)
{
  bool result = false;
  binary_operation(operation_less_or_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator<=(const T& a_value, const variant_t& a_variant)
{
  bool result = false;
  binary_operation(operation_less_or_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator>=(const variant_t& a_variant, const T& a_value)
{
  bool result = false;
  binary_operation(operation_more_or_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}

template <class T>
bool operator>=(const T& a_value, const variant_t& a_variant)
{
  bool result = false;
  binary_operation(operation_more_or_equal,
    a_variant, variant_t(a_value), IRS_NULL, &result);
  return result;
}*/     

struct var_unknown_type_tag
{
};

struct var_integer_type_tag
{
};

struct var_floating_type_tag
{
};

struct var_string_type_tag
{
};

struct var_const_void_ptr_type_tag
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
  typedef var_string_type_tag group_type;
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

#ifdef IRSDEFS_LONG_LONG

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

#endif // IRSDEFS_LONG_LONG

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

template <>
struct var_type_traits_t<void*>
{
  typedef var_const_void_ptr_type_tag group_type;
};

template <>
struct var_type_traits_t<const void*>
{
  typedef var_const_void_ptr_type_tag group_type;
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

/*template <>
bool variant_compare<variant_t>(
  const operation_type_t a_var_operation_type,
  const variant_t& a_first_var,
  const variant_t& a_last_var)
{
  bool variants_equals = true;
  bool result = false;
  if (a_first_var.type() == a_last_var.type()) {
    if (a_first_var.type() == var_type_unknown) {
      variants_equals = true;
    } else {
      variants_equals = (a_first_var == a_last_var);
    }
  } else {
    variants_equals = false;
  }
  switch (a_var_operation_type) {
    case operation_equal: {
      result = variants_equals;
    } break;
    case operation_not_equal: {
      result = !variants_equals;
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("������������ ��� ��������");
    }
  }
  return result;
}*/

template <class T>
bool binary_operation_helper(
  const operation_type_t /*a_var_operation_type*/,
  const T& /*a_first_var*/,
  const T& /*a_second_var*/,
  T* /*ap_result*/,
  var_container_type_tag)
{
  bool operation_is_executed = false;
  // ���������� ��� ��������
  return operation_is_executed;
}

template <class T>
bool binary_operation_helper(
  const operation_type_t a_var_operation_type,
  const T& a_first_var,
  const T& a_second_var,
  T* ap_result,
  var_string_type_tag)
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
      a_second_var, ap_result, var_string_type_tag());
  if (!operation_is_executed) {
    operation_is_executed = true;
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

template <class T>
bool binary_operation_helper(
  const operation_type_t /*a_var_operation_type*/,
  const T& /*a_first_var*/,
  const T& /*a_second_var*/,
  T* /*ap_result*/,
  var_const_void_ptr_type_tag)
{ 
  // ���������� ����� ��������
  return false;
}

template<class T>
void operation_helper(
  const operation_type_t a_var_operation_type,
  const T& a_first_variant,
  const T& a_second_variant,
  T* ap_result_value,
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
    IRS_LIB_ASSERT(ap_result_value != IRS_NULL);
    binary_operation(a_var_operation_type, a_first_variant,
      a_second_variant, ap_result_value);
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
  binary_operation_helper(a_var_operation_type, a_first_var, a_second_var,
    ap_result, group_type());
}

sizens_t get_max_depth(const variant_t& a_variant);

bool is_uniform_dimension(
  const irs::variant::variant_t& a_variant,
  vector<irs::variant::sizens_t>* ap_size_array = IRS_NULL
);

bool is_uniform_type(const variant_t& a_variant);

bool is_uniform_type(const variant_t& a_variant,
  const irs::variant::var_type_t ap_var_type);

class var_type_converter_t
{
public:
  typedef sizens_t size_type;
  typedef stringns_t string_type;  

  typedef map<var_type_t, string_type> table_var_type_and_str_type;
  typedef map<var_type_t, string_type>::const_iterator
    table_var_type_and_str_const_iterator;
  typedef map<string_type, var_type_t> table_str_and_var_type_type;
  typedef map<string_type, var_type_t>::const_iterator
    table_str_and_var_type_const_iterator;

  var_type_converter_t();
  void to_str(const var_type_t a_var_type, string_type* ap_var_type_str);
  bool to_type(const string_type& a_var_type_str, var_type_t* ap_var_type);
private:
  table_var_type_and_str_type m_table_var_type_and_str;
  table_str_and_var_type_type m_table_str_and_var_type;
};

#ifdef __BORLANDC__
void variant_to_tree_view(
  const variant_t& a_variant,
  TTreeView* ap_tree_view,
  TTreeNode* ap_tree_node = IRS_NULL);
#endif // #ifdef __BORLANDC__

#ifdef IRS_LIB_DEBUG
void test_variant();
#endif // IRS_LIB_DEBUG

//! @}

} // namespace variant

} // namespace irs

#endif //__WATCOMC__

#endif //irsvariantH
