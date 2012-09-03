//! \file
//! \ingroup drivers_group
//! \brief Устройства
//!
//! Дата: 28.04.2011
//! Ранняя дата: 08.07.2010

#ifndef irsdevH
#define irsdevH

#include <irsdefs.h>

#include <irscpu.h>
#include <timer.h>

#ifdef __ICCARM__
#include <armioregs.h>
#endif // __ICCARM__
#include <irsgpio.h>
#include <mxdata.h>

#define PWM_ZERO_PULSE 1

#ifdef PWM_ZERO_PULSE
  #include <irsint.h>
  #if defined(__ICCAVR__) || defined(__ICCARM__)
    #include <irsarchint.h>
  #endif // IRS_LINUX
#endif  //  PWM_ZERO_PULSE

#include <irsfinal.h>

namespace irs
{

//! \addtogroup drivers_group
//! @{

class pwm_gen_t
{
public:
  virtual ~pwm_gen_t() {}
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void set_duty(irs_uarc a_duty) = 0; // Коэф. зап. в "попугаях"
  virtual void set_duty(float a_duty) = 0;    // Коэф. заполнения (1/скважность)
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type  a_frequency) = 0; // Гц
  virtual irs_uarc get_max_duty() = 0;
  virtual cpu_traits_t::frequency_type get_max_frequency() = 0;
};

#ifndef __WATCOMC__
class three_phase_pwm_gen_t : public pwm_gen_t
{
public:
  enum phase_t
  {
    PHASE_A,
    PHASE_B,
    PHASE_C,
    PHASE_ALL
  };
  virtual ~three_phase_pwm_gen_t() {}
  using irs::pwm_gen_t::set_duty;
  virtual void set_duty(irs_uarc a_duty, phase_t a_phase) = 0;
  virtual void set_duty(float a_duty, phase_t a_phase) = 0;
};
#endif //__WATCOMC__

#ifdef __ICCARM__

namespace arm
{

class arm_three_phase_pwm_t : public three_phase_pwm_gen_t
{
private:
  typedef cpu_traits_t::frequency_type freq_t;
public:
  arm_three_phase_pwm_t(freq_t a_freq, counter_t a_dead_time);
  virtual ~arm_three_phase_pwm_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty);
  virtual void set_duty(irs_uarc a_duty, phase_t a_phase);
  virtual void set_duty(float a_duty);
  virtual void set_duty(float a_duty, phase_t a_phase);
  virtual freq_t set_frequency(freq_t a_frequency);
  virtual irs_uarc get_max_duty();
  virtual freq_t get_max_frequency();
private:
  enum
  {
    pwm_clk_div = 2,
    pwm_all_sync = 0x7,
    pwm_all_enable = 0x3F,
    pwm_all_disable = 0
  };
  const freq_t m_max_freq;
  const freq_t m_min_freq;
  freq_t m_freq;

  irs_u16 calc_load_reg_value(freq_t a_freq);
  #ifdef PWM_ZERO_PULSE
    event_connect_t<arm_three_phase_pwm_t> m_int_event;
    void interrupt();
  #endif  //  PWM_ZERO_PULSE
};

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)

class gptm_usage_t {
  irs_u8 m_channels;
public:
  inline gptm_usage_t() { m_channels = 0xFF; }
  inline ~gptm_usage_t() { m_channels = 0xFF; }
  inline bool channel_free(gptm_channel_t a_channel) {
    return m_channels & (1 << a_channel);
  }
  inline void set_channel_free(gptm_channel_t a_channel) {
    m_channels |= (1 << a_channel);
  }
  inline void set_channel_use(gptm_channel_t a_channel) {
    m_channels &= ~(1 << a_channel);
  }
};

//  Класс однофазного шим-генератора на основе таймера общего назначения
//  a_timer_module - базовый адрес используемого таймера,
//  a_port - базовый адрес порта выходного сигнала,
//  a_bit - номер бита порта выходного сигнала,
//  a_frequency - частота выходного сигнала,
//  a_duty - коэффициент заполнения выходного сигнала

class gptm_generator_t : public pwm_gen_t
{
public:
  gptm_generator_t(gpio_channel_t a_gpio_channel,
    cpu_traits_t::frequency_type a_frequency, float a_duty = 0.5f);
  ~gptm_generator_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty);
  virtual void set_duty(float a_duty);
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type  a_frequency);
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
private:
  bool m_valid_input_data;
  gptm_channel_t m_gptm_channel;
  irs_u8 m_enable_bit;
  p_arm_port_t mp_enable_reg;
  p_arm_port_t mp_load_reg;
  p_arm_port_t mp_match_reg;
  arm_port_t m_gpio_base;
  void init_gptm_channel(gptm_channel_t a_gptm_channel,
    cpu_traits_t::frequency_type a_frequency, float a_duty);
  void init_gpio_port(gpio_channel_t a_gpio_channel, irs_u8 a_gpio_pmc_value);
  irs_u16 calc_load_value(cpu_traits_t::frequency_type a_frequency);
};

#elif defined(__STM32F100RBT__) || defined(IRS_STM32F2xx)

