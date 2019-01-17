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
#elif defined(IRS_STM32_F2_F4_F7)
void irs::arm::flash_t::flash_erase(irs_u32 /*a_index*/)
{
}
#else
  //#error Тип контроллера не определён
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
#elif defined(IRS_STM32_F2_F4_F7)
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

#ifdef IRS_STM32_F2_F4_F7

std::size_t irs::arm::st_flash_page_index(const irs_u8* ap_pos)
{
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(reinterpret_cast<irs_u32>(ap_pos)));
  const std::size_t last_page_index = st_flash_page_count() - 1;
  const irs_u8* end =
    st_flash_page_begin(last_page_index) + st_flash_page_size(last_page_index);
  if ((ap_pos < st_flash_page_begin(0)) || (ap_pos >= end)) {
    return std::numeric_limits<std::size_t>::max();
  }
  for (int i = st_flash_page_count() - 1; i > 0; i--) {
    if (ap_pos >= st_flash_page_begin(i)) {
      return i;
    }
  }
  return std::numeric_limits<std::size_t>::max();
}

std::size_t irs::arm::st_flash_size_of_diapason_pages(
  std::size_t a_first_page_index, std::size_t a_last_page_index)
{
  std::size_t size = 0;
  std::size_t current_index = a_first_page_index;
  while (current_index <= a_last_page_index) {
    size += st_flash_page_size(current_index);
    current_index++;
  }
  return size;
}

#ifdef USE_STDPERIPH_DRIVER
#ifdef IRS_STM32F_2_AND_4

irs_u8* irs::arm::st_flash_page_begin(std::size_t a_page_index)
{
  IRS_LIB_ASSERT(a_page_index < st_flash_page_count());
  static const std::size_t sector_sizes[] = {
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
  IRS_LIB_ASSERT(st_flash_page_count() == IRS_ARRAYSIZE(sector_sizes));
  return reinterpret_cast<irs_u8*>(sector_sizes[a_page_index]);
}

std::size_t irs::arm::st_flash_page_size(std::size_t a_page_index)
{
  IRS_LIB_ASSERT(a_page_index < st_flash_page_count());
  static const std::size_t sector_sizes[] = {
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
  IRS_LIB_ASSERT(st_flash_page_count() == IRS_ARRAYSIZE(sector_sizes));
  return sector_sizes[a_page_index];
}

std::size_t irs::arm::st_flash_page_count()
{
  return 12;
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
  if (a_buf_size == 0) {
    m_status = irs_st_ready;
    return;
  }

  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(reinterpret_cast<irs_u32>(ap_pos)));
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(
    reinterpret_cast<irs_u32>(ap_pos + a_buf_size - 1)));
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
  if (a_buf_size == 0) {
    m_status = irs_st_ready;
    return;
  }

  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(reinterpret_cast<irs_u32>(ap_pos)));
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(
      reinterpret_cast<irs_u32>(ap_pos + a_buf_size - 1)));
  mp_pos = ap_pos;
  mp_buf = ap_buf;
  mp_end = ap_buf + a_buf_size;
  m_process = process_write;
  m_status = irs_st_busy;
}

