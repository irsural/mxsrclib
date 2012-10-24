//! \file
//! \ingroup configuration_group
//! \brief �������, ������� ������ ���� �������� ������ ������
//!
//! ����: 04.10.2011\n
//! ������ ����: 16.09.2009

#ifndef IRSDEFSH
#define IRSDEFSH

#include <irsdefsbase.h>

#ifdef __ICCARM__
#include <armiomacros.h>
#endif // __ICCARM__

//! \addtogroup configuration_group
//! @{


#ifdef IRS_STM32F_2_AND_4
# include <armioregs.h>
# include <armregs_stm32f2xx.h>

# if defined(IRS_STM32F2xx)
#   include <stm32f2xx.h>
# elif defined(IRS_STM32F4xx)
#   include <stm32f4xx.h>
# endif // defined(IRS_STM32F4xx)

#endif  // IRS_STM32F_2_AND_4

//! @}

#endif //IRSDEFSH
