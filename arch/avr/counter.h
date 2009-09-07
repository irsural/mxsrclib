// Модуль счетчика
// Дата 29.01.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>

// Тип счетчика
typedef irs_i32 counter_t;
// Тип в котором осуществляются целочисленные расчеты
typedef irs_i64 calccnt_t;

// Максимальное время которое можно измерить
#define COUNTER_MAX IRS_I32_MAX
// Число секунд в интервале
//extern inttime_t SECONDS_PER_INTERVAL;
#define SECONDS_PER_INTERVAL 4 //=62500/(Fclk/1024) в секундах
// Количество отсчетов в интервале
//extern counter_t COUNTER_PER_INTERVAL;
#define COUNTER_PER_INTERVAL 62500

// Инициализация счетчика
void counter_init();
// Чтение счетчика
counter_t counter_get();
// Деинициализация счетчика
void counter_deinit();

#endif //COUNTERH
