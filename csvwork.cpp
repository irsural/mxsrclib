//! \file
//! \ingroup file_in_out_group
//! \brief Работа с csv-файлами
//!
//! Дата: 16.09.2010\n
//! Ранняя дата: 17.09.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

//#include <assert.h>

#include <csvwork.h>
#include <irsstrdefs.h>

#include <irsfinal.h>

//---------------------------------------------------------------------------

//***************************************************************************
// Класс csv_file - Запсь csv-файлов

//---------------------------------------------------------------------------
// Конструктор
csv_file::csv_file():
  //m_lines_writed_count(0),
  m_is_first_line(true),
  m_pars(),
  m_vars(),
  //mp_ini_file(0)
  m_csv_file()
{
}
//---------------------------------------------------------------------------
// Деструктор
csv_file::~csv_file()
{
  close();
}
//---------------------------------------------------------------------------
// Запись в list_type подобно map[]
void csv_file::assign_elem(list_type &a_elem_array,
  const string_type& a_name, const elem_t &a_elem)
{
  string_type name_str = a_name;
  list_it_type it = find_if(a_elem_array.begin(), a_elem_array.end(),
    pair_first_equal(name_str));
  if (it == a_elem_array.end()) {
    a_elem_array.emplace_back(name_str, a_elem);
  } else {
    it->second = a_elem;
  }
}
//---------------------------------------------------------------------------
// Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::add_par(const string_type& par_name, e_val_type val_type,
  var_val_t par_value)
{
  assign_elem(m_pars, par_name, elem_t(val_type, par_value));
  return irs_true;
}
//---------------------------------------------------------------------------
// Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::add_par(const string_type& par_name, irs_i32 par_value)
{
  var_val_t par_value_vvt;
  par_value_vvt.uf_i32_type = par_value;
  return add_par(par_name, ec_i32_type, par_value_vvt);
}
//---------------------------------------------------------------------------
// Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::add_par(const string_type& par_name, float par_value)
{
  var_val_t par_value_vvt;
  par_value_vvt.uf_float_type = par_value;
  return add_par(par_name, ec_float_type, par_value_vvt);
}
//---------------------------------------------------------------------------
// Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::add_par(const string_type& par_name,
  const string_type& par_value)
{
  var_val_t par_value_vvt;
  par_value_vvt.uf_str_type = par_value;
  return add_par(par_name, ec_float_type, par_value_vvt);
}
//---------------------------------------------------------------------------
// Поиск елемента в list_type
csv_file::list_it_type csv_file::find_elem(list_type &a_elem_array,
  const string_type& a_name)
{
  string_type name_str = a_name;
  return find_if(a_elem_array.begin(), a_elem_array.end(),
    pair_first_equal(name_str));
}
//---------------------------------------------------------------------------
// Запись переменной, при ошибке возвращает irs_false
irs_bool csv_file::set_var(const string_type& var_name, var_val_t var_value)
{
  //list_it_type it = m_vars.find(var_name);
  list_it_type it = find_elem(m_vars, var_name);

  if (it != m_vars.end()) {
    switch (it->second.type) {
      case ec_i32_type:
        it->second.val.uf_i32_type = var_value.uf_i32_type;
        break;
      case ec_float_type:
        it->second.val.uf_float_type = var_value.uf_float_type;
        break;
      case ec_str_type:
        it->second.val.uf_str_type = var_value.uf_str_type;
        break;
      default:
        //int invalid_type = 0;
        //assert(invalid_type);
        break;
    }
    return irs_true;
  } else {
    return irs_false;
  }
}
//---------------------------------------------------------------------------
// Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::set_var(const string_type& var_name, irs_i32 var_value)
{
  var_val_t var_value_vvt;
  var_value_vvt.uf_i32_type = var_value;
  return set_var(var_name, var_value_vvt);
}
//---------------------------------------------------------------------------
// Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::set_var(const string_type& var_name, float var_value)
{
  var_val_t var_value_vvt;
  var_value_vvt.uf_float_type = var_value;
  return set_var(var_name, var_value_vvt);
}
//---------------------------------------------------------------------------
// Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::set_var(const string_type& var_name,
  const string_type& var_value)
{
  var_val_t var_value_vvt;
  var_value_vvt.uf_str_type = var_value;
  return set_var(var_name, var_value_vvt);
}
//---------------------------------------------------------------------------
// Добавить название столбца csv-файла, при ошибке возвращает irs_false
irs_bool csv_file::add_col(const string_type& col_name, e_val_type val_type)
{
  assign_elem(m_vars, col_name, elem_t(val_type, var_val_t()));
  return irs_true;
}
//---------------------------------------------------------------------------
// Открыть файл name, при ошибке возвращает irs_false
irs_bool csv_file::open(const string_type& file_name)
{
  if (m_csv_file.is_open()) return irs_true;

  m_is_first_line = true;

  m_csv_file.open(IRS_SIMPLE_FROM_TYPE_STR(file_name.c_str()));
  irs_bool is_success = irs_false;
  if (m_csv_file) {
    is_success = irs_true;
  }
  return is_success;
}
//---------------------------------------------------------------------------
// Закрыть файл
irs_bool csv_file::close()
{
  if (m_csv_file.is_open()) {
    m_csv_file.close();
    if (!m_csv_file) {
      return irs_false;
    }
  }
  return irs_true;
}
//---------------------------------------------------------------------------
// Запись строки в csv-файл, при превышении количества записей возвращает
// irs_false
irs_bool csv_file::write_line()
{
  // (Сейчас лимит выключен) Обработка лимита по количеству строк
  //if (m_lines_writed_count >= max_line_count) return irs_false;
  //m_lines_writed_count++;

  if (m_is_first_line) {
    m_is_first_line = false;

    // Список параметров в начале файла
    for (list_it_type it = m_pars.begin(); it != m_pars.end(); it++) {
      //fprintf(mp_ini_file, "%s", it->first.c_str());
      m_csv_file << it->first << ';';
      switch (it->second.type) {
        case ec_i32_type: {
          //fprintf(mp_ini_file, ";%ld\n", it->second.val.uf_i32_type);
          m_csv_file << it->second.val.uf_i32_type << irst('\n');
        } break;
        case ec_float_type: {
          ostrstream strm;
          strm << setprecision(30) << it->second.val.uf_float_type << ends;
          char *sbeg = strm.str();
          char *send = sbeg + strm.pcount();
          ::replace(sbeg, send, '.', ',');
          //fprintf(mp_ini_file, sbeg);
          m_csv_file << IRS_TYPE_FROM_SIMPLE_STR(sbeg) << irst('\n');
          strm.rdbuf()->freeze(false);
        } break;
        case ec_str_type: {
          //fprintf(mp_ini_file, ";%s\n", it->second.val.uf_str_type.c_str());
          m_csv_file << it->second.val.uf_str_type << irst('\n');
        }  break;
        default: {
          IRS_LIB_ASSERT_MSG("Неверный тип в функции csv_file::write_line");
          //int invalid_type = 0;
          //assert(invalid_type);
        } break;
      }
      //fprintf(mp_ini_file, "*\n");
      m_csv_file << irst('\n');
    }

    // Строка с названиями переменных
    for (list_it_type it = m_vars.begin(); it != m_vars.end(); it++) {
      //fprintf(mp_ini_file, "%s;", it->first.c_str());
      m_csv_file << it->first << irst(';');
    }
    //fprintf(mp_ini_file, "\n");
    m_csv_file << irst('\n');
  }

  // Строка со значениями переменных
  for (list_it_type it = m_vars.begin(); it != m_vars.end(); it++) {
    switch (it->second.type) {
      case ec_i32_type: {
        //fprintf(mp_ini_file, "%ld;", it->second.val.uf_i32_type);
        m_csv_file << it->second.val.uf_i32_type;
      } break;
      case ec_float_type: {
        ostrstream strm;
        strm << setprecision(30) << it->second.val.uf_float_type << ends;
        char *sbeg = strm.str();
        char *send = sbeg + strm.pcount();
        replace(sbeg, send, '.', ',');
        //fprintf(mp_ini_file, sbeg);
        m_csv_file << IRS_TYPE_FROM_SIMPLE_STR(sbeg);
        strm.rdbuf()->freeze(false);
      } break;
      case ec_str_type: {
        //fprintf(mp_ini_file, "%s;", it->second.val.uf_str_type.c_str());
        m_csv_file << it->second.val.uf_str_type;
      } break;
      default: {
        IRS_LIB_ASSERT_MSG("Неверный тип в функции csv_file::write_line");
        //int invalid_type = 0;
        //assert(invalid_type);
      } break;
    }
    m_csv_file << irst(';');
  }
  //fprintf(mp_ini_file, "\n");
  m_csv_file << irst('\n');

  return true;
}
//---------------------------------------------------------------------------
// Очистить столбцы
void csv_file::clear_cols()
{
  m_vars.clear();
}
//---------------------------------------------------------------------------
// Очистить столбцы
void csv_file::clear_pars()
{
  m_pars.clear();
}
//---------------------------------------------------------------------------

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

