//! \file
//! \ingroup file_in_out_group
//! \brief Работа с csv-файлами
//!
//! Дата: 15.09.2010\n
//! Ранняя дата: 17.09.2009

#ifndef csvworkH
#define csvworkH

#include <irsdefs.h>

#include <stdio.h>
#include <string.h>

#include <irsstd.h>
#include <irstable.h>
#include <irsstrdefs.h>

#include <irsfinal.h>

//! \addtogroup file_in_out_group
//! @{

//---------------------------------------------------------------------------
// typedef-определения
typedef irs_u32 csv_int;
typedef enum {ec_i32_type, ec_float_type, ec_str_type} e_val_type;
typedef void (*dbg_text_out_fn)(const char *dbg_text);
//---------------------------------------------------------------------------
// Константы
//const csv_int max_line_count = 30000;
//---------------------------------------------------------------------------
// Определения составных типов
struct var_val_t {
  union {
    irs_i32 uf_i32_type;
    float uf_float_type;
  };
  irs::string_t uf_str_type;
};
//---------------------------------------------------------------------------
//! \brief Класс для работы с csv-файлами.
//! \author Krasheninnikov Maxim
class csv_file
{
public:
  typedef irs::char_t char_type;
  typedef irs::string_t string_type;
  //typedef map_elem_t::reverse_iterator map_elem_revit_t;
  //typedef reverse_iterator<map_elem_it_t> map_elem_revit_t;

  // Конструкторы и деструкторы
  csv_file();
  ~csv_file();

  // Методы
  //! \brief Установка функции для вывода отладочной информации.
  void set_dbg_text_out_fn(dbg_text_out_fn a_dbg_text_out_fn);
  //! \brief Запись переменной, при ошибке возвращает irs_false.
  irs_bool set_var(const string_type& var_name, irs_i32 var_value);
  irs_bool set_var(const string_type& var_name, float var_value);
  irs_bool set_var(const string_type& var_name, const string_type& var_value);
  //! \brief Добавить параметр в начале csv-файла, при ошибке
  //!   возвращает irs_false.
  irs_bool add_par(const string_type& par_name, irs_i32 par_value);
  irs_bool add_par(const string_type& par_name, float par_value);
  irs_bool add_par(const string_type& par_name, const string_type& par_value);
  //! \brief Добавить название столбца csv-файла, при ошибке
  //!   возвращает irs_false.
  irs_bool add_col(const string_type& col_name, e_val_type val_type);
  //! \brief Открыть файл name, при ошибке возвращает irs_false.
  irs_bool open(const string_type& file_name);
  //! \brief Закрыть файл.
  irs_bool close();
  //! \brief Запись строки в csv-файл, при превышении количества записей
  //!   возвращает irs_false.
  irs_bool write_line();

  //! \brief Очистить столбцы.
  void clear_cols();
  //! \brief Очистить переменные.
  void clear_pars();

private:
  struct elem_t {
    e_val_type type;
    var_val_t val;
    elem_t():
      type(e_val_type()),
      val(var_val_t())
    {}
    elem_t(e_val_type a_type, var_val_t a_val):
      type(a_type),
      val(a_val)
    {}
  };
  typedef pair<string_type, elem_t> list_item_type;
  typedef vector<list_item_type> list_type;
  typedef list_type::iterator list_it_type;

  // Эквивалентность для первого элемента пары
  struct pair_first_equal: public unary_function<string, bool>
  {
    const string_type& m_elem_for_find;
    pair_first_equal(const string_type& a_elem_for_find):
      m_elem_for_find(a_elem_for_find)
    {}
    bool operator()(list_item_type a_pair) const
    {
      return a_pair.first == m_elem_for_find;
    }
  };

  //csv_int m_lines_writed_count;
  bool m_is_first_line;
  list_type m_pars;
  list_type m_vars;
  //FILE* mp_ini_file;
  irs::ofstream_t m_csv_file;

