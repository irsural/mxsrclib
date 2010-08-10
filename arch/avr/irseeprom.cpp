// ƒоступ к EEPROM AVR
// ƒата: 9.08.2010
// –анн€€ ƒата: 19.08.2009

#include <irseeprom.h>
#include <irserror.h>

__no_init __eeprom irs_u8
  irs::avr::eeprom_t::mp_ee_data[irs::avr::eeprom_t::m_data_size] @
  irs::avr::eeprom_t::m_begin_address;

irs::avr::eeprom_t::eeprom_t(irs_uarc a_start_index, irs_uarc a_size):
  m_start_index(a_start_index),
  m_size(a_size),
  m_crc_error(false)
{
  irs_u32 crc = crc32_table((irs_u8*)&mp_ee_data[m_start_index +
    m_crc_size], 0);
  if (crc == *(__eeprom irs_u32*)&mp_ee_data[m_start_index]) {
    m_crc_error = false;
  } else {
    m_crc_error = true;
    fill_n(&mp_ee_data[m_start_index + m_crc_size], m_size, 0);
    *(__eeprom irs_u32*)&mp_ee_data[m_start_index] =
      crc32_table((irs_u8*)&mp_ee_data[m_start_index + m_crc_size], 0);
  }
}

irs::avr::eeprom_t::~eeprom_t()
{
  //
}

irs_uarc irs::avr::eeprom_t::size()
{
  return m_size;
}
irs_bool irs::avr::eeprom_t::connected()
{
  return irs_true;
}

void irs::avr::eeprom_t::read(irs_u8 *buf, irs_uarc index, irs_uarc size)
{
  if (index >= m_size) return;
  irs_uarc user_size = size;
  if (user_size > m_size - index) user_size = m_size - index;
  copy(mp_ee_data + m_crc_size + m_start_index + index,
    mp_ee_data + m_crc_size + m_start_index + index + user_size, buf);
}

void irs::avr::eeprom_t::write(const irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  if (index >= m_size) return;
  irs_uarc user_size = size;
  if (size > m_size - index) user_size = m_size - index;
  copy(buf, buf + user_size, mp_ee_data + m_crc_size + m_start_index + index);
  *(__eeprom irs_u32*)&mp_ee_data[m_start_index] =
    crc32_table((irs_u8*)&mp_ee_data[m_start_index + m_crc_size], 0);
}

irs_bool irs::avr::eeprom_t::bit(irs_uarc index, irs_uarc bit_index)
{
  if (index >= m_size) return irs_false;
  if (bit_index >= 8) return irs_false;
  irs_u8 value = mp_ee_data[m_start_index + index];
  /*< 7 6 5 4 3 2 1 0 > bit_index */
  value = (value >> bit_index) & 1;
  return value;
}

void irs::avr::eeprom_t::set_bit(irs_uarc index, irs_uarc bit_index)
{
  if (index >= m_size) return;
  if (bit_index >= 8) return;
  mp_ee_data[m_start_index + m_crc_size + index] |= (1 << bit_index);
  *(__eeprom irs_u32*)&mp_ee_data[m_start_index] =
    crc32_table((irs_u8*)&mp_ee_data[m_start_index + m_crc_size], 0);
}

void irs::avr::eeprom_t::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  if (index >= m_size) return;
  if (bit_index >= 8) return;
  mp_ee_data[m_start_index + m_crc_size + index] |= (1 << bit_index);
  *(__eeprom irs_u32*)&mp_ee_data[m_start_index]
    = crc32_table((irs_u8*)&mp_ee_data[m_start_index + m_crc_size], 0);
}

void irs::avr::eeprom_t::tick()
{
  return;
}

bool irs::avr::eeprom_t::error()
{
  return m_crc_error;
}