//class csv_file_t
irs::csvwork::csv_file_t::csv_file_t(const string_type& a_filename
):
  m_delimiter_col(irst(';')),
  m_delimiter_row(irst('\n')),
  m_delim_row(irst("\r\n")),
  m_delimiter_row_size(m_delim_row.size()),
  //m_delimiter_size(2),
  //m_pos(0),
  m_filename(a_filename.c_str()),
  m_file(),
  m_progress(0.),
  m_status(cfs_success),
  m_cur_instruction(ci_calc_col_count),
  m_processing_status(ps_busy_command),
  m_command(cmd_none_command),
  m_delimiter_col_count(0),
  m_delimiter_col_count_cur_row(0),
  m_delimiter_col_count_sum(0),
  //m_delimiter_col_count_mean(0),
  m_delimiter_row_count(0),
  m_ch_count_row_mean(0),
  mp_col_count_user(IRS_NULL),
  mp_row_count_user(IRS_NULL),
  mp_row_user(IRS_NULL),
  m_row_user_buf(),
  mp_table_user(IRS_NULL),
  m_table_uses_col_count(0),
  m_table_uses_row_count(0),
  m_table_uses_cell_count(0),
  m_cell_index(0),
  m_pos_file(0),
  m_col_index(0),
  m_row_index(0),
  m_pos_mode(pos_mode_beg),
  m_row_pos_file(0),
  m_quote(irst('"')),
  m_quote_count(0)
{
  m_special_character = static_cast<string_type>(m_delimiter_col) +
      static_cast<string_type>(m_quote);
}