  // Методы
  // Вывод отладочной информации
  void dbg_msg(const string_type& msg);
  // Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
  irs_bool add_par(const string_type& par_name, e_val_type val_type,
    var_val_t par_value);
  // Запись переменной, при ошибке возвращает irs_false
  irs_bool set_var(const string_type& var_name, var_val_t var_value);
  // Запись в list_type подобно map[]
  void assign_elem(list_type &a_elem_array, const string_type& a_name,
    const elem_t &a_elem);
  // Поиск елемента в list_type
  list_it_type find_elem(list_type &a_elem_array,
    const string_type& a_name);

}; //csv_file
//---------------------------------------------------------------------------

//! @}

namespace irs {

namespace csvwork {

//! \addtogroup file_in_out_group
//! @{

// В Embeded C++ нет fstream, а есть либо ifstream, либо ofstream
// Watcom не переваривает irstable.h в котором определен тип table_string_t
#ifdef IRS_FULL_STDCPPLIB_SUPPORT

//! \brief Статус операции.
enum csv_file_status_t {
  cfs_success = 0, //!< \brief Последняя операция успешно выполнилась.
  cfs_busy = 1,    //!< \brief Операция выполняется.
  cfs_error = 2    //!< \brief Последняя операция завершилась с ошибкой.
};
//! \brief Чтение/запись csv файла в неблокирующем режиме.
//! \author Lyashchov Maxim
class csv_file_t
{
public:
  typedef size_t size_type;
  typedef char_t char_type;
  typedef string_t string_type;
  typedef int delimiter_row_type;
  typedef basic_fstream<char_type, char_traits<char_type> > fstream_type;
  enum pos_mode_t {pos_mode_beg, pos_mode_end};
private:
  char_type m_delimiter_col;
  const char m_delimiter_row;
  const string_type m_delim_row;
  const size_type m_delimiter_row_size;

