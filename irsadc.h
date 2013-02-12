//! \file
//! \ingroup drivers_group
//! \brief Аналоговый ввод-вывод
//!
//! Дата: 16.03.2011

#ifndef irsadcH
#define irsadcH

#include <irsdefs.h>

#include <irsspi.h>
#include <irsstd.h>
#include <mxdata.h>
#include <irsgpio.h>
#include <irsadcabs.h>

#include <irsfinal.h>

namespace irs
{

//! \addtogroup drivers_group
//! @{
//--------------------------  LM95071 ------------------------------------------

class th_lm95071_t : public mxdata_t
{
  enum status_t
  {
    TH_FREE,
    TH_READ,
    TH_RESET
  };
  enum
  {
    m_spi_size = 2,
    m_size = 3,
    m_stop_bit = 1,
    m_new_data_bit = 0,
    m_control_byte = 0
  };
  
  
  status_t m_status;
  spi_t *mp_spi;
  const float m_conv_koef;
  irs_u8 mp_spi_buf[m_spi_size];
  irs_u8 mp_buf[m_size];
  counter_t m_read_counter;
  counter_t m_read_delay;
  bool m_connect;
  irs::timer_t m_wait_timer;
  //  CS
  gpio_pin_t *mp_cs_pin;
public:
  th_lm95071_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, counter_t a_read_delay);
  ~th_lm95071_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
  float get_conv_koef();
};

struct th_lm95071_data_t
{
  irs::bit_data_t new_data_bit;
  irs::bit_data_t stop_bit;
  irs::conn_data_t<irs_i16> temperature_code;
  
  th_lm95071_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    new_data_bit(),
    stop_bit(),
    temperature_code()
  {
    connect(ap_data, a_start_index);    
  }  
  
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    irs_uarc index = a_start_index;
    new_data_bit.connect(ap_data, index, 0);
    stop_bit.connect(ap_data, index, 1);
    index++;
    index = temperature_code.connect(ap_data, index);
  }
};

//--------------------------  AD7791  ------------------------------------------

class adc_ad7791_t : public mxdata_t
{
  static const irs_u8 m_init_sequence_size = 8;
  static const irs_u8 m_reset_sequence_size = 4;
  //
  static const irs_u8 WEN = 7;  //  Write Enable Bit
  static const irs_u8 RS1 = 5;  //  Register Address Bit:
                                //  RS1:RS0 = 00 = Communications Register
                                //  during a Write Operation
  static const irs_u8 RS0 = 4;  //  00 = Status Register during a Read Operation
                                //  01 = Mode Register
                                //  10 = Filter Register
                                //  11 = Data Register
  static const irs_u8 RW = 3;   //  Read/Write Bit: 1 = read
  static const irs_u8 CREAD = 2;//  Continious Read of the Data Register
  static const irs_u8 CH1 = 1;  //  Channel Select
  static const irs_u8 CH0 = 0;  //  CH1:CH0 = 00 = AIN(+) - AIN(-)
                                //            01 = Reserved
                                //            10 = AIN(-) - AIN(-)
                                //            11 = Vdd monitor
  //  Status register AD7791 bits
  static const irs_u8 RDY = 7;  //  Ready bit for ADC.
                                //  Cleared when data is written
                                //  to the ADC data register.
  static const irs_u8 ERR = 6;  //  ADC Error Bit. Set when over/undervoltage
  static const irs_u8 S_CH1 = 1;//  These bits indicate which channel
  static const irs_u8 S_CH0 = 0;//  is being converted by the ADC
  //  Mode register AD7791 bits
  static const irs_u8 MD1 = 7;  //  Mode Select Bits.
  static const irs_u8 MD0 = 6;  //  MD1:MD0 = 00 = Continuous Conversion Mode
                                //            01 = Reserved
                                //            10 = Single Conversion Mode
                                //            11 = Power-Down Mode
  static const irs_u8 BO = 3;   //  Burnout Current Enable Bit.
  static const irs_u8 UB = 2;   //  Unipolar/Bipolar Bit.
  static const irs_u8 BUF = 1;  //  Buffer on / off
  //  Filter Register AD7791
  static const irs_u8 CDIV1 = 5;//  Lower power modes. Clock division
  static const irs_u8 CDIV0 = 4;//  CDIV1:CDIV0 = 00 = Normal Mode
                                //                01 = /2
                                //                10 = /4
                                //                11 = /8
  static const irs_u8 FS2 = 2;  //  These bits set the output rate of the ADC.
  static const irs_u8 FS1 = 1;  //  If the internal clock is divided by 2,
  static const irs_u8 FS0 = 0;  //  the update rates will be divided by 2
                                //  Word  Rate  F-3db N(uV) Rejection
                                //  000   120   28    40    25 dB @ 60 Hz
                                //  001   100   24    25    25 dB @ 50 Hz
                                //  010   33.3  8     3.36            Default__
                                //  011   20    4.7   1.6   80 dB@ 60 Hz       |
                                //  100   16.6  4     1.5   65 dB@ 50 Hz/60Hz <-
                                //  101   16.7  4     1.5   80 dB @ 50 Hz
                                //  110   13.3  3.2   1.2
                                //  111   9.5   2.3   1.1   67 dB @ 50/60 Hz
  enum status_t
  {
    ADC_FREE,
    ADC_REQUEST,
    ADC_READ
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_size = 5;
  static const irs_uarc m_spi_size = 3;
  irs_u8 mp_spi_buf[m_spi_size];
  irs_u8 mp_buf[m_size];
  counter_t m_read_counter;
  counter_t m_read_delay;
  bool m_connect;
  //  CS
  gpio_pin_t *mp_cs_pin;
public:
  adc_ad7791_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, counter_t a_read_delay);
  ~adc_ad7791_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

struct adc_ad7791_data_t
{
  irs::bit_data_t new_data_bit;
  irs::conn_data_t<irs_u32> voltage_code;
  
  adc_ad7791_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    new_data_bit(),
    voltage_code()
  {
    connect(ap_data, a_start_index);    
  }  
  
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    irs_uarc index = a_start_index;
    index = new_data_bit.connect(ap_data, index, 0);
    index++;
    index = voltage_code.connect(ap_data, index);
  }
};

//--------------------------  AD7686  ------------------------------------------

class adc_ad7686_t : public mxdata_t
{
  enum status_t
  {
    ADC_FREE,
    ADC_READ
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_spi_size = 2;
  static const irs_uarc m_size = 3;

