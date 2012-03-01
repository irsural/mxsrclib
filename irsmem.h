// Дата: 19.12.2011
// Дата создания: 25.04.2011

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
  eeprom_at25_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
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

  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
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

//  в cluster_size входит 4 байта црц

class mem_cluster_t
{
public:
  mem_cluster_t(page_mem_t* ap_page_mem, size_t a_cluster_size = 64);
  ~mem_cluster_t();
  void read_cluster(irs_u8 *ap_buf, irs_uarc a_cluster_index);
  void write_cluster(const irs_u8 *ap_buf, irs_uarc a_cluster_index);
  irs_status_t status() const;
  void tick();
  irs_uarc cluster_count() const;
  size_t crc_size() const;
  bool error();
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

  page_mem_t* mp_page_mem;
  status_t m_status;
  status_t m_target_status;
  irs_uarc m_pages_per_half_cluster;
  size_t m_cluster_size;
  size_t m_data_size;
  irs_uarc m_clusters_count;
  bool m_error_status;
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
  mem_data_t(page_mem_t* ap_page_mem, size_type a_cluster_size = 64);
  ~mem_data_t();
  void read(irs_u8* ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8* ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_status_t status();
  size_type size();
  void tick();
  irs_uarc data_count() const;
  bool error();
private:
  enum status_t {
    st_free,
    st_error,
    st_read_begin,
    st_read_process,
    st_write_begin,
    st_write_process,
    st_write_calculation
  };
  mem_cluster_t m_cluster;
  status_t m_status;
  size_type m_size;
  const size_type m_cluster_size;
  const size_type m_cluster_data_size;
  irs_uarc m_data_count;
  raw_data_t<irs_u8> m_data_buf;
  raw_data_t<irs_u8> m_buf;
  irs_uarc m_start_index;
  irs_uarc m_end_index;
  irs_uarc m_data_size;
  int m_cluster_start_index;
  int m_cluster_next_index;
  int m_cluster_end_index;
  int m_cluster_curr_index;
  int m_index_data_buf;
  bool m_end_cluste;
  irs_u8* mp_read_buf;
};

class mxdata_comm_t: public mxdata_t
{
public:
  mxdata_comm_t(irs::mem_data_t* ap_mem_data, 
    irs_uarc a_index, irs_uarc a_size, bool a_init_now = false,
    int a_init_timeout = 1);
  virtual ~mxdata_comm_t(); 
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void write_bit(irs_uarc a_index, irs_uarc a_bit_index, 
    irs_bool a_bit);
  virtual void tick();
  bool error();
  irs::mem_data_t* mem_data();
protected:
  void connect(irs::mem_data_t* ap_mem_data);
private:
  irs::mem_data_t* mp_mem_data;
  raw_data_t<irs_u8> m_data_buf;
  irs_uarc m_mem_data_start_index;
  vector<bool> m_bit_vector;
  enum mode_t {
    mode_free,
    mode_error,
    mode_write,
    mode_write_wait,
    mode_initialization,
    mode_initialization_wait
  };
  mode_t m_mode;
  int m_current_index;
  int m_start_index;
  int m_data_size;
  bool m_connected;
  irs::timer_t m_timer;
  bool m_init_now;
};

class eeprom_at25128_data_t : public mxdata_comm_t
{
public:
  typedef comm_data_t::size_type size_type;
  eeprom_at25128_data_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin, irs_uarc a_size, 
    bool init_now = false,  irs_uarc a_index = 0, size_type a_cluster_size = 64,
    int init_timeout = 1);
private:
  irs::eeprom_at25_t m_page_mem;
  irs::mem_data_t m_mem_data;
};

} // namespace irs

#endif // irsmemh
