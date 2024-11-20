//! \file
//! \ingroup drivers_group

#include <avrdev.h>

#ifndef __ATmega128__

//------------------------------------------------------------------------------

irs::avr::OC0A_pwm_t::OC0A_pwm_t(irs_uarc a_init_duty, CTC_mode_t a_CTC_mode):
  m_max_duty(255),
  m_CTC_mode(a_CTC_mode),
  m_max_frequency((m_CTC_mode == CTC_mode_off)?
    cpu_traits_t::frequency()/(m_max_duty + 1):
    cpu_traits_t::frequency()/(m_max_duty + 1)/2),
  m_frequency(m_max_frequency)
{
  //  output pin = OC0A = PB7
  DDRB_DDB7 = 1;
  PORTB_PORTB7 = 0;
  //
  TCCR0A_COM0A1 = 0;
  TCCR0A_COM0A0 = 0;
  
  switch (m_CTC_mode) {
    case CTC_mode_off: {
      //  Fast PWM, TOP = 0xFF
      TCCR0B_WGM02 = 0;
      TCCR0A_WGM01 = 1;
      TCCR0A_WGM00 = 1;
    } break;
    case CTC_mode_on: {
      // Добавил Крашенинников 19.11.2024 для изменения частоты "пищалки"
      // УПМС-2
      // CTC, TOP = OCR0A
      // В этом режиме set_duty задает период импульсов,
      // а сигнал всегда "меандр"
      TCCR0B_WGM02 = 0;
      TCCR0A_WGM01 = 1;
      TCCR0A_WGM00 = 0;
    } break;
  }
  
  // Clock source = fin / 1
  set_freq_divider(0);
  //  Timer/Counter register
  TCNT0 = 0;
  //  Output compare registers
  if (a_init_duty > m_max_duty) a_init_duty = m_max_duty;
  OCR0A = a_init_duty;
}

irs_uarc irs::avr::OC0A_pwm_t::set_freq_divider(irs_uarc a_divider)
{
  if ((m_CTC_mode == CTC_mode_off) && (a_divider >= 1024) ||
    (m_CTC_mode == CTC_mode_on) && (a_divider > 256))
  {
    TCCR0B_CS02 = 1;
    TCCR0B_CS01 = 0;
    TCCR0B_CS00 = 1;
    return 1024;
  }
  if ((m_CTC_mode == CTC_mode_off) && (a_divider >= 256) ||
    (m_CTC_mode == CTC_mode_on) && (a_divider > 64))
  {
    TCCR0B_CS02 = 1;
    TCCR0B_CS01 = 0;
    TCCR0B_CS00 = 0;
    return 256;
  }
  if ((m_CTC_mode == CTC_mode_off) && (a_divider >= 64) ||
    (m_CTC_mode == CTC_mode_on) && (a_divider > 8))
  {
    TCCR0B_CS02 = 0;
    TCCR0B_CS01 = 1;
    TCCR0B_CS00 = 1;
    return 64;
  }
  if ((m_CTC_mode == CTC_mode_off) && (a_divider >= 8) ||
    (m_CTC_mode == CTC_mode_on) && (a_divider > 1))
  {
    TCCR0B_CS02 = 0;
    TCCR0B_CS01 = 1;
    TCCR0B_CS00 = 0;
    return 8;
  }
  if ((m_CTC_mode == CTC_mode_off) && (a_divider >= 1) ||
    (m_CTC_mode == CTC_mode_on) && (a_divider == 1))
  if (a_divider >= 1)
  {
    TCCR0B_CS02 = 0;
    TCCR0B_CS01 = 0;
    TCCR0B_CS00 = 1;
    return 1;
  }
  TCCR0B_CS02 = 0;
  TCCR0B_CS01 = 0;
  TCCR0B_CS00 = 0;
  return 0;
}

