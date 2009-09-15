#include <irsvariant.h>

irs::variant::variant_t::variant_t():
  m_value(),
  m_type(var_type_int)
{
}

irs::variant::variant_t::variant_t(const variant_t& a_variant):
  m_value(),
  m_type(var_type_int)
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
    case var_type_long_long:
    case var_type_unsigned_long_long: {
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
    #ifdef IRSDEFS_I64
    case var_type_long_long: {
      m_value.val_long_long_type = a_variant.m_value.val_long_long_type;
    } break;
    case var_type_unsigned_long_long: {
      m_value.val_ulong_long_type = a_variant.m_value.val_ulong_long_type;
    } break;
    #endif // IRSDEFS_I64
    case var_type_string: {
      *(m_value.p_val_string_type) = *(a_variant.m_value.p_val_string_type);
    } break;
    case var_type_array: {
      *(m_value.p_val_vector_variant) =
        *(a_variant.m_value.p_val_vector_variant);
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неизвестный тип");
    }
  }
  return *this;
}

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

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const long_long_type a_value)
{
  type_change(var_type_long_double);
  m_value.val_long_double_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const unsigned_long_long_type a_value)
{
  type_change(var_type_long_double);
  m_value.val_long_double_type = a_value;
  return *this;
}

irs::variant::variant_t& irs::variant::variant_t::operator=(
  const string_type& a_value)
{
  type_change(var_type_string);
  *m_value.p_val_string_type = a_value;
  return *this;
}

char irs::variant::variant_t::is_char() const
{
  char value;
  value_get(&value);
  return value;
}


signed char irs::variant::variant_t::is_signed_char() const
{
  signed char value;
  value_get(&value);
  return value;
}

unsigned char irs::variant::variant_t::is_unsigned_char() const
{
  unsigned char value;
  value_get(&value);
  return value;
}

short irs::variant::variant_t::is_short() const
{
  short value;
  value_get(&value);
  return value;
}

unsigned short irs::variant::variant_t::is_unsigned_short() const
{
  unsigned short value;
  value_get(&value);
  return value;
}

int irs::variant::variant_t::is_int() const
{
  int value;
  value_get(&value);
  return value;
}

unsigned int irs::variant::variant_t::is_unsigned_int() const
{
  unsigned int value;
  value_get(&value);
  return value;
}

long irs::variant::variant_t::is_long() const
{
  long value;
  value_get(&value);
  return value;
}

unsigned long irs::variant::variant_t::is_unsigned_long() const
{
  unsigned long value;
  value_get(&value);
  return value;
}

float irs::variant::variant_t::is_float() const
{
  float value;
  value_get(&value);
  return value;
}

double irs::variant::variant_t::is_double() const
{
  double value;
  value_get(&value);
  return value;
}

long double irs::variant::variant_t::is_long_double() const
{
  long double value;
  value_get(&value);
  return value;
}

