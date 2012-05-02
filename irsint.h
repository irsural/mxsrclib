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
  mxfact_event_t();
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
  (mp_object->*mp_member)();
}

// Интерфейс для работы с прерываниями
class interrupt_array_base_t
{
public:
  typedef irs_size_t size_type;

  virtual irs_int_event_gen_t* int_event_gen(size_type a_index) = 0;
  virtual void exec_event(size_type a_index) = 0;
};

// Возвращает массив прерываний
interrupt_array_base_t* interrupt_array();

// Базовый класс событий
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
  typedef irs_u8 gen_index_type;

  interrupt_array_t(gen_index_type a_interrupt_count, gen_index_type
    a_reserve_interrupt_count);

  virtual irs_int_event_gen_t* int_event_gen(size_type a_index);
  virtual void exec_event(size_type a_index);
private:
  enum {
    interrupt_none = 0
  };

  vector<gen_index_type> m_int_event_gen_indexes;
  vector<irs_int_event_gen_t> m_int_event_gens;
  gen_index_type m_int_event_index;

  interrupt_array_t(const interrupt_array_t&);
};

//! @}

} //namespace irs

#endif //irsintH
