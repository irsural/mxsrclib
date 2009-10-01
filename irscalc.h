//---------------------------------------------------------------------------
// �����������
// ����: 24.09.2009

#ifndef irscalcH
#define irscalcH

// � Watcom C++ ���� ������ ��-const ������ ������� map::find
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

// ��������������� ��������� �����.
// �������� ��� ������� �� �����
// ������� ������������� �������,
// �������� �� ��������� ������,
// �������� ��������� �� �������.
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
  tt_function
};
// ������ ���������������

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
      // ������������ ���������� ����������
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
      // ������������ ���������� ����������
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
      // ������������ ���������� ����������
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
    // ��������� � ����� ������ ����������
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
    // ��������� � ����� ������ ����������
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
  union {
    value_type* p_num;
    delimiter_t delimiter;
    const value_type* p_constant;
    const function_t* p_function;
  } m_tok;
public:
  token_t(
  ):
    m_token_type(tt_none),
    m_tok()
  { }
  inline void type_change(const token_type_t a_token_type)
  {
    if (m_token_type != a_token_type) {
      if (m_token_type == tt_number) {
        delete m_tok.p_num;
      } else if (a_token_type == tt_number) {
        m_tok.p_num = new value_type;
      }
    } else {
      // �� ��������� ��������� ����
    }
    m_token_type = a_token_type;
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
      // ������� ������� �� �������� �������������
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
      // ������� ������� �� �������� ��������
    }
    return p_function;
  }
  inline const value_type* get_constant() const
  {
    const value_type* p_constant = IRS_NULL;
    if (m_token_type == tt_constant) {
      p_constant = m_tok.p_constant;
    } else {
      // ������� ������� �� �������� ����������
    }
    return p_constant;
  }
  inline value_type get_number() const
  {
    value_type num;
    num.type(variant::var_type_unknown);
    if (m_token_type == tt_number) {
      num = (*m_tok.p_num);
    } else {
      // ������� ��������� �� �������� ������
    }
    return num;
  }
  inline bool is_operator_logical()
  {
    bool operator_logical_status = false;
    if (m_token_type == tt_delimiter) {
      if ((m_tok.delimiter >= d_and) && (m_tok.delimiter <= d_not)) {
        operator_logical_status = true;
      } else {
        // ��� �� ���������� ��������
      }
    } else {
      // ��� �� ������������
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
        // ��� �� �������� ���������
      }
    } else {
      // ��� �� ������������
    }
    return operator_compare_status;
  }
};

// �������� ������
class detector_token_t
{
public:
  typedef valuens_t value_type;
  typedef sizens_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  detector_token_t(
    list_identifier_t& a_list_identifier):
    m_list_identifier(a_list_identifier),
    m_cur_token_data(),
    mp_prog(IRS_NULL),
    m_prog_pos(0),
    m_ch_valid_name(irst("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"))
  { }
  // ���������� ��������� �� ����� ���������
  inline void set_prog(const string_type* ap_prog);
  inline bool next_token();
  //bool back_token();
  inline bool get_token(token_t* const ap_token);
private:
  // ������ ���������������
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
  // ��������� �� ����� ���������
  const string_type* mp_prog;
  // ������� ������� � ������ ���������
  size_type m_prog_pos;
  // ������ ���������� �������� � ����� ��������������
  const string_type m_ch_valid_name;
  // �������� ������� �� �������������� � ������� �����
  inline bool is_char_digit(const char_type a_ch);
  // �������� �� ������ ����������
  inline bool is_char_exponent(const char_type a_ch);
  // �������� ������� �� �������������� �����
  inline bool is_char_alpha(const char_type a_ch);
  // ���������������� ������� � ������� ������� ���������
  inline bool detect_token();
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
  bool is_ch_alpha = false;
  if (isalphat(a_ch)) {
    is_ch_alpha = true;
  } else {
    // ��������� ����� �������� ��������
    bool is_ch_russian_capital = (a_ch >= irst('�')) && (a_ch <= irst('�'));
    // �������� ����� �������� ��������
    bool is_ch_russian_small = (a_ch >= irst('�'))/* && (a_ch <= irst('�'))*/;
    if (is_ch_russian_capital || is_ch_russian_small) {
      is_ch_alpha = true;
    } else {
      // ������ �� �������� ������ ���������� � �������� ���������
    }
  }
  return is_ch_alpha;
}

