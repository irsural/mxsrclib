//! \file
//! \ingroup math_group
//! \brief Калькулятор
//!
//! Дата: 03.10.2011\n
//! Ранняя дата: 24.09.2009

// В Watcom C++ есть только не-const версия функции map::find
#ifndef __WATCOMC__

// В GNU C++ версии >= 3.4 строки char* имеют кодировку UTF-8
// Русские символы в этой кодировке 'Я' имеют размер 2 байта, что недопустимо
// по стандарту C++
//#if defined(IRS_UNICODE) || defined(IRS_GNUC_VERSION_LESS_3_4)

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irscalc.h>

#include <irsfinal.h>

/*
irs::string irs::preprocessing_str(const irs::string& a_str)
{
  irs::string str = a_str;
  // Удаляем переход на новую строку
  int pos_ch_return = str.find("\r");
  while (pos_ch_return != irs::string::npos) {
    str.erase(pos_ch_return, 2);
    pos_ch_return = str.find("\r");
  }
  // Удаляем табуляции
  int pos_ch_tab = str.find("\t");
  while (pos_ch_tab != irs::string::npos) {
    str.erase(pos_ch_tab, 2);
    pos_ch_tab = str.find("\t");
  }
  return str;
}
*/

void irs::calc::create_keyword_cpp_map(keyword_map_type* ap_keyword_map)
{
  ap_keyword_map->clear();
  (*ap_keyword_map)[irst("and")] = keyword_and;
  (*ap_keyword_map)[irst("and_eq")] = keyword_and_eq;
  (*ap_keyword_map)[irst("asm")] = keyword_asm;
  (*ap_keyword_map)[irst("auto")] = keyword_auto;
  (*ap_keyword_map)[irst("bitand")] = keyword_bitand;
  (*ap_keyword_map)[irst("bitor")] = keyword_bitor;
  (*ap_keyword_map)[irst("bool")] = keyword_bool;
  (*ap_keyword_map)[irst("break")] = keyword_break;
  (*ap_keyword_map)[irst("case")] = keyword_case;
  (*ap_keyword_map)[irst("catch")] = keyword_catch;
  (*ap_keyword_map)[irst("char")] = keyword_char;
  (*ap_keyword_map)[irst("class")] = keyword_class;
  (*ap_keyword_map)[irst("compl")] = keyword_compl;
  (*ap_keyword_map)[irst("const")] = keyword_const;
  (*ap_keyword_map)[irst("const_cast")] = keyword_const_cast;
  (*ap_keyword_map)[irst("continue")] = keyword_continue;
  (*ap_keyword_map)[irst("default")] = keyword_default;
  (*ap_keyword_map)[irst("delete")] = keyword_delete;
  (*ap_keyword_map)[irst("do")] = keyword_do;
  (*ap_keyword_map)[irst("double")] = keyword_double;
  (*ap_keyword_map)[irst("dynamic_cast")] = keyword_dynamic_cast;
  (*ap_keyword_map)[irst("else")] = keyword_else;
  (*ap_keyword_map)[irst("enum")] = keyword_enum;
  (*ap_keyword_map)[irst("explicit")] = keyword_explicit;
  (*ap_keyword_map)[irst("export")] = keyword_export;
  (*ap_keyword_map)[irst("extern")] = keyword_extern;
  (*ap_keyword_map)[irst("false")] = keyword_false;
  (*ap_keyword_map)[irst("float")] = keyword_float;
  (*ap_keyword_map)[irst("for")] = keyword_for;
  (*ap_keyword_map)[irst("friend")] = keyword_friend;
  (*ap_keyword_map)[irst("goto")] = keyword_goto;
  (*ap_keyword_map)[irst("if")] = keyword_if;
  (*ap_keyword_map)[irst("inline")] = keyword_inline;
  (*ap_keyword_map)[irst("int")] = keyword_int;
  (*ap_keyword_map)[irst("long")] = keyword_long;
  (*ap_keyword_map)[irst("mutable")] = keyword_mutable;
  (*ap_keyword_map)[irst("namespace")] = keyword_namespace;
  (*ap_keyword_map)[irst("new")] = keyword_new;
  (*ap_keyword_map)[irst("not")] = keyword_not;
  (*ap_keyword_map)[irst("not_eq")] = keyword_not_eq;
  (*ap_keyword_map)[irst("operator")] = keyword_operator;
  (*ap_keyword_map)[irst("or")] = keyword_or;
  (*ap_keyword_map)[irst("or_eq")] = keyword_or_eq;
  (*ap_keyword_map)[irst("private")] = keyword_private;
  (*ap_keyword_map)[irst("protected")] = keyword_protected;
  (*ap_keyword_map)[irst("public")] = keyword_public;
  (*ap_keyword_map)[irst("register")] = keyword_register;
  (*ap_keyword_map)[irst("reinterpret_cast")] = keyword_reinterpret_cast;
  (*ap_keyword_map)[irst("return")] = keyword_return;
  (*ap_keyword_map)[irst("short")] = keyword_short;
  (*ap_keyword_map)[irst("signed")] = keyword_signed;
  (*ap_keyword_map)[irst("sizeof")] = keyword_sizeof;
  (*ap_keyword_map)[irst("static")] = keyword_static;
  (*ap_keyword_map)[irst("static_cast")] = keyword_static_cast;
  (*ap_keyword_map)[irst("struct")] = keyword_struct;
  (*ap_keyword_map)[irst("switch")] = keyword_switch;
  (*ap_keyword_map)[irst("template")] = keyword_template;
  (*ap_keyword_map)[irst("this")] = keyword_this;
  (*ap_keyword_map)[irst("throw")] = keyword_throw;
  (*ap_keyword_map)[irst("true")] = keyword_true;
  (*ap_keyword_map)[irst("try")] = keyword_try;
  (*ap_keyword_map)[irst("typedef")] = keyword_typedef;
  (*ap_keyword_map)[irst("typeid")] = keyword_typeid;
  (*ap_keyword_map)[irst("typename")] = keyword_typename;
  (*ap_keyword_map)[irst("union")] = keyword_union;
  (*ap_keyword_map)[irst("unsigned")] = keyword_unsigned;
  (*ap_keyword_map)[irst("using")] = keyword_using;
  (*ap_keyword_map)[irst("virtual")] = keyword_virtual;
  (*ap_keyword_map)[irst("void")] = keyword_void;
  (*ap_keyword_map)[irst("volatile")] = keyword_volatile;
  (*ap_keyword_map)[irst("wchar_t")] = keyword_wchar_t;
  (*ap_keyword_map)[irst("while")] = keyword_while;
  (*ap_keyword_map)[irst("xor")] = keyword_xor;
  (*ap_keyword_map)[irst("xor_eq")] = keyword_xor_eq;
}

