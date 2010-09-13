// Классы для работы с мультиметрами
// Дата: 09.09.2010

#ifndef measmulH
#define measmulH

// Версия модуля measmul
#define MESMUL_VERSION 0x0001

#include <irsdefs.h>

#include <timer.h>
#include <measdef.h>
#include <mxifa.h>
#include <irsstd.h>
#include <irscpp.h>
#include <irserror.h>
#include <irsstring.h>
#include <irsmbus.h>
#include <irsfilter.h>
#include <irsdsp.h>

#include <irsfinal.h>

#if (MXIFA_VERSION < 0x0004)
#error mxifa version incompatible. The version should be >= 0.4.
#endif //(MXIFA_VERSION < 0x0004)

// Размер буфера чтения
#define ma_read_buf_size 30

enum type_meas_t{
  tm_first = 1,
  tm_volt_dc = tm_first,
  tm_volt_ac = 2,
  tm_current_dc = 3,
  tm_current_ac = 4,
  tm_resistance_2x = 5,
  tm_resistance_4x = 6,
  tm_frequency = 7,
  tm_phase = 8,
  tm_phase_average = 9,
  tm_time_interval = 10,
  tm_time_interval_average = 11,
  tm_last = tm_time_interval_average};

irs::string type_meas_to_str(const type_meas_t a_type_meas);
bool str_to_type_meas(
  const irs::string& a_str_type_meas, type_meas_t& a_type_meas);

enum multimeter_mode_type_t {mul_mode_type_active, mul_mode_type_passive};

enum multimeter_param_t {
  mul_param_source_values,
  mul_param_filtered_values,
  mul_param_standard_deviation,
  mul_param_standard_deviation_relative,
  mul_param_variation
};

// Абстрактный базовый класс для работы с мультиметрами
class mxmultimeter_t
{
public:
  virtual ~mxmultimeter_t() {};
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  // Установить режим измерения постоянного напряжения
  virtual void set_dc() = 0;
  // Установить режим измерения переменного напряжения
  virtual void set_ac() = 0;
  // Установить положителный фронт запуска
  virtual void set_positive() = 0;
  // Установить отрицательный фронт канала
  virtual void set_negative() = 0;
  // Чтение значения при текущем типа измерения
  virtual void get_value(double *value) = 0;
  // Чтение напряжения
  virtual void get_voltage(double *voltage) = 0;
  // Чтения силы тока
  virtual void get_current(double *current) = 0;
  // Чтение сопротивления
  virtual void get_resistance2x(double *resistance) = 0;
  // Чтение сопротивления
  virtual void get_resistance4x(double *resistance) = 0;
  // Чтение частоты
  virtual void get_frequency(double *frequency) = 0;
  // Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average) = 0;
  // Чтение фазового сдвига
  virtual void get_phase(double *phase) = 0;
  // Чтение временного интервала
  virtual void get_time_interval(double *time_interval) = 0;
  // Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval) = 0;

  // Запуск автокалибровки мультиметра
  virtual void auto_calibration() = 0;
  // Чтение статуса текущей операции
  virtual meas_status_t status() = 0;
  // Прерывание текущей операции
  virtual void abort() = 0;
  // Элементарное действие
  virtual void tick() = 0;
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc) = 0;
  // Установка времени интегрирования в c
  virtual void set_aperture(double aperture) = 0;
  // Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth) = 0;
  // Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance) = 0;
  // Устсновка уровня запуска канала
  virtual void set_start_level(double level) = 0;
  // Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range) = 0;
  // Установка автоматического выбора диапазона измерений
  virtual void set_range_auto() = 0;
  // Установка входного сопротивления канала
  //virtual void set_filter(double ) = 0;
};

// Класс для работы с мультиметром Agilent 3458A
class mx_agilent_3458a_t: public mxmultimeter_t
{
  // Тип для текущего режима
  typedef enum _ma_mode_t {
    ma_mode_start,
    ma_mode_macro,
    ma_mode_commands,
    ma_mode_commands_wait,
    ma_mode_get_value,
    ma_mode_auto_calibration,
    ma_mode_auto_calibration_wait
  } ma_mode_t;
  // Тип для текущей команды
  typedef enum _ma_command_t {
    mac_free,
    mac_get_param,
    mac_get_resistance,
    mac_auto_calibration,
    mac_send_commands
  } ma_command_t;
  // Тип для макрорежима
  typedef enum _macro_mode_t {
    macro_mode_get_voltage,
    macro_mode_get_resistance,
    macro_mode_send_commands,
    macro_mode_stop
  } macro_mode_t;
  // Тип для индексов
  typedef irs_i32 index_t;
  // Тип для типа тока и напряжения - перменное/постоянное
  enum volt_curr_type_t { vct_direct, vct_alternate };
  // Тип измерения ток/напряжение
  enum measure_t {
    meas_value,
    meas_voltage,
    meas_current,
    meas_frequency,
    meas_set_range
  };
  // Режим инициализации
  enum init_mode_t {
    im_start,
    im_write_command,
    im_next_command,
    im_stop
  };

