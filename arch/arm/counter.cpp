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

#ifdef USE_HAL_DRIVER
# if defined(IRS_STM32F4xx)
#   include "stm32f4xx_hal.h"
# elif defined(IRS_STM32F7xx)
#   include "stm32f7xx_hal.h"
# endif // defined(IRS_STM32F7xx)
#endif // USE_HAL_DRIVER

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
bool interrupt_handle_overflow = true;

} // empty namespace

class timer_overflow_event_t: public mxfact_event_t
{
public:
  virtual void exec()
  {
    mxfact_event_t::exec();
    if (!timer_overflow_interrupt_enabled) {
      interrupt_handle_overflow = false;
      return;
    }
    // При чтении флага COUNTFLAG происходит его сброс
    if (!SYSTICKCSR_bit.COUNTFLAG) {
      // SYSTICKCSR_bit.COUNTFLAG здесь никогда не должен быть равен 0, 
      // но на всякий случай выполнается return, а не только чтение флага
      return;
    }
    high_dword += SYSTICKRVR_bit.RELOAD + 1;// low_dword_cnt;
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
  interrupt_handle_overflow = true;
  bool function_handle_overflow = false;
  counter_t SYSTICKCVR_buf = SYSTICKCVR;
  if (SYSTICKCSR_bit.COUNTFLAG) {
    SYSTICKCVR_buf = SYSTICKCVR;
    high_dword += SYSTICKRVR_bit.RELOAD + 1;//low_dword_cnt;
    function_handle_overflow = true;
  }
  const counter_t result = high_dword + (low_dword_top - SYSTICKCVR_buf);
  timer_overflow_interrupt_enabled = true;
  if (!function_handle_overflow && !interrupt_handle_overflow) {
    high_dword += SYSTICKRVR_bit.RELOAD + 1;
  }
  return result;
}

#ifdef USE_HAL_DRIVER
uint32_t HAL_GetTick(void)
{
  const counter_t counter_per_interval = COUNTER_PER_INTERVAL/1000; 
  uint32_t msec = (SECONDS_PER_INTERVAL*(counter_get()/counter_per_interval));
  return msec;
}
#endif // USE_HAL_DRIVER


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
