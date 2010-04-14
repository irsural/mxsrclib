// Тип variant
// Дата: 10.04.2010
// Ранняя дата: 19.10.2009

#include <irsdefs.h>

#ifdef __BORLANDC__
# include <vcl.h>
# pragma hdrstop
#endif // __BORLANDC__

#include <irsvariant.h>

#include <irsfinal.h>

#ifndef __WATCOMC__

irs::variant::variant_t::variant_t():
  m_value(),
  m_type(var_type_unknown)
{
}

irs::variant::variant_t::variant_t(const variant_t& a_variant):
  m_value(),
  m_type(var_type_unknown)
{
  operator=(a_variant);
}

irs::variant::variant_t::~variant_t()
{
  if (m_type == var_type_string) {
    IRS_LIB_ASSERT(m_value.p_val_string_type != IRS_NULL);
    delete m_value.p_val_string_type;
  } else if (m_type == var_type_array) {
    IRS_LIB_ASSERT(m_value.p_val_vector_variant != IRS_NULL);
    delete m_value.p_val_vector_variant;     
  } else {
    // Дополнительные действия не требуются
  }
}

void irs::variant::variant_t::type_change(const var_type_t a_variant_type)
{
  if (m_type == var_type_string) {
    if (a_variant_type != var_type_string) {
      IRS_LIB_ASSERT(m_value.p_val_string_type != IRS_NULL);
      delete m_value.p_val_string_type;
      m_value.p_val_string_type = IRS_NULL;
    } else {
      // Дополнительные действия не требуются
    }
  } else if (m_type == var_type_array) {
    if (a_variant_type != var_type_array) {
      IRS_LIB_ASSERT(m_value.p_val_vector_variant != IRS_NULL);
      delete m_value.p_val_vector_variant;
      m_value.p_val_vector_variant = IRS_NULL;
    } else {
      // Дополнительные действия не требуются
    }
  } else {
    // Текущий тип не требует дополнительных действий
  }
  switch(a_variant_type) {
    case var_type_unknown:
    case var_type_void_ptr:
    case var_type_const_void_ptr:
    case var_type_bool:
    case var_type_char:
    case var_type_singned_char:
    case var_type_unsigned_char:
    case var_type_short:
    case var_type_unsigned_short:
    case var_type_int:
    case var_type_unsigned_int:
    case var_type_long:
    case var_type_unsigned_long:
    case var_type_float:
    case var_type_double:
    case var_type_long_double:
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long:
    case var_type_unsigned_long_long:
    #endif // IRSDEFS_LONG_LONG
    {
      m_type = a_variant_type;
    } break;
    case var_type_string: {
      if (m_type != var_type_string) {
        m_value.p_val_string_type = new string_type;
        m_type = a_variant_type;
      } else {
        m_value.p_val_string_type->clear();
      }
    } break;
    case var_type_array: {
      if (m_type != var_type_array) {
        m_value.p_val_vector_variant = new vector_variant_type;
        m_type = a_variant_type;
      } else {
        m_value.p_val_vector_variant->clear();
      }
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неучтенный тип");
    }
  }
}

void irs::variant::variant_t::type(const var_type_t a_variant_type)
{
  type_change(a_variant_type);
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const variant_t& a_variant)
{
  type_change(a_variant.m_type);
  switch(m_type) {
    case var_type_void_ptr: {
      m_value.p_void_type = a_variant.m_value.p_void_type;
    } break;
    case var_type_bool: {
      m_value.val_bool_type = a_variant.m_value.val_bool_type;
    } break;
    case var_type_char: {
      m_value.val_char_type = a_variant.m_value.val_char_type;
    } break;
    case var_type_singned_char: {
      m_value.val_schar_type = a_variant.m_value.val_schar_type;
    } break;
    case var_type_unsigned_char: {
      m_value.val_uchar_type = a_variant.m_value.val_uchar_type;
    } break;
    case var_type_short: {
      m_value.val_short_type = a_variant.m_value.val_short_type;
    } break;
    case var_type_unsigned_short: {
      m_value.val_ushort_type = a_variant.m_value.val_ushort_type;
    } break;
    case var_type_int: {
      m_value.val_int_type = a_variant.m_value.val_int_type;
    } break;
    case var_type_unsigned_int: {
      m_value.val_uint_type = a_variant.m_value.val_uint_type;
    } break;
    case var_type_long: {
      m_value.val_long_type = a_variant.m_value.val_long_type;
    } break;
    case var_type_unsigned_long: {
      m_value.val_ulong_type = a_variant.m_value.val_ulong_type;
    } break;
    case var_type_float: {
      m_value.val_float_type = a_variant.m_value.val_float_type;
    } break;
    case var_type_double: {
      m_value.val_double_type = a_variant.m_value.val_double_type;
    } break;
    case var_type_long_double: {
      m_value.val_long_double_type = a_variant.m_value.val_long_double_type;
    } break;
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long: {
      m_value.val_long_long_type = a_variant.m_value.val_long_long_type;
    } break;
    case var_type_unsigned_long_long: {
      m_value.val_ulong_long_type = a_variant.m_value.val_ulong_long_type;
    } break;
    #endif // IRSDEFS_LONG_LONG
    case var_type_string: {
      *(m_value.p_val_string_type) = *(a_variant.m_value.p_val_string_type);
    } break;
    case var_type_array: {
      *(m_value.p_val_vector_variant) =
        *(a_variant.m_value.p_val_vector_variant);
    } break;
    case var_type_unknown: {
      // Нет данных для копирования
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неизвестный тип");
    }
  }
  return *this;
}