  // Режим работы мультиметра
  multimeter_mode_type_t m_mul_mode_type;
  // Команды при инициализации
  vector<irs::string> m_init_commands;
  // Индекс команды установки типа напряжения: постоянное/переменное
  //index_t m_voltage_type_index;

  // Команда для текущего типа измерения
  //const irs::string m_value_type;
  // Команда для постоянного напряжения
  const irs::string m_voltage_type_direct;
  // Команда для переменного напряжения
  const irs::string m_voltage_type_alternate;
  // Команда для постоянного тока
  const irs::string m_current_type_direct;
  // Команда для переменного тока
  const irs::string m_current_type_alternate;
  // Команда для постоянного напряжения c диапазоном
  irs::string m_voltage_type_direct_range;
  // Команда для переменного напряжения c диапазоном
  irs::string m_voltage_type_alternate_range;
  // Команда для постоянного тока c диапазоном
  irs::string m_current_type_direct_range;
  // Команда для переменного тока c диапазоном
  irs::string m_current_type_alternate_range;

  // Типа тока и напряжения - перменное/постоянное
  volt_curr_type_t m_volt_curr_type;
  // Индекс команды установки времени интегрирования для напряжения
  //index_t m_time_int_voltage_index;
  // Команда установки времени интегрирования для напряжения
  irs::string m_time_int_measure_command;
  // Команды при чтении напряжения
  vector<irs::string> m_get_measure_commands;
  // Индекс команды установки типа измерения сопротивления:
  // 2-/4-проводная схема
  index_t m_resistance_type_index;
  // Команда для установки 2-проводной схемы измерения сопротивления
  const irs::string m_resistance_type_2x;
  // Команда для установки 4-проводной схемы измерения сопротивления
  const irs::string m_resistance_type_4x;
  // Индекс команды установки времени интегрирования для сопротивления
  index_t m_time_int_resistance_index;
  // Команды при чтении сопротивления
  vector<irs::string> m_get_resistance_commands;

  // Команда для установки диапазона измерений
  irs::string m_set_range_command;
  // Дескриптор канала mxifa
  void *m_handle;
  // Ощибка создания
  bool m_create_error;
  // Текущий режим работы
  ma_mode_t m_mode;
  // Текущий макрорежим работы
  macro_mode_t m_macro_mode;
  // Статус текущей операции
  meas_status_t m_status;
  // Текущая команда
  ma_command_t m_command;
  // Указатель на перменную напряжение пользователя
  double *m_voltage;
  // Указатель на перменную сопротивление пользователя
  double *m_resistance;
  // Запрос на прерывание операции
  bool m_abort_request;
  // Буфер приема
  irs_u8 m_read_buf[ma_read_buf_size];
  // Текущая позиция в буфере приема
  index_t m_read_pos;
  // Текущая команда
  irs::string m_cur_mul_command;
  // Команды для мультиметра
  vector<irs::string> *m_mul_commands;
  // Индекс команд для мультиметра
  index_t m_mul_commands_index;
  // Предыдущая команда
  //ma_command_t m_command_prev;
  // Указатель на переменную в которую будет считано значение
  double *m_value;
  // Выполнить чтение параметра
  bool m_get_parametr_needed;
  // Время таймаута операций
  counter_t m_oper_time;
  // Таймаут операций
  counter_t m_oper_to;
  // Время таймаута калибровки
  counter_t m_acal_time;
  // Таймаут калибровки
  counter_t m_acal_to;
  // Таймер записи команд инициализации
  irs::timer_t m_init_timer;
  // Режим инициализации
  init_mode_t m_init_mode;
  // Индекс команды инициализации
  index_t m_ic_index;

