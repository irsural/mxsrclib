//! \file
//! \ingroup drivers_group
//! \brief Устройства
//!
//! Дата: 08.07.2010
//! Ранняя дата: 08.07.2010

#ifndef irsdevH
#define irsdevH

#include <irsdefs.h>

#include <irsdev.h>
#include <irscpu.h>
#include <timer.h>

#ifdef __ICCARM__
#include <armioregs.h>
#endif // __ICCARM__

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

class wdt_t
{
public:
  wdt_t(size_t a_period_s = 300):
    m_period_s(a_period_s)
  {
    RCGC0_bit.WDT0 = 1;
    for (irs_u8 i = 10; i; i--);
    
    WDT0LOCK = 0x1ACCE551;
    WDT0LOAD = static_cast<irs_u32>(m_period_s*irs::cpu_traits_t::frequency());
    WDT0CTL_bit.RESEN = 1;
    WDT0CTL_bit.INTEN = 1;
    WDT0TEST_bit.STALL = 1; // вероятно не работает с J-Link;
    WDT0LOCK = 0x0;
  }
  void restart()
  {
    WDT0LOCK = 0x1ACCE551;
    WDT0ICR = 0x11111111;
    WDT0LOCK = 0x0;
  }
private:
  size_t m_period_s;
}; // wdt_t

//! @}

} //  arm

#endif  //  __ICCARM__

} //  irs

#endif  //  irsdevH