/*irs::variant::variant_t& irs::variant::variant_t::operator=(void* ap_value)
{
  type_change(var_type_void_ptr);
  m_value.p_void_type = ap_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const void* ap_value)
{
  type_change(var_type_const_void_ptr);
  m_value.p_const_void_type = ap_value;
  return *this;
}*/

irs::variant::variant_t& irs::variant::variant_t::operator=(const bool a_value)
{
  type_change(var_type_bool);
  m_value.val_bool_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(const char a_value)
{
  type_change(var_type_char);
  m_value.val_char_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const signed char a_value)
{
  type_change(var_type_singned_char);
  m_value.val_schar_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const unsigned char a_value)
{
  type_change(var_type_unsigned_char);
  m_value.val_uchar_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const short a_value)
{
  type_change(var_type_short);
  m_value.val_short_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const unsigned short a_value)
{
  type_change(var_type_unsigned_short);
  m_value.val_ushort_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(const int a_value)
{
  type_change(var_type_int);
  m_value.val_int_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const unsigned int a_value)
{
  type_change(var_type_unsigned_int);
  m_value.val_uint_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(const long a_value)
{
  type_change(var_type_long);
  m_value.val_long_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const unsigned long a_value)
{
  type_change(var_type_unsigned_long);
  m_value.val_ulong_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(const float a_value)
{
  type_change(var_type_float);
  m_value.val_float_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const double a_value)
{
  type_change(var_type_double);
  m_value.val_double_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const long double a_value)
{
  type_change(var_type_long_double);
  m_value.val_long_double_type = a_value;
  return *this;
}

#ifdef IRSDEFS_LONG_LONG
irs::variant::variant_t& irs::variant::variant_t::operator=(
  const long_long_type a_value)
{
  type_change(var_type_long_long);
  m_value.val_long_long_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const unsigned_long_long_type a_value)
{
  type_change(var_type_unsigned_long_long);
  m_value.val_ulong_long_type = a_value;
  return *this;
}
#endif // IRSDEFS_LONG_LONG

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const string_type& a_value)
{
  type_change(var_type_string);
  *m_value.p_val_string_type = a_value;
  return *this;
}

void* irs::variant::variant_t::as_void_ptr() const
{
  return as_type<void*>();
}

const void* irs::variant::variant_t::as_const_void_ptr() const
{
  return as_type<const void*>();
}

bool irs::variant::variant_t::as_bool() const
{
  return as_type<bool>();
}

char irs::variant::variant_t::as_char() const
{
  return as_type<char>();
}

signed char irs::variant::variant_t::as_signed_char() const
{
  return as_type<signed char>();
}

unsigned char irs::variant::variant_t::as_unsigned_char() const
{
  return as_type<unsigned char>();
}

short irs::variant::variant_t::as_short() const
{
  return as_type<short>();
}

unsigned short irs::variant::variant_t::as_unsigned_short() const
{
  return as_type<unsigned short>();
}

int irs::variant::variant_t::as_int() const
{
  return as_type<int>();
}

unsigned int irs::variant::variant_t::as_unsigned_int() const
{
  return as_type<unsigned int>();
}

long irs::variant::variant_t::as_long() const
{
  return as_type<long>();
}

unsigned long irs::variant::variant_t::as_unsigned_long() const
{
  return as_type<unsigned long>();
}

float irs::variant::variant_t::as_float() const
{
  return as_type<float>();
}

double irs::variant::variant_t::as_double() const
{
  return as_type<double>();
}

long double irs::variant::variant_t::as_long_double() const
{
  return as_type<long double>();
}

#ifdef IRSDEFS_LONG_LONG
irs::variant::variant_t::long_long_type
irs::variant::variant_t::as_long_long() const
{
  return as_type<long_long_type>();
}

irs::variant::variant_t::unsigned_long_long_type
irs::variant::variant_t::as_unsigned_long_long() const
{
  return as_type<unsigned_long_long_type>();
}
#endif // IRSDEFS_LONG_LONG

irs::variant::variant_t::string_type
irs::variant::variant_t::as_string() const
{ 
  return as_type<string_type>();
}

irs::variant::variant_t::operator bool() const
{
  return as_type<bool>();
}

irs::variant::variant_t::operator char() const
{
  return as_type<char>();
}

irs::variant::variant_t::operator signed char() const
{
  return as_type<signed char>();
}

irs::variant::variant_t::operator unsigned char() const
{
  return as_type<unsigned char>();
}

irs::variant::variant_t::operator short() const
{
  return as_type<short>();
}

irs::variant::variant_t::operator unsigned short() const
{
  return as_type<unsigned short>();
}

irs::variant::variant_t::operator int() const
{
  return as_type<int>();
}

irs::variant::variant_t::operator unsigned int() const
{
  return as_type<unsigned int>();
}

irs::variant::variant_t::operator long() const
{
  return as_type<long>();
}

irs::variant::variant_t::operator unsigned long() const
{
  return as_type<unsigned long>();
}

irs::variant::variant_t::operator float() const
{
  return as_type<float>();
}

irs::variant::variant_t::operator double() const
{
  return as_type<double>();
}

irs::variant::variant_t::operator long double() const
{
  return as_type<long double>();
}

#ifdef IRSDEFS_LONG_LONG
irs::variant::variant_t::operator long_long_type() const
{
  return as_type<long_long_type>();
}

irs::variant::variant_t::operator unsigned_long_long_type() const
{
  return as_type<unsigned_long_long_type>();
}
#endif // IRSDEFS_LONG_LONG

irs::variant::variant_t::operator void*() const
{
  return as_type<void*>();
}

irs::variant::variant_t::operator const void*() const
{
  return as_type<const void*>();
}

/*irs::variant::variant_t::operator const char_type*() const
{
  return m_value.p_val_string_type->c_str();
}*/

irs::variant::variant_t::operator string_type() const
{
  return as_type<string_type>();
}

irs::variant::variant_t& irs::variant::variant_t::operator+=(
  const variant_t& a_variant)
{
  *this = operator+(*this, a_variant);
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator-=(
  const variant_t& a_variant)
{
  *this = operator-(*this, a_variant);
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator*=(
  const variant_t& a_variant)
{
  *this = operator*(*this, a_variant);
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator/=(
  const variant_t& a_variant)
{
  *this = operator/(*this, a_variant);
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator%=(
  const variant_t& a_variant)
{
  *this = operator%(*this, a_variant);
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator++()
{
  prefix_operation(operation_prefix_increment);
  return *this;
}

irs::variant::variant_t irs::variant::variant_t::operator++(int)
{
  variant_t current_variant = *this;
  prefix_operation(operation_prefix_increment);
  return current_variant;
}

irs::variant::variant_t& irs::variant::variant_t::operator--()
{
  prefix_operation(operation_prefix_decrement);
  return *this;
}

irs::variant::variant_t irs::variant::variant_t::operator--(int)
{
  variant_t current_variant = *this;
  prefix_operation(operation_prefix_decrement);
  return current_variant;
}

void irs::variant::binary_operation(
  const operation_type_t a_operation_type,
  const variant_t& a_first_variant,
  const variant_t& a_second_variant,
  variant_t* ap_result_variant,
  bool* ap_result_bool)
{
  typedef variant_t::size_type size_type;
  typedef variant_t::string_type string_type;
  typedef variant_t::vector_variant_type vector_variant_type;
  #ifdef IRSDEFS_LONG_LONG
  typedef variant_t::long_long_type long_long_type;
  typedef variant_t::unsigned_long_long_type unsigned_long_long_type;
  #endif // IRSDEFS_LONG_LONG

  typedef var_type_t var_type;
  var_type operation_vars_type = var_type_unknown;
  if ((a_first_variant.m_type <= var_type_int) &&
    (a_second_variant.m_type <= var_type_int))
  {
    operation_vars_type = var_type_int;
  } else if ((a_first_variant.m_type <= var_type_unsigned_int) &&
    (a_second_variant.m_type <= var_type_unsigned_int))
  {
    operation_vars_type = var_type_unsigned_int;
  } else if ((a_first_variant.m_type <= var_type_long) &&
    (a_second_variant.m_type <= var_type_long))
  {
    operation_vars_type = var_type_long;
  } else if ((a_first_variant.m_type <= var_type_unsigned_long) &&
    (a_second_variant.m_type <= var_type_unsigned_long))
  {
    operation_vars_type = var_type_unsigned_long;
  } else if ((a_first_variant.m_type <= var_type_float) &&
    (a_second_variant.m_type <= var_type_float))
  {
    operation_vars_type = var_type_float;
  } else if ((a_first_variant.m_type <= var_type_double) &&
    (a_second_variant.m_type <= var_type_double))
  {
    operation_vars_type = var_type_double;
  } else if ((a_first_variant.m_type <= var_type_long_double) &&
    (a_second_variant.m_type <= var_type_long_double))
  {
    operation_vars_type = var_type_long_double;
  } else {
    #ifdef IRSDEFS_LONG_LONG
    var_type first_var_type = var_type_unknown;
    var_type second_var_type = var_type_unknown;

    if ((a_first_variant.m_type <= var_type_unsigned_long) ||
      (a_first_variant.m_type == var_type_long_long))
    {
      first_var_type = var_type_long_long;
    } else if (a_first_variant.m_type == var_type_unsigned_long_long) {
      first_var_type = var_type_unsigned_long_long;
    } else {
      // Тип переменной не является стандартным
    }
    if ((a_second_variant.m_type <= var_type_unsigned_long) ||
      (a_second_variant.m_type == var_type_long_long))
    {
      second_var_type = var_type_long_long;
    } else if (a_first_variant.m_type == var_type_unsigned_long_long) {
      second_var_type = var_type_unsigned_long_long;
    } else {
      // Тип операции определить не удалось
    }
    if ((first_var_type != var_type_unknown) &&
      (second_var_type != var_type_unknown))
    {
      if ((first_var_type == var_type_long_long) &&
        (first_var_type == var_type_long_long)) {
        operation_vars_type = var_type_long_long;
      } else {
        operation_vars_type = var_type_unsigned_long_long;
      }
    } else {
      // Тип операции не определен
    }
    #endif // IRSDEFS_LONG_LONG
  }
  if (operation_vars_type == var_type_unknown) {
    if ((a_first_variant.m_type == var_type_string) &&
      (a_second_variant.m_type == var_type_string))
    {
      operation_vars_type = var_type_string;
    } else {
      // Тип операции опеределить не удалось
    }
  } else {
    // Тип операции уже определен
  }
  if (operation_vars_type == var_type_unknown) {
    if ((a_first_variant.m_type == var_type_array) &&
      (a_second_variant.m_type == var_type_array))
    {
      operation_vars_type = var_type_array;
    } else {
      // Тип операции опеределить не удалось
    }
  } else {
    // Тип операции уже определен
  }
  if (operation_vars_type == var_type_unknown) {
    if (((a_first_variant.m_type == var_type_void_ptr) ||
      (a_first_variant.m_type == var_type_const_void_ptr)) &&
      ((a_second_variant.m_type == var_type_void_ptr) ||
      (a_second_variant.m_type == var_type_const_void_ptr)))
    {
      operation_vars_type = var_type_const_void_ptr;
    } else {
      // Тип операции опеределить не удалось
    }
  } else {
    // Тип операции уже определен
  }

  if (operation_is_compare(a_operation_type)) {
    IRS_LIB_ASSERT(ap_result_variant == IRS_NULL);
    IRS_LIB_ASSERT(ap_result_bool != IRS_NULL);
  } else if (operation_is_arithmetic(a_operation_type)) {
    IRS_LIB_ASSERT(ap_result_variant != IRS_NULL);
    IRS_LIB_ASSERT(ap_result_bool == IRS_NULL);
    ap_result_variant->type_change(operation_vars_type);
  } else {
    IRS_LIB_ASSERT_MSG("Недопустимая операция");
  }
  
  switch (operation_vars_type) {
    case var_type_int: {
      int first_var = a_first_variant.value_get<int>();
      int second_var = a_second_variant.value_get<int>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_int_type),
        ap_result_bool);
    } break;
    case var_type_unsigned_int: {
      unsigned int first_var = a_first_variant.value_get<unsigned int>();
      unsigned int second_var = a_second_variant.value_get<unsigned int>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_uint_type),
        ap_result_bool);
    } break;
    case var_type_long: {
      long first_var = a_first_variant.value_get<long>();
      long second_var = a_second_variant.value_get<long>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_long_type),
        ap_result_bool);
    } break;
    case var_type_unsigned_long: {
      unsigned long first_var = a_first_variant.value_get<unsigned long>();
      unsigned long second_var = a_second_variant.value_get<unsigned long>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_ulong_type),
        ap_result_bool);
    } break;
    case var_type_float: {
      float first_var = a_first_variant.value_get<float>();
      float second_var = a_second_variant.value_get<float>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_float_type),
        ap_result_bool);
    } break;
    case var_type_double: {
      double first_var = a_first_variant.value_get<double>();
      double second_var = a_second_variant.value_get<double>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_double_type),
        ap_result_bool);
    } break;
    case var_type_long_double: {
      long double first_var = a_first_variant.value_get<long double>();
      long double second_var = a_second_variant.value_get<long double>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_long_double_type),
        ap_result_bool);
    } break;
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long: {
      long_long_type first_var = a_first_variant.value_get<long_long_type>();
      long_long_type second_var = a_second_variant.value_get<long_long_type>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_long_long_type),
        ap_result_bool);
    } break;
    case var_type_unsigned_long_long: {
      unsigned_long_long_type first_var =
        a_first_variant.value_get<unsigned_long_long_type>();
      unsigned_long_long_type second_var =
        a_second_variant.value_get<unsigned_long_long_type>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.val_ulong_long_type),
        ap_result_bool);
    } break;
    #endif // IRSDEFS_LONG_LONG
    case var_type_void_ptr: {
      void* first_var = a_first_variant.value_get<void*>();
      void* second_var = a_second_variant.value_get<void*>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : &(ap_result_variant->m_value.p_void_type),
        ap_result_bool);
    } break;
    case var_type_const_void_ptr: {
      const void* first_var = a_first_variant.value_get<const void*>();
      const void* second_var = a_second_variant.value_get<const void*>();
      const void* var_helper = IRS_NULL;
      operation_helper(a_operation_type,
        first_var,
        second_var,
        &var_helper,
        ap_result_bool);
    } break;
    case var_type_string: {
      string_type first_var =
        a_first_variant.value_get<string_type>();
      string_type second_var =
        a_second_variant.value_get<string_type>();
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : (ap_result_variant->m_value.p_val_string_type),
        ap_result_bool);
    } break;
    case var_type_array: {
      vector_variant_type first_var =
        a_first_variant.value_get<vector_variant_type>();
      vector_variant_type second_var =
        a_second_variant.value_get<vector_variant_type>();  
      operation_helper(a_operation_type,
        first_var,
        second_var,
        (ap_result_variant == IRS_NULL) ?
          IRS_NULL : (ap_result_variant->m_value.p_val_vector_variant),
        ap_result_bool);

    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Недопустимая комбинация переменных");
    }
  }
} 

