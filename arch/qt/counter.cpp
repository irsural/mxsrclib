// Модуль счетчика
// C++ Builder
// Используем встроенный счетчик процессора
// Дата 17.11.2008

#include <irsdefs.h>

#ifdef IRS_WIN32
# include <winsock2.h>
# include <time.h>
#else // !IRS_WIN32
# include <time.h>
# include <stdio.h>
# include <inttypes.h>
#endif // !IRS_WIN32

#include <counter.h>

#include <irsfinal.h>

#ifdef IRS_WIN32
// Число секунд в интервале
counter_t SECONDS_PER_INTERVAL = 1;
// Количество отсчетов в секунде
counter_t COUNTER_PER_INTERVAL = 300000000;

static irs_u32 count_init = 0;

// Инициализация счетчика
void counter_init()
{
  if (!count_init) {
    counter_refresh();
  }
  count_init++;
}
// Обновление масштабных коэффициентов счетчика
void counter_refresh()
{
  LARGE_INTEGER cpu_freq;
  memset(static_cast<void*>(&cpu_freq), 0, sizeof(cpu_freq));
  QueryPerformanceFrequency(&cpu_freq);
  COUNTER_PER_INTERVAL = static_cast<counter_t>(cpu_freq.QuadPart);
}
// Чтение счетчика
counter_t counter_get()
{
  LARGE_INTEGER cpu_cnt;
  QueryPerformanceCounter(&cpu_cnt);
  return static_cast<counter_t>(cpu_cnt.QuadPart);
}
// Деинициализация счетчика
void counter_deinit()
{
  count_init--;
  if (!count_init) {
  }
}
#else // LINUX
// Число секунд в интервале
counter_t SECONDS_PER_INTERVAL = 1;
// Количество отсчетов в секунде
counter_t COUNTER_PER_INTERVAL = 1000000000;

static irs_u32 count_init = 0;

// Инициализация счетчика
void counter_init()
{
  if (!count_init) {
    counter_refresh();
  }
  count_init++;
}
// Обновление масштабных коэффициентов счетчика
void counter_refresh()
{
    timespec ts2;
    if (clock_getres(CLOCK_MONOTONIC, &ts2) != 0) {
      return;
    }

    const int nsec_per_sec = 1e9;

    if ((ts2.tv_sec == 0) && (ts2.tv_nsec != 0)) {
      COUNTER_PER_INTERVAL = nsec_per_sec/ts2.tv_nsec;
    }
}
// Чтение счетчика
counter_t counter_get()
{
  timespec ts1;

  if (clock_gettime(CLOCK_MONOTONIC, &ts1) != 0) {
    return 0;
  }

  const int nsec_per_sec = 1e9;
  int64_t nsec_count = ts1.tv_nsec + ts1.tv_sec * nsec_per_sec;
  return nsec_count;
}
// Деинициализация счетчика
void counter_deinit()
{
  count_init--;
  if (!count_init) {
  }
}
#endif
