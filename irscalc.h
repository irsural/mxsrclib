//---------------------------------------------------------------------------
// Калькулятор
// Дата: 21.09.2009

#ifndef irscalcH
#define irscalcH

// В Watcom C++ есть только не-const версия функции map::find
#ifndef __WATCOMC__

#include <iostream>
#include <irsstdg.h>
#include <irserror.h>
#include <math.h>
#include <irsvariant.h>
#include <irscpp.h>

namespace irs {

enum type_function_t {
  tf_first = 1,
  tf_r_int_a_int = tf_first,
  tf_r_float_a_float = tf_r_int_a_int+1,
  tf_r_double_a_double = tf_r_float_a_float+1,
  tf_r_ldouble_a_ldouble = tf_r_double_a_double+1,
  tf_last = tf_r_ldouble_a_ldouble};     

// Предварительная обработка стоки.
// Заменяет все запятые на точки
// Удаляет повторяющиеся пробелы,
// переходы на следующую строку,
// заменяет табуляции на пробелы.
irs::string preprocessing_str(const irs::string& a_str);

namespace calc {

typedef size_t sizens_t;
typedef char_t charns_t;
typedef string_t stringns_t;
typedef variant::variant_t valuens_t;

typedef int (*func_r_int_a_int_ptr)(int);
typedef float (*func_r_float_a_float_ptr)(float);
//typedef double (*func_r_double_a_double_ptr)(double);
typedef long double (*func_r_ldouble_a_ldouble_ptr)(long double);

enum token_type_t {
  tt_none,
  tt_number,
  tt_delimiter,
  tt_constant,
  tt_function};
// Список идентификаторов

enum delimiter_t {
  d_none,
  d_plus,
  d_minus,
  d_multiply,
  d_division,
  d_integer_division,
  d_involution,
  d_left_parenthesis,
  d_right_parenthesis,
  d_left_square_bracket,
  d_right_square_bracket,
  d_comma,
  d_and,
  d_or,
  d_not,
  d_compare_equal,
  d_compare_not_equal,
  d_compare_less,
  d_compare_greater,
  d_compare_less_or_equal,
  d_compare_greater_or_equal,
  d_end};

enum keyword_t {
  keyword_and,
  keyword_and_eq,
  keyword_asm,
  keyword_auto,
  keyword_bitand,
  keyword_bitor,
  keyword_bool,
  keyword_break,
  keyword_case,
  keyword_catch,
  keyword_char,
  keyword_class,
  keyword_compl,
  keyword_const,
  keyword_const_cast,
  keyword_continue,
  keyword_default,
  keyword_delete,
  keyword_do,
  keyword_double,
  keyword_dynamic_cast,
  keyword_else,
  keyword_enum,
  keyword_explicit,
  keyword_export,
  keyword_extern,
  keyword_false,
  keyword_float,
  keyword_for,
  keyword_friend,
  keyword_goto,
  keyword_if,
  keyword_inline,
  keyword_int,
  keyword_long,
  keyword_mutable,
  keyword_namespace,
  keyword_new,
  keyword_not,
  keyword_not_eq,
  keyword_operator,
  keyword_or,
  keyword_or_eq,
  keyword_private,
  keyword_protected,
  keyword_public,
  keyword_register,
  keyword_reinterpret_cast,
  keyword_return,
  keyword_short,
  keyword_signed,
  keyword_sizeof,
  keyword_static,
  keyword_static_cast,
  keyword_struct,
  keyword_switch,
  keyword_template,
  keyword_this,
  keyword_throw,
  keyword_true,
  keyword_try,
  keyword_typedef,
  keyword_typeid,
  keyword_typename,
  keyword_union,
  keyword_unsigned,
  keyword_using,
  keyword_virtual,
  keyword_void,
  keyword_volatile,
  keyword_wchar_t,
  keyword_while,
  keyword_xor,
  keyword_xor_eq};

typedef map<stringns_t, keyword_t> keyword_map_type;
typedef map<stringns_t, keyword_t> keyword_map_iterator;
typedef map<stringns_t, keyword_t> keyword_map_const_iterator;

void create_keyword_map(keyword_map_type* ap_keyword_map);

class function_t
{
public:
  inline virtual ~function_t()
  {
  }
  virtual void exec(variant::variant_t* ap_parameters,
    variant::variant_t* ap_returned_parameter) const = 0;
};

class func_r_int_a_int_t: public function_t
{
public:
  typedef int (*func_r_int_a_int_ptr)(int);
  inline func_r_int_a_int_t(func_r_int_a_int_ptr ap_func
  ):
    mp_func(ap_func)
  {
  }
  inline virtual void exec(variant::variant_t* ap_parameters,
    variant::variant_t* ap_returned_parameter) const
  {
    IRS_LIB_ASSERT(ap_parameters->type() == variant::var_type_array);
    if (ap_parameters->size() == 1) {
      *ap_returned_parameter = (*mp_func)((*ap_parameters)[0].as_int());
    } else {
      // Недопустимое количество параметров
    }
  }
private:
  func_r_int_a_int_ptr mp_func;
  func_r_int_a_int_t();
};

class func_r_double_a_double_t: public function_t
{
public:
  typedef double (*func_r_double_a_double_ptr_type)(double);
  inline explicit func_r_double_a_double_t(
    func_r_double_a_double_ptr_type ap_func
  ):
    mp_func(ap_func)
  {
  }
  inline virtual void exec(variant::variant_t* ap_parameters,
    variant::variant_t* ap_returned_parameter) const
  {
    IRS_LIB_ASSERT(ap_parameters->type() == variant::var_type_array);
    if (ap_parameters->size() == 1) {
      *ap_returned_parameter = (*mp_func)((*ap_parameters)[0].as_double());
    } else {
      // Недопустимое количество параметров
    }
  }
private:
  func_r_double_a_double_ptr_type mp_func;
  func_r_double_a_double_t();
};

class pow_t: public function_t
{
public:
  inline virtual void exec(variant::variant_t* ap_parameters,
    variant::variant_t* ap_returned_parameter) const
  {
    IRS_LIB_ASSERT(ap_parameters->type() == variant::var_type_array);
    if (ap_parameters->size() == 2) {
      *ap_returned_parameter = pow((*ap_parameters)[0].as_double(),
        (*ap_parameters)[1].as_double());
    } else {
      // Недопустимое количество параметров
    }
  }
};

class list_identifier_t
{
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  typedef map<string_type, value_type> constant_list_type;
  typedef map<string_type, value_type>::iterator
    constant_list_iterator;
  typedef map<string_type, value_type>::const_iterator
    constant_list_const_iterator;
  typedef map<string_type, handle_t<function_t> > function_map_type;
  typedef map<string_type, handle_t<function_t> >::iterator
    function_map_iterator;
  typedef map<string_type, handle_t<function_t> >::const_iterator
    function_map_const_iterator;
  inline list_identifier_t();
  inline void constant_add(
    const pair<string_type, value_type>& a_constant_pair);
  inline void constant_del(const string_type& a_constant_name);
  inline void constant_clear();
  inline bool constant_is_exists(const string_type& a_constant_name) const;
  inline bool constant_find(const string_type& a_name,
    const value_type** ap_p_constant) const;
  inline void function_add(const pair<string_type,
    handle_t<function_t> >& a_function_pair);
  inline void function_del(const string_type& a_function_name);
  inline void function_clear();
  inline bool is_function_exists(const string_type& a_function_name);
  inline bool function_find(const string_type& a_function_name,
    const function_t** ap_p_function) const;

