// Дата: 26.04.2011
// Дата создания: 25.04.2011

#include <irsdefs.h>

#include <irsmem.h>
#include <irsalg.h>

#include <irsfinal.h>

irs::eeprom_at25128a_t::eeprom_at25128a_t(
  spi_t* ap_spi,
  gpio_pin_t* ap_cs_pin,
  irs_u32 a_ee_start_index,
  irs_u32 a_ee_size
):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_ee_start_index(a_ee_start_index),
  m_ee_size(a_ee_size),
  m_op_status(op_search_data_operation),
  m_ee_status(complete),
  m_need_write(false),
  m_crc_error(false),
  m_write_size(0),
  m_crc_write_begin(false),
  m_need_writes(m_ee_size),
  m_start_block(0),
  m_search_index(0),
  mp_target_buf(m_ee_size),
  m_mode(mode_general),
  m_check_eeprom_status_mode(check_status_send_command),
  m_write_index_cur(0),
  m_crc_need_calc(false)
{
  if (mp_spi && mp_cs_pin)
  {
    memsetex(mp_read_buf, m_spi_size);
    memsetex(mp_send_buf, m_spi_size);
    fill_n(m_need_writes.begin(), m_ee_size, 0);
    mp_cs_pin->set();
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
    read_on_start();
    irs_u32 crc_new = calc_new_crc();
    irs_u32 crc_cur = read_crc_eeprom();
    if (crc_new != crc_cur) {
      irs_u32 crc_old = calc_old_crc();
      if (crc_old != crc_cur) {
        m_crc_error = true;
      }
    }
  }
}

irs::eeprom_at25128a_t::~eeprom_at25128a_t()
{
  return;
}

irs_uarc irs::eeprom_at25128a_t::size()
{
  return m_ee_size;
}

irs_bool irs::eeprom_at25128a_t::connected()
{
  return (mp_spi && mp_cs_pin);
}

void irs::eeprom_at25128a_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_ee_size) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_ee_size) {
    size = irs_u8(m_ee_size - a_index);
  }
  memcpy((void*)ap_buf, (void*)(mp_target_buf.data() + a_index), size);
}

void irs::eeprom_at25128a_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_ee_size) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_ee_size) {
    size = irs_u8(m_ee_size - a_index);
  }
  memcpy((void*)(mp_target_buf.data() + a_index), (void*)ap_buf, size);
  fill_n(m_need_writes.begin() + a_index, size, 1);
}

irs_bool irs::eeprom_at25128a_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_ee_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_target_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::eeprom_at25128a_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_ee_size) return;
  if (a_bit_index > 7) return;
  mp_target_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  m_need_writes[a_index] = 1;
}

void irs::eeprom_at25128a_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_ee_size) return;
  if (a_bit_index > 7) return;
  mp_target_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  m_need_writes[a_index] = 1;
}

