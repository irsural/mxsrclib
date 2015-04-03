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
#include <irsalg.h>
#include <irsgpio.h>
#include <irsadcabs.h>
#include <irsdacabs.h>

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
  void configure_spi();
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
  gpio_pin_t* mp_cs_pin;
  void configure_spi();
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

class cyclic_adc_ad7791_t: public adc_t
{
public:
  enum { channel_count = 1 };
  enum { adc_resolution = 24 };
  enum { adc_max_value = 0xFFFFFF };
  typedef irs_size_t size_type;
  cyclic_adc_ad7791_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
    counter_t a_read_delay);
  virtual size_type get_resolution() const;
  virtual bool new_value_exists(irs_u8 a_channel) const;
  virtual irs_u32 get_u32_data(irs_u8 a_channel);
  virtual void tick();
private:
  adc_ad7791_t m_adc_ad7791;
  adc_ad7791_data_t m_adc_ad7791_data;
};

//--------------------------  ADS8344 ------------------------------------------
class cyclic_adc_ads8344_t: public adc_t
{
public:
  enum { channel_count = 8 };
  enum { adc_resolution = 16 };
  enum { adc_max_value = 0xFFFF };
  enum channel_name_t {
    ch_0 = (1 << 0),            //! < \brief Канал 0
    ch_1 = (1 << 1),            //! < \brief Канал 1
    ch_2 = (1 << 2),            //! < \brief Канал 2
    ch_3 = (1 << 3),            //! < \brief Канал 3
    ch_4 = (1 << 4),            //! < \brief Канал 4
    ch_5 = (1 << 5),            //! < \brief Канал 5
    ch_6 = (1 << 6),            //! < \brief Канал 6
    ch_7 = (1 << 7)             //! < \brief Канал 7
  };
  typedef irs_size_t size_type;
  typedef irs_u16 sample_type;
  cyclic_adc_ads8344_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
    select_channel_type a_selected_channels,
    counter_t a_read_delay);
  virtual size_type get_resolution() const;
  virtual void select_channels(irs_u32 a_selected_channels);
  virtual bool new_value_exists(irs_u8 a_channel) const;
  virtual irs_u32 get_u32_data(irs_u8 a_channel);
  virtual void tick();
private:
  void configure_spi();
  irs_u8 make_control_byte(irs_u8 a_channel);
  enum process_t {
    process_init,
    process_wait_init,
    process_read_write,
    process_wait
  };
  enum { m_spi_buf_size = 4};

  enum { s_bit = 7 };
  enum { a0_bit = 4 };
  enum { sgl_dif_bit = 2 };
  enum { pd1_bit = 1 };
  enum { pd0_bit = 0 };
  //enum { channel_mask = 0x70};
  irs_u8 mp_spi_write_buf[m_spi_buf_size];
  irs_u8 mp_spi_read_buf[m_spi_buf_size];
  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
  process_t m_process;
  bool m_new_value_exists;
  irs_u32 m_sample;
  irs::timer_t m_delay_timer;
  struct channel_t
  {
    int index;
    bool new_value_exists;
    sample_type value;
    channel_t():
      index(1),
      new_value_exists(false),
      value(0)
    {
    }
  };
  vector<channel_t> m_channels;
  size_type m_current_channel;
  bool m_selected_other_channels;
  vector<channel_name_t> m_names;
};

//--------------------------  ADS1298 ------------------------------------------
// 24-bit 8-channel ADC
template<size_t n>
struct get_pos_right_set_bit {

//enum { value = n*Factorial<n-1>::value };
private:
  enum { x = static_cast<int>(n) };
public:
  enum {
    value = ((((x & -x) & 0x0000FFFF) ? 0 : 16)+\
      (((x & -x) & 0x00FF00FF) ? 0 : 8)+\
      (((x & -x) & 0x0F0F0F0F) ? 0 : 4)+\
      (((x & -x) & 0x33333333) ? 0 : 2)+\
      (((x & -x) & 0x55555555) ? 0 : 1)+\
      ((x & -x) ? 0 : 1))
  };
};
template <class field_t, class reg_t, size_t mask>
class register_field_t
{
public:
  register_field_t(reg_t* ap_reg);
  const field_t& operator=(const field_t &a_elem);
  operator field_t() const;
private:
  enum { pos = get_pos_right_set_bit<mask>::value};
  reg_t* mp_reg;
};

template <class field_t, class reg_t, size_t mask>
register_field_t<field_t, reg_t, mask>::register_field_t(reg_t* ap_buf):
  mp_reg(ap_buf)
{
}

