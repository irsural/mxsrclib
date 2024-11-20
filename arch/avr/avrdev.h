//! \file
//! \ingroup drivers_group

#ifndef avrdevH
#define avrdefH

#include <irsdev.h>
#include <ioavr.h>

namespace irs
{
namespace avr
{

//! \addtogroup drivers_group
//! @{

#ifndef __ATmega128__
  
enum CTC_mode_t
{
  CTC_mode_off = 0,
  CTC_mode_on = 1
};

class OC0A_pwm_t : public pwm_gen_t
{
  const irs_uarc m_max_duty;
  CTC_mode_t m_CTC_mode;
  const cpu_traits_t::frequency_type m_max_frequency;
  cpu_traits_t::frequency_type m_frequency;
  
  irs_uarc set_freq_divider(irs_uarc a_divider);
public:
  OC0A_pwm_t(irs_uarc a_init_value = 0, CTC_mode_t a_CTC_mode = CTC_mode_off);
  virtual ~OC0A_pwm_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty); // Коэф. зап. в "попугаях"
  virtual void set_duty(float a_duty);    // Коэф. заполнения (1/скважность)
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type a_frequency); // Гц
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
};

class OC0B_pwm_t : public pwm_gen_t
{
  const irs_uarc m_max_duty;
  const cpu_traits_t::frequency_type m_max_frequency;
  cpu_traits_t::frequency_type m_frequency;
  irs_uarc set_freq_divider(irs_uarc a_divider);
public:
  OC0B_pwm_t(irs_uarc a_init_value = 0);
  virtual ~OC0B_pwm_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty); // Коэф. зап. в "попугаях"
  virtual void set_duty(float a_duty);    // Коэф. заполнения (1/скважность)
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type a_frequency); // Гц
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
};

class OC2A_pwm_t : public pwm_gen_t
{
  const irs_uarc m_max_duty;
  const cpu_traits_t::frequency_type m_max_frequency;
  cpu_traits_t::frequency_type m_frequency;
  irs_uarc set_freq_divider(irs_uarc a_divider);
public:
  OC2A_pwm_t(irs_uarc a_init_value = 0);
  virtual ~OC2A_pwm_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty); // Коэф. зап. в "попугаях"
  virtual void set_duty(float a_duty);    // Коэф. заполнения (1/скважность)
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type a_frequency); // Гц
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
};

class OC1AB_pwm_t : public pwm_gen_t
{
  const irs_uarc m_max_duty;
  const cpu_traits_t::frequency_type m_max_frequency;
  cpu_traits_t::frequency_type m_frequency;
  irs_uarc set_freq_divider(irs_uarc a_divider);
public:
  OC1AB_pwm_t(irs_uarc a_init_value = 0);
  virtual ~OC1AB_pwm_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty); // Коэф. зап. в "попугаях"
  virtual void set_duty(float a_duty);    // Коэф. заполнения (1/скважность)
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type a_frequency); // Гц
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
};

#endif //__ATmega128__

//! @}

} // namespace avr

} // namespace irs

#endif  //  avrdevH