void irs::eeprom_at25128a_t::tick()
{
  mp_spi->tick();
  
  switch(m_mode)
  {
    case mode_get_status:
    {
      switch(m_check_eeprom_status_mode)
      {
        case check_status_send_command:
        {
          if (!mp_spi->get_lock()&&mp_spi->get_status() == irs::spi_t::FREE) {
            mp_spi->lock();
            mp_spi->set_order(irs::spi_t::MSB);
            mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
            mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
            mp_cs_pin->clear();
            mp_send_buf[0] = m_RDSR;
            mp_send_buf[1] = 0;
            mp_send_buf[2] = 0;
            mp_spi->read_write(mp_read_buf, mp_send_buf, m_spi_size);
            m_check_eeprom_status_mode = check_status_get_response;
          }
        } break;
        case check_status_get_response:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            mp_cs_pin->set();
            mp_spi->unlock();
            if (!(mp_read_buf[1] & (1 << m_RDY))) {
              m_ee_status = complete;
            } else {
              m_ee_status = busy;
            }
            m_check_eeprom_status_mode = check_status_send_command;
            m_mode = mode_general;
          }
        } break;
      }
    } break;
    case mode_general:
    {
      switch (m_op_status)
      {
        case op_search_data_operation:
        {
          bool catch_block = false;
          for(; (m_search_index < m_ee_size) ;) {
            if ((m_need_writes[m_search_index]) && !catch_block) {
              m_start_block = m_search_index;
              catch_block = true;
              m_need_write = true;
            }
            if (catch_block && !m_need_writes[m_search_index]) {
              m_write_size = m_search_index - m_start_block;
              size_t start_block =
                (m_ee_start_index + m_crc_size + m_start_block)%m_PAGE_SIZE;
              if ((start_block + m_write_size) > m_PAGE_SIZE)
              {
                m_write_size = m_PAGE_SIZE - start_block;
                m_search_index = m_start_block + m_write_size;
              }
              break;
            }
            m_search_index++;
            if (catch_block && (m_search_index == m_ee_size)) {
              m_write_size = m_search_index - m_start_block;
              size_t start_block =
                (m_ee_start_index + m_crc_size + m_start_block)%m_PAGE_SIZE;
              if ((start_block + m_write_size) > m_PAGE_SIZE)
              {
                m_write_size = m_PAGE_SIZE - start_block;
                m_search_index = m_start_block + m_write_size;
              } else {
                m_search_index = 0;
              }
              break;
            }
            if (!catch_block && (m_search_index == m_ee_size)) {
              m_search_index = 0;
              break;
            }
          }
          if (m_need_write) {
            m_op_status = op_spi_write_begin;
            m_mode = mode_get_status;
          }
        } break;
        case op_spi_write_begin:
        {
          if (!mp_spi->get_lock() && mp_spi->get_status() == irs::spi_t::FREE) {
            if (m_ee_status == complete) {
              if (m_need_write) {
                mp_spi->set_order(irs::spi_t::MSB);
                mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
                mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
                mp_spi->lock();
                mp_cs_pin->clear();
                mp_send_buf[0] = m_WREN;
                mp_send_buf[1] = 0;
                mp_send_buf[2] = 0;
                mp_spi->write(mp_send_buf, m_spi_size);
                m_need_write = false;
                m_op_status = op_spi_wren;
              } else {
                m_op_status = op_search_data_operation;
              }
            } else {
              m_mode = mode_get_status;
            }
          }
        } break;
        case op_spi_wren:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            mp_cs_pin->set();
            if (!m_crc_need_calc) {
              m_op_status = op_spi_write_continue;
            } else {
              m_crc_need_calc = false;
              m_op_status = op_write_crc32_begin;
            }
          }
        } break;
        case op_spi_write_continue:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            mp_cs_pin->clear();
            mp_send_buf[0] = m_WRITE;
            mp_send_buf[1] = IRS_CONST_HIBYTE(static_cast<irs_u16>(
              m_ee_start_index + m_start_block + m_crc_size));
            mp_send_buf[2] = IRS_CONST_LOBYTE(static_cast<irs_u16>(
              m_ee_start_index + m_start_block + m_crc_size));
            mp_spi->write(mp_send_buf, m_spi_size);
            m_write_index_cur = 0;
            m_op_status = op_spi_write_end;
          }
        } break;
        case op_spi_write_end:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            if (m_write_index_cur < m_write_size) {
              if ((m_write_size - m_write_index_cur) >= m_spi_size) {
                mp_spi->write(mp_target_buf.data() + m_start_block +
                  m_write_index_cur, m_spi_size);
                m_write_index_cur += m_spi_size;
              } else {
                mp_spi->write(mp_target_buf.data() + m_start_block +
                  m_write_index_cur, m_write_size - m_write_index_cur);
                m_write_index_cur = m_write_size;
              }
            } else {
              mp_cs_pin->set();
              mp_spi->unlock();
              m_mode = mode_get_status;
              fill_n(m_need_writes.begin() + m_start_block, m_write_size, 0);
              m_need_write = true;
              m_crc_need_calc = true;
              m_op_status = op_spi_write_begin;
            }
          }
        } break;
        case op_write_crc32_begin:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            mp_cs_pin->clear();
            mp_send_buf[0] = m_WRITE;
            mp_send_buf[1] = IRS_CONST_HIBYTE(m_ee_start_index);
            mp_send_buf[2] = IRS_CONST_LOBYTE(m_ee_start_index);
            mp_spi->write(mp_send_buf, m_spi_size);
            m_crc_write_begin = true;
            m_op_status = op_write_crc32_continue;
          }
        } break;
        case op_write_crc32_continue:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            irs_u32 crc = calc_new_crc();
            if (m_crc_write_begin) {
              memcpyex(mp_send_buf, reinterpret_cast<irs_u8*>(&crc),
                m_spi_size);
              mp_spi->write(mp_send_buf, m_spi_size);
              m_crc_write_begin = false;
            } else {
              memcpyex(mp_send_buf,
                reinterpret_cast<irs_u8*>(&crc) + m_spi_size,
                m_crc_size - m_spi_size);
              mp_spi->write(mp_send_buf, m_crc_size - m_spi_size);
              m_op_status = op_write_crc32_end;
            }
          }
        } break;
        case op_write_crc32_end:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            mp_cs_pin->set();
            mp_spi->unlock();
            m_op_status = op_spi_reset;
            m_mode = mode_get_status;
          }
        } break;
        case op_spi_reset:
        {
          if (mp_spi->get_status() == irs::spi_t::FREE) {
            if (m_ee_status == complete) {
              m_op_status = op_search_data_operation;
            } else {
              m_mode = mode_get_status;
            }
          }
        } break;
      }
    } break;
  }
}

