// Модуль счетчика
// C++ Builder
// Используем встроенный счетчик процессора
// Дата 25.11.2007

#pragma inline
//#include <windows.h>
//#include <winbase.h>
#include <time.h>
#include <counter.h>

// Время калибровки в тиках
const clock_t time_int_calibr = 9;
// Время калибровки, с. Состоит из числителя и знаменателя.
// Числитель
const calccnt_t time_calibr_num = 1;
// Знаменатель
const calccnt_t time_calibr_denom = 10;

// Число секунд в интервале
counter_t SECONDS_PER_INTERVAL = 1;
// Количество отсчетов в секунде
counter_t COUNTER_PER_INTERVAL = 300000000;

static irs_u32 count_init = 0;

// Чтение встроенного счетчика процессора
counter_t get_rdtsc(void)
{
  irs_u32 lo;
  irs_u32 hi;
  __asm db 0Fh, 31h
  __asm nop
  __asm nop
  __asm nop
  __asm nop
  __asm mov [lo], eax
  __asm mov [hi], edx
  irs_u64 rdtsc;
  IRS_LODWORD(rdtsc) = lo;
  IRS_HIDWORD(rdtsc) = hi;
  return rdtsc;
}
// Инициализация счетчика
void counter_init()
{
  if (!count_init) {
    calccnt_t cps = static_cast<calccnt_t>(CLOCKS_PER_SEC);
    clock_t time_int_calibr =
      static_cast<clock_t>(time_calibr_num*cps/time_calibr_denom);
	  clock_t start_clock = clock();
    clock_t cur_clock = clock();
    while (cur_clock != start_clock) {
      cur_clock = clock();
    }
    start_clock = cur_clock;
    counter_t start_counter = counter_get();
    while ((cur_clock - start_clock) < time_int_calibr) {
      cur_clock = clock();
    }
    counter_t stop_counter = counter_get();
    COUNTER_PER_INTERVAL = calccnt_t(SECONDS_PER_INTERVAL)*
      calccnt_t(CLOCKS_PER_SEC)*calccnt_t(stop_counter - start_counter)/
      calccnt_t(time_int_calibr);
  }
  count_init++;
}
// Чтение счетчика
counter_t counter_get()
{
	counter_t cnt = get_rdtsc();
	return cnt;
}
// Деинициализация счетчика
void counter_deinit()
{
  count_init--;
  if (!count_init) {
  }
}
