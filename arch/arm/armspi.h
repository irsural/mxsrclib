//! \file
//! \ingroup drivers_group
//! \brief Драйвер SPI для ARM
//!
//! Дата: 16.03.2011
//! Дата создания: 30.11.2010

#ifndef armspiH
#define armspiH

#include <irsdefs.h>

#include <irsspi.h>
#include <mxdata.h>
#include <armioregs.h>

#include <irsfinal.h>

namespace irs {

namespace arm {

//! \addtogroup drivers_group
//! @{
#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
class arm_spi_t: public spi_t
{
public:
  enum ssi_type_t {
    SSI0,
    SSI1
  };
  enum spi_type_t {
    SPI = 0x0,
    TISS = 0x1, // Texas Instruments Synchronous Serial
    MICROWIRE = 0x2
  };

  arm_spi_t(
    irs_u32 a_bitrate,
    spi_type_t a_spi_type = SPI,
    ssi_type_t a_ssi_type = SSI0,
    arm_port_t& a_clk_port = GPIO_PORTH,
    arm_port_t& a_rx_port = GPIO_PORTH,
    arm_port_t& a_tx_port = GPIO_PORTH
  );
  virtual ~arm_spi_t();
  virtual void abort();
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size);
  virtual status_t get_status();
  virtual void lock();
  virtual void unlock();
  virtual bool get_lock();
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(polarity_t a_polarity);
  virtual bool set_phase(phase_t a_phase);
  // Контроллер поддерживает только MSB
  virtual bool set_order(order_t /*a_order*/);
  virtual bool set_data_size(irs_u16 a_data_size);
  virtual void tick();
  virtual void read_write(irs_u8 *ap_read_buf, const irs_u8 *ap_write_buf,
    irs_uarc a_size);
private:
  enum cur_status_t {
    SPI_FREE,
    SPI_READ,
    SPI_WRITE,
    SPI_RW_READ,
    SPI_RW_WRITE
  };
  enum {
    SSI0Clk = 0x1,
    SSI0Rx = 0x1,
    SSI0Tx = 0x1,
    SSI1Clk_E = 0x2,
    SSI1Rx_E = 0x2,
    SSI1Tx_E = 0x2,
    SSI1Clk_F = 0x9,
    SSI1Rx_F = 0x9,
    SSI1Tx_F = 0x9,
    SSI1Clk_H = 0xB,
    SSI1Rx_H = 0xB,
    SSI1Tx_H = 0xB
  };
  struct reg_t {
    volatile __ssicr0_bits* mp_SSICR0_bit;
    volatile __ssicr1_bits* mp_SSICR1_bit;
    volatile unsigned short* mp_SSIDR;
    volatile __ssisr_bits* mp_SSISR_bit;
    volatile unsigned char* mp_SSICPSR;
    volatile __ssiim_bits* mp_SSIIM_bit;
    volatile __ssiris_bits* mp_SSIRIS_bit;
    volatile __ssimis_bits* mp_SSIMIS_bit;
    volatile __ssiicr_bits* mp_SSIICR_bit;

    reg_t(ssi_type_t a_ssi_type);
  };
  cur_status_t m_status;
  const irs_u8* mp_write_buf;
  irs_u8* mp_read_buf;
  irs_u8 m_cur_byte;
  irs_uarc m_packet_size;
  const irs_u32 m_bitrate_def;
  const polarity_t m_polarity_def;
  const phase_t m_phase_def;
  const order_t m_order_def;
  const irs_u16 m_data_size_def;
  bool m_buf_empty;
  bool m_lock;
  spi_type_t m_spi_type;
  ssi_type_t m_ssi_type;
  reg_t m_reg;
  bool m_on_prev;

