//! \file
//! \ingroup time_processing_group
//! \brief Работа со временем
//!
//! Дата: 15.09.2010\n
//! Ранняя дата: 10.07.2009

#ifndef irstimeH
#define irstimeH

#include <irsdefs.h>

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
  basic_stime(IRS_STRING_OSTREAM &a_strm)
{
  time_t timer = time(NULL);
  const tm* date = localtime(&timer);
  a_strm << setfill(static_cast<T>('0'));
  a_strm << setw(2) << date->tm_hour << static_cast<T>(':');
  a_strm << setw(2) << date->tm_min << static_cast<T>(':');
  a_strm << setw(2) << date->tm_sec << static_cast<T>(' ');
  return a_strm;
}
IRS_STREAMSPECDECL ostream_t &stimet(ostream_t &a_strm);
IRS_STREAMSPECDECL ostream &stime(ostream &a_strm);
IRS_STREAMSPECDECL wostream &wstime(wostream &a_strm);
// Запись в поток текущей даты и времени
IRS_STRING_TEMPLATE
inline IRS_STREAMSPECDECL IRS_STRING_OSTREAM &
  basic_sdatetime(IRS_STRING_OSTREAM &a_stream)
{
  time_t timer = time(NULL);
  const tm* date = localtime(&timer);
  a_stream << setfill(static_cast<T>('0'));
  a_stream << setw(2) << date->tm_mday << static_cast<T>('.');
  a_stream << setw(2) << (date->tm_mon + 1) << static_cast<T>('.');
  a_stream << setw(4) << (date->tm_year + 1900) << static_cast<T>(' ');
  a_stream << setw(2) << date->tm_hour << static_cast<T>(':');
  a_stream << setw(2) << date->tm_min << static_cast<T>(':');
  a_stream << setw(2) << date->tm_sec << static_cast<T>(' ');
  return a_stream;
}
IRS_STREAMSPECDECL ostream_t &sdatetimet(ostream_t &a_stream);
IRS_STREAMSPECDECL ostream &sdatetime(ostream &a_stream);
IRS_STREAMSPECDECL wostream &wsdatetime(wostream &a_stream);
//возвращает текущую дату в формате ГМЧ ЧМС и добавляет введенное расширение
//расширение вводить с точкой
string_t file_name_time(string_t a_extension = string_t());

//! @}

} //namespace irs

#endif //irstimeH
