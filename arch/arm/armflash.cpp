// Дата: 11.05.2011
// Дата создания: 6.05.2011

#include <irsdefs.h>

#include <armflash.h>
#include <mxdata.h>
#include <irsalg.h>

#include <irsfinal.h>

irs::arm::flash_t::flash_t(
  size_t a_size,
  size_t a_flash_end_addr
):
  m_size(a_size),
  m_flash_end_addr(a_flash_end_addr),
  m_flash_begin_addr(m_flash_end_addr - (m_size + m_crc_size)),
  m_crc_error(false)
{
  irs_u32 crc_new = calc_new_crc();
  irs_u32 crc32_flash = crc_flash();
  if (crc_new == crc32_flash) {
    m_crc_error = false;
  } else {
    irs_u32 crc_old = calc_old_crc();
    if (crc_old == crc32_flash) {
      m_crc_error = false;
    } else {
      m_crc_error = true;
    }
  }
}

irs::arm::flash_t::~flash_t()
{

}

irs_uarc irs::arm::flash_t::size()
{
  return m_size;
}

irs_bool irs::arm::flash_t::connected()
{
  return true;
}

void irs::arm::flash_t::read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + m_crc_size + a_index + a_size) <=
    FLASH_END_ADDR);
  irs_u32 index = m_flash_begin_addr + m_crc_size + a_index;
  memcpyex(ap_buf, reinterpret_cast<irs_u8*>(index), a_size);
}

void irs::arm::flash_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + m_crc_size + a_index + a_size) <=
    FLASH_END_ADDR);
  raw_data_t<irs_u8> p_data(m_flash_page_size);
  irs_u32 index = m_flash_begin_addr + a_index -
    ((m_flash_begin_addr + a_index)%m_flash_page_size);
  flash_read_block(p_data.data(), index);
  IRS_LIB_ASSERT(((m_flash_begin_addr + m_crc_size + a_index)%
    m_flash_page_size) < m_flash_page_size);
  memcpyex(p_data.data() + ((m_flash_begin_addr + m_crc_size + a_index)%
    m_flash_page_size), ap_buf, a_size);
  IRS_LIB_ASSERT(((m_flash_begin_addr + m_crc_size)%m_flash_page_size) <
    m_flash_page_size);
  irs_u32 crc = crc32_table(p_data.data() + ((m_flash_begin_addr + m_crc_size)%
    m_flash_page_size), m_size);
  IRS_LIB_ASSERT((m_flash_begin_addr%m_flash_page_size) <
    m_flash_page_size);
  memcpyex(p_data.data() + m_flash_begin_addr%m_flash_page_size,
    reinterpret_cast<irs_u8*>(&crc), m_crc_size);
  flash_erase(index);
  flash_write_block(p_data.data(), index);
}

irs_bool irs::arm::flash_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + 1) <= FLASH_END_ADDR);
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  irs_u32 index = m_flash_begin_addr + m_crc_size + a_index;
  irs_u8 byte = 0;
  memcpyex(&byte, reinterpret_cast<irs_u8*>(index), 1);
  return (byte&static_cast<irs_u8>(1 << a_bit_index));
}

void irs::arm::flash_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + 1) <= FLASH_END_ADDR);
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  raw_data_t<irs_u8> p_data(m_flash_page_size);
  irs_u32 index = m_flash_begin_addr + a_index -
    ((m_flash_begin_addr + a_index)%m_flash_page_size);
  flash_read_block(p_data.data(), index);
  IRS_LIB_ASSERT(((m_flash_begin_addr + m_crc_size + a_index)%m_flash_page_size) <
    m_flash_page_size);
  p_data[(m_flash_begin_addr + m_crc_size + a_index)%m_flash_page_size] |=
    static_cast<irs_u8>(1 << a_bit_index);
  irs_u32 crc = crc32_table(p_data.data() + ((m_flash_begin_addr + m_crc_size)%
    m_flash_page_size), m_size);
  memcpyex(p_data.data() + m_flash_begin_addr%m_flash_page_size,
    reinterpret_cast<irs_u8*>(&crc), m_crc_size);
  flash_erase(index);
  flash_write_block(p_data.data(), index);
}

void irs::arm::flash_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + 1) <= FLASH_END_ADDR);
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  raw_data_t<irs_u8> p_data(m_flash_page_size);
  irs_u32 index = m_flash_begin_addr + a_index -
    ((m_flash_begin_addr + a_index)%m_flash_page_size);
  flash_read_block(p_data.data(), index);
  IRS_LIB_ASSERT(((m_flash_begin_addr + m_crc_size + a_index)%m_flash_page_size) <
    m_flash_page_size);
  p_data[(m_flash_begin_addr + m_crc_size + a_index)%m_flash_page_size] &=
    static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  irs_u32 crc = crc32_table(p_data.data() + ((m_flash_begin_addr + m_crc_size)%
    m_flash_page_size), m_size);
  memcpyex(p_data.data() + m_flash_begin_addr%m_flash_page_size,
    reinterpret_cast<irs_u8*>(&crc), m_crc_size);
  flash_erase(index);
  flash_write_block(p_data.data(), index);
}

