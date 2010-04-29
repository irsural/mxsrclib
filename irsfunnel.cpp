// Класс для расширения количества переменнных в mxnet "Воронка"
// Дата: 29.04.2010
// Ранняя дата: 12.02.2009

// Номер файла
#define IRSFUNNELCPP_IDX 13

#include <irsdefs.h>

#include <irserror.h>

#include <irsfunnel.h>

#include <irsfinal.h>

irs::funnel_client_t::funnel_client_t(
  mxdata_t *ap_data,
  irs_uarc a_start_index,
  counter_t a_read_interval,
  const irs_uarc a_area_index,
  const irs_uarc a_area_size,
  const fannel_client_mode_t a_fannel_client_mode):
  m_fannel_client_mode(a_fannel_client_mode),
  m_status(FREE),
  mp_data(ap_data),
  m_is_connected(irs_false),
  m_indexes_are_created(irs_false),
  mp_buf(IRS_NULL),
  m_size(0),
  m_area_index(a_area_index),
  m_area_size(a_area_size),
  m_current_index(0),
  m_write_vector(),
  m_start_index(a_start_index),
  m_read_interval(a_read_interval),
  m_read_counter(0),
  m_size_index(),
  m_read_index(),
  m_read_value(),
  m_write_index(),
  m_write_value(),
  m_protect_timer(make_cnt_s(3)),
  m_is_buf_created(false),
  m_on_read_complete(false),
  m_count_read_byte(0),
  m_read_start_index(0),
  m_need_write_byte(0)
{
  if (mp_data->connected())
  {
    irs_uarc index = m_start_index;
    index = m_size_index.connect(mp_data, index);
    index = m_write_index.connect(mp_data, index);
    index = m_write_value.connect(mp_data, index);
    index = m_read_index.connect(mp_data, index);
    index = m_read_value.connect(mp_data, index);
  }
}

irs::funnel_client_t::~funnel_client_t()
{
  delete []mp_buf;
}

irs_uarc irs::funnel_client_t::size()
{
  //if (m_is_connected)
  return m_area_size;
  //else return 0;
}

irs_bool irs::funnel_client_t::connected()
{
  return (m_is_connected && mp_data->connected());
}

void irs::funnel_client_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (m_is_buf_created) {
    irs_uarc real_size = a_size;
    irs_uarc real_index = a_index;
    memset((void*)ap_buf, 0, real_size);
    if (real_index > m_area_size - 1)
      return;
    if (real_size > m_area_size - real_index) {
      real_size = m_area_size - real_index;
    }
    memcpy((void*)ap_buf, (void*)(mp_buf + real_index), real_size);
  }
}

void irs::funnel_client_t::write(
  const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  if (m_is_buf_created)
  {
    irs_uarc real_size = a_size;
    irs_uarc real_index = a_index;
    if (real_index > m_area_size - 1) {
      return;
    }
    if (real_size > m_area_size - real_index) {
      real_size = m_area_size - real_index;
    }
    memcpy((void*)(mp_buf + real_index), (void*)ap_buf, real_size);
    fill_n(m_write_vector.begin() + a_index, real_size, true);
    m_need_write_byte += real_size;
    m_on_read_complete = false;
    if (m_status != WRITE) {
      m_status = WRITE;
      m_current_index = 0;
    }
  }
}

irs_bool irs::funnel_client_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (m_is_buf_created)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_size) index = m_size - 1;
    if (bit_index > 7) bit_index = 7;
    return irs_u8(mp_buf[index] & (irs_u8(1) << bit_index));
  }
  else return irs_false;
}

void irs::funnel_client_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (m_is_buf_created)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_size) index = m_size - 1;
    if (bit_index > 7) bit_index = 7;
    mp_buf[index] |= irs_u8(irs_u8(1) << bit_index);
    m_write_vector[index] = true;
  }
}

void irs::funnel_client_t::clear_bit(irs_uarc a_index,irs_uarc a_bit_index)
{
  if (m_is_buf_created)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_size) index = m_size - 1;
    if (bit_index > 7) bit_index = 7;
    mp_buf[index] &= irs_u8((1 << bit_index)^0xFF);
    m_write_vector[index] = true;
  }
}

irs_i32 irs::funnel_client_t::get_write_size()
{
  /*int write_size = 0;
  int vsize = m_write_vector.size();
  for (int index = 0; index < vsize; index++) {
    if (m_write_vector[index] == true) {
      write_size++;
    }
  } */
  return m_need_write_byte;//write_size;
}
bool irs::funnel_client_t::read_complete()
{
  return m_on_read_complete;
}
void irs::funnel_client_t::reset_stat_read_complete()
{
  m_on_read_complete = false;
  m_on_reset_stat_read_complete = true;
}
irs_i32 irs::funnel_client_t::get_read_size()
{
  irs_i32 need_read_byte = irs_i32(m_area_size) - m_count_read_byte;
  return need_read_byte;
}