// class func_r_int_a_int_t
bool irs::calc::func_r_int_a_int_t::exec(
  calc_variables_t* ap_calc_variables,
  vector<mutable_ref_t>* ap_parameters,
  variant_t* ap_returned_value) const
{
  bool fsuccess = false;
  if (ap_parameters->size() == 1) {
    variant_t value_variant;
    if (ap_calc_variables->value_read((*ap_parameters)[0], &value_variant)) {
      if (value_variant.convert_to(irs::variant::var_type_int)) {
        *ap_returned_value = (*mp_func)(value_variant.as_int());
        fsuccess = true;
      } else {
        // Недопустимый тип аргумента
      }
    } else {
      // Прочитать аргумент не удалось
    }
  } else {
    // Недопустимое количество параметров
  }
  return fsuccess;
}

// class func_r_double_a_double_t
bool irs::calc::func_r_double_a_double_t::exec(
  calc_variables_t* ap_calc_variables,
  vector<mutable_ref_t>* ap_parameters,
  variant_t* ap_returned_value) const
{
  bool fsuccess = false;
  if (ap_parameters->size() == 1) {
    variant_t value_variant;
    if (ap_calc_variables->value_read((*ap_parameters)[0], &value_variant)) {
      if (value_variant.convert_to(irs::variant::var_type_double)) {
        *ap_returned_value = (*mp_func)(value_variant.as_double());
        fsuccess = true;
      } else {
        // Недопустимый тип аргумента
      }
    } else {
      // Прочитать аргумент не удалось
    }
  } else {
    // Недопустимое количество параметров
  }
  return fsuccess;
}

// class func_r_string_a_string_t
bool irs::calc::func_r_string_a_string_t::exec(
  calc_variables_t* ap_calc_variables,
  vector<mutable_ref_t>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = false;
  if (ap_parameters->size() == 1) {
    variant_type value_variant;
    if (ap_calc_variables->value_read((*ap_parameters)[0], &value_variant)) {
      if (value_variant.convert_to(irs::variant::var_type_string)) {
        *ap_returned_value = (*mp_func)(value_variant.as_string());
        fsuccess = true;
      } else {
        // Недопустимый тип аргумента
      }
    } else {
      // Прочитать аргумент не удалось
    }
  } else {
    // Недопустимое количество параметров
  }
  return fsuccess;
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
// class func_r_str_a_str_loc_t
bool irs::calc::func_r_str_a_str_loc_t::exec(
  calc_variables_t* ap_calc_variables,
  vector<mutable_ref_t>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = false;
  if (ap_parameters->size() == 1) {
    variant_type value_variant;
    if (ap_calc_variables->value_read((*ap_parameters)[0], &value_variant)) {
      if (value_variant.convert_to(irs::variant::var_type_string)) {
        *ap_returned_value = (*mp_func)(value_variant.as_string(),
          get_locale());
        fsuccess = true;
      } else {
        // Недопустимый тип аргумента
      }
    } else {
      // Прочитать аргумент не удалось
    }
  } else {
    // Недопустимое количество параметров
  }
  return fsuccess;
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

// class func_r_dbl_a_dbl_dbl_t
bool irs::calc::func_r_dbl_a_dbl_dbl_t::exec(
  calc_variables_t* ap_calc_variables,
  vector<mutable_ref_t>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() == 2) {
    variant_type first_arg_variant;
    variant_type second_arg_variant;
    if (ap_calc_variables->value_read((*ap_parameters)[0],
      &first_arg_variant)) {
      if (!first_arg_variant.convert_to(irs::variant::var_type_double)) {
        // Недопустимый тип аргумента
        fsuccess = false;
      }
    } else {
      // Прочитать аргумент не удалось
      fsuccess = false;
    }
    if (ap_calc_variables->value_read((*ap_parameters)[1],
      &second_arg_variant)) {
      if (!second_arg_variant.convert_to(irs::variant::var_type_double)) {
        // Недопустимый тип аргумента
        fsuccess = false;
      }
    } else {
      fsuccess = false;
    }
    if (fsuccess) {
      *ap_returned_value = (*mp_func)(first_arg_variant.as_double(),
        second_arg_variant.as_double());
    }
  } else {
    // Недопустимое количество параметров
    fsuccess = false;
  }
  return fsuccess;
}

namespace {

class assert_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool assert_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 1) {
    fsuccess = false;
  }
  variant_type arg_variant;
  if (fsuccess) {
    if (ap_calc_variables->value_read((*ap_parameters)[0], &arg_variant)) {
      if (!arg_variant.convert_to(irs::variant::var_type_bool)) {
        // Недопустимый тип аргумента
        fsuccess = false;
      }
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (!arg_variant.as_bool()) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    *ap_returned_value = true;
  } else {
    *ap_returned_value = false;
  }
  return fsuccess;
}


class pow_t: public irs::calc::function_t
{
public:
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool pow_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() == 2) {
    variant_type first_arg_variant;
    variant_type second_arg_variant;
    if (ap_calc_variables->value_read((*ap_parameters)[0], &first_arg_variant))
    {
      if (!first_arg_variant.convert_to(irs::variant::var_type_double)) {
        // Недопустимый тип аргумента
        fsuccess = false;
      } else {
        // Первый аргумент успешно прочитан
      }
    } else {
      // Прочитать аргумент не удалось
      fsuccess = false;
    }
    if (ap_calc_variables->value_read((*ap_parameters)[1], &second_arg_variant))
    {
      if (!second_arg_variant.convert_to(irs::variant::var_type_double)) {
        // Недопустимый тип аргумента
        fsuccess = false;
      } else {
        // Второй аргумент успешно прочитан
      }
    } else {
      // Прочитать аргумент не удалось
      fsuccess = false;
    }
    if (fsuccess) {
      *ap_returned_value = pow(first_arg_variant.as_double(),
        second_arg_variant.as_double());
    } else {
      // Произошла ошибка
    }
  } else {
    // Недопустимое количество параметров
    fsuccess = false;
  }
  return fsuccess;
}

