//! \file
//! \ingroup time_processing_group
//! \brief Работа со временем
//!
//! Дата: 23.09.2010\n
//! Ранняя дата: 10.07.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irstime.h>
#include <irscpp.h>
#include <time.h>

#include <irsfinal.h>

// Преобразование числа в мс в строку в формате: ЧЧ:ММ:СС.МСЕ
irs::string_t irs::ms_to_strtime(irs::millisecond_t ms, bool show_ms)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  ostringstream_t time_strm;
  #else //IRS_FULL_STDCPPLIB_SUPPORT
  ostrstream time_strm;
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
  time_strm << setfill(irst('0'));
  millisecond_t hour = ms/ms_in_hour;
  time_strm << setw(2) << hour;
  time_strm << irst(':');
  ms %= ms_in_hour;
  millisecond_t minute = ms/ms_in_min;
  time_strm << setw(2) << minute;
  time_strm << irst(':');
  ms %= ms_in_min;
  millisecond_t sec = ms/ms_in_sec;
  time_strm << setw(2) << sec;
  if (show_ms) {
    ms %= ms_in_sec;
    time_strm << irst('.');
    time_strm << setw(3) << ms;
  }
  time_strm << irst('\0');
  string_t time_strg = time_strm.str();
  #ifndef IRS_FULL_STDCPPLIB_SUPPORT
  time_strm.rdbuf()->freeze(false);
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
  return time_strg;
}
// Счетчика ИРС в строку в формате: ЧЧ:ММ:СС.МСЕ
irs::string_t irs::cnt_to_strtime(counter_t cnt, bool show_ms)
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
IRS_STREAMSPECDECL irs::ostream_t &irs::stime(ostream_t &a_strm)
{
  irs::string_t time_strg = irs::ms_to_strtime(irs::system_time());
  a_strm << time_strg.c_str() << ' ';
  return a_strm;
}

// Запись в поток текущей даты и времени
IRS_STREAMSPECDECL irs::ostream_t &irs::sdatetime(ostream_t &a_stream)
{
  time_t timer = time(NULL);
  const tm* date = localtime(&timer);
  a_stream << setfill(irst('0'));
  a_stream << setw(2) << date->tm_mday << irst('.');
  a_stream << setw(2) << (date->tm_mon + 1) << irst('.');
  a_stream << setw(4) << (date->tm_year + 1900) << irst(' ');
  a_stream << setw(2) << date->tm_hour << irst(':');
  a_stream << setw(2) << date->tm_min << irst(':');
  a_stream << setw(2) << date->tm_sec << irst(' ');
  return a_stream;
}

irs::string_t irs::file_name_time(string_t a_extension)
{
  time_t timer = time(NULL);
  const tm* date = localtime(&timer);
  ostrstream stream;
  //stream.imbue(locale::classic());
  stream << setfill('0');
  stream << setw(4) << (date->tm_year + 1900);
  stream << setw(2) << (date->tm_mon + 1);
  stream << setw(2) << date->tm_mday << ' ';
  stream << setw(2) << date->tm_hour;
  stream << setw(2) << date->tm_min;
  stream << setw(2) << date->tm_sec;
  stream << IRS_SIMPLE_FROM_TYPE_STR(a_extension.c_str()) << '\0';
  irs_string_t file_name_time = stream.str();
  stream.rdbuf()->freeze(false);
  return IRS_TYPE_FROM_SIMPLE_STR(file_name_time.c_str());
}

