//! \file
//! \ingroup drivers_group
//! \brief Интерфейс Ethernet (MAC) для ARM
//!
//! Дата создания: 20.05.2010

#ifndef armethH
#define armethH

#include <irsdefs.h>

#include <irstcpip.h>

#include <armioregs.h>
#include <armregs_stm32f2xx.h>
#include <armiomacros.h>

#if defined(IRS_STM32F2xx)
# include <stm32f2xx.h>
# include <stm32f2x7_eth.h>
#elif defined(IRS_STM32F4xx)
# include <stm32f4xx.h>
# include <stm32f4x7_eth.h>
#endif // defined(IRS_STM32F4xx)

#include <irsfinal.h>

//#define NEW_21092011
#define ARMETH_DBGMSG

namespace irs {

namespace arm {

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__) || defined(__STM32F100RBT__)
//! \brief Драйвер Ethernet для контроллеров LM3SxBxx и LM3Sx9xx
//! \author Polyakov Maxim
class arm_ethernet_t : public simple_ethernet_t
{
public:
  enum use_int_t
  {
    USE_INT,
    NO_USE_INT
  };
  arm_ethernet_t(
    buffer_num_t a_buf_num,
    size_t a_buf_size,
    mxmac_t a_mac,
    use_int_t a_use_interrupt = NO_USE_INT);
  virtual ~arm_ethernet_t();
  virtual void send_packet(irs_size_t a_size);
  virtual void set_recv_handled();
  virtual void set_send_buf_locked();
  virtual bool is_recv_buf_filled() const;
  virtual bool is_send_buf_empty() const;
  virtual irs_u8* get_recv_buf();
  virtual irs_u8* get_send_buf();
  virtual irs_size_t recv_buf_size() const;
  virtual irs_size_t send_buf_max_size() const;
  virtual buffer_num_t get_buf_num() const;
  virtual mxmac_t get_local_mac() const;
  virtual void set_mac(mxmac_t& a_mac);
  virtual void tick();
private:
  enum
  {
    // Размер поля адреса приемника
    DA_size = mac_length,
    // Размер поля адреса источника
    SA_size = mac_length,
    // Размер поля длины пакета
    L_size = 2,
    // Размер контрольной суммы
    FCS_size = 4,
    // Добавочный размер для выравнивания на 4 байта
    align_size = 4,
    // Минимальный размер пакета в FIFO
    fifo_min_size = L_size + DA_size + SA_size
  };
  enum
  {
    max_packet_size = 2032
  };
  enum {
    PORTF2_LED1 = 0x100,
    PORTF3_LED0 = 0x1000
  };