irs::csvwork::csv_file_t::~csv_file_t()
{
  m_file.close();
}

void irs::csvwork::csv_file_t::tick()
{
  switch (m_processing_status) {
    case ps_busy_command: {
      if (m_processing_status == ps_busy_command) {
        if (m_command != cmd_none_command) {
          m_pos_file = 0;
          m_progress = 0.;
          m_delimiter_col_count = 0;
          m_delimiter_col_count_cur_row = 0;
          m_delimiter_row_count = 0;
          m_quote_count = 0;
          m_delimiter_col_count_sum = 0;
          m_cell_index = 0;
          m_col_index = 0;
          if (m_command != cmd_get_row) {
            m_row_index = 0;
          }
          m_cell_buf.clear();
          m_ch_count_row_mean = 0;
        }
        switch (m_command) {
          case cmd_get_row:
          case cmd_calc_col_count:
          case cmd_calc_row_count: {
            m_file.open(IRS_SIMPLE_FROM_TYPE_STR(m_filename.c_str()), ios::in);
            if (m_file) {
              m_file.seekg(0, ios::end);
              m_file_size = m_file.tellg();
              m_file.seekg(0, ios::beg);
              m_processing_status = ps_run_instruction;
            } else {
              set_mode_error();
            }
          } break;
          case cmd_rows_erase: {
          } break;
          case cmd_row_push_back: {
            bool success = true;
            // Создаем файл если не существует
            m_file.open(IRS_SIMPLE_FROM_TYPE_STR(m_filename.c_str()),
              ios::out | ios::app | ios::binary);
            success = m_file ? success : false;
            if (success) {
              m_file.seekp(0, ios::end);
              m_file_size = m_file.tellp();
              success = m_file ? success : false;
            } else {
              // Произошла ошибка
            }
            if (success) {
              if (m_file_size == 0) {
                m_file << m_delim_row;
              } else {
                // Файл уже существовал и в нем уже есть записи
              }
            } else {
              // Произошла ошибка
            }
            m_file.close();
            // Открываем файл для чтения/записи
            if (success) {
              m_file.open(IRS_SIMPLE_FROM_TYPE_STR(m_filename.c_str()),
                ios::in | ios::out | ios::binary);
              success = m_file ? success : false;
            } else {
              // Произошла ошибка
            }
            if (success) {
              m_file.seekp(0, ios::end);
              success = m_file ? success : false;
            } else {
              // Произошла ошибка
            }
            if (success) {
              m_file_size = m_file.tellp();
              success = m_file ? success : false;
            } else {
              // Произошла ошибка
            }
            if (success) {
              if (m_file_size >= m_delim_row.size()) {
                m_file.seekg(-static_cast<fstream::pos_type>(
                  m_delimiter_row_size), ios::end);
                success = m_file ? success : false;
                char_type* p_buf = new char_type[m_delimiter_row_size + 1];
                if (success) {
                  m_file.read(p_buf, m_delim_row.size());
                  p_buf[m_delimiter_row_size] = irst('\0');
                  success = m_file ? success : false;
                } else {
                  // Произошла ошибка
                }
                if (success) {
                  if (m_delim_row == string_type(p_buf)) {
                    if (m_file_size == m_delimiter_row_size) {
                      m_file.seekp(0, ios::beg);
                    } else {
                      m_file.seekp(0, ios::end);
                    }
                    success = m_file ? success : false;
                  } else {
                    // Некорректный файл
                    success = false;
                  }
                } else {
                  // Произошла ошибка
                }
              } else {
                // Некорректный файл
                success = false;
              }
            } else {
              // Произошла ошибка
            }
            if (success) {
              m_processing_status = ps_run_instruction;
            } else {
              set_mode_error();
            }
          } break;
          case cmd_save : {
            m_file.open(IRS_SIMPLE_FROM_TYPE_STR(m_filename.c_str()),
              ios::in | ios::out | ios::trunc);
            if (m_file) {
              m_processing_status = ps_run_instruction;
            } else {
              set_mode_error();
            }
          } break;
          case cmd_load: {
            mp_table_user->clear();
            m_file.open(IRS_SIMPLE_FROM_TYPE_STR(m_filename.c_str()),
              ios::in);
            if (m_file) {
              m_file.seekg(0, ios::end);
              m_file_size = m_file.tellg();
              m_file.seekg(0, ios::beg);
              m_processing_status = ps_run_instruction;
            } else {
              set_mode_error();
            }
          } break;
          case cmd_none_command:{
          } break;
        }
      } else {
        m_command = cmd_none_command;
      }
    } break;
    case ps_run_instruction: {
      switch (m_cur_instruction) {
        case ci_calc_col_count: {
          // Читаем до EOF или ошибки
          if (m_file.good()) {
            char_type ch = 0;
            m_file.get(ch);
            m_pos_file++;
            if (ch == m_delimiter_col) {
              if (m_quote_count % 2 == 0) {
                m_delimiter_col_count_cur_row++;
                m_quote_count = 0;
              }
            } else if (ch == m_delimiter_row) {
              m_delimiter_col_count =
                max(m_delimiter_col_count, m_delimiter_col_count_cur_row);
              m_delimiter_col_count_cur_row = 0;
              m_quote_count = 0;
            } else if (ch == m_quote) {
              m_quote_count++;
            }
            if (m_file_size > 0) {
              m_progress = (m_pos_file / static_cast<double>(m_file_size));
            } else {
              m_progress = 1.;
            }
            // Если достигнут EOF
          } else if (m_file.eof()) {
            IRS_LIB_ASSERT(mp_col_count_user != IRS_NULL);
            if (mp_col_count_user != IRS_NULL) {
              if (m_file_size > 0) {
                *mp_col_count_user = m_delimiter_col_count + 1;
              } else {
                *mp_col_count_user = 0;
              }
            }
            set_mode_success();
            // Если есть ошибки, но EOF не установлен
          } else {
            set_mode_error();
          }
        } break;
        case ci_calc_row_count: {
          // Читаем до EOF или ошибки
          if (m_file.good()) {
            char_type ch = irst('\0');
            m_file.get(ch);
            m_pos_file++;
            if (ch == m_delimiter_row) {
              m_delimiter_row_count++;
            }
            if (m_file_size > 0) {
              m_progress = (m_pos_file / static_cast<double>(m_file_size));
            } else {
              m_progress = 1.;
            }
            // Если достигнут EOF
          } else if (m_file.eof()) {
            IRS_LIB_ASSERT(mp_row_count_user != IRS_NULL);
            if (mp_row_count_user != IRS_NULL) {
              *mp_row_count_user = m_delimiter_row_count;
            }
            set_mode_success();
            // Если есть ошибки, но EOF не установлен
          } else {
            set_mode_error();
          }
        } break;
        case ci_rows_erase: {

        } break;
        case ci_row_push_back: {
          bool fsuccess = true;
          const size_type max_cell_count_pack = 10;
          size_type row_user_size = mp_row_user->size();
          size_type i_end =
            min(row_user_size - m_cell_index, max_cell_count_pack);
          for (size_type cell_i = 0; cell_i < i_end; cell_i++) {
            if (!m_file.good()) {
              fsuccess = false;
              break;
            } else {
              if (m_cell_index > 0) {
                m_file << m_delimiter_col;
                  /*reinterpret_cast<const char_type*>(&m_delimiter_col),
                  sizeof(m_delimiter_col));*/
              }
              std::vector<string_type>::iterator it_cell =
                ((*mp_row_user).begin() + m_cell_index);
              if((*it_cell).find_first_of(
                m_special_character) == string_type::npos) {
                m_file << it_cell->c_str();
              } else {
                string_type cell_modified;
                size_type cell_size = (*it_cell).size();
                for (size_type i = 0; i < cell_size; i++) {
                  if ((*it_cell)[i] == m_quote) {
                    cell_modified += m_quote;
                  }
                  cell_modified += (*it_cell)[i];
                }
                cell_modified = irst("\"") + cell_modified + irst("\"");
                m_file << cell_modified.c_str();
              }
              m_cell_index++;
            }
          }
          if (row_user_size > 0) {
            m_progress = (m_cell_index / static_cast<double>(row_user_size));
          } else {
            // Нет данных для записи
          }
          if (fsuccess) {
            if (m_cell_index == row_user_size) {
              if (row_user_size > 0) {
                m_file << m_delim_row;
              } else {
                // Никаких дополнительных действий не требуется
              }
              m_row_user_buf.clear();
              if (m_file.good()) {
                set_mode_success();
              } else {
                set_mode_error();
              }
            } else if (m_cell_index > row_user_size) {
              IRS_LIB_ASSERT_MSG("Выход за пределы массива");
              fsuccess = false;
            } else {
              // Запись не завершена
            }
          } else {
            // Произошла ошибка
          }
          if (!fsuccess) {
            m_row_user_buf.clear();
            set_mode_error();
          } else {
            // Ошибок не произошло
          }
        } break;
        case ci_find_row_pos: {
          // Читаем до EOF или ошибки или до нужной строки pos
          if ((m_file.good()) && (m_delimiter_row_count < m_row_index)) {
            char_type ch = irst('\0');
            m_file.get(ch);
            m_pos_file++;
            if (ch == m_delimiter_row) {
              m_delimiter_row_count++;
              m_ch_count_row_mean = m_pos_file / m_delimiter_row_count;

            }
            if (m_file_size > 0) {
              if (m_ch_count_row_mean > 0) {
                m_progress = (m_pos_file /
                  static_cast<double>(m_row_index * m_ch_count_row_mean +
                  m_ch_count_row_mean));
                if (m_progress > 0.9) {
                  m_progress = 0.9;
                }
              } else {
                m_progress = (m_pos_file / static_cast<double>(m_file_size));
              }
            } else {
              m_progress = 0.9;
            }
            // Если достигнут EOF
          } else if (m_delimiter_row_count >= m_row_index) {
            m_row_pos_file = m_file.tellg();
            m_cur_instruction = ci_get_row;
          } else {
            set_mode_error();
          }
        } break;
        case ci_get_row: {
          bool instruction_success = true;
          bool on_add_ch = false;
          // Читаем до EOF
          if (m_file.good()) {
            char_type ch = irst('\0');
            m_file.get(ch);
            m_pos_file++;
            m_progress = (m_pos_file /
              static_cast<double>(m_row_index * m_ch_count_row_mean +
              m_ch_count_row_mean));

            if (m_progress > 0.99) {
              m_progress = 0.99;
            }
            if (ch == m_delimiter_col) {
              if (m_quote_count % 2 == 0) {
                if (m_quote_count != 0) {
                  vector<string_type>::iterator it_cell_str =
                    mp_row_user->begin() + m_cell_index;
                  *it_cell_str = it_cell_str->substr(1, it_cell_str->size()-2);
                  string_type buf = *it_cell_str;
                  size_type index = 0;
                  size_type index_end = it_cell_str->size();
                  while (index < index_end) {
                    if ((*it_cell_str)[index] == m_quote) {
                      index++;
                      if (((*it_cell_str)[index] == m_quote) &&
                        (index < index_end))
                      {
                        it_cell_str->erase(index-1, 1);
                        index_end = index_end - 1;
                      } else {
                        instruction_success = false;
                        break;
                      }
                    } else {
                      index++;
                    }
                  }
                } else {
                  if (mp_row_user->size() < (m_cell_index+1))
                  {
                    mp_row_user->resize(m_cell_index+1);
                  }
                }
                m_cell_index++;
                m_quote_count = 0;
              } else {
                on_add_ch = true;
              }
            } else if (ch == m_delimiter_row) {
              if (mp_row_user->size() < (m_cell_index+1))
              {
                mp_row_user->resize(m_cell_index+1);
              }
              set_mode_success();
            } else if (ch == m_quote) {
              on_add_ch = true;
              m_quote_count++;
            } else {
              on_add_ch = true;
            }
            if (on_add_ch){
              if (mp_row_user->size() < (m_cell_index+1)) {
                mp_row_user->resize(m_cell_index+1);
              }
              (*mp_row_user)[m_cell_index] += ch;
              on_add_ch = false;
            }
          } else {
            instruction_success = false;
          }
          if (!instruction_success) {
            mp_row_user->clear();
            set_mode_error();
          }
        } break;
        case ci_save: {
          bool instruction_success = true;
          IRS_LIB_ASSERT_EX(
            m_table_uses_col_count == mp_table_user->get_col_count() &&
            m_table_uses_row_count == mp_table_user->get_row_count(),
            "Изменение таблины, во время сохранения в файл, запрещено");
          //bool fsuccess = true;
          const size_type max_cell_count_pack = 10;
          //int row_user_size = mp_row_user->size();
          size_type i_end = min(
            m_table_uses_cell_count - m_cell_index, max_cell_count_pack);
          for (size_type cell_i = 0; cell_i < i_end; cell_i++) {
            if (!m_file.good()) {
              instruction_success = false;
            } else {
              if (m_col_index > 0) {
                m_file << m_delimiter_col;
              }
              string_type cell = mp_table_user->read_cell(
                m_col_index, m_row_index);
              if(cell.find_first_of(
                m_special_character) == string_type::npos)
              {
                m_file << cell;
              } else {
                string_type cell_modified;
                const size_type cell_size = cell.size();
                for (size_type i = 0; i < cell_size; i++) {
                  if (cell[i] == m_quote) {
                    cell_modified += m_quote;
                  }
                  cell_modified += cell[i];
                }
                m_file << irst("\"") << cell_modified << irst("\"");
              }
              m_cell_index++;
              m_col_index++;
              if (m_col_index >= m_table_uses_col_count) {
                m_file << m_delimiter_row;
                m_col_index = 0;
                m_row_index++;
              }
            }
          }
          if (m_table_uses_cell_count > 0) {
            m_progress =
              m_cell_index / static_cast<double>(m_table_uses_cell_count);
          }
          if (m_cell_index >= m_table_uses_cell_count) {
            m_row_user_buf.clear();
            if (m_file.good()) {
              set_mode_success();
            } else {
              set_mode_error();
            }
          } else if (!instruction_success) {
            m_row_user_buf.clear();
            set_mode_error();
          }
        } break;
        case ci_load: {
          bool instruction_success = true;
          bool on_add_ch = false;
          // Читаем до EOF
          if (m_file.good()) {
            char_type ch = irst('\0');
            m_file.get(ch);
            m_pos_file++;
            if (m_file_size > 0) {
              m_progress = m_pos_file / static_cast<double>(m_file_size);
            }
            if ((ch == m_delimiter_col) || (ch == m_delimiter_row)) {
              if (m_quote_count % 2 == 0) {
                if (m_quote_count != 0) {
                  m_cell_buf = m_cell_buf.substr(1, m_cell_buf.size()-2);
                  size_type index = 0;
                  size_type m_cell_buf_size = m_cell_buf.size();
                  while (index < m_cell_buf_size) {
                    if (m_cell_buf[index] == m_quote) {
                      index++;
                      if ((m_cell_buf[index] == m_quote) &&
                        (index < m_cell_buf_size))
                      {
                        m_cell_buf.erase(index-1, 1);
                        m_cell_buf_size = m_cell_buf_size - 1;
                      } else {
                        instruction_success = false;
                        break;
                      }
                    } else {
                      index++;
                    }
                  }
                }
                if (mp_table_user->get_col_count() < (m_col_index+1)) {
                  mp_table_user->set_col_count(m_col_index+1);
                }
                if (mp_table_user->get_row_count() < (m_row_index+1)) {
                  mp_table_user->set_row_count(m_row_index+1);
                }
                const string_type cell_buf = str_conv<string_type>(m_cell_buf);
                mp_table_user->write_cell(
                  m_col_index, m_row_index, cell_buf);
                if (ch == m_delimiter_col) {
                  m_col_index++;
                } else {
                  m_row_index++;
                  m_col_index = 0;
                }
                m_cell_buf.clear();
                m_cell_index++;
                m_quote_count = 0;
              } else {
                on_add_ch = true;
              }
            } else if (ch == m_quote) {
              on_add_ch = true;
              m_quote_count++;
            } else {
              on_add_ch = true;
            }
            if (on_add_ch){
              m_cell_buf += ch;
              on_add_ch = false;
            }
          // Если достигнут EOF
          } else if (m_file.eof()){
            set_mode_success();
          // Если не EOF
          } else {
            instruction_success = false;
          }
          if (!instruction_success) {
            mp_row_user->clear();
            set_mode_error();
          }
        } break;
        default : {
          IRS_LIB_ASSERT_MSG("Неизвестный тип инструкции");
          set_mode_error();
        }
      }
    } break;
    case ps_abort: {
      set_mode_success();
    }
  }
}

