//! \file
//! \ingroup drivers_group
//! \brief ������ ��� ������ � ����������� ����
//!
//! ����: 02.11.2010\n
//! ������ ����: 15.01.2008

//#include <vcl.h>
//#pragma hdrstop

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsstd.h>
#include <timer.h>
#include <string.h>
#include <meassup.h>
#include <stdio.h>
#include <irscpp.h>

#include <irsfinal.h>

// ����� ��� ������ � ���������� ����������� ���� Agilent 6675A

// ������� ��� �������������
const irs_u8* mx_agilent_6675a_t::f_init_commands[] = {
  reinterpret_cast<const irs_u8*>("PRESET NORM\r\n")
  //reinterpret_cast<irs_u8*>("OFORMAT ASCII\r\n"),
};
// ����� ������ �������������
const irs_i32 mx_agilent_6675a_t::f_ic_count =
  IRS_ARRAYOFSIZE(f_init_commands);

// �����������
mx_agilent_6675a_t::mx_agilent_6675a_t(mxifa_ch_t channel,
  gpib_addr_t address):
  f_handle(IRS_NULL),
  f_create_error(irs_false),
  f_mode(mode_start),
  f_status(meas_status_success),
  //f_command(com_free),
  f_abort_request(irs_false)
  //f_current(0.),
  //f_voltage(0.)
{
  init_to_cnt();
  mxifa_init();

  // �������� ������ MXIFA_SUPPLY
  if (mxifa_get_channel_type_ex(channel) == mxifa_ei_win32_ni_usb_gpib) {
    mxifa_win32_ni_usb_gpib_cfg config;
    config.address = address;
    f_handle = mxifa_open_ex(channel, &config, irs_false);
  } else {
    f_handle = mxifa_open(channel, irs_false);
  }

  // ������� ������ ������� GPIB
  memset(f_gpib_command, 0, supag_gpib_com_buf_size);

  #ifdef NOP
  {
    // ������ ������ �������������
    #define write_t TIME_TO_CNT(1, 2)
    counter_t write_to = 0;
    irs_bool write_wait = irs_false;
    index_t ic_index = 0;
    for (;;) {
      mxifa_tick();
      if (write_wait) {
        if (mxifa_write_end(f_handle, irs_false)) {
          write_wait = irs_false;
          ic_index++;
          if (ic_index >= f_ic_count) break;
        } else if (test_to_cnt(write_to)) {
          mxifa_write_end(f_handle, irs_true);
          goto _error;
        }
      } else {
        irs_u8 *icommand = f_init_commands[ic_index];
        irs_u32 len = strlen((char *)icommand);
        mxifa_write_begin(f_handle, IRS_NULL, icommand, len);
        set_to_cnt(write_to, write_t);
        write_wait = irs_true;
      }
    }
  }

  // ������� ������ ������
  memset(f_read_buf, 0, ma_read_buf_size);
  return;

  _error:
  f_create_error = irs_true;
  #endif //NOP
}
// ����������
mx_agilent_6675a_t::~mx_agilent_6675a_t()
{
  mxifa_close_begin(f_handle);
  mxifa_close_end(f_handle, irs_true);
  mxifa_deinit();
  deinit_to_cnt();
}
// ���������� ��� �������������� �������� ��� ���
irs_bool mx_agilent_6675a_t::dc_supported()
{
  return irs_true;
}
// ��������� ����
void mx_agilent_6675a_t::set_current(double current)
{
  if (f_create_error) return;
  ostrstream strm(reinterpret_cast<char*>(f_gpib_command),
    supag_gpib_com_buf_size - 1);
  strm << "CURR " << current << '\0';
  //f_gpib_command[strm.pcount()] = 0;
  f_status = meas_status_busy;
}
// ��������� ����������
void mx_agilent_6675a_t::set_voltage(double voltage)
{
  if (f_create_error) return;
  ostrstream strm(reinterpret_cast<char*>(f_gpib_command),
    supag_gpib_com_buf_size - 1);
  strm << "VOLT " << voltage << '\0';
  //f_gpib_command[strm.pcount()] = 0;
  f_status = meas_status_busy;
}
// ��������� ���������
void mx_agilent_6675a_t::on()
{
  if (f_create_error) return;
  //f_command = com_output_on;
  ::strcpy(reinterpret_cast<char*>(f_gpib_command), "OUTPUT ON");
  f_status = meas_status_busy;
}
// ���������� ���������
void mx_agilent_6675a_t::output_off()
{
  if (f_create_error) return;
  //f_command = com_output_off;
  ::strcpy(reinterpret_cast<char*>(f_gpib_command), "OUTPUT OFF");
  f_status = meas_status_busy;
}
void mx_agilent_6675a_t::off()
{
  if (f_create_error) return;
  //f_command = com_output_off;
  ::strcpy(reinterpret_cast<char*>(f_gpib_command), "OUTPUT OFF");
  f_status = meas_status_busy;
}
// ������ ������� ������� ��������
meas_status_t mx_agilent_6675a_t::status()
{
  if (f_create_error) return meas_status_busy;
  return f_status;
}
// ���������� ������� ��������
void mx_agilent_6675a_t::abort()
{
  if (f_create_error) return;
  f_abort_request = irs_true;
}

