// Модуль счетчика
// C++ Builder
// Используем встроенный счетчик процессора
// Дата: 14.04.2010
// Дата создания: 14.09.2009

#include <irsdefs.h>

#include <windows.h>

#include <limits>

#include <time.h>
#include <counter.h>

#include <irsfinal.h>

// Максимальное время которое можно измерить
counter_t COUNTER_MAX = std::numeric_limits<counter_t>::max();
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

