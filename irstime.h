// Работа со временем
// Дата: 10.07.2009

#ifndef irstimeH
#define irstimeH

#include <irsstd.h>

namespace irs {

typedef irs_u32 millisecond_t;

const millisecond_t min_in_hour = 60;
const millisecond_t sec_in_min = 60;
const millisecond_t ms_in_sec = 1000;
const millisecond_t ms_in_min = ms_in_sec*sec_in_min;
const millisecond_t ms_in_hour = ms_in_min*min_in_hour;

// Преобразование числа в мс в строку в формате: ЧЧ:ММ:СС.МСЕ
irs::string ms_to_strtime(millisecond_t ms, bool show_ms = false);
// Счетчика ИРС в строку в формате: ЧЧ:ММ:СС.МСЕ
irs::string cnt_to_strtime(counter_t cnt, bool show_ms = false);

millisecond_t system_time();
// Запись в поток текущего времени
ostream &stime(ostream &a_strm);

} //namespace irs

#endif //irstimeH