void irs::funnel_client_t::tick()
{
  if (!mp_data->connected()) m_is_connected = irs_false;
  if ((!m_is_connected)/* && (m_status != READ)*/)
  {
    if (m_indexes_are_created)
    {
      if ((m_size_index > 0) && (mp_data->connected()))
      {
        if (m_size != (irs_uarc)m_size_index) {
          m_size = m_size_index;
          if (m_fannel_client_mode == fcm_full_size) {
            m_area_size = m_size;
          } else {
            if (m_size > m_area_index) {
              m_area_size = min(m_area_size, m_size-m_area_index);
            } else {
              m_area_size = 0;
            }
          }
          IRS_LIB_ARRAY_DELETE_ASSERT(mp_buf);
          mp_buf = IRS_LIB_NEW_ASSERT(irs_u8[m_area_size], IRSFUNNELCPP_IDX);
          m_is_buf_created = true;
          if (mp_buf) {
            m_current_index = 0;
            if (m_fannel_client_mode == fcm_full_size) {
              m_write_index = -1; //  чтение нулевого элемента
            } else {
              m_write_index = -irs_i32(m_area_index+1);
            }
            m_protect_timer.start();
            m_write_value = 0;
            m_status = READ;
            m_write_vector.clear();
            m_write_vector.resize(m_area_size, false);
          }
        }
        m_is_connected = true;
      }
    }
    else
    {
      if (mp_data->connected())
      {
        irs_uarc index = m_start_index;
        index = m_size_index.connect(mp_data, index);
        index = m_read_index.connect(mp_data, index);
        index = m_read_value.connect(mp_data, index);
        index = m_write_index.connect(mp_data, index);
        index = m_write_value.connect(mp_data, index);
        m_indexes_are_created = irs_true;
      }
    }
  }
  else
  {
    switch (m_status)
    {
      case FREE:
      {
        if (irs_uarc(m_size_index) != m_size) m_is_connected = false;
        else
        {
          if ((m_size_index > 0) && (mp_data->connected()))
          {
            bool write_needed = false;
            irs_uarc write_start_index = 0;
            for (irs_uarc index = 0; index < m_area_size; index++) {
              if (m_write_vector[index] == true) {
                write_needed = true;
                write_start_index = index;
                break;
              }
            }

            if (write_needed)
            {
              m_current_index = 4 * irs_uarc(write_start_index / 4);
              m_write_index = m_current_index + m_area_index + 1;
              m_write_value = *(irs_i32*)&mp_buf[m_current_index];
              for (irs_uarc i = 0; i < 4; i++)
                m_write_vector[m_current_index + i] = true;
              m_on_read_complete = false;
              m_status = WRITE;
            }
            else
            {
              m_current_index = 0;  // m_area_index
              m_write_index = -irs_i32(m_current_index+m_area_index+1);
              m_write_index = -irs_i32(m_current_index+m_area_index+1);
              m_protect_timer.start();
              m_write_value = 0;
              m_status = READ;
            }
          }
          else m_is_connected = irs_false;
        }
        break;
      }
      case WRITE: {
        irs_bool want_free = irs_true;
        for (; m_current_index <= (m_area_size - 4); m_current_index++)
        {
          if (m_write_vector[m_current_index] == true)
          {
            m_current_index = 4 * irs_uarc(m_current_index/4);
            m_write_index = m_current_index + m_area_index + 1;
            m_protect_timer.start();
            m_write_value = *(irs_i32*)&mp_buf[m_current_index];
            for (irs_uarc i = 0; i < 4; i++)
              m_write_vector[m_current_index + i] = true;
            want_free = irs_false;
            m_status = WRITE_WAIT;
            break;
          }
        }
        if (want_free) {
          m_on_reset_stat_read_complete = true;
          m_status = FREE;
        }
      } break;
      case WRITE_WAIT: {
        bool read_index_success =
          (m_read_index == irs_i32(m_current_index + m_area_index + 1));
        bool read_value_success =
          (m_read_value == *(irs_i32*)&mp_buf[m_current_index]);
        if (read_index_success && read_value_success) {
          for (irs_uarc i = 0; i < 4; i++)
            m_write_vector[m_current_index + i] = false;
          m_current_index += 4;
          m_need_write_byte -= 4;
          m_status = WRITE;
        } else if (m_protect_timer.check()) {
          m_write_index = -irs_i32(m_current_index + m_area_index + 1);
          m_protect_timer.start();
          m_status = WRITE_PROTECT;
        }
      } break;
      case WRITE_PROTECT: {
        bool read_success =
          m_read_index == -irs_i32(m_current_index + m_area_index + 1);
        bool protect_timer_signaled = m_protect_timer.check();
        if (read_success || protect_timer_signaled) {
          m_write_index = irs_i32(m_current_index + m_area_index + 1);
          m_write_value = *(irs_i32*)&mp_buf[m_current_index];
          m_protect_timer.start();
          m_status = WRITE_WAIT;
        }
      } break;
      case READ:
      {
        if (irs_uarc(m_size_index) != m_size)
        {
          m_is_connected = false;
          m_status = FREE;
        }
        else if (m_read_index == irs_i32(m_current_index+m_area_index+1))
        {
          if (m_current_index <= (m_area_size - 4))      // ограничение сверху
          {
            if (m_on_reset_stat_read_complete) {
              m_on_reset_stat_read_complete = false;
              m_count_read_byte = 0;
              //m_read_start_index = m_current_index;
            }
            if (m_count_read_byte >= irs_i32(m_area_size))
              m_on_read_complete = true;
            irs_u8 value[4];
            *(irs_i32*)value = m_read_value;
            for (irs_u8 i = 0; i < 4; i++)
              if (m_write_vector[m_current_index + i] == false)
                mp_buf[m_current_index+ i] = value[i];
            m_current_index += 4;
            m_count_read_byte += 4;
            if (m_current_index < m_area_size) {    // ограничение сверху
              m_write_index = -irs_i32(m_current_index + m_area_index + 1);
              m_protect_timer.start();
            } else {
              m_status = FREE;
              //m_is_connected = irs_true;
              set_to_cnt(m_read_counter, m_read_interval);
            }
          }
        // Защита от зависания
        } else if (m_protect_timer.check()) {
          m_current_index -= 8;
          m_count_read_byte -= 8;
          if (m_count_read_byte < 0) {
            m_count_read_byte = 0;
          }
          if (m_current_index > m_area_size) {
            m_current_index = m_area_size - 4;
          }
          m_write_index = -irs_i32(m_current_index + m_area_index + 1);
          m_protect_timer.start();
        }
        break;
      }
    }
  }
}