template <class field_t, class reg_t, size_t mask>
const field_t& register_field_t<field_t, reg_t, mask>::operator=(
  const field_t &a_value)
{
  if (mp_reg) {
    *mp_reg &= static_cast<reg_t>(~mask);
    *mp_reg |= static_cast<reg_t>((a_value << pos) & mask);
  }
  return a_value;
}

template <class field_t, class reg_t, size_t mask>
register_field_t<field_t, reg_t, mask>::operator field_t() const
{
  field_t value = field_t();
  if (mp_reg) {
    value = *mp_reg & mask;
  }
  return value;
}

class adc_ads1298_continuous_mode_t: public adc_t
{
public:
  typedef irs_size_t size_type;
  enum { adc_resolution = 24 };
  enum { channel_count = 8 };

  //! \brief Скорость АЦП
  //! \details Скорость задается через делитель частоты fmod. fmod = fclk/4 в
  //!   режиме высокого разрешения и fmod = fclk/8 в режиме низкого
  //!   энергопотребления. Подробности здесь
  //!   http://www.ti.com/lit/ds/symlink/ads1298.pdf .
  //!   Значения скорости приведены для fclk = 2,048 МГц.
  enum data_rate_t {
    //! \brief fmod/16: 32kSPS(HR mode) или 16kSPS(low power mode)
    data_rate_fmod_16 = 0,
    //! \brief fmod/32: 16kSPS(HR mode) или 8kSPS(low power mode)
    data_rate_fmod_32 = 1,
    //! \brief fmod/64: 8kSPS(HR mode) или 4kSPS(low power mode)
    data_rate_fmod_64 = 2,
    //! \brief fmod/128: 4kSPS(HR mode) или 2kSPS(low power mode)
    data_rate_fmod_128 = 3,
    //! \brief fmod/256: 2kSPS(HR mode) или 1kSPS(low power mode)
    data_rate_fmod_256 = 4,
    //! \brief fmod/512: 1kSPS(HR mode) или 500SPS(low power mode)
    data_rate_fmod_512 = 5,
    //! \brief fmod/1024: 500SPS(HR mode) или 250SPS(low power mode)
    data_rate_fmod_1024 = 6
  };

  enum channel_input_t {
    chi_normal_electrode_input = 0,
    chi_input_shorted = 1,
    chi_supply = 3,
    chi_temperature_sensor = 4,
    chi_test_signal = 5
  };

  enum gain_t {
    gain_1 = 1,
    gain_2 = 2,
    gain_3 = 3,
    gain_4 = 4,
    gain_6 = 0,
    gain_8 = 5,
    gain_12 = 6
  };

  struct channel_settings_t
  {
    bool power_down;
    channel_input_t input;
    gain_t gain;
    channel_settings_t():
      power_down(false),
      input(chi_normal_electrode_input),
      gain(gain_1)
    {
    }
  };

  struct settings_t
  {
    spi_t* spi;
    gpio_pin_t* cs_pin;
    gpio_pin_t* power_down_pin;
    gpio_pin_t* reset_pin;
    gpio_pin_t* date_ready_pin;
    data_rate_t data_rate;
    channel_settings_t channel_1;
    channel_settings_t channel_2;
    channel_settings_t channel_3;
    channel_settings_t channel_4;
    channel_settings_t channel_5;
    channel_settings_t channel_6;
    channel_settings_t channel_7;
    channel_settings_t channel_8;
    settings_t():
      spi(NULL),
      cs_pin(NULL),
      power_down_pin(NULL),
      reset_pin(NULL),
      date_ready_pin(NULL),
      data_rate(data_rate_fmod_1024),
      channel_1(),
      channel_2(),
      channel_3(),
      channel_4(),
      channel_5(),
      channel_6(),
      channel_7(),
      channel_8()
    {
    }
  };