  const counter_t m_conv_delay;
  irs_u8 mp_buf[m_size];
  irs_u8 mp_spi_buf[m_spi_size];
  counter_t m_read_counter;
  counter_t m_read_delay;
  bool m_connect;
  //  CS
  gpio_pin_t *mp_cs_pin;
public:
  adc_ad7686_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, counter_t a_read_delay);
  ~adc_ad7686_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

struct adc_ad7686_data_t
{
  irs::bit_data_t new_data_bit;
  irs::conn_data_t<irs_u16> voltage_code;
  
  adc_ad7686_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    new_data_bit(),
    voltage_code()
  {
    connect(ap_data, a_start_index);    
  }  
  
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    irs_uarc index = a_start_index;
    index = new_data_bit.connect(ap_data, index, 0);
    index++;
    index = voltage_code.connect(ap_data, index);
  }
};

//--------------------------  AD8400  ------------------------------------------

class dac_ad8400_t : public mxdata_t
{
  enum status_t
  {
    DAC_FREE,
    DAC_WRITE
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_size = 2;
  static const irs_u8 m_packet_size = 2;
  irs_u8 mp_buf[m_size];
  irs_u8 m_init_value;
  bool m_need_write;
  //  CS
  gpio_pin_t *mp_cs_pin;
public:
  dac_ad8400_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, irs_u8 a_init_value);
  ~dac_ad8400_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

struct dac_ad8400_data_t
{
  irs::bit_data_t ready_bit;
  irs::conn_data_t<irs_u8> resistance_code;
  
  dac_ad8400_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    ready_bit(),
    resistance_code()
  {
    connect(ap_data, a_start_index);    
  }  
    
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    irs_uarc index = a_start_index;
    index = ready_bit.connect(ap_data, index, 0);
    index++;
    index = resistance_code.connect(ap_data, index);
  }
};

//--------------------------  AD7376  ------------------------------------------
// Цифровой потенциометр 8 бит

class dac_ad7376_t : public mxdata_t
{
  enum status_t
  {
    DAC_FREE,
    DAC_WRITE
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_size = 2;
  static const irs_u8 m_packet_size = 1;
  static const irs_u8 m_ready_bit_position = 0;
  static const irs_u8 m_rs_bit_position = 1;
  static const irs_u8 m_shdn_bit_position = 2;
  irs_u8 mp_buf[m_size];
  irs_u8 m_init_value;
  bool m_need_write;
  bool m_need_reset;
  bool m_need_shdn;
  //  CS
  gpio_pin_t *mp_cs_pin;
  gpio_pin_t *mp_rs_pin;
  gpio_pin_t *mp_shdn_pin;
  irs::timer_t timerdelay;
public:
  dac_ad7376_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, gpio_pin_t *ap_rs_pin,
    gpio_pin_t *ap_shdn_pin, irs_u8 a_init_value);
  ~dac_ad7376_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

struct dac_ad7376_data_t
{
  irs::bit_data_t ready_bit;
  irs::bit_data_t rs_bit;
  irs::bit_data_t shdn_bit;
  irs::conn_data_t<irs_u8> resistance_code;
  
  dac_ad7376_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    ready_bit(),
    rs_bit(),
    shdn_bit(),
    resistance_code()
  {
    connect(ap_data, a_start_index);    
  }  
  
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    ready_bit.connect(ap_data, a_start_index, 0);
    rs_bit.connect(ap_data, a_start_index, 1);
    shdn_bit.connect(ap_data, a_start_index, 2);
    resistance_code.connect(ap_data, a_start_index+1);
  }
};

//--------------------------  MAX551  ------------------------------------------

class dac_max551_t : public mxdata_t
{
  enum status_t
  {
    DAC_FREE,
    DAC_WRITE
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_size = 3;
  static const irs_u8 m_packet_size = 2;
  irs_u8 mp_buf[m_size];
  irs_u16 m_init_value;
  bool m_need_write;
  bool m_need_reset;
  //  CS
  gpio_pin_t *mp_cs_pin;
public:
  dac_max551_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, irs_u16 a_init_value);
  ~dac_max551_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

struct dac_max551_data_t
{
  irs::bit_data_t ready_bit;
  irs::conn_data_t<irs_u16> voltage_code;
  
  dac_max551_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    ready_bit(),
    voltage_code()
  {
    connect(ap_data, a_start_index);    
  }  
  
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    ready_bit.connect(ap_data, a_start_index, 0);
    voltage_code.connect(ap_data, a_start_index+1);
  }
};

//--------------------------  AD9854  ------------------------------------------

struct dds_ad9854_data_t
{
  irs::bit_data_t ready_status_bit;
  irs::bit_data_t reset_bit;
  //
  irs::conn_data_t<irs_u32> phase_offset_code_1;
  irs::conn_data_t<irs_u32> phase_offset_code_2;
  irs::conn_data_t<irs_u64> frequency_code_1;
  irs::conn_data_t<irs_u64> frequency_code_2;
  irs::conn_data_t<irs_u64> delta_frequency_code;
  irs::conn_data_t<irs_u32> update_clock_rate_code;
  irs::conn_data_t<irs_u32> ramp_rate_clock_code;
  //  Control register bits
  irs::conn_data_t<irs_u32> control_register;
  irs::bit_data_t sdo_active_bit;
  irs::bit_data_t data_order_bit;
  irs::bit_data_t shaped_keying_control_bit;
  irs::bit_data_t shaped_keying_enable_bit;
  irs::bit_data_t internal_sinc_bypass_bit;
  irs::bit_data_t intud_active_bit;
  irs::bit_data_t mode_bit_0;
  irs::bit_data_t mode_bit_1;
  irs::bit_data_t mode_bit_2;
  irs::bit_data_t source_q_dac_bit;
  irs::bit_data_t triangle_bit;
  irs::bit_data_t clear_accum_1_2_bit;
  irs::bit_data_t clear_accum_1_bit;
  irs::conn_data_t<irs_u8> pll_multiplier_code;
  irs::bit_data_t pll_bypass_bit;
  irs::bit_data_t pll_range_bit;
  irs::bit_data_t digital_power_down_bit;
  irs::bit_data_t i_q_dac_power_down_bit;
  irs::bit_data_t q_dac_power_down_bit;
  irs::bit_data_t wres_bit;
  irs::bit_data_t comparator_power_down_bit;
  //
  irs::conn_data_t<irs_u16> i_path_mult_code;
  irs::conn_data_t<irs_u16> q_path_mult_code;
  irs::conn_data_t<irs_u8>  shaped_keying_ramp_rate_code;
  irs::conn_data_t<irs_u16> q_dac_voltage_code;
  
