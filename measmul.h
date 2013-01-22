//! \file
//! \ingroup drivers_group
//! \brief ������ ��� ������ � �������������
//!
//! ����: 07.02.2011\n
//! ������ ����: 10.09.2009


#ifndef measmulH
#define measmulH

// ������ ������ measmul
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

// ������ ������ ������
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
  // �������� �������� (������ double)
  mul_param_source_values = mul_param_first,
  // ��������������� �������� (������ double)
  mul_param_filtered_values,
  // ����������� ��������� (double)
  mul_param_standard_deviation,
  // ����������� ������������� ���������� (double)
  mul_param_standard_deviation_relative,
  // �������� (double)
  mul_param_variation,
  // ������������� �������� (double)
  mul_param_variation_relative,
  // ����� ������������� (double)
  mul_param_sampling_time_s,
  // ��������� ����������� ������� (bool)
  mul_param_analog_filter_enabled,
  // ��������� ������� (struct filter_settings_t ���� irsdsp.h)
  mul_param_filter_settings,
  // ������ �������� (enum mul_sample_format_t ���� measmul.h)
  mul_param_sample_format,
  // ������������ ����� ������ ���� (double)
  mul_param_tick_max_time_s,
  // ��������� ���� ���� ��� ���-�������
  // (enum window_function_form_t ���� irsfilter.h)
  mul_param_window_function_form,
  // ��������� ���� ������� �� ���� ���������� ��������������
  // (enum filter_impulse_response_type_t ���� irsdsp.h)
  mul_param_filter_impulse_response_type,
  // ��������� ������� ������������� �������� (bool)
  mul_param_filtered_values_enabled,
  mul_param_last = mul_param_filtered_values_enabled,
  mul_param_count = (mul_param_last - mul_param_first) + 1
};

//! ������ �������� �������� ����� ������������ � �����������
enum mul_sample_format_t {
  mul_sample_format_str,    //!< ��������� ������ �������� ��������
  mul_sample_format_int16,  //!< �������� ������(16 ���) �������� ��������
  mul_sample_format_int32   //!< �������� ������(32 ����) �������� ��������
};

//! \brief ����������� ������� ����� ��� ������ � �������������
class mxmultimeter_t
{
public:
  virtual ~mxmultimeter_t() {};
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  // ���������� ����� ��������� ����������� ���������� (���������� �������)
  virtual void set_dc() = 0;
  // ���������� ����� ��������� ����������� ���������� (���������� �������)
  virtual void set_ac() = 0;
  // ���������� ������������ ����� ������� (���������� �������)
  virtual void set_positive() = 0;
  // ���������� ������������� ����� ������ (��������� �������)
  virtual void set_negative() = 0;
  // ������ �������� ��� ������� ���� ��������� (��������� �������)
  virtual void get_value(double *value) = 0;
  // ������ ���������� (��������� �������)
  virtual void get_voltage(double *voltage) = 0;
  // ������ ���� ���� (��������� �������)
  virtual void get_current(double *current) = 0;
  // ������ ������������� (��������� �������)
  virtual void get_resistance2x(double *resistance) = 0;
  // ������ ������������� (��������� �������)
  virtual void get_resistance4x(double *resistance) = 0;
  // ������ ������� (��������� �������)
  virtual void get_frequency(double *frequency) = 0;
  // ������ ������������ ������ ��� (��������� �������)
  virtual void get_phase_average(double *phase_average) = 0;
  // ������ �������� ������ (��������� �������)
  virtual void get_phase(double *phase) = 0;
  // ������ ���������� ��������� (��������� �������)
  virtual void get_time_interval(double *time_interval) = 0;
  // ������ ������������ ���������� ��������� (��������� �������)
  virtual void get_time_interval_average(double *ap_time_interval) = 0;
  // ������ ��������� ���������� �� 1-� � 2-� �������
  virtual void get_voltage_ratio(double *voltage_ratio) { *voltage_ratio = 0.; }
  // ����� ������ ��� ����������� ���������
  virtual void select_channel(size_t /*channel*/) { }

  // ������ �������������� ����������� (���������� �������)
  virtual void auto_calibration() = 0;
  // ������ ������� ������� ��������
  virtual meas_status_t status() = 0;
  // ���������� ������� �������� (��������� �������)
  virtual void abort() = 0;
  // ������������ ��������
  virtual void tick() = 0;
  // ��������� ������� �������������� � �������� ������� ���� (20 ��)
  // (���������� �������)
  virtual void set_nplc(double nplc) = 0;
  // ��������� ������� �������������� � c (��������� �������)
  virtual void set_aperture(double aperture) = 0;
  // ��������� ������ ������� (��������� �������)
  virtual void set_bandwidth(double bandwidth) = 0;
  // ��������� �������� ������������� ������ (��������� �������)
  virtual void set_input_impedance(double impedance) = 0;
  // ��������� ������ ������� ������ (��������� �������)
  virtual void set_start_level(double level) = 0;
  // ��������� ��������� ��������� (��������� �������)
  virtual void set_range(type_meas_t a_type_meas, double a_range) = 0;
  // ��������� ��������������� ������ ��������� ��������� (���������� �������)
  virtual void set_range_auto() = 0;
  // ��������� �������� ������������� ������
  //virtual void set_filter(double ) = 0;
};

//! @}

namespace irs {

//! \ingroup drivers_group
//! \brief ����� ��� ������ � ������������ Agilent 3458A ����� irs::hardflow_t
class agilent_3458a_t: public mxmultimeter_t
{
  // ��� ��� �������� ������
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
  // ��� ��� ������� �������
  typedef enum _ma_command_t {
    mac_free,
    mac_get_param,
    mac_get_resistance,
    mac_auto_calibration,
    mac_send_commands
  } ma_command_t;
  // ��� ��� �����������
  typedef enum _macro_mode_t {
    macro_mode_get_voltage,
    macro_mode_get_resistance,
    macro_mode_send_commands,
    macro_mode_stop
  } macro_mode_t;
  // ��� ��� ��������
  typedef irs_i32 index_t;
  typedef irs::hardflow::fixed_flow_t::status_t fixed_flow_status_type;
  // ��� ��� ���� ���� � ���������� - ���������/����������
  enum volt_curr_type_t { vct_direct, vct_alternate };
  // ��� ��������� ���/����������
  enum measure_t {
    meas_value,
    meas_voltage,
    meas_current,
    meas_frequency,
    meas_set_range
  };
  // ����� �������������
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

  // ����� ������ �����������
  multimeter_mode_type_t m_mul_mode_type;
  // ������� ��� �������������
  vector<irs::string> m_init_commands;
  // ������ ������� ��������� ���� ����������: ����������/����������
  //index_t m_voltage_type_index;

