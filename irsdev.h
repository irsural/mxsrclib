//! \file
//! \ingroup drivers_group
//! \brief Устройства
//!
//! Дата: 28.04.2011
//! Ранняя дата: 08.07.2010

#ifndef irsdevH
#define irsdevH

#include <irsdefs.h>

#include <irscpu.h>
#include <timer.h>

#ifdef __ICCARM__
#include <armioregs.h>
#endif // __ICCARM__
#include <irsgpio.h>
#include <mxdata.h>
#include <irsmem.h>

#define PWM_ZERO_PULSE 1

#ifdef PWM_ZERO_PULSE
  #include <irsint.h>
  #if defined(__ICCAVR__) || defined(__ICCARM__)
    #include <irsarchint.h>
  #endif // IRS_LINUX
#endif  //  PWM_ZERO_PULSE

#ifdef USE_STDPERIPH_DRIVER
# if defined(IRS_STM32F2xx)
#   include <stm32f2xx_rtc.h>
# elif defined(IRS_STM32F4xx)
#   include <stm32f4xx_rtc.h>
# endif // defined(IRS_STM32F4xx)
#endif // USE_STDPERIPH_DRIVER

#ifdef USE_STDPERIPH_DRIVER
# ifdef IRS_STM32F7xx
#   include "stm32f7xx_hal.h"
# endif // IRS_STM32F7xx
#endif //

#include <irsfinal.h>

namespace irs {

irs::string_t get_device_name(device_code_t a_device_code);

//! \addtogroup drivers_group
//! @{

class pwm_gen_t
{
public:
  virtual ~pwm_gen_t() {}
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void set_duty(irs_uarc a_duty) = 0; // Коэф. зап. в "попугаях"
  virtual void set_duty(float a_duty) = 0;    // Коэф. заполнения (1/скважность)
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type  a_frequency) = 0; // Гц
  virtual irs_uarc get_max_duty() = 0;
  virtual cpu_traits_t::frequency_type get_max_frequency();
  virtual cpu_traits_t::frequency_type get_timer_frequency();
};

//! \brief Виртуальный интерфейс для драйверов watchdog
class watchdog_t
{
public:
  //! \brief Запускает watchdog
  virtual void start() = 0;
  //! \brief Сбрасывает watchdog
  virtual void restart() = 0;
  //! \brief Возвращает \c true, если произошел сброс из-за watchdog
  virtual bool watchdog_reset_cause() = 0;
  //! \brief Сбрасывает статус сброса, произошедшего из-за watchdog
  virtual void clear_reset_status() = 0;
};

#ifndef __WATCOMC__
class three_phase_pwm_gen_t : public pwm_gen_t
{
public:
  enum phase_t
  {
    PHASE_A,
    PHASE_B,
    PHASE_C,
    PHASE_ALL
  };
  virtual ~three_phase_pwm_gen_t() {}
  using irs::pwm_gen_t::set_duty;
  virtual void set_duty(irs_uarc a_duty, phase_t a_phase) = 0;
  virtual void set_duty(float a_duty, phase_t a_phase) = 0;
};
#endif //__WATCOMC__

#ifdef __ICCARM__

namespace arm
{

class arm_three_phase_pwm_t : public three_phase_pwm_gen_t
{
private:
  typedef cpu_traits_t::frequency_type freq_t;
public:
  arm_three_phase_pwm_t(freq_t a_freq, counter_t a_dead_time);
  virtual ~arm_three_phase_pwm_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty);
  virtual void set_duty(irs_uarc a_duty, phase_t a_phase);
  virtual void set_duty(float a_duty);
  virtual void set_duty(float a_duty, phase_t a_phase);
  virtual freq_t set_frequency(freq_t a_frequency);
  virtual irs_uarc get_max_duty();
  virtual freq_t get_max_frequency();
private:
  enum
  {
    pwm_clk_div = 2,
    pwm_all_sync = 0x7,
    pwm_all_enable = 0x3F,
    pwm_all_disable = 0
  };
  const freq_t m_max_freq;
  const freq_t m_min_freq;
  freq_t m_freq;

  irs_u16 calc_load_reg_value(freq_t a_freq);
  #ifdef PWM_ZERO_PULSE
    event_connect_t<arm_three_phase_pwm_t> m_int_event;
    void interrupt();
  #endif  //  PWM_ZERO_PULSE
};

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)

