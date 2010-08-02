// Работа с csv-файлами
// Дата: 16.05.2010
// Ранняя дата: 17.09.2009

#ifndef csvworkH
#define csvworkH

#include <irsdefs.h>

#include <stdio.h>
#include <string.h>

#include <irsstd.h>
#include <irstable.h>

#include <irsfinal.h>

//---------------------------------------------------------------------------
// typedef-определения
typedef irs_u32 csv_int;
typedef enum {ec_i32_type, ec_float_type, ec_str_type} e_val_type;
typedef void (*dbg_text_out_fn)(const char *dbg_text);
//---------------------------------------------------------------------------
// Константы
const csv_int max_line_count = 30000;
//---------------------------------------------------------------------------
// typedef-определения составных типов
typedef struct _var_val_t {
  union {
    irs_i32 uf_i32_type;
    float uf_float_type;
  };
  irs::string uf_str_type;
} var_val_t;
//---------------------------------------------------------------------------
// Класс для работы с csv-файлами
class csv_file
{
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
  // Эквивалентность для первого элемента пары
  struct pair_first_equal: public unary_function<string, bool>
  {
    const string &m_elem_for_find;
    pair_first_equal(const string &a_elem_for_find):
      m_elem_for_find(a_elem_for_find)
    {}
    bool operator()(pair<string, elem_t> a_pair) const
    {
      return a_pair.first == m_elem_for_find;
    }
  };

  typedef vector<pair<string, elem_t> > map_elem_t;
  typedef map_elem_t::iterator map_elem_it_t;
  //typedef map_elem_t::reverse_iterator map_elem_revit_t;
  //typedef reverse_iterator<map_elem_it_t> map_elem_revit_t;

  csv_int     f_lines_writed_count;
  map_elem_t  f_pars;
  map_elem_t  f_vars;
  FILE*       f_ini_file;

  // Методы
  // Вывод отладочной информации
  void dbg_msg(const char *msg);
  // Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
  irs_bool add_par(const char *par_name, e_val_type val_type,
    var_val_t par_value);
  // Запись переменной, при ошибке возвращает irs_false
  irs_bool set_var(const char *var_name, var_val_t var_value);
  // Запись в map_elem_t подобно map[]
  void assign_elem(map_elem_t &a_elem_array, const string &a_name,
    const elem_t &a_elem);
  // Поиск елемента в map_elem_t
  map_elem_it_t find_elem(map_elem_t &a_elem_array, const string &a_name);

public:
  // Конструкторы и деструкторы
  csv_file();
  ~csv_file();

  // Методы
  // Установка функции для вывода отладочной информации
  void set_dbg_text_out_fn(dbg_text_out_fn a_dbg_text_out_fn);
  // Запись переменной, при ошибке возвращает irs_false
  irs_bool set_var(const char *var_name, irs_i32 var_value);
  irs_bool set_var(const char *var_name, float var_value);
  irs_bool set_var(const char *var_name, const char *var_value);
  // Добавить параметр в начале csv-файла, при ошибке возвращает irs_false
  irs_bool add_par(const char *par_name, irs_i32 par_value);
  irs_bool add_par(const char *par_name, float par_value);
  irs_bool add_par(const char *par_name, const char *par_value);
  // Добавить название столбца csv-файла, при ошибке возвращает irs_false
  irs_bool add_col(const char *col_name, e_val_type val_type);
  // Открыть файл name, при ошибке возвращает irs_false
  irs_bool open(const char *file_name);
  // Закрыть файл
  irs_bool close();
  // Запись строки в csv-файл, при превышении количества записей
  // возвращает irs_false
  irs_bool write_line();

  // Очистить столбцы
  void clear_cols();
  // Очистить переменные
  void clear_pars();
}; //csv_file
//---------------------------------------------------------------------------