irs::avr::eeprom_reserve_t::eeprom_reserve_t(eeprom_t *ap_eeprom1,
  eeprom_t *ap_eeprom2, irs_uarc a_size):
  mp_eeprom1(ap_eeprom1),
  mp_eeprom2(ap_eeprom2),
  m_size(a_size),
  m_error(false)
{
  if (mp_eeprom1->error() && !mp_eeprom2->error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    mp_eeprom2->read(temp, 0, m_size);
    mp_eeprom1->write(temp, 0, m_size);
    delete []temp;
  }
  else if (!mp_eeprom1->error() && mp_eeprom2->error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    mp_eeprom1->read(temp, 0, m_size);
    mp_eeprom2->write(temp, 0, m_size);
    delete []temp;
  }
  else if (mp_eeprom1->error() && mp_eeprom2->error()) m_error = true;
}

irs::avr::eeprom_reserve_t::~eeprom_reserve_t()
{
}

irs_uarc irs::avr::eeprom_reserve_t::size()
{
  return m_size;
}

irs_bool irs::avr::eeprom_reserve_t::connected()
{
  return (mp_eeprom1 && mp_eeprom2 && m_size);
}

void irs::avr::eeprom_reserve_t::read(irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->read(buf, index, size);
}

void irs::avr::eeprom_reserve_t::write(const irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->write(buf, index, size);
  mp_eeprom2->write(buf, index, size);
}

irs_bool irs::avr::eeprom_reserve_t::bit(irs_uarc index, irs_uarc bit_index)
{
  return mp_eeprom1->bit(index, bit_index);
}

void irs::avr::eeprom_reserve_t::set_bit(irs_uarc index, irs_uarc bit_index)
{
  mp_eeprom1->set_bit(index, bit_index);
  mp_eeprom2->set_bit(index, bit_index);
}

void irs::avr::eeprom_reserve_t::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  mp_eeprom1->clear_bit(index, bit_index);
  mp_eeprom2->clear_bit(index, bit_index);
}

void irs::avr::eeprom_reserve_t::tick()
{
}

bool irs::avr::eeprom_reserve_t::double_error()
{
  return m_error;
}

//----------------------------- EEPROM PROTECTED -------------------------------

