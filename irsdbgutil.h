//! \file
//! \ingroup system_utils_group
//! \brief Утилиты для отладки программы
//!
//! Дата: 18.10.2010
//! Дата создания: 09.04.2010

#ifndef IRSDBGUTILH
#define IRSDBGUTILH

// Номер файла
#define IRSDBGUTILH_IDX 4

#include <irsdefs.h>

#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup system_utils_group
//! @{

enum mc_range_param_ident_avr_t {
  mcrpi_avr_call_stack,
  mcrpi_avr_heap,
  mcrpi_avr_return_stack,
  mcrpi_avr_size
};

enum mc_param_ident_avr_t {
  mcpi_avr_interrupt,
  mcpi_avr_out_time_s,
  mcpi_avr_size
};

enum mc_param_avr_interrupt_t {
  mcp_avr_interrupt_none,
  mcp_avr_interrupt_timer0,
  mcp_avr_interrupt_timer4
};

// Базовый класс для проверки памяти
class memory_checker_t
{
public:
  typedef int ident_type;
  typedef irs_size_t value_type;

  enum {
    heap_array_size_def = 10
  };

  virtual ~memory_checker_t();
  virtual irs_size_t range_param_begin(ident_type a_ident) const = 0;
  virtual void range_param_begin(ident_type a_ident,
    value_type a_value) = 0;
  virtual value_type range_param_current(ident_type a_ident) const = 0;
  virtual void range_param_current(ident_type a_ident,
    value_type a_value) = 0;
  virtual value_type range_param_end(ident_type a_ident) const = 0;
  virtual void range_param_end(ident_type a_ident,
    value_type a_value) = 0;
  virtual value_type range_param_size(ident_type a_ident) const = 0;
  virtual value_type range_param_cur_size(ident_type a_ident) const = 0;
  virtual value_type range_param_rest(ident_type a_ident) const = 0;
  virtual value_type param(ident_type a_ident) const = 0;
  virtual void param(ident_type a_ident, value_type a_value) = 0;
  virtual void heap_array_size(value_type a_size) = 0;
  virtual void out_info(ostream* ap_strm) = 0;
  virtual void check() = 0;
};

memory_checker_t* memory_checker();
memory_checker_t* memory_checker_init(
  memory_checker_t::value_type a_call_stack_begin,
  memory_checker_t::value_type a_call_stack_end,
  memory_checker_t::value_type a_heap_begin,
  memory_checker_t::value_type a_heap_end,
  memory_checker_t::value_type a_return_stack_begin,
  memory_checker_t::value_type a_return_stack_end,
  memory_checker_t::value_type a_heap_array_size =
    memory_checker_t::heap_array_size_def
);
memory_checker_t* memory_checker_avr_init(
  memory_checker_t::value_type a_call_stack_begin,
  memory_checker_t::value_type a_heap_begin,
  memory_checker_t::value_type a_return_stack_begin,
  memory_checker_t::value_type a_return_stack_end,
  memory_checker_t::value_type a_heap_array_size =
    memory_checker_t::heap_array_size_def
);
memory_checker_t* memory_checker_avr_init(
  memory_checker_t::value_type a_call_stack_size,
  memory_checker_t::value_type a_heap_size,
  memory_checker_t::value_type a_return_stack_size
);

#ifdef __ICCAVR__
namespace avr {

enum {
  #ifdef __ATmega128__
  call_stack_begin = 0x100,
  return_stack_multiplier = 2
  #else // __ATmega128__
  call_stack_begin = 0x200,
  return_stack_multiplier = 3
  #endif // __ATmega128__
};

size_t heap_pointer(size_t a_heap_array_size = 20);

// Порядок параметров как опциях проекта
template <size_t heap_size, size_t call_stack_size, size_t return_stack_size>
inline void memcheck(ostream* ap_stream)
{
  enum {
    call_stack_origin = call_stack_begin + call_stack_size,
    heap_begin = call_stack_origin,
    return_stack_origin = heap_begin + heap_size +
      return_stack_multiplier*return_stack_size
  };
  (*ap_stream )<< '\n';
  (*ap_stream )<< irsm("heap use: ") << heap_pointer() - heap_begin << '\n';
  irs_u16& reg_Y = *reinterpret_cast<irs_u16*>(0x1C);
  (*ap_stream )<< irsm("call stack use: ") << call_stack_origin - reg_Y << '\n';
  size_t return_stack_levels =
    (return_stack_origin - SP)/return_stack_multiplier;
  (*ap_stream )<< irsm("return stack use: ") << return_stack_levels << '\n';
  (*ap_stream )<< endl;
}

} //namespace avr
#endif //__ICCAVR__

//! @}

} //namespace irs

#endif //IRSDBGUTILH
