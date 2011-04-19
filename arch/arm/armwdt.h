//! \file
//! \brief Watchdog Timer
//! Дата: 19.04.2011
//! Дата создания: 19.04.2011

#ifndef armwdth
#define armwdth

#include <irsdefs.h>

#include <armioregs.h>

#include <irsfinal.h>

namespace irs
{
namespace arm
{

class wdt_t
{
public:
  wdt_t(size_t a_period_s = 300):
    m_period_s(a_period_s)
  {
    RCGC0_bit.WDT0 = 1;
    for (irs_u8 i = 10; i; i--);
    
    WDT0LOCK = 0x1ACCE551;
    WDT0LOAD = static_cast<irs_u32>(m_period_s*irs::cpu_traits_t::frequency());
    WDT0CTL_bit.RESEN = 1;
    WDT0CTL_bit.INTEN = 1;
    WDT0TEST_bit.STALL = 1; // вероятно не работает с J-Link;
    WDT0LOCK = 0x0;
  }
  void restart()
  {
    WDT0LOCK = 0x1ACCE551;
    WDT0ICR = 0x11111111;
    WDT0LOCK = 0x0;
  }
private:
  size_t m_period_s;
}; // wdt_t

} // namespace arm
} // namespace irs

#endif // armwdth
