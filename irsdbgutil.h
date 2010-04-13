// ������� ��� ������� ���������
// ����: 12.04.2010
// ���� ��������: 09.04.2010

#ifndef IRSDBGUTILH
#define IRSDBGUTILH

#include <irsdefs.h>

#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

enum mc_range_param_ident_avr_t {
  mcrpi_avr_call_stack,
  mcrpi_avr_heap,
  mcrpi_avr_return_stack,
  mcrpi_avr_size
};

// ������� ����� ��� �������� ������
class memory_checker_t
{
public:
  typedef int ident_type;
  typedef irs_size_t value_type;
  
  enum {
    heap_array_size_def = 10
  };
  
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
  
} //namespace irs

#endif //IRSDBGUTILH