  dds_ad9854_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    ready_status_bit(),
    reset_bit(),
    //
    phase_offset_code_1(),
    phase_offset_code_2(),
    frequency_code_1(),
    frequency_code_2(),
    delta_frequency_code(),
    update_clock_rate_code(),
    ramp_rate_clock_code(),
    control_register(),
    //  Control register bits
    sdo_active_bit(),
    data_order_bit(),
    shaped_keying_control_bit(),
    shaped_keying_enable_bit(),
    internal_sinc_bypass_bit(),
    intud_active_bit(),
    mode_bit_0(),
    mode_bit_1(),
    mode_bit_2(),
    source_q_dac_bit(),
    triangle_bit(),
    clear_accum_1_2_bit(),
    clear_accum_1_bit(),
    pll_multiplier_code(),
    pll_bypass_bit(),
    pll_range_bit(),
    digital_power_down_bit(),
    i_q_dac_power_down_bit(),
    q_dac_power_down_bit(),
    wres_bit(),
    comparator_power_down_bit(),
    //
    i_path_mult_code(),
    q_path_mult_code(),
    shaped_keying_ramp_rate_code(),
    q_dac_voltage_code()
  {
    connect(ap_data, a_start_index);    
  }  
  
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    irs_uarc index = a_start_index;
    ready_status_bit.connect(ap_data, index, 0);
    reset_bit.connect(ap_data, index, 7);
    index++;
    index = phase_offset_code_1.connect(ap_data, index);
    index = phase_offset_code_2.connect(ap_data, index);
    index = frequency_code_1.connect(ap_data, index);
    index = frequency_code_2.connect(ap_data, index);
    index = delta_frequency_code.connect(ap_data, index);
    index = update_clock_rate_code.connect(ap_data, index);
    index = ramp_rate_clock_code.connect(ap_data, index);
    control_register.connect(ap_data, index);
    //  Control register bits
    sdo_active_bit.connect(ap_data, index, 0);
    data_order_bit.connect(ap_data, index, 1);
    shaped_keying_control_bit.connect(ap_data, index, 4);
    shaped_keying_enable_bit.connect(ap_data, index, 5);
    internal_sinc_bypass_bit.connect(ap_data, index, 6);
    index++;
    intud_active_bit.connect(ap_data, index, 0);
    mode_bit_0.connect(ap_data, index, 1);
    mode_bit_1.connect(ap_data, index, 2);
    mode_bit_2.connect(ap_data, index, 3);
    source_q_dac_bit.connect(ap_data, index, 4);
    triangle_bit.connect(ap_data, index, 5);
    clear_accum_1_2_bit.connect(ap_data, index, 6);
    clear_accum_1_bit.connect(ap_data, index, 7);
    index = pll_multiplier_code.connect(ap_data, index);
    pll_bypass_bit.connect(ap_data, index, 5);
    pll_range_bit.connect(ap_data, index, 6);
    index++;
    digital_power_down_bit.connect(ap_data, index, 0);
    i_q_dac_power_down_bit.connect(ap_data, index, 1);
    q_dac_power_down_bit.connect(ap_data, index, 2);
    wres_bit.connect(ap_data, index, 3);
    comparator_power_down_bit.connect(ap_data, index, 4);
    //
    index++;
    index = i_path_mult_code.connect(ap_data, index);
    index = q_path_mult_code.connect(ap_data, index);
    index = shaped_keying_ramp_rate_code.connect(ap_data, index);
    index = q_dac_voltage_code.connect(ap_data, index);
  }
};

/*-------------------------- структура данных ----------------------------------
  Num   Description
   0 -- Status (1 byte)
   1 -- Phase Offset Tuning Word Register 1 (4 bytes)
   3 -- Phase Offset Tuning Word Register 2 (4 bytes)
   5 -- Frequency Tuning Word 1 (8 bytes)
  13 -- Frequency Tuning Word 2 (8 bytes)
  21 -- Delta Frequency Register (8 bytes)
  29 -- Update Clock Rate Register (4 bytes)
  33 -- Ramp Rate Clock Register (4 bytes)
  37 -- Control Register (4 bytes)
  41 -- I Path Digital Multiplier Register (2 bytes)
  43 -- Q Path Digital Multiplier Register (2 bytes)
  45 -- Shaped On/Off Keying Ramp Rate Reg (1 byte)
  46 -- Q DAC Register (2 bytes)
                                            size = 48 bytes
------------------------------------------------------------------------------*/

