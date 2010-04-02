// Драйвер Ethernet для RTL8019AS 
// Дата: 25.03.2010
// Дата создания: 15.03.2010

#ifndef IRSRTLH
#define IRSRTLH

#include <irsdefs.h>

#include <irsstd.h>
#include <irstcpip.h>
#include <irsavrutil.h>
#include <irsconfig.h>

#include <irsfinal.h>

namespace irs {

class rtl8019as_t: public simple_ethernet_t
{
public:
  typedef rtl8019as_t this_type;
  typedef simple_ethernet_t::buffer_num_t buffer_num_t;
  
  enum {
    ETHERNET_PACKET_MAX = 1554,
    ETHERNET_PACKET_MIN = 64
  };
  enum {
    IORB = 5,
    IOWB = 6,
    RSTDRV = 7,
    rstport = 0x18,
    isr = 0x07,
    cr = 0x0,
    dcrval = 0x58,
    dcr = 0x0e,
    rbcr0 = 0x0a,
    rbcr1 = 0x0b,
    rcr = 0x0c,
    tpsr = 0x04,
    txtstart = 0x40,
    tcr = 0x0d,
    pstart = 0x01,
    rxstart = 0x46,
    bnry = 0x03,
    pstop = 0x02,
    rxstop = 0x60,
    curr = 0x07,
    imr = 0x0f,
    imrval = 0x1b,
    tcrval = 0x00,
    rsar0 = 0x8,
    rsar1 = 0x9,
    tbcr0 = 0x5,
    tbcr1 = 0x6,
    rdmaport = 0x10,
    rdc = 0x40,
    mac_size = 6,
    par0 = 0x01
  };
  
  rtl8019as_t(
    buffer_num_t a_buf_num,
    size_t a_buf_size,
    irs_avr_port_t a_data_port,
    irs_avr_port_t a_address_port,
    mxmac_t a_mac
  );
  virtual ~rtl8019as_t();
  virtual void send_packet(irs_u16 a_size);
  virtual void set_recv_handled();
  virtual bool is_recv_buf_filled();
  virtual irs_u8* get_recv_buf();
  virtual irs_u8* get_send_buf();
  virtual irs_size_t recv_buf_size();
  virtual irs_size_t send_buf_max_size();
  virtual buffer_num_t get_buf_num();
  virtual mxmac_t get_local_mac();
  virtual void tick();
  #ifdef SERGEY_OFF_INT4
  void rtl_interrupt();
  #endif //SERGEY_OFF_INT4
  
private:
  buffer_num_t m_buf_num;
  size_t m_size_buf;
  raw_data_t<irs_u8> m_recv_buf;
  raw_data_t<irs_u8> m_send_buf;
  mxmac_t m_mac;
  #ifndef SERGEY_OFF_INT4
  event_connect_t<this_type> m_rtl_interrupt_event;
  #endif //SERGEY_OFF_INT4
  bool m_recv_status;
  bool m_send_status;
  size_t m_recv_buf_size;
  irs_u8* mp_recv_buf;
  irs_u8* mp_send_buf;
  blink_t m_blink_15;
  blink_t m_blink_16;
  blink_t m_blink_17;
  blink_t m_blink_18;
  blink_t m_blink_19;
  blink_t m_blink_20;
  
  #ifndef SERGEY_OFF_INT4
  void rtl_interrupt();
  #endif //SERGEY_OFF_INT4
  irs_u8 read_rtl(irs_u8 a_reg_addr);
  void write_rtl(irs_u8 a_reg_addr, irs_u8 a_reg_data);
  void init_rtl();
  bool wait_dma();
  void overrun();
  void recv_packet();
};

} //namespace irs

#endif //IRSRTLH