  // Запрещение конструктора по умолчанию
  mx_agilent_3458a_t();
  // Создание команд для напряжени/тока
  void measure_create_commands(measure_t a_measure);
  // Отправка команд инициализации в мультиметр
  void initialize_tick();
public:
  // Конструктор
  mx_agilent_3458a_t(
    mxifa_ch_t channel,
    gpib_addr_t address,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // Деструктор
  ~mx_agilent_3458a_t();
  // Установить режим измерения постоянного напряжения
  virtual void set_dc();
  // Установить режим измерения переменного напряжения
  virtual void set_ac();
  // Установить положителный фронт запуска
  virtual void set_positive();
  // Установить отрицательный фронт канала
  virtual void set_negative();
  // Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  // Чтение напряжения
  virtual void get_voltage(double *voltage);
  // Чтения силы тока
  virtual void get_current(double *current);
  // Чтение сопротивления
  virtual void get_resistance2x(double *resistance);
  // Чтение сопротивления
  virtual void get_resistance4x(double *resistance);
  // Чтение частоты
  virtual void get_frequency(double *frequency);
  // Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average);
  // Чтение фазового сдвига
  virtual void get_phase(double *phase);
  // Чтение временного интервала
  virtual void get_time_interval(double *time_interval);
  // Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);

  // Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  // Чтение статуса текущей операции
  virtual meas_status_t status();
  // Прерывание текущей операции
  virtual void abort();
  // Элементарное действие
  virtual void tick();
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc);
  // Установка времени интегрирования в c
  virtual void set_aperture(double aperture);
  // Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth);
  // Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  // Устсновка уровня запуска канала
  virtual void set_start_level(double level);
  // Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // Установка автоматического выбора диапазона измерений
  virtual void set_range_auto();

  // Установка временного интервала измерения
  //void set_time_interval(double time_interval);
};


namespace irs{

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

// Класс для работы с мультиметром Agilent 3458A в режиме дискретизации
class agilent_3458a_digitizer_t: public mxmultimeter_t
{
public:
  typedef irs_size_t size_type;
  typedef long double math_value_type;
  // Конструктор
  agilent_3458a_digitizer_t(
    irs::hardflow_t* ap_hardflow,
    const filter_settings_t& a_filter_settings,
    const double a_sampling_time_s = 25e-6,
    const double a_interval_s = 30.0
  );
  // Деструктор
  ~agilent_3458a_digitizer_t();
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  // Установить режим измерения постоянного напряжения
  virtual void set_dc();
  // Установить режим измерения переменного напряжения
  virtual void set_ac();
  // Установить положителный фронт запуска
  virtual void set_positive();
  // Установить отрицательный фронт канала
  virtual void set_negative();
  // Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  // Чтение напряжения
  virtual void get_voltage(double *voltage);
  // Чтения силы тока
  virtual void get_current(double *current);
  // Чтение сопротивления
  virtual void get_resistance2x(double *resistance);
  // Чтение сопротивления
  virtual void get_resistance4x(double *resistance);
  // Чтение частоты
  virtual void get_frequency(double *frequency);
  // Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average);
  // Чтение фазового сдвига
  virtual void get_phase(double *phase);
  // Чтение временного интервала
  virtual void get_time_interval(double *time_interval);
  // Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval); 
  // Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  // Чтение статуса текущей операции
  virtual meas_status_t status();
  // Прерывание текущей операции
  virtual void abort();
  // Элементарное действие
  virtual void tick();
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc);
  // Установка времени интегрирования в c
  virtual void set_aperture(double aperture);
  // Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth);
  // Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  // Устсновка уровня запуска канала
  virtual void set_start_level(double level);
  // Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // Установка автоматического выбора диапазона измерений
  virtual void set_range_auto();
private:
  void commands_to_buf_send();
  irs::hardflow_t* mp_hardflow;
  filter_settings_t m_filter_settings;
  iir_filter_t<math_value_type> m_iir_filter;
  // Команды при инициализации
  raw_data_t<irs_u8> m_command_terminator;
  vector<irs_string_t> m_commands;
  raw_data_t<irs_u8> m_buf_send;
  raw_data_t<irs_u8> m_buf_receive;
  raw_data_t<double> m_samples;
  raw_data_t<double> m_filtered_values;
  enum { m_need_samples_count = 1500000 };
  enum { m_need_receive_data_size = m_need_samples_count * sizeof(irs_u16) };
  const double m_sampling_time_default;
  bool m_initialization_complete;
  bool m_coefficient_receive_ok;
  double m_coefficient;
  double* mp_value;
  meas_status_t m_status;
  irs::timer_t m_delay_timer;
}; // class agilent_3458a_digitizer_t