class number_to_string_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool number_to_string_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = false;
  if (ap_parameters->size() == 2) {
    variant_type value_variant;
    if (ap_calc_variables->value_read((*ap_parameters)[0], &value_variant)) {
      if (irs::variant::is_number_type(value_variant.type())) {
        if (value_variant.convert_to(irs::variant::var_type_string)) {
          if (ap_calc_variables->value_write(
            value_variant, &(*ap_parameters)[1])) {
            *ap_returned_value = true;
            fsuccess = true;
          } else {
            *ap_returned_value = false;
          }
        } else {
          // Преобразование окончилось неудачей
        }
      } else {
        // Недопустимый тип аргумента
      }
    } else {
      // Прочитать аргумент не удалось
    }
  } else {
    // Недопустимое количество параметров
  }
  return fsuccess;
}

class string_to_number_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool string_to_number_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 2) {
    fsuccess = false;
  } else {
    // Количество аргументов имеет правильное значение
  }
  variant_type first_arg_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &first_arg_variant))
    {
      fsuccess = false;
    } else {
      // Значение успешно прочитано
    }
  } else {
    // Произошла ошибка
  }
  if (fsuccess) {
    if (first_arg_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    } else {
      // Прочитанное значение имеет строковый тип
    }
  } else {
    // Произошла ошибка
  }
  variant_type number_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[1], &number_variant)) {
      fsuccess = false;
    } else {
      // Значение второго аргумента успешно прочитано
    }
  } else {
    // Произошла ошибка
  }
  if (fsuccess) {
    if (!irs::variant::is_number_type(number_variant.type())) {
      fsuccess = false;
    } else {
      // Второй аргумент имеет числовой тип
    }
  } else {
    // Произошла ошибка
  }
  if (fsuccess) {
    if (!first_arg_variant.convert_to(number_variant.type())) {
      fsuccess = false;
    } else {
      // Первый аргумент успешно сконвертирован в числовой тип
    }
  } else {
    // Произошла ошибка
  }
  if (fsuccess) {
    if (!ap_calc_variables->value_write(
      first_arg_variant, &(*ap_parameters)[1]))
    {
      fsuccess = false;
    } else {
      // Значение второго аргумента успешно перезаписано числом, полученным из
      // строки
    }
  } else {
    // Произошла ошибка
  }
  if (fsuccess) {
    *ap_returned_value = true;
  } else {
    *ap_returned_value = false;
  }
  return fsuccess;
}

/*//! \brief Возвращает размер строки
irs::calc::sizens_t get_str_size(irs::calc::stringns_t a_str)
{
  return a_str.size();
}

//! \brief Возвращает подстроку строки a_str
irs::calc::size_type get_sub_str(irs::calc::stringns_t a_str,
  irs::calc::size_type a_pos, irs::calc::size_type a_count)
{
  return a_str.substr(a_pos, a_count);
}*/

