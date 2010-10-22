//! \file
//! \ingroup time_processing_group
//! \brief Модуль счетчика для avr
//!
//! Дата 01.10.2009\n
//! Ранняя дата 29.01.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>
#include <irslimits.h>

//! \addtogroup time_processing_group
//! @{

// Тип основного счетчика
typedef irs_i32 basic_counter_t;
// Тип счетчика
typedef irs::type_relative_t<basic_counter_t>::signed_type counter_t;
// Тип в котором осуществляются целочисленные расчеты
typedef irs::type_relative_t<basic_counter_t>::larger_type calccnt_t;

// Максимальное время которое можно измерить
#define COUNTER_MAX IRS_I32_MAX
// Число секунд в интервале
extern counter_t SECONDS_PER_INTERVAL;
//#define SECONDS_PER_INTERVAL 4 //=62500/(Fclk/1024) в секундах
// Количество отсчетов в интервале
extern counter_t COUNTER_PER_INTERVAL;
//#define COUNTER_PER_INTERVAL 62500

// Инициализация счетчика
void counter_init();
// Чтение счетчика
counter_t counter_get();
// Деинициализация счетчика
void counter_deinit();

//! @}

#endif //COUNTERH