irs::avr::OC0A_pwm_t::~OC0A_pwm_t()
{
  //  output pin = OC0A = PG5
  DDRB_DDB7 = 0;
  PORTB_PORTB7 = 0;
  //  Clear OC0A on compare match, set on TOP
  TCCR0A_COM0A1 = 0;
  TCCR0A_COM0A0 = 0;
  // Clock source = fin / 1
  set_freq_divider(0);
  //  Output compare registers
  OCR0A = 0;
}

void irs::avr::OC0A_pwm_t::start()
{
  //  output pin = OC0A = PG5
  DDRB_DDB7 = 1;
  PORTB_PORTB7 = 0;
  switch (m_CTC_mode) {
    case CTC_mode_off: {
      //  Clear OC0A on compare match, set on TOP
      TCCR0A_COM0A1 = 1;
      TCCR0A_COM0A0 = 0;
    } break;
    case CTC_mode_on: {
      //  Toggle OC0A on compare match for CTC mode
      TCCR0A_COM0A1 = 0;
      TCCR0A_COM0A0 = 1;
    } break;
  }
  //
  set_freq_divider(m_max_frequency/m_frequency);
}

void irs::avr::OC0A_pwm_t::stop()
{
  //  output pin = OC0A = PG5
  DDRB_DDB7 = 1;
  PORTB_PORTB7 = 0;
  //
  TCCR0A_COM0A1 = 0;
  TCCR0A_COM0A0 = 0;
  //
  set_freq_divider(0);
}

void irs::avr::OC0A_pwm_t::set_duty(irs_uarc a_duty)
{
  if (a_duty > m_max_duty) a_duty = m_max_duty;
  OCR0A = a_duty;
}

void irs::avr::OC0A_pwm_t::set_duty(float a_duty)
{
  if (a_duty > 1.f) a_duty = 1.f;
  if (a_duty < 0.f) a_duty = 0.f;
  OCR0A = irs_uarc(a_duty * float(m_max_duty));
}

irs::cpu_traits_t::frequency_type irs::avr::OC0A_pwm_t::set_frequency(
  irs::cpu_traits_t::frequency_type a_frequency)
{
  switch (m_CTC_mode) {
    case CTC_mode_off: {
      irs_uarc div = 1;
      if (a_frequency < m_max_frequency) div = m_max_frequency/a_frequency;
      div = set_freq_divider(div);
      m_frequency = m_max_frequency/div;
      return m_frequency;
    }
    case CTC_mode_on: {
      //mlog() << "set_frequency(" << a_frequency << ")" << endl;
      if (a_frequency <= 0) {
        a_frequency = 1;
      }
      irs_uarc div = m_max_frequency/a_frequency + 1;
      div = set_freq_divider(div);
      //mlog() << "div = " << div << endl;
      // m_frequency нужна только для делителя
      // Она не отражает реальной частоты
      m_frequency = m_max_frequency/div;
      // Крашенинников 20.11.2024: Перенебрегаю вычитанием единицы из duty
      // и частота всегда будет ниже или близка к заданной,
      // иначе была бы выше или равна
      // Дле округления к ближайшей требуется вычисления с плавающей точкой
      // или более сложные в целых, чего я хотел избежать
      // На данный момент режим CTC используется только в УПМС-2
      irs::cpu_traits_t::frequency_type duty =
        cpu_traits_t::frequency()/(2*a_frequency*div);
      if (duty > 255) {
        duty = 255;
      }
      set_duty(static_cast<irs_uarc>(duty));
      mlog() << "duty = " << duty << endl;
      
      // Пересчет в реально выставляемую частоту
      irs::cpu_traits_t::frequency_type frequency =
        cpu_traits_t::frequency()/(2*div*(1 + duty));
      //mlog() << "Реально выставляемая частота: " << frequency << endl;
      return frequency;
    }
    default: {
      return 0;
    }
  }
}

irs_uarc irs::avr::OC0A_pwm_t::get_max_duty()
{
  return m_max_duty;
}

irs::cpu_traits_t::frequency_type irs::avr::OC0A_pwm_t::get_max_frequency()
{
  return m_max_frequency;
}

//------------------------------------------------------------------------------