  // ������� ��� �������� ���� ���������
  //const irs::string m_value_type;
  // ������� ��� ����������� ����������
  const irs::string m_voltage_type_direct;
  // ������� ��� ����������� ����������
  const irs::string m_voltage_type_alternate;
  // ������� ��� ����������� ����
  const irs::string m_current_type_direct;
  // ������� ��� ����������� ����
  const irs::string m_current_type_alternate;
  // ������� ��� ����������� ���������� c ����������
  irs::string m_voltage_type_direct_range;
  // ������� ��� ����������� ���������� c ����������
  irs::string m_voltage_type_alternate_range;
  // ������� ��� ����������� ���� c ����������
  irs::string m_current_type_direct_range;
  // ������� ��� ����������� ���� c ����������
  irs::string m_current_type_alternate_range;

  // ���� ���� � ���������� - ���������/����������
  volt_curr_type_t m_volt_curr_type;
  // ������ ������� ��������� ������� �������������� ��� ����������
  //index_t m_time_int_voltage_index;
  // ������� ��������� ������� �������������� ��� ����������
  irs::string m_time_int_measure_command;
  // ������� ��� ������ ����������
  vector<irs::string> m_get_measure_commands;
  // ������� ��� ��������� 2-��������� ����� ��������� �������������
  const irs::string m_resistance_type_2x;
  // ������� ��� ��������� 4-��������� ����� ��������� �������������
  const irs::string m_resistance_type_4x;
  // ������� ��� ������ �������������
  vector<irs::string> m_get_resistance_commands;

  // ������� ��� ��������� ��������� ���������
  irs::string m_set_range_command;
  // ��������� ��������
  irs::hardflow_t* mp_hardflow;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  //irs::raw_data_t<irs_u8> m_write_buf;
  // ������ ��������
  bool m_create_error;
  // ������� ����� ������
  ma_mode_t m_mode;
  // ������� ���������� ������
  macro_mode_t m_macro_mode;
  // ������ ������� ��������
  meas_status_t m_status;
  // ������� �������
  ma_command_t m_command;
  // ��������� �� ��������� ���������� ������������
  double *m_voltage;
  // ��������� �� ��������� ������������� ������������
  double *m_resistance;
  // ������ �� ���������� ��������
  bool m_abort_request;
  // ����� ������
  irs_u8 m_read_buf[ma_read_buf_size];
  // ������� ������� � ������ ������
  index_t m_read_pos;
  // ������� �������
  irs::string m_cur_mul_command;
  // ������� ��� �����������
  vector<irs::string> *m_mul_commands;
  // ������ ������ ��� �����������
  index_t m_mul_commands_index;
  // ���������� �������
  //ma_command_t m_command_prev;
  // ��������� �� ���������� � ������� ����� ������� ��������
  double *m_value;
  // ��������� ������ ���������
  bool m_get_parametr_needed;
  // ����� �������� ��������
  counter_t m_oper_time;
  // ������� ��������
  counter_t m_oper_to;
  // ����� �������� ����������
  counter_t m_acal_time;
  // ������� ����������
  counter_t m_acal_to;
  // ������ ������ ������ �������������
  irs::timer_t m_init_timer;
  // ����� �������������
  init_mode_t m_init_mode;
  // ������ ������� �������������
  index_t m_ic_index;
  // ����������, ���� ��������� ������� ������ hardflow ����� �������
  // �������� �� �����������
  bool m_is_clear_buffer_needed;

  bool m_first_meas_resist;

  const irs::string m_resistance_ocomp_off;
  const irs::string m_resistance_ocomp_on;

  // ���������� ������������ �� ���������
  agilent_3458a_t();
  // �������� ������ ��� ���������/����
  void measure_create_commands(measure_t a_measure);
  // �������� ������ ������������� � ����������
  void initialize_tick();
public:
  // �����������
  agilent_3458a_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // ����������
  ~agilent_3458a_t();
  // ���������� ����� ��������� ����������� ����������
  virtual void set_dc();
  // ���������� ����� ��������� ����������� ����������
  virtual void set_ac();
  // ���������� ������������ ����� �������
  virtual void set_positive();
  // ���������� ������������� ����� ������
  virtual void set_negative();
  // ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  // ������ ����������
  virtual void get_voltage(double *voltage);
  // ������ ���� ����
  virtual void get_current(double *current);
  // ������ �������������
  virtual void get_resistance2x(double *resistance);
  // ������ �������������
  virtual void get_resistance4x(double *resistance);
  // ������ �������
  virtual void get_frequency(double *frequency);
  // ������ ������������ ������ ���
  virtual void get_phase_average(double *phase_average);
  // ������ �������� ������
  virtual void get_phase(double *phase);
  // ������ ���������� ���������
  virtual void get_time_interval(double *time_interval);
  // ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);

  // ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration();
  // ������ ������� ������� ��������
  virtual meas_status_t status();
  // ���������� ������� ��������
  virtual void abort();
  // ������������ ��������
  virtual void tick();
  // ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double nplc);
  // ��������� ������� �������������� � c
  virtual void set_aperture(double aperture);
  // ��������� ������ �������
  virtual void set_bandwidth(double bandwidth);
  // ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance);
  // ��������� ������ ������� ������
  virtual void set_start_level(double level);
  // ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto();

  // ��������� ���������� ��������� ���������
  //void set_time_interval(double time_interval);
};

} // namespace irs

//! \ingroup drivers_group
//! \brief ����� ��� ������ � ������������ Agilent 3458A
class mx_agilent_3458a_t: public mxmultimeter_t
{
  // ��� ��� �������� ������
  typedef enum _ma_mode_t {
    ma_mode_start,
    ma_mode_macro,
    ma_mode_commands,
    ma_mode_commands_wait,
    ma_mode_get_value,
    ma_mode_auto_calibration,
    ma_mode_auto_calibration_wait
  } ma_mode_t;
  // ��� ��� ������� �������
  typedef enum _ma_command_t {
    mac_free,
    mac_get_param,
    mac_get_resistance,
    mac_auto_calibration,
    mac_send_commands
  } ma_command_t;
  // ��� ��� �����������
  typedef enum _macro_mode_t {
    macro_mode_get_voltage,
    macro_mode_get_resistance,
    macro_mode_send_commands,
    macro_mode_stop
  } macro_mode_t;
  // ��� ��� ��������
  typedef irs_i32 index_t;
  // ��� ��� ���� ���� � ���������� - ���������/����������
  enum volt_curr_type_t { vct_direct, vct_alternate };
  // ��� ��������� ���/����������
  enum measure_t {
    meas_value,
    meas_voltage,
    meas_current,
    meas_frequency,
    meas_set_range
  };
  // ����� �������������
  enum init_mode_t {
    im_start,
    im_write_command,
    im_next_command,
    im_stop
  };

