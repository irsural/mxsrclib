// Модуль счетчика
// Используем встроенный счетчик процессора
// Дата 24.10.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>

// Тип счетчика
typedef irs_i64 counter_t;
// Тип в котором осуществляются целочисленные расчеты
typedef irs_i64 calccnt_t;

// Максимальное время которое можно измерить
#define COUNTER_MAX IRS_I64_MAX/2
// Число секунд в интервале
extern counter_t SECONDS_PER_INTERVAL;
// Количество отсчетов в интервале
extern counter_t COUNTER_PER_INTERVAL;

// Инициализация счетчика
void counter_init();
// Обновление масштабных коэффициентов счетчика
void counter_refresh();
// Чтение счетчика
counter_t counter_get();
// Деинициализация счетчика
void counter_deinit();

#endif //COUNTERH

