// Утилиты для AVR
// Дата: 01.04.2010
// Ранняя дата: 11.04.2008

#ifndef IRSAVRUTILH
#define IRSAVRUTILH

#include <irsdefs.h>

#include <irsstd.h>
#include <timer.h>

#include <irsfinal.h>

namespace irs {

// Мигание портом
class blink_t
{
  counter_t m_blink_to;
  counter_t m_blink_time;
  p_avr_port_t mp_port_set;
  p_avr_port_t mp_port_dir;
  irs_u8 m_bit;
public:
  blink_t(irs_avr_port_t a_port, irs_u8 a_bit,
    counter_t a_blink_time = 0):
    m_blink_to(0),
    m_blink_time(0),
    mp_port_set(avr_port_map[a_port].set),
    mp_port_dir(avr_port_map[a_port].dir),
    m_bit(a_bit)
  {
    (*mp_port_dir) |= (1 << m_bit);
    (*mp_port_set) &= ~(1 << m_bit);
    //(*mp_port_set) |= (1 << m_bit);
    init_to_cnt();
    if (!a_blink_time) m_blink_time = TIME_TO_CNT(1, 10);
    else m_blink_time = a_blink_time;
    set_to_cnt(m_blink_to, m_blink_time);
  }
  ~blink_t()
  {
    deinit_to_cnt();
  }
  void operator()()
  {
    if (test_to_cnt(m_blink_to)) {
      set_to_cnt(m_blink_to, m_blink_time);
      (*mp_port_set) ^= (1 << m_bit);
    }
  }
  void set()
  {
    (*mp_port_set) |= (1 << m_bit);
  }
  void clear()
  {
    (*mp_port_set) &= ~(1 << m_bit);
  }
  void flip()
  {
    (*mp_port_set) ^= (1 << m_bit);
  }
};

// Порт включается после заданного количества проходов
template <class T>
class count_loop_t
{
  p_avr_port_t mp_port_set;
  p_avr_port_t mp_port_dir;
  irs_u8 m_bit;
  T m_counter;
  T m_counter_threshold;
public:
  count_loop_t(irs_avr_port_t a_port, irs_u8 a_bit, T a_counter_threshold = 0):
    mp_port_set(avr_port_map[a_port].set),
    mp_port_dir(avr_port_map[a_port].dir),
    m_bit(a_bit),
    m_counter(0),
    m_counter_threshold(a_counter_threshold)
  {
    (*mp_port_dir) |= (1 << m_bit);
    (*mp_port_set) &= ~(1 << m_bit);
  }
  void operator()()
  {
    m_counter++;
    if (m_counter >= m_counter_threshold) (*mp_port_set) |= (1 << m_bit);
  }
  void set()
  {
    (*mp_port_set) |= (1 << m_bit);
  }
  void clear(T a_counter_threshold = 0)
  {
    m_counter_threshold = a_counter_threshold;
    m_counter = 0;
    (*mp_port_set) &= ~(1 << m_bit);
  }
};

}

#endif //IRSAVRUTILH