void irs::csvwork::csv_file_t::set_file(const string_type& a_file_name)
{
  IRS_LIB_ASSERT(m_command == cmd_none_command);
  if (m_command == cmd_none_command) {
    m_filename = a_file_name;
  } else {
    IRS_LIB_ASSERT_MSG("Еще не завершена последняя операция с файлом");
  }
}

void irs::csvwork::csv_file_t::get_col_count(size_type* ap_col_count)
{
  IRS_LIB_ASSERT((m_command == cmd_none_command) && (ap_col_count != IRS_NULL));
  if ((m_command == cmd_none_command) && (ap_col_count != IRS_NULL)) {
    mp_col_count_user = ap_col_count;
    m_cur_instruction = ci_calc_col_count;
    m_command = cmd_calc_col_count;
    m_status = cfs_busy;
  }
}

void irs::csvwork::csv_file_t::get_row_count(size_type* ap_row_count)
{
  IRS_LIB_ASSERT((m_command == cmd_none_command) && (ap_row_count != IRS_NULL));
  if ((m_command == cmd_none_command) && (ap_row_count != IRS_NULL)) {
    mp_row_count_user = ap_row_count;
    m_cur_instruction = ci_calc_row_count;
    m_command = cmd_calc_row_count;
    m_status = cfs_busy;
  }
}

