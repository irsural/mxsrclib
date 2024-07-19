// ������ ��������
// ����: 22.01.2018

#include <irsdefs.h>

#include <irsconfig.h>
#include <counter.h>
#include <irscpu.h>
#include <irserror.h>

#ifdef IRS_STM32H7xx
#include <stm32h743xx.h>
#include <stm32h7xx_hal.h>
#elif IRS_STM32F7xx
#include <stm32f746xx.h>
#include <stm32f7xx_hal.h>
#endif

#include <irsfinal.h>

counter_t high_dword = 0;
enum
{
  low_dword_top = 0x00FFFFFF,
  low_dword_cnt = 0x01000000
};
irs_uarc init_cnt = 0;

// ���������� �������� � ���������
counter_t COUNTER_PER_INTERVAL = static_cast<counter_t>(irs::cpu_traits_t::frequency());
// ����� ������ � ���������
counter_t SECONDS_PER_INTERVAL = 1;

namespace {

bool timer_overflow_interrupt_enabled = true;

} // empty namespace

//class timer_overflow_event_t: public mxfact_event_t
//{
//public:
//  virtual void exec()
//  {
//    mxfact_event_t::exec();
//    if (!timer_overflow_interrupt_enabled) {
//      return;
//    }
//    // ��� ������ ����� COUNTFLAG ���������� ��� �����
//    if (/*!SYSTICKCSR_bit.COUNTFLAG*/ !(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
//      // SYSTICKCSR_bit.COUNTFLAG ����� ������� �� ������ ���� ����� 0, 
//      // �� �� ������ ������ ����������� return, � �� ������ ������ �����
//      return;
//    }
//    high_dword += /*SYSTICKRVR_bit.RELOAD + 1*/ (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) + 1;// low_dword_cnt;
//  }
//};

extern "C" {
  
//���������� SysTick
void SysTick_Handler(void)
{
  if (!timer_overflow_interrupt_enabled) {
    return;
  }
  // ��� ������ ����� COUNTFLAG ���������� ��� �����
  if (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
    // SYSTICKCSR_bit.COUNTFLAG ����� ������� �� ������ ���� ����� 0, 
    // �� �� ������ ������ ����������� return, � �� ������ ������ �����
    return;
  }
  high_dword += (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) + 1;
}

uint32_t HAL_GetTick(void)
{
  const counter_t counter_per_interval = COUNTER_PER_INTERVAL/1000; 
  uint32_t msec = (SECONDS_PER_INTERVAL*(counter_get()/counter_per_interval));
  return msec;
}

} // extern "C" {

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
    
    HAL_SYSTICK_Config(0x01000000);
    HAL_NVIC_SetPriority(SysTick_IRQn, 2, 0U);


//    SYSTICKRVR = 0x00FFFFFF;
//    SYSTICKCSR_bit.CLKSOURCE = 1;
//    SYSTICKCSR_bit.TICKINT = 1;
//    SYSTICKCSR_bit.ENABLE = 1;

    SysTick->LOAD = 0x00FFFFFF;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    
    //static timer_overflow_event_t timer_overflow_event;
    //irs::arm::interrupt_array()->int_event_gen(irs::arm::sys_tick_int)->
      //add(&timer_overflow_event);
  }
}

//  ������ ��������
counter_t counter_get()
{
  timer_overflow_interrupt_enabled = false;
  counter_t SYSTICKCVR_buf = /*SYSTICKCVR*/ SysTick->VAL;
  if (/*SYSTICKCSR_bit.COUNTFLAG*/SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
    SYSTICKCVR_buf = /*SYSTICKCVR*/ SysTick->VAL;
    high_dword += /*SYSTICKRVR_bit.RELOAD + 1*/ (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) + 1; //low_dword_cnt;
  }
  const counter_t result = high_dword + (low_dword_top - SYSTICKCVR_buf);
  timer_overflow_interrupt_enabled = true;
  return result;
}

//  ��������������� ��������
void counter_deinit()
{
  init_cnt--;
  if (init_cnt == 0)
  {/*
    SYSTICKCSR_bit.CLKSOURCE = 0;
    SYSTICKCSR_bit.TICKINT = 0;
    SYSTICKCSR_bit.ENABLE = 0;
    */
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  }
}
