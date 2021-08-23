// Прерывания ARM
// Дата: 17.05.2010
// Дата создания: 12.05.2009

#include <irsdefs.h>

#include <irsarchint.h>
#include <irsconfig.h>
#include <irserror.h>
#include <irsstrm.h>

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

  #if IRS_USE_FREE_RTOS
  //void free_rtos_SVC_Handler( void );
  //void free_rtos_PendSV_Handler( void );
  void free_rtos_SysTick_Handler( void );
  #endif // IRS_USE_FREE_RTOS
}

//  Прерывания ядра
void NMI_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::nmi_int);
}

void fault_show(unsigned long a_LR, unsigned long a_MSP,
  unsigned long a_PSP)
{
  unsigned long SP = 0;

  const uint32_t stack_type_bit = 2;
  if ((a_LR&(1 << stack_type_bit)) == 0) {
    SP = a_MSP;
  } else {
    SP = a_PSP;
  }

  irs::mlog() << endl << endl;

  irs::mlog() << "Fault information" << endl << endl;
  
  const uint32_t FP_state_info_bit = 4;
  if ((a_LR&(1 << FP_state_info_bit)) == 0) {
    irs::mlog() << "FP state information is present but not displayed" << endl;
  } else {
    irs::mlog() << "FP state information is absent" << endl;
  }
  irs::mlog() << "(Extended frame; " <<
    "ARM v7-M Architecture Reference Manual)" << endl;

  // IAR выполняет PUSH {R7, LR}
  const unsigned long push_size = 8;

  irs_u32* stack = reinterpret_cast<irs_u32*>(SP + push_size);
  enum { r0, r1, r2, r3, r12, lr, pc, psr };

  irs::mlog() << "R0 = "; irs::out_hex_0x(&irs::mlog(), stack[r0]);
  irs::mlog() << endl;
  irs::mlog() << "R1 = "; irs::out_hex_0x(&irs::mlog(), stack[r1]);
  irs::mlog() << endl;
  irs::mlog() << "R2 = "; irs::out_hex_0x(&irs::mlog(), stack[r2]);
  irs::mlog() << endl;
  irs::mlog() << "R3 = "; irs::out_hex_0x(&irs::mlog(), stack[r3]);
  irs::mlog() << endl;
  irs::mlog() << "R12 = "; irs::out_hex_0x(&irs::mlog(), stack[r12]);
  irs::mlog() << endl;
  irs::mlog() << "LR = "; irs::out_hex_0x(&irs::mlog(), stack[lr]);
  irs::mlog() << endl;
  irs::mlog() << "PC = "; irs::out_hex_0x(&irs::mlog(), stack[pc]);
  irs::mlog() << endl;
  irs::mlog() << "PSR = "; irs::out_hex_0x(&irs::mlog(), stack[psr]);
  irs::mlog() << endl;

  irs::mlog() << irsm("BFAR = ");
  irs::out_hex_0x(&irs::mlog(), 
    (*((volatile unsigned long *)(0xE000ED38))));
  irs::mlog() << endl;
  irs::mlog() << irsm("MMFAR = ");
  irs::out_hex_0x(&irs::mlog(),
    (*((volatile unsigned long *)(0xE000ED34))));
  irs::mlog() << endl;
  irs::mlog() << irsm("CFSR = ");
  irs::out_hex_0x(&irs::mlog(),
    (*((volatile unsigned long *)(0xE000ED28))));
  irs::mlog() << endl;
  irs::mlog() << irsm("HFSR = ");
  irs::out_hex_0x(&irs::mlog(),
    (*((volatile unsigned long *)(0xE000ED2C))));
  irs::mlog() << endl;
  irs::mlog() << irsm("DFSR = ");
  irs::out_hex_0x(&irs::mlog(),
    (*((volatile unsigned long *)(0xE000ED30))));
  irs::mlog() << endl;
  irs::mlog() << irsm("AFSR = ");
  irs::out_hex_0x(&irs::mlog(),
    (*((volatile unsigned long *)(0xE000ED3C))));
  irs::mlog() << endl;

  irs::mlog() << endl << endl;
}