// ������������ ��������
void mx_agilent_6675a_t::tick()
{
  if (f_create_error) return;
  mxifa_tick();
  switch (f_mode) {
    case mode_start: {
      if (f_gpib_command[0]) {
        f_mode = mode_commands;
      } else {
        f_abort_request = irs_false;
      }
    } break;
    case mode_commands: {
      irs_u32 len = strlen(reinterpret_cast<char*>(f_gpib_command));
      mxifa_write_begin(f_handle, IRS_NULL, f_gpib_command, len);
      f_mode = mode_commands_wait;
    } break;
    case mode_commands_wait: {
      if (mxifa_write_end(f_handle, irs_false)) {
        f_mode = mode_stop;
      } else if (f_abort_request) {
        f_abort_request = irs_false;
        mxifa_write_end(f_handle, irs_true);
        f_mode = mode_stop;
      }
    } break;
    case mode_stop: {
      f_gpib_command[0] = 0;
      f_status = meas_status_success;
      f_mode = mode_start;
    } break;
  }
}


// ����� ��� ������ � ���������� ����������� ���� u309m_current_supply_t

// �����������
u309m_current_supply_t::u309m_current_supply_t(irs::hardflow_t *ap_hardflow):
  m_mode(mode_free),
  m_supply_number(m_supply_null),
  m_status(meas_status_success),
  m_eth_data(),
  m_modbus_client(ap_hardflow,
    irs::mxdata_ext_t::mode_refresh_auto, discr_inputs_size_byte,
    coils_size_byte, hold_regs_size, input_regs_size, irs::make_cnt_ms(200),
    3, irs::make_cnt_s(2), 240),
  m_voltage(0),
  m_current(0),
  m_parameter(0),
  m_argument(0),
  m_timer(irs::make_cnt_s(2)),
  m_supply_off(false)
{
  m_eth_data.connect(&m_modbus_client, 0);

  /*m_mode = mode_start;
  m_status =  meas_status_busy;*/
}

// ����������
u309m_current_supply_t::~u309m_current_supply_t()
{
}
// ���������� ��� �������������� �������� ��� ���
irs_bool u309m_current_supply_t::dc_supported()
{
  return irs_true;
}
// ��������� ����
void u309m_current_supply_t::set_current(double a_current)
{
  m_current = static_cast<float>(a_current);
}
// ��������� ����������
void u309m_current_supply_t::set_voltage(double a_voltage)
{
  m_voltage = static_cast<float>(a_voltage);
}
// ��������� ���������
void u309m_current_supply_t::on()
{
  m_mode = mode_supply_on;
  m_status =  meas_status_busy;
}
// ���������� ���������
void u309m_current_supply_t::output_off()
{
  m_mode = mode_supply_output_off;
  m_status =  meas_status_busy;
}

