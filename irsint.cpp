// Прерывания
// Дата: 18.05.2010
// Ранняя дата: 2.09.2009

#include <irsint.h>
#include <irserror.h>
#if defined(__ICCAVR__) || defined(__ICCARM__)
#include <irsarchint.h>
#endif //__ICCAVR__ || __ICCARM__

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
  m_events(),
  m_is_enabled(true)
{
}

void irs::generator_events_t::exec()
{
  if (m_is_enabled) {
    for_each(m_events.begin(), m_events.end(), event_exec_fun_t());
  } else {
    // Генератор выключен. События не генерируются
  }
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

void irs::generator_events_t::enable()
{
  m_is_enabled = true;
}

void irs::generator_events_t::disable()
{
  m_is_enabled = false;
}

bool irs::generator_events_t::is_enabled() const
{
  return m_is_enabled;
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
  #if defined(__ICCAVR__)
  return irs::avr::interrupt_array();
  #elif defined(__ICCARM__)
  return irs::arm::interrupt_array();
  #else //__ICCAVR__
  static interrupt_array_empty_t interrupt_array;
  return &interrupt_array;
  #endif //__ICCAVR__
}

// Работа с прерываниями
irs::interrupt_array_t::interrupt_array_t(gen_index_type a_interrupt_count, 
  gen_index_type a_reserve_interrupt_count
):
  m_int_event_gen_indexes(static_cast<size_t>(a_interrupt_count), 
    static_cast<gen_index_type>(interrupt_none)),
  m_int_event_gens(),
  m_int_event_index(interrupt_none)
{
  m_int_event_gens.reserve(a_reserve_interrupt_count);
}
irs_int_event_gen_t* 
  irs::interrupt_array_t::int_event_gen(size_type a_index)
{
  gen_index_type& gen_index = m_int_event_gen_indexes[a_index];
  if (gen_index == interrupt_none) {
    // Создаем генератор прерываний для номера прерывания "a_index"
    m_int_event_index++;
    m_int_event_gens.resize(m_int_event_index + 1);
    gen_index = m_int_event_index;
  } else {
    // Генератор прерываний для номера прерывания "a_index" уже существует
  }
  return &m_int_event_gens[gen_index];
}
void irs::interrupt_array_t::exec_event(size_type a_index)
{
  gen_index_type& gen_index = m_int_event_gen_indexes[a_index];
  m_int_event_gens[gen_index].exec();
}
