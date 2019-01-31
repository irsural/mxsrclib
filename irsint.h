//! \file
//! \ingroup event_processing_group
//! \brief Прерывания
//!
//! Дата: 20.05.2010\n
//! Ранняя дата: 17.09.2009

#ifndef irsintH
#define irsintH

#include <irsdefs.h>

#ifdef __ICCAVR__
#include <inavr.h>
#endif //__ICCAVR__

#include <irscpp.h>

#include <irsfinal.h>

//! \addtogroup drivers_group
//! @{

// Класс действий
class irs_action_t
{
  irs_action_t *fp_prev_action;
public:
  irs_action_t();
  virtual ~irs_action_t();
  virtual void exec();
  virtual void connect(irs_action_t *&a_action);
};

// Класс событий
class mxfact_event_t
{
  mxfact_event_t *fp_prev_event;
  irs_bool f_occurred;
  irs_action_t *fp_action;
public:
  mxfact_event_t(irs_bool a_start_state = irs_false);
  virtual ~mxfact_event_t();
  virtual void exec();
  virtual irs_bool check();
  virtual void reset();
  virtual void connect(mxfact_event_t *&a_event);
  virtual void add(irs_action_t *a_action);
};

// Класс генератора прерываний
class irs_int_event_gen_t
{
  mxfact_event_t *mp_event;
public:
  inline irs_int_event_gen_t();
  inline ~irs_int_event_gen_t();
  inline void exec();
  inline void add(mxfact_event_t *ap_event);
};

inline irs_int_event_gen_t::irs_int_event_gen_t():
  mp_event(IRS_NULL)
{
}
inline irs_int_event_gen_t::~irs_int_event_gen_t()
{
}
inline void irs_int_event_gen_t::exec()
{
  if (mp_event) {
    mp_event->exec();
  }
}
inline void irs_int_event_gen_t::add(mxfact_event_t *ap_event)
{
  ap_event->connect(mp_event);
}

//! @}

namespace irs {

//! \addtogroup drivers_group
//! @{

// Подключение событий к функциям-членам класса
template <class T>
class event_connect_t: public mxfact_event_t
{
public:
  typedef void (T::*member_type)();

  event_connect_t(T* ap_object, member_type ap_member);

  virtual void exec();
private:
  T* mp_object;
  member_type mp_member;

