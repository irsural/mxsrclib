// Классы для работы с источниками тока
// Для Borland C++ Builder
// Версия 0.1

#ifndef meassupH
#define meassupH

// Версия модуля meassup
#define MESSUP_VERSION 0x0001

#include <irsdefs.h>

#include <measdef.h>
#include <mxifa.h>

#include <irsfinal.h>
#include <mxdatastd.h>

// Размер буфера комманд GPIB
#define supag_gpib_com_buf_size 30
// Число цифр для irs_gcvt
#define supag_ndig 7

// Абстрактный базовый класс для работы с источниками тока
class mxsupply_t
{
public:
  // Постоянный ток поддерживается прибором или нет
  virtual irs_bool dc_supported() = 0;
  // Установка тока
  virtual void set_current(double current) = 0;
  // Установка напряжения
  virtual void set_voltage(double voltage) = 0;
  // Включение источника
  virtual void on() = 0;
  // Выключение источника
  virtual void off() = 0;
  // Чтение статуса текущей операции
  virtual meas_status_t status() = 0;
  // Прерывание текущей операции
  virtual void abort() = 0;
  // Элементарное действие
  virtual void tick() = 0;
};

class mx_agilent_6675a_t: public mxsupply_t
{
  // Тип для текущего режима
  typedef enum _mode_t {
    mode_start,
    mode_commands,
    mode_commands_wait,
    mode_stop
  } mode_t;
  // Тип для текущей команды
  typedef enum _command_t {
    com_free,
    com_set_voltage,
    com_set_current,
    com_output_on,
    com_output_off
  } command_t;

  // Команды при инициализации
  static irs_u8 *f_init_commands[];
  // Число команд инициализации
  static const irs_i32 f_ic_count;

  // Дескриптор канала mxifa
  void *f_handle;
  // Ощибка создания
  irs_bool f_create_error;
  // Текущий режим работы
  mode_t f_mode;
  // Статус текущей операции
  meas_status_t f_status;
  // Текущая команда
  //command_t f_command;
  // Запрос на прерывание операции
  irs_bool f_abort_request;
  // Буфер приема
  //irs_u8 f_read_buf[ma_read_buf_size];
  // Текущая позиция в буфере приема
  //index_t f_read_pos;
  // Ток заданный пользователем
  //double f_current;
  // Напряжение заданное пользователем
  //double f_voltage;

  // Команда GPIB
  irs_u8 f_gpib_command[supag_gpib_com_buf_size];
public:
  // Конструктор
  mx_agilent_6675a_t(mxifa_ch_t channel,
    gpib_addr_t address);
  // Деструктор
  ~mx_agilent_6675a_t();
  // Постоянный ток поддерживается прибором или нет
  virtual irs_bool dc_supported();
  // Установка тока
  virtual void set_current(double current);
  // Установка напряжения
  virtual void set_voltage(double voltage);
  // Включение источника
  virtual void on();
  // Выключение источника
  virtual void off();
  // Чтение статуса текущей операции
  virtual meas_status_t status();
  // Прерывание текущей операции
  virtual void abort();
  // Элементарное действие
  virtual void tick();
};

class mx_cs_stab_t: public mxsupply_t
{
public:
  // Конструктор
  //mx_cs_stab_t(irs::hardflow_t* ap_hardflow);
  mx_cs_stab_t();
  // Деструктор
  ~mx_cs_stab_t();
  // Постоянный ток поддерживается прибором или нет
  virtual irs_bool dc_supported();
  // Установка тока
  virtual void set_current(double current);
  // Установка напряжения
  virtual void set_voltage(double voltage);
  // Включение источника
  virtual void on();
  // Выключение источника
  virtual void off();
  // Чтение статуса текущей операции
  virtual meas_status_t status();
  // Прерывание текущей операции
  virtual void abort();
  // Элементарное действие
  virtual void tick();

private:
  // Тип для текущего режима
  typedef enum _supply_number_t {
    m_supply_null,
    m_supply_200V,
    m_supply_20V,
    m_supply_1A,
    m_supply_17A
  } supply_number_t;

  enum {
    discr_inputs_size_byte = 0,  //bit = discr_inputs_size_byte*8
    coils_size_byte = 6,         //bit = coils_size_byte*8
    hold_regs_size = 112,        //16-bit word
    input_regs_size = 0,         //16-bit word
    sum_size_byte = discr_inputs_size_byte + coils_size_byte +
      hold_regs_size*2 + input_regs_size*2
  };

  // Текущий режим работы
  supply_number_t m_supply_number;

  // Статус текущей операции
  meas_status_t m_status;

  //irs::mxdata_ext_t* mp_client;

  eth_data_t m_eth_data;

  irs::hardflow::udp_flow_t hardflow;

  irs::modbus_client_t m_modbus_client;


  double m_voltage;
  double m_current;
  //Стабилизируемый параметр
  double m_parameter;
  double m_argument;
};

#endif // meassupH