class dds_ad9854_t : public mxdata_t
{
  //  DDS Addresses
  static const irs_u8 ADDR_PH1 = 0;     //  Phase Offset Tuning Word Register 1
  static const irs_u8 ADDR_PH2 = 1;     //  Phase Offset Tuning Word Register 2
  static const irs_u8 ADDR_FREQ1 = 2;   //  Frequency Tuning Word 1
  static const irs_u8 ADDR_FREQ2 = 3;   //  Frequency Tuning Word 2
  static const irs_u8 ADDR_DFREQ = 4;   //  Delta Frequency Register
  static const irs_u8 ADDR_INTUD = 5;   //  Update Clock Rate Register
  static const irs_u8 ADDR_RRCR = 6;    //  Ramp Rate Clock Register
  static const irs_u8 ADDR_CR = 7;      //  Control Register
  static const irs_u8 ADDR_IPATH = 8;   //  I Path Digital Multiplier Register
  static const irs_u8 ADDR_QPATH = 9;   //  Q Path Digital Multiplier Register
  static const irs_u8 ADDR_SKRR = 10;   //  Shaped On/Off Keying Ramp Rate Reg
  static const irs_u8 ADDR_QDAC = 11;   //  Q DAC Register
  //  DDS Sizes
  static const irs_u8 SZ_DDS_PH1 = 2;   //  Phase Offset Tuning Word Register 1
  static const irs_u8 SZ_DDS_PH2 = 2;   //  Phase Offset Tuning Word Register 2
  static const irs_u8 SZ_DDS_FREQ1 = 6; //  Frequency Tuning Word 1
  static const irs_u8 SZ_DDS_FREQ2 = 6; //  Frequency Tuning Word 2
  static const irs_u8 SZ_DDS_DFREQ = 6; //  Delta Frequency Register
  static const irs_u8 SZ_DDS_INTUD = 4; //  Update Clock Rate Register
  static const irs_u8 SZ_DDS_RRCR = 3;  //  Ramp Rate Clock Register
  static const irs_u8 SZ_DDS_CR = 4;    //  Control Register
  static const irs_u8 SZ_DDS_IPATH = 2; //  I Path Digital Multiplier Register
  static const irs_u8 SZ_DDS_QPATH = 2; //  Q Path Digital Multiplier Register
  static const irs_u8 SZ_DDS_SKRR = 1;  //  Shaped On/Off Keying Ramp Rate Reg
  static const irs_u8 SZ_DDS_QDAC = 2;  //  Q DAC Register
  //  DDS mxdata sizes
  static const irs_u8 SZ_PH1 = 2;       //  Phase Offset Tuning Word Register 1
  static const irs_u8 SZ_PH2 = 2;       //  Phase Offset Tuning Word Register 2
  static const irs_u8 SZ_FREQ1 = 8;     //  Frequency Tuning Word 1
  static const irs_u8 SZ_FREQ2 = 8;     //  Frequency Tuning Word 2
  static const irs_u8 SZ_DFREQ = 8;     //  Delta Frequency Register
  static const irs_u8 SZ_INTUD = 4;     //  Update Clock Rate Register
  static const irs_u8 SZ_RRCR = 4;      //  Ramp Rate Clock Register
  static const irs_u8 SZ_CR = 4;        //  Control Register
  static const irs_u8 SZ_IPATH = 2;     //  I Path Digital Multiplier Register
  static const irs_u8 SZ_QPATH = 2;     //  Q Path Digital Multiplier Register
  static const irs_u8 SZ_SKRR = 1;      //  Shaped On/Off Keying Ramp Rate Reg
  static const irs_u8 SZ_QDAC = 2;      //  Q DAC Register
  //  DDS mxdata positions
  static const irs_u8 POS_STATUS = 0;
  static const irs_u8 RESET_BIT = 7;
  //
  static const irs_u8 POS_PH1 = 1;      //  Phase Offset Tuning Word Register 1
  static const irs_u8 POS_PH2 = 3;      //  Phase Offset Tuning Word Register 2
  static const irs_u8 POS_FREQ1 = 5;    //  Frequency Tuning Word 1
  static const irs_u8 POS_FREQ2 = 13;   //  Frequency Tuning Word 2
  static const irs_u8 POS_DFREQ = 21;   //  Delta Frequency Register
  static const irs_u8 POS_INTUD = 29;   //  Update Clock Rate Register
  static const irs_u8 POS_RRCR = 33;    //  Ramp Rate Clock Register
  static const irs_u8 POS_CR = 37;      //  Control Register
  static const irs_u8 POS_PLL_MUL = 39; //  PLL Multiplier 5 bits
  static const irs_u8 POS_IPATH = 41;   //  I Path Digital Multiplier Register
  static const irs_u8 POS_QPATH = 43;   //  Q Path Digital Multiplier Register
  static const irs_u8 POS_SKRR = 45;    //  Shaped On/Off Keying Ramp Rate Reg
  static const irs_u8 POS_QDAC = 46;    //  Q DAC Register
  //  DDS Control Register Bits
  static const irs_u8 SDO = 0;    //  Serial port SDO active bit,
                                  //  default low, inactive
  static const irs_u8 DATAORD = 1;//  Serial port MSB/LSB first bit.
                                  //  Defaults low, MSB first int/ext
  static const irs_u8 SKC = 4;    //  output shaped-keying control bit
  static const irs_u8 SKE = 5;    //  Shaped-keying enable bit. When this bit
                                  //  is set, the output ramping
                                  //  function is enabled and is performed in
                                  //  accordance with the SKC bit requirements
  static const irs_u8 ISINCE = 6; //  Inverse sinc filter bypass, 1 = enable
  static const irs_u8 INTUD = 8;  //  Internal update active bit, 1 = internal
  static const irs_u8 MODE0 = 9;  //  Mode of operation:  MODE[2:0] =
  static const irs_u8 MODE1 = 10; //                   000 = Single-Tone mode
  static const irs_u8 MODE2 = 11; //                   001 = FSK mode
                                  //                   010 = Ramped FSK mode
                                  //                   011 = Chirp mode
                                  //                   100 = BPSK mode
  static const irs_u8 QSRC = 12;  //  Source Q DAC bit. When this bit is set
                                  //  high, the Q path DAC accepts data from
                                  //  the Q DAC register
  static const irs_u8 TRI = 13;   //  Triangle bit.
                                  //  When this bit is set, the AD9854
                                  //  automatically performs a continuous
                                  //  frequency sweep from F1 to F2
                                  //  frequencies and back.
  static const irs_u8 CLA12 = 14; //  Clear accumulator bit.
                                  //  This bit, active high, holds both the
                                  //  Accumulator 1 and Accumulator 2 values
                                  //  at zero for as long as the bit is active.
                                  //  This allows the DDS phase to be
                                  //  initialized via the I/O port.
  static const irs_u8 CLA1 = 15;    //  Clear Accumulator 1 bit.
  static const irs_u8 PLLMUL0 = 16; //  PLL multiplier factor
  static const irs_u8 PLLMUL1 = 17;
  static const irs_u8 PLLMUL2 = 18;
  static const irs_u8 PLLMUL3 = 19;
  static const irs_u8 PLLMUL4 = 20;
  static const irs_u8 PLLBYPS = 21; //  Bypass PLL bit, active high.
                                    //  When this bit is active, the PLL is
                                    //  powered down and the REFCLK
                                    //  input is used to drive the system
                                    //  clock signal. The power-up state of
                                    //  the bypass PLL bit is Logic 1,
                                    //  PLL bypassed.
  static const irs_u8 PLLRNG = 22;  //  PLL range bit, which controls
                                    //  the VCO gain. The power-up state of
                                    //  the PLL range bit is Logic 1, higher
                                    //  gain for frequencies above 200 MHz.
  static const irs_u8 DPD = 24;     //  Digital power-down bit. 1 = power-down
  static const irs_u8 IQPD = 25;    //  Full DAC power-down bit. 1 = power-down
  static const irs_u8 QPD = 26;     //  Q DAC power-down bit. 1 = power-down
  static const irs_u8 WRES = 27;    //  Must always be written to Logic 0.
                                    //  Writing this bit to Logic 1 causes
                                    //  the AD9854 to stop working until a
                                    //  master reset is applied.
  static const irs_u8 CPD = 28;     //  Comparator power-down bit. 1=power-down
  enum status_t
  {
    DDS_FREE,
    DDS_WRITE,
    DDS_UPDATE,
    DDS_UPDATE_WAIT
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_size = 48;
  static const irs_u8 m_num_of_registers = 12;
  static const irs_u8 m_dds_size = 40;
  static const irs_u8 m_max_write_bytes = 8 + 1;
  irs_u8 mp_buf[m_size];
  irs_u8 mp_reg_size[m_num_of_registers];
  irs_u8 mp_mxdata_reg_size[m_num_of_registers];
  irs_u8 mp_reg_position[m_num_of_registers];
  vector<bool> m_write_vector;
  irs_u8 m_current_write_index;
  bool m_all_write;
  bool m_need_write_cr;
  //  CS
  gpio_pin_t *mp_cs_pin;
  gpio_pin_t *mp_reset_pin;
  gpio_pin_t *mp_update_pin;
  //
  counter_t m_refresh_counter;
  counter_t m_refresh_time;
  //
  irs_u8 m_first_byte;
  //
  void reset();
  counter_t m_update_time;
public:
  dds_ad9854_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, gpio_pin_t *ap_reset_pin,
    gpio_pin_t *ap_update_pin);
  ~dds_ad9854_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

//-------------------------- LTC2622 -------------------------------------------
// 12-битный ЦАП
class dac_ltc2622_t : public mxdata_t
{
  enum status_t
  {
    DAC_FREE,
    DAC_WRITE_REGA,
    DAC_WRITE_REGB,
    DAC_DALAY
  };
  enum
  {
    m_size = 5,
    m_data_regA_position = 1,
    m_data_regB_position = 3, 
    m_data_reg_size = 2,
    m_packet_size = 2,
    m_write_buf_size = 3,
    m_data_bytes_size = 2,
    m_ready_bit_regA = 0,
    m_ready_bit_regB = 1,
    m_log_enable_pos = 7
  };
  enum 
  {
    m_com_write_to_input_register = 0x00,
    m_com_write_to_input_register_and_update_all = 0x40,
    m_com_write_to_input_register_and_update = 0x30,
    m_com_no_operation = 0xF0 
  };
  enum 
  {
    m_addr_DACA = 0x00,
    m_addr_DACB = 0x01,
    m_addr_all_DACs = 0x0F
  };
  
