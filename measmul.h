// ������ ��� ������ � �������������
// ����: 09.09.2010

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

#include <irsfinal.h>

#if (MXIFA_VERSION < 0x0004)
#error mxifa version incompatible. The version should be >= 0.4.
#endif //(MXIFA_VERSION < 0x0004)

// ������ ������ ������
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

// ����������� ������� ����� ��� ������ � �������������
class mxmultimeter_t
{
public:
  virtual ~mxmultimeter_t() {};
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
  // ���������� ����� ��������� ����������� ����������
  virtual void set_dc() = 0;
  // ���������� ����� ��������� ����������� ����������
  virtual void set_ac() = 0;
  // ���������� ������������ ����� �������
  virtual void set_positive() = 0;
  // ���������� ������������� ����� ������
  virtual void set_negative() = 0;
  // ������ �������� ��� ������� ���� ���������
  virtual void get_value(double *value) = 0;
  // ������ ����������
  virtual void get_voltage(double *voltage) = 0;
  // ������ ���� ����
  virtual void get_current(double *current) = 0;
  // ������ �������������
  virtual void get_resistance2x(double *resistance) = 0;
  // ������ �������������
  virtual void get_resistance4x(double *resistance) = 0;
  // ������ �������
  virtual void get_frequency(double *frequency) = 0;
  // ������ ������������ ������ ���
  virtual void get_phase_average(double *phase_average) = 0;
  // ������ �������� ������
  virtual void get_phase(double *phase) = 0;
  // ������ ���������� ���������
  virtual void get_time_interval(double *time_interval) = 0;
  // ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval) = 0;

  // ������ �������������� �����������
  virtual void auto_calibration() = 0;
  // ������ ������� ������� ��������
  virtual meas_status_t status() = 0;
  // ���������� ������� ��������
  virtual void abort() = 0;
  // ������������ ��������
  virtual void tick() = 0;
  // ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double nplc) = 0;
  // ��������� ������� �������������� � c
  virtual void set_aperture(double aperture) = 0;
  // ��������� ������ �������
  virtual void set_bandwidth(double bandwidth) = 0;
  // ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance) = 0;
  // ��������� ������ ������� ������
  virtual void set_start_level(double level) = 0;
  // ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range) = 0;
  // ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto() = 0;
  // ��������� �������� ������������� ������
  //virtual void set_filter(double ) = 0;
};

// ����� ��� ������ � ������������ Agilent 3458A
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
  //index_t m_time_int_voltage_index;
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


namespace irs{

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

// ����� ��� ������ � ������������ Agilent 3458A � ������ �������������
class agilent_3458a_digitizer_t: public mxmultimeter_t
{
public:
  typedef irs_size_t size_type;
  typedef long double math_value_type;
  // �����������
  agilent_3458a_digitizer_t(
    irs::hardflow_t* ap_hardflow,
    const filter_settings_t& a_filter_settings,
    const double a_sampling_time_s = 25e-6,
    const double a_interval_s = 30.0
  );
  // ����������
  ~agilent_3458a_digitizer_t();
  virtual void get_param(const multimeter_param_t a_param,
    irs::raw_data_t<irs_u8> *ap_value) const;
  virtual void set_param(const multimeter_param_t a_param,
    const irs::raw_data_t<irs_u8> &a_value);
  virtual bool is_param_exists(const multimeter_param_t a_param) const;
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
private:
  void commands_to_buf_send();
  irs::hardflow_t* mp_hardflow;
  filter_settings_t m_filter_settings;
  iir_filter_t<math_value_type> m_iir_filter;
  // ������� ��� �������������
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

// ����� ��� ������ � ������������ �7-78/1
class v7_78_1_t: public mxmultimeter_t
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
  // ��� ��� ��������
  typedef irs_i32 index_t;


  //static const char m_volt_ac_nplcycles = "DETector:BANDwidth 20";

  multimeter_mode_type_t m_mul_mode_type;