irs::avr::OC0B_pwm_t::OC0B_pwm_t(irs_uarc a_init_duty):
  m_max_duty(255),
  m_max_frequency(cpu_traits_t::frequency()/(m_max_duty + 1)),
  m_frequency(m_max_frequency)
{
  //  output pin = OC0B = PG5
  DDRG_DDG5 = 1;
  PORTG_PORTG5 = 0;
  //  Clear OC0B on compare match, set on TOP
  TCCR0A_COM0B1 = 0;
  TCCR0A_COM0B0 = 0;
  //  Fast PWM, TOP = 0xFF
  TCCR0B_WGM02 = 0;
  TCCR0A_WGM01 = 1;
  TCCR0A_WGM00 = 1;
  // Clock source = fin / 1
  set_freq_divider(0);
  //  Timer/Counter register
  TCNT0 = 0;
  //  Output compare registers
  if (a_init_duty > m_max_duty) a_init_duty = m_max_duty;
  OCR0B = a_init_duty;
}

irs_uarc irs::avr::OC0B_pwm_t::set_freq_divider(irs_uarc a_divider)
{
  if (a_divider >= 1024)
  {
    TCCR0B_CS02 = 1;
    TCCR0B_CS01 = 0;
    TCCR0B_CS00 = 1;
    return 1024;
  }
  if (a_divider >= 256)
  {
    TCCR0B_CS02 = 1;
    TCCR0B_CS01 = 0;
    TCCR0B_CS00 = 0;
    return 256;
  }
  if (a_divider >= 64)
  {
    TCCR0B_CS02 = 0;
    TCCR0B_CS01 = 1;
    TCCR0B_CS00 = 1;
    return 64;
  }
  if (a_divider >= 8)
  {
    TCCR0B_CS02 = 0;
    TCCR0B_CS01 = 1;
    TCCR0B_CS00 = 0;
    return 8;
  }
  if (a_divider >= 1)
  {
    TCCR0B_CS02 = 0;
    TCCR0B_CS01 = 0;
    TCCR0B_CS00 = 1;
    return 1;
  }
  TCCR0B_CS02 = 0;
  TCCR0B_CS01 = 0;
  TCCR0B_CS00 = 0;
  return 0;
}

irs::avr::OC0B_pwm_t::~OC0B_pwm_t()
{
  //  output pin = OC0B = PG5
  DDRG_DDG5 = 0;
  PORTG_PORTG5 = 0;
  //  Clear OC0B on compare match, set on TOP
  TCCR0A_COM0B1 = 0;
  TCCR0A_COM0B0 = 0;
  // Clock source = fin / 1
  set_freq_divider(0);
  //  Output compare registers
  OCR0B = 0;
}

void irs::avr::OC0B_pwm_t::start()
{
  //  output pin = OC0B = PG5
  DDRG_DDG5 = 1;
  PORTG_PORTG5 = 0;
  //  Clear OC0B on compare match, set on TOP
  TCCR0A_COM0B1 = 1;
  TCCR0A_COM0B0 = 0;
  //
  set_freq_divider(m_max_frequency / m_frequency);
}

void irs::avr::OC0B_pwm_t::stop()
{
  //  output pin = OC0B = PG5
  DDRG_DDG5 = 1;
  PORTG_PORTG5 = 0;
  //  Clear OC0B on compare match, set on TOP
  TCCR0A_COM0B1 = 0;
  TCCR0A_COM0B0 = 0;
  //
  set_freq_divider(0);
}

void irs::avr::OC0B_pwm_t::set_duty(irs_uarc a_duty)
{
  if (a_duty > m_max_duty) a_duty = m_max_duty;
  OCR0B = a_duty;
}

void irs::avr::OC0B_pwm_t::set_duty(float a_duty)
{
  if (a_duty > 1.f) a_duty = 1.f;
  if (a_duty < 0.f) a_duty = 0.f;
  OCR0B = irs_uarc(a_duty * float(m_max_duty));
}

