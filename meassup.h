// Классы для работы с источниками тока
// Дата: 05.08.2010
// Ранняя дата: 15.01.2008

#ifndef meassupH
#define meassupH

// Версия модуля meassup
#define MESSUP_VERSION 0x0001

#include <irsdefs.h>

#include <measdef.h>
#include <mxifa.h>

#include <irsfinal.h>
#include <mxdatastd.h>
#include <irsmbus.h>
#include <mxdata.h>

// Размер буфера комманд GPIB
#define supag_gpib_com_buf_size 30
// Число цифр для irs_gcvt
#define supag_ndig 7

// Абстрактный базовый класс для работы с источниками тока
class mxsupply_t
{
public:
  virtual ~mxsupply_t() {}
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
  // Замыкание выхода на землю
  virtual void ground_rele() = 0;

  virtual void output_off() = 0;

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
  irs::mxdata_ext_t::status_t m_status;
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
  virtual ~mx_agilent_6675a_t();
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
  // Замыкание выхода на землю
  virtual void ground_rele();

  virtual void output_off();

};

struct header_conn_data_t
{
  irs::conn_data_t<irs_u8> supply_number;
  // установка реле земли:
  irs::bit_data_t ground_rele_bit;
  // установка реле питания:
  irs::bit_data_t power_rele_bit;
  irs::bit_data_t SR_supply_200V_rele_bit;
  irs::bit_data_t SR_supply_20V_rele_bit;
  irs::bit_data_t bit4;
  irs::bit_data_t bit5;
  irs::bit_data_t bit6;
  irs::bit_data_t bit7;
  irs::conn_data_t<irs_u8> rele_data;
  irs::bit_data_t ADC_200V_overflow; 
  irs::bit_data_t ADC_20V_overflow;
  irs::bit_data_t ADC_1A_overflow;
  irs::bit_data_t ADC_17A_overflow;
  irs::bit_data_t bit_adc_4;
  irs::bit_data_t bit_adc_5;
  irs::bit_data_t bit_adc_6;
  irs::bit_data_t bit_adc_7;
  irs::conn_data_t<irs_u8> ADC_supply_overflow_bits;
  irs::bit_data_t reset_supply_overflow_bit;
  irs::bit_data_t reset_to_default_bit;
  irs::bit_data_t measure_overflow_guard_bit;
  irs::bit_data_t bit_command_3;
  irs::bit_data_t bit_command_4;
  irs::bit_data_t bit_command_5;
  irs::bit_data_t bit_command_6;
  irs::bit_data_t bit_command_7;
  irs::conn_data_t<irs_u8> command_bits;
  irs::conn_data_t<irs_i32> counter;
  
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
    
    index = supply_number.connect(ap_data, index);
    ground_rele_bit.connect(ap_data, index, 0);
    power_rele_bit.connect(ap_data, index, 1);
    SR_supply_200V_rele_bit.connect(ap_data, index, 2);
    SR_supply_20V_rele_bit.connect(ap_data, index, 3);
    bit4.connect(ap_data, index, 4);
    bit5.connect(ap_data, index, 5);
    bit6.connect(ap_data, index, 6);
    bit7.connect(ap_data, index, 7);
    index = rele_data.connect(ap_data, index);
    ADC_200V_overflow.connect(ap_data, index, 0);
    ADC_20V_overflow.connect(ap_data, index, 1);
    ADC_1A_overflow.connect(ap_data, index, 2);
    ADC_17A_overflow.connect(ap_data, index, 3);
    bit_adc_4.connect(ap_data, index, 4);
    bit_adc_5.connect(ap_data, index, 5);
    bit_adc_6.connect(ap_data, index, 6);
    bit_adc_7.connect(ap_data, index, 7);
    index = ADC_supply_overflow_bits.connect(ap_data, index);
    reset_supply_overflow_bit.connect(ap_data, index, 0);
    reset_to_default_bit.connect(ap_data, index, 1);
    measure_overflow_guard_bit.connect(ap_data, index, 2);
    bit_command_3.connect(ap_data, index, 3);
    bit_command_4.connect(ap_data, index, 4);
    bit_command_5.connect(ap_data, index, 5);
    bit_command_6.connect(ap_data, index, 6);
    bit_command_7.connect(ap_data, index, 7);
    index = command_bits.connect(ap_data, index);
    index = counter.connect(ap_data, index);

    return index;
  }
};


