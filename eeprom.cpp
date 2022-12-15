#include "eeprom.h"

using namespace irs;

eeprom_m24_page_t::eeprom_m24_page_t(irs_u16 a_addr, i2c_t* ap_i2c,
  size_type a_page_size,size_type a_page_count, size_t a_address_size
):
  m_addr(a_addr),
  m_page_size(a_page_size),
  m_page_count(a_page_count),
  mp_i2c(ap_i2c),
  m_eeprom_seek(0x0),
  m_status(m24_free),
  mp_buffer(0),
  m_max_seek(m_page_size * m_page_count - 1),
  m_address_size(a_address_size),
  mp_next_page(new irs_u8[m_page_size + m_address_size]),
  mp_eeprom_seek(new irs_u8[m_address_size])
{
  if(mp_next_page)
    memset(mp_next_page, 0, m_page_size + m_address_size);

  if(mp_eeprom_seek)
    memset(mp_eeprom_seek, 0, m_address_size);
}

eeprom_m24_page_t::~eeprom_m24_page_t()
{
  delete [] mp_next_page;
  delete [] mp_eeprom_seek;
}

void eeprom_m24_page_t::read_page(irs_u8 *ap_buf, irs_uarc a_index)
{
  initialize_io_operation(const_cast<irs_u8*>(ap_buf), a_index, m24_read);
}

void eeprom_m24_page_t::write_page(const irs_u8 *ap_buf, irs_uarc a_index)
{
  initialize_io_operation(const_cast<irs_u8*>(ap_buf), a_index, m24_write);
}

size_t eeprom_m24_page_t::page_size() const
{
  return m_page_size;
}

irs_uarc eeprom_m24_page_t::page_count() const
{
  return m_page_count;
}

irs_status_t eeprom_m24_page_t::status() const
{
  if(m_status == m24_free)
    return irs_st_ready;
  else
    return irs_st_busy;
}

void eeprom_m24_page_t::tick()
{
  mp_i2c->tick();
  switch(m_status)
  {
    case m24_free: {
    } break;

    case m24_read: {
      if(mp_i2c->get_status() == spi_to_i2c_t::FREE)
      {
        send_seek();
        m_status = m24_send_addr;
      }
    } break;

    case m24_send_addr: {
      if(mp_i2c->get_status() == spi_to_i2c_t::FREE)
      {
        mp_i2c->read(mp_buffer, m_page_size);
        m_status = m24_wait;
      }
    } break;

    case m24_write: {
      if(mp_i2c->get_status() == spi_to_i2c_t::FREE)
      {
          write_buf();
          m_status = m24_wait;
      }
    } break;

    case m24_wait: {
      if (mp_i2c->get_status() == spi_to_i2c_t::FREE) {
        mp_i2c->unlock();
        m_status = m24_free;
      }
    } break;
  }
}

void eeprom_m24_page_t::write_buf()
{
  memcpy(mp_next_page + m_address_size, mp_buffer, m_page_size);

  mp_next_page[0] = static_cast<irs_u8>((m_eeprom_seek & 0xFF00) >> 8);
  mp_next_page[1] = static_cast<irs_u8>(m_eeprom_seek & 0xFF);

  mp_i2c->write(mp_next_page, m_page_size + m_address_size);
}

void eeprom_m24_page_t::send_seek()
{
  mp_eeprom_seek[0] = static_cast<irs_u8>((m_eeprom_seek & 0xFF00) >> 8);
  mp_eeprom_seek[1] = static_cast<irs_u8>(m_eeprom_seek & 0xFF);

  mp_i2c->write(mp_eeprom_seek, 2);
}

void eeprom_m24_page_t::initialize_io_operation(irs_u8* ap_data, irs_u16 a_seek, m24_status_t a_status)
{
  assert(ap_data != nullptr);
  assert(!mp_i2c->get_lock());
  assert(is_free());

  mp_i2c->lock();
  mp_i2c->set_device_address(m_addr);
  mp_buffer = ap_data;
  m_status = a_status;

  //a_seek        - страница начала записи
  //m_eeprom_seek - адрес байта начала записи
  m_eeprom_seek = (a_seek * m_page_size > m_max_seek) ? m_max_seek : a_seek * m_page_size;
}

/*void eeprom_m24_page_t::set_eeprom_seek()
{
  irs_u32 msk = 0xFF;
  irs_u32 offset = 0;
  for(int i = m_address_size; i > 0; i--)
  {
    offset = 8 * (m_address_size - i);
    msk <<= offset;
    mp_eeprom_seek[i-1] = static_cast<irs_u8>((msk & m_eeprom_seek) >> offset);

  }
}*/

bool eeprom_m24_page_t::is_free()
{
  return (m_status == m24_free);
}



//***********************************************************************************************************************



eeprom_i2c_t::eeprom_i2c_t(irs_u16 a_addr, i2c_t* ap_i2c, irs_uarc a_size,
  size_t a_page_size, size_t a_page_count, size_t a_address_size,
  size_t a_cluster_size, bool a_init_now, irs_uarc a_index,
  counter_t a_init_timeout
):
  mxdata_comm_t(IRS_NULL, a_index, a_size, a_init_now, a_init_timeout),
  m_page_mem(a_addr, ap_i2c, a_page_size, a_page_count, a_address_size),
  m_mem_data(&m_page_mem, a_cluster_size)
{
  IRS_ASSERT(m_mem_data.size() >= a_size);
  connect(&m_mem_data);
}