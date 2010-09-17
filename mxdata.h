// Утилиты для работы с mxdata_t
// Дата: 28.04.2010
// Ранняя дата: 21.09.2009

#ifndef mxdataH
#define mxdataH

// Номер файла
#define MXDATAH_IDX 2

#include <irsdefs.h>

#include <mxdatastd.h>
//#include <irsstd.h>
#include <irserror.h>
#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

class mxdata_ext_t : public mxdata_t
{
public:
  enum status_t { status_completed, status_wait, status_error };
  enum mode_refresh_t { mode_refresh_invalid, mode_refresh_auto,
    mode_refresh_manual };
  
  virtual ~mxdata_ext_t() {};
  virtual void mark_to_send(irs_uarc a_index, irs_uarc a_size) = 0;
  virtual void mark_to_recieve(irs_uarc a_index, irs_uarc a_size) = 0;
  virtual void mark_to_send_bit(irs_uarc a_byte_index,
    irs_uarc a_bit_index) = 0;
  virtual void mark_to_recieve_bit(irs_uarc a_byte_index,
    irs_uarc a_bit_index) = 0;
  virtual void update() = 0;
  virtual status_t status() const = 0;
  virtual void set_refresh_mode(mode_refresh_t a_refresh_mode) = 0;
  virtual void abort() = 0;
};

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

// Приведение некоторых C-функций к шаблонному типу
template <class T>
inline void memsetex(T* ap_data, const T& a_fill_elem, size_t a_size)
{
  IRS_LIB_ASSERT(ap_data != IRS_NULL);
  fill_n(ap_data, a_size, a_fill_elem);
}
template <class T>
inline void memsetex(T* ap_data, size_t a_size)
{
  IRS_LIB_ASSERT(ap_data != IRS_NULL);
  memset(reinterpret_cast<void*>(ap_data), 0, a_size*sizeof(T));
}
template <class T>
inline T* memcpyex(T* ap_dest, const T* ap_src, size_t a_size)
{
  IRS_LIB_ASSERT(ap_dest != IRS_NULL);
  IRS_LIB_ASSERT(ap_src != IRS_NULL);
  return reinterpret_cast<T*>(memcpy(
    reinterpret_cast<void*>(ap_dest),
    reinterpret_cast<const void*>(ap_src),
    a_size*sizeof(T)
  ));
}
template <class T>
inline T* memmoveex(T* ap_dest, const T* ap_src, size_t a_size)
{
  IRS_LIB_ASSERT(ap_dest != IRS_NULL);
  IRS_LIB_ASSERT(ap_src != IRS_NULL);
  return reinterpret_cast<T*>(memmove(
    reinterpret_cast<void*>(ap_dest),
    reinterpret_cast<const void*>(ap_src),
    a_size*sizeof(T)
  ));
}

