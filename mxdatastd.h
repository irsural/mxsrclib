//! \file
//! \ingroup in_out_group
//! \brief Стандартаная библиотека ИРС, класс mxdata_t
//!
//! Дата: 10.08.2010\n
//! Дата создания: 26.09.2009

#ifndef MXDATASTDH
#define MXDATASTDH

#include <irsdefs.h>

#include <irslimits.h>
//#include <irserror.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup in_out_group
//! @{

// Абстактный базовый класс для массива данных
class mxdata_t
{
public:
  virtual ~mxdata_t() {}
  virtual irs_uarc size() = 0;
  virtual irs_bool connected() = 0;
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size) = 0;
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size) = 0;
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index) = 0;
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index) = 0;
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index) = 0;
  virtual inline void write_bit(irs_uarc a_index, irs_uarc a_bit_index,
    irs_bool a_bit);
  virtual void tick() = 0;
};
inline void mxdata_t::write_bit(irs_uarc a_index, irs_uarc a_bit_index,
  irs_bool a_bit)
{
  if (a_bit) {
    set_bit(a_index, a_bit_index);
  } else {
    clear_bit(a_index, a_bit_index);
  }
}

// Локальные данные
class local_data_t: public mxdata_t
{
public:
  inline local_data_t(irs_uarc a_size);
  inline void resize(irs_size_t a_size);
  virtual inline irs_uarc size();
  virtual inline irs_bool connected();
  virtual inline void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual inline void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size);
  virtual inline irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual inline void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual inline void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual inline void tick();
private:
  vector<irs_u8> m_data;

  local_data_t();
};
inline local_data_t::local_data_t(irs_uarc a_size):
  m_data(a_size)
{
}
inline void irs::local_data_t::resize(irs_size_t a_size)
{
  m_data.resize(a_size);
}
inline irs_uarc irs::local_data_t::size()
{
  return m_data.size();
}
inline irs_bool irs::local_data_t::connected()
{
  return irs_true;
}
inline void irs::local_data_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index + a_size > m_data.size()) {
    fill(ap_buf, ap_buf + a_size, 0);
    return;
  }
  vector<irs_u8>::iterator it_begin = m_data.begin() + a_index;
  copy(it_begin, it_begin + a_size, ap_buf);
}
inline void irs::local_data_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index + a_size > m_data.size()) return;
  vector<irs_u8>::iterator it_begin = m_data.begin() + a_index;
  copy(ap_buf, ap_buf + a_size, it_begin);
}
inline irs_bool irs::local_data_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  return (m_data[a_index]&(1 << a_bit_index)) ? irs_true : irs_false;
}
inline void irs::local_data_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  m_data[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
}
inline void irs::local_data_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  m_data[a_index] &= static_cast<irs_u8>(~(1 << a_bit_index));
}
inline void irs::local_data_t::tick()
{
}

// Наложение встроенных типов на mxdata_t
template <class T, int size = sizeof(T), bool check = false>
class conn_data_t
{
public:
  conn_data_t();
  conn_data_t(irs::mxdata_t *a_data, irs_uarc a_index);
  conn_data_t(irs::mxdata_t *a_data, irs_uarc a_index, irs_uarc &a_ret_index);
  irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_index);
  irs::mxdata_t *data() const;
  irs_uarc index() const;
  conn_data_t& operator++();
  T operator++(int);
  conn_data_t& operator--();
  T operator--(int);
  conn_data_t& operator+=(const T& a_value);
  conn_data_t& operator-=(const T& a_value);

  const T &operator=(const T &a_elem);
  const conn_data_t& operator=(const conn_data_t& a_conn_data);
  operator T() const;