irs::cpu_traits_t::frequency_type irs::avr::OC0B_pwm_t::set_frequency(
  irs::cpu_traits_t::frequency_type a_frequency)
{
  irs_uarc div = 1;
  if (a_frequency < m_max_frequency) div = m_max_frequency / a_frequency;
  div = set_freq_divider(div);
  m_frequency = m_max_frequency / div;
  return m_frequency;
}

irs_uarc irs::avr::OC0B_pwm_t::get_max_duty()
{
  return m_max_duty;
}

irs::cpu_traits_t::frequency_type irs::avr::OC0B_pwm_t::get_max_frequency()
{
  return m_max_frequency;
}

//------------------------------------------------------------------------------

irs::avr::OC2A_pwm_t::OC2A_pwm_t(irs_uarc a_init_duty):
  m_max_duty(255),
  m_max_frequency(cpu_traits_t::frequency()/(m_max_duty + 1)),
  m_frequency(m_max_frequency)
{
  //  output pin = OC2A = PB4 (17)
  DDRB_DDB4 = 1;
  PORTB_PORTB4 = 0;
  //  Clear OC2A on compare match, set on TOP
  TCCR2A_COM2A1 = 0;
  TCCR2A_COM2A0 = 0;
  //  Fast PWM, TOP = 0xFF
  TCCR2B_WGM22 = 0;
  TCCR2A_WGM21 = 1;
  TCCR2A_WGM20 = 1;
  set_freq_divider(0);
  //  Timer/Counter register
  TCNT2 = 0;
  //  Output compare registers
  if (a_init_duty > m_max_duty) a_init_duty = m_max_duty;
  OCR2A = a_init_duty;
}

irs_uarc irs::avr::OC2A_pwm_t::set_freq_divider(irs_uarc a_divider)
{
  if (a_divider >= 1024)
  {
    TCCR2B_CS22 = 1;
    TCCR2B_CS21 = 1;
    TCCR2B_CS20 = 1;
    return 1024;
  }
  if (a_divider >= 256)
  {
    TCCR2B_CS22 = 1;
    TCCR2B_CS21 = 1;
    TCCR2B_CS20 = 0;
    return 256;
  }
  if (a_divider >= 128)
  {
    TCCR2B_CS22 = 1;
    TCCR2B_CS21 = 0;
    TCCR2B_CS20 = 1;
    return 1024;
  }
  if (a_divider >= 64)
  {
    TCCR2B_CS22 = 1;
    TCCR2B_CS21 = 0;
    TCCR2B_CS20 = 0;
    return 256;
  }
  if (a_divider >= 32)
  {
    TCCR2B_CS22 = 0;
    TCCR2B_CS21 = 1;
    TCCR2B_CS20 = 1;
    return 64;
  }
  if (a_divider >= 8)
  {
    TCCR2B_CS22 = 0;
    TCCR2B_CS21 = 1;
    TCCR2B_CS20 = 0;
    return 8;
  }
  if (a_divider >= 1)
  {
    TCCR2B_CS22 = 0;
    TCCR2B_CS21 = 0;
    TCCR2B_CS20 = 1;
    return 1;
  }
  TCCR2B_CS22 = 0;
  TCCR2B_CS21 = 0;
  TCCR2B_CS20 = 0;
  return 0;
}

irs::avr::OC2A_pwm_t::~OC2A_pwm_t()
{
  //  output pin = OC0B = PG5
  DDRG_DDG5 = 0;
  PORTG_PORTG5 = 0;
  //  Clear OC0B on compare match, set on TOP
  TCCR0A_COM0B1 = 0;
  TCCR0A_COM0B0 = 0;
  // Clock source
  set_freq_divider(0);
  //  Output compare registers
  OCR0B = 0;
}

void irs::avr::OC2A_pwm_t::start()
{
  //  output pin = OC2A = PB4 (17)
  DDRB_DDB4 = 1;
  PORTB_PORTB4 = 0;
  //  Clear OC2A on compare match, set on TOP
  TCCR2A_COM2A1 = 1;
  TCCR2A_COM2A0 = 0;
  //
  set_freq_divider(m_max_frequency / m_frequency);
}

