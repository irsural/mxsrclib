#ifndef PORT_EXTENDER_H
#define PORT_EXTENDER_H

#include <i2c.h>
#include <irsstd.h>
#include <irsdefs.h>

using namespace irs;

class port_extender_t
{
public:
  virtual void write_pin(irs_u8 a_pin) = 0;
  virtual void read_pin(irs_u8 a_pin, irs_u8* a_user_pin) = 0;
  virtual void clear_pin(irs_u8 a_pin) = 0;
  virtual void tick() = 0;
  virtual bool is_free() = 0;
  virtual irs_status_t get_status() = 0;
  virtual void abort() = 0;
};

class port_extender_pca9539_t : public port_extender_t
{
public:
  port_extender_pca9539_t(i2c_t* ap_i2c, irs_u8 a_i2c_addr, irs_u8 a_default_port_0 = 0x00, irs_u8 a_default_port_1 = 0x00);
  virtual void write_pin(irs_u8 a_pin);
  virtual void read_pin(irs_u8 a_pin, irs_u8* a_user_pin);
  virtual void clear_pin(irs_u8 a_pin);
  virtual void tick();
  virtual bool is_free();
  virtual irs_status_t get_status();
  virtual void abort();
private:
  enum status_t
  {
    in_initialize,
    in_free,
    in_write_pin,
    in_read_pin,
    in_clear_pin,
    in_wait,
    in_wait_reset,
    in_error
  };

  enum command_t
  {
    cmd_input_port_0   = 0x0,
    cmd_input_port_1   = 0x1,
    cmd_output_port_0  = 0x2,
    cmd_output_port_1  = 0x3,
    cmd_inverse_port_0 = 0x4,
    cmd_inverse_port_1 = 0x5,
    cmd_config_port_0  = 0x6,
    cmd_config_port_1  = 0x7,
    cmd_none
  };

  static const irs_u8 buffer_size = 3;
  static const irs_u8 port_size = 8;

  status_t m_status;
  i2c_t* mp_i2c;
  irs_u8 m_i2c_addr;
  irs_u8 m_buffer[buffer_size];
  irs_u8 m_default_port_0;
  irs_u8 m_default_port_1;
  irs_u8 m_pin;
  command_t m_cmd;
  irs_u8 m_port_0;
  irs_u8 m_port_1;
  irs_u8* mp_port;
  irs_u8* mp_user_pin;


  void initialize_port_extender();
  void initialize_io_operation(irs_u8 a_pin, status_t a_status, command_t a_cmd);
  void send_i2c();
};

#endif // PORT_EXTENDER_H
