//! \file
//! \ingroup drivers_group
//! \brief ����� ��� ������ � ��� ��� ARM
//!
//! ���� ��������: 13.01.2011

#ifndef armadc
#define armadc

#include <irsdefs.h>

#include <irsstd.h>
#include <irsadcabs.h>
#include <irsdacabs.h>

#include <irsfinal.h>

namespace irs {

namespace arm {

#ifdef __LM3SxBxx__

//! \addtogroup drivers_group
//! @{

//! \brief ������� ��� ��� ������������ ��������� LM3SxBxx
//! \author Polyakov Maxim
class adc_stellaris_t: public adc_t
{
public:
  enum adc_module_t
  {
    ADC0,
    ADC1
  };
  enum adc_ref_t
  {
    INT_REF = 0,
    EXT_REF = 1
  };

  adc_stellaris_t(select_channel_type a_selected_channels,
    adc_ref_t a_adc_ref = INT_REF,
    adc_module_t a_adc_module = ADC0,
    counter_t a_adc_interval = make_cnt_ms(100));
  ~adc_stellaris_t();
  irs_u16 get_u16_minimum();
  irs_u16 get_u16_maximum();
  irs_u16 get_u16_data(irs_u8 a_channel);
  irs_u32 get_u32_minimum();
  irs_u32 get_u32_maximum();
  irs_u32 get_u32_data(irs_u8 a_channel);
  float get_float_minimum();
  float get_float_maximum();
  float get_float_data(irs_u8 a_channel);
  float get_temperature();
  void tick();
private:
  enum status_t
  {
    WAIT,
    READ_SEQ_0,
    READ_SEQ_1,
    READ_SEQ_2,
    READ_SEQ_3
  };
  enum sequence_t
  {
    SEQ0 = 0,
    SEQ1 = 1,
    SEQ2 = 2,
    SEQ3 = 3
  };
  enum
  {
    PORT_PE7 = 0,
    PORT_PE6 = 1,
    PORT_PE5 = 2,
    PORT_PE4 = 3,
    PORT_PD7 = 4,
    PORT_PD6 = 5,
    PORT_PD5 = 6,
    PORT_PD4 = 7,
    PORT_PE3 = 8,
    PORT_PE2 = 9,
    PORT_PB4 = 10,
    PORT_PB5 = 11,
    PORT_PD3 = 12,
    PORT_PD2 = 13,
    PORT_PD1 = 14,
    PORT_PD0 = 15
  };
  enum
  {
    m_max_adc_channels_cnt = 16,
    m_sequence0_size = 8,
    m_sequence1_size = 4,
    m_sequence2_size = 4,
    m_sequence3_size = 1,
    m_portb_mask = 0x0C00,
    m_portd_mask = 0xF0F0,
    m_porte_mask = 0x030F
  };
  status_t m_status;
  adc_module_t m_adc_module;
  irs::loop_timer_t m_adc_timer;
  vector<irs_u16> m_result_vector;
  irs_u8 m_num_of_channels;

