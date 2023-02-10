#ifndef IRSEEPROM_H
#define IRSEEPROM_H

#include <i2c.h>
#include <irsstd.h>
#include <irsdefs.h>
#include <mxdata.h>
#include <irsmem.h>

namespace irs
{

class eeprom_m24_page_t: public page_mem_t
{
public:
  eeprom_m24_page_t(irs_u16 a_i2c_address, i2c_t* ap_i2c, size_type a_page_size,
    size_type a_page_count, size_type a_address_size = 2);
  virtual ~eeprom_m24_page_t();
  virtual void read_page(irs_u8 *ap_buf, irs_uarc a_index);
  virtual void write_page(const irs_u8 *ap_buf, irs_uarc a_index);
  virtual size_type page_size() const;
  virtual irs_uarc page_count() const;
  virtual irs_status_t status() const;
  virtual void tick();

private:
  enum m24_status_t
  {
    m24_free,
    m24_read,
    m24_write,
    m24_send_addr,
    m24_wait
  };

  bool is_free();
  void write_buf();
  void send_seek();
  void initialize_io_operation(irs_u8* ap_data, irs_u16 a_index, \
  m24_status_t a_status);

  irs_u16 m_i2c_address;
  irs_u8 m_page_size;
  irs_u8 m_page_count;
  i2c_t *mp_i2c;
  irs_u16 m_mem_addr;
  m24_status_t m_status;
  irs_u8 *mp_buffer;
  irs_u32 m_max_seek;
  irs_u8 m_address_size;
  irs_u8 *mp_next_page;
  irs_u8 *mp_mem_addr_buf;
};


// *****************************************************************************


class eeprom_i2c_t: public mxdata_comm_t
{
public:
  eeprom_i2c_t(irs_u16 a_i2c_address, i2c_t* ap_i2c, irs_uarc a_size,
    size_t a_page_size, size_t a_page_count, size_t a_address_size,
    size_t a_cluster_size = 64, bool a_init_now = false,
    irs_uarc a_index = 0,
    counter_t a_init_timeout = irs::make_cnt_s(1));

private:
  eeprom_m24_page_t m_page_mem;
  mem_data_t m_mem_data;
};

} //namespace irs

#endif //IRSEEPROM_H