#endif // IRS_FULL_STDCPPLIB_SUPPORT

// Класс для работы с мультиметром В7-78/1
class v7_78_1_t: public mxmultimeter_t
{
  // Тип для текущего режима
  typedef enum _ma_mode_t {
    ma_mode_start,
    ma_mode_macro,
    ma_mode_commands,
    ma_mode_commands_wait,
    ma_mode_get_value,
    ma_mode_auto_calibration,
    ma_mode_auto_calibration_wait
  } ma_mode_t;
  // Тип для текущей команды
  typedef enum _ma_command_t {
    mac_free,
    mac_get_value,
    mac_get_voltage,
    mac_get_resistance,
    mac_get_current,
    mac_get_frequency,
    mac_auto_calibration
  } ma_command_t;
  typedef enum _macro_mode_t {
    macro_mode_get_value,
    macro_mode_get_voltage,
    macro_mode_get_resistance,
    macro_mode_get_current,
    macro_mode_get_frequency,
    macro_mode_send_commands,
    macro_mode_stop
  } macro_mode_t;
  enum meas_type_t{DC_MEAS, AC_MEAS} m_meas_type;
  enum resistance_meas_type_t{RES_MEAS_2x, RES_MEAS_4x} m_res_meas_type;
  // Тип для индексов
  typedef irs_i32 index_t;


  //static const char m_volt_ac_nplcycles = "DETector:BANDwidth 20";

  multimeter_mode_type_t m_mul_mode_type;

  // Команды при инициализации
  vector<irs::string> f_init_commands;
  /* Индекс команды установки времени интегрирования для
  постоянного напряжения*/
  index_t m_nplc_voltage_dc_index;
  // Индекс команды установки полосы фильтра для переменного напряжения
  index_t m_band_width_voltage_ac_index;
  // Команды чтения значения при произвольных настройках
  vector<irs::string> m_get_value_commands;
  // Команды при чтении напряжения
  vector<irs::string> m_get_voltage_dc_commands;
  vector<irs::string> m_get_voltage_ac_commands;

  // Индекс команды установки времени интегрирования для сопротивления
  index_t m_nplc_resistance_2x_index;
  index_t m_nplc_resistance_4x_index;
  // Команды при чтении сопротивления
  vector<irs::string> m_get_resistance_2x_commands;
  vector<irs::string> m_get_resistance_4x_commands;

  // Индекс команды установки времени интегрирования для тока
  index_t m_nplc_current_dc_index;
  index_t m_nplc_current_ac_index;
  // Команды при чтении тока
  vector<irs::string> m_get_current_dc_commands;
  vector<irs::string> m_get_current_ac_commands;

  // Индекс команды установки времени счета
  index_t m_aperture_frequency_index;
  // Команды при чтении частоты
  vector<irs::string> m_get_frequency_commands;

  // Дескриптор канала mxifa
  void *f_handle;
  // Ощибка создания
  irs_bool f_create_error;
  // Текущий режим работы
  ma_mode_t f_mode;
  // Текущий макрорежим работы
  macro_mode_t f_macro_mode;
  // Статус текущей операции
  meas_status_t f_status;
  // Текущая команда
  ma_command_t f_command;
  // Указатель на переменную измеряемой величины,
  // при произвольном типе измерения
  double* mp_value;
  // Указатель на перменную напряжение пользователя
  double* mp_voltage;
  // Указатель на перменную сопротивление пользователя
  double* mp_resistance;
  // Указатель на перменную ток пользователя
  double* mp_current;
  // Указатель на перменную частота пользователя
  double* mp_frequency;
  // Запрос на прерывание операции
  irs_bool f_abort_request;
  // Буфер приема
  irs_u8 f_read_buf[ma_read_buf_size];
  // Текущая позиция в буфере приема
  index_t f_read_pos;
  // Текущая команда
  irs::string f_cur_mul_command;
  // Команды для мультиметра
  vector<irs::string> *f_mul_commands;
  // Индекс команд для мультиметра
  index_t f_mul_commands_index;
  // Предыдущая команда
  //ma_command_t f_command_prev;
  // Указатель на переменную в которую будет считано значение
  double *f_value;
  // Выполнить чтение параметра
  irs_bool f_get_parametr_needed;
  // Время таймаута операций
  counter_t f_oper_time;
  // Таймаут операций
  counter_t f_oper_to;
  // Время таймаута калибровки
  counter_t f_acal_time;
  // Таймаут калибровки
  counter_t f_acal_to;

