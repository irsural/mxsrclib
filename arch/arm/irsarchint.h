// Прерывания ARM
// Дата: 17.05.2010
// Дата создания: 11.05.2010

#ifndef irsarchintH
#define irsarchintH

#include <irsdefs.h>

#include <irsint.h>
#include <armioregs.h>

#include <irsfinal.h>

namespace irs {

namespace arm {



// Номера прерываний
enum {
  //  Прерывания ядра
  nmi_int,
  hard_fault_int,
  mem_manage_int,
  bus_fault_int,
  usage_fault_int,
  svc_int,
  debug_monitor_int,
  pend_sv_int,
  sys_tick_int,

  #ifndef IRS_DISABLE_EVENT_INTERRUPT

  //  Прерывания периферии
  #ifdef IRS_LM3Sx
  gpio_porta_int,
  gpio_portb_int,
  gpio_portc_int,
  gpio_portd_int,
  gpio_porte_int,
  uart0_int,
  #ifdef __LM3SxBxx__
  uart1_int,
  #endif // __LM3SxBxx__
  ssi0_int,
  #ifdef __LM3SxBxx__
  i2c0_int,
  #endif // __LM3SxBxx__
  pwm_fault_int,
  pwm0_int,
  pwm1_int,
  pwm2_int,
  qei0_int,
  #ifdef __LM3Sx9xx__
  adc_seq0_int,
  adc_seq1_int,
  adc_seq2_int,
  adc_seq3_int,
  #elif defined __LM3SxBxx__
  adc0_seq0_int,
  adc0_seq1_int,
  adc0_seq2_int,
  adc0_seq3_int,
  #endif // __LM3SxBxx__
  watchdog_int,
  timer0a_int,
  timer0b_int,
  timer1a_int,
  timer1b_int,
  timer2a_int,
  timer2b_int,
  analog_comp0_int,
  #ifdef __LM3SxBxx__
  analog_comp1_int,
  analog_comp2_int,
  #endif // __LM3SxBxx__
  sys_control_int,
  flash_control_int,
  gpio_portf_int,
  gpio_portg_int,
  gpio_porth_int,
  #ifdef __LM3SxBxx__
  uart2_int,
  ssi1_int,
  #endif // __LM3SxBxx__
  timer3a_int,
  timer3b_int,
  #ifdef __LM3SxBxx__
  i2c1_int,
  qei1_int,
  #endif // __LM3SxBxx__
  can0_int,
  #ifdef __LM3SxBxx__
  can1_int,
  #endif // __LM3SxBxx__
  ethernet_int,
  #ifdef __LM3Sx9xx__
  hibernation_int,
  #elif defined __LM3SxBxx__
  usb_int,
  pwm3_int,
  udma_software_int,
  udma_error_int,
  adc1_seq0_int,
  adc1_seq1_int,
  adc1_seq2_int,
  adc1_seq3_int,
  i2s0_int,
  epi_int,
  gpio_portj_int,
  #endif // __LM3SxBxx__

  #elif defined(IRS_STM32F2xx)
  //tim1_brk_tim9_int,
  exti3_int,
  tim1_up_tim10_int,
  tim3_int,
  tim8_brk_tim12_int,
  tim8_up_tim13_int,

  #endif //Микроконтроллеры

  #endif // !IRS_DISABLE_EVENT_INTERRUPT
  interrupt_count
};

//  Возвращает массив прерываний
interrupt_array_base_t* interrupt_array();



} //namespace arm

} //namespace irs

#endif //irsarchintH