irs_u8* irs::arm::st_flash_t::page_begin(size_type a_page_index)
{
  return st_flash_page_begin(a_page_index);
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_index(const irs_u8* ap_pos) const
{
  return st_flash_page_index(ap_pos);
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_size(size_type a_page_index) const
{
  return st_flash_page_size(a_page_index);
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_count() const
{
  return st_flash_page_count();
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
  }

  // Нельзя пересекать границу блока 16 байт
  // (см. описание ошибки PGAERR: Programming alignment error)
  // Дополнительно выравниваем на размер записи, иначе не работает
  size_type pos_front = reinterpret_cast<size_type>(*ap_pos);
  while (pos_front%size != 0) {
    size >>= 1;
  }

  FLASH_CR_bit.PSIZE = byte_count_to_psize(size);

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

#endif // IRS_STM32F_2_AND_4
#endif // USE_STDPERIPH_DRIVER

#ifdef USE_HAL_DRIVER
#ifdef IRS_STM32F7xx

irs_u8* irs::arm::st_flash_page_begin(std::size_t a_page_index)
{
  IRS_LIB_ASSERT(a_page_index < st_flash_page_count());
  static const std::size_t sector_sizes[] = {
    0x08000000,
    0x08008000,
    0x08010000,
    0x08018000,
    0x08020000,
    0x08040000,
    0x08080000,
    0x080C0000
  };
  IRS_LIB_ASSERT(st_flash_page_count() == IRS_ARRAYSIZE(sector_sizes));
  return reinterpret_cast<irs_u8*>(sector_sizes[a_page_index]);
}

std::size_t irs::arm::st_flash_page_size(std::size_t a_page_index)
{
  IRS_LIB_ASSERT(a_page_index < st_flash_page_count());
  static const std::size_t sector_sizes[] = {
    1024*32,
    1024*32,
    1024*32,
    1024*32,
    1024*128,
    1024*256,
    1024*256,
    1024*256
  };
  IRS_LIB_ASSERT(st_flash_page_count() == IRS_ARRAYSIZE(sector_sizes));
  return sector_sizes[a_page_index];
}

std::size_t irs::arm::st_flash_page_count()
{
  return 8;
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
  if (a_buf_size == 0) {
    m_status = irs_st_ready;
    return;
  }

  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(reinterpret_cast<irs_u32>(ap_pos)));
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(
    reinterpret_cast<irs_u32>(ap_pos + a_buf_size - 1)));
  memcpyex(ap_buf, ap_pos, a_buf_size);
  m_status = irs_st_ready;
}

void irs::arm::st_flash_t::erase_page(size_type a_page_index)
{
  #pragma diag_suppress=Pe186
  IRS_LIB_ASSERT((a_page_index >= FLASH_SECTOR_0) &&
    (a_page_index <= FLASH_SECTOR_7));
  #pragma diag_default=Pe186

  m_page_index = a_page_index;
  m_process = process_erase;
  m_status = irs_st_busy;
}

void irs::arm::st_flash_t::write(irs_u8* ap_pos,
  const irs_u8 *ap_buf, size_type a_buf_size)
{
  if (a_buf_size == 0) {
    m_status = irs_st_ready;
    return;
  }

  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(reinterpret_cast<irs_u32>(ap_pos)));
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(
      reinterpret_cast<irs_u32>(ap_pos + a_buf_size - 1)));
  mp_pos = ap_pos;
  mp_buf = ap_buf;
  mp_end = ap_buf + a_buf_size;
  m_process = process_write;
  m_status = irs_st_busy;
}

irs_u8* irs::arm::st_flash_t::page_begin(size_type a_page_index)
{
  return st_flash_page_begin(a_page_index);
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_index(const irs_u8* ap_pos) const
{
  return st_flash_page_index(ap_pos);
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_size(size_type a_page_index) const
{
  return st_flash_page_size(a_page_index);
}

irs::arm::st_flash_t::size_type
irs::arm::st_flash_t::page_count() const
{
  return st_flash_page_count();
}

irs_status_t irs::arm::st_flash_t::status() const
{
  return m_status;
}

void irs::arm::st_flash_t::tick()
{
  const irs_status_t flash_status = get_flash_status();

  if ((flash_status == irs_st_error) &&
      (m_process != process_wait_command)) {
    process_end(flash_status);
    m_process = process_wait_command;
  }

  if (flash_status == irs_st_ready) {
    switch (m_process) {
      case process_erase: {
        HAL_FLASH_Unlock();
        FLASH_Erase_Sector(m_page_index, voltage_range());
        m_process = process_wait_for_erase_operation;
      } break;
      case process_wait_for_erase_operation: {
        CLEAR_BIT(FLASH->CR, (FLASH_CR_SER | FLASH_CR_SNB));
        process_end(flash_status);
      } break;
      case process_write: {
        HAL_FLASH_Unlock();
        m_max_psize = psize();
        m_max_byte_count = psize_to_byte_count(m_max_psize);
        byte_count_to_psize_reg(m_max_byte_count);
        FLASH->CR |= FLASH_CR_PG;
        m_process = process_wait_for_write_operation;
      } break;
      case process_wait_for_write_operation: {
        if (mp_buf < mp_end) {
          write_tick(&mp_pos, &mp_buf, mp_end);
        } else {
          mp_pos = IRS_NULL;
          mp_buf = IRS_NULL;
          mp_end = IRS_NULL;
          FLASH->CR &= (~FLASH_CR_PG);
          process_end(flash_status);
        }
      } break;
      case process_wait_command: {
        // Ожидаем команд
      } break;
    }
  }
}

irs_status_t irs::arm::st_flash_t::get_flash_status() const
{
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != RESET) {
    return irs_st_busy;
  } else if (__HAL_FLASH_GET_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | \
      FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_ERSERR )) != RESET) {

    return irs_st_error;
  }
  return irs_st_ready;
}