template <class T>
inline int memcmpex(
  const T* ap_buf_first, const T* ap_buf_second, size_t a_size)
{
  IRS_LIB_ASSERT(ap_buf_first != IRS_NULL);
  IRS_LIB_ASSERT(ap_buf_second != IRS_NULL);
  return memcmp(
    reinterpret_cast<const void*>(ap_buf_first),
    reinterpret_cast<const void*>(ap_buf_second),
    a_size * sizeof(T)
  );
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
  inline void swap(raw_data_t<T>* ap_raw_data);
private:
  enum {
    #ifdef __ICCAVR__
    m_capacity_min = 1
    #else //__ICCAVR__
    m_capacity_min = 16
    #endif //__ICCAVR__
  };

  size_type m_size;
  size_type m_capacity;
  pointer mp_data;

  void reserve_raw(size_type a_capacity);
};
template <class T>
raw_data_t<T>::raw_data_t(size_type a_size):
  m_size(a_size),
  m_capacity((m_size > m_capacity_min)?m_size:m_capacity_min),
  mp_data(IRS_LIB_NEW_ASSERT(value_type[m_capacity], MXDATAH_IDX))
{
  memsetex(mp_data, m_size);
}
template <class T>
raw_data_t<T>::raw_data_t(const raw_data_t<T>& a_raw_data):
  m_size(a_raw_data.m_size),
  m_capacity(a_raw_data.m_capacity),
  mp_data(IRS_LIB_NEW_ASSERT(value_type[m_capacity], MXDATAH_IDX))
{
  memcpyex(mp_data, a_raw_data.mp_data, m_size);
}
template <class T>
raw_data_t<T>::~raw_data_t()
{
  IRS_LIB_ARRAY_DELETE_ASSERT(mp_data);
}
template <class T>
inline typename raw_data_t<T>::reference
  raw_data_t<T>::operator[](size_type a_index)
{
  return mp_data[a_index];
}
template <class T>
inline typename raw_data_t<T>::const_reference
  raw_data_t<T>::operator[](size_type a_index) const
{
  return mp_data[a_index];
}
template <class T>
const raw_data_t<T>&
  raw_data_t<T>::operator=(const raw_data_t<T>& a_raw_data)
{
  raw_data_t<T> raw_data(a_raw_data);
  swap(&raw_data);
  return *this;
}
template <class T>
inline typename raw_data_t<T>::pointer
  raw_data_t<T>::data()
{
  #ifdef IRS_LIB_DEBUG
  return size() ? mp_data : IRS_NULL;
  #else // !IRS_DEBUG
  return mp_data;
  #endif // !IRS_DEBUG
}
template <class T>
inline typename raw_data_t<T>::const_pointer
  raw_data_t<T>::data() const
{
  return mp_data;
}
template <class T>
inline typename raw_data_t<T>::size_type
  raw_data_t<T>::size() const
{
  return m_size;
}
template <class T>
inline typename raw_data_t<T>::size_type
  raw_data_t<T>::capacity() const
{
  return m_capacity;
}
template <class T>
inline bool raw_data_t<T>::empty() const
{
  return m_size == 0;
}
template <class T>
void raw_data_t<T>::reserve(size_type a_capacity)
{
  size_type capacity_norm = max(a_capacity, (size_type)m_capacity_min);
  size_type capacity_new = max(capacity_norm, m_size);
  reserve_raw(capacity_new);
}
template <class T>
void raw_data_t<T>::reserve_raw(size_type a_capacity)
{
  IRS_LIB_ASSERT(a_capacity >= m_capacity_min);
  #ifdef IRS_LIB_CHECK
  if (a_capacity < m_capacity_min) return;
  #endif //IRS_LIB_CHECK
  if (a_capacity == m_capacity) return;

  size_type new_capacity = a_capacity;
  pointer new_data =
    IRS_LIB_NEW_ASSERT(value_type[new_capacity], MXDATAH_IDX);

  size_type new_size = min(new_capacity, m_size);
  memcpyex(new_data, mp_data, new_size);
  IRS_LIB_ARRAY_DELETE_ASSERT(mp_data);

  mp_data = new_data;
  m_capacity = new_capacity;
  m_size = new_size;
}
template <class T>
void raw_data_t<T>::resize(size_type a_size)
{
  size_type new_size = a_size;
  if (new_size > m_capacity) {
    #ifdef __ICCAVR__
    size_type new_capacity = new_size;
    #else //__ICCAVR__
    size_type new_capacity = max(m_capacity*2, new_size);
    #endif //__ICCAVR__
    reserve_raw(new_capacity);
    size_type delta = new_size - m_size;
    memsetex(mp_data + m_size, delta);
  } else {
    if (new_size > m_size) {
      size_type delta = new_size - m_size;
      memsetex(mp_data + m_size, delta);
    }
  }
  m_size = new_size;
}
template <class T>
void raw_data_t<T>::insert(pointer ap_pos, const_pointer ap_first,
  const_pointer ap_last)
{
  IRS_LIB_ASSERT((ap_pos >= data()) && (ap_pos <= data()+size()));
  const size_type insert_bloc_size = ap_last - ap_first;
  const size_type new_size = m_size + insert_bloc_size;
  
  // Это условие нужно, чтобы исключить вызов функции memmoveex,
  // когда size() == 0, ибо тогда параметры ap_first, new_size могут
  // быть равны IRS_NULL, и функция memmoveex поднимет исключение
  if (new_size > 0) {
    // Блок памяти должен быть внешним по отношению к приемному контейнеру
    IRS_LIB_ASSERT((ap_last < data()) || (ap_first >= data()+size()));
    const size_type pos = ap_pos - data();
    const size_type move_bloc_size = (data()+size()) - ap_pos;

    resize(new_size);
    // Смещаем данные на размер вставляемого блока вправо
    pointer p_pos = data() + pos;
    pointer dest = p_pos + insert_bloc_size;
    memmoveex(dest, p_pos, move_bloc_size);
    // Вставляем блок
    memcpyex(p_pos, ap_first, insert_bloc_size);
  } 
}
template <class T>
void raw_data_t<T>::erase(pointer ap_first, pointer ap_last)
{
  IRS_LIB_ASSERT((ap_first >= data()) && (ap_first <= (data()+size())));
  IRS_LIB_ASSERT((ap_last >= data()) && (ap_last <= (data()+size())));
  size_type new_size = size() - (ap_last - ap_first);
  // Это условие нужно, чтобы исключить вызов функции memmoveex,
  // когда size() == 0, ибо тогда параметры ap_first, new_size могут
  // быть равны IRS_NULL, и функция memmoveex поднимет исключение
  if (size() > 0) {
    const size_type move_bloc_size = (data()+size()) - ap_last;
    memmoveex(ap_first, ap_last, move_bloc_size);
    resize(new_size);
  }
}
template <class T>
inline void raw_data_t<T>::clear()
{
  m_size = 0;
}