  bool m_current_volt_dc_meas;
  // Запрещение конструктора по умолчанию
  v7_78_1_t();
public:
  // Конструктор
  v7_78_1_t(
    mxifa_ch_t channel,
    gpib_addr_t address,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // Деструктор
  ~v7_78_1_t();
  // Установить режим измерения постоянного напряжения
  virtual inline void set_dc();
  // Установить режим измерения переменного напряжения
  virtual inline void set_ac();
  // Установить положителный фронт запуска
  virtual void set_positive();
  // Установить отрицательный фронт канала
  virtual void set_negative();
  // Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  // Чтение напряжения
  virtual void get_voltage(double *voltage);
  // Чтения силы тока
  virtual void get_current(double *current);
  // Чтение сопротивления
  virtual void get_resistance2x(double *resistance);
  // Чтение сопротивления
  virtual void get_resistance4x(double *resistance);
  // Чтение частоты
  virtual void get_frequency(double *frequency);
  // Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average);
  // Чтение фазового сдвига
  virtual void get_phase(double *phase);
  // Чтение временного интервала
  virtual void get_time_interval(double *time_interval);
  // Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);

  // Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  // Чтение статуса текущей операции
  virtual meas_status_t status();
  // Прерывание текущей операции
  virtual void abort();
  // Элементарное действие
  virtual void tick();
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc);
  // Установка времени интегрирования в c
  virtual void set_aperture(double aperture);
  // Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth);
  // Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  // Устсновка уровня запуска канала
  virtual void set_start_level(double level);
  // Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // Установка автоматического выбора диапазона измерений
virtual void set_range_auto();
private:
  // Установка временного интервала измерения
  void set_time_interval_meas(const double a_time_interval_meas);
};
// Установить режим измерения постоянного напряжения
inline void irs::v7_78_1_t::set_dc()
{
  m_meas_type = DC_MEAS;
}
// Установить режим измерения переменного напряжения
inline void irs::v7_78_1_t::set_ac()
{
  m_meas_type = AC_MEAS;
}

} // namespace irs

// Класс для работы с частотомером электронно-счетным акип чз-85/3R
namespace irs{
enum msg_status_t{MSG_BUSY, MSG_SUCCESS};
class akip_ch3_85_3r_t: public mxmultimeter_t
{
  class buf_data_t
  {
    enum {m_size_buf = 1024};
    irs_u8 m_arr[m_size_buf];
    irs_u32 m_pos_inf_elem;
    irs_u32 m_count_inf_elem;
  public:
    buf_data_t():m_pos_inf_elem(0), m_count_inf_elem(0)
    {memset(m_arr,0,sizeof(m_arr));}
    void push(const irs_u8* const ap_data, const irs_u32 a_size)
    {
      m_count_inf_elem = min(a_size, (irs_u32)m_size_buf);
      memcpy(m_arr, ap_data, m_count_inf_elem);
      m_pos_inf_elem = 0;
    }
    irs_u8* ptr_data()
    {
      return &(m_arr[m_pos_inf_elem]);
    }
    void pop_front(const irs_u32 a_size)
    {
      m_pos_inf_elem += min(a_size, m_count_inf_elem);
      m_count_inf_elem -= min(a_size, m_count_inf_elem);
    }
    irs_u32 size()
    {
      return  m_count_inf_elem;
    }
  };