void irs::avr::OC2A_pwm_t::stop()
{
  //  output pin = OC2A = PB4 (17)
  DDRB_DDB4 = 1;
  PORTB_PORTB4 = 0;
  //  Clear OC2A on compare match, set on TOP
  TCCR2A_COM2A1 = 0;
  TCCR2A_COM2A0 = 0;
  //
  set_freq_divider(0);
}

void irs::avr::OC2A_pwm_t::set_duty(irs_uarc a_duty)
{
  if (a_duty > m_max_duty) a_duty = m_max_duty;
  OCR2A = a_duty;
}

void irs::avr::OC2A_pwm_t::set_duty(float a_duty)
{
  if (a_duty > 1.f) a_duty = 1.f;
  if (a_duty < 0.f) a_duty = 0.f;
  OCR2A = irs_uarc(a_duty * float(m_max_duty));
}

irs::cpu_traits_t::frequency_type irs::avr::OC2A_pwm_t::set_frequency(
  irs::cpu_traits_t::frequency_type a_frequency)
{
  irs_uarc div = 1;
  if (a_frequency < m_max_frequency) div = m_max_frequency / a_frequency;
  div = set_freq_divider(div);
  m_frequency = m_max_frequency / div;
  return m_frequency;
}

irs_uarc irs::avr::OC2A_pwm_t::get_max_duty()
{
  return m_max_duty;
}

irs::cpu_traits_t::frequency_type irs::avr::OC2A_pwm_t::get_max_frequency()
{
  return m_max_frequency;
}

//------------------------------------------------------------------------------

irs::avr::OC1AB_pwm_t::OC1AB_pwm_t(irs_uarc a_init_duty):
  m_max_duty(255),
  m_max_frequency(cpu_traits_t::frequency()/(m_max_duty + 1)),
  m_frequency(m_max_frequency)
{
  //  output pin = OC1A and OC1B = PB5, PB6 (15, 16)
  DDRB_DDB5 = 1;
  PORTB_PORTB5 = 0;
  DDRB_DDB6 = 1;
  PORTB_PORTB6 = 0;
  //  Clear OC1A on compare match, set on TOP
  TCCR1A_COM1A1 = 0;
  TCCR1A_COM1A0 = 0;
  //  Set OC1B on compare match, clear on TOP
  TCCR1A_COM1B1 = 0;
  TCCR1A_COM1B0 = 0;
  //  Fast PWM 8 bit, TOP = 0xFF
  TCCR1B_WGM13 = 0;
  TCCR1B_WGM12 = 1;
  TCCR1A_WGM11 = 0;
  TCCR1A_WGM10 = 1;
  // Clock source = fin / 64
  set_freq_divider(0);
  //  Timer/Counter register
  TCNT1 = 0;
  //  Output compare registers
  if (a_init_duty > m_max_duty) a_init_duty = m_max_duty;
  OCR1A = a_init_duty;
  OCR1B = a_init_duty;
}

irs_uarc irs::avr::OC1AB_pwm_t::set_freq_divider(irs_uarc a_divider)
{
  if (a_divider >= 1024)
  {
    TCCR1B_CS12 = 1;
    TCCR1B_CS11 = 0;
    TCCR1B_CS10 = 1;
    return 1024;
  }
  if (a_divider >= 256)
  {
    TCCR1B_CS12 = 1;
    TCCR1B_CS11 = 0;
    TCCR1B_CS10 = 0;
    return 256;
  }
  if (a_divider >= 64)
  {
    TCCR1B_CS12 = 0;
    TCCR1B_CS11 = 1;
    TCCR1B_CS10 = 1;
    return 64;
  }
  if (a_divider >= 8)
  {
    TCCR1B_CS12 = 0;
    TCCR1B_CS11 = 1;
    TCCR1B_CS10 = 0;
    return 8;
  }
  if (a_divider >= 1)
  {
    TCCR1B_CS12 = 0;
    TCCR1B_CS11 = 0;
    TCCR1B_CS10 = 1;
    return 1;
  }
  TCCR1B_CS12 = 0;
  TCCR1B_CS11 = 0;
  TCCR1B_CS10 = 0;
  return 0;
}