template <class T>
inline void raw_data_t<T>::swap(raw_data_t<T>* ap_raw_data)
{
  ::swap(m_size, ap_raw_data->m_size);
  ::swap(m_capacity, ap_raw_data->m_capacity);
  ::swap(mp_data, ap_raw_data->mp_data);
}

#ifdef IRS_LIB_DEBUG
bool raw_data_test();
#endif // IRS_DEBUG

template <class T>
inline void swap(irs::raw_data_t<T>& a_raw_data_first,
  irs::raw_data_t<T>& a_raw_data_second)
{
  a_raw_data_first.swap(&a_raw_data_second);
}

// Очередь сырых данных
template <class T>
class deque_data_t
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  deque_data_t(size_type a_size = 0);
  deque_data_t(const deque_data_t<T>& a_deque_data);
  ~deque_data_t();
  inline reference operator[](size_type a_index);
  inline const_reference operator[](size_type a_index) const;
  const deque_data_t& operator=(const deque_data_t<T>& a_raw_data);
  inline size_type size() const;
  inline size_type capacity() const;
  inline bool empty() const;
  void reserve(size_type a_capacity);
  void buf_reserve(size_type a_capacity);
  void resize(size_type a_size);
  void buf_resize(size_type a_size);
  void push_back(const_pointer ap_first, const_pointer ap_last);
  void pop_back(size_type);
  void pop_front(size_type);
  void copy_to(size_type a_pos, size_type a_size,
    pointer ap_dest_first);
  inline void clear();
  inline void swap(deque_data_t<T>* ap_deque_data_src);
private:
  void reserve_buf(size_type a_capacity);
  enum {
    #ifdef __ICCAVR__
    m_capacity_min = 1
    #else //__ICCAVR__
    m_capacity_min = 16
    #endif //__ICCAVR__
  };
  size_type m_ring_size;
  size_type m_capacity;
  pointer mp_buf;
  size_type m_ring_begin_pos;
};

template <class T>
irs::deque_data_t<T>::deque_data_t(size_type a_size):
  m_ring_size(a_size),
  m_capacity((m_ring_size > m_capacity_min) ? m_ring_size : m_capacity_min),
  mp_buf(new value_type[m_capacity]),
  m_ring_begin_pos(0)
{
  memsetex(mp_buf, m_ring_size);
}

template <class T>
irs::deque_data_t<T>::deque_data_t(const deque_data_t<T>& a_deque_data):
  m_ring_size(a_deque_data.m_ring_size),
  m_capacity(a_deque_data.m_capacity),
  mp_buf(new value_type[m_capacity]),
  m_ring_begin_pos(a_deque_data.m_ring_begin_pos)
{
  memcpyex(mp_buf, a_deque_data.mp_buf, m_capacity);
}

template <class T>
irs::deque_data_t<T>::~deque_data_t()
{
  delete []mp_buf;
}

template <class T>
inline typename irs::deque_data_t<T>::reference
  irs::deque_data_t<T>::operator[](size_type a_index)
{
  IRS_LIB_ERROR_IF_NOT(a_index < m_ring_size, ec_standard, "Выход за пределы");

  pointer p_elem = IRS_NULL;
  const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;
  if (a_index < buf_right_part_size) {
    p_elem = &mp_buf[m_ring_begin_pos + a_index];
  } else {
    p_elem = &mp_buf[a_index - buf_right_part_size];
  }
  return *p_elem;
}

template <class T>
inline typename irs::deque_data_t<T>::const_reference
  irs::deque_data_t<T>::operator[](size_type a_index) const
{
  IRS_LIB_ERROR_IF_NOT(a_index < m_ring_size, ec_standard, "Выход за пределы");
  pointer p_elem = IRS_NULL;
  const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;
  if (a_index < buf_right_part_size) {
    p_elem = mp_buf[m_ring_begin_pos + a_index];
  } else {
    p_elem = mp_buf[a_index - buf_right_part_size];
  }
  return *p_elem;
}

