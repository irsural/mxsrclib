// ������ ��� ������ � ����������� ����
// ��� Borland C++ Builder
// ������ 0.1

#ifndef meassupH
#define meassupH

// ������ ������ meassup
#define MESSUP_VERSION 0x0001

#include <irsdefs.h>

#include <measdef.h>
#include <mxifa.h>

#include <irsfinal.h>

// ������ ������ ������� GPIB
#define supag_gpib_com_buf_size 30
// ����� ���� ��� irs_gcvt
#define supag_ndig 7

// ����������� ������� ����� ��� ������ � ����������� ����
class mxsupply_t
{
public:
  // ���������� ��� �������������� �������� ��� ���
  virtual irs_bool dc_supported() = 0;
  // ��������� ����
  virtual void set_current(double current) = 0;
  // ��������� ����������
  virtual void set_voltage(double voltage) = 0;
  // ��������� ���������
  virtual void on() = 0;
  // ���������� ���������
  virtual void off() = 0;
  // ������ ������� ������� ��������
  virtual meas_status_t status() = 0;
  // ���������� ������� ��������
  virtual void abort() = 0;
  // ������������ ��������
  virtual void tick() = 0;
};

class mx_agilent_6675a_t: public mxsupply_t
{
  // ��� ��� �������� ������
  typedef enum _mode_t {
    mode_start,
    mode_commands,
    mode_commands_wait,
    mode_stop
  } mode_t;
  // ��� ��� ������� �������
  typedef enum _command_t {
    com_free,
    com_set_voltage,
    com_set_current,
    com_output_on,
    com_output_off
  } command_t;

  // ������� ��� �������������
  static irs_u8 *f_init_commands[];
  // ����� ������ �������������
  static const irs_i32 f_ic_count;

  // ���������� ������ mxifa
  void *f_handle;
  // ������ ��������
  irs_bool f_create_error;
  // ������� ����� ������
  mode_t f_mode;
  // ������ ������� ��������
  meas_status_t f_status;
  // ������� �������
  //command_t f_command;
  // ������ �� ���������� ��������
  irs_bool f_abort_request;
  // ����� ������
  //irs_u8 f_read_buf[ma_read_buf_size];
  // ������� ������� � ������ ������
  //index_t f_read_pos;
  // ��� �������� �������������
  //double f_current;
  // ���������� �������� �������������
  //double f_voltage;

  // ������� GPIB
  irs_u8 f_gpib_command[supag_gpib_com_buf_size];
public:
  // �����������
  mx_agilent_6675a_t(mxifa_ch_t channel,
    gpib_addr_t address);
  // ����������
  ~mx_agilent_6675a_t();
  // ���������� ��� �������������� �������� ��� ���
  virtual irs_bool dc_supported();
  // ��������� ����
  virtual void set_current(double current);
  // ��������� ����������
  virtual void set_voltage(double voltage);
  // ��������� ���������
  virtual void on();
  // ���������� ���������
  virtual void off();
  // ������ ������� ������� ��������
  virtual meas_status_t status();
  // ���������� ������� ��������
  virtual void abort();
  // ������������ ��������
  virtual void tick();
};

#endif
