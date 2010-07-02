// Классы для работы с источниками тока
// Для Borland C++ Builder
// Версия 0.1

#define UDP_ENABLED

#ifndef meassupH
#define meassupH

// Версия модуля meassup
#define MESSUP_VERSION 0x0001

#include <irsdefs.h>

#include <measdef.h>
#include <mxifa.h>

#include <hardflowg.h>
#include <irsnetdefs.h>
#include <irsmbus.h>


#include <irsfinal.h>
#include <irsdefs.h>

// Размер буфера комманд GPIB
#define supag_gpib_com_buf_size 30
// Число цифр для irs_gcvt
#define supag_ndig 7

struct header_conn_data_t
{
  // счетчик (read only)
  irs::conn_data_t<irs_i32> counter;
  irs::conn_data_t<irs_u8> supply_number;
  // установка реле земли:
  irs::bit_data_t ground_rele_bit;
  // установка реле питания:
  irs::bit_data_t power_rele_bit;
  irs::bit_data_t SR_supply_200V;
  irs::bit_data_t SR_supply_20V;
  irs::bit_data_t bit4;
  irs::bit_data_t bit5;
  irs::bit_data_t bit6;
  irs::bit_data_t bit7;
  irs::conn_data_t<irs_u8> rele_data;

  header_conn_data_t(irs::mxdata_t *ap_data = IRS_NULL, irs_uarc a_index = 0,
    irs_uarc* ap_size = IRS_NULL)
  {
    irs_uarc size = connect(ap_data, a_index);
    if(ap_size != IRS_NULL){
      *ap_size = size;
    }
  }
  
  irs_uarc connect(irs::mxdata_t *ap_data, irs_uarc a_index)
  {
    irs_uarc index = a_index;
    
    index = counter.connect(ap_data, index);
    index = supply_number.connect(ap_data, index);
    ground_rele_bit.connect(ap_data, index, 0);
    power_rele_bit.connect(ap_data, index, 1);
    SR_supply_200V.connect(ap_data, index, 2);
    SR_supply_20V.connect(ap_data, index, 3);
    bit4.connect(ap_data, index, 4);
    bit5.connect(ap_data, index, 5);
    bit6.connect(ap_data, index, 6);
    bit7.connect(ap_data, index, 7);
    index = rele_data.connect(ap_data, index);
    
    return index;
  }
};

struct supply_conn_data_t
{
  irs::conn_data_t<float> temperature;
  irs::conn_data_t<float> ADC_output;
  irs::conn_data_t<float> temperature_ref;
  irs::conn_data_t<float> correct_koef;
  irs::conn_data_t<float> ADC_correct_koef;
  // пропорциональный коэффициент регулятора температуры (write only)
  irs::conn_data_t<float> temp_k;
  // интегральный коэффициент регулятора температуры (write only) 
  irs::conn_data_t<float> temp_ki;
  // дифференциальный коэффициент регулятора температуры (write only)
  irs::conn_data_t<float> temp_kd;
  // значение записываемое в регистр А (write only)
  irs::conn_data_t<float> sense_regA;
  // значение записываемое в регистр B (write only)
  irs::conn_data_t<float> sense_regB;
  // значение цифрового потенциометра (read only)
  irs::conn_data_t<irs_i32> temp_dac_value;
  // значение интегратора (read only)
  irs::conn_data_t<irs_i32> int_val;
  // постоянная времени фильтра температуры (read only)
  irs::conn_data_t<float> temp_time_const;
  // фильтрованная температура (read only)
  irs::conn_data_t<float> temp_filtered;

  supply_conn_data_t(irs::mxdata_t *ap_data = IRS_NULL, irs_uarc a_index = 0,
    irs_uarc* ap_size = IRS_NULL)
  {
    irs_uarc size = connect(ap_data, a_index);
    if(ap_size != IRS_NULL){
      *ap_size = size;
    }
  }
  
  irs_uarc connect(irs::mxdata_t *ap_data, irs_uarc a_index)
  {
    irs_uarc index = a_index;
    
    index = temperature.connect(ap_data, index);
    index = ADC_output.connect(ap_data, index);
    index = temperature_ref.connect(ap_data, index);
    index = correct_koef.connect(ap_data, index);
    index = ADC_correct_koef.connect(ap_data, index);
    index = temp_k.connect(ap_data, index);
    index = temp_ki.connect(ap_data, index);
    index = temp_kd.connect(ap_data, index);
    index = sense_regA.connect(ap_data, index);
    index = sense_regB.connect(ap_data, index);
    index = temp_dac_value.connect(ap_data, index);
    index = int_val.connect(ap_data, index);
    index = temp_time_const.connect(ap_data, index);
    index = temp_filtered.connect(ap_data, index);

    return index;
  }
};

struct eth_data_t
{
  header_conn_data_t header_data;
  supply_conn_data_t supply_200V;
  supply_conn_data_t supply_20V;
  supply_conn_data_t supply_1A;
  supply_conn_data_t supply_17A;

  eth_data_t(irs::mxdata_t *ap_data = IRS_NULL, irs_uarc a_index = 0,
    irs_uarc* ap_size = IRS_NULL)
  {
    irs_uarc size = connect(ap_data, a_index);
    if(ap_size != IRS_NULL){
      *ap_size = size;
    }
  }
  irs_uarc connect(irs::mxdata_t *ap_data, irs_uarc a_index)
  {
    irs_uarc index = a_index;

    index = header_data.connect(ap_data, index);
    index = supply_200V.connect(ap_data, index);
    index = supply_20V.connect(ap_data, index);
    index = supply_1A.connect(ap_data, index);
    index = supply_17A.connect(ap_data, index);

    return index;
  }
};


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
  // Тип для текущего режима
  typedef enum _supply_number_t {
    m_supply_null,
    m_supply_200V,
    m_supply_20V,
    m_supply_1A,
    m_supply_17A
  } supply_number_t;

  enum {
  discr_inputs_size_byte = 0,//1,//8,//8192,  //bit = discr_inputs_size_byte*8
  coils_size_byte = 6,//2,//6,//8192,         //bit = coils_size_byte*8
  hold_regs_size = 112,//94,//94,//10,//65536,         //16-bit word
  input_regs_size = 0,//10,//65536,        //16-bit word
  sum_size_byte = discr_inputs_size_byte + coils_size_byte +
    hold_regs_size*2 + input_regs_size*2
  };

  // Текущий режим работы
  supply_number_t m_supply_number;

  // Статус текущей операции
  meas_status_t m_status;
  // Запрос на прерывание операции
  irs_bool f_abort_request;

  irs::modbus_client_t m_modbus_client;
  #ifdef UDP_ENABLED
  irs::hardflow::udp_flow_t hardflow;
  #else
  irs::hardflow::tcp_client_t hardflow;
  #endif


  eth_data_t m_eth_data;

  double m_voltage;
  double m_current;
  //Стабилизируемый параметр
  double m_parameter;
  double m_argument;
  bool SR_set;

public:
  // Конструктор
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
};


#endif
