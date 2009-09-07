// Утилиты для работы с mxdata_t
// Дата: 30.08.2009

#ifndef mxdataH
#define mxdataH

#include <irsstd.h>
#include <irserror.h>
#include <irscpp.h>

namespace irs {

// Тип для параметра - изменение при первом соединении
enum change_data_first_connect_t {
  change_on_first_connect,
  not_change_on_first_connect
};

// Класс проверки изменения для conn_data_t
template <class T, int size = sizeof(T)>
class change_data_t
{
public:
  change_data_t(const irs::conn_data_t<T, size, false> *ap_conn_data = IRS_NULL,
    change_data_first_connect_t a_first_connect_change =
    not_change_on_first_connect);
  void connect(const irs::conn_data_t<T, size, false> *ap_conn_data,
    change_data_first_connect_t a_first_connect_change =
    not_change_on_first_connect);
  bool changed();
private:
  irs_uarc m_index;
  irs::mxdata_t *mp_data;
  T m_elem;
  bool m_first_connect;
  change_data_first_connect_t m_first_connect_change;
};

template <class T, int size>
change_data_t<T, size>::change_data_t(
  const irs::conn_data_t<T, size, false> *ap_conn_data,
  change_data_first_connect_t a_first_connect_change):
  m_index(ap_conn_data?ap_conn_data->index():0),
  mp_data(ap_conn_data?ap_conn_data->data():0),
  m_elem(),
  m_first_connect(true),
  m_first_connect_change(a_first_connect_change)
{
}
template <class T, int size>
void change_data_t<T, size>::connect(
  const irs::conn_data_t<T, size, false> *ap_conn_data,
  change_data_first_connect_t a_first_connect_change)
{
  m_index = ap_conn_data->index();
  mp_data = ap_conn_data->data();
  m_first_connect_change = a_first_connect_change;
}
template <class T, int size>
bool change_data_t<T, size>::changed()
{
  bool result = false;
  if (m_first_connect) {
    if (mp_data->connected()) {
      m_first_connect = false;
      mp_data->read(reinterpret_cast<irs_u8*>(&m_elem), m_index, size);
      if (m_first_connect_change == change_on_first_connect) {
        result = true;
      } else {
        result = false;
      }
    } else {
      result = false;
    }
  } else {
    T new_elem = T();
    mp_data->read(reinterpret_cast<irs_u8*>(&new_elem), m_index, size);
    if (m_elem != new_elem) {
      m_elem = new_elem;
      result = true;
    } else {
      result = false;
    }
  }
  return result;
}

// Класс проверки изменения для bit_data_t
class change_bit_t
{
public:
  change_bit_t(const bit_data_t *ap_bit_data = 0,
    change_data_first_connect_t a_first_connect_change =
    not_change_on_first_connect);
  void connect(const bit_data_t *ap_bit_data,
    change_data_first_connect_t a_first_connect_change =
    not_change_on_first_connect);
  bool changed();
private:
  //static const irs_uarc m_bit_size = 1;

  irs::mxdata_t *mp_data;
  irs_uarc m_index;
  irs_uarc m_bit_index;
  bool m_first_connect;
  bit_data_t::bit_t m_bit;
  change_data_first_connect_t m_first_connect_change;
};

// Объединение нескольких разнородных классов mxdata_t в один класс
class collect_data_t: public mxdata_t
{
public:
  collect_data_t();
  void add(mxdata_t *ap_data, irs_uarc a_index, irs_uarc a_size);
  void clear();

  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
private:
  struct item_t
  {
    mxdata_t *p_data;
    irs_uarc index;
    irs_uarc size;
    irs_uarc end;
    item_t():
      p_data(IRS_NULL),
      index(0),
      size(0),
      end(0)
    {
    }
  };
  typedef vector<item_t> items_t;
  typedef items_t::iterator item_it_t;
  enum read_write_t { rw_read, rw_write, rw_bit_read, rw_bit_write };

  vector<item_t> m_items;
  irs_uarc m_size;