  void write_data_register(irs_u8 a_data);
  irs_u8 read_data_register();
  bool transfer_complete();
  void init_default();
};

#elif defined(__STM32F100RBT__)
#elif defined(IRS_STM32_F2_F4_F7)

class st_spi_base_t: public irs::spi_t
{
public:
  enum gpio_speed_t {
    gpio_speed_2mhz = IRS_GPIO_SPEED_2MHZ,
    gpio_speed_25mhz = IRS_GPIO_SPEED_25MHZ,
    gpio_speed_50mhz = IRS_GPIO_SPEED_50MHZ,
    gpio_speed_100mhz = IRS_GPIO_SPEED_100MHZ
  };
protected:
  static void clean_buf_from_old_data(spi_regs_t* ap_spi_regs);
  static void _disable_spi(spi_regs_t* ap_spi_regs);
  static bool _set_bitrate(spi_regs_t* ap_spi_regs, irs_u32 a_bitrate);
  static bool _set_polarity(spi_regs_t* ap_spi_regs, polarity_t a_polarity);
  static bool _set_phase(spi_regs_t* ap_spi_regs, phase_t a_phase);
  static bool _set_order(spi_regs_t* ap_spi_regs, order_t a_order);
  static void initialize_gpio_channels(spi_regs_t* ap_spi_regs,
    gpio_channel_t a_sck,
    gpio_channel_t a_miso,
    gpio_channel_t a_mosi,
    int a_gpio_speed);
  static void set_moder_alternate_function(gpio_channel_t a_channel);
};

#ifdef USE_STDPERIPH_DRIVER

#if (defined(IRS_STM32F2xx) || defined(IRS_STM32F4xx))
class st_spi_dma_t: public st_spi_base_t
{
public:
  typedef size_t size_type;
  struct settings_t
  {
    size_type spi_address;
    size_type dma_address;
    irs_u32 bitrate;
    gpio_channel_t sck;
    gpio_channel_t miso;
    gpio_channel_t mosi;
    gpio_speed_t gpio_speed;
    irs::c_array_view_t<irs_u8> rx_buffer;
    //! \brief Канал DMA RX. Использовать константы: DMA1_Stream0 - DMA2_Stream7
    DMA_Stream_TypeDef* rx_dma_y_stream_x;
    //! \brief Канал DMA для RX.
    //!   Использовать константы: DMA1_Stream0 - DMA1_Stream7
    uint32_t rx_dma_channel;
    //! \brief Приоритет. Использовать константы:
    //!   DMA_Priority_Low - DMA_Priority_VeryHigh
    uint32_t rx_dma_priority;

    irs::c_array_view_t<irs_u8> tx_buffer;
    //! \brief Канал DMA TX. Использовать константы: DMA1_Stream0 - DMA2_Stream7
    DMA_Stream_TypeDef* tx_dma_y_stream_x;
    //! \brief Канал DMA для TX.
    //!   Использовать константы: DMA1_Stream0 - DMA1_Stream7
    uint32_t tx_dma_channel;
    //! \brief Приоритет. Использовать константы:
    //!   DMA_Priority_Low - DMA_Priority_VeryHigh
    uint32_t tx_dma_priority;
    settings_t():
      spi_address(NULL),
      dma_address(NULL),
      bitrate(10000),
      sck(PNONE),
      miso(PNONE),
      mosi(PNONE),
      gpio_speed(gpio_speed_2mhz),
      rx_buffer(NULL, 0),
      rx_dma_y_stream_x(0),
      rx_dma_channel(0),
      rx_dma_priority(DMA_Priority_Low),

      tx_buffer(NULL, 0),
      tx_dma_y_stream_x(0),
      tx_dma_channel(0),
      tx_dma_priority(DMA_Priority_Low)
    {
    }
  };

  //! \brief Конструктор
  //!
  //! \param[in] a_settings - настройки
  //!
  //! \details Пример создания SPI для отладочной платы SK-STM32F417:
  //!
  //!   irs::arm::st_spi_dma_t::settings_t spi_settings;
  //!   spi_settings.spi_address = IRS_SPI2_I2S2_BASE;
  //!   spi_settings.dma_address = IRS_DMA1_BASE;
  //!   spi_settings.bitrate = m_fast_spi_bitrate;
  //!   spi_settings.sck = PB10;
  //!   spi_settings.miso = PC2;
  //!   spi_settings.mosi = PC3;
  //!   spi_settings.gpio_speed = irs::arm::st_spi_dma_t::gpio_speed_50mhz;
  //!   spi_settings.rx_dma_channel =DMA_Channel_0;
  //!   spi_settings.rx_dma_y_stream_x = DMA1_Stream3;
  //!   spi_settings.rx_buffer = irs::c_array_view_t<irs_u8>(spi_in_buf, 28);