bool irs::eeprom_at25128a_t::error()
{
  return m_crc_error;
}

irs_u32 irs::eeprom_at25128a_t::calc_old_crc()
{
  return crc32(reinterpret_cast<irs_u32*>(mp_target_buf.data()), 0,
    m_ee_size/4);
}

irs_u32 irs::eeprom_at25128a_t::calc_new_crc()
{
  return crc32_table(mp_target_buf.data(), m_ee_size);
}

irs_u32 irs::eeprom_at25128a_t::read_crc_eeprom()
{
  for (; (mp_spi->get_status() != irs::spi_t::FREE); )
    mp_spi->tick();
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
  mp_spi->lock();
  mp_cs_pin->clear();
  mp_send_buf[0] = m_READ;
  mp_send_buf[1] = IRS_CONST_HIBYTE(m_ee_start_index);
  mp_send_buf[2] = IRS_CONST_LOBYTE(m_ee_start_index);
  mp_spi->read_write(mp_read_buf, mp_send_buf, m_spi_size);
  for (; (mp_spi->get_status() != irs::spi_t::FREE); )
    mp_spi->tick();
  irs_u32 crc = 0;
  if ((m_ee_start_index%m_PAGE_SIZE + m_crc_size) < m_PAGE_SIZE) {
    mp_spi->read(reinterpret_cast<irs_u8*>(&crc), m_spi_size);
    for (; (mp_spi->get_status() != irs::spi_t::FREE); )
      mp_spi->tick();
    mp_spi->read(reinterpret_cast<irs_u8*>(&crc) + m_spi_size,
      m_crc_size - m_spi_size);
    for (; (mp_spi->get_status() != irs::spi_t::FREE); )
      mp_spi->tick();
    mp_cs_pin->set();
    mp_spi->unlock();
    
    m_ee_status = busy;
    while(get_status() == busy) {}
  } else {
    size_t size = m_PAGE_SIZE - (m_ee_start_index%m_PAGE_SIZE);
    mp_spi->read(reinterpret_cast<irs_u8*>(&crc), size);
    for (; (mp_spi->get_status() != irs::spi_t::FREE); )
      mp_spi->tick();
    mp_cs_pin->set();
    mp_spi->unlock();
    m_ee_status = busy;
    while(get_status() == busy) {}
    
    mp_spi->set_order(irs::spi_t::MSB);
    mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
    mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
    mp_spi->lock();
    mp_cs_pin->clear();
    mp_send_buf[0] = m_READ;
    mp_send_buf[1] = IRS_CONST_HIBYTE(m_ee_start_index + size);
    mp_send_buf[2] = IRS_CONST_LOBYTE(m_ee_start_index + size);
    mp_spi->read_write(mp_read_buf, mp_send_buf, m_spi_size);
    for (; (mp_spi->get_status() != irs::spi_t::FREE); )
      mp_spi->tick();
    size = m_crc_size - size;
    mp_spi->read(reinterpret_cast<irs_u8*>(&crc), size);
    for (; (mp_spi->get_status() != irs::spi_t::FREE); )
      mp_spi->tick();
    mp_cs_pin->set();
    mp_spi->unlock();
    m_ee_status = busy;
    while(get_status() == busy) {}
  }
  return crc;
}

