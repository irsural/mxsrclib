#include <irseeprom.h>
#include <irserror.h>

using namespace irs;

eeprom_m24_page_t::eeprom_m24_page_t(irs_u16 a_i2c_address, i2c_t* ap_i2c,
  size_type a_page_size, size_type a_page_count, size_t a_address_size
):
  m_i2c_address(a_i2c_address),
  m_page_size(a_page_size),
  m_page_count(a_page_count),
  mp_i2c(ap_i2c),
  m_mem_addr(0x0),
  m_status(m24_free),
  mp_buffer(0),
  m_max_seek(m_page_size * m_page_count - 1),
  m_address_size(a_address_size),
  mp_next_page(new irs_u8[m_page_size + m_address_size]()),
  mp_mem_addr_buf(new irs_u8[m_address_size]())
{

}

eeprom_m24_page_t::~eeprom_m24_page_t()
{
  delete [] mp_next_page;
  delete [] mp_mem_addr_buf;
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
  return (m_status == m24_free) ? irs_st_ready : irs_st_busy;
}

void eeprom_m24_page_t::tick()
{
  mp_i2c->tick();
  switch(m_status)
  {
    case m24_free: {
    } break;

    case m24_read: {
      if(mp_i2c->get_status() == irs_st_ready)
      {
        send_seek();
        m_status = m24_send_addr;
      }
    } break;

    case m24_send_addr: {
      if(mp_i2c->get_status() == irs_st_ready)
      {
        mp_i2c->read(mp_buffer, m_page_size);
        m_status = m24_wait;
      }
    } break;

    case m24_write: {
      if(mp_i2c->get_status() == irs_st_ready)
      {
          write_buf();
          m_status = m24_wait;
      }
    } break;

    case m24_wait: {
      if (mp_i2c->get_status() == irs_st_ready) {
        mp_i2c->unlock();
        m_status = m24_free;
      }
    } break;
  }
}

void eeprom_m24_page_t::write_buf()
{
  memcpy(mp_next_page + m_address_size, mp_buffer, m_page_size);

  mp_next_page[0] = static_cast<irs_u8>((m_mem_addr & 0xFF00) >> 8);
  mp_next_page[1] = static_cast<irs_u8>(m_mem_addr & 0xFF);

  mp_i2c->write(mp_next_page, m_page_size + m_address_size);
}

void eeprom_m24_page_t::send_seek()
{
  mp_mem_addr_buf[0] = static_cast<irs_u8>((m_mem_addr & 0xFF00) >> 8);
  mp_mem_addr_buf[1] = static_cast<irs_u8>(m_mem_addr & 0xFF);

  mp_i2c->write(mp_mem_addr_buf, 2);
}

void eeprom_m24_page_t::initialize_io_operation(irs_u8* ap_data, irs_u16 a_index,
  m24_status_t a_status)
{
  IRS_ASSERT(ap_data != nullptr);
  IRS_ASSERT(!mp_i2c->get_lock());
  IRS_ASSERT(is_free());

  mp_i2c->lock();
  mp_i2c->set_device_address(m_i2c_address);
  mp_buffer = ap_data;
  m_status = a_status;

  m_mem_addr = (a_index * m_page_size > m_max_seek)
             ? m_max_seek
             : (a_index * m_page_size);
}

bool eeprom_m24_page_t::is_free()
{
  return (m_status == m24_free);
}


// *****************************************************************************


eeprom_i2c_t::eeprom_i2c_t(irs_u16 a_i2c_address, i2c_t* ap_i2c, irs_uarc a_size,
  size_t a_page_size, size_t a_page_count, size_t a_address_size,
  size_t a_cluster_size, bool a_init_now, irs_uarc a_index,
  counter_t a_init_timeout
):
  mxdata_comm_t(IRS_NULL, a_index, a_size, a_init_now, a_init_timeout),
  m_page_mem(a_i2c_address, ap_i2c, a_page_size, a_page_count, a_address_size),
  m_mem_data(&m_page_mem, a_cluster_size)
{
  IRS_ASSERT(m_mem_data.size() >= a_size);
  connect(&m_mem_data);
}
