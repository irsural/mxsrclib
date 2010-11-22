// Конфигурация процессора
// Дата: 22.11.2010
// Дата создания: 17.05.2010

#ifndef armcfgH
#define armcfgH

#include <irsdefs.h>

#include <irsarchint.h>
#include <irsarmutil.h>
#include <irsfinal.h>

class hard_fault_event_t: public mxfact_event_t
{
public:
  hard_fault_event_t(arm_port_t &a_port, irs_u8 a_bit):
    m_hard_fault_blink(a_port, a_bit, irs::make_cnt_ms(100))
  {
    irs::arm::interrupt_array()->int_event_gen(irs::arm::hard_fault_int)->
      add(this);
  }
  irs::blink_t m_hard_fault_blink;
  virtual void exec()
  {
    mxfact_event_t::exec();
    while(true)
    {
      m_hard_fault_blink();
    }
  }
};

void pll_on();

#endif  //  armcfgH
