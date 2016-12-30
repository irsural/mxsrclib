//! \file
//! \ingroup drivers_group
//! \brief Устройства
//!
//! Дата: 08.07.2010
//! Ранняя дата: 08.07.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#ifdef __ICCARM__
#include <armioregs.h>
#include <armcfg.h>
#include <irsdsp.h>


#endif //__ICCARM__

#ifdef PWM_ZERO_PULSE
#include <irserror.h>
#endif  //  PWM_ZERO_PULSE

#include <irsdsp.h>

#include <irsdev.h>

#include <irsfinal.h>

irs::string_t irs::get_device_name(device_code_t a_device_code)
{
  switch (a_device_code) {
    case device_code_itn_2400: return irst("ИТН-2400");
    case device_code_gtch_03m: return irst("ГТЧ-03М");
    case device_code_u5023m: return irst("У5023М");
    case device_code_u309m: return irst("У309М");
    case device_code_hrm: return irst("ВУ");
    default: {
      return irst("Неизвестный прибор");
    }
  }
  #ifdef __BORLANDC__
  return irst("Неизвестный прибор");
  #endif // __BORLANDC__
}

// class pwm_gen_t

irs::cpu_traits_t::frequency_type irs::pwm_gen_t::get_max_frequency()
{
  return get_timer_frequency()/2;
}

irs::cpu_traits_t::frequency_type irs::pwm_gen_t::get_timer_frequency()
{
  // Эта функция создана для обратной совместимости
  IRS_LIB_ERROR(ec_standard, "Не реализована");
  return 0;
}

#ifdef __ICCARM__

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
//  Флаги занятости таймеров общего назначения
irs::arm::gptm_usage_t& gptm_usage() {
  static irs::arm::gptm_usage_t gptm_usage_obj;
  return gptm_usage_obj;
}

#elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
//  Флаги занятости таймеров общего назначения
irs::arm::timers_usage_t& timers_usage() {
  static irs::arm::timers_usage_t timers_usage_obj;
  return timers_usage_obj;
}
#else
  #error Тип контроллера не определён
#endif  //  mcu type

irs::arm::arm_three_phase_pwm_t::arm_three_phase_pwm_t(freq_t a_freq,
  counter_t a_dead_time):

  m_max_freq(cpu_traits_t::frequency() / (pwm_clk_div * (1 + 1))),
  m_min_freq(cpu_traits_t::frequency() / (pwm_clk_div * (IRS_U16_MAX + 1))),
  m_freq(a_freq)
#ifdef PWM_ZERO_PULSE
  ,
  m_int_event(this, &arm_three_phase_pwm_t::interrupt)
#endif  //  PWM_ZERO_PULSE
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
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
  #elif defined(__STM32F100RBT__)
  volatile counter_t dead_time = a_dead_time;
  #elif defined(IRS_STM32_F2_F4_F7)
  volatile counter_t dead_time = a_dead_time;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

irs::arm::arm_three_phase_pwm_t::~arm_three_phase_pwm_t()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  PWMENABLE = pwm_all_disable;
  RCGC0_bit.PWM = 0;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::arm_three_phase_pwm_t::start()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  PWMSYNC = pwm_all_sync;
  PWMENABLE = pwm_all_enable;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::arm_three_phase_pwm_t::stop()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  PWMENABLE = pwm_all_disable;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::arm_three_phase_pwm_t::set_duty(irs_uarc a_duty)
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  PWM0CMPA = a_duty;
  PWM1CMPA = a_duty;
  PWM2CMPA = a_duty;
  PWMCTL = pwm_all_sync;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  volatile irs_uarc duty = a_duty;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::arm_three_phase_pwm_t::set_duty(irs_uarc a_duty,
  phase_t a_phase)
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  switch (a_phase)
  {
    case PHASE_A: PWM0CMPA = a_duty; break;
    case PHASE_B: PWM1CMPA = a_duty; break;
    case PHASE_C: PWM2CMPA = a_duty; break;
  }
  PWMCTL = pwm_all_sync;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  volatile irs_uarc duty = a_duty;
  volatile phase_t phase = a_phase;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::arm_three_phase_pwm_t::set_duty(float a_duty)
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  irs_uarc duty = irs_u16(a_duty * float(PWM0LOAD));
  set_duty(duty);
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  volatile float duty = a_duty;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::arm_three_phase_pwm_t::set_duty(float a_duty,
  phase_t a_phase)
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  irs_uarc duty = irs_u16(a_duty * float(PWM0LOAD));
  set_duty(duty, a_phase);
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  volatile float duty = a_duty;
  volatile phase_t phase = a_phase;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

irs::cpu_traits_t::frequency_type
irs::arm::arm_three_phase_pwm_t::set_frequency(
  cpu_traits_t::frequency_type  a_frequency)
{
  m_freq = bound(a_frequency, m_min_freq, m_max_freq);
  irs_u16 load_value = calc_load_reg_value(m_freq);
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  PWM0LOAD = load_value;
  PWM1LOAD = load_value;
  PWM2LOAD = load_value;
  PWMSYNC = pwm_all_sync;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  volatile irs_u16 lv = load_value;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
  /*PWMSYNC_bit.Sync0 = 1;
  for (irs_u16 i = 2*load_value / 3; i; i--);
  PWMSYNC_bit.Sync1 = 1;
  for (irs_u16 i = 2*load_value / 3; i; i--);
  PWMSYNC_bit.Sync2 = 1;*/
  return m_freq;
}

irs_uarc irs::arm::arm_three_phase_pwm_t::get_max_duty()
{
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  return PWM0LOAD;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  return 0;
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
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
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  //GPIOBDATA_bit.no6 = 1;
  PWM0ISC_bit.IntCntLoad = 1;
  #elif defined(__STM32F100RBT__) || defined(IRS_STM32_F2_F4_F7)
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
  //GPIOBDATA_bit.no6 = 0;
}
#endif  //  PWM_ZERO_PULSE

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)

irs::arm::gptm_generator_t::gptm_generator_t(gpio_channel_t a_gpio_channel,
  cpu_traits_t::frequency_type a_frequency, float a_duty):
  m_valid_input_data(false),
  m_gptm_channel(GPTM_CCP0),
  m_enable_bit(0),
  mp_enable_reg(IRS_NULL),
  mp_load_reg(IRS_NULL),
  mp_match_reg(IRS_NULL),
  m_gpio_base(IRS_NULL)
{
  #ifdef __LM3SxBxx__
  irs_u8 gpio_pmc_value = 0;
  switch (a_gpio_channel) {
    case PA6: {
      if (gptm_usage().channel_free(GPTM_CCP1)) {
        m_gptm_channel = GPTM_CCP1;
        gpio_pmc_value = 2;
        m_valid_input_data = true;
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Указанный при инициализации GPTM пин занят");
      }
      break;
    }
    case PA7: {
      if (gptm_usage().channel_free(GPTM_CCP3)) {
        m_gptm_channel = GPTM_CCP3;
        gpio_pmc_value = 7;
        m_valid_input_data = true;
      } else if (gptm_usage().channel_free(GPTM_CCP4)) {
        m_gptm_channel = GPTM_CCP4;
        gpio_pmc_value = 2;
        m_valid_input_data = true;
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Указанный при инициализации GPTM пин занят");
      }
      break;
    }
    case PB0: {
      if (gptm_usage().channel_free(GPTM_CCP0)) {
        m_gptm_channel = GPTM_CCP0;
        gpio_pmc_value = 1;
        m_valid_input_data = true;
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Указанный при инициализации GPTM пин занят");
      }
      break;
    }
    case PE1: {
      if (gptm_usage().channel_free(GPTM_CCP2)) {
        m_gptm_channel = GPTM_CCP2;
        gpio_pmc_value = 4;
        m_valid_input_data = true;
      } else if (gptm_usage().channel_free(GPTM_CCP6)) {
        m_gptm_channel = GPTM_CCP6;
        gpio_pmc_value = 5;
        m_valid_input_data = true;
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Указанный при инициализации GPTM пин занят");
      }
      break;
    }
    case PF4: {
      if (gptm_usage().channel_free(GPTM_CCP0)) {
        m_gptm_channel = GPTM_CCP0;
        gpio_pmc_value = 1;
        m_valid_input_data = true;
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Указанный при инициализации GPTM пин занят");
      }
      break;
    }
    //  Остальные - дописать по мере надобности
    default:
    {
      IRS_LIB_ERROR(ec_standard, "Неверно указан порт при инициализации GPTM");
    }
  }
  #endif  //  __LM3SxBxx__
  if (m_valid_input_data) {
    init_gpio_port(a_gpio_channel, gpio_pmc_value);
    init_gptm_channel(m_gptm_channel, a_frequency, a_duty);
  } else {
    IRS_LIB_ERROR(ec_standard, "Ой! Не получилось инициализировать GPTM!");
  }
}

irs::arm::gptm_generator_t::~gptm_generator_t()
{
}

void irs::arm::gptm_generator_t::start()
{
  if (m_valid_input_data) {
    *mp_enable_reg |= (1 << m_enable_bit);
  } else {
    IRS_LIB_ERROR(ec_standard, "GPTM не инициализирован");
  }
}

void irs::arm::gptm_generator_t::stop()
{
  if (m_valid_input_data) {
    *mp_enable_reg &= ~(1 << m_enable_bit);
  } else {
    IRS_LIB_ERROR(ec_standard, "GPTM не инициализирован");
  }
}

void irs::arm::gptm_generator_t::set_duty(irs_uarc a_duty)
{
  if (m_valid_input_data) {
    bool need_restart = *mp_enable_reg & (1 << m_enable_bit);
    *mp_enable_reg &= ~(1 << m_enable_bit);
    if (*mp_load_reg - 1 > a_duty) a_duty = *mp_load_reg - 1;
    *mp_match_reg = a_duty;
    if (need_restart) *mp_enable_reg |= (1 << m_enable_bit);
  } else {
    IRS_LIB_ERROR(ec_standard, "GPTM не инициализирован");
  }
}

void irs::arm::gptm_generator_t::set_duty(float a_duty)
{
  if (m_valid_input_data) {
    bool need_restart = *mp_enable_reg & (1 << m_enable_bit);
    *mp_enable_reg &= ~(1 << m_enable_bit);
    irs_u16 match_maximum = *mp_load_reg - 1;
    //  *mp_load_reg != 0 никогда
    irs_u16 match_value
      = static_cast<irs_u16>(a_duty * static_cast<float>(match_maximum));
    if (match_value > match_maximum) match_value = match_maximum;
    *mp_match_reg = match_value;
    if (need_restart) *mp_enable_reg |= (1 << m_enable_bit);
  } else {
    IRS_LIB_ERROR(ec_standard, "GPTM не инициализирован");
  }
}

irs::cpu_traits_t::frequency_type irs::arm::gptm_generator_t::set_frequency(
  cpu_traits_t::frequency_type a_frequency)
{
  if (m_valid_input_data) {
    bool need_restart = *mp_enable_reg & (1 << m_enable_bit);
    *mp_enable_reg &= ~(1 << m_enable_bit);
    *mp_load_reg = calc_load_value(a_frequency);
    if (need_restart) *mp_enable_reg |= (1 << m_enable_bit);
    return cpu_traits_t::frequency() / (1 + *mp_load_reg);
  } else {
    IRS_LIB_ERROR(ec_standard, "GPTM не инициализирован");
    return 0;
  }
}

irs_uarc irs::arm::gptm_generator_t::get_max_duty()
{
  if (!m_valid_input_data) {
    IRS_LIB_ERROR(ec_standard, "GPTM не инициализирован");
  }
  return *mp_load_reg;
}

irs::cpu_traits_t::frequency_type
irs::arm::gptm_generator_t::get_max_frequency()
{
  if (!m_valid_input_data) {
    IRS_LIB_ERROR(ec_standard, "GPTM не инициализирован");
  }
  return cpu_traits_t::frequency() / 2;
}

void irs::arm::gptm_generator_t::init_gptm_channel(
  gptm_channel_t a_gptm_channel, cpu_traits_t::frequency_type a_frequency,
  float a_duty)
{
  gptm_usage().set_channel_use(a_gptm_channel);
  m_valid_input_data = true;

  enum {
    clock_register_shift = 16,
    base_shift = 12,
    enable_shift = 8,
    ams_shift = 3,
    mode_reg_shift = GPTM_TAMR,
    load_reg_shift = GPTM_TAILR,
    match_reg_shift = GPTM_TAMATCHR
  };
  irs_u8 gptm_num = a_gptm_channel >> 1;
  irs_u8 gptm_odd = a_gptm_channel & 1;
  arm_port_t gptm_base = GPTM0_BASE + (gptm_num << base_shift);

  m_enable_bit = enable_shift * gptm_odd;
  mp_enable_reg = reinterpret_cast<p_arm_port_t>(gptm_base + GPTM_CTL);
  mp_load_reg = reinterpret_cast<p_arm_port_t>(gptm_base + load_reg_shift
    + gptm_odd * sizeof(arm_port_t));
  mp_match_reg = reinterpret_cast<p_arm_port_t>(gptm_base + match_reg_shift
    + gptm_odd * sizeof(arm_port_t));

  RCGC1 |= (1 << (clock_register_shift + gptm_num));
  for (irs_u8 i = 10; i; i--);

  *mp_enable_reg &= ~(1 << m_enable_bit);

  *reinterpret_cast<p_arm_port_t>(gptm_base + GPTM_CFG) = GPTM_16_BIT;
  *reinterpret_cast<p_arm_port_t>(gptm_base + mode_reg_shift * (1 + gptm_odd))
    = GPTM_PERIODIC_MODE | (1 << ams_shift); //  в том числе очистка регистра
  *mp_load_reg = calc_load_value(a_frequency);
  *mp_match_reg
    = static_cast<irs_u16>(a_duty * static_cast<float>(*mp_load_reg - 1));
}