  adc_ads1298_continuous_mode_t(const settings_t& a_settings);
  ~adc_ads1298_continuous_mode_t();
  virtual size_type get_resolution() const;
  virtual void select_channels(irs_u32 a_selected_channels);
  virtual inline bool new_value_exists(irs_u8 a_channel) const;
  virtual inline irs_u32 get_u32_data(irs_u8 a_channel);
  virtual inline float get_float_minimum();
  virtual inline float get_float_maximum();
  virtual inline float get_float_data(irs_u8 a_channel);
  virtual inline float get_temperature();
  virtual void tick();
  float get_temperature_degree_celsius(const float a_vref);
  static float convert_value_to_temperature_degree_celsius(irs_u32 a_value,
    const float a_vref);
  static float convert_value_to_temperature_degree_celsius(float a_value,
    const float a_vref);
private:
  void set_gain(int a_gain);
  template <size_t channel>
  inline void read_sample_from_channel()
  {
    if (m_results[channel].enabled) {
      enum {
        channel_1_pos = 3,
        channel_size = 3,
        pos_0 = channel*channel_size + channel_1_pos,
        pos_1 = pos_0 + 1,
        pos_2 = pos_1 + 1
      };

      irs_u8 b0 = m_spi_buf[pos_0];
      irs_u8 b1 = m_spi_buf[pos_1];
      irs_u8 b2 = m_spi_buf[pos_2];
      m_results[channel].value =
        static_cast<irs_u32>((b0 << 24) | (b1 << 16) | (b2 << 8));
      m_results[channel].new_value_exists = true;
    }
  }

  enum process_t {
    process_off,
    process_wait_after_power_up,
    process_wait_reset,
    process_wait_after_reset,
    process_get_spi,
    process_send_sdatac,
    process_wait_spi_read_write,
    process_activate_normal_mode,
    process_send_start,
    process_send_rdatac,
    process_wait_send_rdatac,
    process_fast_read_data,
    process_data_conversion
  };

  enum opcode_t {
    opcode_wakeup = 0x2,
    opcode_standby = 0x4,
    opcode_reset = 0x6,
    opcode_start = 0x8,
    opcode_stop = 0xA,
    opcode_rdatac = 0x10,
    opcode_sdatac = 0x11,
    opcode_rdata = 0x12,
    opcode_rreg = 0x20,
    opcode_wreg = 0x40
  };

  enum reg_t {
    reg_id = 0x0,
    reg_config_1 = 0x1,
    reg_config_2 = 0x2,
    reg_config_3 = 0x3,
    reg_loff = 0x4,
    reg_ch1set = 0x5,
    reg_ch2set = 0x6,
    reg_ch3set = 0x7,
    reg_ch4set = 0x8,
    reg_ch5set = 0x9,
    reg_ch6set = 0xA,
    reg_ch7set = 0xB,
    reg_ch8set = 0xC,
    reg_rld_sensp = 0xD,
    reg_rld_sensn = 0xE,
    reg_loff_sensp = 0xF,
    reg_loff_sensn = 0x10,
    reg_loff_flip = 0x11,
    reg_loff_statp = 0x12,
    reg_loff_statn = 0x13,
    reg_gpio = 0x14,
    reg_pace = 0x15,
    reg_rest = 0x16,
    reg_config4 = 0x17,
    reg_wct1 = 0x18,
    reg_wct2 = 0x19,
    reg_last = reg_wct2
  };

  settings_t m_settings;
  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
  gpio_pin_t* mp_power_down_pin;
  gpio_pin_t* mp_reset_pin;
  gpio_pin_t* mp_date_ready_pin;
  process_t m_process;

  irs_u8 mp_register;
  struct ch_set_t
  {
    irs_u8 reg;
    register_field_t<bool, irs_u8, 0x1 << 7> power_down;
    register_field_t<size_t, irs_u8, 0x7 << 4> gain;
    register_field_t<size_t, irs_u8, 0x7> channel_input;
    ch_set_t():
      reg(0),
      power_down(&reg),
      gain(&reg),
      channel_input(&reg)
    {
    }
  };
  struct config1_t
  {
    irs_u8 reg;
    register_field_t<bool, irs_u8, 0x1 << 7> hight_resolution;
    register_field_t<bool, irs_u8, 0x1 << 6> daisy_en;
    register_field_t<bool, irs_u8, 0x1 << 5> clk_connection;
    register_field_t<size_t, irs_u8, 0x7> output_data_rate;
    config1_t():
      reg(0),
      hight_resolution(&reg),
      daisy_en(&reg),
      clk_connection(&reg),
      output_data_rate(&reg)
    {
    }
  };
  struct config2_t
  {
    irs_u8 reg;
    register_field_t<bool, irs_u8, 0x1 << 5> wct_chopping_scheme;
    register_field_t<bool, irs_u8, 0x1 << 4> test_source;
    register_field_t<bool, irs_u8, 0x1 << 3> test_signal_amplitude;
    register_field_t<size_t, irs_u8, 0x3> test_signal_frequency;
    config2_t():
      reg(0),
      wct_chopping_scheme(&reg),
      test_source(&reg),
      test_signal_amplitude(&reg),
      test_signal_frequency(&reg)
    {
    }
  };