size_t irs::eeprom_at25128a_t::read_part(size_t a_index, size_t a_size)
{
  size_t data_index = a_index;
  while (a_index != (data_index + a_size)) {
    if ((a_size - (a_index - data_index)) >= m_spi_size) {
      mp_spi->read(mp_target_buf.data() +
        (a_index - (m_ee_start_index + m_crc_size)), m_spi_size);
      a_index += m_spi_size;
    } else {
      mp_spi->read(mp_target_buf.data() +
        (a_index - (m_ee_start_index + m_crc_size)),
        a_size - (a_index - data_index));
      a_index = data_index + a_size;
    }
    for (; (mp_spi->get_status() != irs::spi_t::FREE); )
      mp_spi->tick();
  }
  return a_index;
}

void irs::eeprom_at25128a_t::read_on_start()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
  mp_spi->lock();
  mp_cs_pin->clear();
  size_t data_index = m_ee_start_index + m_crc_size;
  size_t first_part_size = 0;
  size_t middle_part_cnt = 0;
  size_t last_part_size = 0;
  if ((data_index%m_PAGE_SIZE + m_ee_size) > m_PAGE_SIZE) {
    first_part_size = m_PAGE_SIZE - data_index%m_PAGE_SIZE;
    last_part_size = (data_index%m_PAGE_SIZE + m_ee_size)%m_PAGE_SIZE;
    middle_part_cnt = (data_index%m_PAGE_SIZE + m_ee_size -
      first_part_size - last_part_size)/m_PAGE_SIZE;
  } else {
    first_part_size = m_ee_size;
  }
  // Чтение first_part
  mp_send_buf[0] = m_READ;
  mp_send_buf[1] = IRS_CONST_HIBYTE(data_index);
  mp_send_buf[2] = IRS_CONST_LOBYTE(data_index);
  mp_spi->read_write(mp_read_buf, mp_send_buf, m_spi_size);
  for (; (mp_spi->get_status() != irs::spi_t::FREE); )
    mp_spi->tick();
  size_t read_index_cur = data_index;
  read_index_cur = read_part(read_index_cur, first_part_size);
  m_ee_status = busy;
  while(get_status() == busy) {}
  // Чтение middle_part
  for (size_t middle_part_idx = 0; middle_part_idx < middle_part_cnt;
    middle_part_idx++)
  {
    mp_spi->set_order(irs::spi_t::MSB);
    mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
    mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
    mp_spi->lock();
    mp_cs_pin->clear();
    data_index = read_index_cur;
    mp_send_buf[0] = m_READ;
    mp_send_buf[1] = IRS_CONST_HIBYTE(data_index);
    mp_send_buf[2] = IRS_CONST_LOBYTE(data_index);
    mp_spi->read_write(mp_read_buf, mp_send_buf, m_spi_size);
    for (; (mp_spi->get_status() != irs::spi_t::FREE); )
      mp_spi->tick();
    read_index_cur = read_part(read_index_cur, m_PAGE_SIZE);
    m_ee_status = busy;
    while(get_status() == busy) {}
  }
  // Чтение last_part
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
  mp_spi->lock();
  mp_cs_pin->clear();
  data_index = read_index_cur;
  mp_send_buf[0] = m_READ;
  mp_send_buf[1] = IRS_CONST_HIBYTE(data_index);
  mp_send_buf[2] = IRS_CONST_LOBYTE(data_index);
  mp_spi->read_write(mp_read_buf, mp_send_buf, m_spi_size);
  for (; (mp_spi->get_status() != irs::spi_t::FREE); )
    mp_spi->tick();
  read_index_cur = read_part(read_index_cur, last_part_size);
  mp_cs_pin->set();
  mp_spi->unlock();
  m_ee_status = busy;
  while(get_status() == busy) {}
}

irs::eeprom_at25128a_t::ee_status_t irs::eeprom_at25128a_t::get_status()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
  mp_spi->lock();
  mp_cs_pin->clear();
  mp_send_buf[0] = m_RDSR;
  mp_send_buf[1] = 0;
  mp_send_buf[2] = 0;
  mp_spi->read_write(mp_read_buf, mp_send_buf, m_spi_size);
  for (; (mp_spi->get_status() != irs::spi_t::FREE); )
    mp_spi->tick();
  mp_cs_pin->set();
  mp_spi->unlock();
  if (!(mp_read_buf[1] & (1 << m_RDY))) {
    m_ee_status = complete;
  } else {
    m_ee_status = busy;
  }
  return m_ee_status;
}

