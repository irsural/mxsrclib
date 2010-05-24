// Классы для работы с мультиметрами
// Дата: 07.05.2010
// Ранняя дата: 10.09.2009

//#define OFF_EXTCOM // Отключение расширенных команд
//#define RESMEAS // Сброс перед измерением

#include <irsdefs.h>

#include <string.h>
#include <stdlib.h>

#include <irsstrdefs.h>

#include <measmul.h>

#include <irsdefs.h>

namespace {

struct str_tm_cont_t
{
  const irs::string volt_dc;
  const irs::string volt_ac;
  const irs::string current_dc;
  const irs::string current_ac;
  const irs::string resistance_2x;
  const irs::string resistance_4x;
  const irs::string frequency;
  const irs::string phase;
  const irs::string phase_average;
  const irs::string time_interval;
  const irs::string time_interval_average;
  str_tm_cont_t():
    volt_dc("Постоянное напряжение"),
    volt_ac("Переменное напряжение"),
    current_dc("Постоянный ток"),
    current_ac("Переменный ток"),
    resistance_2x("Сопротивление по 2-х проводной схеме"),
    resistance_4x("Сопротивление по 4-х проводной схеме"),
    frequency("Частота"),
    phase("Фаза"),
    phase_average("Усредненная фаза"),
    time_interval("Временной интервал"),
    time_interval_average("Усредненный временной интервал ")
  {
  }
};

str_tm_cont_t* str_tm()
{
  static auto_ptr<str_tm_cont_t> p_str_tm_cont(new str_tm_cont_t());
  return p_str_tm_cont.get();
}

} //namespace

irs::string type_meas_to_str(const type_meas_t a_type_meas)
{

  irs::string str_type_meas = str_tm()->volt_dc;

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
  const irs::string& a_str_type_meas, type_meas_t& a_type_meas)
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
{}
// Установить отрицательный фронт канала
void mx_agilent_3458a_t::set_negative()
{}
// Чтение значения при текущем типа измерения
void mx_agilent_3458a_t::get_value(double *ap_value)
{
  if (m_create_error) return;
  measure_create_commands(meas_value);
  m_voltage = ap_value;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение напряжения
void mx_agilent_3458a_t::get_voltage(double *voltage)
{
  if (m_create_error) return;
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
  if (m_create_error) return;
  measure_create_commands(meas_current);
  m_voltage = current;
  m_command = mac_get_param;
  m_status = meas_status_busy;
}
// Чтение сопротивления
void mx_agilent_3458a_t::get_resistance2x(double *resistance)
{
  if (m_create_error) return;
  m_get_resistance_commands[m_resistance_type_index] = m_resistance_type_2x;
  m_resistance = resistance;
  m_command = mac_get_resistance;
  m_status = meas_status_busy;
}
// Чтение сопротивления
void mx_agilent_3458a_t::get_resistance4x(double *resistance)
{
  if (m_create_error) return;
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
  if (m_create_error) return;
  m_command = mac_auto_calibration;
  m_status = meas_status_busy;
}
// Чтение статуса текущей операции
meas_status_t mx_agilent_3458a_t::status()
{
  if (m_create_error) return meas_status_busy;
  return m_status;
}
// Прерывание текущей операции
void mx_agilent_3458a_t::abort()
{
  if (m_create_error) return;
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

          #ifndef NOP
          if (read_count) {
            int i = 0;
            i++;
          }
          #endif //NOP

          buf[read_count] = 0;
          char end_chars[3] = {0x0D, 0x0A, 0x00};
          char *end_number = strstr((char *)m_read_buf, end_chars);
          if (end_number) {
            *end_number = 0;
            char *end_ptr = IRS_NULL;
            double val = strtod((char *)m_read_buf, &end_ptr);
            if (end_ptr == end_number) {
              *m_value = val;
              m_mode = ma_mode_macro;
            }
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
  if (m_create_error) return;
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


// Класс для работы с мультиметром b7-78/1

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
            char *end_ptr = IRS_NULL;
            double val = strtod((char *)f_read_buf, &end_ptr);
            if (end_ptr == end_number) {
              *f_value = val;
              f_mode = ma_mode_macro;
            }
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
    irs::string command = *(mv_multimetr_p_commands_setting[i]);
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
      buf_rd[size_rd] = '\0';
      m_string_msgs += irs::string((char*)buf_rd);
      if(m_meas_stat == meas_status_busy){
        irs::string message;
        msg_status_t msg_stat = m_string_msgs.get_message(message);
        if(msg_stat == MSG_SUCCESS){
          double value = 0.;
          bool fsuccess = false;
          fsuccess = message.to_number(value);
          //fsuccess = string_to_number(message, &value, locale("C"));
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
    irs::string aperture_str = "";
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
    irs::string impedance_str = "";
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
    irs::string level_str = a_level;
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
    irs::string bandwidth_str = "";
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



//---------------------------------------------------------------------------
//#pragma package(smart_init)