  bool belong(irs_uarc a_begin, irs_uarc a_end,
    irs_uarc a_area_begin, irs_uarc a_area_end,
    irs_uarc* a_out_begin, irs_uarc* a_out_end);
  void read_write(read_write_t a_read_write, irs_u8 *ap_buf_read,
    const irs_u8* ap_buf_write, irs_uarc a_index, irs_uarc a_size,
    irs_uarc a_bit_index);
};

// Тип для параметра - изменение при соединении объекта
enum change_on_connect_t {
  change_on_connect,
  not_change_on_connect
};
// Дефолтовый предикат Equal для класса change_obj_t
template <class T, class Prev>
struct change_obj_pred_def_t {
  static inline bool equal(const T& obj_first, const T& obj_second)
  {
    return obj_first == obj_second;
  }
  static inline Prev prev(T& obj)
  {
    return obj;
  }
};
// Проверка объекта на изменение
template <class T, class Prev = T,
  class Pred = change_obj_pred_def_t<T, Prev> >
class change_obj_t
{
public:
  change_obj_t(T* ap_obj = IRS_NULL,
    change_on_connect_t a_change_on_connect =
    not_change_on_connect);
  void connect(T* ap_obj);
  bool changed();
  void add_event(mxfact_event_t *ap_event);
  void change_on_connect_on();
  void change_on_connect_off();
private:
  T* mp_obj;
  Prev m_prev;
  mxfact_event_t *mp_event;
  bool m_changed;
  change_on_connect_t m_change_on_connect;
};
template <class T, class Prev, class Pred>
change_obj_t<T, Prev, Pred>::change_obj_t(T* ap_obj,
  change_on_connect_t a_change_on_connect):
  mp_obj(ap_obj),
  m_prev(Prev()),
  mp_event(IRS_NULL),
  m_changed(false),
  m_change_on_connect(a_change_on_connect)
{
  connect(mp_obj);
}
template <class T, class Prev, class Pred>
void change_obj_t<T, Prev, Pred>::connect(T* ap_obj)
{
  mp_obj = ap_obj;
  if (ap_obj != IRS_NULL) {
    m_prev = Pred::prev(*mp_obj);
    if (m_change_on_connect == change_on_connect) {
      m_changed = true;
    }
  }
}
template <class T, class Prev, class Pred>
bool change_obj_t<T, Prev, Pred>::changed()
{
  if (!Pred::equal(*mp_obj, m_prev)) {
    m_prev = Pred::prev(*mp_obj);
    m_changed = true;
  }
  bool changed_save = m_changed;
  m_changed = false;
  if (changed_save) {
    if (mp_event) mp_event->exec();
  }
  return changed_save;
}
template <class T, class Prev, class Pred>
void change_obj_t<T, Prev, Pred>::add_event(mxfact_event_t *ap_event)
{
  ap_event->connect(mp_event);
}
template <class T, class Prev, class Pred>
void change_obj_t<T, Prev, Pred>::change_on_connect_on()
{
  m_change_on_connect = change_on_connect;
}
template <class T, class Prev, class Pred>
void change_obj_t<T, Prev, Pred>::change_on_connect_off()
{
  m_change_on_connect = not_change_on_connect;
}

// Возвращает входное значение и сбрасывает его
template <class T>
inline T reset_val(T* p_val, T zero_val = T())
{
  T val_save = *p_val;
  *p_val = zero_val;
  return val_save;
}

// Сырые данные
template <class T>
class raw_data_t
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  raw_data_t(size_type a_size = 0);
  raw_data_t(const raw_data_t<T>& a_raw_data);
  ~raw_data_t();
  inline reference operator[](size_type a_index);
  inline const_reference operator[](size_type a_index) const;
  const raw_data_t& operator=(const raw_data_t<T>& a_raw_data);
  inline pointer data();
  inline const_pointer data() const;
  inline size_type size() const;
  inline size_type capacity() const;
  inline bool empty() const;
  void reserve(size_type a_capacity);
  void resize(size_type a_size);
  void insert(pointer ap_pos, const_pointer ap_first, const_pointer ap_last);
  void erase(pointer ap_first, pointer ap_last);
  inline void clear();
private:
  //static const int m_capacity_delta;
  enum {
    m_capacity_min = 16
  };

  size_type m_size;
  //bool m_is_capacity_delta_on;
  size_type m_capacity;
  pointer mp_data;