irs::eeprom_command_t::eeprom_command_t(
  spi_t* ap_spi,
  gpio_pin_t* ap_cs_pin,
  eeprom_type_t a_eeprom_type
):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  mp_cluster_data(ap_spi, ap_cs_pin, 0),
  m_ee_size(0),
  m_page_size(0),
  m_page_data_size(0),
  m_status(status_completed),
  mp_read_buf_cur(IRS_NULL),
  mp_write_buf_cur(IRS_NULL),
  m_page_index(0),
  m_op_size(0),
  m_op_index_cur(0),
  m_need_read(false),
  m_need_write(false),
  m_op_mode(mode_read_write_begin),
  mp_target_buf(0),
  m_first_part_size(0),
  m_middle_part_number(0),
  m_last_part_size(0)
{
  switch(a_eeprom_type)
  {
    case at25128a:
    {
      m_page_size = 64;
      m_ee_size = (16384 - (16384/m_page_size)*m_crc_size)/2;
    } break;
    case at25256a:
    {
      m_page_size = 64;
      m_ee_size = (32768 - (32768/m_page_size)*m_crc_size)/2;
    } break;
    default:
    {
      IRS_FATAL_ERROR("Неверно указан тип eeprom");
    };
  }
  m_page_data_size = m_page_size - m_crc_size;
  mp_cluster_data.resize(m_page_size);
  mp_target_buf.resize(m_page_size);
  if (mp_spi && mp_cs_pin)
  {
    mp_cs_pin->set();
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
  }
}

irs::eeprom_command_t::~eeprom_command_t()
{

}

void irs::eeprom_command_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (m_status != status_wait) {
    if ((a_index + a_size) > m_ee_size) return;
    m_status = status_wait;
    mp_read_buf_cur = ap_buf;
    m_op_index_cur = a_index;
    m_op_size = a_size;
    m_need_read = true;
  } else {
    IRS_LIB_ERROR(ec_standard, "irs::eeprom_command_t::read - "
      "Попытка чтения пока предыдущая операция чтения не завершена");
  }
}

void irs::eeprom_command_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (m_status != status_wait) {
    if ((a_index + a_size) > m_ee_size) return;
    m_status = status_wait;
    mp_write_buf_cur = ap_buf;
    m_op_index_cur = a_index;
    m_op_size = a_size;
    m_need_write = true;
  } else {
    IRS_LIB_ERROR(ec_standard, "irs::eeprom_command_t::write - "
      "Попытка записи пока предыдущая операция записи не завершена");
  }
}

irs::eeprom_command_t::status_t irs::eeprom_command_t::status()
{
  return m_status;
}

irs::eeprom_command_t::size_type irs::eeprom_command_t::size()
{
  return m_ee_size;
}

