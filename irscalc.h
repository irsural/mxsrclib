// Калькулятор
// Дата: 24.10.2009
// Дата создания: 1.02.2009

#ifndef irscalcH
#define irscalcH

// В Watcom C++ есть только не-const версия функции map::find
#ifndef __WATCOMC__

// В GNU C++ версии >= 3.4 строки char* имеют кодировку UTF-8
// Русские символы в этой кодировке (например 'Я') имеют размер 2 байта,
// что недопустимо по стандарту C++
//#if defined(IRS_UNICODE) || defined(IRS_GNUC_VERSION_LESS_3_4)

#include <iostream>
#include <irsstdg.h>
#include <irserror.h>
#include <math.h>
#include <irsvariant.h>
#include <irscpp.h>
#include <irslocale.h>

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

//typedef int (*func_r_int_a_int_ptr)(int);
//typedef float (*func_r_float_a_float_ptr)(float);
//typedef double (*func_r_double_a_double_ptr)(double);
//typedef long double (*func_r_ldouble_a_ldouble_ptr)(long double);

enum token_type_t {
  tt_none,
  tt_number,
  tt_delimiter,
  tt_constant,
  tt_function,
  tt_identifier
};
// Список идентификаторов

enum delimiter_t {
  d_none,
  d_assign,
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
  d_dot,
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
  d_end
};

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
  keyword_xor_eq
};

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
  virtual bool exec(variant::variant_t* ap_parameters,
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
  inline virtual bool exec(variant::variant_t* ap_parameters,
    variant::variant_t* ap_returned_parameter) const
  {
    IRS_LIB_ASSERT(ap_parameters->type() == variant::var_type_array);
    bool fsuccess = true;
    if (ap_parameters->size() == 1) {
      *ap_returned_parameter = (*mp_func)((*ap_parameters)[0].as_int());
    } else {
      // Недопустимое количество параметров
      fsuccess = false;
    }
    return fsuccess;
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
  inline virtual bool exec(variant::variant_t* ap_parameters,
    variant::variant_t* ap_returned_parameter) const
  {
    IRS_LIB_ASSERT(ap_parameters->type() == variant::var_type_array);
    bool fsuccess = true;
    if (ap_parameters->size() == 1) {
      *ap_returned_parameter = (*mp_func)((*ap_parameters)[0].as_double());
    } else {
      // Недопустимое количество параметров
      fsuccess = false;
    }
    return fsuccess;
  }
private:
  func_r_double_a_double_ptr_type mp_func;
  func_r_double_a_double_t();
};

class pow_t: public function_t
{
public:
  inline virtual bool exec(variant::variant_t* ap_parameters,
    variant::variant_t* ap_returned_parameter) const
  {
    IRS_LIB_ASSERT(ap_parameters->type() == variant::var_type_array);
    bool fsuccess = true;
    if (ap_parameters->size() == 2) {
      *ap_returned_parameter = pow((*ap_parameters)[0].as_double(),
        (*ap_parameters)[1].as_double());
    } else {
      // Недопустимое количество параметров
      fsuccess = false;
    }
    return fsuccess;
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
  //vector<function_t> m_func_array;
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
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;

  token_t(): m_tok(), m_token_type(tt_none)
  { }
  token_t(const token_t& a_token): m_tok(), m_token_type(tt_none)
  {
    assign(a_token);
  }
  ~token_t()
  {
    type_change(tt_none);
  }
  inline void type_change(const token_type_t a_token_type)
  {
    if (m_token_type != a_token_type) {
      if (m_token_type == tt_number) {
        delete m_tok.p_num;
      } else if (m_token_type == tt_identifier) {
        delete m_tok.p_identifier;
      } else {
        // Другие типы не требуют освобождения памяти
      }
      if (a_token_type == tt_number) {
        m_tok.p_num = new value_type;
      } else if (a_token_type == tt_identifier) {
        m_tok.p_identifier = new string_type;
      } else {
        // Другие типы не требуют динамического выделения памяти
      }
    } else {
      // Не требуется изменение типа
    }
    m_token_type = a_token_type;
  }
  inline token_t& operator=(const token_t& a_token)
  {
    assign(a_token);
    return *this;
  }
  inline void set_number(const value_type& a_num)
  {
    type_change(tt_number);
    *m_tok.p_num = a_num;
  }
  inline void set_delimiter(const delimiter_t a_delimiter)
  {
    type_change(tt_delimiter);
    m_tok.delimiter = a_delimiter;
  }
  inline void set_identifier(const string_type& a_identifier)
  {
    type_change(tt_identifier);
    *m_tok.p_identifier = a_identifier;
  }
  inline void set_function(const function_t* ap_function)
  {
    type_change(tt_function);
    m_tok.p_function = ap_function;
  }
  inline void set_constant(const value_type* ap_constant)
  {
    type_change(tt_constant);
    m_tok.p_constant = ap_constant;
  }
  inline delimiter_t delimiter() const
  {
    delimiter_t delimiter = d_none;
    if (m_token_type == tt_delimiter) {
      delimiter = m_tok.delimiter;
    } else {
      // Текущая лексема не является ограничителем
    }
    return delimiter;
  }
  inline token_type_t token_type() const
  {
    return m_token_type;
  }
  inline const function_t* get_function() const
  {
    const function_t* p_function = IRS_NULL;
    if (m_token_type == tt_function) {
      p_function = m_tok.p_function;
    } else {
      // Текущая лексема не является функцией
    }
    return p_function;
  }
  inline const string_type& get_identifier() const
  {
    IRS_LIB_ASSERT(m_token_type == tt_identifier);
    return *m_tok.p_identifier;
  }
  inline const value_type* get_constant() const
  {
    const value_type* p_constant = IRS_NULL;
    if (m_token_type == tt_constant) {
      p_constant = m_tok.p_constant;
    } else {
      // Текущая лексема не является константой
    }
    return p_constant;
  }
  inline value_type get_number() const
  {
    IRS_LIB_ASSERT(m_token_type == tt_number);
    /*value_type num;
    num.type(variant::var_type_unknown);
    if (m_token_type == tt_number) {
      num = (*m_tok.p_num);
    } else {
      // Текущая константа не является числом
    }*/
    return *m_tok.p_num;
  }
  inline bool is_operator_logical()
  {
    bool operator_logical_status = false;
    if (m_token_type == tt_delimiter) {
      if ((m_tok.delimiter >= d_and) && (m_tok.delimiter <= d_not)) {
        operator_logical_status = true;
      } else {
        // Это не логический оператор
      }
    } else {
      // Это не ограничитель
    }
    return operator_logical_status;
  }
  inline bool is_operator_compare()
  {
    bool operator_compare_status = false;
    if (m_token_type == tt_delimiter) {
      if ((m_tok.delimiter >= d_compare_equal) &&
        (m_tok.delimiter <= d_compare_greater_or_equal))
      {
        operator_compare_status = true;
      } else {
        // Это не оператор сравнения
      }
    } else {
      // Это не ограничитель
    }
    return operator_compare_status;
  }
private:
  union {
    value_type* p_num;
    delimiter_t delimiter;
    const value_type* p_constant;
    const function_t* p_function;
    string_type* p_identifier;
  } m_tok;
  token_type_t m_token_type;
  inline void assign(const token_t& a_token)
  {
    type_change(a_token.m_token_type);
    switch (m_token_type) {
      case tt_number: {
        *m_tok.p_num = *a_token.m_tok.p_num;
      } break;
      case tt_delimiter: {
        m_tok.delimiter = a_token.m_tok.delimiter;
      } break;
      case tt_constant: {
        m_tok.p_constant = a_token.m_tok.p_constant;
      } break;
      case tt_function: {
        m_tok.p_function = a_token.m_tok.p_function;
      } break;
      case tt_identifier: {
        *m_tok.p_identifier = *a_token.m_tok.p_identifier;
      } break;
      case tt_none: {
        // Нет элементов для копирования
      } break;
      default : {
        IRS_LIB_ASSERT_MSG("Неучтенный тип");
      }
    }
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
  typedef stringns_t::iterator string_iterator;
  typedef stringns_t::const_iterator string_const_iterator;
  struct token_data_t
  {
    token_t token;
    size_type length;
    //bool valid;
    token_data_t():
      token(),
      length(0)/*,
      valid(false)*/
    {}
  };
  typedef deque<token_data_t> token_list_type;
  typedef deque<token_data_t>::iterator token_iterator;
  typedef deque<token_data_t>::const_iterator token_const_iterator;
  detector_token_t(
    list_identifier_t& a_list_identifier):
    m_list_identifier(a_list_identifier),
    m_token_data_list(),
    m_cur_token_data_it(m_token_data_list.end()),
    mp_prog(IRS_NULL),
    m_prog_pos(0),
    m_ch_valid_name(irst("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"))
  { }
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  inline void imbue(const locale& a_locale);
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  // Установить указатель на текст программы
  inline void set_prog(const string_type* ap_prog);
  inline bool next_token();
  inline void back_token();
  //bool back_token();
  inline bool get_token(token_t* const ap_token);
private:
  // Список идентификаторов
  const list_identifier_t& m_list_identifier;
  //token_data_t m_cur_token_data;
  // Список лексем
  token_list_type m_token_data_list;
  token_iterator m_cur_token_data_it;
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  locale m_locale;
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  // Указатель на текст программы
  const string_type* mp_prog;
  // Текущая позиция в тексте программы
  size_type m_prog_pos;
  // Строка допустимых символов в имени идентификатора
  const string_type m_ch_valid_name;
  // Проверка символа на принадлежность к символу числа
  inline bool is_char_digit(const char_type a_ch);
  // Проверка на символ экспоненты
  inline bool is_char_exponent(const char_type a_ch);
  // Проверка символа на принадлежность букве
  inline bool is_char_alpha(const char_type a_ch);
  // Идентифицировать лексему в текущей позиции программы
  inline bool detect_token(const string_type* ap_prog,
    const size_type a_prog_pos, token_data_t* ap_token_data);
};

inline bool detector_token_t::is_char_digit(const char_type a_ch)
{
  return isdigitt(a_ch);
}

inline bool detector_token_t::is_char_exponent(const char_type a_ch)
{
  return (a_ch == irst('e')) || (a_ch == irst('E'));
}

inline bool detector_token_t::is_char_alpha(const char_type a_ch)
{
  /*bool is_ch_alpha = false;
  if (isalphat(a_ch)) {
    is_ch_alpha = true;
  } else {
    // Заглавная буква русского алфавита
    bool is_ch_russian_capital = (a_ch >= irst('А')) && (a_ch <= irst('Я'));
    // Строчная буква русского алфавита
    bool is_ch_russian_small = (a_ch >= irst('а'));
    if (is_ch_russian_capital || is_ch_russian_small) {
      is_ch_alpha = true;
    } else {
      // Символ не является буквой латинского и русского алфавитов
    }
  }*/
  return (isalnumt(a_ch) || a_ch == '_');
}

inline bool detector_token_t::detect_token(const string_type* ap_prog,
  const size_type a_prog_pos, token_data_t* ap_token_data)
{
  bool detected_token = false;
  //bool fsuccess = true;
  if (a_prog_pos == ap_prog->size()) {
    ap_token_data->token.set_delimiter(d_end);
    ap_token_data->length = 0;
    //ap_token_data.valid = true;
    detected_token = true;
  }
  size_type pos;
  char_type ch = irst('\0');
  if (!detected_token) {
    // Пропускаем пробелы, табуляции и переходы на новую строку
    pos = ap_prog->find_first_not_of(irst(" \n\r\t"), a_prog_pos);
    if (pos == string_type::npos) {
      // Достигнут конец программы
      //a_prog_pos = ap_prog->size();
      ap_token_data->token.set_delimiter(d_end);
      ap_token_data->length = 0;
      //ap_token_data.valid = true;
      detected_token = true;
    } else {
      ch = (*ap_prog)[pos];
    }
  }

  if (!detected_token) {
    // Читаем ограничитель
    bool next_ch_not_end_prog = false;
    const size_type begin_ch_pos = pos;
    char_type next_ch = irst('\0');
    if ((pos + 1) < ap_prog->size()) {
      next_ch_not_end_prog = true;
      next_ch = (*ap_prog)[pos + 1];
    } else {
      // Следующий символ отсутсвует
    }
    switch(ch) {
      case irst('&'): {
        if (next_ch_not_end_prog && (next_ch == irst('&'))) {
          ap_token_data->token.set_delimiter(d_and);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case irst('|'): {
        if (next_ch_not_end_prog && (next_ch == irst('|'))) {
          ap_token_data->token.set_delimiter(d_or);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case irst('='): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          ap_token_data->token.set_delimiter(d_compare_equal);
          detected_token = true;
          pos += 2;
        } else {
          // Не удалось распознать ограничитель
        }
      } break;
      case irst('!'): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          ap_token_data->token.set_delimiter(d_compare_not_equal);
          detected_token = true;
          pos += 2;
        } else {
          ap_token_data->token.set_delimiter(d_not);
          detected_token = true;
          pos ++;
        }
      } break;
      case irst('<'): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          ap_token_data->token.set_delimiter(d_compare_less_or_equal);
          detected_token = true;
          pos += 2;
        } else {
          ap_token_data->token.set_delimiter(d_compare_less);
          detected_token = true;
          pos++;
        }
      } break;
      case irst('>'): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          ap_token_data->token.set_delimiter(d_compare_greater_or_equal);
          detected_token = true;
          pos += 2;
        } else {
          ap_token_data->token.set_delimiter(d_compare_greater);
          detected_token = true;
          pos++;
        }
      } break;
      case irst('+'): {
        ap_token_data->token.set_delimiter(d_plus);
        detected_token = true;
        pos++;
      } break;
      case irst('-'): {
        ap_token_data->token.set_delimiter(d_minus);
        detected_token = true;
        pos++;
      } break;
      case irst('*'): {
        ap_token_data->token.set_delimiter(d_multiply);
        detected_token = true;
        pos++;
      } break;
      case irst('/'): {
        ap_token_data->token.set_delimiter(d_division);
        detected_token = true;
        pos++;
      } break;
      case irst('%'): {
        ap_token_data->token.set_delimiter(d_integer_division);
        detected_token = true;
        pos++;
      } break;
      case irst('('): {
        ap_token_data->token.set_delimiter(d_left_parenthesis);
        detected_token = true;
        pos++;
      } break;
      case irst(')'): {
        ap_token_data->token.set_delimiter(d_right_parenthesis);
        detected_token = true;
        pos++;
      } break;
      case irst('['): {
        ap_token_data->token.set_delimiter(d_left_square_bracket);
        detected_token = true;
        pos++;
      } break;
      case irst(']'): {
        ap_token_data->token.set_delimiter(d_right_square_bracket);
        detected_token = true;
        pos++;
      } break;
      case irst(','): {
        ap_token_data->token.set_delimiter(d_comma);
        detected_token = true;
        pos++;
      } break;
      case irst('.'): {
        ap_token_data->token.set_delimiter(d_dot);
        detected_token = true;
        pos++;
      } break;
      default : {
        detected_token = false;
      }
    }
    if (detected_token) {
      ap_token_data->length = pos - a_prog_pos;
      //ap_token_data->valid = true;
    } else {
      pos = begin_ch_pos;
    }
  }

  if (!detected_token) {
    // Читаем число
    if (is_char_digit(ch)) {
      // Запоминаем позицию первого символа числа
      const size_type num_begin_ch = pos;
      string_type num_str;
      pos++;
      // Статус обнаружения символа показателя степени
      bool detected_exponent_ch = false;
      // Статус обнаружения символа - разделителя целой и дробной части
      bool detected_decimal_point = false;
      // Статус обнаружения знака показателя степени
      bool detected_sign_exponent_ch = false;
      // Статус обнаружения суффикса unsigned
      bool detected_suffix_unsigned = false;
      // Статус обнаружения суффикса long
      bool detected_suffix_long = false;
      // Статус обнаружения суффикса float
      bool detected_suffix_float = false;
      while (pos <  ap_prog->size()) {
        // Ищем первый символ, не являющийся цифрой
        pos = ap_prog->find_first_not_of(irst("0123456789"), pos);

        if (pos == string_type::npos) {
          pos = ap_prog->size();
          break;
        } else {
          // Найден символ, не являющийся цифрой
        }
        const char_type ch_number = (*ap_prog)[pos];
        if (ch_number == irst('.')) {
          if ((!detected_exponent_ch) && (!detected_decimal_point) &&
            (!detected_sign_exponent_ch))
          {
            detected_decimal_point = true;
            pos++;
          } else {
            break;
          }
        } else if (is_char_exponent(ch_number)) {
          if (detected_exponent_ch) {
            break;
          } else {
            detected_exponent_ch = true;
            pos++;
          }
        } else if (strchrt(irst("+-"), ch_number)) {
          if (detected_exponent_ch && !detected_sign_exponent_ch) {
            detected_sign_exponent_ch = true;
            pos++;
          } else {
            break;
          }
        } else if (strchrt(irst("Uu"), ch_number)) {
          if (!detected_suffix_unsigned &&
            !detected_suffix_float &&
            !detected_decimal_point &&
            !detected_exponent_ch)
          {
            detected_suffix_unsigned = true;
            pos++;
          } else {
            break;
          }
        } else if (strchrt(irst("Ll"), ch_number)) {
          if (!detected_suffix_long && !detected_suffix_float) {
            detected_suffix_long = true;
            pos++;
          } else {
            break;
          }
        } else if (strchrt(irst("Ff"), ch_number)) {
          if ((detected_decimal_point || detected_exponent_ch) &&
            !detected_suffix_float && !detected_suffix_long) {
            detected_suffix_float = true;
            pos++;
          } else {
            break;
          }
        } else {
          break;
        }
      }
      num_str = ap_prog->substr(num_begin_ch, (pos - num_begin_ch));
      bool convert_str_to_number_success = false;
      if (!detected_decimal_point &&
        !detected_exponent_ch &&
        !detected_suffix_float)
      {
        // Значит число целое
        if (detected_suffix_long) {
          if (detected_suffix_unsigned) {
            unsigned long number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              ap_token_data->token.set_number(number);
            } else {
              // Преобразование в число прошло неудачно
            }
          } else {
            long number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              ap_token_data->token.set_number(number);
            } else {
              // Преобразование в число прошло неудачно
            }
          }
        } else {
          if (detected_suffix_unsigned) {
            unsigned int number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              ap_token_data->token.set_number(number);
            } else {
              // Преобразование в число прошло неудачно
            }
          } else {
            int number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              ap_token_data->token.set_number(number);
            } else {
              // Преобразование в число прошло неудачно
            }
          }
        }
      } else {
        // Значит число с плавающей точкой
        if (detected_suffix_long) {
          long double number = 0.;
          convert_str_to_number_success = num_str.to_number(number);
          if (convert_str_to_number_success) {
            ap_token_data->token.set_number(number);
          } else {
            // Преобразование в число прошло неудачно
          }
        } else if (detected_suffix_float) {
          float number = 0.f;
          convert_str_to_number_success = num_str.to_number(number);
          if (convert_str_to_number_success) {
            ap_token_data->token.set_number(number);
          } else {
            // Преобразование в число прошло неудачно
          }
        } else {
          double number = 0.;
          convert_str_to_number_success = num_str.to_number(number);
          if (convert_str_to_number_success) {
            ap_token_data->token.set_number(number);
          } else {
            // Преобразование в число прошло неудачно
          }
        }
      }

      if (convert_str_to_number_success) {
        ap_token_data->length = pos - a_prog_pos;
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
    if (is_char_alpha(ch)) {
      const size_type pos_begin_name = pos;
      pos = ap_prog->find_first_not_of(m_ch_valid_name, pos);
      if (pos == string_type::npos) {
        pos = ap_prog->size();
      } else {
        // Символ, не принадлежащий группе символов, допустимых в имени,
        // не найден
      }
      string_type identifier_str =
        ap_prog->substr(pos_begin_name, (pos - pos_begin_name));
      detected_token = true;
      if (identifier_str == irst("and")) {
        ap_token_data->token.set_delimiter(d_and);
        //detected_token = true;
      } else if (identifier_str == irst("or")) {
        ap_token_data->token.set_delimiter(d_or);
        //detected_token = true;
      } else if (identifier_str == irst("not")) {
        ap_token_data->token.set_delimiter(d_not);
        //detected_token = true;
      } else {
        ap_token_data->token.set_identifier(identifier_str);
        //detected_token = true;
        /*const function_t* p_function = IRS_NULL;
        if (m_list_identifier.function_find(identifier_str, &p_function)) {
          ap_token_data->token.set_function(p_function);
          detected_token = true;
        } else {
          const value_type* p_constant = IRS_NULL;
          if (m_list_identifier.constant_find(identifier_str, &p_constant)) {
            ap_token_data->token.set_constant(p_constant);
            detected_token = true;
          } else {
            // Констранта с таким именем не найдена
          }
        }*/
      }
      if (detected_token) {
        ap_token_data->length = pos - a_prog_pos;

      } else {
        pos = pos_begin_name;
      }
    } else {
      // Символ не принадлежит группе символов, допустимых в имени
    }
  }
  return detected_token;
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
inline void detector_token_t::imbue(const locale& a_locale)
{
  m_locale = a_locale;
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

inline void detector_token_t::set_prog(const string_type* ap_prog)
{
  mp_prog = ap_prog;
  m_prog_pos = 0;
  //m_cur_token_data.valid = false;
  m_token_data_list.clear();
  m_cur_token_data_it = m_token_data_list.end();
}

inline bool detector_token_t::next_token()
{
  bool fsuccess = true;
  if (m_cur_token_data_it != m_token_data_list.end()) {
    m_prog_pos += m_cur_token_data_it->length;
    m_cur_token_data_it++;
  } else {
    // Сдвиг текущей точки программы не требуется
  }
  if (m_cur_token_data_it == m_token_data_list.end()) {
    if (m_prog_pos <= mp_prog->size()) {
      token_data_t token_data;
      fsuccess = detect_token(mp_prog, m_prog_pos, &token_data);
      if (fsuccess) {
        m_token_data_list.push_back(token_data);
        m_cur_token_data_it = m_token_data_list.end()-1;
      } else {
        // Детектировать лексему не удалось
      }
    } else {
      fsuccess = false;
      IRS_LIB_ASSERT_MSG("Ошибка при перемещении к следующему сиволу");
    }
  } else {
    // Следующая лексема уже детектирована
  }

  #ifdef NOP
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
  #endif // NOP
  return fsuccess;
}

inline void detector_token_t::back_token()
{
  if (m_cur_token_data_it != m_token_data_list.begin()) {
    m_cur_token_data_it--;
    m_prog_pos -= m_cur_token_data_it->length;
  } else {
    IRS_LIB_ASSERT_MSG("Ошибка при перемещении к предыдущей лексеме");
  }
}

inline bool detector_token_t::get_token(token_t* const ap_token)
{
  bool fsuccess = true;  
  if (m_cur_token_data_it == m_token_data_list.end()) {
    token_data_t token_data;
    fsuccess = detect_token(mp_prog, m_prog_pos, &token_data);
    if (fsuccess) {
      m_token_data_list.push_back(token_data);
      m_cur_token_data_it = m_token_data_list.end()-1;
    } else {
      // Детектировать лексему не удалось
    }
  } else {
    // Лексема уже детектирована
  }
  if (fsuccess) {
    *ap_token = m_cur_token_data_it->token;
  } else {
    // Произошла ошибка
  }
  #ifdef NOP
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
  #endif // NOP
  return fsuccess;
}

typedef vector<pair<stringns_t, vector<int> > > id_constant_type;

enum part_id_type_t {part_id_type_name, part_id_type_index};
struct part_id_variable_t
{
  part_id_type_t type;
  irs::variant::variant_t part_id;
};
typedef vector<part_id_variable_t> id_variable_type;
typedef vector<part_id_variable_t>::iterator part_id_variable_iterator;
typedef vector<part_id_variable_t>::const_iterator
  part_id_variable_const_iterator;

bool part_id_name_get(
  const part_id_variable_const_iterator a_begin_part_id_it,
  const part_id_variable_const_iterator a_end_part_id_it,
  stringns_t* ap_name);

bool part_id_index_get(
  const part_id_variable_const_iterator a_begin_part_id_it,
  const part_id_variable_const_iterator a_end_part_id_it,
  irs::variant::variant_t* ap_index);

/*typedef vector<pair<stringns_t, vector<int> > > id_variable_type;
typedef vector<pair<stringns_t, vector<int> > >::iterator
  id_variable_part_name_iterator;
typedef vector<pair<stringns_t, vector<int> > >::const_iterator
  id_variable_part_name_const_iterator;
*/
class handle_external_constant_t
{
public:
  typedef stringns_t string_type;
  typedef vector<pair<string_type, vector<int> > > id_constant_type;
  typedef variant::variant_t variant_type;
  virtual ~handle_external_constant_t();
  virtual bool get(const id_constant_type&, variant_type*) const = 0;
};

class calculator_t
{
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  //typedef mutable_ref_t mutable_ref_type;
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
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  inline void imbue(const locale& a_locale);
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  inline void handle_external_constant_set(
    const handle_external_constant_t* ap_handle_extern_const);
private:
  list_identifier_t m_list_identifier;
  detector_token_t m_detector_token;
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  locale m_locale;
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  const handle_external_constant_t* mp_handle_extern_const;
  inline bool interp(value_type* ap_value);
  // Обрабатывает операцию присвоения
  //inline bool eval_exp_assign(value_type* ap_value);
  // Обрабатывает квадратные скобки
  inline bool eval_exp_square_brackets(value_type* ap_value);
  inline bool eval_exp_constant(id_constant_type* ap_id_constant);
  // Обрабатывает скобки функции и выражение, расположенное внутри них
  inline bool eval_exp_arg_function(value_type* ap_value);
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
  inline bool constant_value_get(const id_constant_type& a_id_constant,
    value_type* ap_value);
  // Выполнение функцию
  inline void func_exec(
    const function_t& a_function,
    const value_type a_in_param,
    value_type* ap_out_param);
};

inline calculator_t::calculator_t():
  m_list_identifier(),
  m_detector_token(m_list_identifier),
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  m_locale(irs::loc().get()),
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  mp_handle_extern_const(IRS_NULL)
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
  function_add(irst("pow"), new pow_t());

  constant_add(irst("IRS_E"), IRS_E);
  constant_add(irst("IRS_LOG2E"), IRS_LOG2E);
  constant_add(irst("IRS_LOG10E"), IRS_LOG10E);
  constant_add(irst("IRS_LN2"), IRS_LN2);
  constant_add(irst("IRS_LN10"), IRS_LN10);
  constant_add(irst("IRS_PI"), IRS_PI);
  constant_add(irst("IRS_PI_2"), IRS_PI_2);
  constant_add(irst("IRS_PI_4"), IRS_PI_4);
  constant_add(irst("IRS_1_PI"), IRS_1_PI);
  constant_add(irst("IRS_2_PI"), IRS_2_PI);
  constant_add(irst("IRS_1_SQRTPI"), IRS_1_SQRTPI);
  constant_add(irst("IRS_2_SQRTPI"), IRS_2_SQRTPI);
  constant_add(irst("IRS_SQRT2"), IRS_SQRT2);
  constant_add(irst("IRS_SQRT_2"), IRS_SQRT_2);

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
  constant_add(irst("arr"), arr);
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
      delimiter_t delim = token.delimiter();
      if (delim == d_end) {
        *ap_value = value;
      } else {
        // Текущим ограничителем должен быть конец программы
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
        if (token.token_type() == tt_delimiter) {
          if (token.delimiter() == d_right_square_bracket) {
            // Закрывающая квадратная скобка присутсвует
            fsuccess = m_detector_token.next_token();
          } else {
            fsuccess = false;
          }
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

inline bool calculator_t::eval_exp_constant(
  vector<pair<string_type, vector<int> > >* ap_id_constant)
{
  bool fsuccess = true;
  ap_id_constant->clear();
  enum last_token_type_t {
    ltt_unknown,
    ltt_identifier,
    ltt_dot,
    ltt_right_square_bracket
  } last_token_type = ltt_unknown;

  token_t token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    if (token.token_type() == tt_identifier) {
      last_token_type = ltt_identifier;
      ap_id_constant->push_back(make_pair(token.get_identifier(),
        vector<int>()));
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      bool is_token_part_identifier = false;
      if (fsuccess) {                       
        if (token.token_type() == tt_identifier) {
          is_token_part_identifier = true;
        } else if (token.token_type() == tt_delimiter) {
          delimiter_t delim = token.delimiter();
          if ((delim == d_dot) ||
            (delim == d_left_square_bracket) ||
            (delim == d_right_square_bracket))
          {
            is_token_part_identifier = true;
          } else {
            // Лексема не является частью идентификатора
          }
        } else {
          // Лексема не является частью идентификатора
        }
      } else {
        // Произошла ошибка
      }
      
      while (fsuccess && is_token_part_identifier) {
        switch (token.token_type()) {
          case tt_identifier: {
            if (last_token_type == ltt_dot) {
              last_token_type = ltt_identifier;
              ap_id_constant->push_back(
                make_pair(token.get_identifier(), vector<int>()));
            } else {
              fsuccess = false;
            }
            is_token_part_identifier = true;
          } break;
          case tt_delimiter: {
            delimiter_t delim = token.delimiter();
            if (delim == d_dot) {
              if ((last_token_type != ltt_identifier) &&
                (last_token_type != ltt_right_square_bracket))
              {
                fsuccess = false;
              } else {
                // Игнорируем точку
                last_token_type = ltt_dot;
              }
              is_token_part_identifier = true;
            } else if (delim == d_left_square_bracket) {
              fsuccess = m_detector_token.next_token();
              value_type index_value;
              if (fsuccess) {        
                fsuccess = eval_exp_logical(&index_value);
              } else {
                // Произошла ошибка
              }
              /*if (fsuccess) {
                fsuccess = m_detector_token.next_token();
              } else {
                // Произошла ошибка
              }*/
              if (fsuccess) {
                fsuccess = m_detector_token.get_token(&token);
              } else {
                // Произошла ошибка
              }
              if (fsuccess) {
                if (token.token_type() == tt_delimiter) {
                  delimiter_t next_delim = token.delimiter();
                  if (next_delim == d_right_square_bracket) {
                    ap_id_constant->back().second.push_back(index_value);
                    last_token_type = ltt_right_square_bracket;
                  } else {
                    fsuccess = false;
                  }
                } else {
                  fsuccess = false;
                }
              } else {
                // Произошла ошибка
              }
              is_token_part_identifier = true;
            } else {
              is_token_part_identifier = false;
            }
          } break;
          default : {
            is_token_part_identifier = false;
          }
        }
        if (fsuccess && is_token_part_identifier) {
          fsuccess = m_detector_token.next_token();
          if (fsuccess) {
            fsuccess = m_detector_token.get_token(&token);
          } else {
            // Произошла ошибка
          }
        } else {
          // Произошла ошибка
        }
      }
    } else {
      fsuccess = false;
    }
  } else {
    // Произошла ошибка
  }
  if (!fsuccess) {
    ap_id_constant->clear();
  } else {
    // Ошибок не произошло
  }
  return fsuccess;
}

inline bool calculator_t::eval_exp_arg_function(value_type* ap_value)
{
  bool fsuccess = true;
  ap_value->type(variant::var_type_array);
  ap_value->resize(0);
  token_t token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    delimiter_t delim = token.delimiter();
    if (delim == d_left_parenthesis) {
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // Произошла ошибка
      }
      if (fsuccess) {
        delim = token.delimiter();
      } else {
        // Произошла ошибка
      }
      while (fsuccess && (delim != d_right_parenthesis)) {
        value_type partial_value;
        if (fsuccess) {
          fsuccess = eval_exp_logical(&partial_value);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          ap_value->resize(ap_value->size() + 1);
          (*ap_value)[ap_value->size()-1] = partial_value;
          fsuccess = m_detector_token.get_token(&token);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          delim = token.delimiter();
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          if (delim == d_comma) {
            fsuccess = m_detector_token.next_token();
            if (fsuccess) {
              fsuccess = m_detector_token.get_token(&token);
            } else {
              // Произошла ошибка
            }
            if (fsuccess) {
              delim = token.delimiter();
            } else {
               // Произошла ошибка
            }
          } else if (delim != d_right_parenthesis) {
            fsuccess = false;
          } else {
            // Продолжаем обработку
          }
        } else {
          // Произошла ошибка
        }
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
      fsuccess = false;
      //fsuccess = atom(ap_value);
    }
  } else {
    // Произошла ошибка
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
      while(token.is_operator_logical() && fsuccess) {
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
    while (token.is_operator_compare() && fsuccess) {
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
    if (token.token_type() == tt_identifier) {
      const function_t* p_function = IRS_NULL;
      if (m_list_identifier.function_find(token.get_identifier(), &p_function))
      { 
        fsuccess = m_detector_token.next_token();
        value_type arguments_func;
        if (fsuccess) {
          fsuccess = eval_exp_arg_function(&arguments_func);
        } else {
          // Произошла ошибка
        }
        if (fsuccess) {
          fsuccess = p_function->exec(&arguments_func, ap_value);
        } else {
          // Произошла ошибка
        }
      } else {
        vector<pair<string_type, vector<int> > > constant;
        fsuccess = eval_exp_constant(&constant);
        if (fsuccess) {
          fsuccess = constant_value_get(constant, ap_value);
        } else {
          // Произошла ошибка
        }
      }  
    } else if (token.token_type() == tt_function) {
      const function_t* const p_func = token.get_function();
      fsuccess = m_detector_token.next_token();
      value_type arguments_func;
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

inline bool calculator_t::constant_value_get(
  const id_constant_type& a_id_constant,
  value_type* ap_value)
{
  bool fsuccess = true;
  IRS_LIB_ASSERT(ap_value != IRS_NULL);
  if (a_id_constant.size() == 1) {
    const size_type index_count = a_id_constant.front().second.size();
    const value_type* p_constant = IRS_NULL;
    if (m_list_identifier.constant_find(
      a_id_constant.front().first, &p_constant))
    {
      for (size_type index_i = 0; index_i < index_count; index_i++) {
        if (p_constant->type() == variant::var_type_array) {
          int cur_index = a_id_constant.front().second[index_i];
          p_constant = &(*p_constant)[cur_index];
        } else {
          fsuccess = false;
          break;
        }
      }
      if (fsuccess) {
        if (p_constant->type() != variant::var_type_array) {
          *ap_value = *p_constant;
        } else {
          fsuccess = false;
        }
      } else {
        // Произошла ошибка
      }
    } else {
      if (mp_handle_extern_const != IRS_NULL) {
        fsuccess = mp_handle_extern_const->get(a_id_constant, ap_value);
      } else {
        fsuccess = false;
      }
    }
  } else if (a_id_constant.size() > 1) {
    // Контейнер не пустой
    if (mp_handle_extern_const != IRS_NULL) {
      fsuccess = mp_handle_extern_const->get(a_id_constant, ap_value);;
    } else {
      fsuccess = false;
    }
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

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
inline void calculator_t::imbue(const locale& a_locale)
{
  m_locale = a_locale;
  m_detector_token.imbue(a_locale);
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

inline void calculator_t::handle_external_constant_set(
  const handle_external_constant_t* ap_handle_extern_const)
{
  mp_handle_extern_const = ap_handle_extern_const;
}

#ifdef NOP
class mutable_ref_t
{
public:
  typedef irs::variant::variant_t variant_t;
  enum type_t {type_unknown, type_value, type_id};
  struct value_tag {};
  struct id_variable_tag {};
  mutable_ref_t();
  mutable_ref_t(type_t a_type = type_value);
  mutable_ref_t(const variant_t& a_value, value_tag);
  mutable_ref_t(const id_variable_type& a_id_variable, id_variable_tag);
  mutable_ref_t(const mutable_ref_t& a_mutable_ref);
  variant_t& value();
  const variant_t& value() const;
  void value(const variant_t& a_value);
  id_variable_type& id();
  const id_variable_type& id() const;
  void id(const id_variable_type& a_id);
  type_t type(type_t) const;
  void swap(mutable_ref_t& a_mutable_ref);
  mutable_ref_t& operator=(const mutable_ref_t& a_mutable_ref);
private:
  void change_type(const type_t a_type);
  variant_t m_value;
  id_variable_type m_id;
  /*union {
    variant_t* p_value;
    id_variable_type* p_id;
  } m_variable;*/
  type_t m_type;
};
#endif // NOP

} // namespace calc

} // namespace irs

//#endif // GNU C++ specific

#endif //__WATCOMC__

#endif //irscalcH
