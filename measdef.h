//! \file
//! \ingroup drivers_group
//! \brief Объявления типов для измерительных приборов.
//! Версия 0.1

#ifndef MESDEFH
#define MESDEFH

//! \addtogroup drivers_group
//! @{

// Тип для статуса
typedef enum _meas_status_t {
  meas_status_success = 0,
  meas_status_busy = 1,
  meas_status_error = 2
} meas_status_t;

//! @}

#endif //MESDEFH

