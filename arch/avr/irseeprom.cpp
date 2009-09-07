// Доступ к EEPROM AVR
// Дата: 19.08.2009

#include <irseeprom.h>

__no_init __eeprom irs_u8
  irs::avr::eeprom_t::mp_ee_data[irs::avr::eeprom_t::m_data_size] @
  irs::avr::eeprom_t::m_begin_address;

irs::avr::eeprom_t::eeprom_t(irs_uarc a_start_index, irs_uarc a_size):
  m_start_index(a_start_index),
  m_size(a_size)
{
  irs_u32 crc = crc32((__eeprom irs_u32*)&mp_ee_data[m_start_index +
    m_crc_size], 0, m_size/4);
  if (crc == *(__eeprom irs_u32*)&mp_ee_data[m_start_index])
  {
    m_crc_error = false;
  }
  else
  {
    m_crc_error = true;
    fill_n(&mp_ee_data[m_start_index + m_crc_size], m_size, 0);
    *(__eeprom irs_u32*)&mp_ee_data[m_start_index]
      = crc32((__eeprom irs_u32*)&mp_ee_data[m_start_index + m_crc_size],
      0, m_size/4);
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
  *(__eeprom irs_u32*)&mp_ee_data[m_start_index]
    = crc32((__eeprom irs_u32*)&mp_ee_data[m_start_index + m_crc_size], 0,
    m_size/4);
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
  *(__eeprom irs_u32*)&mp_ee_data[m_start_index]
    = crc32((__eeprom irs_u32*)&mp_ee_data[m_start_index + m_crc_size],
    0, m_size/4);
}

void irs::avr::eeprom_t::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  if (index >= m_size) return;
  if (bit_index >= 8) return;
  mp_ee_data[m_start_index + m_crc_size + index] |= (1 << bit_index);
  *(__eeprom irs_u32*)&mp_ee_data[m_start_index]
    = crc32((__eeprom irs_u32*)&mp_ee_data[m_start_index + m_crc_size],
    0, m_size/4);
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

irs::avr::eeprom_protected_t::eeprom_protected_t(irs_uarc a_start_index,
  irs_uarc a_size):
  m_index_delta(4),
  m_size(a_size),
  m_eeprom1(a_start_index, m_size),
  m_eeprom2(a_start_index + m_index_delta + m_size, m_size),
  m_error(false)
{
  if (m_eeprom1.error() && !m_eeprom2.error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    m_eeprom2.read(temp, 0, m_size);
    m_eeprom1.write(temp, 0, m_size);
    delete []temp;
  }
  else if (!m_eeprom1.error() && m_eeprom2.error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    m_eeprom1.read(temp, 0, m_size);
    m_eeprom2.write(temp, 0, m_size);
    delete []temp;
  }
  else if (m_eeprom1.error() && m_eeprom2.error()) m_error = true;
}

irs::avr::eeprom_protected_t::~eeprom_protected_t()
{
}

irs_uarc irs::avr::eeprom_protected_t::size()
{
  return m_size;
}

irs_bool irs::avr::eeprom_protected_t::connected()
{
  return m_size;
}

void irs::avr::eeprom_protected_t::read(irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  m_eeprom1.read(buf, index, size);
}

void irs::avr::eeprom_protected_t::write(const irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  m_eeprom1.write(buf, index, size);
  m_eeprom2.write(buf, index, size);
}

irs_bool irs::avr::eeprom_protected_t::bit(irs_uarc index, irs_uarc bit_index)
{
  return m_eeprom1.bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::set_bit(irs_uarc index, irs_uarc bit_index)
{
  m_eeprom1.set_bit(index, bit_index);
  m_eeprom2.set_bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  m_eeprom1.clear_bit(index, bit_index);
  m_eeprom2.clear_bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::tick()
{
}

bool irs::avr::eeprom_protected_t::error()
{
  return m_error;
}