inline bool detector_token_t::detect_token()
{
  bool detected_token = false;
  //bool fsuccess = true;
  if (m_prog_pos == mp_prog->size()) {
    m_cur_token_data.token.set_delimiter(d_end);
    m_cur_token_data.length = 0;
    //m_cur_token_data.valid = true;
    detected_token = true;
  }
  size_type pos;
  char_type ch = irst('\0');
  if (!detected_token) {
    // ���������� �������, ��������� � �������� �� ����� ������
    pos = mp_prog->find_first_not_of(irst(" \n\r\t"), m_prog_pos);
    if (pos == string_type::npos) {
      // ��������� ����� ���������
      m_prog_pos = mp_prog->size();
      m_cur_token_data.token.set_delimiter(d_end);
      m_cur_token_data.length = 0;
      //m_cur_token_data.valid = true;
      detected_token = true;
    } else {
      ch = (*mp_prog)[pos];
    }
  }

  if (!detected_token) {
    // ������ ������������
    bool next_ch_not_end_prog = false;
    const size_type begin_ch_pos = pos;
    char_type next_ch = irst('\0');
    if ((pos + 1) < mp_prog->size()) {
      next_ch_not_end_prog = true;
      next_ch = (*mp_prog)[pos + 1];
    } else {
      // ��������� ������ ����������
    }
    switch(ch) {
      case irst('&'): {
        if (next_ch_not_end_prog && (next_ch == irst('&'))) {
          m_cur_token_data.token.set_delimiter(d_and);
          detected_token = true;
          pos += 2;
        } else {
          // �� ������� ���������� ������������
        }
      } break;
      case irst('|'): {
        if (next_ch_not_end_prog && (next_ch == irst('|'))) {
          m_cur_token_data.token.set_delimiter(d_or);
          detected_token = true;
          pos += 2;
        } else {
          // �� ������� ���������� ������������
        }
      } break;
      case irst('='): {
        if (next_ch_not_end_prog && (next_ch == irst('='))) {
          m_cur_token_data.token.set_delimiter(d_compare_equal);
          detected_token = true;
          pos += 2;
        } else {
          // �� ������� ���������� ������������
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
      case irst(','): {
        m_cur_token_data.token.set_delimiter(d_comma);
        detected_token = true;
        pos++;
      } break;
      default : {
        detected_token = false;
      }
    }
    if (detected_token) {
      m_cur_token_data.length = pos - m_prog_pos;
      //m_cur_token_data.valid = true;
    } else {
      pos = begin_ch_pos;
    }
  }

  if (!detected_token) {
    // ������ �����
    if (is_char_digit(ch)) {
      // ���������� ������� ������� ������� �����
      const size_type num_begin_ch = pos;
      string_type num_str;
      pos++;
      // ������ ����������� ������� ���������� �������
      bool detected_exponent_ch = false;
      // ������ ����������� ������� - ����������� ����� � ������� �����
      bool detected_int_part_delim_ch = false;
      // ������ ����������� ����� ���������� �������
      bool detected_sign_exponent_ch = false;
      // ������ ����������� �������� unsigned
      bool detected_suffix_unsigned = false;
      // ������ ����������� �������� long
      bool detected_suffix_long = false;
      // ������ ����������� �������� float
      bool detected_suffix_float = false;
      while (pos <  mp_prog->size()) {
        // ���� ������ ������, �� ���������� ������
        pos = mp_prog->find_first_not_of(irst("0123456789"), pos);

        if (pos == string_type::npos) {
          pos = mp_prog->size();
          break;
        } else {
          // ������ ������, �� ���������� ������
        }
        const char_type ch_number = (*mp_prog)[pos];
        if (ch_number == irst('.')) {
          if ((!detected_exponent_ch) && (!detected_int_part_delim_ch) &&
            (!detected_sign_exponent_ch))
          {
            detected_int_part_delim_ch = true;
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
            !detected_int_part_delim_ch &&
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
          if (!detected_suffix_float && !detected_suffix_long) {
            detected_suffix_float = true;
            pos++;
          } else {
            break;
          }
        } else {
          break;
        } 
      }
      num_str = mp_prog->substr(num_begin_ch, (pos - num_begin_ch));
      bool convert_str_to_number_success = false;
      if (!detected_int_part_delim_ch &&
        !detected_exponent_ch &&
        !detected_suffix_float)
      {
        // ������ ����� �����
        if (detected_suffix_long) {
          if (detected_suffix_unsigned) {
            unsigned long number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              m_cur_token_data.token.set_number(number);
            } else {
              // �������������� � ����� ������ ��������
            }
          } else {
            long number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              m_cur_token_data.token.set_number(number);
            } else {
              // �������������� � ����� ������ ��������
            }
          }
        } else {
          if (detected_suffix_unsigned) {
            unsigned int number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              m_cur_token_data.token.set_number(number);
            } else {
              // �������������� � ����� ������ ��������
            }
          } else {
            int number = 0;
            convert_str_to_number_success = num_str.to_number(number);
            if (convert_str_to_number_success) {
              m_cur_token_data.token.set_number(number);
            } else {
              // �������������� � ����� ������ ��������
            }
          }
        }
      } else {
        // ������ ����� � ��������� ������
        if (detected_suffix_long) {
          long double number = 0.;
          convert_str_to_number_success = num_str.to_number(number);
          if (convert_str_to_number_success) {
            m_cur_token_data.token.set_number(number);
          } else {
            // �������������� � ����� ������ ��������
          }
        } else if (detected_suffix_float) {
          float number = 0.f;
          convert_str_to_number_success = num_str.to_number(number);
          if (convert_str_to_number_success) {
            m_cur_token_data.token.set_number(number);
          } else {
            // �������������� � ����� ������ ��������
          }
        } else {
          double number = 0.;
          convert_str_to_number_success = num_str.to_number(number);
          if (convert_str_to_number_success) {
            m_cur_token_data.token.set_number(number);
          } else {
            // �������������� � ����� ������ ��������
          }
        }
      }

      if (convert_str_to_number_success) {
        m_cur_token_data.length = pos - m_prog_pos;
        detected_token = true;
      } else {
        // ������ �� ������� ������������� � �����
      }
      if (!detected_token) {
        pos = num_begin_ch;
      } else {
        // ������� �������������
      }
    } else {
      // ������ �� ����������� ������ ��������, ���������� ��� ��������� �����
    }
  }

  // ������ ������������� ��� �������� �����
  if (!detected_token) {
    if (is_char_alpha(ch)) {
      const size_type pos_begin_name = pos;
      pos = mp_prog->find_first_not_of(m_ch_valid_name, pos);
      if (pos == string_type::npos) {
        pos = mp_prog->size();
      } else {
        // ������, �� ������������� ������ ��������, ���������� � �����,
        // �� ������
      }
      string_type identifier_str =
        mp_prog->substr(pos_begin_name, (pos - pos_begin_name));

      if (identifier_str == irst("and")) {
        m_cur_token_data.token.set_delimiter(d_and);
        detected_token = true;
      } else if (identifier_str == irst("or")) {
        m_cur_token_data.token.set_delimiter(d_or);
        detected_token = true;
      } else if (identifier_str == irst("not")) {
        m_cur_token_data.token.set_delimiter(d_not);
        detected_token = true;
      } else {
        const function_t* p_function = IRS_NULL;
        if (m_list_identifier.function_find(identifier_str, &p_function)) {
          m_cur_token_data.token.set_function(p_function);
          detected_token = true;
        } else {
          const value_type* p_constant = IRS_NULL;
          if (m_list_identifier.constant_find(identifier_str, &p_constant)) {
            m_cur_token_data.token.set_constant(p_constant);
            detected_token = true;
          } else {
            // ���������� � ����� ������ �� �������
          }
        }
      }
      if (detected_token) {
        m_cur_token_data.length = pos - m_prog_pos;
        //m_cur_token_data.valid = true;
      } else {
        pos = pos_begin_name;
      }
    } else {
      // ������ �� ����������� ������ ��������, ���������� � �����
    }
  }
  if (detected_token) {
    m_cur_token_data.valid = true;
  } else {
    // ������� �� �������������
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
      IRS_LIB_ASSERT_MSG("������ ��� ����������� � ���������� ������");
      fsuccess = false;
    }
  } else {
    fsuccess = detect_token();
  }
  return fsuccess;
}

