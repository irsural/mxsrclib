//! \file
//! \ingroup drivers_group
//! \brief Классы для работы с источниками тока
//!
//! Дата: 29.07.2010\n
//! Ранняя дата: 15.01.2008

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
//---------------------------------------------------------------------------
// Класс для работы с источником постоянного тока Agilent 6675A

// Команды при инициализации
irs_u8 *mx_agilent_6675a_t::f_init_commands[] = {
  (irs_u8 *)"PRESET NORM\r\n",
  //(irs_u8 *)"OFORMAT ASCII\r\n",
};
// Число команд инициализации
const irs_i32 mx_agilent_6675a_t::f_ic_count =
  IRS_ARRAYOFSIZE(f_init_commands);

// Конструктор
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

  // Открытие канала MXIFA_SUPPLY
  if (mxifa_get_channel_type_ex(channel) ==
      mxifa_ei_win32_ni_usb_gpib) {
    mxifa_win32_ni_usb_gpib_cfg config;
    config.address = address;
    f_handle = mxifa_open_ex(channel, &config, irs_false);
  } else {
    f_handle = mxifa_open(channel, irs_false);
  }

  // Очистка буфера комманд GPIB
  memset(f_gpib_command, 0, supag_gpib_com_buf_size);

  #ifdef NOP
  {
    // Запись команд инициализации
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

  // Очистка буфера приема
  memset(f_read_buf, 0, ma_read_buf_size);
  return;

  _error:
  f_create_error = irs_true;
  #endif //NOP
}
// Деструктор
mx_agilent_6675a_t::~mx_agilent_6675a_t()
{
  mxifa_close_begin(f_handle);
  mxifa_close_end(f_handle, irs_true);
  mxifa_deinit();
  deinit_to_cnt();
}
// Постоянный ток поддерживается прибором или нет
irs_bool mx_agilent_6675a_t::dc_supported()
{
  return irs_true;
}
// Установка тока
void mx_agilent_6675a_t::set_current(double current)
{
  if (f_create_error) return;
  ostrstream strm((char *)f_gpib_command, supag_gpib_com_buf_size - 1);
  strm << "CURR " << current << '\0';
  //f_gpib_command[strm.pcount()] = 0;
  f_status = meas_status_busy;
}
// Установка напряжения
void mx_agilent_6675a_t::set_voltage(double voltage)
{
  if (f_create_error) return;
  ostrstream strm((char *)f_gpib_command, supag_gpib_com_buf_size - 1);
  strm << "VOLT " << voltage << '\0';
  //f_gpib_command[strm.pcount()] = 0;
  f_status = meas_status_busy;
}
// Включение источника
void mx_agilent_6675a_t::on()
{
  if (f_create_error) return;
  //f_command = com_output_on;
  ::strcpy((char *)f_gpib_command, "OUTPUT ON");
  f_status = meas_status_busy;
}
// Выключение источника
void mx_agilent_6675a_t::output_off()
{
  if (f_create_error) return;
  //f_command = com_output_off;
  ::strcpy((char *)f_gpib_command, "OUTPUT OFF");
  f_status = meas_status_busy;
}
void mx_agilent_6675a_t::off()
{
  if (f_create_error) return;
  //f_command = com_output_off;
  ::strcpy((char *)f_gpib_command, "OUTPUT OFF");
  f_status = meas_status_busy;
}
// Чтение статуса текущей операции
meas_status_t mx_agilent_6675a_t::status()
{
  if (f_create_error) return meas_status_busy;
  return f_status;
}
// Прерывание текущей операции
void mx_agilent_6675a_t::abort()
{
  if (f_create_error) return;
  f_abort_request = irs_true;
}

// Элементарное действие
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
      irs_u32 len = strlen((char *)f_gpib_command);
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
//---------------------------------------------------------------------------


// Класс для работы с источником постоянного тока u309m_current_supply_t

// Конструктор
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
  m_argument(0)
{
  m_eth_data.connect(&m_modbus_client, 0);

  /*m_mode = mode_start;
  m_status =  meas_status_busy;*/
}