class gptm_usage_t {
  irs_u8 m_channels;
public:
  inline gptm_usage_t() { m_channels = 0xFF; }
  inline ~gptm_usage_t() { m_channels = 0xFF; }
  inline bool channel_free(gptm_channel_t a_channel) {
    return m_channels & (1 << a_channel);
  }
  inline void set_channel_free(gptm_channel_t a_channel) {
    m_channels |= (1 << a_channel);
  }
  inline void set_channel_use(gptm_channel_t a_channel) {
    m_channels &= ~(1 << a_channel);
  }
};

//  Класс однофазного шим-генератора на основе таймера общего назначения
//  a_timer_module - базовый адрес используемого таймера,
//  a_port - базовый адрес порта выходного сигнала,
//  a_bit - номер бита порта выходного сигнала,
//  a_frequency - частота выходного сигнала,
//  a_duty - коэффициент заполнения выходного сигнала

class gptm_generator_t : public pwm_gen_t
{
public:
  gptm_generator_t(gpio_channel_t a_gpio_channel,
    cpu_traits_t::frequency_type a_frequency, float a_duty = 0.5f);
  ~gptm_generator_t();
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty);
  virtual void set_duty(float a_duty);
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type  a_frequency);
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
private:
  bool m_valid_input_data;
  gptm_channel_t m_gptm_channel;
  irs_u8 m_enable_bit;
  p_arm_port_t mp_enable_reg;
  p_arm_port_t mp_load_reg;
  p_arm_port_t mp_match_reg;
  arm_port_t m_gpio_base;
  void init_gptm_channel(gptm_channel_t a_gptm_channel,
    cpu_traits_t::frequency_type a_frequency, float a_duty);
  void init_gpio_port(gpio_channel_t a_gpio_channel, irs_u8 a_gpio_pmc_value);
  irs_u16 calc_load_value(cpu_traits_t::frequency_type a_frequency);
};

#elif defined(__STM32F100RBT__)
#elif defined(IRS_STM32_F2_F4_F7)

class timers_usage_t {
public:
  inline timers_usage_t() { m_timers = 0xFFFFFFFF; }
  inline ~timers_usage_t() { m_timers = 0xFFFFFFFF; }
  inline bool is_timer_free(size_t a_timer_index) {
    return m_timers & (1 << a_timer_index);
  }
  inline void set_timer_free(size_t a_timer_index) {
    m_timers |= (1 << a_timer_index);
  }
  inline void set_timer_use(size_t a_timer_index) {
    m_timers &= ~(1 << a_timer_index);
  }
private:
  irs_u32 m_timers;
};

enum break_polarity_t {
  break_polarity_active_low,
  break_polarity_active_high
};

//! \brief Драйвер ШИМ-генератора для контроллеров
//!   семейств STM32F2xx и STM32F4xx
//! \author Lyashchov Maxim
class st_pwm_gen_t: public pwm_gen_t
{
public:
  typedef irs_size_t size_type;
  enum output_polarity_t {
    op_active_high,
    op_active_low
  };
  st_pwm_gen_t(gpio_channel_t a_gpio_channel, size_t a_timer_address,
    cpu_traits_t::frequency_type a_frequency, float a_duty = 0.5f);
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty);
  virtual void set_duty(float a_duty);
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type  a_frequency);
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
  virtual cpu_traits_t::frequency_type get_timer_frequency();
  cpu_traits_t::frequency_type get_frequency() const;
  void set_duty(gpio_channel_t a_gpio_channel, irs_uarc a_duty);
  void set_duty(gpio_channel_t a_gpio_channel, float a_duty);
  void break_enable(gpio_channel_t a_gpio_channel, break_polarity_t a_polarity);
  void break_disable();
  void channel_enable(gpio_channel_t a_gpio_channel,
    output_polarity_t a_output_polarity = op_active_high);
  void complementary_channel_enable(gpio_channel_t a_gpio_channel,
    output_polarity_t a_output_polarity = op_active_high);
  void set_dead_time(float a_time);
  // Временно добавлены функции для отладки
  cpu_traits_t::frequency_type get_auto_reload_value();
  cpu_traits_t::frequency_type set_auto_reload_value(
    cpu_traits_t::frequency_type  a_arr);
protected:
  enum output_compare_mode_t {
    ocm_force_inactive_level = 4,
    ocm_pwm_mode_1 = 6
  };
  struct channel_t
  {
    //float duty;
    gpio_channel_t main_channel;
    bool main_channel_active_low;
    gpio_channel_t complementary_channel;
    bool complementary_channel_active_low;
    irs_u32 timer_channel;
    irs_u32* tim_ccr;
    channel_t():
      //duty(0),
      main_channel(PNONE),
      main_channel_active_low(false),
      complementary_channel(PNONE),
      complementary_channel_active_low(false),
      timer_channel(0),
      tim_ccr(NULL)
    {
    }
  };

  const channel_t* get_channel(irs_u32 a_index) const;
  size_type channel_count() const;