  inline bool del_func(const string_type& a_name);
  inline void clear_func();
  //inline const function_t& get_func(size_type a_id_func) const;
private:
  keyword_map_type m_keyword_map;
  vector<function_t> m_func_array;
  constant_list_type m_constant_list;
  function_map_type m_function_map;
  inline bool add_func(
    const string_type& a_name,
    const type_function_t a_type,
    void* ap_function);
};

inline void list_identifier_t::constant_add(const pair<string_type,
  value_type>& a_constant_pair)
{
  IRS_LIB_ASSERT(m_keyword_map.find(a_constant_pair.first) ==
    m_keyword_map.end());
  IRS_LIB_ASSERT(m_constant_list.find(a_constant_pair.first) ==
    m_constant_list.end());
  IRS_LIB_ASSERT(m_function_map.find(a_constant_pair.first) ==
    m_function_map.end());    
  m_constant_list.insert(a_constant_pair);
}

inline void list_identifier_t::constant_del(
  const string_type& a_constant_name)
{
  constant_list_iterator it_constant =
    m_constant_list.find(a_constant_name);
  IRS_LIB_ASSERT(it_constant != m_constant_list.end());
  m_constant_list.erase(it_constant);
}

inline void list_identifier_t::constant_clear()
{
  m_constant_list.clear();
}

inline bool list_identifier_t::constant_is_exists(
  const string_type& a_constant_name) const
{
  return m_constant_list.find(a_constant_name) != m_constant_list.end();
}

inline bool list_identifier_t::constant_find(
  const string_type& a_name, const value_type** ap_p_constant) const
{
  constant_list_const_iterator it_constant =
    m_constant_list.find(a_name);
  bool constant_is_exists = it_constant != m_constant_list.end();
  if (constant_is_exists) {
    *ap_p_constant = &it_constant->second;
  } else {
    // Константа с таким именем отсутсвует
  }
  return constant_is_exists;
}

inline void list_identifier_t::function_add(const pair<string_type,
  handle_t<function_t> >& a_function_pair)
{
  IRS_LIB_ASSERT(m_keyword_map.find(a_function_pair.first) ==
    m_keyword_map.end());
  IRS_LIB_ASSERT(m_constant_list.find(a_function_pair.first) ==
    m_constant_list.end());
  IRS_LIB_ASSERT(m_function_map.find(a_function_pair.first) ==
    m_function_map.end());
  m_function_map.insert(a_function_pair);
}

inline void list_identifier_t::function_del(const string_type& a_function_name)
{
  function_map_iterator it_function = m_function_map.find(a_function_name);
  IRS_LIB_ASSERT(it_function != m_function_map.end());
  m_function_map.erase(it_function);
}

inline void list_identifier_t::function_clear()
{
  m_function_map.clear();
}

inline bool list_identifier_t::is_function_exists(
  const string_type& a_function_name)
{
  return m_function_map.find(a_function_name) != m_function_map.end();
}

inline bool list_identifier_t::function_find(const string_type& a_function_name,
  const function_t** ap_p_function) const
{
  function_map_const_iterator it_function =
    m_function_map.find(a_function_name);
  bool function_is_exists = it_function != m_function_map.end();
  if (function_is_exists) {
    *ap_p_function = it_function->second.get();
  } else {
    // Константа с таким именем отсутсвует
  }
  return function_is_exists;
}

list_identifier_t::list_identifier_t(
):
  m_keyword_map(),
  m_constant_list(),
  m_function_map()
{
  create_keyword_map(&m_keyword_map);
}

class token_t
{
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  token_type_t m_token_type;
  value_type m_num;
  delimiter_t m_delimiter;
  size_type m_id_function;
  const value_type* mp_constant;
  const function_t* mp_function;
public:
  token_t(
  ):
    m_token_type(tt_none),
    m_num(0),
    m_delimiter(d_none),
    m_id_function(0),
    mp_constant(IRS_NULL),
    mp_function(IRS_NULL)
  {}
  inline void set_number(const value_type& a_num)
  {
    m_token_type = tt_number;
    m_num = a_num;
    m_delimiter = d_none;
    mp_function = IRS_NULL;
  }
  inline void set_delimiter(const delimiter_t a_delimiter)
  {
    m_token_type = tt_delimiter;
    m_num = 0;
    m_delimiter = a_delimiter;
    mp_function = IRS_NULL;
  }
  inline void set_function(const function_t* ap_function)
  {
    m_token_type = tt_function;
    m_num = 0;
    m_delimiter = d_none;
    mp_function = ap_function;
  }
  inline void set_constant(const value_type* ap_constant)
  {
    m_token_type = tt_constant;
    m_num = 0;
    m_delimiter = d_none;
    mp_function = IRS_NULL;
    mp_constant = ap_constant;
  }
  inline void set_not_a_token_type()
  {
    m_token_type = tt_none;
    m_num = 0;
    m_delimiter = d_none;
    mp_function = IRS_NULL;
  }
  inline delimiter_t delimiter() const
  {
    return m_delimiter;
  }
  inline token_type_t token_type() const
  {
    return m_token_type;
  }
  inline const function_t* get_function() const
  {
    return mp_function;
  }
  inline const value_type* get_constant() const
  {
    return mp_constant;
  }
  inline value_type get_number() const
  {
    return m_num;
  }
};

inline bool is_token_operator_logical(const token_t& a_token)
{
  bool is_operator_logical = false;
  if (a_token.token_type() == tt_delimiter) {
    const delimiter_t delim = a_token.delimiter();
    if ((delim >= d_and) && (delim <= d_not)) {
      is_operator_logical = true;
    } else {
      // Это не логический оператор
    }
  } else {
    // Это не ограничитель
  }
  return is_operator_logical;
}

inline bool is_token_operator_compare(const token_t& a_token)
{
  bool is_operator_compare = false;
  if (a_token.token_type() == tt_delimiter) {
    const delimiter_t delim = a_token.delimiter();
    if ((delim >= d_compare_equal) && (delim <= d_compare_greater_or_equal)) {
      is_operator_compare = true;
    } else {
      // Это не оператор сравнения
    }
  } else {
    // Это не ограничитель
  }
  return is_operator_compare;
}

// Детектор лексем
class detector_token_t
{
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  //typedef list_identifier_t::array_type array_type;
  detector_token_t(
    list_identifier_t& a_list_identifier):
    m_list_identifier(a_list_identifier),
    m_cur_token_data(),
    mp_prog(IRS_NULL),
    m_prog_pos(0),
    m_exponent_ch(irst('E')),
    m_ch_valid_name("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz")
  { }
  // Установить указатель на текст программы
  inline void set_prog(const string_type* ap_prog);
  inline bool next_token();
  //bool back_token();
  inline bool get_token(token_t* const ap_token);
private:
  // Список идентификаторов
  const list_identifier_t& m_list_identifier;
  struct token_data_t
  {
    token_t token;
    size_type length;
    bool valid;
    token_data_t():
      token(),
      length(0),
      valid(false)
    {}
  };
  token_data_t m_cur_token_data;
  // Указатель на текст программы
  const string_type* mp_prog;
  // Текущая позиция в тексте программы
  size_type m_prog_pos;
  // Символ показателя степени
  char_type m_exponent_ch;
  // Строка допустимых символов в имени идентификатора
  const string_type m_ch_valid_name;
  // Проверка символа на принадлежность к символу числа
  inline bool ch_is_digit(const char_type a_ch);
  // Проверка символа на пригодность в качестве первого символа имени
  inline bool ch_is_first_char_name(const char_type a_ch);
  // Идентифицировать лексему в текущей позиции программы
  inline bool detect_token();
};

inline bool detector_token_t::ch_is_digit(const char_type a_ch)
{
  bool isdigit_status = false;
  if (strchr("0123456789", a_ch)) {
    isdigit_status = true;
  } else if (m_exponent_ch == a_ch) {
    isdigit_status = true;
  }
  return isdigit_status;
}

inline bool detector_token_t::ch_is_first_char_name(const char_type a_ch)
{
  bool ch_valid = false;
  // Заглавная буква латинскаго алфавита
  bool ch_latin_capital = (a_ch >= irst('A')) && (a_ch <= irst('Z'));
  // Строчная буква латинского алфавита
  bool ch_latin_small = (a_ch >= irst('a')) && (a_ch <= irst('z'));
  // Символ подчеркивания
  bool ch_underscore = (a_ch == irst('_'));
 
  if (ch_latin_capital || ch_latin_small || ch_underscore) {
    ch_valid = true;
  }
  return ch_valid;
}

inline bool detector_token_t::detect_token()
{
  bool detected_token = false;
  //bool fsuccess = true;
  if (m_prog_pos == mp_prog->size()) {
    m_cur_token_data.token.set_delimiter(d_end);
    m_cur_token_data.length = 0;
    m_cur_token_data.valid = true;
    detected_token = true;
  }
  size_type pos;
  char_type ch = irst('\0');
  if (!detected_token) {
    // Пропускаем пробелы, табуляции и переходы на новую строку
    pos = mp_prog->find_first_not_of(" \r\t", m_prog_pos);
    if (pos == string_type::npos) {
      // Достигнут конец программы
      m_prog_pos = mp_prog->size();
      m_cur_token_data.token.set_delimiter(d_end);
      m_cur_token_data.length = 0;
      m_cur_token_data.valid = true;
      detected_token = true;
    } else {
      ch = (*mp_prog)[pos];
    }
  }

  if (!detected_token) {
    // Читаем ограничитель
    bool next_ch_not_end_prog = false;
    const size_type begin_ch_pos = pos;
    char_type next_ch = irst('\0');
    if ((pos+1) < mp_prog->size()) {
      next_ch_not_end_prog = true;
      next_ch = (*mp_prog)[pos+1];
    } else {
      // Следующий символ отсутсвует
    }
    switch(ch) {
      case irst('&'): {
        if (next_ch_not_end_prog && (next_ch == irst('&'))) {
          m_cur_token_data.token.set_delimiter(d_and);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case irst('|'): {
        if (next_ch_not_end_prog && (next_ch == irst('|'))) {
          m_cur_token_data.token.set_delimiter(d_or);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case irst('='): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          m_cur_token_data.token.set_delimiter(d_compare_equal);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case irst('!'): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          m_cur_token_data.token.set_delimiter(d_compare_not_equal);
          detected_token = true;
          pos += 2;
        } else {
          m_cur_token_data.token.set_delimiter(d_not);
          detected_token = true;
          pos ++;
        }
      } break;
      case irst('<'): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          m_cur_token_data.token.set_delimiter(d_compare_less_or_equal);
          detected_token = true;
          pos += 2;
        } else {
          m_cur_token_data.token.set_delimiter(d_compare_less);
          detected_token = true;
          pos++;
        }
      } break;
      case irst('>'): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          m_cur_token_data.token.set_delimiter(d_compare_greater_or_equal);
          detected_token = true;
          pos += 2;
        } else {
          m_cur_token_data.token.set_delimiter(d_compare_greater);
          detected_token = true;
          pos++;
        }
      } break;
      case irst('+'): {
        m_cur_token_data.token.set_delimiter(d_plus);
        detected_token = true;
        pos++;
      } break;
      case irst('-'): {
        m_cur_token_data.token.set_delimiter(d_minus);
        detected_token = true;
        pos++;
      } break;
      case irst('*'): {
        m_cur_token_data.token.set_delimiter(d_multiply);
        detected_token = true;
        pos++;
      } break;
      case irst('/'): {
        m_cur_token_data.token.set_delimiter(d_division);
        detected_token = true;
        pos++;
      } break;
      case irst('%'): {
        m_cur_token_data.token.set_delimiter(d_integer_division);
        detected_token = true;
        pos++;
      } break;
      /*case irst('^'): {
        m_cur_token_data.token.set_delimiter(d_involution);
        detected_token = true;
        pos++;
      } break;*/
      case irst('('): {
        m_cur_token_data.token.set_delimiter(d_left_parenthesis);
        detected_token = true;
        pos++;
      } break;
      case irst(')'): {
        m_cur_token_data.token.set_delimiter(d_right_parenthesis);
        detected_token = true;
        pos++;
      } break;
      case irst('['): {
        m_cur_token_data.token.set_delimiter(d_left_square_bracket);
        detected_token = true;
        pos++;
      } break;
      case irst(']'): {
        m_cur_token_data.token.set_delimiter(d_right_square_bracket);
        detected_token = true;
        pos++;
      } break;
      default : {
        detected_token = false;
      }
    }
    if (detected_token) {
      m_cur_token_data.length = pos - m_prog_pos;
      m_cur_token_data.valid = true;
    } else {
      pos = begin_ch_pos;
    }
  }

