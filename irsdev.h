#ifndef irsdevH
#define irsdevH

#include <irsdefs.h>
#include <irsdev.h>
#include <irscpu.h>
#include <timer.h>

#define PWM_ZERO_PULSE 1

#ifdef PWM_ZERO_PULSE
  #include <irsint.h>
  #if !defined(IRS_LINUX)
    #include <irsarchint.h>
  #endif // IRS_LINUX
#endif  //  PWM_ZERO_PULSE

namespace irs
{

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
  using pwm_gen_t::set_duty;
  virtual void set_duty(irs_uarc a_duty, phase_t a_phase) = 0;
  virtual void set_duty(float a_duty, phase_t a_phase) = 0;
};

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

} //  arm

#endif  //  __ICCARM__

} //  irs

#endif  //  irsdevH