inline bool detector_token_t::get_token(token_t* const ap_token)
{
  bool fsuccess = true;
  if (!m_cur_token_data.valid) {
    fsuccess = detect_token();
  } else {
    // ������� ��� �������������
  }
  if (fsuccess) {
    *ap_token = m_cur_token_data.token;
  } else {
    // ��������� ������
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
  // ������������ ���������� ������
  inline bool eval_exp_square_brackets(value_type* ap_value);
  // ������������ ������ ������� ���������, ������������� ������ ���
  inline bool eval_exp_arg_function(value_type* ap_value);
  // ������������ ���������� ��������
  inline bool eval_exp_logical(value_type* ap_value);
  // ������������ �������� ���������
  inline bool eval_exp_compare(value_type* ap_value);
  // ������������ �������� � ���������
  inline bool eval_exp_arithmetic_level1(value_type* ap_value);
  // ������������ ���������, �������, ������������� �������
  inline bool eval_exp_arithmetic_level2(value_type* ap_value);
  // ������������ ������� ���������� �������� "��"
  inline bool eval_exp_not(value_type* ap_value);  
  // ������������ ���������� � �������
  inline bool eval_exp_power(value_type* ap_value);
  // ������������ ������
  inline bool eval_exp_brackets(value_type* ap_value);
  // ������������ ������� � �����
  inline bool atom(value_type* ap_value);
  // ���������� �������
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
        // ������� ������������� ������ ���� ����� ���������
        fsuccess = false;
      }            
    } else {
      // ��������� ������
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
        // ��������� ������
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        if (token.token_type() == tt_delimiter) {
          if (token.delimiter() == d_right_square_bracket) {
            // ����������� ���������� ������ �����������
            fsuccess = m_detector_token.next_token();
          } else {
            fsuccess = false;
          }
        } else {
          fsuccess = false;
        }
      } else {
        // ��������� ������
      }
    } else {
      // ���������� ����� ������ �� ����������
      fsuccess = false;
    }
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
        // ��������� ������
      }
      if (fsuccess) {
        delim = token.delimiter();
      } else {
        // ��������� ������
      }
      while (fsuccess && (delim != d_right_parenthesis)) {
        value_type partial_value;
        if (fsuccess) {
          fsuccess = eval_exp_logical(&partial_value);
        } else {
          // ��������� ������
        }
        if (fsuccess) {
          ap_value->resize(ap_value->size() + 1);
          (*ap_value)[ap_value->size()-1] = partial_value;
          fsuccess = m_detector_token.get_token(&token);
        } else {
          // ��������� ������
        }
        if (fsuccess) {
          delim = token.delimiter();
        } else {
          // ��������� ������
        }
        if (fsuccess) {
          if (delim == d_comma) {
            fsuccess = m_detector_token.next_token();
            if (fsuccess) {
              fsuccess = m_detector_token.get_token(&token);
            } else {
              // ��������� ������
            }
            if (fsuccess) {
              delim = token.delimiter();
            } else {
               // ��������� ������
            }
          } else if (delim != d_right_parenthesis) {
            fsuccess = false;
          } else {
            // ���������� ���������
          }
        } else {
          // ��������� ������
        }
      }
      if (fsuccess) {
        if (token.delimiter() == d_right_parenthesis) {
          // ����������� ������� ������ �����������
          fsuccess = m_detector_token.next_token();
        } else {
          fsuccess = false;
        }
      } else {
        // ��������� ������
      }
    } else {
      fsuccess = false;
      //fsuccess = atom(ap_value);
    }
  } else {
    // ��������� ������
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
              // ��������� ������
            }
          } else {
            // ������� �� �������� ���������� �������� ���������
          }
        } else {
          // ��������� ������
        }
        if (fsuccess) {
          delim = token.delimiter();
        } else {
          // ��������� ������
        }
      }
    } else {
      // ��������� ������
    }
  } else {
    // ��������� ������
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
        // ��������� ������
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
            IRS_LIB_ASSERT_MSG("������� ������ ���� ��������� ���������");
          }
        }
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }
    }
  } else {
    // ��������� ������
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
          // ��������� ������
        }
        if (fsuccess) {
          if (delim == d_plus) {
            *ap_value = *ap_value + partial_value;
          } else if (delim == d_minus) {
            *ap_value = *ap_value - partial_value;
          }
          fsuccess = m_detector_token.get_token(&token);
        } else {
          // ��������� ������
        }
        if (fsuccess) {
          delim = token.delimiter();
        } else {
          // ��������� ������
        }
      }
    } else {
      // ��������� ������ ��� ���������� �������
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
          // ��������� ������
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
          // ��������� ������
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
        // ��������� ������
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
        // ��������� ������
      }
    }
  } else {
    // ��������� ������
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
          // ��������� ������
        }
      }
    } else {
      // ��������� ������
    }
  } else {
    // ��������� ������
  }
  return fsuccess;
}

