// ������ � ��������
// ����: 10.09.2009

#ifndef TIMERH
#define TIMERH

#include <irsdefs.h>
#include <counter.h>

// �������������� ������� � �������� � ���� ��������� ���������
// _TIME_NUM_ � ����������� _TIME_DENOM_ � ������� ��������
#define TIME_TO_CNT(_TIME_NUM_, _TIME_DENOM_)\
	( \
    counter_t(\
      ((calccnt_t)(_TIME_NUM_)) * COUNTER_PER_INTERVAL /\
      ( ((calccnt_t)(_TIME_DENOM_)) * SECONDS_PER_INTERVAL )\
    )\
  )
// �������������� �� ����������� � ���������� ������ ��������
#define S_TO_CNT(s) TIME_TO_CNT(s, 1)
// ����� � ������ ��������
#define MS_TO_CNT(ms) TIME_TO_CNT(ms, 1000)
// �������������� �� ����������� � ���������� ������ ��������
#define MCS_TO_CNT(mcs) TIME_TO_CNT(mcs, 1000000)
// ����� � ������ ��������
#define IRS_PAUSE(pause_time)\
  {\
    counter_t pause_to = 0;\
    set_to_cnt(pause_to, pause_time);\
    while (!test_to_cnt(pause_to));\
  }
// �������������� �������� � ������������ �����, �
#define CNT_TO_DBLTIME(_CNT_)\
  ( ((double)(SECONDS_PER_INTERVAL)) * ((double)(_CNT_)) / \
    ((double)(COUNTER_PER_INTERVAL)) )
// �������������� ������������� ������� � �������, �
#define DBLTIME_TO_CNT(_DBL_TIME_)\
  ( (counter_t)\
    ( \
      ((double)(COUNTER_PER_INTERVAL))*((double)(_DBL_TIME_)) / \
      ((double)(SECONDS_PER_INTERVAL)) \
    ) \
  )

namespace irs {

// ������������� �������
void init_timer();

// ������� ������� �������� �������� ����� ���������
// ��������� ����� � ��������
inline counter_t make_cnt(const counter_t& a_num, const counter_t& a_denom)
{
  init_timer();
  return TIME_TO_CNT(a_num, a_denom);
}
// � ��������
inline counter_t make_cnt_s(const float& a_time_s)
{
  init_timer();
  return DBLTIME_TO_CNT(a_time_s);
}
// � ��������
inline counter_t make_cnt_s(const double& a_time_s)
{
  init_timer();
  return DBLTIME_TO_CNT(a_time_s);
}
// � ��������
inline counter_t make_cnt_s(const int& a_time_s)
{
  init_timer();
  return TIME_TO_CNT(a_time_s, 1);
}
// � ������������
inline counter_t make_cnt_ms(const int& a_time_ms)
{
  init_timer();
  return TIME_TO_CNT(a_time_ms, 1000);
}
// � �������������
inline counter_t make_cnt_mcs(const int& a_time_mcs)
{
  init_timer();
  return TIME_TO_CNT(a_time_mcs, 1000000);
}
// � �������������
inline counter_t make_cnt_us(const int& a_time_us)
{
  init_timer();
  return TIME_TO_CNT(a_time_us, 1000000);
}

//#define irs_timer_num_def 0
//#define irs_timer_denom_def 1000
enum {
  timer_num_def = 0,
  timer_denom_def = 1000
};
  
// ����� �������
class timer_t
{
public:

  timer_t(const int& num = timer_num_def,
    const int& denom = timer_denom_def);
  timer_t(const counter_t& a_time);
  void set(const int& num = timer_num_def,
    const int& denom = timer_denom_def);
  void set(const counter_t& a_time);
  inline counter_t get() const;
  void start();
  void stop();
  bool check();
  bool stopped();
private:
  counter_t m_to;
  counter_t m_time;
  bool m_start;
};
inline counter_t timer_t::get() const
{
  return m_time;
}

// ����� ������������ �������
class loop_timer_t
{
public:
  loop_timer_t(const int& num = timer_num_def,
    const int& denom = timer_denom_def);
  loop_timer_t(const counter_t& a_time);
  void set(const int& num = timer_num_def,
    const int& denom = timer_denom_def);
  void set(const counter_t& a_time);
  inline counter_t get();
  void start();
  bool check();
private:
  counter_t m_to;
  counter_t m_time;
};
inline counter_t loop_timer_t::get()
{
  return m_time;
}

// ����� ������������ ������� �� ������� ��������� �� �������
class strong_loop_timer_t
{
public:
  strong_loop_timer_t(const int& num = timer_num_def,
    const int& denom = timer_denom_def);
  strong_loop_timer_t(const counter_t& a_time);
  void set(const int& num = timer_num_def,
    const int& denom = timer_denom_def);
  void set(const counter_t& a_time);
  inline counter_t get();
  void start();
  bool check();
private:
  counter_t m_to;
  counter_t m_time;
};
inline counter_t strong_loop_timer_t::get()
{
  return m_time;
}

// ��������� �������
class measure_time_t
{
public:
  measure_time_t();
  ~measure_time_t();
  void start();
  double get() const;
  counter_t get_cnt() const;
private:
  counter_t m_start_cnt;
};

inline void pause(counter_t a_counter)
{
  irs::timer_t pause_timer(a_counter);
  pause_timer.start();
  while(!pause_timer.check());
}

} //namespace irs

// ��������� �������� t �� ���������� counter
void set_to_cnt(counter_t &to, counter_t t);
// ��������� ���������� �������� t �� ���������� counter
void next_to_cnt(counter_t &to, counter_t t);
// �������� �������� �� ���������� counter
irs_bool test_to_cnt(counter_t to);
// ������������� �������
void init_to_cnt();
// ��������������� �������
void deinit_to_cnt();

// ���������� ������� ����������� ���������
class mx_time_int_t
{
  irs_bool _first;// = irs_true;
  counter_t cnt_prev;// = 0;
  counter_t show_int_to;// = 0;
  counter_t show_int_t; //TIME_TO_CNT(1, 1)
  irs_u32 count_loop;// = 0;
  double time_int;// = 0;

  void init();
public:
  mx_time_int_t();
  mx_time_int_t(counter_t time_cnt);
  mx_time_int_t(double time);
  ~mx_time_int_t();
  const double &operator()();
  void set_show_int(counter_t time_cnt);
  void set_show_int(double time);
  const double &time();
};

// ���������� ������� ������������� ���������
class mx_time_int_local_t
{
  irs_bool first_step;// = irs_false;
  irs_bool _first;// = irs_true;
  counter_t cnt_prev;// = 0;
  counter_t show_int_to;// = 0;
  counter_t show_int_t;// TIME_TO_CNT(1, 1)
  irs_u32 count_loop;// = 0;
  double time_int;// = 0;
  counter_t cnt_int;// = 0;

  void init();
public:
  mx_time_int_local_t();
  mx_time_int_local_t(counter_t time_cnt);
  mx_time_int_local_t(double time);
  ~mx_time_int_local_t();
  void set_show_int(counter_t time_cnt);
  void set_show_int(double time);
  void start();
  const double &operator()();
  const double &time();
};
#endif //TIMERH

