// Драйвер SPI
// Дата 26.02.2009
// AVR

#ifndef avrspiH
#define avrspiH

#ifndef __ATmega128__

#include <irsspi.h>
#include <irsstd.h>

namespace irs
{

namespace avr
{

class avr_spi_t : public spi_t
{
public:
  enum spi_type_t
  {
    USART0,
    USART1,
    SPI
  };
private:
  //  переменные состояния
  enum current_status_t
  {
    SPI_FREE,
    SPI_READ,
    SPI_WRITE
  };
  current_status_t m_status;
  //  буфера
  irs_u8 *mp_buf;
  irs_u8 *mp_target_buf;
  irs_u8 m_buffer_size;
  irs_u8 m_current_byte;
  irs_u8 m_packet_size;
  //  параметры передачи
  irs_u32 m_bitrate;
  polarity_t m_polarity;
  phase_t m_phase;
  order_t m_order;
  //  флаги
  bool m_buf_empty;
  bool m_lock;
  //  параметры железа
  irs_u32 m_f_osc;
  spi_type_t m_spi_type;
  //  функции работы с железом
  void write_data_register(irs_u8 a_data);
  irs_u8 read_data_register();
  bool transfer_complete();
public:
  avr_spi_t(irs_u8 a_buffer_size, irs_u32 a_f_osc, spi_type_t a_spi_type);
  virtual ~avr_spi_t();
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
  virtual bool set_order(order_t a_order);
  virtual void tick();
};

} //  avr

} //  irs

#endif //__ATmega128__

#endif // avrspiH
