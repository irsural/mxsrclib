//! \file
//! \ingroup system_utils_group
//! \brief Утилиты для отладки программы
//!
//! Дата: 09.03.2011\n
//! Дата создания: 09.04.2010

// Номер файла
#define IRSDBGUTILCPP_IDX 3

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <timer.h>
#include <irserror.h>
#ifdef __ICCAVR__
#include <irsarchint.h>
#endif //__ICCAVR__

#include <irsdbgutil.h>

#include <irsfinal.h>

namespace irs {

// Класс по умолчанию для проверки памяти. Пока пустой
class default_memory_checker_t: public memory_checker_t
{
public:
  default_memory_checker_t();
  virtual value_type range_param_begin(ident_type a_ident) const;
  virtual void range_param_begin(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_current(ident_type a_ident) const;
  virtual void range_param_current(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_end(ident_type a_ident) const;
  virtual void range_param_end(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_size(ident_type a_ident) const;
  virtual value_type range_param_cur_size(ident_type a_ident) const;
  virtual value_type range_param_rest(ident_type a_ident) const;
  virtual value_type param(ident_type a_ident) const;
  virtual void param(ident_type a_ident, value_type a_value);
  virtual void heap_array_size(value_type a_size);
  virtual void out_info(ostream* ap_strm);
  virtual void check();
private:
};

#ifdef __ICCAVR__
// Проверка памяти для AVR
// На 23.04.2010 примерный расход памяти:
// HEAP: 96 байт; CSTACK: 74 байт; Статической: 101 байт
struct memory_checker_range_param_t
{
  typedef memory_checker_t::value_type value_type;

  value_type begin;
  value_type current;
  value_type end;

  memory_checker_range_param_t():
    begin(0),
    current(0),
    end(0)
  {
  }
  memory_checker_range_param_t(value_type a_begin, value_type a_current,
    value_type a_end
  ):
    begin(a_begin),
    current(a_current),
    end(a_end)
  {
  }
};

class avr_memory_checker_t;
struct memory_checker_traits_t
{
  typedef memory_checker_t::value_type
    (avr_memory_checker_t::*param_get_method_type)(
    memory_checker_t::ident_type a_ident) const;
  typedef void (avr_memory_checker_t::*param_set_method_type)(
    memory_checker_t::ident_type a_ident, memory_checker_t::value_type);
};
struct memory_checker_range_param_ext_t
{
  typedef memory_checker_traits_t::param_get_method_type
    range_param_get_method_type;

  memory_checker_range_param_t param;
  range_param_get_method_type cur_size_method;
  range_param_get_method_type rest_method;

  memory_checker_range_param_ext_t():
    param(memory_checker_range_param_t()),
    cur_size_method(range_param_get_method_type()),
    rest_method(range_param_get_method_type())
  {
  }
  memory_checker_range_param_ext_t(
    memory_checker_range_param_t a_param,
    range_param_get_method_type a_cur_size_method,
    range_param_get_method_type a_rest_method
  ):
    param(a_param),
    cur_size_method(a_cur_size_method),
    rest_method(a_rest_method)
  {
  }
};
struct memory_checker_param_t
{
  typedef memory_checker_t::value_type value_type;
  typedef memory_checker_traits_t::param_get_method_type
    param_get_method_type;
  typedef memory_checker_traits_t::param_set_method_type
    param_set_method_type;

  value_type param;
  param_get_method_type get_method;
  param_set_method_type set_method;

  memory_checker_param_t():
    param(),
    get_method(),
    set_method()
  {
  }
  memory_checker_param_t(
    value_type a_param,
    param_get_method_type a_get_method,
    param_set_method_type a_set_method
  ):
    param(a_param),
    get_method(a_get_method),
    set_method(a_set_method)
  {
  }
};
class avr_memory_checker_t: public memory_checker_t
{
public:
  typedef avr_memory_checker_t this_type;
  typedef memory_checker_range_param_ext_t range_param_type;
  typedef memory_checker_param_t param_type;

  avr_memory_checker_t(interrupt_flag_type a_interrupt_flag = interrupt_on);
  virtual value_type range_param_begin(ident_type a_ident) const;
  virtual void range_param_begin(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_current(ident_type a_ident) const;
  virtual void range_param_current(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_end(ident_type a_ident) const;
  virtual void range_param_end(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_size(ident_type a_ident) const;
  virtual value_type range_param_cur_size(ident_type a_ident) const;
  virtual value_type range_param_rest(ident_type a_ident) const;
  virtual value_type param(ident_type a_ident) const;
  virtual void param(ident_type a_ident, value_type a_value);
  virtual void heap_array_size(value_type a_size);
  virtual void out_info(ostream* ap_strm);
  virtual void check();
private:
  enum {
    m_heap_array_max_size = 4096,
    m_out_time_s_def = 10
  };

  static char const IRS_ICCAVR_FLASH m_call_stack_ident_name[];
  static char const IRS_ICCAVR_FLASH m_heap_ident_name[];
  static char const IRS_ICCAVR_FLASH m_return_stack_ident_name[];
  static IRS_ICCAVR_FLASH char const IRS_ICCAVR_FLASH* const
    m_ident_name_list[];

  vector<range_param_type> m_range_param_list;
  vector<param_type> m_param_list;
  value_type m_heap_array_size;
  event_connect_t<this_type> m_check_event;
  loop_timer_t m_out_timer;
  irs_u16& m_reg_Y;

  value_type first_delta(ident_type a_ident) const;
  value_type second_delta(ident_type a_ident) const;
  void out_param(ostream* ap_strm, ident_type a_ident) const;
  void select_max(ident_type a_ident, irs_u16 a_value);
  void timer0_init();
  void timer0_deinit();
  void timer4_init();
  void timer4_deinit();

  value_type simple_param_get(ident_type a_ident) const;
  void simple_param_set(ident_type a_ident, value_type a_value);
  void interrupt_set(ident_type a_ident, value_type a_value);
  void out_time_set(ident_type a_ident, value_type a_value);
  value_type heap_pointer();
};
#endif //__ICCAVR__

} //namespace irs

// Базовый класс для проверки памяти
irs::memory_checker_t::~memory_checker_t()
{
}

// Класс по умолчанию для проверки памяти. Пока пустой
irs::default_memory_checker_t::default_memory_checker_t()
{
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_begin(
  ident_type /*a_ident*/) const
{
  return 0;
}
void irs::default_memory_checker_t::range_param_begin(
  ident_type /*a_ident*/, value_type /*a_value*/)
{
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_current(
  ident_type /*a_ident*/) const
{
  return 0;
}
void irs::default_memory_checker_t::range_param_current(
  ident_type /*a_ident*/, value_type /*a_value*/)
{
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_end(
  ident_type /*a_ident*/) const
{
  return 0;
}
void irs::default_memory_checker_t::range_param_end(
  ident_type /*a_ident*/, value_type /*a_value*/)
{
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_size(
  ident_type /*a_ident*/) const
{
  return 0;
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_cur_size(
  ident_type /*a_ident*/) const
{
  return 0;
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_rest(
  ident_type /*a_ident*/) const
{
  return 0;
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::param(ident_type /*a_ident*/) const
{
  return 0;
}
void irs::default_memory_checker_t::param(ident_type /*a_ident*/,
  value_type /*a_value*/)
{
}
void irs::default_memory_checker_t::heap_array_size(value_type /*a_size*/)
{
}
void irs::default_memory_checker_t::out_info(ostream* /*ap_strm*/)
{
}
void irs::default_memory_checker_t::check()
{
}

#ifdef __ICCAVR__
// Проверка памяти для AVR
char const IRS_ICCAVR_FLASH irs::avr_memory_checker_t::
  m_call_stack_ident_name[] = "call stack (CSTACK)";
char const IRS_ICCAVR_FLASH irs::avr_memory_checker_t::
  m_heap_ident_name[] = "heap";
char const IRS_ICCAVR_FLASH irs::avr_memory_checker_t::
  m_return_stack_ident_name[] = "return stack (RSTACK)";
IRS_ICCAVR_FLASH char const IRS_ICCAVR_FLASH* const
  irs::avr_memory_checker_t::m_ident_name_list[] =
{
  m_call_stack_ident_name,
  m_heap_ident_name,
  m_return_stack_ident_name
};
irs::avr_memory_checker_t::avr_memory_checker_t(
  interrupt_flag_type a_interrupt_flag
):
  m_range_param_list(mcrpi_avr_size),
  m_param_list(mcpi_avr_size),
  m_heap_array_size(heap_array_size_def),
  m_check_event(this, check),
  m_out_timer(make_cnt_s(m_out_time_s_def)),
  m_reg_Y(*reinterpret_cast<irs_u16*>(0x1C))
{
  lock_interrupt_t lock_interrupt;

  m_range_param_list[mcrpi_avr_return_stack] = range_param_type(
    memory_checker_range_param_t(), second_delta, first_delta);
  m_range_param_list[mcrpi_avr_call_stack] = range_param_type(
    memory_checker_range_param_t(), second_delta, first_delta);
  m_range_param_list[mcrpi_avr_heap] = range_param_type(
    memory_checker_range_param_t(), first_delta, second_delta);

  if (a_interrupt_flag == interrupt_on) {
    m_param_list[mcpi_avr_interrupt] = param_type(mcp_avr_interrupt_timer0,
      simple_param_get, interrupt_set);
  }
  m_param_list[mcpi_avr_out_time_s] = param_type(m_out_time_s_def,
    simple_param_get, out_time_set);

  m_range_param_list[mcrpi_avr_call_stack].param.current = m_reg_Y;
  m_range_param_list[mcrpi_avr_heap].param.current = heap_pointer();
  m_range_param_list[mcrpi_avr_return_stack].param.current = SP;

  if (m_param_list[mcpi_avr_interrupt].param == mcp_avr_interrupt_timer0) {
    timer0_init();
  }
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_begin(
  ident_type a_ident) const
{
  return m_range_param_list[a_ident].param.begin;
}
void irs::avr_memory_checker_t::range_param_begin(
  ident_type a_ident, value_type a_value)
{
  m_range_param_list[a_ident].param.begin = a_value;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_current(
  ident_type a_ident) const
{
  return m_range_param_list[a_ident].param.current;
}
void irs::avr_memory_checker_t::range_param_current(
  ident_type a_ident, value_type a_value)
{
  m_range_param_list[a_ident].param.current = a_value;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_end(
  ident_type a_ident) const
{
  return m_range_param_list[a_ident].param.end;
}
void irs::avr_memory_checker_t::range_param_end(
  ident_type a_ident, value_type a_value)
{
  m_range_param_list[a_ident].param.end = a_value;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_size(
  ident_type a_ident) const
{
  return m_range_param_list[a_ident].param.end -
    m_range_param_list[a_ident].param.begin;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_cur_size(
  ident_type a_ident) const
{
  return (this->*m_range_param_list[a_ident].cur_size_method)(a_ident);
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_rest(
  ident_type a_ident) const
{
  return (this->*m_range_param_list[a_ident].rest_method)(a_ident);
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::first_delta(
  irs::avr_memory_checker_t::ident_type a_ident) const
{
  return m_range_param_list[a_ident].param.current -
    m_range_param_list[a_ident].param.begin;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::second_delta(ident_type a_ident) const
{
  return m_range_param_list[a_ident].param.end -
    m_range_param_list[a_ident].param.current;
}
void irs::avr_memory_checker_t::out_param(ostream* ap_strm,
  ident_type a_ident) const
{
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), "--- ");
  (*ap_strm) << m_ident_name_list[a_ident] << endl;

  // begin
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), "begin: ");
  irs_size_t begin = range_param_begin(a_ident);
  (*ap_strm) << begin;
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), ", ");
  out_hex_0x(ap_strm, begin);
  (*ap_strm) << endl;

  // current
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), "current: ");
  irs_size_t current = range_param_current(a_ident);
  (*ap_strm) << current;
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), ", ");
  out_hex_0x(ap_strm, current);
  (*ap_strm) << endl;

  // end
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), "end: ");
  irs_size_t end = range_param_end(a_ident);
  (*ap_strm) << end;
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), ", ");
  out_hex_0x(ap_strm, end);
  (*ap_strm) << endl;

  // size
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), "size: ");
  irs_size_t size = range_param_size(a_ident);
  (*ap_strm) << size;
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), ", ");
  out_hex_0x(ap_strm, size);
  (*ap_strm) << endl;

  // cur_size
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), "cur_size: ");
  irs_size_t cur_size = range_param_cur_size(a_ident);
  (*ap_strm) << cur_size;
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), ", ");
  out_hex_0x(ap_strm, cur_size);
  (*ap_strm) << endl;

  // rest
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), "rest: ");
  irs_size_t rest = range_param_rest(a_ident);
  (*ap_strm) << rest;
  IRS_STRM_ICCAVR_FLASH_OUT((*ap_strm), ", ");
  out_hex_0x(ap_strm, rest);
  (*ap_strm) << endl;
}
irs::avr_memory_checker_t::value_type irs::avr_memory_checker_t::param(
  ident_type a_ident) const
{
  return (this->*m_param_list[a_ident].get_method)(a_ident);
}
void irs::avr_memory_checker_t::param(ident_type a_ident, value_type a_value)
{
  (this->*m_param_list[a_ident].set_method)(a_ident, a_value);
}
void irs::avr_memory_checker_t::heap_array_size(value_type a_size)
{
  IRS_LIB_ASSERT((a_size != 0) && (a_size < m_heap_array_max_size));
  m_heap_array_size = a_size;
}
void irs::avr_memory_checker_t::out_info(ostream* ap_strm)
{
  if (m_out_timer.check() || (m_out_timer.get() == 0)) {
    (*ap_strm) << endl;
    for (ident_type ident = 0; ident < mcrpi_avr_size; ++ident) {
      out_param(ap_strm, ident);
    }
    (*ap_strm) << endl;
  }
}
void irs::avr_memory_checker_t::select_max(ident_type a_ident,
  irs_u16 a_value)
{
  value_type& param_current = m_range_param_list[a_ident].param.current;
  value_type param_current_prev = param_current;
  value_type param_cur_size_prev = range_param_cur_size(a_ident);
  param_current = static_cast<value_type>(a_value);
  value_type param_cur_size = range_param_cur_size(a_ident);

  #ifdef IRS_LIB_DEBUG
  if (param_cur_size >= range_param_size(a_ident)) {
    lock_interrupt_t lock_interrupt;
    IRS_LIB_DBG_RAW_MSG(irsm("\n\n"));
    IRS_LIB_DBG_RAW_MSG(m_ident_name_list[a_ident]);
    IRS_LIB_DBG_RAW_MSG(irsm(" overflow!!!") << endl);
    out_param(&mlog(), a_ident);
    bool range_param_is_overflow =
      (param_cur_size < range_param_size(a_ident));
    IRS_LIB_ASSERT(range_param_is_overflow);
  }
  #endif //IRS_LIB_DEBUG

  if (param_cur_size_prev > param_cur_size) {
    param_current = param_current_prev;
  }
}
void irs::avr_memory_checker_t::timer0_init()
{
  irs::avr::interrupt_array()->int_event_gen(irs::avr::timer0_ovf_int)->
    add(&m_check_event);

  #ifdef __ATmega128__
  TCCR0 = (0 << FOC0)|(0 << WGM01)|(0 << COM01)|(0 << COM00)|(0 << WGM00)|
    (1 << CS02)|(1 << CS01)|(1 << CS00);
  OCR0 = 0xFF;
  TCNT0 = 0x0;
  TIMSK_TOIE0 = 1;
  #else //__ATmega128__
  TCCR0A = (0 << COM0A1)|(0 << COM0A0)|(0 << COM0B1)|(0 << COM0B0)|
    (0 << 3)|(0 << 2)|(0 << WGM01)|(0 << WGM00);
  TCCR0B = (0 << FOC0A)|(0 << FOC0B)|(0 << 5)|(0 << 4)|
    (0 << WGM02)|(1 << CS02)|(0 << CS01)|(1 << CS00);
  //TCCR0B = (0 << FOC0A)|(0 << FOC0B)|(0 << 5)|(0 << 4)|
    //(0 << WGM02)|(0 << CS02)|(1 << CS01)|(1 << CS00);
  TCNT0 = 0x0;
  OCR0A = 0xFF;
  OCR0B = 0xFF;
  TIMSK0_TOIE0 = 1;
  #endif //__ATmega128__
}
void irs::avr_memory_checker_t::timer0_deinit()
{
  #ifdef __ATmega128__
  TCCR0 = (0 << FOC0)|(0 << WGM01)|(0 << COM01)|(0 << COM00)|(0 << WGM00)|
    (0 << CS02)|(0 << CS01)|(0 << CS00);
  TIMSK_TOIE0 = 0;
  #else //__ATmega128__
  TCCR0A = (0 << COM0A1)|(0 << COM0A0)|(0 << COM0B1)|(0 << COM0B0)|
    (0 << 3)|(0 << 2)|(0 << WGM01)|(0 << WGM00);
  TCCR0B = (0 << FOC0A)|(0 << FOC0B)|(0 << 5)|(0 << 4)|
    (0 << WGM02)|(0 << CS02)|(0 << CS01)|(0 << CS00);
  TIMSK0_TOIE0 = 0;
  #endif //__ATmega128__
};
void irs::avr_memory_checker_t::timer4_init()
{
  #ifdef __ATmega2561__
  irs::avr::interrupt_array()->int_event_gen(irs::avr::timer4_ovf_int)->
    add(&m_check_event);

  TCCR4A_COM4A1 = 0;  //  Выходы отключены
  TCCR4A_COM4A0 = 0;
  TCCR4A_COM4B1 = 0;
  TCCR4A_COM4B0 = 0;
  TCCR4A_COM4C1 = 0;
  TCCR4A_COM4C0 = 0;

  TCCR4B_WGM43 = 0;   //  Fast PWM 8-bit
  TCCR4B_WGM42 = 1;
  TCCR4A_WGM41 = 0;
  TCCR4A_WGM40 = 1;

  TCCR4B_ICNC4 = 0;   //  Бесполезное
  TCCR4B_ICES4 = 0;
  TCCR4C = 0;

  TCCR4B_CS42 = 1;    //  Делитель на 1024
  TCCR4B_CS41 = 0;
  TCCR4B_CS40 = 1;

  TCNT4 = 0;
  OCR4A = 0;
  OCR4B = 0;
  OCR4C = 0;
  ICR4 = 0;
  TIMSK4 = 0;
  TIMSK4_TOIE4 = 1;   //  Прерывание по переполнению
  TIFR4_TOV4 = 1;     //  Сброс флага прерывания

  #else //__ATmega2561__
  IRS_LIB_ASSERT_MSG(irsm("Неподдерживаемый контроллер!"));
  #endif //__ATmega2561__
}
void irs::avr_memory_checker_t::timer4_deinit()
{
  #ifdef __ATmega2561__
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4 = 0;
  TIMSK4 = 0;
  #else //__ATmega2561__
  IRS_LIB_ASSERT_MSG(irsm("Неподдерживаемый контроллер!"));
  #endif //__ATmega2561__
};
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::simple_param_get(ident_type a_ident) const
{
  return m_param_list[a_ident].param;
}
void irs::avr_memory_checker_t::simple_param_set(ident_type a_ident,
  value_type a_value)
{
  m_param_list[a_ident].param = a_value;
}
void irs::avr_memory_checker_t::interrupt_set(ident_type a_ident,
  value_type a_value)
{
  value_type& interrupt_param = m_param_list[a_ident].param;
  interrupt_param = a_value;
  switch (interrupt_param) {
    case mcp_avr_interrupt_none: {
      timer0_deinit();
      timer4_deinit();
      break;
    }
    case mcp_avr_interrupt_timer0: {
      timer0_init();
      break;
    }
    case mcp_avr_interrupt_timer4: {
      timer4_init();
      break;
    }
  };
}
void irs::avr_memory_checker_t::out_time_set(ident_type a_ident,
  value_type a_value)
{
  m_param_list[a_ident].param = a_value;
  m_out_timer.set(make_cnt_s(static_cast<int>(a_value)));
  m_out_timer.start();
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::heap_pointer()
{
  auto_arr<irs_u8> p_heap_check_var(
    IRS_LIB_NEW_ASSERT(irs_u8[m_heap_array_size], IRSDBGUTILCPP_IDX));
  return reinterpret_cast<value_type>(p_heap_check_var.get());
}
void irs::avr_memory_checker_t::check()
{
  lock_interrupt_t lock_interrupt;

  select_max(mcrpi_avr_return_stack, SP);
  select_max(mcrpi_avr_call_stack, m_reg_Y);
  select_max(mcrpi_avr_heap, heap_pointer());
}
#endif //__ICCAVR__

irs::memory_checker_t* irs::memory_checker()
{
  #ifdef __ICCAVR__
  static avr_memory_checker_t memory_checker_obj(
    memory_checker_t::interrupt_off);
  #else //__ICCAVR__
  static default_memory_checker_t memory_checker_obj;
  #endif //__ICCAVR__
  return &memory_checker_obj;
}

irs::memory_checker_t* irs::memory_checker_init(
  memory_checker_t::value_type a_call_stack_begin,
  memory_checker_t::value_type a_call_stack_end,
  memory_checker_t::value_type a_heap_begin,
  memory_checker_t::value_type a_heap_end,
  memory_checker_t::value_type a_return_stack_begin,
  memory_checker_t::value_type a_return_stack_end,
  memory_checker_t::value_type a_heap_array_size
)
{
  irs::memory_checker()->range_param_begin(irs::mcrpi_avr_call_stack,
    a_call_stack_begin);
  irs::memory_checker()->range_param_end(irs::mcrpi_avr_call_stack,
    a_call_stack_end);
  irs::memory_checker()->range_param_begin(irs::mcrpi_avr_heap,
    a_heap_begin);
  irs::memory_checker()->range_param_end(irs::mcrpi_avr_heap,
    a_heap_end);
  irs::memory_checker()->range_param_begin(irs::mcrpi_avr_return_stack,
    a_return_stack_begin);
  irs::memory_checker()->range_param_end(irs::mcrpi_avr_return_stack,
    a_return_stack_end);
  irs::memory_checker()->heap_array_size(a_heap_array_size);
  return irs::memory_checker();
}

#ifdef __ICCAVR__
irs::memory_checker_t* irs::memory_checker_avr_init(
  memory_checker_t::value_type a_call_stack_begin,
  memory_checker_t::value_type a_heap_begin,
  memory_checker_t::value_type a_return_stack_begin,
  memory_checker_t::value_type a_return_stack_end,
  memory_checker_t::value_type a_heap_array_size,
  memory_checker_t::interrupt_flag_type a_interrupt_flag
)
{
  memory_checker_t* p_memory_checker =
    memory_checker_init(a_call_stack_begin, a_heap_begin, a_heap_begin,
    a_return_stack_begin, a_return_stack_begin, a_return_stack_end,
    a_heap_array_size);
  if (a_interrupt_flag == memory_checker_t::interrupt_on) {
    p_memory_checker->
      param(irs::mcpi_avr_interrupt, irs::mcp_avr_interrupt_timer0);
  }
  return p_memory_checker;
}

irs::memory_checker_t* irs::memory_checker_avr_init(
  memory_checker_t::value_type a_call_stack_size,
  memory_checker_t::value_type a_heap_size,
  memory_checker_t::value_type a_return_stack_size,
  memory_checker_t::interrupt_flag_type a_interrupt_flag
)
{
  #ifdef __ATmega128__
  memory_checker_t::value_type call_stack_begin = 0x100;
  irs_u8 multiplier = 2;
  #else // __ATmega128__
  memory_checker_t::value_type call_stack_begin = 0x200;
  irs_u8 multiplier = 3;
  #endif // __ATmega128__

  memory_checker_t::value_type heap_begin = call_stack_begin +
    a_call_stack_size;
  return memory_checker_avr_init(call_stack_begin, heap_begin,
    heap_begin + a_heap_size, heap_begin + a_heap_size +
    a_return_stack_size*multiplier, a_interrupt_flag);
}

size_t irs::avr::heap_pointer(size_t a_heap_array_size)
{
  auto_arr<irs_u8> p_heap_check_var(
    IRS_LIB_NEW_ASSERT(irs_u8[a_heap_array_size], IRSDBGUTILCPP_IDX));
  return reinterpret_cast<size_t>(p_heap_check_var.get());
}
#endif //__ICCAVR__
