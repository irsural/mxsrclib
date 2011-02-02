//! \file
//! \ingroup drivers_group
//! \brief Классы для работы с мультиметрами
//!
//! Дата: 24.11.2010\n
//! Ранняя дата: 10.09.2009

//#define OFF_EXTCOM // Отключение расширенных команд
//#define RESMEAS // Сброс перед измерением

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <string.h>
#include <stdlib.h>

#include <irsstrdefs.h>
#include <irsstring.h>
#include <irsdsp.h>
#include <irsalg.h>

#include <measmul.h>

#include <irsfinal.h>

// class mxmultimeter_t
void mxmultimeter_t::get_param(const multimeter_param_t /*a_param*/,
  irs::raw_data_t<irs_u8>* /*ap_value*/) const
{
  // Игнорируем
}
void mxmultimeter_t::set_param(const multimeter_param_t /*a_param*/,
  const irs::raw_data_t<irs_u8>& /*a_value*/)
{
  // Игнорируем
}
bool mxmultimeter_t::is_param_exists(
  const multimeter_param_t /*a_param*/) const
{
  return false;
}

namespace {

struct str_tm_cont_t
{
  const irs::string_t volt_dc;
  const irs::string_t volt_ac;
  const irs::string_t current_dc;
  const irs::string_t current_ac;
  const irs::string_t resistance_2x;
  const irs::string_t resistance_4x;
  const irs::string_t frequency;
  const irs::string_t phase;
  const irs::string_t phase_average;
  const irs::string_t time_interval;
  const irs::string_t time_interval_average;
  str_tm_cont_t():
    volt_dc(irst("Постоянное напряжение")),
    volt_ac(irst("Переменное напряжение")),
    current_dc(irst("Постоянный ток")),
    current_ac(irst("Переменный ток")),
    resistance_2x(irst("Сопротивление по 2-х проводной схеме")),
    resistance_4x(irst("Сопротивление по 4-х проводной схеме")),
    frequency(irst("Частота")),
    phase(irst("Фаза")),
    phase_average(irst("Усредненная фаза")),
    time_interval(irst("Временной интервал")),
    time_interval_average(irst("Усредненный временной интервал"))
  {
  }
};

str_tm_cont_t* str_tm()
{
  static auto_ptr<str_tm_cont_t> p_str_tm_cont(new str_tm_cont_t());
  return p_str_tm_cont.get();
}

} //namespace

irs::string_t type_meas_to_str(const type_meas_t a_type_meas)
{

  irs::string_t str_type_meas = str_tm()->volt_dc;

  switch(a_type_meas){
    case tm_volt_dc:        { str_type_meas = str_tm()->volt_dc;       } break;
    case tm_volt_ac:        { str_type_meas = str_tm()->volt_ac;       } break;
    case tm_current_dc:     { str_type_meas = str_tm()->current_dc;    } break;
    case tm_current_ac:     { str_type_meas = str_tm()->current_ac;    } break;
    case tm_resistance_2x:  { str_type_meas = str_tm()->resistance_2x; } break;
    case tm_resistance_4x:  { str_type_meas = str_tm()->resistance_4x; } break;
    case tm_frequency:      { str_type_meas = str_tm()->frequency;     } break;
    case tm_phase:          { str_type_meas = str_tm()->phase;         } break;
    case tm_phase_average:  { str_type_meas = str_tm()->phase_average; } break;
    case tm_time_interval:  { str_type_meas = str_tm()->time_interval; } break;
    case tm_time_interval_average:
                            { str_type_meas = str_tm()->time_interval_average;
                                                                    } break;
    default:{
      #ifdef IRS_LIB_DEBUG
      irs::error_trans_base_t* error_trans = irs::error_trans();
      //  добавлено (void*)
      //char msg_cstr[] = "Отсутствует текстовое представление значения "
          //"переменной перечисляемого типа";
      //char file_cstr[] = __FILE__;
      IRS_SPEC_CSTR_DECLARE(msg_cstr, "Отсутствует текстовое представление "
        "значения переменной перечисляемого типа");
      IRS_SPEC_CSTR_DECLARE(file_cstr, __FILE__);
      error_trans->throw_error(irs::ec_standard, file_cstr, __LINE__,
        (void*)msg_cstr);
      #endif //IRS_LIB_DEBUG
    } break;
  }
  return str_type_meas;
}

bool str_to_type_meas(
  const irs::string_t& a_str_type_meas, type_meas_t& a_type_meas)
{
  bool fsuccess = true;
  if (a_str_type_meas == str_tm()->volt_dc) {
    a_type_meas = tm_volt_dc;
  } else if (a_str_type_meas == str_tm()->volt_ac) {
    a_type_meas = tm_volt_ac;
  } else if (a_str_type_meas == str_tm()->current_dc) {
    a_type_meas = tm_current_dc;
  } else if (a_str_type_meas == str_tm()->current_ac) {
    a_type_meas = tm_current_ac;
  } else if (a_str_type_meas == str_tm()->resistance_2x) {
    a_type_meas = tm_resistance_2x;
  } else if (a_str_type_meas == str_tm()->resistance_4x) {
    a_type_meas = tm_resistance_4x;
  } else if (a_str_type_meas == str_tm()->frequency) {
    a_type_meas = tm_frequency;
  } else if (a_str_type_meas == str_tm()->phase) {
    a_type_meas = tm_phase;
  } else if (a_str_type_meas == str_tm()->phase_average) {
    a_type_meas = tm_phase_average;
  } else if (a_str_type_meas == str_tm()->time_interval) {
    a_type_meas = tm_time_interval;
  } else if (a_str_type_meas == str_tm()->time_interval_average) {
    a_type_meas = tm_time_interval;
  } else{
    fsuccess = false;
  }
  return fsuccess;
}