  // ������� ��� �������������
  vector<irs::string> f_init_commands;
  /* ������ ������� ��������� ������� �������������� ���
  ����������� ����������*/
  index_t m_nplc_voltage_dc_index;
  // ������ ������� ��������� ������ ������� ��� ����������� ����������
  index_t m_band_width_voltage_ac_index;
  // ������� ������ �������� ��� ������������ ����������
  vector<irs::string> m_get_value_commands;
  // ������� ��� ������ ����������
  vector<irs::string> m_get_voltage_dc_commands;
  vector<irs::string> m_get_voltage_ac_commands;

  // ������ ������� ��������� ������� �������������� ��� �������������
  index_t m_nplc_resistance_2x_index;
  index_t m_nplc_resistance_4x_index;
  // ������� ��� ������ �������������
  vector<irs::string> m_get_resistance_2x_commands;
  vector<irs::string> m_get_resistance_4x_commands;

  // ������ ������� ��������� ������� �������������� ��� ����
  index_t m_nplc_current_dc_index;
  index_t m_nplc_current_ac_index;
  // ������� ��� ������ ����
  vector<irs::string> m_get_current_dc_commands;
  vector<irs::string> m_get_current_ac_commands;

  // ������ ������� ��������� ������� �����
  index_t m_aperture_frequency_index;
  // ������� ��� ������ �������
  vector<irs::string> m_get_frequency_commands;

  // ���������� ������ mxifa
  void *f_handle;
  // ������ ��������
  irs_bool f_create_error;
  // ������� ����� ������
  ma_mode_t f_mode;
  // ������� ���������� ������
  macro_mode_t f_macro_mode;
  // ������ ������� ��������
  meas_status_t f_status;
  // ������� �������
  ma_command_t f_command;
  // ��������� �� ���������� ���������� ��������,
  // ��� ������������ ���� ���������
  double* mp_value;
  // ��������� �� ��������� ���������� ������������
  double* mp_voltage;
  // ��������� �� ��������� ������������� ������������
  double* mp_resistance;
  // ��������� �� ��������� ��� ������������
  double* mp_current;
  // ��������� �� ��������� ������� ������������
  double* mp_frequency;
  // ������ �� ���������� ��������
  irs_bool f_abort_request;
  // ����� ������
  irs_u8 f_read_buf[ma_read_buf_size];
  // ������� ������� � ������ ������
  index_t f_read_pos;
  // ������� �������
  irs::string f_cur_mul_command;
  // ������� ��� �����������
  vector<irs::string> *f_mul_commands;
  // ������ ������ ��� �����������
  index_t f_mul_commands_index;
  // ���������� �������
  //ma_command_t f_command_prev;
  // ��������� �� ���������� � ������� ����� ������� ��������
  double *f_value;
  // ��������� ������ ���������
  irs_bool f_get_parametr_needed;
  // ����� �������� ��������
  counter_t f_oper_time;
  // ������� ��������
  counter_t f_oper_to;
  // ����� �������� ����������
  counter_t f_acal_time;
  // ������� ����������
  counter_t f_acal_to;

  bool m_current_volt_dc_meas;
  // ���������� ������������ �� ���������
  v7_78_1_t();
public:
  // �����������
  v7_78_1_t(
    mxifa_ch_t channel,
    gpib_addr_t address,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // ����������
  ~v7_78_1_t();
  // ���������� ����� ��������� ����������� ����������
  virtual inline void set_dc();
  // ���������� ����� ��������� ����������� ����������
  virtual inline void set_ac();
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
private:
  // ��������� ���������� ��������� ���������
  void set_time_interval_meas(const double a_time_interval_meas);
};
// ���������� ����� ��������� ����������� ����������
inline void irs::v7_78_1_t::set_dc()
{
  m_meas_type = DC_MEAS;
}
// ���������� ����� ��������� ����������� ����������
inline void irs::v7_78_1_t::set_ac()
{
  m_meas_type = AC_MEAS;
}

} // namespace irs

// ����� ��� ������ � ������������ ����������-������� ���� ��-85/3R
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
    // ��������� ���������� ��������
    vector<irs::string> mv_bad_str;
    // ���������� ���������(���� ��� ������������)
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
        // �������� ������ �� �����
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
  // ������ ������� ��������
  command_status_t m_send_command_stat;
  meas_status_t m_meas_stat;

