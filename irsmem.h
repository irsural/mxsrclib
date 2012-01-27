// –î–∞—Ç–∞: 19.12.2011
// –î–∞—Ç–∞ —Å–æ–∑–¥–∞–Ω–∏—è: 25.04.2011

#ifndef irsmemh
#define irsmemh

#include <irsdefs.h>

#include <irsspi.h>
#include <irsgpio.h>
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {

enum eeprom_type_t {
  at25010,
  at25020,
  at25040,
  at25128,
  at25256
};

class eeprom_at25_t: public page_mem_t
{
public:
  eeprom_at25_t(spi_t& a_spi, gpio_pin_t& a_cs_pin,
    eeprom_type_t a_eeprom_type = at25128);
  ~eeprom_at25_t();
  void read_page(irs_u8 *ap_buf, irs_uarc a_index);
  void write_page(const irs_u8 *ap_buf, irs_uarc a_index);
  size_type page_size() const;
  irs_uarc page_count() const;
  irs_status_t status() const;
  void tick();
private:
  enum {
    m_spi_size = 4,
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
    m_WPEN = 7,
    //  ANSWER POSITION
    m_command_pos = 0,
    m_addr_lo_pos = 2,
    m_addr_hi_pos = 1,
    m_RDSR_answer_pos = 1,
    m_WRSR_data_pos = 1,
    //  ASK SIZES
    m_RDSR_size = 2,
    m_WRSR_size = 2,
    m_WREN_size = 1,
    m_initiate_size = 3
  };
  enum status_t {
    st_free,
    st_check_ready_prepare,
    st_check_ready,
    st_write_protect_check,
    st_write_protect_disable_wait,
    st_write_protect_disable,
    st_write_protect_disable_finish,
    st_read_prepare,
    st_read_initiate,
    st_read,
    st_read_modulo,
    st_write_enable,
    st_write_prepare,
    st_write_initiate,
    st_write,
    st_write_modulo,
    st_complete,
    st_error
  };

  spi_t& m_spi;
  gpio_pin_t& m_cs_pin;
  size_type m_page_size;
  irs_uarc m_page_count;
  status_t m_status;
  status_t m_target_status;
  irs_u8 mp_read_buf[m_spi_size];
  irs_u8 mp_write_buf[m_spi_size];
  irs_u8* mp_read_user_buf;
  irs_u8 const* mp_write_user_buf;
  irs_uarc m_num_of_iterations;
  irs_uarc m_current_iteration;
  size_type m_modulo_size;
  irs_uarc m_page_addr;

  void prepare_spi();
  void clear_spi();
  void write_protect_disable();
  void write_enable();
  bool status_register_write_protect();
  void transaction_initiate(const irs_u8 a_command);
  void read_status_register();
  bool status_register_ready();
};

//  ‚ cluster_size ‚ıÓ‰ËÚ 4 ·‡ÈÚ‡ ˆˆ

class mem_cluster_t
{
public:
  mem_cluster_t(page_mem_t& a_page_mem, size_t a_cluster_size = 64);
  ~mem_cluster_t();
  void read_cluster(irs_u8 *ap_buf, irs_uarc a_cluster_index);
  void write_cluster(const irs_u8 *ap_buf, irs_uarc a_cluster_index);
  bool error() const;
  irs_status_t status() const;
  void tick();
private:
  enum {
    m_crc_size = 4
  };
  enum operation_t {
    op_read,
    op_write
  };
  enum status_t {
    st_free,
    st_error,
    st_read_begin,
    st_read_process,
    st_read_check_crc,
    st_write_begin,
    st_write_process,
    st_write_begin_2_half
  };

  page_mem_t& m_page_mem;
  status_t m_status;
  status_t m_target_status;
  irs_uarc m_pages_per_half_cluster;
  size_t m_cluster_size;
  size_t m_data_size;
  irs_uarc m_clusters_count;
  bool m_error;
  irs_uarc m_page_index;
  irs_uarc m_cluster_index;
  irs_uarc m_cluster_data_index;
  raw_data_t<irs_u8> m_cluster_data;
  raw_data_view_t<irs_u32, irs_u8> m_cluster_data_32;
  irs_u8* mp_read_buf;
};

class mem_data_t : public comm_data_t
{
public:
  typedef comm_data_t::size_type size_type;
  mem_data_t(page_mem_t& a_page_mem, size_type a_cluster_size,
    size_type a_start_index, size_type a_size);
  ~mem_data_t();
  void read(irs_u8* ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8* ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_status_t status();
  size_type size();
  void tick();
private:
  enum status_t {
    st_free,
    st_error,
    st_read_begin,
    st_read_process,
    st_write_begin,
    st_write_process
  };
  mem_cluster_t m_cluster;
  status_t m_status;
  size_type m_cluster_data_size;
  size_type m_size;
};

} // namespace irs

#endif // irsmemh
