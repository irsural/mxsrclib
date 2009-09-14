// Модуль счетчика
// Linux
// Дата 13.09.2009

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>
#include <time.h>
#include <irslimits.h>

// Тип основного счетчика
typedef clock_t basic_counter_t;
// Тип счетчика
//typedef irs_i32 counter_t;
typedef irs::type_relative_t<basic_counter_t>::signed_type counter_t;
// Тип в котором осуществляются целочисленные расчеты
//typedef irs_i64 calccnt_t;
typedef irs::type_relative_t<basic_counter_t>::larger_type calccnt_t;

// Максимальное время которое можно измерить
extern counter_t COUNTER_MAX;
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