// Деструктор
u309m_current_supply_t::~u309m_current_supply_t()
{
}
// Постоянный ток поддерживается прибором или нет
irs_bool u309m_current_supply_t::dc_supported()
{
  return irs_true;
}
// Установка тока
void u309m_current_supply_t::set_current(double a_current)
{
  m_current = static_cast<float>(a_current);
}
// Установка напряжения
void u309m_current_supply_t::set_voltage(double a_voltage)
{
  m_voltage = static_cast<float>(a_voltage);
}
// Включение источника
void u309m_current_supply_t::on()
{
  m_mode = mode_supply_on_ground_rele_off;
  m_status =  meas_status_busy;
}
// Выключение источника
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
// Чтение статуса текущей операции
meas_status_t u309m_current_supply_t::status()
{
  return m_status;
}
// Прерывание текущей операции
void u309m_current_supply_t::abort()
{
  m_mode = mode_supply_output_off;
  m_status =  meas_status_busy;
}
irs::mxdata_t* u309m_current_supply_t::get_supply_data()
{
  return &m_modbus_client;
}
// Элементарное действие
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
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_start_value;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;

    case mode_start_value: {
      m_argument = 0;

      m_eth_data.supply_200V.sense_regA = m_argument;
      m_eth_data.supply_20V.sense_regA = m_argument;

      m_eth_data.supply_1A.sense_regA = m_argument;
      m_eth_data.supply_1A.sense_regB = m_argument;

      m_eth_data.supply_17A.sense_regA = m_argument;
      m_eth_data.supply_17A.sense_regB = m_argument;

      m_mode = mode_start_value_wait;
    } break;
    case mode_start_value_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_start_supply;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;

    case mode_start_supply: {
      m_supply_number = m_supply_null;
      m_eth_data.header_data.supply_number = m_supply_number;

      m_mode = mode_start_supply_wait;
    } break;
    case mode_start_supply_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on_ground_rele_off: {
      m_eth_data.header_data.ground_rele_bit = 0;
      m_mode = mode_supply_on_ground_rele_off_wait;
    } break;

    case mode_supply_on_ground_rele_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_supply_on;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on: {
      bool is_supply_200V = (21 < m_voltage) && (m_voltage < 210)
             && (m_current < 0.01);
      bool is_supply_20V = (1.9 < m_voltage) && (m_voltage <= 21)
             && (0.01 <= m_current) && (m_current < 0.25);
      bool is_supply_1A = (0.4 < m_voltage) && (m_voltage <= 1.5)
             && (0.25 <= m_current) && (m_current < 1.1);
      bool is_supply_17A = (0.7 < m_voltage) && (m_voltage <= 2.5)
             && (1.1 <= m_current) && (m_current < 17);



      if (is_supply_200V) {
        m_supply_number = m_supply_200V;
        m_parameter = m_voltage;
      } else {
        if (is_supply_20V) {
          m_supply_number = m_supply_20V;
          m_parameter = m_voltage;
        } else {
          if (is_supply_1A) {
            m_supply_number = m_supply_1A;
            m_parameter = m_current;
          } else {
            if (is_supply_17A) {
              m_supply_number = m_supply_17A;
              m_parameter = m_current;
            } else {
              m_supply_number = m_supply_null;
            }
          }
        }
      }
      m_eth_data.header_data.supply_number = m_supply_number;
      m_mode = mode_supply_on_wait;
    } break;
    case mode_supply_on_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_supply_on_value;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_on_value: {
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
          m_eth_data.supply_1A.sense_regB = 65000;
        } break;
        case m_supply_17A: {
          m_argument = m_parameter;
          m_eth_data.supply_17A.sense_regA = m_argument;
          m_eth_data.supply_17A.sense_regB = 32000;
        } break;
        default: {
          m_argument = 0;
        } break;
      }
      m_mode = mode_supply_on_value_wait;
    } break;
    case mode_supply_on_value_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;

    case mode_supply_output_off: {
      switch (m_supply_number) {
        case m_supply_200V: {

        } break;
        case m_supply_20V: {

        } break;
        case m_supply_1A: {
          m_argument = 0;
          m_eth_data.supply_1A.sense_regA = m_argument;
          m_eth_data.supply_1A.sense_regB = m_argument;
        } break;
        case m_supply_17A: {
          m_argument = 0;
          m_eth_data.supply_17A.sense_regA = m_argument;
          m_eth_data.supply_17A.sense_regB = m_argument;
        } break;
        default: {
          m_argument = 0;
        } break;
      }
      m_mode = mode_supply_output_off_wait;
      m_status =  meas_status_busy;
    } break;
    case mode_supply_output_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_supply_number = m_supply_null;
        m_eth_data.header_data.supply_number = m_supply_number;
        m_mode = mode_ground_rele_on;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;

    case mode_ground_rele_on: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed) {
        m_eth_data.header_data.ground_rele_bit = 1;
        m_mode = mode_ground_rele_on_wait;
        m_status =  meas_status_busy;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;
    case mode_ground_rele_on_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_mode = mode_free;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
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
    } break;
    case mode_off_value_off_wait: {
      if (m_modbus_client.status() == irs::mxdata_ext_t::status_completed)
      {
        m_mode = mode_supply_output_off;
        m_status = meas_status_success;
      } else if (m_modbus_client.status() == irs::mxdata_ext_t::status_error)
      {
        m_status = meas_status_error;
      }
    } break;
  }  
}