  ch_set_t get_ch_set_from_channel_settings(
    const channel_settings_t& a_channel_settings);

  ch_set_t m_ch_set;
  config1_t m_config1;
  config2_t m_config2;

  const double m_t_clk_max;


  vector<irs_u8> m_spi_buf;

  struct channel_result_t
  {
    bool enabled;
    bool new_value_exists;
    irs_u32 value;
    channel_result_t(bool a_enabled = false):
      enabled(a_enabled),
      new_value_exists(0),
      value(0)
    {}
  };

  vector<channel_result_t> m_results;

  bool m_temperature_sensor_enabled;
  size_type m_temperature_channel_index;

  irs::timer_t m_t_after_power_up_delay;
  irs::timer_t m_t_reset_delay;
  irs::timer_t m_t_after_reset_delay;

  irs::timer_t m_t_cssc_delay;
  irs::timer_t m_t_sccs_delay;
  irs::timer_t m_t_csh_delay;

  bool try_get_spi();
  void send_opcode(opcode_t a_opcode);
  void write_regs(const map<reg_t, irs_u8>& a_regs);
  void read_data();
  bool spi_read_write_is_complete() const;
  void spi_prepare();
  void spi_release();
  void write_regs_for_activate_normal_mode();
  void read_samples_from_spi_buf();
};

inline bool adc_ads1298_continuous_mode_t::new_value_exists(
  irs_u8 a_channel) const
{
  return m_results[a_channel].new_value_exists;
}

inline irs_u32 adc_ads1298_continuous_mode_t::get_u32_data(
  irs_u8 a_channel)
{
  m_results[a_channel].new_value_exists = false;
  return m_results[a_channel].value;
}

inline float adc_ads1298_continuous_mode_t::get_float_minimum()
{
  return -1.f;
}

inline float adc_ads1298_continuous_mode_t::get_float_maximum()
{
  return +1.f;
}

inline float adc_ads1298_continuous_mode_t::get_float_data(irs_u8 a_channel)
{
  m_results[a_channel].new_value_exists = false;
  const float value = static_cast<irs_i32>(m_results[a_channel].value);
  const irs_i32 i32_min = IRS_I32_MIN;
  return value/(i32_min*-1.f) ;
}

inline float adc_ads1298_continuous_mode_t::get_temperature()
{
  if (m_temperature_sensor_enabled) {
    return get_float_data(static_cast<irs_u8>(m_temperature_channel_index));
  }
  return 0;
}

//--------------------------  AD7683  ------------------------------------------

class adc_ad7683_t : public mxdata_t
{
  enum status_t
  {
    ADC_FREE,
    ADC_READ
  };
  status_t m_status;
  spi_t *mp_spi;
  static const irs_uarc m_spi_size = 3;
  static const irs_uarc m_spi_data_size = 2;
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
  adc_ad7683_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, counter_t a_read_delay);
  ~adc_ad7683_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
};

struct adc_ad7683_data_t
{
  irs::bit_data_t new_data_bit;
  irs::conn_data_t<irs_u16> voltage_code;

  adc_ad7683_data_t(irs::mxdata_t *ap_data, irs_uarc a_start_index = 0):
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

class cyclic_adc_ad7683_t: public adc_t
{
public:
  enum { channel_count = 1 };
  enum { adc_resolution = 16 };
  enum { adc_max_value = 0xFFFF };
  typedef irs_size_t size_type;
  cyclic_adc_ad7683_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
    counter_t a_read_delay);
  virtual size_type get_resolution() const;
  virtual bool new_value_exists(irs_u8 a_channel) const;
  virtual irs_u32 get_u32_data(irs_u8 a_channel);
  virtual void tick();
private:
  adc_ad7683_t m_adc_ad7683;
  adc_ad7683_data_t m_adc_ad7683_data;
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

class cyclic_adc_ad7686_t: public adc_t
{
public:
  enum { channel_count = 1 };
  enum { adc_resolution = 16 };
  enum { adc_max_value = 0xFFFF };
  typedef irs_size_t size_type;
  cyclic_adc_ad7686_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
    counter_t a_read_delay);
  virtual size_type get_resolution() const;
  virtual bool new_value_exists(irs_u8 a_channel) const;
  virtual irs_u32 get_u32_data(irs_u8 a_channel);
  virtual void tick();
private:
  adc_ad7686_t m_adc_ad7686;
  adc_ad7686_data_t m_adc_ad7686_data;
};