  if (!detected_token) {
    // Читаем число       
    if (ch_is_digit(ch)) {
      // Запоминаем позицию первого символа числа
      const size_type num_begin_ch = pos;
      string_type num_str;
      pos++;
      // Статус обнаружения символа показателя степени
      bool detected_exponent_ch = false;
      // Статус обнаружения символа - разделителя целой и дробной части
      bool detected_int_part_delim_ch = false;
      while (pos <  mp_prog->size()) {
        // Ищем первый символ, не являющийся цифрой
        pos = mp_prog->find_first_not_of("0123456789", pos);
        // Если не найден такой символ
        if (pos == string_type::npos) {
          pos = mp_prog->size();
          break;
        }
        const char_type ch_number = (*mp_prog)[pos];
        if (ch_number == irst('.')) {
          if ((!detected_exponent_ch) && (!detected_int_part_delim_ch)) {
            detected_exponent_ch = true;
            pos++;
          } else {
            break;
          }
        } else if (ch_number == m_exponent_ch) {
          if (detected_exponent_ch) {
            break;
          }
          if ((pos + 1) >= mp_prog->size()) {
            break;
          }
          char_type ch_next = (*mp_prog)[pos + 1];
          if (strchr("0123456789", ch_next)) {
            detected_exponent_ch = true;
            pos++;
            continue;
          }
          if (!strchr("+-", ch_next)) {
            break;
          }
          size_type new_pos = pos + 1;
          new_pos = mp_prog->find_first_not_of("+-", new_pos);
          if (new_pos == string_type::npos) {
            break;
          }
          char_type ch_not_sign = (*mp_prog)[new_pos];
          if (strchr("0123456789", ch_not_sign)) {
            pos = new_pos;
            continue;
          } else {
            break;
          }
        } else {
          break;
        }
      }
      num_str = mp_prog->substr(num_begin_ch, (pos - num_begin_ch));
      double number;
      if (num_str.to_number(number)) {
        m_cur_token_data.token.set_number(number);
        m_cur_token_data.length = pos - m_prog_pos;
        m_cur_token_data.valid = true;
        detected_token = true;
      } else {
        // Строку не удалось преобразовать в число
      }
      if (!detected_token) {
        pos = num_begin_ch;
      } else {
        // Лексема детектирована
      }
    } else {
      // Символ не принадлежит группе символов, допустимых при написании чисел
    }
  }

