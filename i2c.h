#ifndef I2C_T_H
#define I2C_T_H

#include <irsdefs.h>
#include <irsspi.h>


namespace irs
{

class i2c_t
{
public:
  enum status_t
  {
    i2c_ready,
    i2c_busy
  };

  virtual ~i2c_t() {}
  virtual bool is_free() = 0;
  virtual status_t get_status() = 0;
  virtual void lock() = 0;
  virtual bool get_lock() = 0;
  virtual void unlock() = 0;
  virtual void set_device_address(irs_u16 a_addr_device) = 0;
  virtual void tick() = 0;
  virtual void read(irs_u8* ap_buf, size_t a_size) = 0;
  virtual void write(irs_u8* ap_buf, size_t a_size) = 0;
  virtual void abort() = 0;
};

class spi_to_i2c_t : public spi_t
{
public:
  spi_to_i2c_t(i2c_t *a_i2c, irs_u16 a_addr_device, irs_u16 a_buffer_size);
  virtual ~spi_to_i2c_t() { }

  virtual void read(irs_u8 *ap_buf, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_size);
  virtual status_t get_status();
  virtual void lock();
  virtual void unlock();
  virtual bool get_lock();
  virtual void tick();
  virtual void abort();

  virtual bool set_data_size(irs_u16 a_data_size);
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(polarity_t a_polarity);
  virtual bool set_phase(phase_t a_phase);
  virtual bool set_order(order_t a_order);
  virtual void read_write(irs_u8 *ap_read_buf,
    const irs_u8 *ap_write_buf, irs_uarc a_size) { }
  virtual inline void reset_configuration() { }
  void init_default() { }

private:
  i2c_t *mp_i2c;
  irs_u16 m_address_device;
  irs_u16 m_buffer_size;
  irs_u8 *mp_buffer;
};


}

#endif /*I2C_T_H*/