  void set_mux_and_ctl_regs(irs_u32 a_mux, irs_u32 a_ctl, sequence_t a_seq);
  bool check_sequence_ready(sequence_t a_seq);
  void clear_fifo(sequence_t a_seq);
};

//! @}

#elif defined(IRS_STM32_F2_F4_F7)

//! \addtogroup drivers_group
//! @{

//! \brief ������� ��� ��� ������������ ��������� STM32F2xx � STM32F4xx
//! \author Lyashchov Maxim
class st_adc_t: public adc_t
{
private:
  enum {
    ADC1_MASK = 1 << 30,
    ADC2_MASK = 1 << 29,
    ADC3_MASK = 1 << 28
  };
public:
  enum adc_channel_t {
    ADC123_PA0_CH0 = (1 << 0) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA1_CH1 = (1 << 1) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA2_CH2 = (1 << 2) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA3_CH3 = (1 << 3) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC12_PA4_CH4 = (1 << 4) | ADC1_MASK | ADC2_MASK,
    ADC3_PF6_CH4 = (1 << 4) | ADC3_MASK,
    ADC12_PA5_CH5 = (1 << 5) | ADC1_MASK | ADC2_MASK,
    ADC3_PF7_CH5 = (1 << 5) | ADC3_MASK,
    ADC12_PA6_CH6 = (1 << 6) | ADC1_MASK | ADC2_MASK,
    ADC3_PF8_CH6 = (1 << 6) | ADC3_MASK,
    ADC12_PA7_CH7 = (1 << 7) | ADC1_MASK | ADC2_MASK,
    ADC3_PF9_CH7 = (1 << 7) | ADC3_MASK,
    ADC12_PB0_CH8 = (1 << 8) | ADC1_MASK | ADC2_MASK,
    ADC3_PF10_CH8 = (1 << 8) | ADC3_MASK,
    ADC12_PB1_CH9 = (1 << 9) | ADC1_MASK | ADC2_MASK,
    ADC3_PF3_CH9 = (1 << 9) | ADC3_MASK,
    ADC123_PC0_CH10 = (1 << 10) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC1_CH11 = (1 << 11) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC2_CH12 = (1 << 12) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC3_CH13 = (1 << 13) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC12_PC4_CH14 = (1 << 14) | ADC1_MASK | ADC2_MASK,
    ADC3_PF4_CH14 = (1 << 14) | ADC3_MASK,
    ADC12_PC5_CH15 = (1 << 15) | ADC1_MASK | ADC2_MASK,
    ADC3_PF5_CH15 = (1 << 15) | ADC3_MASK,
    ADC1_TEMPERATURE = (1 << 16) | ADC1_MASK,
    ADC1_V_BATTERY = (1 << 18) | ADC1_MASK
  };
  st_adc_t(size_t a_adc_address, select_channel_type a_selected_channels,
    counter_t a_adc_interval = make_cnt_ms(100),
    counter_t a_adc_battery_interval = make_cnt_ms(100),
    bool a_single_conversion = false,
    irs_u8 a_sampling_time = 0x07,
    irs_u8 a_clock_div = 0x3);
  virtual ~st_adc_t();
  virtual size_type get_resolution() const;
  virtual irs_u16 get_u16_minimum();
  virtual irs_u16 get_u16_maximum();
  virtual irs_u16 get_u16_data(irs_u8 a_channel);
  virtual irs_u32 get_u32_minimum();
  virtual irs_u32 get_u32_maximum();
  virtual irs_u32 get_u32_data(irs_u8 a_channel);
  virtual float get_float_minimum();
  virtual float get_float_maximum();
  virtual float get_float_data(irs_u8 a_channel);
  virtual float get_temperature();
  virtual float get_v_battery();
  virtual void tick();
  float get_temperature_degree_celsius(const float a_vref);
private:
  irs_u32 adc_channel_to_channel_index(adc_channel_t a_adc_channel);
  void read_injected_channel_value();
  adc_regs_t* mp_adc;
  irs::loop_timer_t m_adc_timer;
  irs::loop_timer_t m_adc_battery_timer;
  enum { reqular_channel_count = 16 };
  enum { adc_resolution = 12 };
  enum { adc_max_value = 0xFFF };
  vector<irs_u32> m_regular_channels_values;
  vector<irs_u32> m_active_channels;
  size_t m_current_channel;
  bool m_temperature_sensor_enabled;
  irs_i16 m_temperature_channel_value;
  bool m_v_battery_measurement_enabled;
  irs_u16 m_v_battery_channel_value;
  enum injected_channel_switch_t {
    ics_temperature_sensor,
    ics_v_battery
  };
  injected_channel_switch_t m_injected_channel_selected;
  const bool m_single_conversion;
};

#ifdef USE_HAL_DRIVER

//! \brief ������� ��� ��� ������������ ��������� STM32F2xx,
//!   STM32F4xx STM32F7xx. DMA-������.
//! \author Lyashchov Maxim
class st_hal_adc_dma_t: public adc_t
{
private:
  enum {
    ADC1_MASK = 1 << 30,
    ADC2_MASK = 1 << 29,
    ADC3_MASK = 1 << 28
  };
public:
  enum adc_channel_t {
    ADC123_PA0_CH0 = (1 << 0) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA1_CH1 = (1 << 1) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA2_CH2 = (1 << 2) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA3_CH3 = (1 << 3) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC12_PA4_CH4 = (1 << 4) | ADC1_MASK | ADC2_MASK,
    ADC3_PF6_CH4 = (1 << 4) | ADC3_MASK,
    ADC12_PA5_CH5 = (1 << 5) | ADC1_MASK | ADC2_MASK,
    ADC3_PF7_CH5 = (1 << 5) | ADC3_MASK,
    ADC12_PA6_CH6 = (1 << 6) | ADC1_MASK | ADC2_MASK,
    ADC3_PF8_CH6 = (1 << 6) | ADC3_MASK,
    ADC12_PA7_CH7 = (1 << 7) | ADC1_MASK | ADC2_MASK,
    ADC3_PF9_CH7 = (1 << 7) | ADC3_MASK,
    ADC12_PB0_CH8 = (1 << 8) | ADC1_MASK | ADC2_MASK,
    ADC3_PF10_CH8 = (1 << 8) | ADC3_MASK,
    ADC12_PB1_CH9 = (1 << 9) | ADC1_MASK | ADC2_MASK,
    ADC3_PF3_CH9 = (1 << 9) | ADC3_MASK,
    ADC123_PC0_CH10 = (1 << 10) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC1_CH11 = (1 << 11) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC2_CH12 = (1 << 12) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC3_CH13 = (1 << 13) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC12_PC4_CH14 = (1 << 14) | ADC1_MASK | ADC2_MASK,
    ADC3_PF4_CH14 = (1 << 14) | ADC3_MASK,
    ADC12_PC5_CH15 = (1 << 15) | ADC1_MASK | ADC2_MASK,
    ADC3_PF5_CH15 = (1 << 15) | ADC3_MASK,
    ADC1_TEMPERATURE = (1 << 16) | ADC1_MASK,
    ADC1_V_BATTERY = (1 << 18) | ADC1_MASK
  };
  typedef irs_size_t size_type;