void irs::csvwork::csv_file_t::row_push_back(
  std::vector<string_type>* const ap_row)
{
  IRS_LIB_ASSERT((m_command == cmd_none_command) && (ap_row != IRS_NULL));
  if (m_command == cmd_none_command) {
    mp_row_user = ap_row;
    m_cur_instruction = ci_row_push_back;
    m_command = cmd_row_push_back;
    m_status = cfs_busy;
  }
}

void irs::csvwork::csv_file_t::row_push_back(vector<string_type>& a_row)
{
  IRS_LIB_ASSERT(m_command == cmd_none_command);
  if (m_command == cmd_none_command) {
    m_row_user_buf = a_row;
    mp_row_user = &m_row_user_buf;
    m_cur_instruction = ci_row_push_back;
    m_command = cmd_row_push_back;
    m_status = cfs_busy;
  }
}

void irs::csvwork::csv_file_t::save(table_string_t* const ap_table_user)
{
  IRS_LIB_ASSERT((m_command == cmd_none_command) &&
    (ap_table_user != IRS_NULL));
  if (m_command == cmd_none_command) {
    mp_table_user = ap_table_user;
    m_table_uses_col_count = mp_table_user->get_col_count();
    m_table_uses_row_count = mp_table_user->get_row_count();
    m_table_uses_cell_count = m_table_uses_col_count * m_table_uses_row_count;
    m_cur_instruction = ci_save;
    m_command = cmd_save;
    m_status = cfs_busy;
  }
}

