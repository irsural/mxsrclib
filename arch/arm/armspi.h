//! \file
//! \ingroup drivers_group
//! \brief Драйвер SPI для ARM
//!
//! Дата: 30.11.2010
//! Дата создания: 30.11.2010

#ifndef armspiH
#define armspiH

#include <irsdefs.h>

#include <irsspi.h>
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {

namespace arm {

//! \addtogroup drivers_group
//! @{

class arm_spi_t: public spi_t
{
public:
  enum spi_type_t {
    SPI = 0x0,
    TISS = 0x1, // Texas Instruments Synchronous Serial
    MICROWIRE = 0x2
  };
  
  arm_spi_t(irs_u8 a_buffer_size, irs_u32 a_f_osc, spi_type_t a_spi_type);
  virtual ~arm_spi_t();
  virtual void abort();
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size);
  virtual status_t get_status();
  virtual void lock();
  virtual void unlock();
  virtual bool get_lock();
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(polarity_t a_polarity);
  virtual bool set_phase(phase_t a_phase);
  virtual bool set_order(order_t /*a_order*/);
  virtual void tick();
private:
  enum cur_status_t {
    SPI_FREE,
    SPI_READ,
    SPI_WRITE
  };
  enum {
    PORTA_SSI0Clk = 0x100,
    PORTA_SSI0Fss = 0x1000,
    PORTA_SSI0Rx = 0x10000,
    PORTA_SSI0Tx = 0x100000
  };
  
  cur_status_t m_status;
  raw_data_t<irs_u8> mp_buf;
  irs_u8* mp_target_buf;
  irs_u8 m_buf_size;
  irs_u8 m_cur_byte;
  irs_u8 m_packet_size;
  irs_u32 m_bitrate;
  polarity_t m_polarity;
  phase_t m_phase;
  order_t m_order;
  bool m_buf_empty;
  bool m_lock;
  irs_u32 m_f_osc;
  spi_type_t m_spi_type;
  
  void write_data_register(irs_u8 a_data);
  irs_u8 read_data_register();
  bool transfer_complete();
};

//! @}

} // namespace arm

} // namespace irs

#endif // armspiH