void irs::variant::variant_t::prefix_operation(
  const operation_type_t a_unary_operation_type)
{
  switch(m_type) {
    case var_type_bool: {
      prefix_operation(a_unary_operation_type, &m_value.val_bool_type);
    } break;
    case var_type_char: {
      prefix_operation(a_unary_operation_type, &m_value.val_char_type);
    } break;
    case var_type_singned_char: {
      prefix_operation(a_unary_operation_type, &m_value.val_schar_type);
    } break;
    case var_type_unsigned_char: {
      prefix_operation(a_unary_operation_type, &m_value.val_uchar_type);
    } break;
    case var_type_short: {
      prefix_operation(a_unary_operation_type, &m_value.val_short_type);
    } break;
    case var_type_unsigned_short: {
      prefix_operation(a_unary_operation_type, &m_value.val_ushort_type);
    } break;
    case var_type_int: {
      prefix_operation(a_unary_operation_type, &m_value.val_int_type);
    } break;
    case var_type_unsigned_int: {
      prefix_operation(a_unary_operation_type, &m_value.val_uint_type);
    } break;
    case var_type_long: {
      prefix_operation(a_unary_operation_type, &m_value.val_long_type);
    } break;
    case var_type_unsigned_long: {
      prefix_operation(a_unary_operation_type, &m_value.val_ulong_type);
    } break;
    case var_type_float: {
      prefix_operation(a_unary_operation_type, &m_value.val_float_type);
    } break;
    case var_type_double: {
      prefix_operation(a_unary_operation_type, &m_value.val_double_type);
    } break;
    case var_type_long_double: {
      prefix_operation(a_unary_operation_type, &m_value.val_long_double_type);
    } break;
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long: {
      prefix_operation(a_unary_operation_type, &m_value.val_long_long_type);
    } break;
    case var_type_unsigned_long_long: {
      prefix_operation(a_unary_operation_type, &m_value.val_ulong_long_type);
    } break;
    #endif // IRSDEFS_LONG_LONG
    default : {
      IRS_LIB_ASSERT_MSG("Недопустимая операция для данного типа");
    }
  }
}

