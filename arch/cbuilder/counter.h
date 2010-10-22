//! \file
//! \ingroup time_processing_group
//! \brief Модуль счетчика для C++ Builder
//!
//! Дата: 14.04.2010\n
//! Дата создания: 14.09.2009

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>

#include <irslimits.h>

#include <irsfinal.h>

//! \addtogroup time_processing_group
//! @{

// Тип основного счетчика
typedef irs_i64 basic_counter_t;
// Тип счетчика
//typedef irs_i64 counter_t;
typedef irs::type_relative_t<basic_counter_t>::signed_type counter_t;
// Тип в котором осуществляются целочисленные расчеты
//typedef irs_i64 calccnt_t;
typedef irs::type_relative_t<basic_counter_t>::larger_type calccnt_t;

// Максимальное время которое можно измерить
extern counter_t COUNTER_MAX;
//#define COUNTER_MAX IRS_I64_MAX
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

//! @}

#endif //COUNTERH