//--------------------------  AD8400  ------------------------------------------

class dac_ad8400_t : public mxdata_t
{
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
private:
  void configure_spi();
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
  irs_u8 mp_write_buffer[m_packet_size];
  irs_u8 m_init_value;
  bool m_need_write;
  //  CS
  gpio_pin_t *mp_cs_pin;
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

class simple_dac_ad8400_t: public dac_t
{
public:
  simple_dac_ad8400_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
    irs_u8 a_init_value);
  virtual irs_status_t get_status() const;
  virtual size_t get_resolution() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data);
  virtual void tick();
private:
  enum { dac_resulution = 8 };
  dac_ad8400_t m_dac_ad8400;
  dac_ad8400_data_t m_dac_ad8400_data;
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
  irs_u8 mp_write_buffer[m_packet_size];
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
  irs_u8 mp_write_buffer[m_packet_size];
  irs_u16 m_init_value;
  bool m_need_write;
  bool m_need_reset;
  //  CS
  gpio_pin_t *mp_cs_pin;
  void configure_spi();
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
  irs_u8 m_write_buffer[m_max_write_bytes];
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
  void configure_spi();
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
  void configure_spi();
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
  void configure_spi();
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

class simple_dac_ad5293_t: public dac_t
{
public:
  simple_dac_ad5293_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin);
  virtual irs_status_t get_status() const { return irs_st_ready; }
  virtual size_t get_resolution() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data);
  virtual void tick();
private:
  enum { dac_resulution = 10 };
  dac_ad5293_t m_dac_ad5293;
  dac_ad5293_data_t m_dac_ad5293_data;
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

//-------------------------- AD5686 -------------------------------------------
class dac_ad5686_t: public dac_t
{
public:
  typedef irs_u16 sample_type;
  typedef irs_size_t size_type;
  typedef irs_u32 select_channel_type;
  dac_ad5686_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin);
  virtual ~dac_ad5686_t();
  virtual irs_status_t get_status() const;
  virtual size_t get_resolution() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data);
  virtual void tick();
private:
  void configure_spi();
  enum { resolution = 16 };
  enum { buf_size = 3 };
  enum command_t {
    command_write_to_and_update_channel = 3,
    command_software_reset = 6
  };
  enum { command_shift = 4 };
  enum process_t {
    process_reset,
    process_request,
    process_free,
    process_write
  };
  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
  struct channel_t
  {
    bool new_value_exists;
    sample_type value;
    channel_t():
      new_value_exists(false),
      value(0)
    {
    }
  };
  vector<channel_t> m_channels;
  size_type m_current_channel;
  process_t m_process;
  irs_u8 mp_buf[buf_size];
};


//-------------------------- AD7794 -------------------------------------------
// 24-bit 3-channel ADC
class adc_ad7794_t: public adc_request_t
{
public:
  enum freq_t {
    freq_470Hz = 1,
    freq_242Hz = 2,
    freq_123Hz = 3,
    freq_62Hz = 4,
    freq_50Hz = 5,
    freq_39Hz = 6,
    freq_33_2Hz = 7,
    freq_19_6Hz = 8,
    freq_16_7Hz_80dB = 9,
    freq_16_7Hz_65dB = 10,
    freq_12_5Hz = 11,
    freq_10Hz = 12,
    freq_8_33Hz = 13,
    freq_6_25Hz = 14,
    freq_4_17Hz = 15
  };

  adc_ad7794_t(spi_t *ap_spi,
    gpio_pin_t* ap_cs_pin);
  ~adc_ad7794_t();
  virtual void start();
  virtual void stop();
  virtual meas_status_t status() const;
  virtual irs_i32 get_value();
  virtual void tick();
  virtual void set_param(adc_param_t a_param, const int a_value);
  virtual void get_param(adc_param_t a_param, int* ap_value);

private:
  void set_channel(int a_channel);
  void set_mode(int a_mode);
  void set_freq(int a_freq);
  void set_gain(int a_gain);
  void set_buf(int a_buf);
  void set_ref_det(int a_ref_det);
  void set_ub(int a_ub);
  void set_bo(int a_bo);

