#include <irsdev.h>

#ifdef __ICCARM__

#include <armioregs.h>
#include <irsdsp.h>

irs::arm::arm_three_phase_pwm_t::arm_three_phase_pwm_t(freq_t a_freq):
  m_max_freq(cpu_traits_t::frequency() / (pwm_clk_div * (1 + 1))),
  m_min_freq(cpu_traits_t::frequency() / (pwm_clk_div * (IRS_U16_MAX + 1))),
  m_freq(a_freq)
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
  PWMSYNC = 0x3;
  //  Отключение ножек
  PWMENABLE = 0;
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
  PWM0CTL_bit.CmpBUpd = 1;  //  обновление содержимого по PWMCTL
  PWM0CTL_bit.CmpAUpd = 1;
  PWM0CTL_bit.LoadUpd = 1;
  PWM0CTL_bit.Debug = 0;    //  Счётчик останавливается при отладке
  PWM0CTL_bit.Mode = 1;     //  Считать в обе стороны
  PWM0CTL_bit.Enable = 1;
  PWM1CTL_bit.CmpBUpd = 1;  //  обновление содержимого по PWMCTL
  PWM1CTL_bit.CmpAUpd = 1;
  PWM1CTL_bit.LoadUpd = 1;
  PWM1CTL_bit.Debug = 0;    //  Счётчик останавливается при отладке
  PWM1CTL_bit.Mode = 1;     //  Считать в обе стороны
  PWM1CTL_bit.Enable = 1;
  PWM2CTL_bit.CmpBUpd = 1;  //  обновление содержимого по PWMCTL
  PWM2CTL_bit.CmpAUpd = 1;
  PWM2CTL_bit.LoadUpd = 1;
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
  PWM0CMPA = 0;
  PWM0CMPB = 0;
  PWM1CMPA = 0;
  PWM1CMPB = 0;
  PWM2CMPA = 0;
  PWM2CMPB = 0;
  //  Поведение ноги
  enum
  {
    pwm_pin_do_nothing = 0x0,
    pwm_pin_invert = 0x1,
    pwm_pin_clear = 0x2,
    pwm_pin_set = 0x3
  };
  //  Верхний ключ
  PWM0GENA_bit.ActZero = pwm_pin_do_nothing;
  PWM0GENA_bit.ActLoad = pwm_pin_do_nothing;
  PWM0GENA_bit.ActCmpAU = pwm_pin_set;
  PWM0GENA_bit.ActCmpAD = pwm_pin_clear;
  PWM0GENA_bit.ActCmpBU = pwm_pin_do_nothing;
  PWM0GENA_bit.ActCmpBD = pwm_pin_do_nothing;
}

irs::arm::arm_three_phase_pwm_t::~arm_three_phase_pwm_t()
{
}

void irs::arm::arm_three_phase_pwm_t::start()
{
}

void irs::arm::arm_three_phase_pwm_t::stop()
{
}

void irs::arm::arm_three_phase_pwm_t::set_duty(irs_uarc /*a_duty*/)
{
}

void irs::arm::arm_three_phase_pwm_t::set_duty(irs_uarc /*a_duty*/,
  phase_t /*a_phase*/)
{
}

void irs::arm::arm_three_phase_pwm_t::set_duty(float /*a_duty*/)
{
}

void irs::arm::arm_three_phase_pwm_t::set_duty(float /*a_duty*/,
  phase_t /*a_phase*/)
{
}

irs::cpu_traits_t::frequency_type
irs::arm::arm_three_phase_pwm_t::set_frequency(
  cpu_traits_t::frequency_type  /*a_frequency*/)
{
  return 0;
}

irs_uarc irs::arm::arm_three_phase_pwm_t::get_max_duty()
{
  return 0;
}

irs::cpu_traits_t::frequency_type
irs::arm::arm_three_phase_pwm_t::get_max_frequency()
{
  return 0;
}

irs_u16 irs::arm::arm_three_phase_pwm_t::calc_load_reg_value(freq_t a_freq)
{
  freq_t freq = bound(a_freq, m_min_freq, m_max_freq);
  return cpu_traits_t::frequency() / (pwm_clk_div * freq);
}

#endif  //  __ICCARM__
