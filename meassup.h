// ������ ��� ������ � ����������� ����
// ��� Borland C++ Builder
// ������ 0.1

#define UDP_ENABLED

#ifndef meassupH
#define meassupH

// ������ ������ meassup
#define MESSUP_VERSION 0x0001

#include <irsdefs.h>

#include <measdef.h>
#include <mxifa.h>

#include <hardflowg.h>
#include <irsnetdefs.h>
#include <irsmbus.h>


#include <irsfinal.h>
#include <irsdefs.h>

// ������ ������ ������� GPIB
#define supag_gpib_com_buf_size 30
// ����� ���� ��� irs_gcvt
#define supag_ndig 7

struct header_conn_data_t
{
  // ������� (read only)
  irs::conn_data_t<irs_i32> counter;
  irs::conn_data_t<irs_u8> supply_number;
  // ��������� ���� �����:
  irs::bit_data_t ground_rele_bit;
  // ��������� ���� �������:
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
  // ���������������� ����������� ���������� ����������� (write only)
  irs::conn_data_t<float> temp_k;
  // ������������ ����������� ���������� ����������� (write only) 
  irs::conn_data_t<float> temp_ki;
  // ���������������� ����������� ���������� ����������� (write only)
  irs::conn_data_t<float> temp_kd;
  // �������� ������������ � ������� � (write only)
  irs::conn_data_t<float> sense_regA;
  // �������� ������������ � ������� B (write only)
  irs::conn_data_t<float> sense_regB;
  // �������� ��������� ������������� (read only)
  irs::conn_data_t<irs_i32> temp_dac_value;
  // �������� ����������� (read only)
  irs::conn_data_t<irs_i32> int_val;
  // ���������� ������� ������� ����������� (read only)
  irs::conn_data_t<float> temp_time_const;
  // ������������� ����������� (read only)
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

class mx_cs_stab_t: public mxsupply_t
{
  // ��� ��� �������� ������
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

  // ������� ����� ������
  supply_number_t m_supply_number;

  // ������ ������� ��������
  meas_status_t m_status;
  // ������ �� ���������� ��������
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
  //��������������� ��������
  double m_parameter;
  double m_argument;
  bool SR_set;

public:
  // �����������
  mx_cs_stab_t();
  // ����������
  ~mx_cs_stab_t();
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
