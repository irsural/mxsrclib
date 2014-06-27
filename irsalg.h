//! \file
//! \ingroup signal_processing_group
//! \brief Алгоритмы
//!
//! Дата: 09.09.2011\n
//! Ранняя дата: 2.09.2009

#ifndef IRSALGH
#define IRSALGH

// Номер файла
#define IRSALGH_IDX 10

#include <irsdefs.h>

#include <irserror.h>
#include <mxdata.h>
#include <irslimits.h>

#include <irsfinal.h>

//! \addtogroup signal_processing_group
//! @{

// Реализация алгоритма кольцевого буфера
class alg_ring {
  //! \brief Текущее начало буфера
  irs_u32 f_begin;
  //! \brief Текущая длина буфера
  irs_u32 f_size;
  //! \brief Максимальная длина буфера
  irs_u32 f_size_max;
  //! \brief Кольцевой буфер
  void **f_buf_ring;
  //! \brief Ошибка в конструкторе
  irs_bool f_create_error;

  //! \brief Запрет конструктора по умолчанию
  alg_ring();
  //! \brief Увеличение начала буфера на 1 с учетом границы
  void begin_inc();
  //! \brief Вычисление конца буфера
  irs_u32 get_end();

public:
  //! \brief Конструктор
  alg_ring(int size_max);
  //! brief Деструктор
  ~alg_ring();
  //! brief Чтение и удаление первого элемента буфера
  void *read_and_remove_first();
  //! \brief Добавление нового элемента в конец буфера
  void *add_last_and_get_removed(void *buf_elem);
  //! \brief Ошибка в конструкторе
  irs_bool create_error();

  //! \brief Чтение длины буфера
  irs_u32 get_size()const {
    return f_size;
  }
  //! \brief Чтение элемента буфера
  void *operator[](irs_u32 index)const;
};

//! @}
//------------------------------------------------------------------------------
namespace irs {

  //! \ingroup signal_processing_group
  //! \brief Расчет СКО
  // template <class data_t = double, class calc_t = double>
  template<class data_t, class calc_t>
  class sko_calc_t {
    irs_u32 m_count;
    deque<data_t> m_val_array;
    calc_t m_average;
    bool m_accepted_average;
    calc_t sum()const;
    sko_calc_t();
  public:
    sko_calc_t(irs_u32 a_count);
    ~sko_calc_t();
    void clear();
    void add(data_t a_val);
    operator data_t()const;
    data_t relative()const;
    data_t average()const;
    void resize(irs_u32 a_count);
    void set_average(data_t a_average);
    void clear_average();
    irs_u32 size();
  };

  // Версия sko_calc_t для sko_calc_t<double, double>
  // Т. к. C++ Builder 4 криво работает с параметрами шаблона по умолчанию
  class sko_calc_dbl_t : public sko_calc_t<double, double> {
  public:
    sko_calc_dbl_t(irs_u32 a_count) : sko_calc_t<double, double>(a_count) {
    }
  private:
    sko_calc_dbl_t();
  };

} // namespace irs

template<class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>::sko_calc_t(irs_u32 a_count):
  m_count((a_count >= 1)?a_count:1),
  m_val_array(),
  m_average(0),
  m_accepted_average(false)
{
}

template<class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>::~sko_calc_t() {
  clear();
}

template<class data_t, class calc_t>
calc_t irs::sko_calc_t<data_t, calc_t>::sum()const {
  const size_t size = m_val_array.size();
  calc_t sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum += m_val_array[i];
  }
  return sum;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::resize(irs_u32 a_count) {
  m_count = ((a_count >= 1)?a_count:1);
  if (m_val_array.size() > m_count) {
    m_val_array.resize(m_count);
  }
}

