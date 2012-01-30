// Дата: 27.12.2011
// Дата создания: 25.04.2011

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsmem.h>
#include <irsalg.h>
#include <irserror.h>

#include <irsfinal.h>

irs::eeprom_at25_t::eeprom_at25_t(spi_t& a_spi, gpio_pin_t& a_cs_pin,
  eeprom_type_t a_eeprom_type):
  m_spi(a_spi),
  m_cs_pin(a_cs_pin),
  m_page_size(0),
  m_page_count(0),
  m_status(st_check_ready_prepare),
  m_target_status(st_write_protect_check),
  mp_read_user_buf(IRS_NULL),
  mp_write_user_buf(IRS_NULL),
  m_num_of_iterations(0),
  m_current_iteration(0),
  m_modulo_size(0),
  m_page_addr(0)
{
  m_cs_pin.set();
  switch (a_eeprom_type) {
    case at25010: m_page_size = 8;  m_page_count = 128; break;
    case at25020: m_page_size = 8;  m_page_count = 256; break;
    case at25040: m_page_size = 8;  m_page_count = 512; break;
    case at25128: m_page_size = 64; m_page_count = 256; break;
    case at25256: m_page_size = 64; m_page_count = 512; break;
    default: m_page_size = 64; m_page_count = 256;  //  at25128
  }
  m_num_of_iterations = m_page_size / m_spi_size;
  m_modulo_size = m_page_size % m_spi_size;
  memsetex(mp_read_buf, m_spi_size);
  memsetex(mp_write_buf, m_spi_size);
}

void irs::eeprom_at25_t::prepare_spi()
{
  m_spi.set_bitrate(500000);
  m_spi.set_order(irs::spi_t::MSB);
  m_spi.set_polarity(irs::spi_t::RISING_EDGE);
  m_spi.set_phase(irs::spi_t::LEAD_EDGE);
  m_spi.lock();
  m_cs_pin.clear();
}

void irs::eeprom_at25_t::clear_spi()
{
  m_cs_pin.set();
  m_spi.unlock();
}

void irs::eeprom_at25_t::read_status_register()
{
  prepare_spi();
  memsetex(mp_write_buf, m_RDSR_size);
  mp_write_buf[m_command_pos] = m_RDSR;
  m_spi.read_write(mp_read_buf, mp_write_buf, m_RDSR_size);
}

void irs::eeprom_at25_t::write_protect_disable()
{
  prepare_spi();
  mp_write_buf[m_command_pos] = m_WRSR;
  mp_write_buf[m_WRSR_data_pos] = 0;
  m_spi.write(mp_write_buf, m_WRSR_size);
}

void irs::eeprom_at25_t::write_enable()
{
  prepare_spi();
  mp_write_buf[m_command_pos] = m_WREN;
  m_spi.write(mp_write_buf, m_WREN_size);
}
bool irs::eeprom_at25_t::status_register_ready()
{
  return !(mp_read_buf[m_RDSR_answer_pos] & (1 << m_RDY));
}

bool irs::eeprom_at25_t::status_register_write_protect()
{
  irs_u8 wp_mask = (1 << m_BP0) | (1 << m_BP1) | (1 << m_WPEN);
  return mp_read_buf[m_RDSR_answer_pos] & wp_mask;
}

void irs::eeprom_at25_t::transaction_initiate(const irs_u8 a_command)
{
  prepare_spi();
  mp_write_buf[m_command_pos] = a_command;
  mp_write_buf[m_addr_lo_pos] = IRS_LOBYTE(m_page_addr);
  mp_write_buf[m_addr_hi_pos] = IRS_HIBYTE(m_page_addr);
  m_spi.write(mp_write_buf, m_initiate_size);
}

irs::eeprom_at25_t::~eeprom_at25_t()
{
}

void irs::eeprom_at25_t::read_page(irs_u8 *ap_buf, irs_uarc a_index)
{
  if (m_status == st_free && ap_buf != IRS_NULL && a_index < m_page_count) {
    m_status = st_check_ready_prepare;
    m_target_status = st_read_prepare;
    mp_read_user_buf = ap_buf;
    m_page_addr = a_index * m_page_size;
    m_current_iteration = 0;
  }
}

