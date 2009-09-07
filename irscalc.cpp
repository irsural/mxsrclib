// Калькулятор
// Дата: 26.02.2009

#include <irscalc.h>

#ifndef __ICCAVR__

// class calculator_t
irs::calculator_t::calculator_t():
  mp_error_trans(irs::error_trans()),
  mstr_character_delimiter("+-*^/%()"),
  mstr_character_number("0123456789.E"),
  mstr_character_identifier(""),
  m_lexeme_array(),
  m_id_func_array(),
  m_num_const_array(),
  m_cur_lexeme_index(0)
{
  m_id_func_array.reserve(40);
  add_func_r_double_a_double("acos", acos);
  add_func_r_double_a_double("asin", asin);
  add_func_r_double_a_double("atan", atan);
  add_func_r_double_a_double("ceil", ceil);
  add_func_r_double_a_double("cos", cos);
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

  add_num_const("e", IRS_E);
  add_num_const("pi", IRS_PI);
}
bool irs::calculator_t::parser(
  const irs::string& a_str, vector<lexeme_t>& lexeme_array) const
{
  bool fsuccess = true;
  int cur_index = 0;
  lexeme_array.clear();
  int a_str_size = a_str.size();
  while(cur_index < a_str_size) {
    irs::string ch = a_str.substr(cur_index, 1);
    // проверяет ограничители
    if (ch.find_first_of(mstr_character_delimiter) != irs::string::npos) {
      const int lexeme_size = 1;
      lexeme_t lexeme;
      /*if (ch == "+" || ch == "-") {
        // Обработка множества подряд идущих знаков "+" и "-"
        cur_index++;
        if (cur_index < a_str_size) {
          irs::string ch_next = a_str.substr(cur_index, 1);
          while(cur_index < a_str_size) {
            if (ch_next == "+") {
              // nop :)
            } else if (ch_next == "-") {
              if (ch == "-") {
                ch = "+";
              } else {
                ch = "-";
              }
            } else {
              break;
            }
            cur_index++;
            ch = a_str.substr(cur_index, 1);
          }
        }
        if (ch == "+") {
          lexeme.set_delimiter(td_plus);
        } else {
          lexeme.set_delimiter(td_minus);
        }*/
      if (ch == "+") { lexeme.set_delimiter(td_plus);
      } else if (ch == "-") { lexeme.set_delimiter(td_minus);
      } else if (ch == "*") { lexeme.set_delimiter(td_multiply);
      } else if (ch == "/") { lexeme.set_delimiter(td_division);
      } else if (ch == "%") { lexeme.set_delimiter(td_integer_division);
      } else if (ch == "^") { lexeme.set_delimiter(td_involution);
      } else if (ch == "(") { lexeme.set_delimiter(td_left_bracket);
      } else if (ch == ")") { lexeme.set_delimiter(td_right_bracket);
      }

      cur_index += lexeme_size;
      lexeme_array.push_back(lexeme);
      // Читаем число
    } else if (ch.find_first_of(mstr_character_number) != irs::string::npos) {
      irs::string num_str = ch;
      cur_index++;
      //int lexeme_size = 1;
      if (cur_index < a_str_size) {
        bool sign_exp_detect_stat = false;
        bool sign_point_detect_stat = false;
        bool sign_plus_or_minus_detect_stat = false;
        while(cur_index < a_str_size) {
          irs::string ch = a_str.substr(cur_index, 1);
          if (ch.find_first_of("0123456789") != irs::string::npos) {
          } else if ((ch.find_first_of(".") != irs::string::npos) &&
            (!sign_point_detect_stat) &&
            (!sign_exp_detect_stat)) {
            sign_point_detect_stat = true;
          /*} else if ((ch.find_first_of("e") != irs::string::npos) &&
            (!sign_exp_detect_stat)) {
            sign_exp_detect_stat = true;*/
          } else if ((ch.find_first_of("E") != irs::string::npos) &&
            (!sign_exp_detect_stat)) {
            sign_exp_detect_stat = true;
          } else if (
            (ch.find_first_of("+-") != irs::string::npos) &&
            (!sign_plus_or_minus_detect_stat) &&
            sign_exp_detect_stat) {
            sign_plus_or_minus_detect_stat = true;
          } else {
            break;
          }
          num_str += ch;
          cur_index++;
        }
      }
      num_type num;
      IRS_LIB_ASSERT(num_str.to_number(num));
      lexeme_t lexeme;
      lexeme.set_number(num);
      lexeme_array.push_back(lexeme);
      // читаем идентификатор
    } else if (isalpha(static_cast<int>(ch[0]))) {
      irs::string identifier_str = ch;
      cur_index++;
      if (cur_index < a_str_size) {
        irs::string ch = a_str.substr(cur_index, 1);
        while(cur_index < a_str_size) {
          if (isalpha(static_cast<int>(ch[0])) ||
            isdigit(static_cast<int>(ch[0]))) {
            identifier_str += ch;
          } else {
            break;
          }
          cur_index++;
          ch = a_str.substr(cur_index, 1);
        }
      }
      int func_pos = find_function_name(identifier_str);

      if (func_pos != irs::npos) {
        lexeme_t lexeme;
        lexeme.set_id_function(func_pos);
        lexeme_array.push_back(lexeme);
      } else {
        int num_const_pos = find_num_const_name(identifier_str);
        if (num_const_pos != irs::npos) {
          num_const_t num_counst = m_num_const_array[num_const_pos];
          num_type num = num_counst.value;
          lexeme_t lexeme;
          lexeme.set_number(num);
          lexeme_array.push_back(lexeme);
        } else {
          fsuccess = false;
          lexeme_array.clear();
          return fsuccess;
        }
      }
      // пропускаем неизвестный символ
    } else {
      cur_index++;
    }
  }
  if (fsuccess) {
    lexeme_t lexeme;
    lexeme.set_not_a_type_lexeme();
    lexeme_array.push_back(lexeme);
  }
  return fsuccess;
}
/*bool irs::calculator_t::find(
  const irs::string& a_str, int& a_id_function) const
{
  bool fsuccess = false;
  int size = m_id_func_array.size();
  for (int i = 0; i < size; i++) {
    if (a_str == m_id_func_array[i].name) {
      a_id_function = i;
      fsuccess = true;
      break;
    }
  }
  return fsuccess;
} */
bool irs::calculator_t::interp(
  calculator_t::num_type& a_value)
{
  num_type value = 0;
  m_cur_lexeme_index = 0;
  bool fsuccess = eval_exp1(value);
  if (fsuccess) {
    a_value = value;
  }
  return fsuccess;
}
bool irs::calculator_t::calc(const irs::string& a_str, num_type& a_num)
{
  bool fsuccess = parser(a_str, m_lexeme_array);
  if (fsuccess) {
    fsuccess = interp(a_num);
  }
  return fsuccess;
}