  // ����� ������ �����������
  multimeter_mode_type_t m_mul_mode_type;
  // ������� ��� �������������
  vector<irs::string> m_init_commands;
  // ������ ������� ��������� ���� ����������: ����������/����������
  //index_t m_voltage_type_index;

  // ������� ��� �������� ���� ���������
  //const irs::string m_value_type;
  // ������� ��� ����������� ����������
  const irs::string m_voltage_type_direct;
  // ������� ��� ����������� ����������
  const irs::string m_voltage_type_alternate;
  // ������� ��� ����������� ����
  const irs::string m_current_type_direct;
  // ������� ��� ����������� ����
  const irs::string m_current_type_alternate;
  // ������� ��� ����������� ���������� c ����������
  irs::string m_voltage_type_direct_range;
  // ������� ��� ����������� ���������� c ����������
  irs::string m_voltage_type_alternate_range;
  // ������� ��� ����������� ���� c ����������
  irs::string m_current_type_direct_range;
  // ������� ��� ����������� ���� c ����������
  irs::string m_current_type_alternate_range;

  // ���� ���� � ���������� - ���������/����������
  volt_curr_type_t m_volt_curr_type;
  // ������ ������� ��������� ������� �������������� ��� ����������
  // index_t m_time_int_voltage_index;
  // ������� ��������� ������� �������������� ��� ����������
  irs::string m_time_int_measure_command;
  // ������� ��� ������ ����������
  vector<irs::string> m_get_measure_commands;
  // ������ ������� ��������� ���� ��������� �������������:
  // 2-/4-��������� �����
  index_t m_resistance_type_index;
  // ������� ��� ��������� 2-��������� ����� ��������� �������������
  const irs::string m_resistance_type_2x;
  // ������� ��� ��������� 4-��������� ����� ��������� �������������
  const irs::string m_resistance_type_4x;
  // ������ ������� ��������� ������� �������������� ��� �������������
  index_t m_time_int_resistance_index;
  // ������� ��� ������ �������������
  vector<irs::string> m_get_resistance_commands;

  // ������� ��� ��������� ��������� ���������
  irs::string m_set_range_command;
  // ���������� ������ mxifa
  void *m_handle;
  // ������ ��������
  bool m_create_error;
  // ������� ����� ������
  ma_mode_t m_mode;
  // ������� ���������� ������
  macro_mode_t m_macro_mode;
  // ������ ������� ��������
  meas_status_t m_status;
  // ������� �������
  ma_command_t m_command;
  // ��������� �� ��������� ���������� ������������
  double *m_voltage;
  // ��������� �� ��������� ������������� ������������
  double *m_resistance;
  // ������ �� ���������� ��������
  bool m_abort_request;
  // ����� ������
  irs_u8 m_read_buf[ma_read_buf_size];
  // ������� ������� � ������ ������
  index_t m_read_pos;
  // ������� �������
  irs::string m_cur_mul_command;
  // ������� ��� �����������
  vector<irs::string> *m_mul_commands;
  // ������ ������ ��� �����������
  index_t m_mul_commands_index;
  // ���������� �������
  //ma_command_t m_command_prev;
  // ��������� �� ���������� � ������� ����� ������� ��������
  double *m_value;
  // ��������� ������ ���������
  bool m_get_parametr_needed;
  // ����� �������� ��������
  counter_t m_oper_time;
  // ������� ��������
  counter_t m_oper_to;
  // ����� �������� ����������
  counter_t m_acal_time;
  // ������� ����������
  counter_t m_acal_to;
  // ������ ������ ������ �������������
  irs::timer_t m_init_timer;
  // ����� �������������
  init_mode_t m_init_mode;
  // ������ ������� �������������
  index_t m_ic_index;