void irs::eeprom_at25_t::write_page(const irs_u8 *ap_buf, irs_uarc a_index)
{
  if (m_status == st_free && ap_buf != IRS_NULL && a_index < m_page_count) {
    m_status = st_check_ready_prepare;
    m_target_status = st_write_enable;
    mp_write_user_buf = ap_buf;
    m_page_addr = a_index * m_page_size;
    m_current_iteration = 1;
  }
}

irs::page_mem_t::size_type irs::eeprom_at25_t::page_size() const
{
  return m_page_size;
}

irs_uarc irs::eeprom_at25_t::page_count() const
{
  return m_page_count;
}

irs_status_t irs::eeprom_at25_t::status() const
{
  switch (m_status) {
    case st_free: return irs_st_ready;
    case st_error: return irs_st_error;
    default: return irs_st_busy;
  }
}

void irs::eeprom_at25_t::tick()
{
  m_spi.tick();
  switch (m_status) {
    //  Проверка готовности
    case st_check_ready_prepare: {
      if ((m_spi.get_status() == irs::spi_t::FREE) && !m_spi.get_lock()) {
        read_status_register();
        m_status = st_check_ready;
      }
      break;
    }
    case st_check_ready: {
      if (m_spi.get_status() == irs::spi_t::FREE) {
        clear_spi();
        if (status_register_ready()) {
          m_status = m_target_status;
        } else {
          m_status = st_check_ready_prepare;
        }
      }
      break;
    }
    //  Проверка защиты от записи
    case st_write_protect_check: {
      if (status_register_write_protect()) {
        m_status = st_write_protect_disable_wait;
      } else {
        m_status = st_free;
      }
      break;
    }
    case st_write_protect_disable_wait: {
      if ((m_spi.get_status() == irs::spi_t::FREE) && !m_spi.get_lock()) {
        write_enable();
        m_status = st_write_protect_disable;
      }
      break;
    }
    case st_write_protect_disable: {
      if (m_spi.get_status() == irs::spi_t::FREE) {
        clear_spi();
        write_protect_disable();
        m_status = st_write_protect_disable_finish;
      }
      break;
    }
    case st_write_protect_disable_finish: {
      if (m_spi.get_status() == irs::spi_t::FREE) {
        clear_spi();
        m_status = st_free;
      }
      break;
    }
    //  Чтение
    case st_read_prepare: {
      //  Посылка команды на чтение и начального адреса
      transaction_initiate(m_READ);
      m_status = st_read_initiate;
      break;
    }
    case st_read_initiate: {
      //  Команда драйверу spi на считывание первых m_spi_size байт
      if (m_spi.get_status() == irs::spi_t::FREE) {
        if (m_num_of_iterations == 0) {
          m_spi.read(mp_read_buf, m_modulo_size);
          m_status = st_read_modulo;
        } else {
          m_spi.read(mp_read_buf, m_spi_size);
          m_status = st_read;
        }
      }
    }
    case st_read: {
      //  Чтение данных по m_spi_size байт
      if (m_spi.get_status() == irs::spi_t::FREE) {
        if (m_current_iteration < m_num_of_iterations) {
          irs_uarc target_index = m_current_iteration * m_spi_size;
          memcpy(mp_read_user_buf + target_index, mp_read_buf, m_spi_size);
          m_current_iteration++;
          m_spi.read(mp_read_buf, m_spi_size);
        } else {
          if (m_modulo_size == 0) {
            m_status = st_complete;
          } else {
            m_status = st_read_modulo;
            m_spi.read(mp_read_buf, m_modulo_size);
          }
        }
      }
      break;
    }
    case st_read_modulo: {
      //  Чтение остатка (если необходимо)
      if (m_spi.get_status() == irs::spi_t::FREE) {
        irs_uarc target_index = m_current_iteration * m_spi_size;
        memcpy(mp_read_user_buf + target_index, mp_read_buf, m_modulo_size);
        m_status = st_complete;
      }
      break;
    }
    //  Запись
    case st_write_enable: {
      //  Посылка команды разрешения записи
      write_enable();
      m_status = st_write_prepare;
      break;
    }
    case st_write_prepare: {
      //  Посылка команды на запись и начального адреса
      if (m_spi.get_status() == irs::spi_t::FREE) {
        clear_spi();
        for (irs_u8 i = 10; i; i--);
        transaction_initiate(m_WRITE);
        m_status = st_write_initiate;
      }
      break;
    }
    case st_write_initiate: {
      //  Команда драйверу spi на запись первых m_spi_size байт
      if (m_spi.get_status() == irs::spi_t::FREE) {
        if (m_num_of_iterations == 0) {
          memcpy(mp_write_buf, mp_write_user_buf, m_modulo_size);
          m_spi.write(mp_write_buf, m_modulo_size);
          m_status = st_write_modulo;
        } else {
          memcpy(mp_write_buf, mp_write_user_buf, m_spi_size);
          m_spi.write(mp_write_buf, m_spi_size);
          m_status = st_write;
        }
      }
      break;
    }
    case st_write: {
      //  Запись данных по m_spi_size байт
      if (m_spi.get_status() == irs::spi_t::FREE) {
        if (m_current_iteration < m_num_of_iterations) {
          irs_uarc target_index = m_current_iteration * m_spi_size;
          memcpy(mp_write_buf, mp_write_user_buf + target_index, m_spi_size);
          m_current_iteration++;
          m_spi.write(mp_write_buf, m_spi_size);
        } else {
          if (m_modulo_size == 0) {
            m_status = st_complete;
          } else {
            irs_uarc target_index = m_current_iteration * m_spi_size;
            memcpy(mp_write_buf, mp_write_user_buf + target_index,
              m_modulo_size);
            m_spi.write(mp_write_buf, m_modulo_size);
            m_status = st_write_modulo;
          }
        }
      }
      break;
    }
    case st_write_modulo: {
      //  Запись остатка (если необходимо)
      if (m_spi.get_status() == irs::spi_t::FREE) {
        m_status = st_complete;
      }
      break;
    }
    case st_complete: {
      //  Завершение операции
      clear_spi();
      m_status = st_free;
      break;
    }
    default: {}
  }
}