private:
  irs_uarc m_index;
  irs::mxdata_t *m_data;
  //T m_elem;
  //irs_bool m_connected;
};
template <class T, int size, bool check>
conn_data_t<T, size, check>::conn_data_t():
  m_index(0),
  m_data(IRS_NULL)//,
  //m_elem()
  //m_connected(irs_false)
{
}
template <class T, int size, bool check>
conn_data_t<T, size, check>::conn_data_t(irs::mxdata_t *a_data,
  irs_uarc a_index):
  m_index(0),
  m_data(IRS_NULL)
  //m_elem()
  //m_connected(irs_false)
{
  connect(a_data, a_index);
}
template <class T, int size, bool check>
conn_data_t<T, size, check>::conn_data_t(irs::mxdata_t *a_data,
  irs_uarc a_index, irs_uarc &a_ret_index):
  m_index(0),
  m_data(IRS_NULL)//,
  //m_elem()
  //m_connected(irs_false)
{
  a_ret_index = connect(a_data, a_index);
}
template <class T, int size, bool check>
irs_uarc conn_data_t<T, size, check>::connect(irs::mxdata_t *a_data,
  irs_uarc a_index)
{
  //if (a_data) {
    //if (a_index + size <= a_data->size()) {
      //m_connected = irs_true;
      m_index = a_index;
      m_data = a_data;
      return a_index + size;
    //} else {
      //m_connected = irs_true;
    //}
  //}
  //return a_index;
}
template <class T, int size, bool check>
const T& conn_data_t<T, size, check>::operator=(const T &a_elem)
{
  //if (check) if (!m_connected) return m_elem;
  m_data->write(reinterpret_cast<const irs_u8 *>(&a_elem), m_index, size);
  return a_elem;
}
template <class T, int size, bool check>
const conn_data_t<T, size, check>& conn_data_t<T, size, check>::operator=(
  const conn_data_t<T, size, check>& a_conn_data)
{
  T elem = a_conn_data;
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->write(p_elem_u8, m_index, size);
  return *this;
}
template <class T, int size, bool check>
conn_data_t<T, size, check>::operator T() const
{
  //if (check) if (!m_connected) return m_elem;
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  if (irs::numeric_limits<T>::is_floating) {
    m_data->read(p_elem_u8, m_index, size);
    if (irs::numeric_limits<T>::is_inf_or_nan(p_elem_u8)) {
      fill(p_elem_u8, p_elem_u8 + sizeof(T), irs_u8());
      elem = irs::numeric_limits<T>::mark_bad();
    }
  } else {
    m_data->read(p_elem_u8, m_index, size);
  }
  return elem;
}
template <class T, int size, bool check>
irs::mxdata_t *conn_data_t<T, size, check>::data() const
{
  return m_data;
}
template <class T, int size, bool check>
irs_uarc conn_data_t<T, size, check>::index() const
{
  return m_index;
}
template <class T, int size, bool check>
conn_data_t<T, size, check>& conn_data_t<T, size, check>::operator++()
{
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->read(p_elem_u8, m_index, size);
  elem++;
  m_data->write(p_elem_u8, m_index, size);
  return *this;
}
template <class T, int size, bool check>
T conn_data_t<T, size, check>::operator++(int)
{
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->read(p_elem_u8, m_index, size);
  T elem_before = elem;
  elem++;
  m_data->write(p_elem_u8, m_index, size);
  return elem_before;
}
template <class T, int size, bool check>
conn_data_t<T, size, check>& conn_data_t<T, size, check>::operator--()
{
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->read(p_elem_u8, m_index, size);
  elem--;
  m_data->write(p_elem_u8, m_index, size);
  return *this;
}
template <class T, int size, bool check>
T conn_data_t<T, size, check>::operator--(int)
{
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->read(p_elem_u8, m_index, size);
  T elem_before = elem;
  elem--;
  m_data->write(p_elem_u8, m_index, size);
  return elem_before;
}
template <class T, int size, bool check>
conn_data_t<T, size, check>& conn_data_t<T, size, check>::operator+=(
  const T& a_value)
{
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->read(p_elem_u8, m_index, size);
  elem += a_value;
  m_data->write(p_elem_u8, m_index, size);
  return *this;
}
template <class T, int size, bool check>
conn_data_t<T, size, check>& conn_data_t<T, size, check>::operator-=(
  const T& a_value)
{
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->read(p_elem_u8, m_index, size);
  elem -= a_value;
  m_data->write(p_elem_u8, m_index, size);
  return *this;
}

// Наложение массива на mxdata_t
template <class T, int elem_size = sizeof(T), bool check = false>
class array_data_t
{
public:

  array_data_t();
  // a_data_index - смещение в mxdata_t в байтах
  // a_array_size - количество элементов типа T
  array_data_t(irs::mxdata_t *a_data, irs_uarc a_data_index,
    irs_uarc a_array_size);
  array_data_t(irs::mxdata_t *a_data, irs_uarc a_data_index,
    irs_uarc a_array_size, irs_uarc &a_ret_data_index);
  irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_data_index,
    irs_uarc a_array_size);
  conn_data_t<T, elem_size, check> &operator[](irs_uarc a_array_index);