  class string_msgs_t:public irs::string
  {
    // Ошибочные комбинации подстрок
    vector<irs::string> mv_bad_str;
    // Возвращает сообщение(если оно сформировано)
  public:
    string_msgs_t():
      mv_bad_str()
    {
      mv_bad_str.push_back(irst("LOC\n"));
      mv_bad_str.push_back(irst("DEG"));
      mv_bad_str.push_back(irst("Hz"));
      mv_bad_str.push_back(irst("s"));
    }
    msg_status_t get_message(irs::string& a_msg_str)
    {
      msg_status_t msg_stat = MSG_BUSY;
      const size_type line_fit_pos = find(irst('\n'));
      if(line_fit_pos != irs::string::npos){
        // Удаление мусора из строк
        irs_u32 size_bad_str = mv_bad_str.size();
        for(irs_u32 i = 0; i < size_bad_str; i++){
          irs::string bad_str = mv_bad_str[i];
          const size_type bad_str_pos = find(bad_str);
          if(bad_str_pos != irs::string::npos){
            erase(bad_str_pos, bad_str.size());
          }
        }
        const size_type new_line_fit_pos = find(irst('\n'));
        if(new_line_fit_pos != irs::string::npos){
          msg_stat = MSG_SUCCESS;
          const size_type msg_size = new_line_fit_pos;
          a_msg_str.assign(*this, 0, msg_size);
          erase(0, msg_size + 1);
          //clear();
        }else{
          msg_stat = MSG_BUSY;
        }
      }else{
        msg_stat = MSG_BUSY;
      }
      return msg_stat;
    }
  };
  error_trans_base_t *mp_error_trans;

  hardflow_t* mp_hardflow;
  multimeter_mode_type_t m_mul_mode_type;
  const counter_t m_time_between_big_cmd;
  const counter_t m_time_between_small_cmd;
  loop_timer_t m_time_interval;

  enum meas_type_t{DC_MEAS, AC_MEAS} m_meas_type;

  deque<irs::string> md_buf_cmds;

  buf_data_t m_buf_cmd;
  string_msgs_t m_string_msgs;

  enum command_status_t{CS_SUCCESS = 0, CS_BUSY = 1, CS_ERROR = 2};
  // Статус текущей операции
  command_status_t m_send_command_stat;
  meas_status_t m_meas_stat;

  // Указатель на пользовательскую переменную, в которую
  //будет сохранен результат.
  double* mp_result;
  // Тип для индексов
  typedef irs_i32 index_t;
  // Указатель на последнюю команду ИЗМЕРЕНИЙ
  vector<irs::string>* mp_last_commands;

  // Команды при инициализации
  vector<irs::string> mv_init_commands;

  // Команды при перезакгрузке мультиметра
  vector<irs::string> mv_reset_multimetr_commands;

  // Указатели на команды настроек мультиметра
  vector<irs::string*> mv_multimetr_p_commands_setting;

  // Команды при чтении значения измеряемой величины,
  // при произвольном типе измерения
  vector<irs::string> mv_get_value_commands;

  // Команды при чтении усредненной разности фаз
  vector<irs::string> mv_get_phase_average_commands;

  // Команды при чтении разности фаз
  vector<irs::string> mv_get_phase_commands;

  // Команды при чтении временного интервала
  vector<irs::string> mv_get_time_interval_commands;

  // Команды при чтении усредненного временного интервала
  vector<irs::string> mv_get_time_interval_average_commands;

  // Команды при чтении частоты
  vector<irs::string> mv_get_frequency_commands;

  // Команды, большие по времении выполнения
  vector<irs::string> mv_big_time_commands;

  // Команды установки времени счета
  irs::string ms_set_interval_command;

  // Команды установки входного сопротивления каналов
  irs::string ms_set_in_impedance_channel_1_command;
  irs::string ms_set_in_impedance_channel_2_command;

  // Команды установки вида связи входа канала
  irs::string ms_set_type_coupling_channel_1_command;
  irs::string ms_set_type_coupling_channel_2_command;

  // Команды установки типа фронта запуска канала
  irs::string ms_set_type_slope_channel_1_command;
  irs::string ms_set_type_slope_channel_2_command;

  // Команды установки фильтра канала
  irs::string ms_set_filter_channel_1_command;
  irs::string ms_set_filter_channel_2_command;

  // Установка уровня запуска канала
  irs::string ms_set_level_channel_1_command;
  irs::string ms_set_level_channel_2_command;

  // Запрещение конструктора по умолчанию
  akip_ch3_85_3r_t();
public:
  // Конструктор
  akip_ch3_85_3r_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // Деструктор
  ~akip_ch3_85_3r_t();
  void connect(hardflow_t* ap_hardflow);
  // Установить режим измерения постоянного напряжения
  virtual void set_dc();
  // Установить режим измерения переменного напряжения
  virtual void set_ac();
  // Установить положителный фронт запуска
  virtual void set_positive();
  // Установить отрицательный фронт канала
  virtual void set_negative();
  // Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  // Чтение напряжения
  virtual void get_voltage(double *ap_voltage);
  // Чтения силы тока
  virtual void get_current(double *ap_current);
  // Чтение сопротивления
  virtual void get_resistance2x(double *ap_resistance);
  // Чтение сопротивления
  virtual void get_resistance4x(double *ap_resistance);