  // ���������� ������������ �� ���������
  mx_agilent_3458a_t();
  // �������� ������ ��� ���������/����
  void measure_create_commands(measure_t a_measure);
  // �������� ������ ������������� � ����������
  void initialize_tick();
public:
  // �����������
  mx_agilent_3458a_t(
    mxifa_ch_t channel,
    gpib_addr_t address,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // ����������
  ~mx_agilent_3458a_t();
  // ���������� ����� ��������� ����������� ����������
  virtual void set_dc();
  // ���������� ����� ��������� ����������� ����������
  virtual void set_ac();
  // ���������� ������������ ����� �������
  virtual void set_positive();
  // ���������� ������������� ����� ������
  virtual void set_negative();
  // ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  // ������ ����������
  virtual void get_voltage(double *voltage);
  // ������ ���� ����
  virtual void get_current(double *current);
  // ������ �������������
  virtual void get_resistance2x(double *resistance);
  // ������ �������������
  virtual void get_resistance4x(double *resistance);
  // ������ �������
  virtual void get_frequency(double *frequency);
  // ������ ������������ ������ ���
  virtual void get_phase_average(double *phase_average);
  // ������ �������� ������
  virtual void get_phase(double *phase);
  // ������ ���������� ���������
  virtual void get_time_interval(double *time_interval);
  // ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);

  // ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration();
  // ������ ������� ������� ��������
  virtual meas_status_t status();
  // ���������� ������� ��������
  virtual void abort();
  // ������������ ��������
  virtual void tick();
  // ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double nplc);
  // ��������� ������� �������������� � c
  virtual void set_aperture(double aperture);
  // ��������� ������ �������
  virtual void set_bandwidth(double bandwidth);
  // ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance);
  // ��������� ������ ������� ������
  virtual void set_start_level(double level);
  // ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto();

  // ��������� ���������� ��������� ���������
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

// ����������� ������������������ ���� � ����� ���������� ������ ����
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
        "��������� ������ �� �������������� ����������");
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
  // ������������� ��������� �� �������/�������� ������� ������.
  // ���� ����������� ��������, ��� ������������
  void set(
    const bool a_flip_data_enabled,
    const size_type a_sample_size,
    const double a_coefficient,
    const raw_data_t<irs_u8>* ap_data,
    raw_data_t<double>* ap_samples);
  // ���������� ������ �������������
  bool completed() const;
  // ������������ ��������
  void tick();
  // ���������� ��������
  void abort();
  // ��������� ������� ����
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
  // ���������� ������ �������������
  bool completed() const;
  // ������������ ��������
  void tick();
  // ���������� ��������
  void abort();
  // ��������� ������� ����
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
  // ���������� ������ �������������
  bool completed() const;
  // ������������ ��������
  void tick();
  // ���������� ��������
  void abort();
  // ��������� ������� ����
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
  // ���������� ������ �������������
  bool completed() const;
  // ������������ ��������
  void tick();
  // ���������� ��������
  void abort();
  // ��������� ������� ����
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

// ����� ����������� ����������.
// ��������� �������: value_t - ��� ����������, ��� �������,
// iterator_t - ��� ��������� ��� ��������� ������� ��������,
// container_t - ��� ����������, ������������ ��������������� ��������
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
  // ���������� ������ �������������
  bool completed() const;
  // ������������ ��������
  void tick();
  // ���������� ��������
  void abort();
  // ��������� ������� ����
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

//! \brief ����� ����������� ���������� �������� � �������� ����������
//!   ���������������.
//!
//! ��������� �������: value_t - ��� ����������, ��� �������,
//!   iterator_t - ��� ��������� ��� ��������� ������� ��������,
//!   container_t - ��� ����������, ������������ ��������������� ��������.
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
  //! \brief ���������� ������ �������������
  bool completed() const;
  //! \brief ������������ ��������
  void tick();
  //! \brief ���������� ��������
  void abort();
  //! \brief ��������� ������� ����
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

//! \brief ����� ��� ������ � ������������ Agilent 3458A � ������ �������������
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
  //! \brief �����������
  agilent_3458a_digitizer_t(
    irs::hardflow_t* ap_hardflow,
    const filter_settings_t& a_filter_settings,
    const double a_sampling_time_s = 25e-6,
    const double a_interval_s = 30.0
  );
  //! \brief ����������
  ~agilent_3458a_digitizer_t();
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_dc();
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_ac();
  //! \brief ���������� ������������ ����� �������
  virtual void set_positive();
  //! \brief ���������� ������������� ����� ������
  virtual void set_negative();
  //! \brief ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  //! \brief ������ ����������
  virtual void get_voltage(double *voltage);
  //! \brief ������ ���� ����
  virtual void get_current(double *current);
  //! \brief ������ �������������
  virtual void get_resistance2x(double *resistance);
  //! \brief ������ �������������
  virtual void get_resistance4x(double *resistance);
  //! \brief ������ �������
  virtual void get_frequency(double *frequency);
  //! \brief ������ ������������ ������ ���
  virtual void get_phase_average(double *phase_average);
  //! \brief ������ �������� ������
  virtual void get_phase(double *phase);
  //! \brief ������ ���������� ���������
  virtual void get_time_interval(double *time_interval);
  //! \brief ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval); 
  //! \brief ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration();
  //! \brief ������ ������� ������� ��������
  virtual meas_status_t status();
  //! \brief ���������� ������� ��������
  virtual void abort();
  //! \brief ������������ ��������
  virtual void tick();
  //! \brief ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double nplc);
  //! \brief ��������� ������� �������������� � c
  virtual void set_aperture(double aperture);
  //! \brief ��������� ������ �������
  virtual void set_bandwidth(double bandwidth);
  //! \brief ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance);
  //! \brief ��������� ������ ������� ������
  virtual void set_start_level(double level);
  //! \brief ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief ��������� ��������������� ������ ��������� ���������
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
  // ������� ��� �������������
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
    "���������� �������� ��� �� ���������");
  m_sum += a_val;
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::add(
  iterator_t ap_begin, iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "���������� �������� ��� �� ���������");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "�������� ������ ������ ���� ������ ��������� �����");
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
    // �������� ���������
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
    // �������� ��� ���������
  }
}

template <class data_t, class iterator_t>
void accumulate_asynch_t<data_t, iterator_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "������������ ����� ���� ������ ���� ������ ����");
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
    "���������� �������� ��� �� ���������");
  mp_sko_calc->add(a_val);
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::add(iterator_t ap_begin,
  iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "���������� �������� ��� �� ���������");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "�������� ������ ������ ���� ������ ��������� �����");
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
    // �������� ���������
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
    // �������� ��� ���������
  }
}

template <class data_t, class calc_t, class iterator_t>
void sko_calc_asynch_t<data_t, calc_t, iterator_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "������������ ����� ���� ������ ���� ������ ����");
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
    "���������� �������� ��� �� ���������");
  IRS_LIB_ERROR_IF(mp_begin > mp_end, ec_standard, "�������� ������ ������"
    " ���� ������ ��������� �����");
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
    // �������� ���������
  }
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::abort()
{
  if (!m_completed) {
    mp_end = mp_begin;
    m_completed = true;
  } else {
    // �������� ��� ���������
  }
}

template <class value_t, class iterator_t>
void delta_calc_asynch_t<value_t, iterator_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "������������ ����� ���� ������ ���� ������ ����");
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
    // ������������ ������ �����
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
    "���������� �������� ��� �� ���������");
  m_iir_filter.set(a_sample);
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set(
  iterator_t ap_begin, iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "���������� �������� ��� �� ���������");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "�������� ������ ������ ���� ������ ��������� �����");
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
            // ������ ��� �� ������ ���������
          }
          mp_filt_values->insert(mp_filt_values->end(),
            static_cast<typename container_t::value_type>(m_iir_filter.get()));
        } else {
          // ������������ �� ��������� ����� ������������� ��������
        }
        ++mp_begin;
        count++;
      }
      m_completed = (mp_begin == mp_end);
    }
  } else {
    // �������� ���������
  }
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::abort()
{
  if (!m_completed) {
    mp_end = mp_begin;
    m_completed = true;
  } else {
    // ������ �� ������
  }
}

template <class value_t, class iterator_t, class container_t>
void iir_filter_asynch_t<value_t, iterator_t, container_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "������������ ����� ���� ������ ���� ������ ����");
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
    "���������� �������� ��� �� ���������");
  m_fir_filter.set(a_sample);
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::set(
  iterator_t ap_begin, iterator_t ap_end)
{
  IRS_LIB_ERROR_IF(!m_completed, ec_standard,
    "���������� �������� ��� �� ���������");
  IRS_LIB_ERROR_IF(ap_begin > ap_end, ec_standard,
    "�������� ������ ������ ���� ������ ��������� �����");
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
              // ������ ��� �� ������ ���������
            }
            mp_filt_values->insert(mp_filt_values->end(),
              static_cast<typename container_t::value_type>(
              m_fir_filter.get()));
          } else {
            // ������������ �� ��������� ����� ������������� ��������
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
    // �������� ���������
  }
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::abort()
{
  if (!m_completed) {
    mp_end = mp_begin;
    m_completed = true;
  } else {
    // ������ �� ������
  }
}