  inline irs_u8 bt(irs_u8 a_enum)
  {
    return a_enum;
  }
  inline irs_u8* to_pu8(irs_u16 &a_reg)
  {
    return reinterpret_cast<irs_u8*> (a_reg);
  }  
 
  status_t m_status;
  
  spi_t *mp_spi;
  
  irs_u8 mp_buf[m_size];
  irs_u8 mp_write_buf[m_write_buf_size];
  irs_u8& m_command;
   
  irs_u16& m_regA;
  irs_u16& m_regB;
  irs_u16& m_write_reg;
  
  irs::timer_t m_wait_timer;
  
  bool m_need_write;
  
  gpio_pin_t *mp_cs_pin;
  
public:
  dac_ltc2622_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, irs_u16 a_init_regA,
    irs_u16 a_init_regB);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void tick();
  
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
};

struct dac_ltc2622_data_t
{
  irs::bit_data_t ready_bit_A;
  irs::bit_data_t ready_bit_B;
  irs::bit_data_t log_enable;
  irs::conn_data_t<irs_u16> voltage_code_A;
  irs::conn_data_t<irs_u16> voltage_code_B;
  
  dac_ltc2622_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    ready_bit_A(),
    ready_bit_B(),
    log_enable(),
    voltage_code_A(),
    voltage_code_B()
  {
    connect(ap_data, a_start_index);
  }  
   
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    ready_bit_A.connect(ap_data, a_start_index, 0);
    ready_bit_B.connect(ap_data, a_start_index, 1);
    log_enable.connect(ap_data, a_start_index, 7);
    a_start_index++;
    a_start_index = voltage_code_A.connect(ap_data, a_start_index);
    a_start_index = voltage_code_B.connect(ap_data, a_start_index);
  }
};

//--------------------------  AD102S021  ---------------------------------------

class adc_adc102s021_t : public mxdata_t
{
  private:
  enum status_t
  {
    ADC_FREE,
    ADC_READ,
    ADC_READ_WAIT
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_size = 2;
  static const irs_uarc m_target_size = 4;
  static const irs_u8 ADD1 = 4; //ADD1
  static const irs_u8 ADD0 = 3; //ADD1
  static const irs_u8 Z5 = 7; //Zero
  static const irs_u8 Z4 = 6; //Zero
  static const irs_u8 Z3 = 5; //Z3
  static const irs_u8 Z2 = 4; //Z2
  static const irs_u8 Z1 = 3; //Z1
  static const irs_u8 Z0 = 2; //Z0
  irs::timer_t m_timer;
  irs_u8 mp_target_buf[m_target_size];
  irs_u8 mp_write_buf[m_size];
  irs_u8 mp_read_buf[m_size];
  bool m_connect;
  bool m_in_first_enable;
  bool m_round;
  //  CS
  gpio_pin_t *mp_cs_pin;
  public:
  adc_adc102s021_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, counter_t a_read_delay);
  ~adc_adc102s021_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

struct adc_adc102s021_data_t
{
  irs::conn_data_t<irs_u16> voltage_code_A;
  irs::conn_data_t<irs_u16> voltage_code_B;

  
  adc_adc102s021_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    voltage_code_A(),
    voltage_code_B()
  {
    connect(ap_data, a_start_index);    
  }  
  
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    irs_uarc index = a_start_index;
    index = voltage_code_A.connect(ap_data, index);
    index = voltage_code_B.connect(ap_data, index);
  }
};

//--------------------------  AD5293  ------------------------------------------