irs::funnel_server_t::funnel_server_t(mxdata_t *ap_data, irs_uarc a_data_shift,
  mxdata_t *ap_trans_data, irs_uarc a_size, irs_uarc a_start_index):
  mp_data(ap_data),
  mp_trans_data(ap_trans_data),
  m_size(a_size),
  m_current_index(0),
  m_current_value(0),
  m_is_connected(irs_false),
  m_start_index(a_start_index),
  m_prev_operation_read(irs_false),
  m_data_shift(a_data_shift),
  m_size_index(),
  m_write_index(),
  m_write_value(),
  m_read_index(),
  m_read_value()
{
  if (mp_trans_data->connected() && (mp_data->connected()) && m_size)
  {
    irs_uarc index = m_start_index;
    index = m_size_index.connect(mp_trans_data, index);
    index = m_write_index.connect(mp_trans_data, index);
    index = m_write_value.connect(mp_trans_data, index);
    index = m_read_index.connect(mp_trans_data, index);
    index = m_read_value.connect(mp_trans_data, index);

    m_size_index = irs_i32(m_size);
    m_is_connected = irs_true;
  }
}

irs::funnel_server_t::~funnel_server_t()
{
  m_size_index = 0;
}

irs_uarc irs::funnel_server_t::size()
{
  if (m_is_connected) return m_size;
  else return 0;
}

irs_bool irs::funnel_server_t::connected()
{
  return m_is_connected;
}

void irs::funnel_server_t::tick()
{
  if (mp_trans_data->connected() && (mp_data->connected()) && m_size)
  {
    if (!m_is_connected)
    {
      irs_uarc index = m_start_index;
      index = m_size_index.connect(mp_trans_data, index);
      index = m_write_index.connect(mp_trans_data, index);
      index = m_write_value.connect(mp_trans_data, index);
      index = m_read_index.connect(mp_trans_data, index);
      index = m_read_value.connect(mp_trans_data, index);
    }

    m_size_index = irs_i32(m_size);
    m_is_connected = irs_true;

    if (m_read_index < 0)  //  READ from server
    {
      if (irs_i32(m_current_index) != (-1 * irs_i32(m_read_index - 1)))
      {
        m_current_index = -1 * m_read_index - 1;
        irs_u8 value[4];
        mp_data->read(value, m_data_shift + m_current_index, 4);
        m_current_value = *((irs_i32*)value);
        m_write_value = m_current_value;
        m_write_index = m_current_index + 1;
        m_prev_operation_read = irs_true;
      }
    }
    else if (m_read_index > 0)                   //  WRITE to server
    {
      if (((m_current_index != m_read_index-1)
       /*&& (m_current_value != m_read_value)*/)
       || m_prev_operation_read)
      {
        m_current_index = m_read_index-1;
        m_current_value = m_read_value;
        mp_data->write((irs_u8*)&m_current_value,
          m_data_shift + m_current_index, 4);
        m_write_value = m_current_value;
        m_write_index = m_current_index + 1;
        m_prev_operation_read = irs_false;
      }
    }
  }
  else
  {
    if (mp_trans_data->connected()) m_size_index = 0;
    m_is_connected = irs_false;
  }
}
