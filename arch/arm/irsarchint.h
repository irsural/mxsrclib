// Прерывания ARM
// Дата: 17.05.2010
// Дата создания: 11.05.2010

#ifndef irsarchintH
#define irsarchintH

#include <irsdefs.h>

#include <irsint.h>

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
  //  Прерывания периферии
  gpio_porta_int,
  gpio_portb_int,
  gpio_portc_int,
  gpio_portd_int,
  gpio_porte_int,
  uart0_int,
  ssi0_int,
  pwm_fault_int,
  pwm0_int,
  pwm1_int,
  pwm2_int,
  qei0_int,
  adc_seq0_int,
  adc_seq1_int,
  adc_seq2_int,
  adc_seq3_int,
  watchdog_int,
  timer0a_int,
  timer0b_int,
  timer1a_int,
  timer1b_int,
  timer2a_int,
  timer2b_int,
  analog_comp0_int,
  sys_control_int,
  flash_control_int,
  gpio_portf_int,
  gpio_portg_int,
  gpio_porth_int,
  timer3a_int,
  timer3b_int,
  can0_int,
  ethernet_int,
  hibernation_int,
  //
  interrupt_count
};

//  Возвращает массив прерываний
interrupt_array_base_t* interrupt_array();

} //namespace arm

} //namespace irs

#endif //irsarchintH