bool irs::variant::operator==(const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  bool result = false;
  binary_operation(operation_equal,
    a_first_variant, a_second_variant, IRS_NULL, &result);
  return result;
}

bool irs::variant::operator!=(const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  bool result = false;
  binary_operation(operation_not_equal,
    a_first_variant, a_second_variant, IRS_NULL, &result);
  return result;
}

bool irs::variant::operator<(const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  bool result = false;
  binary_operation(operation_less,
    a_first_variant, a_second_variant, IRS_NULL, &result);
  return result;
}

bool irs::variant::operator>(const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  bool result = false;
  binary_operation(operation_more,
    a_first_variant, a_second_variant, IRS_NULL, &result);
  return result;
}

bool irs::variant::operator<=(const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  bool result = false;
  binary_operation(operation_less_or_equal,
    a_first_variant, a_second_variant, IRS_NULL, &result);
  return result;
}

bool irs::variant::operator>=(const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  bool result = false;
  binary_operation(operation_more_or_equal,
    a_first_variant, a_second_variant, IRS_NULL, &result);
  return result;
}

irs::variant::variant_t irs::variant::operator+(
  const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  variant_t result;
  binary_operation(operation_sum,
    a_first_variant, a_second_variant, &result, IRS_NULL);
  return result;
}

irs::variant::variant_t irs::variant::operator-(
  const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  variant_t result;
  binary_operation(operation_difference,
    a_first_variant, a_second_variant, &result, IRS_NULL);
  return result;
}