struct supply_conn_data_t
{
  enum supply_type_t{
    supply_200V,
    supply_20V,
    supply_1A,
    supply_17A
  };
  
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
  // коэффициент передачи пропорциональной части изодромного звена
  irs::conn_data_t<float> temp_prop_koef;
  irs::conn_data_t<float> temp_k_stable;
  irs::conn_data_t<float> temp_ki_stable;
  irs::conn_data_t<float> temp_kd_stable;
  irs::conn_data_t<float> stable_time_sec;
  
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
    if (ap_size != IRS_NULL) {
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
    index = supply_200V.temp_prop_koef.connect(ap_data, index);
    index = supply_20V.temp_prop_koef.connect(ap_data, index);
    index = supply_1A.temp_prop_koef.connect(ap_data, index);
    index = supply_17A.temp_prop_koef.connect(ap_data, index);
    index = supply_200V.temp_k_stable.connect(ap_data, index);
    index = supply_200V.temp_ki_stable.connect(ap_data, index);
    index = supply_200V.temp_kd_stable.connect(ap_data, index);
    index = supply_20V.temp_k_stable.connect(ap_data, index);
    index = supply_20V.temp_ki_stable.connect(ap_data, index);
    index = supply_20V.temp_kd_stable.connect(ap_data, index);
    index = supply_1A.temp_k_stable.connect(ap_data, index);
    index = supply_1A.temp_ki_stable.connect(ap_data, index);
    index = supply_1A.temp_kd_stable.connect(ap_data, index);
    index = supply_17A.temp_k_stable.connect(ap_data, index);
    index = supply_17A.temp_ki_stable.connect(ap_data, index);
    index = supply_17A.temp_kd_stable.connect(ap_data, index);
    index = supply_200V.stable_time_sec.connect(ap_data, index);
    index = supply_20V.stable_time_sec.connect(ap_data, index);
    index = supply_1A.stable_time_sec.connect(ap_data, index);
    index = supply_17A.stable_time_sec.connect(ap_data, index);
    return index;
  }
};

class u309m_current_supply_t: public mxsupply_t
{
public:
  // Конструктор
  u309m_current_supply_t(irs::hardflow_t* ap_hardflow);
  // Деструктор
  virtual ~u309m_current_supply_t();
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
  // Замыкание выхода на землю
  virtual void ground_rele();

  virtual void output_off();



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
    coils_size_byte = 4,         //bit = coils_size_byte*8
    hold_regs_size = 154,        //16-bit word
    input_regs_size = 0,         //16-bit word
    sum_size_byte = discr_inputs_size_byte + coils_size_byte +
      hold_regs_size*2 + input_regs_size*2
  };

  //Режимы работы
  typedef enum _mode_t {
    mode_free,

    mode_start,
    mode_start_wait,

    mode_start_value,
    mode_start_value_wait,

    mode_start_supply,
    mode_start_supply_wait,

    mode_supply_on_ground_rele_off,
    mode_supply_on_ground_rele_off_wait,

    mode_supply_on,
    mode_supply_on_wait,

    mode_supply_on_value,
    mode_supply_on_value_wait,

    mode_supply_output_off,
    mode_supply_output_off_wait,

    mode_ground_rele_on,
    mode_ground_rele_on_wait,

    mode_value_off,
    mode_value_off_wait
  } mode_t;

  mode_t m_mode;

  // Текущий режим работы
  supply_number_t m_supply_number;

  // Статус текущей операции
  meas_status_t m_status;

  irs::mxdata_ext_t::status_t m_status_modbus;

  eth_data_t m_eth_data;

  irs::modbus_client_t m_modbus_client;


  float m_voltage;
  float m_current;
  //Стабилизируемый параметр
  float m_parameter;
  float m_argument;
};

class dummy_supply_t: public mxsupply_t
{
public:
  // Постоянный ток поддерживается прибором или нет
  virtual irs_bool dc_supported() {return irs_true;}
  // Установка тока
  virtual void set_current(double current) {}
  // Установка напряжения
  virtual void set_voltage(double voltage) {}
  // Включение источника
  virtual void on() {}
  // Выключение источника
  virtual void off() {}
  // Чтение статуса текущей операции
  virtual meas_status_t status() {return meas_status_success;}
  // Прерывание текущей операции
  virtual void abort() {}
  // Элементарное действие
  virtual void tick() {}
  // Замыкание выхода на землю
  virtual void ground_rele() {}

  virtual void output_off() {}

};

#endif // meassupH

