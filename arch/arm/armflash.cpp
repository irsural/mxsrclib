// Дата: 11.05.2011
// Дата создания: 6.05.2011

#include <irsdefs.h>

#include <armflash.h>
#include <mxdata.h>

#include <irsfinal.h>

irs::arm::flash_t::flash_t(
  size_t a_size,
  size_t a_flash_end_addr
):
  m_size(a_size),
  m_flash_end_addr(a_flash_end_addr),
  m_flash_begin_addr(m_flash_end_addr - m_size)
{
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
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + a_size) <= m_flash_end_addr);
  irs_u32 index = m_flash_begin_addr + a_index;
  memcpyex(ap_buf, reinterpret_cast<irs_u8*>(index), a_size);
}

void irs::arm::flash_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + a_size) <= m_flash_end_addr);
  raw_data_t<irs_u8> p_data(m_flash_page_size);
  irs_u32 index = m_flash_begin_addr + a_index -
    ((m_flash_begin_addr + a_index)%m_flash_page_size);
  flash_read_block(p_data.data(), index);
  memcpyex(p_data.data() + ((m_flash_begin_addr + a_index)%m_flash_page_size),
    ap_buf, a_size);
  flash_erase(index);
  flash_write_block(p_data.data(), index);
}

irs_bool irs::arm::flash_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + 1) <= m_flash_end_addr);
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  irs_u32 index = m_flash_begin_addr + a_index;
  irs_u8 byte = 0;
  memcpyex(&byte, reinterpret_cast<irs_u8*>(index), 1);
  return (byte&static_cast<irs_u8>(1 << a_bit_index));
}

void irs::arm::flash_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + 1) <= m_flash_end_addr);
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  raw_data_t<irs_u8> p_data(m_flash_page_size);
  irs_u32 index = m_flash_begin_addr + a_index -
    ((m_flash_begin_addr + a_index)%m_flash_page_size);
  flash_read_block(p_data.data(), index);
  p_data[(m_flash_begin_addr + a_index)%m_flash_page_size] |=
    static_cast<irs_u8>(1 << a_bit_index);
  flash_erase(index);
  flash_write_block(p_data.data(), index);
}

void irs::arm::flash_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  IRS_LIB_ASSERT((m_flash_begin_addr + a_index + 1) <= m_flash_end_addr);
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  raw_data_t<irs_u8> p_data(m_flash_page_size);
  irs_u32 index = m_flash_begin_addr + a_index -
    ((m_flash_begin_addr + a_index)%m_flash_page_size);
  flash_read_block(p_data.data(), index);
  p_data[(m_flash_begin_addr + a_index)%m_flash_page_size] &=
    static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  flash_erase(index);
  flash_write_block(p_data.data(), index);
}

void irs::arm::flash_t::flash_erase(irs_u32 a_index)
{
  IRS_LIB_ASSERT((a_index + m_flash_page_size) <= m_flash_end_addr);
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
  IRS_LIB_ASSERT((a_index + m_flash_page_size) <= m_flash_end_addr);
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
  IRS_LIB_ASSERT((a_index + m_flash_page_size) <= m_flash_end_addr);
  memcpyex(ap_buf, reinterpret_cast<irs_u8*>(a_index),
    m_flash_page_size);
}

void irs::arm::flash_t::tick()
{
}
