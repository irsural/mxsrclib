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

irs::eeprom_at25_t::eeprom_at25_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
  eeprom_type_t a_eeprom_type):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
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
  mp_cs_pin->set();
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
  mp_spi->set_bitrate(500000);
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);//RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
  mp_spi->lock();
  mp_cs_pin->clear();
}

void irs::eeprom_at25_t::clear_spi()
{
  mp_cs_pin->set();
  mp_spi->unlock();
}

void irs::eeprom_at25_t::read_status_register()
{
  prepare_spi();
  memsetex(mp_write_buf, m_RDSR_size);
  mp_write_buf[m_command_pos] = m_RDSR;
  mp_spi->read_write(mp_read_buf, mp_write_buf, m_RDSR_size);
}

void irs::eeprom_at25_t::write_protect_disable()
{
  prepare_spi();
  mp_write_buf[m_command_pos] = m_WRSR;
  mp_write_buf[m_WRSR_data_pos] = (1 << m_WEN);
  mp_spi->write(mp_write_buf, m_WRSR_size);
}

void irs::eeprom_at25_t::write_enable()
{
  prepare_spi();
  mp_write_buf[m_command_pos] = m_WREN;
  mp_spi->write(mp_write_buf, m_WREN_size);
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
  mp_spi->write(mp_write_buf, m_initiate_size);
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
  mp_spi->tick();
  switch (m_status) {
    //  Проверка готовности
    case st_check_ready_prepare: {
      if ((mp_spi->get_status() == irs::spi_t::FREE) && !mp_spi->get_lock()) {
        read_status_register();
        m_status = st_check_ready;
      }
      break;
    }
    case st_check_ready: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
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
      if ((mp_spi->get_status() == irs::spi_t::FREE) && !mp_spi->get_lock()) {
        write_enable();
        m_status = st_write_protect_disable;
      }
      break;
    }
    case st_write_protect_disable: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        clear_spi();
        write_protect_disable();
        m_status = st_write_protect_disable_finish;
      }
      break;
    }
    case st_write_protect_disable_finish: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        clear_spi();
        m_status = st_free;
      }
      break;
    }
    //  Чтение
    case st_read_prepare: {
      //  Посылка команды на чтение и начального адреса
      if (mp_spi->get_status() == irs::spi_t::FREE && !mp_spi->get_lock()) {
        transaction_initiate(m_READ);
        m_status = st_read_initiate;
      }
      break;
    }
    case st_read_initiate: {
      //  Команда драйверу spi на считывание первых m_spi_size байт
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_num_of_iterations == 0) {
          mp_spi->read(mp_read_buf, m_modulo_size);
          m_status = st_read_modulo;
        } else {
          mp_spi->read(mp_read_buf, m_spi_size);
          m_status = st_read;
        }
      }
    } break;
    case st_read: {
      //  Чтение данных по m_spi_size байт
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_current_iteration < m_num_of_iterations) {
          irs_uarc target_index = m_current_iteration * m_spi_size;
          memcpy(mp_read_user_buf + target_index, mp_read_buf, m_spi_size);
          m_current_iteration++;
          mp_spi->read(mp_read_buf, m_spi_size);
        } else {
          if (m_modulo_size == 0) {
            m_status = st_complete;
          } else {
            m_status = st_read_modulo;
            mp_spi->read(mp_read_buf, m_modulo_size);
          }
        }
      }
      break;
    }
    case st_read_modulo: {
      //  Чтение остатка (если необходимо)
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        irs_uarc target_index = m_current_iteration * m_spi_size;
        memcpy(mp_read_user_buf + target_index, mp_read_buf, m_modulo_size);
        m_status = st_complete;
      }
      break;
    }
    //  Запись
    case st_write_enable: {
      //  Посылка команды разрешения записи
      if (mp_spi->get_status() == irs::spi_t::FREE && !mp_spi->get_lock()) {
        write_enable();
        m_status = st_write_prepare;
      }
      break;
    }
    case st_write_prepare: {
      //  Посылка команды на запись и начального адреса
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        clear_spi();
        for (irs_u8 i = 10; i; i--);
        transaction_initiate(m_WRITE);
        m_status = st_write_initiate;
      }
      break;
    }
    case st_write_initiate: {
      //  Команда драйверу spi на запись первых m_spi_size байт
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_num_of_iterations == 0) {
          memcpy(mp_write_buf, mp_write_user_buf, m_modulo_size);
          mp_spi->write(mp_write_buf, m_modulo_size);
          m_status = st_write_modulo;
        } else {
          memcpy(mp_write_buf, mp_write_user_buf, m_spi_size);
          mp_spi->write(mp_write_buf, m_spi_size);
          m_status = st_write;
        }
      }
      break;
    }
    case st_write: {
      //  Запись данных по m_spi_size байт
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        if (m_current_iteration < m_num_of_iterations) {
          irs_uarc target_index = m_current_iteration * m_spi_size;
          memcpy(mp_write_buf, mp_write_user_buf + target_index, m_spi_size);
          m_current_iteration++;
          mp_spi->write(mp_write_buf, m_spi_size);
        } else {
          if (m_modulo_size == 0) {
            m_status = st_complete;
          } else {
            irs_uarc target_index = m_current_iteration * m_spi_size;
            memcpy(mp_write_buf, mp_write_user_buf + target_index,
              m_modulo_size);
            mp_spi->write(mp_write_buf, m_modulo_size);
            m_status = st_write_modulo;
          }
        }
      }
      break;
    }
    case st_write_modulo: {
      //  Запись остатка (если необходимо)
      if (mp_spi->get_status() == irs::spi_t::FREE) {
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
irs::mem_cluster_t::mem_cluster_t(page_mem_t* ap_page_mem,
  size_t a_cluster_size):
  mp_page_mem(ap_page_mem),
  m_status(st_free),
  m_target_status(m_status),
  m_pages_per_half_cluster(a_cluster_size / mp_page_mem->page_size()),
  m_cluster_size(mp_page_mem->page_size() * m_pages_per_half_cluster),
  m_data_size(m_cluster_size - m_crc_size),
  m_clusters_count((mp_page_mem->page_count() / m_pages_per_half_cluster) / 2),
  m_error_status(false),
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

irs_uarc irs::mem_cluster_t::cluster_count() const
{
  return m_clusters_count;
}

size_t irs::mem_cluster_t::crc_size() const
{
  return m_crc_size;
}

void irs::mem_cluster_t::read_cluster(irs_u8 *ap_buf, irs_uarc a_cluster_index)
{
  IRS_LIB_ERROR_IF(!((status() != irs_st_busy) &&
    (a_cluster_index < m_clusters_count) && ap_buf), ec_standard, "")
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
  IRS_LIB_ERROR_IF(!((status() != irs_st_busy) &&
    (a_cluster_index < m_clusters_count) && ap_buf), ec_standard, "");
  if ((status() != irs_st_busy) && (a_cluster_index < m_clusters_count)
      && ap_buf)
  {
    m_cluster_index = a_cluster_index;
    c_array_view_t<const irs_u8> user_buf(ap_buf, m_data_size);
    mem_copy(user_buf, 0, m_cluster_data, 0, m_data_size);
    m_status = st_write_begin;
  }
}

bool irs::mem_cluster_t::error()
{
  bool result;
  result = m_error_status;
  m_error_status = false;
  return result;
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
  mp_page_mem->tick();
  switch (m_status) {
    case st_read_begin: {
      m_page_index = 2 * m_cluster_index * m_pages_per_half_cluster;
      m_cluster_data_index = 0;
      m_status = st_read_process;
      break;
    }
    case st_read_process: {
      if (mp_page_mem->status() == irs_st_ready) {
        if (m_cluster_data_index < 2 * m_cluster_size) {
          mp_page_mem->read_page(m_cluster_data.data()+m_cluster_data_index,
            m_page_index);
          m_page_index++;
          m_cluster_data_index += mp_page_mem->page_size();
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
          mem_copy(m_cluster_data, 0, m_cluster_data, m_cluster_size,
            m_cluster_size);
          #ifdef NOP
          for (size_t i = 0; i < m_cluster_size; i++) {
            if (m_cluster_data[i] != m_cluster_data[m_cluster_size+i]) {
              int k = 0;
              k++;
            }
          }
          #endif //NOP
          m_page_index = (2 * m_cluster_index + 1) * m_pages_per_half_cluster;
          m_cluster_data_index = 0;
          m_status = st_write_process;
          m_target_status = st_free;
        }
      } else {
        if (crc_2 == ee_crc_2) {
          //  2 -> 1
          mem_copy(m_cluster_data, m_cluster_size, user_buf, 0, m_data_size);
          mem_copy(m_cluster_data, m_cluster_size, m_cluster_data, 0,
            m_cluster_size);
          #ifdef NOP
          for (int i = 0; i < m_cluster_size; i++) {
            if (m_cluster_data[i] != m_cluster_data[m_cluster_size+i]) {
              int k = 0;
              k++;
            }
          }
          #endif //NOP
          m_page_index = 2 * m_cluster_index * m_pages_per_half_cluster;
          m_cluster_data_index = 0;
          m_status = st_write_process;
          m_target_status = st_free;
        } else {
          //  error
          IRS_LIB_DBG_MSG(CNT_TO_DBLTIME(counter_get()) << " Ошибка CRC");
          memsetex(m_cluster_data.data(), m_cluster_data.size());
          mem_copy(m_cluster_data, 0, user_buf, 0, m_data_size);
          m_status = st_write_begin;
          m_error_status = true;
          irs::mlog() << m_cluster_index << endl;
        }
      }
      break;
    }
    case st_write_begin: {
      m_page_index = 2 * m_cluster_index * m_pages_per_half_cluster;
      m_cluster_data_index = 0;
      irs_u32 crc = crc32_table(m_cluster_data.data(), m_data_size);
      m_cluster_data_32[m_data_size / m_crc_size] = crc;
      m_status = st_write_process;
      m_target_status = st_write_begin_2_half;
      break;
    }
    case st_write_process: {
      if (mp_page_mem->status() == irs_st_ready) {
        if (m_cluster_data_index < m_cluster_size) {
          mp_page_mem->write_page(m_cluster_data.data()+m_cluster_data_index,
            m_page_index);
          m_page_index++;
          m_cluster_data_index += mp_page_mem->page_size();
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

irs::mem_data_t::mem_data_t(page_mem_t* ap_page_mem, size_type a_cluster_size):
  m_cluster(ap_page_mem, a_cluster_size),
  m_status(st_free),
  m_cluster_size(a_cluster_size),
  m_cluster_data_size(a_cluster_size - m_cluster.crc_size()),
  m_data_count(0),
  m_data_buf(),
  m_buf(),
  m_start_index(0),
  m_end_index(0),
  m_data_size(0),
  m_cluster_start_index(0),
  m_cluster_next_index(0),
  m_cluster_end_index(0),
  m_cluster_curr_index(0),
  m_index_data_buf(0),
  m_end_cluste(false),
  mp_read_buf(NULL)
{
  m_data_count =
    (ap_page_mem->page_count()*static_cast<irs_uarc>(ap_page_mem->page_size()))
     /2;
  int crc_mem_size = m_cluster.cluster_count()*m_cluster.crc_size();
  m_data_count = m_data_count - crc_mem_size;
  m_buf.resize(m_cluster_data_size);
}

irs::mem_data_t::~mem_data_t()
{
}

bool irs::mem_data_t::error()
{
  return m_cluster.error();
}

void irs::mem_data_t::read(irs_u8* ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_ERROR_IF(!((status() != irs_st_busy) && ap_buf &&
    ((a_index + a_size) <= m_data_count)), ec_standard, "");
  if ((status() != irs_st_busy) && ap_buf &&
    ((a_index + a_size) <= m_data_count))
  {
    m_start_index = a_index;
    m_end_index = a_index + a_size - 1;
    m_data_size = a_size;
    mp_read_buf = ap_buf;
    m_status = st_read_begin;
    m_cluster_start_index = (m_start_index)/m_cluster_data_size;
    m_cluster_end_index = (m_end_index)/m_cluster_data_size;
    m_cluster_curr_index = m_cluster_start_index;
    m_index_data_buf = 0;
    m_end_cluste = false;
  }
}

void irs::mem_data_t::write(const irs_u8* ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_ERROR_IF(!((status() != irs_st_busy) && ap_buf &&
    ((a_index + a_size) <= m_data_count)), ec_standard, "");
  if ((status() != irs_st_busy) && ap_buf &&
    ((a_index + a_size) <= m_data_count))
  {
    m_start_index = a_index;
    m_end_index = a_index + a_size - 1;
    m_data_size = a_size;
    c_array_view_t<const irs_u8> user_buf(ap_buf, m_data_size);
    m_data_buf.resize(m_data_size);
    mem_copy(user_buf, 0, m_data_buf, 0, m_data_size);
    m_status = st_write_begin;
    m_cluster_start_index = (m_start_index)/m_cluster_data_size;
    m_cluster_end_index = (m_end_index)/m_cluster_data_size;
    m_cluster_curr_index = m_cluster_start_index;
    m_index_data_buf = 0;
    m_end_cluste = false;
  }
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
  return m_data_count;
}

void irs::mem_data_t::tick()
{
  m_cluster.tick();

  switch (m_status) {
    case st_read_begin: {
      if (m_cluster.status() != irs_st_busy) {
        if(m_cluster_curr_index > m_cluster_end_index) {
          m_status = st_free;
        } else if (m_cluster_curr_index == m_cluster_end_index) {
          m_status = st_read_process;
          m_end_cluste = true;
          m_cluster.read_cluster(m_buf.data(), m_cluster_curr_index);
        } else if (m_cluster_curr_index < m_cluster_end_index) {
          m_status = st_read_process;
          m_cluster.read_cluster(m_buf.data(), m_cluster_curr_index);
        }
      }
    } break;
    case st_read_process: {
      if (m_cluster.status() != irs_st_busy) {
        int start_index = m_start_index%m_cluster_data_size;
        int data_size = 0;
        if(m_end_cluste) {
          data_size = m_end_index + 1  - m_start_index;
        } else {
          data_size = m_cluster_data_size - start_index;
        }
        c_array_view_t<irs_u8> user_buf(mp_read_buf, m_data_size);
        mem_copy(m_buf, start_index, user_buf, m_index_data_buf, data_size);
        m_index_data_buf = m_index_data_buf + data_size;
        m_start_index = m_start_index + data_size;
        m_cluster_curr_index++;
        m_status = st_read_begin;
      }
    } break;
    case st_write_begin: {
      if (m_cluster.status() != irs_st_busy) {
        if(m_cluster_curr_index > m_cluster_end_index) {
          m_status = st_free;
        } else if (m_cluster_curr_index == m_cluster_end_index) {
          m_status = st_write_calculation;
          m_end_cluste = true;
          m_cluster.read_cluster(m_buf.data(), m_cluster_curr_index);
        } else if (m_cluster_curr_index < m_cluster_end_index) {
          m_status = st_write_calculation;
          m_cluster.read_cluster(m_buf.data(), m_cluster_curr_index);
        }
      }
    } break;
    case st_write_calculation: {
      if (m_cluster.status() != irs_st_busy) {
        int start_index = m_start_index%m_cluster_data_size;
        int data_size = 0;
        if(m_end_cluste) {
          data_size = m_end_index + 1  - m_start_index;
        } else {
          data_size = m_cluster_data_size - start_index;
        }
        mem_copy(m_data_buf, m_index_data_buf, m_buf, start_index, data_size);
        m_index_data_buf = m_index_data_buf + data_size;
        m_start_index = m_start_index + data_size;
        m_status = st_write_process;
      }
    } break;
    case st_write_process: {
      if (m_cluster.status() != irs_st_busy) {
        m_cluster.write_cluster(m_buf.data(), m_cluster_curr_index);
        m_cluster_curr_index++;
        m_status = st_write_begin;
      }
    } break;
    case st_error: {


    } break;
     case st_free: {


    } break;
    default: {}
  }
}

irs::mxdata_comm_t::mxdata_comm_t(irs::mem_data_t* ap_mem_data,
  irs_uarc a_index, irs_uarc a_size, bool a_init_now, counter_t a_init_timeout):
  mp_mem_data(ap_mem_data),
  m_data_buf(a_size),
  m_mem_data_start_index(a_index),
  m_bit_vector(a_size),
  m_data_changed(false),
  m_mode(mode_free),
  m_current_index(0),
  m_start_index(0),
  m_data_size(0),
  m_connected(false),
  m_timer(a_init_timeout),
  m_init_now(a_init_now)
{
  if (ap_mem_data) {
    connect(ap_mem_data);
  }
}

irs::mxdata_comm_t::~mxdata_comm_t()
{
}

irs_uarc irs::mxdata_comm_t::size()
{
  return m_data_buf.size();
}

irs_bool irs::mxdata_comm_t::connected()
{
  return m_connected;
}

void irs::mxdata_comm_t::read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  if (ap_buf && ((a_size + a_index) <= m_data_buf.size())) {
    c_array_view_t<irs_u8> read_buf(ap_buf, a_size);
    mem_copy(m_data_buf, a_index, read_buf, 0, a_size);
  }
}

void irs::mxdata_comm_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size)
{
  if (ap_buf && ((a_size + a_index) <= m_data_buf.size())) {
    c_array_view_t<const irs_u8> write_buf(ap_buf, a_size);
    mem_copy(write_buf, 0, m_data_buf, a_index, a_size);
    for (int i = a_index; (i - a_index) < a_size; i++) {
      m_bit_vector[i] = true;
      m_data_changed = true;
    }
  }
}

irs_bool irs::mxdata_comm_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if ((a_index < m_data_buf.size()) && (a_bit_index <= 7)) {
    return static_cast<irs_bool>((m_data_buf[a_index] >> a_bit_index) & 1);
  } else return irs_false;

}

void irs::mxdata_comm_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  write_bit(a_index, a_bit_index, 1);
}

void irs::mxdata_comm_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  write_bit(a_index, a_bit_index, 0);
}

void irs::mxdata_comm_t::write_bit(irs_uarc a_index, irs_uarc a_bit_index,
    irs_bool a_bit)
{
  if ((a_index < m_data_buf.size()) && (a_bit_index <= 7)) {
    if (a_bit) {
      m_data_buf[a_index] |= irs_u8(1 << a_bit_index);
    } else {
      m_data_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
    }
    m_bit_vector[a_index] = true;
    m_data_changed = true;
  }
}

void irs::mxdata_comm_t::tick()
{
  mp_mem_data->tick();

  switch (m_mode) {
    case mode_free: {
      if (m_data_changed) {
        if (m_current_index >= m_bit_vector.size()) {
          m_current_index = 0;
        }
        const bool start_index_zero = (m_current_index == 0);
        bool start = false;
        bool finish = false;
        for ( ; m_current_index < m_bit_vector.size(); m_current_index++) {
          if (m_bit_vector[m_current_index] == 1) {
            m_start_index = m_current_index;
            start = true;
            break;
          }
        }
        if (start_index_zero && (m_current_index == m_bit_vector.size())) {
          m_data_changed = false;
        }

        for ( ; m_current_index < m_bit_vector.size(); m_current_index++) {
          if (m_bit_vector[m_current_index] == 0) {
            m_data_size = m_current_index - m_start_index;
            finish = true;
            m_mode = mode_write;
            break;
          }
        }
        if ((!finish) && start) {
          m_data_size = m_current_index - m_start_index;
          m_mode = mode_write;
        }
      }
    } break;
    case mode_initialization: {
      if (mp_mem_data->status() != irs_st_busy) {
        IRS_LIB_ERROR_IF(!((m_mem_data_start_index + m_data_buf.size()) <=
          mp_mem_data->size()), ec_standard, "Превышен размер eeprom");
        mp_mem_data->read(m_data_buf.data(),
          m_mem_data_start_index, m_data_buf.size());
        m_mode = mode_initialization_wait;
      }
    } break;
    case mode_initialization_wait: {
      if (mp_mem_data->status() == irs_st_ready) {
        m_mode = mode_free;
        m_connected = true;
      }
    } break;
    case mode_write: {
      if (mp_mem_data->status() != irs_st_busy) {
        IRS_LIB_ERROR_IF(!check_index(m_data_buf, m_start_index, m_data_size),
          ec_standard,
          "Итераторы вне контейнера или begin > end.");
        #ifdef IRS_LIB_CHECK
        if (!check_index(m_data_buf, m_start_index, m_data_size)) {
          break;
        }
        #endif //IRS_LIB_CHECK
        mp_mem_data->write(m_data_buf.data() + m_start_index,
          m_mem_data_start_index + m_start_index, m_data_size);
        for (int i = m_start_index; (i - m_start_index) < m_data_size; i++) {
          m_bit_vector[i] = false;
        }
        m_mode = mode_write_wait;
      }
    } break;
    case mode_write_wait: {
      if (mp_mem_data->status() != irs_st_busy) {
        m_mode = mode_free;
      } /*else if (mp_mem_data->status() == irs_st_error) {
        for (int i = m_start_index; (i - m_start_index) < m_data_size; i++) {
          m_bit_vector[i] = true;
        }
      }*/
    } break;
    default: {}
  }
}

void irs::mxdata_comm_t::connect(irs::mem_data_t* ap_mem_data)
{
  mp_mem_data = ap_mem_data;
  m_mode = mode_initialization;
  if (m_init_now) {
    m_timer.start();
    while (!m_connected) {
      tick();
      if (m_timer.check()) {
        IRS_LIB_ERROR(ec_standard, "Истек таймаут инициализации eeprom!");
        break;
      }
    }
  }
}

irs::mem_data_t* irs::mxdata_comm_t::mem_data()
{
  return mp_mem_data;
}

irs::eeprom_at25128_data_t::eeprom_at25128_data_t(spi_t* ap_spi,
  gpio_pin_t* ap_cs_pin, irs_uarc a_size, bool a_init_now,
  irs_uarc a_index, size_type a_cluster_size, counter_t a_init_timeout
):
  mxdata_comm_t(IRS_NULL, a_index, a_size, a_init_now, a_init_timeout),
  m_page_mem(ap_spi, ap_cs_pin, at25128),
  m_mem_data(&m_page_mem, a_cluster_size)
{
  connect(&m_mem_data);
}

bool irs::eeprom_at25128_data_t::error()
{
  return m_mem_data.error();
}
