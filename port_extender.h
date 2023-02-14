#ifndef PORT_EXTENDER_H
#define PORT_EXTENDER_H

#include <irsstd.h>
#include <irsdefs.h>
#include <i2c.h>

namespace irs
{

class port_extender_t
{
public:
  enum error_t
  {
    err_invalid_arg,
    err_none
  };

  virtual void write_pin(irs_u8 a_port, irs_u8 a_pin) = 0;
  virtual void read_pin(irs_u8 a_port, irs_u8 a_pin, irs_u8* a_user_pin) = 0;
  virtual void clear_pin(irs_u8 a_port, irs_u8 a_pin) = 0;
  virtual void toggle_pin(irs_u8 a_port, irs_u8 a_pin);
  virtual void tick() = 0;
  virtual bool is_free() = 0;
  virtual irs_status_t get_status() = 0;
  virtual void abort() = 0;
  virtual error_t get_error() = 0;
  virtual bool is_ready() = 0;
};

class port_extender_pca9539_t : public port_extender_t
{
public:
  enum port_t
  {
    port_0 = 0,
    port_1 = 1,
    port_none = 255
  };

  enum pin_t
  {
    pin_0 = 0,
    pin_1 = 1,
    pin_2 = 2,
    pin_3 = 3,
    pin_4 = 4,
    pin_5 = 5,
    pin_6 = 6,
    pin_7 = 7,
    pin_none = 255
  };

  port_extender_pca9539_t(irs_u8 a_i2c_addr, i2c_t* ap_i2c, irs_u8 a_default_port_0 = 0x00, irs_u8 a_default_port_1 = 0x00, bool do_reset = true);
  virtual void write_pin(irs_u8 a_port, irs_u8 a_pin);
  virtual void read_pin(irs_u8 a_port, irs_u8 a_pin, irs_u8* a_user_pin);
  virtual void clear_pin(irs_u8 a_port, irs_u8 a_pin);
  virtual void toggle_pin(irs_u8 a_port, irs_u8 a_pin);
  virtual void tick();
  virtual bool is_free();
  virtual irs_status_t get_status();
  virtual void abort();
  virtual error_t get_error();
  virtual bool is_ready();

private:
  enum status_t
  {
    in_initialize,
    in_free,
    in_write_pin,
    in_read_pin,
    in_clear_pin,
    in_toggle_pin,
    in_wait,
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
  static const irs_u8 port_count = 2;

  status_t m_status;
  i2c_t* mp_i2c;
  irs_u8 m_i2c_addr;
  irs_u8 m_buffer[buffer_size];
  irs_u8 m_default_port_0;
  irs_u8 m_default_port_1;
  irs_u8 m_pin;
  command_t m_cmd;
  irs_u8 m_inner_port_0;
  irs_u8 m_inner_port_1;
  irs_u8* mp_inner_port;
  irs_u8* mp_user_pin;
  error_t m_error;
  irs_u8 m_port;


  void initialize_port_extender();
  void initialize_io_operation(irs_u8 a_port, irs_u8 a_pin, status_t a_status, command_t a_cmd);
  void send_i2c();
  void check_arg(irs_u8 a_port, irs_u8 a_pin);
};

} // namespace irs

#endif // PORT_EXTENDER_H