bool irs::calculator_t::eval_exp1(calculator_t::num_type& a_value)
{
  bool fsuccess = true;
  num_type partial_value = 0;
  fsuccess = eval_exp2(a_value);
  if (fsuccess) {
    lexeme_t lexeme = m_lexeme_array[m_cur_lexeme_index];
    while((lexeme == td_plus) || (lexeme == td_minus)) {
      next_lexeme();
      fsuccess = eval_exp2(partial_value);
      if (!fsuccess) {
        break;
      }
      if (lexeme == td_plus) {
        a_value = a_value + partial_value;
      } else if (lexeme == td_minus) {
        a_value = a_value - partial_value;
      }
      lexeme = m_lexeme_array[m_cur_lexeme_index];
    }
  }
  return fsuccess;
}

bool irs::calculator_t::eval_exp2(num_type& a_value)
{
  bool fsuccess = true;
  num_type partial_value = 0;
  fsuccess = eval_exp3(a_value);
  if (fsuccess) {
    lexeme_t lexeme = m_lexeme_array[m_cur_lexeme_index];
    while(
      (lexeme == td_multiply) ||
      (lexeme == td_division) ||
      (lexeme == td_integer_division)) {
      next_lexeme();
      fsuccess = eval_exp3(partial_value);
      if (!fsuccess) {
        break;
      }
      if (lexeme == td_multiply) {
        a_value = a_value * partial_value;
      } else if (lexeme == td_division) {
        if (partial_value != 0) {
          a_value = a_value / partial_value;
        } else {
          fsuccess = false;
        }
      } else if (lexeme == td_integer_division) {
        a_value = static_cast<int>(a_value) % static_cast<int>(partial_value);
      }
      lexeme = m_lexeme_array[m_cur_lexeme_index];
    }
  }
  return fsuccess;
}

bool irs::calculator_t::eval_exp3(num_type& a_value)
{
  bool fsuccess = true;
  num_type partial_value = 0;
  fsuccess = eval_exp4(a_value);
  if (fsuccess) {
    lexeme_t lexeme = m_lexeme_array[m_cur_lexeme_index];
    while(lexeme == td_involution) {
      next_lexeme();
      fsuccess = eval_exp4(partial_value);
      if (!fsuccess) {
        break;
      }
      if (lexeme == td_involution) {
        a_value = pow(a_value, partial_value);
      }
      lexeme = m_lexeme_array[m_cur_lexeme_index];
    }
  }
  return fsuccess;
}

