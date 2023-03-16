#include <i2c.h>
#include <irserror.h>
#include <string.h>



using namespace irs;

spi_to_i2c_t::spi_to_i2c_t(i2c_t *a_i2c, irs_u16 a_addr_device,
  irs_u16 a_buffer_size):
  mp_i2c(a_i2c),
  m_address_device(a_addr_device),
  m_buffer_size(a_buffer_size),
  mp_buffer(new irs_u8 [a_buffer_size]())
{
  IRS_ASSERT((m_buffer_size != 0) && (mp_buffer != nullptr));
}

void spi_to_i2c_t::write(const irs_u8 *ap_buf, irs_uarc a_size)
{
  IRS_ASSERT((a_size != 0) && (ap_buf != nullptr));
  memcpy(mp_buffer, ap_buf, a_size);
  mp_i2c->set_device_address(m_address_device);
  mp_i2c->write(const_cast<irs_u8*>(mp_buffer), a_size);
}

void spi_to_i2c_t::read(irs_u8 *ap_buf,irs_uarc a_size)
{
  mp_i2c->set_device_address(m_address_device);
  mp_i2c->read(ap_buf, a_size);
}

spi_to_i2c_t::status_t spi_to_i2c_t::get_status()
{
  return (mp_i2c->is_free()) ? FREE : BUSY;
}

void spi_to_i2c_t::lock()
{
  mp_i2c->lock();
}

void spi_to_i2c_t::unlock()
{
  mp_i2c->unlock();
}

bool spi_to_i2c_t::get_lock()
{
  return mp_i2c->get_lock();
}

void spi_to_i2c_t::tick()
{
  mp_i2c->tick();
}

void spi_to_i2c_t::abort()
{
  mp_i2c->abort();
}

bool spi_to_i2c_t::set_data_size(irs_u16 a_data_size)
{
  return true;
}

bool spi_to_i2c_t::set_bitrate(irs_u32 a_bitrate)
{
  return true;
}

bool spi_to_i2c_t::set_polarity(polarity_t a_polarity)
{
  return true;
}

bool spi_to_i2c_t::set_phase(phase_t a_phase)
{
  return true;
}

bool spi_to_i2c_t::set_order(order_t a_order)
{
  return true;
}
