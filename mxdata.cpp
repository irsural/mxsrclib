// ������� ��� ������ � mxdata_t
// ����: 4.08.2009

#include <string.h>

#include <mxdata.h>

// ����� �������� ��������� ��� bit_data_t
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

// ����������� ���������� ����������� ������� mxdata_t � ���� �����
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
  // ������� ������� ���������� ����������� �������
  if ((a_area_begin <= a_begin) && (a_end <= a_area_end)) {
    *a_out_begin = a_begin;
    *a_out_end = a_end;
    belong_result = true;
  // ������� ������� ������ � ����������� �������
  } else if ((a_begin <= a_area_begin) && (a_area_end <= a_end)) {
    *a_out_begin = a_area_begin;
    *a_out_end = a_area_end;
    belong_result = true;
  // ������ ������� ������� ������ � ����������� �������
  } else if ((a_begin <= a_area_begin) && (a_area_begin < a_end)) {
    *a_out_begin = a_area_begin;
    *a_out_end = a_end;
    belong_result = true;
  // ����� ������� ������� ������ � ����������� �������
  } else if ((a_begin < a_area_end) && (a_area_end <= a_end)) {
    *a_out_begin = a_begin;
    *a_out_end = a_area_end;
    belong_result = true;
  // ������� ������� �� ������������ � ����������� ��������
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
          it->p_data->read(ap_buf_read + begin, index, size);
        } break;
        case rw_write: {
          it->p_data->write(ap_buf_write + begin, index, size);
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

