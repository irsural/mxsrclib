// Работа с таймером
// Дата 18.07.2008

#include <timer.h>
#include <irsdefs.h>

namespace irs {

// Инициализация таймера
class init_timer_t
{
public:
  init_timer_t()
  {
    init_to_cnt();
  }
  ~init_timer_t()
  {
    deinit_to_cnt();
  }
};

} //namespace irs

// Класс таймера
irs::timer_t::timer_t(const int& num, const int& denom):
  m_to(0),
  m_time(0),
  m_start(false)
{
  init_timer();
  m_time = TIME_TO_CNT(num, denom);
}
irs::timer_t::timer_t(const counter_t& a_time):
  m_to(0),
  m_time(a_time),
  m_start(false)
{
  init_timer();
}
void irs::timer_t::set(const int& num, const int& denom)
{
  m_time = TIME_TO_CNT(num, denom);
  start();
}
void irs::timer_t::set(const counter_t& a_time)
{
  m_time = a_time;
  start();
}
void irs::timer_t::start()
{
  m_start = true;
  set_to_cnt(m_to, m_time);
}
void irs::timer_t::stop()
{
  m_start = false;
}
bool irs::timer_t::check()
{
  bool checked = false;
  if (m_start) {
    if (test_to_cnt(m_to)) {
      m_start = false;
      checked = true;
    }
  }
  return checked;
}
bool irs::timer_t::stopped()
{
  return !m_start;
}

// Класс циклического таймера
irs::loop_timer_t::loop_timer_t(const int& num,
  const int& denom):
  m_to(0),
  m_time(0)
{
  init_timer();
  m_time = TIME_TO_CNT(num, denom);
  set_to_cnt(m_to, m_time);
}
irs::loop_timer_t::loop_timer_t(const counter_t& a_time):
  m_to(0),
  m_time(a_time)
{
  init_timer();
  set_to_cnt(m_to, m_time);
}
void irs::loop_timer_t::set(const int& num, const int& denom)
{
  m_time = TIME_TO_CNT(num, denom);
}
void irs::loop_timer_t::set(const counter_t& a_time)
{
  m_time = a_time;
}
void irs::loop_timer_t::start()
{
  set_to_cnt(m_to, m_time);
}
bool irs::loop_timer_t::check()
{
  bool checked = false;
  if (test_to_cnt(m_to)) {
    set_to_cnt(m_to, m_time);
    checked = true;
  }
  return checked;
}

// Класс циклического таймера со строгой привязкой ко времени
irs::strong_loop_timer_t::strong_loop_timer_t(const int& num,
  const int& denom):
  m_to(0),
  m_time(0)
{
  init_timer();
  m_time = TIME_TO_CNT(num, denom);
  set_to_cnt(m_to, m_time);
}
irs::strong_loop_timer_t::strong_loop_timer_t(const counter_t& a_time):
  m_to(0),
  m_time(a_time)
{
  init_timer();
  set_to_cnt(m_to, m_time);
}
void irs::strong_loop_timer_t::set(const int& num,
  const int& denom)
{
  m_time = TIME_TO_CNT(num, denom);
}
void irs::strong_loop_timer_t::set(const counter_t& a_time)
{
  m_time = a_time;
}
void irs::strong_loop_timer_t::start()
{
  set_to_cnt(m_to, m_time);
}
bool irs::strong_loop_timer_t::check()
{
  bool checked = false;
  if (test_to_cnt(m_to)) {
    next_to_cnt(m_to, m_time);
    checked = true;
  }
  return checked;
}

// Измерение времени
irs::measure_time_t::measure_time_t():
  m_start_cnt(0)
{
  init_to_cnt();
  start();
}
irs::measure_time_t::~measure_time_t()
{
  deinit_to_cnt();
}
void irs::measure_time_t::start()
{
  m_start_cnt = counter_get();
}
double irs::measure_time_t::get()
{
  counter_t cur_cnt = counter_get();
  return CNT_TO_DBLTIME(cur_cnt - m_start_cnt);
}
counter_t irs::measure_time_t::get_cnt()
{
  counter_t cur_cnt = counter_get();
  return cur_cnt - m_start_cnt;
}