  buffer_num_t m_buf_num;
  #ifndef NEW_21092011
  irs_size_t m_buf_size;
  #endif //NEW_21092011
  irs_u8* mp_rx_buf;
  irs_u8* mp_tx_buf;
  bool m_rx_buf_handled;
  bool m_rx_buf_filled;
  irs_size_t m_rx_size;
  bool m_rx_int_flag;
  event_connect_t<arm_ethernet_t> m_rx_int_event;
  const use_int_t m_use_interrupt;
  bool m_send_buf_locked;
  bool m_send_packet_action;
  irs_size_t m_rx_buf_size;
  irs_size_t m_tx_buf_size;

  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  inline void rx_interrupt()
  {
    MACIS_bit.RXINT = 1;
    MACIM_bit.RXINT = 0;
    m_rx_int_flag = true;
  }
  inline void set_fifo(irs_u32 data) { MACDATA = data; }
  inline irs_u32 get_fifo() { return MACDATA; }
  inline void send_packet() { MACTR_bit.NEWTX = 1; }
  inline bool tx_buf_empty() const { return (MACTR_bit.NEWTX == 0); }
  inline bool packets_in_mac() { return (MACNP_bit.NPR > 0); }
  inline void reset_fifo() { MACRCTL_bit.RSTFIFO = 1; }
  inline void clear_rx_interrupt() { MACIM_bit.RXINT = 1; }
  #elif defined(__STM32F100RBT__)
  inline void rx_interrupt() {}
  inline void set_fifo(irs_u32 /*data*/) {}
  inline irs_u32 get_fifo() { return 0; }
  inline void send_packet() { }
  inline bool tx_buf_empty() const { return false; }
  inline bool packets_in_mac() { return false; }
  inline void reset_fifo() {}
  inline void clear_rx_interrupt() {}
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
};

#elif defined IRS_STM32F_2_AND_4

//! \brief Драйвер Ethernet для контроллеров STM32F2xx и STM32F4xx
//! \author Lyashchov Maxim
class st_arm_ethernet_t: public simple_ethernet_t
{
public:
  typedef irs_size_t size_type;
  enum mii_mode_t {
    normal_mii_mode,
    reduced_mii_mode
  };
  struct config_t
  {
    enum { txd_count = 4 };
    enum { rxd_count = 4 };
    mii_mode_t mii_mode;
    gpio_channel_t rx_clk_or_ref_clk;
    gpio_channel_t txd[txd_count];
    gpio_channel_t tx_en;
    gpio_channel_t tx_clk;
    gpio_channel_t rxd[rxd_count];
    gpio_channel_t rx_er;
    gpio_channel_t rx_dv_or_crs_dv;
    gpio_channel_t rx_crs;
    gpio_channel_t col;
    gpio_channel_t mdc;
    gpio_channel_t mdio;
    config_t():
      mii_mode(reduced_mii_mode),
      rx_clk_or_ref_clk(PA1),
      txd(),
      tx_en(PB11),
      tx_clk(PC3),
      rxd(),
      rx_er(PB10),
      rx_dv_or_crs_dv(PA7),
      rx_crs(PA0),
      col(PA3),
      mdc(PC1),
      mdio(PA2)
    {
      txd[0] = PG13;
      txd[1] = PG14;
      txd[2] = PC2;
      txd[3] = PE2;

      rxd[0] = PC4;
      rxd[1] = PC5;
      rxd[2] = PB0;
      rxd[3] = PB1;
    }
  };
  st_arm_ethernet_t(
    size_t a_buf_size,
    mxmac_t& a_mac, const config_t a_config);
  virtual ~st_arm_ethernet_t();
  virtual void send_packet(irs_size_t a_size);
  virtual void set_recv_handled();
  virtual void set_send_buf_locked();
  virtual bool is_recv_buf_filled() const;
  virtual bool is_send_buf_empty() const;
  virtual irs_u8* get_recv_buf();
  virtual irs_u8* get_send_buf();
  virtual irs_size_t recv_buf_size() const;
  virtual irs_size_t send_buf_max_size() const;
  virtual buffer_num_t get_buf_num() const;
  virtual mxmac_t get_local_mac() const;
  virtual void set_mac(mxmac_t& a_mac);
  virtual void tick();
private:
  void rcc_configuration();
  void gpio_configuration();
  void transmit();
  void receive();

  enum { phy_address = 0x1 } ;
  enum { frame_check_sequence_size = 4 };
  enum buf_status_t {
    buf_empty,
    buf_locked,
    buf_filled
  };
  struct buf_t
  {
    vector<irs_u8> buf;
    size_type data_size;
    buf_status_t status;
    buf_t(size_type a_buf_size):
      buf(a_buf_size, 0),
      data_size(0),
      status(buf_empty)
    {
    }
    inline irs_u8* begin()
    {
      return buf.empty() ? IRS_NULL : &buf.front();
    }
    inline size_type size() const
    {
      return buf.size();
    }
  };
  buf_t m_receive_buf;
  buf_t m_transmit_buf;
  config_t m_config;
};
#else
  #error Тип контроллера не определён
#endif // mcu type
} //  arm

} //  irs

#endif  //  armethH
