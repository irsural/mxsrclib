//! \file
//! \ingroup time_processing_group
//! \brief ������ �� ��������
//!
//! ����: 15.09.2010\n
//! ������ ����: 10.07.2009

#ifndef irstimeH
#define irstimeH

#include <irsdefs.h>

#include <time.h>

#include <irsstring.h>
#include <irsstrdefs.h>
#include <timer.h>

#include <irsfinal.h>

#ifndef IRS_USE_ST_RTC
# if defined(IRS_STM32F4xx) && defined(USE_STDPERIPH_DRIVER)
#   define IRS_USE_ST_RTC 1
# else
#   define IRS_USE_ST_RTC 0
# endif
#endif // IRS_USE_ST_RTC


namespace irs {

//! \addtogroup time_processing_group
//! @{

typedef irs_u32 millisecond_t;

const millisecond_t min_in_hour = 60;
const millisecond_t sec_in_min = 60;
const millisecond_t ms_in_sec = 1000;
const millisecond_t ms_in_min = ms_in_sec*sec_in_min;
const millisecond_t ms_in_hour = ms_in_min*min_in_hour;

// �������������� ����� � �� � ������ � �������: ��:��:��.���
string_t ms_to_strtime(millisecond_t ms, bool show_ms = false);
// �������� ��� � ������ � �������: ��:��:��.���
string_t cnt_to_strtime(counter_t cnt, bool show_ms = false);

millisecond_t system_time();

// ������ � ����� �������� �������
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

/*#if IRS_USE_ST_RTC
double get_time_from_stm32();
double get_time_double();
#endif // IRS_USE_ST_RTC*/
double get_time_double();

// ������ � ����� ������� ���� � �������
IRS_STRING_TEMPLATE
inline IRS_STREAMSPECDECL IRS_STRING_OSTREAM &
  basic_sdatetime(IRS_STRING_OSTREAM &a_stream)
{
  #ifdef __ICCARM__
  time_t time_s = time(NULL);
  double seconds_double = get_time_double();
  int milliseconds = 0;
  /*if (time_s == static_cast<time_t>(-1)) {
    seconds_double = CNT_TO_DBLTIME(counter_get());
    time_s = static_cast<time_t>(seconds_double);
  } else {

    #if IRS_USE_ST_RTC
    seconds_double = get_time_from_stm32();
    #else // !IRS_USE_ST_RTC
    seconds_double = CNT_TO_DBLTIME(counter_get());
    time_s = static_cast<time_t>(seconds_double);
    #endif // !IRS_USE_ST_RTC
  }*/
  //time = time(NULL);
  milliseconds = static_cast<int>(
    (seconds_double - static_cast<irs_u64>(seconds_double))*1000);
  #else // !__ICCARM__
  time_t time_s = time(NULL);
  #endif // !__ICCARM__
  
  ios::fmtflags flags = a_stream.flags();
  streamsize precision = a_stream.precision();
  
  const tm* date = localtime(&time_s);
  a_stream << setfill(static_cast<IRS_STRING_CHAR_TYPE>('0'));
  a_stream << right << dec;
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
  #ifdef __ICCARM__
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(':');
  a_stream << setw(3) << milliseconds;
  #endif // __ICCARM__
  a_stream << static_cast<IRS_STRING_CHAR_TYPE>(' ');
  
  a_stream.precision(precision);
  a_stream.flags(flags);
  
  return a_stream;
}
IRS_STREAMSPECDECL ostream_t &sdatetimet(ostream_t &a_stream);
IRS_STREAMSPECDECL ostream &sdatetime(ostream &a_stream);
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
IRS_STREAMSPECDECL wostream &wsdatetime(wostream &a_stream);
#endif //IRS_FULL_STDCPPLIB_SUPPORT

//���������� ������� ���� � ������� ��� ��� � ��������� ��������� ����������
//���������� ������� � ������
string_t file_name_time(string_t a_extension = string_t());

//! @}

struct time_remain_t {
  time_t time;
  counter_t remain;
};

class cur_time_t
{
public:
  cur_time_t();
  ~cur_time_t();
  void set(time_t a_time);
  time_t get();
  time_remain_t get_remain();
private:
  double m_cur_time;
  time_remain_t m_cur_time_remain;
  counter_t m_time_set;
  bool m_set_time;
};

cur_time_t* cur_time();

} //namespace irs

#endif //irstimeH