// Инициализация таймера
void irs::init_timer()
{
  static irs::init_timer_t init_timer_i;
}

// Установка таймаута t по переменной counter
void set_to_cnt(counter_t &to, counter_t t)
{
  to = counter_get() + t;
}
// Установка следующего таймаута t по переменной counter
void next_to_cnt(counter_t &to, counter_t t)
{
  to += t;
}
// Проверка таймаута по переменной counter
irs_bool test_to_cnt(counter_t to)
{
  irs_bool res = irs_false;
  if (counter_get() - to > 0) res = irs_true;
  return res;
}
// Инициализация таймера
void init_to_cnt()
{
  counter_init();
}
// Деинициализация таймера
void deinit_to_cnt()
{
  counter_deinit();
}

// Вычисление времени циклических процессов
void mx_time_int_t::init()
{
  init_to_cnt();
  _first = irs_true;
  cnt_prev = 0;
  show_int_to = 0;
  show_int_t = TIME_TO_CNT(1, 1);
  count_loop = 0;
  time_int = 0;
}
mx_time_int_t::mx_time_int_t()
{
  init();
}
mx_time_int_t::mx_time_int_t(counter_t time_cnt)
{
  init();
  set_show_int(time_cnt);
}
mx_time_int_t::mx_time_int_t(double time)
{
  init();
  set_show_int(time);
}
mx_time_int_t::~mx_time_int_t()
{
  deinit_to_cnt();
}
void mx_time_int_t::set_show_int(counter_t time_cnt)
{
  show_int_t = time_cnt;
}
void mx_time_int_t::set_show_int(double time)
{
  show_int_t = DBLTIME_TO_CNT(time);
}
const double &mx_time_int_t::operator()()
{
  if (_first) {
    _first = irs_false;
    cnt_prev = counter_get();
    set_to_cnt(show_int_to, show_int_t);
  } else {
    count_loop++;
    if (test_to_cnt(show_int_to)) {
      set_to_cnt(show_int_to, show_int_t);
      counter_t cnt_cur = counter_get();
      counter_t cnt_int = cnt_cur - cnt_prev;
      cnt_prev = cnt_cur;
      time_int = CNT_TO_DBLTIME(double(cnt_int)/double(count_loop));
      count_loop = 0;
    }
  }
  return time_int;
}
const double &mx_time_int_t::time()
{
  return time_int;
}

// Вычисление времени нециклических процессов
void mx_time_int_local_t::init()
{
  init_to_cnt();
  first_step = irs_true;
  _first = irs_true;
  cnt_prev = 0;
  show_int_to = 0;
  show_int_t = TIME_TO_CNT(1, 1);
  count_loop = 0;
  time_int = 0;
  cnt_int = 0;
}
mx_time_int_local_t::mx_time_int_local_t()
{
  init();
}
mx_time_int_local_t::mx_time_int_local_t(counter_t time_cnt)
{
  init();
  set_show_int(time_cnt);
}
mx_time_int_local_t::mx_time_int_local_t(double time)
{
  init();
  set_show_int(time);
}
mx_time_int_local_t::~mx_time_int_local_t()
{
  deinit_to_cnt();
}
void mx_time_int_local_t::set_show_int(counter_t time_cnt)
{
  show_int_t = time_cnt;
}
void mx_time_int_local_t::set_show_int(double time)
{
  show_int_t = DBLTIME_TO_CNT(time);
}
void mx_time_int_local_t::start()
{
  first_step = irs_false;
  if (_first) {
    _first = irs_false;
    set_to_cnt(show_int_to, show_int_t);
  }
  cnt_prev = counter_get();
}
const double &mx_time_int_local_t::operator()()
{
  if (first_step) {
    start();
  } else {
    counter_t cnt_cur = counter_get();
    first_step = irs_true;
    count_loop++;
    cnt_int += (cnt_cur - cnt_prev);
    if (test_to_cnt(show_int_to)) {
      set_to_cnt(show_int_to, show_int_t);
      //cnt_prev = cnt_cur;
      time_int = CNT_TO_DBLTIME(double(cnt_int)/double(count_loop));
      count_loop = 0;
      cnt_int = 0;
    }
  }
  return time_int;
}
const double &mx_time_int_local_t::time()
{
  return time_int;
}

