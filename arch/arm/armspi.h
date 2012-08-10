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
#include <armioregs.h>

#include <irsfinal.h>

namespace irs {

namespace arm {

//! \addtogroup drivers_group
//! @{
#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
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
    irs_u32 a_bitrate,
    spi_type_t a_spi_type = SPI,
    ssi_type_t a_ssi_type = SSI0,
    arm_port_t& a_clk_port = GPIO_PORTH,
    arm_port_t& a_rx_port = GPIO_PORTH,
    arm_port_t& a_tx_port = GPIO_PORTH
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
  // Контроллер поддерживает только MSB
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
    SSI1Clk_E = 0x2,
    SSI1Rx_E = 0x2,
    SSI1Tx_E = 0x2,
    SSI1Clk_F = 0x9,
    SSI1Rx_F = 0x9,
    SSI1Tx_F = 0x9,
    SSI1Clk_H = 0xB,
    SSI1Rx_H = 0xB,
    SSI1Tx_H = 0xB
  };
  struct reg_t {
    volatile __ssicr0_bits* mp_SSICR0_bit;
    volatile __ssicr1_bits* mp_SSICR1_bit;
    volatile unsigned short* mp_SSIDR;
    volatile __ssisr_bits* mp_SSISR_bit;
    volatile unsigned char* mp_SSICPSR;
    volatile __ssiim_bits* mp_SSIIM_bit;
    volatile __ssiris_bits* mp_SSIRIS_bit;
    volatile __ssimis_bits* mp_SSIMIS_bit;
    volatile __ssiicr_bits* mp_SSIICR_bit;

    reg_t(ssi_type_t a_ssi_type);
  };
  cur_status_t m_status;
  const irs_u8* mp_write_buf;
  irs_u8* mp_read_buf;
  irs_u8 m_cur_byte;
  irs_uarc m_packet_size;
  const irs_u32 m_bitrate_def;
  const polarity_t m_polarity_def;
  const phase_t m_phase_def;
  const order_t m_order_def;
  const irs_u16 m_data_size_def;
  bool m_buf_empty;
  bool m_lock;
  spi_type_t m_spi_type;
  ssi_type_t m_ssi_type;
  reg_t m_reg;
  bool m_on_prev;

  void write_data_register(irs_u8 a_data);
  irs_u8 read_data_register();
  bool transfer_complete();
  void init_default();
};

#elif defined(__STM32F100RBT__)
#else
  #error Тип контроллера не определён
#endif  //  mcu type

//! @}

} // namespace arm

} // namespace irs

#endif // armspiH