irs::avr::OC1AB_pwm_t::~OC1AB_pwm_t()
{
  //  output pin = OC1A and OC1B = PB5, PB6 (15, 16)
  DDRB_DDB5 = 0;
  PORTB_PORTB5 = 0;
  DDRB_DDB6 = 0;
  PORTB_PORTB6 = 0;
  //  Clear OC1A on compare match, set on TOP
  TCCR1A_COM1A1 = 0;
  TCCR1A_COM1A0 = 0;
  //  Set OC1B on compare match, clear on TOP
  TCCR1A_COM1B1 = 0;
  TCCR1A_COM1B0 = 0;
  //  Timer/Counter register
  TCNT1 = 0;
  //  Output compare registers
  OCR1A = 0;
  OCR1B = 0;
  set_freq_divider(0);
}

void irs::avr::OC1AB_pwm_t::start()
{
  //  output pin = OC1A and OC1B = PB5, PB6 (15, 16)
  DDRB_DDB5 = 1;
  PORTB_PORTB5 = 0;
  DDRB_DDB6 = 1;
  PORTB_PORTB6 = 0;
  //  Prescaler reset
  GTCCR_PSRSYNC = 1;
  //  Timer/Counter register
  TCNT1 = 0;
  irs_u8 zero = m_max_duty / 2;
  OCR1A = zero;
  OCR1B = zero;
  //  Clear OC1A on compare match, set on TOP
  TCCR1A_COM1A1 = 1;
  TCCR1A_COM1A0 = 0;
  //  Set OC1B on compare match, clear on TOP
  TCCR1A_COM1B1 = 1;
  TCCR1A_COM1B0 = 1;
  //
  set_freq_divider(m_max_frequency / m_frequency);
}

void irs::avr::OC1AB_pwm_t::stop()
{
  //  output pin = OC1A and OC1B = PB5, PB6 (15, 16)
  DDRB_DDB5 = 1;
  PORTB_PORTB5 = 0;
  DDRB_DDB6 = 1;
  PORTB_PORTB6 = 0;
  //  Clear OC1A on compare match, set on TOP
  TCCR1A_COM1A1 = 0;
  TCCR1A_COM1A0 = 0;
  //  Set OC1B on compare match, clear on TOP
  TCCR1A_COM1B1 = 0;
  TCCR1A_COM1B0 = 0;
  //
  set_freq_divider(0);
}

void irs::avr::OC1AB_pwm_t::set_duty(irs_uarc a_duty)
{
  if (a_duty > m_max_duty) a_duty = m_max_duty;
  OCR1A = a_duty;
  OCR1B = a_duty;
}

void irs::avr::OC1AB_pwm_t::set_duty(float a_duty)
{
  if (a_duty > 1.f) a_duty = 1.f;
  if (a_duty < 0.f) a_duty = 0.f;
  OCR1A = irs_uarc(a_duty * float(m_max_duty));
  OCR1B = irs_uarc(a_duty * float(m_max_duty));
}

irs::cpu_traits_t::frequency_type irs::avr::OC1AB_pwm_t::set_frequency(
  irs::cpu_traits_t::frequency_type a_frequency)
{
  irs_uarc div = 1;
  if (a_frequency < m_max_frequency) div = m_max_frequency / a_frequency;
  div = set_freq_divider(div);
  m_frequency = m_max_frequency / div;
  return m_frequency;
}

irs_uarc irs::avr::OC1AB_pwm_t::get_max_duty()
{
  return m_max_duty;
}

irs::cpu_traits_t::frequency_type irs::avr::OC1AB_pwm_t::get_max_frequency()
{
  return m_max_frequency;
}

//------------------------------------------------------------------------------
#endif //__ATmega128__