  event_connect_t();
};
template <class T>
event_connect_t<T>::event_connect_t(T* ap_object, member_type ap_member):
  mp_object(ap_object),
  mp_member(ap_member)
{
}
template <class T>
void event_connect_t<T>::exec()
{
  mxfact_event_t::exec();
  (mp_object->*mp_member)();
}

template <class A>
class irs_action_1_t
{
public:
  irs_action_1_t();
  virtual ~irs_action_1_t();
  virtual void exec(A a);
  virtual void connect(irs_action_1_t<A> *&a_action);
private:
  irs_action_1_t<A>* mp_prev_action;
};

template <class A>
irs_action_1_t<A>::irs_action_1_t():
  mp_prev_action(IRS_NULL)
{
}

template <class A>
irs_action_1_t<A>::~irs_action_1_t()
{
}

template <class A>
void irs_action_1_t<A>::exec(A a)
{
  if (mp_prev_action) {
    mp_prev_action->exec(a);
  }
}

template <class A>
void irs_action_1_t<A>::connect(irs_action_1_t<A>*& a_action)
{
  mp_prev_action = a_action;
  a_action = this;
}

// Класс событий для подключения функций с одним параметром
template <class A>
class mxfact_event_1_t
{
public:
  mxfact_event_1_t();
  virtual ~mxfact_event_1_t();
  virtual void exec(A a);
  virtual bool check();
  virtual void reset();
  virtual void connect(mxfact_event_1_t<A>*& a_event);
  virtual void add(irs_action_1_t<A> *a_action);
private:
  mxfact_event_1_t<A>* mp_prev_event;
  bool m_occurred;
  irs_action_1_t<A>* mp_action;
};

template <class A>
mxfact_event_1_t<A>::mxfact_event_1_t():
  mp_prev_event(IRS_NULL),
  m_occurred(false),
  mp_action(IRS_NULL)
{
}

template <class A>
mxfact_event_1_t<A>::~mxfact_event_1_t()
{
}

template <class A>
void mxfact_event_1_t<A>::exec(A a)
{
  if (mp_prev_event) {
    mp_prev_event->exec(a);
  }
  if (mp_action) {
    mp_action->exec(a);
  }
  m_occurred = true;
}

template <class A>
bool mxfact_event_1_t<A>::check()
{
  bool prev_occurred = m_occurred;
  m_occurred = false;
  return prev_occurred;
}

template <class A>
void mxfact_event_1_t<A>::reset()
{
  m_occurred = false;
}

template <class A>
void mxfact_event_1_t<A>::connect(mxfact_event_1_t<A>*& a_event)
{
  mp_prev_event = a_event;
  a_event = this;
}

template <class A>
void mxfact_event_1_t<A>::add(irs_action_1_t<A>* a_action)
{
  a_action->connect(mp_action);
}

template <class T, class A>
class event_connect_1_t: public mxfact_event_1_t<A>
{
public:
  typedef void (T::*member_type)(A);
  event_connect_1_t(T* ap_object, member_type ap_member);
  virtual void exec(A a);
private:
  event_connect_1_t();
  T* mp_object;
  member_type mp_member;
};
template <class T, class A>
event_connect_1_t<T, A>::event_connect_1_t(T* ap_object, member_type ap_member):
  mp_object(ap_object),
  mp_member(ap_member)
{
}
template <class T, class A>
void event_connect_1_t<T, A>::exec(A a)
{
  mxfact_event_1_t<A>::exec(a);
  (mp_object->*mp_member)(a);
}

//! \brief Базовый класс событий
class event_t
{
  bool m_occurred;
public:
  event_t();
  virtual ~event_t();
  virtual void exec();
  virtual bool check();
  virtual void reset();
};

//! \brief Базовый класс событий для вызова функций с одним аргументом
template <class A>
class event_1_t
{
  bool m_occurred;
public:
  event_1_t();
  virtual ~event_1_t();
  virtual void exec(A);
  virtual bool check();
  virtual void reset();
};

template <class A>
irs::event_1_t<A>::event_1_t():
  m_occurred(false)
{
}

template <class A>
irs::event_1_t<A>::~event_1_t()
{
}

template <class A>
void irs::event_1_t<A>::exec(A)
{
  m_occurred = true;
}

template <class A>
bool irs::event_1_t<A>::check()
{
  bool prev_occurred = m_occurred;
  m_occurred = irs_false;
  return prev_occurred;
}

template <class A>
void irs::event_1_t<A>::reset()
{
  m_occurred = false;
}

// Генератор событий
class generator_events_t
{
public:
  typedef vector<event_t*> event_container_type;

  generator_events_t();
  void exec();
  void push_back(event_t* ap_event);
  void erase(event_t* ap_event);
  void clear();
  void enable();
  void disable();
  bool is_enabled() const;
private:
  event_container_type m_events;
  bool m_is_enabled;
};

inline generator_events_t& operator << (generator_events_t& a_generator_events,
  event_t* ap_event)
{
  a_generator_events.push_back(ap_event);
  return a_generator_events;
}
inline generator_events_t& operator >> (generator_events_t& a_generator_events,
  event_t* ap_event)
{
  a_generator_events.erase(ap_event);
  return a_generator_events;
}
inline generator_events_t& operator << (generator_events_t& a_generator_events,
  event_t& p_event)
{
  a_generator_events.push_back(&p_event);
  return a_generator_events;
}
inline generator_events_t& operator >> (generator_events_t& a_generator_events,
  event_t& p_event)
{
  a_generator_events.erase(&p_event);
  return a_generator_events;
}

// Генератор событий
template <class argument_type>
class generator_events_1_t
{
public:
  typedef event_1_t<argument_type> event_type;
  typedef vector<event_type*> event_container_type;