irs::variant::variant_t irs::variant::operator*(
  const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  variant_t result;
  binary_operation(operation_multiplication,
    a_first_variant, a_second_variant, &result, IRS_NULL);
  return result;
}

irs::variant::variant_t irs::variant::operator/(
  const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  variant_t result;
  binary_operation(operation_division,
    a_first_variant, a_second_variant, &result, IRS_NULL);
  return result;
}

irs::variant::variant_t irs::variant::operator%(
  const variant_t& a_first_variant,
  const variant_t& a_second_variant)
{
  variant_t result;
  binary_operation(operation_integer_division,
    a_first_variant, a_second_variant, &result, IRS_NULL);
  return result;
}

bool irs::variant::variant_t::convert_to(const var_type_t a_var_type)
{
  bool convert_success = false;
  if (m_type != a_var_type) {
    if (is_type_number(m_type)) {
      switch(a_var_type) {
        case var_type_bool: {
          bool value = as_bool();
          type_change(a_var_type);
          m_value.val_bool_type = value;
          convert_success = true;
        } break;
        case var_type_char: {
          char value = as_char();
          type_change(a_var_type);
          m_value.val_char_type = value;
          convert_success = true;
        } break;
        case var_type_singned_char: {
          signed char value = as_signed_char();
          type_change(a_var_type);
          m_value.val_schar_type = value;
          convert_success = true;
        } break;
        case var_type_unsigned_char: {
          unsigned char value = as_unsigned_char();
          type_change(a_var_type);
          m_value.val_uchar_type = value;
          convert_success = true;
        } break;
        case var_type_short: {
          short value = as_short();
          type_change(a_var_type);
          m_value.val_short_type = value;
          convert_success = true;
        } break;
        case var_type_unsigned_short: {
          unsigned short value = as_unsigned_short();
          type_change(a_var_type);
          m_value.val_ushort_type = value;
          convert_success = true;
        } break;
        case var_type_int: {
          int value = as_int();
          type_change(a_var_type);
          m_value.val_int_type = value;
          convert_success = true;
        } break;
        case var_type_unsigned_int: {
          unsigned int value = as_unsigned_int();
          type_change(a_var_type);
          m_value.val_uint_type = value;
          convert_success = true;
        } break;
        case var_type_long: {
          long value = as_long();
          type_change(a_var_type);
          m_value.val_long_type = value;
          convert_success = true;
        } break;
        case var_type_unsigned_long: {
          unsigned long value = as_unsigned_long();
          type_change(a_var_type);
          m_value.val_ulong_type = value;
          convert_success = true;
        } break;
        case var_type_float: {
          float value = as_float();
          type_change(a_var_type);
          m_value.val_float_type = value;
          convert_success = true;
        } break;
        case var_type_double: {
          double value = as_double();
          type_change(a_var_type);
          m_value.val_double_type = value;
          convert_success = true;
        } break;
        case var_type_long_double: {
          long double value = as_long_double();
          type_change(a_var_type);
          m_value.val_long_double_type = value;
          convert_success = true;
        } break;
        #ifdef IRSDEFS_LONG_LONG
        case var_type_long_long: {
          long_long_type value = as_long_long();
          type_change(a_var_type);
          m_value.val_long_long_type = value;
          convert_success = true;
        } break;
        case var_type_unsigned_long_long: {
          unsigned_long_long_type value = as_unsigned_long_long();
          type_change(a_var_type);
          m_value.val_ulong_long_type = value;
          convert_success = true;
        } break;
        #endif // IRSDEFS_LONG_LONG
        case var_type_string: {
          string_type value = as_string();
          type_change(a_var_type);
          *m_value.p_val_string_type = value;
          convert_success = true;
        } break;
        default : {
          // Ошибка при конвертировании
        }
      }
    } else if (m_type == var_type_string) {
      switch(a_var_type) {
        case var_type_bool: {
          bool value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_bool_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_char: {
          char value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_char_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_singned_char: {
          signed char value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_schar_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_unsigned_char: {
          unsigned char value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_uchar_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_short: {
          short value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_short_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_unsigned_short: {
          unsigned short value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_ushort_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_int: {
          int value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_int_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_unsigned_int: {
          unsigned int value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_uint_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_long: {
          long value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_long_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_unsigned_long: {
          unsigned long value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_ulong_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_float: {
          float value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_float_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_double: {
          double value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_double_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_long_double: {
          long double value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_long_double_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        #ifdef IRSDEFS_LONG_LONG
        case var_type_long_long: {
          long_long_type value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_long_long_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        case var_type_unsigned_long_long: {
          unsigned_long_long_type value;
          if (m_value.p_val_string_type->to_number(value)) {
            type_change(a_var_type);
            m_value.val_ulong_long_type = value;
            convert_success = true;
          } else {
            // Преобразовать строку в число не удалось
          }
        } break;
        #endif // IRSDEFS_LONG_LONG
        default : {
          // Ошибка при конвертировании
        }
      }
    } else if (m_type == var_type_unknown) {
      // Этот тип преобразовать невозможно
    } else {
      IRS_LIB_ASSERT_MSG("Неучтенный тип");
    }
  } else {
    // Операция конвертирования не требуется
    convert_success = true;
  }
  return convert_success;
}

bool irs::variant::variant_t::is_type_number(var_type_t a_var_type)
{
  bool is_type_num = false;
  switch(a_var_type) {
    case var_type_bool:
    case var_type_char:
    case var_type_singned_char:
    case var_type_unsigned_char:
    case var_type_short:
    case var_type_unsigned_short:
    case var_type_int:
    case var_type_unsigned_int:
    case var_type_long:
    case var_type_unsigned_long:
    case var_type_float:
    case var_type_double:
    case var_type_long_double:
    #ifdef IRSDEFS_LONG_LONG
    case var_type_long_long:
    case var_type_unsigned_long_long:
    #endif // IRSDEFS_LONG_LONG
    {
      is_type_num = true;
    } break;
    default : {
      // Тип не является числовым типом
    }
  }
  return is_type_num;
}

// class var_type_converter_t
irs::variant::var_type_converter_t::var_type_converter_t():
  m_table_var_type_and_str(),
  m_table_str_and_var_type()
{
  m_table_var_type_and_str.insert(
    make_pair(var_type_unknown, irst("var_type_unknown")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_unknown"), var_type_unknown));

  m_table_var_type_and_str.insert(
    make_pair(var_type_bool, irst("var_type_bool")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_bool"), var_type_bool));

  m_table_var_type_and_str.insert(
    make_pair(var_type_char, irst("var_type_char")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_char"), var_type_char));

  m_table_var_type_and_str.insert(
    make_pair(var_type_singned_char, irst("var_type_singned_char")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_singned_char"), var_type_singned_char));

  m_table_var_type_and_str.insert(
    make_pair(var_type_unsigned_char, irst("var_type_unsigned_char")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_unsigned_char"), var_type_unsigned_char));

  m_table_var_type_and_str.insert(
    make_pair(var_type_int, irst("var_type_int")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_unsigned_char"), var_type_unsigned_char));

  m_table_var_type_and_str.insert(
    make_pair(var_type_unsigned_int, irst("var_type_unsigned_int")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_unsigned_char"), var_type_unsigned_char));

  m_table_var_type_and_str.insert(
    make_pair(var_type_long, irst("var_type_long")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_unsigned_char"), var_type_unsigned_char));

  m_table_var_type_and_str.insert(
    make_pair(var_type_unsigned_long, irst("var_type_unsigned_long")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_unsigned_char"), var_type_unsigned_char));

  m_table_var_type_and_str.insert(
    make_pair(var_type_float, irst("var_type_float")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_float"), var_type_float));

  m_table_var_type_and_str.insert(
    make_pair(var_type_double, irst("var_type_double")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_double"), var_type_double));

  m_table_var_type_and_str.insert(
    make_pair(var_type_long_double, irst("var_type_long_double")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_long_double"), var_type_long_double));

  #ifdef IRSDEFS_LONG_LONG
  m_table_var_type_and_str.insert(
    make_pair(var_type_long_long, irst("var_type_long_long")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_long_long"), var_type_long_long));

  m_table_var_type_and_str.insert(
    make_pair(var_type_unsigned_long_long,
      irst("var_type_unsigned_long_long")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_unsigned_long_long"),
      var_type_unsigned_long_long));
  #endif // IRSDEFS_LONG_LONG
  m_table_var_type_and_str.insert(
    make_pair(var_type_void_ptr, irst("var_type_void_ptr")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_void_ptr"), var_type_void_ptr));

  m_table_var_type_and_str.insert(
    make_pair(var_type_const_void_ptr, irst("var_type_const_void_ptr")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_const_void_ptr"), var_type_const_void_ptr));

  m_table_var_type_and_str.insert(
    make_pair(var_type_string, irst("var_type_string")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_string"), var_type_string));

  m_table_var_type_and_str.insert(
    make_pair(var_type_array, irst("var_type_array")));
  m_table_str_and_var_type.insert(
    make_pair(irst("var_type_array"), var_type_array));
}

void irs::variant::var_type_converter_t::to_str(
  const var_type_t a_var_type, string_type* ap_var_type_str)
{
  table_var_type_and_str_const_iterator var_type_it =
    m_table_var_type_and_str.find(a_var_type);
  if (var_type_it != m_table_var_type_and_str.end()) {
    *ap_var_type_str = var_type_it->second;
  } else {
    IRS_LIB_ASSERT_MSG("Неучтенный тип");
  }
}

bool irs::variant::var_type_converter_t::to_type(
  const string_type& a_var_type_str, var_type_t* ap_var_type)
{
  bool convert_success = true;
  table_str_and_var_type_const_iterator var_type_str_it =
    m_table_str_and_var_type.find(a_var_type_str);
  if (var_type_str_it != m_table_str_and_var_type.end()) {
    *ap_var_type = var_type_str_it->second;
  } else {
    convert_success = false;
  }
  return convert_success;
}

#ifdef __BORLANDC__

/*void irs::variant::variant_to_tree_view(
  const irs::variant::variant_t& a_variant,
  TTreeView* ap_tree_view,
  TTreeNode* ap_tree_node)
{
  typedef irs::variant::sizens_t size_type;
  typedef irs::variant::stringns_t string_type;
  var_type_converter_t var_type_converter;
  string_type var_type_str;
  var_type_converter.to_str(a_variant.type(), &var_type_str);
  TTreeNode* p_new_node =
    ap_tree_view->Items->AddChildFirst(ap_tree_node, var_type_str.c_str());
  if (a_variant.type() == irs::variant::var_type_array) {
    const size_type array_size = a_variant.size();
    for (size_type elem_i = 0; elem_i < array_size; elem_i++) {
      variant_to_tree_view(a_variant[elem_i], ap_tree_view, p_new_node);
    }
  } else {
    irs::variant::variant_t var = a_variant;
    if (var.convert_to(irs::variant::var_type_string)) {
      ap_tree_view->Items->AddChildFirst(p_new_node, var.as_string().c_str());
    } else {
      // Невозможно преобразовать в строку
    }
  }
}*/

#endif // __BORLANDC__

#ifdef IRS_LIB_DEBUG
void irs::variant::test_variant()
{
  /*#pragma pack(push, 1)
  struct test_struct_t{
    char ch;
    short s;
    int i;
    double d;
    long double ld;
  } test_struct;
  #pragma pack(pop)

  int test_struct_size = sizeof(test_struct);*/
  typedef variant_t::size_type size_type;
  // Поддерживаемые типы
  char var_char_type = 'q';
  signed char var_signed_char_type = 'w';
  unsigned char var_unsigned_char_type = 'e';
  //bool var_bool_type = false;
  short var_short_type = 10;
  unsigned short var_unsigned_short_type = 11;
  int var_int_type = 12;
  unsigned int var_unsigned_int_type = 13;
  long var_long_type = 14;
  unsigned long var_unsigned_long_type = 15;
  float var_float_type = 0.16f;
  double var_double_type = 0.17;
  long double var_long_double_type = 0.18;
  #ifdef IRSDEFS_LONG_LONG
  variant_t::long_long_type var_long_long_type = 19;
  variant_t::unsigned_long_long_type var_unsigned_long_long_type = 20;
  #endif // IRSDEFS_LONG_LONG
  void* var_void_ptr = IRS_NULL;
  variant_t::string_type var_string_type = "test variant";

  // Тест операторов записи и чтения
  variant_t variant = var_char_type;
  char var_char_type_result_read = variant;
  IRS_LIB_ASSERT(var_char_type == var_char_type_result_read);
  variant = var_signed_char_type;
  signed char var_signed_char_type_result_read = variant;
  IRS_LIB_ASSERT(var_signed_char_type == var_signed_char_type_result_read);
  variant = var_unsigned_char_type;
  unsigned char var_unsigned_char_type_result_read = variant;
  IRS_LIB_ASSERT(var_unsigned_char_type == var_unsigned_char_type_result_read);
  variant = var_short_type;
  short var_short_type_result_read = variant;
  IRS_LIB_ASSERT(var_short_type == var_short_type_result_read);
  variant = var_unsigned_short_type;
  unsigned short var_unsigned_short_type_result_read = variant;
  IRS_LIB_ASSERT(var_unsigned_short_type ==
    var_unsigned_short_type_result_read);
  variant = var_int_type;
  int var_int_type_result_read = variant;
  IRS_LIB_ASSERT(var_int_type == var_int_type_result_read);
  variant = var_unsigned_int_type;
  unsigned int var_unsigned_int_type_result_read = variant;
  IRS_LIB_ASSERT(var_unsigned_int_type == var_unsigned_int_type_result_read);
  variant = var_long_type;
  long var_long_type_result_read = variant;
  IRS_LIB_ASSERT(var_long_type == var_long_type_result_read);
  variant = var_unsigned_long_type;
  unsigned long var_unsigned_long_type_result_read = variant;
  IRS_LIB_ASSERT(var_unsigned_long_type == var_unsigned_long_type_result_read);
  variant = var_float_type;
  float var_float_type_result_read = variant;
  IRS_LIB_ASSERT(var_float_type == var_float_type_result_read);
  variant = var_double_type;
  double var_double_type_result_read = variant;
  IRS_LIB_ASSERT(var_double_type == var_double_type_result_read);
  variant = var_long_double_type;
  long double var_long_double_type_result_read = variant;
  IRS_LIB_ASSERT(var_long_double_type == var_long_double_type_result_read);
  #ifdef IRSDEFS_LONG_LONG
  variant = var_long_long_type;
  variant_t::long_long_type var_long_long_type_result_read = variant;
  IRS_LIB_ASSERT(var_long_long_type == var_long_long_type_result_read);
  variant = var_unsigned_long_long_type;
  variant_t::unsigned_long_long_type var_unsigned_long_long_type_result_read =
    variant;
  IRS_LIB_ASSERT(var_unsigned_long_long_type ==
    var_unsigned_long_long_type_result_read);
  #endif // IRSDEFS_LONG_LONG
  variant = var_void_ptr;  
  variant = var_string_type;
  variant_t::string_type var_string_type_result_read = variant;
  
  IRS_LIB_ASSERT(var_string_type == var_string_type_result_read);

  // Тест операторов сравнения
  int first_var = 23;
  double double_var = 12.097;
  variant_t first_variant = first_var;
  variant_t second_variant = double_var;
  first_variant == second_variant;
  IRS_LIB_ASSERT(!(first_variant == second_variant));
  IRS_LIB_ASSERT(first_variant != second_variant);
  IRS_LIB_ASSERT(!(first_variant < second_variant));
  IRS_LIB_ASSERT(first_variant > second_variant);
  IRS_LIB_ASSERT(!(first_variant <= second_variant));
  IRS_LIB_ASSERT(first_variant >= second_variant);

  IRS_LIB_ASSERT(!(double_var == first_variant));
  IRS_LIB_ASSERT(first_variant != double_var);
  IRS_LIB_ASSERT(!(first_variant < double_var));
  IRS_LIB_ASSERT(first_variant > double_var);
  IRS_LIB_ASSERT(!(first_variant <= double_var));
  IRS_LIB_ASSERT(first_variant >= double_var);

  const int var_helper = 0;
  variant_t void_ptr_variant = &var_helper;
  /*
  IRS_LIB_ASSERT(!(void_ptr_var == void_ptr_variant));
  IRS_LIB_ASSERT(void_ptr_variant != void_ptr_var);
  IRS_LIB_ASSERT(!(void_ptr_variant < void_ptr_var));
  IRS_LIB_ASSERT(void_ptr_variant > void_ptr_var);
  IRS_LIB_ASSERT(!(void_ptr_variant <= void_ptr_var));
  IRS_LIB_ASSERT(void_ptr_variant >= void_ptr_var);
  */
  // Тест унарных и бинарных арифметических операторов
  int first_int_var = 232;
  int second_int_var = 342;
  first_variant = first_int_var;
  second_variant = second_int_var;
  IRS_LIB_ASSERT((first_variant += second_variant) ==
    (first_int_var += second_int_var));
  IRS_LIB_ASSERT((first_variant -= second_variant) ==
    (first_int_var -= second_int_var));
  IRS_LIB_ASSERT((first_variant *= second_variant) ==
    (first_int_var *= second_int_var));
  IRS_LIB_ASSERT((first_variant /= second_variant) ==
    (first_int_var /= second_int_var));
  IRS_LIB_ASSERT((first_variant %= second_variant) ==
    (first_int_var %= second_int_var)); 

  IRS_LIB_ASSERT((first_variant + second_variant) ==
    (first_int_var + second_int_var));
  IRS_LIB_ASSERT((first_variant - second_variant) ==
    (first_int_var - second_int_var));
  IRS_LIB_ASSERT((first_variant * second_variant) ==
    (first_int_var * second_int_var));
  IRS_LIB_ASSERT((first_variant / second_variant) ==
    (first_int_var / second_int_var));
  IRS_LIB_ASSERT((first_variant % second_variant) ==
    (first_int_var % second_int_var));
 
  IRS_LIB_ASSERT(++first_variant == ++first_int_var);
  IRS_LIB_ASSERT(first_variant++ == first_int_var++);
  IRS_LIB_ASSERT(--first_variant == --first_int_var);
  IRS_LIB_ASSERT(first_variant-- == first_int_var--);

  // Проверка на совместимость с типом string
  variant_t::string_type s1 = "test ";
  variant_t::string_type s2 = "variant";
  variant_t::string_type sum_s1_and_s2 = s1 + s2;
  first_variant = s1;
  second_variant = s2;
  first_variant += second_variant;
  IRS_LIB_ASSERT(first_variant == sum_s1_and_s2);
  //IRS_LIB_ASSERT(static_cast<variant_t::string_type>(first_variant) ==
    //sum_s1_and_s2);

  // Проверака функции assign_no_cast
  first_variant = first_int_var;
  first_variant.assign_no_cast(double_var);
  IRS_LIB_ASSERT(static_cast<int>(first_variant) ==
    static_cast<int>(double_var));
  variant_t::string_type string_var = "test";
  first_variant = string_var;
  first_variant.assign_no_cast(string_var);    

  // Проверка оператора копирования
  first_variant.type(var_type_array);
  second_variant.type(var_type_array);
  first_variant = second_variant;

  // Проверка variant_t в качестве массива
  first_variant.type(var_type_array);
  const size_type new_size = 10;
  first_variant.resize(new_size);
  //size_type cur_size = first_variant.size();
  const size_type elem_index = 1;
  second_variant = first_variant[elem_index];

  // Проверка функций присвоения и преобразования
  first_variant.type(var_type_int);
  first_variant.assign_no_cast(10);
  first_variant.convert_to(var_type_string);
  string s = first_variant;
  first_variant = s;
  first_int_var = first_variant;
  first_variant.convert_to(var_type_int);
  double_var = first_variant;
}
#endif // IRS_LIB_DEBUG

#endif //__WATCOMC__