void HardFault_Handler()
{
  fault_show(__get_LR(), __get_MSP(), __get_PSP());

  irs::arm::interrupt_array()->exec_event(irs::arm::hard_fault_int);
}
void MemManage_Handler()
{
  fault_show(__get_LR(), __get_MSP(), __get_PSP());

  irs::arm::interrupt_array()->exec_event(irs::arm::mem_manage_int);
}
void BusFault_Handler()
{
  fault_show(__get_LR(), __get_MSP(), __get_PSP());

  irs::arm::interrupt_array()->exec_event(irs::arm::bus_fault_int);
}
void UsageFault_Handler()
{
  fault_show(__get_LR(), __get_MSP(), __get_PSP());

  irs::arm::interrupt_array()->exec_event(irs::arm::usage_fault_int);
}
#if !IRS_USE_FREE_RTOS
void SVC_Handler()
{
  //free_rtos_SVC_Handler();
  irs::arm::interrupt_array()->exec_event(irs::arm::svc_int);
}
#endif // !IRS_USE_FREE_RTOS
void DebugMon_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::debug_monitor_int);
}
#if !IRS_USE_FREE_RTOS
void PendSV_Handler()
{
  //free_rtos_PendSV_Handler();
  irs::arm::interrupt_array()->exec_event(irs::arm::pend_sv_int);
}
#endif // !IRS_USE_FREE_RTOS
void SysTick_Handler()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::sys_tick_int);
  #if IRS_USE_FREE_RTOS
  free_rtos_SysTick_Handler();
  #endif // IRS_USE_FREE_RTOS
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

#elif defined(IRS_STM32_F2_F4_F7)

//  Прерывания периферии

/*void irs_arm_tim1_brk_tim9_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim1_brk_tim9_int);
  TIM1_SR_bit.BIF = 0;
  TIM9_SR_bit.UIF = 0;
  TIM9_SR_bit.CC1IF = 0;
  TIM9_SR_bit.CC2IF = 0;
}*/

void irs_arm_exti0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::exti0_int);
  EXTI_PR_bit.PR6 = 1;
}

void irs_arm_exti3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::exti3_int);
  EXTI_PR_bit.PR9 = 1;
}

void irs_arm_dma1_stream5()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::dma1_stream5_int);
} 

void irs_arm_exti9_5_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::exti9_5_int);
  if (EXTI_PR_bit.PR5) {
    EXTI_PR_bit.PR5 = 1;
  } else if (EXTI_PR_bit.PR6) {
    EXTI_PR_bit.PR6 = 1;
  } else if (EXTI_PR_bit.PR7) {
    EXTI_PR_bit.PR7 = 1;
  } else if (EXTI_PR_bit.PR8) {
    EXTI_PR_bit.PR8 = 1;
  } else if (EXTI_PR_bit.PR9) {
    EXTI_PR_bit.PR9 = 1;
  }
  // Без этой инструкции прерывание иногда вызывается повторно
  __DSB();
}

void irs_arm_tim1_up_tim10_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim1_up_tim10_int);
  TIM1_SR_bit.UIF = 0;
  TIM1_SR_bit.CC3IF = 0;
  TIM10_SR_bit.UIF = 0;
  TIM10_SR_bit.CC1IF = 0;
}

void irs_arm_tim2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim2_int);
}

void irs_arm_tim3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim3_int);
}

void irs_arm_tim5_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim5_int);
}

void irs_arm_exti15_10_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::exti15_10_int);
  if (EXTI_PR_bit.PR10) {
    EXTI_PR_bit.PR10 = 1;
  } else if (EXTI_PR_bit.PR11) {
    EXTI_PR_bit.PR11 = 1;
  } else if (EXTI_PR_bit.PR12) {
    EXTI_PR_bit.PR12 = 1;
  } else if (EXTI_PR_bit.PR13) {
    EXTI_PR_bit.PR13 = 1;
  } else if (EXTI_PR_bit.PR14) {
    EXTI_PR_bit.PR14 = 1;
  } else if (EXTI_PR_bit.PR15) {
    EXTI_PR_bit.PR15 = 1;
  }
  //необходимо более подробное исследование
}

void irs_arm_tim8_brk_tim12_func()
{
  //irs::arm::interrupt_array()->exec_event(irs::arm::tim8_brk_tim12_int);
  //irs::mlog() << irsm("BIF = ") << mp_timer->TIM_SR_bit.BIF << endl;
  TIM8_SR_bit.BIF = 0;
  TIM12_SR_bit.UIF = 0;
  TIM12_SR_bit.CC1IF = 0;
  TIM12_SR_bit.CC2IF = 0;
}

void irs_arm_tim8_up_tim13_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim8_up_tim13_int);
  TIM8_SR_bit.UIF = 0;
  TIM13_SR_bit.UIF = 0;
  TIM13_SR_bit.CC1IF = 0;
}

void irs_arm_tim8_cc_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::tim8_cc_int);
  TIM8_SR_bit.CC1IF = 0;
  TIM8_SR_bit.CC2IF = 0;
  TIM8_SR_bit.CC3IF = 0;
  TIM8_SR_bit.CC4IF = 0;
}

void irs_arm_usart1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usart1_int);
}

void irs_arm_usart2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usart2_int);
}

void irs_arm_usart3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usart3_int);
}

void irs_arm_sdio_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::sdio_int);
}

void irs_arm_usart4_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usart4_int);
}

void irs_arm_usart5_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usart5_int);
}

void irs_arm_dma2_stream0_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::dma2_stream0_int);
}