namespace irs {

namespace csvwork {

// В Embeded C++ нет fstream, а есть либо ifstream, либо ofstream
// Watcom не переваривает irstable.h в котором определен тип table_string_t
#if !defined(__embedded_cplusplus) && !defined(__WATCOMC__)

enum csv_file_status_t {cfs_success = 0, cfs_busy = 1, cfs_error = 2};
class csv_file_t
{
public:
  typedef size_t size_type;
  typedef char_t char_type;
  typedef irs_string_t string_type;
  typedef int delimiter_row_type;
  enum pos_mode_t {pos_mode_beg, pos_mode_end};
private:
  char_type m_delimiter_col;
  const char m_delimiter_row;
  const string_type m_delim_row;
  const size_type m_delimiter_row_size;

  //size_type m_pos;
  string m_filename;
  fstream m_file;
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
  vector<string>* mp_row_user;
  vector<string> m_row_user_buf;
  table_string_t* mp_table_user;
  string m_cell_buf;
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
  string m_special_character;
  //csv_file_t();

public:   
  csv_file_t(const string& a_filename = "");
  ~csv_file_t();
  // Выдает текущее состояние выполняемой операции. Значения от 0 до 1
  inline double get_progress();
  inline csv_file_status_t get_status();
  void tick();
  void set_file(const string_type& a_file_name);
  void get_col_count(size_type* ap_col_count);
  void get_row_count(size_type* ap_row_count);
  // Добавить строку в конец CSV файла
  // Буфер внутрь не копируется, а считываюется прямо из буфера пользователя
  void row_push_back(vector<string>* const  ap_row);
  // Добавить строку в конец CSV файла
  // Буфер строк копируется во внутренний буфер
  void row_push_back(vector<string>& a_row);
  // Сохранение в файл таблицы. Старое содержимое удаляется
  // Таблица внутрь не копируется
  void save(table_string_t* const ap_table_user);
  void load(table_string_t* const ap_table_user);
  // Прочитать строку
  void get_row(
    vector<string>* ap_row,
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

class csv_file_synchro_t
{
  char m_delimiter_col;
  char m_delimiter_row;
  char m_delimiter_cell;
  string m_special_character;
  string m_filename;
  fstream m_file;
  enum status_file_t {stat_file_close, stat_file_open};
  status_file_t m_status_file;
public:
  csv_file_synchro_t(const string& a_filename = "");
  ~csv_file_synchro_t();
  inline void set_delimiter_col(char a_delimiter_col);
  inline void set_delimiter_row(char a_delimiter_row);
  inline void set_delimiter_coll(char a_delimiter_cell);
  inline void open(const string& a_filename);
  void close();
  bool save(const table_string_t& a_table_string);
  bool load(table_string_t& a_table_string);
};

inline void csv_file_synchro_t::set_delimiter_col(char a_delimiter_col)
{
  IRS_LIB_ASSERT(a_delimiter_col != m_delimiter_row);
  IRS_LIB_ASSERT(a_delimiter_col != m_delimiter_cell);
  m_delimiter_col = a_delimiter_col;
  m_special_character = static_cast<string>(m_delimiter_col) +
    static_cast<string>(m_delimiter_cell);
}

inline void csv_file_synchro_t::set_delimiter_row(char a_delimiter_row)
{
  IRS_LIB_ASSERT(a_delimiter_row != m_delimiter_col);
  IRS_LIB_ASSERT(a_delimiter_row != m_delimiter_cell);
  m_delimiter_row = a_delimiter_row;
}

inline void csv_file_synchro_t::set_delimiter_coll(char a_delimiter_cell)
{
  IRS_LIB_ASSERT(a_delimiter_cell != m_delimiter_col);
  IRS_LIB_ASSERT(a_delimiter_cell != m_delimiter_row);
  m_delimiter_cell = a_delimiter_cell;
  m_special_character = static_cast<string>(m_delimiter_col) +
    static_cast<string>(m_delimiter_cell);
}

inline void csv_file_synchro_t::open(const string& a_filename)
{
  //bool fsuccess = true;
  m_filename = a_filename;
  m_file.open(m_filename.c_str());
  m_status_file = stat_file_open;
}

inline void csv_file_synchro_t::close()
{
  m_file.close();
  m_status_file = stat_file_close;
  m_filename = "";

}

#endif //!defined(__embedded_cplusplus) && !defined(__WATCOMC__)

} // namespace csvwork

} // namespace irs

#endif //csvworkH
