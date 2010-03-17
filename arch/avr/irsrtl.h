// Драйвер Ethernet для RTL8019AS 
// Дата: 17.03.2010
// Дата создания: 15.03.2010

#ifndef IRSRTLH
#define IRSRTLH

#include <irsdefs.h>

#include <irsstd.h>
#include <hardflowg.h>

#include <irsfinal.h>

#define ETHERNET_PACKET_TX 300
#define ETHERNET_PACKET_RX 304
//#define ETHERNET_PACKET_TX 1550
//#define ETHERNET_PACKET_RX 1554

// Длина MAC(Ethernet)-адреса
#define IRS_RTL_MAC_SIZE 6

namespace irs {

namespace hardflow {

class ethernet_rtl_t//: public hardflow_t
{
public:
  typedef ethernet_rtl_t this_type;
  typedef hardflow_t::size_type size_type;
  
  enum {
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
    mac_size = 6
  };
  
  ethernet_rtl_t();
  virtual ~ethernet_rtl_t();
  /*virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual void tick();
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);*/
  void set_rtl_ports(irs_avr_port_t a_data_port, irs_avr_port_t a_address_port);
  void init_rtl(const irs_u8 *ap_mac);
  void send_packet(irs_u16 a_length, irs_u8 *a_tx_buf);
  bool get_rx_hard();
  bool get_tx_hard();
  irs_u8* get_rx_buf();
  irs_u8* get_tx_buf();
  irs_u16 get_rx_len_hard();
  
private:
  raw_data_t<irs_u8> m_rx_buf;
  raw_data_t<irs_u8> m_tx_buf;
  raw_data_t<irs_u8> m_mac_save;
  event_connect_t<this_type> m_rtl_interrupt_event;
  bool m_rx_hard;
  irs_u16 m_rx_len_hard;
  
  void rtl_interrupt();
  irs_u8 read_rtl(irs_u8 a_reg_addr);
  void write_rtl(irs_u8 a_reg_addr, irs_u8 a_reg_data);
  void reset_rtl();
  bool wait_dma();
  void overrun();
  void recv_packet();
};

} //namespace hardflow

} //namespace irs

#endif //IRSRTLH
