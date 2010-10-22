//! \file
//! \ingroup container_group
//! \ingroup in_out_group
//! \brief Утилиты для работы с mxdata_t
//!
//! Дата: 10.08.2010\n
//! Ранняя дата: 3.12.2009

// Номер файла
#define MXDATACPP_IDX 2

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <string.h>

#include <mxdata.h>

#include <irsfinal.h>

// Класс проверки изменения для bit_data_t
irs::change_bit_t::change_bit_t(const bit_data_t *ap_bit_data,
  change_data_first_connect_t a_first_connect_change):
  mp_data(ap_bit_data?ap_bit_data->data():0),
  m_index(ap_bit_data?ap_bit_data->index():0),
  m_bit_index(ap_bit_data?ap_bit_data->bit_index():0),
  m_first_connect(true),
  m_bit(0),
  m_first_connect_change(a_first_connect_change)
{
}
void irs::change_bit_t::connect(const bit_data_t *ap_bit_data,
  change_data_first_connect_t a_first_connect_change)
{
  if (ap_bit_data) {
    mp_data = ap_bit_data->data();
    m_index = ap_bit_data->index();
    m_bit_index = ap_bit_data->bit_index();
    m_first_connect_change = a_first_connect_change;
  }
}
bool irs::change_bit_t::changed()
{
  bool result = true;
  if (m_first_connect) {
    if (mp_data->connected()) {
      m_first_connect = false;
      //mp_data->read(&m_bit, m_index, m_bit_size);
      m_bit = mp_data->bit(m_index, m_bit_index);
      if (m_first_connect_change == change_on_first_connect) {
        result = true;
      } else {
        result = false;
      }
    } else {
      result = false;
    }
  } else {
    //irs_u8 new_bit = 0;
    //mp_data->read(&new_bit, m_index, m_bit_size);
    bit_data_t::bit_t new_bit = mp_data->bit(m_index, m_bit_index);
    //if ((m_bit^new_bit)&(1 << m_bit_index)) {
    if (m_bit != new_bit) {
      m_bit = new_bit;
      result = true;
    } else {
      result = false;
    }
  }
  return result;
}