private:
  irs_u32 get_timer_channel_and_select_alternate_function(
    gpio_channel_t a_gpio_channel);
  void select_alternate_function_for_break_channel();
  void set_duty_register(irs_u32* ap_tim_ccr, irs_uarc a_duty);
  void set_duty_register(irs_u32* ap_tim_ccr, float a_duty);
  irs_u32* get_tim_ccr_register(irs_u32 a_timer_channel);
  channel_t* find_channel(irs_u32 a_timer_channel);

  void channel_enable_helper(gpio_channel_t a_gpio_channel,
    bool a_complementary, output_polarity_t a_output_polarity);



  //channel_t* find_channel(irs_u32 a_timer_channel);
  void set_mode_capture_compare_registers_for_all_channels(
    output_compare_mode_t a_mode);
  void set_mode_capture_compare_registers(output_compare_mode_t a_mode,
    channel_t* ap_channel);
  cpu_traits_t::frequency_type timer_frequency() const;
  vector<st_timer_name_t> get_available_timers(gpio_channel_t a_gpio_channel);

  bool m_started;
  vector<channel_t> m_channels;
  gpio_channel_t m_gpio_channel;
  tim_regs_t* mp_timer;
  cpu_traits_t::frequency_type m_frequency;
  //float m_duty;
  irs_u32* mp_tim_ccr;
  irs_u32 m_timer_channel;
  gpio_channel_t m_complementary_gpio_channel;
  gpio_channel_t m_break_gpio_channel;
};

#ifdef USE_HAL_DRIVER
#define ST_HAL_PWM_GEN_DMA_NEW 1

#if !ST_HAL_PWM_GEN_DMA_NEW
//! \brief Не доделан!!! Драйвер ШИМ-генератора для контроллеров
//!   семейств STM32F2xx, STM32F4xx, STM32F7xx. DMA-версия.
//! \author Lyashchov Maxim
class st_hal_pwm_gen_dma_t: public pwm_gen_t
{
public:
  typedef irs_size_t size_type;
  struct settings_t
  {
    gpio_channel_t gpio_channel;
    size_t timer_address;
    cpu_traits_t::frequency_type frequency; // заменить на double
    float duty;
    size_type dma_address;
    //gpio_speed_t gpio_speed;

    irs::c_array_view_t<irs_u8> tx_buffer;
    //! \brief Канал DMA TX. Использовать константы: DMA1_Stream0 - DMA2_Stream7
    DMA_Stream_TypeDef* tx_dma_y_stream_x;
    //! \brief Канал DMA для TX.
    //!   Использовать константы: DMA_CHANNEL_0 - DMA_CHANNEL_7
    uint32_t tx_dma_channel;
    //! \brief Приоритет. Использовать константы:
    //!   DMA_PRIORITY_LOW - DMA_PRIORITY_VERY_HIGH
    uint32_t tx_dma_priority;

    IRQn_Type interrupt;
    irs::arm::interrupt_id_t interrupt_id;
    irs_u32 interrupt_priority;

    void (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);
    void (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);
    void (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);

    irs_u32 data_item_byte_count;
    settings_t():
      gpio_channel(PNONE),
      timer_address(0),
      frequency(62500),
      duty(0.5f),
      dma_address(NULL),
      //gpio_speed(gpio_speed_2mhz),

      tx_buffer(NULL, 0),
      tx_dma_y_stream_x(0),
      tx_dma_channel(0),
      tx_dma_priority(DMA_PRIORITY_LOW),

      interrupt(DMA2_Stream1_IRQn),
      interrupt_id(irs::arm::dma2_stream1_int),
      interrupt_priority(0),

      XferCpltCallback(NULL),
      XferHalfCpltCallback(NULL),
      XferErrorCallback(NULL),