// ������������ ��������� �� ��������
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
        // ��������� ������
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        if (token.delimiter() == d_right_parenthesis) {
          // ����������� ������� ������ �����������
          fsuccess = m_detector_token.next_token();
        } else {
          fsuccess = false;
        }
      } else {
        // ��������� ������
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
      const function_t* const p_func = token.get_function();
      fsuccess = m_detector_token.next_token();
      value_type arguments_func;
      if (fsuccess) {
        fsuccess = eval_exp_arg_function(&arguments_func);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        fsuccess = p_func->exec(&arguments_func, ap_value);
      } else {
        // ��������� ������
      }
    } else if (token.token_type() == tt_constant) {
      const value_type* p_constant = token.get_constant();
      fsuccess = m_detector_token.next_token();
      while (fsuccess && (p_constant->type() == variant::var_type_array)) {
        value_type elem_index;
        if (fsuccess) {       
          fsuccess = eval_exp_square_brackets(&elem_index);
        } else {
          // ��������� ������
        }
        if (fsuccess) {
          if (elem_index < p_constant->size()) {
            p_constant = &(*p_constant)[elem_index];
          } else {
            fsuccess = false;
          }                  
        } else {
          // ��������� ������
        }
      }
      if (fsuccess) {
        *ap_value = *p_constant;
      } else {
        // ��������� ������
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
          // ��������� ������
        }
      } else if (token.delimiter() == d_minus) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = atom(ap_value);
          *ap_value *= -1;
        } else {
          // ��������� ������
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

#ifdef NOP
template<class T>
class mutable_ref_t
{
public:
  enum type_t {type_value, type_reference};
  struct value_tag {};
  struct reference_tag {};
  mutable_ref_t(type_t a_type = type_reference);
  mutable_ref_t(T& a_value, reference_tag);
  mutable_ref_t(const T& a_value, value_tag);
  mutable_ref_t(const mutable_ref_t<T>& a_ref);
  type_t type() const;
  operator T&();
  //operator const T&() const;
  void set_value(const T& a_value);
  void set_reference(T& a_value);
  T& operator=(const T& a_value);
  mutable_ref_t& operator=(const mutable_ref_t<T>& a_ref);
private:
  T* mp_value;
  type_t m_type;
};

template<class T>
mutable_ref_t<T>::mutable_ref_t(type_t a_type):
  mp_value((a_type == type_reference) ? IRS_NULL : new T()),
  m_type(a_type)
{
}

template<class T>
mutable_ref_t<T>::mutable_ref_t(const T& a_value, value_tag):
  mp_value(new T(a_value)),
  m_type(type_value)
{
}

template<class T>
mutable_ref_t<T>::mutable_ref_t(T& a_value, reference_tag):
  mp_value(&a_value),
  m_type(type_reference)
{
}

template<class T>
mutable_ref_t<T>::mutable_ref_t(const mutable_ref_t<T>& a_ref):
  mp_value((a_ref.m_type == type_reference) ?
    a_ref.mp_value : new T(a_ref.mp_value)),
  m_type(a_ref.m_type)
{ 
}

template<class T>
typename mutable_ref_t<T>::type_t mutable_ref_t<T>::type() const
{
  return m_type;
}

template<class T>
mutable_ref_t<T>::operator T&() 
{
  return *mp_value;
}

/*template<class T>
mutable_ref_t<T>::operator const T&() const
{
  return *mp_value;
}*/

template<class T>
void mutable_ref_t<T>::set_value(const T& a_value)
{
  if (m_type == type_reference) {
    mp_value = new T(a_value);
  } else {
    *mp_value = a_value;
  }
}

template<class T>
void mutable_ref_t<T>::set_reference(T& a_value)
{
  if (m_type == type_value) {
    delete mp_value;
  } else {
    // ������ �� ��������
  }
  mp_value = &a_value;
}

template<class T>
T& mutable_ref_t<T>::operator=(const T& a_value)
{
  *mp_value = a_value;
  return *mp_value;
}

/*template<class T>
T* mutable_ref_t<T>::operator=(T* ap_value)
{
  if (m_location_memory_internal) {
    delete mp_value;
  } else {
    // ��������� ��������� �� ������� ������
  }
  mp_value = ap_value;
  m_location_memory_internal = false;
  return mp_value;
}*/

template<class T>
mutable_ref_t<T>& mutable_ref_t<T>::operator=(const mutable_ref_t<T>& a_ref)
{
  mutable_ref_t<T> ref(a_ref);
  swap(this, ref);
  return *this;
}

inline void mutable_ref_test()
{
  typedef int test_type;
  // ������� ��������, ��� �������� ������� ����������� ����������
  // �� ���������� ������ ������� ������ mutable_ref_t
  test_type* p_var = IRS_NULL;
  p_var = new test_type;
  mutable_ref_t<test_type> mref_var;
  // ���������� ����������� �� ���������� ������ ref_int;
  mref_var.set_value(*p_var);
  // ������� ���������� ������ � �������� ���������
  delete p_var;
  p_var = IRS_NULL;
  // � ������� ������ ���� �������� ������, � �������
  // �� ����� ��� ������ ��������
  mref_var = 10;
  IRS_LIB_ASSERT(mref_var == 10);
  // ������ ��������, ��� ������ ������ mutable_ref_t �������� � ��������
  // ������ �� ������� ����������
  p_var = new test_type;
  // � ���������� ���������� ����� mref_int ����������� ������ ���������
  mref_var.set_reference(*p_var);
  mref_var = 99;
  // � ���������� var ������ ��������� ����� ����������� ��������
  test_type var = *p_var;
  IRS_LIB_ASSERT(var == 99);

  mutable_ref_t<int> mref_var2(20, mutable_ref_t<int>::value_tag());
  mref_var2 = 22;

  string str;
  mutable_ref_t<string> mref_string;
  mref_string.set_reference(str);
  mref_string = "sdfsa"; 
  IRS_LIB_ASSERT(mref_string == "sdfsa");
}

#endif //NOP

} // namespace calc

} // namespace irs


#endif //__WATCOMC__

#endif //irscalcH