  struct channel_config_t
  {
    adc_channel_t channel_id;
    irs_u32 sampling_time;
    channel_config_t():
      channel_id(ADC123_PA0_CH0),
      sampling_time(ADC_SAMPLETIME_480CYCLES)
    {
    }
  };

  struct settings_t
  {
    size_t adc_address;
    vector<channel_config_t> channels;
    //select_channel_type selected_channels;
    //irs_u8 sampling_time;
    //! \brief �������� ����� PCLK2.
    //!   ������������ ���������:
    //!   ADC_CLOCKPRESCALER_PCLK_DIV1 - ADC_CLOCKPRESCALER_PCLK_DIV8
    irs_u32 clock_prescaller;

    size_type dma_address;
    //gpio_speed_t gpio_speed;

    irs::c_array_view_t<irs_u8> rx_buffer;
    //! \brief ����� DMA RX. ������������ ���������: DMA1_Stream0 - DMA2_Stream7
    DMA_Stream_TypeDef* rx_dma_y_stream_x;
    //! \brief ����� DMA ��� RX.
    //!   ������������ ���������: DMA_CHANNEL_0 - DMA_CHANNEL_7
    uint32_t rx_dma_channel;
    //! \brief ���������. ������������ ���������:
    //!   DMA_PRIORITY_LOW - DMA_PRIORITY_VERY_HIGH
    uint32_t rx_dma_priority;

    //bool interrupt_enabled;
    IRQn_Type interrupt;
    irs::arm::interrupt_id_t interrupt_id;
    irs_u32 interrupt_priority;
    //void (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);
    //void (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);
    //void (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);

    irs_u32 data_item_byte_count;
    settings_t():
      adc_address(0),
      channels(),
      //selected_channels(0),
      //sampling_time(0x07),
      clock_prescaller(ADC_CLOCKPRESCALER_PCLK_DIV8),

      dma_address(NULL),
      //gpio_speed(gpio_speed_2mhz),

      rx_buffer(NULL, 0),
      rx_dma_y_stream_x(0),
      rx_dma_channel(0),

      rx_dma_priority(DMA_PRIORITY_LOW),

      //interrupt_enabled(false),
      interrupt(DMA2_Stream1_IRQn),
      interrupt_id(irs::arm::dma2_stream1_int),
      interrupt_priority(0),
      //XferCpltCallback(NULL),
      //XferHalfCpltCallback(NULL),
      //XferErrorCallback(NULL),

      data_item_byte_count(2)
    {
    }
  };
  st_hal_adc_dma_t(const settings_t& a_settings/*,
    counter_t a_adc_interval = make_cnt_ms(100),
    counter_t a_adc_battery_interval = make_cnt_ms(100),
    bool a_single_conversion = false*/
  );
  virtual ~st_hal_adc_dma_t();
  virtual size_type get_resolution() const;
  virtual irs_u16 get_u16_minimum();
  virtual irs_u16 get_u16_maximum();
  virtual irs_u16 get_u16_data(irs_u8 a_channel);
  virtual irs_u32 get_u32_minimum();
  virtual irs_u32 get_u32_maximum();
  virtual irs_u32 get_u32_data(irs_u8 a_channel);
  virtual float get_float_minimum();
  virtual float get_float_maximum();
  virtual float get_float_data(irs_u8 a_channel);
  virtual float get_temperature();
  virtual float get_v_battery();
  virtual void tick();
  float get_temperature_degree_celsius(const float a_vref);