void irs::arm::gptm_generator_t::init_gpio_port(gpio_channel_t a_gpio_channel,
  irs_u8 gpio_pmc_value)
{
  enum {
    num_of_gpio_bits = 8,
    pmc_field_len = 4
  };
  irs_u8 gpio_port_number = a_gpio_channel / num_of_gpio_bits;
  irs_u8 gpio_bit = a_gpio_channel % num_of_gpio_bits;
  irs_u8 pmc_shift = gpio_bit * pmc_field_len;

  switch (gpio_port_number) {
    case 0: m_gpio_base = PORTA_BASE; break;
    case 1: m_gpio_base = PORTB_BASE; break;
    case 2: m_gpio_base = PORTC_BASE; break;
    case 3: m_gpio_base = PORTD_BASE; break;
    case 4: m_gpio_base = PORTE_BASE; break;
    case 5: m_gpio_base = PORTF_BASE; break;
    case 6: m_gpio_base = PORTG_BASE; break;
    case 7: m_gpio_base = PORTH_BASE; break;
    #ifdef __LM3SxBxx__
    case 8: m_gpio_base = PORTJ_BASE; break;
    #endif  //  __LM3SxBxx__
    default: IRS_LIB_ERROR(ec_standard,
      "Неверно указан порт при инициализации GPIO в инициализации GPTM");
  }
  irs_u32 den_reg = m_gpio_base + GPIO_DEN;
  irs_u32 dr8r_reg = m_gpio_base + GPIO_DR8R;
  irs_u32 afsel_reg = m_gpio_base + GPIO_AFSEL;
  irs_u32 pctl_reg = m_gpio_base + GPIO_PCTL;
  //  Если irs_u32 заменить на arm_port_t, появляется Warning[Pa082]:
  //  undefined behavior: the order of volatile accesses is undefined in
  //  this statement

  RCGC2 |= (1 << gpio_port_number);
  for (irs_u8 i = 10; i; i--);

  *reinterpret_cast<p_arm_port_t>(den_reg) |= (1 << gpio_bit);
  *reinterpret_cast<p_arm_port_t>(dr8r_reg) |= (1 << gpio_bit);
  *reinterpret_cast<p_arm_port_t>(afsel_reg) |= (1 << gpio_bit);
  *reinterpret_cast<p_arm_port_t>(pctl_reg) |= (gpio_pmc_value << pmc_shift);
}

irs_u16 irs::arm::gptm_generator_t::calc_load_value(
  cpu_traits_t::frequency_type a_frequency)
{
  irs_u16 load_value = cpu_traits_t::frequency() / a_frequency;
  if (load_value == 0) load_value = 1;
  return load_value;
}

#elif defined(__STM32F100RBT__)
#elif defined(IRS_STM32_F2_F4_F7)
// class st_pwm_gen_t
irs::arm::st_pwm_gen_t::st_pwm_gen_t(gpio_channel_t a_gpio_channel,
  size_t a_timer_address,
  cpu_traits_t::frequency_type a_frequency,
  float a_duty
):
  m_started(false),
  m_channels(),
  m_gpio_channel(a_gpio_channel),
  mp_timer(reinterpret_cast<tim_regs_t*>(a_timer_address)),
  m_frequency(a_frequency),
  m_timer_channel(0),
  m_complementary_gpio_channel(PNONE),
  m_break_gpio_channel(PNONE)
{
  clock_enable(a_timer_address);

  //mp_timer->TIM_CR1_bit.CMS = 2;
  // 0: Counter used as upcounter
  mp_timer->TIM_CR1_bit.DIR = 0;
  mp_timer->TIM_ARR = timer_frequency()/a_frequency - 1;
  stop();
  // 1: TIMx_ARR register is buffered
  mp_timer->TIM_CR1_bit.ARPE = 1;
  // 1: Re-initialize the counter and generates an update of the registers.
  mp_timer->TIM_EGR_bit.UG = 1;
  if ((a_timer_address == IRS_TIM1_PWM1_BASE) ||
    (a_timer_address == IRS_TIM8_PWM2_BASE)) {
    mp_timer->TIM_BDTR_bit.OSSI = 1;
  }
  mp_timer->TIM_CR1_bit.CEN = 1;

  if (a_gpio_channel != PNONE) {
    channel_enable(a_gpio_channel);
    set_duty(a_gpio_channel, a_duty);
  }
}

