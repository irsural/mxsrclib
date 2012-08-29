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

#ifndef IRS_DISABLE_EVENT_INTERRUPT

#ifdef IRS_LM3Sx

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
#ifdef __LM3SxBxx__
void irs_arm_uart1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::uart1_int);
}
#endif // __LM3SxBxx__
void irs_arm_ssi0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::ssi0_int);
}
#ifdef __LM3SxBxx__
void irs_arm_i2c0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::i2c0_int);
}
#endif // __LM3SxBxx__
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
#ifdef __LM3Sx9xx__
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
#elif defined __LM3SxBxx__
void irs_arm_adc0_seq0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq0_int);
}
void irs_arm_adc0_seq1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq1_int);
}
void irs_arm_adc0_seq2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq2_int);
}
void irs_arm_adc0_seq3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq3_int);
}
#endif // __LM3SxBxx__
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
#ifdef __LM3SxBxx__
void irs_arm_analog_comp1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::analog_comp1_int);
}
void irs_arm_analog_comp2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::analog_comp2_int);
}
#endif // __LM3SxBxx__
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
#ifdef __LM3SxBxx__
void irs_arm_uart2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::uart2_int);
}
void irs_arm_ssi1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::ssi1_int);
}
#endif // __LM3SxBxx__
void irs_arm_timer3a_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer3a_int);
}
void irs_arm_timer3b_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::timer3b_int);
}
#ifdef __LM3SxBxx__
void irs_arm_i2c1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::i2c1_int);
}
void irs_arm_qei1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::qei1_int);
}
#endif // __LM3SxBxx__
void irs_arm_can0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::can0_int);
}
#ifdef __LM3SxBxx__
void irs_arm_can1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::can1_int);
}
#endif // __LM3SxBxx__
void irs_arm_ethernet_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::ethernet_int);
}
#ifdef __LM3Sx9xx__
void irs_arm_hibernation_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::hibernation_int);
}
#elif defined __LM3SxBxx__
void irs_arm_usb_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usb_int);
}
void irs_arm_pwm3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::pwm3_int);
}
void irs_arm_udma_software_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::udma_software_int);
}
void irs_arm_udma_error_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::udma_error_int);
}
void irs_arm_adc1_seq0_func()
{
  GPIOBDATA_bit.no5 = 1;
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq0_int);
  GPIOBDATA_bit.no5 = 0;
}
void irs_arm_adc1_seq1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq1_int);
}
void irs_arm_adc1_seq2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq2_int);
}
void irs_arm_adc1_seq3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::adc0_seq3_int);
}
void irs_arm_i2s0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::i2s0_int);
}
void irs_arm_epi_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::epi_int);
}
void irs_arm_gpio_portj_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::gpio_portj_int);
}
#endif // __LM3SxBxx__

#elif defined(IRS_STM32F2xx)

//  Прерывания периферии
void irs_arm_tim3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim3_int);
}
//  Прерывания периферии
void irs_arm_tim8_up_tim13_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim8_up_tim13_int);
  TIM8_SR_bit.UIF = 0;
  TIM13_SR_bit.UIF = 0;
  TIM13_SR_bit.CC1IF = 0;
}
#endif // defined(IRS_STM32F2xx)

void irs_arm_default_int_func()
{
  while (true);
}

typedef void( *intfunc )( void );

#ifdef IRS_LM3Sx

#pragma location = ".periph_intvec"
__root const intfunc __int_vector_table[] =
{
  irs_arm_gpio_porta_func,
  irs_arm_gpio_portb_func,
  irs_arm_gpio_portc_func,
  irs_arm_gpio_portd_func,
  irs_arm_gpio_porte_func,
  irs_arm_uart0_func,
  #ifdef __LM3Sx9xx__
  irs_arm_default_int_func,
  #elif defined __LM3SxBxx__
  irs_arm_uart1_func,
  #endif // __LM3SxBxx__
  irs_arm_ssi0_func,
  #ifdef __LM3Sx9xx__
  irs_arm_default_int_func,
  #elif defined __LM3SxBxx__
  irs_arm_i2c0_func,
  #endif // __LM3SxBxx__
  irs_arm_pwm_fault_func,
  irs_arm_pwm0_func,
  irs_arm_pwm1_func,
  irs_arm_pwm2_func,
  irs_arm_qei0_func,
  #ifdef __LM3Sx9xx__
  irs_arm_adc_seq0_func,
  irs_arm_adc_seq1_func,
  irs_arm_adc_seq2_func,
  irs_arm_adc_seq3_func,
  #elif defined __LM3SxBxx__
  irs_arm_adc0_seq0_func,
  irs_arm_adc0_seq1_func,
  irs_arm_adc0_seq2_func,
  irs_arm_adc0_seq3_func,
  #endif // __LM3SxBxx__
  irs_arm_watchdog_func,
  irs_arm_timer0a_func,
  irs_arm_timer0b_func,
  irs_arm_timer1a_func,
  irs_arm_timer1b_func,
  irs_arm_timer2a_func,
  irs_arm_timer2b_func,
  irs_arm_analog_comp0_func,
  #ifdef __LM3Sx9xx__
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  #elif defined __LM3SxBxx__
  irs_arm_analog_comp1_func,
  irs_arm_analog_comp2_func,
  #endif // __LM3SxBxx__
  irs_arm_sys_control_func,
  irs_arm_flash_control_func,
  irs_arm_gpio_portf_func,
  irs_arm_gpio_portg_func,
  irs_arm_gpio_porth_func,
  #ifdef __LM3Sx9xx__
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  #elif defined __LM3SxBxx__
  irs_arm_uart2_func,
  irs_arm_ssi1_func,
  #endif // __LM3SxBxx__
  irs_arm_timer3a_func,
  irs_arm_timer3b_func,
  #ifdef __LM3Sx9xx__
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  #elif defined __LM3SxBxx__
  irs_arm_i2c1_func,
  irs_arm_qei1_func,
  #endif // __LM3SxBxx__
  irs_arm_can0_func,
  #ifdef __LM3Sx9xx__
  irs_arm_default_int_func,
  irs_arm_default_int_func,
  #elif defined __LM3SxBxx__
  irs_arm_can1_func,
  irs_arm_default_int_func,
  #endif // __LM3SxBxx__
  irs_arm_ethernet_func,
  #ifdef __LM3Sx9xx__
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
  irs_arm_default_int_func,
  irs_arm_default_int_func
  #elif defined __LM3SxBxx__
  irs_arm_default_int_func,
  irs_arm_usb_func,
  irs_arm_pwm3_func,
  irs_arm_udma_software_func,
  irs_arm_udma_error_func,
  irs_arm_adc1_seq0_func,
  irs_arm_adc1_seq1_func,
  irs_arm_adc1_seq2_func,
  irs_arm_adc1_seq3_func,
  irs_arm_i2s0_func,
  irs_arm_epi_func,
  irs_arm_gpio_portj_func,
  irs_arm_default_int_func
  #endif // __LM3SxBxx__
};