  enum mode_t {
    mode_free,
    mode_spi_rw,
    mode_spi_rw_wait,
    mode_read_all_reg,
    mode_read_data,
    mode_read_wait,
    mode_ready_wait,
    mode_get_data
  };
  enum operating_modes_t {
    om_continuous = 0,
    om_single = 1,
    om_idle = 2,
    om_power_down = 3,
    om_internal_zero_scale = 4,
    om_internal_full_scale = 5,
    om_system_zero_scale = 6,
    om_system_full_scale = 7
  };
  enum {
    m_size_buf = 16,
    m_write_buf_size = 4
  };
  enum param_byte_pos_t {
    m_gain_byte_pos = 1,
    m_freq_byte_pos = 0,
    m_ch_byte_pos = 0,
    m_mode_byte_pos = 1,
    m_buf_byte_pos = 0,
    m_ref_det_byte_pos = 0,
    m_ub_byte_pos = 1,
    m_bo_byte_pos = 1
  };
  enum param_pos_t {
    m_rw_pos = 6,
    m_rs_pos = 3,
    m_gain_pos = 0,
    m_freq_pos = 0,
    m_ch_pos = 0,
    m_mode_pos = 5,
    m_ready_pos = 7,
    m_buf_pos = 4,
    m_ref_det_pos = 5,
    m_ub_pos = 4,
    m_bo_pos = 5
  };
  enum param_size_t {
    m_gain_size = 3,
    m_freq_size = 4,
    m_ch_size = 4,
    m_mode_size = 3,
    m_buf_size = 1,
    m_ref_det_size = 1,
    m_ub_size = 1,
    m_bo_size = 1
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
  vector<int> m_conv_time_vector;
  irs_u8 m_freq;
  int m_reserved_interval;
  timer_t m_timer;
  bool m_ready_wait;
  bool m_get_data;
  irs_i32 m_value;
  operating_modes_t m_read_mode;
  operating_modes_t m_cur_read_mode;
  irs_u8 mp_get_buff[m_write_buf_size];
  int* mp_value_param;
  bool m_read_calibration_coeff;

  void spi_prepare();
  void spi_release();
  void creation_reg_comm(reg_t a_reg, transaction_type_t a_tt);
  int calculation_shift(reg_t a_reg);
  int calculation_number_byte(reg_t a_reg, param_byte_pos_t byte_pos);
  int to_int(double a_number);
  int filling_units(param_size_t a_size);
  void creation_reg(reg_t a_reg, int a_value, param_byte_pos_t a_byte_pos,
    param_pos_t a_start_pos, param_size_t a_size);
  int get(reg_t a_reg, param_byte_pos_t a_byte_pos,
    param_pos_t a_start_pos, param_size_t a_size);
  irs_i32 conversion_spi_value();
};

class cyclic_adc_ad7794_t: public adc_t
{
public:
  enum { channel_count = 1 };
  enum { adc_resolution = 24 };
  enum { adc_max_value = 0xFFFFFF };
  enum channel_name_t {
    ch_1 = (1 << 0),            //! < \brief AIN1(+)/AIN1(-)
    ch_2 = (1 << 1),            //! < \brief AIN2(+)/AIN2(-)
    ch_3 = (1 << 2),            //! < \brief AIN3(+)/AIN3(-)
    ch_4 = (1 << 3),            //! < \brief AAIN4(+)/AIN4(-)
    ch_5 = (1 << 4),            //! < \brief AIN5(+)/AIN5(-)
    ch_6 = (1 << 5),            //! < \brief AIN6(+)/AIN6(-)
    ch_temp_sensor = (1 << 6),  //! < \brief Temp Sensor
    ch_avdd_monitor = (1 << 7), //! < \brief AVDD Monitor
    ch_0 = (1 << 8)             //! < \brief AIN1(-)/AIN1(-)
  };
  enum freq_t {
    freq_470Hz = 1,
    freq_242Hz = 2,
    freq_123Hz = 3,
    freq_62Hz = 4,
    freq_50Hz = 5,
    freq_39Hz = 6,
    freq_33_2Hz = 7,
    freq_19_6Hz = 8,
    freq_16_7Hz_80dB = 9,
    freq_16_7Hz_65dB = 10,
    freq_12_5Hz = 11,
    freq_10Hz = 12,
    freq_8_33Hz = 13,
    freq_6_25Hz = 14,
    freq_4_17Hz = 15
  };
  typedef irs_size_t size_type;
  typedef irs_u32 sample_type;
  cyclic_adc_ad7794_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
    select_channel_type a_selected_channels,
    counter_t a_read_delay, bool a_unipolar = true,
    freq_t a_freq = freq_4_17Hz);
  virtual size_type get_resolution() const;
  virtual bool new_value_exists(irs_u8 a_channel) const;
  virtual irs_u32 get_u32_data(irs_u8 a_channel);
  virtual void tick();
private:

