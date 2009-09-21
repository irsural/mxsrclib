//---------------------------------------------------------------------------
// Калькулятор
// Дата: 19.09.2009

#ifndef irscalcH
#define irscalcH

// В Watcom C++ есть только не-const версия функции map::find
#ifndef __WATCOMC__

#include <iostream>
#include <irsstdg.h>
#include <irserror.h>
#include <math.h>
#include <irsvariant.h>

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
typedef double (*func_r_double_a_double_ptr)(double);
typedef long double (*func_r_ldouble_a_ldouble_ptr)(long double);
enum token_type_t {
  tt_none,
  tt_number,
  tt_delimiter,
  tt_constant,
  tt_function/*,
  tt_array*/};
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
  d_compare_equal,
  d_compare_not_equal,
  d_compare_less,
  d_compare_greater,
  d_compare_less_or_equal,
  d_compare_greater_or_equal,
  d_end};

struct function_t
{
  typedef stringns_t string_type;
  string_type name;
  type_function_t type;
  void* ptr;
  union{
    int (*func_r_int_a_int_ptr)(int);
    float (*func_r_float_a_float_ptr)(float);
    double (*func_r_double_a_double_ptr)(double);
    long double (*func_r_ldouble_a_ldouble_ptr)(long double);
  };
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
  typedef vector<value_type> array_type;
  typedef map<string_type, vector<value_type> > array_list_type;
  typedef map<string_type, vector<value_type> >::iterator
    array_list_iterator;
  typedef map<string_type, vector<value_type> >::const_iterator
    array_list_const_iterator;
  inline list_identifier_t();
  inline int find_function(const string_type& a_function_name) const;
  inline bool add_func_r_int_a_int(
    const string_type& a_name, func_r_int_a_int_ptr ap_function);
  inline bool add_func_r_float_a_float(
    const string_type& a_name, func_r_float_a_float_ptr ap_function);
  inline bool add_func_r_double_a_double(
    const string_type& a_name, func_r_double_a_double_ptr ap_function);
  inline bool add_func_r_ldouble_a_ldouble(
    const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function);
  inline void constant_add(
    const pair<string_type, value_type>& a_constant_pair);
  inline void constant_del(const string_type& a_constant_name);
  inline void constant_clear();
  inline bool constant_is_exists(const string_type& a_constant_name) const;
  inline bool constant_find(const string_type& a_name,
    const value_type** ap_p_constant) const;
  /*inline void array_add(
    const pair<string_type, array_type>& a_array_pair);
  inline void array_del(const string_type& a_array_name);
  inline void array_clear();
  inline bool array_is_exists(const string_type& a_array_name) const;
  inline bool array_find(const string_type& a_name,
    const array_type** ap_p_array) const;*/
  inline bool del_func(const string_type& a_name);
  inline void clear_func();
  //void clear_constant();
  inline const function_t& get_func(size_type a_id_func) const;
private:
  vector<function_t> m_func_array;
  constant_list_type m_constant_list;
  array_list_type m_array_list;
  inline bool add_func(
    const string_type& a_name,
    const type_function_t a_type,
    void* ap_function);
};

inline bool list_identifier_t::add_func_r_int_a_int(
  const string_type& a_name, func_r_int_a_int_ptr ap_function)
{
  return add_func(a_name, tf_r_int_a_int, reinterpret_cast<void*>(ap_function));
}

inline bool list_identifier_t::add_func_r_float_a_float(
  const string_type& a_name, func_r_float_a_float_ptr ap_function)
{
  return add_func(a_name, tf_r_float_a_float,
    reinterpret_cast<void*>(ap_function));
}