  ADC_HandleTypeDef* get_adc_handle();
  // ����� �������� DMA_HandleTypeDef �� ADC_HandleTypeDef
  void transfer_complete(ADC_HandleTypeDef *ap_adc_handle);
  void half_transfer_complete(ADC_HandleTypeDef *ap_adc_handle);
  void xfer_error_callback(ADC_HandleTypeDef *ap_adc_handle);

private:
  irs_u32 adc_channel_to_channel_index(adc_channel_t a_adc_channel);
  void read_injected_channel_value();

  void reset_dma();
  void start_dma();
  void stop_dma();
  void invalidate_dcache_rx();


  enum part_t {
    sbp_right_part,
    sbp_left_part
  };
  void process(part_t a_part);

  settings_t m_settings;
  adc_regs_t* mp_adc;
  //irs::loop_timer_t m_adc_timer;
  //irs::loop_timer_t m_adc_battery_timer;
  enum { reqular_channel_count = 16 };
  enum { adc_resolution = 12 };
  enum { adc_max_value = 0xFFF };
  vector<irs_u32> m_regular_channels_values;
  vector<irs_u32> m_active_channels;
  size_t m_current_channel;
  bool m_temperature_sensor_enabled;
  irs_i16 m_temperature_channel_value;
  bool m_v_battery_measurement_enabled;
  irs_u16 m_v_battery_channel_value;
  enum injected_channel_switch_t {
    ics_temperature_sensor,
    ics_v_battery
  };
  injected_channel_switch_t m_injected_channel_selected;
  //const bool m_single_conversion;

  ADC_HandleTypeDef m_adc_handle;
  DMA_HandleTypeDef m_hdma_rx;
  bool m_rx_status;
  bool m_hdma_init;
  irs::c_array_view_t<irs_u8> m_rx_buffer;

  class dma_event_t: public irs::event_t
  {
  public:
    dma_event_t(DMA_HandleTypeDef* ap_hdma_rx);
    virtual void exec();
    /*void enable();
    void disable();*/
  private:
    DMA_HandleTypeDef* mp_hdma_rx;
    bool m_enabled;
  };
  dma_event_t m_dma_event;
  //static map<ADC_HandleTypeDef*, st_hal_adc_dma_t*> m_adc_handle_map;
  //static st_hal_adc_dma_t* mp_temp;