  // Чтение частоты
  virtual void get_frequency(double *ap_frequency);
  // Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *ap_phase_average);
  // Чтение фазового сдвига
  virtual void get_phase(double *ap_phase);
  // Чтение временного интервала
  virtual void get_time_interval(double *ap_time_interval);
  // Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);
  // Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  // Чтение статуса текущей операции
  virtual meas_status_t status();
  // Перезагрузка с восстановлением параматров
  virtual void abort();
  // Элементарное действие
  virtual void tick();
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double a_nplc);
  // Установка времени счета в cек
  virtual void set_aperture(double a_aperture);
  // Установка фильтра в герцах
  virtual void set_bandwidth(double a_bandwidth);
  // Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  // Устсновка уровня запуска канала
  virtual void set_start_level(double a_level);
  // Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // Установка автоматического выбора диапазона измерений
  virtual void set_range_auto();
private:
  // Установка временного интервала измерения
  void set_time_interval_meas(const double a_time_interval_meas);
};
  // Установить режим измерения постоянного напряжения
/*inline void irs::akip_ch3_85_3r_t::set_dc()
  {m_meas_type = DC_MEAS;}
// Установить режим измерения переменного напряжения
inline void irs::akip_ch3_85_3r_t::set_ac()
  {m_meas_type = AC_MEAS;}
*/
}

class dummy_multimeter_t: public mxmultimeter_t
{
public:
  // Установить режим измерения постоянного напряжения
  virtual void set_dc() {}
  // Установить режим измерения переменного напряжения
  virtual void set_ac() {}
  // Установить положителный фронт запуска
  virtual void set_positive() {}
  // Установить отрицательный фронт канала
  virtual void set_negative() {}
  // Чтение значения при текущем типа измерения
  virtual void get_value(double* /*value*/) {}
  // Чтение напряжения
  virtual void get_voltage(double* /*voltage*/) {}
  // Чтения силы тока
  virtual void get_current(double* /*current*/) {}
  // Чтение сопротивления
  virtual void get_resistance2x(double* /*resistance*/) {}
  // Чтение сопротивления
  virtual void get_resistance4x(double* /*resistance*/) {}
  // Чтение частоты
  virtual void get_frequency(double* /*frequency*/) {}
  // Чтение усредненного сдвира фаз
  virtual void get_phase_average(double* /*phase_average*/) {}
  // Чтение фазового сдвига
  virtual void get_phase(double* /*phase*/) {}
  // Чтение временного интервала
  virtual void get_time_interval(double* /*time_interval*/) {}
  // Чтение усредненного временного интервала
  virtual void get_time_interval_average(double* /*ap_time_interval*/) {}

  // Запуск автокалибровки мультиметра
  virtual void auto_calibration() {}
  // Чтение статуса текущей операции
  virtual meas_status_t status() { return meas_status_success; }
  // Прерывание текущей операции
  virtual void abort() {}
  // Элементарное действие
  virtual void tick() {}
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double /*nplc*/) {}
  // Установка времени интегрирования в c
  virtual void set_aperture(double /*aperture*/) {}
  // Установка полосы фильтра
  virtual void set_bandwidth(double /*bandwidth*/) {}
  // Установка входного сопротивления канала
  virtual void set_input_impedance(double /*impedance*/) {}
  // Установка уровня запуска канала
  virtual void set_start_level(double /*level*/) {}
  // Установка диапазона измерений
  virtual void set_range(type_meas_t /*a_type_meas*/, double /*a_range*/) {}
  // Установка автоматического выбора диапазона измерений
  virtual void set_range_auto() {}
  // Установка входного сопротивления канала
  //virtual void set_filter(double ) {}
};