#elif defined(IRS_STM32F2xx)

#pragma location = ".periph_intvec"
__root const intfunc __int_vector_table[] =
{
  irs_arm_default_int_func,  // 0
  irs_arm_default_int_func,  // 1
  irs_arm_default_int_func,  // 2
  irs_arm_default_int_func,  // 3
  irs_arm_default_int_func,  // 4
  irs_arm_default_int_func,  // 5
  irs_arm_default_int_func,  // 6
  irs_arm_default_int_func,  // 7
  irs_arm_default_int_func,  // 8
  irs_arm_default_int_func,  // 9
  irs_arm_default_int_func,  // 10
  irs_arm_default_int_func,  // 11
  irs_arm_default_int_func,  // 12
  irs_arm_default_int_func,  // 13
  irs_arm_default_int_func,  // 14
  irs_arm_default_int_func,  // 15
  irs_arm_default_int_func,  // 16
  irs_arm_default_int_func,  // 17
  irs_arm_default_int_func,  // 18
  irs_arm_default_int_func,  // 19
  irs_arm_default_int_func,  // 20
  irs_arm_default_int_func,  // 21
  irs_arm_default_int_func,  // 22
  irs_arm_default_int_func,  // 23
  irs_arm_default_int_func,  // 24
  irs_arm_default_int_func,  // 25
  irs_arm_default_int_func,  // 26
  irs_arm_default_int_func,  // 27
  irs_arm_default_int_func,  // 28
  irs_arm_tim3_func,         // 29
  irs_arm_default_int_func,  // 30
  irs_arm_default_int_func,  // 31
  irs_arm_default_int_func,  // 32
  irs_arm_default_int_func,  // 33
  irs_arm_default_int_func,  // 34
  irs_arm_default_int_func,  // 35
  irs_arm_default_int_func,  // 36
  irs_arm_default_int_func,  // 37
  irs_arm_default_int_func,  // 38
  irs_arm_default_int_func,  // 39
  irs_arm_default_int_func,  // 40
  irs_arm_default_int_func,  // 41
  irs_arm_default_int_func,  // 42
  irs_arm_default_int_func,  // 43
  irs_arm_tim8_up_tim13_func, // 44
  irs_arm_default_int_func,  // 45
  irs_arm_default_int_func,  // 46
  irs_arm_default_int_func,  // 47
  irs_arm_default_int_func,  // 48
  irs_arm_default_int_func,  // 49
  irs_arm_default_int_func,  // 50
  irs_arm_default_int_func,  // 51
  irs_arm_default_int_func,  // 52
  irs_arm_default_int_func,  // 53
  irs_arm_default_int_func,  // 54
  irs_arm_default_int_func,  // 55
  irs_arm_default_int_func,  // 56
  irs_arm_default_int_func,  // 57
  irs_arm_default_int_func,  // 58
  irs_arm_default_int_func,  // 59
  irs_arm_default_int_func,  // 60
  irs_arm_default_int_func,  // 61
  irs_arm_default_int_func,  // 62
  irs_arm_default_int_func,  // 63
  irs_arm_default_int_func,  // 64
  irs_arm_default_int_func,  // 65
  irs_arm_default_int_func,  // 66
  irs_arm_default_int_func,  // 67
  irs_arm_default_int_func,  // 68
  irs_arm_default_int_func,  // 69
  irs_arm_default_int_func,  // 70
  irs_arm_default_int_func,  // 71
  irs_arm_default_int_func,  // 72
  irs_arm_default_int_func,  // 73
  irs_arm_default_int_func,  // 74
  irs_arm_default_int_func,  // 75
  irs_arm_default_int_func,  // 76
  irs_arm_default_int_func,  // 77
  irs_arm_default_int_func,  // 78
  irs_arm_default_int_func,  // 79
  irs_arm_default_int_func   // 80
};
#endif // defined(IRS_STM32F2xx)

#endif // !IRS_DISABLE_EVENT_INTERRUPT