  void reserve_raw(size_type a_capacity);
};
template <class T>
irs::raw_data_t<T>::raw_data_t(size_type a_size):
  m_size(a_size),
  //m_is_capacity_delta_on(true),
  m_capacity((m_size > m_capacity_min)?m_size:m_capacity_min),
  mp_data(new value_type[m_capacity])
{
  memset(mp_data, 0, m_size);
}
template <class T>
irs::raw_data_t<T>::raw_data_t(const raw_data_t<T>& a_raw_data):
  m_size(a_raw_data.m_size),
  //m_is_capacity_delta_on(a_raw_data.m_is_capacity_delta_on),
  m_capacity(a_raw_data.m_capacity),
  mp_data(new value_type[m_capacity])
{
  memcpy(mp_data, a_raw_data.mp_data, m_size);
}
template <class T>
irs::raw_data_t<T>::~raw_data_t()
{
  delete []mp_data;
}
template <class T>
inline typename irs::raw_data_t<T>::reference
  irs::raw_data_t<T>::operator[](size_type a_index)
{
  return mp_data[a_index];
}
template <class T>
inline typename irs::raw_data_t<T>::const_reference
  irs::raw_data_t<T>::operator[](size_type a_index) const
{
  return mp_data[a_index];
}
template <class T>
const typename irs::raw_data_t<T>&
  irs::raw_data_t<T>::operator=(const raw_data_t<T>& a_raw_data)
{
  m_size = a_raw_data.m_size;
  //m_is_capacity_delta_on = a_raw_data.m_is_capacity_delta_on;
  m_capacity = a_raw_data.m_capacity;
  delete []mp_data;
  mp_data = new value_type[m_capacity];
  memcpy(mp_data, a_raw_data.mp_data, m_size);
  return *this;
}
template <class T>
inline typename irs::raw_data_t<T>::pointer
  irs::raw_data_t<T>::data()
{
  return mp_data;
}
template <class T>
inline typename irs::raw_data_t<T>::const_pointer
  irs::raw_data_t<T>::data() const
{
  return mp_data;
}
template <class T>
inline typename irs::raw_data_t<T>::size_type
  irs::raw_data_t<T>::size() const
{
  return m_size;
}
template <class T>
inline typename irs::raw_data_t<T>::size_type
  irs::raw_data_t<T>::capacity() const
{
  return m_capacity;
}
template <class T>
inline bool irs::raw_data_t<T>::empty() const
{
  return m_size == 0;
}
template <class T>
void irs::raw_data_t<T>::reserve(size_type a_capacity)
{
  size_type capacity_norm = max(a_capacity, (size_type)m_capacity_min);
  size_type capacity_new = max(capacity_norm, m_size);
  reserve_raw(capacity_new);
  #ifdef NOP
  if (a_capacity <= 0) {
    //m_is_capacity_delta_on = false;
    reserve_raw(m_size);
  } else if (a_capacity < m_size) {
    reserve_raw(m_size);
  } else {
    reserve_raw(a_capacity);
  }
  #endif //NOP
}
template <class T>
void irs::raw_data_t<T>::reserve_raw(size_type a_capacity)
{
  IRS_LIB_ASSERT(a_capacity >= m_capacity_min);
  #ifdef IRS_LIB_CHECK
  if (a_capacity < m_capacity_min) return;
  #endif //IRS_LIB_CHECK
  if (a_capacity == m_capacity) return;

  size_type new_capacity = a_capacity;
  pointer new_data = new value_type[new_capacity];
  size_type new_size = min(new_capacity, m_size);
  memcpy(new_data, mp_data, new_size);
  delete []mp_data;
  mp_data = new_data;
  m_capacity = new_capacity;
  m_size = new_size;
}
template <class T>
void irs::raw_data_t<T>::resize(size_type a_size)
{
  size_type new_size = a_size;
  if (new_size > m_capacity) {
    size_type new_capacity = m_capacity*2;
    reserve_raw(new_capacity);
    size_type delta = new_size - m_size;
    memset(mp_data + m_size, 0, delta);
  } else {
    if (new_size > m_size) {
      size_type delta = new_size - m_size;
      memset(mp_data + m_size, 0, delta);
    }
  }
  m_size = new_size;
}
template <class T>
void irs::raw_data_t<T>::insert(pointer ap_pos, const_pointer ap_first,
  const_pointer ap_last)
{
  IRS_LIB_ASSERT((ap_pos >= data()) && (ap_pos <= data()+size()));
  const size_type insert_bloc_size = ap_last - ap_first;
  const size_type new_size = m_size + insert_bloc_size;
  // Блок памяти должен быть внешним по отношению к приемному контейнеру
  IRS_LIB_ASSERT((ap_last < data()) || (ap_first >= data()+size()));
  const size_type pos = ap_pos - data();
  const size_type move_bloc_size = (data()+size()) - ap_pos;
  resize(new_size);
  // Смещаем данные на размер вставляемого блока вправо
  pointer p_pos = data() + pos;
  pointer dest = p_pos + insert_bloc_size;
  memmove(dest, p_pos, move_bloc_size);
  // Вставляем блок
  memcpy(p_pos, ap_first, insert_bloc_size);
}
template <class T>
void irs::raw_data_t<T>::erase(pointer ap_first, pointer ap_last)
{
  IRS_LIB_ASSERT((ap_first >= data()) && (ap_first <= (data()+size())));
  IRS_LIB_ASSERT((ap_last >= data()) && (ap_last <= (data()+size())));
  size_type new_size = size() - (ap_last - ap_first);
  const size_type move_bloc_size = (data()+size()) - ap_last;
  memmove(ap_first, ap_last, move_bloc_size);
  resize(new_size);
}
template <class T>
inline void irs::raw_data_t<T>::clear()
{
  m_size = 0;
}

