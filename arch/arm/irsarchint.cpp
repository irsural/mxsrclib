// Прерывания ARM
// Дата: 17.05.2010
// Дата создания: 12.05.2009

#include <irsdefs.h>

#include <irsarchint.h>
#include <irsconfig.h>

#include <irsfinal.h>

// Возвращает массив прерываний ARM
irs::interrupt_array_base_t* irs::arm::interrupt_array()
{
  enum
  {
    default_interrupt_count = 5
  };
  static auto_ptr<interrupt_array_t> 
    p_interrupt_array(new interrupt_array_t(interrupt_count, 
    default_interrupt_count));
  return p_interrupt_array.get();
}

extern "C"
{
  void NMI_Handler( void );
  void HardFault_Handler( void );
  void MemManage_Handler( void );
  void BusFault_Handler( void );
  void UsageFault_Handler( void );
  void SVC_Handler( void );
  void DebugMon_Handler( void );
  void PendSV_Handler( void );
  void SysTick_Handler( void );
}

//  Прерывания ядра
void NMI_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::nmi_int);
}

void HardFault_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::hard_fault_int);
}
void MemManage_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::mem_manage_int);
}
void BusFault_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::bus_fault_int);
}
void UsageFault_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usage_fault_int);
}
void SVC_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::svc_int);
}
void DebugMon_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::debug_monitor_int);
}
void PendSV_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::pend_sv_int);
}
void SysTick_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::sys_tick_int);
}
//  Прерывания периферии
void irs_arm_gpio_porta_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_porta_int);
}
void irs_arm_gpio_portb_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_portb_int);
}
void irs_arm_gpio_portc_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_portc_int);
}
void irs_arm_gpio_portd_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_portd_int);
}
void irs_arm_gpio_porte_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_porte_int);
}
void irs_arm_uart0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::uart0_int);
}
void irs_arm_ssi0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::ssi0_int);
}
void irs_arm_pwm_fault_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::pwm_fault_int);
}
void irs_arm_pwm0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::pwm0_int);
}
void irs_arm_pwm1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::pwm1_int);
}
void irs_arm_pwm2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::pwm2_int);
}
void irs_arm_qei0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::qei0_int);
}
void irs_arm_adc_seq0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc_seq0_int);
}
void irs_arm_adc_seq1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc_seq1_int);
}
void irs_arm_adc_seq2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc_seq2_int);
}
void irs_arm_adc_seq3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc_seq3_int);
}
void irs_arm_watchdog_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::watchdog_int);
}
void irs_arm_timer0a_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer0a_int);
}
void irs_arm_timer0b_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer0b_int);
}
void irs_arm_timer1a_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer1a_int);
}
void irs_arm_timer1b_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer1b_int);
}
void irs_arm_timer2a_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer2a_int);
}
void irs_arm_timer2b_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer2b_int);
}
void irs_arm_analog_comp0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::analog_comp0_int);
}
void irs_arm_sys_control_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::sys_control_int);
}
void irs_arm_flash_control_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::flash_control_int);
}
void irs_arm_gpio_portf_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_portf_int);
}
void irs_arm_gpio_portg_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_portg_int);
}
void irs_arm_gpio_porth_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_porth_int);
}
void irs_arm_timer3a_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer3a_int);
}
void irs_arm_timer3b_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer3b_int);
}
void irs_arm_can0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::can0_int);
}
void irs_arm_ethernet_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::ethernet_int);
}
void irs_arm_hibernation_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::hibernation_int);
}

void irs_arm_default_int_func()
{
  while (true);
}

typedef void( *intfunc )( void );

#pragma location = ".periph_intvec"
__root const intfunc __int_vector_table[] = 
{
  irs_arm_gpio_porta_func,
  irs_arm_gpio_portb_func,
  irs_arm_gpio_portc_func,
  irs_arm_gpio_portd_func,
  irs_arm_gpio_porte_func,
  irs_arm_uart0_func,
  irs_arm_default_int_func,
  irs_arm_ssi0_func,
  irs_arm_default_int_func,
  irs_arm_pwm_fault_func,
  irs_arm_pwm0_func,
  irs_arm_pwm1_func,
  irs_arm_pwm2_func,
  irs_arm_qei0_func,
  irs_arm_adc_seq0_func,
  irs_arm_adc_seq1_func,
  irs_arm_adc_seq2_func,
  irs_arm_adc_seq3_func,
  irs_arm_watchdog_func,
  irs_arm_timer0a_func,
  irs_arm_timer0b_func,
  irs_arm_timer1a_func,
  irs_arm_timer1b_func,
  irs_arm_timer2a_func,
  irs_arm_timer2b_func,
  irs_arm_analog_comp0_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_sys_control_func,
  irs_arm_flash_control_func,
  irs_arm_gpio_portf_func,
  irs_arm_gpio_portg_func,
  irs_arm_gpio_porth_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_timer3a_func,
  irs_arm_timer3b_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_can0_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_ethernet_func,
  irs_arm_hibernation_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  irs_arm_default_int_func
};