private:
  irs_uarc m_data_index;
  irs_uarc m_array_size;
  irs::mxdata_t *m_data;
  conn_data_t<T, elem_size, check> m_elem;
  //irs_bool m_connected;
};
template <class T, int elem_size, bool check>
array_data_t<T, elem_size, check>::array_data_t():
  m_data_index(0),
  m_array_size(0),
  m_data(IRS_NULL),
  m_elem()//,
  //m_connected(irs_false)
{
}
template <class T, int elem_size, bool check>
array_data_t<T, elem_size, check>::array_data_t(irs::mxdata_t *a_data,
  irs_uarc a_data_index, irs_uarc a_array_size):
  m_data_index(0),
  m_array_size(0),
  m_data(IRS_NULL),
  m_elem()//,
  //m_connected(irs_false)
{
  connect(a_data, a_data_index, a_array_size);
}
template <class T, int elem_size, bool check>
array_data_t<T, elem_size, check>::array_data_t(irs::mxdata_t *a_data,
  irs_uarc a_data_index, irs_uarc a_array_size, irs_uarc &a_ret_data_index):
  m_data_index(0),
  m_array_size(0),
  m_data(IRS_NULL),
  m_elem()//,
  //m_connected(irs_false)
{
  a_ret_data_index = connect(a_data, a_data_index, a_array_size);
}
template <class T, int elem_size, bool check>
irs_uarc array_data_t<T, elem_size, check>::connect(irs::mxdata_t *a_data,
  irs_uarc a_data_index, irs_uarc a_array_size)
{
  //if (a_data) {
    //if (a_data_index + elem_size*a_array_size <= a_data->size()) {
      //m_connected = irs_true;
      m_data_index = a_data_index;
      m_array_size = a_array_size;
      m_data = a_data;
      return a_data_index + a_array_size*elem_size;
    //} else {
      //m_connected = irs_true;
    //}
  //}
  //return a_data_index;
}
template <class T, int elem_size, bool check>
conn_data_t<T, elem_size, check> &array_data_t<T, elem_size, check>::
operator[](irs_uarc a_array_index)
{
  m_elem.connect(m_data, m_data_index + a_array_index*elem_size);
  return m_elem;
}
// Наложение бита на mxdata_t
class bit_data_t
{
public:
  typedef int bit_t;

  inline bit_data_t(irs::mxdata_t *a_data = 0, irs_uarc a_index = 0,
    irs_uarc a_bit_index = 0);
  inline irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_index,
    irs_uarc a_bit_index);
  inline irs::mxdata_t *data() const;
  inline irs_uarc index() const;
  inline irs_uarc bit_index() const;

  inline const bit_data_t& operator=(const bit_data_t& a_elem);
  inline bit_t operator=(bit_t a_elem);
  inline operator bit_t() const;
private:
  static const irs_uarc m_max_bit_count = 8;

  irs::mxdata_t *mp_data;
  irs_uarc m_index;
  irs_uarc m_bit_index;
};
inline bit_data_t::bit_data_t(mxdata_t *ap_data, irs_uarc a_index,
  irs_uarc a_bit_index):
  mp_data(ap_data),
  m_index(a_index),
  m_bit_index(a_bit_index)
{
}
inline irs_uarc bit_data_t::connect(mxdata_t *ap_data, irs_uarc a_index,
  irs_uarc a_bit_index)
{
  //bit_t data_valid = (ap_data != 0);
  //bit_t index_valid = a_index < ap_data->size();
  //bit_t bit_index_valid = a_bit_index < m_max_bit_count;
  //if (data_valid && index_valid && bit_index_valid) {
    mp_data = ap_data;
    m_index = a_index;
    m_bit_index = a_bit_index;
  //} else {
    //throw conn_fail_e();
  //}
  return a_index;
}
inline const bit_data_t& bit_data_t::operator=
  (const bit_data_t& a_elem)
{
  if (a_elem) {
    mp_data->set_bit(m_index, m_bit_index);
  } else {
    mp_data->clear_bit(m_index, m_bit_index);
  }
  return a_elem;
}
inline bit_data_t::bit_t bit_data_t::operator=
  (bit_data_t::bit_t a_elem)
{
  if (a_elem) {
    mp_data->set_bit(m_index, m_bit_index);
  } else {
    mp_data->clear_bit(m_index, m_bit_index);
  }
  return a_elem;
}
inline bit_data_t::operator bit_t() const
{
  bit_t ret_bit = 0;
  if (mp_data->bit(m_index, m_bit_index)) ret_bit = 1;
  return ret_bit;
}
inline irs::mxdata_t *bit_data_t::data() const
{
  return mp_data;
}
inline irs_uarc bit_data_t::index() const
{
  return m_index;
}
inline irs_uarc bit_data_t::bit_index() const
{
  return m_bit_index;
}

//! @}

} //namespace irs

#endif //MXDATASTDH
