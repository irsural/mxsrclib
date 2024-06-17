//! \file
//! \ingroup container_group
//! \ingroup in_out_group
//! \brief Утилиты для работы с mxdata_t
//!
//! Дата: 24.04.2011
//! Ранняя дата: 21.09.2009

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

//! \ingroup in_out_group
class mxdata_ext_t : public mxdata_t
{
public:
  enum status_t {
    status_completed,
    status_wait,
    status_error
  };
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

//! \brief Класс упрощённого доступа к памяти

class comm_data_t
{
public:
  typedef size_t size_type;
  virtual ~comm_data_t() {};
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size) = 0;
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size) = 0;
  virtual irs_status_t status() = 0;
  virtual size_type size() = 0;
  virtual void tick() = 0;
};

//! \brief Класс доступа к памяти со страничной организацией
class page_mem_t
{
public:
  typedef size_t size_type;
  virtual ~page_mem_t() {};
  virtual void read_page(irs_u8 *ap_buf, irs_uarc a_index) = 0;
  virtual void write_page(const irs_u8 *ap_buf, irs_uarc a_index) = 0;
  virtual size_type page_size() const = 0;
  virtual irs_uarc page_count() const = 0;
  virtual irs_status_t status() const = 0;
  virtual void tick() = 0;
};

//! \brief Класс доступа к памяти со страничной организацией и разными размерами
//!   страниц
class various_page_mem_t
{
public:
  typedef size_t size_type;
  virtual ~various_page_mem_t() {};
  virtual void read(irs_u8* ap_pos, irs_u8 *ap_buf,
    size_type a_buf_size) = 0;
  virtual void erase_page(size_type a_page_index) = 0;
  virtual void write(irs_u8* ap_pos, const irs_u8 *ap_buf,
    size_type a_buf_size) = 0;
  virtual irs_u8* page_begin(size_type a_page_index) = 0;
  virtual size_type page_index(const irs_u8* ap_pos) const = 0;
  virtual size_type page_size(size_type a_page_index) const = 0;
  virtual size_type page_count() const = 0;
  virtual irs_status_t status() const = 0;
  virtual void tick() = 0;
};

//! \brief Проверка итератора с произвольным доступом на попадание внутрь
//! контейнера, равенство end() является допустимым
//!
//! Возвращает true, если итератор внутри контейнера
//! Работает также на указателях и контейнерах типа raw_data_t
template <class C, class I>
bool check_iterator(C& a_container, I a_iterator)
{
  return (a_container.begin() <= a_iterator) &&
    (a_iterator <= a_container.end());
}

//! \brief Проверка итератора с произвольным доступом на попадание внутрь
//! контейнера, равенство end() не допускается
//!
//! Возвращает true, если итератор внутри контейнера
//! Работает также на указателях и контейнерах типа raw_data_t
template <class C, class I>
bool check_iterator_strict(C& a_container, I a_iterator)
{
  return (a_container.begin() <= a_iterator) &&
    (a_iterator < a_container.end());
}

//! \brief Проверка двух итератора с произвольным доступом на попадание внутрь
//! контейнера
//!
//! Возвращает true, если оба итератора внутри контейнера и begin <= end
//! Равенство итератора end() является допустимым
//! Работает также на указателях и контейнерах типа raw_data_t
template <class C, class I>
bool check_iterator(C& a_container, I a_iterator_begin, I a_iterator_end)
{
  return (a_iterator_begin <= a_iterator_end) &&
    check_iterator(a_container, a_iterator_begin) &&
    check_iterator(a_container, a_iterator_end);
}

//! \brief Проверка двух итератора с произвольным доступом на попадание внутрь
//! контейнера
//!
//! Возвращает true, если оба итератора внутри контейнера и begin <= end
//! Допускается равенство end() только параметра a_iterator_end
//! Работает также на указателях и контейнерах типа raw_data_t
template <class C, class I>
bool check_iterator_strict(C& a_container, I a_iterator_begin, I a_iterator_end)
{
  return (a_iterator_begin <= a_iterator_end) &&
    check_iterator_strict(a_container, a_iterator_begin) &&
    check_iterator(a_container, a_iterator_end);
}

//! \brief Проверка индекса в контейнере с произвольным доступом на попадание
//! внутрь контейнера
//!
//! Возвращает true, если индекс внутри контейнера
template <class C, class S>
bool check_index(C& a_container, S a_index)
{
  return check_iterator_strict(a_container, a_container.begin() + a_index);
}