//! \brief Возвращает строку с удаленными пробельными символами слева и справа
irs::calc::stringns_t trim(irs::calc::stringns_t a_str)
{
  irs::calc::stringns_t str = a_str;
  const irs::calc::stringns_t white_space(irst(" \n\r\t"));
  irs::calc::stringns_t::size_type pos = str.find_last_not_of(white_space);
  str.erase(pos + 1);
  pos = str.find_first_not_of(white_space);
  str.erase(0, pos);
  return str;
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
//! \brief Возвращает строку с удаленными пробельными символами
irs::calc::stringns_t remove_space(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  irs::calc::stringns_t str = a_str;
  std::size_t i = a_str.size();
  while (i > 0) {
    i--;
    if (irs::isspacet(str[i], a_loc)) {
      str.erase(i, 1);
    }
  }
  return str;
}

//! \brief Возвращает строку с удаленными буквами
irs::calc::stringns_t remove_alpha(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  irs::calc::stringns_t str = a_str;
  std::size_t i = a_str.size();
  while (i > 0) {
    i--;
    if (irs::isalphat(str[i], a_loc)) {
      str.erase(i, 1);
    }
  }
  return str;
}

//! \brief Возвращает строку с удаленными цифрами
irs::calc::stringns_t remove_digit(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  irs::calc::stringns_t str = a_str;
  std::size_t i = a_str.size();
  while (i > 0) {
    i--;
    if (irs::isdigitt(str[i], a_loc)) {
      str.erase(i, 1);
    }
  }
  return str;
}

//! \brief Возвращает строку с удаленными символами, не являющимися числами
irs::calc::stringns_t remove_not_digit(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  irs::calc::stringns_t str = a_str;
  std::size_t i = a_str.size();
  while (i > 0) {
    i--;
    if (!irs::isdigitt(str[i], a_loc)) {
      str.erase(i, 1);
    }
  }
  return str;
}

//! \brief Возвращает строку с удаленными символами, не являющимися буквами
irs::calc::stringns_t remove_not_alpha(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  irs::calc::stringns_t str = a_str;
  std::size_t i = a_str.size();
  while (i > 0) {
    i--;
    if (!irs::isalphat(str[i], a_loc)) {
      str.erase(i, 1);
    }
  }
  return str;
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
//! \brief Возвращает строку, все символы которой в нижнем регистре
irs::calc::stringns_t to_lower(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  return irs::to_lower(a_str, a_loc);
}

//! \brief Возвращает строку, все символы которой в верхнем регистре
irs::calc::stringns_t to_upper(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  return irs::to_upper(a_str, a_loc);
}

irs::calc::stringns_t to_upper_first_symbol(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  irs::calc::stringns_t str = a_str;
  if (!str.empty()) {
    str[0] = toupper(str[0], a_loc);
  }
  return str;
}

irs::calc::stringns_t to_lower_first_symbol(irs::calc::stringns_t a_str,
  locale a_loc = irs::loc().get())
{
  irs::calc::stringns_t str = a_str;
  if (!str.empty()) {
    str[0] = tolower(str[0], a_loc);
  }
  return str;
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

irs::calc::stringns_t addition_dot_if_not_exists(irs::calc::stringns_t a_str)
{
  irs::calc::stringns_t str = a_str;
  if (!str.empty()) {
    if (str[str.size() - 1] != irst('.')) {
      str += irst(".");
    }
  } else {
    str += irst(".");
  }
  return str;
}

class str_size_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool str_size_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 1) {
    fsuccess = false;
  }
  variant_type arg_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &arg_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (arg_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = arg_variant.as_string();
    *ap_returned_value = str.size();
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

class str_resize_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool str_resize_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 2) {
    fsuccess = false;
  }
  variant_type first_arg_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0],
      &first_arg_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (first_arg_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  variant_type number_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[1], &number_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (!irs::variant::is_number_type(number_variant.type())) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = first_arg_variant.as_string();
    str.resize(number_variant.as_unsigned_int());
    variant_type result_variant = str;
    if (!ap_calc_variables->value_write(result_variant, &(*ap_parameters)[0])) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    *ap_returned_value = true;
  } else {
    *ap_returned_value = false;
  }
  return fsuccess;
}

class str_symbol_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool str_symbol_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 2) {
    fsuccess = false;
  } else {
    // Количество аргументов имеет правильное значение
  }
  variant_type str_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &str_variant))
    {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (str_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  variant_type pos_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[1], &pos_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (!irs::variant::is_number_type(pos_variant.type())) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = str_variant.as_string();
    *ap_returned_value = str.substr(pos_variant.as_unsigned_int(), 1);
  }
  return fsuccess;
}

class sub_str_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool sub_str_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 3) {
    fsuccess = false;
  } else {
    // Количество аргументов имеет правильное значение
  }
  variant_type str_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &str_variant))
    {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (str_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  variant_type pos_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[1], &pos_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (!irs::variant::is_number_type(pos_variant.type())) {
      fsuccess = false;
    }
  }
  variant_type count_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[2], &count_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (!irs::variant::is_number_type(count_variant.type())) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = str_variant.as_string();
    *ap_returned_value = str.substr(pos_variant.as_unsigned_int(),
      count_variant.as_unsigned_int());
  }
  return fsuccess;
}

/*class trim_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool trim_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 1) {
    fsuccess = false;
  }
  variant_type arg_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &arg_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (arg_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = arg_variant.as_string();
    *ap_returned_value = trim(str);
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

class remove_space_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool remove_space_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 1) {
    fsuccess = false;
  }
  variant_type arg_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &arg_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (arg_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = arg_variant.as_string();
    *ap_returned_value = remove_space(str);
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

class to_upper_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool to_upper_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 1) {
    fsuccess = false;
  }
  variant_type arg_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &arg_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (arg_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = arg_variant.as_string();
    *ap_returned_value = irs::to_upper(str);
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

class to_lower_t: public irs::calc::function_t
{
public:
  typedef irs::calc::stringns_t string_type;
  typedef irs::variant::variant_t variant_type;
  typedef irs::calc::calc_variables_t calc_variables_type;
  typedef irs::calc::mutable_ref_t mutable_ref_type;
  virtual bool exec(
    calc_variables_type* ap_calc_variables,
    vector<mutable_ref_type>* ap_parameters,
    variant_type* ap_returned_value) const;
};

bool to_lower_t::exec(
  calc_variables_type* ap_calc_variables,
  vector<mutable_ref_type>* ap_parameters,
  variant_type* ap_returned_value) const
{
  bool fsuccess = true;
  if (ap_parameters->size() != 1) {
    fsuccess = false;
  }
  variant_type arg_variant;
  if (fsuccess) {
    if (!ap_calc_variables->value_read((*ap_parameters)[0], &arg_variant)) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (arg_variant.type() != irs::variant::var_type_string) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    string_type str = arg_variant.as_string();
    *ap_returned_value = irs::to_lower(str);
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}*/

