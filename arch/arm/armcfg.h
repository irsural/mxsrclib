// Конфигурация процессора
// Дата: 22.11.2010
// Дата создания: 17.05.2010

#ifndef armcfgH
#define armcfgH

#include <irsdefs.h>

#include <irsarchint.h>
#include <irsarmutil.h>
#include <irsstrm.h>
#include <armioregs.h>

#include <intrinsics.h>

#include <irsfinal.h>

class hard_fault_event_t: public mxfact_event_t
{
public:
  hard_fault_event_t(arm_port_t &a_port, irs_u8 a_bit):
    //m_hard_fault_blink(a_port, a_bit, irs::make_cnt_ms(100)),
    m_pin_led(a_port, a_bit, irs::gpio_pin_t::dir_out),
    m_MMFAR(IRS_NULL),
    m_BFAR(IRS_NULL)
  {
    irs::arm::interrupt_array()->int_event_gen(irs::arm::hard_fault_int)->
      add(this);
    /*irs::arm::interrupt_array()->int_event_gen(irs::arm::mem_manage_int)->
      add(this);
    irs::arm::interrupt_array()->int_event_gen(irs::arm::bus_fault_int)->
      add(this);
    irs::arm::interrupt_array()->int_event_gen(irs::arm::usage_fault_int)->
      add(this);*/
  }
  //irs::blink_t m_hard_fault_blink;
  irs::arm::io_pin_t m_pin_led;
  irs_u32 m_MMFAR;
  irs_u32 m_BFAR;
  
  #pragma diag_suppress=Pe940
  #pragma optimize=no_inline
  static irs_u32* hard_fault_handler_asm()
  {
    /*if (__get_CONTROL() & 2) {
      asm ("MRS R0, PSP"); 
    } else {
      asm ("MRS R0, MSP");
    }*/
    asm (
      "TST LR, #4 \n"
      "ITE EQ \n"
      "MRSEQ R0, MSP \n"
      "MRSNE R0, PSP"
    );
  }
  #pragma diag_default=Pe940
  
  void hard_fault_handler_c(irs_u32* ap_stack)
  {
    struct stack_frame_t {
      irs_u32 R0;
      irs_u32 R1;
      irs_u32 R2;
      irs_u32 R3;
      irs_u32 R12;
      irs_u32 LR;
      irs_u32 PC;
      irs_u32 PSR;
    };
    stack_frame_t &stack = reinterpret_cast<stack_frame_t&>(*ap_stack);
  
    irs::mlog() << irsm("[Hard fault handler]") << endl;
    irs::mlog() << irsm("R0 = 0x") << hex << stack.R0 << endl;
    irs::mlog() << irsm("R1 = 0x") << stack.R1 << endl;
    irs::mlog() << irsm("R2 = 0x") << stack.R2 << endl;
    irs::mlog() << irsm("R3 = 0x") << stack.R3 << endl;
    irs::mlog() << irsm("R12 = 0x") << stack.R12 << endl;
    irs::mlog() << irsm("LR = 0x") << stack.LR << endl;
    irs::mlog() << irsm("PC = 0x") << stack.PC << endl;
    irs::mlog() << irsm("PSR = 0x") << stack.PSR << endl;
    irs::mlog() << irsm("MMFAR = 0x") << MMFAR << endl;
    irs::mlog() << irsm("BFAR = 0x") << BFAR << endl;
    irs::mlog() << irsm("CFSR = 0x") << CFSR << endl;
    irs::mlog() << irsm("HFSR = 0x") << HFSR << endl;
    irs::mlog() << irsm("DFSR = 0x") << DFSR << endl;
    irs::mlog() << irsm("AFSR = 0x") << AFSR << endl;
    #ifdef NOP
    irs::mlog() << irsm("BFAR = 0x") << 
      (*((volatile unsigned long *)(0xE000ED38))) << endl;
    irs::mlog() << irsm("CFSR = 0x") <<
      (*((volatile unsigned long *)(0xE000ED28))) << endl;
    irs::mlog() << irsm("HFSR = 0x") <<
      (*((volatile unsigned long *)(0xE000ED2C))) << endl;
    irs::mlog() << irsm("DFSR = 0x") <<
      (*((volatile unsigned long *)(0xE000ED30))) << endl;
    irs::mlog() << irsm("AFSR = 0x") <<
      (*((volatile unsigned long *)(0xE000ED3C))) << endl;
    #endif //NOP
    return;
  }

  virtual void exec()
  {
    m_pin_led.set();
    hard_fault_handler_c(hard_fault_handler_asm());
    mxfact_event_t::exec();
    while(true)
    {
      //m_hard_fault_blink();
    }
  }
};

void pll_on();

#endif  //  armcfgH
