// Устройства
// Дата: 08.07.2010
// Ранняя дата: 08.07.2010

#include <irsdefs.h>

#ifdef __ICCARM__
#include <armioregs.h>
#include <irsdsp.h>
#include <irsdev.h>
#endif //__ICCARM__

#ifdef PWM_ZERO_PULSE
#include <irserror.h>
#endif  //  PWM_ZERO_PULSE

#include <irsfinal.h>


#ifdef __ICCARM__

irs::arm::arm_three_phase_pwm_t::arm_three_phase_pwm_t(freq_t a_freq,
  counter_t a_dead_time):

  m_max_freq(cpu_traits_t::frequency() / (pwm_clk_div * (1 + 1))),
  m_min_freq(cpu_traits_t::frequency() / (pwm_clk_div * (IRS_U16_MAX + 1))),
  m_freq(a_freq)
#ifdef PWM_ZERO_PULSE
  , m_int_event(this, interrupt)
#endif  //  PWM_ZERO_PULSE
{
  RCGC0_bit.PWM = 1;
  RCGC2_bit.PORTF = 1;  //  Выходы PWM 0, 1
  RCGC2_bit.PORTD = 1;  //  Выходы PWM 2, 3
  RCGC2_bit.PORTE = 1;  //  Выходы PWM 4, 5
  for (irs_u8 i = 10; i; i--);
  //  Выходы PWM 0, 1
  GPIOFDIR_bit.no0 = 1;
  GPIOFDIR_bit.no1 = 1;
  GPIOFDEN_bit.no0 = 1;
  GPIOFDEN_bit.no1 = 1;
  GPIOFAFSEL_bit.no0 = 1;
  GPIOFAFSEL_bit.no1 = 1;
  //  Выходы PWM 2, 3
  GPIODDIR_bit.no2 = 1;
  GPIODDIR_bit.no3 = 1;
  GPIODDEN_bit.no2 = 1;
  GPIODDEN_bit.no3 = 1;
  GPIODAFSEL_bit.no2 = 1;
  GPIODAFSEL_bit.no3 = 1;
  //  Выходы PWM 4, 5
  GPIOEDIR_bit.no0 = 1;
  GPIOEDIR_bit.no1 = 1;
  GPIOEDEN_bit.no0 = 1;
  GPIOEDEN_bit.no1 = 1;
  GPIOEAFSEL_bit.no0 = 1;
  GPIOEAFSEL_bit.no1 = 1;
  //  Тактовая частота модуля ШИМ
  //RCC_bit.USEPWMDIV = 1;
  RCC_bit.PWMDIV = 0; //  = Fcpu / 2
  //  Регистры LOAD и COMP применяются при переходе через 0
  PWMCTL_bit.GlobalSync0 = 1;
  PWMCTL_bit.GlobalSync1 = 1;
  PWMCTL_bit.GlobalSync2 = 1;
  //  Сброс счётчиков в 0
  PWMSYNC = pwm_all_sync;
  //  Отключение ножек
  PWMENABLE = pwm_all_disable;
  //  Не инвертировать выходной сигнал
  PWMINVERT = 0;
  //  Ножки в ноль при PWM Fault
  PWMFAULT_bit.Fault0 = 1;
  PWMFAULT_bit.Fault1 = 1;
  PWMFAULT_bit.Fault2 = 1;
  PWMFAULT_bit.Fault3 = 1;
  PWMFAULT_bit.Fault4 = 1;
  PWMFAULT_bit.Fault5 = 1;
  //  Прерываний нет
  PWMINTEN = 0;
  //  Настройка блоков ШИМ
  PWM0CTL_bit.CmpBUpd = 0;  //  обновление содержимого по PWMCTL
  PWM0CTL_bit.CmpAUpd = 1;
  PWM0CTL_bit.LoadUpd = 0;
  PWM0CTL_bit.Debug = 0;    //  Счётчик останавливается при отладке
  PWM0CTL_bit.Mode = 1;     //  Считать в обе стороны
  PWM0CTL_bit.Enable = 1;
  PWM1CTL_bit.CmpBUpd = 0;  //  обновление содержимого по PWMCTL
  PWM1CTL_bit.CmpAUpd = 1;
  PWM1CTL_bit.LoadUpd = 0;
  PWM1CTL_bit.Debug = 0;    //  Счётчик останавливается при отладке
  PWM1CTL_bit.Mode = 1;     //  Считать в обе стороны
  PWM1CTL_bit.Enable = 1;
  PWM2CTL_bit.CmpBUpd = 0;  //  обновление содержимого по PWMCTL
  PWM2CTL_bit.CmpAUpd = 1;
  PWM2CTL_bit.LoadUpd = 0;
  PWM2CTL_bit.Debug = 0;    //  Счётчик останавливается при отладке
  PWM2CTL_bit.Mode = 1;     //  Считать в обе стороны
  PWM2CTL_bit.Enable = 1;
  // Выключение прерываний
  PWM0INTEN = 0;
  PWM1INTEN = 0;
  PWM2INTEN = 0;
  //  Предел счёта
  irs_u16 load_value = calc_load_reg_value(m_freq);
  PWM0LOAD = load_value;
  PWM1LOAD = load_value;
  PWM2LOAD = load_value;
  //  Компаратор
  PWM0CMPA = load_value / 2;
  PWM0CMPB = 0;
  PWM1CMPA = load_value / 2;
  PWM1CMPB = 0;
  PWM2CMPA = load_value / 2;
  PWM2CMPB = 0;
  //  Поведение ноги
  enum
  {
    pwm_pin_do_nothing = 0x0,
    pwm_pin_invert = 0x1,
    pwm_pin_clear = 0x2,
    pwm_pin_set = 0x3
  };
  //  Верхний ключ фазы А
  PWM0GENA_bit.ActZero = pwm_pin_do_nothing;
  PWM0GENA_bit.ActLoad = pwm_pin_do_nothing;
  PWM0GENA_bit.ActCmpAU = pwm_pin_clear;
  PWM0GENA_bit.ActCmpAD = pwm_pin_set;
  PWM0GENA_bit.ActCmpBU = pwm_pin_do_nothing;
  PWM0GENA_bit.ActCmpBD = pwm_pin_do_nothing;
  //  Нижний ключ фазы А
  PWM0GENB_bit.ActZero = pwm_pin_do_nothing;
  PWM0GENB_bit.ActLoad = pwm_pin_do_nothing;
  PWM0GENB_bit.ActCmpAU = pwm_pin_set;
  PWM0GENB_bit.ActCmpAD = pwm_pin_clear;
  PWM0GENB_bit.ActCmpBU = pwm_pin_do_nothing;
  PWM0GENB_bit.ActCmpBD = pwm_pin_do_nothing;
  //  Верхний ключ фазы B
  PWM1GENA_bit.ActZero = pwm_pin_do_nothing;
  PWM1GENA_bit.ActLoad = pwm_pin_do_nothing;
  PWM1GENA_bit.ActCmpAU = pwm_pin_clear;
  PWM1GENA_bit.ActCmpAD = pwm_pin_set;
  PWM1GENA_bit.ActCmpBU = pwm_pin_do_nothing;
  PWM1GENA_bit.ActCmpBD = pwm_pin_do_nothing;
  //  Нижний ключ фазы B
  PWM1GENB_bit.ActZero = pwm_pin_do_nothing;
  PWM1GENB_bit.ActLoad = pwm_pin_do_nothing;
  PWM1GENB_bit.ActCmpAU = pwm_pin_set;
  PWM1GENB_bit.ActCmpAD = pwm_pin_clear;
  PWM1GENB_bit.ActCmpBU = pwm_pin_do_nothing;
  PWM1GENB_bit.ActCmpBD = pwm_pin_do_nothing;
  //  Верхний ключ фазы C
  PWM2GENA_bit.ActZero = pwm_pin_do_nothing;
  PWM2GENA_bit.ActLoad = pwm_pin_do_nothing;
  PWM2GENA_bit.ActCmpAU = pwm_pin_clear;
  PWM2GENA_bit.ActCmpAD = pwm_pin_set;
  PWM2GENA_bit.ActCmpBU = pwm_pin_do_nothing;
  PWM2GENA_bit.ActCmpBD = pwm_pin_do_nothing;
  //  Нижний ключ фазы C
  PWM2GENB_bit.ActZero = pwm_pin_do_nothing;
  PWM2GENB_bit.ActLoad = pwm_pin_do_nothing;
  PWM2GENB_bit.ActCmpAU = pwm_pin_set;
  PWM2GENB_bit.ActCmpAD = pwm_pin_clear;
  PWM2GENB_bit.ActCmpBU = pwm_pin_do_nothing;
  PWM2GENB_bit.ActCmpBD = pwm_pin_do_nothing;
  //  Dead-time включен
  PWM0DBCTL_bit.Enable = 1;
  PWM1DBCTL_bit.Enable = 1;
  PWM2DBCTL_bit.Enable = 1;
  const irs_u16 dead_time = a_dead_time / pwm_clk_div;
  //  Dead-time по переднему фронту верхнего ключа
  PWM0DBRISE_bit.RiseDelay = dead_time;
  PWM1DBRISE_bit.RiseDelay = dead_time;
  PWM2DBRISE_bit.RiseDelay = dead_time;
  //  Dead-time по заднему фронту верхнего ключа
  PWM0DBFALL_bit.FallDelay = dead_time;
  PWM1DBFALL_bit.FallDelay = dead_time;
  PWM2DBFALL_bit.FallDelay = dead_time;
  //
#ifdef PWM_ZERO_PULSE
  RCGC2_bit.PORTC = 1;
  for (char i = 10; i > 0; i--);
  GPIOCDEN_bit.no4 = 1;
  GPIOCDIR_bit.no4 = 1;
  GPIOCDATA_bit.no4 = 0;
  //
  PWMINTEN_bit.IntPWM0 = 1;
  PWM0INTEN_bit.IntCntLoad = 1;
  //
  SETENA0_bit.SETENA10 = 1;
  //
  interrupt_array()->int_event_gen(pwm0_int)
    ->add(&m_int_event);
  //  Отложить прерывание таймера
  //SETPEND0_bit.SETPEND19 = 1;
  SHPR2_bit.PRI_15 = 0x7F;
  IP4_bit.PRI_19 = 0xFF;
#endif  //  PWM_ZERO_PULSE
  /*PWMSYNC_bit.Sync0 = 1;
  for (irs_u16 i = 2*load_value / 3; i; i--);
  PWMSYNC_bit.Sync1 = 1;
  for (irs_u16 i = 2*load_value / 3; i; i--);
  PWMSYNC_bit.Sync2 = 1;*/
}