void irs::csvwork::csv_file_t::load(table_string_t* ap_table_user)
{
  IRS_LIB_ASSERT((m_command == cmd_none_command) &&
    (ap_table_user != IRS_NULL));
  if (m_command == cmd_none_command) {
    mp_table_user = ap_table_user;
    m_cur_instruction = ci_load;
    m_command = cmd_load;
    m_status = cfs_busy;
  }
}

bool irs::csvwork::csv_file_t::clear()
{
  bool fsuccess = true;
  IRS_LIB_ASSERT(m_command == cmd_none_command);
  if (m_command == cmd_none_command) {
    m_file.open(IRS_SIMPLE_FROM_TYPE_STR(m_filename.c_str()),
      ios::in|ios::out|ios::trunc);
    if(!m_file) {
      fsuccess = false;
    } else {
      m_file.close();
    }
  } else {
    fsuccess = false;
  }
  return fsuccess;
}

void irs::csvwork::csv_file_t::get_row(
  std::vector<string_type>* ap_row,
  const int a_row_index,
  const pos_mode_t a_pos_mode)
{
  IRS_LIB_ASSERT(
    (m_command == cmd_none_command) &&
    (ap_row != IRS_NULL) &&
    (a_row_index >= 0));

  if (
    (m_command == cmd_none_command) &&
    (ap_row != IRS_NULL) &&
    (a_row_index >= 0))
  {
    mp_row_user = ap_row;
    mp_row_user->clear();
    m_row_index = a_row_index;
    m_pos_mode = a_pos_mode;
    m_cur_instruction = ci_find_row_pos;
    m_command = cmd_get_row;
    m_status = cfs_busy;
  }
}

