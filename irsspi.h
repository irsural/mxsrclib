//! \file
//! \ingroup drivers_group
//! \brief Абстрактный базовый класс для SPI
//!
//! Дата: 29.02.2008

#ifndef irsspiH
#define irsspiH

#include <irsdefs.h>

namespace irs
{

//! \addtogroup string_processing_group
//! @{

class spi_t
{
public:
  enum status_t   {FREE, BUSY};
  enum polarity_t {RISING_EDGE, FALLING_EDGE};
  enum phase_t    {LEAD_EDGE, TRAIL_EDGE};
  enum order_t    {LSB, MSB};

  virtual ~spi_t() {}
  virtual void abort() = 0;
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size) = 0;
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size) = 0;
  virtual status_t get_status() = 0;
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual bool get_lock() = 0;
  virtual bool set_bitrate(irs_u32 a_bitrate) = 0;
  virtual bool set_polarity(polarity_t a_polarity) = 0;
  virtual bool set_phase(phase_t a_phase) = 0;
  virtual bool set_order(order_t a_order) = 0;
  virtual bool set_data_size(irs_u16 a_data_size) = 0;
  virtual void tick() = 0;
  virtual void read_write(irs_u8 *ap_read_buf, const irs_u8 *ap_write_buf, 
    irs_uarc a_size) = 0;
};

//! @}

}

#endif //irsspiH