template <class value_t, class iterator_t, class container_t>
void fir_filter_asynch_t<value_t, iterator_t, container_t>::set_tick_max_time(
  const double a_set_tick_max_time_s)
{
  IRS_LIB_ERROR_IF(a_set_tick_max_time_s <= 0, ec_standard,
    "������������ ����� ���� ������ ���� ������ ����");
  m_tick_timer.set(make_cnt_s(a_set_tick_max_time_s));
}

#endif // IRS_FULL_STDCPPLIB_SUPPORT

//! \brief ����� ��� ������ � ������������ �7-78/1
class v7_78_1_t: public mxmultimeter_t
{
  //! \brief ��� ��� �������� ������
  typedef enum _ma_mode_t {
    ma_mode_start,
    ma_mode_macro,
    ma_mode_commands,
    ma_mode_commands_wait,
    ma_mode_get_value,
    ma_mode_auto_calibration,
    ma_mode_auto_calibration_wait
  } ma_mode_t;
  //! \brief ��� ��� ������� �������
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
  //! \brief ��� ��� ��������
  typedef irs_i32 index_t;


  //static const char m_volt_ac_nplcycles = "DETector:BANDwidth 20";

  multimeter_mode_type_t m_mul_mode_type;

  //! \brief ������� ��� �������������
  vector<irs::string> f_init_commands;
  //! \brief ������ ������� ��������� ������� �������������� ���
  //!  ����������� ����������
  index_t m_nplc_voltage_dc_index;
  //! \brief ������ ������� ��������� ������ ������� ��� ����������� ����������
  index_t m_band_width_voltage_ac_index;
  //! \brief ������� ������ �������� ��� ������������ ����������
  vector<irs::string> m_get_value_commands;
  //! \brief ������� ��� ������ ����������
  vector<irs::string> m_get_voltage_dc_commands;
  vector<irs::string> m_get_voltage_ac_commands;

  //! \brief ������ ������� ��������� ������� �������������� ��� �������������
  index_t m_nplc_resistance_2x_index;
  index_t m_nplc_resistance_4x_index;
  //! \brief ������� ��� ������ �������������
  vector<irs::string> m_get_resistance_2x_commands;
  vector<irs::string> m_get_resistance_4x_commands;

  //! \brief ������ ������� ��������� ������� �������������� ��� ����
  index_t m_nplc_current_dc_index;
  index_t m_nplc_current_ac_index;
  //! \brief ������� ��� ������ ����
  vector<irs::string> m_get_current_dc_commands;
  vector<irs::string> m_get_current_ac_commands;

  //! \brief ������ ������� ��������� ������� �����
  index_t m_aperture_frequency_index;
  //! \brief ������� ��� ������ �������
  vector<irs::string> m_get_frequency_commands;

  //! \brief ���������� ������ mxifa
  void *f_handle;
  //! \brief ������ ��������
  irs_bool f_create_error;
  //! \brief ������� ����� ������
  ma_mode_t f_mode;
  //! \brief ������� ���������� ������
  macro_mode_t f_macro_mode;
  //! \brief ������ ������� ��������
  meas_status_t f_status;
  //! \brief ������� �������
  ma_command_t f_command;
  //! \brief ��������� �� ���������� ���������� ��������,
  //!  ��� ������������ ���� ���������
  double* mp_value;
  //! \brief ��������� �� ��������� ���������� ������������
  double* mp_voltage;
  //! \brief ��������� �� ��������� ������������� ������������
  double* mp_resistance;
  //! \brief ��������� �� ��������� ��� ������������
  double* mp_current;
  //! \brief ��������� �� ��������� ������� ������������
  double* mp_frequency;
  //! \brief ������ �� ���������� ��������
  irs_bool f_abort_request;
  //! \brief ����� ������
  irs_u8 f_read_buf[ma_read_buf_size];
  //! \brief ������� ������� � ������ ������
  index_t f_read_pos;
  //! \brief ������� �������
  irs::string f_cur_mul_command;
  //! \brief ������� ��� �����������
  vector<irs::string> *f_mul_commands;
  //! \brief ������ ������ ��� �����������
  index_t f_mul_commands_index;
  // ���������� �������
  //ma_command_t f_command_prev;
  //! \brief ��������� �� ���������� � ������� ����� ������� ��������
  double *f_value;
  //! \brief ��������� ������ ���������
  irs_bool f_get_parametr_needed;
  //! \brief ����� �������� ��������
  counter_t f_oper_time;
  //! \brief ������� ��������
  counter_t f_oper_to;
  //! \brief ����� �������� ����������
  counter_t f_acal_time;
  //! \brief ������� ����������
  counter_t f_acal_to;

  bool m_current_volt_dc_meas;
  //! \brief ���������� ������������ �� ���������
  v7_78_1_t();
public:
  //! \brief �����������
  v7_78_1_t(
    mxifa_ch_t channel,
    gpib_addr_t address,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  //! \brief ����������
  ~v7_78_1_t();
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual inline void set_dc();
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual inline void set_ac();
  //! \brief ���������� ������������ ����� �������
  virtual void set_positive();
  //! \brief ���������� ������������� ����� ������
  virtual void set_negative();
  //! \brief ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  //! \brief ������ ����������
  virtual void get_voltage(double *voltage);
  //! \brief ������ ���� ����
  virtual void get_current(double *current);
  //! \brief ������ �������������
  virtual void get_resistance2x(double *resistance);
  //! \brief ������ �������������
  virtual void get_resistance4x(double *resistance);
  //! \brief ������ �������
  virtual void get_frequency(double *frequency);
  //! \brief ������ ������������ ������ ���
  virtual void get_phase_average(double *phase_average);
  //! \brief ������ �������� ������
  virtual void get_phase(double *phase);
  //! \brief ������ ���������� ���������
  virtual void get_time_interval(double *time_interval);
  //! \brief ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);

  //! \brief ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration();
  //! \brief ������ ������� ������� ��������
  virtual meas_status_t status();
  //! \brief ���������� ������� ��������
  virtual void abort();
  //! \brief ������������ ��������
  virtual void tick();
  //! \brief ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double nplc);
  //! \brief ��������� ������� �������������� � c
  virtual void set_aperture(double aperture);
  //! \brief ��������� ������ �������
  virtual void set_bandwidth(double bandwidth);
  //! \brief ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance);
  //! \brief ��������� ������ ������� ������
  virtual void set_start_level(double level);
  //! \brief ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto();
private:
  //! \brief ��������� ���������� ��������� ���������
  void set_time_interval_meas(const double a_time_interval_meas);
};
//! \brief ���������� ����� ��������� ����������� ����������
inline void irs::v7_78_1_t::set_dc()
{
  m_meas_type = DC_MEAS;
}
//! \brief ���������� ����� ��������� ����������� ����������
inline void irs::v7_78_1_t::set_ac()
{
  m_meas_type = AC_MEAS;
}

//! \ingroup drivers_group
//! \brief ����� ��� ������ � ������������ agilent 34420a
class agilent_34420a_t: public mxmultimeter_t
{
  typedef irs::hardflow::fixed_flow_t::status_t fixed_flow_status_type;
  //! \brief ��� ��� �������� ������
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
  //! \brief ��� ��� ������� �������
  enum ma_command_t {
    mac_free,
    mac_get_value,
    mac_get_voltage,
    mac_get_resistance,
    mac_set_params,
    mac_auto_calibration
  };
  enum macro_mode_t {
    //! \brief ����� ������ ������� ���������� ��������.
    macro_mode_get_value,
    //! \brief ����� �������� ������ ��� ������ ����������.
    macro_mode_get_voltage,
    //! \brief ����� �������� ������ ��� ������ �������������.
    macro_mode_get_resistance,
    //! \brief ����� ������� ������ ��� ��������� ����������.
    macro_mode_set_params,
    //! \brief ����� ���������� �������� ������ � ������ ��������.
    macro_mode_stop
  };
  //! \brief ����� �������������
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
  //! \brief ��� ��� ��������
  typedef irs_i32 index_t;


