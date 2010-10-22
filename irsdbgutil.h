//! \file
//! \ingroup system_utils_group
//! \brief Утилиты для отладки программы
//!
//! Дата: 23.09.2010
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

//! @}

} //namespace irs

#endif //IRSDBGUTILH
