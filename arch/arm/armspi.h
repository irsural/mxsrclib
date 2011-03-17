//! \file
//! \ingroup drivers_group
//! \brief Драйвер SPI для ARM
//!
//! Дата: 16.03.2011
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
  enum ssi_type_t {
    SSI0,
    SSI1
  };
  enum spi_type_t {
    SPI = 0x0,
    TISS = 0x1, // Texas Instruments Synchronous Serial
    MICROWIRE = 0x2
  };
  
  arm_spi_t(
    irs_u8 a_buffer_size,
    irs_u32 a_f_osc,
    spi_type_t a_spi_type = SPI,
    ssi_type_t a_ssi_type = SSI0
  );
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
  virtual bool set_data_size(irs_u16 a_data_size);
  virtual void tick();
  virtual void read_write(irs_u8 *ap_read_buf, const irs_u8 *ap_write_buf, 
    irs_uarc a_size);
private:
  enum cur_status_t {
    SPI_FREE,
    SPI_READ,
    SPI_WRITE,
    SPI_RW_READ,
    SPI_RW_WRITE
  };
  enum {
    SSI0Clk = 0x1,
    SSI0Rx = 0x1,
    SSI0Tx = 0x1,
    SSI1Clk = 0xB,
    SSI1Rx = 0xB,
    SSI1Tx = 0xB
  };
  
  cur_status_t m_status;
  raw_data_t<irs_u8> mp_buf;
  raw_data_t<irs_u8> mp_rw_buf;
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
  ssi_type_t m_ssi_type;
  
  void write_data_register(irs_u8 a_data);
  irs_u8 read_data_register();
  bool transfer_complete();
  void init_default();
};

//! @}

} // namespace arm

} // namespace irs

#endif // armspiH