// обрабатывает выражения со скобками
bool irs::calculator_t::eval_exp4(num_type& a_value)
{
  bool fsuccess = true;
  lexeme_t lexeme = m_lexeme_array[m_cur_lexeme_index];
  if (lexeme == td_left_bracket) {
    next_lexeme();
    fsuccess = eval_exp1(a_value);
    if (fsuccess) {
      next_lexeme();
    }
  } else {
    fsuccess = atom(a_value);
  }
  return fsuccess;
}



bool irs::calculator_t::atom(num_type& a_value)
{
  bool fsuccess = true;
  lexeme_t lexeme = m_lexeme_array[m_cur_lexeme_index];
  if (lexeme == tl_identifier) {
    next_lexeme();
    num_type result = 0;
    fsuccess = eval_exp3(result);
    if (fsuccess) {
      function_exec(m_id_func_array[lexeme.get_id_function()], result, a_value);
    } else {
      fsuccess = false;
    }
  } else if (lexeme == tl_number) {
    a_value = lexeme.get_number();
    next_lexeme();
  } else if (lexeme == tl_delimiter) {
    if (lexeme == td_plus) {
      next_lexeme();
      fsuccess = atom(a_value);
    } else if (lexeme == td_minus) {
      next_lexeme();
      fsuccess = atom(a_value);
      a_value *= -1;
    } else {
      fsuccess = false;
    }
  } else {
    fsuccess = false;
  }
  return fsuccess;
}

void irs::calculator_t::function_exec(
  const function_t& a_function,
  const num_type a_in_param,
  num_type& a_out_param)
{
  void* ptr_function = a_function.ptr;
  switch(a_function.type) {
    case tf_r_int_a_int: {
      a_out_param =
        (*((func_r_int_a_int_ptr)(ptr_function)))(static_cast<int>(a_in_param));
    } break;
    case tf_r_float_a_float: {
      a_out_param =
        (*((func_r_float_a_float_ptr)(ptr_function)))
        (static_cast<float>(a_in_param));
    } break;
    case tf_r_double_a_double: {
      a_out_param =
        (*((func_r_double_a_double_ptr)(ptr_function)))(a_in_param);
    } break;
    case tf_r_ldouble_a_ldouble: {
      a_out_param = static_cast<double>(
        (*((func_r_ldouble_a_ldouble_ptr)(ptr_function)))(a_in_param)
      );
    } break;
  }
}

int irs::calculator_t::find_function_name(
  const irs::string& a_function_name) const
{
  int position = irs::npos;
  int function_count = m_id_func_array.size();
  for (int index_func = 0; index_func < function_count; index_func++) {
    if (m_id_func_array[index_func].name == a_function_name) {
      position = index_func;
      break;
    }
  }
  return position;
}

int irs::calculator_t::find_num_const_name(
  const irs::string& a_num_const_name) const
{
  int position = irs::npos;
  int num_const_count = m_num_const_array.size();
  for (int i = 0; i < num_const_count; i++) {
    if (m_num_const_array[i].name == a_num_const_name) {
      position = i;
      break;
    }
  }
  return position;
}

bool irs::calculator_t::add_function(
  const irs::string& a_name,
  const type_function_t a_type,
  void* ap_function)
{
  bool fsuccess = true;
  if (find_function_name(a_name) != irs::npos) {
    fsuccess = false;
  } else {
    function_t function;
    function.name = a_name;
    function.type = a_type;
    function.ptr = ap_function;
    m_id_func_array.push_back(function);
  }
  return fsuccess;
}



irs::string irs::preprocessing_str(const irs::string& a_str)
{
  irs::string str = a_str;
  //Удаляем пробелы
  /*int pos_ch_space = str.find(' ');
  while (pos_ch_space != irs::string::npos) {
    str.erase(pos_ch_space, 1);
    pos_ch_space = str.find(' ');
  }*/
  typedef irs::string::size_type str_size_t;
  // Удаляем переход на новую строку
  str_size_t pos_ch_return = str.find("\r");
  while (pos_ch_return != irs::string::npos) {
    str.erase(pos_ch_return, 2);
    pos_ch_return = str.find("\r");
  }
  // Удаляем табуляции
  str_size_t pos_ch_tab = str.find("\t");
  while (pos_ch_tab != irs::string::npos) {
    str.erase(pos_ch_tab, 2);
    pos_ch_tab = str.find("\t");
  }
  // Заменяем запятые на точки
  str_size_t pos_ch_decimal_point = str.find(",");
  while (pos_ch_decimal_point != irs::string::npos) {
    str.replace(pos_ch_decimal_point, 1, ".");
    pos_ch_decimal_point = str.find(",");
  }


  return str;
}

#endif //__ICCAVR__
