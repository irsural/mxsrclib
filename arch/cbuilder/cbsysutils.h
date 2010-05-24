// Функции для работы с компонентами C++Builder
// Дата: 09.05.2010
// Дата создания: 9.09.2009

#ifndef cbsysutilsH
#define cbsysutilsH

#include <irsdefs.h>

// ***************************************************************************
// Исправление ошибки файла sysvary.h в C++ Builder 4, который находится
// в include-дереве файла OleCtnrs.hpp. В include-дереве sysvary.h отсутствует
// необходимый файл utilcls.h
#if __BORLANDC__ < IRS_CPP_BUILDER6
#include <utilcls.h>
#endif //__BORLANDC__ < IRS_CPP_BUILDER6
// ***************************************************************************
#include <OleCtnrs.hpp>
#include <SysUtils.hpp>
#include <Grids.hpp>
#include <wstring.h>

#include <irssysutils.h>
#include <irstable.h>
#include <irserror.h>
#include <irsdefs.h>
#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

namespace cbuilder {

typedef irs::string string_t;
typedef size_t sizens_t;

#if IRS_LIB_VERSION_SUPPORT_LESS(390)
// Универсальная функция перевода чисел в текст
template<class T>
inline AnsiString number_to_str(
  const T a_num,
  const int a_precision = -1,
  const irs::num_mode_t a_num_mode = irs::num_mode_general)
{
  //irs::string str_value = a_num;
  //AnsiString astring_value = str_value.c_str();
  irs::string str_value = irs::number_to_str(a_num, a_precision, a_num_mode);
  AnsiString astring_value = str_value.c_str();
  return astring_value;
};
// Универсальная функция перевода текста в число
template<class T>
inline bool str_to_number(const AnsiString& a_str, T& a_number)
{
  irs::string str = a_str.c_str();
  return str.to_number(a_number);
};
#endif // #if IRS_LIB_VERSION_SUPPORT_LESS(390)  

// Структура версии файла
struct file_version_t
{
  unsigned short major;
  unsigned short minor;
  unsigned short release;
  unsigned short build;
};

// Запрос версии файла
bool get_file_version(
  const irs::string& a_file_name, file_version_t& a_version);
// Перевод структуры о версии файла в строку
irs::string file_version_to_str(const file_version_t& a_file_version);

void file_xls_table_read(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,
  const string_t& a_cell_last,
  table_string_t* ap_table_string,
  bool* ap_read_success);

void file_xls_table_write(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,
  const table_string_t& a_table_string,
  bool* ap_read_success);

// Функции, обеспечивающие безопасное копирование из StringGrid в table_string_t
// и обратно. При недостаточном размере таблицы-приемника, ее размеры будут
// автоматически увеличены
void string_grid_to_table_string(
  const TStringGrid& a_string_grid,
  irs::table_string_t* ap_table_string
);

void string_grid_to_table_string(
  const rect_t& a_string_grid_rect,
  const TStringGrid& a_string_grid,
  const point_t& a_table_string_point,
  irs::table_string_t* ap_table_string
);

void table_string_to_string_grid(
  const table_string_t& a_table_string,
  TStringGrid* ap_string_grid
);

void table_string_to_string_grid(
  const rect_t& a_table_string_rect,
  const table_string_t& a_table_string,
  const point_t& a_string_grid_point,
  TStringGrid* ap_string_grid
);

// Вывод в компонент
//void out_to_tree_view();



}; //namespace cbuilder

}; //namespace irs

#endif //cbsysutilsH

