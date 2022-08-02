// ������ ��������
// ����: 17.05.2010
// ������ ����: 17.05.2010

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

// ���������� �������� � ���������
extern counter_t COUNTER_PER_INTERVAL = irs::cpu_traits_t::frequency();
// ����� ������ � ���������
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
    #if defined(IRS_NIIET_1921)
    if (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
       return;
    }
     high_dword += (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) + 1;// low_dword_cnt;
    #else
    // ��� ������ ����� COUNTFLAG ���������� ��� �����
    if (!SYSTICKCSR_bit.COUNTFLAG) {
      // SYSTICKCSR_bit.COUNTFLAG ����� ������� �� ������ ���� ����� 0, 
      // �� �� ������ ������ ����������� return, � �� ������ ������ �����
      return;
    }
    high_dword += SYSTICKRVR_bit.RELOAD + 1;// low_dword_cnt;
    #endif
    }
};

//  ������������� ��������
void counter_init()
{
  init_cnt++;
  if (init_cnt == 1)
  {
    // ���������� �������� � ���������
    COUNTER_PER_INTERVAL = irs::cpu_traits_t::frequency();
    // ����� ������ � ���������
    SECONDS_PER_INTERVAL = 1;
    
    #if defined(IRS_NIIET_1921)
    SysTick->LOAD = 0x00FFFFFF;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    #else
    SYSTICKRVR = 0x00FFFFFF;
    SYSTICKCSR_bit.CLKSOURCE = 1;
    SYSTICKCSR_bit.TICKINT = 1;
    SYSTICKCSR_bit.ENABLE = 1;
    #endif
    static timer_overflow_event_t timer_overflow_event;
    irs::arm::interrupt_array()->int_event_gen(irs::arm::sys_tick_int)->
      add(&timer_overflow_event);
  }
}

//  ������ ��������
counter_t counter_get()
{
  timer_overflow_interrupt_enabled = false;
  
  #if defined(IRS_NIIET_1921)
  counter_t SYSTICKCVR_buf = SysTick->VAL;
  if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
    SYSTICKCVR_buf = SysTick->VAL;
    high_dword += (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) + 1;//low_dword_cnt;
  } 
  #else
  counter_t SYSTICKCVR_buf = SYSTICKCVR;
  if (SYSTICKCSR_bit.COUNTFLAG) {
    SYSTICKCVR_buf = SYSTICKCVR;
    high_dword += SYSTICKRVR_bit.RELOAD + 1;//low_dword_cnt;
  } 
  #endif

  const counter_t result = high_dword + (low_dword_top - SYSTICKCVR_buf);
  timer_overflow_interrupt_enabled = true;
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


//  ��������������� ��������
void counter_deinit()
{
  init_cnt--;
  if (init_cnt == 0)
  {
    #if defined(IRS_NIIET_1921)
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    #else
    SYSTICKCSR_bit.CLKSOURCE = 0;
    SYSTICKCSR_bit.TICKINT = 0;
    SYSTICKCSR_bit.ENABLE = 0;
    #endif
  }
}
