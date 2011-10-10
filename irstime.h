//! \file
//! \ingroup time_processing_group
//! \brief Работа со временем
//!
//! Дата: 15.09.2010\n
//! Ранняя дата: 10.07.2009

#ifndef irstimeH
#define irstimeH

#include <irsdefs.h>

#include <time.h>

#include <irsstring.h>
#include <irsstrdefs.h>
//#include <irsstrconv.h>
#include <timer.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup time_processing_group
//! @{

typedef irs_u32 millisecond_t;

const millisecond_t min_in_hour = 60;
const millisecond_t sec_in_min = 60;
const millisecond_t ms_in_sec = 1000;
const millisecond_t ms_in_min = ms_in_sec*sec_in_min;
const millisecond_t ms_in_hour = ms_in_min*min_in_hour;

// Преобразование числа в мс в строку в формате: ЧЧ:ММ:СС.МСЕ
string_t ms_to_strtime(millisecond_t ms, bool show_ms = false);
// Счетчика ИРС в строку в формате: ЧЧ:ММ:СС.МСЕ
string_t cnt_to_strtime(counter_t cnt, bool show_ms = false);

millisecond_t system_time();

// Запись в поток текущего времени
IRS_STRING_TEMPLATE
inline IRS_STREAMSPECDECL IRS_STRING_OSTREAM&
  basic_stime(IRS_STRING_OSTREAM &a_stream)
{
  time_t timer = time(NULL);
  const tm* date = localtime(&timer);
  a_stream << setfill(static_cast<IRS_STRING_CHAR_TYPE>('0'));
  a_stream << setw(2) << date->tm_hour;
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(':');
  a_stream << setw(2) << date->tm_min;
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(':');
  a_stream << setw(2) << date->tm_sec;
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(' ');
  return a_stream;
}
IRS_STREAMSPECDECL ostream_t &stimet(ostream_t &a_stream);
IRS_STREAMSPECDECL ostream &stime(ostream &a_stream);
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
IRS_STREAMSPECDECL wostream &wstime(wostream &a_stream);
#endif //IRS_FULL_STDCPPLIB_SUPPORT

// Запись в поток текущей даты и времени
IRS_STRING_TEMPLATE
inline IRS_STREAMSPECDECL IRS_STRING_OSTREAM &
  basic_sdatetime(IRS_STRING_OSTREAM &a_stream)
{
  time_t timer = time(NULL);
  const tm* date = localtime(&timer);
  a_stream << setfill(static_cast<IRS_STRING_CHAR_TYPE>('0'));
  a_stream << setw(2) << date->tm_mday;
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>('.');
  a_stream << setw(2) << (date->tm_mon + 1);
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>('.');
  a_stream << setw(4) << (date->tm_year + 1900);
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(' ');
  a_stream << setw(2) << date->tm_hour;
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(':');
  a_stream << setw(2) << date->tm_min;
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(':');
  a_stream << setw(2) << date->tm_sec;
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(' ');
  return a_stream;
}
IRS_STREAMSPECDECL ostream_t &sdatetimet(ostream_t &a_stream);
IRS_STREAMSPECDECL ostream &sdatetime(ostream &a_stream);
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
IRS_STREAMSPECDECL wostream &wsdatetime(wostream &a_stream);
#endif //IRS_FULL_STDCPPLIB_SUPPORT

//возвращает текущую дату в формате ГМЧ ЧМС и добавляет введенное расширение
//расширение вводить с точкой
string_t file_name_time(string_t a_extension = string_t());

//! @}

} //namespace irs

#endif //irstimeH
