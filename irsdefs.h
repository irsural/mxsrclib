//! \file
//! \ingroup configuration_group
//! \brief Инклуды, которые должны быть включены раньше других
//!
//! Дата: 04.10.2011\n
//! Ранняя дата: 16.09.2009

#ifndef IRSDEFSH
#define IRSDEFSH

#include <irsdefsbase.h>

#ifdef __ICCARM__
#include <armiomacros.h>
#endif // __ICCARM__

//! \addtogroup configuration_group
//! @{


#ifdef IRS_STM32_F2_F4_F7
# include <armioregs.h>
# include <armregs_stm32f2xx.h>

# if defined(IRS_STM32F2xx)
#   include <stm32f2xx.h>
# elif defined(IRS_STM32F4xx)
#   include <stm32f4xx.h>
# elif defined(IRS_STM32F7xx)
#   include <stm32f7xx.h>
#endif // defined(IRS_STM32F7xx)

#endif  // IRS_STM32_F2_F4_F7

//! @}

#endif //IRSDEFSH