void irs::arm::st_flash_t::process_end(irs_status_t a_flash_status)
{
  m_status = a_flash_status;
  clear_status_flags();
  HAL_FLASH_Lock();
  m_process = process_wait_command;
}

void irs::arm::st_flash_t::clear_status_flags()
{
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |\
    FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|\
    FLASH_FLAG_ERSERR);
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
  }

  // Нельзя пересекать границу блока 16 байт
  // (см. описание ошибки PGAERR: Programming alignment error)
  // Дополнительно выравниваем на размер записи, иначе не работает
  size_type pos_front = reinterpret_cast<size_type>(*ap_pos);
  while (pos_front%size != 0) {
    size >>= 1;
  }


  byte_count_to_psize_reg(size);

  switch (size) {
    case 1: {
      *reinterpret_cast<__IO irs_u8*>(*ap_pos) =
        *reinterpret_cast<const irs_u8*>(*ap_begin);
    } break;
    case 2: {
      *reinterpret_cast<__IO irs_u16*>(*ap_pos) =
        *reinterpret_cast<const irs_u16*>(*ap_begin);
    } break;
    case 4: {
      *reinterpret_cast<__IO irs_u32*>(*ap_pos) =
        *reinterpret_cast<const irs_u32*>(*ap_begin);
    } break;
    case 8: {
      *reinterpret_cast<__IO irs_u64*>(*ap_pos) =
        *reinterpret_cast<const irs_u64*>(*ap_begin);
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Недопустимое значение");
    }
  }
  __DSB();
  *ap_pos += size;
  *ap_begin += size;
}