  //!   spi_settings.tx_dma_channel = DMA_Channel_0;
  //!   spi_settings.tx_dma_y_stream_x = DMA1_Stream4;
  //!   spi_settings.tx_buffer = irs::c_array_view_t<irs_u8>(spi_out_buf, 28);
  //!   rs::arm::st_spi_dma_t spi(spi_settings);
  st_spi_dma_t(
    const settings_t& a_settings
  );
  virtual ~st_spi_dma_t();
  virtual void abort();
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size);
  virtual status_t get_status();
  virtual void lock();
  virtual void unlock();
  virtual bool get_lock();
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(polarity_t a_polarity);
  virtual bool set_phase(phase_t a_phase);
  // Контроллер поддерживает только MSB
  virtual bool set_order(order_t /*a_order*/);
  virtual bool set_data_size(irs_u16 a_data_size);
  virtual void tick();
  virtual void read_write(irs_u8 *ap_read_buf, const irs_u8 *ap_write_buf,
    irs_uarc a_size);
private:
  void initialize_gpio_channels(gpio_channel_t a_sck,
    gpio_channel_t a_miso,
    gpio_channel_t a_mosi,
    int a_gpio_speed);
  void set_default();
  void enable_spi();
  void disable_spi();
  void reset_dma();
  void start_spi_dma();
  void stop_spi_dma();
  size_type calc_max_byte_count() const;
  enum { data_size_default = 8 };
  enum process_t {
    process_wait_command,
    process_read_write
  };
  bool m_lock;
  settings_t m_settings;
  spi_regs_t* mp_spi_regs;
  dma_regs_t* mp_dma;
  dma_regs_t::dma_stream_regs_t* mp_rx_stream;
  dma_regs_t::dma_stream_regs_t* mp_tx_stream;
  DMA_Stream_TypeDef* mp_rx_dma_y_stream_x;
  DMA_Stream_TypeDef* mp_tx_dma_y_stream_x;
  irs::c_array_view_t<irs_u8> m_rx_buffer;
  irs::c_array_view_t<irs_u8> m_tx_buffer;
  irs_u32 m_bitrate_default;
  process_t m_process;
  const irs_u8* mp_write_buf;
  irs_u8* mp_read_buf;
  size_type m_packet_size;
  size_type m_index;
  size_type m_read_buf_index;
  size_type m_data_item_byte_count;
  size_type m_byte_count;
  size_type m_max_byte_count;
};
#elif defined(IRS_STM32F7xx)

#endif // defined(IRS_STM32F7xx)

#endif // USE_STDPERIPH_DRIVER

#ifdef USE_HAL_DRIVER
class st_hal_spi_dma_t: public st_spi_base_t
{
public:
  typedef size_t size_type;
  struct settings_t
  {
    size_type spi_address;
    size_type dma_address;
    irs_u32 bitrate;
    gpio_channel_t sck;
    gpio_channel_t miso;
    gpio_channel_t mosi;
    gpio_speed_t gpio_speed;
    irs::c_array_view_t<irs_u8> rx_buffer;
    //! \brief Канал DMA RX. Использовать константы: DMA1_Stream0 - DMA2_Stream7
    DMA_Stream_TypeDef* rx_dma_y_stream_x;
    //! \brief Канал DMA для RX.
    //!   Использовать константы: DMA_CHANNEL_0 - DMA_CHANNEL_7
    uint32_t rx_dma_channel;
    //! \brief Приоритет. Использовать константы:
    //!   DMA_PRIORITY_LOW - DMA_PRIORITY_VERY_HIGH
    uint32_t rx_dma_priority;

    irs::c_array_view_t<irs_u8> tx_buffer;
    //! \brief Канал DMA TX. Использовать константы: DMA1_Stream0 - DMA2_Stream7
    DMA_Stream_TypeDef* tx_dma_y_stream_x;
    //! \brief Канал DMA для TX.
    //!   Использовать константы: DMA_CHANNEL_0 - DMA_CHANNEL_7
    uint32_t tx_dma_channel;
    //! \brief Приоритет. Использовать константы:
    //!   DMA_PRIORITY_LOW - DMA_PRIORITY_VERY_HIGH
    uint32_t tx_dma_priority;
    settings_t():
      spi_address(NULL),
      dma_address(NULL),
      bitrate(10000),
      sck(PNONE),
      miso(PNONE),
      mosi(PNONE),
      gpio_speed(gpio_speed_2mhz),
      rx_buffer(NULL, 0),
      rx_dma_y_stream_x(0),
      rx_dma_channel(0),
      rx_dma_priority(DMA_PRIORITY_LOW),