  generator_events_1_t();
  void exec(argument_type a);
  void push_back(event_type* ap_event);
  void erase(event_type* ap_event);
  void clear();
  void enable();
  void disable();
  bool is_enabled() const;
private:
  event_container_type m_events;
  bool m_is_enabled;
};

template <class argument_type>
irs::generator_events_1_t<argument_type>::generator_events_1_t():
  m_events(),
  m_is_enabled(true)
{
}

template <class argument_type>
class event_exec_fun_1_t
{
public:
  event_exec_fun_1_t(argument_type arg):
    m_argument(arg)
  {
  }
  void operator()(event_1_t<argument_type> *ap_event)
  {
    ap_event->exec(m_argument);
  }
private:
  event_exec_fun_1_t();
  argument_type m_argument;
};

template <class argument_type>
void irs::generator_events_1_t<argument_type>::exec(argument_type a)
{
  if (m_is_enabled) {
    event_exec_fun_1_t<argument_type> event_exec_fun_1(a);
    for_each(m_events.begin(), m_events.end(), event_exec_fun_1);
  } else {
    // Генератор выключен. События не генерируются
  }
}

template <class argument_type>
void irs::generator_events_1_t<argument_type>::push_back(event_type* ap_event)
{
  typename event_container_type::iterator it_event =
    find(m_events.begin(), m_events.end(), ap_event);
  if (it_event == m_events.end()) {
    m_events.push_back(ap_event);
  } else {
    // Объект уже добавлен
  }
}

template <class argument_type>
void irs::generator_events_1_t<argument_type>::erase(event_type* ap_event)
{
  typename event_container_type::iterator it_event =
    find(m_events.begin(), m_events.end(), ap_event);
  IRS_LIB_ASSERT(it_event != m_events.end());
  if (it_event != m_events.end()) {
    m_events.erase(it_event);
  }
}

template <class argument_type>
void irs::generator_events_1_t<argument_type>::clear()
{
  m_events.clear();
}

template <class argument_type>
void irs::generator_events_1_t<argument_type>::enable()
{
  m_is_enabled = true;
}

template <class argument_type>
void irs::generator_events_1_t<argument_type>::disable()
{
  m_is_enabled = false;
}

template <class argument_type>
bool irs::generator_events_1_t<argument_type>::is_enabled() const
{
  return m_is_enabled;
}

template <class argument_type>
inline generator_events_1_t<argument_type>& operator <<
  (generator_events_1_t<argument_type>& a_generator_events,
  event_1_t<argument_type>* ap_event)
{
  a_generator_events.push_back(ap_event);
  return a_generator_events;
}

template <class argument_type>
inline generator_events_1_t<argument_type>& operator >> (
  generator_events_1_t<argument_type>& a_generator_events,
  event_1_t<argument_type>* ap_event)
{
  a_generator_events.erase(ap_event);
  return a_generator_events;
}

template <class argument_type>
inline generator_events_1_t<argument_type>& operator << (
  generator_events_1_t<argument_type>& a_generator_events,
  event_1_t<argument_type>& p_event)
{
  a_generator_events.push_back(&p_event);
  return a_generator_events;
}

template <class argument_type>
inline generator_events_1_t<argument_type>& operator >> (
  generator_events_1_t<argument_type>& a_generator_events,
  event_1_t<argument_type>& p_event)
{
  a_generator_events.erase(&p_event);
  return a_generator_events;
}

template<class owner_type>
class event_function_t: public event_t
{
public:
  typedef void (owner_type::*function_type)();
  event_function_t(owner_type* ap_owner, function_type ap_function);
  virtual void exec();
private:
  // В Watcome без конструктора по умолчанию не компилируется
  // class error_out_t
  //event_function_t();
  owner_type* mp_owner;
  function_type mp_function;
};
template <class owner_type>
event_function_t<owner_type>::event_function_t(owner_type* ap_owner,
  function_type ap_function
):
  mp_owner(ap_owner),
  mp_function(ap_function)
{
}
template <class owner_type>
void event_function_t<owner_type>::exec()
{
  (mp_owner->*mp_function)();
}

template <class owner_type>
inline event_t* make_event(
  owner_type* ap_owner,
  void (owner_type::*ap_function)()
)
{
  return new event_function_t<owner_type>(ap_owner, ap_function);
}

template<class owner_type, class argument_type>
class event_function_1_t: public event_1_t<argument_type>
{
public:
  typedef void (owner_type::*function_type)(argument_type);
  event_function_1_t(owner_type* ap_owner, function_type ap_function);
  virtual void exec(argument_type a);
private:
  // В Watcome без конструктора по умолчанию не компилируется
  // class error_out_t
  //event_function_1_t();
  owner_type* mp_owner;
  function_type mp_function;
};
template<class owner_type, class argument_type>
event_function_1_t<owner_type, argument_type>::event_function_1_t(
  owner_type* ap_owner,
  function_type ap_function
):
  mp_owner(ap_owner),
  mp_function(ap_function)
{
}
template<class owner_type, class argument_type>
void event_function_1_t<owner_type, argument_type>::exec(argument_type a)
{
  (mp_owner->*mp_function)(a);
}

template<class owner_type, class argument_type>
inline event_1_t<argument_type>* make_event_1(
  owner_type* ap_owner,
  void (owner_type::*ap_function)(argument_type)
)
{
  return new event_function_1_t<owner_type, argument_type>(ap_owner,
    ap_function);
}

// Интерфейс для работы с прерываниями
class interrupt_array_base_t
{
public:
  typedef irs_size_t size_type;