      data_item_byte_count(2)
    {
    }
  };
  enum output_polarity_t {
    op_active_high,
    op_active_low
  };
  st_hal_pwm_gen_dma_t(const settings_t& a_settings);
  virtual void start();
  virtual void stop();
  virtual void set_duty(irs_uarc a_duty);
  virtual void set_duty(float a_duty);
  virtual cpu_traits_t::frequency_type set_frequency(
    cpu_traits_t::frequency_type  a_frequency);
  virtual irs_uarc get_max_duty();
  virtual cpu_traits_t::frequency_type get_max_frequency();
  virtual cpu_traits_t::frequency_type get_timer_frequency();
  cpu_traits_t::frequency_type get_frequency() const;
  void set_duty(gpio_channel_t a_gpio_channel, irs_uarc a_duty);
  void set_duty(gpio_channel_t a_gpio_channel, float a_duty);
  void break_enable(gpio_channel_t a_gpio_channel, break_polarity_t a_polarity);
  void break_disable();
  void channel_enable(gpio_channel_t a_gpio_channel,
    output_polarity_t a_output_polarity = op_active_high);
  void complementary_channel_enable(gpio_channel_t a_gpio_channel,
    output_polarity_t a_output_polarity = op_active_high);
  void set_dead_time(float a_time);
  cpu_traits_t::frequency_type get_auto_reload_value();
  cpu_traits_t::frequency_type set_auto_reload_value(
    cpu_traits_t::frequency_type  a_arr);

  //DMA_HandleTypeDef* get_hdma_handle();
private:
  irs_u32 get_timer_channel_and_select_alternate_function(
    gpio_channel_t a_gpio_channel);

  void select_alternate_function_for_break_channel();
  void set_duty_register(irs_u32* ap_tim_ccr, irs_uarc a_duty);
  void set_duty_register(irs_u32* ap_tim_ccr, float a_duty);
  irs_u32* get_tim_ccr_register(irs_u32 a_timer_channel);

  void channel_enable_helper(gpio_channel_t a_gpio_channel,
    bool a_complementary, output_polarity_t a_output_polarity);

  void reset_dma();
  void start_dma();
  void stop_dma();
  void cleand_dcache_tx();

  enum output_compare_mode_t {
    ocm_force_inactive_level = 4,
    ocm_pwm_mode_1 = 6
  };
  struct channel_t
  {
    //float duty;
    gpio_channel_t main_channel;
    bool main_channel_active_low;
    gpio_channel_t complementary_channel;
    bool complementary_channel_active_low;
    irs_u32 timer_channel;
    irs_u32* tim_ccr;
    channel_t():
      //duty(0),
      main_channel(PNONE),
      main_channel_active_low(false),
      complementary_channel(PNONE),
      complementary_channel_active_low(false),
      timer_channel(0),
      tim_ccr(NULL)
    {
    }
  };

  channel_t* find_channel(irs_u32 a_timer_channel);
  void set_mode_capture_compare_registers_for_all_channels(
    output_compare_mode_t a_mode);
  void set_mode_capture_compare_registers(output_compare_mode_t a_mode,
    channel_t* ap_channel);
  cpu_traits_t::frequency_type timer_frequency() const;
  vector<st_timer_name_t> get_available_timers(gpio_channel_t a_gpio_channel);

  settings_t m_settings;

  bool m_started;
  vector<channel_t> m_channels;
  gpio_channel_t m_gpio_channel;
  tim_regs_t* mp_timer;
  cpu_traits_t::frequency_type m_frequency;
  //float m_duty;
  irs_u32* mp_tim_ccr;
  irs_u32 m_timer_channel;
  gpio_channel_t m_complementary_gpio_channel;
  gpio_channel_t m_break_gpio_channel;

  //dma_regs_t* mp_dma;
  DMA_HandleTypeDef m_hdma_tx;
  //bool m_tx_status;
  bool m_hdma_init;
  irs::c_array_view_t<irs_u8> m_tx_buffer;

  class dma_event_t: public irs::event_t
  {
  public:
    dma_event_t(DMA_HandleTypeDef* ap_hdma_tx);
    virtual void exec();
    /*void enable();
    void disable();*/
  private:
    DMA_HandleTypeDef* mp_hdma_tx;
    bool m_enabled;
  };
  dma_event_t m_dma_event;
};

#else // ST_HAL_PWM_GEN_DMA_NEW

//! \brief Не доделан!!! Драйвер ШИМ-генератора для контроллеров
//!   семейств STM32F2xx, STM32F4xx, STM32F7xx. DMA-версия.
//! \author Lyashchov Maxim
class st_hal_pwm_gen_dma_t: public st_pwm_gen_t
{
public:
  typedef irs_size_t size_type;
  struct settings_t
  {
    gpio_channel_t gpio_channel;
    size_t timer_address;
    cpu_traits_t::frequency_type frequency; // заменить на double
    float duty;
    size_type dma_address;
    //gpio_speed_t gpio_speed;