//! \brief Проверка диапазона в контейнере с произвольным доступом на попадание
//! внутрь контейнера
//!
//! Возвращает true, если индекс внутри контейнера
template <class C, class S>
bool check_index(C& a_container, S a_index, S a_size)
{
  return check_iterator_strict(a_container, a_container.begin() + a_index,
    a_container.begin() + a_index + a_size);
}

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
  IRS_LIB_ASSERT(((ap_dest != IRS_NULL) && (ap_src != IRS_NULL)) ||
    (a_size == 0));
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

//! \brief Возвращает указатель на данные, хранящиеся в std::vector
template <class T>
T* vector_data(vector<T>& a_v, size_t a_pos = 0)
{
  return (a_pos < a_v.size()) ? &a_v[a_pos] : IRS_NULL;
}

//! \brief Возвращает указатель на данные, хранящиеся в std::vector
template <class T>
const T* vector_data(const vector<T>& a_v, size_t a_pos = 0)
{
  return (a_pos < a_v.size()) ? &a_v[a_pos] : IRS_NULL;
}

//! \brief Контейнер сырых данных с элементами интерфейса std::vector<T>
//! \authors
//! - Krasheninnikov Maxim\n
//!     Первоначальная реализация
//! - Lyashchov Maksim\n
//!     Добавление методов, подобных тем, что имеются у std::vector<T>
//!
//! Этот контейнер реализован в рамках альтернативы контейнеру std::vector<T>,
//!   для тех случаев, когда необходимо записывать и читать данные из
//!   контейнера в виде массива данных.
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
  raw_data_t(const_pointer ap_data, size_type a_size);
  ~raw_data_t();
  inline reference operator[](size_type a_index);
  inline const_reference operator[](size_type a_index) const;
  const raw_data_t& operator=(const raw_data_t<T>& a_raw_data);
  inline pointer data();
  inline const_pointer data() const;
  inline pointer begin();
  inline const_pointer begin() const;
  inline pointer end();
  inline const_pointer end() const;
  inline size_type size() const;
  inline size_type capacity() const;
  inline bool empty() const;
  void reserve(size_type a_capacity);
  void resize(size_type a_size);
  void insert(pointer ap_pos, const_pointer ap_first, const_pointer ap_last);
  pointer insert(pointer ap_pos, const_reference a_value);
  pointer erase(pointer ap_first, pointer ap_last);
  pointer erase(pointer ap_pos);
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

  #ifdef IRS_LIB_CHECK
  value_type m_bad_index_value;
  #endif //IRS_LIB_CHECK
  size_type m_size;
  size_type m_capacity;
  pointer mp_data;

  void reserve_raw(size_type a_capacity);
};
template <class T>
raw_data_t<T>::raw_data_t(size_type a_size):
  #ifdef IRS_LIB_CHECK
  m_bad_index_value(value_type()),
  #endif //IRS_LIB_CHECK
  m_size(a_size),
  m_capacity((m_size > m_capacity_min)?m_size:
    static_cast<size_t>(m_capacity_min)),
  mp_data(IRS_LIB_NEW_ASSERT(value_type[m_capacity], MXDATAH_IDX))
{
  memsetex(mp_data, m_size);
}
template <class T>
raw_data_t<T>::raw_data_t(const raw_data_t<T>& a_raw_data):
  #ifdef IRS_LIB_CHECK
  m_bad_index_value(a_raw_data.m_bad_index_value),
  #endif //IRS_LIB_CHECK
  m_size(a_raw_data.m_size),
  m_capacity(a_raw_data.m_capacity),
  mp_data(IRS_LIB_NEW_ASSERT(value_type[m_capacity], MXDATAH_IDX))
{
  memcpyex(mp_data, a_raw_data.mp_data, m_size);
}
template <class T>
raw_data_t<T>::raw_data_t(const_pointer ap_data, size_type a_size):
  #ifdef IRS_LIB_CHECK
  m_bad_index_value(value_type()),
  #endif //IRS_LIB_CHECK
  m_size(a_size),
  m_capacity((m_size > m_capacity_min) ?
    m_size : static_cast<size_type>(m_capacity_min)),
  mp_data(IRS_LIB_NEW_ASSERT(value_type[m_capacity], MXDATAH_IDX))
{
  memcpyex(mp_data, ap_data, m_size);
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
  IRS_LIB_ERROR_IF(!check_index(*this, a_index), ec_standard, "");
  #ifdef IRS_LIB_CHECK
  if (!check_index(*this, a_index)) {
    return m_bad_index_value;
  }
  #endif //IRS_LIB_CHECK
  return mp_data[a_index];
}
template <class T>
inline typename raw_data_t<T>::const_reference
  raw_data_t<T>::operator[](size_type a_index) const
{
  IRS_LIB_ERROR_IF(!check_index(*this, a_index), ec_standard, "");
  #ifdef IRS_LIB_CHECK
  if (!check_index(*this, a_index)) {
    return m_bad_index_value;
  }
  #endif //IRS_LIB_CHECK
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
inline typename raw_data_t<T>::pointer raw_data_t<T>::begin()
{
  return data();
}
template <class T>
inline typename raw_data_t<T>::const_pointer raw_data_t<T>::begin() const
{
  return data();
}
template <class T>
inline typename raw_data_t<T>::pointer raw_data_t<T>::end()
{
  return begin() + size();
}
template <class T>
inline typename raw_data_t<T>::const_pointer raw_data_t<T>::end() const
{
  return begin() + size();
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
  size_type capacity_norm = max(a_capacity,
    static_cast<size_type>(m_capacity_min));
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
  if (new_size > m_size) {
    IRS_LIB_ERROR_IF_NOT((ap_last < data()) || (ap_first >= (data() + size())),
      ec_standard,
      "Блок памяти должен быть внешним по отношению к приемному контейнеру");
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
typename raw_data_t<T>::pointer raw_data_t<T>::insert(pointer ap_pos,
  const_reference a_value)
{
  IRS_LIB_ERROR_IF_NOT((ap_pos >= data()) && (ap_pos <= data()+size()),
    ec_standard, "Выход за пределы диапазона");
  // Позиция от нуля
  const size_type pos = ap_pos - data();
  const size_type old_size = m_size;
  resize(size() + 1);
  pointer p_pos = data() + pos;
  if ((old_size != 0) && (pos != old_size)) {
    pointer dest = p_pos + 1;
    const size_type move_bloc_size = old_size - pos;
    // Смещаем данные на одну позицию вправо
    memmoveex(dest, p_pos, move_bloc_size);
  }
  *p_pos = a_value;
  return p_pos;
}
template <class T>
typename raw_data_t<T>::pointer raw_data_t<T>::erase(
  pointer ap_first, pointer ap_last)
{
  IRS_LIB_ERROR_IF_NOT((ap_first >= data()) && (ap_first <= (data()+size())),
    ec_standard, "Выход за пределы диапазона");
  IRS_LIB_ERROR_IF_NOT((ap_last >= data()) && (ap_last <= (data()+size())),
    ec_standard, "Выход за пределы диапазона");
  size_type new_size = size() - (ap_last - ap_first);
  // Это условие нужно, чтобы исключить вызов функции memmoveex,
  // когда size() == 0, ибо тогда параметры ap_first, new_size могут
  // быть равны IRS_NULL, и функция memmoveex поднимет исключение
  if (size() > 0) {
    const size_type move_bloc_size = (data()+size()) - ap_last;
    memmoveex(ap_first, ap_last, move_bloc_size);
    resize(new_size);
  }
  return ap_first;
}
template <class T>
typename raw_data_t<T>::pointer raw_data_t<T>::erase(pointer ap_pos)
{
  IRS_LIB_ERROR_IF(empty(), ec_standard, "Контейнер уже пустой");
  IRS_LIB_ERROR_IF_NOT((ap_pos >= data()) && (ap_pos <end()), ec_standard,
    "Выход за пределы диапазона");
  return erase(ap_pos, ap_pos + 1);
}
template <class T>
inline void raw_data_t<T>::clear()
{
  m_size = 0;
}
template <class T>
inline void raw_data_t<T>::swap(raw_data_t<T>* ap_raw_data)
{
  IRS_STD swap(m_size, ap_raw_data->m_size);
  IRS_STD swap(m_capacity, ap_raw_data->m_capacity);
  IRS_STD swap(mp_data, ap_raw_data->mp_data);
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

//! \brief Контейнер представления сырых данных одного типа в виде другого
template <class T, class VT>
class raw_data_view_t
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  typedef VT viewed_value_type;
  typedef raw_data_t<viewed_value_type> viewed_type;

  raw_data_view_t(const raw_data_view_t& a_raw_data);
  explicit raw_data_view_t(viewed_type* ap_viewed_data = IRS_NULL);
  inline reference operator[](size_type a_index);
  inline const_reference operator[](size_type a_index) const;
  const raw_data_view_t& operator=(const raw_data_view_t& a_raw_data);
  inline void connect(viewed_type* ap_viewed_data);
  inline pointer data();
  inline const_pointer data() const;
  inline pointer begin();
  inline const_pointer begin() const;
  inline pointer end();
  inline const_pointer end() const;
  inline size_type size() const;
  inline size_type capacity() const;
  inline bool empty() const;
  void reserve(size_type a_capacity);
  void resize(size_type a_size);
  inline void clear();
  inline void swap(raw_data_view_t* ap_raw_data_view);
private:
  #ifdef IRS_LIB_CHECK
  value_type m_bad_index_value;
  #endif //IRS_LIB_CHECK
  viewed_type* mp_viewed_data;
};
template <class T, class VT>
raw_data_view_t<T, VT>::raw_data_view_t(
  const raw_data_view_t<T, VT>& a_raw_data_view
):
  #ifdef IRS_LIB_CHECK
  m_bad_index_value(value_type()),
  #endif //IRS_LIB_CHECK
  mp_viewed_data(a_raw_data_view.mp_viewed_data)
{
}
template <class T, class VT>
raw_data_view_t<T, VT>::raw_data_view_t(viewed_type* ap_viewed_data):
  #ifdef IRS_LIB_CHECK
  m_bad_index_value(value_type()),
  #endif //IRS_LIB_CHECK
  mp_viewed_data(ap_viewed_data)
{
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::reference
  raw_data_view_t<T, VT>::operator[](size_type a_index)
{
  IRS_LIB_ERROR_IF(!check_index(*this, a_index), ec_standard, "");
  #ifdef IRS_LIB_CHECK
  if (!check_index(*this, a_index)) {
    return m_bad_index_value;
  }
  #endif //IRS_LIB_CHECK
  pointer this_data = reinterpret_cast<pointer>(mp_viewed_data->data());
  return this_data[a_index];
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::const_reference
  raw_data_view_t<T, VT>::operator[](size_type a_index) const
{
  IRS_LIB_ERROR_IF(!check_index(*this, a_index), ec_standard, "");
  #ifdef IRS_LIB_CHECK
  if (!check_index(*this, a_index)) {
    return m_bad_index_value;
  }
  #endif //IRS_LIB_CHECK
  const_pointer this_data =
    reinterpret_cast<const_pointer>(mp_viewed_data->data());
  return this_data[a_index];
}
template <class T, class VT>
const raw_data_view_t<T, VT>& raw_data_view_t<T, VT>::operator=(
  const raw_data_view_t<T, VT>& a_raw_data_view)
{
  raw_data_view_t<T, VT> raw_data_view(a_raw_data_view);
  swap(&raw_data_view);
  return *this;
}
template <class T, class VT>
inline void raw_data_view_t<T, VT>::connect(viewed_type* ap_viewed_data)
{
  mp_viewed_data = ap_viewed_data;
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::pointer
  raw_data_view_t<T, VT>::data()
{
  pointer this_data = reinterpret_cast<pointer>(mp_viewed_data->data());
  return this_data;
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::const_pointer
  raw_data_view_t<T, VT>::data() const
{
  const_pointer this_data =
    reinterpret_cast<const_pointer>(mp_viewed_data->data());
  return this_data;
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::pointer raw_data_view_t<T, VT>::begin()
{
  return data();
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::const_pointer
  raw_data_view_t<T, VT>::begin() const
{
  return data();
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::pointer
  raw_data_view_t<T, VT>::end()
{
  return begin() + size();
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::const_pointer
  raw_data_view_t<T, VT>::end() const
{
  return begin() + size();
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::size_type
  raw_data_view_t<T, VT>::size() const
{
  return mp_viewed_data->size()*sizeof(viewed_value_type)/
    sizeof(value_type);
}
template <class T, class VT>
inline typename raw_data_view_t<T, VT>::size_type
  raw_data_view_t<T, VT>::capacity() const
{
  return mp_viewed_data->capacity()*sizeof(viewed_value_type)/
    sizeof(value_type);
}
template <class T, class VT>
inline bool raw_data_view_t<T, VT>::empty() const
{
  return mp_viewed_data->empty();
}
template <class T, class VT>
void raw_data_view_t<T, VT>::reserve(size_type a_capacity)
{
  mp_viewed_data->reserve(a_capacity*sizeof(value_type)/
    sizeof(viewed_value_type));
}
template <class T, class VT>
void raw_data_view_t<T, VT>::resize(size_type a_size)
{
  mp_viewed_data->resize(a_size*sizeof(value_type)/
    sizeof(viewed_value_type));
}
template <class T, class VT>
inline void raw_data_view_t<T, VT>::clear()
{
  mp_viewed_data->clear();
}
template <class T, class VT>
inline void raw_data_view_t<T, VT>::swap(
  raw_data_view_t<T, VT>* ap_raw_data_view)
{
  ::swap(mp_viewed_data, ap_raw_data_view->mp_viewed_data);
}
template <class T, class VT>
inline void swap(irs::raw_data_view_t<T, VT>& a_raw_data_view_first,
  irs::raw_data_view_t<T, VT>& a_raw_data_view_second)
{
  a_raw_data_view_first.swap(&a_raw_data_view_second);
}

//! \brief Класс контейнер для стандартного C-массива
template <class T>
class c_array_view_t
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  inline c_array_view_t(pointer ap_carray, size_type a_size);
  inline c_array_view_t(const c_array_view_t<T>& a_c_array_view);
  inline pointer data();
  inline const_pointer data() const;
  inline size_type size() const;
  inline pointer begin();
  inline const_pointer begin() const;
  inline pointer end();
  inline const_pointer end() const;
  inline reference operator[](size_type a_index);
  inline const_reference operator[](size_type a_index) const;
private:
  pointer mp_data;
  #ifdef IRS_LIB_CHECK
  value_type m_bad_index_value;
  #endif //IRS_LIB_CHECK
  size_type m_size;
};
template <class T>
inline c_array_view_t<T>::c_array_view_t(pointer ap_data,
  size_type a_size
):
  mp_data(ap_data),
  #ifdef IRS_LIB_CHECK
  m_bad_index_value(value_type()),
  #endif //IRS_LIB_CHECK
  m_size(a_size)
{
}
template <class T>
inline c_array_view_t<T>::c_array_view_t(const c_array_view_t<T> &a_c_array_view
):
  mp_data(a_c_array_view.mp_data),
  #ifdef IRS_LIB_CHECK
  m_bad_index_value(a_c_array_view.m_bad_index_value),
  #endif //IRS_LIB_CHECK
  m_size(a_c_array_view.m_size)
{
}
template <class T>
inline typename c_array_view_t<T>::pointer c_array_view_t<T>::data()
{
  return mp_data;
}
template <class T>
inline typename c_array_view_t<T>::const_pointer c_array_view_t<T>::data() const
{
  return mp_data;
}
template <class T>
inline typename c_array_view_t<T>::size_type c_array_view_t<T>::size() const
{
  return m_size;
}
template <class T>
inline typename c_array_view_t<T>::pointer c_array_view_t<T>::begin()
{
  return data();
}
template <class T>
inline typename c_array_view_t<T>::const_pointer
  c_array_view_t<T>::begin() const
{
  return data();
}
template <class T>
inline typename c_array_view_t<T>::pointer c_array_view_t<T>::end()
{
  return begin() + size();
}
template <class T>
inline typename c_array_view_t<T>::const_pointer
  c_array_view_t<T>::end() const
{
  return begin() + size();
}
template <class T>
inline typename c_array_view_t<T>::reference c_array_view_t<T>::operator[](
  size_type a_index)
{
  IRS_LIB_ERROR_IF(!check_index(*this, a_index), ec_standard, "");
  #ifdef IRS_LIB_CHECK
  if (!check_index(*this, a_index)) {
    return m_bad_index_value;
  }
  #endif //IRS_LIB_CHECK
  return mp_data[a_index];
}
template <class T>
inline typename c_array_view_t<T>::const_reference
c_array_view_t<T>::operator[](size_type a_index) const
{
  IRS_LIB_ERROR_IF(!check_index(*this, a_index), ec_standard, "");
  #ifdef IRS_LIB_CHECK
  if (!check_index(*this, a_index)) {
    return m_bad_index_value;
  }
  #endif //IRS_LIB_CHECK
  return mp_data[a_index];
}

//! \brief Копирование с проверкой
template <class src_type, class dest_type>
inline void mem_copy(const src_type& a_src_data, size_t a_src_index,
  dest_type& a_dest_data, size_t a_dest_index, size_t a_size)
{
  #if defined(IRS_LIB_CHECK) || defined(IRS_LIB_DEBUG)
  bool is_out_of_range = !check_index(a_src_data, a_src_index, a_size) ||
    !check_index(a_dest_data, a_dest_index, a_size);
  #endif //defined(IRS_LIB_CHECK) || defined(IRS_LIB_DEBUG)
  IRS_LIB_ERROR_IF(is_out_of_range, ec_standard,
    "Выход за диапазон в функции mem_copy");
  #ifdef IRS_LIB_CHECK
  if (is_out_of_range) {
    return;
  }
  #endif //IRS_LIB_CHECK

  memcpyex(a_dest_data.data() + a_dest_index,
    a_src_data.data() + a_src_index, a_size);
}

// Очередь сырых данных
template <class T, class Al = allocator<T> >
class deque_data_t
{
public:
#if __cplusplus < 201703L
  typedef typename Al::template rebind<T>::other _Alloc;
#else
  typedef typename std::allocator_traits<Al>::template rebind_alloc<T> _Alloc;
#endif
  typedef _Alloc allocator_type;

  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  deque_data_t(size_type a_size = 0);
  deque_data_t(const deque_data_t<T, Al>& a_deque_data);
  ~deque_data_t();
  inline reference operator[](size_type a_index);
  inline const_reference operator[](size_type a_index) const;
  inline value_type& front();
  inline const value_type& front() const;
  inline value_type& back();
  inline const value_type& back() const;
  const deque_data_t& operator=(const deque_data_t<T, Al>& a_raw_data);
  inline size_type size() const;
  inline size_type capacity() const;
  inline bool empty() const;
  void reserve(size_type a_capacity);
  void buf_reserve(size_type a_capacity);
  void resize(size_type a_size);
  void buf_resize(size_type a_size);
  void push_back(const value_type& a_value);
  void push_back(const_pointer ap_first, const_pointer ap_last);
  void push_front(const value_type& a_value);
  void push_front(const_pointer ap_first, const_pointer ap_last);
  void pop_back(size_type = 1);
  void pop_front(size_type = 1);
  void copy_to(size_type a_pos, size_type a_size,
    pointer ap_dest_first);
  inline void clear();
  inline void swap(deque_data_t<T, Al>* ap_deque_data_src);
private:
  void reserve_buf(size_type a_capacity);
  enum {
    #ifdef __ICCAVR__
    m_capacity_min = 1
    #else //__ICCAVR__
    m_capacity_min = 16
    #endif //__ICCAVR__
  };
  _Alloc m_alloc;

  size_type m_ring_size;
  size_type m_capacity;
  pointer mp_buf;
  size_type m_ring_begin_pos;
};

template <class T, class Al>
irs::deque_data_t<T, Al>::deque_data_t(size_type a_size):
  m_alloc(Al()),
  m_ring_size(a_size),
  m_capacity((m_ring_size > m_capacity_min) ? m_ring_size :
    static_cast<size_t>(m_capacity_min)),
  mp_buf(m_alloc.allocate(m_capacity)),
  m_ring_begin_pos(0)
{
  memsetex(mp_buf, m_ring_size);
}

template <class T, class Al>
irs::deque_data_t<T, Al>::deque_data_t(const deque_data_t<T, Al>& a_deque_data):
  m_alloc(a_deque_data.m_alloc),
  m_ring_size(a_deque_data.m_ring_size),
  m_capacity(a_deque_data.m_capacity),
  mp_buf(m_alloc.allocate(m_capacity)),
  m_ring_begin_pos(a_deque_data.m_ring_begin_pos)
{
  memcpyex(mp_buf, a_deque_data.mp_buf, m_capacity);
}

template <class T, class Al>
irs::deque_data_t<T, Al>::~deque_data_t()
{
  m_alloc.deallocate(mp_buf, m_capacity);
}

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::reference
  irs::deque_data_t<T, Al>::front()
{
  return operator[](0);
}

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::const_reference
  irs::deque_data_t<T, Al>::front() const
{
  return operator[](0);
}

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::reference
  irs::deque_data_t<T, Al>::back()
{
  return operator[](size() - 1);
}

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::const_reference
  irs::deque_data_t<T, Al>::back() const
{
  return operator[](size() - 1);
}

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::reference
  irs::deque_data_t<T, Al>::operator[](size_type a_index)
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

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::const_reference
  irs::deque_data_t<T, Al>::operator[](size_type a_index) const
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

template <class T, class Al>
const typename irs::deque_data_t<T, Al>&
  irs::deque_data_t<T, Al>::operator=(const deque_data_t<T, Al>& a_deque_data)
{
  deque_data_t<T, Al> deque_data(a_deque_data);
  swap(&deque_data);
  return *this;
}

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::size_type
  irs::deque_data_t<T, Al>::size() const
{
  return m_ring_size;
}

template <class T, class Al>
inline typename irs::deque_data_t<T, Al>::size_type
  irs::deque_data_t<T, Al>::capacity() const
{
  return m_capacity;
}

template <class T, class Al>
inline bool irs::deque_data_t<T, Al>::empty() const
{
  return m_ring_size == 0;
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::reserve(size_type a_capacity)
{
  size_type capacity_norm = max(a_capacity,
    static_cast<size_type>(m_capacity_min));
  size_type capacity_new = max(capacity_norm, m_ring_size);
  reserve_buf(capacity_new);
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::resize(size_type a_size)
{
  size_type new_size = a_size;
  if (new_size > m_capacity) {
    size_type new_capacity = new_size*2;
    reserve_buf(new_capacity);
  } else {
    //
  }
  m_ring_size = new_size;
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::push_back(const value_type& a_value)
{
  push_back(&a_value, &a_value + 1);
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::push_back(
  const_pointer ap_first, const_pointer ap_last)
{
  // Размер вставляемых данных
  const size_type insert_data_size = (ap_last - ap_first);
  const size_type old_ring_size = m_ring_size;
  const size_type new_ring_size = insert_data_size + m_ring_size;
  resize(new_ring_size);
  const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;
  if (buf_right_part_size < old_ring_size) {
    // Старые элементы заполняют всю правую часть
    // Вставляем элементы в левую часть, после старых элементов
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

template <class T, class Al>
void irs::deque_data_t<T, Al>::push_front(const value_type& a_value)
{
  push_front(&a_value, &a_value + 1);
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::push_front(
  const_pointer ap_first, const_pointer ap_last)
{
  // Размер вставляемых данных
  const size_type insert_data_size = (ap_last - ap_first);
  const size_type old_ring_size = m_ring_size;
  const size_type new_ring_size = insert_data_size + m_ring_size;
  resize(new_ring_size);
  const size_type buf_right_part_size = m_capacity - m_ring_begin_pos;

  if (buf_right_part_size < old_ring_size) {
    // Старые элементы заполняют всю правую часть
    // Вставляем элементы в левую половину, перед правой частью
    pointer dest = mp_buf + (m_ring_begin_pos - insert_data_size);
    memcpyex(dest, ap_first, insert_data_size);
    m_ring_begin_pos -= insert_data_size;
  } else {
    const size_type size_copy_data_to_buf_left_part =
      min(insert_data_size, m_ring_begin_pos);
    pointer dest = mp_buf +
      (m_ring_begin_pos - size_copy_data_to_buf_left_part);
    const_pointer source = ap_first +
      (insert_data_size - size_copy_data_to_buf_left_part);
    memcpyex(dest, source, size_copy_data_to_buf_left_part);
    m_ring_begin_pos -= size_copy_data_to_buf_left_part;

    const size_type size_copy_data_to_buf_right_part =
      insert_data_size - size_copy_data_to_buf_left_part;
    if (size_copy_data_to_buf_right_part > 0) {
      dest = mp_buf + (m_capacity - size_copy_data_to_buf_right_part);
      memcpyex(dest, ap_first, size_copy_data_to_buf_right_part);
      m_ring_begin_pos = m_capacity - size_copy_data_to_buf_right_part;
    }
  }
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::pop_back(size_type a_size)
{
  IRS_LIB_ASSERT(a_size <= m_ring_size);
  const size_type erase_data_size = min(a_size, m_ring_size);
  m_ring_size -= erase_data_size;
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::pop_front(size_type a_size)
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

template <class T, class Al>
void irs::deque_data_t<T, Al>::copy_to(size_type a_pos, size_type a_size,
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

template <class T, class Al>
inline void irs::deque_data_t<T, Al>::clear()
{
  m_ring_size = 0;
  m_ring_begin_pos = 0;
}

template <class T, class Al>
inline void irs::deque_data_t<T, Al>::swap(deque_data_t<T, Al>* ap_deque_data_src)
{
  ::swap(m_alloc, ap_deque_data_src->m_alloc);
  ::swap(m_ring_size, ap_deque_data_src->m_ring_size);
  ::swap(m_capacity, ap_deque_data_src->m_capacity);
  ::swap(mp_buf, ap_deque_data_src->mp_buf);
  ::swap(m_ring_begin_pos, ap_deque_data_src->m_ring_begin_pos);
}

template <class T, class Al>
void irs::deque_data_t<T, Al>::reserve_buf(size_type a_capacity)
{
  IRS_LIB_ASSERT(a_capacity >= m_capacity_min);
  if (a_capacity > m_capacity) {
    size_type new_capacity = a_capacity;
    pointer p_new_buf = m_alloc.allocate(new_capacity);
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
    m_alloc.deallocate(mp_buf, m_capacity);
    mp_buf = p_new_buf;
    m_capacity = new_capacity;
    m_ring_size = new_ring_size;
    m_ring_begin_pos = 0;
  } else {
    // Запрашиваемое количество памяти уже выделено
  }
}

template <class T, class Al>
inline void swap(deque_data_t<T, Al>& a_deque_data_first,
  deque_data_t<T, Al>& a_deque_data_second)
{
  a_deque_data_first->swap(&a_deque_data_second);
}

#ifdef IRS_LIB_DEBUG
template<class T = int, class Al = allocator<T> >
bool deque_data_test()
{
  typedef size_t size_type;
  typedef T testing_type;
  bool test_success = true;
  irs::deque_data_t<testing_type, Al> deque_data;
  // Создаем и заполняем буфер тестовыми данными
  const size_type buf_size = 100;
  testing_type test_data_buf[buf_size];
  for (size_type i = 0; i < buf_size; i++) {
    test_data_buf[i] = i;
  }
  // Тест на вставку, копирование и удаление данных
  deque_data.push_back(test_data_buf, test_data_buf + buf_size);
  test_success = (buf_size == deque_data.size());
  IRS_LIB_ASSERT(test_success);
  if (test_success) {
    testing_type buf[buf_size];
    memset(buf, 0, buf_size);
    deque_data.copy_to(0, buf_size, buf);
    test_success = (memcmpex(buf, test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    const size_type pop_data_size = buf_size/2;
    deque_data.pop_front(pop_data_size);
    deque_data.push_back(test_data_buf, test_data_buf + buf_size);
    deque_data.push_back(test_data_buf, test_data_buf + buf_size);
    deque_data.pop_front(buf_size - pop_data_size);
    deque_data.pop_back(buf_size);
    test_success = (deque_data.size() == buf_size);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    testing_type buf[buf_size];
    memset(buf, 0, buf_size);
    deque_data.copy_to(0, buf_size, buf);
    test_success = (memcmpex(buf, test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    deque_data.reserve(buf_size * 10);
    deque_data.resize(buf_size);
    testing_type buf[buf_size];
    memset(buf, 0, buf_size);
    deque_data.copy_to(0, buf_size, buf);
    test_success = (memcmpex(buf, test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    deque_data.clear();
    test_success = (deque_data.size() == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    deque_data.reserve(0);
    deque_data.push_back(test_data_buf, test_data_buf + buf_size);
    test_success = (buf_size == deque_data.size());
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    testing_type buf[buf_size];
    memset(buf, 0, buf_size);
    deque_data.copy_to(0, buf_size, buf);
    test_success = (memcmpex(buf, test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    for (size_type i = 0; i < buf_size; i++) {
      if (deque_data[i] != test_data_buf[i]) {
        test_success = false;
        break;
      } else {
        // Продолжаем проверку
      }
    }
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    irs::deque_data_t<testing_type, Al> deque_data_second;
    deque_data_second = deque_data;
    if (deque_data_second.size() == deque_data.size()) {
      for (size_type i = 0; i < buf_size; i++) {
        if (deque_data[i] != deque_data_second[i]) {
          test_success = false;
          break;
        } else {
          // Продолжаем проверку
        }
      }
    } else {
      test_success = false;
    }
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  return test_success;
}

#endif // IRS_LIB_DEBUG

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
  #if defined(IRS_FULL_STDCPPLIB_SUPPORT) || \
    (defined(__ICCARM__) && __VER__ >= 7000000)
  template <class T2>
  handle_t(const handle_t<T2>& a_handle);
  #endif // IRS_FULL_STDCPPLIB_SUPPORT || ICCARM ver. >= 7.0
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
  void clear();
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
template<class T>
void handle_t<T>::clear()
{
  if (mp_rep) {
    IRS_LIB_ASSERT(mp_rep->counter > 0);
    mp_rep->counter--;
    if (mp_rep->counter == 0) {
      IRS_LIB_DELETE_ASSERT(mp_rep);
    } else {
      mp_rep = IRS_NULL;
    }
  }
}
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
    mp_rep->counter++;
  }
}
#if defined(IRS_FULL_STDCPPLIB_SUPPORT) || \
  (defined(__ICCARM__) && __VER__ >= 7000000)
template <class T>
template <class T2>
handle_t<T>::handle_t(const handle_t<T2>& a_handle):
  mp_rep(reinterpret_cast<const handle_t<T>&>(a_handle).mp_rep)
{
  #ifdef IRS_LIB_DEBUG
  // Проверяем, что преобразование из T2 в Т является допустимым
  T* p_other = static_cast<T*>(a_handle.get());
  if (get() == p_other) {
  #endif // IRS_LIB_DEBUG
    if (mp_rep != IRS_NULL) {
      mp_rep->counter++;
    }
  #ifdef IRS_LIB_DEBUG
  }
  #endif //IRS_LIB_DEBUG
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT || ICCARM ver. >= 7.0
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
  handle_t handle_copy(a_handle);
  swap(handle_copy);
}
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template <class T>
template <class T2>
void handle_t<T>::operator=(const handle_t<T2>& a_handle)
{
  handle_t handle_copy(a_handle);
  swap(handle_copy);
}
#endif // IRS_FULL_STDCPPLIB_SUPPORT
template <class T>
T& handle_t<T>::operator*() const
{
  IRS_LIB_ASSERT((mp_rep != IRS_NULL) && (mp_rep->object != IRS_NULL));
  return *mp_rep->object;
}
template <class T>
T* handle_t<T>::operator->() const
{
  IRS_LIB_ASSERT((mp_rep != IRS_NULL) && (mp_rep->object != IRS_NULL));
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
  IRS_STD swap(mp_rep, a_handle.mp_rep);
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
