#include <armcfg.h>
#include <armioregs.h>

void pll_on()
{
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
}
