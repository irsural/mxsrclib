//! \file
//! \ingroup drivers_group
//! \brief Классы для работы с мультиметрами
//!
//! Дата: 07.02.2011\n
//! Ранняя дата: 10.09.2009


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
#include <irsint.h>
#include <irsalg.h>

#include <irsfinal.h>

#if (MXIFA_VERSION < 0x0004)
#error mxifa version incompatible. The version should be >= 0.4.
#endif //(MXIFA_VERSION < 0x0004)

//! \addtogroup drivers_group
//! @{

// Размер буфера чтения
#define ma_read_buf_size 30

enum type_meas_t {
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
  tm_last = tm_time_interval_average,
  tm_for_set_type_size = 0xFFFFFFFF
};

irs::string_t type_meas_to_str(const type_meas_t a_type_meas);
bool str_to_type_meas(
  const irs::string_t& a_str_type_meas, type_meas_t& a_type_meas);

enum multimeter_mode_type_t {mul_mode_type_active, mul_mode_type_passive};

enum multimeter_param_t {
  mul_param_first,
  // Исходные значения (массив double)
  mul_param_source_values = mul_param_first,
  // Отфильтрованные значения (массив double)
  mul_param_filtered_values,
  // Стандартное оклонение (double)
  mul_param_standard_deviation,
  // Стаднартное относительное отклонение (double)
  mul_param_standard_deviation_relative,
  // Вариация (double)
  mul_param_variation,
  // Относительная вариация (double)
  mul_param_variation_relative,
  // Время дискретизации (double)
  mul_param_sampling_time_s,
  // Включение аналогового фильтра (bool)
  mul_param_analog_filter_enabled,
  // Настройки фильтра (struct filter_settings_t файл irsdsp.h)
  mul_param_filter_settings,
  // Формат отсчетов (enum mul_sample_format_t файл measmul.h)
  mul_param_sample_format,
  // Максимальное время одного тика (double)
  mul_param_tick_max_time_s,
  // Установка типа окна для КИХ-фильтра
  // (enum window_function_form_t файл irsfilter.h)
  mul_param_window_function_form,
  // Установка типа фильтра по типу импульсной характеристики
  // (enum filter_impulse_response_type_t файл irsdsp.h)
  mul_param_filter_impulse_response_type,
  // Включение расчета фильтрованных значений (bool)
  mul_param_filtered_values_enabled,
  mul_param_last = mul_param_filtered_values_enabled,
  mul_param_count = (mul_param_last - mul_param_first) + 1
};

//! Формат передачи отсчетов между мультиметром и компьютером
enum mul_sample_format_t {
  mul_sample_format_str,    //!< Строковый формат передачи отсчетов
  mul_sample_format_int16,  //!< Бинарный формат(16 бит) передачи отсчетов
  mul_sample_format_int32   //!< Бинарный формат(32 бита) передачи отсчетов
};

//! \brief Абстрактный базовый класс для работы с мультиметрами
class mxmultimeter_t
{
public:
  virtual ~mxmultimeter_t() {};
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  // Установить режим измерения постоянного напряжения (мгновенная команда)
  virtual void set_dc() = 0;
  // Установить режим измерения переменного напряжения (мгновенная команда)
  virtual void set_ac() = 0;
  // Установить положителный фронт запуска (мгновенная команда)
  virtual void set_positive() = 0;
  // Установить отрицательный фронт канала (статусная команда)
  virtual void set_negative() = 0;
  // Чтение значения при текущем типа измерения (статусная команда)
  virtual void get_value(double *value) = 0;
  // Чтение напряжения (статусная команда)
  virtual void get_voltage(double *voltage) = 0;
  // Чтения силы тока (статусная команда)
  virtual void get_current(double *current) = 0;
  // Чтение сопротивления (статусная команда)
  virtual void get_resistance2x(double *resistance) = 0;
  // Чтение сопротивления (статусная команда)
  virtual void get_resistance4x(double *resistance) = 0;
  // Чтение частоты (статусная команда)
  virtual void get_frequency(double *frequency) = 0;
  // Чтение усредненного сдвира фаз (статусная команда)
  virtual void get_phase_average(double *phase_average) = 0;
  // Чтение фазового сдвига (статусная команда)
  virtual void get_phase(double *phase) = 0;
  // Чтение временного интервала (статусная команда)
  virtual void get_time_interval(double *time_interval) = 0;
  // Чтение усредненного временного интервала (статусная команда)
  virtual void get_time_interval_average(double *ap_time_interval) = 0;
  // Чтение отношения напряжений на 1-м и 2-м каналах
  virtual void get_voltage_ratio(double *voltage_ratio) { *voltage_ratio = 0.; }
  // Выбор канала для последующих измерений
  virtual void select_channel(size_t /*channel*/) { }

  // Запуск автокалибровки мультиметра (мгновенная команда)
  virtual void auto_calibration() = 0;
  // Чтение статуса текущей операции
  virtual meas_status_t status() = 0;
  // Прерывание текущей операции (статусная команда)
  virtual void abort() = 0;
  // Элементарное действие
  virtual void tick() = 0;
  // Установка времени интегрирования в периодах частоты сети (20 мс)
  // (мгновенная команда)
  virtual void set_nplc(double nplc) = 0;
  // Установка времени интегрирования в c (статусная команда)
  virtual void set_aperture(double aperture) = 0;
  // Установка полосы фильтра (статусная команда)
  virtual void set_bandwidth(double bandwidth) = 0;
  // Установка входного сопротивления канала (статусная команда)
  virtual void set_input_impedance(double impedance) = 0;
  // Устсновка уровня запуска канала (статусная команда)
  virtual void set_start_level(double level) = 0;
  // Установка диапазона измерений (статусная команда)
  virtual void set_range(type_meas_t a_type_meas, double a_range) = 0;
  // Установка автоматического выбора диапазона измерений (мгновенная команда)
  virtual void set_range_auto() = 0;
  // Установка входного сопротивления канала
  //virtual void set_filter(double ) = 0;
};

//! @}