void u309m_current_supply_t::off()
{
  m_mode = mode_off_value_off;
  m_status =  meas_status_busy;
}
// ������ ������� ������� ��������
meas_status_t u309m_current_supply_t::status()
{
  return m_status;
}
// ���������� ������� ��������
void u309m_current_supply_t::abort()
{
  m_mode = mode_supply_output_off;
  m_status =  meas_status_busy;
}
irs::mxdata_t* u309m_current_supply_t::get_supply_data()
{
  return &m_modbus_client;
}
// ������������ ��������
void u309m_current_supply_t::tick()
{
  m_modbus_client.tick();

  switch (m_mode) {

    case mode_free: {

    } break;

    case mode_start: {
      m_eth_data.header_data.ground_rele_bit = 1;
      m_mode = mode_start_wait;

    } break;
    case mode_start_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_timer.start();
        m_mode = mode_start_value;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_start_value: {
      if (m_timer.check()) {
        m_timer.stop();
        m_argument = 0;

        m_eth_data.supply_200V.sense_regA = m_argument;
        m_eth_data.supply_20V.sense_regA = m_argument;

        m_eth_data.supply_1A.sense_regA = m_argument;
        m_eth_data.supply_1A.sense_regB = m_argument;

        m_eth_data.supply_17A.sense_regA = m_argument;
        m_eth_data.supply_17A.sense_regB = m_argument;

        m_mode = mode_start_value_wait;
      }
    } break;
    case mode_start_value_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_start_supply;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_start_supply: {
      m_supply_number = m_supply_null;
      m_eth_data.header_data.supply_number = m_supply_number;

      m_mode = mode_start_supply_wait;
      m_timer.start();
    } break;
    case mode_start_supply_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on: {
      bool is_supply_200V = (21 < m_voltage) && (m_voltage < 210) &&
        (m_current < 0.01);
      bool is_supply_20V = (1.9 < m_voltage) && (m_voltage <= 21) &&
        (0.01 <= m_current) && (m_current < 0.25);
      bool is_supply_1A = (0.4 < m_voltage) && (m_voltage <= 1.5) &&
        (0.25 <= m_current) && (m_current < 1.1);
      /*bool is_supply_20V = (1.9 < m_voltage) && (m_voltage <= 21) &&
        (0.01 <= m_current) && (m_current < 0.05);
      bool is_supply_1A = (0.4 < m_voltage) && (m_voltage <= 2.5) &&
        (0.01 <= m_current) && (m_current < 1.1);*/
      bool is_supply_17A = (0.7 < m_voltage) && (m_voltage <= 2.5) &&
        (1.1 <= m_current) && (m_current < 17.5);

      if (is_supply_200V) {
        m_supply_number = m_supply_200V;
        m_parameter = m_voltage;
        //m_eth_data.header_data.SR_supply_200V_rele_bit = 1;//��������� ��� �����
      } else if (is_supply_20V) {
        m_supply_number = m_supply_20V;
        m_parameter = m_voltage;
      } else if (is_supply_1A) {
        m_supply_number = m_supply_1A;
        m_parameter = m_current;
      } else if (is_supply_17A) {
        m_supply_number = m_supply_17A;
        m_parameter = m_current;
      } else {
        m_supply_number = m_supply_null;
      }
      if (is_supply_1A || is_supply_17A) {
        m_eth_data.header_data.supply_number = m_supply_number;
      }
      m_mode = mode_supply_on_wait;
      m_supply_off = false;
    } break;
    case mode_supply_on_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_timer.start();
        m_mode = mode_supply_on_ground_rele_off;
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on_ground_rele_off: {
      if (m_timer.check()) {
        m_timer.stop();
        m_eth_data.header_data.ground_rele_bit = 0;
        m_mode = mode_supply_on_ground_rele_off_wait;
        m_status = meas_status_busy;
      }
    } break;

    case mode_supply_on_ground_rele_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_timer.start();
        if (m_supply_number == m_supply_20V || m_supply_number == m_supply_200V)
        {
          m_mode = mode_supply_on_ground_rele_off_voltage;
        } else {
          m_mode = mode_supply_on_value;
        }
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on_ground_rele_off_voltage: {
      if (m_timer.check())
      {
        m_timer.stop();
        m_eth_data.header_data.supply_number = m_supply_number;
        m_mode = mode_supply_on_ground_rele_off_voltage_wait;
        m_status = meas_status_busy;
      }
    } break;

    case mode_supply_on_ground_rele_off_voltage_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_timer.start();
        m_mode = mode_supply_on_value;
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;


    case mode_supply_on_value: {
      if (m_timer.check())
      {
        m_timer.stop();
        MEASSUP_ASSERT(m_eth_data.header_data.supply_number == m_supply_number);
        switch (m_supply_number) {
          case m_supply_200V: {
            m_argument = m_parameter;
            m_eth_data.supply_200V.sense_regA = m_argument;
          } break;
          case m_supply_20V: {
            m_argument = m_parameter;
            m_eth_data.supply_20V.sense_regA = m_argument;
          } break;
          case m_supply_1A: {
            m_argument = m_parameter;
            m_eth_data.supply_1A.sense_regA = m_argument;
            m_eth_data.supply_1A.sense_regB = 65000; //44600;
          } break;
          case m_supply_17A: {
            m_argument = m_parameter;
            m_eth_data.supply_17A.sense_regA = m_argument;
            m_eth_data.supply_17A.sense_regB = 50590;
          } break;
          default: {
            m_argument = 0;
          } break;
        };
        m_mode = mode_supply_on_value_wait;
        m_status = meas_status_busy;
      }
    } break;
    case mode_supply_on_value_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_output_off: {
      switch (m_supply_number) {
        case m_supply_200V: {
          if (!m_supply_off) {
            //m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        case m_supply_20V: {
          if (!m_supply_off) {
            //m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        case m_supply_1A: {
          if (!m_supply_off) {
            m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        case m_supply_17A: {
          if (!m_supply_off) {
            m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        default: {

        } break;
      }
      m_mode = mode_supply_output_off_wait;
      m_status =  meas_status_busy;
      m_timer.start();
    } break;
    case mode_supply_output_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        if (m_supply_off) {
          m_mode = mode_ground_rele_on;
          m_supply_off = false;
          m_status =  meas_status_busy;
          m_timer.start();
        } else {
          m_mode = mode_free;
          m_status = meas_status_success;
        }
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_ground_rele_on: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_eth_data.header_data.ground_rele_bit = 1;
        m_mode = mode_ground_rele_on_wait;
        m_status =  meas_status_busy;
        m_timer.start();
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;
    case mode_ground_rele_on_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_off_value_off: {
      m_argument = 0;

      m_eth_data.supply_200V.sense_regA = m_argument;
      m_eth_data.supply_20V.sense_regA = m_argument;

      m_eth_data.supply_1A.sense_regA = m_argument;
      m_eth_data.supply_1A.sense_regB = m_argument;

      m_eth_data.supply_17A.sense_regA = m_argument;
      m_eth_data.supply_17A.sense_regB = m_argument;

      //m_eth_data.header_data.SR_supply_200V_rele_bit = 0;//��������� ��� �����

      m_mode = mode_off_value_off_wait;
      m_status = meas_status_busy;
      m_supply_off = true;
      m_timer.start();
    } break;
    case mode_off_value_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_mode = mode_supply_output_off;
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;
  }
}

u309m_arm_current_supply_t::u309m_arm_current_supply_t
  (irs::hardflow_t *ap_hardflow):
  m_mode(mode_free),
  m_supply_number(m_supply_null),
  m_status(meas_status_success),
  m_eth_data(),
  m_modbus_client(ap_hardflow,
    irs::mxdata_ext_t::mode_refresh_auto, discr_inputs_size_byte,
    coils_size_byte, hold_regs_size, input_regs_size, irs::make_cnt_ms(200),
    3, irs::make_cnt_s(2), 240),
  m_voltage(0),
  m_current(0),
  m_parameter(0),
  m_argument(0),
  m_timer(irs::make_cnt_s(2)),
  m_supply_off(false)
{
  m_eth_data.connect(&m_modbus_client, 0);

  /*m_mode = mode_start;
  m_status =  meas_status_busy;*/
}

// ����������
u309m_arm_current_supply_t::~u309m_arm_current_supply_t()
{
}
// ���������� ��� �������������� �������� ��� ���
irs_bool u309m_arm_current_supply_t::dc_supported()
{
  return irs_true;
}
// ��������� ����
void u309m_arm_current_supply_t::set_current(double a_current)
{
  m_current = static_cast<float>(a_current);
}
// ��������� ����������
void u309m_arm_current_supply_t::set_voltage(double a_voltage)
{
  m_voltage = static_cast<float>(a_voltage);
}
// ��������� ���������
void u309m_arm_current_supply_t::on()
{
  m_mode = mode_supply_on;
  m_status =  meas_status_busy;
}
// ���������� ���������
void u309m_arm_current_supply_t::output_off()
{
  m_mode = mode_supply_output_off;
  m_status =  meas_status_busy;
}

void u309m_arm_current_supply_t::off()
{
  m_mode = mode_off_value_off;
  m_status =  meas_status_busy;
}
// ������ ������� ������� ��������
meas_status_t u309m_arm_current_supply_t::status()
{
  return m_status;
}
// ���������� ������� ��������
void u309m_arm_current_supply_t::abort()
{
  m_mode = mode_supply_output_off;
  m_status =  meas_status_busy;
}
irs::mxdata_t* u309m_arm_current_supply_t::get_supply_data()
{
  return &m_modbus_client;
}
// ������������ ��������
void u309m_arm_current_supply_t::tick()
{
  m_modbus_client.tick();

  switch (m_mode) {

    case mode_free: {

    } break;

    case mode_start: {
      m_eth_data.header_data.ground_rele_bit = 1;
      m_mode = mode_start_wait;

    } break;
    case mode_start_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_timer.start();
        m_mode = mode_start_value;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_start_value: {
      if (m_timer.check()) {
        m_timer.stop();
        m_argument = 0;

        m_eth_data.supply_200V.sense_regA = m_argument;
        m_eth_data.supply_20V.sense_regA = m_argument;

        m_eth_data.supply_1A.sense_regA = m_argument;
        m_eth_data.supply_1A.sense_regB = m_argument;

        m_eth_data.supply_17A.sense_regA = m_argument;
        m_eth_data.supply_17A.sense_regB = m_argument;

        m_mode = mode_start_value_wait;
      }
    } break;
    case mode_start_value_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_start_supply;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_start_supply: {
      m_supply_number = m_supply_null;
      m_eth_data.header_data.supply_number = m_supply_number;

      m_mode = mode_start_supply_wait;
      m_timer.start();
    } break;
    case mode_start_supply_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on: {
      bool is_supply_200V = (21 < m_voltage) && (m_voltage < 210) &&
        (m_current < 0.01);
      /*bool is_supply_20V = (1.9 < m_voltage) && (m_voltage <= 21) &&
        (0.01 <= m_current) && (m_current < 0.25);
      bool is_supply_1A = (0.4 < m_voltage) && (m_voltage <= 1.5) &&
        (0.25 <= m_current) && (m_current < 1.1);*/
      bool is_supply_20V = (1.9 < m_voltage) && (m_voltage <= 21) &&
        (0.01 <= m_current) && (m_current < 0.05);
      bool is_supply_1A = (0.4 < m_voltage) && (m_voltage <= 2.5) &&
        (0.01 <= m_current) && (m_current < 1.1);
      bool is_supply_17A = (0.7 < m_voltage) && (m_voltage <= 2.5) &&
        (1.1 <= m_current) && (m_current < 17.5);

      if (is_supply_200V) {
        m_supply_number = m_supply_200V;
        m_parameter = m_voltage;
      } else if (is_supply_20V) {
        m_supply_number = m_supply_20V;
        m_parameter = m_voltage;
      } else if (is_supply_1A) {
        m_supply_number = m_supply_1A;
        m_parameter = m_current;
      } else if (is_supply_17A) {
        m_supply_number = m_supply_17A;
        m_parameter = m_current;
      } else {
        m_supply_number = m_supply_null;
      }
      if (is_supply_1A || is_supply_17A) {
        m_eth_data.header_data.supply_number = m_supply_number;
      }
      m_mode = mode_supply_on_wait;
      m_supply_off = false;
    } break;
    case mode_supply_on_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_timer.start();
        m_mode = mode_supply_on_ground_rele_off;
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on_ground_rele_off: {
      if (m_timer.check()) {
        m_timer.stop();
        m_eth_data.header_data.ground_rele_bit = 0;
        m_mode = mode_supply_on_ground_rele_off_wait;
        m_status = meas_status_busy;
      }
    } break;

    case mode_supply_on_ground_rele_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_timer.start();
        if (m_supply_number == m_supply_20V || m_supply_number == m_supply_200V)
        {
          m_mode = mode_supply_on_ground_rele_off_voltage;
        } else {
          m_mode = mode_supply_on_value;
        }
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on_ground_rele_off_voltage: {
      if (m_timer.check())
      {
        m_timer.stop();
        m_eth_data.header_data.supply_number = m_supply_number;
        m_mode = mode_supply_on_ground_rele_off_voltage_wait;
        m_status = meas_status_busy;
      }
    } break;

    case mode_supply_on_ground_rele_off_voltage_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_timer.start();
        m_mode = mode_supply_on_value;
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;


    case mode_supply_on_value: {
      if (m_timer.check())
      {
        m_timer.stop();
        MEASSUP_ASSERT(m_eth_data.header_data.supply_number == m_supply_number);
        switch (m_supply_number) {
          case m_supply_200V: {
            m_argument = m_parameter;
            m_eth_data.supply_200V.sense_regA = m_argument;
          } break;
          case m_supply_20V: {
            m_argument = m_parameter;
            m_eth_data.supply_20V.sense_regA = m_argument;
          } break;
          case m_supply_1A: {
            m_argument = m_parameter;
            m_eth_data.supply_1A.sense_regA = m_argument;
            m_eth_data.supply_1A.sense_regB = 65000; //44600;
          } break;
          case m_supply_17A: {
            m_argument = m_parameter;
            m_eth_data.supply_17A.sense_regA = m_argument;
            m_eth_data.supply_17A.sense_regB = 50590;
          } break;
          default: {
            m_argument = 0;
          } break;
        }
        m_mode = mode_supply_on_value_wait;
        m_status = meas_status_busy;
      }
    } break;
    case mode_supply_on_value_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_output_off: {
      switch (m_supply_number) {
        case m_supply_200V: {
          if (!m_supply_off) {
            //m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        case m_supply_20V: {
          if (!m_supply_off) {
            //m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        case m_supply_1A: {
          if (!m_supply_off) {
            m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        case m_supply_17A: {
          if (!m_supply_off) {
            m_eth_data.header_data.ground_rele_bit = 1;
          } else {
            m_eth_data.header_data.supply_number = m_supply_null;
            //m_supply_off = false;
          }
        } break;
        default: {

        } break;
      }
      m_mode = mode_supply_output_off_wait;
      m_status =  meas_status_busy;
      m_timer.start();
    } break;
    case mode_supply_output_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        if (m_supply_off) {
          m_mode = mode_ground_rele_on;
          m_supply_off = false;
          m_status =  meas_status_busy;
          m_timer.start();
        } else {
          m_mode = mode_free;
          m_status = meas_status_success;
        }
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_ground_rele_on: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_eth_data.header_data.ground_rele_bit = 1;
        m_mode = mode_ground_rele_on_wait;
        m_status =  meas_status_busy;
        m_timer.start();
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;
    case mode_ground_rele_on_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;

    case mode_off_value_off: {
      m_argument = 0;

      m_eth_data.supply_200V.sense_regA = m_argument;
      m_eth_data.supply_20V.sense_regA = m_argument;

      m_eth_data.supply_1A.sense_regA = m_argument;
      m_eth_data.supply_1A.sense_regB = m_argument;

      m_eth_data.supply_17A.sense_regA = m_argument;
      m_eth_data.supply_17A.sense_regB = m_argument;

      m_mode = mode_off_value_off_wait;
      m_status = meas_status_busy;
      m_supply_off = true;
      m_timer.start();
    } break;
    case mode_off_value_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed &&
        m_timer.check())
      {
        m_timer.stop();
        m_mode = mode_supply_output_off;
        m_status = meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error) {
        m_status = meas_status_error;
      }
    } break;
  }  
}
