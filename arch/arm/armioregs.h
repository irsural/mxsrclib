#ifndef armioregsH
#define armioregsH

#if (((__TID__ >> 8) & 0x7F) != 0x4F)
#error This file should only be compiled by ARM IAR compiler and assembler
#endif

#include <armiomacros.h>

#if defined(__LM3SxBxx__) || \
    defined(__LM3Sx9xx__)
  #include <armregs_lm3s.h>
  #define IRS_LM3Sx
#elif defined(__STM32F100RBT__)
  #include <armregs_stm32.h>
#elif defined(IRS_STM32F2xx) || defined(IRS_STM32F4xx)
  #include <armregs_stm32f2xx.h>
#else
  #error Тип контроллера не определён
#endif  // Микроконтроллеры

#if defined(__LM3SxBxx__) || \
    defined(__LM3Sx9xx__) || \
    defined(__STM32F100RBT__) || \
    defined(IRS_STM32F2xx) || \
    defined(IRS_STM32F4xx)
  #define CORTEX_M3_CORE
  #include <armregs_cortex_m3.h>
#else
  #error Тип контроллера не определён
#endif  //  Микроконтроллеры

#endif  //  armioregsH