void irs_arm_dma2_stream1_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::dma2_stream1_int);
}

void irs_arm_dma2_stream2_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::dma2_stream2_int);
}

void irs_arm_dma2_stream3_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::dma2_stream3_int);
}

void irs_arm_dma2_stream6_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::dma2_stream6_int);
}

//#ifndef USE_USB_OTG_FS
void irs_arm_otg_fs_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::otg_fs_int);
}
//#endif // USE_USB_OTG_FS
void irs_arm_usart6_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::usart6_int);
}

void irs_arm_otg_hs_func()
{
  irs::arm::interrupt_array()->exec_event(irs::arm::otg_hs_int);
}
#endif // defined(IRS_STM32_F2_F4_F7)

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

#elif defined(IRS_STM32_F2_F4_F7)

#pragma location = ".periph_intvec"
__root const intfunc __int_vector_table[] =
{
  irs_arm_default_int_func,  // 0
  irs_arm_default_int_func,  // 1
  irs_arm_default_int_func,  // 2
  irs_arm_default_int_func,  // 3
  irs_arm_default_int_func,  // 4
  irs_arm_default_int_func,  // 5
  irs_arm_exti0_func,        // 6
  irs_arm_default_int_func,  // 7
  irs_arm_default_int_func,  // 8
  irs_arm_exti3_func,        // 9
  irs_arm_default_int_func,  // 10
  irs_arm_default_int_func,  // 11
  irs_arm_default_int_func,  // 12
  irs_arm_default_int_func,  // 13
  irs_arm_default_int_func,  // 14
  irs_arm_default_int_func,  // 15
  irs_arm_dma1_stream5,      // 16
  irs_arm_default_int_func,  // 17
  irs_arm_default_int_func,  // 18
  irs_arm_default_int_func,  // 19
  irs_arm_default_int_func,  // 20
  irs_arm_default_int_func,  // 21
  irs_arm_default_int_func,  // 22
  irs_arm_exti9_5_func,      // 23
  irs_arm_default_int_func,  // 24
  irs_arm_tim1_up_tim10_func,// 25
  irs_arm_default_int_func,  // 26
  irs_arm_default_int_func,  // 27
  irs_arm_tim2_func,  // 28
  irs_arm_tim3_func,         // 29
  irs_arm_default_int_func,  // 30
  irs_arm_default_int_func,  // 31
  irs_arm_default_int_func,  // 32
  irs_arm_default_int_func,  // 33
  irs_arm_default_int_func,  // 34
  irs_arm_default_int_func,  // 35
  irs_arm_default_int_func,  // 36
  irs_arm_usart1_func,  // 37
  irs_arm_usart2_func,  // 38
  irs_arm_usart3_func,  // 39
  irs_arm_exti15_10_func,    // 40
  irs_arm_default_int_func,  // 41
  irs_arm_default_int_func,  // 42
  irs_arm_tim8_brk_tim12_func,// 43
  irs_arm_tim8_up_tim13_func,// 44
  irs_arm_default_int_func,  // 45
  irs_arm_tim8_cc_func,  // 46
  irs_arm_default_int_func,  // 47
  irs_arm_default_int_func,  // 48
  irs_arm_sdio_func,  // 49
  irs_arm_tim5_func,  // 50
  irs_arm_default_int_func,  // 51
  irs_arm_usart4_func,  // 52
  irs_arm_usart5_func,  // 53
  irs_arm_default_int_func,  // 54
  irs_arm_default_int_func,  // 55
  irs_arm_dma2_stream0_func,  // 56
  irs_arm_dma2_stream1_func,  // 57
  irs_arm_dma2_stream2_func,  // 58
  irs_arm_dma2_stream3_func, // 59
  irs_arm_default_int_func,  // 60
  irs_arm_default_int_func,  // 61
  irs_arm_default_int_func,  // 62
  irs_arm_default_int_func,  // 63
  irs_arm_default_int_func,  // 64
  irs_arm_default_int_func,  // 65
  irs_arm_default_int_func,  // 66
  irs_arm_otg_fs_func,       // 67
  irs_arm_default_int_func,  // 68
  irs_arm_dma2_stream6_func, // 69
  irs_arm_default_int_func,  // 70
  irs_arm_usart6_func,       // 71
  irs_arm_default_int_func,  // 72
  irs_arm_default_int_func,  // 73
  irs_arm_default_int_func,  // 74
  irs_arm_default_int_func,  // 75
  irs_arm_default_int_func,  // 76
  irs_arm_otg_hs_func,       // 77
  irs_arm_default_int_func,  // 78
  irs_arm_default_int_func,  // 79
  irs_arm_default_int_func   // 80
};
#endif // defined(IRS_STM32_F2_F4_F7)

#endif // !IRS_DISABLE_EVENT_INTERRUPT