  // ��������� �� ���������������� ����������, � �������
  //����� �������� ���������.
  double* mp_result;
  // ��� ��� ��������
  typedef irs_i32 index_t;
  // ��������� �� ��������� ������� ���������
  vector<irs::string>* mp_last_commands;

  // ������� ��� �������������
  vector<irs::string> mv_init_commands;

  // ������� ��� ������������� �����������
  vector<irs::string> mv_reset_multimetr_commands;

  // ��������� �� ������� �������� �����������
  vector<irs::string*> mv_multimetr_p_commands_setting;

  // ������� ��� ������ �������� ���������� ��������,
  // ��� ������������ ���� ���������
  vector<irs::string> mv_get_value_commands;

  // ������� ��� ������ ����������� �������� ���
  vector<irs::string> mv_get_phase_average_commands;

  // ������� ��� ������ �������� ���
  vector<irs::string> mv_get_phase_commands;

  // ������� ��� ������ ���������� ���������
  vector<irs::string> mv_get_time_interval_commands;

  // ������� ��� ������ ������������ ���������� ���������
  vector<irs::string> mv_get_time_interval_average_commands;

  // ������� ��� ������ �������
  vector<irs::string> mv_get_frequency_commands;

  // �������, ������� �� �������� ����������
  vector<irs::string> mv_big_time_commands;

  // ������� ��������� ������� �����
  irs::string ms_set_interval_command;

  // ������� ��������� �������� ������������� �������
  irs::string ms_set_in_impedance_channel_1_command;
  irs::string ms_set_in_impedance_channel_2_command;

  // ������� ��������� ���� ����� ����� ������
  irs::string ms_set_type_coupling_channel_1_command;
  irs::string ms_set_type_coupling_channel_2_command;

  // ������� ��������� ���� ������ ������� ������
  irs::string ms_set_type_slope_channel_1_command;
  irs::string ms_set_type_slope_channel_2_command;

  // ������� ��������� ������� ������
  irs::string ms_set_filter_channel_1_command;
  irs::string ms_set_filter_channel_2_command;

  // ��������� ������ ������� ������
  irs::string ms_set_level_channel_1_command;
  irs::string ms_set_level_channel_2_command;

  // ���������� ������������ �� ���������
  akip_ch3_85_3r_t();
public:
  // �����������
  akip_ch3_85_3r_t(
    hardflow_t* ap_hardflow,
    multimeter_mode_type_t a_mul_mode_type = mul_mode_type_active);
  // ����������
  ~akip_ch3_85_3r_t();
  void connect(hardflow_t* ap_hardflow);
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
  virtual void get_voltage(double *ap_voltage);
  // ������ ���� ����
  virtual void get_current(double *ap_current);
  // ������ �������������
  virtual void get_resistance2x(double *ap_resistance);
  // ������ �������������
  virtual void get_resistance4x(double *ap_resistance);

  // ������ �������
  virtual void get_frequency(double *ap_frequency);
  // ������ ������������ ������ ���
  virtual void get_phase_average(double *ap_phase_average);
  // ������ �������� ������
  virtual void get_phase(double *ap_phase);
  // ������ ���������� ���������
  virtual void get_time_interval(double *ap_time_interval);
  // ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);
  // ������ �������������� (������� ACAL) �����������
  virtual void auto_calibration();
  // ������ ������� ������� ��������
  virtual meas_status_t status();
  // ������������ � ��������������� ����������
  virtual void abort();
  // ������������ ��������
  virtual void tick();
  // ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double a_nplc);
  // ��������� ������� ����� � c��
  virtual void set_aperture(double a_aperture);
  // ��������� ������� � ������
  virtual void set_bandwidth(double a_bandwidth);
  // ��������� �������� ������������� ������
  virtual void set_input_impedance(double impedance);
  // ��������� ������ ������� ������
  virtual void set_start_level(double a_level);
  // ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto();
private:
  // ��������� ���������� ��������� ���������
  void set_time_interval_meas(const double a_time_interval_meas);
};
  // ���������� ����� ��������� ����������� ����������
/*inline void irs::akip_ch3_85_3r_t::set_dc()
  {m_meas_type = DC_MEAS;}
// ���������� ����� ��������� ����������� ����������
inline void irs::akip_ch3_85_3r_t::set_ac()
  {m_meas_type = AC_MEAS;}
*/
}

