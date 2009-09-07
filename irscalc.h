//---------------------------------------------------------------------------
// Калькулятор
// Дата: 26.02.2009

#ifndef irscalcH
#define irscalcH

#ifndef __ICCAVR__ 

#include <iostream>
#include <irsstdg.h>
#include <irserror.h>
#include <math.h>
//---------------------------------------------------------------------------
namespace irs {
enum type_function_t {
  tf_first = 1,
  tf_r_int_a_int = tf_first,
  tf_r_float_a_float = tf_r_int_a_int+1,
  tf_r_double_a_double = tf_r_float_a_float+1,
  tf_r_ldouble_a_ldouble = tf_r_double_a_double+1,
  tf_last = tf_r_ldouble_a_ldouble};
class calculator_t
{
public:
  enum type_lexeme_t{tl_none, tl_number, tl_delimiter, tl_identifier};
  enum type_delimiter_t{
    td_none,
    td_plus,
    td_minus,
    td_multiply,
    td_division,
    td_integer_division,
    td_involution,
    td_left_bracket,
    td_right_bracket,
    td_comma};
  typedef double num_type;
private:
  irs::error_trans_base_t* mp_error_trans;
  const irs::string mstr_character_delimiter;
  const irs::string mstr_character_number;
  irs::string mstr_character_identifier;
  class lexeme_t
  {
    static const int m_none_id_func = -1;
    type_lexeme_t m_type_lexeme;
    num_type m_num;
    type_delimiter_t m_delimiter;
    int m_id_function;
  public:
    lexeme_t(
    ):
      m_type_lexeme(tl_none),
      m_num(static_cast<num_type>(0)),
      m_delimiter(td_none),
      m_id_function(m_none_id_func)
    {}
    void set_number(const num_type a_num)
    {
      m_type_lexeme = tl_number;
      m_num = a_num;
      m_delimiter = td_none;
      m_id_function = m_none_id_func;
    }
    void set_delimiter(const type_delimiter_t a_type_delimiter)
    {
      m_type_lexeme = tl_delimiter;
      m_num = static_cast<num_type>(0);
      m_delimiter = a_type_delimiter;
      m_id_function = m_none_id_func;
    }
    void set_id_function(const int a_id_function)
    {
      m_type_lexeme = tl_identifier;
      m_num = static_cast<num_type>(0);
      m_delimiter = td_none;
      m_id_function = a_id_function;
    }
    void set_not_a_type_lexeme()
    {
      m_type_lexeme = tl_none;
      m_num = static_cast<num_type>(0);
      m_delimiter = td_none;
      m_id_function = m_none_id_func;
    }
    bool operator==(const type_lexeme_t a_type_lexeme)
    {
      return (m_type_lexeme == a_type_lexeme);
    }
    bool operator==(const type_delimiter_t a_delimiter)
    {
      return (m_delimiter == a_delimiter);
    }
    int get_id_function()
    {
      return m_id_function;
    }
    num_type get_number()
    {
      return m_num;
    }
  };
  typedef int (*func_r_int_a_int_ptr)(int);
  typedef float (*func_r_float_a_float_ptr)(float);
  typedef double (*func_r_double_a_double_ptr)(double);
  typedef long double (*func_r_ldouble_a_ldouble_ptr)(long double);

  struct function_t
  {
    irs::string name;
    type_function_t type;
    void* ptr;
    union{
      int (*func_r_int_a_int_ptr)(int);
      float (*func_r_float_a_float_ptr)(float);
      double (*func_r_double_a_double_ptr)(double);
      long double (*func_r_ldouble_a_ldouble_ptr)(long double);
    };
  };
  struct num_const_t
  {
    irs::string name;
    num_type value;
  };
  vector<lexeme_t> m_lexeme_array;
  vector<function_t> m_id_func_array;
  vector<num_const_t> m_num_const_array;
  int m_cur_lexeme_index;
  bool parser(
    const irs::string& a_str, vector<lexeme_t>& lexeme_array) const;
  bool find(const irs::string& a_str, int& a_id_function) const;
  bool interp(calculator_t::num_type& a_value);
  // Обрабатывает сложение и вычитание
  bool eval_exp1(num_type& a_value);
  // Обрабатывает умножение, деление, целочисленное деление
  bool eval_exp2(num_type& a_value);
  // Обрабатывает возведение в степень
  bool eval_exp3(num_type& a_value);
  // Обрабатывает скобки
  bool eval_exp4(num_type& a_value);