template <class T>
const typename irs::deque_data_t<T>&
  irs::deque_data_t<T>::operator=(const deque_data_t<T>& a_deque_data)
{
  deque_data_t<T> deque_data(a_deque_data);
  swap(&deque_data);
  return *this;
} 

template <class T>
inline typename irs::deque_data_t<T>::size_type
  irs::deque_data_t<T>::size() const
{
  return m_ring_size;
}

template <class T>
inline typename irs::deque_data_t<T>::size_type
  irs::deque_data_t<T>::capacity() const
{
  return m_capacity;
}

template <class T>
inline bool irs::deque_data_t<T>::empty() const
{
  return m_ring_size == 0;
}

template <class T>
void irs::deque_data_t<T>::reserve(size_type a_capacity)
{
  size_type capacity_norm = max(a_capacity, (size_type)m_capacity_min);
  size_type capacity_new = max(capacity_norm, m_ring_size);
  reserve_buf(capacity_new);
}

template <class T>
void irs::deque_data_t<T>::resize(size_type a_size)
{
  size_type new_size = a_size;
  if (new_size > m_capacity) {
    size_type new_capacity = new_size * 2;
    reserve_buf(new_capacity);
  } else {
    //
  }
  m_ring_size = new_size;
}

template <class T>
void irs::deque_data_t<T>::push_back(
  const_pointer ap_first, const_pointer ap_last)
{
  // Размер вставляемых данных
  const size_type insert_data_size = ap_last - ap_first;
  const size_type old_ring_size = m_ring_size;
  const size_type new_ring_size = insert_data_size + m_ring_size;
  resize(new_ring_size);
  const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;
  if (buf_right_part_size < old_ring_size) {
    pointer dest = mp_buf + (old_ring_size - buf_right_part_size);
    memcpyex(dest, ap_first, insert_data_size);
  } else {
    pointer dest = mp_buf + m_ring_begin_pos + old_ring_size;
    const size_type size_copy_data_to_buf_right_part =
      min(insert_data_size, buf_right_part_size - old_ring_size);
    memcpyex(dest, ap_first, size_copy_data_to_buf_right_part);
    const size_type size_copy_data_to_buf_left_part =
      insert_data_size - size_copy_data_to_buf_right_part;
    memcpyex(mp_buf, ap_first + size_copy_data_to_buf_right_part,
      size_copy_data_to_buf_left_part);
  }
}

template <class T>
void irs::deque_data_t<T>::pop_back(size_type a_size)
{
  IRS_LIB_ASSERT(a_size <= m_ring_size);
  const size_type erase_data_size = min(a_size, m_ring_size);
  m_ring_size -= erase_data_size;
}    

template <class T>
void irs::deque_data_t<T>::pop_front(size_type a_size)
{
  IRS_LIB_ASSERT(a_size <= m_ring_size);
  const size_type erase_data_size = min(a_size, m_ring_size);
  const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;
  if (erase_data_size < buf_right_part_size) {
    m_ring_begin_pos += erase_data_size;
  } else {
    m_ring_begin_pos = erase_data_size - buf_right_part_size;
  }
  m_ring_size -= erase_data_size;
}

template <class T>
void irs::deque_data_t<T>::copy_to(size_type a_pos, size_type a_size,
  pointer ap_dest_first)
{
  IRS_LIB_ASSERT(a_size <= (m_ring_size - a_pos));
  const size_type copy_data_size = min(a_size, m_ring_size - a_pos);
  const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;
  size_type size_copy_data_from_buf_right_part =
    min(buf_right_part_size, copy_data_size);
  memcpyex(ap_dest_first, mp_buf + m_ring_begin_pos + a_pos,
    size_copy_data_from_buf_right_part);
  if (size_copy_data_from_buf_right_part < copy_data_size) {
    memcpyex(ap_dest_first + size_copy_data_from_buf_right_part, mp_buf,
      copy_data_size - size_copy_data_from_buf_right_part);
  } else {
    // Дополнительное копирование из левой части буфера не требуется
  }
}

template <class T>
inline void irs::deque_data_t<T>::clear()
{
  m_ring_size = 0;
  m_ring_begin_pos = 0;
}

template <class T>
inline void irs::deque_data_t<T>::swap(deque_data_t<T>* ap_deque_data_src)
{
  ::swap(m_ring_size, ap_deque_data_src->m_ring_size);
  ::swap(m_capacity, ap_deque_data_src->m_capacity);
  ::swap(mp_buf, ap_deque_data_src->mp_buf);
  ::swap(m_ring_begin_pos, ap_deque_data_src->m_ring_begin_pos);
}