void irs::csvwork::csv_file_t::set_mode_error()
{
  m_file.close();
  m_file.clear();
  m_progress = 0.;
  m_status = cfs_error;
  m_command = cmd_none_command;
  m_processing_status = ps_busy_command;
}

void irs::csvwork::csv_file_t::set_mode_success()
{
  m_file.close();
  m_file.clear();
  m_progress = 1.;
  m_status = cfs_success;
  m_command = cmd_none_command;
  m_processing_status = ps_busy_command;
}

/*void irs::csv_file_t::row_erase(const int a_row_begin, const int a_row_end)
{

}*/


//class csv_file_synchro_t

irs::csvwork::csv_file_synchro_t::csv_file_synchro_t(
  const string_type& a_file_name):
  m_delimiter_col(irst(';')),
  m_delimiter_row(irst('\n')),
  m_delimiter_cell(irst('"')),
  m_special_character(),
  m_file_name(a_file_name)
{
  m_special_character = static_cast<string_type>(m_delimiter_col) +
    static_cast<string_type>(m_delimiter_cell);
}

irs::csvwork::csv_file_synchro_t::~csv_file_synchro_t()
{
}

void irs::csvwork::csv_file_synchro_t::set_file_name(
  const string_type& a_file_name)
{
  m_file_name = a_file_name;
}

const irs::csvwork::csv_file_synchro_t::string_type&
irs::csvwork::csv_file_synchro_t::get_file_name() const
{
  return m_file_name;
}

void irs::csvwork::csv_file_synchro_t::set_delimiter_col(
  char_type a_delimiter_col)
{
  IRS_LIB_ASSERT(a_delimiter_col != m_delimiter_row);
  IRS_LIB_ASSERT(a_delimiter_col != m_delimiter_cell);
  m_delimiter_col = a_delimiter_col;
  m_special_character = static_cast<string_type>(m_delimiter_col) +
    static_cast<string_type>(m_delimiter_cell);
}

void irs::csvwork::csv_file_synchro_t::set_delimiter_row(
  char_type a_delimiter_row)
{
  IRS_LIB_ASSERT(a_delimiter_row != m_delimiter_col);
  IRS_LIB_ASSERT(a_delimiter_row != m_delimiter_cell);
  m_delimiter_row = a_delimiter_row;
}

void irs::csvwork::csv_file_synchro_t::set_delimiter_coll(
  char_type a_delimiter_cell)
{
  IRS_LIB_ASSERT(a_delimiter_cell != m_delimiter_col);
  IRS_LIB_ASSERT(a_delimiter_cell != m_delimiter_row);
  m_delimiter_cell = a_delimiter_cell;
  m_special_character = static_cast<string_type>(m_delimiter_col) +
    static_cast<string_type>(m_delimiter_cell);
}

