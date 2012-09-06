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

#include <irsfinal.h>

namespace irs {

namespace arm {

#ifdef __LM3SxBxx__

//! \addtogroup string_processing_group
//! @{

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
#ifdef NOP
class st_adc_t: public adc_t
{
public:
  st_adc_t(size_t a_adc_address, counter_t a_adc_interval = make_cnt_ms(100));
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
};
#endif // NOP
#endif  //  IRS_STM32F2xx

} // namespace arm

} // namespace irs

#endif //armadc