namespace irs {

// Класс для работы с мультиметром National Instruments PXI-4071
class ni_pxi_4071_t: public mxmultimeter_t
{
public:
  enum { // power_freq
    power_50_Hz,
    power_60_Hz
  };
  enum { // resolution
    digits_3_5,
    digits_4_5,
    digits_5_5,
    digits_6_5,
    digits_7_5
  };
  enum { // meas_type
    precision,
    high_speed
  };
  enum { // intergation time units
    sec,
    plc
  };
  
  
  ni_pxi_4071_t(
    hardflow_t* ap_hardflow,
    filter_settings_t* ap_filter,
    counter_t a_update_time
  );
  ~ni_pxi_4071_t();
  // Установить режим измерения постоянного напряжения
  virtual void set_dc();
  // Установить режим измерения переменного напряжения
  virtual void set_ac();
  // Установить положителный фронт запуска
  virtual void set_positive();
  // Установить отрицательный фронт канала
  virtual void set_negative();
  // Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  // Чтение напряжения
  virtual void get_voltage(double *ap_voltage);
  // Чтения силы тока
  virtual void get_current(double *ap_current);
  // Чтение сопротивления
  virtual void get_resistance2x(double *ap_resistance);
  // Чтение сопротивления
  virtual void get_resistance4x(double *ap_resistance);
  // Чтение частоты
  virtual void get_frequency(double *ap_frequency);
  // Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *ap_phase_average);
  // Чтение фазового сдвига
  virtual void get_phase(double *ap_phase);
  // Чтение временного интервала
  virtual void get_time_interval(double *ap_time_interval);
  // Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);

  // Запуск автокалибровки мультиметра
  virtual void auto_calibration();
  // Чтение статуса текущей операции
  virtual meas_status_t status();
  // Прерывание текущей операции
  virtual void abort();
  // Элементарное действие
  virtual void tick();
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double a_nplc);
  // Установка времени интегрирования в c
  virtual void set_aperture(double a_aperture);
  // Установка полосы фильтра
  virtual void set_bandwidth(double /*a_bandwidth*/);
  // Установка входного сопротивления канала
  virtual void set_input_impedance(double /*a_impedance*/);
  // Установка уровня запуска канала
  virtual void set_start_level(double /*a_level*/);
  // Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // Установка автоматического выбора диапазона измерений
  virtual void set_range_auto();
  // Установка входного сопротивления канала
  //virtual void set_filter(double a_time_interval);
  
  mxdata_t* mxdata();
private:
  enum {
    auto_range = -1
  };
  enum mode_t {
    start_mode,
    get_value_mode,
    stop_mode
  };
  struct eth_mul_data_t {
    // Установки:
    //bit_data_t auto_zero;
    //bit_data_t power_freq; // 0 - 60 Hz; 1 - 50 Hz
    conn_data_t<double> samples_per_sec;
    conn_data_t<double> integrate_time;
    conn_data_t<double> resolution_digits;
    conn_data_t<double> range;
    //bit_data_t filtering;
    conn_data_t<double> sampling_freq;
    conn_data_t<double> low_cutoff_freq;
    conn_data_t<double> stopband_ripple;
    conn_data_t<double> passband_ripple;
    conn_data_t<irs_u16> meas_type;
    conn_data_t<irs_u16> meas_mode;
    conn_data_t<irs_u16> integrate_time_units; // 0 - Seconds; 1 - PLCs
    conn_data_t<irs_u16> filter_type;
    conn_data_t<irs_u16> filter_order;
        
    // Считываемые значения:
    conn_data_t<double> meas_value;
    //bit_data_t out_of_range;
  
    eth_mul_data_t(irs::mxdata_t *ap_data = IRS_NULL, irs_uarc a_index = 0,
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
      
      index = samples_per_sec.connect(ap_data, index);
      index = integrate_time.connect(ap_data, index);
      index = resolution_digits.connect(ap_data, index);
      index = range.connect(ap_data, index);
      index = sampling_freq.connect(ap_data, index);
      index = low_cutoff_freq.connect(ap_data, index);
      index = stopband_ripple.connect(ap_data, index);
      index = passband_ripple.connect(ap_data, index);
      index = meas_type.connect(ap_data, index);
      index = meas_mode.connect(ap_data, index);
      index = integrate_time_units.connect(ap_data, index);
      index = filter_type.connect(ap_data, index);
      index = filter_order.connect(ap_data, index);
            
      index = meas_value.connect(ap_data, index);
      
      return index;
    }
  };
  
  hardflow_t* mp_hardflow;
  modbus_client_t m_modbus_client;
  eth_mul_data_t m_eth_mul_data;
  meas_status_t m_status;
  double* mp_value;
  bool m_abort_request;
  mode_t m_mode;
  filter_settings_t* mp_filter;
};

} //namespace irs

#endif //measmulH