void irs::arm::flash_t::flash_erase(irs_u32 a_index)
{
  IRS_LIB_ASSERT((a_index + m_flash_page_size) <= FLASH_END_ADDR);
  FCMISC_bit.AMISC = 1;
  FMA = a_index;
  FMC = m_WRITE_KEY | m_ERASE;
  while(FMC & m_ERASE){}
  if (FCRIS_bit.ARIS) {
    // error
  }
}

void irs::arm::flash_t::flash_write_block(irs_u8* ap_buf,
  irs_u32 a_index)
{
  IRS_LIB_ASSERT((a_index + m_flash_page_size) <= FLASH_END_ADDR);
  irs_u32 size = m_flash_page_size;
  FCMISC_bit.AMISC = 1;
  while(size)
  {
    FMD = reinterpret_cast<const irs_u32&>(*ap_buf);
    FMA = a_index;
    FMC = m_WRITE_KEY | m_WRITE;

    while(FMC & m_WRITE){}
    ap_buf += m_irs_u32_size;
    a_index += m_irs_u32_size;
    size -= m_irs_u32_size;
  }
  if (FCRIS_bit.ARIS) {
    // error
  }
}

void irs::arm::flash_t::flash_read_block(irs_u8* ap_buf, irs_u32 a_index)
{
  IRS_LIB_ASSERT((a_index + m_flash_page_size) <= FLASH_END_ADDR);
  memcpyex(ap_buf, reinterpret_cast<irs_u8*>(a_index),
    m_flash_page_size);
}

irs_u32 irs::arm::flash_t::crc_flash()
{
  irs_u32 crc = 0;
  memcpyex(reinterpret_cast<irs_u8*>(&crc),
    reinterpret_cast<irs_u8*>(m_flash_begin_addr), m_crc_size);
  return crc;
}

irs_u32 irs::arm::flash_t::calc_old_crc() const
{
  return crc32(reinterpret_cast<irs_u32*>(m_flash_begin_addr + m_crc_size),
    0, m_size/4);
}

irs_u32 irs::arm::flash_t::calc_new_crc() const
{
  return crc32_table(reinterpret_cast<irs_u8*>(m_flash_begin_addr + m_crc_size),
    m_size);
}

void irs::arm::flash_t::tick()
{
}

bool irs::arm::flash_t::error()
{
  return m_crc_error;
}

//------------------------------------------------------------------------------

irs::arm::flash_protected_t::flash_protected_t(
  size_t a_size,
  size_t a_flash_end_addr
):
  m_size(a_size),
  m_flash1(a_size, a_flash_end_addr),
  m_flash2(a_size, a_flash_end_addr - m_flash_page_size),
  m_crc_error(false)
{
  if (m_flash1.error() && !m_flash2.error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    m_flash2.read(temp, 0, m_size);
    m_flash1.write(temp, 0, m_size);
    delete []temp;
  }
  else if (m_flash1.error() && m_flash2.error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    m_flash1.read(temp, 0, m_size);
    m_flash2.write(temp, 0, m_size);
    delete []temp;
  }
  else if (m_flash1.error() && m_flash2.error()) m_crc_error = true;
}

irs::arm::flash_protected_t::~flash_protected_t()
{
}

irs_uarc irs::arm::flash_protected_t::size()
{
  return m_size;
}

irs_bool irs::arm::flash_protected_t::connected()
{
  return true;
}

void irs::arm::flash_protected_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  m_flash1.read(ap_buf, a_index, a_size);
}

void irs::arm::flash_protected_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  m_flash1.write(ap_buf, a_index, a_size);
  m_flash2.write(ap_buf, a_index, a_size);
}

irs_bool irs::arm::flash_protected_t::bit(irs_uarc a_index,
  irs_uarc a_bit_index)
{
  return m_flash1.bit(a_index, a_bit_index);
}

void irs::arm::flash_protected_t::set_bit(irs_uarc a_index,
  irs_uarc a_bit_index)
{
  m_flash1.set_bit(a_index, a_bit_index);
  m_flash2.set_bit(a_index, a_bit_index);
}

void irs::arm::flash_protected_t::clear_bit(irs_uarc a_index,
  irs_uarc a_bit_index)
{
  m_flash1.clear_bit(a_index, a_bit_index);
  m_flash2.clear_bit(a_index, a_bit_index);
}

void irs::arm::flash_protected_t::tick()
{

}

bool irs::arm::flash_protected_t::double_error()
{
  return m_crc_error;
}
