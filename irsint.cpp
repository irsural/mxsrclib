// Прерывания
// Дата: 2.09.2009

#include <irsint.h>
#include <irserror.h>
#ifdef __ICCAVR__
#include <irsarchint.h>
#endif //__ICCAVR__

namespace irs {

// В C++ Builder 4 mem_fun не переваривает функции возвращающие void
// Поэтому определяем собственный функциональный объект для
// функции irs::generator_events_t::exec
struct event_exec_fun_t
{
  void operator()(event_t *ap_event)
  {
    ap_event->exec();
  }
};

// Пустая реализация работы с прерываниями
class interrupt_array_empty_t: public interrupt_array_base_t
{
public:
  typedef irs_size_t size_type;

  interrupt_array_empty_t();
    
  virtual irs_int_event_gen_t* int_event_gen(size_type a_index);
  virtual void exec_event(size_type a_index);
private:
  irs_int_event_gen_t m_gen;
};

} //namespace irs

// Класс действий
irs_action_t::irs_action_t():
  fp_prev_action(IRS_NULL)
{
}
irs_action_t::~irs_action_t()
{
}
void irs_action_t::exec()
{
  if (fp_prev_action) fp_prev_action->exec();
}
void irs_action_t::connect(irs_action_t *&a_action)
{
  fp_prev_action = a_action;
  a_action = this;
}

//#ifdef NOP
// class generator_events_t
irs::generator_events_t::generator_events_t():
  m_events()
{
}

void irs::generator_events_t::exec()
{
  for_each(m_events.begin(), m_events.end(), event_exec_fun_t());
}
void irs::generator_events_t::push_back(event_t* ap_event)
{
  event_container_type::iterator it_event =
    find(m_events.begin(), m_events.end(), ap_event);
  if (it_event == m_events.end()) {
    m_events.push_back(ap_event);
  } else {
    // Объект уже добавлен
  }
}
void irs::generator_events_t::erase(event_t* ap_event)
{
  event_container_type::iterator it_event =
    find(m_events.begin(), m_events.end(), ap_event);
  IRS_LIB_ASSERT(it_event != m_events.end());
  if (it_event != m_events.end()) {
    m_events.erase(it_event);
  }
}
void irs::generator_events_t::clear()
{
  m_events.clear();
}

// Класс событий
irs::event_t::event_t():
  m_occurred(false)
{
}
irs::event_t::~event_t()
{
}
void irs::event_t::exec()
{
  m_occurred = true;
}
bool irs::event_t::check()
{
  bool prev_occurred = m_occurred;
  m_occurred = irs_false;
  return prev_occurred;
}
void irs::event_t::reset()
{
  m_occurred = false;
}

//#endif // NOP

// Класс событий
mxfact_event_t::mxfact_event_t():
  fp_prev_event(IRS_NULL),
  f_occurred(irs_false),
  fp_action(IRS_NULL)
{
}
mxfact_event_t::~mxfact_event_t()
{
}
void mxfact_event_t::exec()
{
  if (fp_prev_event) fp_prev_event->exec();
  if (fp_action) fp_action->exec();
  f_occurred = irs_true;
}
irs_bool mxfact_event_t::check()
{
  irs_bool prev_occurred = f_occurred;
  f_occurred = irs_false;
  return prev_occurred;
}
void mxfact_event_t::reset()
{
  f_occurred = irs_false;
}
void mxfact_event_t::connect(mxfact_event_t *&a_event)
{
  fp_prev_event = a_event;
  a_event = this;
}
void mxfact_event_t::add(irs_action_t *a_action)
{
  a_action->connect(fp_action);
}

// Пустая реализация работы с прерываниями
irs::interrupt_array_empty_t::interrupt_array_empty_t():
  m_gen()
{
}
irs_int_event_gen_t* 
  irs::interrupt_array_empty_t::int_event_gen(size_type /*a_index*/)
{
  return &m_gen;
}
void irs::interrupt_array_empty_t::exec_event(size_type /*a_index*/)
{
}

// Возвращает массив прерываний
irs::interrupt_array_base_t* irs::interrupt_array()
{
  #ifdef __ICCAVR__
  return irs::avr::interrupt_array();
  #else //__ICCAVR__
  static auto_ptr<interrupt_array_t> 
    p_interrupt_array(new interrupt_array_empty_t);
  return p_interrupt_array.get();
  #endif //__ICCAVR__
}
