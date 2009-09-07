// Модуль счетчика
// Дата: 1.02.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>
#include <time.h>

// Тип счетчика
typedef clock_t counter_t;
// Тип в котором осуществляются целочисленные расчеты
typedef irs_i32 calccnt_t;

// Максимальное время которое можно измерить
//#define COUNTER_MAX IRS_I32_MAX
extern counter_t COUNTER_MAX;
// Число секунд в интервале
//extern inttime_t SECONDS_PER_INTERVAL;
//#define SECONDS_PER_INTERVAL 1
// Количество отсчетов в интервале
//extern counter_t COUNTER_PER_INTERVAL;
//#define COUNTER_PER_INTERVAL CLOCKS_PER_SEC

// Число секунд в интервале
extern counter_t SECONDS_PER_INTERVAL;
// Количество отсчетов в секунде
extern counter_t COUNTER_PER_INTERVAL;

// Инициализация счетчика
void counter_init();
// Чтение счетчика
counter_t counter_get();
// Деинициализация счетчика
void counter_deinit();

#endif //COUNTERH