double min_d(double a, double b)
{
  return min(a, b);
}

double max_d(double a, double b)
{
  return max(a, b);
}

} // empty namespace

bool irs::calc::part_id_name_get(
  const part_id_variable_const_iterator a_begin_part_id_it,
  const part_id_variable_const_iterator a_end_part_id_it,
  stringns_t* ap_name)
{
  IRS_LIB_ASSERT(ap_name != IRS_NULL);
  bool fsuccess = false;
  const int single_part_id = 1;
  if (distance(a_begin_part_id_it, a_end_part_id_it) >= single_part_id) {
    if (a_begin_part_id_it->type == part_id_type_name) {
      if (a_begin_part_id_it->part_id.type() == irs::variant::var_type_string) {
        *ap_name = a_begin_part_id_it->part_id.as_string();
        fsuccess = true;
      } else {
        IRS_LIB_ASSERT_MSG("Имя имеет неправильный тип");
      }
    } else {
      // Текущая часть идентификатора не является именем
    }
  } else {
    // Нет частей идентификатора для обработки
  }
  return fsuccess;
}

bool irs::calc::part_id_index_get(
  const part_id_variable_const_iterator a_begin_part_id_it,
  const part_id_variable_const_iterator a_end_part_id_it,
  irs::variant::variant_t* ap_index)
{
  IRS_LIB_ASSERT(ap_index != IRS_NULL);
  bool fsuccess = false;
  const ptrdiff_t single_part_id = 1;
  if (distance(a_begin_part_id_it, a_end_part_id_it) >= single_part_id) {
    if (a_begin_part_id_it->type == part_id_type_index) {
      irs::variant::variant_t index_variant = a_begin_part_id_it->part_id;
      if (index_variant.convert_to(ap_index->type())) {
        ap_index->assign_no_cast(a_begin_part_id_it->part_id);
        fsuccess = true;
      } else {
        IRS_LIB_ASSERT_MSG("Индекс имеет неправильный тип");
      }
    } else {
      // Текущая часть идентификатора не является именем
    }
  } else {
    // Нет частей идентификатора для обработки
  }
  return fsuccess;
}

// class calc_variables_t
irs::calc::calc_variables_t::calc_variables_t(
  calculator_t* const ap_calculator
):
  mp_calculator(ap_calculator)
{
  IRS_LIB_ASSERT(mp_calculator);
}

bool irs::calc::calc_variables_t::value_read(
  const mutable_ref_t& a_mutable_ref_src,
  variant_t* ap_value_dest)
{
  IRS_LIB_ASSERT(mp_calculator);
  return mp_calculator->value_read(a_mutable_ref_src, ap_value_dest);
}

bool irs::calc::calc_variables_t::value_write(
  variant_t& a_variant_src,
  mutable_ref_t* ap_mutable_ref_dest)
{
  IRS_LIB_ASSERT(mp_calculator);
  return mp_calculator->value_write(a_variant_src, ap_mutable_ref_dest);
}

