// Утилиты для отладки программы
// Дата: 09.04.2010
// Дата создания: 09.04.2010

#ifndef IRSDBGUTILH
#define IRSDBGUTILH

#include <irsdefs.h>

#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

enum mc_range_param_ident_avr_t {
  mcrpi_avr_return_stack,
  mcrpi_avr_call_stack,
  mcrpi_avr_heap,
  mcrpi_avr_size
};

// Базовый класс для проверки памяти
class memory_checker_t
{
public:
  typedef int ident_type;
  typedef irs_size_t value_type;
  
  virtual ~memory_checker_t();
  virtual irs_size_t range_param_begin(ident_type a_ident) = 0;
  virtual void range_param_begin(ident_type a_ident,
    value_type a_value) = 0;
  virtual value_type range_param_current(ident_type a_ident) = 0;
  virtual void range_param_current(ident_type a_ident,
    value_type a_value) = 0;
  virtual value_type range_param_end(ident_type a_ident) = 0;
  virtual void range_param_end(ident_type a_ident,
    value_type a_value) = 0;
  virtual value_type range_param_size(ident_type a_ident) = 0;
  virtual value_type range_param_cur_size(ident_type a_ident) = 0;
  virtual value_type range_param_rest(ident_type a_ident) = 0;
  virtual void out_info(ostream* ap_strm) = 0;
  virtual void check() = 0;
};

memory_checker_t* memory_checker();
  
} //namespace irs

#endif //IRSDBGUTILH
