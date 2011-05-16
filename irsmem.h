// Дата: 26.04.2011
// Дата создания: 25.04.2011

#ifndef irsmemh
#define irsmemh

#include <irsdefs.h>

#include <irsspi.h>
#include <irsgpio.h>
#include <mxdata.h>


#include <irsfinal.h>

namespace irs {

class eeprom_at25128a_t : public mxdata_t
{
public:
  eeprom_at25128a_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin, 
    irs_u32 a_ee_start_index, irs_u32 a_ee_size);
  ~eeprom_at25128a_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
  bool error();
private:
  enum mode_t {
    mode_get_status,
    mode_general
  };
  enum check_status_mode_t {
    check_status_send_command,
    check_status_get_response
  };
  enum operation_status_t {
    op_search_data_operation,
    op_spi_write_begin,
    op_spi_wren,
    op_spi_write_continue,
    op_spi_write_end,
    op_write_crc32_begin,
    op_write_crc32_continue,
    op_write_crc32_end,
    op_spi_reset
  };
  enum ee_status_t {
    complete,
    busy
  };
  enum {
    m_spi_size = 3,
    
    m_crc_size = sizeof(irs_u32),
    
    m_PAGE_SIZE = 64,
    
    // INSTRUCTION CODES
    m_WREN = 0x06,
    m_WRDI = 0x04,
    m_RDSR = 0x05,
    m_WRSR = 0x01,
    m_READ = 0x03,
    m_WRITE = 0x02,
    
    // STATUS REGISTER BIT
    m_RDY = 0,
    m_WEN = 1,
    m_BP0 = 2,
    m_BP1 = 3,
    m_WPEN = 7
  };
  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
  size_t m_ee_start_index;
  size_t m_ee_size;
  operation_status_t m_op_status;
  ee_status_t m_ee_status;
  bool m_need_write;
  bool m_crc_error;
  size_t m_write_size;
  bool m_crc_write_begin;
  vector<bool> m_need_writes;
  size_t m_start_block;
  size_t m_search_index;
  irs::raw_data_t<irs_u8> mp_target_buf;
  irs_u8 mp_read_buf[m_spi_size];
  irs_u8 mp_send_buf[m_spi_size];
  mode_t m_mode;
  check_status_mode_t m_check_eeprom_status_mode;
  size_t m_write_index_cur;
  bool m_crc_need_calc;
  
  irs_u32 calc_old_crc();
  irs_u32 calc_new_crc();
  irs_u32 read_crc_eeprom();
  size_t read_part(size_t a_index, size_t a_size);
  void read_on_start();
  ee_status_t get_status();
  
}; // eeprom_at25128a_t

class eeprom_command_t: public comm_data_t
{
public:
  typedef comm_data_t::size_type size_type;
  typedef comm_data_t::eeprom_type_t eeprom_type_t;
  typedef comm_data_t::status_t status_t;
  
  eeprom_command_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
    eeprom_type_t a_eeprom_type);
  ~eeprom_command_t();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual status_t status();
  virtual size_type size();
  virtual void tick();