// Класс для работы с мультиметром Agilent 3458A через irs::hardflow_t
// Конструктор
irs::agilent_3458a_t::agilent_3458a_t(
  hardflow_t* ap_hardflow,
  multimeter_mode_type_t a_mul_mode_type
):
  m_mul_mode_type(a_mul_mode_type),
  m_init_commands(),
  m_voltage_type_direct("DCV"),
  m_voltage_type_alternate("ACV"),
  m_current_type_direct("DCI"),
  m_current_type_alternate("ACI"),
  m_voltage_type_direct_range("DCV AUTO"),
  m_voltage_type_alternate_range("ACV AUTO"),
  m_current_type_direct_range("DCI AUTO"),
  m_current_type_alternate_range("ACI AUTO"),
  m_volt_curr_type(vct_direct),
  //m_time_int_voltage_index(0),
  m_time_int_measure_command("NPLC 100"),
  m_get_measure_commands(),
  m_resistance_type_index(0),
  m_resistance_type_2x("OHM AUTO"),
  m_resistance_type_4x("OHMF AUTO"),
  m_time_int_resistance_index(0),
  m_get_resistance_commands(),
  mp_hardflow(ap_hardflow),
  m_fixed_flow(mp_hardflow),
  m_create_error(true),
  m_mode(ma_mode_start),
  m_macro_mode(macro_mode_stop),
  m_status(meas_status_success),
  m_command(mac_free),
  m_voltage(IRS_NULL),
  m_resistance(IRS_NULL),
  m_abort_request(false),
  m_read_pos(0),
  m_cur_mul_command(),
  m_mul_commands(IRS_NULL),
  m_mul_commands_index(0),
  //m_command_prev(mac_free),
  m_value(IRS_NULL),
  m_get_parametr_needed(false),
  m_oper_time(0),
  m_oper_to(0),
  m_acal_time(0),
  m_acal_to(0),
  m_init_timer(irs::make_cnt_s(1)),
  m_init_mode(im_start),
  m_ic_index(0)
{
  IRS_LIB_ERROR_IF(!mp_hardflow, ec_standard, "Недопустимо передавать нулевой "
    "указатель в качестве hardflow_t*");  
  irs::string_t read_timeout_str = read_timeout_s;
  mp_hardflow->set_param(irst("read_timeout"), read_timeout_str);
  m_fixed_flow.read_timeout(make_cnt_s(
    read_timeout_s + fixed_flow_read_timeout_delta_s));

  init_to_cnt();

  m_oper_time = TIME_TO_CNT(1, 1);
  m_acal_time = TIME_TO_CNT(20*60, 1);

  // Команды при инициализации
  m_init_commands.push_back("RESET");
  m_init_commands.push_back("PRESET NORM");
  /*
  Автоматическая коррекция нуля (время счета * 2, обязательна
  для четырехпроводной схемы)
    AZERO ON
  Звуковой сигнал разрешен
    BEEP ON
  Измерение напряжения постоянного тока, автопереключение пределов
    DCV AUTO
  Значение задержки по умолчанию
    DELAY -1
  Индикатор включен
    DISP ON
  Фиксация входного сопротивления запрещена
    FIXEDZ OFF
  Входной буфер запрещен
    INBUF OFF
  Клавиатура мультиметра включена
    LOCK OFF
  Математические операции в реальном времени запрещены
    MATH OFF
  Память отсчетов запрещена
    MEM OFF
  Отсчеты в памяти вещественные одинарные
    MFORMAT SREAL
  Математическая постобработка зпрещена
    MMATH OFF
  Число разрядов индикации 6,5
    NDIG 6
  Время интегрирования
    NPLC 1
  1 отсчет на запуск, событие запуска - автозапуск
    NRDGS 1,AUTO
  Компенсация смещения при измерении сопротивлений запрещена
    OCOMP OFF
  Формат вывода - ASCII
    OFORMAT ASCII
  Событие подготовки запуска установлено на Auto
    TARM AUTO
  Таймер 1 с
    TIMER 1
  Событие запуска установлено на SYN
    TRIG SYN
  */
  //m_init_commands.push_back("MEM FIFO");
  #ifndef OFF_EXTCOM
  // Запоминание чисел в виде вещественных чисел двойной точности
  m_init_commands.push_back("MFORMAT DREAL");
  // Компенсация наведенного напряжения смещения (Время счета * 2
  // для сопротивления)
  m_init_commands.push_back("OCOMP ON");
  m_init_commands.push_back("LFILTER ON");
  //m_init_commands.push_back("RANGE 1000");
  //m_init_commands.push_back("RES 1E-6");
  // Входное сопротивление фиксируется на 10 МОм для всех пределов
  //m_init_commands.push_back("FIXEDZ ON");
  // Автоматическая коррекция нуля (время счета * 2, обязательна
  // для четырехпроводной схемы)
  //m_init_commands.push_back("AZERO OFF");
  #endif //OFF_EXTCOM

  // Команды при чтении сопротивления
  m_resistance_type_index = m_get_resistance_commands.size();
  m_get_resistance_commands.push_back("OHMF AUTO");
  m_time_int_resistance_index = m_get_resistance_commands.size();
  m_get_resistance_commands.push_back("NPLC 20");
  m_get_resistance_commands.push_back("TRIG SGL");

  // Очистка буфера приема
  memset(m_read_buf, 0, ma_read_buf_size);
  return;
}
// Создание команд для напряжени/тока
void irs::agilent_3458a_t::measure_create_commands(measure_t a_measure)
{
  m_get_measure_commands.clear();
  switch (a_measure) {
    case meas_value: {
      // Команды чтения значения при произвольном типе измерения
      //m_get_measure_commands.push_back(m_value_type);
      //m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
    } break;
    case meas_voltage: {
      // Команды при чтении напряжения
      //m_voltage_type_index = m_get_measure_commands.size();
      switch (m_volt_curr_type) {
        case vct_direct: {
          m_get_measure_commands.push_back(m_voltage_type_direct_range);
        } break;
        case vct_alternate: {
          m_get_measure_commands.push_back(m_voltage_type_alternate_range);
          //m_get_measure_commands.push_back("SETACV SYNC");
        } break;
      }
      //m_time_int_voltage_index = m_get_measure_commands.size();
      m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
    } break;
    case meas_current: {
      // Команды при чтении тока
      //m_voltage_type_index = m_get_measure_commands.size();
      switch (m_volt_curr_type) {
        case vct_direct: {
          m_get_measure_commands.push_back(m_current_type_direct_range);
        } break;
        case vct_alternate: {
          m_get_measure_commands.push_back(m_current_type_alternate_range);
        } break;
      }
      //m_time_int_voltage_index = m_get_measure_commands.size();
      m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
    } break;
    case meas_frequency: {
      m_get_measure_commands.push_back("FREQ");
      m_get_measure_commands.push_back("FSOURCE ACV");
      m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
   } break;
   case meas_set_range: {
      m_get_measure_commands.push_back(m_set_range_command);
   } break;
  }
  //m_get_measure_commands.push_back(m_range_measute_command);
}
// Деструктор
irs::agilent_3458a_t::~agilent_3458a_t()
{
  deinit_to_cnt();
}
// Установить режим измерения постоянного напряжения
void irs::agilent_3458a_t::set_dc()
{
  //m_get_measure_commands[m_voltage_type_index] = m_voltage_type_direct;
  m_volt_curr_type = vct_direct;
}
// Установить режим измерения переменного напряжения
void irs::agilent_3458a_t::set_ac()
{
  //m_get_measure_commands[m_voltage_type_index] = m_voltage_type_alternate;
  m_volt_curr_type = vct_alternate;
}
// Установить положителный фронт запуска
void irs::agilent_3458a_t::set_positive()
{
}
// Установить отрицательный фронт канала
void irs::agilent_3458a_t::set_negative()
{
}
// Чтение значения при текущем типа измерения
void irs::agilent_3458a_t::get_value(double *ap_value)
{
  //if (m_create_error) return;
  measure_create_commands(meas_value);
  m_voltage = ap_value;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение напряжения
void irs::agilent_3458a_t::get_voltage(double *voltage)
{
  //if (m_create_error) return;
  measure_create_commands(meas_voltage);
  m_voltage = voltage;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение усредненного сдвира фаз
void irs::agilent_3458a_t::get_phase_average(double * /*phase_average*/)
{
}
// Чтение фазового сдвига
void irs::agilent_3458a_t::get_phase(double * /*phase*/)
{
}
// Чтение временного интервала
void irs::agilent_3458a_t::get_time_interval(double * /*time_interval*/)
{
}
// Чтение усредненного временного интервала
void irs::agilent_3458a_t::get_time_interval_average(
  double * /*ap_time_interval*/)
{
}
// Чтения силы тока
void irs::agilent_3458a_t::get_current(double *current)
{
  //if (m_create_error) return;
  measure_create_commands(meas_current);
  m_voltage = current;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение сопротивления
void irs::agilent_3458a_t::get_resistance2x(double *resistance)
{
  //if (m_create_error) return;
  m_get_resistance_commands[m_resistance_type_index] = m_resistance_type_2x;
  m_resistance = resistance;
  m_command = mac_get_resistance;
  m_status = meas_status_busy;
}
// Чтение сопротивления
void irs::agilent_3458a_t::get_resistance4x(double *resistance)
{
  //if (m_create_error) return;
  m_get_resistance_commands[m_resistance_type_index] = m_resistance_type_4x;
  m_resistance = resistance;
  m_command = mac_get_resistance;
  m_status = meas_status_busy;
}
// Чтение частоты
void irs::agilent_3458a_t::get_frequency(double* /*frequency*/)
{
}
// Запуск автокалибровки (команда ACAL) мультиметра
void irs::agilent_3458a_t::auto_calibration()
{
  //if (m_create_error) return;
  m_command = mac_auto_calibration;
  m_status = meas_status_busy;
}
// Чтение статуса текущей операции
meas_status_t irs::agilent_3458a_t::status()
{
  //if (m_create_error) return meas_status_busy;
  return m_status;
}
// Прерывание текущей операции
void irs::agilent_3458a_t::abort()
{
  //if (m_create_error) return;
  m_abort_request = true;
}
// Отправка команд инициализации в мультиметр
void irs::agilent_3458a_t::initialize_tick()
{
  switch (m_mul_mode_type) {
    case mul_mode_type_active: {
      // Запись команд инициализации
      switch (m_init_mode) {
        case im_start: {
          if (m_create_error) {
            m_init_timer.start();
            m_ic_index = 0;
            m_init_mode = im_write_command;
          }
        } break;
        case im_write_command: {
          irs::string& icomm = m_init_commands[m_ic_index];
          size_t icomm_size = icomm.size();
          const irs_u8* icomm_u8 =
            reinterpret_cast<const irs_u8*>(icomm.c_str());
          m_fixed_flow.write(mp_hardflow->channel_next(), icomm_u8, icomm_size);
          m_fixed_flow.write_timeout(m_oper_time);
          m_init_mode = im_next_command;
        } break;
        case im_next_command: {
          switch (m_fixed_flow.write_status()) {
            case irs::hardflow::fixed_flow_t::status_success: {
              m_ic_index++;
              if (m_ic_index < static_cast<index_t>(m_init_commands.size())) {
                m_init_mode = im_write_command;
              } else {
                m_create_error = false;
                m_init_mode = im_start;
              }
            } break;
            case irs::hardflow::fixed_flow_t::status_error: {
              m_init_mode = im_write_command;
            } break;
            case irs::hardflow::fixed_flow_t::status_wait: {
              if (m_init_timer.check()) {
                m_init_mode = im_start;
              }
            } break;
            default : {
              IRS_LIB_ASSERT_MSG("Неучтенный статус");
            }
          }
        } break;
        default: {
          IRS_LIB_ASSERT_MSG("m_init_mode не должен быть default");
        } break;
      } //switch (m_init_mode)
    } break;
    case mul_mode_type_passive: {
      m_create_error = false;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("m_mul_mode_type не должен быть default");
    } break;
  } //switch (m_mul_mode_type)
}
// Элементарное действие
void irs::agilent_3458a_t::tick()
{
  IRS_LIB_ASSERT(mp_hardflow);
  mp_hardflow->tick();
  m_fixed_flow.tick();
  initialize_tick();
  if (m_create_error) return;
  switch (m_mode) {
    case ma_mode_start: {
      m_abort_request = false;
      switch (m_command) {
        case mac_get_param: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_get_voltage;
          m_mode = ma_mode_macro;
        } break;
        case mac_get_resistance: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_get_resistance;
          m_mode = ma_mode_macro;
        } break;
        case mac_auto_calibration: {
          m_macro_mode = macro_mode_stop;
          m_mode = ma_mode_auto_calibration;
        } break;
        case mac_send_commands: {
          m_macro_mode = macro_mode_send_commands;
          m_mode = ma_mode_macro;
        } break;
        default : {
          // Добавлено для подовления warning'а. Компилятор gcc4.
        }
      }
    } break;
    case ma_mode_macro: {
      if (m_abort_request) {
        m_abort_request = false;
        m_macro_mode = macro_mode_stop;
      }
      switch (m_macro_mode) {
        case macro_mode_get_voltage: {
          if (m_get_parametr_needed) {
            m_value = m_voltage;
            m_macro_mode = macro_mode_stop;
            m_mode = ma_mode_get_value;
          } else {
            m_get_parametr_needed = true;
            m_mul_commands = &m_get_measure_commands;
            m_mul_commands_index = 0;
            //m_command_prev = m_command;
            m_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_resistance: {
          if (m_get_parametr_needed) {
            m_value = m_resistance;
            m_macro_mode = macro_mode_stop;
            m_mode = ma_mode_get_value;
          } else {
            m_get_parametr_needed = true;
            m_mul_commands = &m_get_resistance_commands;
            m_mul_commands_index = 0;
            //m_command_prev = m_command;
            m_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_send_commands: {
          m_mul_commands = &m_get_measure_commands;
          m_mul_commands_index = 0;
          m_mode = ma_mode_commands;
          m_macro_mode = macro_mode_stop;
        } break;
        case macro_mode_stop: {
          m_command = mac_free;
          m_status = meas_status_success;
          m_mode = ma_mode_start;
        } break;
      }
    } break;
    case ma_mode_commands: {
      m_cur_mul_command = (*m_mul_commands)[m_mul_commands_index];
      irs_u8 *command = (irs_u8 *)m_cur_mul_command.c_str();
      irs_u32 len = strlen((char *)command);
      m_fixed_flow.write(mp_hardflow->channel_next(), command, len);
      m_fixed_flow.write_timeout(m_oper_time);
      m_mode = ma_mode_commands_wait;
    } break;
    case ma_mode_commands_wait: {
      if (m_abort_request) {
        m_fixed_flow.write_abort();
        m_mode = ma_mode_macro;
      } else {
        switch (m_fixed_flow.write_status()) {
          case irs::hardflow::fixed_flow_t::status_success: {
            m_mul_commands_index++;
            if (m_mul_commands_index >= (index_t)m_mul_commands->size()) {
              m_mode = ma_mode_macro;
            } else {
              m_mode = ma_mode_commands;
            }
          } break;
          case irs::hardflow::fixed_flow_t::status_error: {
            m_mode = ma_mode_commands;
          } break;
          default : {
            // Ожидаем окончания операции
          }
        }
      }
    } break;
    case ma_mode_get_value: {
      m_fixed_flow.read(mp_hardflow->channel_next(),
        m_read_buf, sample_size);
      m_mode = ma_mode_get_value_wait;
    } break;
    case ma_mode_get_value_wait: {
      if (!m_abort_request) {
        switch (m_fixed_flow.read_status()) {
          case irs::hardflow::fixed_flow_t::status_success: {
            char* p_number_in_cstr = reinterpret_cast<char*>(m_read_buf);
            if (!irs::cstr_to_number_classic(p_number_in_cstr, *m_value)) {
              *m_value = 0;
            }
            m_mode = ma_mode_macro;
          } break;
          case irs::hardflow::fixed_flow_t::status_error: {
            m_mode = ma_mode_macro;
          } break;
          default : {
            // Ожидаем выполнения
          }
        }
      } else {
        m_fixed_flow.read_abort();
        m_mode = ma_mode_macro;
      }
    } break;

    case ma_mode_auto_calibration: {
      irs_u8 *command = (irs_u8 *)"ACAL ALL";
      irs_u32 len = strlen((char *)command);
      m_fixed_flow.write(mp_hardflow->channel_next(), command, len);
      m_fixed_flow.write_timeout(m_acal_time);
      m_mode = ma_mode_auto_calibration_wait;
    } break;
    case ma_mode_auto_calibration_wait: {
      if (test_to_cnt(m_acal_to)) {
        const fixed_flow_status_type status = m_fixed_flow.write_status();
        if (status == irs::hardflow::fixed_flow_t::status_success)
          m_mode = ma_mode_macro;
      } else if (m_abort_request) {
        m_fixed_flow.write_abort();
        m_mode = ma_mode_macro;
      }
    } break;
  }
}
// Установка времени интегрирования в периодах частоты сети (20 мс)
void irs::agilent_3458a_t::set_nplc(double nplc)
{
  irs::string nplc_str = nplc;
  nplc_str = "NPLC " + nplc_str;
  //m_get_measure_commands[m_time_int_voltage_index] = nplc_str;
  m_time_int_measure_command = nplc_str;
  m_get_resistance_commands[m_time_int_resistance_index] = nplc_str;
}
// Установка времени интегрирования в c
void irs::agilent_3458a_t::set_aperture(double aperture)
{
  irs::string aperture_str = aperture;
  aperture_str = "APER " + aperture_str;
  //m_get_measure_commands[m_time_int_voltage_index] = aperture_str;
  m_time_int_measure_command = aperture_str;
  m_get_resistance_commands[m_time_int_resistance_index] = aperture_str;
}
// Установка полосы фильтра
void irs::agilent_3458a_t::set_bandwidth(double /*bandwidth*/)
{
}
void irs::agilent_3458a_t::set_input_impedance(double /*impedance*/)
{
}
void irs::agilent_3458a_t::set_start_level(double /*level*/)
{
}
void irs::agilent_3458a_t::set_range(
  type_meas_t a_type_meas, double a_range)
{
  //if (m_create_error) return;
  irs::string range_str = a_range;
  switch(a_type_meas) {
    case tm_volt_dc: {
      m_voltage_type_direct_range = m_voltage_type_direct+" "+range_str;
      m_set_range_command = m_voltage_type_direct_range;
    } break;
    case tm_volt_ac: {
      m_voltage_type_alternate_range = m_voltage_type_alternate+" "+range_str;
      m_set_range_command = m_voltage_type_alternate_range;
    } break;
    case tm_current_dc: {
      m_current_type_direct_range = m_current_type_direct+" "+range_str;
      m_set_range_command = m_current_type_direct_range;
    } break;
    case tm_current_ac: {
      m_current_type_alternate_range = m_current_type_alternate+" "+range_str;
      m_set_range_command = m_current_type_alternate_range;
    } break;
    default : {
      // Остальные типы в данном мультиметре не используются
      IRS_ERROR(irs::ec_standard, "Unknown type range!");
    }
  }
  measure_create_commands(meas_set_range);
  m_command = mac_send_commands;
  m_status = meas_status_busy;
}

void irs::agilent_3458a_t::set_range_auto()
{
  irs::string range_str = "AUTO";
  m_voltage_type_direct_range = m_voltage_type_direct+" "+range_str;
  m_voltage_type_alternate_range = m_voltage_type_alternate+" "+range_str;
  m_current_type_direct_range = m_current_type_direct+" "+range_str;
  m_current_type_alternate_range = m_current_type_alternate+" "+range_str;
}

// Класс для работы с мультиметром Agilent 3458A
// Конструктор
mx_agilent_3458a_t::mx_agilent_3458a_t(
  mxifa_ch_t channel,
  gpib_addr_t address,
  multimeter_mode_type_t a_mul_mode_type
):
  m_mul_mode_type(a_mul_mode_type),
  m_init_commands(),
  m_voltage_type_direct("DCV"),
  m_voltage_type_alternate("ACV"),
  m_current_type_direct("DCI"),
  m_current_type_alternate("ACI"),
  m_voltage_type_direct_range("DCV AUTO"),
  m_voltage_type_alternate_range("ACV AUTO"),
  m_current_type_direct_range("DCI AUTO"),
  m_current_type_alternate_range("ACI AUTO"),
  m_volt_curr_type(vct_direct),
  //m_time_int_voltage_index(0),
  m_time_int_measure_command("NPLC 100"),
  m_get_measure_commands(),
  m_resistance_type_index(0),
  m_resistance_type_2x("OHM AUTO"),
  m_resistance_type_4x("OHMF AUTO"),
  m_time_int_resistance_index(0),
  m_get_resistance_commands(),
  m_handle(IRS_NULL),
  m_create_error(true),
  m_mode(ma_mode_start),
  m_macro_mode(macro_mode_stop),
  m_status(meas_status_success),
  m_command(mac_free),
  m_voltage(IRS_NULL),
  m_resistance(IRS_NULL),
  m_abort_request(false),
  m_read_pos(0),
  m_cur_mul_command(),
  m_mul_commands(IRS_NULL),
  m_mul_commands_index(0),
  //m_command_prev(mac_free),
  m_value(IRS_NULL),
  m_get_parametr_needed(false),
  m_oper_time(0),
  m_oper_to(0),
  m_acal_time(0),
  m_acal_to(0),
  m_init_timer(irs::make_cnt_s(1)),
  m_init_mode(im_start),
  m_ic_index(0)
{
  init_to_cnt();
  mxifa_init();

  m_oper_time = TIME_TO_CNT(1, 1);
  m_acal_time = TIME_TO_CNT(20*60, 1);

  // Открытие канала MXIFA_MULTIMETER
  if (mxifa_get_channel_type_ex(channel) ==
      mxifa_ei_win32_ni_usb_gpib) {
    mxifa_win32_ni_usb_gpib_cfg config;
    config.address = address;
    m_handle = mxifa_open_ex(channel, &config, false);
  } else {
    m_handle = mxifa_open(channel, false);
  }


  // Команды при инициализации
  m_init_commands.push_back("RESET");
  m_init_commands.push_back("PRESET NORM");
  /*
  Автоматическая коррекция нуля (время счета * 2, обязательна
  для четырехпроводной схемы)
    AZERO ON
  Звуковой сигнал разрешен
    BEEP ON
  Измерение напряжения постоянного тока, автопереключение пределов
    DCV AUTO
  Значение задержки по умолчанию
    DELAY -1
  Индикатор включен
    DISP ON
  Фиксация входного сопротивления запрещена
    FIXEDZ OFF
  Входной буфер запрещен
    INBUF OFF
  Клавиатура мультиметра включена
    LOCK OFF
  Математические операции в реальном времени запрещены
    MATH OFF
  Память отсчетов запрещена
    MEM OFF
  Отсчеты в памяти вещественные одинарные
    MFORMAT SREAL
  Математическая постобработка зпрещена
    MMATH OFF
  Число разрядов индикации 6,5
    NDIG 6
  Время интегрирования
    NPLC 1
  1 отсчет на запуск, событие запуска - автозапуск
    NRDGS 1,AUTO
  Компенсация смещения при измерении сопротивлений запрещена
    OCOMP OFF
  Формат вывода - ASCII
    OFORMAT ASCII
  Событие подготовки запуска установлено на Auto
    TARM AUTO
  Таймер 1 с
    TIMER 1
  Событие запуска установлено на SYN
    TRIG SYN
  */
  #ifndef OFF_EXTCOM
  // Запоминание чисел в виде вещественных чисел двойной точности
  m_init_commands.push_back("MFORMAT DREAL");
  // Компенсация наведенного напряжения смещения (Время счета * 2
  // для сопротивления)
  m_init_commands.push_back("OCOMP ON");
  m_init_commands.push_back("LFILTER ON");
  //m_init_commands.push_back("RANGE 1000");
  //m_init_commands.push_back("RES 1E-6");
  // Входное сопротивление фиксируется на 10 МОм для всех пределов
  //m_init_commands.push_back("FIXEDZ ON");
  // Автоматическая коррекция нуля (время счета * 2, обязательна
  // для четырехпроводной схемы)
  //m_init_commands.push_back("AZERO OFF");
  #endif //OFF_EXTCOM

  // Команды при чтении сопротивления
  m_resistance_type_index = m_get_resistance_commands.size();
  m_get_resistance_commands.push_back("OHMF AUTO");
  m_time_int_resistance_index = m_get_resistance_commands.size();
  m_get_resistance_commands.push_back("NPLC 20");
  m_get_resistance_commands.push_back("TRIG SGL");

  // Очистка буфера приема
  memset(m_read_buf, 0, ma_read_buf_size);
  return;
}
// Создание команд для напряжени/тока
void mx_agilent_3458a_t::measure_create_commands(measure_t a_measure)
{
  m_get_measure_commands.clear();
  switch (a_measure) {
    case meas_value: {
      // Команды чтения значения при произвольном типе измерения
      //m_get_measure_commands.push_back(m_value_type);
      //m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
    } break;
    case meas_voltage: {
      // Команды при чтении напряжения
      //m_voltage_type_index = m_get_measure_commands.size();
      switch (m_volt_curr_type) {
        case vct_direct: {
          m_get_measure_commands.push_back(m_voltage_type_direct_range);
        } break;
        case vct_alternate: {
          m_get_measure_commands.push_back(m_voltage_type_alternate_range);
          //m_get_measure_commands.push_back("SETACV SYNC");
        } break;
      }
      //m_time_int_voltage_index = m_get_measure_commands.size();
      m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
    } break;
    case meas_current: {
      // Команды при чтении тока
      //m_voltage_type_index = m_get_measure_commands.size();
      switch (m_volt_curr_type) {
        case vct_direct: {
          m_get_measure_commands.push_back(m_current_type_direct_range);
        } break;
        case vct_alternate: {
          m_get_measure_commands.push_back(m_current_type_alternate_range);
        } break;
      }
      //m_time_int_voltage_index = m_get_measure_commands.size();
      m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
    } break;
    case meas_frequency: {
      m_get_measure_commands.push_back("FREQ");
      m_get_measure_commands.push_back("FSOURCE ACV");
      m_get_measure_commands.push_back(m_time_int_measure_command);
      m_get_measure_commands.push_back("TRIG SGL");
   } break;
   case meas_set_range: {
      m_get_measure_commands.push_back(m_set_range_command);
   } break;
  }
  //m_get_measure_commands.push_back(m_range_measute_command);
}
// Деструктор
mx_agilent_3458a_t::~mx_agilent_3458a_t()
{
  //if (m_create_error) return;
  mxifa_close_begin(m_handle);
  mxifa_close_end(m_handle, true);
  mxifa_deinit();
  deinit_to_cnt();
}
// Установить режим измерения постоянного напряжения
void mx_agilent_3458a_t::set_dc()
{
  //m_get_measure_commands[m_voltage_type_index] = m_voltage_type_direct;
  m_volt_curr_type = vct_direct;
}
// Установить режим измерения переменного напряжения
void mx_agilent_3458a_t::set_ac()
{
  //m_get_measure_commands[m_voltage_type_index] = m_voltage_type_alternate;
  m_volt_curr_type = vct_alternate;
}
// Установить положителный фронт запуска
void mx_agilent_3458a_t::set_positive()
{
}
// Установить отрицательный фронт канала
void mx_agilent_3458a_t::set_negative()
{
}
// Чтение значения при текущем типа измерения
void mx_agilent_3458a_t::get_value(double *ap_value)
{
  //if (m_create_error) return;
  measure_create_commands(meas_value);
  m_voltage = ap_value;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение напряжения
void mx_agilent_3458a_t::get_voltage(double *voltage)
{
  //if (m_create_error) return;
  measure_create_commands(meas_voltage);
  m_voltage = voltage;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение усредненного сдвира фаз
void mx_agilent_3458a_t::get_phase_average(double * /*phase_average*/)
{
}
// Чтение фазового сдвига
void mx_agilent_3458a_t::get_phase(double * /*phase*/)
{
}
// Чтение временного интервала
void mx_agilent_3458a_t::get_time_interval(double * /*time_interval*/)
{
}
// Чтение усредненного временного интервала
void mx_agilent_3458a_t::get_time_interval_average(
  double * /*ap_time_interval*/)
{
}
// Чтения силы тока
void mx_agilent_3458a_t::get_current(double *current)
{
  //if (m_create_error) return;
  measure_create_commands(meas_current);
  m_voltage = current;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение сопротивления
void mx_agilent_3458a_t::get_resistance2x(double *resistance)
{
  //if (m_create_error) return;
  m_get_resistance_commands[m_resistance_type_index] = m_resistance_type_2x;
  m_resistance = resistance;
  m_command = mac_get_resistance;
  m_status = meas_status_busy;
}
// Чтение сопротивления
void mx_agilent_3458a_t::get_resistance4x(double *resistance)
{
  //if (m_create_error) return;
  m_get_resistance_commands[m_resistance_type_index] = m_resistance_type_4x;
  m_resistance = resistance;
  m_command = mac_get_resistance;
  m_status = meas_status_busy;
}
// Чтение частоты
void mx_agilent_3458a_t::get_frequency(double* /*frequency*/)
{
}
// Запуск автокалибровки (команда ACAL) мультиметра
void mx_agilent_3458a_t::auto_calibration()
{
  //if (m_create_error) return;
  m_command = mac_auto_calibration;
  m_status = meas_status_busy;
}
// Чтение статуса текущей операции
meas_status_t mx_agilent_3458a_t::status()
{
  //if (m_create_error) return meas_status_busy;
  return m_status;
}
// Прерывание текущей операции
void mx_agilent_3458a_t::abort()
{
  //if (m_create_error) return;
  m_abort_request = true;
}
// Отправка команд инициализации в мультиметр
void mx_agilent_3458a_t::initialize_tick()
{
  switch (m_mul_mode_type) {
    case mul_mode_type_active: {
      // Запись команд инициализации
      switch (m_init_mode) {
        case im_start: {
          if (m_create_error) {
            m_init_timer.start();
            m_ic_index = 0;
            m_init_mode = im_write_command;
          }
        } break;
        case im_write_command: {
          irs::string& icomm = m_init_commands[m_ic_index];
          size_t icomm_size = icomm.size();
          const irs_u8* icomm_u8 =
            reinterpret_cast<const irs_u8*>(icomm.c_str());
          mxifa_write_begin(m_handle, IRS_NULL, icomm_u8, icomm_size);
          set_to_cnt(m_oper_to, m_oper_time);
          m_init_mode = im_next_command;
        } break;
        case im_next_command: {
          if (mxifa_write_end(m_handle, false)) {
            m_ic_index++;
            if (m_ic_index < static_cast<index_t>(m_init_commands.size())) {
              m_init_mode = im_write_command;
            } else {
              m_create_error = false;
              m_init_mode = im_start;
            }
          }
          if (test_to_cnt(m_oper_to)) {
            mxifa_write_end(m_handle, true);
            m_init_mode = im_write_command;
          }
          if (m_init_timer.check()) {
            mxifa_write_end(m_handle, true);
            m_init_mode = im_start;
          }
        } break;
        default: {
          IRS_LIB_ASSERT_MSG("m_init_mode не должен быть default");
        } break;
      } //switch (m_init_mode)
    } break;
    case mul_mode_type_passive: {
      m_create_error = false;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("m_mul_mode_type не должен быть default");
    } break;
  } //switch (m_mul_mode_type)
}
// Элементарное действие
void mx_agilent_3458a_t::tick()
{
  mxifa_tick();
  initialize_tick();
  if (m_create_error) return;
  switch (m_mode) {
    case ma_mode_start: {
      m_abort_request = false;
      switch (m_command) {
        case mac_get_param: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_get_voltage;
          m_mode = ma_mode_macro;
        } break;
        case mac_get_resistance: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_get_resistance;
          m_mode = ma_mode_macro;
        } break;
        case mac_auto_calibration: {
          m_macro_mode = macro_mode_stop;
          m_mode = ma_mode_auto_calibration;
        } break;
        case mac_send_commands: {
          m_macro_mode = macro_mode_send_commands;
          m_mode = ma_mode_macro;
        } break;
        default : {
          // Добавлено для подовления warning'а. Компилятор gcc4.
        }
      }
    } break;
    case ma_mode_macro: {
      if (m_abort_request) {
        m_abort_request = false;
        m_macro_mode = macro_mode_stop;
      }
      switch (m_macro_mode) {
        case macro_mode_get_voltage: {
          if (m_get_parametr_needed) {
            m_value = m_voltage;
            m_macro_mode = macro_mode_stop;
            m_mode = ma_mode_get_value;
          } else {
            m_get_parametr_needed = true;
            m_mul_commands = &m_get_measure_commands;
            m_mul_commands_index = 0;
            //m_command_prev = m_command;
            m_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_resistance: {
          if (m_get_parametr_needed) {
            m_value = m_resistance;
            m_macro_mode = macro_mode_stop;
            m_mode = ma_mode_get_value;
          } else {
            m_get_parametr_needed = true;
            m_mul_commands = &m_get_resistance_commands;
            m_mul_commands_index = 0;
            //m_command_prev = m_command;
            m_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_send_commands: {
          m_mul_commands = &m_get_measure_commands;
          m_mul_commands_index = 0;
          m_mode = ma_mode_commands;
          m_macro_mode = macro_mode_stop;
        } break;
        case macro_mode_stop: {
          m_command = mac_free;
          m_status = meas_status_success;
          m_mode = ma_mode_start;
        } break;
      }
    } break;
    case ma_mode_commands: {
      m_cur_mul_command = (*m_mul_commands)[m_mul_commands_index];
      irs_u8 *command = (irs_u8 *)m_cur_mul_command.c_str();
      irs_u32 len = strlen((char *)command);
      mxifa_write_begin(m_handle, IRS_NULL, command, len);
      m_mode = ma_mode_commands_wait;
      set_to_cnt(m_oper_to, m_oper_time);
    } break;
    case ma_mode_commands_wait: {
      if (mxifa_write_end(m_handle, false)) {
        m_mul_commands_index++;
        if (m_mul_commands_index >= (index_t)m_mul_commands->size()) {
          m_mode = ma_mode_macro;
        } else {
          m_mode = ma_mode_commands;
        }
      } else if (test_to_cnt(m_oper_to)) {
        mxifa_write_end(m_handle, true);
        m_mode = ma_mode_commands;
      } else if (m_abort_request) {
        mxifa_write_end(m_handle, true);
        m_mode = ma_mode_macro;
      }
    } break;
    case ma_mode_get_value: {
      if (m_abort_request) {
        m_mode = ma_mode_macro;
      } else {
        irs_u8 *buf = m_read_buf + m_read_pos;
        mxifa_sz_t size = (ma_read_buf_size - 1) - m_read_pos;
        if ((size > 0) && (size < ma_read_buf_size)) {
          mxifa_sz_t read_count =
            mxifa_fast_read(m_handle, IRS_NULL, buf, size);
          buf[read_count] = 0;
          char end_chars[3] = {0x0D, 0x0A, 0x00};
          char *end_number = strstr((char *)m_read_buf, end_chars);
          if (end_number) {
            *end_number = 0;
            char* p_number_in_cstr = reinterpret_cast<char*>(m_read_buf);
            if (irs::cstr_to_number_classic(p_number_in_cstr, *m_value)) {
              m_mode = ma_mode_macro;
            } else {
              *m_value = 0;
            }
            #ifdef NOP
            char *end_ptr = IRS_NULL;
            double val = strtod((char *)m_read_buf, &end_ptr);
            if (end_ptr == end_number) {
              *m_value = val;
              m_mode = ma_mode_macro;
            }
            #endif //NOP
            irs_u8 *read_bytes_end_ptr = buf + read_count;
            irs_u8 *next_str_ptr = (irs_u8 *)end_number + 2;
            mxifa_sz_t rest_bytes = read_bytes_end_ptr - next_str_ptr;
            memmove((void *)m_read_buf, (void *)next_str_ptr, rest_bytes);
            m_read_pos = rest_bytes;
          } else {
            m_read_pos += read_count;
          }
        } else {
          m_read_buf[0] = m_read_buf[ma_read_buf_size - 1];
          m_read_pos = 1;
        }
      }
    } break;

    case ma_mode_auto_calibration: {
      irs_u8 *command = (irs_u8 *)"ACAL ALL";
      irs_u32 len = strlen((char *)command);
      mxifa_write_begin(m_handle, IRS_NULL, command, len);
      set_to_cnt(m_acal_to, m_acal_time);
      m_mode = ma_mode_auto_calibration_wait;
    } break;
    case ma_mode_auto_calibration_wait: {
      if (test_to_cnt(m_acal_to)) {
        if (mxifa_write_end(m_handle, false))
          m_mode = ma_mode_macro;
      } else if (m_abort_request) {
        mxifa_write_end(m_handle, true);
        m_mode = ma_mode_macro;
      }
    } break;
  }
}
// Установка времени интегрирования в периодах частоты сети (20 мс)
void mx_agilent_3458a_t::set_nplc(double nplc)
{
  irs::string nplc_str = nplc;
  nplc_str = "NPLC " + nplc_str;
  //m_get_measure_commands[m_time_int_voltage_index] = nplc_str;
  m_time_int_measure_command = nplc_str;
  m_get_resistance_commands[m_time_int_resistance_index] = nplc_str;
}
// Установка времени интегрирования в c
void mx_agilent_3458a_t::set_aperture(double aperture)
{
  irs::string aperture_str = aperture;
  aperture_str = "APER " + aperture_str;
  //m_get_measure_commands[m_time_int_voltage_index] = aperture_str;
  m_time_int_measure_command = aperture_str;
  m_get_resistance_commands[m_time_int_resistance_index] = aperture_str;
}
// Установка полосы фильтра
void mx_agilent_3458a_t::set_bandwidth(double /*bandwidth*/)
{
}
void mx_agilent_3458a_t::set_input_impedance(double /*impedance*/)
{
}
void mx_agilent_3458a_t::set_start_level(double /*level*/)
{
}
void mx_agilent_3458a_t::set_range(
  type_meas_t a_type_meas, double a_range)
{
  //if (m_create_error) return;
  irs::string range_str = a_range;
  switch(a_type_meas) {
    case tm_volt_dc: {
      m_voltage_type_direct_range = m_voltage_type_direct+" "+range_str;
      m_set_range_command = m_voltage_type_direct_range;
    } break;
    case tm_volt_ac: {
      m_voltage_type_alternate_range = m_voltage_type_alternate+" "+range_str;
      m_set_range_command = m_voltage_type_alternate_range;
    } break;
    case tm_current_dc: {
      m_current_type_direct_range = m_current_type_direct+" "+range_str;
      m_set_range_command = m_current_type_direct_range;
    } break;
    case tm_current_ac: {
      m_current_type_alternate_range = m_current_type_alternate+" "+range_str;
      m_set_range_command = m_current_type_alternate_range;
    } break;
    default : {
      // Остальные типы в данном мультиметре не используются
      IRS_ERROR(irs::ec_standard, "Unknown type range!");
    }
  }
  measure_create_commands(meas_set_range);
  m_command = mac_send_commands;
  m_status = meas_status_busy;
}

void mx_agilent_3458a_t::set_range_auto()
{
  irs::string range_str = "AUTO";
  m_voltage_type_direct_range = m_voltage_type_direct+" "+range_str;
  m_voltage_type_alternate_range = m_voltage_type_alternate+" "+range_str;
  m_current_type_direct_range = m_current_type_direct+" "+range_str;
  m_current_type_alternate_range = m_current_type_alternate+" "+range_str;
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

namespace {

double sampling_time_normalize(const double a_sampling_time,
  const double a_sampling_time_default);
double interval_normalize(const double a_interval,
  const double a_interval_default);
irs::filter_settings_t filter_settings_normalize(
  const irs::filter_settings_t a_filter_settings,
  const irs::filter_settings_t a_filter_settings_default);
mul_sample_format_t sample_format_normalize(
  const mul_sample_format_t a_sample_format);
double range_normalize(const double a_range);
double tick_max_time_normalize(const double a_tick_max_time);
irs::agilent_3458a_digitizer_t::size_type
get_sample_count(const double a_sampling_time, const double a_interval);
double get_aperture(const double a_sampling_time);
irs::irs_string_t make_sweep_cmd(const double sampling_time,
  const irs::agilent_3458a_digitizer_t::size_type a_sample_count);
irs::irs_string_t make_aperture_cmd(const double a_aperture);
irs::irs_string_t make_range_cmd(const double a_range);

} // empty namespace

// Класс для работы с мультиметром Agilent 3458A в режиме дискретизации
irs::agilent_3458a_digitizer_t::agilent_3458a_digitizer_t(
  irs::hardflow_t* ap_hardflow,
  const filter_settings_t& a_filter_settings,
  const double a_sampling_time_s,
  const double a_interval_s
):
  mp_hardflow(ap_hardflow),
  m_process(process_get_coefficient),
  m_filter_impulse_response_type(firt_infinite),
  m_filter_settings(a_filter_settings),
  m_window_function_form(wff_hann),
  m_fir_filter_coefficients(),
  m_iir_filter(),
  m_command_terminator(),
  m_commands(),
  m_buf_send(),
  m_buf_receive(),
  m_samples(),
  m_samples_for_user(),
  m_filtered_values(),
  m_filtered_values_for_user(),
  m_nplc_coef(20e-3),
  m_sampling_time_default(25e-6),
  m_interval_default(30.0),
  m_range_default(1000.0),
  m_iscale_byte_count(30),
  m_sample_size(sizeof(irs_u16)),
  m_sampling_time(25e-6),
  m_interval(0),
  m_settings_change_event(),
  m_new_interval(0),
  m_interval_change_event(),
  m_interval_change_gen_events(),
  m_new_sampling_time(0),
  m_sampling_time_s_change_event(),
  m_sampling_time_s_change_gen_events(),
  m_new_filter_settings(),
  m_filter_settings_change_event(),
  m_filter_settings_change_gen_events(),
  m_new_filter_impulse_response_type(firt_infinite),
  m_filter_impulse_response_type_change_event(),
  m_filter_impulse_response_type_change_gen_events(),
  m_window_function_form_change_event(),
  m_window_function_form_change_gen_events(),
  m_new_range(1000),
  m_range_change_event(),
  m_range_change_gen_events(),
  m_new_sample_format(mul_sample_format_int16),
  m_sample_format_change_event(),
  m_sample_format_change_gen_events(),
  m_calc_filtered_values_enabled(false),
  m_new_calc_filtered_values_enabled(false),
  m_calc_filtered_values_enabled_change_event(),
  m_calc_filtered_values_enabled_change_gen_events(),
  m_tick_max_time_s(0.005),
  m_need_receive_data_size(m_iscale_byte_count),
  m_set_settings_complete(false),
  m_coefficient_receive_ok(false),
  m_coefficient(0),
  mp_value(IRS_NULL),
  m_status(meas_status_success),
  m_delay_timer(irs::make_cnt_ms(1)),
  m_data_to_values(),
  m_accumulate_asynch(),
  m_filter_coef_sum_asynch(),
  m_sko_calc_asynch(1000),
  m_sko_for_user(0),
  m_sko_relative_for_user(0),
  m_delta_calc_asynch(),
  m_delta_absolute_for_user(0),
  m_delta_relative_for_user(0),
  m_iir_filter_asynch(),
  m_fir_filter_asynch()
{
  m_interval_change_gen_events.push_back(&m_interval_change_event);
  m_interval_change_gen_events.push_back(&m_settings_change_event);

  m_sampling_time_s_change_gen_events.push_back(
    &m_sampling_time_s_change_event);
  m_sampling_time_s_change_gen_events.push_back(&m_settings_change_event);

  m_filter_settings_change_gen_events.push_back(
    &m_filter_settings_change_event);
  m_filter_settings_change_gen_events.push_back(&m_settings_change_event);

  m_filter_impulse_response_type_change_gen_events.push_back(
    &m_filter_impulse_response_type_change_event);
  m_filter_impulse_response_type_change_gen_events.push_back(
    &m_settings_change_event);

  m_window_function_form_change_gen_events.push_back(
    &m_window_function_form_change_event);
  m_window_function_form_change_gen_events.push_back(&m_settings_change_event);

  m_range_change_gen_events.push_back(&m_range_change_event);
  m_range_change_gen_events.push_back(&m_settings_change_event);

  m_sample_format_change_gen_events.push_back(&m_sample_format_change_event);
  m_sample_format_change_gen_events.push_back(&m_settings_change_event);

  m_calc_filtered_values_enabled_change_gen_events.push_back(
    &m_calc_filtered_values_enabled_change_event);
  m_calc_filtered_values_enabled_change_gen_events.push_back(
    &m_settings_change_event);

  vector<math_type> num_coef_list;
  vector<math_type> denom_coef_list;
  get_coef_iir_filter(a_filter_settings, &num_coef_list, &denom_coef_list);
  m_iir_filter.set_coefficients(num_coef_list.begin(), num_coef_list.end(),
    denom_coef_list.begin(), denom_coef_list.end());

  m_iir_filter_asynch.set_coefficients(num_coef_list.begin(),
    num_coef_list.end(), denom_coef_list.begin(), denom_coef_list.end());

  const size_type order = get_sample_count(a_sampling_time_s, a_interval_s);
  get_coef_window_func_hann(order, &m_fir_filter_coefficients);
  m_fir_filter_asynch.set_coefficients(m_fir_filter_coefficients.begin(),
    m_fir_filter_coefficients.end());
  m_fir_filter_asynch.set_filt_value_buf(&m_filtered_values, 0);

  IRS_LIB_ERROR_IF(a_sampling_time_s <= 0, ec_standard,
    "Период дискретизации должен быть положительным числом");
  IRS_LIB_ERROR_IF(a_interval_s < 0, ec_standard,
    "Интервал должен быть неотрицательным числом");
  m_command_terminator.resize(2);
  m_command_terminator[0] = 0x0D;
  m_command_terminator[1] = 0x0A;

  m_commands.push_back("RESET");
  m_commands.push_back("PRESET DIG");
  m_commands.push_back("LFILTER ON");
  m_commands.push_back("MEM OFF");

  m_sampling_time = sampling_time_normalize(a_sampling_time_s,
    m_sampling_time_default);
  const double aperture = get_aperture(m_sampling_time);
  m_commands.push_back(make_aperture_cmd(aperture));
  m_interval = interval_normalize(a_interval_s, m_interval_default);
  // Округляем по правилам математики
  const size_type sample_count = get_sample_count(m_sampling_time, m_interval);
  m_commands.push_back(make_sweep_cmd(m_sampling_time, sample_count));
  m_commands.push_back(make_range_cmd(m_range_default));
  m_commands.push_back("ISCALE?");
  mp_hardflow->set_param(irst("read_timeout"), irst("3"));
}
irs::agilent_3458a_digitizer_t::~agilent_3458a_digitizer_t()
{
}

void irs::agilent_3458a_digitizer_t::get_param(const multimeter_param_t a_param,
  irs::raw_data_t<irs_u8> *ap_value) const
{
  IRS_LIB_ASSERT(ap_value);
  switch (a_param) {
    case mul_param_source_values: {
      const irs_u8* p_src_first =
        reinterpret_cast<const irs_u8*>(m_samples_for_user.data());
      const irs_u8* p_src_last = p_src_first +
        m_samples_for_user.size()*sizeof(double);
      ap_value->insert(ap_value->data(), p_src_first, p_src_last);
    } break;
    case mul_param_filtered_values: {
      const irs_u8* p_src_first =
        reinterpret_cast<const irs_u8*>(m_filtered_values_for_user.data());
      const irs_u8* p_src_last = p_src_first +
        m_filtered_values_for_user.size() * sizeof(double);
      ap_value->insert(ap_value->data(), p_src_first, p_src_last);
    } break;
    case mul_param_standard_deviation:
    case mul_param_standard_deviation_relative: {
      double deviation = 0.;
      if (a_param == mul_param_standard_deviation) {
        deviation = m_sko_for_user;
      } else {
        deviation = m_sko_relative_for_user;
      }
      ap_value->insert(ap_value->begin(),
        reinterpret_cast<const irs_u8*>(&deviation),
        reinterpret_cast<irs_u8*>(&deviation) + sizeof(deviation));
    } break;
    case mul_param_variation:
    case mul_param_variation_relative: {
      double variation = 0.;
      if (a_param == mul_param_variation) {
        variation = m_delta_absolute_for_user;
      } else {
        variation = m_delta_relative_for_user;
      }
      ap_value->insert(ap_value->data(),
        reinterpret_cast<const irs_u8*>(&variation),
        reinterpret_cast<const irs_u8*>(&variation) + sizeof(variation));
    } break;
    default : {
      // Игнорируем
    }
  }
}
void irs::agilent_3458a_digitizer_t::set_param(const multimeter_param_t a_param,
  const irs::raw_data_t<irs_u8> &a_value)
{
  switch (a_param) {   
    case mul_param_sampling_time_s: {
      if (sizeof(double) == a_value.size()) {
        m_new_sampling_time = sampling_time_normalize(
          *reinterpret_cast<const double*>(a_value.data()),
          m_sampling_time_default);
        m_sampling_time_s_change_gen_events.exec();
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Недопустимый размер параметра mul_param_sampling_time_s");
      }
    } break;
    case mul_param_filter_settings: {
      if (sizeof(filter_settings_t) == a_value.size()) {
        filter_settings_t filter_settings =
          *reinterpret_cast<const filter_settings_t*>(a_value.data());
        m_new_filter_settings = filter_settings_normalize(filter_settings,
          filter_settings);
        m_filter_settings_change_gen_events.exec();
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Недопустимый размер параметра mul_param_filter_settings");
      }
    } break;
    case mul_param_sample_format: {
      if (sizeof(mul_sample_format_t) == a_value.size()) {
        m_new_sample_format = sample_format_normalize(
          *reinterpret_cast<const mul_sample_format_t*>(a_value.data()));
        m_sample_format_change_gen_events.exec();
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Недопустимый размер параметра mul_param_filter_settings");
      }
    } break;
    case mul_param_tick_max_time_s: {
      if (sizeof(double) == a_value.size()) {
        m_tick_max_time_s = tick_max_time_normalize(
          *reinterpret_cast<const double*>(a_value.data()));
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Недопустимый размер параметра mul_param_filter_settings");
      }
    } break;
    case mul_param_filter_impulse_response_type: {
      if (sizeof(filter_impulse_response_type_t) == a_value.size()) {
        m_new_filter_impulse_response_type =
          *reinterpret_cast<const filter_impulse_response_type_t*>(
          a_value.data());
        m_filter_impulse_response_type_change_gen_events.exec();
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Недопустимый размер параметра "
            "mul_param_filter_impulse_response_type");
      }
    } break;
    case mul_param_window_function_form: {
      if (sizeof(window_function_form_t) == a_value.size()) {
        m_new_window_function_form =
          *reinterpret_cast<const window_function_form_t*>(a_value.data());
        m_window_function_form_change_gen_events.exec();
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Недопустимый размер параметра mul_param_window_function_form");
      }
    } break;
    case mul_param_filtered_values_enabled: {
      if (sizeof(bool) == a_value.size()) {
        m_new_calc_filtered_values_enabled =
          *reinterpret_cast<const bool*>(a_value.data());
        m_calc_filtered_values_enabled_change_gen_events.exec();
      } else {
        IRS_LIB_ERROR(ec_standard,
          "Недопустимый размер параметра mul_param_filtered_values_enabled");
      }
    } break;
    default : {
      // Игнорируем
    }
  }
}
bool irs::agilent_3458a_digitizer_t::is_param_exists(
  const multimeter_param_t a_param) const
{
  return (a_param == mul_param_source_values) ||
    (a_param == mul_param_filtered_values) ||
    (a_param == mul_param_standard_deviation) ||
    (a_param == mul_param_standard_deviation_relative) ||
    (a_param == mul_param_variation) ||
    (a_param == mul_param_variation_relative) ||
    (a_param == mul_param_sampling_time_s) ||
    (a_param == mul_param_filter_settings) ||
    (a_param == mul_param_sample_format) ||
    (a_param == mul_param_window_function_form) ||
    (a_param == mul_param_filter_impulse_response_type) ||
    (a_param == mul_param_tick_max_time_s) ||
    (a_param == mul_param_filtered_values_enabled);
}
void irs::agilent_3458a_digitizer_t::set_dc()
{
}
// Установить режим измерения переменного напряжения
void irs::agilent_3458a_digitizer_t::set_ac()
{
}
// Установить положителный фронт запуска
void irs::agilent_3458a_digitizer_t::set_positive()
{
}
// Установить отрицательный фронт канала
void irs::agilent_3458a_digitizer_t::set_negative()
{
}
// Чтение значения при текущем типа измерения
void irs::agilent_3458a_digitizer_t::get_value(double *ap_value)
{
  mp_value = ap_value;
  m_status = meas_status_busy;
  m_buf_receive.clear();
}
// Чтение напряжения
void irs::agilent_3458a_digitizer_t::get_voltage(double *voltage)
{
  mp_value = voltage;
  m_status = meas_status_busy;
  m_buf_receive.clear();
}
// Чтения силы тока
void irs::agilent_3458a_digitizer_t::get_current(double */*current*/)
{
}
// Чтение сопротивления
void irs::agilent_3458a_digitizer_t::get_resistance2x(double */*resistance*/)
{
}
// Чтение сопротивления
void irs::agilent_3458a_digitizer_t::get_resistance4x(double */*resistance*/)
{
}
// Чтение частоты
void irs::agilent_3458a_digitizer_t::get_frequency(double */*frequency*/)
{
}
// Чтение усредненного сдвира фаз
void irs::agilent_3458a_digitizer_t::get_phase_average(double */*phase_average*/)
{
}
// Чтение фазового сдвига
void irs::agilent_3458a_digitizer_t::get_phase(double * /*phase*/)
{
}
// Чтение временного интервала
void irs::agilent_3458a_digitizer_t::get_time_interval(double * /*time_interval*/)
{
}
// Чтение усредненного временного интервала
void irs::agilent_3458a_digitizer_t::get_time_interval_average(
  double * /*ap_time_interval*/)
{
}
// Запуск автокалибровки (команда ACAL) мультиметра
void irs::agilent_3458a_digitizer_t::auto_calibration()
{
}
// Чтение статуса текущей операции
meas_status_t irs::agilent_3458a_digitizer_t::status()
{
  return m_status;
}
// Прерывание текущей операции
void irs::agilent_3458a_digitizer_t::abort()
{
}

// Элементарное действие
void irs::agilent_3458a_digitizer_t::tick()
{
  IRS_LIB_ASSERT(mp_hardflow);
  mp_hardflow->tick();
  commands_to_buf_send();
  static measure_time_t measure_time_calc;
  switch (m_process) {
    case process_write_data: {
      write_data_tick();
      if (m_buf_send.empty()) {
        m_process = process_wait;
      } else {
        // Остаемся в этом процессе до окончания записи
      }
    } break;
    case process_get_value: {
      read_data_tick();
      if (m_buf_receive.size() == m_need_receive_data_size) {
        bool flip_data_enabled = false;
        #if (IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)
        flip_data_enabled = true;
        #endif // (IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)
        m_data_to_values.set_tick_max_time(m_tick_max_time_s);
        m_data_to_values.set(flip_data_enabled, m_sample_size,
          m_coefficient, &m_buf_receive, &m_samples);
        m_accumulate_asynch.clear();
        m_accumulate_asynch.set_tick_max_time(m_tick_max_time_s);
        m_filter_coef_sum_asynch.clear();
        m_filter_coef_sum_asynch.set_tick_max_time(m_tick_max_time_s);
        m_sko_calc_asynch.clear();
        m_sko_calc_asynch.set_tick_max_time(m_tick_max_time_s);
        m_delta_calc_asynch.clear();
        m_delta_calc_asynch.set_tick_max_time(m_tick_max_time_s);
        if (m_filter_impulse_response_type == firt_infinite) {
          m_iir_filter_asynch.set_tick_max_time(m_tick_max_time_s);
          m_iir_filter_asynch.set_filt_value_buf(&m_filtered_values,
            get_sample_count(m_sampling_time, m_interval));
        } else {
          m_fir_filter_asynch.set_tick_max_time(m_tick_max_time_s);
          if (m_calc_filtered_values_enabled) {
            m_fir_filter_asynch.set_filt_value_buf(&m_filtered_values,
              get_sample_count(m_sampling_time, m_interval));
          } else {
            m_fir_filter_asynch.set_filt_value_buf(IRS_NULL, 0);
          }
        }
        m_process = process_calc;
        measure_time_calc.start();
        IRS_LIB_DBG_MSG("Начинаем вычисления");
      } else {
        // Читаем необходимое количество данных
      }
    } break;
    case process_calc: {
      if (!m_data_to_values.completed()) {
        m_data_to_values.tick();
        if (m_data_to_values.completed()) {
          IRS_LIB_DBG_MSG("Конвертация значений завершена: " <<
            measure_time_calc.get() << " с");
          m_samples_for_user = m_samples;
          m_accumulate_asynch.add(m_samples.begin(), m_samples.end());
        } else {
          // Операция еще не завершена
        }
      } else if (!m_accumulate_asynch.completed()) {
        m_accumulate_asynch.tick();
        if (m_accumulate_asynch.completed()) {
          IRS_LIB_DBG_MSG("Вычисление суммы завершено: " <<
            measure_time_calc.get() << " с");
          if (m_filter_impulse_response_type == firt_finite) {
            calc_coef_fir_filter();
            m_filter_coef_sum_asynch.add(m_fir_filter_coefficients.begin(),
              m_fir_filter_coefficients.end());
          } else {
            set_coef_filter();
            filter_start();
          }
        } else {
          // Операция еще не завершена
        }
      } else if ((m_filter_impulse_response_type == firt_finite) &&
        !m_filter_coef_sum_asynch.completed()) {

        m_filter_coef_sum_asynch.tick();
        if (m_filter_coef_sum_asynch.completed()) {
          set_coef_filter();
          filter_start();
        } else {
          // Подсчет суммы коэффициентов еще не завершен
        }
      } else if (!filter_completed()) {
        filter_tick();
        if (filter_completed()) {
          *mp_value = static_cast<double>(filter_get());   
          IRS_LIB_DBG_MSG("Фильтрация завершена: " <<
            measure_time_calc.get() << " с");
          if (m_calc_filtered_values_enabled) {
            m_filtered_values_for_user = m_filtered_values;
            m_sko_calc_asynch.resize(m_filtered_values.size());
            m_sko_calc_asynch.add(m_filtered_values.begin(),
              m_filtered_values.end());
          } else {
            m_filtered_values_for_user.clear();
            m_sko_calc_asynch.resize(m_samples.size());
            m_sko_calc_asynch.add(m_samples.begin(), m_samples.end());
          }
        } else {
          // Фильтрация еще не завершена
        }
      } else if (!m_sko_calc_asynch.completed()) {
        m_sko_calc_asynch.tick();
        if (m_sko_calc_asynch.completed()) {
          IRS_LIB_DBG_MSG("Вычисление СКО завершено: " <<
            measure_time_calc.get() << " с");
          m_sko_for_user = m_sko_calc_asynch;
          m_sko_relative_for_user = m_sko_calc_asynch.relative();
          if (m_calc_filtered_values_enabled) {
            m_delta_calc_asynch.resize(m_filtered_values.size());
            m_delta_calc_asynch.add(m_filtered_values.begin(),
              m_filtered_values.end());
          } else {
            m_delta_calc_asynch.resize(m_samples.size());
            m_delta_calc_asynch.add(m_samples.begin(), m_samples.end());
          }
        } else {
          // Операция еще не завершена
        }
      } else if (!m_delta_calc_asynch.completed()) {
        m_delta_calc_asynch.tick();
        if (m_delta_calc_asynch.completed()) {

        } else {
          // Операция еще не завершена
        }
      } else {
        IRS_LIB_DBG_MSG("Вычисление дельты завершено: " <<
          measure_time_calc.get() << " с");
        m_delta_absolute_for_user = m_delta_calc_asynch.absolute();
        m_delta_relative_for_user = m_delta_calc_asynch.relative();
        m_status = meas_status_success;
        m_process = process_wait;
        IRS_LIB_DBG_MSG("Вычисления завершены. " <<
          measure_time_calc.get() << " c");
      }
    } break;
    case process_set_settings: {
      m_settings_change_event.exec();
      if (m_interval_change_event.check()) {
        set_interval();
        m_process = process_write_data;
      } else if (m_sampling_time_s_change_event.check()) {
        set_sampling_time();
        m_process = process_get_coefficient;
      } else if (m_filter_settings_change_event.check()) {
        set_filter_settings();
        m_process = process_write_data;
      } else if (m_filter_impulse_response_type_change_event.check()) {
        set_filter_impulse_response_type();
        m_process = process_write_data;
      } else if (m_window_function_form_change_event.check()) {
        set_window_function_form();
        m_process = process_write_data;
      } else if (m_sample_format_change_event.check()) {
        set_sample_format();
        m_process = process_get_coefficient;
      } else if (m_range_change_event.check()) {
        set_range();
        m_process = process_get_coefficient;
      } else if (m_calc_filtered_values_enabled_change_event.check()) {
        set_filtered_values_enabled();
        m_process = process_write_data;
      } else {
        m_settings_change_event.check();
        m_process = process_wait;
      }
    } break;
    case process_get_coefficient: {
      write_data_tick();
      if (m_buf_send.empty()) {
        read_data_tick();
        std_string_t buf(reinterpret_cast<char*>(m_buf_receive.data()),
        m_buf_receive.size());
        size_type pos = buf.find("\r\n");
        if (pos != irs_string_t::npos) {
          irs_string_t coefficient_str = buf.substr(0, pos);
          if (str_to_num_classic(coefficient_str, &m_coefficient)) {
            m_buf_receive.clear();

            string_type read_timeout_str;
            num_to_str(m_interval*2 + 1, &read_timeout_str);
            mp_hardflow->set_param(irst("read_timeout"), read_timeout_str);

            m_commands.push_back("TARM SYN");
            m_commands.push_back("TRIG SYN");
            m_need_receive_data_size =
              get_sample_count(m_sampling_time, m_interval)*m_sample_size;
            m_process = process_write_data;
          } else {
            IRS_LIB_ASSERT_MSG("Значение коэффициента считать не удалось");
          }
        } else {
          // Конец команды не найден
        }
      } else {
        // Ожидаем окончания записи
      }
    } break;
    case process_wait: {
      if (m_settings_change_event.check()) {
        m_process = process_set_settings;
      } else if (m_status == meas_status_busy) {
        m_samples.clear();
        m_filtered_values.clear();
        const size_type sample_count =
          get_sample_count(m_sampling_time, m_interval);
        m_samples.reserve(sample_count);
        m_process = process_get_value;
      } else {
        // Ожидаем запрос на измерение или запрос на изменение настроек
      }
    } break;
    default: {
      IRS_LIB_ASSERT("Недопустимый процесс");
    }
  }
}
// Установка времени интегрирования в периодах частоты сети (20 мс)
void irs::agilent_3458a_digitizer_t::set_nplc(double nplc)
{
  m_new_interval = interval_normalize(nplc*m_nplc_coef, m_interval_default);
  m_interval_change_gen_events.exec();
}

// Установка времени интегрирования в c
void irs::agilent_3458a_digitizer_t::set_aperture(double /*aperture*/)
{
}
// Установка полосы фильтра
void irs::agilent_3458a_digitizer_t::set_bandwidth(double /*bandwidth*/)
{
}
// Установка входного сопротивления канала
void irs::agilent_3458a_digitizer_t::set_input_impedance(double /*impedance*/)
{
}
// Устсновка уровня запуска канала
void irs::agilent_3458a_digitizer_t::set_start_level(double /*level*/)
{
}
// Установка диапазона измерений
void irs::agilent_3458a_digitizer_t::set_range(type_meas_t a_type_meas,
  double a_range)
{
  if (m_status != meas_status_busy) {
    m_new_range = range_normalize(a_range);
    m_range_change_gen_events.exec();
    m_status = meas_status_success;
  } else {
    IRS_LIB_ERROR(ec_standard, "Предыдущая команда еще не завершена");
  }
}

// Установка автоматического выбора диапазона измерений
void irs::agilent_3458a_digitizer_t::set_range_auto()
{
}

void irs::agilent_3458a_digitizer_t::commands_to_buf_send()
{
  const irs_u8* p_cmd_terminator_begin =
    m_command_terminator.data();
  const irs_u8* p_cmd_terminator_end =
    m_command_terminator.data() + m_command_terminator.size();

  const size_type cmd_count = m_commands.size();
  for (size_type cmd_i = 0; cmd_i < cmd_count; cmd_i++) {
    const irs_u8* p_cmd_begin = reinterpret_cast<const irs_u8*>(
      m_commands[cmd_i].c_str());
    const irs_u8* p_cmd_end = p_cmd_begin +
      m_commands[cmd_i].size();
    m_buf_send.insert(m_buf_send.data() + m_buf_send.size(), p_cmd_begin,
      p_cmd_end);
    // Вставляем терминатор после каждой команды
    m_buf_send.insert(m_buf_send.data() + m_buf_send.size(),
      p_cmd_terminator_begin, p_cmd_terminator_end);
  }
  m_commands.clear();
}

void irs::agilent_3458a_digitizer_t::read_data_tick()
{
  const size_type buf_receive_cur_size = m_buf_receive.size();
  m_buf_receive.resize(static_cast<size_type>(m_need_receive_data_size));
  const size_type read_size = mp_hardflow->read(
    mp_hardflow->channel_next(), m_buf_receive.data() + buf_receive_cur_size,
      m_need_receive_data_size - buf_receive_cur_size);
  m_buf_receive.resize(buf_receive_cur_size + read_size);
  IRS_LIB_DBG_MSG_IF(read_size > 0, "Прочитано = " << read_size);
}

void irs::agilent_3458a_digitizer_t::write_data_tick()
{
  if (!m_buf_send.empty()) {
    const size_type write_size = mp_hardflow->write(
    mp_hardflow->channel_next(), m_buf_send.data(), m_buf_send.size());
    IRS_DBG_MSG("Записано = " << write_size);
    if (write_size > 0) {
      const size_type buf_new_size = m_buf_send.size() - write_size;
      memmoveex(m_buf_send.data(), m_buf_send.data() + write_size,
        buf_new_size);
      m_buf_send.resize(buf_new_size);
    } else {
      // Ничего не записано
    }
  } else {
    // Нет данных для записи
  }
}

void irs::agilent_3458a_digitizer_t::set_interval()
{
  m_interval = m_new_interval;
  const size_type sample_count =
    get_sample_count(m_sampling_time, m_interval);
  m_need_receive_data_size = sample_count*m_sample_size;
  m_commands.push_back(make_sweep_cmd(m_sampling_time, sample_count));
  string_type read_timeout_str;
  num_to_str(m_interval*2 + 1, &read_timeout_str);
  mp_hardflow->set_param(irst("read_timeout"), read_timeout_str);
}

void irs::agilent_3458a_digitizer_t::set_sampling_time()
{
  m_sampling_time = m_new_sampling_time;
  const double aperture = get_aperture(m_sampling_time);
  m_commands.push_back(make_aperture_cmd(aperture));
  const size_type sample_count = get_sample_count(m_sampling_time,
    m_interval);
  m_need_receive_data_size = sample_count*m_sample_size;
  m_commands.push_back(make_sweep_cmd(m_sampling_time, sample_count));
  m_commands.push_back("TARM HOLD");
  m_commands.push_back("ISCALE?");
  mp_hardflow->set_param(irst("read_timeout"), irst("3"));
  m_need_receive_data_size = m_iscale_byte_count;
}

void irs::agilent_3458a_digitizer_t::set_filter_settings()
{
  m_filter_settings = m_new_filter_settings;
}

void irs::agilent_3458a_digitizer_t::set_filter_impulse_response_type()
{
  m_filter_impulse_response_type = m_new_filter_impulse_response_type;
}

void irs::agilent_3458a_digitizer_t::set_window_function_form()
{
  m_filter_impulse_response_type = m_new_filter_impulse_response_type;
}

void irs::agilent_3458a_digitizer_t::set_sample_format()
{
  if (m_new_sample_format == mul_sample_format_int16) {
    m_sample_size = sizeof(short_int_sample_format_type);
    m_commands.push_back("MFORMAT SINT");
    m_commands.push_back("OFORMAT SINT");
  } else {
    m_sample_size = sizeof(double_int_sample_format_type);
    m_commands.push_back("MFORMAT DINT");
    m_commands.push_back("OFORMAT DINT");
  }
  m_commands.push_back("TARM HOLD");
  m_commands.push_back("ISCALE?");
  mp_hardflow->set_param(irst("read_timeout"), irst("3"));
  m_need_receive_data_size = m_iscale_byte_count;
}

void irs::agilent_3458a_digitizer_t::set_range()
{
  m_commands.push_back(make_range_cmd(m_new_range));
  m_commands.push_back("TARM HOLD");
  m_commands.push_back("ISCALE?");
  mp_hardflow->set_param(irst("read_timeout"), irst("3"));
  m_need_receive_data_size = m_iscale_byte_count;
}

void irs::agilent_3458a_digitizer_t::set_filtered_values_enabled()
{
  m_calc_filtered_values_enabled = m_new_calc_filtered_values_enabled;
}

void irs::agilent_3458a_digitizer_t::set_coef_filter()
{
  if (m_filter_impulse_response_type == firt_infinite) {
    vector<math_type> num_coef_list;
    vector<math_type> denom_coef_list;
    get_coef_iir_filter(m_filter_settings, &num_coef_list, &denom_coef_list);
    m_iir_filter_asynch.set_coefficients(num_coef_list.begin(),
      num_coef_list.end(), denom_coef_list.begin(), denom_coef_list.end());
  } else {
    const math_type coef_sum = m_filter_coef_sum_asynch.get();
    const size_type coef_count = m_fir_filter_coefficients.size();
    if (coef_sum != 0) {
      for (size_type coef_i = 0; coef_i < coef_count; coef_i++) {
        m_fir_filter_coefficients[coef_i] /= coef_sum;
      }
    } else {
      for (size_type coef_i = 0; coef_i < coef_count; coef_i++) {
        m_fir_filter_coefficients[coef_i] = 0;
      }
    }
    m_fir_filter_asynch.set_coefficients(m_fir_filter_coefficients.begin(),
      m_fir_filter_coefficients.end());
  }
}

void irs::agilent_3458a_digitizer_t::calc_coef_fir_filter()
{
  const size_type order = get_sample_count(m_sampling_time, m_interval);
  get_coef_fir_filter(m_window_function_form, order,
    &m_fir_filter_coefficients);
}

void irs::agilent_3458a_digitizer_t::filter_start()
{
  math_type average = 0;
  if (!m_samples.empty()) {
    average = m_accumulate_asynch.get()/m_samples.size();
  } else {
    // Среднее оставляем нулем
  }
  if (m_filter_impulse_response_type == firt_infinite) {
    m_iir_filter_asynch.sync(average);
    m_iir_filter_asynch.set(m_samples.begin(), m_samples.end());
  } else {
    m_fir_filter_asynch.sync(average);
    m_fir_filter_asynch.set(m_samples.begin(), m_samples.end());
  }
}

void irs::agilent_3458a_digitizer_t::filter_tick()
{
  if (m_filter_impulse_response_type == firt_infinite) {
    m_iir_filter_asynch.tick();
  } else {
    m_fir_filter_asynch.tick();
  }
}

bool irs::agilent_3458a_digitizer_t::filter_completed()
{
  bool completed = true;
  if (m_filter_impulse_response_type == firt_infinite) {
    completed = m_iir_filter_asynch.completed();
  } else {
    completed = m_fir_filter_asynch.completed();
  }
  return completed;
}

irs::agilent_3458a_digitizer_t::math_type
irs::agilent_3458a_digitizer_t::filter_get()
{
  math_type value = 0;
  if (m_filter_impulse_response_type == firt_infinite) {
    value = m_iir_filter_asynch.get();
  } else {
    value = m_fir_filter_asynch.get();
    /*if (m_filter_coef_sum_asynch.get() != 0) {
      value = m_fir_filter_asynch.get()/m_filter_coef_sum_asynch.get();
      IRS_LIB_DBG_MSG("Чистое значение фильтра " << m_fir_filter_asynch.get());
      IRS_LIB_DBG_MSG("Итоговое значение фильтра " << value);
    } else {
      value = 0;
    }*/
    IRS_LIB_DBG_MSG("Сумма коэффициентов " << m_filter_coef_sum_asynch.get());
  }
  return value;
}

/*void irs::agilent_3458a_digitizer_t::filtered_values_normalize()
{
  if (m_calc_filtered_values_enabled &&
    (m_filter_impulse_response_type == firt_finite)) {

    const math_type coef_sum = m_filter_coef_sum_asynch.get();
    const size_type value_count = m_filtered_values.size();
    if (coef_sum != 0) {
      for (size_type value_i = 0; value_i < value_count; value_i++) {
        m_filtered_values[value_i] /= coef_sum;
      }
    } else {
      for (size_type value_i = 0; value_i < value_count; value_i++) {
        m_filtered_values[value_i] = 0;
      }
    }
  }
}*/

namespace {

double sampling_time_normalize(const double a_sampling_time,
  const double a_sampling_time_default)
{
  return (a_sampling_time > 0) ? a_sampling_time : a_sampling_time_default;
}

double interval_normalize(const double a_interval,
  const double a_interval_default)
{
  return (a_interval > 0) ? a_interval : a_interval_default;
}

irs::filter_settings_t filter_settings_normalize(
  const irs::filter_settings_t a_filter_settings,
  const irs::filter_settings_t a_filter_settings_default)
{
  irs::filter_settings_t filter_settings_normal;
  if ((a_filter_settings.family == irs::ff_chebyshev_ripple_stop) ||
    (a_filter_settings.order <= 0)) {
    filter_settings_normal = a_filter_settings_default;
  } else {
    filter_settings_normal = a_filter_settings;
  }
  return filter_settings_normal;
}

mul_sample_format_t sample_format_normalize(
  const mul_sample_format_t a_sample_format)
{
  mul_sample_format_t mul_sample_format = mul_sample_format_int16;
  if ((a_sample_format == mul_sample_format_int16) ||
    (a_sample_format == mul_sample_format_int32)) {

    mul_sample_format = a_sample_format;
  } else {
    // Возвращаем стандартное значение
  }
  return mul_sample_format;
}

double range_normalize(const double a_range)
{
  double range = 1000.0;
  if (a_range > 100.0) {
    range = 1000.0;
  } else if (a_range > 10.0) {
    range = 100.0;
  } else if (a_range > 1.0) {
    range = 10.0;
  } else if (a_range > 0.1) {
    range = 1.0;
  } else if (a_range >= 0.0) {
    range = 0.1;
  } else {
    range = 1000.0;
  }
  return range;
}

double tick_max_time_normalize(const double a_tick_max_time)
{
  return (a_tick_max_time > 0) ? a_tick_max_time: 0.01;
}

irs::agilent_3458a_digitizer_t::size_type
get_sample_count(const double a_sampling_time, const double a_interval)
{
  typedef irs::agilent_3458a_digitizer_t::size_type size_type;
  return static_cast<size_type>(floor(a_interval/a_sampling_time + 0.5));
}

double get_aperture(const double a_sampling_time)
{
  return a_sampling_time/10.0;
}

irs::irs_string_t make_sweep_cmd(const double a_sampling_time,
  const size_t a_sample_count)
{
  irs::irs_string_t sampling_time_str;
  num_to_str_classic(a_sampling_time, &sampling_time_str);
  irs::irs_string_t sample_count_str;
  num_to_str_classic(a_sample_count, &sample_count_str);
  return irs::irs_string_t("SWEEP " + sampling_time_str + ", " +
    sample_count_str);
}

irs::irs_string_t make_aperture_cmd(const double a_aperture)
{
  irs::irs_string_t aperture_str;
  num_to_str_classic(a_aperture, &aperture_str);
  return irs::irs_string_t("APER " + aperture_str);
}

irs::irs_string_t make_range_cmd(const double a_range)
{
  irs::irs_string_t range_str;
  num_to_str_classic(a_range, &range_str);
  return irs::irs_string_t("RANGE " + range_str);
}

} // empty namespace

void irs::flip_data(irs_u8* ap_data, const size_t a_size)
{
  irs_u8* data = new irs_u8[a_size];
  for (size_t byte_i = 0; byte_i < a_size; byte_i++) {
    *(data + byte_i) = *(ap_data + (a_size - byte_i - 1));
  }
  memcpy(ap_data, data, a_size);
  delete[] data;
}

// class data_to_values_t
irs::data_to_values_t::data_to_values_t():
  mp_data(IRS_NULL),
  m_flip_data_enabled(false),
  m_value_size(1),
  m_coefficient(0),
  mp_samples(IRS_NULL),
  m_pos(0),
  m_sample_count(0),
  m_completed(false),
  m_delta_tick(1000),
  m_tick_timer(make_cnt_s(0.001))
{
}

irs::data_to_values_t::data_to_values_t(
  const bool a_flip_data_enabled,
  const size_type a_value_size,
  const double a_coefficient,
  const raw_data_t<irs_u8>* ap_data,
  raw_data_t<double>* ap_samples
):
  mp_data(IRS_NULL),
  m_flip_data_enabled(false),
  m_value_size(1),
  m_coefficient(0),
  mp_samples(IRS_NULL),
  m_pos(0),
  m_sample_count(0),
  m_completed(false),
  m_delta_tick(1000),
  m_tick_timer(make_cnt_s(0.001))
{
  set(a_flip_data_enabled, a_value_size, a_coefficient, ap_data, ap_samples);
}

void irs::data_to_values_t::set(
  const bool a_flip_data_enabled,
  const size_type a_value_size,
  const double a_coefficient,
  const raw_data_t<irs_u8>* ap_data,
  raw_data_t<double>* ap_samples)
{
  IRS_LIB_ASSERT(a_value_size <= sizeof(irs_i64));
  mp_data = ap_data;
  m_flip_data_enabled = a_flip_data_enabled;
  m_value_size = a_value_size;
  m_coefficient = a_coefficient;
  mp_samples = ap_samples;
  m_pos = 0;
  if (mp_data && mp_samples) {
    m_sample_count = mp_data->size()/a_value_size;
    mp_samples->clear();
    mp_samples->resize(m_sample_count);
    m_completed = false;
  } else {
    m_sample_count = 0;
    m_completed = true;
  }
}

bool irs::data_to_values_t::completed() const
{
  return m_completed;
}

void irs::data_to_values_t::tick()
{
  if (!m_completed) {
    m_tick_timer.start();
    raw_data_t<irs_u8> mult_value_raw(m_value_size);
    while (!m_tick_timer.check() && !m_completed) {
      const size_type cur_max_pos = min((m_pos + m_delta_tick), m_sample_count);
      while (m_pos < cur_max_pos) {
        const irs_u8* p_mult_value = (mp_data->data() + (m_pos*m_value_size));
        memcpy(mult_value_raw.data(), p_mult_value, m_value_size);
        //#if (IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)
        if (m_flip_data_enabled) {
          flip_data(mult_value_raw.data(), m_value_size);
        } else {
          // Разворот данных не требуется
        }
        irs_i64 mult_value = data_to_value<irs_i64>(mult_value_raw.data(),
          mult_value_raw.size());
        const double sample = mult_value*m_coefficient;
        (*mp_samples)[m_pos] = sample;
        m_pos++;
      }
      m_completed = (m_pos == m_sample_count);
    }
  } else {
    // Операция завершена
  }
}

void irs::data_to_values_t::abort()
{
  if (!m_completed) {
    mp_samples->clear();
    m_completed = true;
  } else {
    // Операция уже завершилась
  }
}

void irs::data_to_values_t::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "Максимальное время тика должно быть больше нуля");
  m_tick_timer.set(make_cnt_s(a_set_tick_max_time_s));
}


bool irs::data_to_values_test()
{
  typedef irs_size_t size_type;
  typedef double math_type;
  const irs_size_t sample_count = 100000;
  raw_data_t<irs_u8> data(sample_count*sizeof(irs_i16));
  for (irs_size_t byte_i = 0; byte_i < sample_count*sizeof(irs_i16); byte_i++) {
    data[byte_i] = static_cast<irs_u8>(rand());
  }
  const double coefficient = 10;
  raw_data_t<math_type> samples_first;
  multimeter_data_to_sample_array<irs_i16>(data, coefficient, &samples_first);
  raw_data_t<math_type> samples_second;
  data_to_values_t multimeter_data_to_sample_array(true, sizeof(irs_i16),
    coefficient, &data, &samples_second);
  while (!multimeter_data_to_sample_array.completed()) {
    multimeter_data_to_sample_array.tick();
  }
  return equal(samples_first.data(),
    samples_first.data() + samples_first.size(),
    samples_second.data());;
}

bool irs::accumulate_asynch_test()
{
  typedef irs_size_t size_type;
  typedef double math_type;
  const irs_size_t sample_count = 100000;
  vector<math_type> samples(sample_count, 0);

  math_type sum = 0;
  for (irs_size_t sample_i = 0; sample_i < sample_count; sample_i++) {
    samples[sample_i] = rand();
    sum += samples[sample_i];
  }
  accumulate_asynch_t<math_type, vector<math_type>::iterator> accumulate_asynch;
  accumulate_asynch.add(samples.begin(), samples.end());
  while(!accumulate_asynch.completed()) {
    accumulate_asynch.tick();
  }
  return sum == accumulate_asynch.get();
}

bool irs::sko_calc_asynch_test()
{
  typedef irs_size_t size_type;
  typedef double math_type;
  bool success = true;
  const irs_size_t sample_count = 100000;
  vector<math_type> samples(sample_count, 0);
  sko_calc_t<math_type, math_type> sko_calc(sample_count);

  for (irs_size_t sample_i = 0; sample_i < sample_count; sample_i++) {
    samples[sample_i] = rand();
    sko_calc.add(samples[sample_i]);
  }
  sko_calc_asynch_t<math_type, math_type, vector<math_type>::iterator>
    sko_calc_asynch(sample_count);
  sko_calc_asynch.add(samples.begin(), samples.end());
  while(!sko_calc_asynch.completed()) {
    sko_calc_asynch.tick();
  }
  success = success && (sko_calc == sko_calc_asynch);
  success = success && (sko_calc.relative() == sko_calc_asynch.relative());
  success = success && (sko_calc.average() == sko_calc_asynch.average());
  return success;
}

bool irs::delta_calc_asynch_test()
{
  typedef irs_size_t size_type;
  typedef double math_type;
  bool success = true;
  const irs_size_t sample_count = 100000;
  vector<math_type> samples(sample_count, 0);
  delta_calc_t<math_type> delta_calc(sample_count);

  for (irs_size_t sample_i = 0; sample_i < sample_count; sample_i++) {
    samples[sample_i] = rand();
    delta_calc.add(samples[sample_i]);
  }
  delta_calc_asynch_t<math_type, vector<math_type>::iterator>
    delta_calc_asynch(sample_count);
  delta_calc_asynch.add(samples.begin(), samples.end());
  while(!delta_calc_asynch.completed()) {
    delta_calc_asynch.tick();
  }
  success = success && (delta_calc.absolute() == delta_calc_asynch.absolute());
  success = success && (delta_calc.relative() == delta_calc_asynch.relative());
  success = success && (delta_calc.average() == delta_calc_asynch.average());
  return success;
}

bool irs::iir_filter_asynch_test()
{
  typedef irs_size_t size_type;
  typedef long double math_type;
  const irs_size_t sample_count = 100000;
  vector<math_type> samples(sample_count, 0);

  filter_settings_t filter_settings;
  filter_settings.family = ff_chebyshev_ripple_pass;
  filter_settings.bandform = fb_low_pass;
  filter_settings.order = 5;
  filter_settings.sampling_time_s = 20e-6;
  filter_settings.low_cutoff_freq_hz = 20;
  filter_settings.passband_ripple_db = 0.3;
  vector<math_type> num_coef_list;
  vector<math_type> denom_coef_list;
  get_coef_iir_filter(filter_settings, &num_coef_list, &denom_coef_list);
  iir_filter_t<math_type> iir_filter(num_coef_list.begin(), num_coef_list.end(),
    denom_coef_list.begin(), denom_coef_list.end());
  raw_data_t<double> filt_values_first(sample_count);
  for (irs_size_t sample_i = 0; sample_i < sample_count; sample_i++) {
    samples[sample_i] = rand();
    iir_filter.set(samples[sample_i]);
    filt_values_first[sample_i] = static_cast<double>(iir_filter.get());
  }

  raw_data_t<double> filt_values_second;
  typedef iir_filter_asynch_t<math_type, vector<math_type>::iterator,
    raw_data_t<double> > iir_filter_asynch_type;
  iir_filter_asynch_type iir_filter_asynch(
    num_coef_list.begin(),
    num_coef_list.end(),
    denom_coef_list.begin(),
    denom_coef_list.end(),
    &filt_values_second,
    sample_count);
  iir_filter_asynch.set(samples.begin(), samples.end());
  while (!iir_filter_asynch.completed()) {
    iir_filter_asynch.tick();
  }
  return equal(filt_values_first.begin(), filt_values_first.end(),
    filt_values_second.begin());
}

#endif // IRS_FULL_STDCPPLIB_SUPPORT

// Класс для работы с мультиметром v7-78/1

// Конструктор
irs::v7_78_1_t::v7_78_1_t(mxifa_ch_t channel,
  gpib_addr_t address,
  multimeter_mode_type_t a_mul_mode_type
):
  m_meas_type(DC_MEAS),
  m_res_meas_type(RES_MEAS_2x),
  m_mul_mode_type(a_mul_mode_type),
  f_init_commands(),
  m_nplc_voltage_dc_index(0),
  m_band_width_voltage_ac_index(0),
  m_get_value_commands(0),
  m_get_voltage_dc_commands(),
  m_get_voltage_ac_commands(),
  m_nplc_resistance_2x_index(0),
  m_nplc_resistance_4x_index(0),
  m_get_resistance_2x_commands(),
  m_get_resistance_4x_commands(),
  m_nplc_current_dc_index(0),
  m_nplc_current_ac_index(0),
  m_get_current_dc_commands(),
  m_get_current_ac_commands(),
  m_aperture_frequency_index(0),
  m_get_frequency_commands(),
  f_handle(IRS_NULL),
  f_create_error(irs_false),
  f_mode(ma_mode_start),
  f_macro_mode(macro_mode_stop),
  f_status(meas_status_success),
  f_command(mac_free),
  mp_value(IRS_NULL),
  mp_voltage(IRS_NULL),
  mp_resistance(IRS_NULL),
  mp_current(IRS_NULL),
  mp_frequency(IRS_NULL),
  f_abort_request(irs_false),
  f_read_pos(0),
  f_cur_mul_command(),
  f_mul_commands(IRS_NULL),
  f_mul_commands_index(0),
  //f_command_prev(mac_free),
  f_value(IRS_NULL),
  f_get_parametr_needed(irs_false),
  f_oper_time(0),
  f_oper_to(0),
  f_acal_time(0),
  f_acal_to(0),
  m_current_volt_dc_meas(true)
{
  init_to_cnt();
  mxifa_init();

  f_oper_time = TIME_TO_CNT(1, 1);
  f_acal_time = TIME_TO_CNT(20*60, 1);

  // Открытие канала MXIFA_MULTIMETER
  if (mxifa_get_channel_type_ex(channel) == mxifa_ei_win32_ni_usb_gpib) {
    mxifa_win32_ni_usb_gpib_cfg config;
    config.address = address;
    f_handle = mxifa_open_ex(channel, &config, irs_false);
  } else {
    f_handle = mxifa_open(channel, irs_false);
  }

  // Команды при инициализации
  // Очистка регистров
  f_init_commands.push_back("*CLS");
  // Програмный запуск
  f_init_commands.push_back("TRIGger:SOURce IMMediate");

  // Команды чтения значения измеряемой величины, при произвольных настройках
  m_get_value_commands.push_back("TRIGger:SOURce IMMediate");
  m_get_value_commands.push_back("READ?");

  // Команды при чтении напряжения
  // Настраиваемся на измерение напряжения
  // m_get_voltage_commands.push_back("MEAS:VOLT:DC?");
  m_get_voltage_dc_commands.push_back("CONFigure:VOLTage:DC");
  m_get_voltage_dc_commands.push_back("TRIGger:SOURce IMMediate");
  // Установка разрешения
  //m_get_voltage_dc_commands.push_back("VOLTage:DC:RESolution MIN");
  // Автовыбор пределов
  m_get_voltage_dc_commands.push_back("VOLTage:DC:RANGe:AUTO ON");
  m_nplc_voltage_dc_index = m_get_voltage_dc_commands.size();
  // Время интегрирования в количествах циклов сети питания
  m_get_voltage_dc_commands.push_back("VOLT:DC:NPLCycles 10");
  // запускаем измерение
  m_get_voltage_dc_commands.push_back("READ?");

  m_get_voltage_ac_commands.push_back("CONFigure:VOLTage:AC");
  m_get_voltage_ac_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_get_voltage_ac_commands.push_back("VOLTage:AC:RANGe:AUTO ON");
  m_band_width_voltage_ac_index = m_get_voltage_ac_commands.size();
  m_get_voltage_ac_commands.push_back("DETector:BANDwidth 20");
  // запускаем измерение
  m_get_voltage_ac_commands.push_back("READ?");

  // Команды при чтении сопротивления по 2-х проводной линии
  m_get_resistance_2x_commands.push_back("CONFigure:RESistance");
  m_get_resistance_2x_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_get_resistance_2x_commands.push_back("RESistance:RANGe:AUTO ON");
  m_nplc_resistance_2x_index = m_get_resistance_2x_commands.size();
  // Время интегрирования в количествах циклов сети питания
  m_get_resistance_2x_commands.push_back("RESistance:NPLCycles 10");
  // запускаем измерение
  m_get_resistance_2x_commands.push_back("READ?");

  // Команды при чтении сопротивления по 4-х проводной линии
  m_get_resistance_4x_commands.push_back("CONFigure:FRESistance");
  m_get_resistance_4x_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_get_resistance_4x_commands.push_back("FRESistance:RANGe:AUTO ON");
  m_nplc_resistance_4x_index = m_get_resistance_4x_commands.size();
  // Время интегрирования в количествах циклов сети питания
  m_get_resistance_4x_commands.push_back("FRESistance:NPLCycles 10");
  // запускаем измерение
  m_get_resistance_4x_commands.push_back("READ?");

  // Команды при чтении постоянного тока
  m_get_current_dc_commands.push_back("CONFigure:Current:DC");
  m_get_current_dc_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_get_current_dc_commands.push_back("Current:DC:RANGe:AUTO ON");
  m_nplc_current_dc_index = m_get_current_dc_commands.size();
  // Время интегрирования в количествах циклов сети питания
  m_get_current_dc_commands.push_back("Current:DC:NPLCycles 10");
  // запускаем измерение
  m_get_current_dc_commands.push_back("READ?");

  // Команды при чтении переменного тока
  m_get_current_ac_commands.push_back("CONFigure:Current:AC");
  m_get_current_ac_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_get_current_ac_commands.push_back("Current:AC:RANGe:AUTO ON");
  m_nplc_current_ac_index = m_get_current_ac_commands.size();
  m_get_current_ac_commands.push_back("DETector:BANDwidth 20");
  // запускаем измерение
  m_get_current_ac_commands.push_back("READ?");

  // Команды при чтении частоты
  m_get_frequency_commands.push_back("CONFigure:FREQuency");
  m_get_frequency_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_get_frequency_commands.push_back("FREQuency:VOLT:RANGe:AUTO ON");
  m_aperture_frequency_index = m_get_frequency_commands.size();
  m_get_frequency_commands.push_back("FREQuency:APERture 0.1");
  // запускаем измерение
  m_get_frequency_commands.push_back("READ?");

  {
    if (m_mul_mode_type == mul_mode_type_active) {
      // Запись команд инициализации
      #define write_t TIME_TO_CNT(2, 1)
      counter_t write_to = 0;
      irs_bool write_wait = irs_false;
      set_to_cnt(write_to, write_t);
      index_t ic_index = 0;
      for (;;) {
        mxifa_tick();
        if (write_wait) {
          if (mxifa_write_end(f_handle, irs_false)) {
            write_wait = irs_false;
            ic_index++;
            if (ic_index >= (index_t)f_init_commands.size()) break;
          } else if (test_to_cnt(f_oper_to)) {
            mxifa_write_end(f_handle, irs_true);
            write_wait = irs_false;
          } else if (test_to_cnt(write_to)) {
            mxifa_write_end(f_handle, irs_true);
            goto _error;
          }
        } else {
          irs_u8 *icommand = (irs_u8 *)(f_init_commands[ic_index].c_str());
          irs_u32 len = strlen((char *)icommand);
          mxifa_write_begin(f_handle, IRS_NULL, icommand, len);
          set_to_cnt(f_oper_to, f_oper_time);
          write_wait = irs_true;
        }
      }
    }
  }

  // Очистка буфера приема
  memset(f_read_buf, 0, ma_read_buf_size);
  return;

  _error:
  f_create_error = irs_true;
}
// Деструктор
irs::v7_78_1_t::~v7_78_1_t()
{
  if (f_create_error) return;
  mxifa_close_begin(f_handle);
  mxifa_close_end(f_handle, irs_true);
  mxifa_deinit();
  deinit_to_cnt();
}
// Установить положителный фронт запуска
void irs::v7_78_1_t::set_positive()
{}
// Установить отрицательный фронт канала
void irs::v7_78_1_t::set_negative()
{}
// Чтение значения при текущем типа измерения
void irs::v7_78_1_t::get_value(double *ap_value)
{
  if (f_create_error) return;
  mp_value = ap_value;
  f_command = mac_get_value;
  f_status = meas_status_busy;
}
// Чтение напряжения
void irs::v7_78_1_t::get_voltage(double *voltage)
{
  if (f_create_error) return;
  mp_voltage = voltage;
  f_command = mac_get_voltage;
  f_status = meas_status_busy;
}
// Чтение усредненного сдвира фаз
void irs::v7_78_1_t::get_phase_average(double * /*phase_average*/)
{
}
// Чтение фазового сдвига
void irs::v7_78_1_t::get_phase(double * /*phase*/)
{
}
// Чтение временного интервала
void irs::v7_78_1_t::get_time_interval(double * /*time_interval*/)
{
}
// Чтение усредненного временного интервала
void irs::v7_78_1_t::get_time_interval_average(double * /*ap_time_interval*/)
{
}
// Чтения силы тока
void irs::v7_78_1_t::get_current(double *current)
{
  if (f_create_error) return;
  mp_current = current;
  f_command = mac_get_current;
  f_status = meas_status_busy;
}
// Чтение сопротивления по двухпроводной схеме
void irs::v7_78_1_t::get_resistance2x(double *resistance)
{
  if (f_create_error) return;
  mp_resistance = resistance;
  f_command = mac_get_resistance;
  m_res_meas_type = RES_MEAS_2x;
  f_status = meas_status_busy;
}
// Чтение сопротивления по четырехпроводной схеме
void irs::v7_78_1_t::get_resistance4x(double *resistance)
{
  if (f_create_error) return;
  mp_resistance = resistance;
  f_command = mac_get_resistance;
  m_res_meas_type = RES_MEAS_4x;
  f_status = meas_status_busy;
}
// Чтение частоты
void irs::v7_78_1_t::get_frequency(double *frequency)
{
  if (f_create_error) return;
  mp_frequency = frequency;
  f_command = mac_get_frequency;
  f_status = meas_status_busy;
}
// Запуск автокалибровки мультиметра
void irs::v7_78_1_t::auto_calibration()
{}
// Чтение статуса текущей операции
meas_status_t irs::v7_78_1_t::status()
{
  if (f_create_error) return meas_status_busy;
  return f_status;
}
// Прерывание текущей операции
void irs::v7_78_1_t::abort()
{
  if (f_create_error) return;
  f_abort_request = irs_true;
}
// Элементарное действие
void irs::v7_78_1_t::tick()
{
  if (f_create_error) return;
  mxifa_tick();
  switch (f_mode) {
    case ma_mode_start: {
      f_abort_request = irs_false;
      switch (f_command) {
        case mac_get_value: {
          f_get_parametr_needed = irs_false;
          f_macro_mode = macro_mode_get_value;
          f_mode = ma_mode_macro;
        } break;
        case mac_get_voltage: {
          f_get_parametr_needed = irs_false;
          f_macro_mode = macro_mode_get_voltage;
          f_mode = ma_mode_macro;
        } break;
        case mac_get_resistance: {
          f_get_parametr_needed = irs_false;
          f_macro_mode = macro_mode_get_resistance;
          f_mode = ma_mode_macro;
        } break;
        case mac_get_current: {
          f_get_parametr_needed = irs_false;
          f_macro_mode = macro_mode_get_current;
          f_mode = ma_mode_macro;
        } break;
        case mac_get_frequency: {
          f_get_parametr_needed = irs_false;
          f_macro_mode = macro_mode_get_frequency;
          f_mode = ma_mode_macro;
        } break;
        case mac_auto_calibration: {
          f_macro_mode = macro_mode_stop;
          f_mode = ma_mode_auto_calibration;
        } break;
        case mac_free: {
        } break;
      }
    } break;
    case ma_mode_macro: {
      if (f_abort_request) {
        f_abort_request = irs_false;
        f_macro_mode = macro_mode_stop;
      }
      switch (f_macro_mode) {
        case macro_mode_get_value:{
          if (f_get_parametr_needed) {
            f_value = mp_value;
            f_macro_mode = macro_mode_stop;
            f_mode = ma_mode_get_value;
          } else {
            f_get_parametr_needed = irs_true;
            f_mul_commands = &m_get_value_commands;
            f_mul_commands_index = 0;
            f_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_voltage: {
          if (f_get_parametr_needed) {
            f_value = mp_voltage;
            f_macro_mode = macro_mode_stop;
            f_mode = ma_mode_get_value;
          } else {
            f_get_parametr_needed = irs_true;
            if(m_meas_type == DC_MEAS)
              f_mul_commands = &m_get_voltage_dc_commands;
            else
              f_mul_commands = &m_get_voltage_ac_commands;
            f_mul_commands_index = 0;
            //f_command_prev = f_command;
            f_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_resistance: {
          if (f_get_parametr_needed) {
            f_value = mp_resistance;
            f_macro_mode = macro_mode_stop;
            f_mode = ma_mode_get_value;
          } else {
            f_get_parametr_needed = irs_true;
            if(m_res_meas_type == RES_MEAS_2x)
              f_mul_commands = &m_get_resistance_2x_commands;
            else
              f_mul_commands = &m_get_resistance_4x_commands;
            f_mul_commands_index = 0;
            //f_command_prev = f_command;
            f_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_current: {
          if (f_get_parametr_needed) {
            f_value = mp_current;
            f_macro_mode = macro_mode_stop;
            f_mode = ma_mode_get_value;
          } else {
            f_get_parametr_needed = irs_true;
            if(m_meas_type == DC_MEAS)
              f_mul_commands = &m_get_current_dc_commands;
            else
              f_mul_commands = &m_get_current_ac_commands;
            f_mul_commands_index = 0;
            //f_command_prev = f_command;
            f_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_frequency: {
          if (f_get_parametr_needed) {
            f_value = mp_frequency;
            f_macro_mode = macro_mode_stop;
            f_mode = ma_mode_get_value;
          } else {
            f_get_parametr_needed = irs_true;
            f_mul_commands = &m_get_frequency_commands;
            f_mul_commands_index = 0;
            //f_command_prev = f_command;
            f_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_stop: {
          f_command = mac_free;
          f_status = meas_status_success;
          f_mode = ma_mode_start;
        } break;
        default : {
        }
      }
    } break;
    case ma_mode_commands: {
      f_cur_mul_command = (*f_mul_commands)[f_mul_commands_index];
      irs_u8 *command = (irs_u8 *)f_cur_mul_command.c_str();
      irs_u32 len = strlen((char *)command);
      mxifa_write_begin(f_handle, IRS_NULL, command, len);
      f_mode = ma_mode_commands_wait;
      set_to_cnt(f_oper_to, f_oper_time);
    } break;
    case ma_mode_commands_wait: {
      if (mxifa_write_end(f_handle, irs_false)) {
        f_mul_commands_index++;
        if (f_mul_commands_index >= (index_t)f_mul_commands->size()) {
          f_mode = ma_mode_macro;
        } else {
          f_mode = ma_mode_commands;
        }
      } else if (test_to_cnt(f_oper_to)) {
        mxifa_write_end(f_handle, irs_true);
        f_mode = ma_mode_commands;
      } else if (f_abort_request) {
        mxifa_write_end(f_handle, irs_true);
        f_mode = ma_mode_macro;
      }
    } break;
    case ma_mode_get_value: {

      if (f_abort_request) {
        f_mode = ma_mode_macro;
      } else {
        irs_u8 *buf = f_read_buf + f_read_pos;
        mxifa_sz_t size = (ma_read_buf_size - 1) - f_read_pos;
        if ((size > 0) && (size < ma_read_buf_size)) {
          mxifa_sz_t read_count =
            mxifa_fast_read(f_handle, IRS_NULL, buf, size);

          #ifndef NOP
          if (read_count) {
            int i = 0;
            i++;
          }
          #endif //NOP

          buf[read_count] = 0;
          char end_chars[] = {0x0A, 0x00};
          size_t end_chars_size = strlen(end_chars);
          char *end_number = strstr((char *)f_read_buf, end_chars);
          if (end_number) {
            *end_number = 0;
            char* p_number_in_cstr = reinterpret_cast<char*>(f_read_buf);
            if (irs::cstr_to_number_classic(p_number_in_cstr, *f_value)) {
              f_mode = ma_mode_macro;
            } else {
              *f_value = 0;
            }
            #ifdef NOP
            char *end_ptr = IRS_NULL;
            double val = strtod((char *)f_read_buf, &end_ptr);
            if (end_ptr == end_number) {
              *f_value = val;
              f_mode = ma_mode_macro;
            }
            #endif // NOP
            irs_u8 *read_bytes_end_ptr = buf + read_count;
            irs_u8 *next_str_ptr = (irs_u8 *)end_number + end_chars_size;
            mxifa_sz_t rest_bytes = read_bytes_end_ptr - next_str_ptr;
            memmove((void *)f_read_buf, (void *)next_str_ptr, rest_bytes);
            f_read_pos = rest_bytes;
          } else {
            f_read_pos += read_count;
          }
        } else {
          f_read_buf[0] = f_read_buf[ma_read_buf_size - 1];
          f_read_pos = 1;
        }
      }
    } break;

    case ma_mode_auto_calibration: {
      irs_u8 *command = (irs_u8 *)"ACAL ALL";
      irs_u32 len = strlen((char *)command);
      mxifa_write_begin(f_handle, IRS_NULL, command, len);
      set_to_cnt(f_acal_to, f_acal_time);
      f_mode = ma_mode_auto_calibration_wait;
    } break;
    case ma_mode_auto_calibration_wait: {
      if (test_to_cnt(f_acal_to)) {
        if (mxifa_write_end(f_handle, irs_false))
          f_mode = ma_mode_macro;
      } else if (f_abort_request) {
        mxifa_write_end(f_handle, irs_true);
        f_mode = ma_mode_macro;
      }
    } break;
  }
}
// Установка времени интегрирования в периодах частоты сети (20 мс)
void irs::v7_78_1_t::set_nplc(double nplc)
{
  if (nplc <= 0.02)
    nplc = 0.02;
  else if (nplc <= 0.2)
    nplc = 0.2;
  else if (nplc <= 1)
    nplc = 1;
  else if (nplc <= 10)
    nplc = 10;
  else
    nplc = 100;
  irs::string nplc_str = nplc;
  irs::string nplc_volt_dc_str =
    static_cast<irs::string>("VOLTage:DC:NPLCycles " +  nplc_str);
  irs::string nplc_resistance_2x_str =
    static_cast<irs::string>("RESistance:NPLCycles " + nplc_str);
  irs::string nplc_resistance_4x_str =
    static_cast<irs::string>("FRESistance:NPLCycles " + nplc_str);
  m_get_voltage_dc_commands[m_nplc_voltage_dc_index] = nplc_volt_dc_str;
  m_get_resistance_2x_commands[m_nplc_resistance_2x_index] =
    nplc_resistance_2x_str;
  m_get_resistance_4x_commands[m_nplc_resistance_4x_index] =
    nplc_resistance_4x_str;
}
// Установка времени интегрирования в c
void irs::v7_78_1_t::set_aperture(double aperture)
{
  set_time_interval_meas(aperture);
  if (aperture <= 0.01)
    aperture = 0.01;
  else if (aperture <= 0.1)
    aperture = 0.1;
  else
    aperture = 1;
  irs::string aperture_str = aperture;
  irs::string aperture_frequency_str =
    static_cast<irs::string>("FREQuency:APERture " + aperture_str);
  m_get_frequency_commands[m_aperture_frequency_index] =
    aperture_frequency_str;
}
// Установка полосы фильтра
void irs::v7_78_1_t::set_bandwidth(double bandwidth)
{
  if (bandwidth <= 3)
    bandwidth = 3;
  else if (bandwidth <= 20)
    bandwidth = 20;
  else
    bandwidth = 200;
  irs::string bandwidth_str = bandwidth;
  irs::string bandwidth_volt_ac_str =
    static_cast<irs::string>("DETector:BANDwidth " + bandwidth_str);
  m_get_voltage_ac_commands[m_band_width_voltage_ac_index] =
    bandwidth_volt_ac_str;
}
void irs::v7_78_1_t::set_input_impedance(double /*impedance*/)
{
}
void irs::v7_78_1_t::set_start_level(double /*level*/)
{
}
void irs::v7_78_1_t::set_range(type_meas_t /*a_type_meas*/, double /*a_range*/)
{
}
void irs::v7_78_1_t::set_range_auto()
{
}
 // Установка временного интервала измерения
void irs::v7_78_1_t::set_time_interval_meas(const double a_time_interval_meas)
{
  double nplc = 0.0;
  double bandwidth = 0.0;
  if (a_time_interval_meas != 0){
    const double coef_time_interval_to_nplc = 50;
    const double coef_time_interval_to_bandwidth = 1;
    nplc = a_time_interval_meas/coef_time_interval_to_nplc;
    bandwidth = coef_time_interval_to_bandwidth/a_time_interval_meas;
  }else{
    nplc = 0.0;
    bandwidth = 0.0;
  }
  set_nplc(nplc);
  set_bandwidth(bandwidth);
}

// Класс для работы с мультиметром agilent 34420a
irs::agilent_34420a_t::agilent_34420a_t(
  hardflow_t* ap_hardflow,
  multimeter_mode_type_t a_mul_mode_type
):
  m_meas_type(DC_MEAS),
  m_res_meas_type(RES_MEAS_2x),
  m_mul_mode_type(a_mul_mode_type),
  m_init_commands(),
  m_range_voltage_dc("SENSe1:VOLTage:DC:RANGe"),
  m_range_voltage_dc_index(0),
  m_nplc_voltage_dc_index(0),
  m_get_value_commands(0),
  m_get_voltage_dc_commands(),
  m_range_resistance_2x("RESistance:RANGe"),
  m_range_resistance_4x("FRESistance:RANGe"),
  m_range_resistance_2x_index(0),
  m_range_resistance_4x_index(0),
  m_nplc_resistance_2x_index(0),
  m_nplc_resistance_4x_index(0),
  m_get_resistance_2x_commands(),
  m_get_resistance_4x_commands(),
  m_set_params_commands(),
  mp_hardflow(ap_hardflow),
  m_fixed_flow(mp_hardflow),
  m_create_error(false),
  m_mode(ma_mode_start),
  m_macro_mode(macro_mode_stop),
  m_status(meas_status_success),
  m_command(mac_free),
  mp_value(IRS_NULL),
  m_abort_request(irs_false),
  m_read_pos(0),
  m_cur_mul_command(),
  mp_mul_commands(IRS_NULL),
  m_mul_commands_index(0),
  //mp_value(IRS_NULL),
  m_get_parametr_needed(irs_false),
  m_oper_time(0),
  m_oper_to(0),
  m_acal_time(0),
  m_acal_to(0),
  m_init_timer(irs::make_cnt_s(1)),
  m_init_mode(im_start),
  m_ic_index(0),
  m_current_volt_dc_meas(true)
{
  IRS_LIB_ERROR_IF(!mp_hardflow, ec_standard, "Недопустимо передавать нулевой "
    "указатель в качестве hardflow_t*");
  irs::string_t read_timeout_str = read_timeout_s;
  mp_hardflow->set_param(irst("read_timeout"), read_timeout_str);
  m_fixed_flow.read_timeout(make_cnt_s(
    read_timeout_s + fixed_flow_read_timeout_delta_s));

  init_to_cnt();
  //mxifa_init();

  m_oper_time = TIME_TO_CNT(1, 1);
  m_acal_time = TIME_TO_CNT(20*60, 1);

  // Команды при инициализации
  // Очистка регистров
  m_init_commands.push_back("*RST");
  // Програмный запуск
  m_init_commands.push_back("TRIGger:SOURce IMMediate");

  // Команды чтения значения измеряемой величины, при произвольных настройках
  m_get_value_commands.push_back("TRIGger:SOURce IMMediate");
  m_get_value_commands.push_back("READ?");

  // Команды при чтении напряжения
  // Настраиваемся на измерение напряжения
  m_get_voltage_dc_commands.push_back("CONFigure:VOLTage:DC");
  m_get_voltage_dc_commands.push_back("TRIGger:SOURce IMMediate");

  // Автовыбор пределов
  m_range_voltage_dc_index = m_get_voltage_dc_commands.size();
  m_get_voltage_dc_commands.push_back(m_range_voltage_dc + ":AUTO ON");
  m_nplc_voltage_dc_index = m_get_voltage_dc_commands.size();
  // Время интегрирования в количествах циклов сети питания
  m_get_voltage_dc_commands.push_back("SENSe1:VOLT:DC:NPLCycles 10");
  // запускаем измерение
  m_get_voltage_dc_commands.push_back("READ?");

  // Команды при чтении сопротивления по 2-х проводной линии
  m_get_resistance_2x_commands.push_back("CONFigure:RESistance");
  m_get_resistance_2x_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_range_resistance_2x_index = m_get_resistance_2x_commands.size();
  m_get_resistance_2x_commands.push_back(m_range_resistance_2x + ":AUTO ON");
  m_nplc_resistance_2x_index = m_get_resistance_2x_commands.size();
  // Время интегрирования в количествах циклов сети питания
  m_get_resistance_2x_commands.push_back("RESistance:NPLCycles 10");
  // запускаем измерение
  m_get_resistance_2x_commands.push_back("READ?");

  // Команды при чтении сопротивления по 4-х проводной линии
  m_get_resistance_4x_commands.push_back("CONFigure:FRESistance");
  m_get_resistance_4x_commands.push_back("TRIGger:SOURce IMMediate");
  // Автовыбор пределов
  m_range_resistance_4x_index = m_get_resistance_4x_commands.size();
  m_get_resistance_4x_commands.push_back(m_range_resistance_4x + ":AUTO ON");
  m_nplc_resistance_4x_index = m_get_resistance_4x_commands.size();
  // Время интегрирования в количествах циклов сети питания
  m_get_resistance_4x_commands.push_back("FRESistance:NPLCycles 10");
  // запускаем измерение
  m_get_resistance_4x_commands.push_back("READ?");

  // Очистка буфера приема
  memset(m_read_buf, 0, ma_read_buf_size);
}

// Деструктор
irs::agilent_34420a_t::~agilent_34420a_t()
{
  deinit_to_cnt();
}

// Отправка команд инициализации в мультиметр
void irs::agilent_34420a_t::initialize_tick()
{
  switch (m_mul_mode_type) {
    case mul_mode_type_active: {
      // Запись команд инициализации
      switch (m_init_mode) {
        case im_start: {
          if (m_create_error) {
            m_init_timer.start();
            m_ic_index = 0;
            m_init_mode = im_write_command;
          }
        } break;
        case im_write_command: {
          irs::string& icomm = m_init_commands[m_ic_index];
          size_t icomm_size = icomm.size();
          const irs_u8* icomm_u8 =
            reinterpret_cast<const irs_u8*>(icomm.c_str());
          m_fixed_flow.write(mp_hardflow->channel_next(), icomm_u8, icomm_size);
          m_fixed_flow.write_timeout(m_oper_time);
          m_init_mode = im_next_command;
        } break;
        case im_next_command: {
          switch (m_fixed_flow.write_status()) {
            case irs::hardflow::fixed_flow_t::status_success: {
              m_ic_index++;
              if (m_ic_index < static_cast<index_t>(m_init_commands.size())) {
                m_init_mode = im_write_command;
              } else {
                m_create_error = false;
                m_init_mode = im_start;
              }
            } break;
            case irs::hardflow::fixed_flow_t::status_error: {
              m_init_mode = im_write_command;
            } break;
            case irs::hardflow::fixed_flow_t::status_wait: {
              if (m_init_timer.check()) {
                m_init_mode = im_start;
              }
            } break;
            default : {
              IRS_LIB_ASSERT_MSG("Неучтенный статус");
            }
          }
        } break;
        default: {
          IRS_LIB_ASSERT_MSG("m_init_mode не должен быть default");
        } break;
      } //switch (m_init_mode)
    } break;
    case mul_mode_type_passive: {
      m_create_error = false;
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("m_mul_mode_type не должен быть default");
    } break;
  } //switch (m_mul_mode_type)
}

// Установить положителный фронт запуска
void irs::agilent_34420a_t::set_positive()
{}
// Установить отрицательный фронт канала
void irs::agilent_34420a_t::set_negative()
{}
// Чтение значения при текущем типа измерения
void irs::agilent_34420a_t::get_value(double *ap_value)
{
  if (m_create_error) return;
  mp_value = ap_value;
  m_command = mac_get_value;
  m_status = meas_status_busy;
}
// Чтение напряжения
void irs::agilent_34420a_t::get_voltage(double *voltage)
{
  if (m_create_error) return;
  mp_value = voltage;
  m_command = mac_get_voltage;
  m_status = meas_status_busy;
}
// Чтение усредненного сдвира фаз
void irs::agilent_34420a_t::get_phase_average(double * /*phase_average*/)
{
}
// Чтение фазового сдвига
void irs::agilent_34420a_t::get_phase(double * /*phase*/)
{
}
// Чтение временного интервала
void irs::agilent_34420a_t::get_time_interval(double * /*time_interval*/)
{
}
// Чтение усредненного временного интервала
void irs::agilent_34420a_t::get_time_interval_average(double * /*ap_time_interval*/)
{
}
// Чтения силы тока
void irs::agilent_34420a_t::get_current(double *current)
{
}
// Чтение сопротивления по двухпроводной схеме
void irs::agilent_34420a_t::get_resistance2x(double *resistance)
{
  if (m_create_error) return;
  mp_value = resistance;
  m_command = mac_get_resistance;
  m_res_meas_type = RES_MEAS_2x;
  m_status = meas_status_busy;
}
// Чтение сопротивления по четырехпроводной схеме
void irs::agilent_34420a_t::get_resistance4x(double *resistance)
{
  if (m_create_error) return;
  mp_value = resistance;
  m_command = mac_get_resistance;
  m_res_meas_type = RES_MEAS_4x;
  m_status = meas_status_busy;
}
// Чтение частоты
void irs::agilent_34420a_t::get_frequency(double *frequency)
{
}
// Запуск автокалибровки мультиметра
void irs::agilent_34420a_t::auto_calibration()
{}
// Чтение статуса текущей операции
meas_status_t irs::agilent_34420a_t::status()
{
  if (m_create_error) return meas_status_busy;
  return m_status;
}
// Прерывание текущей операции
void irs::agilent_34420a_t::abort()
{
  if (m_create_error) return;
  m_abort_request = irs_true;
}
// Элементарное действие
void irs::agilent_34420a_t::tick()
{
  IRS_LIB_ASSERT(mp_hardflow);
  mp_hardflow->tick();
  m_fixed_flow.tick();
  initialize_tick();
  if (m_create_error) return;
  switch (m_mode) {
    case ma_mode_start: {
      m_abort_request = false;
      switch (m_command) {
        case mac_get_value: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_get_value;
          m_mode = ma_mode_macro;
        } break;
        case mac_get_voltage: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_get_voltage;
          m_mode = ma_mode_macro;
        } break;
        case mac_get_resistance: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_get_resistance;
          m_mode = ma_mode_macro;
        } break;
        case mac_set_params: {
          m_get_parametr_needed = false;
          m_macro_mode = macro_mode_set_params;
          m_mode = ma_mode_macro;
        } break;
        case mac_free: {
        } break;
        default : {
          IRS_LIB_ASSERT("Недопустимое значение");
        }
      }
    } break;
    case ma_mode_macro: {
      if (m_abort_request) {
        m_abort_request = false;
        m_macro_mode = macro_mode_stop;
      }
      switch (m_macro_mode) {
        case macro_mode_get_value:{
          if (m_get_parametr_needed) {
            m_macro_mode = macro_mode_stop;
            m_mode = ma_mode_get_value;
          } else {
            m_get_parametr_needed = true;
            mp_mul_commands = &m_get_value_commands;
            m_mul_commands_index = 0;
            m_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_voltage: {
          if (m_get_parametr_needed) {
            m_macro_mode = macro_mode_stop;
            m_mode = ma_mode_get_value;
          } else {
            m_get_parametr_needed = true;
            mp_mul_commands = &m_get_voltage_dc_commands;
            m_mul_commands_index = 0;
            m_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_get_resistance: {
          if (m_get_parametr_needed) {
            m_macro_mode = macro_mode_stop;
            m_mode = ma_mode_get_value;
          } else {
            m_get_parametr_needed = true;
            if(m_res_meas_type == RES_MEAS_2x)
              mp_mul_commands = &m_get_resistance_2x_commands;
            else
              mp_mul_commands = &m_get_resistance_4x_commands;
            m_mul_commands_index = 0;
            m_mode = ma_mode_commands;
          }
        } break;
        case macro_mode_set_params: {
          mp_mul_commands = &m_set_params_commands;
          m_mul_commands_index = 0;
          m_mode = ma_mode_commands;
          m_macro_mode = macro_mode_stop;
        } break;
        case macro_mode_stop: {
          m_command = mac_free;
          m_status = meas_status_success;
          m_mode = ma_mode_start;
        } break;
        default : {
          IRS_LIB_ASSERT("Недопустимое значение");
        }
      }
    } break;
    case ma_mode_commands: {
      m_cur_mul_command = (*mp_mul_commands)[m_mul_commands_index] + "\n";
      irs_u8 *command = (irs_u8 *)m_cur_mul_command.c_str();
      irs_u32 len = strlen((char *)command);
      m_fixed_flow.write(mp_hardflow->channel_next(), command, len);
      m_fixed_flow.write_timeout(m_oper_time);
      m_mode = ma_mode_commands_wait;
    } break;
    case ma_mode_commands_wait: {
      if (m_abort_request) {
        m_fixed_flow.write_abort();
        m_mode = ma_mode_macro;
      } else {
        switch (m_fixed_flow.write_status()) {
          case irs::hardflow::fixed_flow_t::status_success: {
            m_mul_commands_index++;
            if (m_mul_commands_index >= (index_t)mp_mul_commands->size()) {
              m_mode = ma_mode_macro;
            } else {
              m_mode = ma_mode_commands;
            }
          } break;
          case irs::hardflow::fixed_flow_t::status_error: {
            m_mode = ma_mode_commands;
          } break;
          default : {
            // Ожидаем окончания операции
          }
        }
      }
    } break;
    case ma_mode_get_value: {
      m_fixed_flow.read(mp_hardflow->channel_next(),
        m_read_buf, sample_size);
      m_mode = ma_mode_get_value_wait;
    } break;
    case ma_mode_get_value_wait: {
      if (!m_abort_request) {
        switch (m_fixed_flow.read_status()) {
          case irs::hardflow::fixed_flow_t::status_success: {
            char* p_number_in_cstr = reinterpret_cast<char*>(m_read_buf);
            if (!irs::cstr_to_number_classic(p_number_in_cstr, *mp_value)) {
              *mp_value = 0;
            }
            m_mode = ma_mode_macro;
          } break;
          case irs::hardflow::fixed_flow_t::status_error: {
            m_mode = ma_mode_macro;
          } break;
          default : {
            // Ожидаем выполнения
          }
        }
      } else {
        m_fixed_flow.read_abort();
        m_mode = ma_mode_macro;
      }
    } break;
    default : {
      IRS_LIB_ASSERT("Недопустимое значение");
    }
  }
}
// Установка времени интегрирования в периодах частоты сети (20 мс)
void irs::agilent_34420a_t::set_nplc(double nplc)
{
  if (nplc <= 0.02) {
    nplc = 0.02;
  } else if (nplc <= 0.2) {
    nplc = 0.2;
  } else if (nplc <= 1) {
    nplc = 1;
  } else if (nplc <= 10) {
    nplc = 10;
  } else if (nplc <= 10) {
    nplc = 100;
  } else {
    nplc = 200;
  }
  irs::string nplc_str = nplc;
  irs::string nplc_volt_dc_str =
    static_cast<irs::string>("VOLTage:DC:NPLCycles " +  nplc_str);
  irs::string nplc_resistance_2x_str =
    static_cast<irs::string>("RESistance:NPLCycles " + nplc_str);
  irs::string nplc_resistance_4x_str =
    static_cast<irs::string>("FRESistance:NPLCycles " + nplc_str);
  m_get_voltage_dc_commands[m_nplc_voltage_dc_index] = nplc_volt_dc_str;
  m_get_resistance_2x_commands[m_nplc_resistance_2x_index] =
    nplc_resistance_2x_str;
  m_get_resistance_4x_commands[m_nplc_resistance_4x_index] =
    nplc_resistance_4x_str;
}
// Установка времени интегрирования в c
void irs::agilent_34420a_t::set_aperture(double aperture)
{
}
// Установка полосы фильтра
void irs::agilent_34420a_t::set_bandwidth(double bandwidth)
{
}
void irs::agilent_34420a_t::set_input_impedance(double /*impedance*/)
{
}
void irs::agilent_34420a_t::set_start_level(double /*level*/)
{
}
void irs::agilent_34420a_t::set_range(type_meas_t a_type_meas,
  double a_range)
{
  m_set_params_commands.clear();
  irs::string range_str;
  irs::num_to_str_classic(a_range, &range_str);
  switch(a_type_meas) {
    case tm_volt_dc: {
      m_get_voltage_dc_commands[m_range_voltage_dc_index] =
        m_range_voltage_dc+ " " + range_str;
      m_set_params_commands.push_back(
        m_get_voltage_dc_commands[m_range_voltage_dc_index]);
    } break;
    case tm_resistance_2x: {
      m_get_resistance_2x_commands[m_range_resistance_2x_index] =
        m_range_resistance_2x+ " " + range_str;
      m_set_params_commands.push_back(
        m_get_resistance_2x_commands[m_range_resistance_2x_index]);
    } break;
    case tm_resistance_4x: {
      m_get_resistance_4x_commands[m_range_resistance_4x_index] =
        m_range_resistance_4x+ " " + range_str;
      m_set_params_commands.push_back(
        m_get_resistance_4x_commands[m_range_resistance_4x_index]);
    } break;
    default : {
      // Остальные типы в данном мультиметре не используются
    }
  }
  m_command = mac_set_params;
  m_status = meas_status_busy;
}
void irs::agilent_34420a_t::set_range_auto()
{
  m_get_voltage_dc_commands[m_range_voltage_dc_index] =
    m_range_voltage_dc + ":AUTO ON";
  m_get_resistance_2x_commands[m_range_resistance_2x_index] =
    m_range_resistance_2x + ":AUTO ON";
  m_get_resistance_4x_commands[m_range_resistance_4x_index] =
    m_range_resistance_4x + ":AUTO ON";
}
 // Установка временного интервала измерения
void irs::agilent_34420a_t::set_time_interval_meas(
  const double /*a_time_interval_meas*/)
{
}

// Класс для работы с частотомером электронно-счетным акип чз-85/3R
// Конструктор
irs::akip_ch3_85_3r_t::akip_ch3_85_3r_t(
  hardflow_t* ap_hardflow,
  multimeter_mode_type_t a_mul_mode_type):
  mp_error_trans(irs::error_trans()),
  mp_hardflow(ap_hardflow),
  m_mul_mode_type(a_mul_mode_type),
  m_time_between_big_cmd(make_cnt_ms(800)),
  m_time_between_small_cmd(make_cnt_ms(1)),
  m_time_interval(m_time_between_small_cmd),
  md_buf_cmds(),
  m_buf_cmd(),
  m_string_msgs(),
  m_send_command_stat(CS_SUCCESS),
  m_meas_stat(meas_status_success),
  mp_result(NULL),
  mp_last_commands(NULL),
  mv_init_commands(),
  mv_reset_multimetr_commands(),
  mv_multimetr_p_commands_setting(),
  mv_get_value_commands(),
  mv_get_phase_average_commands(),
  mv_get_phase_commands(),
  mv_get_time_interval_commands(),
  mv_get_time_interval_average_commands(),
  mv_get_frequency_commands(),
  mv_big_time_commands()
{
  // Команды, большие по времении выполнения
  mv_big_time_commands.push_back("*RST\n");
  mv_big_time_commands.push_back(":FORMat ASCii\n");

  // Команды при инициализации
  if (m_mul_mode_type == mul_mode_type_active) {
    mv_init_commands.push_back("*RST\n");     // временно
  }
  mv_init_commands.push_back(":FORMat ASCii\n");

  // Команды при перезагрузке мультиметра
  if (m_mul_mode_type == mul_mode_type_active) {
    mv_reset_multimetr_commands.push_back("*RST\n");
  }
  mv_reset_multimetr_commands.push_back(":FORMat ASCii\n");

  // Команды при чтении значения измеряемой величины,
  // при произвольном типе измерения
  mv_get_value_commands.push_back(":MEASure?\n");

  // Команды при чтении усредненной разности фаз
  //mv_get_phase_average_commands.push_back(":FREQuency:ARM 100mS");
  mv_get_phase_average_commands.push_back(":FUNCtion \"PHASe:AVERage 1,2\"\n");
  mv_get_phase_average_commands.push_back(":MEASure?\n");

  // Команды при чтении разности фаз
  mv_get_phase_commands.push_back(":FUNCtion \"PHASe 1,2\"\n");
  mv_get_phase_commands.push_back(":MEASure?\n");

  // Команды при чтении временного интервала
  mv_get_time_interval_commands.push_back(":FUNCtion \"TINTerval 1,2\"\n");
  mv_get_time_interval_commands.push_back(":MEASure?\n");

  // Команды при чтении усредненного временного интервала
  mv_get_time_interval_average_commands.push_back(
    ":FUNCtion \"TINTerval:AVERage 1,2\"\n");
  mv_get_time_interval_average_commands.push_back(":MEASure?\n");

  // Команды при чтении частоты
  mv_get_frequency_commands.push_back(":FUNCtion \"FREQuency 1\"\n");
  mv_get_frequency_commands.push_back(":MEASure?\n");

  // Команды установки времени счета
  ms_set_interval_command = ":FREQuency:ARM 100mS\n";

  // Команды установки входного сопротивления каналов
  ms_set_in_impedance_channel_1_command = ":INPut1:IMPedance 1M\n";
  ms_set_in_impedance_channel_2_command = ":INPut2:IMPedance 1M\n";

  // Команды установки вида связи входа канала
  ms_set_type_coupling_channel_1_command = ":INPut1:COUPling AC\n";
  ms_set_type_coupling_channel_2_command = ":INPut2:COUPling AC\n";

  // Команды установки типа фронта запуска канала
  ms_set_type_slope_channel_1_command = ":EVENt1:SLOPe POSitive\n";
  ms_set_type_slope_channel_2_command = ":EVENt2:SLOPe POSitive\n";

  // Команды установки фильтра канала
  ms_set_filter_channel_1_command = ":INPut1:FILTer 1\n";
  ms_set_filter_channel_2_command = ":INPut2:FILTer 1\n";

  // Установка уровня запуска канала
  ms_set_level_channel_1_command = ":EVENt1:LEVel 0\n";
  ms_set_level_channel_2_command = ":EVENt2:LEVel 0\n";

  // Указатели на команды настроек мультиметра
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_interval_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_in_impedance_channel_1_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_in_impedance_channel_2_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_type_slope_channel_1_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_type_slope_channel_2_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_filter_channel_1_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_filter_channel_2_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_level_channel_1_command);
  mv_multimetr_p_commands_setting.push_back(
    &ms_set_level_channel_2_command);

  connect(ap_hardflow);
}
// Деструктор
irs::akip_ch3_85_3r_t::~akip_ch3_85_3r_t()
{
}

void irs::akip_ch3_85_3r_t::connect(hardflow_t* ap_hardflow)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    mp_hardflow = ap_hardflow;
    if(mp_hardflow != NULL){
      //Запись команд инициализации
      irs_u32 size = mv_init_commands.size();
      for(irs_u32 i = 0; i < size; i++){
        md_buf_cmds.push_back(mv_init_commands[i]);
      }
    }
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::set_dc()
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    ms_set_type_coupling_channel_1_command = ":INPut1:COUPling DC\n";
    ms_set_type_coupling_channel_2_command = ":INPut2:COUPling DC\n";
    md_buf_cmds.push_back(ms_set_type_coupling_channel_1_command);
    md_buf_cmds.push_back(ms_set_type_coupling_channel_2_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::set_ac()
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    ms_set_type_coupling_channel_1_command = ":INPut1:COUPling AC\n";
    ms_set_type_coupling_channel_2_command = ":INPut2:COUPling AC\n";
    md_buf_cmds.push_back(ms_set_type_coupling_channel_1_command);
    md_buf_cmds.push_back(ms_set_type_coupling_channel_2_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
// Установить положителный фронт запуска
void irs::akip_ch3_85_3r_t::set_positive()
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    ms_set_type_coupling_channel_1_command = ":EVENt1:SLOPe POSitive\n";
    ms_set_type_coupling_channel_2_command = ":EVENt2:SLOPe POSitive\n";
    md_buf_cmds.push_back(ms_set_type_coupling_channel_1_command);
    md_buf_cmds.push_back(ms_set_type_coupling_channel_2_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
// Установить отрицательный фронт канала
void irs::akip_ch3_85_3r_t::set_negative()
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    ms_set_type_coupling_channel_1_command = ":EVENt1:SLOPe NEGative\n";
    ms_set_type_coupling_channel_2_command = ":EVENt2:SLOPe NEGative\n";
    md_buf_cmds.push_back(ms_set_type_coupling_channel_1_command);
    md_buf_cmds.push_back(ms_set_type_coupling_channel_2_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::get_value(double *ap_value)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    m_meas_stat = meas_status_busy;
    mp_result = ap_value;
    irs_u32 size = mv_get_value_commands.size();
    for(irs_u32 i = 0; i < size; i++){
      md_buf_cmds.push_back(mv_get_value_commands[i]);
    }
    mp_last_commands = &mv_get_value_commands;
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::get_voltage(double * /*ap_voltage*/)
{
}
// Чтения силы тока
void irs::akip_ch3_85_3r_t::get_current(double * /*ap_current*/)
{
}
// Чтение сопротивления по двухпроводной схеме
void irs::akip_ch3_85_3r_t::get_resistance2x(double * /*ap_resistance*/)
{
}
// Чтение сопротивления по четырехпроводной схеме
void irs::akip_ch3_85_3r_t::get_resistance4x(double * /*ap_resistance*/)
{
}
void irs::akip_ch3_85_3r_t::get_frequency(double *ap_frequency)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    m_meas_stat = meas_status_busy;
    mp_result = ap_frequency;
    irs_u32 size = mv_get_frequency_commands.size();
    for(irs_u32 i = 0; i < size; i++){
      md_buf_cmds.push_back(mv_get_frequency_commands[i]);
    }
    mp_last_commands = &mv_get_frequency_commands;
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::get_phase_average(double *ap_phase_average)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    m_meas_stat = meas_status_busy;
    mp_result = ap_phase_average;
    irs_u32 size = mv_get_phase_average_commands.size();
    for(irs_u32 i = 0; i < size; i++){
      md_buf_cmds.push_back(mv_get_phase_average_commands[i]);
    }
    mp_last_commands = &mv_get_phase_average_commands;
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::get_phase(double *ap_phase)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    m_meas_stat = meas_status_busy;
    mp_result = ap_phase;
    irs_u32 size = mv_get_phase_commands.size();
    for(irs_u32 i = 0; i < size; i++){
      md_buf_cmds.push_back(mv_get_phase_commands[i]);
    }
    mp_last_commands = &mv_get_phase_commands;
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::get_time_interval(double *ap_time_interval)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    m_meas_stat = meas_status_busy;
    mp_result = ap_time_interval;
    irs_u32 size = mv_get_time_interval_commands.size();
    for(irs_u32 i = 0; i < size; i++){
      md_buf_cmds.push_back(mv_get_time_interval_commands[i]);
    }
    mp_last_commands = &mv_get_time_interval_commands;
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::get_time_interval_average(double *ap_time_interval)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    m_meas_stat = meas_status_busy;
    mp_result = ap_time_interval;
    irs_u32 size = mv_get_time_interval_average_commands.size();
    for(irs_u32 i = 0; i < size; i++){
      md_buf_cmds.push_back(mv_get_time_interval_average_commands[i]);
    }
    mp_last_commands = &mv_get_time_interval_average_commands;
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::auto_calibration()
{}
meas_status_t irs::akip_ch3_85_3r_t::status()
{
  meas_status_t status = meas_status_success;
  if (m_send_command_stat == CS_BUSY || m_meas_stat == meas_status_busy) {
    status = meas_status_busy;
  } else if ((m_send_command_stat == CS_SUCCESS) &&
    (m_meas_stat == meas_status_success)) {
    status = meas_status_success;
  } else {
    status = meas_status_error;
  }
  return status;
}
void irs::akip_ch3_85_3r_t::abort()
{
  m_send_command_stat = CS_BUSY;
  m_meas_stat = meas_status_success;
  m_string_msgs.clear();
  irs_u32 size = 0;
  size = mv_reset_multimetr_commands.size();
  for(irs_u32 i = 0; i < size; i++){
    md_buf_cmds.push_back(mv_reset_multimetr_commands[i]);
  }
  size = mv_multimetr_p_commands_setting.size();
  for(irs_u32 i = 0; i < size; i++){
    irs::irs_string_t command = *(mv_multimetr_p_commands_setting[i]);
    md_buf_cmds.push_back(command);
  }
}
// Элементарное действие
void irs::akip_ch3_85_3r_t::tick()
{
  irs_uarc ch = mp_hardflow->channel_next();
  if(mp_hardflow != NULL) {
    static const irs_u32 size_buf_rd = 1024;
    bool big_cmd = false;
    if(m_time_interval.check()) {
      const irs_u32 size_buf_cmd = m_buf_cmd.size();
      if(size_buf_cmd > 0) {
        irs_u32 size_wr =
          mp_hardflow->write(ch, m_buf_cmd.ptr_data(), size_buf_cmd);
        m_buf_cmd.pop_front(size_wr);
      } else if(md_buf_cmds.size() > 0) {
        const char* p_command_str = md_buf_cmds[0].c_str();
        irs_u32 size_big_time_cmds = mv_big_time_commands.size();
        for(irs_u32 i = 0; i < size_big_time_cmds; i++){
          if(p_command_str == mv_big_time_commands[i]){
            big_cmd = true;
            break;
          }
        }
        irs_u32 byte_count = md_buf_cmds[0].size();

        m_buf_cmd.push(reinterpret_cast<const irs_u8*>(p_command_str),
          byte_count);
        md_buf_cmds.pop_front();
        irs_u32 size_wr =
          mp_hardflow->write(ch, m_buf_cmd.ptr_data(), m_buf_cmd.size());
        m_buf_cmd.pop_front(size_wr);

      } else {
        m_send_command_stat = CS_SUCCESS;
      }

      irs_u8 buf_rd[size_buf_rd];
      memset(buf_rd, 0, sizeof(buf_rd));

      irs_u32 size_rd = mp_hardflow->read(ch, buf_rd, size_buf_rd);
      buf_rd[size_rd] = irst('\0');
      m_string_msgs += irs::irs_string_t((char*)buf_rd);
      if(m_meas_stat == meas_status_busy){
        irs::irs_string_t message;
        msg_status_t msg_stat = m_string_msgs.get_message(message);
        if(msg_stat == MSG_SUCCESS){
          double value = 0.;
          bool fsuccess = false;
          //fsuccess = message.to_number(value);
          #ifdef IRS_FULL_STDCPPLIB_SUPPORT
          fsuccess = string_to_number(message, &value, locale::classic());
          #else // !IRS_FULL_STDCPPLIB_SUPPORT
          fsuccess = string_to_number(message, &value);
          #endif // !IRS_FULL_STDCPPLIB_SUPPORT
          if(fsuccess){
            *mp_result = value;
            m_meas_stat = meas_status_success;
          }else{
            m_meas_stat = meas_status_error;
          }
        }
      }
      if(big_cmd){
        m_time_interval.set(m_time_between_big_cmd);
      }else{
        m_time_interval.set(m_time_between_small_cmd);
      }
      m_time_interval.start();
    }
  }
}

// Установка времени интегрирования в c
void irs::akip_ch3_85_3r_t::set_aperture(double a_aperture)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    irs::irs_string_t aperture_str = "";
    if(a_aperture <= 10e-6)
      aperture_str = "10US";
    else if(a_aperture <= 100e-6)
      aperture_str = "100US";
    else if(a_aperture <= 1e-3)
      aperture_str = "1mS";
    else if(a_aperture <= 10e-3)
      aperture_str = "10mS";
    else if(a_aperture <= 100e-3)
      aperture_str = "100mS";
    else if(a_aperture <= 300e-3)
      aperture_str = "300mS";
    else if(a_aperture <= 1)
      aperture_str = "1S";
    else if(a_aperture <= 10)
      aperture_str = "10S";
    else if(a_aperture <= 100)
      aperture_str = "100S";
    else
      aperture_str = "1000S";
    ms_set_interval_command = ":FREQuency:ARM "+aperture_str+"\n";
    md_buf_cmds.push_back(ms_set_interval_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}

void irs::akip_ch3_85_3r_t::set_input_impedance(double a_impedance)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    irs::irs_string_t impedance_str = "";
    if(a_impedance <= 50)
      impedance_str = "50";
    else
      impedance_str = "1M";
    ms_set_in_impedance_channel_1_command =
      ":INPut1:IMPedance "+impedance_str+"\n";
    ms_set_in_impedance_channel_2_command =
      ":INPut2:IMPedance "+impedance_str+"\n";
    md_buf_cmds.push_back(ms_set_in_impedance_channel_1_command);
    md_buf_cmds.push_back(ms_set_in_impedance_channel_2_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}

void irs::akip_ch3_85_3r_t::set_start_level(double a_level)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    irs::irs_string_t level_str = a_level;
    ms_set_level_channel_1_command = ":EVENt1:LEVel "+level_str+"\n";
    ms_set_level_channel_2_command = ":EVENt2:LEVel "+level_str+"\n";
    md_buf_cmds.push_back(ms_set_level_channel_1_command);
    md_buf_cmds.push_back(ms_set_level_channel_2_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}
void irs::akip_ch3_85_3r_t::set_range(type_meas_t /*a_type_meas*/,
  double /*a_range*/)
{
}

void irs::akip_ch3_85_3r_t::set_range_auto()
{
}

void irs::akip_ch3_85_3r_t::set_nplc(double /*a_nplc*/)
{
}

// Установка полосы фильтра
void irs::akip_ch3_85_3r_t::set_bandwidth(double a_bandwidth)
{
  if(m_send_command_stat != CS_BUSY && m_meas_stat != meas_status_busy){
    m_send_command_stat = CS_BUSY;
    irs::irs_string_t bandwidth_str = "";
    if(a_bandwidth <= 100)
      bandwidth_str = "1";
    else
      bandwidth_str = "0";
    ms_set_filter_channel_1_command = ":INPut1:FILTer "+bandwidth_str+"\n";
    ms_set_filter_channel_2_command = ":INPut2:FILTer "+bandwidth_str+"\n";
    md_buf_cmds.push_back(ms_set_filter_channel_1_command);
    md_buf_cmds.push_back(ms_set_filter_channel_2_command);
  }else
    IRS_FATAL_ERROR("Запрещенное действие во время измерений.");
}

// Класс для работы с мультиметром National Instruments PXI-4071
irs::ni_pxi_4071_t::ni_pxi_4071_t(
  hardflow_t* ap_hardflow,
  const filter_settings_t& a_filter,
  counter_t a_update_time,
  multimeter_mode_type_t a_mul_mode_type
):
  m_mul_mode_type(a_mul_mode_type),
  mp_hardflow(ap_hardflow),
  m_modbus_client(mp_hardflow, irs::mxdata_ext_t::mode_refresh_auto,
    0, 0, 38, 24, a_update_time, 3, irs::make_cnt_s(2), 260, 1),
  m_eth_mul_data(),
  m_status(meas_status_success),
  mp_value(IRS_NULL),
  m_abort_request(false),
  m_mode(start_mode),
  m_filter(a_filter),
  m_window_size(0)/*,
  m_pause(irs::make_cnt_ms(500))*/
{
  m_eth_mul_data.connect(&m_modbus_client, 0);
  m_eth_mul_data.meas_status = meas_status_success;
  
  #ifdef NOP
  m_eth_mul_data.meas_mode = high_speed;
  if (m_mul_mode_type == mul_mode_type_active) {
    if (a_filter != zero_struct_t<filter_settings_t>::get()) {
      m_eth_mul_data.filter_type = m_filter.family;
      m_eth_mul_data.filter_order = static_cast<irs_u8>(m_filter.order);
      m_eth_mul_data.sampling_freq = 1/m_filter.sampling_time_s;
      m_eth_mul_data.low_cutoff_freq = m_filter.low_cutoff_freq_hz;
      m_eth_mul_data.passband_ripple = m_filter.passband_ripple_db;
      m_eth_mul_data.stopband_ripple = m_filter.stopband_ripple_db;
    } else {
     /* m_filter.family =
        static_cast<filter_family_t>(
        static_cast<irs_u8>(m_eth_mul_data.filter_type));
      m_filter.order = static_cast<irs_u8>(m_eth_mul_data.filter_order);
      m_filter.sampling_time_s = 1/m_eth_mul_data.sampling_freq;
      m_filter.low_cutoff_freq_hz = m_eth_mul_data.low_cutoff_freq;
      m_filter.passband_ripple_db = m_eth_mul_data.passband_ripple;
      m_filter.stopband_ripple_db = m_eth_mul_data.stopband_ripple; */
    }
  }
  //работает не правильно т.к в начале нет коннекта и все значения 0
  #endif // NOP
}

irs::ni_pxi_4071_t::~ni_pxi_4071_t()
{
}

void irs::ni_pxi_4071_t::get_param(const multimeter_param_t a_param,
  irs::raw_data_t<irs_u8> *ap_value) const
{
  IRS_LIB_ASSERT(ap_value);
  switch(a_param)
  {
    case mul_param_source_values:
    {
      double source_value = m_eth_mul_data.meas_value_not_filtered;
      ap_value->insert(ap_value->data(),
        reinterpret_cast<const irs_u8*>(&source_value),
        reinterpret_cast<const irs_u8*>(&source_value) + sizeof(source_value));
    } break;
    case mul_param_filtered_values:
    {
      double filtered_value = m_eth_mul_data.meas_value_not_filtered;
      ap_value->insert(ap_value->data(),
        reinterpret_cast<const irs_u8*>(&filtered_value),
        reinterpret_cast<const irs_u8*>(&filtered_value) +
        sizeof(filtered_value));
    } break;
    case mul_param_sampling_time_s:
    {
      double sampling_time_s = 0;
      if (m_eth_mul_data.sampling_freq == 0) {
        sampling_time_s = 0;
      } else {
        sampling_time_s = 1/m_eth_mul_data.sampling_freq;
      }
      //double sampling_time_s = 1/m_eth_mul_data.samples_per_sec;
      ap_value->insert(ap_value->data(),
        reinterpret_cast<const irs_u8*>(&sampling_time_s),
        reinterpret_cast<const irs_u8*>(&sampling_time_s) +
        sizeof(sampling_time_s));
    } break;
    // СКО
    case mul_param_standard_deviation:
    {
    
    } break;
    // Относительное СКО
    case mul_param_standard_deviation_relative:
    {
    
    } break;
    // Дельта
    case mul_param_variation:
    {
    
    } break;
    // Относительная дельта
    case mul_param_variation_relative:
    {
    
    } break;
    case mul_param_filter_settings:
    {
      ap_value->insert(ap_value->data(),
        reinterpret_cast<const irs_u8*>(&m_filter),
        reinterpret_cast<const irs_u8*>(&m_filter) + sizeof(m_filter));
    } break;
    default:
    {
    }
  }
}

void irs::ni_pxi_4071_t::set_param(const multimeter_param_t a_param,
  const irs::raw_data_t<irs_u8> &a_value)
{
  switch (a_param)
  {
    case mul_param_sampling_time_s:
    {
    
    } break;
    case mul_param_filter_settings:
    {
      m_filter = *reinterpret_cast<const filter_settings_t*>(a_value.data());
      m_eth_mul_data.filter_type = m_filter.family;
      m_eth_mul_data.filter_order = static_cast<irs_u8>(m_filter.order);
      m_eth_mul_data.sampling_freq = 1/m_filter.sampling_time_s;
      m_eth_mul_data.low_cutoff_freq = m_filter.low_cutoff_freq_hz;
      m_eth_mul_data.passband_ripple = m_filter.passband_ripple_db;
      m_eth_mul_data.stopband_ripple = m_filter.stopband_ripple_db;
    } break;
    default:
    {
    }
  }
}

bool irs::ni_pxi_4071_t::is_param_exists(
  const multimeter_param_t a_param) const
{
  return (a_param == mul_param_sampling_time_s) ||
    (a_param == mul_param_filter_settings) ||
    (a_param == mul_param_filtered_values) ||
    (a_param == mul_param_source_values);
}

void irs::ni_pxi_4071_t::set_dc()
{
  switch(m_eth_mul_data.meas_type)
  {
    case tm_volt_ac:
    {
      m_eth_mul_data.meas_type = tm_volt_dc;
    } break;
    case tm_current_ac:
    {
      m_eth_mul_data.meas_type = tm_current_dc;
    } break;
    default:
    {
    }
  }
}

void irs::ni_pxi_4071_t::set_ac()
{
  switch(m_eth_mul_data.meas_type)
  {
    case tm_volt_dc:
    {
      m_eth_mul_data.meas_type = tm_volt_ac;
    } break;
    case tm_current_dc:
    {
      m_eth_mul_data.meas_type = tm_current_ac;
    } break;
    default:
    {
    }
  }
}

void irs::ni_pxi_4071_t::set_positive()
{
}

void irs::ni_pxi_4071_t::set_negative()
{
}

void irs::ni_pxi_4071_t::get_value(double* ap_value)
{
  if (m_eth_mul_data.meas_status == meas_status_success) {
    mp_value = ap_value;
    m_status = meas_status_busy;
    m_eth_mul_data.meas_status = meas_status_busy;
    m_mode = get_value_mode;
    //m_pause.start();
  }
}

void irs::ni_pxi_4071_t::get_voltage(double* ap_voltage)
{
  if (m_eth_mul_data.meas_status == meas_status_success) {
    /*if (m_eth_mul_data.meas_type != tm_volt_dc) {
      m_eth_mul_data.meas_type = tm_volt_dc;
    }*/
    mp_value = ap_voltage;
    m_status = meas_status_busy;
    m_eth_mul_data.meas_status = meas_status_busy;
    m_mode = get_voltage_mode;
  }
}

void irs::ni_pxi_4071_t::get_current(double* ap_current)
{
  if (m_eth_mul_data.meas_type != tm_current_dc) {
    m_eth_mul_data.meas_type = tm_current_dc;
  }
  mp_value = ap_current;
  //m_status = meas_status_busy;
  m_eth_mul_data.meas_status = meas_status_busy;
  m_mode = get_current_mode;
}

void irs::ni_pxi_4071_t::get_resistance2x(double* ap_resistance)
{
  if (m_eth_mul_data.meas_type != tm_resistance_2x) {
    m_eth_mul_data.meas_type = tm_resistance_2x;
  }
  mp_value = ap_resistance;
  //m_status = meas_status_busy;
  m_eth_mul_data.meas_status = meas_status_busy;
  m_mode = get_resistance2x_mode;
}

void irs::ni_pxi_4071_t::get_resistance4x(double* ap_resistance)
{
  if (m_eth_mul_data.meas_type != tm_resistance_4x) {
    m_eth_mul_data.meas_type = tm_resistance_4x;
  }
  mp_value = ap_resistance;
  //m_status = meas_status_busy;
  m_eth_mul_data.meas_status = meas_status_busy;
  m_mode = get_resistance4x_mode;
}

void irs::ni_pxi_4071_t::get_frequency(double* /*ap_frequency*/)
{
}

void irs::ni_pxi_4071_t::get_phase_average(double* /*ap_phase_average*/)
{
}

void irs::ni_pxi_4071_t::get_phase(double* /*ap_phase*/)
{
}

void irs::ni_pxi_4071_t::get_time_interval(double* /*ap_time_interval*/)
{
}

void irs::ni_pxi_4071_t::get_time_interval_average(
  double* /*ap_time_interval_average*/)
{
}

void irs::ni_pxi_4071_t::auto_calibration()
{
}

meas_status_t irs::ni_pxi_4071_t::status()
{
  return m_status;
  /*if (m_eth_mul_data.meas_status) {
    return meas_status_busy;
  } else {
    return meas_status_success;
  }*/
}

void irs::ni_pxi_4071_t::abort()
{
  m_abort_request = true;
}

void irs::ni_pxi_4071_t::tick()
{
  m_modbus_client.tick();
  if (m_modbus_client.connected() &&
    (m_mul_mode_type == mul_mode_type_passive)) {
    m_filter.family =
      static_cast<filter_family_t>(
      static_cast<irs_u8>(m_eth_mul_data.filter_type));
    m_filter.order = static_cast<irs_u8>(m_eth_mul_data.filter_order);
    if (m_eth_mul_data.sampling_freq == 0) {
      m_filter.sampling_time_s = 0;
    } else {
      m_filter.sampling_time_s = 1/m_eth_mul_data.sampling_freq;
    }
    m_filter.low_cutoff_freq_hz = m_eth_mul_data.low_cutoff_freq;
    m_filter.passband_ripple_db = m_eth_mul_data.passband_ripple;
    m_filter.stopband_ripple_db = m_eth_mul_data.stopband_ripple;
  }
  switch(m_mode)
  {
    case start_mode:
    {
      
    } break;
    case get_value_mode:
    {
      //if (m_pause.check()) {
      if (m_eth_mul_data.meas_status == meas_status_success) {
        *mp_value = m_eth_mul_data.meas_value;
        m_mode = start_mode;
        m_status = meas_status_success;
        //m_eth_mul_data.meas_status = meas_status_success;
        //m_pause.stop();
      }
    } break;
    case get_voltage_mode:
    {
      if (m_eth_mul_data.meas_status == meas_status_busy) {
      //if (m_eth_mul_data.meas_type == tm_volt_dc) {
        m_mode = get_value_mode;
      //}
      }
    } break;
    case get_current_mode:
    {
      if (m_eth_mul_data.meas_type == tm_current_dc) {
        m_mode = get_value_mode;
      }
    } break;
    case get_resistance2x_mode:
    {
      if (m_eth_mul_data.meas_type == tm_resistance_2x) {
        m_mode = get_value_mode;
      }
    } break;
    case get_resistance4x_mode:
    {
      if (m_eth_mul_data.meas_type == tm_resistance_4x) {
        m_mode = get_value_mode;
      }
    } break;
  }
  switch(m_eth_mul_data.meas_mode)
  {
    case digitizer:
    {
      // range, meas_type, (nplc/aperture), resolution
    } break;
    case high_speed:
    {
      // range, meas_type, sample/sec, filter options
    } break;
  }
}

void irs::ni_pxi_4071_t::set_nplc(double a_nplc)
{
  double nplc = 0;
  if (a_nplc > 0.0004445) {
    if (a_nplc < 7450) {
      nplc = a_nplc;
    } else {
      nplc = 7450;
    }
  } else {
    nplc = 0.0004445;
  }
  m_eth_mul_data.integrate_time_units = plc;
  m_eth_mul_data.integrate_time = nplc;
}

void irs::ni_pxi_4071_t::set_aperture(double a_aperture)
{
  double aperture = 0;
  if (a_aperture > 0.00000889) {
    if (a_aperture < 149) {
      aperture = a_aperture;
    } else {
      aperture = 149;
    }
  } else {
    aperture = 0.00000889;
  }
  m_eth_mul_data.integrate_time_units = sec;
  m_eth_mul_data.integrate_time = aperture;
}

void irs::ni_pxi_4071_t::set_bandwidth(double /*a_bandwidth*/)
{
}

void irs::ni_pxi_4071_t::set_input_impedance(double /*a_impedance*/)
{
}

void irs::ni_pxi_4071_t::set_start_level(double /*a_level*/)
{
}

void irs::ni_pxi_4071_t::set_range(type_meas_t a_type_meas, double a_range)
{
  //m_eth_mul_data.meas_type = a_type_meas; // закоменчено временно из-за проблем
                                            // с установкой типа измерения
  
  double range = 0;
  switch(a_type_meas)
  {
    case tm_volt_dc:
    {
      if (a_range > 300) {
        range = 300;
      } else if ((a_range <= 300) && (a_range > 100)) {
        range = 300;
      } else if ((a_range <= 100) && (a_range > 10)) {
        range = 100;
      } else if ((a_range <= 10) && (a_range > 1)) {
        range = 10;
      } else if ((a_range <= 1) && (a_range > 0.1)) {
        range = 1;
      } else if (a_range <= 0.1) {
        range = 0.1;
      }
      m_eth_mul_data.range = range;
    } break;
    case tm_volt_ac:
    {
      if (a_range > 300) {
        range = 300;
      } else if ((a_range <= 300) && (a_range > 50)) {
        range = 300;
      } else if ((a_range <= 50) && (a_range > 5)) {
        range = 50;
      } else if ((a_range <= 5) && (a_range > 0.5)) {
        range = 5;
      } else if ((a_range <= 0.5) && (a_range > 0.05)) {
        range = 0.5;
      } else if (a_range <= 0.05) {
        range = 0.05;
      }
      m_eth_mul_data.range = range;
    } break;
    case tm_current_dc:
    {
      if (a_range > 1) {
        range = 1;
      } else if ((a_range <= 1) && (a_range > 0.2)) {
        range = 1;
      } else if ((a_range <= 0.2) && (a_range > 0.02)) {
        range = 0.2;
      } else if (a_range <= 0.02) {
        range = 0.02;
      }
      m_eth_mul_data.range = range;
    } break;
    case tm_current_ac:
    {
      if (a_range > 1) {
        range = 1;
      } else if ((a_range <= 1) && (a_range > 0.1)) {
        range = 1;
      } else if ((a_range <= 0.1) && (a_range > 0.01)) {
        range = 0.1;
      } else if (a_range <= 0.01) {
        range = 0.01;
      }
      m_eth_mul_data.range = range;
    } break;
    default:
    {
      IRS_ERROR(irs::ec_standard, "Unknown type range!");
    }
  }
}

void irs::ni_pxi_4071_t::set_range_auto()
{
  if (m_eth_mul_data.meas_mode == digitizer) {
    m_eth_mul_data.range = auto_range;
  } else {
    // digitizer mode: auto_range not supported
  }
}

irs::mxdata_t* irs::ni_pxi_4071_t::mxdata()
{
  return &m_modbus_client;
}

//---------------------------------------------------------------------------

