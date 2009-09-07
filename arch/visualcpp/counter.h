// Заголовки
// Модуль счетчика
// Watcom DOS
// Используем встроенный счетчик процессора
// Версия 0.3
#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>

// Тип счетчика
//typedef irs_i32 counter_t;
typedef irs_i64 counter_t;
// Тип в котором осуществляются целочисленные расчеты
typedef irs_i64 calccnt_t;

// Максимальное время которое можно измерить
#define COUNTER_MAX IRS_I64_MAX/2
// Число секунд в интервале
extern counter_t SECONDS_PER_INTERVAL;
//#define SECONDS_PER_INTERVAL 1
// Количество отсчетов в интервале
extern counter_t COUNTER_PER_INTERVAL;
//#define COUNTER_PER_INTERVAL 3000000000

// Инициализация счетчика
void counter_init();
// Чтение счетчика
counter_t counter_get();
// Деинициализация счетчика
void counter_deinit();

#endif //COUNTERH