class dac_ad5293_t : public mxdata_t
{
  enum status_t
  {
    #ifdef NOP
    DAC_RESET,
    DAC_FREE,
    DAC_START,
    DAC_DATA,
    DAC_DATA_WAIT,
    DAC_SDO,
    DAC_SDO_WAIT,
    DAC_NOP,
    DAC_WRITE,
    #endif //NOP
    //
    FREE,
    RESET,
    UNLOCK,
    RESISTANCE,
    HIZ,
    ZERO
  };
  enum write_status_t
  {
    WS_READY,
    WS_DOWN_CS,
    WS_WRITE,
    WS_UP_CS,
    WS_PAUSE
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_size = 2;
  irs_u8 mp_buf[m_size];
  irs_u8 mp_write_buf[m_size];
  irs::timer_t m_wait_timer;
  static const irs_u8 Z5 = 7; //C3
  static const irs_u8 Z4 = 6; //C3
  static const irs_u8 C3 = 5; //C3
  static const irs_u8 C2 = 4; //C2  
  static const irs_u8 C1 = 3; //C1
  static const irs_u8 C0 = 2; //C0
  static const irs_u8 Control2 = 2; //Calibration enable
  static const irs_u8 Control1 = 1; //RDAC register write protect.
  static const irs_u8 HI = 0;//High impedance
  /*static const irs_u8 ZERO0_bit = 6; //не используемый бит
  static const irs_u8 ZERO1_bit = 7; //не используемый бит*/
  bool m_need_write;
  //  CS
  gpio_pin_t *mp_cs_pin;
  write_status_t m_write_status;

  bool write_to_dac(status_t a_command);
  inline bool write_ready() { return (m_write_status == WS_READY); };
  inline bool spi_ready() 
  { 
    return (!mp_spi->get_lock() && mp_spi->get_status() == irs::spi_t::FREE); 
  };
  void write_tick();
public:
  dac_ad5293_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin);
  ~dac_ad5293_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
}; // dac_ad5293_t

struct dac_ad5293_data_t
{
  irs::conn_data_t<irs_u16> resistance_code;
  
  dac_ad5293_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    resistance_code()
  {
    connect(ap_data, a_start_index);    
  }  
    
  void connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    irs_uarc index = a_start_index;
    index = resistance_code.connect(ap_data, index);
  }
};

//--------------------------- AD5791 -------------------------------------------
// 20-bit DAC
class dac_ad5791_t : public mxdata_t
{
public:
  dac_ad5791_t(
    spi_t *ap_spi,
    gpio_pin_t *ap_cs_pin,
    gpio_pin_t *ap_ldac_pin,
    gpio_pin_t *ap_clr_pin,
    gpio_pin_t *ap_reset_pin,
    irs_u32 a_default_value);
  
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
private:
  enum status_t {
    st_reset,
    st_prepare_options,
    st_prepare_voltage_code,
    st_spi_prepare,
    st_spi_wait,
    st_ldac_clear,
    st_ldac_set,
    st_free
  };
  enum {
    m_size = 7,
    m_status_pos = 0,
    m_options_pos = 1,
    m_lin_comp_pos = 2,
    m_voltage_code_pos = 3,
    m_write_buf_size = 3,
    m_ready_bit_pos = 0,
    m_rbuf_bit_pos = 1,
    m_opgnd_bit_pos = 2,
    m_dactri_bit_pos = 3,
    m_bin2sc_bit_pos = 4,
    m_sdodis_bit_pos = 5,
    m_master_byte_shift = 28,
    m_mid_byte_shift = 20,
    m_least_byte_shift = 12
  };
  enum {
    addr_code = 0x01 << 4,
    addr_control = 0x02 << 4,
    addr_clearcode = 0x03 << 4,
    addr_softcontrol = 0x04 << 4
  };
  enum {
    m_reset_interval = 1  //  ms
  };
 
  status_t m_status;
  status_t m_target_status;
  spi_t* mp_spi;
  irs_u8 mp_buf[m_size];
  irs_u8 mp_write_buf[m_write_buf_size];
  irs::timer_t m_timer;
  bool m_need_write_options;
  bool m_need_write_voltage_code;
  gpio_pin_t* mp_cs_pin;
  gpio_pin_t* mp_ldac_pin;
  gpio_pin_t* mp_clr_pin;
  gpio_pin_t* mp_reset_pin;
};

struct dac_ad5791_data_t
{
  irs::bit_data_t ready_bit;
  irs::bit_data_t rbuf_bit;
  irs::bit_data_t opgnd_bit;
  irs::bit_data_t dactri_bit;
  irs::bit_data_t bin2sc_bit;
  irs::bit_data_t sdodis_bit;
  irs::conn_data_t<irs_u8> lin_comp;
  irs::conn_data_t<irs_i32> signed_voltage_code;
  irs::conn_data_t<irs_u32> unsigned_voltage_code;
  
  dac_ad5791_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    ready_bit(),
    rbuf_bit(),
    opgnd_bit(),
    dactri_bit(),
    bin2sc_bit(),
    sdodis_bit(),
    lin_comp(),
    signed_voltage_code(),
    unsigned_voltage_code()
  {
    connect(ap_data, a_start_index);
  }  
   
  irs_uarc connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    ready_bit.connect(ap_data, a_start_index, 0);
    a_start_index++;
    rbuf_bit.connect(ap_data, a_start_index, 1);
    opgnd_bit.connect(ap_data, a_start_index, 2);
    dactri_bit.connect(ap_data, a_start_index, 3);
    bin2sc_bit.connect(ap_data, a_start_index, 4);
    sdodis_bit.connect(ap_data, a_start_index, 5);
    a_start_index++;
    a_start_index = lin_comp.connect(ap_data, a_start_index);
    signed_voltage_code.connect(ap_data, a_start_index);
    a_start_index = unsigned_voltage_code.connect(ap_data, a_start_index);
    return a_start_index;
  }
};