irs::avr::eeprom_protected_t::eeprom_protected_t(
  irs_uarc a_start_index,
  irs_uarc a_size,
  irs_u32 a_eeprom_id,
  old_eeprom_data_t* ap_old_eeprom_data
):
  mp_eeprom_header(IRS_NULL),
  mp_eeprom1(IRS_NULL),
  mp_eeprom2(IRS_NULL),
  m_error(false)
{
  irs_u8 __eeprom* p_ee_data = (irs_u8 __eeprom*)m_eeprom_begin_address;
  if (a_eeprom_id) {
    irs_uarc old_id_idx = 0;
    if (old_eeprom_id_index(a_eeprom_id, old_id_idx)) {
      // eeprom нового типа (3 блока)
      irs_uarc old_size = reinterpret_cast<irs_uarc __eeprom&>(*(p_ee_data +
        old_id_idx + sizeof(irs_u32)));
      irs_uarc copy_size = (old_size > a_size) ? a_size : old_size;
      eeprom_t old_eeprom1(old_id_idx + sizeof(irs_u32) + sizeof(irs_uarc),
        old_size);
      eeprom_t old_eeprom2(old_id_idx + sizeof(irs_u32) + sizeof(irs_uarc) +
        m_crc_size + old_size, old_size);
      if (!old_eeprom2.error()) {
        if (old_id_idx >= a_start_index) {
          copy(p_ee_data + old_id_idx + m_eeprom_header_size +
            m_crc_size*2 + old_size, p_ee_data + old_id_idx +
            m_eeprom_header_size + m_crc_size*2 + old_size + copy_size,
            p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*3 +
            a_size);
        } else {
          copy_backward(p_ee_data + old_id_idx + m_eeprom_header_size +
            m_crc_size*2 + old_size, p_ee_data + old_id_idx +
            m_eeprom_header_size + m_crc_size*2 + old_size + copy_size,
            p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*3 +
            a_size + copy_size);
        }
        // копируем из еепром2 в еепром1
        copy(p_ee_data + a_start_index + m_eeprom_header_size +
            m_crc_size*3 + a_size, p_ee_data + a_start_index + 
            m_eeprom_header_size + m_crc_size*3 + a_size*2,
            p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2);
      } else if (!old_eeprom1.error()) {
        if (old_id_idx >= a_start_index) {
          copy(p_ee_data + old_id_idx + m_eeprom_header_size +
            m_crc_size, p_ee_data + old_id_idx + m_eeprom_header_size +
            m_crc_size + copy_size, p_ee_data +
            a_start_index + m_eeprom_header_size + m_crc_size*2);
        } else {
          copy_backward(p_ee_data + old_id_idx + m_eeprom_header_size +
            m_crc_size, p_ee_data + old_id_idx + m_eeprom_header_size +
            m_crc_size + copy_size, p_ee_data +
            a_start_index + m_eeprom_header_size + m_crc_size*2 + copy_size);
        }
        // копируем из еепром1 в еепром2
        copy(p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2,
          p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2 +
          a_size, p_ee_data + a_start_index + m_eeprom_header_size +
          m_crc_size*3 + a_size);
      } else if (old_eeprom1.error() && old_eeprom2.error()) {
        m_error = true;
      }
    } else {
      // eeprom старого типа (2 блока)
      eeprom_t old_eeprom1(ap_old_eeprom_data->index, ap_old_eeprom_data->size);
      eeprom_t old_eeprom2(ap_old_eeprom_data->index +
        ap_old_eeprom_data->size + m_crc_size, ap_old_eeprom_data->size);
      irs_uarc copy_size = (ap_old_eeprom_data->size > a_size) ?
        a_size : ap_old_eeprom_data->size;
      if (!old_eeprom2.error()) {
        irs_uarc in_begin = ap_old_eeprom_data->index + m_crc_size*2 + 
          ap_old_eeprom_data->size;
        irs_uarc in_end = ap_old_eeprom_data->index + m_crc_size*2 + 
          ap_old_eeprom_data->size + copy_size;
        if (ap_old_eeprom_data->index >= (a_start_index + m_crc_size +
          m_eeprom_header_size))
        {
          irs_uarc out_begin = a_start_index + m_eeprom_header_size +
            m_crc_size*3 + a_size;
          copy(p_ee_data + in_begin, p_ee_data + in_end, p_ee_data + out_begin);
        } else {
          irs_uarc out_end = a_start_index + m_eeprom_header_size +
            m_crc_size*3 + a_size + copy_size;
          copy_backward(p_ee_data + in_begin, p_ee_data + in_end,
            p_ee_data + out_end);
        }
        // копируем из еепром2 в еепром1
        copy(p_ee_data + a_start_index + m_eeprom_header_size +
            m_crc_size*3 + a_size, p_ee_data + a_start_index + 
            m_eeprom_header_size + m_crc_size*3 + a_size*2,
            p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2);
      } else if (!old_eeprom1.error()) {
        if (ap_old_eeprom_data->index >= (a_start_index + m_crc_size +
          m_eeprom_header_size))
        {
          copy(p_ee_data + m_crc_size,
            p_ee_data + m_crc_size + copy_size,
            p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2);
        } else {
          copy_backward(p_ee_data + m_crc_size,
            p_ee_data + m_crc_size + copy_size,
            p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2 +
            copy_size);
        }
        // копируем из еепром1 в еепром2
        copy(p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2,
          p_ee_data + a_start_index + m_eeprom_header_size + m_crc_size*2 +
          a_size, p_ee_data + a_start_index + m_eeprom_header_size +
          m_crc_size*3 + a_size);
      } else if (old_eeprom1.error() && old_eeprom2.error()) {
        m_error = true;
      }
      // CRC32 дл€ eeprom1
      *(__eeprom irs_u32*)&p_ee_data[a_start_index + m_crc_size +
        m_eeprom_header_size] = crc32_table((irs_u8*)&p_ee_data[a_start_index +
        m_crc_size*2 + m_eeprom_header_size], 0);
      // CRC32 дл€ eeprom2
      *(__eeprom irs_u32*)&p_ee_data[a_start_index + m_crc_size*2 +
        m_eeprom_header_size + a_size] =
        crc32_table((irs_u8*)&p_ee_data[a_start_index + m_crc_size*3 +
        m_eeprom_header_size + a_size], 0);
    }
    *(__eeprom irs_u32*)&p_ee_data[a_start_index + m_crc_size] =
      a_eeprom_id;
    *(__eeprom irs_u32*)&p_ee_data[a_start_index + m_crc_size +
      sizeof(irs_u32)] = a_size;    
    mp_eeprom_header.reset(new eeprom_t(a_start_index, m_eeprom_header_size));
    mp_eeprom1.reset(new eeprom_t(a_start_index + m_crc_size +
      m_eeprom_header_size, a_size));
    mp_eeprom2.reset(new eeprom_t(a_start_index + m_crc_size*2 +
      m_eeprom_header_size + a_size, a_size));
  } else {
    mp_eeprom1.reset(new eeprom_t(a_start_index, a_size));
    mp_eeprom2.reset(new eeprom_t(a_start_index + m_crc_size + a_size, a_size));
    if (mp_eeprom1->error() && !mp_eeprom2->error()) {
      copy(p_ee_data + a_start_index + a_size + m_crc_size*2,
        p_ee_data + a_start_index + (a_size + m_crc_size)*2,
        p_ee_data + a_start_index + m_crc_size);
    } else if (!mp_eeprom1->error() && mp_eeprom2->error()) {
      copy(p_ee_data + a_start_index + m_crc_size,
        p_ee_data + a_start_index + m_crc_size + a_size,
        p_ee_data + a_start_index + a_size + m_crc_size*2);
    } else if (mp_eeprom1->error() && mp_eeprom2->error()) {
      m_error = true;
    }
  }
}

