// Дата: 11.05.2011
// Дата создания: 6.05.2011

#include <irsdefs.h>

#include <armflash.h>
#include <irscpu.h>
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

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
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

#elif defined(__STM32F100RBT__)
void irs::arm::flash_t::flash_erase(irs_u32 /*a_index*/)
{
}
#elif defined(IRS_STM32F_2_AND_4)
void irs::arm::flash_t::flash_erase(irs_u32 /*a_index*/)
{
}
#else
  #error Тип контроллера не определён
#endif  //  mcu type

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
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
#elif defined(__STM32F100RBT__)
void irs::arm::flash_t::flash_write_block(irs_u8* /*ap_buf*/,
  irs_u32 /*a_index*/)
{
}
#elif defined(IRS_STM32F_2_AND_4)
void irs::arm::flash_t::flash_write_block(irs_u8* /*ap_buf*/,
  irs_u32 /*a_index*/)
{
}
#else
  #error Тип контроллера не определён
#endif  //  mcu type

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

// class st_flash_t
irs::arm::st_flash_t::st_flash_t():
  m_process(process_wait_command),
  m_status(irs_st_ready),
  mp_pos(IRS_NULL),
  mp_buf(IRS_NULL),
  m_page_index(0),
  m_max_psize(psize()),
  m_max_byte_count(psize_to_byte_count(m_max_psize))
{
}

irs::arm::st_flash_t::~st_flash_t()
{
}

void irs::arm::st_flash_t::read(irs_u8* ap_pos, irs_u8 *ap_buf,
  size_type a_buf_size)
{
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(reinterpret_cast<irs_u32>(ap_pos)));
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(
    reinterpret_cast<irs_u32>(ap_pos + a_buf_size)));
  memcpyex(ap_buf, ap_pos, a_buf_size);
  m_status = irs_st_ready;
}

void irs::arm::st_flash_t::erase_page(size_type a_page_index)
{
  m_page_index = a_page_index;
  m_process = process_erase;
  m_status = irs_st_busy;
}

void irs::arm::st_flash_t::write(irs_u8* ap_pos,
  const irs_u8 *ap_buf, size_type a_buf_size)
{
  mp_pos = ap_pos;
  mp_buf = ap_buf;
  mp_end = ap_buf + a_buf_size;
  m_process = process_write;
  m_status = irs_st_busy;
}

