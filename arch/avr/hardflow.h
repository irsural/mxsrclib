// Низкоуровневая передача через SPI
// Дата: 26.02.2009

#ifndef hardflowH
#define hardflowH

#ifndef __ATmega128__

#include <timer.h>
#include <irsstd.h>
#include <string.h>
#include <hardflowg.h>

namespace irs {

namespace avr {

class spi_master_flow_t : public hardflow_t
{
  enum status_t
  {
    PREPARATION,
    TRANSLATION_ENTER,
    TRANSLATION,
    PROCESSING
  };
  //  служебные переменные
  status_t m_status;
  //  структура пакета
  static const irs_uarc m_neg_pos = 0;
  static const irs_uarc m_crc_pos = 1;
  static const irs_uarc m_channel_id_pos = 2;
  static const irs_uarc m_size_pos = 3;
  static const irs_uarc m_data_pos = 4;
  static const irs_uarc m_header_size = 4;
  //  Временные интервалы
  counter_t m_byte_delay;
  counter_t m_byte_counter;
  counter_t m_preparation_delay;
  counter_t m_preparation_counter;
  //  буфера
  irs_u8 *mp_read_buffer;
  irs_u8 *mp_write_buffer;
  irs_u8 *mp_user_read_buffer;
  irs_u8 *mp_user_write_buffer;
  irs_u8 m_buffer_size;
  irs_u8 m_current_byte;
  irs_u8 m_rb_channel_id;
  irs_u8 m_rb_packet_size;
  irs_u8 m_rb_read_size;
  irs_u8 m_max_buffer_size;
  //  флаги
  bool m_rb_new_data;
  bool m_wb_new_data;
  bool m_can_write;
  //  функции обращения к железу
  inline void write_data_register(irs_u8 a_data);
  inline irs_u8 read_data_register();
  inline bool transfer_complete();
  inline void ss_high();
  inline void ss_low();
  //  переменные железа
  irs_u8 m_sclk_div;
  irs_u8 m_current_sclk_div;
  //  служебные функции
  void apply_sclk_div(irs_u8 a_sclk_div);
public:
  spi_master_flow_t(irs_u8 a_buffer_size);
  virtual ~spi_master_flow_t();
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const irs::string &a_name, const irs::string &a_value);
  virtual irs_uarc read(irs_uarc a_channel_id, irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual irs_uarc write(irs_uarc a_channel_id, const irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual void tick();
};


class spi_slave_flow_t : public hardflow_t
{
  //  Прерывания
  class spi_stc_interrupt_t: public mxfact_event_t
  {
    spi_slave_flow_t *mp_outer;
  public:
    spi_stc_interrupt_t(spi_slave_flow_t *ap_outer);
    virtual void exec();
  };
  class pcint0_interrupt_t: public mxfact_event_t
  {
    spi_slave_flow_t *mp_outer;
  public:
    pcint0_interrupt_t(spi_slave_flow_t *ap_outer);
    virtual void exec();
  };
  friend class spi_stc_interrupt_t;
  friend class pcint0_interrupt_t;
  //  структура пакета
  static const irs_uarc m_neg_pos = 0;
  static const irs_uarc m_crc_pos = 1;
  static const irs_uarc m_channel_id_pos = 2;
  static const irs_uarc m_size_pos = 3;
  static const irs_uarc m_data_pos = 4;
  static const irs_uarc m_header_size = 4;
  //  буфера
  irs_u8 *mp_read_buffer;
  irs_u8 *mp_user_read_buffer;
  irs_u8 *mp_write_buffer;
  irs_u8 *mp_user_write_buffer;
  irs_u8 m_buffer_size;
  irs_u8 m_current_byte;
  irs_u8 m_rb_channel_id;
  irs_u8 m_rb_read_size;
  irs_u8 m_rb_packet_size;
  irs_u8 m_buffer_top;
  //  флаги
  bool m_rb_new_data;
  bool m_wb_new_data;
  bool m_can_write;
  spi_stc_interrupt_t m_spi_stc_interrupt;
  pcint0_interrupt_t m_pcint0_interrupt;
  //  функции обращения к железу
  inline void write_data_register(irs_u8 a_data);
  inline irs_u8 read_data_register();
  inline void byte_interrupt();
  inline void packet_interrupt();
public:
  spi_slave_flow_t(irs_u8 a_buffer_size);
  virtual ~spi_slave_flow_t();
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const string &a_name, const string &a_value);
  virtual irs_uarc read(irs_uarc a_channel_id, irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual irs_uarc write(irs_uarc a_channel_id, const irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual void tick();
};

} //  avr

} //  irs

#endif //__ATmega128__

#endif //hardflowH