// Объединение нескольких разнородных классов mxdata_t в один класс
irs::collect_data_t::collect_data_t():
  m_items(),
  m_size(0)
{
}
void irs::collect_data_t::add(mxdata_t *ap_data,
  irs_uarc a_index, irs_uarc a_size)
{
  m_size += a_size;
  m_items.push_back(item_t());
  item_t& item = m_items.back();
  item.p_data = ap_data;
  item.index = a_index;
  item.size = a_size;
  item.end = m_size;
}
void irs::collect_data_t::clear()
{
  m_size = 0;
  m_items.clear();
}
irs_uarc irs::collect_data_t::size()
{
  return m_size;
}
irs_bool irs::collect_data_t::connected()
{
  const item_it_t items_end = m_items.end();
  for (item_it_t it = m_items.begin(); it != items_end; ++it) {
    if (!it->p_data->connected()) {
      return irs_false;
    }
  }
  return irs_true;
}
bool irs::collect_data_t::belong(irs_uarc a_begin, irs_uarc a_end,
    irs_uarc a_area_begin, irs_uarc a_area_end,
    irs_uarc* a_out_begin, irs_uarc* a_out_end)
{
  bool belong_result = false;
  // Главная область охватывает проверяемую область
  if ((a_area_begin <= a_begin) && (a_end <= a_area_end)) {
    *a_out_begin = a_begin;
    *a_out_end = a_end;
    belong_result = true;
  // Главная область входит в проверяемую область
  } else if ((a_begin <= a_area_begin) && (a_area_end <= a_end)) {
    *a_out_begin = a_area_begin;
    *a_out_end = a_area_end;
    belong_result = true;
  // Начало главной области входит в проверяемую область
  } else if ((a_begin <= a_area_begin) && (a_area_begin < a_end)) {
    *a_out_begin = a_area_begin;
    *a_out_end = a_end;
    belong_result = true;
  // Конец главной области входит в проверяемую область
  } else if ((a_begin < a_area_end) && (a_area_end <= a_end)) {
    *a_out_begin = a_begin;
    *a_out_end = a_area_end;
    belong_result = true;
  // Главная область не пересекается с проверяемой областью
  } else {
    *a_out_begin = a_end;
    *a_out_end = a_end;
    belong_result = false;
  }
  return belong_result;
}
void irs::collect_data_t::read_write(read_write_t a_read_write,
  irs_u8* ap_buf_read, const irs_u8* ap_buf_write, irs_uarc a_index,
  irs_uarc a_size, irs_uarc a_bit_index)
{
  irs_uarc out_begin = 0;
  irs_uarc out_end = 0;
  irs_uarc begin = 0;
  const item_it_t items_end = m_items.end();
  const irs_uarc area_begin = a_index;
  const irs_uarc area_end = a_index + a_size;
  for (item_it_t it = m_items.begin(); it != items_end; ++it) {
    if (belong(begin, it->end, area_begin, area_end, &out_begin, &out_end)) {
      irs_uarc index = out_begin - begin + it->index;
      irs_uarc size = out_end - out_begin;
      switch (a_read_write) {
        case rw_read: {
          it->p_data->read(ap_buf_read, index, size);
        } break;
        case rw_write: {
          it->p_data->write(ap_buf_write, index, size);
        } break;
        case rw_bit_read: {
          irs_bool bit = it->p_data->bit(index, a_bit_index);
          if (bit) {
            *ap_buf_read = 1;
          } else {
            *ap_buf_read = 0;
          }
        } break;
        case rw_bit_write: {
          if (*ap_buf_write) {
            it->p_data->set_bit(index, a_bit_index);
          } else {
            it->p_data->clear_bit(index, a_bit_index);
          }
        } break;
      }
    }
    begin = it->end;
  }
}
void irs::collect_data_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  read_write(rw_read, ap_buf, IRS_NULL, a_index, a_size, 0);
}
void irs::collect_data_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  read_write(rw_write, IRS_NULL, ap_buf, a_index, a_size, 0);
}
irs_bool irs::collect_data_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  irs_u8 bit_result = 0;
  read_write(rw_bit_read, &bit_result, IRS_NULL, a_index, 1, a_bit_index);
  if (bit_result) {
    return irs_true;
  } else {
    return irs_false;
  }
}
void irs::collect_data_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  const irs_u8 bit_on = 1;
  read_write(rw_bit_write, IRS_NULL, &bit_on, a_index, 1, a_bit_index);
}
void irs::collect_data_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  const irs_u8 bit_off = 0;
  read_write(rw_bit_write, IRS_NULL, &bit_off, a_index, 1, a_bit_index);
}
void irs::collect_data_t::tick()
{
  const item_it_t items_end = m_items.end();
  for (item_it_t it = m_items.begin(); it != items_end; ++it) {
    it->p_data->tick();
  }
}

