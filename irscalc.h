//---------------------------------------------------------------------------
// �����������
// ����: 18.09.2009

#ifndef irscalcH
#define irscalcH

#include <iostream>
#include <irsstdg.h>
#include <irserror.h>
#include <math.h>
#include <irsstrdefs.h>

#define new_calculator_valid

//---------------------------------------------------------------------------
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

// ����� ������ ������������, ������ ����� ������� class calculator_t
#ifdef new_calculator_valid
namespace calc {
typedef size_t size_type;
typedef irs::char_t charns_t;
typedef irs::string str_type;
typedef irs::string_t stringns_t;
typedef double num_type;
typedef int (*func_r_int_a_int_ptr)(int);
typedef float (*func_r_float_a_float_ptr)(float);
typedef double (*func_r_double_a_double_ptr)(double);
typedef long double (*func_r_ldouble_a_ldouble_ptr)(long double);
enum token_type_t {
  tt_none,
  tt_number,
  tt_delimiter,
  tt_function,
  tt_array};
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
template<class num_t>
class list_identifier_t
{
public:
  typedef num_t value_type;
  typedef size_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  typedef map<string_type, num_t> num_const_list_type;
  typedef typename map<string_type, num_t>::iterator
    num_const_list_iterator;
  typedef typename map<string_type, num_t>::const_iterator
    num_const_list_const_iterator;
  typedef vector<num_t> array_type;
  typedef map<string_type, vector<num_t> > array_list_type;
  typedef typename map<string_type, vector<num_t> >::iterator
    array_list_iterator;
  typedef typename map<string_type, vector<num_t> >::const_iterator
    array_list_const_iterator;
  list_identifier_t();
  int find_function(const string_type& a_function_name) const;
  inline bool add_func_r_int_a_int(
    const string_type& a_name, func_r_int_a_int_ptr ap_function);
  inline bool add_func_r_float_a_float(
    const string_type& a_name, func_r_float_a_float_ptr ap_function);
  inline bool add_func_r_double_a_double(
    const string_type& a_name, func_r_double_a_double_ptr ap_function);
  inline bool add_func_r_ldouble_a_ldouble(
    const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function);
  inline void num_const_add(
    const pair<string_type, num_t>& a_num_const_pair);
  inline void num_const_del(const string_type& a_num_const_name);
  inline void num_const_clear();
  inline bool num_const_is_exists(const string_type& a_num_const_name) const;
  inline bool num_const_find(const string_type& a_name, num_t* ap_value) const;
  inline void array_add(
    const pair<string_type, array_type>& a_array_pair);
  inline void array_del(const string_type& a_array_name);
  inline void array_clear();
  inline bool array_is_exists(const string_type& a_array_name) const;
  inline bool array_find(const string_type& a_name,
    const array_type** ap_p_array) const;
  bool del_func(const string_type& a_name);
  void clear_func();
  //void clear_num_const();
  inline const function_t& get_func(size_type a_id_func) const; 
private:
  vector<function_t> m_func_array;
  num_const_list_type m_num_const_list;
  array_list_type m_array_list;
  bool add_func(
    const string_type& a_name,
    const type_function_t a_type,
    void* ap_function);
};

template<class num_t>
inline bool list_identifier_t<num_t>::add_func_r_int_a_int(
  const string_type& a_name, func_r_int_a_int_ptr ap_function)
{
  return add_func(a_name, tf_r_int_a_int, reinterpret_cast<void*>(ap_function));
}

template<class num_t>
inline bool list_identifier_t<num_t>::add_func_r_float_a_float(
  const string_type& a_name, func_r_float_a_float_ptr ap_function)
{
  return add_func(a_name, tf_r_float_a_float,
    reinterpret_cast<void*>(ap_function));
}

template<class num_t>
inline void list_identifier_t<num_t>::num_const_add(const pair<string_type,
  num_t>& a_num_const_pair)
{
  IRS_LIB_ASSERT(find_function(a_num_const_pair.first) == irs::npos);
  IRS_LIB_ASSERT(m_num_const_list.find(a_num_const_pair.first) ==
    m_num_const_list.end());     
  m_num_const_list.insert(a_num_const_pair);
}

template<class num_t>
inline void list_identifier_t<num_t>::num_const_del(
  const string_type& a_num_const_name)
{
  num_const_list_iterator it_num_const =
    m_num_const_list.find(a_num_const_name);
  IRS_LIB_ASSERT(it_num_const != m_num_const_list.end());
  m_num_const_list.erase(it_num_const);
}

template<class num_t>
inline void list_identifier_t<num_t>::num_const_clear()
{
  m_num_const_list.clear();
}

template<class num_t>
inline bool list_identifier_t<num_t>::num_const_is_exists(
  const string_type& a_num_const_name) const
{
  return m_num_const_list.find(a_num_const_name) != m_num_const_list.end();
}

template<class num_t>
inline bool list_identifier_t<num_t>::num_const_find(
  const string_type& a_name, num_t* ap_value) const
{
  num_const_list_const_iterator it_num_const =
    m_num_const_list.find(a_name);
  bool num_const_is_exists = it_num_const != m_num_const_list.end();
  if (num_const_is_exists) {
    *ap_value = it_num_const->second;
  } else {
    // ��������� � ����� ������ ����������
  }
  return num_const_is_exists;
}

template<class num_t>
inline void list_identifier_t<num_t>::array_add(
  const pair<string_type, array_type>& a_array_pair)
{
  IRS_LIB_ASSERT(find_function(a_array_pair.first) == irs::npos);
  IRS_LIB_ASSERT(m_num_const_list.find(a_array_pair.first) ==
    m_num_const_list.end());
  IRS_LIB_ASSERT(m_array_list.find(a_array_pair.first) ==
    m_array_list.end());
  m_array_list.insert(a_array_pair);
}

template<class num_t>
inline void list_identifier_t<num_t>::array_del(
  const string_type& a_array_name)
{
  array_list_iterator it_array =
    m_array_list.find(a_array_name);
  IRS_LIB_ASSERT(it_array != m_array_list.end());
  m_array_list.erase(it_array);
}

template<class num_t>
inline void list_identifier_t<num_t>::array_clear()
{
  m_array_list.clear();
}

template<class num_t>
inline bool list_identifier_t<num_t>::array_is_exists(
  const string_type& a_array_name) const
{
  return m_array_list.find(a_array_name) != m_array_list.end();
}

template<class num_t>
inline bool list_identifier_t<num_t>::array_find(const string_type& a_name,
  const array_type** ap_p_array) const
{
  array_list_const_iterator it_array =
    m_array_list.find(a_name);
  bool array_is_exists = it_array != m_array_list.end();
  if (array_is_exists) {
    *ap_p_array = &it_array->second;
  } else {
    // ��������� � ����� ������ ����������
  }
  return array_is_exists;
}

template<class num_t>
inline bool list_identifier_t<num_t>::add_func_r_double_a_double(
  const string_type& a_name, func_r_double_a_double_ptr ap_function)
{
  return add_func(a_name, tf_r_double_a_double,
    reinterpret_cast<void*>(ap_function));
}

template<class num_t>
inline bool list_identifier_t<num_t>::add_func_r_ldouble_a_ldouble(
  const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function)
{
  return add_func(a_name, tf_r_ldouble_a_ldouble,
    reinterpret_cast<void*>(ap_function));
}

template<class num_t>
inline const function_t& list_identifier_t<num_t>::
  get_func(size_type a_id_func) const
{
  return m_func_array[a_id_func];
}

template<class num_t>
list_identifier_t<num_t>::list_identifier_t(
):
  m_func_array(),
  m_num_const_list()
{ }

template<class num_t>
int list_identifier_t<num_t>::
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
template<class num_t>
bool list_identifier_t<num_t>::add_func(
  const string_type& a_name,
  const type_function_t a_type,
  void* ap_function)
{
  bool fsuccess = true;
  if (find_function(a_name) != irs::npos) {
    fsuccess = false;
  } else if (num_const_is_exists(a_name)) {
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

template<class num_t>
bool list_identifier_t<num_t>::del_func(const string_type& a_name)
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

template<class num_t>
void list_identifier_t<num_t>::clear_func()
{
  m_func_array.clear();
}

/*template<class num_t>
void list_identifier_t<num_t>::clear_num_const()
{
  m_num_const_array.clear();
}*/

template<class num_t>
class token_t
{
public:
  typedef size_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  typedef typename list_identifier_t<num_t>::array_type array_type;
  //static const int m_none_id_func = -1;
private:
  token_type_t m_token_type;
  num_t m_num;
  delimiter_t m_delimiter;
  size_type m_id_function;
  const array_type* mp_array;
public:
  token_t(
  ):
    m_token_type(tt_none),
    m_num(0),
    m_delimiter(d_none),
    m_id_function(0),
    mp_array(IRS_NULL)
  {}
  void set_number(const num_t& a_num)
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
  void set_array(const array_type* ap_array)
  {
    m_token_type = tt_array;
    m_num = 0;
    m_delimiter = d_none;
    m_id_function = 0;
    mp_array = ap_array;
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
  const array_type* get_array() const
  {
    return mp_array;
  }
  num_t get_number() const
  {
    return m_num;
  }
};

// �������� ������
template<class num_t>
class detector_token_t
{
public:
  typedef size_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  typedef typename list_identifier_t<num_t>::array_type array_type;
  detector_token_t(
    list_identifier_t<num_t>& a_list_identifier):
    m_list_identifier(a_list_identifier),
    m_cur_token_data(),
    mp_prog(IRS_NULL),
    m_prog_pos(0),
    m_exponent_ch('E'),
    m_ch_valid_name("_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz")
  { }
  // ���������� ��������� �� ����� ���������
  void set_prog(const string_type* ap_prog);
  bool next_token();
  //bool back_token();
  inline bool get_token(token_t<num_t>* const ap_token);
private:
  // ������ ���������������
  const list_identifier_t<num_t>& m_list_identifier;
  template<class td_num_t>
  struct token_data_t
  {
    token_t<td_num_t> token;
    size_type length;
    bool valid;
    token_data_t():
      token(),
      length(0),
      valid(false)
    {}
  };
  token_data_t<num_t> m_cur_token_data;
  // ��������� �� ����� ���������
  const string_type* mp_prog;
  // ������� ������� � ������ ���������
  size_type m_prog_pos;
  // ������ ���������� �������
  char_type m_exponent_ch;
  // ������ ���������� �������� � ����� ��������������
  const string_type m_ch_valid_name;
  // �������� ������� �� �������������� � ������� �����
  inline bool ch_is_digit(const char_type a_ch);
  // �������� ������� �� ����������� � �������� ������� ������� �����
  inline bool ch_is_first_char_name(const char_type a_ch);
  // ���������������� ������� � ������� ������� ���������
  bool detect_token();
};

template<class num_t>
inline bool detector_token_t<num_t>::ch_is_digit(const char_type a_ch)
{
  bool isdigit_status = false;
  if (strchr("0123456789", a_ch)) {
    isdigit_status = true;
  } else if (m_exponent_ch == a_ch) {
    isdigit_status = true;
  }
  return isdigit_status;
}

template<class num_t>
inline bool detector_token_t<num_t>::ch_is_first_char_name(const char_type a_ch)
{
  bool ch_valid = false;
  // ��������� ����� ���������� ��������
  bool ch_latin_capital = (a_ch >= 'A') && (a_ch <= 'Z');
  // �������� ����� ���������� ��������
  bool ch_latin_small = (a_ch >= 'a') && (a_ch <= 'z');
  // ������ �������������
  bool ch_underscore = (a_ch == '_');

  if (ch_latin_capital || ch_latin_small || ch_underscore) {
    ch_valid = true;
  }
  return ch_valid;
}

template<class num_t>
bool detector_token_t<num_t>::detect_token()
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
    // ���������� �������, ��������� � �������� �� ����� ������
    pos = mp_prog->find_first_not_of(" \r\t", m_prog_pos);
    if (pos == string_type::npos) {
      // ��������� ����� ���������
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
    // ������ ������������
    bool next_ch_not_end_prog = false;
    const size_type begin_ch_pos = pos;
    char_type next_ch = '\0';
    if ((pos+1) < mp_prog->size()) {
      next_ch_not_end_prog = true;
      next_ch = (*mp_prog)[pos+1];
    } else {
      // ��������� ������ ����������
    }
    switch(ch) {
      case '=': {
        if (next_ch_not_end_prog && (next_ch == '=')) {
          m_cur_token_data.token.set_delimiter(d_compare_equal);
          detected_token = true;
          pos += 2;
        } else {
          // �� ������� ���������� ������������
        }
      } break;
      case '!': {
        if (next_ch_not_end_prog && (next_ch == '=')) {
          m_cur_token_data.token.set_delimiter(d_compare_not_equal);
          detected_token = true;
          pos += 2;
        } else {
          // �� ������� ���������� ������������
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
    // ������ �����
    if (ch_is_digit(ch)) {
      string_type num_str;
      // ���������� ������� ������� ������� �����
      const size_type num_begin_ch = pos;
      pos++;
      // ������ ����������� ������� ���������� �������
      bool detected_exponent_ch = false;
      // ������ ����������� ������� - ����������� ����� � ������� �����
      bool detected_int_part_delim_ch = false;
      while (pos <  mp_prog->size()) {
        // ���� ������ ������, �� ���������� ������
        pos = mp_prog->find_first_not_of("0123456789", pos);
        // ���� �� ������ ����� ������
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

  // ������ �������������
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
        num_t num;
        if (m_list_identifier.num_const_find(identifier_str, &num)) {
          m_cur_token_data.token.set_number(num);
          m_cur_token_data.length = pos - pos_begin_name;
          m_cur_token_data.valid = true;
          detected_token = true;
        } else {
          const array_type* p_array = IRS_NULL;
          if (m_list_identifier.array_find(identifier_str, &p_array)) {
            m_cur_token_data.token.set_array(p_array);
            m_cur_token_data.length = pos - pos_begin_name;
            m_cur_token_data.valid = true;
            detected_token = true;
          } else {
            // ������ � ����� ������ �� ������
          }
        }
      }
    }
  }   
  return detected_token;
}

template<class num_t>
void detector_token_t<num_t>::set_prog(const string_type* ap_prog)
{
  mp_prog = ap_prog;
  m_prog_pos = 0;
  m_cur_token_data.valid = false;
}

template<class num_t>
inline bool detector_token_t<num_t>::
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

template<class num_t>
inline bool detector_token_t<num_t>::
  get_token(token_t<num_t>* const ap_token)
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

template<class num_t>
class calculator_t
{
public:
  typedef size_t size_type;
  typedef charns_t char_type;
  typedef stringns_t string_type;
  typedef typename list_identifier_t<num_t>::array_type array_type;
  calculator_t();
  bool calc(const string_type* ap_prog, num_t* ap_num);
  int find_function(const string_type& a_function_name) const;
  int find_num_const(const string_type& a_num_const_name) const;
  inline bool add_func_r_int_a_int(
    const string_type& a_name, func_r_int_a_int_ptr ap_function);
  inline bool add_func_r_float_a_float(
    const string_type& a_name, func_r_float_a_float_ptr ap_function);
  inline bool add_func_r_double_a_double(
    const string_type& a_name, func_r_double_a_double_ptr ap_function);
  inline bool add_func_r_ldouble_a_ldouble(
    const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function);
  inline void num_const_add(const string_type& a_name, const num_t& a_value);
  inline bool del_func(const string_type& a_name);
  inline void num_const_del(const string_type& a_name);
  void clear_func();
  void num_const_clear();

  inline void array_add(const string_type& a_name,
    const array_type& a_array);
  inline void array_del(const string_type& a_name);
  void array_clear();
private:
  list_identifier_t<num_t> m_list_identifier;
  detector_token_t<num_t> m_detector_token;
  bool interp(num_t* ap_value);
  // ������������ ���������� ������
  bool eval_exp_square_brackets(num_t* a_value);
  // ������������ �������� ���������
  bool eval_exp_compare(num_t* a_value);
  // ������������ �������� � ���������
  bool eval_exp_arithmetic_leve1(num_t* a_value);
  // ������������ ���������, �������, ������������� �������
  bool eval_exp_arithmetic_leve2(num_t* a_value);
  // ������������ ���������� � �������
  bool eval_exp_power(num_t* a_value);
  // ������������ ������
  bool eval_exp_brackets(num_t* a_value);
  // ������������ ������� � �����
  bool atom(num_t* a_value);
  // ���������� �������
  void func_exec(
    const function_t& a_function,
    const num_t a_in_param,
    num_t* ap_out_param);
  // ������ �������� �������
  bool array_elem_read(const array_type* ap_array,
    const size_type a_elem_index,
    num_t* ap_array_elem) const;
};

template<class num_t>
calculator_t<num_t>::calculator_t():
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

  num_const_add("e", IRS_E);
  num_const_add("pi", IRS_PI);

  array_type test_array;
  test_array.push_back(0);
  test_array.push_back(1);
  test_array.push_back(2);
  test_array.push_back(3);
  test_array.push_back(4);
  array_add("arr", test_array);
  
}

template<class num_t>
bool calculator_t<num_t>::interp(
  num_t* ap_value)
{
  num_t value = 0;
  // m_cur_lexeme_index = 0;
  bool fsuccess = eval_exp_compare(&value);
  if (fsuccess) {
    *ap_value = value;
  }
  return fsuccess;
}

template<class num_t>
bool calculator_t<num_t>::eval_exp_square_brackets(num_t* ap_value)
{
  bool fsuccess = true;
  token_t<num_t> token;
  fsuccess = m_detector_token.get_token(&token);
  const delimiter_t delim = token.delimiter();
  if (fsuccess) {
    if (delim == d_left_square_bracket) {
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = eval_exp_compare(ap_value);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        if (token == d_right_square_bracket) {
          // ����������� ���������� ������ �����������
          fsuccess = m_detector_token.next_token();
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

template<class num_t>
bool calculator_t<num_t>::eval_exp_compare(num_t* ap_value)
{
  bool fsuccess = true;
  num_t partial_value = 0;
  fsuccess = eval_exp_arithmetic_leve1(ap_value);
  if (fsuccess) {
    token_t<num_t> token;
    fsuccess = m_detector_token.get_token(&token);
    if (fsuccess) {
      delimiter_t delim = token.delimiter();
      if ((delim >= d_compare_equal) && (delim <= d_compare_greater_or_equal)) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_arithmetic_leve1(&partial_value);
        } else {
          // ��������� ������
        }
      } else {
        // ������� �� �������� ��������� ���������
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
            // ������� �� �������� ��������� ���������
          }
        }
      } else {
        // ��������� ������
      }
    } else {
      // ��������� ������
    }
  } else {
    // ��������� ������
  }
  return fsuccess;
}

template<class num_t>
bool calculator_t<num_t>::eval_exp_arithmetic_leve1(num_t* ap_value)
{
  bool fsuccess = true;
  num_t partial_value = 0;
  fsuccess = eval_exp_arithmetic_leve2(ap_value);
  if (fsuccess) {
    //token_t token = m_lexeme_array[m_cur_lexeme_index];
    token_t<num_t> token;
    fsuccess = m_detector_token.get_token(&token);
    delimiter_t delim = token.delimiter();
    if (fsuccess) {
      while(((delim == d_plus) || (delim == d_minus)) && fsuccess) {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_arithmetic_leve2(&partial_value);
        } else {
          // ��������� ������
        }
        if (fsuccess) {
          if (delim == d_plus) {
            *ap_value = *ap_value + partial_value;
          } else if (delim == d_minus) {
            *ap_value = *ap_value - partial_value;
          }
          //token = m_lexeme_array[m_cur_lexeme_index];
          fsuccess = m_detector_token.get_token(&token);
        } else {
          // ��������� ������
        }
        delim = token.delimiter();
      }
    } else {
      // ��������� ������ ��� ���������� �������
    }
  }
  return fsuccess;
}

template<class num_t>
bool calculator_t<num_t>::eval_exp_arithmetic_leve2(num_t* ap_value)
{
  bool fsuccess = true;
  num_t partial_value = 0;
  fsuccess = eval_exp_power(ap_value);
  if (fsuccess) {
    token_t<num_t> token;
    fsuccess = m_detector_token.get_token(&token);
    delimiter_t delim = token.delimiter();
    if (fsuccess) {
      while(fsuccess && (delim >= d_multiply) && (delim <= d_integer_division))
      {
        fsuccess = m_detector_token.next_token();
        if (fsuccess) {
          fsuccess = eval_exp_power(&partial_value);
        } else {
          // ��������� ������
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
          // ��������� ������
        }
        delim = token.delimiter();
      }
    }
  }
  return fsuccess;
}

template<class num_t>
bool calculator_t<num_t>::eval_exp_power(num_t* ap_value)
{
  bool fsuccess = true;
  num_t partial_value = 0;
  fsuccess = eval_exp_brackets(ap_value);
  if (fsuccess) {
    token_t<num_t> token;
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

// ������������ ��������� �� ��������
template<class num_t>
bool calculator_t<num_t>::eval_exp_brackets(num_t* ap_value)
{
  bool fsuccess = true;
  token_t<num_t> token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    const delimiter_t delim = token.delimiter();
    if (delim == d_left_parenthesis) {
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = eval_exp_compare(ap_value);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        if (token == d_right_parenthesis) {
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


template<class num_t>
bool calculator_t<num_t>::atom(num_t* ap_value)
{
  bool fsuccess = true;
  token_t<num_t> token;
  fsuccess = m_detector_token.get_token(&token);
  if (fsuccess) {
    if (token == tt_function) {
      const int id_func = token.get_id_function();
      fsuccess = m_detector_token.next_token();
      num_t arg_func = 0;
      if (fsuccess) {
        fsuccess = eval_exp_power(&arg_func);
      } else {
        // ��������� ������
      }
      /*if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }*/
      if (fsuccess) {
        func_exec(m_list_identifier.get_func(id_func),
          arg_func, ap_value);
      } else {
        // ��������� ������
      }
    } else if (token == tt_array) {
      const array_type* p_array = token.get_array();
      fsuccess = m_detector_token.next_token();
      if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }
      if (fsuccess) {
        if (token.token_type_get() != tt_delimiter) {
          fsuccess = false;
        } else {
          if (token.delimiter() != d_left_square_bracket) {
            fsuccess = false;
          } else {
            // ��������� ������� �������� ���������� �������
          }
        }
      } else {
        // ��������� ������
      }
      num_t elem_index;
      if (fsuccess) {

        fsuccess = eval_exp_square_brackets(&elem_index);
      } else {
        // ��������� ������
      }
      /*if (fsuccess) {
        fsuccess = m_detector_token.get_token(&token);
      } else {
        // ��������� ������
      }*/
      if (fsuccess) {
        fsuccess = array_elem_read(p_array, elem_index, ap_value);
      } else {
        // ��������� ������
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
          // ��������� ������
        }
      } else if (token == d_minus) {
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

template<class num_t>
void calculator_t<num_t>::func_exec(
  const function_t& a_function,
  const num_t a_in_param,
  num_t* ap_out_param)
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

template<class num_t>
bool calculator_t<num_t>::array_elem_read(const array_type* ap_array,
  const size_type a_elem_index,
  num_t* ap_array_elem) const
{
  bool fsuccess = true;
  if (a_elem_index < ap_array->size()) {
    *ap_array_elem = (*ap_array)[a_elem_index];
  } else {
    fsuccess = false;
  }
  return fsuccess;
}

template<class num_t>
bool calculator_t<num_t>::
  calc(const string_type* ap_prog, num_t* ap_num)
{
  m_detector_token.set_prog(ap_prog);
  return interp(ap_num);
}

template<class num_t>
inline int calculator_t<num_t>::find_function(
  const string_type& a_function_name) const
{
  return m_list_identifier.find_function(a_function_name);
}

template<class num_t>
inline int calculator_t<num_t>::find_num_const(
  const string& a_num_const_name) const
{
  return m_list_identifier.find_num_const(a_num_const_name);
}

template<class num_t>
inline bool calculator_t<num_t>::add_func_r_int_a_int(
  const string_type& a_name, func_r_int_a_int_ptr ap_function)
{
  return m_list_identifier.add_func_r_int_a_int(a_name, ap_function);
}

template<class num_t>
inline bool calculator_t<num_t>::add_func_r_float_a_float(
  const string_type& a_name, func_r_float_a_float_ptr ap_function)
{
  return m_list_identifier.add_func_r_float_a_float(a_name, ap_function);
}

template<class num_t>
inline bool calculator_t<num_t>::add_func_r_double_a_double(
  const string_type& a_name, func_r_double_a_double_ptr ap_function)
{
  return m_list_identifier.add_func_r_double_a_double(a_name, ap_function);
}

template<class num_t>
inline bool calculator_t<num_t>::add_func_r_ldouble_a_ldouble(
  const string_type& a_name, func_r_ldouble_a_ldouble_ptr ap_function)
{
  return m_list_identifier.add_func_r_ldouble_a_ldouble(a_name, ap_function);
}

template<class num_t>
inline void calculator_t<num_t>::num_const_add(
  const string_type& a_name, const num_t& a_value)
{
  m_list_identifier.num_const_add(make_pair(a_name, a_value));
}

template<class num_t>
inline bool calculator_t<num_t>::del_func(const string_type& a_name)
{
  return m_list_identifier.del_func(a_name);
}

template<class num_t>
inline void calculator_t<num_t>::num_const_del(const string_type& a_name)
{
  m_list_identifier.num_const_del(a_name);
}

template<class num_t>
void calculator_t<num_t>::clear_func()
{
  m_list_identifier.clear_func();
}

template<class num_t>
void calculator_t<num_t>::num_const_clear()
{
  m_list_identifier.num_const_clear();
}

template<class num_t>
inline void calculator_t<num_t>::array_add(const string_type& a_name,
  const array_type& a_array)
{
  m_list_identifier.array_add(make_pair(a_name, a_array));
}

template<class num_t>
inline void calculator_t<num_t>::array_del(const string_type& a_name)
{
  m_list_identifier.array_del(a_name);
}

template<class num_t>
void calculator_t<num_t>::array_clear()
{
  m_list_identifier.array_clear();
}

}; // namespace calc

#endif // new_calculator_valid
}; // namespace irs

#endif