class timers_usage_t {
public:
  inline timers_usage_t() { m_timers = 0xFFFFFFFF; }
  inline ~timers_usage_t() { m_timers = 0xFFFFFFFF; }
  inline bool is_timer_free(size_t a_timer_index) {
    return m_timers & (1 << a_timer_index);
  }
  inline void set_timer_free(size_t a_timer_index) {
    m_timers |= (1 << a_timer_index);
  }
  inline void set_timer_use(size_t a_timer_index) {
    m_timers &= ~(1 << a_timer_index);
  }
private:
  irs_u32 m_timers;
};

/*enum init_pwm_gen_mode_t
{
  init_pwm_gen_channel_only,
  init_pwm_gen_full
};*/

enum break_polarity_t {
  break_polarity_active_low,
  break_polarity_active_high
};

class st_pwm_gen_t: public pwm_gen_t
{
public:
  st_pwm_gen_t(gpio_channel_t a_gpio_channel, size_t a_timer_address,
    cpu_traits_t::frequency_type a_frequency, float a_duty = 0.5f);
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty);
  virtual void set_duty(float a_duty);
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type  a_frequency);
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
  void break_enable(gpio_channel_t a_gpio_channel, break_polarity_t a_polarity);
  void break_disable();
  void complementary_channel_enable(gpio_channel_t a_gpio_channel);  
private:
  void initialize_timer_and_get_tim_ccr_register();  
  cpu_traits_t::frequency_type  timer_frequency();
  vector<st_timer_name_t> get_available_timers(gpio_channel_t a_gpio_channel);
  gpio_channel_t m_gpio_channel;
  tim_regs_t* mp_timer;
  cpu_traits_t::frequency_type m_frequency;
  float m_duty;
  irs_u32* mp_tim_ccr;  
  gpio_channel_t m_complementary_gpio_channel;
  gpio_channel_t m_break_gpio_channel;
  //bool m_break_enabled;  
  //break_polarity_t m_break_polarity;
};

#else
  #error Тип контроллера не определён
#endif  //  mcu type

class watchdog_timer_t
{
public:
  watchdog_timer_t(size_t a_period_s = 300):
    m_period_s(a_period_s)
  {
    #ifdef __LM3SxBxx__
    RCGC0_bit.WDT0 = 1;
    for (irs_u8 i = 10; i; i--);
    #endif  //  __LM3SxBxx__
    #ifdef __LM3Sx9xx__
    RCGC0_bit.WDT = 1;
    for (irs_u8 i = 10; i; i--);
    #endif  //  __LM3SxBxx__
  }
  void start()
  {
    #ifdef __LM3SxBxx__
    WDT0LOCK = 0x1ACCE551;
    WDT0LOAD = static_cast<irs_u32>(m_period_s*irs::cpu_traits_t::frequency());
    WDT0CTL_bit.RESEN = 1;
    WDT0CTL_bit.INTEN = 1;
    WDT0TEST_bit.STALL = 1; // вероятно не работает с J-Link;
    WDT0LOCK = 0x0;
    #endif  //  __LM3SxBxx__
    #ifdef __LM3Sx9xx__
    WDTLOCK = 0x1ACCE551;
    WDTLOAD = static_cast<irs_u32>(m_period_s*irs::cpu_traits_t::frequency());
    WDTCTL_bit.RESEN = 1;
    WDTCTL_bit.INTEN = 1;
    WDTTEST_bit.STALL = 1; // вероятно не работает с J-Link;
    WDTLOCK = 0x0;
    #endif  //  __LM3SxBxx__
  }
  void restart()
  {
    #ifdef __LM3SxBxx__
    WDT0LOCK = 0x1ACCE551;
    WDT0ICR = 0x11111111;
    WDT0LOCK = 0x0;
    #endif  //  __LM3SxBxx__
    #ifdef __LM3Sx9xx__
    WDTLOCK = 0x1ACCE551;
    WDTICR = 0x11111111;
    WDTLOCK = 0x0;
    #endif  //  __LM3Sx9xx__
  }
  bool watchdog_reset_cause()
  {
    #if defined(__LM3SxBxx__)
    return RESC_bit.WDT0;
    #elif defined(__LM3Sx9xx__)
    return RESC_bit.WDT;
    #elif defined(__STM32F100RBT__)
    return false;
    #elif defined(IRS_STM32F2xx)
    return false;
    #else
      #error Тип контроллера не определён
    #endif  //  mcu type
  }
private:
  size_t m_period_s;
}; // watchdog_timer_t

//! @}

} //  arm

#endif  //  __ICCARM__

class pwm_pin_t: public gpio_pin_t
{
public:
  pwm_pin_t(irs::handle_t<pwm_gen_t> ap_pwm_gen);
  virtual bool pin();
  virtual void set();
  virtual void clear();
  virtual void set_dir(dir_t a_dir);
  void set_state(io_pin_value_t a_value);
private:
  irs::handle_t<pwm_gen_t> mp_pwm_gen;
  bool m_started;
};

} //  irs

#endif  //  irsdevH