  // Обрабатывает функции и числа
  bool atom(num_type& a_value);
  // Выполнение функцию
  void function_exec(
    const function_t& a_function,
    const num_type a_in_param,
    num_type& a_out_param);
  inline void next_lexeme();
  int find_function_name(const irs::string& a_function_name) const;
  int find_num_const_name(const irs::string& a_num_const_name) const;
  bool add_function(
    const irs::string& a_name,
    const type_function_t a_type,
    void* ap_function);
public:
  calculator_t();
  bool calc(const irs::string& a_str, num_type& a_num);

  inline bool add_func_r_int_a_int(
    const irs::string& a_name, func_r_int_a_int_ptr ap_function);
  inline bool add_func_r_float_a_float(
    const irs::string& a_name, func_r_float_a_float_ptr ap_function);
  inline bool add_func_r_double_a_double(
    const irs::string& a_name, func_r_double_a_double_ptr ap_function);
  inline bool add_func_r_ldouble_a_ldouble(
    const irs::string& a_name, func_r_ldouble_a_ldouble_ptr ap_function);

  inline bool add_num_const(const irs::string& a_name, const num_type& a_value);

};


  // Предварительная обработка стоки.
  // Заменяет все запятые на точки
  // Удаляет повторяющиеся пробелы,
  // переходы на следующую строку,
  // заменяет табуляции на пробелы.
  irs::string preprocessing_str(const irs::string& a_str);
}; // namespace irs

inline void irs::calculator_t::next_lexeme()
{
  m_cur_lexeme_index++;
  IRS_ASSERT_EX(m_cur_lexeme_index < static_cast<int>(m_lexeme_array.size()),
    "Выход за пределы массива лексем");
}

inline bool irs::calculator_t::add_func_r_int_a_int(
  const irs::string& a_name, func_r_int_a_int_ptr ap_function)
{
  //  добавлено "(void*)"
  return add_function(a_name, tf_r_int_a_int, (void*)ap_function);
}

inline bool irs::calculator_t::add_func_r_float_a_float(
  const irs::string& a_name, func_r_float_a_float_ptr ap_function)
{
  //  добавлено "(void*)"
  return add_function(a_name, tf_r_float_a_float, (void*)ap_function);
}

inline bool irs::calculator_t::add_func_r_double_a_double(
  const irs::string& a_name, func_r_double_a_double_ptr ap_function)
{
  //  добавлено "(void*)"
  return add_function(a_name, tf_r_double_a_double, (void*)ap_function);
}

inline bool irs::calculator_t::add_func_r_ldouble_a_ldouble(
  const irs::string& a_name, func_r_ldouble_a_ldouble_ptr ap_function)
{
  //  добавлено "(void*)"
  return add_function(a_name, tf_r_ldouble_a_ldouble, (void*)ap_function);
}

inline bool irs::calculator_t::add_num_const(
  const irs::string& a_name, const num_type& a_value)
{
  bool fsuccess = true;
  if ((find_function_name(a_name) == irs::npos) &&
    (find_num_const_name(a_name) ==  irs::npos))
  {
    num_const_t num_const;
    num_const.name = a_name;
    num_const.value = a_value;
    m_num_const_array.push_back(num_const);
  } else {
    fsuccess = false;
  }
  return fsuccess;
}

#endif //__ICCAVR__ 

#endif //irscalcH