    irs::c_array_view_t<irs_u8> tx_buffer;
    //! \brief Канал DMA TX. Использовать константы: DMA1_Stream0 - DMA2_Stream7
    DMA_Stream_TypeDef* tx_dma_y_stream_x;
    //! \brief Канал DMA для TX.
    //!   Использовать константы: DMA_CHANNEL_0 - DMA_CHANNEL_7
    uint32_t tx_dma_channel;
    //! \brief Приоритет. Использовать константы:
    //!   DMA_PRIORITY_LOW - DMA_PRIORITY_VERY_HIGH
    uint32_t tx_dma_priority;

    bool interrupt_enabled;
    IRQn_Type interrupt;
    interrupt_id_t interrupt_id;
    irs_u32 interrupt_priority;
    void (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);
    void (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);
    void (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);

    irs_u32 data_item_byte_count;
    settings_t():
      gpio_channel(PNONE),
      timer_address(0),
      frequency(62500),
      duty(0.5f),
      dma_address(NULL),
      //gpio_speed(gpio_speed_2mhz),

      tx_buffer(NULL, 0),
      tx_dma_y_stream_x(0),
      tx_dma_channel(0),
      tx_dma_priority(DMA_PRIORITY_LOW),

      interrupt_enabled(false),
      interrupt(DMA2_Stream1_IRQn),
      interrupt_id(irs::arm::dma2_stream1_int),
      interrupt_priority(0),
      XferCpltCallback(NULL),
      XferHalfCpltCallback(NULL),
      XferErrorCallback(NULL),

      data_item_byte_count(2)
    {
    }
  };
  enum output_polarity_t {
    op_active_high,
    op_active_low
  };
  st_hal_pwm_gen_dma_t(const settings_t& a_settings);
  virtual void start();
  virtual void stop();
  //DMA_HandleTypeDef* get_hdma_handle();
private:

  void reset_dma();
  void start_dma();
  void stop_dma();
  void cleand_dcache_tx();

  settings_t m_settings;
  tim_regs_t* mp_timer;
  DMA_HandleTypeDef m_hdma_tx;
  //bool m_tx_status;
  bool m_hdma_init;
  irs::c_array_view_t<irs_u8> m_tx_buffer;

  class dma_event_t: public irs::event_t
  {
  public:
    dma_event_t(DMA_HandleTypeDef* ap_hdma_tx);
    virtual void exec();
    /*void enable();
    void disable();*/
  private:
    DMA_HandleTypeDef* mp_hdma_tx;
    bool m_enabled;
  };
  dma_event_t m_dma_event;
};
#endif // ST_HAL_PWM_GEN_DMA_NEW
#endif // USE_HAL_DRIVER

#else
  #error Тип контроллера не определён
#endif  //  mcu type

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__) ||\
  defined(__STM32F100RBT__)
class watchdog_timer_t: public watchdog_t
{
public:
  watchdog_timer_t(size_t a_period_s = 300):
    m_period_s(a_period_s)
  {
    #ifdef __LM3SxBxx__
    RCGC0_bit.WDT0 = 1;
    for (irs_u8 i = 10; i; i--);
    #endif  //  __LM3SxBxx__
    #ifdef __LM3Sx9xx__
    RCGC0_bit.WDT = 1;
    for (irs_u8 i = 10; i; i--);
    #endif  //  __LM3SxBxx__
  }
  virtual void start()
  {
    #ifdef __LM3SxBxx__
    WDT0LOCK = 0x1ACCE551;
    WDT0LOAD = static_cast<irs_u32>(m_period_s*irs::cpu_traits_t::frequency());
    WDT0CTL_bit.RESEN = 1;
    WDT0CTL_bit.INTEN = 1;
    WDT0TEST_bit.STALL = 1; // вероятно не работает с J-Link;
    WDT0LOCK = 0x0;
    #endif  //  __LM3SxBxx__
    #ifdef __LM3Sx9xx__
    WDTLOCK = 0x1ACCE551;
    WDTLOAD = static_cast<irs_u32>(m_period_s*irs::cpu_traits_t::frequency());
    WDTCTL_bit.RESEN = 1;
    WDTCTL_bit.INTEN = 1;
    WDTTEST_bit.STALL = 1; // вероятно не работает с J-Link;
    WDTLOCK = 0x0;
    #endif  //  __LM3SxBxx__
  }
  virtual void restart()
  {
    #ifdef __LM3SxBxx__
    WDT0LOCK = 0x1ACCE551;
    WDT0ICR = 0x11111111;
    WDT0LOCK = 0x0;
    #endif  //  __LM3SxBxx__
    #ifdef __LM3Sx9xx__
    WDTLOCK = 0x1ACCE551;
    WDTICR = 0x11111111;
    WDTLOCK = 0x0;
    #endif  //  __LM3Sx9xx__
  }
  virtual bool watchdog_reset_cause()
  {
    #if defined(__LM3SxBxx__)
    return RESC_bit.WDT0;
    #elif defined(__LM3Sx9xx__)
    return RESC_bit.WDT;
    #elif defined(__STM32F100RBT__)
    return false;
    #elif defined(IRS_STM32_F2_F4_F7)
    return false;
    #else
      #error Тип контроллера не определён
    #endif  //  mcu type
  }
  virtual void clear_reset_status()
  {
    #if defined(__LM3SxBxx__)
    RESC_bit.WDT0 = 0;
    #elif defined(__LM3Sx9xx__)
    RESC_bit.WDT = 0;
    #elif defined(__STM32F100RBT__)
    volatile i = 0;
    #elif defined(IRS_STM32_F2_F4_F7)
    volatile i = 0;
    #else
      #error Тип контроллера не определён
    #endif  //  mcu type
  }
private:
  size_t m_period_s;
}; // watchdog_timer_t

