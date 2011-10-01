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
IRS_STREAMSPECDECL ostream_t &stime(ostream_t &a_strm);
// Запись в поток текущей даты и времени
IRS_STREAMSPECDECL ostream_t &sdatetime(ostream_t &a_stream);
//возвращает текущую дату в формате ГМЧ ЧМС и добавляет введенное расширение
//расширение вводить с точкой
string_t file_name_time(string_t a_extension = string_t());

//! @}

} //namespace irs

#endif //irstimeH