template <class T>
void irs::deque_data_t<T>::reserve_buf(size_type a_capacity)
{
  IRS_LIB_ASSERT(a_capacity >= m_capacity_min);
  if (a_capacity != m_capacity) {  
    size_type new_capacity = a_capacity;
    pointer p_new_buf = new value_type[new_capacity];
    size_type new_ring_size = min(new_capacity, m_ring_size);
    const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;
    size_type size_copy_data_from_buf_right_part =
      min(buf_right_part_size, new_ring_size);  
    memcpyex(p_new_buf, mp_buf + m_ring_begin_pos,
      size_copy_data_from_buf_right_part);
    if (size_copy_data_from_buf_right_part < new_ring_size) {
      memcpyex(p_new_buf + size_copy_data_from_buf_right_part, mp_buf,
        new_ring_size - size_copy_data_from_buf_right_part);
    } else {
      // Дополнительное копирование из левой части буфера не требуется
    }
    delete []mp_buf;
    mp_buf = p_new_buf;
    m_capacity = new_capacity;
    m_ring_size = new_ring_size;
    m_ring_begin_pos = 0;
  } else {
    // Запрашиваемое количество памяти уже выделено 
  }
}

template <class T>
inline void swap(deque_data_t<T>& a_deque_data_first,
  deque_data_t<T>& a_deque_data_second)
{
  a_deque_data_first->swap(&a_deque_data_second);
}

#ifdef IRS_LIB_DEBUG
bool deque_data_test();
#endif // IRS_DEBUG

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
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  template <class T2>
  handle_t(const handle_t<T2>& a_handle);
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  ~handle_t();
  void operator=(const handle_t& a_handle);
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  template <class T2>
  void operator=(const handle_t<T2>& a_handle);
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  T& operator*() const;
  T* operator->() const;
  T* get() const;
  void reset(T* ap_object = IRS_NULL);
  inline bool is_equal(const handle_t& a_handle) const;
  inline bool is_empty() const;
  inline void swap(handle_t& a_handle);
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  template <class T2>
  inline void swap(handle_t<T2>& a_handle);
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
private:
  handle_rep_t<T>* mp_rep;
};
template <class T>
inline void swap(handle_t<T>& a_first_handle, handle_t<T>& a_second_handle)
{
  a_first_handle.swap(a_second_handle);
}
template <class T, class T2>
inline void swap(handle_t<T>& a_first_handle, handle_t<T2>& a_second_handle)
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
  mp_rep((ap_object == IRS_NULL)?IRS_NULL:
    IRS_LIB_NEW_ASSERT(handle_rep_t<T>(1, ap_object), MXDATAH_IDX))
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

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class T>
template <class T2>
handle_t<T>::handle_t(const handle_t<T2>& a_handle):
  mp_rep(reinterpret_cast<handle_rep_t<T>*>(a_handle.mp_rep))
{
  #ifdef IRS_LIB_DEBUG
  // Проверяем, что объект T2 является дочерним от Т
  static_cast<T*>(a_handle.mp_rep.object);
  #endif // IRS_LIB_DEBUG
  if (mp_rep != IRS_NULL) {
    a_handle.mp_rep->counter++;
  }
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

template <class T>
handle_t<T>::~handle_t()
{
  if (mp_rep != IRS_NULL) {
    mp_rep->counter--;
    IRS_LIB_ASSERT(mp_rep->counter >= 0);
    if (mp_rep->counter == 0) {
      IRS_LIB_DELETE_ASSERT(mp_rep->object);
      IRS_LIB_DELETE_ASSERT(mp_rep);
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
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class T>
template <class T2>
void handle_t<T>::operator=(const handle_t<T2>& a_handle)
{
  const bool hanldes_equals =
    (this->mp_rep.object == static_cast<T*>(a_handle.mp_rep.object)) &&
    (this->mp_rep.counter == static_cast<T*>(a_handle.mp_rep.counter));

  if (!hanldes_equals) {
    handle_t handle_copy(a_handle);
    swap(handle_copy);
  } else {
    // Если дескрипторы равны, то ничего не делаем
  }
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT
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
  return (mp_rep) ? mp_rep->object : IRS_NULL;
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

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class T>
template <class T2>
inline void handle_t<T>::swap(handle_t<T2>& a_handle)
{
  handle_rep_t<T>* p_rep_helper = static_cast<T*>(a_handle.mp_rep);
  a_handle.mp_rep = mp_rep;
  mp_rep = p_rep_helper;
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT

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

// Зануление структур
template <class T>
struct zero_struct_t
{
  static T get()
  {
    T struct_instance;
    memsetex(&struct_instance, 1);
    return struct_instance;
  }
};

} //namespace irs

#endif //mxdataH