  enum process_t {
    process_set_param,
    process_select_channel,
    process_read_value,
    process_wait_read_value
  };
  adc_ad7794_t m_adc;
  struct parameter_t
  {
    adc_param_t name;
    int value;
    parameter_t(adc_param_t a_name, int a_value):
      name(a_name),
      value(a_value)
    {
    }
  };
  vector<parameter_t> m_params;
  size_type m_current_param;
  struct channel_t
  {
    int index;
    bool new_value_exists;
    sample_type value;
    channel_t():
      index(1),
      new_value_exists(false),
      value(0)
    {
    }
  };
  vector<channel_t> m_channels;
  size_type m_current_channel;
  process_t m_process;
  loop_timer_t m_timer;
  bool m_unipolar;
};

//-------------------------- AD7799 -------------------------------------------
// 24-bit 3-channel ADC
class adc_ad7799_t: public adc_request_t
{
public:
  enum freq_t {
    freq_500Hz = 1,
    freq_250Hz = 2,
    freq_125Hz = 3,
    freq_62_5Hz = 4,
    freq_50Hz = 5,
    freq_39_2Hz = 6,
    freq_33_3Hz = 7,
    freq_19_6Hz = 8,
    freq_16_7Hz_80dB = 9,
    freq_16_7Hz_65dB = 10,
    freq_12_5Hz = 11,
    freq_10Hz = 12,
    freq_8_33Hz = 13,
    freq_6_25Hz = 14,
    freq_4_17Hz = 15
  };

  adc_ad7799_t(spi_t *ap_spi, gpio_pin_t* ap_cs_pin);
  ~adc_ad7799_t();
  virtual void start();
  virtual void stop();
  virtual meas_status_t status() const;
  virtual irs_i32 get_value();
  virtual void tick();
  virtual void set_param(adc_param_t a_param, const int a_value);
  virtual void get_param(adc_param_t a_param, int* ap_value);

private:
  void set_channel(int a_channel);
  void set_mode(int a_mode);
  void set_freq(int a_freq);
  void set_gain(int a_gain);
  void set_buf(int a_buf);
  void set_ref_det(int a_ref_det);
  void set_ub(int a_ub);
  void set_bo(int a_bo);

  enum mode_t {
    mode_free,
    mode_spi_rw,
    mode_spi_rw_wait,
    mode_read_all_reg,
    mode_read_data,
    mode_read_wait,
    mode_ready_wait,
    mode_get_data
  };
  enum operating_modes_t {
    om_continuous = 0,
    om_single = 1,
    om_idle = 2,
    om_power_down = 3,
    om_internal_zero_scale = 4,
    om_internal_full_scale = 5,
    om_system_zero_scale = 6,
    om_system_full_scale = 7
  };
  enum {
    m_size_buf = 16,
    m_write_buf_size = 4
  };
  enum param_byte_pos_t {
    m_gain_byte_pos = 1,
    m_freq_byte_pos = 0,
    m_ch_byte_pos = 0,
    m_mode_byte_pos = 1,
    m_buf_byte_pos = 0,
    m_ref_det_byte_pos = 0,
    m_ub_byte_pos = 1,
    m_bo_byte_pos = 1
  };
  enum param_pos_t {
    m_rw_pos = 6,
    m_rs_pos = 3,
    m_gain_pos = 0,
    m_freq_pos = 0,
    m_ch_pos = 0,
    m_mode_pos = 5,
    m_ready_pos = 7,
    m_buf_pos = 4,
    m_ref_det_pos = 5,
    m_ub_pos = 4,
    m_bo_pos = 5
  };
  enum param_size_t {
    m_gain_size = 3,
    m_freq_size = 4,
    m_ch_size = 3,
    m_mode_size = 3,
    m_buf_size = 1,
    m_ref_det_size = 1,
    m_ub_size = 1,
    m_bo_size = 1
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
  vector<int> m_conv_time_vector;
  irs_u8 m_freq;
  int m_reserved_interval;
  timer_t m_timer;
  bool m_ready_wait;
  bool m_get_data;
  irs_i32 m_value;
  operating_modes_t m_read_mode;
  operating_modes_t m_cur_read_mode;
  irs_u8 mp_get_buff[m_write_buf_size];
  int* mp_value_param;
  bool m_read_calibration_coeff;