private:
  enum {
    m_spi_size = 3,
    m_crc_size = sizeof(irs_u32)
  };
  enum operation_mode_t {
    mode_read_write_begin,
    mode_first_part_size,
    mode_middle_part_size,
    mode_last_part_size,
    mode_spi_reset
  };
  class eeprom_cluster_t
  {
  public:
    typedef comm_data_t::size_type size_type;
    eeprom_cluster_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin, size_type a_size):
      mp_spi(ap_spi),
      mp_cs_pin(ap_cs_pin),
      m_size(a_size)
    {
      if (mp_spi && mp_cs_pin)
      {
        memsetex(mp_recv_buf, m_spi_size);
        memsetex(mp_send_buf, m_spi_size);
        mp_cs_pin->set();
        for (; (mp_spi->get_status() != irs::spi_t::FREE) &&
          (mp_spi->get_lock()); )
        {
          mp_spi->tick();
        }
        while(get_status() == busy) {}
      }
    }
    ~eeprom_cluster_t() {}
    void read(irs_u8 *ap_buf, irs_uarc a_index)
    {
      mp_spi->set_order(irs::spi_t::MSB);
      mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
      mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
      mp_spi->lock();
      mp_cs_pin->clear();
      mp_send_buf[0] = m_READ;
      mp_send_buf[1] = IRS_CONST_HIBYTE(static_cast<irs_u16>(a_index));
      mp_send_buf[2] = IRS_CONST_LOBYTE(static_cast<irs_u16>(a_index));
      mp_spi->read_write(mp_recv_buf, mp_send_buf, m_spi_size);
      for (; (mp_spi->get_status() != irs::spi_t::FREE); )
        mp_spi->tick();
      size_type data_index_cur = 0;
      while (data_index_cur != m_size) {
        if ((m_size - data_index_cur) >= m_spi_size) {
          mp_spi->read(ap_buf + data_index_cur, m_spi_size);
          data_index_cur += m_spi_size;
        } else {
          mp_spi->read(ap_buf + data_index_cur, m_size - data_index_cur);
          data_index_cur = m_size;
        }
        for (; (mp_spi->get_status() != irs::spi_t::FREE); )
          mp_spi->tick();
      }
      mp_cs_pin->set();
      mp_spi->unlock();
      while(get_status() == busy) {}
    }
    void write(const irs_u8 *ap_buf, irs_uarc a_index)
    {
      mp_spi->set_order(irs::spi_t::MSB);
      mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
      mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
      mp_spi->lock();
      mp_cs_pin->clear();
      mp_send_buf[0] = m_WREN;
      mp_send_buf[1] = 0;
      mp_send_buf[2] = 0;
      mp_spi->write(mp_send_buf, m_spi_size);
      for (; (mp_spi->get_status() != irs::spi_t::FREE); )
        mp_spi->tick();
      mp_cs_pin->set();
      //for (irs_u8 i = 10; i; i--);
      mp_cs_pin->clear();
      mp_send_buf[0] = m_WRITE;
      mp_send_buf[1] = IRS_CONST_HIBYTE(static_cast<irs_u16>(a_index));
      mp_send_buf[2] = IRS_CONST_LOBYTE(static_cast<irs_u16>(a_index));
      mp_spi->write(mp_send_buf, m_spi_size);
      for (; (mp_spi->get_status() != irs::spi_t::FREE); )
        mp_spi->tick();
      size_type data_index_cur = 0;
      while (data_index_cur != m_size) {
        if ((m_size - data_index_cur) >= m_spi_size) {
          mp_spi->write(ap_buf + data_index_cur, m_spi_size);
          data_index_cur += m_spi_size;
        } else {
          mp_spi->write(ap_buf + data_index_cur, m_size - data_index_cur);
          data_index_cur = m_size;
        }
        for (; (mp_spi->get_status() != irs::spi_t::FREE); )
          mp_spi->tick();
      }
      mp_cs_pin->set();
      mp_spi->unlock();
      while(get_status() == busy) {}
    }
    void resize(size_type a_size)
    {
      m_size = a_size;
    }
  private:
    enum ee_status_t {
      complete,
      busy
    };
    enum {
      m_spi_size = 3,
      
      // INSTRUCTION CODES
      m_WREN = 0x06,
      m_WRDI = 0x04,
      m_RDSR = 0x05,
      m_WRSR = 0x01,
      m_READ = 0x03,
      m_WRITE = 0x02,
      
      // STATUS REGISTER BIT
      m_RDY = 0,
      m_WEN = 1,
      m_BP0 = 2,
      m_BP1 = 3,
      m_WPEN = 7
    };
    spi_t* mp_spi;
    gpio_pin_t* mp_cs_pin;
    size_type m_size;
    irs_u8 mp_recv_buf[m_spi_size];
    irs_u8 mp_send_buf[m_spi_size];
    
    ee_status_t get_status()
    {
      mp_spi->set_order(irs::spi_t::MSB);
      mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
      mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
      mp_spi->lock();
      mp_cs_pin->clear();
      mp_send_buf[0] = m_RDSR;
      mp_send_buf[1] = 0;
      mp_send_buf[2] = 0;
      mp_spi->read_write(mp_recv_buf, mp_send_buf, m_spi_size);
      for (; (mp_spi->get_status() != irs::spi_t::FREE); )
        mp_spi->tick();
      mp_cs_pin->set();
      mp_spi->unlock();
      ee_status_t ee_status = busy;
      if (!(mp_recv_buf[1] & (1 << m_RDY))) {
        ee_status = complete;
      } else {
        ee_status = busy;
      }
      return ee_status;
    }
  }; // eeprom_cluster_t
  
  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
  eeprom_cluster_t mp_cluster_data;
  size_type m_ee_size;
  size_type m_page_size;
  size_type m_page_data_size;
  status_t m_status;
  irs_u8* mp_read_buf_cur;
  const irs_u8* mp_write_buf_cur;
  size_type m_page_index;
  size_type m_op_size;
  size_type m_op_index_cur;
  bool m_need_read;
  bool m_need_write;
  operation_mode_t m_op_mode;
  raw_data_t<irs_u8> mp_target_buf;
  size_type m_first_part_size;
  size_type m_middle_part_number;
  size_type m_last_part_size;
  
  void write_part(size_type a_size);
  void read_part(size_type a_size);
}; // eeprom_command_t

//------------------------------------------------------------------------------

class eeprom_spi_t: public mxdata_t
{
public:
  typedef comm_data_t::size_type size_type;
  
  eeprom_spi_t(eeprom_command_t* ap_eeprom_command, size_type a_size);
  ~eeprom_spi_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
  bool error();
private:
  enum mode_t {
    mode_search_write_data,
    mode_write_data
  };
  
  eeprom_command_t* mp_eeprom_command;
  size_type m_size;
  raw_data_t<irs_u8> mp_target_buf;
  vector<bool> m_need_writes;
  bool m_need_write;
  size_type m_write_size;
  size_type m_search_index;
  size_type m_start_block;
  bool m_error;
  mode_t m_mode;
}; // eeprom_spi_t

} // namespace irs

#endif // irsmemh