irs::arm::arm_three_phase_pwm_t::~arm_three_phase_pwm_t()
{
  PWMENABLE = pwm_all_disable;
  RCGC0_bit.PWM = 0;
}

void irs::arm::arm_three_phase_pwm_t::start()
{
  PWMSYNC = pwm_all_sync;
  PWMENABLE = pwm_all_enable;
}

void irs::arm::arm_three_phase_pwm_t::stop()
{
  PWMENABLE = pwm_all_disable;
}

void irs::arm::arm_three_phase_pwm_t::set_duty(irs_uarc a_duty)
{
  PWM0CMPA = a_duty;
  PWM1CMPA = a_duty;
  PWM2CMPA = a_duty;
  PWMCTL = pwm_all_sync;
}

void irs::arm::arm_three_phase_pwm_t::set_duty(irs_uarc a_duty,
  phase_t a_phase)
{
  switch (a_phase)
  {
    case PHASE_A: PWM0CMPA = a_duty; break;
    case PHASE_B: PWM1CMPA = a_duty; break;
    case PHASE_C: PWM2CMPA = a_duty; break;
  }
  PWMCTL = pwm_all_sync;
}

void irs::arm::arm_three_phase_pwm_t::set_duty(float a_duty)
{
  irs_uarc duty = irs_u16(a_duty * float(PWM0LOAD));
  set_duty(duty);
}