// Дескриптор объекта с автоматическим уничтожением объекта по счетчику ссылок
template <class T>
struct handle_rep_t
{
  int counter;
  T* object;

  handle_rep_t():
    counter(0),
    object(IRS_NULL)
  {
  }
  handle_rep_t(int a_counter, T* ap_object):
    counter(a_counter),
    object(ap_object)
  {
  }
};
template <class T>
class handle_t
{
public:
  handle_t(T* ap_object = IRS_NULL);
  handle_t(const handle_t& a_handle);
  ~handle_t();
  void operator=(const handle_t& a_handle);
  T& operator*() const;
  T* operator->() const;
  T* get() const;
  void reset(T* ap_object = IRS_NULL);
  inline bool is_equal(const handle_t& a_handle) const;
  inline bool is_empty() const;
  inline void swap(handle_t& a_handle);
private:
  handle_rep_t<T>* mp_rep;
};
template <class T>
inline void swap(handle_t<T>& a_first_handle, handle_t<T>& a_second_handle)
{
  a_first_handle.swap(a_second_handle);
}
template <class T>
inline bool operator==(const handle_t<T>& a_first_handle,
  const handle_t<T>& a_second_handle)
{
  return a_first_handle.is_equal(a_second_handle);
}
template <class T>
inline bool operator!=(const handle_t<T>& a_first_handle,
  const handle_t<T>& a_second_handle)
{
  return !a_first_handle.is_equal(a_second_handle);
}
template <class T>
handle_t<T>::handle_t(T* ap_object):
  mp_rep((ap_object == IRS_NULL)?IRS_NULL:new handle_rep_t<T>(1, ap_object))
{
}
template <class T>
handle_t<T>::handle_t(const handle_t& a_handle):
  mp_rep(a_handle.mp_rep)
{
  if (mp_rep != IRS_NULL) {
    a_handle.mp_rep->counter++;
  }
}
template <class T>
handle_t<T>::~handle_t()
{
  if (mp_rep != IRS_NULL) {
    mp_rep->counter--;
    IRS_LIB_ASSERT(mp_rep->counter >= 0)
    if (mp_rep->counter == 0) {
      delete mp_rep->object;
      delete mp_rep;
    }
  }
}
template <class T>
void handle_t<T>::operator=(const handle_t& a_handle)
{
  if (*this != a_handle) {
    handle_t handle_copy(a_handle);
    swap(handle_copy);
  } else {
    // Если дескрипторы равны, то ничего не делаем
  }
}
template <class T>
T& handle_t<T>::operator*() const
{
  return *mp_rep->object;
}
template <class T>
T* handle_t<T>::operator->() const
{
  return mp_rep->object;
}
template <class T>
T* handle_t<T>::get() const
{
  return mp_rep->object;
}
template <class T>
void handle_t<T>::reset(T* ap_object)
{
  if ((mp_rep == IRS_NULL) || (mp_rep->object != ap_object)) {
    handle_t handle_copy(ap_object);
    swap(handle_copy);
  } else {
    // Если дескриптор уже подключен к этому объекту, то ничего не делаем
  }
}
template <class T>
inline bool handle_t<T>::is_equal(const handle_t& a_handle) const
{
  return mp_rep == a_handle.mp_rep;
}
template <class T>
inline bool handle_t<T>::is_empty() const
{
  return mp_rep == IRS_NULL;
}
template <class T>
inline void handle_t<T>::swap(handle_t& a_handle)
{
  ::swap(mp_rep, a_handle.mp_rep);
}
#ifdef NOP
inline void handle_test()
{
  handle_t<int> p_int = new int(3);
  handle_t<int> p_int_copy(p_int);
  handle_t<int> p_int_assign;
  p_int_assign = p_int;
  if (!p_int.is_empty()) {
    *p_int = 5;
  }
  handle_t<vector<int> > p_vec = new vector<int>(5, 7);
  if (p_int == p_int_assign) {
    size_t size = p_vec->size();
  }
}
#endif //NOP

} //namespace irs

#endif //mxdataH