  // Читаем идентификатор или ключевое слово
  if (!detected_token) {
    if (ch_is_first_char_name(ch)) {
      const size_type pos_begin_name = pos;
      pos = mp_prog->find_first_not_of(m_ch_valid_name, pos);
      if (pos == string_type::npos) {
        pos = mp_prog->size();
      } else {
        // Символ, не принадлежащий группе символов, допустимых в имени,
        // не найден
      }
      string_type identifier_str =
        mp_prog->substr(pos_begin_name, (pos - pos_begin_name));

      if (identifier_str == irst("and")) {
        m_cur_token_data.token.set_delimiter(d_and);
        m_cur_token_data.length = pos - m_prog_pos;
        m_cur_token_data.valid = true;
        detected_token = true;
      } else if (identifier_str == irst("or")) {
        m_cur_token_data.token.set_delimiter(d_or);
        m_cur_token_data.length = pos - m_prog_pos;
        m_cur_token_data.valid = true;
        detected_token = true;
      } else if (identifier_str == irst("not")) {
        m_cur_token_data.token.set_delimiter(d_not);
        m_cur_token_data.length = pos - m_prog_pos;
        m_cur_token_data.valid = true;
        detected_token = true;
      } else {
        const function_t* p_function = IRS_NULL;
        if (m_list_identifier.function_find(identifier_str, &p_function)) {
          m_cur_token_data.token.set_function(p_function);
          m_cur_token_data.length = pos - m_prog_pos;
          m_cur_token_data.valid = true;
          detected_token = true;
        } else {
          const value_type* p_constant = IRS_NULL;
          if (m_list_identifier.constant_find(identifier_str, &p_constant)) {
            m_cur_token_data.token.set_constant(p_constant);
            m_cur_token_data.length = pos - m_prog_pos;
            m_cur_token_data.valid = true;
            detected_token = true;
          } else {
            // Констранта с таким именем не найдена
          }
        }
      }
      if (!detected_token) {
        pos = pos_begin_name;
      } else {
        // Лексема детектирована
      }
    } else {
      // Символ не принадлежит группе символов, допустимых в имени
    }
  }   
  return detected_token;
}

