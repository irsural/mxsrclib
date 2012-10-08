//! \file
//! \ingroup drivers_group
//! \brief Класс для работы с АЦП для ARM
//!
//! Дата: 28.06.2011
//! Дата создания: 13.01.2011

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

//! \brief Драйвер АЦП для контроллеров семейства LM3SxBxx
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

#elif defined(IRS_STM32F2xx)

//! \addtogroup drivers_group
//! @{

//! \brief Драйвер АЦП для контроллеров семейства STM32F2xx
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
    ADC3_PF5_CH15 = (1 << 15) | ADC3_MASK
  };
  st_adc_t(size_t a_adc_address, select_channel_type a_selected_channels,
    counter_t a_adc_interval = make_cnt_ms(100));
  virtual ~st_adc_t();
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
  virtual void tick();
private:
  irs_u32 adc_channel_to_channel_index(adc_channel_t a_adc_channel);
  adc_regs_t* mp_adc;
  irs::loop_timer_t m_adc_timer;
  enum { reqular_channel_count = 16 };
  enum { adc_resolution = 12 };
  enum { adc_max_value = 0xFFF };
  vector<irs_u32> m_regular_channels_values;
  vector<irs_u32> m_active_channels;
  size_t m_current_channel;
  irs_i16 m_temperature_channel_value;
};

//! \brief Драйвер ЦАП для контроллеров семейства STM32F2xx
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

#endif  //  IRS_STM32F2xx

} // namespace arm

} // namespace irs

#endif //armadc