  //static const char m_volt_ac_nplcycles = "DETector:BANDwidth 20";

  multimeter_mode_type_t m_mul_mode_type;

  //! \brief ������� ��� �������������
  vector<irs::string> m_init_commands;
  //! \brief ������� ������ �������� ��� ������������ ����������
  vector<irs::string> m_get_value_commands;
  //! \brief ������� ���������/���������� ����������� �������
  irs::string m_analog_filter;
  irs::string m_configure_voltage_dc;
  index_t m_configure_voltage_dc_index;
  //! \brief ������� ��������� ��������� ��� ����������
  irs::string m_range_voltage_dc;
  //! \brief ����� ������� ���������/���������� ����������� �������,
  //!   ������������ ������
  irs::string m_analog_filter_status;
  //! \brief ������ ������� ���������/���������� ����������� ������� ���
  //!   ����������
  index_t m_analog_filter_voltage_dc_index;
  //! \brief ������ ������� ��������� ������� �������������� ���
  //!  ����������� ����������
  index_t m_nplc_voltage_dc_index;
  //! \brief ������� ��� ������ ����������
  vector<irs::string> m_get_voltage_dc_commands;

  irs::string m_configure_resistance_2x;
  index_t m_configure_resistance_2x_index;
  irs::string m_range_resistance_2x;
  //! \brief ������ ������� ��������� ������� �������������� ��� �������������
  index_t m_nplc_resistance_2x_index;
  //! \brief ������� ��� ������ �������������
  vector<irs::string> m_get_resistance_2x_commands;

  irs::string m_configure_resistance_4x;
  index_t m_configure_resistance_4x_index;
  irs::string m_range_resistance_4x;
  //! \brief ������ ������� ��������� ������� �������������� ��� �������������
  index_t m_nplc_resistance_4x_index;
  vector<irs::string> m_get_resistance_4x_commands;
  //! \brief ������� ������ ������
  index_t m_select_channel_dc_index;
  irs::string m_select_channel;
  vector<irs::string> m_front;
  vector<irs::string> m_channel;
  size_t m_current_channel;
  vector<irs::string> m_select_channel_command;
  //! \brief ������� ��� ��������� ����������
  vector<irs::string> m_set_params_commands;
  // ��������� ��������
  irs::hardflow_t* mp_hardflow;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  //! \brief ������ ��������
  bool m_create_error;
  //! \brief ������� ����� ������
  ma_mode_t m_mode;
  //! \brief ������� ���������� ������
  macro_mode_t m_macro_mode;
  //! \brief ������ ������� ��������
  meas_status_t m_status;
  //! \brief ������� �������
  ma_command_t m_command;
  //! \brief ��������� �� ���������� ���������� ��������,
  //!  ��� ������������ ���� ���������
  double* mp_value;
  //! \brief ������ �� ���������� ��������
  bool m_abort_request;
  //! \brief ����� ������
  irs_u8 m_read_buf[ma_read_buf_size];
  //! \brief ������� ������� � ������ ������
  index_t m_read_pos;
  //! \brief ������� �������
  irs::string m_cur_mul_command;
  //! \brief ������� ��� �����������
  vector<irs::string> *mp_mul_commands;
  //! \brief ������ ������ ��� �����������
  index_t m_mul_commands_index;
  // ���������� �������
  //ma_command_t f_command_prev;
  //! \brief ��������� �� ���������� � ������� ����� ������� ��������
  //double *mp_value;
  //! \brief ��������� ������ ���������
  irs_bool m_get_parametr_needed;
  //! \brief ����� �������� ��������
  counter_t m_oper_time;
  //! \brief ������� ��������
  counter_t m_oper_to;
  //! \brief ����� �������� ����������
  counter_t m_acal_time;
  //! \brief ������� ����������
  counter_t m_acal_to;
  // ������ ������ ������ �������������
  irs::timer_t m_init_timer;
  // ����� �������������
  init_mode_t m_init_mode;
  // ������ ������� �������������
  index_t m_ic_index;
  bool m_current_volt_dc_meas;
  //! \brief ���������� ������������ �� ���������
  agilent_34420a_t();
  // �������� ������ ������������� � ����������
  void initialize_tick();
public:
  //! \brief �����������
  agilent_34420a_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  //! \brief ����������
  ~agilent_34420a_t();
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual inline void set_dc();
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual inline void set_ac();
  //! \brief ���������� ������������ ����� �������
  virtual void set_positive();
  //! \brief ���������� ������������� ����� ������
  virtual void set_negative();
  //! \brief ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  //! \brief ������ ����������
  virtual void get_voltage(double *voltage);
  //! \brief ������ ���� ����
  virtual void get_current(double *current);
  //! \brief ������ �������������
  virtual void get_resistance2x(double *resistance);
  //! \brief ������ �������������
  virtual void get_resistance4x(double *resistance);
  //! \brief ������ �������
  virtual void get_frequency(double *frequency);
  //! \brief ������ ������������ ������ ���
  virtual void get_phase_average(double *phase_average);
  //! \brief ������ �������� ������
  virtual void get_phase(double *phase);
  //! \brief ������ ���������� ���������
  virtual void get_time_interval(double *time_interval);
  //! \brief ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);
  //! \brief ������ ��������� ���������� �� 1-� � 2-� �������
  virtual void get_voltage_ratio(double *voltage_ratio);
  //! \brief ����� ������ ��� ����������� ���������
  virtual void select_channel(size_t channel);