void irs::eeprom_command_t::tick()
{
  mp_spi->tick();
  
  switch(m_op_mode)
  {
    case mode_read_write_begin:
    {
      if (!mp_spi->get_lock() && mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_need_read || m_need_write) {
          if ((m_op_index_cur%m_page_data_size + m_op_size + m_crc_size) >
            m_page_size)
          {
            m_first_part_size = m_page_size - m_crc_size -
              m_op_index_cur%m_page_data_size;
            m_last_part_size = (m_op_size - m_first_part_size)%
              m_page_data_size;
            m_middle_part_number = (m_op_size - m_first_part_size -
              m_last_part_size)/m_page_data_size;
          } else {
            m_first_part_size = m_op_size;
          }
          m_page_index = (m_op_index_cur/m_page_data_size)*m_page_size*2;
          if (m_first_part_size) {
            m_op_mode = mode_first_part_size;
          } else if (m_middle_part_number) {
            m_op_mode = mode_middle_part_size;
          } else {
            m_op_mode = mode_last_part_size;
          }
        }
      }
    } break;
    case mode_first_part_size:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_need_write) {
          write_part(m_first_part_size);
        } else if (m_need_read) {
          read_part(m_first_part_size);
        }
        if (m_middle_part_number) {
          m_op_mode = mode_middle_part_size;
        } else if (m_last_part_size) {
          m_op_mode = mode_last_part_size;
        } else {
          m_op_mode = mode_spi_reset;
        }
      }
    } break;
    case mode_middle_part_size:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_need_write) {
          if (m_middle_part_number) {
            write_part(m_page_data_size);
            m_middle_part_number--;
          } else {
            if (m_last_part_size) {
              m_op_mode = mode_last_part_size;
            } else {
              m_op_mode = mode_spi_reset;
            }
          }
        } else if (m_need_read) {
          if (m_middle_part_number) {
            read_part(m_page_data_size);
            m_middle_part_number--;
          } else {
            if (m_last_part_size) {
              m_op_mode = mode_last_part_size;
            } else {
              m_op_mode = mode_spi_reset;
            }
          }
        }
      }
    } break;
    case mode_last_part_size:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_need_write) {
          write_part(m_last_part_size);
        } else if (m_need_read) {
          read_part(m_last_part_size);
        }
        m_op_mode = mode_spi_reset;
      }
    } break;
    case mode_spi_reset:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_status != status_error) {
          m_status = status_completed;
        }
        if (m_need_read) {
          m_need_read = false;
        } else if (m_need_write) {
          m_need_write = false;
        }
        m_first_part_size = 0;
        m_middle_part_number = 0;
        m_last_part_size = 0;
        m_op_mode = mode_read_write_begin;
      }
    } break;
  }
}

void irs::eeprom_command_t::write_part(size_type a_size)
{
  mp_cluster_data.read(mp_target_buf.data(), m_page_index);
  irs_u32 crc = 0;
  memcpyex(reinterpret_cast<irs_u8*>(&crc), mp_target_buf.data(), m_crc_size);
  if (crc == crc32_table(mp_target_buf.data() + m_crc_size,
    m_page_data_size))
  {
    memcpyex(mp_target_buf.data() + m_crc_size + 
      m_op_index_cur%m_page_data_size, mp_write_buf_cur,
      a_size);
    crc = crc32_table(mp_target_buf.data() + m_crc_size,
      m_page_data_size);
    memcpyex(mp_target_buf.data(), reinterpret_cast<irs_u8*>(&crc),
      m_crc_size);
    mp_cluster_data.write(mp_target_buf.data(), m_page_index);
    mp_cluster_data.write(mp_target_buf.data(),
      m_page_index + m_page_size);
  } else {
    mp_cluster_data.read(mp_target_buf.data(),
      m_page_index + m_page_size);
    memcpyex(reinterpret_cast<irs_u8*>(&crc), mp_target_buf.data(), m_crc_size);
    if (crc == crc32_table(mp_target_buf.data() + m_crc_size,
      m_page_data_size))
    {
      memcpyex(mp_target_buf.data() + m_crc_size +
        m_op_index_cur%m_page_data_size, mp_write_buf_cur,
        a_size);
      crc = crc32_table(mp_target_buf.data() + m_crc_size,
        m_page_data_size);
      memcpyex(mp_target_buf.data(), reinterpret_cast<irs_u8*>(&crc),
       m_crc_size);
      mp_cluster_data.write(mp_target_buf.data(), m_page_index);
      mp_cluster_data.write(mp_target_buf.data(),
        m_page_index + m_page_data_size);
    } else {
      m_status = status_error;
    }
  }
  mp_write_buf_cur += a_size;
  m_op_index_cur += a_size;
  m_page_index += m_page_size*2;
}

void irs::eeprom_command_t::read_part(size_type a_size)
{
  mp_cluster_data.read(mp_target_buf.data(), m_page_index);
  irs_u32 crc = 0;
  memcpyex(reinterpret_cast<irs_u8*>(&crc), mp_target_buf.data(), m_crc_size);
  if (crc == crc32_table(mp_target_buf.data() + m_crc_size,
    m_page_data_size))
  {
    memcpyex(mp_read_buf_cur, mp_target_buf.data() + m_crc_size + 
      m_op_index_cur%m_page_data_size, a_size);
  } else {
    mp_cluster_data.read(mp_target_buf.data(), m_page_index + m_page_size);
    memcpyex(reinterpret_cast<irs_u8*>(&crc), mp_target_buf.data(), m_crc_size);
    if (crc == crc32_table(mp_target_buf.data() + m_crc_size,
      m_page_data_size))
    {
      memcpyex(mp_read_buf_cur, mp_target_buf.data() + m_crc_size + 
        m_op_index_cur%m_page_data_size, a_size);
    } else {
      m_status = status_error;
    }
  }
  mp_read_buf_cur += a_size;
  m_op_index_cur += a_size;
  m_page_index += m_page_size*2;
}