inline void detector_token_t::set_prog(const string_type* ap_prog)
{
  mp_prog = ap_prog;
  m_prog_pos = 0;
  m_cur_token_data.valid = false;
}

inline bool detector_token_t::next_token()
{
  bool fsuccess = true;
  if (m_cur_token_data.valid) {
    if ((m_prog_pos + m_cur_token_data.length) <= mp_prog->size()) {
      m_prog_pos += m_cur_token_data.length;
      m_cur_token_data.valid = false;
      fsuccess = detect_token();
    } else {
      IRS_LIB_ASSERT_MSG("Ошибка при перемещении к следующему сиволу");
      fsuccess = false;
    }
  } else {
    fsuccess = detect_token();
  }
  return fsuccess;
}

inline bool detector_token_t::
  get_token(token_t* const ap_token)
{
  bool fsuccess = true;
  if (!m_cur_token_data.valid) {
    fsuccess = detect_token();
  } else {
    // Лексема уже детектирована
  }
  if (fsuccess) {
    *ap_token = m_cur_token_data.token;
  } else {
    // Произошла ошибка
  }         
  return fsuccess;
}

class calculator_t
{
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  inline calculator_t();
  inline bool calc(const string_type* ap_prog, value_type* ap_num);
  inline void constant_add(const string_type& a_name,
    const value_type& a_value);
  inline void constant_del(const string_type& a_name);
  inline void constant_clear();
  inline void function_add(const string_type& a_name,
    const handle_t<function_t> ap_function);
  inline void function_del(const string_type& a_name);
  inline void function_clear();
private:
  list_identifier_t m_list_identifier;
  detector_token_t m_detector_token;
  inline bool interp(value_type* ap_value);
  // Обрабатывает квадратные скобки
  inline bool eval_exp_square_brackets(value_type* ap_value);
  // Обрабатывает логические операции
  inline bool eval_exp_logical(value_type* ap_value);
  // Обрабатывает операции отношения
  inline bool eval_exp_compare(value_type* ap_value);
  // Обрабатывает сложение и вычитание
  inline bool eval_exp_arithmetic_level1(value_type* ap_value);
  // Обрабатывает умножение, деление, целочисленное деление
  inline bool eval_exp_arithmetic_level2(value_type* ap_value);
  // Обрабатывает унарную логическую операцию "не"
  inline bool eval_exp_not(value_type* ap_value);  
  // Обрабатывает возведение в степень
  inline bool eval_exp_power(value_type* ap_value);
  // Обрабатывает скобки
  inline bool eval_exp_brackets(value_type* ap_value);
  // Обрабатывает функции и числа
  inline bool atom(value_type* ap_value);
  // Выполнение функцию
  inline void func_exec(
    const function_t& a_function,
    const value_type a_in_param,
    value_type* ap_out_param);
};