class dummy_multimeter_t: public mxmultimeter_t
{
public:
  // ���������� ����� ��������� ����������� ����������
  virtual void set_dc() {}
  // ���������� ����� ��������� ����������� ����������
  virtual void set_ac() {}
  // ���������� ������������ ����� �������
  virtual void set_positive() {}
  // ���������� ������������� ����� ������
  virtual void set_negative() {}
  // ������ �������� ��� ������� ���� ���������
  virtual void get_value(double* /*value*/) {}
  // ������ ����������
  virtual void get_voltage(double* /*voltage*/) {}
  // ������ ���� ����
  virtual void get_current(double* /*current*/) {}
  // ������ �������������
  virtual void get_resistance2x(double* /*resistance*/) {}
  // ������ �������������
  virtual void get_resistance4x(double* /*resistance*/) {}
  // ������ �������
  virtual void get_frequency(double* /*frequency*/) {}
  // ������ ������������ ������ ���
  virtual void get_phase_average(double* /*phase_average*/) {}
  // ������ �������� ������
  virtual void get_phase(double* /*phase*/) {}
  // ������ ���������� ���������
  virtual void get_time_interval(double* /*time_interval*/) {}
  // ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double* /*ap_time_interval*/) {}

  // ������ �������������� �����������
  virtual void auto_calibration() {}
  // ������ ������� ������� ��������
  virtual meas_status_t status() { return meas_status_success; }
  // ���������� ������� ��������
  virtual void abort() {}
  // ������������ ��������
  virtual void tick() {}
  // ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double /*nplc*/) {}
  // ��������� ������� �������������� � c
  virtual void set_aperture(double /*aperture*/) {}
  // ��������� ������ �������
  virtual void set_bandwidth(double /*bandwidth*/) {}
  // ��������� �������� ������������� ������
  virtual void set_input_impedance(double /*impedance*/) {}
  // ��������� ������ ������� ������
  virtual void set_start_level(double /*level*/) {}
  // ��������� ��������� ���������
  virtual void set_range(type_meas_t /*a_type_meas*/, double /*a_range*/) {}
  // ��������� ��������������� ������ ��������� ���������
  virtual void set_range_auto() {}
  // ��������� �������� ������������� ������
  //virtual void set_filter(double ) {}
};

namespace irs {

// ����� ��� ������ � ������������ National Instruments PXI-4071
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
  virtual void get_voltage(double *ap_voltage);
  // ������ ���� ����
  virtual void get_current(double *ap_current);
  // ������ �������������
  virtual void get_resistance2x(double *ap_resistance);
  // ������ �������������
  virtual void get_resistance4x(double *ap_resistance);
  // ������ �������
  virtual void get_frequency(double *ap_frequency);
  // ������ ������������ ������ ���
  virtual void get_phase_average(double *ap_phase_average);
  // ������ �������� ������
  virtual void get_phase(double *ap_phase);
  // ������ ���������� ���������
  virtual void get_time_interval(double *ap_time_interval);
  // ������ ������������ ���������� ���������
  virtual void get_time_interval_average(double *ap_time_interval);

  // ������ �������������� �����������
  virtual void auto_calibration();
  // ������ ������� ������� ��������
  virtual meas_status_t status();
  // ���������� ������� ��������
  virtual void abort();
  // ������������ ��������
  virtual void tick();
  // ��������� ������� �������������� � �������� ������� ���� (20 ��)
  virtual void set_nplc(double a_nplc);
  // ��������� ������� �������������� � c
  virtual void set_aperture(double a_aperture);
  // ��������� ������ �������
  virtual void set_bandwidth(double /*a_bandwidth*/);
  // ��������� �������� ������������� ������
  virtual void set_input_impedance(double /*a_impedance*/);
  // ��������� ������ ������� ������
  virtual void set_start_level(double /*a_level*/);
  // ��������� ��������� ���������
  virtual void set_range(type_meas_t a_type_meas, double a_range);
  // ��������� ��������������� ������ ��������� ���������
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
    get_value_mode,
    stop_mode
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