  template<class T, class R>
  R average(const T* ap_begin, const T* ap_end, const size_t a_step)
  {
    const size_t intpart = static_cast<size_t>(ap_end - ap_begin)/a_step;
    const size_t remainder = static_cast<size_t>(ap_end - ap_begin)%a_step;
    const size_t count = remainder == 0 ? intpart : (intpart + 1);

    R result = 0.;
    while (ap_begin < ap_end) {
      result += *ap_begin;
      ap_begin += a_step;
    }
    result /= count;
    return result;
  }
};
#endif // USE_HAL_DRIVER

//! \brief ������� ��� DMA ��� ������������ ��������� STM32F2xx
//! \author Gavrishchuk Maxim
//!
//! ����: 17.10.2012
//! ���� ��������: 17.10.2012
//!���������� ��������� ��������� settings_adc_dma_t � ������������
//! � ������������� �� ����������
class st_adc_dma_t: public adc_dma_t
{
private:
  enum {
    ADC1_MASK = 1 << 30,
    ADC2_MASK = 1 << 29,
    ADC3_MASK = 1 << 28
  };

public:
  enum dma_stream_t {
    DMA_STREAM0 = 0,
    DMA_STREAM1 = 1,
    DMA_STREAM2 = 2,
    DMA_STREAM3 = 3,
    DMA_STREAM4 = 4,
    DMA_STREAM5 = 5,
    DMA_STREAM6 = 6,
    DMA_STREAM7 = 7
  };
  enum dma_channel_t {
    DMA_CH0 = 0,
    DMA_CH1 = 1,
    DMA_CH2 = 2,
    DMA_CH3 = 3,
    DMA_CH4 = 4,
    DMA_CH5 = 5,
    DMA_CH6 = 6,
    DMA_CH7 = 7
  };
  enum timer_channel_t {
    TIM_CH1 = 0,
    TIM_CH2 = 1,
    TIM_CH3 = 2,
    TIM_CH4 = 3
  };
  struct settings_adc_dma_t {
    settings_adc_dma_t(size_t a_adc_address,
      select_channel_type a_adc_selected_channels,
      size_t a_dma_address,
      dma_stream_t a_dma_stream,
      dma_channel_t a_dma_channel,
      size_t a_timer_address,
      timer_channel_t a_timer_channel);
    size_t adc_address;
    select_channel_type adc_selected_channels;
    size_t dma_address;
    dma_stream_t dma_stream;
    dma_channel_t dma_channel;
    size_t timer_address;
    timer_channel_t timer_channel;
  };
  enum adc_channel_t {
    ADC123_PA0_CH0 = (1 << 0) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA1_CH1 = (1 << 1) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA2_CH2 = (1 << 2) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PA3_CH3 = (1 << 3) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC12_PA4_CH4 = (1 << 4) | ADC1_MASK | ADC2_MASK,
    ADC3_PF6_CH4 = (1 << 4) | ADC3_MASK,
    ADC12_PA5_CH5 = (1 << 5) | ADC1_MASK | ADC2_MASK,
    ADC3_PF7_CH5 = (1 << 5) | ADC3_MASK,
    ADC12_PA6_CH6 = (1 << 6) | ADC1_MASK | ADC2_MASK,
    ADC3_PF8_CH6 = (1 << 6) | ADC3_MASK,
    ADC12_PA7_CH7 = (1 << 7) | ADC1_MASK | ADC2_MASK,
    ADC3_PF9_CH7 = (1 << 7) | ADC3_MASK,
    ADC12_PB0_CH8 = (1 << 8) | ADC1_MASK | ADC2_MASK,
    ADC3_PF10_CH8 = (1 << 8) | ADC3_MASK,
    ADC12_PB1_CH9 = (1 << 9) | ADC1_MASK | ADC2_MASK,
    ADC3_PF3_CH9 = (1 << 9) | ADC3_MASK,
    ADC123_PC0_CH10 = (1 << 10) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC1_CH11 = (1 << 11) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC2_CH12 = (1 << 12) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC123_PC3_CH13 = (1 << 13) | ADC1_MASK | ADC2_MASK | ADC3_MASK,
    ADC12_PC4_CH14 = (1 << 14) | ADC1_MASK | ADC2_MASK,
    ADC3_PF4_CH14 = (1 << 14) | ADC3_MASK,
    ADC12_PC5_CH15 = (1 << 15) | ADC1_MASK | ADC2_MASK,
    ADC3_PF5_CH15 = (1 << 15) | ADC3_MASK
  };
  st_adc_dma_t(settings_adc_dma_t* ap_settings,
    irs::c_array_view_t<irs_u16> a_buff,
    double a_frequency = 1000.);
  virtual ~st_adc_dma_t();
  virtual void start();
  virtual void stop();
  virtual bool status();
  virtual void set_frequency(double a_frequency);
  virtual void set_prescaler(irs_u16 a_psc);
  virtual void set_buff(irs::c_array_view_t<irs_u16> a_buff);
private:
  void set_adc_timer_channel(size_t a_timer_address,
    timer_channel_t a_timer_channel);
  irs_u32 adc_channel_to_channel_index(adc_channel_t a_adc_channel);
  cpu_traits_t::frequency_type timer_frequency();
  void set_sample_time(double& a_frequency);
  adc_regs_t* mp_adc;
  enum { reqular_channel_count = 16 };
  enum { adc_resolution = 12 };
  enum { adc_max_value = 0xFFF };
  vector<irs_u32> m_regular_channels_values;
  vector<irs_u32> m_active_channels;
  size_t m_current_channel;
  irs_i16 m_temperature_channel_value;
  dma_regs_t* mp_dma;
  tim_regs_t* mp_timer;
  irs::c_array_view_t<irs_u16> m_buff;
  double m_frequency;
  double m_set_freq;
  irs_u16 m_psc;
  size_t m_buff_size;
  bool m_start;
  settings_adc_dma_t* mp_settings;
};

//! \brief ������� ��� ��� ������������ ��������� STM32F2xx
//! \author Lyashchov Maxim
class st_dac_t: public dac_t
{
public:
  typedef size_t size_type;
  enum dac_channel_t {
    DAC_PA4_CH0 = (1 << 0),
    DAC_PA5_CH1 = (1 << 1)
  };
  st_dac_t(select_channel_type a_selected_channels);
  virtual size_t get_resolution() const;
  virtual irs_u32 get_u32_maximum() const;
  virtual void set_u32_data(size_t a_channel, const irs_u32 a_data);
  virtual float get_float_maximum() const;
  virtual void set_float_data(size_t a_channel, const float a_data);
  virtual void tick() {};
private:
  void set_u16_normalized_data(size_t a_channel, const irs_u16 a_data);
  enum { dac_resolution = 12 };
  enum { dac_max_value = 0xFFF };
  struct data_reg_t {
    __REG32 data        :12;
    __REG32             :20;
  };
  vector<volatile data_reg_t*> m_channels;
};

//! @}

#endif  //  IRS_STM32_F2_F4_F7

} // namespace arm

} // namespace irs

#endif //armadc
