#ifndef irsdevH
#define irsdevH

#include <irsdefs.h>
#include <irsdev.h>
#include <irscpu.h>

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

} //  irs

#endif  //  irsdevH