  void spi_prepare();
  void spi_release();
  void creation_reg_comm(reg_t a_reg, transaction_type_t a_tt);
  int calculation_shift(reg_t a_reg);
  int calculation_number_byte(reg_t a_reg, param_byte_pos_t byte_pos);
  int to_int(double a_number);
  int filling_units(param_size_t a_size);
  void creation_reg(reg_t a_reg, int a_value, param_byte_pos_t a_byte_pos,
    param_pos_t a_start_pos, param_size_t a_size);
  int get(reg_t a_reg, param_byte_pos_t a_byte_pos,
    param_pos_t a_start_pos, param_size_t a_size);
  irs_i32 conversion_spi_value();
};

//-------------------------- DAC8531 -------------------------------------------
// 16-bit DAC

class dac_8531_t: public dac_t
{
public:
  dac_8531_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, float a_init_data);
  virtual size_t get_resolution() const;
  virtual irs_u32 get_u32_maximum() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data);
  virtual float get_float_maximum() const;
  virtual void set_float_data(size_t a_channel, const float a_data);
  virtual void tick();

private:
  void set_u16_normalized_data(size_t a_channel, const irs_u16 a_data);
  enum {
    dac_resolution = 16,
    write_buf_size = 3
  };
  enum { dac_max_value = 0xFFFF };
  enum mode_t {
    mode_free,
    mode_write,
    mode_write_wait
  };
  spi_t *mp_spi;
  gpio_pin_t *mp_cs_pin;
  irs_u16 m_data;
  irs_u16 m_new_data;
  irs_u8 mp_spi_buf[write_buf_size];
  mode_t m_mode;
};

//-------------------------- DAC1220 -------------------------------------------
// 20-bit sigma-delta DAC

class dac_1220_t: public dac_t
{
public:
  dac_1220_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
    float a_init_data, counter_t a_startup_pause);
  virtual irs_status_t get_status() const;
  virtual size_t get_resolution() const;
  virtual irs_u32 get_u32_maximum() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data);
  virtual float get_float_maximum() const;
  virtual void set_float_data(size_t a_channel, const float a_data);
  virtual void tick();

private:
  enum {
    m_dac_resolution = 20,
    m_max_write_buf_size = 4,
    m_dac_max_value = 0x000FFFFF,
    m_cmd_pos = 0,
    m_data_pos = 1
  };
  enum status_t {
    st_startup,
    st_write_command,
    st_write_data,
    st_prepare_spi,
    st_wait_spi,
    st_free
  };
  enum cmr_bits_1_t {
    cmr_adpt = 7,
    cmr_calpin = 6,
    cmr_write1 = 5,
    cmr_crst = 1
  };
  enum cmr_bits_0_t {
    cmr_res = 7,
    cmr_clr = 6,
    cmr_df = 5,
    cmr_disf = 4,
    cmr_bd = 3,
    cmr_msb = 2,
    cmr_md1 = 1,
    cmr_md0 = 0
  };
  enum {
    m_data_reg_size = 4,
    m_data_reg_write_command = 0x40,
    m_command_reg_size = 3,
    m_command_reg_write_command = 0x24
  };
  spi_t* mp_spi;
  gpio_pin_t* mp_cs_pin;
  irs_u32 m_data;
  irs_u8 mp_spi_buf[m_max_write_buf_size];
  status_t m_status;
  irs_status_t m_return_status;
  bool m_need_write;
  timer_t m_timer;
  size_t m_write_buf_size;
};

//-------------------------- LTC8043 -------------------------------------------
// 12-bit DAC

class dac_ltc8043_t: public dac_t
{
public:
  dac_ltc8043_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin, float a_init_data = 0.f);
  virtual size_t get_resolution() const;
  virtual irs_u32 get_u32_maximum() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data);
  virtual float get_float_maximum() const;
  virtual void set_float_data(size_t a_channel, const float a_data);
  virtual void tick();

private:
  enum {
    dac_resolution = 12,
    write_buf_size = 2,
    cs_interval = 1
  };
  enum { dac_max_value = 0x0FFF };
  enum mode_t {
    mode_free,
    mode_write,
    mode_write_wait,
    mode_wait_cs
  };
  spi_t *mp_spi;
  gpio_pin_t *mp_cs_pin;
  irs_u16 m_data;
  irs_u16 m_new_data;
  irs_u8 mp_spi_buf[write_buf_size];
  mode_t m_mode;
  timer_t m_timer;
};

//! @}

} // namespace irs

#endif // irsadcH
