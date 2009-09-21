// Работа со временем
// Дата: 20.09.2009
// Ранняя дата: 10.07.2009

#include <irstime.h>
#include <irscpp.h>
#include <time.h>

// Преобразование числа в мс в строку в формате: ЧЧ:ММ:СС.МСЕ
irs::string irs::ms_to_strtime(irs::millisecond_t ms, bool show_ms)
{
  ostrstream time_strm;
  time_strm << setfill('0');
  irs::millisecond_t hour = ms/ms_in_hour;
  time_strm << setw(2) << hour;
  time_strm << ':';
  ms %= ms_in_hour;
  irs::millisecond_t minute = ms/ms_in_min;
  time_strm << setw(2) << minute;
  time_strm << ':';
  ms %= ms_in_min;
  irs::millisecond_t sec = ms/ms_in_sec;
  time_strm << setw(2) << sec;
  if (show_ms) {
    ms %= ms_in_sec;
    time_strm << '.';
    time_strm << setw(3) << ms;
  }
  time_strm << '\0';
  irs::string time_strg = time_strm.str();
  time_strm.rdbuf()->freeze(false);
  return time_strg;
}
// Счетчика ИРС в строку в формате: ЧЧ:ММ:СС.МСЕ
irs::string irs::cnt_to_strtime(counter_t cnt, bool show_ms)
{
  const irs::millisecond_t ms_in_sec = 1000;
  irs::millisecond_t ms =
    static_cast<irs::millisecond_t>(ms_in_sec*CNT_TO_DBLTIME(cnt));
  return ms_to_strtime(ms, show_ms);
}

irs::millisecond_t irs::system_time()
{
  time_t time_s_19700101 = 0;
  // time считывает время в секундах прошедшее начиная с 1 января 1970
  // Работает во всех Windows и UNIX-системах
  time(&time_s_19700101);
  struct tm time_struct = *localtime(&time_s_19700101);
  // Время в мс с начала суток
  millisecond_t time_ms =
    static_cast<millisecond_t>(
      time_struct.tm_hour*ms_in_hour +
      time_struct.tm_min*ms_in_min +
      time_struct.tm_sec*ms_in_sec
    );
  return time_ms;
}

// Запись в поток текущего времени
IRS_STREAMSPECDECL ostream &irs::stime(ostream &a_strm)
{
  irs::string time_strg = irs::ms_to_strtime(irs::system_time());
  a_strm << time_strg.c_str() << ' ';
  return a_strm;
}

