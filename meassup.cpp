// Классы для работы с источниками тока
// Дата: 1.12.2008

//#include <vcl.h>
//#pragma hdrstop

#include <irsstd.h>
#include <timer.h>
#include <string.h>
#include <meassup.h>
#include <stdio.h>
#include <irscpp.h>
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
//#pragma package(smart_init)
