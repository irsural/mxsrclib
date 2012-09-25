// Модуль счетчика
// Дата: 17.05.2010
// Ранняя дата: 17.05.2010

#include <irsdefs.h>

#include <irsconfig.h>
#include <counter.h>
#include <irsarchint.h>
#include <irscpu.h>
#include <irserror.h>

#include <armioregs.h>

#include <irsfinal.h>

counter_t high_dword = 0;
enum
{
  low_dword_top = 0x00FFFFFF,
  low_dword_cnt = 0x01000000
};
irs_uarc init_cnt = 0;

// Количество отсчетов в интервале
extern counter_t COUNTER_PER_INTERVAL = irs::cpu_traits_t::frequency();
// Число секунд в интервале
extern counter_t SECONDS_PER_INTERVAL = 1;

namespace {

bool timer_overflow_interrupt_enabled = true;

} // empty namespace

class timer_overflow_event_t: public mxfact_event_t
{
public:
  virtual void exec()
  {
    mxfact_event_t::exec();
    if (!timer_overflow_interrupt_enabled) {
      return;
    }
    // При чтении флага COUNTFLAG происходит его сброс
    if (!SYSTICKCSR_bit.COUNTFLAG) {
      return;
    }
    high_dword += low_dword_cnt;
  }
};

//  Инициализация счетчика
void counter_init()
{
  init_cnt++;
  if (init_cnt == 1)
  {
    // Количество отсчетов в интервале
    COUNTER_PER_INTERVAL = irs::cpu_traits_t::frequency();
    // Число секунд в интервале
    SECONDS_PER_INTERVAL = 1;

    SYSTICKRVR = 0x00FFFFFF;
    SYSTICKCSR_bit.CLKSOURCE = 1;
    SYSTICKCSR_bit.TICKINT = 1;
    SYSTICKCSR_bit.ENABLE = 1;

    static timer_overflow_event_t timer_overflow_event;
    irs::arm::interrupt_array()->int_event_gen(irs::arm::sys_tick_int)->
      add(&timer_overflow_event);
  }
}

//  Чтение счётчика
counter_t counter_get()
{
  timer_overflow_interrupt_enabled = false;
  counter_t SYSTICKCVR_buf = SYSTICKCVR;
  if (SYSTICKCSR_bit.COUNTFLAG) {
    SYSTICKCVR_buf = SYSTICKCVR;
    high_dword += low_dword_cnt;
  }
  const counter_t result = high_dword + (low_dword_top - SYSTICKCVR_buf);
  timer_overflow_interrupt_enabled = true;
  return result;
}

//  Деинициализация счётчика
void counter_deinit()
{
  init_cnt--;
  if (init_cnt == 0)
  {
    SYSTICKCSR_bit.CLKSOURCE = 0;
    SYSTICKCSR_bit.TICKINT = 0;
    SYSTICKCSR_bit.ENABLE = 0;
  }
}