//------------------------------------------------------------------------------
//
irs::mem_cluster_t::mem_cluster_t(page_mem_t& a_page_mem,
  size_t a_cluster_size):
  m_page_mem(a_page_mem),
  m_status(st_free),
  m_target_status(m_status),
  m_pages_per_half_cluster(a_cluster_size / m_page_mem.page_size()),
  m_cluster_size(m_page_mem.page_size() * m_pages_per_half_cluster),
  m_data_size(m_cluster_size - m_crc_size),
  m_clusters_count((m_page_mem.page_count() / m_pages_per_half_cluster) / 2),
  m_error(false),
  m_page_index(0),
  m_cluster_index(0),
  m_cluster_data_index(0),
  m_cluster_data(m_cluster_size * 2), //  2 полукластера
  m_cluster_data_32(&m_cluster_data),
  mp_read_buf(IRS_NULL)
{
}

irs::mem_cluster_t::~mem_cluster_t()
{
}

void irs::mem_cluster_t::read_cluster(irs_u8 *ap_buf, irs_uarc a_cluster_index)
{
  IRS_LIB_ASSERT (!((status() != irs_st_busy) && (a_cluster_index < m_clusters_count) 
      && ap_buf))
  if ((status() != irs_st_busy) && (a_cluster_index < m_clusters_count) 
      && ap_buf) 
  {
    mp_read_buf = ap_buf;
    m_cluster_index = a_cluster_index;
    m_status = st_read_begin;
  }
}

void irs::mem_cluster_t::write_cluster(const irs_u8 *ap_buf,
  irs_uarc a_cluster_index)
{
  IRS_LIB_ASSERT (!((status() != irs_st_busy) && 
    (a_cluster_index < m_clusters_count) && ap_buf));
  if ((status() != irs_st_busy) && (a_cluster_index < m_clusters_count) && ap_buf) 
  {
    m_cluster_index = a_cluster_index;
    c_array_view_t<const irs_u8> user_buf(ap_buf, m_data_size);
    mem_copy(user_buf, 0, m_cluster_data, 0, m_data_size);
    m_status = st_write_begin;
  }
  
}

bool irs::mem_cluster_t::error() const
{
  return m_error;
}

irs_status_t irs::mem_cluster_t::status() const
{
  switch (m_status) {
    case st_free: return irs_st_ready;
    case st_error: return irs_st_error;
    default: return irs_st_busy;
  }
}

