//! \file
//! \ingroup configuration_group
//! \brief Инклуды, которые должны быть включены раньше других
//!
//! Дата: 04.10.2011\n
//! Ранняя дата: 16.09.2009

#ifndef IRSDEFSH
#define IRSDEFSH

#include <irsdefsbase.h>

//! \addtogroup configuration_group
//! @{

#ifdef IRS_STM32F_2_AND_4
# include <armioregs.h>
# include <armregs_stm32f2xx.h>
#endif  // IRS_STM32F_2_AND_4

//! @}

#endif //IRSDEFSH
