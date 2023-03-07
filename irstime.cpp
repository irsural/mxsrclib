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
#include <irsdev.h>

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
IRS_STREAMSPECDECL irs::ostream_t &irs::stimet(ostream_t &a_stream)
{
  return basic_stime(a_stream);
}
IRS_STREAMSPECDECL ostream &irs::stime(ostream &a_stream)
{
  return basic_stime(a_stream);
}
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
IRS_STREAMSPECDECL wostream &irs::wstime(wostream &a_stream)
{
  return basic_stime(a_stream);
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

/*#if (defined(IRS_STM32F4xx) && defined(USE_STDPERIPH_DRIVER)) ||\
  defined(USE_HAL_DRIVER)
double irs::get_time_from_stm32()
{
  return irs::arm::st_rtc_t::get_instance()->get_time_double();
}
#endif // defined(IRS_STM32F4xx) && defined(USE_STDPERIPH_DRIVER)*/
double irs::get_time_double()
{
  #if IRS_USE_ST_RTC
  return irs::arm::st_rtc_t::get_instance()->get_time_double();
  #else // !IRS_USE_ST_RTC
  time_t time_s = time(NULL);
  if (time_s == static_cast<time_t>(-1)) {
    return CNT_TO_DBLTIME(counter_get());
  } else {
    return time_s;
  }
  #endif // !IRS_USE_ST_RTC
}

// Запись в поток текущей даты и времени
IRS_STREAMSPECDECL irs::ostream_t &irs::sdatetimet(ostream_t &a_stream)
{
  return basic_sdatetime(a_stream);
}
IRS_STREAMSPECDECL ostream &irs::sdatetime(ostream &a_stream)
{
  return basic_sdatetime(a_stream);
}
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
IRS_STREAMSPECDECL wostream &irs::wsdatetime(wostream &a_stream)
{
  return basic_sdatetime(a_stream);
}
#endif //IRS_FULL_STDCPPLIB_SUPPORT

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


irs::cur_time_t::cur_time_t():
  m_cur_time(0),
  m_cur_time_remain(),
  m_time_set(0),
  m_set_time(false)
{
  memset(reinterpret_cast<void*>(&m_cur_time_remain), 0,
    sizeof(m_cur_time_remain));
}

irs::cur_time_t::~cur_time_t()
{
}

void irs::cur_time_t::set(time_t a_time)
{
  m_cur_time = static_cast<double>(a_time);
  m_time_set = counter_get();
  m_set_time = true;
}

time_t irs::cur_time_t::get()
{
  time_remain_t time_remain = get_remain();
  return time_remain.time;
}

irs::time_remain_t irs::cur_time_t::get_remain()
{
  time_t local_time = 0;
  if (m_set_time) {
    double delta_time = CNT_TO_DBLTIME(counter_get() - m_time_set);
    double time_s = m_cur_time + delta_time;
    local_time = static_cast<time_t>(time_s);
    m_cur_time_remain.time = local_time;
    m_cur_time_remain.remain = static_cast<counter_t>(time_s - local_time);
  } else {
    time(&local_time);
    time_t gm_time = mktime(gmtime(&local_time));
    time_t loc_time = mktime(localtime(&local_time));
    local_time += static_cast<time_t>(loc_time - gm_time);
    m_cur_time_remain.time = local_time;
    m_cur_time_remain.remain = 0;
  }
  return m_cur_time_remain;
}

irs::cur_time_t* irs::cur_time()
{
  static irs::cur_time_t cur_time_sys;
  return &cur_time_sys;
}

#ifdef __ICCARM__

#if IRS_USE_ST_RTC

#if __VER__ < 8000000
_STD_BEGIN
#endif // __VER__

#if !_DLIB_TIME_ALLOW_64

__time32_t (__time32)(__time32_t *t)
{
  #ifdef IRS_STM32_F2_F4_F7
  if (t)
  {
    *t = irs::arm::st_rtc_t::get_instance()->get_time();
    return *t;
  }
  return irs::arm::st_rtc_t::get_instance()->get_time();
  #else
  return 0;
  #endif
}

#else

__time64_t (__time64)(__time64_t *t)
{
  #ifdef IRS_STM32_F2_F4_F7
  if (t)
  {
    *t = irs::arm::st_rtc_t::get_instance()->get_time();
    return *t;
  }
  return irs::arm::st_rtc_t::get_instance()->get_time();
  #else
  return 0;
  #endif
}
//CLOCKS_PER_SEC


#endif // _DLIB_TIME_ALLOW_64

#if __VER__ < 8000000
_STD_END
#endif // __VER__

#endif // IRS_USE_ST_RTC

#endif // __ICCARM__