inline void list_identifier_t::constant_add(const pair<string_type,
  value_type>& a_constant_pair)
{
  IRS_LIB_ASSERT(find_function(a_constant_pair.first) == irs::npos);
  IRS_LIB_ASSERT(m_constant_list.find(a_constant_pair.first) ==
    m_constant_list.end());     
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

/*inline void list_identifier_t::array_add(
  const pair<string_type, array_type>& a_array_pair)
{
  IRS_LIB_ASSERT(find_function(a_array_pair.first) == irs::npos);
  IRS_LIB_ASSERT(m_constant_list.find(a_array_pair.first) ==
    m_constant_list.end());
  IRS_LIB_ASSERT(m_array_list.find(a_array_pair.first) ==
    m_array_list.end());
  m_array_list.insert(a_array_pair);
}

inline void list_identifier_t::array_del(
  const string_type& a_array_name)
{
  array_list_iterator it_array =
    m_array_list.find(a_array_name);
  IRS_LIB_ASSERT(it_array != m_array_list.end());
  m_array_list.erase(it_array);
}

inline void list_identifier_t::array_clear()
{
  m_array_list.clear();
}

inline bool list_identifier_t::array_is_exists(
  const string_type& a_array_name) const
{
  return m_array_list.find(a_array_name) != m_array_list.end();
}

inline bool list_identifier_t::array_find(const string_type& a_name,
  const array_type** ap_p_array) const
{
  array_list_const_iterator it_array =
    m_array_list.find (a_name);
  bool array_is_exists = it_array != m_array_list.end();
  if (array_is_exists) {
    *ap_p_array = &it_array->second;
  } else {
    // Константа с таким именем отсутсвует
  }
  return array_is_exists;
}*/

inline bool list_identifier_t::add_func_r_double_a_double(
  const string_type& a_name, func_r_double_a_double_ptr ap_function)
{
  return add_func(a_name, tf_r_double_a_double,
    reinterpret_cast<void*>(ap_function));
}

inline bool list_identifier_t::add_func_r_ldouble_a_ldouble(
  const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function)
{
  return add_func(a_name, tf_r_ldouble_a_ldouble,
    reinterpret_cast<void*>(ap_function));
}

inline const function_t& list_identifier_t::
  get_func(size_type a_id_func) const
{
  return m_func_array[a_id_func];
}

list_identifier_t::list_identifier_t(
):
  m_func_array(),
  m_constant_list()
{ }

inline int list_identifier_t::
  find_function(const string_type& a_function_name) const
{
  int position = irs::npos;
  int function_count = m_func_array.size();
  for (int index_func = 0; index_func < function_count; index_func++) {
    if (m_func_array[index_func].name == a_function_name) {
      position = index_func;
      break;
    }
  }
  return position;
}
inline bool list_identifier_t::add_func(
  const string_type& a_name,
  const type_function_t a_type,
  void* ap_function)
{
  bool fsuccess = true;
  if (find_function(a_name) != irs::npos) {
    fsuccess = false;
  } else if (constant_is_exists(a_name)) {
    fsuccess = false;
  } else {
    function_t function;
    function.name = a_name;
    function.type = a_type;
    function.ptr = ap_function;
    m_func_array.push_back(function);
  }
  return fsuccess;
}

inline bool list_identifier_t::del_func(const string_type& a_name)
{
  bool fsuccess = true;
  int position = find_function(a_name);
  if (position == irs::npos) {
    fsuccess = false;
  } else {
    m_func_array.erase(m_func_array.begin() + position);
  }
  return fsuccess;
}

inline void list_identifier_t::clear_func()
{
  m_func_array.clear();
}

/*
void list_identifier_t::clear_constant()
{
  m_constant_array.clear();
}*/

class token_t
{
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  typedef list_identifier_t::array_type array_type;
  //static const int m_none_id_func = -1;
private:
  token_type_t m_token_type;
  value_type m_num;
  delimiter_t m_delimiter;
  size_type m_id_function;
  const value_type* mp_constant;
  //const array_type* mp_array;
public:
  token_t(
  ):
    m_token_type(tt_none),
    m_num(0),
    m_delimiter(d_none),
    m_id_function(0),
    mp_constant(IRS_NULL)
    //mp_array(IRS_NULL)
  {}
  void set_number(const value_type& a_num)
  {
    m_token_type = tt_number;
    m_num = a_num;
    m_delimiter = d_none;
    m_id_function = 0;
  }
  void set_delimiter(const delimiter_t a_delimiter)
  {
    m_token_type = tt_delimiter;
    m_num = 0;
    m_delimiter = a_delimiter;
    m_id_function = 0;
  }
  void set_id_function(const int a_id_function)
  {
    m_token_type = tt_function;
    m_num = 0;
    m_delimiter = d_none;
    m_id_function = a_id_function;
  }
  void set_constant(const value_type* ap_constant)
  {
    m_token_type = tt_constant;
    m_num = 0;
    m_delimiter = d_none;
    m_id_function = 0;
    mp_constant = ap_constant;
  }
  void set_not_a_token_type()
  {
    m_token_type = tt_none;
    m_num = 0;
    m_delimiter = d_none;
    m_id_function = 0;
  }
  delimiter_t delimiter() const
  {
    return m_delimiter;
  }
  token_type_t token_type_get() const
  {
    return m_token_type;
  }
  bool operator==(const token_type_t a_token_type)
  {
    return (m_token_type == a_token_type);
  }
  bool operator==(const delimiter_t a_delimiter)
  {
    return (m_delimiter == a_delimiter);
  }
  bool operator!=(const delimiter_t a_delimiter)
  {
    return (m_delimiter != a_delimiter);
  }
  size_type get_id_function() const
  {
    return m_id_function;
  }
  const value_type* get_constant() const
  {
    return mp_constant;
  }
  value_type get_number() const
  {
    return m_num;
  }
};

// Детектор лексем
class detector_token_t
{
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  typedef list_identifier_t::array_type array_type;
  detector_token_t(
    list_identifier_t& a_list_identifier):
    m_list_identifier(a_list_identifier),
    m_cur_token_data(),
    mp_prog(IRS_NULL),
    m_prog_pos(0),
    m_exponent_ch('E'),
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
  bool ch_latin_capital = (a_ch >= 'A') && (a_ch <= 'Z');
  // Строчная буква латинского алфавита
  bool ch_latin_small = (a_ch >= 'a') && (a_ch <= 'z');
  // Символ подчеркивания
  bool ch_underscore = (a_ch == '_');

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
  char_type ch = '\0';
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
    char_type next_ch = '\0';
    if ((pos+1) < mp_prog->size()) {
      next_ch_not_end_prog = true;
      next_ch = (*mp_prog)[pos+1];
    } else {
      // Следующий символ отсутсвует
    }
    switch(ch) {
      case '=': {
        if (next_ch_not_end_prog && (next_ch == '=')) {
          m_cur_token_data.token.set_delimiter(d_compare_equal);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case '!': {
        if (next_ch_not_end_prog && (next_ch == '=')) {
          m_cur_token_data.token.set_delimiter(d_compare_not_equal);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case '<': {
        if (next_ch_not_end_prog && (next_ch == '=')) {
          m_cur_token_data.token.set_delimiter(d_compare_less_or_equal);
          detected_token = true;
          pos += 2;
        } else {
          m_cur_token_data.token.set_delimiter(d_compare_less);
          detected_token = true;
          pos++;
        }
      } break;
      case '>': {
        if (next_ch_not_end_prog && (next_ch == '=')) {
          m_cur_token_data.token.set_delimiter(d_compare_greater_or_equal);
          detected_token = true;
          pos += 2;
        } else {
          m_cur_token_data.token.set_delimiter(d_compare_greater);
          detected_token = true;
          pos++;
        }
      } break;
      case '+': {
        m_cur_token_data.token.set_delimiter(d_plus);
        detected_token = true;
        pos++;
      } break;
      case '-': {
        m_cur_token_data.token.set_delimiter(d_minus);
        detected_token = true;
        pos++;
      } break;
      case '*': {
        m_cur_token_data.token.set_delimiter(d_multiply);
        detected_token = true;
        pos++;
      } break;
      case '/': {
        m_cur_token_data.token.set_delimiter(d_division);
        detected_token = true;
        pos++;
      } break;
      case '%': {
        m_cur_token_data.token.set_delimiter(d_integer_division);
        detected_token = true;
        pos++;
      } break;
      case '^': {
        m_cur_token_data.token.set_delimiter(d_involution);
        detected_token = true;
        pos++;
      } break;
      case '(': {
        m_cur_token_data.token.set_delimiter(d_left_parenthesis);
        detected_token = true;
        pos++;
      } break;
      case ')': {
        m_cur_token_data.token.set_delimiter(d_right_parenthesis);
        detected_token = true;
        pos++;
      } break;
      case '[': {
        m_cur_token_data.token.set_delimiter(d_left_square_bracket);
        detected_token = true;
        pos++;
      } break;
      case ']': {
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
      string_type num_str;
      // Запоминаем позицию первого символа числа
      const size_type num_begin_ch = pos;
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
        if ((ch_number == ',') || (ch_number == '.')) {
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
          char_type ch_next = (*mp_prog)[pos+1];
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
        m_cur_token_data.length = pos - num_begin_ch;
        m_cur_token_data.valid = true;
        detected_token = true;
      }
    }
  }

  // Читаем идентификатор
  if (!detected_token) {
    if (ch_is_first_char_name(ch)) {
      size_type pos_begin_name = pos;
      pos = mp_prog->find_first_not_of(m_ch_valid_name, pos);
      if (pos == string_type::npos) {
        pos = mp_prog->size();
      }
      string_type identifier_str =
        mp_prog->substr(pos_begin_name, (pos - pos_begin_name));
      int func_pos = m_list_identifier.find_function(identifier_str);
      if (func_pos != irs::npos) {
        m_cur_token_data.token.set_id_function(func_pos);
        m_cur_token_data.length = pos - pos_begin_name;
        m_cur_token_data.valid = true;
        detected_token = true;
      } else {
        const value_type* p_constant = IRS_NULL; 
        if (m_list_identifier.constant_find(identifier_str, &p_constant)) {
          m_cur_token_data.token.set_constant(p_constant);
          m_cur_token_data.length = pos - pos_begin_name;
          m_cur_token_data.valid = true;
          detected_token = true;
        } else {
          // Констранта с таким именем не найдена
        }
      }
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

inline bool detector_token_t::
  next_token()
{
  bool fsuccess = true;
  if (!m_cur_token_data.valid) {
    fsuccess = detect_token();
  }
  if (fsuccess) {
    if ((m_prog_pos + m_cur_token_data.length) > mp_prog->size()) {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    m_prog_pos += m_cur_token_data.length;
    m_cur_token_data.valid = false;
  }
  return fsuccess;
}

inline bool detector_token_t::
  get_token(token_t* const ap_token)
{
  bool fsuccess = true;
  if (!m_cur_token_data.valid) {
    fsuccess = detect_token();
  }
  if (fsuccess) {
    *ap_token = m_cur_token_data.token;
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
  typedef list_identifier_t::array_type array_type;
  inline calculator_t();
  inline bool calc(const string_type* ap_prog, value_type* ap_num);
  inline int find_function(const string_type& a_function_name) const;
  //int find_constant(const string_type& a_constant_name) const;
  inline bool add_func_r_int_a_int(
    const string_type& a_name, func_r_int_a_int_ptr ap_function);
  inline bool add_func_r_float_a_float(
    const string_type& a_name, func_r_float_a_float_ptr ap_function);
  inline bool add_func_r_double_a_double(
    const string_type& a_name, func_r_double_a_double_ptr ap_function);
  inline bool add_func_r_ldouble_a_ldouble(
    const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function);
  inline void constant_add(const string_type& a_name,
    const value_type& a_value);
  inline bool del_func(const string_type& a_name);
  inline void constant_del(const string_type& a_name);
  inline void clear_func();
  inline void constant_clear();

private:
  list_identifier_t m_list_identifier;
  detector_token_t m_detector_token;
  inline bool interp(value_type* ap_value);
  // Обрабатывает квадратные скобки
  inline bool eval_exp_square_brackets(value_type* a_value);
  // Обрабатывает операции отношения
  inline bool eval_exp_compare(value_type* a_value);
  // Обрабатывает сложение и вычитание
  inline bool eval_exp_arithmetic_leve1(value_type* a_value);
  // Обрабатывает умножение, деление, целочисленное деление
  inline bool eval_exp_arithmetic_leve2(value_type* a_value);
  // Обрабатывает возведение в степень
  inline bool eval_exp_power(value_type* a_value);
  // Обрабатывает скобки
  inline bool eval_exp_brackets(value_type* a_value);
  // Обрабатывает функции и числа
  inline bool atom(value_type* a_value);
  // Выполнение функцию
  inline void func_exec(
    const function_t& a_function,
    const value_type a_in_param,
    value_type* ap_out_param);
  // Чтение элемента массива
  inline bool array_elem_read(const array_type* ap_array,
    const size_type a_elem_index,
    value_type* ap_array_elem) const;
};

inline calculator_t::calculator_t():
  m_list_identifier(),
  m_detector_token(m_list_identifier)
{
  add_func_r_double_a_double("acos", acos);
  add_func_r_double_a_double("asin", asin);
  add_func_r_double_a_double("atan", atan);
  add_func_r_double_a_double("ceil", ceil);
  add_func_r_double_a_double("cos", cos);
  add_func_r_double_a_double("cosh", cosh);
  add_func_r_double_a_double("exp", exp);
  add_func_r_double_a_double("fabs", fabs);
  add_func_r_double_a_double("floor", floor);
  add_func_r_double_a_double("ln", log);
  add_func_r_double_a_double("log", log10);
  add_func_r_double_a_double("sin", sin);
  add_func_r_double_a_double("sinh", sinh);
  add_func_r_double_a_double("sqrt", sqrt);
  add_func_r_double_a_double("tan", tan);
  add_func_r_double_a_double("tanh", tanh);

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
  bool fsuccess = eval_exp_compare(&value);
  if (fsuccess) {
    *ap_value = value;
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
        fsuccess = eval_exp_compare(ap_value);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        if (token == d_right_square_bracket) {
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

inline bool calculator_t::eval_exp_compare(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_arithmetic_leve1(ap_value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    if (fsuccess) {
      delimiter_t delim = token.delimiter();
      if ((delim >= d_compare_equal) && (delim <= d_compare_greater_or_equal)) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_arithmetic_leve1(&partial_value);
        } else {
          // Произошла ошибка
        }
      } else {
        // Лексема не является операцией сравнения
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
            // Лексема не является операцией сравнения
          }
        }
      } else {
        // Произошла ошибка
      }
    } else {
      // Произошла ошибка
    }
  } else {
    // Произошла ошибка
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_arithmetic_leve1(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_arithmetic_leve2(ap_value);
  if (fsuccess) {
    //token_t token = m_lexeme_array[m_cur_lexeme_index];
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    delimiter_t delim = token.delimiter();
    if (fsuccess) {
      while(((delim == d_plus) || (delim == d_minus)) && fsuccess) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_arithmetic_leve2(&partial_value);
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
        delim = token.delimiter();
      }
    } else {
      // Произошла ошибка при извлечении лексемы
    }
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_arithmetic_leve2(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_power(ap_value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    delimiter_t delim = token.delimiter();
    if (fsuccess) {
      while(fsuccess && (delim >= d_multiply) && (delim <= d_integer_division))
      {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_power(&partial_value);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          if (token == d_multiply) {
            *ap_value = *ap_value * partial_value;
          } else if (token == d_division) {
            if (partial_value != 0) {
              *ap_value = *ap_value / partial_value;
            } else {
              fsuccess = false;
            }
          } else if (token == d_integer_division) {
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

inline bool calculator_t::eval_exp_power(value_type* ap_value)
{
  bool fsuccess = true;
  value_type partial_value = 0;
  fsuccess = eval_exp_brackets(ap_value);
  if (fsuccess) {
    token_t token;
    fsuccess = m_detector_token.get_token(&token);
    if (fsuccess) {
      while(fsuccess && (token == d_involution)) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_brackets(&partial_value);
        }
        if (fsuccess) {
          if (token == d_involution) {
            *ap_value = pow(*ap_value, static_cast<double>(partial_value));
          }
          fsuccess = m_detector_token.get_token(&token);
        }
      }
    }
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
        fsuccess = eval_exp_compare(ap_value);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        if (token == d_right_parenthesis) {
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
    if (token == tt_function) {
      const int id_func = token.get_id_function();
      fsuccess = m_detector_token.next_token();
      value_type arg_func = 0;
      if (fsuccess) {
        fsuccess = eval_exp_power(&arg_func);
      } else {
        // Произошла ошибка
      }
      /*if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }*/
      if (fsuccess) {
        func_exec(m_list_identifier.get_func(id_func),
          arg_func, ap_value);
      } else {
        // Произошла ошибка
      }
    } else if (token == tt_constant) {
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
      /*if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        if (token.token_type_get() != tt_delimiter) {
          fsuccess = false;
        } else {
          if (token.delimiter() != d_left_square_bracket) {
            fsuccess = false;
          } else {
            // Следующая лексема является квадратной скобкой
          }
        }
      } else {
        // Произошла ошибка
      }*/
      /*value_type elem_index;
      if (fsuccess) {

        fsuccess = eval_exp_square_brackets(&elem_index);
      } else {
        // Произошла ошибка
      }*/
      /*if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }*/
      if (fsuccess) {
        *ap_value = *p_constant;
      } else {
        // Произошла ошибка
      }
    } else if (token == tt_number) {
      *ap_value = token.get_number();
      fsuccess = m_detector_token.next_token();
    } else if (token == tt_delimiter) {
      if (token == d_plus) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = atom(ap_value);
        } else {
          // Произошла ошибка
        }
      } else if (token == d_minus) {
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

inline void calculator_t::func_exec(
  const function_t& a_function,
  const value_type a_in_param,
  value_type* ap_out_param)
{
  void* ptr_function = a_function.ptr;
  switch(a_function.type) {
    case tf_r_int_a_int: {
      *ap_out_param =
        (*((func_r_int_a_int_ptr)(ptr_function)))(a_in_param);
    } break;
    case tf_r_float_a_float: {
      *ap_out_param =
        (*((func_r_float_a_float_ptr)(ptr_function)))(a_in_param);
    } break;
    case tf_r_double_a_double: {
      *ap_out_param =
        (*((func_r_double_a_double_ptr)(ptr_function)))(a_in_param);
    } break;
    case tf_r_ldouble_a_ldouble: {
      *ap_out_param =
        (*((func_r_ldouble_a_ldouble_ptr)(ptr_function)))(a_in_param);
    } break;
  }
}

inline bool calculator_t::array_elem_read(const array_type* ap_array,
  const size_type a_elem_index,
  value_type* ap_array_elem) const
{
  bool fsuccess = true;
  if (a_elem_index < ap_array->size()) {
    *ap_array_elem = (*ap_array)[a_elem_index];
  } else {
    fsuccess = false;
  }
  return fsuccess;
}

inline bool calculator_t::
  calc(const string_type* ap_prog, value_type* ap_num)
{
  m_detector_token.set_prog(ap_prog);
  return interp(ap_num);
}

inline int calculator_t::find_function(
  const string_type& a_function_name) const
{
  return m_list_identifier.find_function(a_function_name);
}

/*inline int calculator_t::find_constant(
  const string_type& a_constant_name) const
{
  return m_list_identifier.find_constant(a_constant_name);
}*/

inline bool calculator_t::add_func_r_int_a_int(
  const string_type& a_name, func_r_int_a_int_ptr ap_function)
{
  return m_list_identifier.add_func_r_int_a_int(a_name, ap_function);
}

inline bool calculator_t::add_func_r_float_a_float(
  const string_type& a_name, func_r_float_a_float_ptr ap_function)
{
  return m_list_identifier.add_func_r_float_a_float(a_name, ap_function);
}

inline bool calculator_t::add_func_r_double_a_double(
  const string_type& a_name, func_r_double_a_double_ptr ap_function)
{
  return m_list_identifier.add_func_r_double_a_double(a_name, ap_function);
}

inline bool calculator_t::add_func_r_ldouble_a_ldouble(
  const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function)
{
  return m_list_identifier.add_func_r_ldouble_a_ldouble(a_name, ap_function);
}

inline void calculator_t::constant_add(
  const string_type& a_name, const value_type& a_value)
{
  m_list_identifier.constant_add(make_pair(a_name, a_value));
}

inline bool calculator_t::del_func(const string_type& a_name)
{
  return m_list_identifier.del_func(a_name);
}

inline void calculator_t::constant_del(const string_type& a_name)
{
  m_list_identifier.constant_del(a_name);
}

inline void calculator_t::clear_func()
{
  m_list_identifier.clear_func();
}

inline void calculator_t::constant_clear()
{
  m_list_identifier.constant_clear();
}

/*inline void calculator_t::array_add(const string_type& a_name,
  const array_type& a_array)
{
  m_list_identifier.array_add(make_pair(a_name, a_array));
}

inline void calculator_t::array_del(const string_type& a_name)
{
  m_list_identifier.array_del(a_name);
}

inline void calculator_t::array_clear()
{
  m_list_identifier.array_clear();
}*/

}; // namespace calc

}; // namespace irs

#endif //__WATCOMC__

#endif //irscalcH