irs_u8* irs::arm::st_flash_t::page_begin(size_type a_page_index)
{
  IRS_LIB_ASSERT(a_page_index < sector_count);
  static const size_type sector_sizes[] = {
    0x08000000,
    0x08004000,
    0x08008000,
    0x0800C000,
    0x08010000,
    0x08020000,
    0x08040000,
    0x08060000,
    0x08080000,
    0x080A0000,
    0x081C0000,
    0x081E0000
  };
  IRS_LIB_ASSERT(sector_count == IRS_ARRAYSIZE(sector_sizes));
  return reinterpret_cast<irs_u8*>(sector_sizes[a_page_index]);
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_size(size_type a_page_index) const
{
  IRS_LIB_ASSERT(a_page_index < sector_count);
  static const size_type sector_sizes[] = {
    1024*16,
    1024*16,
    1024*16,
    1024*16,
    1024*64,
    1024*128,
    1024*128,
    1024*128,
    1024*128,
    1024*128,
    1024*128,
    1024*128
  };
  IRS_LIB_ASSERT(sector_count == IRS_ARRAYSIZE(sector_sizes));
  return sector_sizes[a_page_index];
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_count() const
{
  return sector_count;
}

irs_status_t irs::arm::st_flash_t::status() const
{
  return m_status;
}

void irs::arm::st_flash_t::tick()
{
  const FLASH_Status flash_status = FLASH_GetStatus();
  if (flash_status != FLASH_BUSY) {
    switch (m_process) {
      case process_erase: {
        FLASH_Unlock();
        FLASH_CR_bit.PSIZE = psize();
        FLASH_CR_bit.SER = 1;
        FLASH_CR_bit.SNB = m_page_index;
        FLASH_CR_bit.STRT = 1;
        m_process = process_wait_for_erase_operation;
      } break;
      case process_wait_for_erase_operation: {
        FLASH_CR_bit.SER = 0;
        FLASH_CR_bit.SNB = 0;
        process_end(flash_status);
      } break;
      case process_write: {
        FLASH_Unlock();
        FLASH_CR_bit.PSIZE = psize();
        FLASH_CR_bit.PG = 1;
        m_max_psize = psize();
        m_max_byte_count = psize_to_byte_count(m_max_psize);
        FLASH_CR_bit.PSIZE = m_max_psize;
        m_process = process_wait_for_write_operation;
      } break;
      case process_wait_for_write_operation: {
        if (mp_buf < mp_end) {
          write_tick(&mp_pos, &mp_buf, mp_end);
        } else {
          mp_pos = IRS_NULL;
          mp_buf = IRS_NULL;
          mp_end = IRS_NULL;
          FLASH_CR_bit.PG = 0;
          process_end(flash_status);
        }
      } break;
      case process_wait_command: {
        // Ожидаем команд
      } break;
    }
  }
}

void irs::arm::st_flash_t::process_end(FLASH_Status a_flash_status)
{
  FLASH_Lock();
  m_status = convert_status(a_flash_status);
  m_process = process_wait_command;
}

void irs::arm::st_flash_t::write_tick(
   irs_u8** ap_pos,  const irs_u8** ap_begin, const irs_u8* ap_end)
{
  const size_type buf_size = ap_end - *ap_begin;
  size_type size = m_max_byte_count;
  if (buf_size < m_max_byte_count) {
    size = buf_size;
    size_type write_byte_count = m_max_byte_count;
    while (size < write_byte_count) {
      write_byte_count >>= 1;
    }
    size = write_byte_count;
    FLASH_CR_bit.PSIZE = byte_count_to_psize(size);
  }
  switch (size) {
    case 1: {
      *reinterpret_cast<irs_u8*>(*ap_pos) =
        *reinterpret_cast<const irs_u8*>(*ap_begin);
    } break;
    case 2: {
      *reinterpret_cast<irs_u16*>(*ap_pos) =
        *reinterpret_cast<const irs_u16*>(*ap_begin);
    } break;
    case 4: {
      *reinterpret_cast<irs_u32*>(*ap_pos) =
        *reinterpret_cast<const irs_u32*>(*ap_begin);
    } break;
    case 8: {
      *reinterpret_cast<irs_u64*>(*ap_pos) =
        *reinterpret_cast<const irs_u64*>(*ap_begin);
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимое значение");
    }
  }
  *ap_pos += size;
  *ap_begin += size;
}

irs_status_t irs::arm::st_flash_t::convert_status(FLASH_Status a_flash_status)
{
  irs_status_t status = irs_st_ready;
  if (a_flash_status == FLASH_BUSY) {
    status = irs_st_busy;
  } else if (a_flash_status == FLASH_COMPLETE) {
    status = irs_st_ready;
  } else {
    status = irs_st_error;
  }
  return status;
}

irs_u32 irs::arm::st_flash_t::psize()
{
  irs_u32 psize = 0;
  const irs_u8 range = voltage_range();
  if (range == VoltageRange_1) {
    psize = 0; // byte
  } else if (range == VoltageRange_2) {
    psize = 1; // half word
  } else if (range == VoltageRange_3) {
    psize = 2; // word
  } else {
    psize = 3; // double word
  }
  return psize;
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::psize_to_byte_count(irs_u32 a_psize)
{
  IRS_LIB_ASSERT(a_psize <= 3);
  return (1 << a_psize);
}

irs_u32 irs::arm::st_flash_t::byte_count_to_psize(size_type a_bype_count)
{
  IRS_LIB_ASSERT((a_bype_count == 1) || (a_bype_count == 2) ||
    (a_bype_count == 4) || (a_bype_count == 8));
  size_type byte_count = a_bype_count;
  irs_u32 psize = 0;
  while (byte_count > 1) {
    byte_count >>= 1;
    psize++;
  }
  return psize;
}

irs_u8 irs::arm::st_flash_t::voltage_range()
{
  const double flash_voltage = irs::cpu_traits_t::flash_voltage();
  irs_u8 voltage_range = VoltageRange_1;
  if(flash_voltage <= 2.1) {
    voltage_range = VoltageRange_1;
  } else if((flash_voltage > 2.1) && (flash_voltage) < 2.7) {
    voltage_range = VoltageRange_2;
  } else if((flash_voltage > 2.7) && (flash_voltage <= 3.6)) {
    voltage_range = VoltageRange_3;
  } else {
    voltage_range = VoltageRange_4;
  }
  return voltage_range;
}