//-------------------------- AD7799 -------------------------------------------
// 24-bit 3-channel ADC
class adc_ad7799_t: public adc_request_t
{
public:
  adc_ad7799_t(spi_t *ap_spi,
  gpio_pin_t* ap_cs_pin);
  ~adc_ad7799_t();
  virtual void start();
  virtual meas_status_t status() const; 
  virtual void set_channel(irs_u8 a_channel);
  virtual void set_mode(irs_u8 a_mode);
  virtual void set_freq(irs_u8 a_freq);
  virtual void set_gain(irs_u8 a_gain);
  virtual irs_u32 get_value() const;
  virtual void tick();

private:
  enum mode_t {
    mode_free,
    mode_spi,
    mode_spi_rw,
    mode_spi_rw_wait,
    mode_read_all_reg,
    mode_read
  };
  enum {
    m_size_buf = 16,
    m_write_buf_size = 4  
  };
  enum {
    m_gain_byte_pos = 1,
    m_freq_byte_pos = 0,
    m_ch_byte_pos = 0,
    m_mode_byte_pos = 1
  };
  enum {
    m_rw_pos = 6,
    m_rs_pos = 3,
    m_gain_pos = 0,
    m_freq_pos = 0,
    m_ch_pos = 0,
    m_mode_pos = 5,
    ready_pos = 7
  };
  enum reg_index_t {
    m_reg_comm_index = 0,
    m_reg_status_index = 0,
    m_reg_mode_index = 1,
    m_reg_conf_index = 2,
    m_reg_data_index = 3,
    m_reg_id_index = 4,
    m_reg_io_index = 5,
    m_reg_offs_index = 6,
    m_reg_fs_index = 7
  };
  enum reg_size_t {
    m_reg_comm_size = 1,
    m_reg_status_size = 1,
    m_reg_mode_size = 2,
    m_reg_conf_size = 2,
    m_reg_data_size = 3,
    m_reg_id_size = 1,
    m_reg_io_size = 1,
    m_reg_offs_size = 3,
    m_reg_fs_size = 3
  };
  enum transaction_type_t {
    tt_read,
    tt_write
  };
  struct reg_t {
    reg_t(reg_index_t a_index, reg_size_t a_size):
      index(a_index),
      size(a_size)
    {}
    ~reg_t() {};
    reg_index_t index;
    reg_size_t size;
  };
  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
  meas_status_t m_status;
  mode_t m_mode;
  irs_u8 mp_buf[m_size_buf];
  irs_u8 mp_spi_buf[m_write_buf_size];
  size_t m_spi_transaction_size;
  vector<reg_t> m_reg;
  bool m_read_all_reg;
  irs_u32 m_count_init;
  irs_u32 m_shift;
  bool m_read_data;
  vector<int> m_filter_settle_time_vector;
  vector<float> m_conv_time_vector;
  irs_u8 m_freq;
  int m_reserved_interval;
  timer_t m_timer;
  bool m_ready_wait;
  bool m_get_data;
  irs_u32 m_value;
  bool m_ready;
  irs_u8 m_read_mode;
  irs_u8 mp_get_buff[4];
 
  void spi_prepare();
  void spi_release();
  void creation_reg_comm(reg_t a_reg, transaction_type_t a_tt);
  int calculation_shift(reg_t a_reg);
  int calculation_number_byte(reg_t a_reg, int byte_pos);
};
/*class adc_ad7799_t : public mxdata_t
{
public:
  adc_ad7799_t(
    spi_t *ap_spi,
    gpio_pin_t *ap_cs_pin);
  
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
private:
  enum status_t {
    st_reset,
    st_initial_read_reg,
    st_initial_copy_reg,
    st_spi_prepare,
    st_spi_wait,
    st_prepare_write_reg,
    st_pwr_down,
    st_cont_conv,
    st_single_conv
  };
  enum {
    m_size = 21,
    // m_buf_status_pos = 0,
    
    // m_buf_mode_pos = 1,
    // m_buf_mode_size = 2,
    // m_buf_mode_filt_pos = 1,
    // m_buf_mode_mode_pos = 2,
    
    // m_buf_conf_pos = 3,
    // m_buf_conf_size = 3,
    // m_buf_conf_gain_pos = 3,
    // m_buf_conf_chan_pos = 4,
    // m_buf_conf_bits_pos = 5,
    // m_buf_conf_buf_bit_pos = 0,
    // m_buf_conf_refdet_bit_pos = 1,
    // m_buf_conf_ub_bit_pos = 2,
    // m_buf_conf_bo_bit_pos = 3,
    
    // m_buf_io_pos = 6,
    // m_buf_io_size = 1,
    // m_buf_io_io1_bit_pos = 0,
    // m_buf_io_io2_bit_pos = 1,
    // m_buf_io_ioen_bit_pos = 2,
    
    // m_buf_data_pos = 7,
    // m_buf_data_size = 4,
    
    // m_buf_offs_pos = 12,
    // m_buf_offs_size = 4,
    // m_buf_fs_pos = 16,
    // m_buf_fs_size = 4,
    
    // m_write_buf_size = 5,
    // m_ready_bit_pos = 0,
    // m_start_bit_pos = 1,
    // m_error_bit_pos = 2,
    // m_noref_bit_pos = 3,
    
    // m_buf_error_bit_pos = 2,
    // m_buf_noref_bit_pos = 3,
    
    m_buf_filter_pos = 1,
    m_buf_filter_mask = 0x0F,
    m_buf_mode_pos = 2,
    m_buf_mode_mask = 0x07,
    m_reset_size = 4
  };
  // enum {
    // m_reg_comm_addr = 0x00 << 5,
    // m_reg_stat_addr = 0x00 << 5,
    // m_reg_mode_addr = 0x01 << 5,
    // m_reg_conf_addr = 0x02 << 5,
    // m_reg_data_addr = 0x03 << 5,
    // m_reg_id_addr =   0x04 << 5,
    // m_reg_io_addr =   0x05 << 5,
    // m_reg_offs_addr = 0x06 << 5,
    // m_reg_fs_addr =   0x07 << 5,
    // m_reg_comm_pos = 0,
    // m_reg_comm_size = 1,
    // m_reg_comm_pos = 0,
    // m_reg_stat_pos = 1,
    // m_reg_stat_size = 1,
    // m_reg_error_shift = 6,
    // m_reg_noref_shift = 5,
    // m_reg_mode_size = 2,
    // m_reg_filt_pos = 2,
    // m_reg_filt_shift = 0,
    // m_reg_filt_mask = 0x0F,
    // m_reg_mode_pos = 1,
    // m_reg_mode_shift = 5,
    // m_reg_mode_mask = 0x07,
    // m_reg_conf_size = 2,
    // m_reg_gain_pos = 1,
    // m_reg_gain_shift = 0,
    // m_reg_gain_mask = 0x07,
    // m_reg_chan_pos = 2,
    // m_reg_chan_shift = 0,
    // m_reg_chan_mask = 0x07,
    // m_reg_bo_pos = 1,
    // m_reg_bo_shift = 5,
    // m_reg_ub_pos = 1,
    // m_reg_ub_shift = 4,
    // m_reg_ref_det_pos = 2,
    // m_reg_ref_det_shift = 5,
    // m_reg_buf_pos = 2,
    // m_reg_buf_shift = 4,
    // m_reg_io_pos = 1,
    // m_reg_io1_shift = 4,
    // m_reg_io1_shift = 5,
    // m_reg_ioen_shift = 6,
    // m_reg_data_pos = 1,
    // m_reg_data_size = 3,
    // m_reg_id_size = 1,
    // m_reg_io_size = 1,
    // m_reg_offs_size = 3,
    // m_reg_fs_size = 3,
    
    // m_reg_bit_mask = 0x01,
    // m_reg_rw_bit_pos = 6
  // };
  enum {
    m_wait_interval = 1,    //  ms
    m_reserved_interval = 1
  };
  enum mode_t {
    md_cont_conv = 0,
    md_single_conv = 1,
    md_idle = 2,
    md_pwr_down = 3,
    md_int_zero = 4,
    md_int_fs = 5,
    md_sys_zero = 6,
    md_sys_fs = 7
  };
  enum transaction_type_t {
    tt_read,
    tt_write
  };
  enum reg_index_t {
    m_reg_status_index = 0,
    m_reg_mode_index = 1,
    m_reg_conf_index = 2,
    m_reg_io_index = 3,
    m_reg_data_index = 4,
    m_reg_id_index = 5,
    m_reg_offs_index = 6,
    m_reg_fs_index = 7
  };
  struct entity_t {
    irs_uarc size;      //  кол-во байт в регистре микросхемы и буфере
    irs_uarc mask;      //  маска (если entity <= 1 байта) до сдвигов
    irs_uarc ic_pos;    //  смещение в байтах в регистре микросхемы
    irs_uarc ic_shift;  //  сдвиг внутри байта (если entity <= 1 байта)
    irs_uarc ic_mask;   //  маска (если entity <= 1 байта)
    irs_uarc buf_pos;   //  смещение в байтах в буфере
    irs_uarc buf_shift; //  сдвиг внутри байта (если entity <= 1 байта)
  };
  class reg_t {
  public:
    reg_t(
      irs_uarc a_buf_first, 
      irs_uarc a_buf_size, 
      irs_uarc a_ic_size, 
      irs_u8 a_ic_addr,
	  bool a_read_only);
    ~reg_t() {};
    void push_back_entity(entity_t a_entity);
    void write_data_processing(const irs_u8* ap_buf, 
      irs_uarc a_index, irs_uarc a_size);
    void read_data_processing(irs_u8* ap_buf, 
      irs_uarc a_index, irs_uarc a_size);
    irs_u8 fill_write_spi_buf(irs_u8* ap_spi_buf);
    void extract_read_spi_buf(const irs_u8* ap_spi_buf);
    irs_u8 request_read_spi_buf(irs_u8* ap_spi_buf);
    bool need_write();
    void need_read();
  private:
  enum {
    m_addr_pos = 0,
    m_data_pos = 1,
    m_addr_shift = 3,
    m_r_bit = 6
  };
    inline bool overlap(irs_uarc a_index, irs_uarc a_size) 
    {
      return (a_index < m_buf_first + m_buf_size) 
          && (a_index + a_size > m_buf_first);
    };
    irs_uarc m_buf_first;
    irs_uarc m_buf_size;
    irs_u8 m_ic_addr;
    vector<irs_u8> m_ic_vector;
    vector<entity_t> m_entity_vector;
    bool m_need_read;
    bool m_need_write;
    bool m_read_only;
    irs_u8 m_filter;
    irs_u8 m_mode;
  };
  
  status_t m_status;
  status_t m_target_status;
  transaction_type_t m_transaction_type;
  mode_t m_mode;
  spi_t* mp_spi;
  irs_u8 mp_buf[m_size];
  irs_u8 mp_spi_buf[m_write_buf_size];
  size_t m_spi_transaction_size;
  irs::timer_t m_timer;
  vector<reg_t> m_reg_vector;
  vector<int> m_filter_settle_time_vector;
  vector<int> m_conv_time_vector;
  gpio_pin_t* mp_cs_pin;
  bool m_initial_read;
  size_t m_current_reg;
  // size_t m_status_reg;
  // size_t m_data_reg;
};*/