// class calculator_t
irs::calc::calculator_t::calculator_t():
  m_list_identifier(),
  m_detector_token(m_list_identifier),
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  m_locale(irs::loc().get()),
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  mp_handle_extern_variable(IRS_NULL)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  m_detector_token.imbue(m_locale);
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  func_r_double_a_double_t::func_r_double_a_double_ptr_type
    func_r_dbl_a_dbl_ptr = IRS_NULL;
  func_r_dbl_a_dbl_ptr = acos;
  function_add(irst("acos"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = asin;
  function_add(irst("asin"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = atan;
  function_add(irst("atan"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = ceil;
  function_add(irst("ceil"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = cos;
  function_add(irst("cos"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = cosh;
  function_add(irst("cosh"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = exp;
  function_add(irst("exp"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = fabs;
  function_add(irst("fabs"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = floor;
  function_add(irst("floor"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = log;
  function_add(irst("log"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = log10;
  function_add(irst("log10"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = sin;
  function_add(irst("sin"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = sinh;
  function_add(irst("sinh"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = sqrt;
  function_add(irst("sqrt"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = tan;
  function_add(irst("tan"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = tanh;
  function_add(irst("tanh"),
    new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));

  func_r_string_a_string_t::func_r_string_a_string_ptr_type
    func_r_string_a_string_ptr = IRS_NULL;
  func_r_string_a_string_ptr = trim;
  function_add(irst("trim"),
    new func_r_string_a_string_t(func_r_string_a_string_ptr));
  /*func_r_string_a_string_ptr = ::to_upper;
  function_add(irst("to_upper"),
    new func_r_string_a_string_t(func_r_string_a_string_ptr));
  func_r_string_a_string_ptr = ::to_lower;
  function_add(irst("to_lower"),
    new func_r_string_a_string_t(func_r_string_a_string_ptr));*/
  func_r_string_a_string_ptr = addition_dot_if_not_exists;
  function_add(irst("addition_dot_if_not_exists"),
    new func_r_string_a_string_t(func_r_string_a_string_ptr));
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  func_r_str_a_str_loc_t::func_r_str_a_str_loc_ptr_type
    func_r_str_a_str_loc_ptr = IRS_NULL;
  func_r_str_a_str_loc_ptr = ::to_upper;
  function_add(irst("to_upper"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  func_r_str_a_str_loc_ptr = ::to_lower;
  function_add(irst("to_lower"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  func_r_str_a_str_loc_ptr = remove_space;
  function_add(irst("remove_space"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  func_r_str_a_str_loc_ptr = remove_alpha;
  function_add(irst("remove_alpha"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  func_r_str_a_str_loc_ptr = remove_digit;
  function_add(irst("remove_digit"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  func_r_str_a_str_loc_ptr = remove_not_digit;
  function_add(irst("remove_not_digit"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  func_r_str_a_str_loc_ptr = to_upper_first_symbol;
  function_add(irst("to_upper_first_symbol"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  func_r_str_a_str_loc_ptr = to_lower_first_symbol;
  function_add(irst("to_lower_first_symbol"),
    new func_r_str_a_str_loc_t(func_r_str_a_str_loc_ptr));
  #endif // IRS_FULL_STDCPPLIB_SUPPORT

  func_r_dbl_a_dbl_dbl_t::func_r_dbl_a_dbl_ptr_type
    func_r_dbl_a_dbl_dbl_ptr = IRS_NULL;
  func_r_dbl_a_dbl_dbl_ptr = min_d;
  function_add(irst("min"),
    new func_r_dbl_a_dbl_dbl_t(func_r_dbl_a_dbl_dbl_ptr));
  func_r_dbl_a_dbl_dbl_ptr = max_d;
  function_add(irst("max"),
    new func_r_dbl_a_dbl_dbl_t(func_r_dbl_a_dbl_dbl_ptr));

  function_add(irst("assert"), new assert_t());
  function_add(irst("pow"), new pow_t());
  function_add(irst("number_to_string"), new number_to_string_t());
  function_add(irst("string_to_number"), new string_to_number_t());

  function_add(irst("get_str_size"), new str_size_t());
  function_add(irst("set_str_size"), new str_resize_t());
  function_add(irst("get_str_symbol"), new str_symbol_t());
  function_add(irst("get_sub_str"), new sub_str_t());
  //function_add(irst("trim"), new trim_t());
  //function_add(irst("remove_space"), new remove_space_t());
  //function_add(irst("to_upper"), new to_upper_t());
  //function_add(irst("to_lower"), new to_lower_t());

  variable_add(irst("IRS_E"), IRS_E, v_is_constant);
  variable_add(irst("IRS_LOG2E"), IRS_LOG2E, v_is_constant);
  variable_add(irst("IRS_LOG10E"), IRS_LOG10E, v_is_constant);
  variable_add(irst("IRS_LN2"), IRS_LN2, v_is_constant);
  variable_add(irst("IRS_LN10"), IRS_LN10, v_is_constant);
  variable_add(irst("IRS_PI"), IRS_PI, v_is_constant);
  variable_add(irst("IRS_PI_2"), IRS_PI_2, v_is_constant);
  variable_add(irst("IRS_PI_4"), IRS_PI_4, v_is_constant);
  variable_add(irst("IRS_1_PI"), IRS_1_PI, v_is_constant);
  variable_add(irst("IRS_2_PI"), IRS_2_PI, v_is_constant);
  variable_add(irst("IRS_1_SQRTPI"), IRS_1_SQRTPI, v_is_constant);
  variable_add(irst("IRS_2_SQRTPI"), IRS_2_SQRTPI, v_is_constant);
  variable_add(irst("IRS_SQRT2"), IRS_SQRT2, v_is_constant);
  variable_add(irst("IRS_SQRT_2"), IRS_SQRT_2, v_is_constant);
  variable_add(irst("var10"), 0);

  value_type arr;
  arr.type(variant::var_type_array);
  arr.resize(5);
  for (int i = 0; i < 5; i++) {
    arr[i].type(variant::var_type_array);
    arr[i].resize(5);
  }
  int cnt = 0;
  for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 5; y++) {
      arr[x][y].type(variant::var_type_int);
      arr[x][y] = cnt*100;
      cnt++;
    }
  }
  variable_add(irst("arr"), arr);
}

bool irs::calc::calculator_t::atom(mutable_ref_t* ap_value)
{
  bool fsuccess = true;
  token_t token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    if (token.token_type() == tt_identifier) {
      function_t* p_function = IRS_NULL;
      if (m_list_identifier.function_find(token.get_identifier(), &p_function))
      {
        fsuccess = m_detector_token.next_token();
        vector<mutable_ref_t> arguments_func;
        if (fsuccess) {
          fsuccess = eval_exp_arg_function(&arguments_func);
        } else {
          // Произошла ошибка
        }
        variant_t result_value_variant;
        if (fsuccess) {
          calc_variables_t calc_variables(this);
          #ifdef IRS_FULL_STDCPPLIB_SUPPORT
          p_function->imbue(m_locale);
          #endif // IRS_FULL_STDCPPLIB_SUPPORT
          fsuccess = p_function->exec(&calc_variables, &arguments_func,
            &result_value_variant);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          ap_value->type(mutable_ref_t::type_value);
          ap_value->value(result_value_variant);
        } else {
          // Произошла ошибка
        }
      } else {
        id_variable_type id_variable;
        fsuccess = eval_exp_variable(&id_variable);
        if (fsuccess) {
          //fsuccess = variable_value_get(constant, ap_value);
          ap_value->type(mutable_ref_t::type_id);
          ap_value->id(id_variable);
        } else {
          // Произошла ошибка
        }
      }
    /*
    } else if (token.token_type() == tt_function) {
      const function_t* const p_func = token.get_function();
      fsuccess = m_detector_token.next_token();
      vector<variant_t> arguments_func;
      if (fsuccess) {
        fsuccess = eval_exp_arg_function(&arguments_func);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        fsuccess = p_func->exec(&arguments_func, ap_value);
      } else {
        // Произошла ошибка
      }
    } else if (token.token_type() == tt_constant) {
      const value_type* p_constant = token.get_constant();
      fsuccess = m_detector_token.next_token();
      while (fsuccess && (p_constant->type() == variant::var_type_array)) {
        value_type elem_index;
        if (fsuccess) {
          fsuccess = eval_exp_square_brackets(&elem_index);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          if (elem_index < p_constant->size()) {
            p_constant = &(*p_constant)[elem_index];
          } else {
            fsuccess = false;
          }
        } else {
          // Произошла ошибка
        }
      }
      if (fsuccess) {
        *ap_value = *p_constant;
      } else {
        // Произошла ошибка
      }
    */
    } else if ((token.token_type() == tt_number) ||
      (token.token_type() == tt_string))
    {
      variant_t value_variant;
      if (token.token_type() == tt_number) {
        value_variant = token.get_number();
      } else {
        value_variant = token.get_string();
      }
      ap_value->type(mutable_ref_t::type_value);
      ap_value->value(value_variant);
      fsuccess = m_detector_token.next_token();
    } else if (token.token_type() == tt_delimiter) {
      if (token.delimiter() == d_plus) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = atom(ap_value);
        } else {
          // Произошла ошибка
        }
      } else if (token.delimiter() == d_minus) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = atom(ap_value);
          variant_t value_variant;
          if (fsuccess) {
            fsuccess = value_read(*ap_value, &value_variant);
          } else {
            // Произошла ошибка
          }
          if (fsuccess) {
            value_variant *= -1;
            ap_value->type(mutable_ref_t::type_value);
            ap_value->value(value_variant);
          } else{
            // Произошла ошибка
          }
        } else {
          // Произошла ошибка
        }
      } else {
        fsuccess = false;
      }
    } else {
      fsuccess = false;
    }
  }
  return fsuccess;
}

bool irs::calc::calculator_t::local_variable_value(
  const mode_io_value_t a_mode_io_value,
  const id_variable_type& id_variable,
  variant_t* ap_value)
{
  IRS_LIB_ASSERT(ap_value != IRS_NULL);
  bool fsuccess = true;
  const size_type part_count = id_variable.size();
  variant_t* p_value_variant = IRS_NULL;
  list_identifier_t::variable_t* p_variable = IRS_NULL;
  if (part_count > 0) {
    //variant_t value_helper;
    variant_t index_variant;
    IRS_LIB_ASSERT(id_variable.front().type == part_id_type_name);
    if (m_list_identifier.variable_find(id_variable.front().part_id,
      &p_variable))
    {
      p_value_variant = &p_variable->value;
      for (size_type part_i = 1; part_i < part_count; part_i++) {
        if (id_variable[part_i].type == part_id_type_index) {
          if (p_value_variant->type() == irs::variant::var_type_array) {
            index_variant = id_variable[part_i].part_id;
            if (irs::variant::is_number_type(index_variant.type())) {
              p_value_variant =
                &(*p_value_variant)[index_variant.as_type<size_type>()];
              //value = value_helper;
            } else {
              // Недопустимый тип индекса
              fsuccess = false;
              break;
            }
          } else {
            // Недопустимое количество индексов
            fsuccess = false;
            break;
          }
        } else {
          // Часть идентификатора должна быть индексом
          fsuccess = false;
          break;
        }
      }
    } else {
      fsuccess = false;
    }
  } else {
    // Пустой идентификатор
    fsuccess = false;
  }
  if (fsuccess) {
    switch (a_mode_io_value) {
      case mode_io_value_read: {
        *ap_value = *p_value_variant;
      } break;
      case mode_io_value_write: {
        if (p_variable->variability == v_is_variable) {
          *p_value_variant = *ap_value;
        } else {
          // Переменная является константой
          fsuccess = false;
        }
      } break;
      default : {
        IRS_LIB_ASSERT_MSG("Недопустимый режим");
      }
    }
  } else {
    // Получить значение не удалось
  }
  return fsuccess;
}

bool irs::calc::calculator_t::extern_variable_value(
  const mode_io_value_t a_mode_io_value,
  const id_variable_type& id_variable,
  variant_t* ap_value
)
{
  IRS_LIB_ASSERT(ap_value != IRS_NULL);
  bool fsuccess = false;
  if (mp_handle_extern_variable) {
    switch (a_mode_io_value) {
      case mode_io_value_read: {
        fsuccess = mp_handle_extern_variable->get(id_variable, ap_value);
      } break;
      case mode_io_value_write: {
        fsuccess = mp_handle_extern_variable->set(id_variable, *ap_value);
      } break;
      default : {
        IRS_LIB_ASSERT_MSG("Недопустимый режим");
      }
    }
  } else {
    // Список на внешние переменные не установлен
  }
  return fsuccess;
}

bool irs::calc::calculator_t::assign_variable(
  const mutable_ref_t& a_right_value_mref,
  mutable_ref_t* ap_left_value_mref)
{
  bool assign_success = false;
  if (ap_left_value_mref->type() == mutable_ref_t::type_id) {
    assign_success = value_write(a_right_value_mref, ap_left_value_mref);
  } else {
    // Левое значение должно быть ссылкой на переменную
  }
  return assign_success;
}

bool irs::calc::calculator_t::value_read(
  const mutable_ref_t& a_mutable_ref_src,
  variant_t* ap_value_dest)
{
  IRS_LIB_ASSERT(ap_value_dest != IRS_NULL);
  bool value_read_success = false;
  const mutable_ref_t::type_t type = a_mutable_ref_src.type();
  switch (type) {
    case mutable_ref_t::type_value: {
      *ap_value_dest = a_mutable_ref_src.value();
      value_read_success = true;
    } break;
    case mutable_ref_t::type_id: {
      const id_variable_type id_variable = a_mutable_ref_src.id();
      variant_t value_src_variant;
      if (local_variable_value(
        mode_io_value_read, id_variable, ap_value_dest))
      {
        //*ap_value_dest = value_src_variant;
        value_read_success = true;
      } else if (extern_variable_value(mode_io_value_read, id_variable,
        ap_value_dest)) {
        value_read_success = true;
      } else {
        // Переменная не найдена
      }
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неизвестный тип");
    }
  }
  return value_read_success;
}

bool irs::calc::calculator_t::value_write(variant_t& a_variant_src,
  mutable_ref_t* ap_mutable_ref_dest)
{
  IRS_LIB_ASSERT(ap_mutable_ref_dest != IRS_NULL);
  bool value_write_success = false;
  const mutable_ref_t::type_t mutable_ref_dest_type =
    ap_mutable_ref_dest->type();
  switch (mutable_ref_dest_type) {
    case mutable_ref_t::type_value: {
      ap_mutable_ref_dest->value(a_variant_src);
      value_write_success = true;
    } break;
    case mutable_ref_t::type_id: {
      const id_variable_type id_variable_dest = ap_mutable_ref_dest->id();
      if (local_variable_value(
        mode_io_value_write, id_variable_dest, &a_variant_src))
      {
        value_write_success = true;
      } else if (extern_variable_value(mode_io_value_write, id_variable_dest,
        &a_variant_src)) {
        value_write_success = true;
      } else {
        // Записать значение не удалось
      }
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неизвестный тип");
    }
  }
  return value_write_success;
}

bool irs::calc::calculator_t::value_write(
  const mutable_ref_t& a_mutable_ref_src,
  mutable_ref_t* ap_mutable_ref_dest)
{
  IRS_LIB_ASSERT(ap_mutable_ref_dest != IRS_NULL);
  bool value_write_success = false;
  variant_t value_variant_src;
  if (value_read(a_mutable_ref_src, &value_variant_src)) {
    value_write_success = value_write(value_variant_src,
      ap_mutable_ref_dest);
  } else {
    // Прочить значение не удалось
  }
  return value_write_success;
}


#ifndef NOP
// class mutable_ref_t
irs::calc::mutable_ref_t::mutable_ref_t(type_t a_type):
  m_value(),
  m_id(),
  m_type(a_type)
{
}

irs::calc::mutable_ref_t::mutable_ref_t(const variant_t& a_value, value_tag):
  m_value(a_value),
  m_id(),
  m_type(type_value)
{
}

irs::calc::mutable_ref_t::mutable_ref_t(
  const id_variable_type& a_id_variable, id_variable_tag
):
  m_value(),
  m_id(a_id_variable),
  m_type(type_id)
{
}

irs::calc::mutable_ref_t::mutable_ref_t(const mutable_ref_t& a_mutable_ref):
  m_type(type_unknown)
{
  m_type = a_mutable_ref.m_type;
  m_value = a_mutable_ref.m_value;
  m_id = a_mutable_ref.m_id;
}

irs::calc::mutable_ref_t::variant_t& irs::calc::mutable_ref_t::value()
{
  return m_value;
}

const irs::calc::mutable_ref_t::variant_t&
irs::calc::mutable_ref_t::value() const
{
  return m_value;
}

void irs::calc::mutable_ref_t::value(const variant_t& a_value)
{
  m_type = type_value;
  m_value = a_value;
  m_id.clear();
}

irs::calc::id_variable_type& irs::calc::mutable_ref_t::id()
{
  return m_id;
}

const irs::calc::id_variable_type& irs::calc::mutable_ref_t::id() const
{
  return m_id;
}

void irs::calc::mutable_ref_t::id(const id_variable_type& a_id)
{
  m_type = type_id;
  m_id = a_id;
}

irs::calc::mutable_ref_t::type_t irs::calc::mutable_ref_t::type() const
{
  return m_type;
}

void irs::calc::mutable_ref_t::type(const type_t a_type)
{
  change_type(a_type);
}

void irs::calc::mutable_ref_t::change_type(const type_t a_type)
{
  m_type = a_type;
  m_value.type(irs::variant::var_type_unknown);
  m_id.clear();
}

void irs::calc::mutable_ref_t::swap(mutable_ref_t& a_mutable_ref)
{
  IRS_STD swap(m_type, a_mutable_ref.m_type);
  IRS_STD swap(m_value, a_mutable_ref.m_value);
  IRS_STD swap(m_id, a_mutable_ref.m_id);
}

irs::calc::mutable_ref_t&
irs::calc::mutable_ref_t::operator=(const mutable_ref_t& a_mutable_ref)
{
  mutable_ref_t mutable_ref(a_mutable_ref);
  IRS_STD swap(mutable_ref, *this);
  return *this;
}

#endif // NOP

bool irs::calc::calc_variables_t::value_write(
  const mutable_ref_t& a_mutable_ref_src,
  mutable_ref_t* ap_mutable_ref_dest)
{
  IRS_LIB_ASSERT(mp_calculator);
  return mp_calculator->value_write(a_mutable_ref_src, ap_mutable_ref_dest);
}

//#endif // GNU C++ specific

#endif //__WATCOMC__