#ifdef IRS_LIB_DEBUG
bool irs::raw_data_test()
{
  typedef size_t size_type;
  typedef int testing_type;
  bool test_success = true;
  typedef irs::raw_data_t<testing_type> raw_data_type;
  irs::raw_data_t<testing_type> raw_data;
  // Создаем и заполняем буфер тестовыми данными
  const size_type buf_size = 100;
  // Размер буфера должен быть больше нуля
  IRS_LIB_ASSERT(buf_size > 0);
  // Размер буфера должен быть четным
  IRS_LIB_ASSERT(!(buf_size & 1));
  testing_type test_data_buf[buf_size];
  for (size_type i = 0; i < buf_size; i++) {
    test_data_buf[i] = i;
  }
  // Тест на вставку, копирование и удаление данных
  raw_data.insert(raw_data.data(), test_data_buf, test_data_buf + buf_size);
  test_success = (buf_size == raw_data.size());
  IRS_LIB_ASSERT(test_success);
  if (test_success) {
    test_success = (memcmpex(raw_data.data(), test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }
  if (test_success) {
    const size_type pop_data_size = buf_size/2;

    // Удаление из начала количества элементов, равного половине буфера
    raw_data.erase(raw_data.data(), raw_data.data() + pop_data_size);

    // Вставка в конец буфера
    raw_data.insert(raw_data.data() + raw_data.size(), test_data_buf,
      test_data_buf + buf_size);

    // Вставка в конец буфера
    raw_data.insert(raw_data.data() + raw_data.size(),
      test_data_buf, test_data_buf + buf_size);

    // Удаление из начала количества элементов, равного половине буфера
    raw_data.erase(raw_data.data(), raw_data.data() + buf_size - pop_data_size);

    // Удаление из конца количества элементов, равного размеру буфера
    raw_data.erase(
      raw_data.data() + (raw_data.size() - buf_size),
      raw_data.data() + raw_data.size());

    // Удаляем первый элемент и опять вставляем
    raw_data_type::pointer p_first_elem = raw_data.erase(raw_data.begin(),
      raw_data.begin() + 1);
    raw_data.insert(p_first_elem, test_data_buf[0]);
    
    // Вставляем в начало элемент и сразу же удаляем его
    raw_data_type::pointer p_inserted_elem = raw_data.insert(raw_data.begin(),
      testing_type());
    raw_data.erase(p_inserted_elem);

    test_success = (raw_data.size() == buf_size);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Проверка на равенство данных контейнера данным тестового буфера
  if (test_success) {
    test_success = (memcmpex(raw_data.data(), test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Проверка удаления пустого количества элементов
  if (test_success) {
    raw_data.erase(raw_data.begin(), raw_data.begin());
    raw_data.erase(raw_data.end(), raw_data.end());
    test_success = (memcmpex(raw_data.data(), test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Проверка вставки пустого количества элементов
  if (test_success) {
    raw_data.insert(raw_data.begin(), raw_data.begin(), raw_data.begin());
    raw_data.insert(raw_data.end(), raw_data.end(), raw_data.end());
    test_success = (memcmpex(raw_data.data(), test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Проверка функции resize
  if (test_success) {
    raw_data.reserve(buf_size * 10);
    raw_data.resize(buf_size);   
    test_success = (memcmpex(raw_data.data(), test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Проверка функции clear
  if (test_success) {
    raw_data.clear();
    test_success = (raw_data.size() == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Опять вставляем тестовый буфер в пустой контейнер
  if (test_success) {
    raw_data.reserve(0);
    raw_data.insert(raw_data.data() + raw_data.size(),
      test_data_buf, test_data_buf + buf_size);
    test_success = (buf_size == raw_data.size());
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Проверяем на равенство с тестовым буфером
  if (test_success) {
    test_success = (memcmpex(raw_data.data(), test_data_buf, buf_size) == 0);
    IRS_LIB_ASSERT(test_success);
  } else {
    // Произошла ошибка
  }

  // Проверяем оператор operator[]
  if (test_success) {
    for (size_type i = 0; i < buf_size; i++) {
      if (raw_data[i] != test_data_buf[i]) {
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

  // Проверяем копирование одного контейнера в другой
  if (test_success) {            
    irs::raw_data_t<testing_type> raw_data_second;
    raw_data_second = raw_data;
    if (raw_data_second.size() == raw_data.size()) {
      for (size_type i = 0; i < buf_size; i++) {
        if (raw_data[i] != raw_data_second[i]) {
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
#endif // IRS_DEBUG

#ifdef IRS_LIB_DEBUG
bool irs::deque_data_test()
{
  typedef size_t size_type;
  typedef int testing_type;
  bool test_success = true;
  irs::deque_data_t<testing_type> deque_data;
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
    irs::deque_data_t<testing_type> deque_data_second;
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
#endif // IRS_DEBUG