irs::avr::eeprom_protected_t::~eeprom_protected_t()
{
}

bool irs::avr::eeprom_protected_t::old_eeprom_id_index(irs_u32 a_search_id,
  irs_uarc& a_old_eeprom_id_idx)
{
  irs_u8 __eeprom* p_ee_data = (irs_u8 __eeprom*)m_eeprom_begin_address;
  for (irs_uarc old_index = 0; old_index < (m_eeprom_size - sizeof(irs_u32));
    old_index++)
  {
    irs_u32 old_id = reinterpret_cast<irs_u32 __eeprom&>(
      *(p_ee_data + old_index));
    if (a_search_id == old_id) {
      a_old_eeprom_id_idx = old_index;
      return true;
    }
  }
  return false;
}

irs_uarc irs::avr::eeprom_protected_t::size()
{
  irs_uarc size = 0;
  mp_eeprom_header->read((irs_u8*)&size, m_crc_size + sizeof(irs_u32),
    sizeof(irs_uarc));
  return size;
}

irs_bool irs::avr::eeprom_protected_t::connected()
{
  irs_uarc size = 0;
  mp_eeprom_header->read((irs_u8*)&size, m_crc_size + sizeof(irs_u32),
    sizeof(irs_uarc));
  return size;
}

void irs::avr::eeprom_protected_t::read(irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->read(buf, index, size);
}

void irs::avr::eeprom_protected_t::write(const irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->write(buf, index, size);
  mp_eeprom2->write(buf, index, size);
}

irs_bool irs::avr::eeprom_protected_t::bit(irs_uarc index, irs_uarc bit_index)
{
  return mp_eeprom1->bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::set_bit(irs_uarc index, irs_uarc bit_index)
{
  mp_eeprom1->set_bit(index, bit_index);
  mp_eeprom2->set_bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  mp_eeprom1->clear_bit(index, bit_index);
  mp_eeprom2->clear_bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::tick()
{
}

bool irs::avr::eeprom_protected_t::error()
{
  return m_error;
}
