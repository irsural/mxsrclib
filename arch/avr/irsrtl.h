//! \file
//! \ingroup drivers_group
//! \brief Драйвер Ethernet для RTL8019AS
//!
//! Дата: 01.07.2010\n
//! Дата создания: 15.03.2010

#ifndef IRSRTLH
#define IRSRTLH

#include <irsdefs.h>

#include <irsstd.h>
#include <irstcpip.h>
#include <irsconfig.h>
#include <timer.h>

#include <irsfinal.h>

//#define IRS_LIB_IRSRTL_DEBUG_TYPE IRS_LIB_DEBUG_BASE

#ifdef IRS_LIB_IRSRTL_DEBUG_TYPE
#if (IRS_LIB_IRSRTL_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
# define IRS_LIB_RTL_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
#endif
#else // IRS_LIB_IRSRTL_DEBUG_TYPE
#define IRS_LIB_RTL_DBG_RAW_MSG_BASE(msg)
#endif // IRS_LIB_IRSRTL_DEBUG_TYPE


namespace irs {

//! \addtogroup drivers_group
//! @{

struct rtl_port_str_t
{
  p_avr_port_t rtl_data_port_set;
  p_avr_port_t rtl_data_port_get;
  p_avr_port_t rtl_data_port_dir;
  p_avr_port_t rtl_address_port_set;
  p_avr_port_t rtl_address_port_get;
  p_avr_port_t rtl_address_port_dir;
};

//! \brief Драйвер Ethernet для RTL8019AS
//! \author Sergeev Sergey
class rtl8019as_t: public simple_ethernet_t
{
public:
  typedef rtl8019as_t this_type;
  typedef simple_ethernet_t::buffer_num_t buffer_num_t;
  
  enum {
    //! \brief Регистры
    CR = 0x0,
    PAR0 = 0x01,
    
    PSTART = 0x01,
    PSTOP = 0x02,
    BNRY = 0x03,
    //! \brief Адресс страницы в которой находится пакет для передачи
    TPSR = 0x04,
    TBCR0 = 0x05,
    TBCR1 = 0x06,
    ISR = 0x07,
    RSAR0 = 0x08,
    RSAR1 = 0x09,
    RBCR0 = 0x0a,
    RBCR1 = 0x0b,
    RCR = 0x0c,
    TCR = 0x0d,
    DCR = 0x0e,
    IMR = 0x0f,
    
    CURR = 0x07,

    CONFIG3 = 0x06,
      
    RDMAPORT = 0x10,
    RSTPORT = 0x18,
    
    //! \brief Биты CR
    STP = 0,
    STA = 1,
    TXP = 2,
    RD0 = 3,
    RD1 = 4,
    RD2 = 5,
    PS0 = 6,
    PS1 = 7,
    cr_page0 = ((0 << PS1)|(0 << PS0)),
    cr_page1 = ((0 << PS1)|(1 << PS0)),
    cr_page2 = ((1 << PS1)|(0 << PS0)),
    cr_page3 = ((1 << PS1)|(1 << PS0)),
    cr_remote_read = ((0 << RD2)|(0 << RD1)|(1 << RD0)),
    cr_remote_write = ((0 << RD2)|(1 << RD1)|(0 << RD0)),
    cr_remote_send = ((0 << RD2)|(1 << RD1)|(1 << RD0)),
    cr_dma_abort_complete = ((1 << RD2)|(1 << RD1)|(1 << RD0)),
    cr_rtl_start = ((1 << STA)|(0 << STP)),
    cr_rtl_stop = ((0 << STA)|(1 << STP)),

    //! \brief Биты ISR
    RDC = 0x40,
    
    //! \brief Биты DCR
    WTS = 0,
    BOS = 1,
    LAS = 2,
    LS = 3,
    ARM = 4,
    FT0 = 5,
    FT1 = 6,
    
    //! \brief Биты CONFIG3
    //! \brief Тип 0-го светодиода
    LEDS0 = 4,
    
    //! \brief Готовые значения для регистров
    //dcrval = 0x58,
    dcrval = ((1 << LS)|(1 << ARM)|(1 << FT1)),
    //! \brief Адрес для TPSR
    txstart = 0x40,
    rxstart = 0x46,
    rxstop = 0x60,
    imrval = 0x1b,
    tcrval = 0x00
  };
  //! \brief Пины AVR для RTL
  enum {
    IORB = 5,
    IOWB  = 6,
    RSTDRV = 7
  };
  enum {
    ETHERNET_PACKET_MAX = 1554,
    ETHERNET_PACKET_MIN = 64
  };
  enum { mac_size = 6};
  
  rtl8019as_t(
    buffer_num_t a_buf_num,
    size_t a_buf_size,
    irs_avr_port_t a_data_port,
    irs_avr_port_t a_address_port,
    mxmac_t a_mac,
    counter_t a_recv_timeout_cnt = make_cnt_s(2)
  );
  virtual ~rtl8019as_t();
  virtual void send_packet(irs_size_t a_size);
  virtual void set_recv_handled();
  virtual void set_send_buf_locked();
  virtual bool is_recv_buf_filled();
  virtual bool is_send_buf_empty();
  virtual irs_u8* get_recv_buf();
  virtual irs_u8* get_send_buf();
  virtual irs_size_t recv_buf_size();
  virtual irs_size_t send_buf_max_size();
  virtual buffer_num_t get_buf_num();
  virtual mxmac_t get_local_mac();
  virtual void set_mac(mxmac_t& a_mac);
  virtual void tick();
  
private:
  buffer_num_t m_buf_num;
  size_t m_size_buf;
  raw_data_t<irs_u8> m_recv_buf;
  raw_data_t<irs_u8> m_send_buf;
  mxmac_t m_mac;
  //event_connect_t<this_type> m_rtl_interrupt_event;
  bool m_recv_buf_locked;
  size_t m_recv_buf_size;
  irs_u8* mp_recv_buf;
  irs_u8* mp_send_buf;
  timer_t m_recv_timeout;
  bool m_send_buf_empty;
  irs_u8 m_FIFO_new_packets;
  irs_u8 m_cur_pack_pointer;
  irs_u8 m_next_pack_pointer;
  rtl_port_str_t m_rtl_port_str;
  
  void rtl_interrupt();
  irs_u8 read_rtl(irs_u8 a_reg_addr);
  void write_rtl(irs_u8 a_reg_addr, irs_u8 a_reg_data);
  void init_rtl();
  bool wait_dma();
  void overrun();
  void recv_packet();
  bool new_rx_packet();
};

//! @}

} //namespace irs

#endif //IRSRTLH