#elif defined(IRS_STM32F_2_AND_4)
#ifdef USE_STDPERIPH_DRIVER
//! \brief Драйвер независимого сторожевого таймера для контроллеров
//!   семейств STM32F2xx и STM32F4xx
//! \details Работает от независимого генератора с частотой ~32 кГц
//! \author Lyashchov Maxim
class st_independent_watchdog_t: public watchdog_t
{
public:
  typedef irs_size_t size_type;
  //! \param[in] a_period_s Период срабатывания в секундах. Допустимо указывать
  //!   значения [0, 100]. Однако фактически
  //!   сторожевой таймер работает со следующим диапазоном
  //!   (для LSI = 32 кГц) [0.000125, 32.768]
  st_independent_watchdog_t(double a_period_s = 32);
  virtual void start();
  virtual void restart();
  virtual bool watchdog_reset_cause();
  //! \brief Сбрасывает флаги перезагрузки: LPWRRSTF, WWDGRSTF,
  //!   IWDGRSTF, SFTRSTF
  virtual void clear_reset_status();
private:
  double m_period_s;
  irs_u8 m_counter_start_value;
}; // st_independent_watchdog_t

//! \brief Драйвер оконного сторожевого таймера для контроллеров
//!   семейств STM32F2xx и STM32F4xx
//! \details Работает от PCLK1 с тимичными значениями 30 МГц для STM32F2xx и
//!   42 Мгц для stm32f4xx
//! \author Lyashchov Maxim
class st_window_watchdog_t: public watchdog_t
{
public:
  typedef irs_size_t size_type;
  //! \param[in] a_period_min_s Минимальный период сброса в секундах. Допустимо
  //!   указывать значения [0, 1]. Однако фактически
  //!   сторожевой таймер работает со следующими диапазономи значений
  //!   - [0.00013653, 0.06991] (stm32f2xx с Fpclk1 = 30 MHz)
  //!   - [0.00009752, 0.04993] (stm32f4xx с Fpclk1 = 42 MHz)
  //!   Должно выполняться условие a_period_min_s <= a_period_max_s
  //! \param[in] a_period_max_s Максимальный период сброса в секундах.
  //!   Допустимые значения такие же, как у параметра a_period_min_s.
  //!   Должно выполняться условие a_period_max_s >= a_period_min_s
  st_window_watchdog_t(double a_period_min_s, double a_period_max_s);
  virtual void start();
  virtual void restart();
  virtual bool watchdog_reset_cause();
  //! \brief Сбрасывает флаги перезагрузки: LPWRRSTF, WWDGRSTF,
  //!   IWDGRSTF, SFTRSTF
  virtual void clear_reset_status();
private:
  irs_u8 m_counter_start_value;
}; // watchdog_timer_t
#endif // USE_STDPERIPH_DRIVER
#endif  //  definde(IRS_STM32F_2_AND_4)