void irs::mem_cluster_t::tick()
{
  m_page_mem.tick();
  switch (m_status) {
    case st_read_begin: {
      m_page_index = 2 * m_cluster_index * m_pages_per_half_cluster;
      m_cluster_data_index = 0;
      m_status = st_read_process;
      break;
    }
    case st_read_process: {
      if (m_page_mem.status() == irs_st_ready) {
        if (m_cluster_data_index < 2 * m_cluster_size) {
          m_page_mem.read_page(&m_cluster_data[m_cluster_data_index],
            m_page_index);
          m_page_index++;
          m_cluster_data_index += m_page_mem.page_size();
        } else {
          m_status = st_read_check_crc;
        }
      }
      break;
    }
    case st_read_check_crc: {
      irs_u8* p_data_1_ptr = m_cluster_data.data();
      irs_u8* p_data_2_ptr = m_cluster_data.data() + m_cluster_size;
      irs_u32 crc_1 = crc32_table(p_data_1_ptr, m_data_size);
      irs_u32 crc_2 = crc32_table(p_data_2_ptr, m_data_size);
      irs_u32 ee_crc_1 = m_cluster_data_32[m_data_size / m_crc_size];
      irs_u32 ee_crc_2
        = m_cluster_data_32[(2 * m_cluster_size - 1) / m_crc_size];

      c_array_view_t<irs_u8> user_buf(mp_read_buf, m_data_size);
      if (crc_1 == ee_crc_1) {
        mem_copy(m_cluster_data, 0, user_buf, 0, m_data_size);
        if (crc_2 == ee_crc_2) {
          m_status = st_free;
        } else {
          //  1 -> 2
          m_page_index = (2 * m_cluster_index + 1) * m_pages_per_half_cluster;
          m_cluster_data_index = 0;
          m_status = st_write_process;
          m_target_status = st_free;
        }
      } else {
        if (crc_2 == ee_crc_2) {
          //  2 -> 1
          mem_copy(m_cluster_data, m_cluster_size, user_buf, 0, m_data_size);
          mem_copy(m_cluster_data, 0, m_cluster_data, m_cluster_size,
            m_cluster_size);
          m_page_index = 2 * m_cluster_index * m_pages_per_half_cluster;
          m_cluster_data_index = 0;
          m_status = st_write_process;
          m_target_status = st_free;
        } else {
          //  error
          m_status = st_error;
        }
      }
      break;
    }
    case st_write_begin: {
      m_page_index = 2 * m_cluster_index * m_pages_per_half_cluster;
      m_cluster_data_index = 0;
      irs_uarc crc = crc32_table(m_cluster_data.data(), m_data_size);
      m_cluster_data_32[m_data_size / m_crc_size] = crc;
      m_status = st_write_process;
      m_target_status = st_write_begin_2_half;
      break;
    }
    case st_write_process: {
      if (m_page_mem.status() == irs_st_ready) {
        if (m_cluster_data_index < m_cluster_size) {
          m_page_mem.write_page(&m_cluster_data[m_cluster_data_index],
            m_page_index);
          m_page_index++;
          m_cluster_data_index += m_page_mem.page_size();
        } else {
          m_status = m_target_status;
        }
      }
      break;
    }
    case st_write_begin_2_half: {
      m_page_index = (2 * m_cluster_index + 1) * m_pages_per_half_cluster;
      m_cluster_data_index = 0;
      m_status = st_write_process;
      m_target_status = st_free;
      break;
    }
    default: {}
  }
}

//------------------------------------------------------------------------------

irs::mem_data_t::mem_data_t(page_mem_t& a_page_mem, size_type a_cluster_size,
  size_type /*a_start_index*/, size_type a_size):
  m_cluster(a_page_mem, a_cluster_size),
  //m_start_index(a_start_index),
  m_status(st_free),
  m_size(a_size)
{
}

irs::mem_data_t::~mem_data_t()
{
}

void irs::mem_data_t::read(irs_u8* ap_buf, irs_uarc a_index,
  irs_uarc /*a_size*/)
{
  if (status() != irs_st_busy && a_index < m_size && ap_buf) {
  }
}

void irs::mem_data_t::write(const irs_u8* /*ap_buf*/, irs_uarc /*a_index*/,
  irs_uarc /*a_size*/)
{
}

irs_status_t irs::mem_data_t::status()
{
  switch (m_status) {
    case st_free: return irs_st_ready;
    case st_error: return irs_st_error;
    default: return irs_st_busy;
  }
}

irs::mem_data_t::size_type irs::mem_data_t::size()
{
  return m_size;
}

void irs::mem_data_t::tick()
{
}