  //! \brief ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration();
  //! \brief ������ ������� ������� ��������
  virtual meas_status_t status();
  //! \brief ���������� ������� ��������
  virtual void abort();
  //! \brief ������������ ��������
  virtual void tick();
  //! \brief ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double nplc);
  //! \brief ��������� ������� �������������� � c
  virtual void set_aperture(double aperture);
  //! \brief ��������� ������ �������
  virtual void set_bandwidth(double bandwidth);
  //! \brief ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance);
  //! \brief ��������� ������ ������� ������
  virtual void set_start_level(double level);
  //! \brief ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief ��������� ��������������� ������ ��������� ���������
virtual void set_range_auto();
private:
  //! \brief ��������� ���������� ��������� ���������
  void set_time_interval_meas(const double a_time_interval_meas);
};
//! \brief ���������� ����� ��������� ����������� ����������
inline void irs::agilent_34420a_t::set_dc()
{
  m_meas_type = DC_MEAS;
}
//! \brief ���������� ����� ��������� ����������� ����������
inline void irs::agilent_34420a_t::set_ac()
{
  m_meas_type = AC_MEAS;
}

//! \brief ����� ��� ������ � ������������ ����������-������� ���� ��-85/3R
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
    // ��������� ���������� ��������
    vector<irs::irs_string_t> mv_bad_str;
    // ���������� ���������(���� ��� ������������)
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
        // �������� ������ �� �����
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
  //! \brief ������ ������� ��������
  command_status_t m_send_command_stat;
  meas_status_t m_meas_stat;

  //! \brief ��������� �� ���������������� ����������, � �������
  //!  ����� �������� ���������.
  double* mp_result;
  //! \brief ��� ��� ��������
  typedef irs_i32 index_t;
  //! \brief ��������� �� ��������� ������� ���������
  vector<irs::string>* mp_last_commands;

  //! \brief ������� ��� �������������
  vector<irs::string> mv_init_commands;

  //! \brief ������� ��� ������������� �����������
  vector<irs::string> mv_reset_multimeter_commands;

  //! \brief ��������� �� ������� �������� �����������
  vector<irs::string*> mv_multimeter_p_commands_setting;

  //! \brief ������� ��� ������ �������� ���������� ��������,
  //!  ��� ������������ ���� ���������
  vector<irs::string> mv_get_value_commands;

  //! \brief ������� ��� ������ ����������� �������� ���
  vector<irs::string> mv_get_phase_average_commands;

  //! \brief ������� ��� ������ �������� ���
  vector<irs::string> mv_get_phase_commands;

  //! \brief ������� ��� ������ ���������� ���������
  vector<irs::string> mv_get_time_interval_commands;

  //! \brief ������� ��� ������ ������������ ���������� ���������
  vector<irs::string> mv_get_time_interval_average_commands;

  //! \brief ������� ��� ������ �������
  vector<irs::string> mv_get_frequency_commands;

  //! \brief �������, ������� �� �������� ����������
  vector<irs::string> mv_big_time_commands;

  //! \brief ������� ��������� ������� �����
  irs::string ms_set_interval_command;

  //! \brief ������� ��������� �������� ������������� �������
  irs::string ms_set_in_impedance_channel_1_command;
  irs::string ms_set_in_impedance_channel_2_command;

  //! \brief ������� ��������� ���� ����� ����� ������
  irs::string ms_set_type_coupling_channel_1_command;
  irs::string ms_set_type_coupling_channel_2_command;

  //! \brief ������� ��������� ���� ������ ������� ������
  irs::string ms_set_type_slope_channel_1_command;
  irs::string ms_set_type_slope_channel_2_command;

  //! \brief ������� ��������� ������� ������
  irs::string ms_set_filter_channel_1_command;
  irs::string ms_set_filter_channel_2_command;

  //! \brief ��������� ������ ������� ������
  irs::string ms_set_level_channel_1_command;
  irs::string ms_set_level_channel_2_command;

  //! \brief ���������� ������������ �� ���������
  akip_ch3_85_3r_t();
public:
  //! \brief �����������
  akip_ch3_85_3r_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  //! \brief ����������
  ~akip_ch3_85_3r_t();
  void connect(hardflow_t* ap_hardflow);
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_dc();
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_ac();
  //! \brief ���������� ������������ ����� �������
  virtual void set_positive();
  //! \brief ���������� ������������� ����� ������
  virtual void set_negative();
  //! \brief ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  //! \brief ������ ����������
  virtual void get_voltage(double *ap_voltage);
  //! \brief ������ ���� ����
  virtual void get_current(double *ap_current);
  //! \brief ������ �������������
  virtual void get_resistance2x(double *ap_resistance);
  //! \brief ������ �������������
  virtual void get_resistance4x(double *ap_resistance);

  //! \brief ������ �������
  virtual void get_frequency(double *ap_frequency);
  //! \brief ������ ������������ ������ ���
  virtual void get_phase_average(double *ap_phase_average);
  //! \brief ������ �������� ������
  virtual void get_phase(double *ap_phase);
  //! \brief ������ ���������� ���������
  virtual void get_time_interval(double *ap_time_interval);
  //! \brief ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);
  //! \brief ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration();
  //! \brief ������ ������� ������� ��������
  virtual meas_status_t status();
  //! \brief ������������ � ��������������� ����������
  virtual void abort();
  //! \brief ������������ ��������
  virtual void tick();
  //! \brief ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double a_nplc);
  //! \brief ��������� ������� ����� � c��
  virtual void set_aperture(double a_aperture);
  //! \brief ��������� ������� � ������
  virtual void set_bandwidth(double a_bandwidth);
  //! \brief ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance);
  //! \brief ��������� ������ ������� ������
  virtual void set_start_level(double a_level);
  //! \brief ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto();
private:
  //! \brief ��������� ���������� ��������� ���������
  void set_time_interval_meas(const double a_time_interval_meas);
};
  // ���������� ����� ��������� ����������� ����������
/*inline void irs::akip_ch3_85_3r_t::set_dc()
  {m_meas_type = DC_MEAS;}
// ���������� ����� ��������� ����������� ����������
inline void irs::akip_ch3_85_3r_t::set_ac()
  {m_meas_type = AC_MEAS;}
*/