#ifdef USE_HAL_DRIVER
#ifdef IRS_STM32_F2_F4_F7
//! \brief Драйвер независимого сторожевого таймера для контроллеров
//!   семейств STM32F2xx, STM32F4xx, STM32F7xx
//! \details Работает от независимого генератора с частотой ~32 кГц.
//!   LSI теперь необходимо включать снаружи. Смотри пример кода "Включаем LSI"
//!
//! \code{.cpp}
//!   // Включаем LSI
//!   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//!   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
//!   RCC_OscInitStruct.LSIState = RCC_LSI_ON;
//!   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//!   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
//!     IRS_LIB_ASSERT_MSG("Ошибка RCC_OscInitStruct LSI");
//!   }
//! \endcode
//! \author Lyashchov Maxim
class st_independent_watchdog_t: public watchdog_t
{
public:
  typedef irs_size_t size_type;
  //! \param[in] a_period_s Период срабатывания в секундах. Допустимо указывать
  //!   значения [0, 100]. Однако фактически
  //!   сторожевой таймер работает со следующим диапазоном
  //!   (для LSI = 32 кГц) [0.000125, 32.768]
  st_independent_watchdog_t(double a_period_s = 32);
  virtual void start();
  virtual void restart();
  virtual bool watchdog_reset_cause();
  //! \brief Сбрасывает флаги перезагрузки: LPWRRSTF, WWDGRSTF,
  //!   IWDGRSTF, SFTRSTF
  virtual void clear_reset_status();
private:
  double m_period_s;
  irs_u8 m_counter_start_value;
  IWDG_HandleTypeDef m_iwdg_handle;
}; // st_independent_watchdog_t
#endif // IRS_STM32_F2_F4_F7
#endif // USE_HAL_DRIVER

#ifdef USE_STDPERIPH_DRIVER
#ifdef IRS_STM32F_2_AND_4
class st_rtc_t
{
public:
  enum clock_source_t {
    clock_source_lsi,
    clock_source_lse
  };
  static st_rtc_t* reset(clock_source_t a_clock_source);
  static st_rtc_t* get_instance();
  time_t get_time() const;
  double get_time_double() const;
  void set_time(const time_t a_time);
  //! \param[in] a_koefficient - множитель, на который необходимо домножить
  //!   частоту, чтобы получить заданную. Если a_koefficient == 1, то
  //!   калибровка отсутствует.
  void set_calibration(double a_koefficient);
  double get_calibration() const;
  double get_calibration_coefficient_min() const;
  double get_calibration_coefficient_max() const;
private:
  st_rtc_t(clock_source_t a_clock_source);
  st_rtc_t();
  st_rtc_t(const st_rtc_t& a_st_rtc);
  st_rtc_t& operator=(const st_rtc_t& a_st_rtc);
  void rtc_config(clock_source_t a_clock_source);

  /**
  * @brief  Writes data to all Backup data registers.
  * @param  FirstBackupData: data to write to first backup data register.
  * @retval None
  */
  void write_to_backup_reg(irs_u16 a_first_backup_data);
  enum { backup_first_data = 0x32F2 };
  enum { rtc_bkp_dr_number = 0x14 };
  irs_u32 bkp_data_reg[rtc_bkp_dr_number];
  static handle_t<st_rtc_t> mp_st_rtc;
};
#endif // IRS_STM32F_2_AND_4
#endif // USE_STDPERIPH_DRIVER

#ifdef USE_HAL_DRIVER
#ifdef IRS_STM32_F2_F4_F7

//! \brief Этот класс позволяет получать время из микроконтронтроллеров STM32
//! \details Для его использования необходимо реализовать в проекте функции
//!    HAL_RTC_MspInit, HAL_RTC_MspDeIni. Их можно найти в примерах к отладочным
//!   платам
class st_rtc_t
{
public:
  /*enum clock_source_t {
    clock_source_lsi,
    clock_source_lse
  };*/
  static st_rtc_t* reset();
  static st_rtc_t* get_instance();
  time_t get_time();
  double get_time_double();
  void set_time(const time_t a_time);
  //! \param[in] a_koefficient - множитель, на который необходимо домножить
  //!   частоту, чтобы получить заданную. Если a_koefficient == 1, то
  //!   калибровка отсутствует.
  bool set_calibration(double a_coefficient);
  double get_calibration() const;
  double get_calibration_coefficient_min() const;
  double get_calibration_coefficient_max() const;