namespace irs {

//! \ingroup drivers_group
//! \brief Класс для работы с мультиметром Agilent 3458A через irs::hardflow_t
class agilent_3458a_t: public mxmultimeter_t
{
  // Тип для текущего режима
  typedef enum _ma_mode_t {
    ma_mode_start,
    ma_mode_macro,
    ma_mode_commands,
    ma_mode_commands_wait,
    ma_mode_get_value,
    ma_mode_get_value_wait,
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
  typedef irs::hardflow::fixed_flow_t::status_t fixed_flow_status_type;
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

  enum {
    sample_size = 18
  };

  enum {
    read_timeout_s = 100
  };
  
  enum {
    fixed_flow_read_timeout_delta_s = 2
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
  // Команда для установки 2-проводной схемы измерения сопротивления
  const irs::string m_resistance_type_2x;
  // Команда для установки 4-проводной схемы измерения сопротивления
  const irs::string m_resistance_type_4x;
  // Команды при чтении сопротивления
  vector<irs::string> m_get_resistance_commands;

  // Команда для установки диапазона измерений
  irs::string m_set_range_command;
  // Интерфейс передачи
  irs::hardflow_t* mp_hardflow;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  //irs::raw_data_t<irs_u8> m_write_buf;
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
  // Установлен, если требуется очистка буфера hardflow перед чтением
  // значения из мультиметра
  bool m_is_clear_buffer_needed;

  bool m_first_meas_resist;

  const irs::string m_resistance_ocomp_off;
  const irs::string m_resistance_ocomp_on;

  // Запрещение конструктора по умолчанию
  agilent_3458a_t();
  // Создание команд для напряжени/тока
  void measure_create_commands(measure_t a_measure);
  // Отправка команд инициализации в мультиметр
  void initialize_tick();
public:
  // Конструктор
  agilent_3458a_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // Деструктор
  ~agilent_3458a_t();
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

} // namespace irs

//! \ingroup drivers_group
//! \brief Класс для работы с мультиметром Agilent 3458A
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
  // index_t m_time_int_voltage_index;
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

namespace irs {

//! \addtogroup drivers_group
//! @{

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

template <class T>
T flip_data(const T& a_data)
{
  T fliped_data = T();
  for (size_t byte_i = 0; byte_i < sizeof(T); byte_i++) {
    *(reinterpret_cast<irs_u8*>(&fliped_data) + byte_i) =
      *(reinterpret_cast<const irs_u8*>(&a_data) + (sizeof(T) - byte_i - 1));
  }
  return fliped_data;
}

void flip_data(irs_u8* ap_data, const size_t a_size);

// Преобразует последовательность байт в число выбранного целого типа
template <class T>
T data_to_value(irs_u8* ap_data, const size_t a_size)
{
  IRS_STATIC_ASSERT(is_integral_type<T>::value);
  T value = 0;
  switch (a_size) {
    case 1: {
      value = static_cast<T>(*ap_data);
    } break;
    case 2: {
      value = static_cast<T>(*reinterpret_cast<irs_i16*>(ap_data));
    } break;
    case 4: {
      value = static_cast<T>(*reinterpret_cast<irs_i32*>(ap_data));
    } break;
    case 6: {
      value = static_cast<T>(*reinterpret_cast<irs_i64*>(ap_data));
    } break;
    default: {
      IRS_LIB_ERROR(ec_standard,
        "Выбранный размер не поддерживается алгоритмом");
    }
  }
  return value;
}

class data_to_values_t
{
public:
  typedef size_t size_type;
  data_to_values_t();
  data_to_values_t(
    const bool a_flip_data_enabled,
    const size_type a_sample_size,
    const double a_coefficient,
    const raw_data_t<irs_u8>* ap_data,
    raw_data_t<double>* ap_samples
  );
  // Устанавливает указатели на входные/выходные массивы данных.
  // Если выполняется операция, она сбрасывается
  void set(
    const bool a_flip_data_enabled,
    const size_type a_sample_size,
    const double a_coefficient,
    const raw_data_t<irs_u8>* ap_data,
    raw_data_t<double>* ap_samples);
  // Вовзращает статус завершенности
  bool completed() const;
  // Элементарное действие
  void tick();
  // Прерывание операции
  void abort();
  // Установка времени тика
  void set_tick_max_time(const double a_set_tick_max_time_s);
private:
  const raw_data_t<irs_u8>* mp_data;
  bool m_flip_data_enabled;
  size_type m_value_size;
  double m_coefficient;
  raw_data_t<double>* mp_samples;
  size_type m_pos;
  size_type m_sample_count;
  bool m_completed;
  size_type m_delta_tick;
  timer_t m_tick_timer;
};

bool data_to_values_test();

template <class data_t, class iterator_t>
class accumulate_asynch_t
{
public:
  typedef size_t size_type;
  typedef data_t value_type;
  accumulate_asynch_t();
  void add(data_t a_val);
  void add(iterator_t ap_begin, iterator_t ap_end);
  data_t get() const;
  void clear();
  // Вовзращает статус завершенности
  bool completed() const;
  // Элементарное действие
  void tick();
  // Прерывание операции
  void abort();
  // Установка времени тика
  void set_tick_max_time(const double a_set_tick_max_time_s);
private:
  value_type m_sum;
  bool m_completed;
  size_type m_delta_tick;
  timer_t m_tick_timer;
  iterator_t mp_begin;
  iterator_t mp_end;
};

bool accumulate_asynch_test();

template <class data_t, class calc_t, class iterator_t>
class sko_calc_asynch_t
{
public:
  typedef size_t size_type;
  sko_calc_asynch_t(irs_u32 a_count);
  void resize(irs_u32 a_count);
  void add(data_t a_val);
  void add(iterator_t ap_begin, iterator_t ap_end);
  operator data_t() const;
  data_t relative() const;
  data_t average() const;
  void clear();
  // Вовзращает статус завершенности
  bool completed() const;
  // Элементарное действие
  void tick();
  // Прерывание операции
  void abort();
  // Установка времени тика
  void set_tick_max_time(const double a_set_tick_max_time_s);
private:
  handle_t<sko_calc_t<data_t, calc_t> > mp_sko_calc;
  bool m_completed;
  size_type m_delta_tick;
  timer_t m_tick_timer;
  iterator_t mp_begin;
  iterator_t mp_end;
};

bool sko_calc_asynch_test();

template <class value_t, class iterator_t>
class delta_calc_asynch_t
{
public:
  typedef size_t size_type;
  delta_calc_asynch_t(const size_type a_count = 100);
  void resize(const size_type a_count);
  void add(value_t a_val);
  void add(iterator_t ap_begin, iterator_t ap_end);
  value_t absolute() const;
  value_t relative() const;
  value_t average() const;
  void clear();
  // Вовзращает статус завершенности
  bool completed() const;
  // Элементарное действие
  void tick();
  // Прерывание операции
  void abort();
  // Установка времени тика
  void set_tick_max_time(const double a_set_tick_max_time_s);
private:
  delta_calc_t<value_t> m_delta_calc;
  bool m_completed;
  size_type m_delta_tick;
  timer_t m_tick_timer;
  iterator_t mp_begin;
  iterator_t mp_end;
};

bool delta_calc_asynch_test();

// Класс асинхронной фильтрации.
// Параметры шаблона: value_t - тип переменных, для расчета,
// iterator_t - тип итератора для получения входных значений,
// container_t - тип контейнера, принимающего отфильтрованные значения
template <class value_t, class iterator_t, class container_t>
class iir_filter_asynch_t
{
public:
  typedef value_t value_type;
  typedef iterator_t iterator_type;
  typedef irs_size_t size_type;
  iir_filter_asynch_t();
  template <class coef_iterator>
  iir_filter_asynch_t(
    coef_iterator a_num_coef_list_begin,
    coef_iterator a_num_coef_list_end,
    coef_iterator a_denom_coef_list_begin,
    coef_iterator a_denom_coef_list_end,
    container_t* ap_filt_values = IRS_NULL,
    const size_type a_filt_value_max_count = 1000);
  typedef deque<value_t> coef_list_type;
  typedef deque<value_t> delay_line_type;
  //void set_filter_settings(const filter_settings_t& a_filter_setting);
  template <class coef_iterator>
  void set_coefficients(
    coef_iterator a_num_coef_list_begin,
    coef_iterator a_num_coef_list_end,
    coef_iterator a_denom_coef_list_begin,
    coef_iterator a_denom_coef_list_end);
  void set_filt_value_buf(container_t* ap_filt_values,
    const size_type a_filt_value_count);
  void sync(value_t a_value);
  value_t filt(const value_t a_sample);
  void set(const value_t a_sample);
  void set(iterator_t ap_begin, iterator_t ap_end);
  value_t get() const;
  void reset();
  // Вовзращает статус завершенности
  bool completed() const;
  // Элементарное действие
  void tick();
  // Прерывание операции
  void abort();
  // Установка времени тика
  void set_tick_max_time(const double a_set_tick_max_time_s);
private:
  iir_filter_t<value_t> m_iir_filter;
  bool m_completed;
  size_type m_delta_tick;
  timer_t m_tick_timer;
  iterator_t mp_begin;
  iterator_t mp_end;
  container_t* mp_filt_values;
  size_type m_filt_value_max_count;
};

bool iir_filter_asynch_test();

//! \brief Класс асинхронной фильтрации фильтром с конечной импульсной
//!   характеристикой.
//!
//! Параметры шаблона: value_t - тип переменных, для расчета,
//!   iterator_t - тип итератора для получения входных значений,
//!   container_t - тип контейнера, принимающего отфильтрованные значения.
template <class value_t, class iterator_t, class container_t>
class fir_filter_asynch_t
{
public:
  typedef value_t value_type;
  typedef iterator_t iterator_type;
  typedef irs_size_t size_type;
  typedef deque<value_t> coef_list_type;
  typedef deque<value_t> delay_line_type;
  fir_filter_asynch_t();
  template <class coef_iterator_t>
  fir_filter_asynch_t(coef_iterator_t a_coef_list_begin,
    coef_iterator_t a_coef_list_end);
  template <class coef_iterator_t>
  void set_coefficients(coef_iterator_t a_coef_list_begin,
    coef_iterator_t a_coef_list_end);
  void set_filt_value_buf(container_t* ap_filt_values,
    const size_type a_filt_value_count);
  void sync(value_t a_value);
  value_t filt(const value_t a_sample);
  void set(const value_t a_sample);
  void set(iterator_t ap_begin, iterator_t ap_end);
  value_t get() const;
  void reset();
  //! \brief Вовзращает статус завершенности
  bool completed() const;
  //! \brief Элементарное действие
  void tick();
  //! \brief Прерывание операции
  void abort();
  //! \brief Установка времени тика
  void set_tick_max_time(const double a_set_tick_max_time_s);
private:
  fir_filter_t<value_t> m_fir_filter;
  bool m_completed;
  size_type m_delta_tick;
  size_type m_insert_point_count;
  timer_t m_tick_timer;
  iterator_t mp_begin;
  iterator_t mp_end;
  container_t* mp_filt_values;
  size_type m_filt_value_max_count;
};

//! \brief Класс для работы с мультиметром Agilent 3458A в режиме дискретизации
class agilent_3458a_digitizer_t: public mxmultimeter_t
{
public:
  typedef irs_size_t size_type;
  typedef string_t string_type;
  typedef long double math_type;
  typedef irs_i16 short_int_sample_format_type;
  typedef irs_i32 double_int_sample_format_type;
  typedef data_to_values_t data_to_values_type;
  typedef accumulate_asynch_t<double, raw_data_t<double>::pointer>
    accumulate_asynch_type;
  typedef accumulate_asynch_t<math_type, vector<math_type>::iterator>
    filter_coef_sum_asynch_type;
  typedef sko_calc_asynch_t<double, double,
    raw_data_t<double>::pointer> sko_calc_asynch_type;
  typedef delta_calc_asynch_t<double, raw_data_t<double>::pointer>
    delta_calc_asynch_type;
  typedef iir_filter_asynch_t<math_type, raw_data_t<double>::pointer,
    raw_data_t<double> > iir_filter_asynch_type;
  typedef fir_filter_asynch_t<math_type, raw_data_t<double>::pointer,
    raw_data_t<double> > fir_filter_asynch_type;
  //! \brief Конструктор
  agilent_3458a_digitizer_t(
    irs::hardflow_t* ap_hardflow,
    const filter_settings_t& a_filter_settings,
    const double a_sampling_time_s = 25e-6,
    const double a_interval_s = 30.0
  );
  //! \brief Деструктор
  ~agilent_3458a_digitizer_t();
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  //! \brief Установить режим измерения постоянного напряжения
  virtual void set_dc();
  //! \brief Установить режим измерения переменного напряжения
  virtual void set_ac();
  //! \brief Установить положителный фронт запуска
  virtual void set_positive();
  //! \brief Установить отрицательный фронт канала
  virtual void set_negative();
  //! \brief Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  //! \brief Чтение напряжения
  virtual void get_voltage(double *voltage);
  //! \brief Чтения силы тока
  virtual void get_current(double *current);
  //! \brief Чтение сопротивления
  virtual void get_resistance2x(double *resistance);
  //! \brief Чтение сопротивления
  virtual void get_resistance4x(double *resistance);
  //! \brief Чтение частоты
  virtual void get_frequency(double *frequency);
  //! \brief Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average);
  //! \brief Чтение фазового сдвига
  virtual void get_phase(double *phase);
  //! \brief Чтение временного интервала
  virtual void get_time_interval(double *time_interval);
  //! \brief Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval); 
  //! \brief Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  //! \brief Чтение статуса текущей операции
  virtual meas_status_t status();
  //! \brief Прерывание текущей операции
  virtual void abort();
  //! \brief Элементарное действие
  virtual void tick();
  //! \brief Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc);
  //! \brief Установка времени интегрирования в c
  virtual void set_aperture(double aperture);
  //! \brief Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth);
  //! \brief Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  //! \brief Устсновка уровня запуска канала
  virtual void set_start_level(double level);
  //! \brief Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief Установка автоматического выбора диапазона измерений
  virtual void set_range_auto();
private:
  void commands_to_buf_send();
  void read_data_tick();
  void write_data_tick();
  void calc_tick();
  void set_interval();
  void set_sampling_time();
  void set_filter_settings();
  void set_filter_impulse_response_type();
  void set_window_function_form();
  void set_sample_format();
  void set_range();
  void set_filtered_values_enabled();
  void set_coef_filter();
  void calc_coef_fir_filter();
  void filter_start();
  void filter_tick();
  bool filter_completed();
  math_type filter_get();
  //void filtered_values_normalize();
  irs::hardflow_t* mp_hardflow;
  enum process_t {
    process_wait,
    process_read_data,
    process_write_data,
    process_get_value,
    process_calc,
    process_set_settings,
    process_get_coefficient
  };
  process_t m_process;
  filter_impulse_response_type_t m_filter_impulse_response_type;
  filter_settings_t m_filter_settings;
  window_function_form_t m_window_function_form;
  vector<math_type> m_fir_filter_coefficients;
  iir_filter_t<math_type> m_iir_filter;
  // Команды при инициализации
  raw_data_t<irs_u8> m_command_terminator;
  vector<irs_string_t> m_commands;
  raw_data_t<irs_u8> m_buf_send;
  raw_data_t<irs_u8> m_buf_receive;
  raw_data_t<double> m_samples;
  raw_data_t<double> m_samples_for_user;
  raw_data_t<double> m_filtered_values;
  raw_data_t<double> m_filtered_values_for_user;
  const double m_nplc_coef;
  const double m_sampling_time_default;
  const double m_interval_default;
  const double m_range_default;
  const size_type m_iscale_byte_count;
  size_type m_sample_size;
  double m_sampling_time;
  double m_interval;
  event_t m_settings_change_event;

  double m_new_interval;
  event_t m_interval_change_event;
  generator_events_t m_interval_change_gen_events;

  double m_new_sampling_time;
  event_t m_sampling_time_s_change_event;
  generator_events_t m_sampling_time_s_change_gen_events;

  filter_settings_t m_new_filter_settings;
  event_t m_filter_settings_change_event;
  generator_events_t m_filter_settings_change_gen_events;

  filter_impulse_response_type_t m_new_filter_impulse_response_type;
  event_t m_filter_impulse_response_type_change_event;
  generator_events_t m_filter_impulse_response_type_change_gen_events;

  window_function_form_t m_new_window_function_form;
  event_t m_window_function_form_change_event;
  generator_events_t m_window_function_form_change_gen_events;

  double m_new_range;
  event_t m_range_change_event;
  generator_events_t m_range_change_gen_events;

  mul_sample_format_t m_new_sample_format;
  event_t m_sample_format_change_event;
  generator_events_t m_sample_format_change_gen_events;

  bool m_calc_filtered_values_enabled;
  bool m_new_calc_filtered_values_enabled;
  event_t m_calc_filtered_values_enabled_change_event;
  generator_events_t m_calc_filtered_values_enabled_change_gen_events;

  double m_tick_max_time_s;
  size_type m_need_receive_data_size;
  bool m_set_settings_complete;
  bool m_coefficient_receive_ok;
  double m_coefficient;
  double* mp_value;
  meas_status_t m_status;
  irs::timer_t m_delay_timer;
  data_to_values_type m_data_to_values;
  accumulate_asynch_type m_accumulate_asynch;
  filter_coef_sum_asynch_type m_filter_coef_sum_asynch;
  sko_calc_asynch_type m_sko_calc_asynch;
  double m_sko_for_user;
  double m_sko_relative_for_user;
  delta_calc_asynch_type m_delta_calc_asynch;
  double m_delta_absolute_for_user;
  double m_delta_relative_for_user;
  iir_filter_asynch_type m_iir_filter_asynch;
  fir_filter_asynch_type m_fir_filter_asynch;
}; // class agilent_3458a_digitizer_t

template <class SAMLE_TYPE>
void multimeter_data_to_sample_array(const raw_data_t<irs_u8>& a_data,
  const double a_coefficient, raw_data_t<double>* ap_samples)
{
  typedef size_t size_type;
  const size_type sample_count = a_data.size()/sizeof(SAMLE_TYPE);
  ap_samples->resize(sample_count);
  for (size_type samples_i = 0; samples_i < sample_count; samples_i++) {
    typedef SAMLE_TYPE mul_data_t;
    const mul_data_t multim_value = *reinterpret_cast<const mul_data_t*>(
      (a_data.data() + (samples_i*sizeof(SAMLE_TYPE))));
    const double sample = flip_data(multim_value)*a_coefficient;
    //ap_samples->resize(ap_samples->size() + 1);
    (*ap_samples)[samples_i] = sample;
  }
}

// class accumulate_asynch_t
template <class data_t, class iterator_t>
accumulate_asynch_t<data_t, iterator_t>::accumulate_asynch_t():
  m_sum(0),
  m_completed(true),
  m_delta_tick(1000),
  m_tick_timer(make_cnt_s(0.001)),
  mp_begin(),
  mp_end(mp_begin)
{
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::add(data_t a_val)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  m_sum += a_val;
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::add(
  iterator_t ap_begin, iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "Итератор начала должен быть меньше итератора конца");
  mp_begin = ap_begin;
  mp_end = ap_end;
  m_completed = false;
}

template <class data_t, class iterator_t>
typename accumulate_asynch_t<data_t, iterator_t>::value_type
accumulate_asynch_t<data_t, iterator_t>::get() const
{
  return m_sum;
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::clear()
{
  m_sum = 0;
}

template <class data_t, class iterator_t>
bool accumulate_asynch_t<data_t, iterator_t>::completed() const
{
  return m_completed;
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::tick()
{
  if (!m_completed) {
    m_tick_timer.start();
    while (!m_tick_timer.check() && !m_completed) {
      size_type count = 0;
      while ((count < m_delta_tick) && (mp_begin != mp_end)) {
        m_sum += *mp_begin;
        ++mp_begin;
        count++;
      }
      m_completed = (mp_begin == mp_end);
    }
  } else {
    // Операция завершена
  }
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::abort()
{
  if (!m_completed) {
    mp_end = mp_begin;
    clear();
    m_completed = true;
  } else {
    // Операция уже завершена
  }
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "Максимальное время тика должно быть больше нуля");
  m_tick_timer.set(make_cnt_s(a_set_tick_max_time_s));
}

// class sko_calc_asynch_t
template <class data_t, class calc_t, class iterator_t>
sko_calc_asynch_t<data_t, calc_t, iterator_t>::sko_calc_asynch_t(irs_u32 a_count
):
  mp_sko_calc(new sko_calc_t<data_t, calc_t>(a_count)),
  m_completed(true),
  m_delta_tick(1000),
  m_tick_timer(make_cnt_s(0.001)),
  mp_begin(),
  mp_end(mp_begin)
{
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::resize(irs_u32 a_count)
{
  mp_sko_calc.reset(new sko_calc_t<data_t, calc_t>(a_count));
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::add(data_t a_val)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  mp_sko_calc->add(a_val);
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::add(iterator_t ap_begin,
  iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "Итератор начала должен быть меньше итератора конца");
  mp_begin = ap_begin;
  mp_end = ap_end;
  m_completed = false;
}

template <class data_t, class calc_t, class iterator_t>
sko_calc_asynch_t<data_t, calc_t, iterator_t>::operator data_t() const
{
  return mp_sko_calc->operator data_t();
}

template <class data_t, class calc_t, class iterator_t>
data_t sko_calc_asynch_t<data_t, calc_t, iterator_t>::relative() const
{
  return mp_sko_calc->relative();
}

template <class data_t, class calc_t, class iterator_t>
data_t sko_calc_asynch_t<data_t, calc_t, iterator_t>::average() const
{
  return mp_sko_calc->average();
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::clear()
{
  mp_sko_calc->clear();
}

template <class data_t, class calc_t, class iterator_t>
bool sko_calc_asynch_t<data_t, calc_t, iterator_t>::completed() const
{
  return m_completed;
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::tick()
{
  if (!m_completed) {
    m_tick_timer.start();
    while (!m_tick_timer.check() && !m_completed) {
      size_type count = 0;
      while ((count < m_delta_tick) && (mp_begin != mp_end)) {
        mp_sko_calc->add(*mp_begin);
        ++mp_begin;
        count++;
      }
      m_completed = (mp_begin == mp_end);
    }
  } else {
    // Операция завершена
  }
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::abort()
{
  if (!m_completed) {
    mp_sko_calc->clear();
    mp_begin = mp_end;
    m_completed = true;
  } else {
    // Операция уже завершена
  }
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "Максимальное время тика должно быть больше нуля");
  m_tick_timer.set(make_cnt_s(a_set_tick_max_time_s));
}

bool sko_calc_asynch_test();

// class delta_calc_asynch_t
template <class value_t, class iterator_t>
delta_calc_asynch_t<value_t, iterator_t>::delta_calc_asynch_t(
  const size_type a_count
):
  m_delta_calc(a_count),
  m_completed(true),
  m_delta_tick(1000),
  m_tick_timer(make_cnt_s(0.001)),
  mp_begin(),
  mp_end(mp_begin)
{
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::resize(const size_type a_count)
{
  m_delta_calc.resize(a_count);
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::add(value_t a_val)
{
  IRS_LIB_ASSERT(m_completed);
  m_delta_calc.add(a_val);
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::add(iterator_t ap_begin,
  iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  IRS_LIB_ERROR_IF(mp_begin > mp_end, ec_standard, "Итератор начала должен"
    " быть меньше итератора конца");
  mp_begin = ap_begin;
  mp_end = ap_end;
  m_completed = false;
}

template <class value_t, class iterator_t>
value_t delta_calc_asynch_t<value_t, iterator_t>::absolute() const
{
  return m_delta_calc.absolute();
}

template <class value_t, class iterator_t>
value_t delta_calc_asynch_t<value_t, iterator_t>::relative() const
{
  return m_delta_calc.relative();
}

template <class value_t, class iterator_t>
value_t delta_calc_asynch_t<value_t, iterator_t>::average() const
{
  return m_delta_calc.average();
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::clear()
{
  m_delta_calc.clear();
}

template <class value_t, class iterator_t>
bool delta_calc_asynch_t<value_t, iterator_t>::completed() const
{
  return m_completed;
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::tick()
{
  if (!m_completed) {
    m_tick_timer.start();
    while (!m_tick_timer.check() && !m_completed) {
      size_type count = 0;
      while ((count < m_delta_tick) && (mp_begin != mp_end)) {
        m_delta_calc.add(*mp_begin);
        ++mp_begin;
        count++;
      }
      m_completed = (mp_begin == mp_end);
    }
  } else {
    // Операция завершена
  }
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::abort()
{
  if (!m_completed) {
    mp_end = mp_begin;
    m_completed = true;
  } else {
    // Операция уже завершена
  }
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "Максимальное время тика должно быть больше нуля");
  m_tick_timer.set(make_cnt_s(a_set_tick_max_time_s));
}

// class iir_filter_asynch_t
template <class value_t, class iterator_t, class container_t>
iir_filter_asynch_t<value_t, iterator_t, container_t>::iir_filter_asynch_t():
  m_iir_filter(),
  m_completed(true),
  m_delta_tick(1000),
  m_tick_timer(make_cnt_s(0.001)),
  mp_begin(),
  mp_end(mp_begin),
  mp_filt_values(IRS_NULL),
  m_filt_value_max_count(1000)
{
}

template <class value_t, class iterator_t, class container_t>
template <class coef_iterator>
iir_filter_asynch_t<value_t, iterator_t, container_t>::iir_filter_asynch_t(
  coef_iterator a_num_coef_list_begin,
  coef_iterator a_num_coef_list_end,
  coef_iterator a_denom_coef_list_begin,
  coef_iterator a_denom_coef_list_end,
  container_t* ap_filt_values,
  const size_type a_filt_value_max_count
):
  m_iir_filter(a_num_coef_list_begin, a_num_coef_list_end,
    a_denom_coef_list_begin, a_denom_coef_list_end),
  m_completed(true),
  m_delta_tick(1000),
  m_tick_timer(make_cnt_s(0.001)),
  mp_begin(),
  mp_end(mp_begin),
  mp_filt_values(IRS_NULL),
  m_filt_value_max_count(1000)
{
  set_filt_value_buf(ap_filt_values, a_filt_value_max_count);
}

/*template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set_filter_settings(
  const filter_settings_t& a_filter_setting)
{
  m_iir_filter.set_filter_settings(a_filter_setting);
}*/

template <class value_t, class iterator_t, class container_t>
template <class coef_iterator>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set_coefficients(
  coef_iterator a_num_coef_list_begin,
  coef_iterator a_num_coef_list_end,
  coef_iterator a_denom_coef_list_begin,
  coef_iterator a_denom_coef_list_end)
{
  m_iir_filter.set_coefficients(a_num_coef_list_begin, a_num_coef_list_end,
    a_denom_coef_list_begin, a_denom_coef_list_end);
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set_filt_value_buf(
  container_t* ap_filt_values,
  const size_type a_filt_value_count)
{
  mp_filt_values = ap_filt_values;
  if (mp_filt_values) {
    mp_filt_values->clear();
  } else {
    // Пользователь удалил буфер
  }
  m_filt_value_max_count = a_filt_value_count;
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::sync(
  value_t a_value)
{
  m_iir_filter.sync(a_value);
}

template <class value_t, class iterator_t, class container_t>
value_t iir_filter_asynch_t<value_t, iterator_t, container_t>::filt(
  const value_t a_sample)
{
  m_iir_filter.filt(a_sample);
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set(
  const value_t a_sample)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  m_iir_filter.set(a_sample);
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set(
  iterator_t ap_begin, iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "Итератор начала должен быть меньше итератора конца");
  mp_begin = ap_begin;
  mp_end = ap_end;
  m_completed = false;
}

template <class value_t, class iterator_t, class container_t>
value_t iir_filter_asynch_t<value_t, iterator_t, container_t>::get() const
{
  return m_iir_filter.get();
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::reset()
{
  m_iir_filter.reset();
}

template <class value_t, class iterator_t, class container_t>
bool iir_filter_asynch_t<value_t, iterator_t, container_t>::completed() const
{
  return m_completed;
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::tick()
{
  if (!m_completed) {
    m_tick_timer.start();
    while (!m_tick_timer.check() && !m_completed) {
      size_type count = 0;
      while ((count < m_delta_tick) && (mp_begin != mp_end) && !m_completed) {
        m_iir_filter.set(*mp_begin);
        if (mp_filt_values && (m_filt_value_max_count > 0)) {
          if (mp_filt_values->size() >= m_filt_value_max_count) {
            mp_filt_values->erase(mp_filt_values->begin());
          } else {
            // Размер еще не достиг максимума
          }
          mp_filt_values->insert(mp_filt_values->end(),
            static_cast<typename container_t::value_type>(m_iir_filter.get()));
        } else {
          // Пользователь не установил буфер фильтрованных значений
        }
        ++mp_begin;
        count++;
      }
      m_completed = (mp_begin == mp_end);
    }
  } else {
    // Операция завершена
  }
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::abort()
{
  if (!m_completed) {
    mp_end = mp_begin;
    m_completed = true;
  } else {
    // Ничего не делаем
  }
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "Максимальное время тика должно быть больше нуля");
  m_tick_timer.set(make_cnt_s(a_set_tick_max_time_s));
}

// class fir_filter_asynch_t
template <class value_t, class iterator_t, class container_t>
fir_filter_asynch_t<value_t, iterator_t, container_t>::fir_filter_asynch_t():
  m_fir_filter(),
  m_completed(true),
  m_delta_tick(1000),
  m_insert_point_count(100000),
  m_tick_timer(make_cnt_s(0.001)),
  mp_begin(),
  mp_end(mp_begin),
  mp_filt_values(IRS_NULL),
  m_filt_value_max_count(1000)
{
}

template <class value_t, class iterator_t, class container_t>
template <class coef_iterator_t>
fir_filter_asynch_t<value_t, iterator_t, container_t>::fir_filter_asynch_t(
  coef_iterator_t a_coef_list_begin,
  coef_iterator_t a_coef_list_end
):
  m_fir_filter(a_coef_list_begin, a_coef_list_end),
  m_completed(true),
  m_delta_tick(1000),
  m_insert_point_count(100000),
  m_tick_timer(make_cnt_s(0.001)),
  mp_begin(),
  mp_end(mp_begin),
  mp_filt_values(IRS_NULL),
  m_filt_value_max_count(1000)
{
}

template <class value_t, class iterator_t, class container_t>
template <class coef_iterator_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::set_coefficients(
  coef_iterator_t a_coef_list_begin,
  coef_iterator_t a_coef_list_end)
{
  m_fir_filter.set_coefficients(a_coef_list_begin, a_coef_list_end);
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::set_filt_value_buf(
  container_t* ap_filt_values,
  const size_type a_filt_value_count)
{
  mp_filt_values = ap_filt_values;
  if (mp_filt_values) {
    mp_filt_values->clear();
    m_delta_tick = 100;
  } else {
    m_delta_tick = 1000;
  }
  m_filt_value_max_count = a_filt_value_count;
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::sync(
  value_t a_value)
{
  m_fir_filter.sync(a_value);
}

template <class value_t, class iterator_t, class container_t>
typename fir_filter_asynch_t<value_t, iterator_t, container_t>::value_type
fir_filter_asynch_t<value_t, iterator_t, container_t>::filt(
  const value_t a_sample)
{
  m_fir_filter.filt(a_sample);
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::set(
  const value_t a_sample)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  m_fir_filter.set(a_sample);
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::set(
  iterator_t ap_begin, iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "Предыдущая операция еще не завершена");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "Итератор начала должен быть меньше итератора конца");
  mp_begin = ap_begin;
  mp_end = ap_end;
  m_completed = false;
}

template <class value_t, class iterator_t, class container_t>
typename fir_filter_asynch_t<value_t, iterator_t, container_t>::value_type
fir_filter_asynch_t<value_t, iterator_t, container_t>::get() const
{
  return m_fir_filter.get();
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::reset()
{
  m_fir_filter.reset();
}

template <class value_t, class iterator_t, class container_t>
bool fir_filter_asynch_t<value_t, iterator_t, container_t>::completed() const
{
  return m_completed;
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::tick()
{
  if (!m_completed) {
    m_tick_timer.start();
    while (!m_tick_timer.check() && !m_completed) {
      if (mp_filt_values && (m_filt_value_max_count > 0)) {
        size_type count = 0;
        while ((count < m_delta_tick) && (mp_begin != mp_end) && !m_completed) {
          m_fir_filter.set(*mp_begin);
          if (mp_filt_values && (m_filt_value_max_count > 0)) {
            if (mp_filt_values->size() >= m_filt_value_max_count) {
              mp_filt_values->erase(mp_filt_values->begin());
            } else {
              // Размер еще не достиг максимума
            }
            mp_filt_values->insert(mp_filt_values->end(),
              static_cast<typename container_t::value_type>(
              m_fir_filter.get()));
          } else {
            // Пользователь не установил буфер фильтрованных значений
          }
          ++mp_begin;
          count++;
        }
      } else {
        iterator_t end = mp_begin;
        advance(end, min(static_cast<size_type>(distance(mp_begin, mp_end)),
          m_insert_point_count));
        m_fir_filter.set(mp_begin, end);
        mp_begin = end;
      }
      m_completed = (mp_begin == mp_end);
    }
  } else {
    // Операция завершена
  }
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::abort()
{
  if (!m_completed) {
    mp_end = mp_begin;
    m_completed = true;
  } else {
    // Ничего не делаем
  }
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "Максимальное время тика должно быть больше нуля");
  m_tick_timer.set(make_cnt_s(a_set_tick_max_time_s));
}

#endif // IRS_FULL_STDCPPLIB_SUPPORT

//! \brief Класс для работы с мультиметром В7-78/1
class v7_78_1_t: public mxmultimeter_t
{
  //! \brief Тип для текущего режима
  typedef enum _ma_mode_t {
    ma_mode_start,
    ma_mode_macro,
    ma_mode_commands,
    ma_mode_commands_wait,
    ma_mode_get_value,
    ma_mode_auto_calibration,
    ma_mode_auto_calibration_wait
  } ma_mode_t;
  //! \brief Тип для текущей команды
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
  //! \brief Тип для индексов
  typedef irs_i32 index_t;


  //static const char m_volt_ac_nplcycles = "DETector:BANDwidth 20";

  multimeter_mode_type_t m_mul_mode_type;

  //! \brief Команды при инициализации
  vector<irs::string> f_init_commands;
  //! \brief Индекс команды установки времени интегрирования для
  //!  постоянного напряжения
  index_t m_nplc_voltage_dc_index;
  //! \brief Индекс команды установки полосы фильтра для переменного напряжения
  index_t m_band_width_voltage_ac_index;
  //! \brief Команды чтения значения при произвольных настройках
  vector<irs::string> m_get_value_commands;
  //! \brief Команды при чтении напряжения
  vector<irs::string> m_get_voltage_dc_commands;
  vector<irs::string> m_get_voltage_ac_commands;

  //! \brief Индекс команды установки времени интегрирования для сопротивления
  index_t m_nplc_resistance_2x_index;
  index_t m_nplc_resistance_4x_index;
  //! \brief Команды при чтении сопротивления
  vector<irs::string> m_get_resistance_2x_commands;
  vector<irs::string> m_get_resistance_4x_commands;

  //! \brief Индекс команды установки времени интегрирования для тока
  index_t m_nplc_current_dc_index;
  index_t m_nplc_current_ac_index;
  //! \brief Команды при чтении тока
  vector<irs::string> m_get_current_dc_commands;
  vector<irs::string> m_get_current_ac_commands;

  //! \brief Индекс команды установки времени счета
  index_t m_aperture_frequency_index;
  //! \brief Команды при чтении частоты
  vector<irs::string> m_get_frequency_commands;

  //! \brief Дескриптор канала mxifa
  void *f_handle;
  //! \brief Ощибка создания
  irs_bool f_create_error;
  //! \brief Текущий режим работы
  ma_mode_t f_mode;
  //! \brief Текущий макрорежим работы
  macro_mode_t f_macro_mode;
  //! \brief Статус текущей операции
  meas_status_t f_status;
  //! \brief Текущая команда
  ma_command_t f_command;
  //! \brief Указатель на переменную измеряемой величины,
  //!  при произвольном типе измерения
  double* mp_value;
  //! \brief Указатель на перменную напряжение пользователя
  double* mp_voltage;
  //! \brief Указатель на перменную сопротивление пользователя
  double* mp_resistance;
  //! \brief Указатель на перменную ток пользователя
  double* mp_current;
  //! \brief Указатель на перменную частота пользователя
  double* mp_frequency;
  //! \brief Запрос на прерывание операции
  irs_bool f_abort_request;
  //! \brief Буфер приема
  irs_u8 f_read_buf[ma_read_buf_size];
  //! \brief Текущая позиция в буфере приема
  index_t f_read_pos;
  //! \brief Текущая команда
  irs::string f_cur_mul_command;
  //! \brief Команды для мультиметра
  vector<irs::string> *f_mul_commands;
  //! \brief Индекс команд для мультиметра
  index_t f_mul_commands_index;
  // Предыдущая команда
  //ma_command_t f_command_prev;
  //! \brief Указатель на переменную в которую будет считано значение
  double *f_value;
  //! \brief Выполнить чтение параметра
  irs_bool f_get_parametr_needed;
  //! \brief Время таймаута операций
  counter_t f_oper_time;
  //! \brief Таймаут операций
  counter_t f_oper_to;
  //! \brief Время таймаута калибровки
  counter_t f_acal_time;
  //! \brief Таймаут калибровки
  counter_t f_acal_to;

  bool m_current_volt_dc_meas;
  //! \brief Запрещение конструктора по умолчанию
  v7_78_1_t();
public:
  //! \brief Конструктор
  v7_78_1_t(
    mxifa_ch_t channel,
    gpib_addr_t address,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  //! \brief Деструктор
  ~v7_78_1_t();
  //! \brief Установить режим измерения постоянного напряжения
  virtual inline void set_dc();
  //! \brief Установить режим измерения переменного напряжения
  virtual inline void set_ac();
  //! \brief Установить положителный фронт запуска
  virtual void set_positive();
  //! \brief Установить отрицательный фронт канала
  virtual void set_negative();
  //! \brief Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  //! \brief Чтение напряжения
  virtual void get_voltage(double *voltage);
  //! \brief Чтения силы тока
  virtual void get_current(double *current);
  //! \brief Чтение сопротивления
  virtual void get_resistance2x(double *resistance);
  //! \brief Чтение сопротивления
  virtual void get_resistance4x(double *resistance);
  //! \brief Чтение частоты
  virtual void get_frequency(double *frequency);
  //! \brief Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average);
  //! \brief Чтение фазового сдвига
  virtual void get_phase(double *phase);
  //! \brief Чтение временного интервала
  virtual void get_time_interval(double *time_interval);
  //! \brief Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);

  //! \brief Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  //! \brief Чтение статуса текущей операции
  virtual meas_status_t status();
  //! \brief Прерывание текущей операции
  virtual void abort();
  //! \brief Элементарное действие
  virtual void tick();
  //! \brief Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc);
  //! \brief Установка времени интегрирования в c
  virtual void set_aperture(double aperture);
  //! \brief Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth);
  //! \brief Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  //! \brief Устсновка уровня запуска канала
  virtual void set_start_level(double level);
  //! \brief Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief Установка автоматического выбора диапазона измерений
  virtual void set_range_auto();
private:
  //! \brief Установка временного интервала измерения
  void set_time_interval_meas(const double a_time_interval_meas);
};
//! \brief Установить режим измерения постоянного напряжения
inline void irs::v7_78_1_t::set_dc()
{
  m_meas_type = DC_MEAS;
}
//! \brief Установить режим измерения переменного напряжения
inline void irs::v7_78_1_t::set_ac()
{
  m_meas_type = AC_MEAS;
}

//! \ingroup drivers_group
//! \brief Класс для работы с мультиметром agilent 34420a
class agilent_34420a_t: public mxmultimeter_t
{
  typedef irs::hardflow::fixed_flow_t::status_t fixed_flow_status_type;
  //! \brief Тип для текущего режима
  enum ma_mode_t {
    ma_mode_start,
    ma_mode_macro,
    ma_mode_commands,
    ma_mode_commands_wait,
    ma_mode_get_value,
    ma_mode_get_value_wait,
    ma_mode_auto_calibration,
    ma_mode_auto_calibration_wait
  };
  //! \brief Тип для текущей команды
  enum ma_command_t {
    mac_free,
    mac_get_value,
    mac_get_voltage,
    mac_get_resistance,
    mac_set_params,
    mac_auto_calibration
  };
  enum macro_mode_t {
    //! \brief Режим чтения текущей измеряемой величины.
    macro_mode_get_value,
    //! \brief Режим отправки команд для чтения напряжения.
    macro_mode_get_voltage,
    //! \brief Режим отправки команд для чтения сопротивления.
    macro_mode_get_resistance,
    //! \brief Режим посылки команд для установки параметров.
    macro_mode_set_params,
    //! \brief Режим завершения отправки команд и чтения значения.
    macro_mode_stop
  };
  //! \brief Режим инициализации
  enum init_mode_t {
    im_start,
    im_write_command,
    im_next_command,
    im_stop
  };

  enum {
    sample_size = 16
  };

  enum {
    read_timeout_s = 20
  };

  enum {
    fixed_flow_read_timeout_delta_s = 2
  };

  enum meas_type_t{DC_MEAS, AC_MEAS} m_meas_type;
  enum resistance_meas_type_t{RES_MEAS_2x, RES_MEAS_4x} m_res_meas_type;
  //! \brief Тип для индексов
  typedef irs_i32 index_t;


  //static const char m_volt_ac_nplcycles = "DETector:BANDwidth 20";

  multimeter_mode_type_t m_mul_mode_type;

  //! \brief Команды при инициализации
  vector<irs::string> m_init_commands;
  //! \brief Команды чтения значения при произвольных настройках
  vector<irs::string> m_get_value_commands;
  //! \brief Команда включения/выключения аналогового фильтра
  irs::string m_analog_filter;
  irs::string m_configure_voltage_dc;
  index_t m_configure_voltage_dc_index;
  //! \brief Команда установки диапазона для напряжения
  irs::string m_range_voltage_dc;
  //! \brief Часть команды включения/выключения аналогового фильтра,
  //!   отображающая статус
  irs::string m_analog_filter_status;
  //! \brief Индекс команды включения/выключения аналогового фильтра для
  //!   напряжения
  index_t m_analog_filter_voltage_dc_index;
  //! \brief Индекс команды установки времени интегрирования для
  //!  постоянного напряжения
  index_t m_nplc_voltage_dc_index;
  //! \brief Команды при чтении напряжения
  vector<irs::string> m_get_voltage_dc_commands;

  irs::string m_configure_resistance_2x;
  index_t m_configure_resistance_2x_index;
  irs::string m_range_resistance_2x;
  //! \brief Индекс команды установки времени интегрирования для сопротивления
  index_t m_nplc_resistance_2x_index;
  //! \brief Команды при чтении сопротивления
  vector<irs::string> m_get_resistance_2x_commands;

  irs::string m_configure_resistance_4x;
  index_t m_configure_resistance_4x_index;
  irs::string m_range_resistance_4x;
  //! \brief Индекс команды установки времени интегрирования для сопротивления
  index_t m_nplc_resistance_4x_index;
  vector<irs::string> m_get_resistance_4x_commands;
  //! \brief Команда выбора канала
  index_t m_select_channel_dc_index;
  irs::string m_select_channel;
  vector<irs::string> m_front;
  vector<irs::string> m_channel;
  size_t m_current_channel;
  vector<irs::string> m_select_channel_command;
  //! \brief Команды для установки параметров
  vector<irs::string> m_set_params_commands;
  // Интерфейс передачи
  irs::hardflow_t* mp_hardflow;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  //! \brief Ощибка создания
  bool m_create_error;
  //! \brief Текущий режим работы
  ma_mode_t m_mode;
  //! \brief Текущий макрорежим работы
  macro_mode_t m_macro_mode;
  //! \brief Статус текущей операции
  meas_status_t m_status;
  //! \brief Текущая команда
  ma_command_t m_command;
  //! \brief Указатель на переменную измеряемой величины,
  //!  при произвольном типе измерения
  double* mp_value;
  //! \brief Запрос на прерывание операции
  bool m_abort_request;
  //! \brief Буфер приема
  irs_u8 m_read_buf[ma_read_buf_size];
  //! \brief Текущая позиция в буфере приема
  index_t m_read_pos;
  //! \brief Текущая команда
  irs::string m_cur_mul_command;
  //! \brief Команды для мультиметра
  vector<irs::string> *mp_mul_commands;
  //! \brief Индекс команд для мультиметра
  index_t m_mul_commands_index;
  // Предыдущая команда
  //ma_command_t f_command_prev;
  //! \brief Указатель на переменную в которую будет считано значение
  //double *mp_value;
  //! \brief Выполнить чтение параметра
  irs_bool m_get_parametr_needed;
  //! \brief Время таймаута операций
  counter_t m_oper_time;
  //! \brief Таймаут операций
  counter_t m_oper_to;
  //! \brief Время таймаута калибровки
  counter_t m_acal_time;
  //! \brief Таймаут калибровки
  counter_t m_acal_to;
  // Таймер записи команд инициализации
  irs::timer_t m_init_timer;
  // Режим инициализации
  init_mode_t m_init_mode;
  // Индекс команды инициализации
  index_t m_ic_index;
  bool m_current_volt_dc_meas;
  //! \brief Запрещение конструктора по умолчанию
  agilent_34420a_t();
  // Отправка команд инициализации в мультиметр
  void initialize_tick();
public:
  //! \brief Конструктор
  agilent_34420a_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  //! \brief Деструктор
  ~agilent_34420a_t();
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  //! \brief Установить режим измерения постоянного напряжения
  virtual inline void set_dc();
  //! \brief Установить режим измерения переменного напряжения
  virtual inline void set_ac();
  //! \brief Установить положителный фронт запуска
  virtual void set_positive();
  //! \brief Установить отрицательный фронт канала
  virtual void set_negative();
  //! \brief Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  //! \brief Чтение напряжения
  virtual void get_voltage(double *voltage);
  //! \brief Чтения силы тока
  virtual void get_current(double *current);
  //! \brief Чтение сопротивления
  virtual void get_resistance2x(double *resistance);
  //! \brief Чтение сопротивления
  virtual void get_resistance4x(double *resistance);
  //! \brief Чтение частоты
  virtual void get_frequency(double *frequency);
  //! \brief Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average);
  //! \brief Чтение фазового сдвига
  virtual void get_phase(double *phase);
  //! \brief Чтение временного интервала
  virtual void get_time_interval(double *time_interval);
  //! \brief Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);
  //! \brief Чтение отношения напряжений на 1-м и 2-м каналах
  virtual void get_voltage_ratio(double *voltage_ratio);
  //! \brief Выбор канала для последующих измерений
  virtual void select_channel(size_t channel);

  //! \brief Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  //! \brief Чтение статуса текущей операции
  virtual meas_status_t status();
  //! \brief Прерывание текущей операции
  virtual void abort();
  //! \brief Элементарное действие
  virtual void tick();
  //! \brief Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc);
  //! \brief Установка времени интегрирования в c
  virtual void set_aperture(double aperture);
  //! \brief Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth);
  //! \brief Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  //! \brief Устсновка уровня запуска канала
  virtual void set_start_level(double level);
  //! \brief Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief Установка автоматического выбора диапазона измерений
virtual void set_range_auto();
private:
  //! \brief Установка временного интервала измерения
  void set_time_interval_meas(const double a_time_interval_meas);
};
//! \brief Установить режим измерения постоянного напряжения
inline void irs::agilent_34420a_t::set_dc()
{
  m_meas_type = DC_MEAS;
}
//! \brief Установить режим измерения переменного напряжения
inline void irs::agilent_34420a_t::set_ac()
{
  m_meas_type = AC_MEAS;
}

//! \brief Класс для работы с частотомером электронно-счетным акип чз-85/3R
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
      m_count_inf_elem = min(a_size, static_cast<irs_u32>(m_size_buf));
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
    vector<irs::irs_string_t> mv_bad_str;
    // Возвращает сообщение(если оно сформировано)
  public:
    string_msgs_t():
      mv_bad_str()
    {
      mv_bad_str.push_back("LOC\n");
      mv_bad_str.push_back("DEG");
      mv_bad_str.push_back("Hz");
      mv_bad_str.push_back("s");
    }
    msg_status_t get_message(irs::irs_string_t& a_msg_str)
    {
      msg_status_t msg_stat = MSG_BUSY;
      const size_type line_fit_pos = find('\n');
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
        const size_type new_line_fit_pos = find('\n');
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

  deque<irs::irs_string_t> md_buf_cmds;

  buf_data_t m_buf_cmd;
  string_msgs_t m_string_msgs;

  enum command_status_t{CS_SUCCESS = 0, CS_BUSY = 1, CS_ERROR = 2};
  //! \brief Статус текущей операции
  command_status_t m_send_command_stat;
  meas_status_t m_meas_stat;

  //! \brief Указатель на пользовательскую переменную, в которую
  //!  будет сохранен результат.
  double* mp_result;
  //! \brief Тип для индексов
  typedef irs_i32 index_t;
  //! \brief Указатель на последнюю команду ИЗМЕРЕНИЙ
  vector<irs::string>* mp_last_commands;

  //! \brief Команды при инициализации
  vector<irs::string> mv_init_commands;

  //! \brief Команды при перезакгрузке мультиметра
  vector<irs::string> mv_reset_multimeter_commands;

  //! \brief Указатели на команды настроек мультиметра
  vector<irs::string*> mv_multimeter_p_commands_setting;

  //! \brief Команды при чтении значения измеряемой величины,
  //!  при произвольном типе измерения
  vector<irs::string> mv_get_value_commands;

  //! \brief Команды при чтении усредненной разности фаз
  vector<irs::string> mv_get_phase_average_commands;

  //! \brief Команды при чтении разности фаз
  vector<irs::string> mv_get_phase_commands;

  //! \brief Команды при чтении временного интервала
  vector<irs::string> mv_get_time_interval_commands;

  //! \brief Команды при чтении усредненного временного интервала
  vector<irs::string> mv_get_time_interval_average_commands;

  //! \brief Команды при чтении частоты
  vector<irs::string> mv_get_frequency_commands;

  //! \brief Команды, большие по времении выполнения
  vector<irs::string> mv_big_time_commands;

  //! \brief Команды установки времени счета
  irs::string ms_set_interval_command;

  //! \brief Команды установки входного сопротивления каналов
  irs::string ms_set_in_impedance_channel_1_command;
  irs::string ms_set_in_impedance_channel_2_command;

  //! \brief Команды установки вида связи входа канала
  irs::string ms_set_type_coupling_channel_1_command;
  irs::string ms_set_type_coupling_channel_2_command;

  //! \brief Команды установки типа фронта запуска канала
  irs::string ms_set_type_slope_channel_1_command;
  irs::string ms_set_type_slope_channel_2_command;

  //! \brief Команды установки фильтра канала
  irs::string ms_set_filter_channel_1_command;
  irs::string ms_set_filter_channel_2_command;

  //! \brief Установка уровня запуска канала
  irs::string ms_set_level_channel_1_command;
  irs::string ms_set_level_channel_2_command;

  //! \brief Запрещение конструктора по умолчанию
  akip_ch3_85_3r_t();
public:
  //! \brief Конструктор
  akip_ch3_85_3r_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  //! \brief Деструктор
  ~akip_ch3_85_3r_t();
  void connect(hardflow_t* ap_hardflow);
  //! \brief Установить режим измерения постоянного напряжения
  virtual void set_dc();
  //! \brief Установить режим измерения переменного напряжения
  virtual void set_ac();
  //! \brief Установить положителный фронт запуска
  virtual void set_positive();
  //! \brief Установить отрицательный фронт канала
  virtual void set_negative();
  //! \brief Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  //! \brief Чтение напряжения
  virtual void get_voltage(double *ap_voltage);
  //! \brief Чтения силы тока
  virtual void get_current(double *ap_current);
  //! \brief Чтение сопротивления
  virtual void get_resistance2x(double *ap_resistance);
  //! \brief Чтение сопротивления
  virtual void get_resistance4x(double *ap_resistance);

  //! \brief Чтение частоты
  virtual void get_frequency(double *ap_frequency);
  //! \brief Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *ap_phase_average);
  //! \brief Чтение фазового сдвига
  virtual void get_phase(double *ap_phase);
  //! \brief Чтение временного интервала
  virtual void get_time_interval(double *ap_time_interval);
  //! \brief Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);
  //! \brief Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration();
  //! \brief Чтение статуса текущей операции
  virtual meas_status_t status();
  //! \brief Перезагрузка с восстановлением параматров
  virtual void abort();
  //! \brief Элементарное действие
  virtual void tick();
  //! \brief Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double a_nplc);
  //! \brief Установка времени счета в cек
  virtual void set_aperture(double a_aperture);
  //! \brief Установка фильтра в герцах
  virtual void set_bandwidth(double a_bandwidth);
  //! \brief Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance);
  //! \brief Устсновка уровня запуска канала
  virtual void set_start_level(double a_level);
  //! \brief Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief Установка автоматического выбора диапазона измерений
  virtual void set_range_auto();
private:
  //! \brief Установка временного интервала измерения
  void set_time_interval_meas(const double a_time_interval_meas);
};
  // Установить режим измерения постоянного напряжения
/*inline void irs::akip_ch3_85_3r_t::set_dc()
  {m_meas_type = DC_MEAS;}
// Установить режим измерения переменного напряжения
inline void irs::akip_ch3_85_3r_t::set_ac()
  {m_meas_type = AC_MEAS;}
*/

class dummy_multimeter_t: public mxmultimeter_t
{
public:
  //! \brief Установить режим измерения постоянного напряжения
  virtual void set_dc() {}
  //! \brief Установить режим измерения переменного напряжения
  virtual void set_ac() {}
  //! \brief Установить положителный фронт запуска
  virtual void set_positive() {}
  //! \brief Установить отрицательный фронт канала
  virtual void set_negative() {}
  //! \brief Чтение значения при текущем типа измерения
  virtual void get_value(double* value) { *value = 100; }//rand(); }
  //! \brief Чтение напряжения
  virtual void get_voltage(double* voltage) { *voltage = 100; }//rand(); }
  //! \brief Чтения силы тока
  virtual void get_current(double* /*current*/) {}
  //! \brief Чтение сопротивления
  virtual void get_resistance2x(double* /*resistance*/) {}
  //! \brief Чтение сопротивления
  virtual void get_resistance4x(double* resistance)
  {
    static int i = 0;
    //*resistance = abs( rand() );
    *resistance = i++;
  }
  //! \brief Чтение частоты
  virtual void get_frequency(double* /*frequency*/) {}
  //! \brief Чтение усредненного сдвира фаз
  virtual void get_phase_average(double* /*phase_average*/) {}
  //! \brief Чтение фазового сдвига
  virtual void get_phase(double* /*phase*/) {}
  //! \brief Чтение временного интервала
  virtual void get_time_interval(double* /*time_interval*/) {}
  //! \brief Чтение усредненного временного интервала
  virtual void get_time_interval_average(double* /*ap_time_interval*/) {}

  //! \brief Запуск автокалибровки мультиметра
  virtual void auto_calibration() {}
  //! \brief Чтение статуса текущей операции
  virtual meas_status_t status() { return meas_status_success; }
  //! \brief Прерывание текущей операции
  virtual void abort() {}
  //! \brief Элементарное действие
  virtual void tick() {}
  //! \brief Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double /*nplc*/) {}
  //! \brief Установка времени интегрирования в c
  virtual void set_aperture(double /*aperture*/) {}
  //! \brief Установка полосы фильтра
  virtual void set_bandwidth(double /*bandwidth*/) {}
  //! \brief Установка входного сопротивления канала
  virtual void set_input_impedance(double /*impedance*/) {}
  //! \brief Установка уровня запуска канала
  virtual void set_start_level(double /*level*/) {}
  //! \brief Установка диапазона измерений
  virtual void set_range(type_meas_t /*a_type_meas*/, double /*a_range*/) {}
  //! \brief Установка автоматического выбора диапазона измерений
  virtual void set_range_auto() {}
  // Установка входного сопротивления канала
  //virtual void set_filter(double ) {}
};



//! \author Sergeev Sergey
//!
//! \brief Класс для работы с мультиметром National Instruments PXI-4071
class ni_pxi_4071_t: public mxmultimeter_t
{
public:
  typedef irs_size_t size_type;
  enum { // power_freq
    power_50_Hz,
    power_60_Hz
  };
  //! \brief resolution
  enum {
    digits_3_5,
    digits_4_5,
    digits_5_5,
    digits_6_5,
    digits_7_5
  };
  //! \brief тип измерений
  enum { // meas_mode
    digitizer,
    high_speed
  };
  //! \brief intergation time units
  enum { 
    sec,
    plc
  };
  
  
  ni_pxi_4071_t(
    hardflow_t* ap_hardflow,
    const filter_settings_t& a_filter = zero_struct_t<filter_settings_t>::get(),
    counter_t a_update_time = make_cnt_ms(200),
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active
  );
  ~ni_pxi_4071_t();
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  //! \brief Установить режим измерения постоянного напряжения
  virtual void set_dc();
  //! \brief Установить режим измерения переменного напряжения
  virtual void set_ac();
  //! \brief Установить положителный фронт запуска
  virtual void set_positive();
  //! \brief Установить отрицательный фронт канала
  virtual void set_negative();
  //! \brief Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  //! \brief Чтение напряжения
  virtual void get_voltage(double *ap_voltage);
  //! \brief Чтения силы тока
  virtual void get_current(double *ap_current);
  //! \brief Чтение сопротивления
  virtual void get_resistance2x(double *ap_resistance);
  //! \brief Чтение сопротивления
  virtual void get_resistance4x(double *ap_resistance);
  //! \brief Чтение частоты
  virtual void get_frequency(double *ap_frequency);
  //! \brief Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *ap_phase_average);
  //! \brief Чтение фазового сдвига
  virtual void get_phase(double *ap_phase);
  //! \brief Чтение временного интервала
  virtual void get_time_interval(double *ap_time_interval);
  //! \brief Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval);

  //! \brief Запуск автокалибровки мультиметра
  virtual void auto_calibration();
  //! \brief Чтение статуса текущей операции
  virtual meas_status_t status();
  //! \brief Прерывание текущей операции
  virtual void abort();
  //! \brief Элементарное действие
  virtual void tick();
  //! \brief Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double a_nplc);
  //! \brief Установка времени интегрирования в c
  virtual void set_aperture(double a_aperture);
  //! \brief Установка полосы фильтра
  virtual void set_bandwidth(double /*a_bandwidth*/);
  //! \brief Установка входного сопротивления канала
  virtual void set_input_impedance(double /*a_impedance*/);
  //! \brief Установка уровня запуска канала
  virtual void set_start_level(double /*a_level*/);
  //! \brief Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief Установка автоматического выбора диапазона измерений
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
    meas_value_mode,
    get_value_mode,
    get_voltage_mode,
    get_current_mode,
    get_resistance2x_mode,
    get_resistance4x_mode
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
    conn_data_t<double> meas_value_not_filtered;
    // Статус мультиметра:
    conn_data_t<irs_u16> meas_status;
        
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
      index = meas_value_not_filtered.connect(ap_data, index);
      index = meas_status.connect(ap_data, index);

      return index;
    }
  };

  multimeter_mode_type_t m_mul_mode_type;
  hardflow_t* mp_hardflow;
  modbus_client_t m_modbus_client;
  eth_mul_data_t m_eth_mul_data;
  meas_status_t m_status;
  double* mp_value;
  bool m_abort_request;
  mode_t m_mode;
  filter_settings_t m_filter;
  size_type m_window_size;
};

//! \brief Класс для работы с термометром termex lt-300
//! \details Параметры com_flow_t 4800, FALSE, NOPARITY, 8, ONESTOPBIT,
//!   DTR_CONTROL_ENABLE
class termex_lt_300_t: public mxmultimeter_t
{
public:
  //! \brief Конструктор
  termex_lt_300_t(hardflow_t* ap_hardflow);
  //! \brief Деструктор
  ~termex_lt_300_t();
  //! \brief Установить режим измерения постоянного напряжения
  virtual inline void set_dc() {}
  //! \brief Установить режим измерения переменного напряжения
  virtual inline void set_ac() {}
  //! \brief Установить положителный фронт запуска
  virtual void set_positive() {}
  //! \brief Установить отрицательный фронт канала
  virtual void set_negative() {}
  //! \brief Чтение значения при текущем типа измерения
  virtual void get_value(double *ap_value);
  //! \brief Чтение напряжения
  virtual void get_voltage(double *voltage) {}
  //! \brief Чтения силы тока
  virtual void get_current(double *current) {}
  //! \brief Чтение сопротивления
  virtual void get_resistance2x(double *resistance) {}
  //! \brief Чтение сопротивления
  virtual void get_resistance4x(double *resistance) {}
  //! \brief Чтение частоты
  virtual void get_frequency(double *frequency) {}
  //! \brief Чтение усредненного сдвира фаз
  virtual void get_phase_average(double *phase_average) {}
  //! \brief Чтение фазового сдвига
  virtual void get_phase(double *phase) {}
  //! \brief Чтение временного интервала
  virtual void get_time_interval(double *time_interval) {}
  //! \brief Чтение усредненного временного интервала
  virtual void get_time_interval_average(double *ap_time_interval) {}
  //! \brief Запуск автокалибровки (команда ACAL) мультиметра
  virtual void auto_calibration() {}
  //! \brief Чтение статуса текущей операции
  virtual meas_status_t status();
  //! \brief Прерывание текущей операции
  virtual void abort() {}
  //! \brief Элементарное действие
  virtual void tick();
  //! \brief Установка времени интегрирования в периодах частоты сети (20 мс)
  virtual void set_nplc(double nplc) {}
  //! \brief Установка времени интегрирования в c
  virtual void set_aperture(double aperture) {}
  //! \brief Установка полосы фильтра
  virtual void set_bandwidth(double bandwidth) {}
  //! \brief Установка входного сопротивления канала
  virtual void set_input_impedance(double impedance) {}
  //! \brief Устсновка уровня запуска канала
  virtual void set_start_level(double level) {}
  //! \brief Установка диапазона измерений
  virtual void set_range(type_meas_t a_type_meas, double a_range) {}
  //! \brief Установка автоматического выбора диапазона измерений
  virtual void set_range_auto() {}

private:
  irs::hardflow_t* mp_hardflow;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  enum mode_t {
    mode_free,
    mode_read,
    mode_read_wait,
    mode_start_read,
    mode_start_read_wait
  };
  mode_t m_mode;
  const size_t m_read_chunk_size;
  const irs::irs_string_t m_end_line;
  irs::irs_string_t m_read_string;
  irs::raw_data_t<irs_u8> m_read_data;
  irs::raw_data_t<irs_u8> m_transmit_data;
  meas_status_t m_status;
  double *mp_value;
  irs_uarc m_ch;
  irs::timer_t m_delay_after_get_value_timer;

  static irs::raw_data_t<irs_u8> u8_from_str(const irs::irs_string_t& a_string);
};

//! @}

} //namespace irs

#endif //measmulH
