#ifndef SPI_NIIET_H
#define SPI_NIIET_H

#include "plib035.h"
#include <irsspi.h>

namespace irs
{

class spi_niiet_t : public spi_t
{
public:
  spi_niiet_t();
  virtual ~spi_niiet_t() {}
  virtual void abort();
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size);
  virtual status_t get_status();
  irs_status_t get_irs_status();
  virtual void lock();
  virtual void unlock();
  virtual bool get_lock();
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(polarity_t a_polarity);
  virtual bool set_phase(phase_t a_phase);
  virtual bool set_order(order_t a_order);
  virtual bool set_data_size(irs_u16 a_data_size);
  virtual void tick();
  virtual void read_write(irs_u8 *ap_read_buf, const irs_u8 *ap_write_buf,
    irs_uarc a_size);

private:
  enum inner_state
  {
    st_free,
    st_read,
    st_write,
    st_read_write
  };

  inner_state m_status;
  irs_u8 *mp_buf;
  irs_uarc m_buf_size;
  bool m_lock;
  irs_u32 m_bitrate;
  polarity_t m_polarity;
  phase_t m_phase;
  order_t m_order;
  irs_u32 m_frequency;

  void init_io_oper(irs_u8 *ap_buf, irs_uarc a_size, inner_state a_status);
  void setCS();
  void clearCS();
  void read_buf();
  void write_buf();
  void read_write_buf();
};

}

#endif //SPI_NIIET_H