  void tick();
  //! \brief Запускает калибровку с указанной длительность. Чем больше интервал
  //!   времени, в течение которого происходит калибровка, тем точнее калибровка
  void start_calibration(const double a_interval_s = 180);
  //! \brief Возвращает \с false, если происходит калибровка
  bool calibration_completed() const;
  //! \brief Задает калибровочный коэффициент для
  //!   глобального счетчика (irs::get_counter()).
  //! \details Используется для коррекции интервалов времени,
  //!   измеренных с помощью irs::get_counter() и зависимых от нее классов
  void set_counter_calibration(double a_coefficient);
  double get_counter_calibration() const;
  size_t get_backup_reg_count() const;
  irs_u32 read_from_backup_reg(irs_u32 a_index);
  void write_to_backup_reg(irs_u32 a_index, irs_u32 a_data);
private:
  st_rtc_t();
  st_rtc_t(const st_rtc_t& a_st_rtc);
  st_rtc_t& operator=(const st_rtc_t& a_st_rtc);
  void rtc_config_default();
  void rtc_config_calibration();

  #if defined(IRS_STM32F4xx) || defined(IRS_STM32F7xx)
  bool set_calibration_force(double a_coefficient);
  #endif // defined(IRS_STM32F4xx) || defined(IRS_STM32F7xx)
  enum { backup_init_data = 0x32F2 };

  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F4xx)
  enum { rtc_bkp_dr_number = 0x14 };
  #elif defined(IRS_STM32F7xx)
  enum { rtc_bkp_dr_number = 0x20 };
  #endif // defined(IRS_STM32F7xx)

  irs_u32 m_prediv_s;

  irs_u32 bkp_data_reg[rtc_bkp_dr_number];
  RTC_HandleTypeDef RtcHandle;
  static handle_t<st_rtc_t> mp_st_rtc;

  // Для калибровки
  enum calibration_process_t {
    cp_off,
    cp_measure_time,
    cp_sync
  };
  calibration_process_t m_calibration_process;
  irs::measure_time_t m_measure_time;
  double m_rtc_time_start;

  irs::measure_time_t m_calibration_time;
  double m_calibration_interval;
  double m_rtc_time_begin;

  time_t m_previous_time_s;
  double m_counter_calibr_coeff;
};

//! \brief Класс для чтения/записи в backup-регистры RTC
class rtc_backup_reg_page_mem_t: public page_mem_t
{
public:
  typedef size_t size_type;
  rtc_backup_reg_page_mem_t(st_rtc_t* ap_st_rtc, size_type a_page_size);
  virtual void read_page(irs_u8 *ap_buf, irs_uarc a_index);
  virtual void write_page(const irs_u8 *ap_buf, irs_uarc a_index);
  virtual size_type page_size() const;
  virtual irs_uarc page_count() const;
  virtual irs_status_t status() const;
  virtual void tick();
private:
  typedef irs_u32 reg_type;
  enum {
    reg_size = sizeof(reg_type)
  };
  st_rtc_t* mp_st_rtc;
  irs_u8* mp_pos;
  size_type m_size;
  size_type m_page_size;
  size_type m_page_count;
};

class rtc_backup_reg_data_t : public mxdata_comm_t
{
public:
  typedef comm_data_t::size_type size_type;
  rtc_backup_reg_data_t(st_rtc_t* ap_st_rtc, irs_uarc a_index, irs_uarc a_size,
    size_type a_cluster_size = 64);
  bool error();
private:
  rtc_backup_reg_page_mem_t m_page_mem;
  irs::mem_data_t m_mem_data;
};

#endif // IRS_STM32_F2_F4_F7
#endif // USE_HAL_DRIVER

//! @}

} //  arm

#endif  //  __ICCARM__

class pwm_pin_t: public gpio_pin_t
{
public:
  pwm_pin_t(irs::handle_t<pwm_gen_t> ap_pwm_gen);
  virtual bool pin();
  virtual void set();
  virtual void clear();
  virtual void set_dir(dir_t a_dir);
  void set_state(io_pin_value_t a_value);
private:
  irs::handle_t<pwm_gen_t> mp_pwm_gen;
  bool m_started;
};

class decoder_t
{
public:
  typedef irs_size_t size_type;
  decoder_t();
  /*#ifdef IRS_STM32_F2_F4_F7
  void add(gpio_channel_t a_channel);
  #endif // IRS_STM32_F2_F4_F7*/
  void add(irs::handle_t<gpio_pin_t> ap_gpio_pin);
  void select_pin(irs_u32 a_pin_index);
  irs_u32 get_selected_pin();
private:
  vector<irs::handle_t<gpio_pin_t> > m_pins;
  irs_u32 m_pin_index;
};

} //  irs

#endif  //  irsdevH