bool irs::csvwork::csv_file_synchro_t::save(
  const table_string_t& a_table_string)
{
  typedef table_string_t::size_type size_type;
  bool fsuccess = true;
  ofstream_type ofile;
  if (!m_file_name.empty()) {
    ofile.open(IRS_SIMPLE_FROM_TYPE_STR(m_file_name.c_str()),
      ios::in|ios::out|ios::trunc);
    if (!ofile.good()) {
      fsuccess = false;
    }
  } else {
    fsuccess = false;
  }

  if (fsuccess) {
    string_type cell_modified_str;
    const size_type col_count = a_table_string.get_col_count();
    const size_type row_count = a_table_string.get_row_count();
    for (size_type row = 0; row < row_count; row++) {
      bool find_not_empty_cell_success = false;
      for (size_type col = 0; col < col_count; col++) {
        string_type cell_str = a_table_string.read_cell(col, row);
        if (!cell_str.empty()) {
          if (!find_not_empty_cell_success) {
            string_type delimiter_col_array(col, m_delimiter_col);
            ofile << delimiter_col_array;
            find_not_empty_cell_success = true;
          } else {
            ofile << m_delimiter_col;
          }
          bool find_special_character_success = false;
          if (cell_str.find_first_of(m_special_character) !=
            string_type::npos) {

            find_special_character_success = true;
          }
          size_type cell_str_size = cell_str.size();
          for (size_type i = 0; i < cell_str_size; i++) {
            if (cell_str[i] == m_delimiter_cell) {
              cell_modified_str += m_delimiter_cell;
            }
            cell_modified_str += cell_str[i];
          }
          if (find_special_character_success) {
            cell_modified_str = irst("\"") + cell_modified_str + irst("\"");
          }
          ofile << cell_modified_str;
          cell_modified_str.clear();
        } else if (find_not_empty_cell_success) {
          ofile << m_delimiter_col;
        }
      }
      ofile << m_delimiter_row;
    }
  }
  ofile.close();
  return fsuccess;
}

bool irs::csvwork::csv_file_synchro_t::load(table_string_t* ap_table_string)
{
  typedef table_string_t::size_type size_type;
  IRS_LIB_ASSERT(ap_table_string);

  bool fsuccess = true;
  ap_table_string->clear();
  ifstream_type ifile;
  if (!m_file_name.empty()) {
    ifile.open(IRS_SIMPLE_FROM_TYPE_STR(m_file_name.c_str()), ios::in);
    if (!ifile.good()) {
      fsuccess = false;
    }
  } else {
    fsuccess = false;
  }

  if (fsuccess) {
    string_type cell_str;
    size_type quote_count = 0;
    enum mode_t { find_delimiter, processing_str} mode;
    enum delimiter_t {delimiter_col, deliminer_row};
    struct coord_cur_cell_t
    {
      size_type col;
      size_type row;
    } coord_cur_cell = {0, 0};

    delimiter_t cur_delimiter = delimiter_col;
    mode = find_delimiter;
    char_type ch = static_cast<char_type>(ifile.get());
    while(ifile.good()) {
      if (mode == find_delimiter) {
        if (ch == m_delimiter_cell) {
          quote_count++;
          cell_str += ch;
        }
        if (((ch == m_delimiter_col) ||
          (ch == m_delimiter_row)) &&
          (quote_count % 2 == 0))
        {
          if (ch == m_delimiter_col) {
            cur_delimiter = delimiter_col;
          } else {
            cur_delimiter = deliminer_row;
          }
          mode = processing_str;
        } else {
          if (ch == m_delimiter_cell) {
            quote_count++;
          }
          cell_str += ch;
        }
      }
      if (mode == processing_str) {
        bool processing_str_success = true;
        if (quote_count != 0) {
          cell_str = cell_str.substr(1, cell_str.size()-2);
          size_type index = 0;
          size_type index_end = cell_str.size();
          while (index < index_end) {
            if (cell_str[index] == m_delimiter_cell) {
              index++;
              if ((cell_str[index] == m_delimiter_cell) &&
                (index < index_end))
              {
                cell_str.erase(index-1, 1);
                index_end = index_end - 1;
              } else {
                processing_str_success = false;
                break;
                // Запомнить координаты ячейки, в которой произошла ошибка
              }
            } else {
              index++;
            }
          }
        }
        if (processing_str_success) {
          size_type col_count = ap_table_string->get_col_count();
          if (col_count <= coord_cur_cell.col) {
            ap_table_string->set_col_count(coord_cur_cell.col+1);
          }
          size_type row_count = ap_table_string->get_row_count();
          if (row_count <= coord_cur_cell.row) {
            ap_table_string->set_row_count(coord_cur_cell.row+1);
          }
          ap_table_string->write_cell(
            coord_cur_cell.col, coord_cur_cell.row, cell_str);
          if (cur_delimiter == delimiter_col) {
            coord_cur_cell.col++;
          } else {
            coord_cur_cell.col = 0;
            coord_cur_cell.row++;
          }
        } else {
          // Удаляем все
          ap_table_string->clear();
        }
        cell_str.clear();
        quote_count = 0;
        mode = find_delimiter;
      }
      ch = static_cast<char_type>(ifile.get());
    }
  }
  ifile.close();
  return fsuccess;
}

#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

