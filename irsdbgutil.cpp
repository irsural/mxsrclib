// Утилиты для отладки программы
// Дата: 09.04.2010
// Дата создания: 09.04.2010

#include <irsdefs.h>

#include <irsdbgutil.h>
#include <irserror.h>
#include <irsstrm.h>

#include <irsfinal.h>

namespace irs {

// Класс по умолчанию для проверки памяти. Пока пустой
class default_memory_checker_t: public memory_checker_t
{
public:
  default_memory_checker_t();
  virtual value_type range_param_begin(ident_type a_ident);
  virtual void range_param_begin(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_current(ident_type a_ident);
  virtual void range_param_current(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_end(ident_type a_ident);
  virtual void range_param_end(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_size(ident_type a_ident);
  virtual value_type range_param_cur_size(ident_type a_ident);
  virtual value_type range_param_rest(ident_type a_ident);
  virtual void out_info(ostream* ap_strm);
  virtual void check();
private:
};

struct memory_checker_param_t
{
  typedef memory_checker_t::value_type value_type;
  
  value_type begin;
  value_type current;
  value_type end;
  
  memory_checker_param_t():
    begin(0),
    current(0),
    end(0)
  {
  }
  memory_checker_param_t(value_type a_begin, value_type a_current,
    value_type a_end
  ):
    begin(a_begin),
    current(a_current),
    end(a_end)
  {
  }
};
  

#ifdef __ICCAVR__
// Проверка памяти для AVR
class avr_memory_checker_t: public memory_checker_t
{
public:
  typedef avr_memory_checker_t this_type;
  typedef value_type (this_type::*range_param_get_method_type)(
    ident_type a_ident);
  struct var_item_type
  {
    memory_checker_param_t param;
    range_param_get_method_type cur_size_method;
    range_param_get_method_type rest_method;
    
    var_item_type():
      param(memory_checker_param_t()),
      cur_size_method(range_param_get_method_type()),
      rest_method(range_param_get_method_type())
    {
    }
    var_item_type(
      memory_checker_param_t a_param,
      range_param_get_method_type a_cur_size_method,
      range_param_get_method_type a_rest_method
    ):
      param(a_param),
      cur_size_method(a_cur_size_method),
      rest_method(a_rest_method)
    {
    }
  };
  
  avr_memory_checker_t();
  virtual value_type range_param_begin(ident_type a_ident);
  virtual void range_param_begin(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_current(ident_type a_ident);
  virtual void range_param_current(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_end(ident_type a_ident);
  virtual void range_param_end(ident_type a_ident,
    value_type a_value);
  virtual value_type range_param_size(ident_type a_ident);
  virtual value_type range_param_cur_size(ident_type a_ident);
  virtual value_type range_param_rest(ident_type a_ident);
  virtual void out_info(ostream* ap_strm);
  virtual void check();
private:
  vector<var_item_type> m_var_list;
  vector<char IRS_ICCAVR_FLASH*> m_ident_name_list;
  
  value_type first_delta(ident_type a_ident);
  value_type second_delta(ident_type a_ident);
  void out_param(ostream* ap_strm, ident_type a_ident);
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
  ident_type /*a_ident*/)
{
  return 0;
}
void irs::default_memory_checker_t::range_param_begin(
  ident_type /*a_ident*/, value_type /*a_value*/)
{
}
irs::default_memory_checker_t::value_type 
  irs::default_memory_checker_t::range_param_current(
  ident_type /*a_ident*/)
{
  return 0;
}
void irs::default_memory_checker_t::range_param_current(
  ident_type /*a_ident*/, value_type /*a_value*/)
{
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_end(
  ident_type /*a_ident*/)
{
  return 0;
}
void irs::default_memory_checker_t::range_param_end(
  ident_type /*a_ident*/, value_type /*a_value*/)
{
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_size(
  ident_type /*a_ident*/)
{
  return 0;
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_cur_size(
  ident_type /*a_ident*/)
{
  return 0;
}
irs::default_memory_checker_t::value_type
  irs::default_memory_checker_t::range_param_rest(
  ident_type /*a_ident*/)
{
  return 0;
}
void irs::default_memory_checker_t::out_info(ostream* /*ap_strm*/)
{
}
void irs::default_memory_checker_t::check()
{
}

#ifdef __ICCAVR__
// Проверка памяти для AVR
irs::avr_memory_checker_t::avr_memory_checker_t():
  m_var_list(mcrpi_avr_size),
  m_ident_name_list(mcrpi_avr_size)
{
  m_var_list[mcrpi_avr_return_stack] = var_item_type(
    memory_checker_param_t(), second_delta, first_delta);
  m_var_list[mcrpi_avr_call_stack] = var_item_type(
    memory_checker_param_t(), second_delta, first_delta);
  m_var_list[mcrpi_avr_heap] = var_item_type(
    memory_checker_param_t(), first_delta, second_delta);
  
  static char IRS_ICCAVR_FLASH return_stack_name[] = "return stack (RSTACK)";
  m_ident_name_list[mcrpi_avr_return_stack] = return_stack_name;
  static char IRS_ICCAVR_FLASH call_stack_name[] = "call stack (CSTACK)";
  m_ident_name_list[mcrpi_avr_call_stack] = call_stack_name;
  static char IRS_ICCAVR_FLASH heap_name[] = "heap";
  m_ident_name_list[mcrpi_avr_heap] = heap_name;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_begin(
  ident_type a_ident)
{
  return m_var_list[a_ident].param.begin;
}
void irs::avr_memory_checker_t::range_param_begin(
  ident_type a_ident, value_type a_value)
{
  m_var_list[a_ident].param.begin = a_value;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_current(
  ident_type a_ident)
{
  return m_var_list[a_ident].param.current;
}
void irs::avr_memory_checker_t::range_param_current(
  ident_type a_ident, value_type a_value)
{
  m_var_list[a_ident].param.current = a_value;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_end(
  ident_type a_ident)
{
  return m_var_list[a_ident].param.end;
}
void irs::avr_memory_checker_t::range_param_end(
  ident_type a_ident, value_type a_value)
{
  m_var_list[a_ident].param.end = a_value;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_size(
  ident_type a_ident)
{
  return m_var_list[a_ident].param.end - m_var_list[a_ident].param.begin;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_cur_size(
  ident_type a_ident)
{
  return (this->*m_var_list[a_ident].cur_size_method)(a_ident);
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::range_param_rest(
  ident_type a_ident)
{
  return (this->*m_var_list[a_ident].rest_method)(a_ident);
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::first_delta(
  irs::avr_memory_checker_t::ident_type a_ident)
{
  return m_var_list[a_ident].param.current - m_var_list[a_ident].param.begin;
}
irs::avr_memory_checker_t::value_type
  irs::avr_memory_checker_t::second_delta(ident_type a_ident)
{
  return m_var_list[a_ident].param.end - m_var_list[a_ident].param.current;
}
void irs::avr_memory_checker_t::out_param(ostream* ap_strm, ident_type a_ident)
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
void irs::avr_memory_checker_t::out_info(ostream* ap_strm)
{
  (*ap_strm) << endl;
  for (ident_type ident = 0; ident < mcrpi_avr_size; ++ident) {
    out_param(ap_strm, ident);
  }
  (*ap_strm) << endl;
}
void irs::avr_memory_checker_t::check()
{
  //irs_u8 call_stack_check_var = 0;
  m_var_list[mcrpi_avr_return_stack].param.current = 
    static_cast<value_type>(SP);
  m_var_list[mcrpi_avr_call_stack].param.current = 
    static_cast<value_type>(*reinterpret_cast<irs_u16*>(0x1C));
  auto_ptr<irs_u8> p_heap_check_var(IRS_LIB_NEW_ASSERT(new irs_u8));
  m_var_list[mcrpi_avr_heap].param.current = 
    reinterpret_cast<value_type>(p_heap_check_var.get());
  
  #ifdef NOP
  IRS_LIB_DBG_RAW_MSG(endl << endl << irsm("************* Y = "));
  IRS_LIB_DBG_RAW_MSG((*(irs_u16*)0x1C) << endl << endl);
  IRS_LIB_DBG_RAW_MSG(endl << endl << irsm("************* SP = "));
  IRS_LIB_DBG_RAW_MSG(SP << endl << endl);
  #endif //NOP
}
#endif //__ICCAVR__

irs::memory_checker_t* irs::memory_checker()
{
  #ifdef __ICCAVR__
  static avr_memory_checker_t memory_checker_obj;
  #else //__ICCAVR__
  static default_memory_checker_t memory_checker_obj;
  #endif //__ICCAVR__
  return &memory_checker_obj;
}