      tx_buffer(NULL, 0),
      tx_dma_y_stream_x(0),
      tx_dma_channel(0),
      tx_dma_priority(DMA_PRIORITY_LOW)
    {
    }
  };

  //! \brief Конструктор
  //!
  //! \param[in] a_settings - настройки
  //!
  //! \details Пример создания SPI для отладочной платы SK-STM32F417:
  //!
  //!   irs::arm::st_hal_spi_dma_t::settings_t spi_settings;
  //!   spi_settings.spi_address = IRS_SPI2_I2S2_BASE;
  //!   spi_settings.dma_address = IRS_DMA1_BASE;
  //!   spi_settings.bitrate = m_fast_spi_bitrate;
  //!   spi_settings.sck = PB10;
  //!   spi_settings.miso = PC2;
  //!   spi_settings.mosi = PC3;
  //!   spi_settings.gpio_speed = irs::arm::st_hal_spi_dma_t::gpio_speed_50mhz;
  //!   spi_settings.rx_dma_channel =DMA_Channel_0;
  //!   spi_settings.rx_dma_y_stream_x = DMA1_Stream3;
  //!   spi_settings.rx_buffer = irs::c_array_view_t<irs_u8>(spi_in_buf, 28);

  //!   spi_settings.tx_dma_channel = DMA_Channel_0;
  //!   spi_settings.tx_dma_y_stream_x = DMA1_Stream4;
  //!   spi_settings.tx_buffer = irs::c_array_view_t<irs_u8>(spi_out_buf, 28);
  //!   rs::arm::st_hal_spi_dma_t spi(spi_settings);
  st_hal_spi_dma_t(
    const settings_t& a_settings
  );
  virtual ~st_hal_spi_dma_t();
  virtual void abort();
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size);
  virtual status_t get_status();
  virtual void lock();
  virtual void unlock();
  virtual bool get_lock();
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(polarity_t a_polarity);
  virtual bool set_phase(phase_t a_phase);
  // Контроллер поддерживает только MSB
  virtual bool set_order(order_t /*a_order*/);
  virtual bool set_data_size(irs_u16 a_data_size);
  virtual void tick();
  virtual void read_write(irs_u8 *ap_read_buf, const irs_u8 *ap_write_buf,
    irs_uarc a_size);
private:
  void initialize_gpio_channels(gpio_channel_t a_sck,
    gpio_channel_t a_miso,
    gpio_channel_t a_mosi,
    int a_gpio_speed);
  void set_default();
  void enable_spi();
  void disable_spi();
  void reset_dma();
  void start_spi_dma();
  void stop_spi_dma();
  size_type calc_max_byte_count() const;
  void cleand_dcache_tx();
  void invalidate_dcache_rx();

  enum { data_size_default = 8 };
  enum process_t {
    process_wait_command,
    process_read_write
  };
  bool m_lock;
  settings_t m_settings;
  spi_regs_t* mp_spi_regs;
  dma_regs_t* mp_dma;

  SPI_HandleTypeDef m_spi_handle;
  DMA_HandleTypeDef m_hdma_tx;
  DMA_HandleTypeDef m_hdma_rx;
  bool m_tx_status;
  bool m_rx_status;
  bool m_hdma_init;

  irs::c_array_view_t<irs_u8> m_rx_buffer;
  irs::c_array_view_t<irs_u8> m_tx_buffer;
  irs_u32 m_bitrate_default;
  process_t m_process;
  const irs_u8* mp_write_buf;
  irs_u8* mp_read_buf;
  size_type m_packet_size;
  size_type m_index;
  size_type m_read_buf_index;
  size_type m_data_item_byte_count;
  size_type m_byte_count;
  size_type m_max_byte_count;
};
#endif // USE_HAL_DRIVER