irs_u32 irs::arm::st_flash_t::psize()
{
  irs_u32 psize = 0;
  const irs_u8 range = voltage_range();
  if (range == FLASH_VOLTAGE_RANGE_1) {
    psize = 0; // byte
  } else if (range == FLASH_VOLTAGE_RANGE_2) {
    psize = 1; // half word
  } else if (range == FLASH_VOLTAGE_RANGE_3) {
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

void irs::arm::st_flash_t::byte_count_to_psize_reg(size_type a_bype_count)
{
  FLASH->CR &= CR_PSIZE_MASK;
  switch (a_bype_count) {
    case 1: {
      FLASH->CR |= FLASH_PSIZE_BYTE;
    } break;
    case 2: {
      FLASH->CR |= FLASH_PSIZE_HALF_WORD;
    } break;
    case 4: {
      FLASH->CR |= FLASH_PSIZE_WORD;
    } break;
    case 8: {
      FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Указано недопустимое значение количества байт");
    }
  }
  //__DSB();
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
  irs_u8 voltage_range = FLASH_VOLTAGE_RANGE_1;
  if(flash_voltage <= 2.1) {
    voltage_range = FLASH_VOLTAGE_RANGE_1;
  } else if((flash_voltage > 2.1) && (flash_voltage) < 2.7) {
    voltage_range = FLASH_VOLTAGE_RANGE_2;
  } else if((flash_voltage > 2.7) && (flash_voltage <= 3.6)) {
    voltage_range = FLASH_VOLTAGE_RANGE_3;
  } else {
    voltage_range = FLASH_VOLTAGE_RANGE_4;
  }
  return voltage_range;
}

#endif // IRS_STM32F7xx
#endif // USE_HAL_DRIVER

// class st_flash_files_t
irs::arm::st_flash_files_t::st_flash_files_t(
  various_page_mem_t* ap_various_page_mem
):
  m_files(),
  mp_various_page_mem(ap_various_page_mem),
  m_various_page_mem_file_array()
{
}

/*void irs::arm::st_flash_files_t::add_file(string_type a_name,
  size_type a_first_page_index,
  size_type a_last_page_index, mode_io_t a_mode)
{
  IRS_LIB_ASSERT(a_first_page_index <= a_last_page_index);
  IRS_LIB_ASSERT(a_last_page_index < st_flash_page_count());
  if (m_files.find(a_name) != m_files.end()) {
    IRS_LIB_ASSERT_MSG("Указанный файл уже есть в списке");
    return;
  }
  bool diapason_valid = true;
  if (a_first_page_index > a_last_page_index) {
    diapason_valid = false;
  }
  files_map_type::const_iterator it = m_files.begin();
  while (it != m_files.end()) {
    if (!((a_last_page_index < it->second.first_page_index) ||
      (a_first_page_index > it->second.last_page_index))) {
      diapason_valid = false;
      break;
    }
    ++it;
  }
  if (diapason_valid) {
    file_t file;
    file.first_page_index = a_first_page_index;
    file.last_page_index = a_last_page_index;
    file.mode = a_mode;
    m_files.insert(std::make_pair(a_name, file));
  } else {
    IRS_LIB_ASSERT_MSG("Указан недопустимый диапазон");
  }
}*/

void irs::arm::st_flash_files_t::add_file(
  const string_type& a_name, irs_u8* ap_begin,
  size_type a_current_size, size_type a_max_size,
  bool a_save_load_size_enabled)
{
  add_file_helper(a_name, ap_begin, a_current_size, a_max_size,
    mode_read_write, a_save_load_size_enabled);
}

void irs::arm::st_flash_files_t::add_file_write_only(
  const string_type& a_name,
  irs_u8* ap_begin, size_type a_max_size, bool a_save_size_enabled)
{
  add_file_helper(a_name, ap_begin, 0, a_max_size, mode_write_only,
    a_save_size_enabled);
}

void irs::arm::st_flash_files_t::add_file_read_only(
  const string_type& a_name,
  irs_u8* ap_begin, size_type a_size, bool a_load_size_enabled)
{
  add_file_helper(a_name, ap_begin, a_size, a_size, mode_read_only,
    a_load_size_enabled);
}

void irs::arm::st_flash_files_t::add_file_helper(
  const string_type& a_name,
  irs_u8* ap_begin, size_type a_current_size, size_type a_max_size,
  mode_io_t a_mode, bool a_save_load_saze_enabled)
{
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(reinterpret_cast<irs_u32>(ap_begin)));
  IRS_LIB_ASSERT(IS_FLASH_ADDRESS(
      reinterpret_cast<irs_u32>(ap_begin + (a_max_size - 1))));
  if (m_files.find(a_name) != m_files.end()) {
    IRS_LIB_ASSERT_MSG("Указанный файл уже есть в списке");
    return;
  }
  file_t file;
  file.pos = ap_begin;
  if (a_save_load_saze_enabled) {
    file.size = 0;
  } else {
    file.size = a_current_size;
  }
  file.max_size = a_max_size;
  file.save_load_size_enabled = a_save_load_saze_enabled;
  file.first_page_index = st_flash_page_index(ap_begin);
  file.last_page_index = st_flash_page_index(ap_begin + (a_max_size - 1));
  IRS_LIB_ASSERT(file.first_page_index <= file.last_page_index);
  file.mode = a_mode;

  bool diapason_valid = true;
  files_map_type::const_iterator it = m_files.begin();
  while (diapason_valid && (it != m_files.end())) {
    if ((a_mode == mode_read_only) && (it->second.mode == mode_read_only)) {
      const irs_u8* new_file_begin = file.pos;
      const irs_u8* new_file_end = file.pos + file.size;
      const irs_u8* file_begin = it->second.pos;
      const irs_u8* file_end = it->second.pos + it->second.size;
      diapason_valid = (new_file_end <= file_begin) ||
        (new_file_begin >= file_end);
    } else {
      diapason_valid = ((file.last_page_index < it->second.first_page_index) ||
        (file.first_page_index > it->second.last_page_index));
    }
    ++it;
  }
  if (diapason_valid) {
    if (file.save_load_size_enabled &&
      ((a_mode == mode_read_only) || (a_mode == mode_read_write))) {
      //irs_u8* begin = st_flash_page_begin(file.first_page_index);
      file.size = *reinterpret_cast<file_size_type*>(file.pos);
      if (file.size > file.max_size) {
        file.size = 0;
      }
    }
    m_files.insert(std::make_pair(a_name, file));
  } else {
    IRS_LIB_ASSERT_MSG("Указаный диапазон страниц уже занят");
  }
}

irs::hfftp::file_read_only_t* irs::arm::st_flash_files_t::open_for_read(
  const string_type& a_file_name,
  irs::hfftp::file_error_t* ap_error_code)
{
  files_map_type::iterator it = get_file_for_open(a_file_name, ap_error_code);
  if (it == m_files.end()) {
    return IRS_NULL;
  }
  if ((it->second.mode != mode_read_only) &&
    (it->second.mode != mode_read_write)) {
    *ap_error_code = irs::hfftp::file_error_access_violation;
    return IRS_NULL;
  }
  //irs_u8* begin = st_flash_page_begin(it->second.first_page_index);
  irs_u8* begin = it->second.pos;
  if (it->second.save_load_size_enabled) {
    begin += sizeof(file_size_type);
  }
  //const size_type size = get_size_of_diapason_pages(it->second.first_page_index,
    //it->second.last_page_index);
  it->second.handle.reset(new irs::hfftp::memory_file_read_only_t(
    begin, it->second.size));
  *ap_error_code = irs::hfftp::file_error_no_error;
  return static_cast<irs::hfftp::file_read_only_t*>(it->second.handle.get());
}

irs::hfftp::file_write_only_t* irs::arm::st_flash_files_t::open_for_write(
  const string_type& a_file_name,
  irs::hfftp::file_error_t* ap_error_code)
{
  if (!mp_various_page_mem) {
    *ap_error_code = irs::hfftp::file_error_access_violation;
    return IRS_NULL;
  }
  files_map_type::iterator it = get_file_for_open(a_file_name, ap_error_code);
  if (it == m_files.end()) {
    return IRS_NULL;
  }
  if ((it->second.mode != mode_write_only) &&
    (it->second.mode != mode_read_write)) {
    *ap_error_code = irs::hfftp::file_error_access_violation;
    return IRS_NULL;
  }
  irs::hfftp::various_page_mem_file_write_only_t* file =
    new irs::hfftp::various_page_mem_file_write_only_t(
    mp_various_page_mem,
    it->second.pos,// first_page_index,
    it->second.max_size,// last_page_index,
    it->second.save_load_size_enabled);
  it->second.handle.reset(file);
  m_various_page_mem_file_array.push_back(file);
  return file;
}

irs::arm::st_flash_files_t::files_map_type::iterator
irs::arm::st_flash_files_t::get_file_for_open(const string_type& a_file_name,
  irs::hfftp::file_error_t* ap_error_code)
{
  files_map_type::iterator it = m_files.find(a_file_name);
  if (it == m_files.end()) {
    *ap_error_code = irs::hfftp::file_error_file_not_found;
  } else if (!it->second.handle.is_empty()) {
    *ap_error_code = irs::hfftp::file_error_access_violation;
    it = m_files.end();
  } else {
    *ap_error_code = irs::hfftp::file_error_no_error;
  }
  return it;
}

void irs::arm::st_flash_files_t::close(irs::hfftp::file_base_t* ap_file)
{
  files_map_type::iterator it = find_file_iterator(ap_file);
  if (it != m_files.end()) {
    irs::hfftp::various_page_mem_file_write_only_t*
      various_page_mem_write_only =
      dynamic_cast<irs::hfftp::various_page_mem_file_write_only_t*>(ap_file);
    if (various_page_mem_write_only) {
      if ((various_page_mem_write_only->status() == irs_st_busy) ||
        it->second.save_load_size_enabled) {
        m_files_marked_as_closed.push_back(various_page_mem_write_only);
        return;
      }
    }
    close_file(it);
    /*for (size_type i = 0; i < m_various_page_mem_file_array.size(); i++) {
      if (m_various_page_mem_file_array[i] == ap_file) {
        m_various_page_mem_file_array.erase(
          m_various_page_mem_file_array.begin() + i);
      }
    }
    it->second.size = it->second.handle->size();
    it->second.handle.reset();*/
  } else {
    IRS_LIB_ASSERT_MSG("Указанный файл не был открыт этим экземпляром.");
  }
}

irs::arm::st_flash_files_t::files_map_type::iterator
irs::arm::st_flash_files_t::find_file_iterator(
  irs::hfftp::file_base_t* ap_file)
{
  files_map_type::iterator it = m_files.begin();
  while (it != m_files.end()) {
    if (it->second.handle.get() == ap_file) {
      return it;
    }
    ++it;
  }
  return m_files.end();
}

void irs::arm::st_flash_files_t::close_file(files_map_type::iterator a_iterator)
{
  if (a_iterator->second.handle.is_empty()) {
    return;
  }

  for (size_type i = 0; i < m_various_page_mem_file_array.size(); i++) {
    if (m_various_page_mem_file_array[i] == a_iterator->second.handle.get()) {
      m_various_page_mem_file_array.erase(
        m_various_page_mem_file_array.begin() + i);
      break;
    }
  }
  a_iterator->second.size = a_iterator->second.handle->size();
  a_iterator->second.handle.reset();
}

irs::arm::st_flash_files_t::file_status_t
irs::arm::st_flash_files_t::get_file_status(
  const string_type& a_name) const
{
  const files_map_type::const_iterator it = m_files.find(a_name);
  if (it == m_files.end()) {
    throw std::logic_error("Файл не найден");
  }
  if (it->second.handle.is_empty()) {
    return file_status_closed;
  } else if (
    dynamic_cast<irs::hfftp::memory_file_read_only_t*>(it->second.handle.get())) {
    return file_status_reading;
  } else {
    return file_status_writing;
  }
}

irs::arm::st_flash_files_t::size_type
irs::arm::st_flash_files_t::get_file_size(
  const string_type& a_name) const
{
  const files_map_type::const_iterator it = m_files.find(a_name);
  if (it == m_files.end()) {
    throw std::logic_error("Файл не найден");
  }
  return it->second.size;
}

irs_u8* irs::arm::st_flash_files_t::get_file_data_begin(const string_type& a_name)
{
  const files_map_type::const_iterator it = m_files.find(a_name);
  if (it == m_files.end()) {
    throw std::logic_error("Файл не найден");
  }
  irs_u8* begin = st_flash_page_begin(it->second.first_page_index);
  if (it->second.save_load_size_enabled) {
    begin += sizeof(file_size_type);
  }
  return begin;
}

void irs::arm::st_flash_files_t::tick()
{
  for (size_type i = 0; i < m_various_page_mem_file_array.size(); i++) {
    m_various_page_mem_file_array[i]->tick();
  }
  std::vector<hfftp::various_page_mem_file_write_only_t*>::iterator it =
    m_files_marked_as_closed.begin();
  while (it != m_files_marked_as_closed.end()) {
    if ((*it)->status() != irs_st_busy) {
      if (((*it)->status() == irs_st_ready) &&
        (*it)->save_size_enabled() &&
        !(*it)->size_saved()) {
        (*it)->save_size();
      } else {
        close_file(find_file_iterator((*it)));
        m_files_marked_as_closed.erase(it);
        break;
      }
    }
    ++it;
  }
}

#endif  // IRS_STM32_F2_F4_F7