#ifdef IRSDEFS_I64
irs::variant::variant_t::long_long_type
irs::variant::variant_t::is_long_long() const
{
  long_long_type value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::unsigned_long_long_type
irs::variant::variant_t::is_unsigned_long_long() const
{
  unsigned_long_long_type value;
  value_get(&value);
  return value;
}
#endif // IRSDEFS_I64

irs::variant::variant_t::string_type
irs::variant::variant_t::is_string() const
{
  string value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator char() const
{
  char value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator signed char() const
{
  signed char value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator unsigned char() const
{
  unsigned char value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator short() const
{
  short value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator unsigned short() const
{
  unsigned short value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator int() const
{
  int value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator unsigned int() const
{
  unsigned int value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator long() const
{
  long value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator unsigned long() const
{
  unsigned long value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator float() const
{
  float value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator double() const
{
  double value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator long double() const
{
  long double value;
  value_get(&value);
  return value;
}

#ifdef IRSDEFS_I64
irs::variant::variant_t::operator long_long_type() const
{
  long_long_type value;
  value_get(&value);
  return value;
}

irs::variant::variant_t::operator unsigned_long_long_type() const
{
  unsigned_long_long_type value;
  value_get(&value);
  return value;
}
#endif // IRSDEFS_I64

/*irs::variant::variant_t::operator const char_type*() const
{
  return m_value.p_val_string_type->c_str();
}*/

irs::variant::variant_t::operator const string_type() const
{
  string_type value;
  value_get(&value);
  return value;
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
  #ifdef IRSDEFS_I64
  typedef variant_t::long_long_type long_long_type;
  typedef variant_t::unsigned_long_long_type unsigned_long_long_type;
  #endif // IRSDEFS_I64

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
    #ifdef IRSDEFS_I64
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
    #endif // IRSDEFS_I64
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
    #ifdef IRSDEFS_I64
    case var_type_long_long: {
      prefix_operation(a_unary_operation_type, &m_value.val_long_long_type);
    } break;
    case var_type_unsigned_long_long: {
      prefix_operation(a_unary_operation_type, &m_value.val_ulong_long_type);
    } break;
    #endif // IRSDEFS_I64
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

#ifdef IRS_LIB_DEBUG
void irs::variant::test_variant()
{
  typedef variant_t::size_type size_type;
  // Поддерживаемые типы
  char var_char_type = 'q';
  signed char var_signed_char_type = 'w';
  unsigned char var_unsigned_char_type = 'e';
  bool var_bool_type = false;
  short var_short_type = 10;
  unsigned short var_unsigned_short_type = 11;
  int var_int_type = 12;
  unsigned int var_unsigned_int_type = 13;
  long var_long_type = 14;
  unsigned long var_unsigned_long_type = 15;
  float var_float_type = 0.16;
  double var_double_type = 0.17;
  long double var_long_double_type = 0.18;
  variant_t::long_long_type var_long_long_type = 19;
  variant_t::unsigned_long_long_type var_unsigned_long_long_type = 20;
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
  variant = var_long_long_type;
  variant_t::long_long_type var_long_long_type_result_read = variant;
  IRS_LIB_ASSERT(var_long_long_type == var_long_long_type_result_read);
  variant = var_unsigned_long_long_type;
  variant_t::unsigned_long_long_type var_unsigned_long_long_type_result_read =
    variant;
  IRS_LIB_ASSERT(var_unsigned_long_long_type ==
    var_unsigned_long_long_type_result_read);
  variant = var_string_type;
  variant_t::string_type var_string_type_result_read = variant;
  
  IRS_LIB_ASSERT(var_string_type == var_string_type_result_read);

  // Тест операторов сравнения
  int first_var = 23;
  double second_double_var = 12.097;
  variant_t first_variant = first_var;
  variant_t second_variant = second_double_var;
  first_variant == second_variant;
  IRS_LIB_ASSERT(!(first_variant == second_variant));
  IRS_LIB_ASSERT(first_variant != second_variant);
  IRS_LIB_ASSERT(!(first_variant < second_variant));
  IRS_LIB_ASSERT(first_variant > second_variant);
  IRS_LIB_ASSERT(!(first_variant <= second_variant));
  IRS_LIB_ASSERT(first_variant >= second_variant);

  IRS_LIB_ASSERT(!(second_double_var == first_variant));
  IRS_LIB_ASSERT(first_variant != second_double_var);
  IRS_LIB_ASSERT(!(first_variant < second_double_var));
  IRS_LIB_ASSERT(first_variant > second_double_var);
  IRS_LIB_ASSERT(!(first_variant <= second_double_var));
  IRS_LIB_ASSERT(first_variant >= second_double_var);

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
  first_variant.assign_no_cast(second_double_var);
  IRS_LIB_ASSERT(static_cast<int>(first_variant) ==
    static_cast<int>(second_double_var));
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
  size_type cur_size = first_variant.size();
  const size_type elem_index = 1;
  second_variant = first_variant[elem_index];
}
#endif // IRS_LIB_DEBUG