void irs::arm::arm_three_phase_pwm_t::set_duty(float a_duty,
  phase_t a_phase)
{
  irs_uarc duty = irs_u16(a_duty * float(PWM0LOAD));
  set_duty(duty, a_phase);
}

irs::cpu_traits_t::frequency_type
irs::arm::arm_three_phase_pwm_t::set_frequency(
  cpu_traits_t::frequency_type  a_frequency)
{
  m_freq = bound(a_frequency, m_min_freq, m_max_freq);
  irs_u16 load_value = calc_load_reg_value(m_freq);
  PWM0LOAD = load_value;
  PWM1LOAD = load_value;
  PWM2LOAD = load_value;
  PWMSYNC = pwm_all_sync;
  /*PWMSYNC_bit.Sync0 = 1;
  for (irs_u16 i = 2*load_value / 3; i; i--);
  PWMSYNC_bit.Sync1 = 1;
  for (irs_u16 i = 2*load_value / 3; i; i--);
  PWMSYNC_bit.Sync2 = 1;*/
  return m_freq;
}

irs_uarc irs::arm::arm_three_phase_pwm_t::get_max_duty()
{
  return PWM0LOAD;
}

irs::cpu_traits_t::frequency_type
irs::arm::arm_three_phase_pwm_t::get_max_frequency()
{
  return cpu_traits_t::frequency() / (pwm_clk_div * 2);
}

irs_u16 irs::arm::arm_three_phase_pwm_t::calc_load_reg_value(freq_t a_freq)
{
  freq_t freq = bound(a_freq, m_min_freq, m_max_freq);
  return cpu_traits_t::frequency() / (pwm_clk_div * freq);
}

#ifdef PWM_ZERO_PULSE
void irs::arm::arm_three_phase_pwm_t::interrupt()
{
  //GPIOBDATA_bit.no6 = 1;
  PWM0ISC_bit.IntCntLoad = 1;
  //GPIOBDATA_bit.no6 = 0;
}
#endif  //  PWM_ZERO_PULSE

#endif  //  __ICCARM__