inline calculator_t::calculator_t():
  m_list_identifier(),
  m_detector_token(m_list_identifier)
{
  func_r_double_a_double_t::func_r_double_a_double_ptr_type
    func_r_dbl_a_dbl_ptr = IRS_NULL;
  func_r_dbl_a_dbl_ptr = acos;
  function_add("acos", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = asin;
  function_add("asin", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = atan;
  function_add("atan", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = ceil;
  function_add("ceil", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = cos;
  function_add("cos", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = cosh;
  function_add("cosh", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = exp;
  function_add("exp", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = fabs;
  function_add("fabs", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = floor;
  function_add("floor", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = log;
  function_add("log", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = log10;
  function_add("log10", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = sin;
  function_add("sin", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = sinh;
  function_add("sinh", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = sqrt;
  function_add("sqrt", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = tan;
  function_add("tan", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  func_r_dbl_a_dbl_ptr = tanh;
  function_add("tanh", new func_r_double_a_double_t(func_r_dbl_a_dbl_ptr));
  function_add("pow", new pow_t());

  constant_add("e", IRS_E);
  constant_add("pi", IRS_PI);

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
  constant_add("arr", arr);   
}

inline bool calculator_t::interp(
  value_type* ap_value)
{
  value_type value = 0;
  // m_cur_lexeme_index = 0;
  bool fsuccess = eval_exp_logical(&value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    if (fsuccess) {
      if (token.token_type() == tt_delimiter) {
        delimiter_t delim = token.delimiter();
        if (delim == d_end) {
          *ap_value = value;
        } else {
          // Текущим ограничителем должен быть конец программы
          fsuccess = false;
        }
      } else {
        // Текущей лексемой должен быть ограничитель
        fsuccess = false;
      }
    } else {
      // Произошла ошибка
    }
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_square_brackets(value_type* ap_value)
{
  bool fsuccess = true;
  token_t token;
  fsuccess = m_detector_token.get_token(&token);
  const delimiter_t delim = token.delimiter();
  if (fsuccess) {
    if (delim == d_left_square_bracket) {
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = eval_exp_logical(ap_value);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        if (token.delimiter() == d_right_square_bracket) {
          // Закрывающая квадратная скобка присутсвует
          fsuccess = m_detector_token.next_token();
        } else {
          fsuccess = false;
        }
      } else {
        // Произошла ошибка
      }
    } else {
      // Квадратная левая скобка не обнаружена
      fsuccess = false;
    }
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_logical(value_type* ap_value)
{
  bool fsuccess = true;
  fsuccess = eval_exp_compare(ap_value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    if (fsuccess) {
      delimiter_t delim = token.delimiter();
      while(is_token_operator_logical(token) && fsuccess) { 
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          if ((delim == d_and) || (delim == d_or)) {
            value_type partial_value = 0;
            fsuccess = eval_exp_compare(&partial_value);
            if (fsuccess) {
              if (delim == d_and) {
                *ap_value = ap_value->as_bool() && partial_value.as_bool();
              } else {
                *ap_value = ap_value->as_bool() || partial_value.as_bool();
              }
              fsuccess = m_detector_token.get_token(&token);
            } else {
              // Произошла ошибка
            }
          } else {
            // Лексема не является логической бинарной операцией
          }
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          delim = token.delimiter();
        } else {
          // Произошла ошибка
        }
      }
    } else {
      // Произошла ошибка
    }
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_compare(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_arithmetic_level1(ap_value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    delimiter_t delim;// = token.delimiter();
    while (is_token_operator_compare(token) && fsuccess) {
      delim = token.delimiter();
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = eval_exp_arithmetic_level1(&partial_value);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        switch (delim) {
          case d_compare_equal: {
            *ap_value = (*ap_value == partial_value);
          } break;
          case d_compare_not_equal: {
            *ap_value = (*ap_value != partial_value);
          } break;
          case d_compare_less: {
            *ap_value = (*ap_value < partial_value);
          } break;
          case d_compare_greater: {
            *ap_value = (*ap_value > partial_value);
          } break;
          case d_compare_less_or_equal: {
            *ap_value = (*ap_value <= partial_value);
          } break;
          case d_compare_greater_or_equal: {
            *ap_value = (*ap_value >= partial_value);
          } break;
          default : {
            IRS_LIB_ASSERT_MSG("Лексема должна быть операцией сравнения");
          }
        }
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      /*if (fsuccess) {
        delim = token.delimiter();
      } else {
        // Произошла ошибка
      }*/
    }
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_arithmetic_level1(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_arithmetic_level2(ap_value);
  if (fsuccess) {
    //token_t token = m_lexeme_array[m_cur_lexeme_index];
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    delimiter_t delim = token.delimiter();
    if (fsuccess) {
      while(((delim == d_plus) || (delim == d_minus)) && fsuccess) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_arithmetic_level2(&partial_value);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          if (delim == d_plus) {
            *ap_value = *ap_value + partial_value;
          } else if (delim == d_minus) {
            *ap_value = *ap_value - partial_value;
          }
          fsuccess = m_detector_token.get_token(&token);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          delim = token.delimiter();
        } else {
          // Произошла ошибка
        }
      }
    } else {
      // Произошла ошибка при извлечении лексемы
    }
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_arithmetic_level2(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_not(ap_value);
  //fsuccess = eval_exp_power(ap_value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    delimiter_t delim = token.delimiter();
    if (fsuccess) {
      while(fsuccess && (delim >= d_multiply) && (delim <= d_integer_division))
      {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_not(&partial_value);
          //fsuccess = eval_exp_power(&partial_value);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          if (token.delimiter() == d_multiply) {
            *ap_value = *ap_value * partial_value;
          } else if (token.delimiter() == d_division) {
            if (partial_value != 0) {
              *ap_value = *ap_value / partial_value;
            } else {
              fsuccess = false;
            }
          } else if (token.delimiter() == d_integer_division) {
            *ap_value =
              static_cast<int>(*ap_value) % static_cast<int>(partial_value);
          }
        }
        if (fsuccess) {
          fsuccess = m_detector_token.get_token(&token);
        } else {
          // Произошла ошибка
        }
        delim = token.delimiter();
      }
    }
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_not(value_type* ap_value)
{
  bool fsuccess = true;
  token_t token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    size_type operation_not_cnt = 0;
    while(fsuccess && (token.delimiter() == d_not)) {
      operation_not_cnt++;
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
    }
    if (operation_not_cnt == 0) {
      fsuccess = eval_exp_brackets(ap_value);
    } else {
      value_type partial_value = 0;
      fsuccess = eval_exp_brackets(&partial_value);
      if (fsuccess) {
        if (operation_not_cnt % 2 == 0) {
          *ap_value = partial_value.as_bool();
        } else {
          *ap_value = !partial_value.as_bool();
        }
      } else {
        // Произошла ошибка
      }
    }
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_power(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_brackets(ap_value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    if (fsuccess) {
      while(fsuccess && (token.delimiter() == d_involution)) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_brackets(&partial_value);
        }
        if (fsuccess) {
          if (token.delimiter() == d_involution) {
            *ap_value = pow(*ap_value, static_cast<double>(partial_value));
          }
          fsuccess = m_detector_token.get_token(&token);
        } else {
          // Произошла ошибка
        }
      }
    } else {
      // Произошла ошибка
    }
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

// обрабатывает выражения со скобками
inline bool calculator_t::eval_exp_brackets(value_type* ap_value)
{
  bool fsuccess = true;
  token_t token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    const delimiter_t delim = token.delimiter();
    if (delim == d_left_parenthesis) {
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = eval_exp_logical(ap_value);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        if (token.delimiter() == d_right_parenthesis) {
          // Закрывающая круглая скобка присутсвует
          fsuccess = m_detector_token.next_token();
        } else {
          fsuccess = false;
        }
      } else {
        // Произошла ошибка
      }
    } else {
      fsuccess = atom(ap_value);
    }
  }
  return fsuccess;
}


inline bool calculator_t::atom(value_type* ap_value)
{
  bool fsuccess = true;
  token_t token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    if (token.token_type() == tt_function) {
      const function_t* p_func = token.get_function();
      fsuccess = m_detector_token.next_token();
      value_type arg_func = 0;
      if (fsuccess) {
        fsuccess = eval_exp_arithmetic_level1(&arg_func);
        //fsuccess = eval_exp_power(&arg_func);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        value_type parameters;
        parameters.type(irs::variant::var_type_array);
        parameters.resize(1);
        parameters[0] = arg_func;  
        p_func->exec(&parameters, ap_value);
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
    } else if (token.token_type() == tt_number) {
      *ap_value = token.get_number();
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
          *ap_value *= -1;
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

inline bool calculator_t::
  calc(const string_type* ap_prog, value_type* ap_num)
{
  m_detector_token.set_prog(ap_prog);
  return interp(ap_num);
}

inline void calculator_t::constant_add(
  const string_type& a_name, const value_type& a_value)
{
  m_list_identifier.constant_add(make_pair(a_name, a_value));
}

inline void calculator_t::constant_del(const string_type& a_name)
{
  m_list_identifier.constant_del(a_name);
}    

inline void calculator_t::constant_clear()
{
  m_list_identifier.constant_clear();
}

inline void calculator_t::function_add(const string_type& a_name,
  const handle_t<function_t> ap_function)
{
  m_list_identifier.function_add(make_pair(a_name, ap_function));
}

inline void calculator_t::function_del(const string_type& a_name)
{
  m_list_identifier.function_del(a_name);
}

inline void calculator_t::function_clear()
{
  m_list_identifier.function_clear();
}

} // namespace calc

} // namespace irs

#endif //__WATCOMC__

#endif //irscalcH
