//! \file
//! \ingroup system_utils_group
//! \brief Конфигурация процессора
//!
//! Дата: 30.04.2012\n
//! Дата создания: 17.05.2010

#include <irspch.h>

#include <armcfg.h>
#include <armioregs.h>
#include <irscpu.h>

#include <irsfinal.h>

void pll_on()
{
  #ifdef __LM3Sx9xx__
  LDOPCTL_bit.VADJ = 0x1B;  //  V = 2.75 V
  RCC_bit.BYPASS = 1;   //  Clocking from OSC source
  RCC_bit.USESYS = 0;   //  Disable System Clock Divider
  RCC_bit.MOSCDIS = 0;  //  Enable main osc
  RCC_bit.XTAL = 0xE;   //  Crystal 8 MHz
  RCC_bit.OSCSRC = 0x0; //  Main osc
  RCC_bit.PWRDN = 0;    //  Power on PLL
  RCC_bit.SYSDIV = 0x3; //  to 50 MHz
  RCC_bit.USESYS = 1;   //  Enable System Clock Divider
  while (!RIS_bit.PLLLRIS); //  Wait for PLL lock
  RCC_bit.BYPASS = 0;   //  Clocking from PLL source
  irs::cpu_traits_t::frequency(50000000);
  #elif __LM3SxBxx__
  RCC2_bit.USERCC2 = 1; // Use RCC2 bit field
  RCC2_bit.DIV400 = 1;
  RCC2_bit.SYSDIV2LSB = 0;
  RCC_bit.BYPASS = 1;   //  Clocking from OSC source
  RCC_bit.USESYS = 0;   //  Disable System Clock Divider
  RCC_bit.XTAL = 0xE;   //  Crystal 8 MHz
  RCC2_bit.OSCSRC2 = 0x0; //  Main osc
  RCC2_bit.PWRDN2 = 0;    //  Power on PLL
  RCC2_bit.SYSDIV2 = 0x2; //  to 80 MHz
  RCC_bit.USESYS = 1;   //  Enable System Clock Divider
  while (!RIS_bit.PLLLRIS); //  Wait for PLL lock
  RCC2_bit.BYPASS2 = 0;   //  Clocking from PLL source
  irs::cpu_traits_t::frequency(80000000);
  #endif // ARM_devices
}