class dummy_multimeter_t: public mxmultimeter_t
{
public:
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_dc() {}
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_ac() {}
  //! \brief ���������� ������������ ����� �������
  virtual void set_positive() {}
  //! \brief ���������� ������������� ����� ������
  virtual void set_negative() {}
  //! \brief ������ �������� ��� ������� ���� ���������
  virtual void get_value(double* value) { *value = 100; }//rand(); }
  //! \brief ������ ����������
  virtual void get_voltage(double* voltage) { *voltage = 100; }//rand(); }
  //! \brief ������ ���� ����
  virtual void get_current(double* /*current*/) {}
  //! \brief ������ �������������
  virtual void get_resistance2x(double* /*resistance*/) {}
  //! \brief ������ �������������
  virtual void get_resistance4x(double* resistance)
  {
    static int i = 0;
    //*resistance = abs( rand() );
    *resistance = i++;
  }
  //! \brief ������ �������
  virtual void get_frequency(double* /*frequency*/) {}
  //! \brief ������ ������������ ������ ���
  virtual void get_phase_average(double* /*phase_average*/) {}
  //! \brief ������ �������� ������
  virtual void get_phase(double* /*phase*/) {}
  //! \brief ������ ���������� ���������
  virtual void get_time_interval(double* /*time_interval*/) {}
  //! \brief ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double* /*ap_time_interval*/) {}

  //! \brief ������ �������������� �����������
  virtual void auto_calibration() {}
  //! \brief ������ ������� ������� ��������
  virtual meas_status_t status() { return meas_status_success; }
  //! \brief ���������� ������� ��������
  virtual void abort() {}
  //! \brief ������������ ��������
  virtual void tick() {}
  //! \brief ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double /*nplc*/) {}
  //! \brief ��������� ������� �������������� � c
  virtual void set_aperture(double /*aperture*/) {}
  //! \brief ��������� ������ �������
  virtual void set_bandwidth(double /*bandwidth*/) {}
  //! \brief ��������� �������� ������������� ������
  virtual void set_input_impedance(double /*impedance*/) {}
  //! \brief ��������� ������ ������� ������
  virtual void set_start_level(double /*level*/) {}
  //! \brief ��������� ��������� ���������
  virtual void set_range(type_meas_t /*a_type_meas*/, double /*a_range*/) {}
  //! \brief ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto() {}
  // ��������� �������� ������������� ������
  //virtual void set_filter(double ) {}
};



//! \author Sergeev Sergey
//!
//! \brief ����� ��� ������ � ������������ National Instruments PXI-4071
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
  //! \brief ��� ���������
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
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_dc();
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual void set_ac();
  //! \brief ���������� ������������ ����� �������
  virtual void set_positive();
  //! \brief ���������� ������������� ����� ������
  virtual void set_negative();
  //! \brief ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  //! \brief ������ ����������
  virtual void get_voltage(double *ap_voltage);
  //! \brief ������ ���� ����
  virtual void get_current(double *ap_current);
  //! \brief ������ �������������
  virtual void get_resistance2x(double *ap_resistance);
  //! \brief ������ �������������
  virtual void get_resistance4x(double *ap_resistance);
  //! \brief ������ �������
  virtual void get_frequency(double *ap_frequency);
  //! \brief ������ ������������ ������ ���
  virtual void get_phase_average(double *ap_phase_average);
  //! \brief ������ �������� ������
  virtual void get_phase(double *ap_phase);
  //! \brief ������ ���������� ���������
  virtual void get_time_interval(double *ap_time_interval);
  //! \brief ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);

  //! \brief ������ �������������� �����������
  virtual void auto_calibration();
  //! \brief ������ ������� ������� ��������
  virtual meas_status_t status();
  //! \brief ���������� ������� ��������
  virtual void abort();
  //! \brief ������������ ��������
  virtual void tick();
  //! \brief ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double a_nplc);
  //! \brief ��������� ������� �������������� � c
  virtual void set_aperture(double a_aperture);
  //! \brief ��������� ������ �������
  virtual void set_bandwidth(double /*a_bandwidth*/);
  //! \brief ��������� �������� ������������� ������
  virtual void set_input_impedance(double /*a_impedance*/);
  //! \brief ��������� ������ ������� ������
  virtual void set_start_level(double /*a_level*/);
  //! \brief ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  //! \brief ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto();
  // ��������� �������� ������������� ������
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
    // ���������:
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
    // ����������� ��������:
    conn_data_t<double> meas_value;
    conn_data_t<double> meas_value_not_filtered;
    // ������ �����������:
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

//! \brief ����� ��� ������ � ����������� termex lt-300
//! \details ��������� com_flow_t 4800, FALSE, NOPARITY, 8, ONESTOPBIT,
//!   DTR_CONTROL_ENABLE
class termex_lt_300_t: public mxmultimeter_t
{
public:
  //! \brief �����������
  termex_lt_300_t(hardflow_t* ap_hardflow);
  //! \brief ����������
  ~termex_lt_300_t();
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual inline void set_dc() {}
  //! \brief ���������� ����� ��������� ����������� ����������
  virtual inline void set_ac() {}
  //! \brief ���������� ������������ ����� �������
  virtual void set_positive() {}
  //! \brief ���������� ������������� ����� ������
  virtual void set_negative() {}
  //! \brief ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *ap_value);
  //! \brief ������ ����������
  virtual void get_voltage(double *voltage) {}
  //! \brief ������ ���� ����
  virtual void get_current(double *current) {}
  //! \brief ������ �������������
  virtual void get_resistance2x(double *resistance) {}
  //! \brief ������ �������������
  virtual void get_resistance4x(double *resistance) {}
  //! \brief ������ �������
  virtual void get_frequency(double *frequency) {}
  //! \brief ������ ������������ ������ ���
  virtual void get_phase_average(double *phase_average) {}
  //! \brief ������ �������� ������
  virtual void get_phase(double *phase) {}
  //! \brief ������ ���������� ���������
  virtual void get_time_interval(double *time_interval) {}
  //! \brief ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval) {}
  //! \brief ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration() {}
  //! \brief ������ ������� ������� ��������
  virtual meas_status_t status();
  //! \brief ���������� ������� ��������
  virtual void abort() {}
  //! \brief ������������ ��������
  virtual void tick();
  //! \brief ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double nplc) {}
  //! \brief ��������� ������� �������������� � c
  virtual void set_aperture(double aperture) {}
  //! \brief ��������� ������ �������
  virtual void set_bandwidth(double bandwidth) {}
  //! \brief ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance) {}
  //! \brief ��������� ������ ������� ������
  virtual void set_start_level(double level) {}
  //! \brief ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range) {}
  //! \brief ��������� ��������������� ������ ��������� ���������
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