irs_u32 irs::arm::st_pwm_gen_t::
get_timer_channel_and_select_alternate_function(gpio_channel_t a_gpio_channel)
{
  irs_u32 timer_channel = 0;
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  switch (a_gpio_channel) {
     case PA0: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 1;
        GPIOA_AFRL_bit.AFRL0 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA5: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOA_AFRL_bit.AFRL5 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA7: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOA_AFRL_bit.AFRL7 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA8: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOA_AFRH_bit.AFRH8 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA10: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 3;
        GPIOA_AFRH_bit.AFRH10 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB0: {
      if (timer_address == IRS_TIM3_BASE) {
        timer_channel = 3;
        GPIOB_AFRL_bit.AFRL0 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB3: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 2;
        GPIOB_AFRL_bit.AFRL3 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB4: {
      if (timer_address == IRS_TIM3_BASE) {
        timer_channel = 1;
        GPIOB_AFRL_bit.AFRL4 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB6: {
      if (timer_address == IRS_TIM4_BASE) {
        timer_channel = 1;
        GPIOB_AFRL_bit.AFRL6 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB8: {
      if (timer_address == IRS_TIM4_BASE) {
        timer_channel = 3;
        GPIOB_AFRH_bit.AFRH8 = 2;
      } else if (timer_address == IRS_TIM10_BASE) {
        timer_channel = 1;
        GPIOB_AFRH_bit.AFRH8 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB9: {
      if (timer_address == IRS_TIM4_BASE) {
        timer_channel = 4;
        GPIOB_AFRH_bit.AFRH9 = 2;
      } else if (timer_address == IRS_TIM11_BASE) {
        timer_channel = 1;
        GPIOB_AFRH_bit.AFRH9 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB10: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 3;
        GPIOB_AFRH_bit.AFRH10 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB11: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 4;
        GPIOB_AFRH_bit.AFRH11 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PC6: {
      if (timer_address == IRS_TIM3_BASE) {
        timer_channel = 1;
        GPIOC_AFRL_bit.AFRL6 = 2;
      } else if (timer_address == IRS_TIM8_PWM2_BASE) {
        timer_channel = 1;
        GPIOC_AFRL_bit.AFRL6 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PC7: {
      if (timer_address == IRS_TIM3_BASE) {
        timer_channel = 2;
        GPIOC_AFRL_bit.AFRL7 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE5: {
      if (timer_address == IRS_TIM9_BASE) {
        timer_channel = 1;
        GPIOE_AFRL_bit.AFRL5 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE6: {
      if (timer_address == IRS_TIM9_BASE) {
        timer_channel = 2;
        GPIOE_AFRL_bit.AFRL6 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE8: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOE_AFRH_bit.AFRH8 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE9: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOE_AFRH_bit.AFRH9 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE10: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 2;
        GPIOE_AFRH_bit.AFRH10 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE11: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 2;
        GPIOE_AFRH_bit.AFRH11 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH6: {
      if (timer_address == IRS_TIM12_BASE) {
        timer_channel = 1;
        GPIOH_AFRL_bit.AFRL6 = 9;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH10: {
      if (timer_address == IRS_TIM5_BASE) {
        timer_channel = 1;
        GPIOH_AFRH_bit.AFRH10 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH11: {
      if (timer_address == IRS_TIM5_BASE) {
        timer_channel = 2;
        GPIOH_AFRH_bit.AFRH11 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH12: {
      if (timer_address == IRS_TIM5_BASE) {
        timer_channel = 3;
        GPIOH_AFRH_bit.AFRH12 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимая или неопределенная комбинация "
        "порта и таймера");
    }
  }
  return timer_channel;
}

void irs::arm::st_pwm_gen_t::select_alternate_function_for_break_channel()
{
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  if (m_break_gpio_channel != PNONE) {
    if (timer_address == IRS_TIM1_PWM1_BASE) {
      switch (m_break_gpio_channel) {
        case PA6: {
          GPIOA_AFRL_bit.AFRL6 = 1;
        } break;
        case PB12: {
          GPIOB_AFRH_bit.AFRH12 = 1;
        } break;
        case PE15: {
          GPIOE_AFRH_bit.AFRH15 = 1;
        } break;
        default: {
          IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
        }
      }
    } else if (timer_address == IRS_TIM8_PWM2_BASE) {
      switch (m_break_gpio_channel) {
        case PA6: {
          GPIOA_AFRL_bit.AFRL6 = 3;
        } break;
        default: {
          IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
        }
      }
    } else {
      IRS_LIB_ASSERT_MSG("Для выбранного таймера дополнительный канал не "
        "определен");
    }
  }
}

irs_u32* irs::arm::st_pwm_gen_t::get_tim_ccr_register(irs_u32 a_timer_channel)
{
  irs_u32* tim_ccr;
  switch (a_timer_channel) {
    case 1: {
      tim_ccr = &mp_timer->TIM_CCR1;
    } break;
    case 2: {
      tim_ccr = &mp_timer->TIM_CCR2;
    } break;
    case 3: {
      tim_ccr = &mp_timer->TIM_CCR3;
    } break;
    case 4: {
      tim_ccr = &mp_timer->TIM_CCR4;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимый канал");
    } break;
  }
  return tim_ccr;
}

irs::arm::st_pwm_gen_t::channel_t*
irs::arm::st_pwm_gen_t::find_channel(irs_u32 a_timer_channel)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    channel_t* channel = &m_channels[i];
    if (channel->timer_channel == a_timer_channel) {
      return channel;
    }
  }
  return NULL;
}

const irs::arm::st_pwm_gen_t::channel_t*
irs::arm::st_pwm_gen_t::get_channel(irs_u32 a_index) const
{
  return &m_channels[a_index];
}

irs::arm::st_pwm_gen_t::size_type irs::arm::st_pwm_gen_t::channel_count() const
{
  return m_channels.size();
}

void irs::arm::st_pwm_gen_t::
set_mode_capture_compare_registers_for_all_channels(
  output_compare_mode_t a_mode)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    set_mode_capture_compare_registers(a_mode, &m_channels[i]);
  }
}

void irs::arm::st_pwm_gen_t::set_mode_capture_compare_registers(
  output_compare_mode_t a_mode, channel_t* ap_channel)
{
  const irs_u32 OCxM_value = a_mode;
  // Output Compare preload enable
  const irs_u32 OCxPE_value = 1;
  // 0: OC1 active high
  const irs_u32 CCxP_value = ap_channel->main_channel_active_low ? 1 : 0;
  // 1: On - OCx signal is output on the corresponding output pin
  const irs_u32 CCxE_value = (ap_channel->main_channel == PNONE) ? 0 : 1;
  // 0: OC1N active high.
  const irs_u32 CCxNP_value =
    ap_channel->complementary_channel_active_low ? 1 : 0;
  // 0: Off - OC1N is not active
  irs_u32 CCxNE_value = (ap_channel->complementary_channel == PNONE) ? 0 : 1;

  switch (ap_channel->timer_channel) {
    case 1: {
      mp_timer->TIM_CCMR1_bit.OC1M = OCxM_value;
      mp_timer->TIM_CCMR1_bit.OC1PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC1P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC1E = CCxE_value;
      mp_timer->TIM_CCER_bit.CC1NP = CCxNP_value;
      mp_timer->TIM_CCER_bit.CC1NE = CCxNE_value;
    } break;
    case 2: {
      mp_timer->TIM_CCMR1_bit.OC2M = OCxM_value;
      mp_timer->TIM_CCMR1_bit.OC2PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC2P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC2E = CCxE_value;
      mp_timer->TIM_CCER_bit.CC2NP = CCxNP_value;
      mp_timer->TIM_CCER_bit.CC2NE = CCxNE_value;
    } break;
    case 3: {
      mp_timer->TIM_CCMR2_bit.OC3M = OCxM_value;
      mp_timer->TIM_CCMR2_bit.OC3PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC3P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC3E = CCxE_value;
      mp_timer->TIM_CCER_bit.CC3NP = CCxNP_value;
      mp_timer->TIM_CCER_bit.CC3NE = CCxNE_value;
    } break;
    case 4: {
      mp_timer->TIM_CCMR2_bit.OC4M = OCxM_value;
      mp_timer->TIM_CCMR2_bit.OC4PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC4P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC4E = CCxE_value;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимый канал");
    } break;
  }
}

irs::cpu_traits_t::frequency_type
irs::arm::st_pwm_gen_t::timer_frequency() const
{
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  return cpu_traits_t::timer_frequency(timer_address);
}

void irs::arm::st_pwm_gen_t::start()
{
  set_mode_capture_compare_registers_for_all_channels(ocm_pwm_mode_1);
  if ((reinterpret_cast<size_t>(mp_timer) == IRS_TIM1_PWM1_BASE) ||
    (reinterpret_cast<size_t>(mp_timer) == IRS_TIM8_PWM2_BASE)) {
    mp_timer->TIM_BDTR_bit.MOE = 1;
  }
  m_started = true;
}

void irs::arm::st_pwm_gen_t::stop()
{
  if ((reinterpret_cast<size_t>(mp_timer) == IRS_TIM1_PWM1_BASE) ||
    (reinterpret_cast<size_t>(mp_timer) == IRS_TIM8_PWM2_BASE)) {
    mp_timer->TIM_BDTR_bit.MOE = 0;

  }
  set_mode_capture_compare_registers_for_all_channels(ocm_force_inactive_level);
  m_started = false;
}

void irs::arm::st_pwm_gen_t::set_duty(irs_uarc a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    *m_channels[i].tim_ccr = static_cast<irs_u32>(a_duty);
  }
}

void irs::arm::st_pwm_gen_t::set_duty(float a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    *m_channels[i].tim_ccr = static_cast<irs_u32>(a_duty*get_max_duty());
  }
}

void irs::arm::st_pwm_gen_t::set_duty(gpio_channel_t a_gpio_channel,
  irs_uarc a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    channel_t* channel = &m_channels[i];
    if ((channel->main_channel == a_gpio_channel) ||
        (channel->complementary_channel == a_gpio_channel)) {
      set_duty_register(channel->tim_ccr, a_duty);
      break;
    }
  }
}

void irs::arm::st_pwm_gen_t::set_duty(gpio_channel_t a_gpio_channel,
  float a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    channel_t* channel = &m_channels[i];
    if ((channel->main_channel == a_gpio_channel) ||
        (channel->complementary_channel == a_gpio_channel)) {
      set_duty_register(channel->tim_ccr, a_duty);
      break;
    }
  }
}

void irs::arm::st_pwm_gen_t::set_duty_register(irs_u32* ap_tim_ccr,
  irs_uarc a_duty)
{
  *ap_tim_ccr = static_cast<irs_u32>(a_duty);
}

void irs::arm::st_pwm_gen_t::set_duty_register(irs_u32* ap_tim_ccr,
  float a_duty)
{
  *ap_tim_ccr = static_cast<irs_u32>(a_duty*get_max_duty());
}

irs::cpu_traits_t::frequency_type irs::arm::st_pwm_gen_t::set_frequency(
  cpu_traits_t::frequency_type a_frequency)
{
  m_frequency = a_frequency;
  mp_timer->TIM_ARR = timer_frequency()/a_frequency - 1;
  return get_frequency();
}

irs::cpu_traits_t::frequency_type irs::arm::st_pwm_gen_t::get_frequency() const
{
  return timer_frequency()/(mp_timer->TIM_ARR + 1);
}

irs_uarc irs::arm::st_pwm_gen_t::get_max_duty()
{
  return timer_frequency()/m_frequency - 1;
}

irs::cpu_traits_t::frequency_type irs::arm::st_pwm_gen_t::get_max_frequency()
{
  return get_timer_frequency()/2;
}

irs::cpu_traits_t::frequency_type irs::arm::st_pwm_gen_t::get_timer_frequency()
{
  return timer_frequency();
}

void irs::arm::st_pwm_gen_t::break_enable(gpio_channel_t a_gpio_channel,
  break_polarity_t a_polarity)
{
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  if ((timer_address != IRS_TIM1_PWM1_BASE) &&
    (timer_address != IRS_TIM8_PWM2_BASE)) {
    IRS_LIB_ERROR(ec_standard, "Недопустимый таймер");
  }
  m_break_gpio_channel = a_gpio_channel;
  select_alternate_function_for_break_channel();
  gpio_moder_alternate_function_enable(a_gpio_channel);
  clock_enable(a_gpio_channel);
  if (a_polarity == break_polarity_active_low) {
    mp_timer->TIM_BDTR_bit.BKP = 0;
  } else {
    mp_timer->TIM_BDTR_bit.BKP = 1;
  }
  mp_timer->TIM_BDTR_bit.BKE = 1;
  mp_timer->TIM_SR_bit.BIF = 0;
}

void irs::arm::st_pwm_gen_t::channel_enable(gpio_channel_t a_gpio_channel,
  output_polarity_t a_output_polarity)
{
  channel_enable_helper(a_gpio_channel, false, a_output_polarity);
  /*
  clock_enable(a_gpio_channel);
  gpio_moder_alternate_function_enable(a_gpio_channel);

  const irs_u32 timer_channel = get_timer_channel_and_select_alternate_function(
    a_gpio_channel);

  channel_t* existing_channel = find_channel(timer_channel);
  channel_t channel;
  if (existing_channel) {
    channel = *existing_channel;
  }
  //channel.duty = 0;
  channel.main_channel = a_gpio_channel;
  channel.timer_channel = timer_channel;
  channel.tim_ccr = get_tim_ccr_register(channel.timer_channel);

  set_duty_register(channel.tim_ccr, a_duty);

  m_channels.push_back(channel);
  if (m_started) {
    set_mode_capture_compare_registers(ocm_pwm_mode_1, &m_channels.back());
  } else {
    set_mode_capture_compare_registers(ocm_force_inactive_level,
      &m_channels.back());
  }*/
}

void irs::arm::st_pwm_gen_t::complementary_channel_enable(
  gpio_channel_t a_gpio_channel, output_polarity_t a_output_polarity)
{
  channel_enable_helper(a_gpio_channel, true, a_output_polarity);
  /*clock_enable(a_gpio_channel);
  gpio_moder_alternate_function_enable(a_gpio_channel);
  //m_complementary_gpio_channel = a_gpio_channel;
  //select_alternate_function_for_complementary_channel();

  const irs_u32 timer_channel = get_timer_channel_and_select_alternate_function(
    a_gpio_channel);

  channel_t* existing_channel = find_channel(timer_channel);
  channel_t channel;
  if (existing_channel) {
    channel = *existing_channel;
  }

  channel.complementary_channel = a_gpio_channel;
  channel.timer_channel = timer_channel;
  channel.tim_ccr = get_tim_ccr_register(channel.timer_channel);

  set_duty_register(channel.tim_ccr, 0.f);

  m_channels.push_back(channel);

  if (m_started) {
    set_mode_capture_compare_registers(ocm_pwm_mode_1, &m_channels.back());
  } else {
    set_mode_capture_compare_registers(ocm_force_inactive_level,
      &m_channels.back());
  }*/
}

void irs::arm::st_pwm_gen_t::channel_enable_helper(
  gpio_channel_t a_gpio_channel, bool a_complementary,
  output_polarity_t a_output_polarity)
{
  clock_enable(a_gpio_channel);
  gpio_moder_alternate_function_enable(a_gpio_channel);

  const irs_u32 timer_channel = get_timer_channel_and_select_alternate_function(
    a_gpio_channel);

  channel_t* existing_channel = find_channel(timer_channel);
  channel_t channel;
  if (existing_channel) {
    channel = *existing_channel;
  }
  if (a_complementary) {
    channel.complementary_channel = a_gpio_channel;
    channel.complementary_channel_active_low =
      (a_output_polarity == op_active_low);
  } else {
    channel.main_channel = a_gpio_channel;
    channel.main_channel_active_low = (a_output_polarity == op_active_low);
  }
  channel.timer_channel = timer_channel;
  channel.tim_ccr = get_tim_ccr_register(channel.timer_channel);

  if (!existing_channel) {
    set_duty_register(channel.tim_ccr, 0.f);
  }
  set_mode_capture_compare_registers(ocm_pwm_mode_1, &channel);

  if (existing_channel) {
    *existing_channel = channel;
  } else {
    m_channels.push_back(channel);
  }
}

void irs::arm::st_pwm_gen_t::set_dead_time(float a_time)
{
  const double dead_time = a_time;

  const double t_dtg = 1./timer_frequency();

  const double step_1 = t_dtg;
  const irs_u8 DTG_6_0_max = 127;
  const double max_1 = DTG_6_0_max*step_1;


  const double step_2 = t_dtg*2;
  const irs_u8 DTG_5_0_max = 63;
  const irs_u8 step_count_min_2 = 64;
  const double max_2 = (step_count_min_2 + DTG_5_0_max)*step_2;


  const irs_u8 DTG_4_0_max = 31;
  const double step_3 = t_dtg*8;
  const irs_u8 step_count_min_3 = 32;
  const double max_3 = (step_count_min_3 + DTG_4_0_max)*step_3;


  const double step_4 = t_dtg*16;
  const irs_u8 step_count_min_4 = 32;


  if (dead_time <= max_1) {
    const irs_u8 value = irs::round<double, irs_u8>(dead_time/step_1);
    mp_timer->TIM_BDTR_bit.DTG = min(value, DTG_6_0_max);
  } else if (dead_time <= max_2) {
    const irs_u8 value =
      round<double, irs_u8>(dead_time/step_2-step_count_min_2);
    mp_timer->TIM_BDTR_bit.DTG = (4 << 5) | min(value, DTG_5_0_max);
  } else if (dead_time <= max_3) {
    const irs_u8 value =
      round<double, irs_u8>(dead_time/step_3-step_count_min_3);
    mp_timer->TIM_BDTR_bit.DTG = (6 << 5) | min(value, DTG_4_0_max);
  } else {
    const irs_u8 value =
      round<double, irs_u8>(dead_time/step_4-step_count_min_4);
    mp_timer->TIM_BDTR_bit.DTG = (7 << 5) | min(value, DTG_4_0_max);
  }
}

irs::cpu_traits_t::frequency_type
irs::arm::st_pwm_gen_t::get_auto_reload_value()
{
  return mp_timer->TIM_ARR;
}

irs::cpu_traits_t::frequency_type
irs::arm::st_pwm_gen_t::set_auto_reload_value(
  cpu_traits_t::frequency_type a_arr)
{
  mp_timer->TIM_ARR = a_arr;
  m_frequency = get_frequency();
  return mp_timer->TIM_ARR;
}

#ifdef USE_HAL_DRIVER
#if !ST_HAL_PWM_GEN_DMA_NEW
// class st_hal_pwm_gen_dma_t
irs::arm::st_hal_pwm_gen_dma_t::st_hal_pwm_gen_dma_t(
  const settings_t& a_settings
):
  m_settings(a_settings),
  m_started(false),
  m_channels(),
  m_gpio_channel(a_settings.gpio_channel),
  mp_timer(reinterpret_cast<tim_regs_t*>(a_settings.timer_address)),
  m_frequency(a_settings.frequency),
  m_timer_channel(0),
  m_complementary_gpio_channel(PNONE),
  m_break_gpio_channel(PNONE),

  m_hdma_tx(),
  //m_tx_status(false),
  m_hdma_init(false),

  m_tx_buffer(a_settings.tx_buffer),
  m_dma_event(&m_hdma_tx)
{
  //IRS_LIB_ASSERT("Не доделан!!!");

  IRS_LIB_ASSERT(a_settings.dma_address);
  IRS_LIB_ASSERT(a_settings.frequency > 0);

  IRS_LIB_ASSERT(a_settings.tx_buffer.data());
  IRS_LIB_ASSERT(a_settings.tx_buffer.size() > 0);
  IRS_LIB_ASSERT(IS_DMA_STREAM_ALL_INSTANCE(a_settings.tx_dma_y_stream_x));
  IRS_LIB_ASSERT(IS_DMA_CHANNEL(a_settings.tx_dma_channel));
  IRS_LIB_ASSERT(IS_DMA_PRIORITY(a_settings.tx_dma_priority));

  IRS_LIB_ASSERT((a_settings.data_item_byte_count == 1) ||
    (a_settings.data_item_byte_count == 2) ||
    (a_settings.data_item_byte_count == 4));

  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // Ничего не делаем
  #elif IRS_STM32F7xx
  IRS_LIB_ASSERT(reinterpret_cast<size_t>(a_settings.tx_buffer.data())%32 == 0);
  //IRS_LIB_ASSERT(a_settings.tx_buffer.size()%32 == 0);
  #else
  #error Тип контроллера не определён
  #endif // IRS_STM32F7xx

  clock_enable(a_settings.timer_address);
  // 0: Counter used as upcounter
  //mp_timer->TIM_CR1_bit.CMS = 2; // Center-aligned mode 2.
  mp_timer->TIM_CR1_bit.DIR = 0;
  mp_timer->TIM_ARR = timer_frequency()/m_frequency - 1;
  stop();
  // 1: TIMx_ARR register is buffered
  mp_timer->TIM_CR1_bit.ARPE = 1;
  // 1: Re-initialize the counter and generates an update of the registers.
  mp_timer->TIM_EGR_bit.UG = 1;
  if ((a_settings.timer_address == IRS_TIM1_PWM1_BASE) ||
    (a_settings.timer_address == IRS_TIM8_PWM2_BASE)) {
    mp_timer->TIM_BDTR_bit.OSSI = 1;
  }
  mp_timer->TIM_CR1_bit.CEN = 1;

  if (m_gpio_channel != PNONE) {
    channel_enable(m_gpio_channel);
    set_duty(m_gpio_channel, a_settings.duty);
  }

  irs::clock_enable(a_settings.dma_address);

  irs::interrupt_array()->int_gen_events(irs::arm::dma2_stream1_int)->
    push_back(&m_dma_event);
  /* Enable the DMA STREAM global Interrupt */
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  NVIC_SetPriority (DMA2_Stream1_IRQn, 0);

  reset_dma();
}

irs_u32 irs::arm::st_hal_pwm_gen_dma_t::
get_timer_channel_and_select_alternate_function(gpio_channel_t a_gpio_channel)
{
  irs_u32 timer_channel = 0;
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  switch (a_gpio_channel) {
     case PA0: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 1;
        GPIOA_AFRL_bit.AFRL0 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA5: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOA_AFRL_bit.AFRL5 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA7: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOA_AFRL_bit.AFRL7 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA8: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOA_AFRH_bit.AFRH8 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PA10: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 3;
        GPIOA_AFRH_bit.AFRH10 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB0: {
      if (timer_address == IRS_TIM3_BASE) {
        timer_channel = 3;
        GPIOB_AFRL_bit.AFRL0 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB3: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 2;
        GPIOB_AFRL_bit.AFRL3 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB6: {
      if (timer_address == IRS_TIM4_BASE) {
        timer_channel = 1;
        GPIOB_AFRL_bit.AFRL6 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB8: {
      if (timer_address == IRS_TIM4_BASE) {
        timer_channel = 3;
        GPIOB_AFRH_bit.AFRH8 = 2;
      } else if (timer_address == IRS_TIM10_BASE) {
        timer_channel = 1;
        GPIOB_AFRH_bit.AFRH8 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB9: {
      if (timer_address == IRS_TIM4_BASE) {
        timer_channel = 4;
        GPIOB_AFRH_bit.AFRH9 = 2;
      } else if (timer_address == IRS_TIM11_BASE) {
        timer_channel = 1;
        GPIOB_AFRH_bit.AFRH9 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB10: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 3;
        GPIOB_AFRH_bit.AFRH10 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PB11: {
      if (timer_address == IRS_TIM2_BASE) {
        timer_channel = 4;
        GPIOB_AFRH_bit.AFRH11 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PC6: {
      if (timer_address == IRS_TIM3_BASE) {
        timer_channel = 1;
        GPIOC_AFRL_bit.AFRL6 = 2;
      } else if (timer_address == IRS_TIM8_PWM2_BASE) {
        timer_channel = 1;
        GPIOC_AFRL_bit.AFRL6 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PC7: {
      if (timer_address == IRS_TIM3_BASE) {
        timer_channel = 2;
        GPIOC_AFRL_bit.AFRL7 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE5: {
      if (timer_address == IRS_TIM9_BASE) {
        timer_channel = 1;
        GPIOE_AFRL_bit.AFRL5 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE6: {
      if (timer_address == IRS_TIM9_BASE) {
        timer_channel = 2;
        GPIOE_AFRL_bit.AFRL6 = 3;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE8: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOE_AFRH_bit.AFRH8 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE9: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 1;
        GPIOE_AFRH_bit.AFRH9 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE10: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 2;
        GPIOE_AFRH_bit.AFRH10 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PE11: {
      if (timer_address == IRS_TIM1_PWM1_BASE) {
        timer_channel = 2;
        GPIOE_AFRH_bit.AFRH11 = 1;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH6: {
      if (timer_address == IRS_TIM12_BASE) {
        timer_channel = 1;
        GPIOH_AFRL_bit.AFRL6 = 9;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH10: {
      if (timer_address == IRS_TIM5_BASE) {
        timer_channel = 1;
        GPIOH_AFRH_bit.AFRH10 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH11: {
      if (timer_address == IRS_TIM5_BASE) {
        timer_channel = 2;
        GPIOH_AFRH_bit.AFRH11 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    case PH12: {
      if (timer_address == IRS_TIM5_BASE) {
        timer_channel = 3;
        GPIOH_AFRH_bit.AFRH12 = 2;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
      }
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимая или неопределенная комбинация "
        "порта и таймера");
    }
  }
  return timer_channel;
}

void irs::arm::st_hal_pwm_gen_dma_t::
select_alternate_function_for_break_channel()
{
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  if (m_break_gpio_channel != PNONE) {
    if (timer_address == IRS_TIM1_PWM1_BASE) {
      switch (m_break_gpio_channel) {
        case PA6: {
          GPIOA_AFRL_bit.AFRL6 = 1;
        } break;
        case PE15: {
          GPIOE_AFRH_bit.AFRH15 = 1;
        } break;
        default: {
          IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
        }
      }
    } else if (timer_address == IRS_TIM8_PWM2_BASE) {
      switch (m_break_gpio_channel) {
        case PA6: {
          GPIOA_AFRL_bit.AFRL6 = 3;
        } break;
        default: {
          IRS_LIB_ASSERT_MSG("Недопустимая комбинация порта и таймера");
        }
      }
    } else {
      IRS_LIB_ASSERT_MSG("Для выбранного таймера дополнительный канал не "
        "определен");
    }
  }
}

irs_u32* irs::arm::st_hal_pwm_gen_dma_t::get_tim_ccr_register(irs_u32 a_timer_channel)
{
  irs_u32* tim_ccr;
  switch (a_timer_channel) {
    case 1: {
      tim_ccr = &mp_timer->TIM_CCR1;
    } break;
    case 2: {
      tim_ccr = &mp_timer->TIM_CCR2;
    } break;
    case 3: {
      tim_ccr = &mp_timer->TIM_CCR3;
    } break;
    case 4: {
      tim_ccr = &mp_timer->TIM_CCR4;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимый канал");
    } break;
  }
  return tim_ccr;
}

irs::arm::st_hal_pwm_gen_dma_t::channel_t*
irs::arm::st_hal_pwm_gen_dma_t::find_channel(irs_u32 a_timer_channel)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    channel_t* channel = &m_channels[i];
    if (channel->timer_channel == a_timer_channel) {
      return channel;
    }
  }
  return NULL;
}

void irs::arm::st_hal_pwm_gen_dma_t::
set_mode_capture_compare_registers_for_all_channels(
  output_compare_mode_t a_mode)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    set_mode_capture_compare_registers(a_mode, &m_channels[i]);
  }
}

void irs::arm::st_hal_pwm_gen_dma_t::set_mode_capture_compare_registers(
  output_compare_mode_t a_mode, channel_t* ap_channel)
{
  const irs_u32 OCxM_value = a_mode;
  // Output Compare preload enable
  const irs_u32 OCxPE_value = 1;
  // 0: OC1 active high
  const irs_u32 CCxP_value = ap_channel->main_channel_active_low ? 1 : 0;
  // 1: On - OCx signal is output on the corresponding output pin
  const irs_u32 CCxE_value = (ap_channel->main_channel == PNONE) ? 0 : 1;
  // 0: OC1N active high.
  const irs_u32 CCxNP_value =
    ap_channel->complementary_channel_active_low ? 1 : 0;
  // 0: Off - OC1N is not active
  irs_u32 CCxNE_value = (ap_channel->complementary_channel == PNONE) ? 0 : 1;

  switch (ap_channel->timer_channel) {
    case 1: {
      mp_timer->TIM_CCMR1_bit.OC1M = OCxM_value;
      mp_timer->TIM_CCMR1_bit.OC1PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC1P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC1E = CCxE_value;
      mp_timer->TIM_CCER_bit.CC1NP = CCxNP_value;
      mp_timer->TIM_CCER_bit.CC1NE = CCxNE_value;
    } break;
    case 2: {
      mp_timer->TIM_CCMR1_bit.OC2M = OCxM_value;
      mp_timer->TIM_CCMR1_bit.OC2PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC2P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC2E = CCxE_value;
      mp_timer->TIM_CCER_bit.CC2NP = CCxNP_value;
      mp_timer->TIM_CCER_bit.CC2NE = CCxNE_value;
    } break;
    case 3: {
      mp_timer->TIM_CCMR2_bit.OC3M = OCxM_value;
      mp_timer->TIM_CCMR2_bit.OC3PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC3P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC3E = CCxE_value;
      mp_timer->TIM_CCER_bit.CC3NP = CCxNP_value;
      mp_timer->TIM_CCER_bit.CC3NE = CCxNE_value;
    } break;
    case 4: {
      mp_timer->TIM_CCMR2_bit.OC4M = OCxM_value;
      mp_timer->TIM_CCMR2_bit.OC4PE = OCxPE_value;
      mp_timer->TIM_CCER_bit.CC4P = CCxP_value;
      mp_timer->TIM_CCER_bit.CC4E = CCxE_value;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимый канал");
    } break;
  }
}

irs::cpu_traits_t::frequency_type
irs::arm::st_hal_pwm_gen_dma_t::timer_frequency() const
{
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  return cpu_traits_t::timer_frequency(timer_address);
}

void irs::arm::st_hal_pwm_gen_dma_t::start()
{
  set_mode_capture_compare_registers_for_all_channels(ocm_pwm_mode_1);
  if ((reinterpret_cast<size_t>(mp_timer) == IRS_TIM1_PWM1_BASE) ||
    (reinterpret_cast<size_t>(mp_timer) == IRS_TIM8_PWM2_BASE)) {
    mp_timer->TIM_BDTR_bit.MOE = 1;
  }
  m_started = true;
  start_spi_dma();
}

void irs::arm::st_hal_pwm_gen_dma_t::stop()
{
  if ((reinterpret_cast<size_t>(mp_timer) == IRS_TIM1_PWM1_BASE) ||
    (reinterpret_cast<size_t>(mp_timer) == IRS_TIM8_PWM2_BASE)) {
    mp_timer->TIM_BDTR_bit.MOE = 0;

  }
  set_mode_capture_compare_registers_for_all_channels(ocm_force_inactive_level);
  stop_spi_dma();
  m_started = false;
}

void irs::arm::st_hal_pwm_gen_dma_t::set_duty(irs_uarc a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    *m_channels[i].tim_ccr = static_cast<irs_u32>(a_duty);
  }
}

void irs::arm::st_hal_pwm_gen_dma_t::set_duty(float a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    *m_channels[i].tim_ccr = static_cast<irs_u32>(a_duty*get_max_duty());
  }
}

void irs::arm::st_hal_pwm_gen_dma_t::set_duty(gpio_channel_t a_gpio_channel,
  irs_uarc a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    channel_t* channel = &m_channels[i];
    if ((channel->main_channel == a_gpio_channel) ||
        (channel->complementary_channel == a_gpio_channel)) {
      set_duty_register(channel->tim_ccr, a_duty);
      break;
    }
  }
}

void irs::arm::st_hal_pwm_gen_dma_t::set_duty(gpio_channel_t a_gpio_channel,
  float a_duty)
{
  for (size_type i = 0; i < m_channels.size(); i++) {
    channel_t* channel = &m_channels[i];
    if ((channel->main_channel == a_gpio_channel) ||
        (channel->complementary_channel == a_gpio_channel)) {
      set_duty_register(channel->tim_ccr, a_duty);
      break;
    }
  }
}

void irs::arm::st_hal_pwm_gen_dma_t::set_duty_register(irs_u32* ap_tim_ccr,
  irs_uarc a_duty)
{
  *ap_tim_ccr = static_cast<irs_u32>(a_duty);
}

void irs::arm::st_hal_pwm_gen_dma_t::set_duty_register(irs_u32* ap_tim_ccr,
  float a_duty)
{
  *ap_tim_ccr = static_cast<irs_u32>(a_duty*get_max_duty());
}

irs::cpu_traits_t::frequency_type irs::arm::st_hal_pwm_gen_dma_t::set_frequency(
  cpu_traits_t::frequency_type a_frequency)
{
  m_frequency = a_frequency;
  mp_timer->TIM_ARR = timer_frequency()/a_frequency - 1;
  return get_frequency();
}

irs::cpu_traits_t::frequency_type irs::arm::st_hal_pwm_gen_dma_t::get_frequency() const
{
  return timer_frequency()/(mp_timer->TIM_ARR + 1);
}

irs_uarc irs::arm::st_hal_pwm_gen_dma_t::get_max_duty()
{
  return timer_frequency()/m_frequency - 1;
}

irs::cpu_traits_t::frequency_type irs::arm::st_hal_pwm_gen_dma_t::get_max_frequency()
{
  return get_timer_frequency()/2;
}

irs::cpu_traits_t::frequency_type irs::arm::st_hal_pwm_gen_dma_t::get_timer_frequency()
{
  return timer_frequency();
}

void irs::arm::st_hal_pwm_gen_dma_t::break_enable(gpio_channel_t a_gpio_channel,
  break_polarity_t a_polarity)
{
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  if ((timer_address != IRS_TIM1_PWM1_BASE) &&
    (timer_address != IRS_TIM8_PWM2_BASE)) {
    IRS_LIB_ERROR(ec_standard, "Недопустимый таймер");
  }
  m_break_gpio_channel = a_gpio_channel;
  select_alternate_function_for_break_channel();
  gpio_moder_alternate_function_enable(a_gpio_channel);
  clock_enable(a_gpio_channel);
  if (a_polarity == break_polarity_active_low) {
    mp_timer->TIM_BDTR_bit.BKP = 0;
  } else {
    mp_timer->TIM_BDTR_bit.BKP = 1;
  }
  mp_timer->TIM_BDTR_bit.BKE = 1;
  mp_timer->TIM_SR_bit.BIF = 0;
}

void irs::arm::st_hal_pwm_gen_dma_t::channel_enable(gpio_channel_t a_gpio_channel,
  output_polarity_t a_output_polarity)
{
  channel_enable_helper(a_gpio_channel, false, a_output_polarity);
  /*
  clock_enable(a_gpio_channel);
  gpio_moder_alternate_function_enable(a_gpio_channel);

  const irs_u32 timer_channel = get_timer_channel_and_select_alternate_function(
    a_gpio_channel);

  channel_t* existing_channel = find_channel(timer_channel);
  channel_t channel;
  if (existing_channel) {
    channel = *existing_channel;
  }
  //channel.duty = 0;
  channel.main_channel = a_gpio_channel;
  channel.timer_channel = timer_channel;
  channel.tim_ccr = get_tim_ccr_register(channel.timer_channel);

  set_duty_register(channel.tim_ccr, a_duty);

  m_channels.push_back(channel);
  if (m_started) {
    set_mode_capture_compare_registers(ocm_pwm_mode_1, &m_channels.back());
  } else {
    set_mode_capture_compare_registers(ocm_force_inactive_level,
      &m_channels.back());
  }*/
}

void irs::arm::st_hal_pwm_gen_dma_t::complementary_channel_enable(
  gpio_channel_t a_gpio_channel, output_polarity_t a_output_polarity)
{
  channel_enable_helper(a_gpio_channel, true, a_output_polarity);
  /*clock_enable(a_gpio_channel);
  gpio_moder_alternate_function_enable(a_gpio_channel);
  //m_complementary_gpio_channel = a_gpio_channel;
  //select_alternate_function_for_complementary_channel();

  const irs_u32 timer_channel = get_timer_channel_and_select_alternate_function(
    a_gpio_channel);

  channel_t* existing_channel = find_channel(timer_channel);
  channel_t channel;
  if (existing_channel) {
    channel = *existing_channel;
  }

  channel.complementary_channel = a_gpio_channel;
  channel.timer_channel = timer_channel;
  channel.tim_ccr = get_tim_ccr_register(channel.timer_channel);

  set_duty_register(channel.tim_ccr, 0.f);

  m_channels.push_back(channel);

  if (m_started) {
    set_mode_capture_compare_registers(ocm_pwm_mode_1, &m_channels.back());
  } else {
    set_mode_capture_compare_registers(ocm_force_inactive_level,
      &m_channels.back());
  }*/
}

void irs::arm::st_hal_pwm_gen_dma_t::channel_enable_helper(
  gpio_channel_t a_gpio_channel, bool a_complementary,
  output_polarity_t a_output_polarity)
{
  clock_enable(a_gpio_channel);
  gpio_moder_alternate_function_enable(a_gpio_channel);

  const irs_u32 timer_channel = get_timer_channel_and_select_alternate_function(
    a_gpio_channel);

  channel_t* existing_channel = find_channel(timer_channel);
  channel_t channel;
  if (existing_channel) {
    channel = *existing_channel;
  }
  if (a_complementary) {
    channel.complementary_channel = a_gpio_channel;
    channel.complementary_channel_active_low =
      (a_output_polarity == op_active_low);
  } else {
    channel.main_channel = a_gpio_channel;
    channel.main_channel_active_low = (a_output_polarity == op_active_low);
  }
  channel.timer_channel = timer_channel;
  channel.tim_ccr = get_tim_ccr_register(channel.timer_channel);

  if (!existing_channel) {
    set_duty_register(channel.tim_ccr, 0.f);
  }
  set_mode_capture_compare_registers(ocm_pwm_mode_1, &channel);

  if (existing_channel) {
    *existing_channel = channel;
  } else {
    m_channels.push_back(channel);
  }
}

void irs::arm::st_hal_pwm_gen_dma_t::set_dead_time(float a_time)
{
  const double dead_time = a_time;

  const double t_dtg = 1./timer_frequency();

  const double step_1 = t_dtg;
  const irs_u8 DTG_6_0_max = 127;
  const double max_1 = DTG_6_0_max*step_1;


  const double step_2 = t_dtg*2;
  const irs_u8 DTG_5_0_max = 63;
  const irs_u8 step_count_min_2 = 64;
  const double max_2 = (step_count_min_2 + DTG_5_0_max)*step_2;


  const irs_u8 DTG_4_0_max = 31;
  const double step_3 = t_dtg*8;
  const irs_u8 step_count_min_3 = 32;
  const double max_3 = (step_count_min_3 + DTG_4_0_max)*step_3;


  const double step_4 = t_dtg*16;
  const irs_u8 step_count_min_4 = 32;


  if (dead_time <= max_1) {
    const irs_u8 value = irs::round<double, irs_u8>(dead_time/step_1);
    mp_timer->TIM_BDTR_bit.DTG = min(value, DTG_6_0_max);
  } else if (dead_time <= max_2) {
    const irs_u8 value =
      round<double, irs_u8>(dead_time/step_2-step_count_min_2);
    mp_timer->TIM_BDTR_bit.DTG = (4 << 5) | min(value, DTG_5_0_max);
  } else if (dead_time <= max_3) {
    const irs_u8 value =
      round<double, irs_u8>(dead_time/step_3-step_count_min_3);
    mp_timer->TIM_BDTR_bit.DTG = (6 << 5) | min(value, DTG_4_0_max);
  } else {
    const irs_u8 value =
      round<double, irs_u8>(dead_time/step_4-step_count_min_4);
    mp_timer->TIM_BDTR_bit.DTG = (7 << 5) | min(value, DTG_4_0_max);
  }
}

irs::cpu_traits_t::frequency_type
irs::arm::st_hal_pwm_gen_dma_t::get_auto_reload_value()
{
  return mp_timer->TIM_ARR;
}

irs::cpu_traits_t::frequency_type
irs::arm::st_hal_pwm_gen_dma_t::set_auto_reload_value(
  cpu_traits_t::frequency_type a_arr)
{
  mp_timer->TIM_ARR = a_arr;
  m_frequency = get_frequency();
  return mp_timer->TIM_ARR;
}

/*DMA_HandleTypeDef* irs::arm::st_hal_pwm_gen_dma_t::get_hdma_handle()
{
  return &m_hdma_tx;
}*/

void irs::arm::st_hal_pwm_gen_dma_t::reset_dma()
{
  if (m_hdma_init) {
    stop_dma();
    HAL_DMA_DeInit(&m_hdma_tx);
  }

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  m_hdma_tx.Instance                 = m_settings.tx_dma_y_stream_x;
  m_hdma_tx.Init.Channel             = m_settings.tx_dma_channel;
  m_hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  m_hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_1QUARTERFULL;

  m_hdma_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
  m_hdma_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  m_hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  m_hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  m_hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;


  m_hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  //m_hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  switch (m_settings.data_item_byte_count) {
    case 1: {
      m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    } break;
    case 2: {
      m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    } break;
    case 4: {
      m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    } break;
    default: {
      IRS_ASSERT_MSG("Неучтенное значение размера элемента");
    }
  }

  m_hdma_tx.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  m_hdma_tx.Init.Priority            = m_settings.tx_dma_priority;

  m_hdma_tx.XferCpltCallback = m_settings.XferCpltCallback;
  m_hdma_tx.XferHalfCpltCallback = m_settings.XferHalfCpltCallback;
  m_hdma_tx.XferErrorCallback = m_settings.XferErrorCallback;

  HAL_DMA_Init(&m_hdma_tx);

  //mp_timer->TIM_CR1_bit.URS = 1;
  // Capture/Compare 1 DMA request enable. CC1 DMA request enabled
  mp_timer->TIM_DIER_bit.CC1DE = 1; // __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
  // CCx DMA requests sent when update event occurs
  //mp_timer->TIM_CR2_bit.CCDS = 1;
  //mp_timer->TIM_DIER_bit.UDE = 1;


  //mp_spi_regs->SPI_CR2_bit.TXDMAEN = 1;

  m_hdma_init = true;
}

void irs::arm::st_hal_pwm_gen_dma_t::start_dma()
{
  cleand_dcache_tx();
  /* Enable the Tx DMA channel */

  /*for (size_type i = 0; i < m_channels.size(); i++) {
    *m_channels[i].tim_ccr = static_cast<irs_u32>(a_duty*get_max_duty());
  }*/


  /*HAL_TIM_OC_Start_DMA(&m_hdma_tx, (uint32_t)m_tx_buffer.data(),
    reinterpret_cast<irs_u32>(&mp_timer->TIM_CCR1),
    m_settings.tx_buffer.size()/m_settings.data_item_byte_count);
  */
  /*HAL_DMA_Start*/HAL_DMA_Start_IT(&m_hdma_tx, (uint32_t)m_tx_buffer.data(),
    reinterpret_cast<irs_u32>(&mp_timer->TIM_CCR1),
    m_settings.tx_buffer.size()/m_settings.data_item_byte_count);
}

void irs::arm::st_hal_pwm_gen_dma_t::cleand_dcache_tx()
{
  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // Ничего не делаем
  #elif IRS_STM32F7xx
  SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t*>(m_tx_buffer.data()),
    m_tx_buffer.size());
  #else
  #error Тип контроллера не определён
  #endif // IRS_STM32F7xx
}

void irs::arm::st_hal_pwm_gen_dma_t::stop_dma()
{
  HAL_DMA_Abort(&m_hdma_tx);
}

// class dma_event_t
irs::arm::st_hal_pwm_gen_dma_t::dma_event_t::dma_event_t(
  DMA_HandleTypeDef* ap_hdma_tx
):
  mp_hdma_tx(ap_hdma_tx),
  m_enabled(false)
{
}

void irs::arm::st_hal_pwm_gen_dma_t::dma_event_t::exec()
{
  //if (m_enabled) {
    irs::event_t::exec();
    /* Check the interrupt and clear flag */
    HAL_DMA_IRQHandler(mp_hdma_tx);
  //}
}

/*void irs::arm::st_hal_pwm_gen_dma_t::dma_event_t::enable()
{
  m_enabled = true;
}

void irs::arm::st_hal_pwm_gen_dma_t::dma_event_t::disable()
{
  m_enabled = false;
}*/

#else // ST_HAL_PWM_GEN_DMA_NEW
// class st_hal_pwm_gen_dma_lite_t
irs::arm::st_hal_pwm_gen_dma_t::st_hal_pwm_gen_dma_t(
  const settings_t& a_settings
):
  st_pwm_gen_t(a_settings.gpio_channel, a_settings.timer_address,
    a_settings.frequency, a_settings.duty),
  m_settings(a_settings),
  mp_timer(reinterpret_cast<tim_regs_t*>(a_settings.timer_address)),
  m_hdma_tx(),
  //m_tx_status(false),
  m_hdma_init(false),

  m_tx_buffer(a_settings.tx_buffer),
  m_dma_event(&m_hdma_tx)
{

  irs::clock_enable(a_settings.dma_address);

  irs::interrupt_array()->int_gen_events(a_settings.interrupt_id
    )->push_back(&m_dma_event);

  if (a_settings.interrupt_enabled) {
    /* Enable the DMA STREAM global Interrupt */
    HAL_NVIC_EnableIRQ(a_settings.interrupt);
    NVIC_SetPriority (a_settings.interrupt, a_settings.interrupt_priority);
  }

  reset_dma();
}

void irs::arm::st_hal_pwm_gen_dma_t::start()
{
  st_pwm_gen_t::start();
  start_dma();
}

void irs::arm::st_hal_pwm_gen_dma_t::stop()
{
  st_pwm_gen_t::stop();
  stop_dma();
}

void irs::arm::st_hal_pwm_gen_dma_t::reset_dma()
{
  if (m_hdma_init) {
    stop_dma();
    HAL_DMA_DeInit(&m_hdma_tx);
  }

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  m_hdma_tx.Instance                 = m_settings.tx_dma_y_stream_x;
  m_hdma_tx.Init.Channel             = m_settings.tx_dma_channel;
  m_hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  m_hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_1QUARTERFULL;

  m_hdma_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
  m_hdma_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  m_hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  m_hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  m_hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;


  m_hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  //m_hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  switch (m_settings.data_item_byte_count) {
    case 1: {
      m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    } break;
    case 2: {
      m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    } break;
    case 4: {
      m_hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    } break;
    default: {
      IRS_ASSERT_MSG("Неучтенное значение размера элемента");
    }
  }

  m_hdma_tx.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  m_hdma_tx.Init.Priority            = m_settings.tx_dma_priority;

  m_hdma_tx.XferCpltCallback = m_settings.XferCpltCallback;
  m_hdma_tx.XferHalfCpltCallback = m_settings.XferHalfCpltCallback;
  m_hdma_tx.XferErrorCallback = m_settings.XferErrorCallback;

  HAL_DMA_Init(&m_hdma_tx);


  if (channel_count() > 0) {
    const channel_t* channel = get_channel(0);

    switch (channel->timer_channel) {
      case 1: {
        // Capture/Compare 1 DMA request enable. CC1 DMA request enabled
        mp_timer->TIM_DIER_bit.CC1DE = 1;
      } break;
      case 2: {
        // Capture/Compare 1 DMA request enable. CC2 DMA request enabled
        mp_timer->TIM_DIER_bit.CC2DE = 1;
      } break;
      case 3: {
        // Capture/Compare 1 DMA request enable. CC3 DMA request enabled
        mp_timer->TIM_DIER_bit.CC3DE = 1;
      } break;
      case 4: {
        // Capture/Compare 1 DMA request enable. CC4 DMA request enabled
        mp_timer->TIM_DIER_bit.CC4DE = 1;
      } break;
      default: {
        IRS_ASSERT_MSG("Неучтенное значение номера канала");
      }
    }
  } else {
    IRS_ASSERT_MSG("Не задан канал таймера");
  }
  //mp_timer->TIM_CR1_bit.URS = 1;
  // Capture/Compare 1 DMA request enable. CC1 DMA request enabled
  //mp_timer->TIM_DIER_bit.CC1DE = 1; // __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
  // CCx DMA requests sent when update event occurs
  //mp_timer->TIM_CR2_bit.CCDS = 1;
  //mp_timer->TIM_DIER_bit.UDE = 1;


  //mp_spi_regs->SPI_CR2_bit.TXDMAEN = 1;

  m_hdma_init = true;
}

void irs::arm::st_hal_pwm_gen_dma_t::start_dma()
{
  cleand_dcache_tx();
  /* Enable the Tx DMA channel */

  /*for (size_type i = 0; i < m_channels.size(); i++) {
    *m_channels[i].tim_ccr = static_cast<irs_u32>(a_duty*get_max_duty());
  }*/


  /*HAL_TIM_OC_Start_DMA(&m_hdma_tx, (uint32_t)m_tx_buffer.data(),
    reinterpret_cast<irs_u32>(&mp_timer->TIM_CCR1),
    m_settings.tx_buffer.size()/m_settings.data_item_byte_count);
  */

  //irs_u32* tim_ccr = get_tim_ccr_register(m_settings.);

  const channel_t* channel = get_channel(0);
  if (channel_count() > 0) {
    /*HAL_DMA_Start*/HAL_DMA_Start_IT(&m_hdma_tx, (uint32_t)m_tx_buffer.data(),
      reinterpret_cast<irs_u32>(channel->tim_ccr),
      m_settings.tx_buffer.size()/m_settings.data_item_byte_count);
  } else {
    IRS_ASSERT_MSG("Не задан канал таймера");
  }
  /*HAL_DMA_Start_IT(&m_hdma_tx, (uint32_t)m_tx_buffer.data(),
    reinterpret_cast<irs_u32>(&mp_timer->TIM_CCR1),
    m_settings.tx_buffer.size()/m_settings.data_item_byte_count);*/
}

void irs::arm::st_hal_pwm_gen_dma_t::cleand_dcache_tx()
{
  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // Ничего не делаем
  #elif IRS_STM32F7xx
  SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t*>(m_tx_buffer.data()),
    m_tx_buffer.size());
  #else
  #error Тип контроллера не определён
  #endif // IRS_STM32F7xx
}

void irs::arm::st_hal_pwm_gen_dma_t::stop_dma()
{
  HAL_DMA_Abort(&m_hdma_tx);
}

// class dma_event_t
irs::arm::st_hal_pwm_gen_dma_t::dma_event_t::dma_event_t(
  DMA_HandleTypeDef* ap_hdma_tx
):
  mp_hdma_tx(ap_hdma_tx),
  m_enabled(false)
{
}

void irs::arm::st_hal_pwm_gen_dma_t::dma_event_t::exec()
{
  //if (m_enabled) {
    irs::event_t::exec();
    /* Check the interrupt and clear flag */
    HAL_DMA_IRQHandler(mp_hdma_tx);
  //}
}
#endif // ST_HAL_PWM_GEN_DMA_NEW
#endif // USE_HAL_DRIVER

#endif  //  mcu type

#if defined(USE_STDPERIPH_DRIVER) || defined(USE_HAL_DRIVER)
#ifdef IRS_STM32_F2_F4_F7
namespace {
  void calc_prescaller_and_counter_start(const double a_period_s,
    size_t* ap_prescaler_divider_key,
    irs_u16* ap_counter_start)
  {
    typedef size_t size_type;

    #if (defined(IRS_STM32F2xx) || defined(IRS_STM32F7xx))
    const double divider = 32000;
    #elif defined(IRS_STM32F4xx)
    const double divider = 40000;
    #endif // defined(IRS_STM32F4xx)

    size_type prescaler_divider_key_max = 6;
    size_type prescaler_divider_key = 0;
    size_type prescaler_divider_value_min = 4;
    const double counter_max = 0xFFF;
    const double coefficient =
      (1/(divider/prescaler_divider_value_min))*(counter_max);
    while (prescaler_divider_key < prescaler_divider_key_max) {
      if (coefficient*pow(2, prescaler_divider_key) >= a_period_s) {
        break;
      }
      prescaler_divider_key++;
    }
    *ap_prescaler_divider_key = prescaler_divider_key;
    const double prescaler_divider_value =
      prescaler_divider_value_min*pow(2, prescaler_divider_key);
    double counter_start = a_period_s/(1/(divider/prescaler_divider_value));
    counter_start = irs::bound(counter_start, 1., counter_max);
    *ap_counter_start = static_cast<irs_u16>(counter_start);
  }
}
#endif // IRS_STM32_F2_F4_F7
#endif // defined(USE_STDPERIPH_DRIVER) || defined(USE_HAL_DRIVER)

#ifdef USE_STDPERIPH_DRIVER
#if defined(IRS_STM32F_2_AND_4)

// class st_independent_watchdog_t
irs::arm::st_independent_watchdog_t::st_independent_watchdog_t(
  double a_period_s
):
  m_period_s(a_period_s),
  m_counter_start_value(0)
{
  IRS_LIB_ASSERT((a_period_s >= 0) && (a_period_s <= 100));
  RCC_LSICmd(ENABLE);
  while(RCC_CSR_bit.LSIRDY);
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  size_type prescaler_divider_key = 0;
  irs_u16 counter_start = 0;
  calc_prescaller_and_counter_start(a_period_s, &prescaler_divider_key,
    &counter_start);

  IWDG_SetPrescaler(prescaler_divider_key);
  IWDG_SetReload(static_cast<irs_u16>(counter_start));
  IWDG_ReloadCounter();
}

void irs::arm::st_independent_watchdog_t::start()
{
  IWDG_Enable();
}

void irs::arm::st_independent_watchdog_t::restart()
{
  IWDG_ReloadCounter();
}

bool irs::arm::st_independent_watchdog_t::watchdog_reset_cause()
{
  return RCC_CSR_bit.IWDGRSTF;
}

void irs::arm::st_independent_watchdog_t::clear_reset_status()
{
  RCC_CSR_bit.RMVF = 1;
}

// class st_window_watchdog_t
irs::arm::st_window_watchdog_t::st_window_watchdog_t(
  double a_period_min_s,
  double a_period_max_s
):
  m_counter_start_value(0)
{
  IRS_LIB_ASSERT((a_period_max_s >= 0) && (a_period_max_s <= 1));
  IRS_LIB_ASSERT((a_period_min_s >= 0) && (a_period_min_s <= 1));
  IRS_LIB_ASSERT(a_period_min_s < a_period_max_s);
  const double period_max_ms = a_period_max_s*1000;
  const double period_min_ms = a_period_min_s*1000;
  //clock_enable(IRS_WWDG_BASE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
  const double t_pclk1_ms =
    (1./irs::cpu_traits_t::periphery_frequency_first())*1000;
  const double coefficient = t_pclk1_ms*4096;
  const double counter_end = 0x3F;
  const double counter_min = 0x40;
  const double counter_max = 0x7F;
  const double counter_delta = 0x40;
  size_type wdgtb_prescaler = 0;
  size_type wdgtb_prescaler_max = 3;
  while (wdgtb_prescaler < wdgtb_prescaler_max) {
    if ((coefficient*pow(2, wdgtb_prescaler)*(counter_delta)) >=
      period_max_ms) {
      break;
    }
    wdgtb_prescaler++;
  }

  WWDG_CFR_bit.WDGTB = wdgtb_prescaler;
  double counter_start_value =
    ceil(period_max_ms/(coefficient*pow(2, wdgtb_prescaler))) + counter_end;
  m_counter_start_value = static_cast<irs_u8>(bound(counter_start_value,
    counter_min, counter_max));
  double window_value = ceil((period_max_ms - period_min_ms)/
    (coefficient*pow(2, wdgtb_prescaler))) + counter_end;
  if ((window_value > static_cast<double>(m_counter_start_value)) ||
    (counter_start_value > counter_max)) {
    window_value =  floor((m_counter_start_value - counter_end)*
      (a_period_max_s-a_period_min_s)/a_period_max_s) + counter_end;
  }
  window_value = bound(window_value, counter_min,
    static_cast<double>(m_counter_start_value));
  WWDG_SetWindowValue(static_cast<irs_u8>(window_value));
}

void irs::arm::st_window_watchdog_t::start()
{
  WWDG_Enable(m_counter_start_value);
}

void irs::arm::st_window_watchdog_t::restart()
{
  WWDG_SetCounter(m_counter_start_value);
}

bool irs::arm::st_window_watchdog_t::watchdog_reset_cause()
{
  return RCC_CSR_bit.WWDGRSTF;
}

void irs::arm::st_window_watchdog_t::clear_reset_status()
{
  RCC_CSR_bit.RMVF = 1;
}
#endif // USE_STDPERIPH_DRIVER
#endif // defined(IRS_STM32F_2_AND_4)

#ifdef USE_HAL_DRIVER
#ifdef IRS_STM32_F2_F4_F7
// class st_independent_watchdog_t
irs::arm::st_independent_watchdog_t::st_independent_watchdog_t(
  double a_period_s
):
  m_period_s(a_period_s),
  m_counter_start_value(0)
{
  IRS_LIB_ASSERT((a_period_s >= 0) && (a_period_s <= 100));

  size_type prescaler_divider_key = 0;
  irs_u16 counter_start = 0;
  calc_prescaller_and_counter_start(a_period_s, &prescaler_divider_key,
    &counter_start);

  m_iwdg_handle.Instance = IWDG;
  m_iwdg_handle.Init.Prescaler = prescaler_divider_key;
  m_iwdg_handle.Init.Reload    = static_cast<irs_u16>(counter_start);
  m_iwdg_handle.Init.Window    = IWDG_WINDOW_DISABLE;

  if (HAL_IWDG_Init(&m_iwdg_handle) != HAL_OK) {
    IRS_LIB_ASSERT_MSG("Ошибка HAL_IWDG_Init");
  }
}

void irs::arm::st_independent_watchdog_t::start()
{
  if (HAL_IWDG_Start(&m_iwdg_handle) != HAL_OK) {
    IRS_LIB_ASSERT_MSG("Ошибка HAL_IWDG_Start");
  }
}

void irs::arm::st_independent_watchdog_t::restart()
{
  if (HAL_IWDG_Refresh(&m_iwdg_handle) != HAL_OK) {
    IRS_LIB_ASSERT_MSG("Ошибка HAL_IWDG_Refresh");
  }
}

bool irs::arm::st_independent_watchdog_t::watchdog_reset_cause()
{
  return ((RCC->CSR & RCC_CSR_IWDGRSTF) == RCC_CSR_IWDGRSTF);
}

void irs::arm::st_independent_watchdog_t::clear_reset_status()
{
  SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}
#endif // IRS_STM32_F2_F4_F7
#endif // USE_HAL_DRIVER


#ifdef USE_STDPERIPH_DRIVER
#ifdef IRS_STM32F_2_AND_4
// class st_rtc_t
irs::handle_t<irs::arm::st_rtc_t> irs::arm::st_rtc_t::mp_st_rtc;

irs::arm::st_rtc_t::st_rtc_t(clock_source_t a_clock_source)
{
  mp_st_rtc = this;

  irs_u32 bkp_data_reg_init[rtc_bkp_dr_number] =
  {
    RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2,
    RTC_BKP_DR3, RTC_BKP_DR4, RTC_BKP_DR5,
    RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8,
    RTC_BKP_DR9, RTC_BKP_DR10, RTC_BKP_DR11,
    RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14,
    RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17,
    RTC_BKP_DR18,  RTC_BKP_DR19
  };
  memcpy(bkp_data_reg, bkp_data_reg_init, sizeof(bkp_data_reg_init));

  // Enable the PWR clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  PWR_BackupAccessCmd(ENABLE);

  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != backup_first_data) {
    rtc_config(a_clock_source);
  }
}

irs::arm::st_rtc_t* irs::arm::st_rtc_t::reset(clock_source_t a_clock_source)
{
  mp_st_rtc.reset();
  mp_st_rtc.reset(new st_rtc_t(a_clock_source));
  return mp_st_rtc.get();
}

irs::arm::st_rtc_t* irs::arm::st_rtc_t::get_instance()
{
  if (mp_st_rtc.is_empty()) {
    mp_st_rtc.reset(new st_rtc_t(clock_source_lsi));
  }
  return mp_st_rtc.get();
}

time_t irs::arm::st_rtc_t::get_time() const
{
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateSrtruct;

  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateSrtruct);

  tm now_tm;

  now_tm.tm_hour = RTC_TimeStruct.RTC_Hours;
  now_tm.tm_min = RTC_TimeStruct.RTC_Minutes;
  now_tm.tm_sec = RTC_TimeStruct.RTC_Seconds;

  now_tm.tm_mday = RTC_DateSrtruct.RTC_Date;
  now_tm.tm_mon = RTC_DateSrtruct.RTC_Month - RTC_Month_January;
  now_tm.tm_year = RTC_DateSrtruct.RTC_Year + 100;

  now_tm.tm_isdst = 0;

  now_tm.tm_wday = 0; // Не используется
  now_tm.tm_yday = 0; // Не используется

  return mktime(&now_tm);
}

double irs::arm::st_rtc_t::get_time_double() const
{
  #ifdef IRS_STM32F4xx
  irs_u32 ms =
    1000 - ((irs_u32)((irs_u32)RTC_GetSubSecond()*1000)/(irs_u32)0x3FF);
  time_t s = get_time();
  return s + ms/1000.;
  #else // !IRS_STM32F4xx
  return get_time();
  #endif // !IRS_STM32F4xx
}

void irs::arm::st_rtc_t::set_time(const time_t a_time)
{
  const tm time_tm = *gmtime(&a_time);
  RTC_TimeTypeDef  RTC_TimeStruct;
  RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours   = time_tm.tm_hour;
  RTC_TimeStruct.RTC_Minutes = time_tm.tm_min;
  RTC_TimeStruct.RTC_Seconds = time_tm.tm_sec;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

  RTC_DateTypeDef RTC_DateSrtruct;
  RTC_DateSrtruct.RTC_WeekDay = RTC_Weekday_Thursday;
  RTC_DateSrtruct.RTC_Date = time_tm.tm_mday;
  RTC_DateSrtruct.RTC_Month = time_tm.tm_mon + RTC_Month_January;
  RTC_DateSrtruct.RTC_Year = time_tm.tm_year - 100;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateSrtruct);
}
#ifdef IRS_STM32F4xx
void irs::arm::st_rtc_t::set_calibration(double a_coefficient)
{
  // Ограничение для исключения деления на 0. Диапазон взят с запасом
  // Реальный допустимый диапазон: [-487.1 ppm +488.5 ppm], шаг 0.954 ppm.
  // Он реализован ниже
  a_coefficient = range(a_coefficient, 0.9, 1.1);
  double calm_value = 0;
  irs_u32 rtc_smooth_calib_plus_pulses = RTC_SmoothCalibPlusPulses_Reset;
  if (a_coefficient <= 1) {
    calm_value = (1 << 20)*(1/a_coefficient - 1);
  } else {
    rtc_smooth_calib_plus_pulses = RTC_SmoothCalibPlusPulses_Set;
    calm_value = (1 << 20)*(1/a_coefficient) - (1 << 20) + 512;
  }
  const double max_cycles = 511.;
  calm_value = range(calm_value, 0., max_cycles);
  irs_u32 calm = round<double, irs_u32>(calm_value);
  RTC_SmoothCalibConfig(RTC_SmoothCalibPeriod_32sec,
    rtc_smooth_calib_plus_pulses, calm);
}
#else // !IRS_STM32F4xx
void irs::arm::st_rtc_t::set_calibration(double)
{
}
#endif // !IRS_STM32F4xx

double irs::arm::st_rtc_t::get_calibration() const
{
  #ifdef IRS_STM32F4xx
  const int calp = RTC_CALR_bit.CALP;
  const int calm = RTC_CALR_bit.CALM;
  return 1 + static_cast<double>(calp*512 - calm)/((1 << 20) + calm - calp*512);
  #else // !IRS_STM32F4xx
  return 1;
  #endif // !IRS_STM32F4xx
}

double irs::arm::st_rtc_t::get_calibration_coefficient_min() const
{
  const double max_negative_offset_ppm = 487.1;
  return 1 - max_negative_offset_ppm/1e6; // 0.9995129
}

double irs::arm::st_rtc_t::get_calibration_coefficient_max() const
{
  const double max_positive_offset_ppm = 488.5;
  return 1 + max_positive_offset_ppm/1e6; // 1.0004885
}

void irs::arm::st_rtc_t::rtc_config(clock_source_t a_clock_source)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset RTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);

  if (a_clock_source == clock_source_lsi) {
    RCC_LSICmd(ENABLE);
    // Wait till LSI is ready
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
    }
    // Select the RTC Clock Source
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  } else {
    RCC_LSEConfig(RCC_LSE_ON);
    // Wait till LSE is ready
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
    }
    // Select the RTC Clock Source
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  }

  RCC_RTCCLKCmd(ENABLE);

  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  RTC_AlarmCmd(RTC_Alarm_B, DISABLE);

  /* Configure the RTC data register and RTC prescaler */
  /* ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/

  RTC_InitTypeDef  RTC_InitStructure;
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
  ErrorStatus status = RTC_Init(&RTC_InitStructure);
  if (status != SUCCESS) {
    irs::mlog() << "RTC_Init Error" << endl;
  }

  /* Set the time to 00h 00mn 00s AM */
  RTC_TimeTypeDef  RTC_TimeStruct;
  RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
  RTC_TimeStruct.RTC_Hours   = 0;
  RTC_TimeStruct.RTC_Minutes = 0;
  RTC_TimeStruct.RTC_Seconds = 0;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

  RTC_DateTypeDef RTC_DateSrtruct;
  RTC_DateSrtruct.RTC_WeekDay = RTC_Weekday_Thursday;
  RTC_DateSrtruct.RTC_Date = 14;
  RTC_DateSrtruct.RTC_Month = RTC_Month_October;
  RTC_DateSrtruct.RTC_Year = 13;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateSrtruct);

  /*  Backup SRAM ***************************************************************/
  /* Enable BKPRAM Clock */

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

  /* Write to Backup SRAM with 32-Bit Data */
  for (irs_u32 uwIndex = 0x0; uwIndex < 0x1000; uwIndex += 4)
  {
    *(__IO uint32_t *) (BKPSRAM_BASE + uwIndex) = uwIndex;
  }
  /* Check the written Data */
  irs_u32 uwErrorIndex = 0;
  for (irs_u32 uwIndex = 0x0; uwIndex < 0x1000; uwIndex += 4)
  {
    if ((*(__IO uint32_t *) (BKPSRAM_BASE + uwIndex)) != uwIndex)
    {
      uwErrorIndex++;
    }
  }

  if(uwErrorIndex)
  {
    IRS_DBG_MSG("BKP SRAM Number of errors " << uwErrorIndex);
  }
  else
  {
    IRS_DBG_MSG("BKP SRAM write OK ");
  }

  /* Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode */
  PWR_BackupRegulatorCmd(ENABLE);

  /* Wait until the Backup SRAM low power Regulator is ready */
  while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
  {
  }

/* RTC Backup Data Registers **************************************************/
  /* Write to RTC Backup Data Registers */
  write_to_backup_reg(backup_first_data);
}

void irs::arm::st_rtc_t::write_to_backup_reg(irs_u16 a_first_backup_data)
{
  for (irs_u32 index = 0; index < rtc_bkp_dr_number; index++)
  {
    RTC_WriteBackupRegister(bkp_data_reg[index],
      a_first_backup_data + (index * 0x5A));
  }
}

#endif // IRS_STM32F_2_AND_4
#endif // USE_STDPERIPH_DRIVER

#ifdef USE_HAL_DRIVER
#ifdef IRS_STM32_F2_F4_F7

#define RTC_ASYNCH_PREDIV_DEFAULT   0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV_DEFAULT    0x00FF /* LSE as RTC clock */

#define RTC_ASYNCH_PREDIV_MIN       0
#define RTC_SYNCH_PREDIV_MAX        0x7FFF

// class st_rtc_t
irs::handle_t<irs::arm::st_rtc_t> irs::arm::st_rtc_t::mp_st_rtc;

irs::arm::st_rtc_t::st_rtc_t():
  m_prediv_s(RTC_SYNCH_PREDIV_DEFAULT),
  RtcHandle(),

  m_calibration_process(cp_off),
  m_measure_time(),
  m_rtc_time_start(0),
  m_calibration_time(),
  m_calibration_interval(0),
  m_rtc_time_begin(0),
  m_previous_time_s(0),
  m_counter_calibr_coeff(1)
{
  mp_st_rtc = this;

  irs_u32 bkp_data_reg_init[rtc_bkp_dr_number] =
  {
    RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2,
    RTC_BKP_DR3, RTC_BKP_DR4, RTC_BKP_DR5,
    RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8,
    RTC_BKP_DR9, RTC_BKP_DR10, RTC_BKP_DR11,
    RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14,
    RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17,
    RTC_BKP_DR18,  RTC_BKP_DR19
    #ifdef IRS_STM32F7xx
    ,
    RTC_BKP_DR20, RTC_BKP_DR21, RTC_BKP_DR22,
    RTC_BKP_DR23, RTC_BKP_DR24, RTC_BKP_DR25,
    RTC_BKP_DR26, RTC_BKP_DR27, RTC_BKP_DR28,
    RTC_BKP_DR29, RTC_BKP_DR30, RTC_BKP_DR31
    #endif
  };
  memcpy(bkp_data_reg, bkp_data_reg_init, sizeof(bkp_data_reg_init));

  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV_DEFAULT;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV_DEFAULT;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  RtcHandle.Instance            = RTC;


  // Если устройство было выключено до завершения калибровки, то значения
  // делителей не было возвращено к стандартным значениям.
  const irs_u32 prediv_s = (RtcHandle.Instance->PRER & RTC_PRER_PREDIV_S);
  const irs_u32 prediv_a = (RtcHandle.Instance->PRER & RTC_PRER_PREDIV_A) >> 16;
  const irs_u32 prediv_s_is_not_default = prediv_s != RTC_SYNCH_PREDIV_DEFAULT;
  const irs_u32 prediv_a_is_not_default = prediv_a != RTC_ASYNCH_PREDIV_DEFAULT;

  // Проверяем, инициализирован ли календарь
  const bool calendar_has_not_been_initialized =
    (RtcHandle.Instance->ISR & RTC_ISR_INITS) == (uint32_t)RESET;

  if (calendar_has_not_been_initialized ||
      prediv_s_is_not_default ||
      prediv_a_is_not_default) {
    rtc_config_default();
  }
}

irs::arm::st_rtc_t* irs::arm::st_rtc_t::reset()
{
  mp_st_rtc.reset();
  mp_st_rtc.reset(new st_rtc_t());
  return mp_st_rtc.get();
}

irs::arm::st_rtc_t* irs::arm::st_rtc_t::get_instance()
{
  if (mp_st_rtc.is_empty()) {
    mp_st_rtc.reset(new st_rtc_t());
  }
  return mp_st_rtc.get();
}

time_t irs::arm::st_rtc_t::get_time()
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);

  tm now_tm;

  now_tm.tm_hour = stimestructureget.Hours;
  now_tm.tm_min = stimestructureget.Minutes;
  now_tm.tm_sec = stimestructureget.Seconds;

  now_tm.tm_mday = sdatestructureget.Date;
  now_tm.tm_mon = sdatestructureget.Month - RTC_MONTH_JANUARY;
  now_tm.tm_year = sdatestructureget.Year + 100;

  now_tm.tm_isdst = 0;

  now_tm.tm_wday = sdatestructureget.WeekDay - RTC_WEEKDAY_MONDAY;
  now_tm.tm_yday = 0; // Не используется

  return mktime(&now_tm);
}

double irs::arm::st_rtc_t::get_time_double()
{
  RTC_TimeTypeDef timestruct;
  HAL_RTC_GetTime(&RtcHandle, &timestruct, RTC_FORMAT_BIN);

  double s_fraction = ((1.f*(m_prediv_s-timestruct.SubSeconds))/(m_prediv_s+1));
  //double ms = (1000.*timestruct.SubSeconds/59.);

  time_t s = get_time();
  return s + s_fraction;
}

void irs::arm::st_rtc_t::set_time(const time_t a_time)
{
  HAL_PWR_EnableBkUpAccess();

  const tm time_tm = *gmtime(&a_time);

  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  sdatestructure.Year = time_tm.tm_year - 100;
  sdatestructure.Month = time_tm.tm_mon + RTC_MONTH_JANUARY;
  sdatestructure.Date = time_tm.tm_mday;
  sdatestructure.WeekDay = time_tm.tm_wday + RTC_WEEKDAY_MONDAY;

  if (HAL_RTC_SetDate(&RtcHandle,&sdatestructure, RTC_FORMAT_BIN) != HAL_OK) {
    IRS_LIB_DBG_MSG("Initialization Error");
  }

  stimestructure.Hours = time_tm.tm_hour;
  stimestructure.Minutes = time_tm.tm_min;
  stimestructure.Seconds = time_tm.tm_sec;
  stimestructure.SubSeconds = 0;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN) != HAL_OK) {
    IRS_LIB_DBG_MSG("Initialization Error");
  }
  HAL_PWR_DisableBkUpAccess();
}

#if defined(IRS_STM32F4xx) || defined(IRS_STM32F7xx)
bool irs::arm::st_rtc_t::set_calibration(double a_coefficient)
{
  if (m_calibration_process != cp_off) {
    return false;
  }
  return set_calibration_force(a_coefficient);
}
bool irs::arm::st_rtc_t::set_calibration_force(double a_coefficient)
{
  // Ограничение для исключения деления на 0. Диапазон взят с запасом
  // Реальный допустимый диапазон: [-487.1 ppm +488.5 ppm], шаг 0.954 ppm.
  // Он реализован ниже
  a_coefficient = range(a_coefficient, 0.9, 1.1);

  double calm_value = 0;
  irs_u32 rtc_smooth_calib_plus_pulses = RTC_SMOOTHCALIB_PLUSPULSES_RESET;
  if (a_coefficient <= 1) {
    calm_value = (1 << 20)*(1/a_coefficient - 1);
  } else {
    rtc_smooth_calib_plus_pulses = RTC_SMOOTHCALIB_PLUSPULSES_SET;
    calm_value = (1 << 20)*(1/a_coefficient) - (1 << 20) + 512;
  }
  const double max_cycles = 511.;
  calm_value = range(calm_value, 0., max_cycles);
  uint32_t smooth_calib_minus_pulses_value = round<double, irs_u32>(calm_value);

  HAL_PWR_EnableBkUpAccess();

  HAL_StatusTypeDef status =
    HAL_RTCEx_SetSmoothCalib(&RtcHandle, RTC_SMOOTHCALIB_PERIOD_32SEC,
      rtc_smooth_calib_plus_pulses, smooth_calib_minus_pulses_value);

  HAL_PWR_DisableBkUpAccess();

  return (status == HAL_OK);
}
#else // !(defined(IRS_STM32F4xx) || defined(IRS_STM32F7xx))
bool irs::arm::st_rtc_t::set_calibration(double /*a_coefficient*/)
{
  return false;
}
#endif // !(defined(IRS_STM32F4xx) || defined(IRS_STM32F7xx))

double irs::arm::st_rtc_t::get_calibration() const
{
  #if defined(IRS_STM32F4xx) || defined(IRS_STM32F7xx)
  const int calp = (RtcHandle.Instance->CALR & RTC_CALR_CALP) ? 1 : 0;
  const int calm = (RtcHandle.Instance->CALR & RTC_CALR_CALM);
  return 1 + static_cast<double>(calp*512 - calm)/((1 << 20) + calm - calp*512);
  #else // Other
  return 1;
  #endif // Other
}

double irs::arm::st_rtc_t::get_calibration_coefficient_min() const
{
  const double max_negative_offset_ppm = 487.1;
  return 1 - max_negative_offset_ppm/1e6; // 0.9995129
}

double irs::arm::st_rtc_t::get_calibration_coefficient_max() const
{
  const double max_positive_offset_ppm = 488.5;
  return 1 + max_positive_offset_ppm/1e6; // 1.0004885
}

void irs::arm::st_rtc_t::rtc_config_default()
{
  m_prediv_s = RTC_SYNCH_PREDIV_DEFAULT;

  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV_DEFAULT;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV_DEFAULT;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  RtcHandle.Instance            = RTC;

  HAL_PWR_EnableBkUpAccess();
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK) {
    IRS_LIB_DBG_MSG("Initialization Error");
  }
  HAL_PWR_DisableBkUpAccess();

}

void irs::arm::st_rtc_t::rtc_config_calibration()
{
  /*if (HAL_RTC_DeInit(&RtcHandle) != HAL_OK) {
    IRS_LIB_DBG_MSG("Deinitialization Error");
  }*/

  m_prediv_s = RTC_SYNCH_PREDIV_MAX;

  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV_MIN;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV_MAX;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

  HAL_PWR_EnableBkUpAccess();

  if (HAL_RTC_Init(&RtcHandle) != HAL_OK) {
    IRS_LIB_DBG_MSG("Initialization Error");
  }

  HAL_PWR_DisableBkUpAccess();
}

size_t irs::arm::st_rtc_t::get_backup_reg_count() const
{
  return rtc_bkp_dr_number;
}

irs_u32 irs::arm::st_rtc_t::read_from_backup_reg(irs_u32 a_index)
{
  irs_u32 data = HAL_RTCEx_BKUPRead(&RtcHandle, bkp_data_reg[a_index]);
  return data;
}

void irs::arm::st_rtc_t::write_to_backup_reg(irs_u32 a_index, irs_u32 a_data)
{
  HAL_PWR_EnableBkUpAccess();
  HAL_RTCEx_BKUPWrite(&RtcHandle, bkp_data_reg[a_index], a_data);
  HAL_PWR_DisableBkUpAccess();
}

void irs::arm::st_rtc_t::start_calibration(const double a_interval_s)
{
  m_calibration_interval = a_interval_s;
  m_rtc_time_start = irs::get_time_double();
  m_measure_time.start();

  rtc_config_calibration();

  set_calibration_force(1);

  m_rtc_time_begin = irs::get_time_double();
  m_calibration_time.start();
  m_calibration_process = cp_measure_time;
}

bool irs::arm::st_rtc_t::calibration_completed() const
{
  return m_calibration_process == cp_off;
}

void irs::arm::st_rtc_t::set_counter_calibration(double a_coefficient)
{
  m_counter_calibr_coeff = a_coefficient;
}

double irs::arm::st_rtc_t::get_counter_calibration() const
{
  return m_counter_calibr_coeff;
}

void irs::arm::st_rtc_t::tick()
{
  switch (m_calibration_process) {
    case cp_off: {
    } break;
    case cp_measure_time: {
      if (m_calibration_time.get() > m_calibration_interval) {
        double rtc_time_end = irs::get_time_double();
        double time = m_calibration_time.get()*m_counter_calibr_coeff;

        double rtc_time = rtc_time_end - m_rtc_time_begin;
        double k = time/rtc_time;

        rtc_config_default();

        set_calibration_force(k);

        m_previous_time_s =
          static_cast<time_t>(m_rtc_time_start + m_measure_time.get());
        m_calibration_process = cp_sync;
      }
    } break;
    case cp_sync: {
      const time_t time_s =
        static_cast<time_t>(m_rtc_time_start + m_measure_time.get());
      // Ловим момент, когда значение миллисекунд близко к нулю
      if (time_s != m_previous_time_s) {
        set_time(time_s);
        m_calibration_process = cp_off;
      }
    } break;
  }
}

/*void irs::arm::st_rtc_t::write_to_backup_reg(irs_u16 a_first_backup_data)
{
  HAL_PWR_EnableBkUpAccess();
  for (irs_u32 index = 0; index < rtc_bkp_dr_number; index++) {
    HAL_RTCEx_BKUPWrite(&RtcHandle, bkp_data_reg[index],
      a_first_backup_data + (index * 0x5A));
  }
  HAL_PWR_DisableBkUpAccess();
}*/

// class rtc_backup_reg_page_mem_t
irs::arm::rtc_backup_reg_page_mem_t::rtc_backup_reg_page_mem_t(
  st_rtc_t* ap_st_rtc,
  size_type a_page_size
):
  mp_st_rtc(ap_st_rtc),
  m_page_size(a_page_size),
  m_page_count(ap_st_rtc->get_backup_reg_count()*reg_size/a_page_size)
{
  IRS_LIB_ASSERT(m_page_size%reg_size == 0);
}

void irs::arm::rtc_backup_reg_page_mem_t::read_page(irs_u8 *ap_buf, irs_uarc a_index)
{
  IRS_LIB_ASSERT(a_index < m_page_count);

  const size_t reg_begin = (m_page_size*a_index)/reg_size;
  const size_t reg_end = (m_page_size*(a_index + 1))/reg_size;

  IRS_LIB_ASSERT(m_page_size%reg_size == 0);

  reg_type* buf = reinterpret_cast<reg_type*>(ap_buf);
  for (size_t i = reg_begin; i < reg_end;i++) {
    *buf = mp_st_rtc->read_from_backup_reg(i);    
    buf++;
  }
}

void irs::arm::rtc_backup_reg_page_mem_t::write_page(const irs_u8 *ap_buf,
  irs_uarc a_index)
{
  IRS_LIB_ASSERT(a_index < m_page_count);

  const size_t reg_begin = (m_page_size*a_index)/reg_size;
  const size_t reg_end = (m_page_size*(a_index + 1))/reg_size;

  IRS_LIB_ASSERT(m_page_size%reg_size == 0);

  const reg_type* buf = reinterpret_cast<const reg_type*>(ap_buf);
  for (size_t i = reg_begin; i < reg_end ;i++) {
    mp_st_rtc->write_to_backup_reg(i, *buf);    
    buf++;
  }
}

irs::arm::rtc_backup_reg_page_mem_t::size_type
irs::arm::rtc_backup_reg_page_mem_t::page_size() const
{
  return m_page_size;
}

irs_uarc irs::arm::rtc_backup_reg_page_mem_t::page_count() const
{
  return m_page_count;
}

irs_status_t irs::arm::rtc_backup_reg_page_mem_t::status() const
{
  return irs_st_ready;
}

void irs::arm::rtc_backup_reg_page_mem_t::tick()
{
}

irs::arm::rtc_backup_reg_data_t::rtc_backup_reg_data_t(st_rtc_t* ap_st_rtc,
  irs_uarc a_index, irs_uarc a_size, size_type a_cluster_size
):
  mxdata_comm_t(IRS_NULL, a_index, a_size, true),
  m_page_mem(ap_st_rtc, a_cluster_size),
  m_mem_data(&m_page_mem, a_cluster_size)
{
  connect(&m_mem_data);
}

bool irs::arm::rtc_backup_reg_data_t::error()
{
  return m_mem_data.error();
}

#endif // IRS_STM32_F2_F4_F7
#endif // USE_HAL_DRIVER

// class pwm_pin_t
irs::pwm_pin_t::pwm_pin_t(irs::handle_t<pwm_gen_t> ap_pwm_gen):
  mp_pwm_gen(ap_pwm_gen),
  m_started(false)
{
  mp_pwm_gen->stop();
}

bool irs::pwm_pin_t::pin()
{
  return m_started;
}

void irs::pwm_pin_t::set()
{
  mp_pwm_gen->start();
  m_started = true;
}

void irs::pwm_pin_t::clear()
{
  mp_pwm_gen->stop();
  m_started = false;
}

void irs::pwm_pin_t::set_dir(dir_t /*a_dir*/)
{
}

// class decoder_t
irs::decoder_t::decoder_t():
  m_pins(),
  m_pin_index(0)
{
}

/*#ifdef IRS_STM32_F2_F4_F7
void irs::decoder_t::add(gpio_channel_t a_channel)
{
  const size_t port_address = get_port_address(a_channel);
  const irs_u32 pin_index = get_pin_index(a_channel);
  m_pins.push_back(new irs::arm::io_pin_t(port_address, pin_index,
    irs::io_t::dir_out));

  add(new irs::arm::io_pin_t(port_address, pin_index, irs::io_t::dir_out));
}
#endif // IRS_STM32_F2_F4_F7*/

void irs::decoder_t::add(irs::handle_t<gpio_pin_t> ap_gpio_pin)
{
  m_pins.push_back(ap_gpio_pin);
}

void irs::decoder_t::select_pin(irs_u32 a_pin_index)
{
  m_pin_index = a_pin_index;
  const size_type count = min(m_pins.size(), sizeof(a_pin_index));
  irs_u32 code = 1;
  for (size_type i = 0; i < count; i++) {
    if (a_pin_index & code) {
      m_pins[i]->set();
    } else {
      m_pins[i]->clear();
    }
    code <<= 1;
  }
}

irs_u32 irs::decoder_t::get_selected_pin()
{
  return m_pin_index;
}

#endif  //  __ICCARM__