//------------------------------------------------------------------------------

irs::eeprom_spi_t::eeprom_spi_t(
  eeprom_command_t* ap_eeprom_command,
  size_type a_size
):
  mp_eeprom_command(ap_eeprom_command),
  m_size((mp_eeprom_command->size() < a_size)?mp_eeprom_command->size():a_size),
  mp_target_buf(m_size),
  m_need_writes(m_size),
  m_need_write(false),
  m_write_size(0),
  m_search_index(0),
  m_start_block(0),
  m_error(false),
  m_mode(mode_search_write_data)
{
  mp_eeprom_command->read(mp_target_buf.data(), 0, m_size);
  for(; ((mp_eeprom_command->status() != eeprom_command_t::status_completed)&&
    (mp_eeprom_command->status() != eeprom_command_t::status_error)); )
  {
    mp_eeprom_command->tick();
  }
  if (mp_eeprom_command->status() == eeprom_command_t::status_error) {
    m_error = true;
  }
}

irs::eeprom_spi_t::~eeprom_spi_t()
{
}

irs_uarc irs::eeprom_spi_t::size()
{
  return static_cast<irs_uarc>(m_size);
}

irs_bool irs::eeprom_spi_t::connected()
{
  return true;
}

void irs::eeprom_spi_t::read(irs_u8* ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) {
    size = irs_u8(m_size - a_index);
  }
  memcpy((void*)ap_buf, (void*)(mp_target_buf.data() + a_index), size);
}

void irs::eeprom_spi_t::write(const irs_u8* ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) {
    size = irs_u8(m_size - a_index);
  }
  memcpy((void*)(mp_target_buf.data() + a_index), (void*)ap_buf, size);
  fill_n(m_need_writes.begin() + a_index, size, 1);
}

irs_bool irs::eeprom_spi_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_target_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::eeprom_spi_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_target_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  m_need_writes[a_index] = 1;
}

void irs::eeprom_spi_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_target_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  m_need_writes[a_index] = 1;
}

bool irs::eeprom_spi_t::error()
{
  return m_error;
}

void irs::eeprom_spi_t::tick()
{
  mp_eeprom_command->tick();
  
  switch(m_mode)
  {
    case mode_search_write_data:
    {
      bool catch_block = false;
      for(; (m_search_index < m_size) ;) {
        if ((m_need_writes[m_search_index]) && !catch_block) {
          m_start_block = m_search_index;
          catch_block = true;
          m_need_write = true;
        }
        if (catch_block && !m_need_writes[m_search_index]) {
          m_write_size = m_search_index - m_start_block;
          if (m_write_size > m_size)
          {
            m_write_size = m_size;
            m_search_index = m_start_block + m_write_size;
          }
          break;
        }
        m_search_index++;
        if (catch_block && (m_search_index == m_size)) {
          m_write_size = m_search_index - m_start_block;
          if (m_write_size > m_size)
          {
            m_write_size = m_size;
            m_search_index = m_start_block + m_write_size;
          } else {
            m_search_index = 0;
          }
          break;
        }
        if (!catch_block && (m_search_index == m_size)) {
          m_search_index = 0;
          break;
        }
      }
      if (m_need_write) {
        m_mode = mode_write_data;
      }
    } break;
    case mode_write_data:
    {
      if (mp_eeprom_command->status() == eeprom_command_t::status_completed)
      {
        mp_eeprom_command->write(mp_target_buf.data() + m_start_block,
          m_start_block, m_write_size);
        fill_n(m_need_writes.begin() + m_start_block, m_write_size, 0);
        m_mode = mode_search_write_data;
        m_need_write = false;
      }
      else if (mp_eeprom_command->status() == eeprom_command_t::status_error)
      {
        m_error = true;
        m_mode = mode_search_write_data;
      }
    }
  }
}