class arm_spi_t:  public st_spi_base_t//public spi_t
{
public:
  //! \brief Конструктор
  //!
  //! \param[in] a_spi_address - адрес
  //! \param[in] a_bitrate - битрейт
  //! \param[in] a_sck - ножка контроллера SCK
  //! \param[in] a_miso - ножка контроллера MISO
  //! \param[in] a_mosi - ножка контроллера MOSI
  //! \param[in] a_gpio_speed - скорость ножек, подбирается опытным путем
  //!
  //! \details Пример создания SPI для отладочной платы SK-STM32F417:
  //!   arm_spi_t spi(IRS_SPI1_BASE, 500000, PA5, PA6, PB5,
  //!     irs::arm::arm_spi_t::gpio_speed_100mhz);
  arm_spi_t(
    size_t a_spi_address,
    irs_u32 a_bitrate,
    gpio_channel_t a_sck,
    gpio_channel_t a_miso,
    gpio_channel_t a_mosi,
    gpio_speed_t a_gpio_speed = gpio_speed_2mhz
  );
  virtual ~arm_spi_t();
  virtual void abort();
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size);
  virtual status_t get_status();
  virtual void lock();
  virtual void unlock();
  virtual bool get_lock();
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(polarity_t a_polarity);
  virtual bool set_phase(phase_t a_phase);
  // Контроллер поддерживает только MSB
  virtual bool set_order(order_t /*a_order*/);
  virtual bool set_data_size(irs_u16 a_data_size);
  virtual void tick();
  virtual void read_write(irs_u8 *ap_read_buf, const irs_u8 *ap_write_buf,
    irs_uarc a_size);
private:
  void initialize_gpio_channels(gpio_channel_t a_sck,
    gpio_channel_t a_miso,
    gpio_channel_t a_mosi,
    int a_gpio_speed);
  void set_default();
  void enable_spi();
  void disable_spi();

  inline void writeToDR();
  inline void readFromDR();

  template <class T>
  inline void writeToDR(const irs_u8* ap_buf, size_t a_pos);
  template <class T>
  inline void readFromDR(irs_u8* ap_buf, size_t a_pos);

  enum { data_size_default = 8 };
  enum process_t {
    process_wait_command,
    process_read_write
  };
  bool m_lock;
  spi_regs_t* mp_spi_regs;
  irs_u32 m_bitrate_default;
  process_t m_process;
  const irs_u8* mp_write_buf;
  irs_u8* mp_read_buf;
  irs_uarc m_packet_size;
  irs_uarc m_write_buf_index;
  irs_uarc m_read_buf_index;
  irs_u32 m_data_item_byte_count;
};

template <class T>
void arm_spi_t::writeToDR(const irs_u8* ap_buf, size_t a_pos)
{
  if (ap_buf) {
    *reinterpret_cast<T*>(&mp_spi_regs->SPI_DR) =
      *reinterpret_cast<const T*>(&ap_buf[a_pos]);
  } else {
    *reinterpret_cast<T*>(&mp_spi_regs->SPI_DR) = T(0);
  }
}

template <class T>
void arm_spi_t::readFromDR(irs_u8* ap_buf, size_t a_pos)
{
  if (ap_buf) {
    *reinterpret_cast<T*>(&ap_buf[a_pos]) =
      *reinterpret_cast<T*>(&mp_spi_regs->SPI_DR);
  } else {
    volatile T data = *reinterpret_cast<T*>(&mp_spi_regs->SPI_DR);
  }
}

inline void arm_spi_t::writeToDR()
{
  const size_t pos = m_write_buf_index*m_data_item_byte_count;
  if (m_data_item_byte_count == 1) {
    writeToDR<irs_u8>(mp_write_buf, pos);
  } else {
    writeToDR<irs_u16>(mp_write_buf, pos);
  }
  m_write_buf_index++;
}

inline void arm_spi_t::readFromDR()
{
  const size_t pos = m_read_buf_index*m_data_item_byte_count;
  if (m_data_item_byte_count == 1) {
    readFromDR<irs_u8>(mp_read_buf, pos);
  } else {
    readFromDR<irs_u16>(mp_read_buf, pos);
  }
  m_read_buf_index++;

}

#else
  #error Тип контроллера не определён
#endif  //  mcu type

//! @}

} // namespace arm

} // namespace irs

#endif // armspiH