struct adc_ad7799_data_t
{
  irs::bit_data_t ready_bit;
  irs::bit_data_t start_bit;
  irs::bit_data_t error_bit;
  irs::bit_data_t noref_bit;
  irs::conn_data_t<irs_u8> filter;
  irs::conn_data_t<irs_u8> mode;
  irs::conn_data_t<irs_u8> gain;
  irs::conn_data_t<irs_u8> channel;
  irs::bit_data_t buf_bit;
  irs::bit_data_t ref_det_bit;
  irs::bit_data_t ub_bit;
  irs::bit_data_t bo_bit;
  irs::bit_data_t io1_bit;
  irs::bit_data_t io2_bit;
  irs::bit_data_t ioen_bit;
  irs::conn_data_t<irs_u32> data;
  irs::conn_data_t<irs_u8> id;
  irs::conn_data_t<irs_u32> offset;
  irs::conn_data_t<irs_u32> full_scale;
  
  adc_ad7799_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
    ready_bit(),
    start_bit(),
    error_bit(),
    noref_bit(),
    filter(),
    mode(),
    gain(),
    channel(),
    buf_bit(),
    ref_det_bit(),
    ub_bit(),
    bo_bit(),
    io1_bit(),
    io2_bit(),
    ioen_bit(),
    data(),
    id(),
    offset(),
    full_scale()
  {
    connect(ap_data, a_start_index);
  }  
   
  irs_uarc connect(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0)
  {
    ready_bit.connect(ap_data, a_start_index, 0);
    start_bit.connect(ap_data, a_start_index, 1);
    error_bit.connect(ap_data, a_start_index, 2);
    noref_bit.connect(ap_data, a_start_index, 3);
    a_start_index++;
    a_start_index = filter.connect(ap_data, a_start_index);
    a_start_index = mode.connect(ap_data, a_start_index);
    a_start_index = gain.connect(ap_data, a_start_index);
    a_start_index = channel.connect(ap_data, a_start_index);
    buf_bit.connect(ap_data, a_start_index, 0);
    ref_det_bit.connect(ap_data, a_start_index, 1);
    ub_bit.connect(ap_data, a_start_index, 2);
    bo_bit.connect(ap_data, a_start_index, 3);
    a_start_index++;
    io1_bit.connect(ap_data, a_start_index, 0);
    io2_bit.connect(ap_data, a_start_index, 1);
    ioen_bit.connect(ap_data, a_start_index, 2);
    a_start_index++;
    a_start_index = data.connect(ap_data, a_start_index);
    a_start_index = id.connect(ap_data, a_start_index);
    a_start_index = offset.connect(ap_data, a_start_index);
    a_start_index = full_scale.connect(ap_data, a_start_index);
    return a_start_index;
  }
};

//! @}

} // namespace irs

#endif // irsadcH