  //size_type m_pos;
  string_type m_filename;
  fstream_type m_file;
  double m_progress;
  csv_file_status_t m_status;
  enum cur_instruction_t {
    ci_calc_col_count,
    ci_calc_row_count,
    ci_rows_erase,
    ci_row_push_back,
    ci_save,
    ci_load,
    ci_find_row_pos,
    ci_get_row};
  cur_instruction_t m_cur_instruction;
  enum processing_status_t {ps_busy_command, ps_run_instruction, ps_abort};
  processing_status_t m_processing_status;
  enum command_t {
    cmd_none_command,
    cmd_calc_col_count,
    cmd_calc_row_count,
    cmd_rows_erase,
    cmd_row_push_back,
    cmd_save,
    cmd_load,
    cmd_get_row};
  command_t m_command;
  size_type m_delimiter_col_count;
  size_type m_delimiter_col_count_cur_row;
  size_type m_delimiter_col_count_sum;
  //int m_delimiter_col_count_mean;
  size_type m_delimiter_row_count;
  size_type m_ch_count_row_mean;
  size_type* mp_col_count_user;
  size_type* mp_row_count_user;
  vector<string_type>* mp_row_user;
  vector<string_type> m_row_user_buf;
  table_string_t* mp_table_user;
  string_type m_cell_buf;
  size_type m_table_uses_col_count;
  size_type m_table_uses_row_count;
  size_type m_table_uses_cell_count;
  size_type m_cell_index;
  size_type m_file_size;
  size_type m_pos_file;
  size_type m_col_index;
  size_type m_row_index;
  pos_mode_t m_pos_mode;
  size_type m_row_pos_file;
  const char_type m_quote;
  size_type m_quote_count;
  string_type m_special_character;
  //csv_file_t();

public:   
  csv_file_t(const string_type& a_filename = string_type());
  ~csv_file_t();
  //! \brief Выдает текущее состояние выполняемой операции. Значения от 0 до 1.
  inline double get_progress();
  inline csv_file_status_t get_status();
  void tick();
  void set_file(const string_type& a_file_name);
  void get_col_count(size_type* ap_col_count);
  void get_row_count(size_type* ap_row_count);
  //! \brief Добавить строку в конец CSV файла.
  //!
  //! Буфер внутрь не копируется, а считываюется прямо из буфера пользователя.
  void row_push_back(vector<string_type>* const  ap_row);
  //! \brief Добавить строку в конец CSV файла.
  //!
  //! Буфер строк копируется во внутренний буфер.
  void row_push_back(vector<string_type>& a_row);
  //! \brief Сохранение в файл таблицы. Старое содержимое удаляется.
  //!
  //! Таблица внутрь не копируется.
  void save(table_string_t* const ap_table_user);
  void load(table_string_t* const ap_table_user);
  //! \brief Прочитать строку.
  void get_row(
    vector<string_type>* ap_row,
    const int a_row_index,
    const pos_mode_t a_pos_mode = pos_mode_beg);
  bool clear();
  //void row_erase(const int a_row_begin, const int a_row_end);
private:
  void set_mode_error();
  void set_mode_success();
};

inline double csv_file_t::get_progress()
{
  return m_progress;
}

inline csv_file_status_t csv_file_t::get_status()
{
  return m_status;
}

//! \brief Чтение/запись csv файла в блокирующем режиме.
//! \author Lyashchov Maxim
class csv_file_synchro_t
{
public:
  typedef irs_size_t size_type;
  typedef char_t char_type;
  typedef string_t string_type;
  typedef basic_fstream<char_type, char_traits<char_type> > fstream_type;
  csv_file_synchro_t(const string_type& a_filename = irst(""));
  ~csv_file_synchro_t();
  inline void set_delimiter_col(char_type a_delimiter_col);
  inline void set_delimiter_row(char_type a_delimiter_row);
  inline void set_delimiter_coll(char_type a_delimiter_cell);
  inline void open(const string_type& a_filename);
  void close();
  bool save(const table_string_t& a_table_string);
  bool load(table_string_t& a_table_string);
private:
  char_type m_delimiter_col;
  char_type m_delimiter_row;
  char_type m_delimiter_cell;
  string_type m_special_character;
  string_type m_filename;
  fstream_type m_file;
  enum status_file_t {stat_file_close, stat_file_open};
  status_file_t m_status_file;
};

inline void csv_file_synchro_t::set_delimiter_col(char_type a_delimiter_col)
{
  IRS_LIB_ASSERT(a_delimiter_col != m_delimiter_row);
  IRS_LIB_ASSERT(a_delimiter_col != m_delimiter_cell);
  m_delimiter_col = a_delimiter_col;
  m_special_character = static_cast<string_type>(m_delimiter_col) +
    static_cast<string_type>(m_delimiter_cell);
}

inline void csv_file_synchro_t::set_delimiter_row(char_type a_delimiter_row)
{
  IRS_LIB_ASSERT(a_delimiter_row != m_delimiter_col);
  IRS_LIB_ASSERT(a_delimiter_row != m_delimiter_cell);
  m_delimiter_row = a_delimiter_row;
}

inline void csv_file_synchro_t::set_delimiter_coll(char_type a_delimiter_cell)
{
  IRS_LIB_ASSERT(a_delimiter_cell != m_delimiter_col);
  IRS_LIB_ASSERT(a_delimiter_cell != m_delimiter_row);
  m_delimiter_cell = a_delimiter_cell;
  m_special_character = static_cast<string_type>(m_delimiter_col) +
    static_cast<string_type>(m_delimiter_cell);
}

inline void csv_file_synchro_t::open(const string_type& a_filename)
{
  //bool fsuccess = true;
  m_filename = a_filename;
  m_file.open(IRS_SIMPLE_FROM_TYPE_STR(m_filename.c_str()));
  m_status_file = stat_file_open;
}

inline void csv_file_synchro_t::close()
{
  m_file.close();
  m_status_file = stat_file_close;
  m_filename = "";

}

#endif // IRS_FULL_STDCPPLIB_SUPPORT

//! @}

} // namespace csvwork

} // namespace irs

#endif //csvworkH
