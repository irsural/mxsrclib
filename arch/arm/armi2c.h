#ifndef armi2cH
#define armi2cH

#include "irsi2c.h"

namespace irs
{

namespace arm
{
#if defined (IRS_NIIET_1921)

class arm_i2c_t : public i2c_t
{
public:
  arm_i2c_t();
  ~arm_i2c_t();
  void lock();
  void unlock();
  bool get_lock();
  void set_device_addr(irs_u16 a_device_addr);
  irs_u16 get_device_addr();
  void set_status(status_t a_status);
  void read(irs_u8 *ap_buf, irs_u16 a_size);
  void write(irs_u8 *ap_buf, irs_u16 a_size);
  status_t get_status();
  void tick();
private:
  bool m_lock;
  status_t m_status;
  irs_u16 m_device_address;
};

class port_extender_i2c_t //common class for r/w to pca9539 port extender chip.
{
public:
  enum {
    input_port_0 = 0,
    input_port_1 = 1,
    output_port_0 = 2,
    output_port_1 = 3,
    polarity_inversion_port_0 = 4,
    polarity_inversion_port_1 = 5,
    configuration_port_0 = 6,
    configuration_port_1 = 7
  };

  port_extender_i2c_t(arm_i2c_t* ap_arm_i2c_t);
  ~port_extender_i2c_t();

  irs_u8 read_register (irs_u8 a_reg);
  irs_u16 read_registers_group (irs_u8 a_first_reg);
  void write_to_register(irs_u8 a_reg, irs_u8 a_data);
  void write_to_registers_group(irs_u8 a_reg, irs_u16 a_data);
  void set_logic_level_data (irs_u16 a_logic_level_data);
  irs_u16 get_logic_level_data();
  void set_input_output_direction(irs_u16 a_input_output_direction);
  irs_u16 get_input_output_direction();
private:
  arm_i2c_t* mp_arm_i2c_t;
  irs_u16 m_logic_level_data;
  irs_u16 m_input_output_direction;
};

class eeprom_i2c_t : public arm_i2c_t
{
public:
  eeprom_i2c_t(arm_i2c_t* ap_arm_i2c_t);
  ~eeprom_i2c_t();
  void read_byte(irs_u16 a_address, irs_u8* ap_buf);
  void read_page(irs_u16 a_address, irs_u8* ap_buf, irs_u16 a_size);
  void write_byte(irs_u16 a_address, irs_u8* ap_buf);
  void write_page(irs_u16 a_address, irs_u8* ap_buf, irs_u16 a_size);
  void set_page_size(irs_u8 a_page_size);
  irs_u8 get_page_size(void);
private:
  arm_i2c_t* mp_arm_i2c_t;
  irs_u8 m_page_size;
};

#else
  #error Тип контроллера не определён
#endif //defined (IRS_NIIET_1921)

} //namespace arm

}//namespace irs

#endif