  virtual irs_int_event_gen_t* int_event_gen(size_type a_index) = 0;
  virtual generator_events_t* int_gen_events(size_type a_index) = 0;
  virtual void exec_event(size_type a_index) = 0;
};

// Возвращает массив прерываний
interrupt_array_base_t* interrupt_array();

// Блокировка прерываний по принципу: выделение ресурса есть инициализация
#ifdef __ICCAVR__
class lock_interrupt_t
{
  irs_u8 m_old_state;
public:
  lock_interrupt_t():
    m_old_state(__save_interrupt())
  {
    __disable_interrupt();
  }
  ~lock_interrupt_t()
  {
    __restore_interrupt(m_old_state);
  }
};
#else //__ICCAVR__
class lock_interrupt_t
{
public:
};
#endif //__ICCAVR__

// Работа с прерываниями
class interrupt_array_t: public interrupt_array_base_t
{
public:
  typedef interrupt_array_base_t base_type;
  typedef base_type::size_type size_type;
  typedef irs_u32 gen_index_type;

  interrupt_array_t(gen_index_type a_interrupt_count, gen_index_type
    a_reserve_interrupt_count);
  //! \brief Устаревшая функия
  virtual irs_int_event_gen_t* int_event_gen(size_type a_index);
  virtual generator_events_t* int_gen_events(size_type a_index);
  virtual void exec_event(size_type a_index);
private:
  enum {
    interrupt_none = 0
  };

  vector<gen_index_type> m_int_event_gen_indexes;
  vector<irs_int_event_gen_t> m_int_event_gens;
  gen_index_type m_int_event_index;
  vector<gen_index_type> m_int_gen_events_indexes;
  vector<generator_events_t> m_generators_events;
  gen_index_type m_int_gen_event_index;
  interrupt_array_t(const interrupt_array_t&);
};

//! @}

} //namespace irs

#endif //irsintH