template<class data_t, class calc_t>
irs_u32 irs::sko_calc_t<data_t, calc_t>::size() {
  irs_u32 size = static_cast<irs_u32>(m_val_array.size());
  return size;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::set_average(data_t a_average)
{
  m_average = static_cast<calc_t>(a_average);
  m_accepted_average = true;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::clear_average()
{
  m_accepted_average = false;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::clear() {
  m_val_array.resize(0);
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::add(data_t a_val) {
  const size_t size = m_val_array.size();
  if (size < m_count) {
    m_val_array.push_back(a_val);
  }
  else if (size == m_count) {
    m_val_array.pop_front();
    m_val_array.push_back(a_val);
  }
  else if (size > m_count) {
    size_t margin = size - m_count;
    for (size_t i = 0; i < margin; i++) {
      m_val_array.pop_front();
    }
  }
}

template<class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>:: operator data_t()const {
  const size_t size = m_val_array.size();
  if (size) {
    calc_t square_sum = 0.;
    calc_t average = 0;
    if (!m_accepted_average) {
      average = sum() / size;
    } else {
      average = m_average;
    }
    for (size_t i = 0; i < size; i++) {
      calc_t val = static_cast<calc_t> (m_val_array[i]);
      val -= average;
      square_sum += val * val;
    }
    calc_t calced_sko = sqrt(square_sum / size);
    return calced_sko;
  }
  return 0.;
}

template<class data_t, class calc_t>
data_t irs::sko_calc_t<data_t, calc_t>::relative()const {
  const size_t size = m_val_array.size();
  if (size) {
    calc_t average = sum() / size;
    calc_t sko = *this;
    if (average != 0) {
      calc_t sko_reltive = sko / average;
      return sko_reltive;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

template<class data_t, class calc_t>
data_t irs::sko_calc_t<data_t, calc_t>::average()const {
  size_t size = m_val_array.size();
  if (size == 0) {
    size = 1;
  }
  return sum() / size;
}

namespace irs {
  template<class data_t, class calc_t>
  calc_t sko_calc(data_t* ap_buff, size_t a_size) {
    calc_t result = 0;
    if ((ap_buff != IRS_NULL) && (a_size != 0)) {
      irs::c_array_view_t<data_t> buff =
        irs::c_array_view_t<data_t>(
        reinterpret_cast<data_t*>(ap_buff), a_size);
      calc_t sum = 0;
      for (size_t i = 0; i < a_size; i++) {
        sum += static_cast<calc_t>(buff[i]);
      }
      calc_t average = sum / a_size;
      calc_t val = 0;
      calc_t square_sum = 0;
      for (size_t i = 0; i < a_size; i++) {
        val = static_cast<calc_t>(buff[i]);
        val -= average;
        square_sum += val * val;
      }
      result = sqrt(square_sum / a_size);
    }
    return result;
  }
} //namespace irs


namespace irs {

  //! \ingroup signal_processing_group
  //! \brief Расчет Дельта
  template<class T>
  class delta_calc_t {
  public:
    delta_calc_t(size_t a_count = 100);
    ~delta_calc_t();
    void add(T a_val);
    T max()const;
    T min()const;
    T absolute()const;
    void clear();
    void resize(size_t a_count);
    T average()const;
    T relative()const;
    size_t size();

  private:
    deque<T>m_array;
    size_t m_count;
  };

} // namespace irs

template<class T>
irs::delta_calc_t<T>::delta_calc_t(size_t a_count) : m_array(),
m_count(a_count) {
}

template<class T>
irs::delta_calc_t<T>::~delta_calc_t() {
}

template<class T>
void irs::delta_calc_t<T>::resize(size_t a_count) {
  m_count = a_count;
  if (m_array.size() > m_count) {
    m_array.resize(m_count);
  }
}

template<class T>
void irs::delta_calc_t<T>::add(T a_val) {
  size_t size = m_array.size();
  if (size < m_count) {
    m_array.push_back(a_val);
  }
  else if (size == m_count) {
    m_array.pop_front();
    m_array.push_back(a_val);
  }
  else if (size > m_count) {
    size_t margin = size - m_count;
    for (size_t i = 0; i < margin; i++) {
      m_array.pop_front();
    }
  }
}

template<class T>
T irs::delta_calc_t<T>::max()const {
  size_t size = m_array.size();
  T max = 0;
  if (size != 0) {
    max = *max_element(m_array.begin(), m_array.end());
  }
  return max;
}

template<class T>
T irs::delta_calc_t<T>::min()const {
  size_t size = m_array.size();
  T min = 0;
  if (size != 0) {
    min = *min_element(m_array.begin(), m_array.end());
  }
  return min;
}

template<class T>
T irs::delta_calc_t<T>::absolute()const {
  T delta = max() - min();
  return delta;
}

template<class T>
void irs::delta_calc_t<T>::clear() {
  m_array.resize(0);
}

template<class T>
T irs::delta_calc_t<T>::average()const {
  size_t size = m_array.size();
  T sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum = sum + m_array[i];
  }
  if (size == 0) {
    size = 1;
  }
  return sum / size;
}

template<class T>
T irs::delta_calc_t<T>::relative()const {
  T average_value = average();
  T relative_value = 0;
  if (average_value != 0) {
    relative_value = absolute() / average_value;
  } else {
    relative_value = 0;
  }
  return relative_value;
}

template<class T>
size_t irs::delta_calc_t<T>::size() {
  size_t size = m_array.size();
  return size;
}

namespace irs {

#define IRS_USE_DEQUE_DATA 1

template <class data_t, class calc_t>
class fast_average_t
{
public:
  typedef std::size_t size_type;
  fast_average_t(size_type a_count);
  void add(data_t a_val);
  calc_t get() const;
  void resize(size_type a_count);
  size_type size() const;
  size_type max_size() const;
  bool is_full() const;
  void clear();
  void preset(size_type a_start_pos, size_type a_count);
private:
  fast_average_t();
  size_type m_max_count;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<data_t> m_samples;
  #else
  std::deque<data_t> m_samples;
  #endif // IRS_USE_DEQUE_DATA
  calc_t m_sum;
};

template <class data_t, class calc_t>
fast_average_t<data_t, calc_t>::fast_average_t(size_type a_count):
  m_max_count(a_count),
  m_samples(),
  m_sum(0)
{
  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(a_count);
  #endif // IRS_USE_DEQUE_DATA
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::add(data_t a_val)
{
  if (m_max_count > 0) {
    IRS_LIB_ASSERT(m_samples.size() <= m_max_count);
    if (m_samples.size() == m_max_count) {
      m_sum -= m_samples.front();
      m_samples.pop_front();
    }
    m_samples.push_back(a_val);
    m_sum += a_val;
  }
}

template <class data_t, class calc_t>
calc_t fast_average_t<data_t, calc_t>::get() const
{
  if (m_samples.empty()) {
    return 0;
  }
  return m_sum/m_samples.size();
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::resize(size_type a_count)
{
  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(a_count);
  #endif // IRS_USE_DEQUE_DATA
  m_max_count = a_count;
  while (m_samples.size() > m_max_count) {
    m_sum -= m_samples.front();
    m_samples.pop_front();
  }
}

template <class data_t, class calc_t>
typename fast_average_t<data_t, calc_t>::size_type
fast_average_t<data_t, calc_t>::size() const
{
  return m_samples.size();
}

template <class data_t, class calc_t>
typename fast_average_t<data_t, calc_t>::size_type
fast_average_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template <class data_t, class calc_t>
bool fast_average_t<data_t, calc_t>::is_full() const
{
  return m_samples.size() == m_max_count;
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::clear()
{
  m_samples.clear();
  m_sum = 0;
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::preset(
  size_type a_start_pos, size_type a_count)
{
  const size_type erase_front_count = min(m_samples.size(), a_start_pos);
  m_samples.pop_front(erase_front_count);
  const size_type erase_back_count = min(m_samples.size(),
    m_samples.size() - a_count);
  m_samples.pop_back(erase_back_count);
  const size_type size = min(m_samples.size(), a_count);
  if (size > 0) {
    while (m_samples.size() < m_max_count) {
      for (size_type i = 0; i < size; i++) {
        m_samples.push_back(m_samples[i]);
      }
    }
  }
  m_sum = 0;
  for (size_type i = 0; i < m_samples.size(); i++) {
    m_sum += m_samples[i];
  }
}

template <class data_t, class calc_t>
class fast_multi_average_t
{
public:
  typedef std::size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  fast_multi_average_t(const vector<size_type>& a_sizes);
  //! \brief Добавляет отсчет
  //! \param[in] a_val - значение отсчета
  void add(data_t a_val);
  //! \brief Возвращает значение окна с индексом a_index
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Изменение размера окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, size_type a_new_size);
  //! \brief Возвращает текущий размер окна
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окона
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает статус заполения окна
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index) const;
  //! \brief Очищает окно
  //! \param[in] a_index - индекс окна
  void clear(size_type a_index);
  //! \brief Очищает все окна
  void clear();
private:
  fast_multi_average_t();
  size_type m_count;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<data_t> m_samples;
  #else
  std::deque<data_t> m_samples;
  #endif // IRS_USE_DEQUE_DATA
  struct window_t
  {
    size_type max_size;
    size_type size;
    calc_t sum;
    window_t():
      max_size(0),
      size(0),
      sum(0)
    {
    }
  };

  vector<window_t> m_windows;
};

template <class data_t, class calc_t>
fast_multi_average_t<data_t, calc_t>::fast_multi_average_t(const vector<size_type>& a_sizes):
  m_count(0),
  m_samples(),
  m_windows(0)
{
  for (size_type i = 0; i < a_sizes.size(); i++) {
    m_count = max(m_count, a_sizes[i]);
    window_t window;
    window.max_size = a_sizes[i];
    m_windows.push_back(window);
  }
  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(m_count);
  #endif // IRS_USE_DEQUE_DATA
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::add(data_t a_val)
{
  if (m_count > 0) {
    IRS_LIB_ASSERT(m_samples.size() <= m_count);
    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      if ((window.size > 0) && (window.size == window.max_size)) {
        window.sum -= m_samples[m_samples.size() - window.max_size];
        window.size--;
      }
    }
    if (m_samples.size() == m_count) {
      m_samples.pop_front();
    }
    m_samples.push_back(a_val);

    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      window.sum += a_val;
      if (window.size < window.max_size) {
        window.size++;
      }
    }
  }
}

template <class data_t, class calc_t>
calc_t fast_multi_average_t<data_t, calc_t>::get(size_type a_index) const
{
  if (m_samples.empty()) {
    return 0;
  }
  return m_windows[a_index].sum/m_windows[a_index].size;
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::resize(size_type a_index,
  size_type a_new_size)
{
  window_t& window = m_windows[a_index];
  const size_type start_index = m_samples.size() - window.size;
  window.max_size = a_new_size;
  if (window.size > window.max_size) {
    const size_type count = window.size - window.max_size;
    for (size_type i = 0; i < count; i++) {
      window.sum -= m_samples[start_index + i];
    }
    window.size = window.max_size;
  }

  m_count = 0;
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_count = max(m_count, m_windows[i].max_size);
  }

  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(m_count);
  #endif // IRS_USE_DEQUE_DATA

  while (m_samples.size() > m_count) {
    m_samples.pop_front();
  }
}

template <class data_t, class calc_t>
typename fast_multi_average_t<data_t, calc_t>::size_type
fast_multi_average_t<data_t, calc_t>::size(size_type a_index) const
{
  return m_windows[a_index].size;
}

template <class data_t, class calc_t>
typename fast_multi_average_t<data_t, calc_t>::size_type
fast_multi_average_t<data_t, calc_t>::max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template <class data_t, class calc_t>
bool fast_multi_average_t<data_t, calc_t>::is_full(size_type a_index) const
{
  return m_windows[a_index].size == m_windows[a_index].max_size;
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::clear(size_type a_index)
{
  m_windows[a_index].size = 0;
  m_windows[a_index].sum = 0;
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::clear()
{
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].size = 0;
    m_windows[i].sum = 0;
  }
}

template<class data_t, class calc_t>
class fast_sko_t
{
public:
  typedef std::size_t size_type;
  fast_sko_t(size_type a_count, size_type a_average_sample_count);
  ~fast_sko_t();
  void clear();
  void add(data_t a_val);
  operator calc_t()const;
  calc_t relative()const;
  calc_t average()const;
  void resize(size_type a_size);
  void resize_average(size_type a_size);
  void clear_average();
  size_type size() const;
  size_type average_size() const;
  size_type max_size() const;
  size_type average_max_size();
  bool is_full();
private:
  fast_sko_t();
  size_type m_count;
  fast_average_t<data_t, calc_t> m_average;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<calc_t> m_square_elems;
  #else
  std::deque<calc_t> m_square_elems;
  #endif // IRS_USE_DEQUE_DATA
  calc_t m_square_sum;
};

template<class data_t, class calc_t>
fast_sko_t<data_t, calc_t>::fast_sko_t(
  size_type a_count, size_type a_average_sample_count
):
  m_count(a_count),
  m_average(a_average_sample_count),
  m_square_elems(),
  m_square_sum(0)
{
  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(a_count);
  #endif // IRS_USE_DEQUE_DATA
}

template<class data_t, class calc_t>
fast_sko_t<data_t, calc_t>::~fast_sko_t()
{
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::resize(size_type a_size)
{
  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(a_size);
  #endif // IRS_USE_DEQUE_DATA
  m_count = a_size;
  while (m_square_elems.size() > m_count) {
    m_square_sum -= m_square_elems.front();
    m_square_elems.pop_front();
  }
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::resize_average(size_type a_size)
{
  m_average.resize(a_size);
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::size() const
{
  return m_square_elems.size();
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::average_size() const
{
  return m_average.size();
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::max_size() const
{
  return m_count;
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::average_max_size()
{
  return m_average.max_size();
}

template<class data_t, class calc_t>
bool fast_sko_t<data_t, calc_t>::is_full()
{
  return (m_square_elems.size() == m_count) && m_average.is_full();
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::clear()
{
  m_square_elems.clear();
  m_average.clear();
  m_square_sum = 0;
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);
  if (m_count > 0) {
    IRS_LIB_ASSERT(m_square_elems.size() <= m_count);
    if (m_count == m_square_elems.size()) {
      m_square_sum -= m_square_elems.front();
      m_square_elems.pop_front();
    }
    calc_t elem = a_val - m_average.get();
    elem *= elem;
    m_square_elems.push_back(elem);
    m_square_sum += elem;
  }
}

template<class data_t, class calc_t>
fast_sko_t<data_t, calc_t>::operator calc_t()const
{
  if (m_square_elems.empty()) {
    return 0;
  }
  if (m_square_sum < 0) {
    return 0;
  }
  return sqrt(m_square_sum/m_square_elems.size());
}

template<class data_t, class calc_t>
calc_t fast_sko_t<data_t, calc_t>::relative() const
{
  if (m_square_elems.empty()) {
    return 0;
  }
  const calc_t average = m_average.get();
  if (average == 0) {
    return 0;
  }
  const calc_t sko = *this;
  calc_t sko_reltive = sko/average;
  return sko_reltive;
}

template<class data_t, class calc_t>
calc_t fast_sko_t<data_t, calc_t>::average()const
{
  return m_average.get();
}

template<class data_t, class calc_t>
class fast_multi_sko_with_single_average_t
{
public:
  typedef std::size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  //! \param[in] a_average_sizes - массив размеров окон для среднего значения
  fast_multi_sko_with_single_average_t(
    const vector<size_type>& a_sizes, size_type a_average_size);
  ~fast_multi_sko_with_single_average_t();
  //! \brief Отчищает все окна
  void clear();
  //! \brief Добавляет отсчет
  void add(data_t a_val);
  //! \brief Добавляет отсчет в среднее значение
  void add_to_average(data_t a_val);
  //! \brief Возвращает значение sko
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Возвращает среднее значение
  calc_t average() const;
  //! \brief Изменяет размер окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, size_type a_new_size);
  //! \brief Изменяет размер окна среднего значения
  //! \param[in] a_new_size - новый размер окна
  void resize_average(size_type a_new_size);
  //! \brief Отчищает среднее значение
  void clear_average();
  //! \brief Возвращает размер окна sko
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окна sko с наибольшим количеством элементов
  size_type size() const;
  //! \brief Возвращает размер окна среднего значения
  size_type average_size() const;
  //! \brief Возвращает максимальный размер окна sko
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер самого большого окна sko
  size_type max_size() const;
  //! \brief Возвращает максимальный размер окна среднего значения
  size_type average_max_size() const;
  //! \brief Возвращает статус заполнения окна
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index);
  void preset(size_type a_start_pos, size_type a_size);
  void preset_average(size_type a_start_pos, size_type a_size);
private:
  fast_multi_sko_with_single_average_t();
  size_type m_max_count;
  fast_average_t<data_t, calc_t> m_average;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<calc_t> m_square_elems;
  #else
  std::deque<calc_t> m_square_elems;
  #endif // IRS_USE_DEQUE_DATA
  struct window_t
  {
    size_type max_size;
    size_type size;
    calc_t square_sum;
    window_t():
      max_size(0),
      size(0),
      square_sum()
    {
    }
  };
  vector<window_t> m_windows;
};

template<class data_t, class calc_t>
fast_multi_sko_with_single_average_t<data_t, calc_t>::
fast_multi_sko_with_single_average_t(
  const vector<size_type>& a_sizes,
  size_type a_average_size
):
  m_max_count(0),
  m_average(a_average_size),
  m_square_elems(),
  m_windows()
{
  for (size_type i = 0; i < a_sizes.size(); i++) {
    m_max_count = max(m_max_count, a_sizes[i]);
    window_t window;
    window.max_size = a_sizes[i];
    m_windows.push_back(window);
  }
  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(m_max_count);
  #endif // IRS_USE_DEQUE_DATA
}

template<class data_t, class calc_t>
fast_multi_sko_with_single_average_t<data_t, calc_t>::
~fast_multi_sko_with_single_average_t()
{
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::resize(
  size_type a_index,
  size_type a_new_size)
{
  window_t& window = m_windows[a_index];
  const size_type start_index = m_square_elems.size() - window.size;
  window.max_size = a_new_size;
  if (window.size > window.max_size) {
    const size_type count = window.size - window.max_size;
    for (size_type i = 0; i < count; i++) {
      window.square_sum -= m_square_elems[start_index + i];
    }
    window.size = window.max_size;
  }

  m_max_count = 0;
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_max_count = max(m_max_count, m_windows[i].max_size);
  }

  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(m_max_count);
  #endif // IRS_USE_DEQUE_DATA

  while (m_square_elems.size() > m_max_count) {
    m_square_elems.pop_front();
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::
resize_average(size_type a_new_size)
{
  m_average.resize(a_new_size);
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::size(
  size_type a_index) const
{
  return m_windows[a_index].size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::size() const
{
  return m_square_elems.size();
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::average_size() const
{
  return m_average.size();
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::
max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::average_max_size() const
{
  return m_average.max_size();
}

template<class data_t, class calc_t>
bool fast_multi_sko_with_single_average_t<data_t, calc_t>::
is_full(size_type a_index)
{
  return (m_windows[a_index].size == m_windows[a_index].max_size) &&
    m_average.is_full();
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::preset(
  size_type a_start_pos, size_type a_size)
{
  preset_average(a_start_pos, a_size);

  const size_type erase_front_count = min(m_square_elems.size(), a_start_pos);
  m_square_elems.pop_front(erase_front_count);
  const size_type erase_back_count = min(m_square_elems.size(),
    m_square_elems.size() - a_size);
  m_square_elems.pop_back(erase_back_count);
  const size_type size = min(m_square_elems.size(), a_size);
  if (size > 0) {
    while (m_square_elems.size() < m_max_count) {
      for (size_type i = 0; i < size; i++) {
        m_square_elems.push_back(m_square_elems[i]);
      }
    }
  }
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& window = m_windows[i];
    window.square_sum = 0;
    const size_type min_size = min(m_square_elems.size(), window.max_size);
    const size_type start_pos = m_square_elems.size() - min_size;
    for (size_type j = start_pos; j < m_square_elems.size(); j++) {
      window.square_sum += m_square_elems[j];
    }
    window.size = min_size;
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::preset_average(
  size_type a_start_pos, size_type a_size)
{
  const size_type average_start_pos =
    m_square_elems.size() - m_average.size();
  if ((a_start_pos + a_size) > average_start_pos) {
    size_type average_pos = 0;
    if (a_start_pos < average_start_pos) {
      average_pos = 0;
    } else {
      average_pos = a_start_pos - average_start_pos;
    }
    size_type size = 0;
    if ((a_start_pos + a_size) > (average_start_pos + m_average.size())) {
      size = m_average.size();
    } else {
      size = (average_start_pos + m_average.size()) - (a_start_pos + a_size);
    }
    m_average.preset(average_pos, size);
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::clear()
{
  m_square_elems.clear();
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].size = 0;
    m_windows[i].square_sum = 0;
  }
  m_average.clear();
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);
  if (m_max_count > 0) {
    IRS_LIB_ASSERT(m_square_elems.size() <= m_max_count);
    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      if ((window.size > 0) && (window.size == window.max_size)) {
        window.square_sum -=
          m_square_elems[m_square_elems.size() - window.max_size];
        window.size--;
      }
    }
    calc_t elem = a_val - m_average.get();
    elem *= elem;

    if (m_max_count == m_square_elems.size()) {
      m_square_elems.pop_front();
    }

    m_square_elems.push_back(elem);
    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      window.square_sum += elem;
      if (window.size < window.max_size) {
        window.size++;
      }
    }
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::add_to_average(
  data_t a_val)
{
  m_average.add(a_val);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_with_single_average_t<data_t, calc_t>::get(
  size_type a_index) const
{
  const window_t& window = m_windows[a_index];
  if (window.size == 0) {
    return 0;
  }
  if (window.square_sum < 0) {
    return 0;
  }
  return sqrt(window.square_sum/window.size);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_with_single_average_t<data_t, calc_t>::average() const
{
  return m_average.get();
}

template<class data_t, class calc_t>
class fast_multi_sko_t
{
public:
  typedef std::size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  //! \param[in] a_average_sizes - массив размеров окон для среднего значения
  fast_multi_sko_t(const vector<size_type>& a_sizes,
    const vector<size_type>& a_average_sizes);
  ~fast_multi_sko_t();
  //! \brief Отчищает все окна
  void clear();
  //! \brief Добавляет отсчет
  void add(data_t a_val);
  //! \brief Возвращает значение sko
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Возвращает среднее значение
  //! \param[in] a_index - индекс окна
  calc_t average(size_type a_index) const;
  //! \brief Изменяет размер окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, size_type a_new_size);
  //! \brief Изменяет размер окна среднего значения
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize_average(size_type a_index, size_type a_new_size);
  //! \brief Отчищает среднее значение для всех окон
  void clear_average();
  //! \brief Возвращает размер окна sko
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окна среднего значения
  //! \param[in] a_index - индекс окна
  size_type average_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер окна sko
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер окна среднего значения
  //! \param[in] a_index - индекс окна
  size_type average_max_size(size_type a_index) const;
  //! \brief Возвращает статус заполнения окна
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index);
private:
  fast_multi_sko_t();
  size_type m_count;
  fast_multi_average_t<data_t, calc_t> m_average;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<calc_t> m_square_elems;
  #else
  std::deque<calc_t> m_square_elems;
  #endif // IRS_USE_DEQUE_DATA
  struct window_t
  {
    size_type max_size;
    size_type size;
    calc_t square_sum;
    #if IRS_USE_DEQUE_DATA
    irs::deque_data_t<calc_t> m_square_elems;
    #else
    std::deque<calc_t> m_square_elems;
    #endif // IRS_USE_DEQUE_DATA
    window_t():
      max_size(0),
      square_sum(),
      m_square_elems()
    {
    }
  };
  vector<window_t> m_windows;
};

template<class data_t, class calc_t>
fast_multi_sko_t<data_t, calc_t>::fast_multi_sko_t(
  const vector<size_type>& a_sizes,
  const vector<size_type>& a_average_sizes
):
  m_count(0),
  m_average(a_average_sizes),
  m_square_elems(),
  m_windows()
  //m_square_sum(0)
{
  for (size_type i = 0; i < a_sizes.size(); i++) {
    m_count = max(m_count, a_sizes[i]);
    window_t window;
    window.max_size = a_sizes[i];
    #if IRS_USE_DEQUE_DATA
    m_square_elems.reserve(window.max_size);
    #endif // IRS_USE_DEQUE_DATA
    m_windows.push_back(window);
  }
}

template<class data_t, class calc_t>
fast_multi_sko_t<data_t, calc_t>::~fast_multi_sko_t()
{
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::resize(size_type a_index,
  size_type a_new_size)
{
  window_t& window = m_windows[a_index];
  window.max_size = a_new_size;
  while (window.square_elems.size() > window.max_size) {
    window.square_sum -= window.square_elems.front();
    window.square_elems.pop_front();
  }
  #if IRS_USE_DEQUE_DATA
  window.square_elems.reserve(m_count);
  #endif // IRS_USE_DEQUE_DATA
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::resize_average(size_type a_index,
  size_type a_new_size)
{
  m_average.resize(a_index, a_new_size);
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::size(size_type a_index) const
{
  return m_windows[a_index].size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::average_size(size_type a_index) const
{
  return m_average.size(a_index);
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::average_max_size(size_type a_index) const
{
  return m_average.max_size(a_index);
}

template<class data_t, class calc_t>
bool fast_multi_sko_t<data_t, calc_t>::is_full(size_type a_index)
{
  return (
    m_windows[a_index].square_elems.size() == m_windows[a_index].max_size) &&
    m_average.is_full(a_index);
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::clear()
{
  m_square_elems.clear();
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].square_elems.clear();
    m_windows[i].square_sum = 0;
  }
  m_average.clear();
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);
  if (m_count > 0) {
    IRS_LIB_ASSERT(m_square_elems.size() <= m_count);
    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      if (!window.square_elems.empty() &&
          (window.square_elems.size() == window.max_size)) {
        window.square_sum -= window.square_elems.front();
        window.square_elems.pop_front();
      }
      if (window.square_elems.size() < window.max_size) {
        calc_t elem = a_val - m_average.get(i);
        elem *= elem;
        window.square_sum += elem;
        window.square_elems.push_back(elem);
      }
    }
  }
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_t<data_t, calc_t>::get(size_type a_index) const
{
  window_t& window = m_windows[a_index];
  if (window.square_elems.empty()) {
    return 0;
  }
  if (window.square_sum < 0) {
    return 0;
  }
  return sqrt(window.square_sum/window.square_elems.size());
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_t<data_t, calc_t>::average(size_type a_index) const
{
  return m_average.get(a_index);
}

//! \addtogroup signal_processing_group
//! @{

//! \brief CRC-16/IBM
//! \details
//!   Width: 16
//!   Poly: 0x8005 x^16 + x^15 + x^2 + 1
//!   Init: 0xFFFF
//!   RefIn: true
//!   RefOut: true
//!   XorOut: 0x0000
//!   Check : 0x4B37 ("123456789")
//!   Максимальная длина: 4095 байт (32767 бит) - обнаружение
//!     одинарных, двойных, тройных и всех нечетных ошибок
inline irs_u16 crc16(const irs_u8* ap_buf, size_t a_size)
{
  irs_u16 crc = 0xffff;
  for (size_t i = 0; i < a_size; i++) {
    crc = static_cast<irs_u16>(crc ^ ap_buf[i]);
    for (size_t j = 0;j < 8; j++) {
      crc = static_cast<irs_u16>(
        (crc & 0x0001) ? ((crc >> 1) ^ 0xA001) : (crc >> 1));
    }
  }
  return (crc);
}

struct crc16_data_t {
  enum {
    size = 256
  };
#ifdef __ICCAVR__
#define STATIC_MEMORY_MODIFIER static IRS_ICCAVR_FLASH
#define MEMORY_MODIFIER IRS_ICCAVR_FLASH
#else //__ICCAVR__
#define STATIC_MEMORY_MODIFIER
#define MEMORY_MODIFIER
#endif //__ICCAVR__

  STATIC_MEMORY_MODIFIER irs_u16 table[size];
#ifndef __ICCAVR__

  crc16_data_t();
#endif //__ICCAVR__
};

//! \brief CRC-16/IBM
//! \details
//!   Width: 16
//!   Poly: 0x8005 x^16 + x^15 + x^2 + 1
//!   Init: 0xFFFF
//!   RefIn: true
//!   RefOut: true
//!   XorOut: 0x0000
//!   Check : 0x4B37 ("123456789")
//!   Максимальная длина: 4095 байт (32767 бит) - обнаружение
//!     одинарных, двойных, тройных и всех нечетных ошибок
//!   Потребление памяти: не менее 512 байт
inline irs_u16 crc16_table(irs_u8* ap_buf, size_t a_size)
{
  static handle_t<crc16_data_t> crc16_data = new crc16_data_t();
  irs_u16 crc = 0xFFFF;
  for (size_t i = 0; i < a_size; i++) {
    crc = static_cast<irs_u16>(
      (crc >> 8) ^ crc16_data->table[(crc & 0xFF) ^ *(ap_buf++)]);
  }
  return crc;
}


struct crc32_data_t {
  enum {
    size = 256
  };
#ifdef __ICCAVR__
#define STATIC_MEMORY_MODIFIER static IRS_ICCAVR_FLASH
#define MEMORY_MODIFIER IRS_ICCAVR_FLASH
#else //__ICCAVR__
#define STATIC_MEMORY_MODIFIER
#define MEMORY_MODIFIER
#endif //__ICCAVR__

  STATIC_MEMORY_MODIFIER irs_u32 table[size];
#ifndef __ICCAVR__

  crc32_data_t();
#endif //__ICCAVR__
};

//! \brief Зеркальный табличный метод расчета crc32
//! \details Зеркальный метод является наиболее распространенным
//!   Необходимый объем оперативной памяти не менее 1024 байт
//!   Значительный прирост в скорости вычисления по сравнению с нетабличным
//!   методом.
//!   Тесты производительности (массив на 1000 байт):
//!     - STM32f217ZGT (120 МГц, IAR 6.3 без оптимизации) - 421 мкс.
// Перенесен из cpp- в h-файл и сделан шаблонным, чтобы можно было вычислять
//  для eeprom-указателей в AVR
template<class T>
irs_u32 crc32_table(const T* ap_buf, const size_t a_size)
{
  IRS_STATIC_ASSERT(sizeof(T) == 1);
  static handle_t<crc32_data_t> crc32_data = new crc32_data_t();
  irs_u32 crc = 0xFFFFFFFF;
  for (size_t i = 0; i < a_size; i++) {
    crc = ((crc >> 8) & 0x00FFFFFF) ^
      crc32_data->table[(crc ^ *(ap_buf++)) & 0xFF];
  }
  return ~crc;
}

class crc32_table_stream_t
{
public:
  typedef std::size_t size_type;
  crc32_table_stream_t();
  template<class T>
  inline void put(const T* ap_buf, size_type a_size = 1);
  irs_u32 get_crc();
  void reset();
private:
  static handle_t<crc32_data_t> mp_crc32_data;
  irs_u32 m_crc;
};

template<class T>
inline void crc32_table_stream_t::put(const T* ap_buf, size_type a_size)
{
  IRS_STATIC_ASSERT(sizeof(T) == 1);
  for (size_t i = 0; i < a_size; i++) {
    m_crc = ((m_crc >> 8) & 0x00FFFFFF) ^
      mp_crc32_data->table[(m_crc ^ *(ap_buf++)) & 0xFF];
  }
}

// T должно быть 32 битным
// Алгоритм является зеркальным, но не совпадает с распространенными
// алгоритмами. В распространенных алгоритмах инвертируется выходное значение
// Функция используется для внутренних потребностей и переделыватся не будет.
template<class T>
irs_u32 crc32(T * a_buf, irs_uarc a_start, irs_uarc a_cnt) {
  // Взято с википедии
  IRS_STATIC_ASSERT(sizeof(T) == 4);
  irs_uarc top = a_start + a_cnt;
  irs_u32 crc = 0xFFFFFFFF; /* Начальное состояние сдвигового регистра */
  irs_u8 flag = 0;
  for (irs_uarc i = a_start; i < top; i++) {
    crc = crc ^ a_buf[i];
    for (irs_uarc j = 1; j <= 32; j++) {
      flag = IRS_LOBYTE(crc) & irs_u8(1);
      crc >>= 1; /* Сдвиг регистра на 1 позицию */
      if (flag)
        crc ^= 0xEDB88320; // 0xEDB88320 - примитивный полином
    }
  }
  return(crc); /* Здесь должно быть return (~crc) */
}

template<class T>
inline irs_u32 crc32(T * a_buf, size_t a_size) {
  return crc32(a_buf, 0, a_size);
}

irs_u8 crc8(irs_u8 * a_buf, irs_u8 a_start, irs_u8 a_cnt);

// Приведение фазы к диапазону a_phase_begin - a_phase_end
double phase_normalize(double a_phase_in, double a_phase_begin = 0.,
  double a_phase_end = 360.);
// Приведение фазы к диапазону -180 - +180
double phase_normalize_180(double a_phase_in);

// Приведение фазы к диапазону a_phase_begin - a_phase_end
template<class T>
T phase_norm(T a_phase_in, T a_phase_begin = 0., T a_phase_end = 360.) {
  const T phase_period = a_phase_end - a_phase_begin;
  T div_int = floor((a_phase_in - a_phase_begin) / phase_period);
  T phase_out = a_phase_in - div_int * phase_period;

  return phase_out;
}

// Приведение фазы к диапазону -180 - +180
template<class T>
T phase_norm_180(T a_phase_in) {
  return phase_norm(a_phase_in, T(-180.), T(+180.));
}

// функция интерполяции массива точек прямой методом МНК.
// Записывает в a_korf_k и a_koef_b коэффициенты прямой y=a_korf_k*x+a_koef_b,
// апроксимирующей набор точек, представленный массивами a_array_x и a_array_y,
// размера a_size
template<class TYPE>
void interp_line_mnk(TYPE* a_array_x, TYPE* a_array_y,
  const unsigned int& a_size, long double& a_koef_k, long double& a_koef_b) {
  long double S_xy = 0.0, S_x2 = 0.0, S_x = 0.0, S_y = 0.0;
  for (unsigned int i = 0; i < a_size; i++) {
    S_xy = S_xy + a_array_x[i] * a_array_y[i];
    S_x2 = S_x2 + a_array_x[i] * a_array_x[i];
    S_x = S_x + a_array_x[i];
    S_y = S_y + a_array_y[i];
  }
  a_koef_k = (S_xy - S_x * S_y / a_size) / (S_x2 - S_x * S_x / a_size);
  a_koef_b = (S_y - a_koef_k * S_x) / a_size;
}

// Вычисляет коэффициенты k и b прямой y = k*x+b
template<class TYPE>
int koef_line(const TYPE a_y1, const TYPE a_x1, const TYPE a_y2,
  const TYPE a_x2, TYPE& a_k, TYPE& a_b) {
  if (a_x1 == a_x2)
    return 1;
  TYPE difference = a_x1 - a_x2;
  a_k = (a_y1 - a_y2) / difference;
  a_b = (a_y2 * a_x1 - a_x2 * a_y1) / difference;
  return 0;
}

// Функция возвращает среднее арифметическое значение элементов массива
// Аналог функции Mean из библиотеки Math.hpp C++ Builder6
double mean(const double*ap_data, const int a_data_size);

// Функция возвращает дисперсию элементов массива
// Аналог функции Variance из библиотеки Math.hpp C++ Builder6
double variance(const double*ap_data, const int a_data_size);

// Функция возвращает среднее квадратическое отклонение элементов массива
// Аналог функции StdDev из библиотеки Math.hpp C++ Builder6
double std_dev(const double*ap_data, const int a_data_size);

template<class In, class Out, class Pred>
Out copy_if(In a_first, In a_last, Out a_res, Pred a_pred) {
  while (a_first != a_last) {
    if (a_pred(*a_first)) {
      *a_res++ = *a_first;
    }
    ++a_first;
  }
  return a_res;
}

//! @}

} // namespace irs

#endif //IRSALGH
