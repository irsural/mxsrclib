// UDP/IP-стек 
// Дата: 17.03.2010
// дата создания: 16.03.2010

#ifndef IRSUDPSTACKH
#define IRSUDPSTACKH

#include <irsdefs.h>

#include <irsrtl.h>

#include <irsfinal.h>

// Размер заголовков Ethernet + IP + UDP
#define HEADERS_SIZE 42
// Размер буфера передачи
#define UDP_BUFFER_SIZE_TX (ETHERNET_PACKET_TX - HEADERS_SIZE)
// Размер буфера приема
#define UDP_BUFFER_SIZE_RX (ETHERNET_PACKET_RX - HEADERS_SIZE)

// Размер IP-адреса
#define IRS_UDP_IP_SIZE 4

// Размер MAC-адреса
#define IRS_UDP_MAC_SIZE 6

namespace irs {

struct mac_t 
{
  irs_u8 val[IRS_RTL_MAC_SIZE];
  
  bool operator==(const mac_t& a_mac) const
  {
    return memcmp((void *)val, (void *)a_mac.val, IRS_RTL_MAC_SIZE) == 0;
  }
  bool operator!=(const mac_t& a_mac) const
  {
    return !(val == a_mac.val);
  }
};

struct ip_t
{
  irs_u8 val[IRS_UDP_IP_SIZE];
  
  bool operator==(const ip_t& a_ip) const
  {
    return memcmp((void *)val, (void *)a_ip.val, IRS_UDP_IP_SIZE) == 0;
  }
  bool operator!=(const ip_t& a_ip) const
  {
    return !(val == a_ip.val);
  }
};

const ip_t& zero_ip();
const ip_t& broadcast_ip();

const mac_t& zero_mac();
const mac_t& broadcast_mac();

namespace hardflow {

class simple_udp_t: public hardflow_t
{
public:
  typedef hardflow_t::size_type size_type;
  
  enum {
    ARPBUF_SIZE = 42,
    ARPBUF_SENDSIZE = 60,
    ICMPBUF_SIZE = 200
  };
  
  simple_udp_t(
    ethernet_rtl_t* ap_ethernet_rtl,
    const irs_u8* ap_mac, 
    const irs_u8* ap_ip,
    irs_avr_port_t a_data_port,
    irs_avr_port_t a_address_port
  );
  virtual ~simple_udp_t();
  void open_udp(irs_u16 a_local_port, irs_u16 a_dest_port, 
    const irs_u8* a_dest_ip);
  void close_udp();
  irs_u8 write_udp_begin();
  void write_udp_end(irs_u8* a_dest_ip, irs_u16* a_dest_port, irs_u16 a_size);
  irs_u16 read_udp_begin(irs_u8* a_dest_ip, irs_u16* a_dest_port);
  void read_udp_end();
  
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual void tick();
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  
private:
  ethernet_rtl_t* mp_ethernet_rtl;
  raw_data_t<irs_u8> m_ip;
  raw_data_t<irs_u8> m_mac;
  raw_data_t<irs_u8> m_arp_buf;
  raw_data_t<irs_u8> m_icmp_buf;
  raw_data_t<irs_u8> m_arp_cash;
  irs_u16 m_rx_len_icmp;
  raw_data_t<irs_u8> m_dest_ip;
  raw_data_t<irs_u8> m_dest_ip_def;
  bool m_user_rx;
  bool m_user_tx;
  bool m_tx_udp;
  irs_u16 m_identif;
  irs_u16 m_rx_user_length;
  irs_u16 m_tx_user_length_hard;
  irs_u16 m_tx_user_length_udp;
  irs_u16 m_dest_port;
  irs_u16 m_dest_port_def;
  irs_u16 m_local_port;
  bool m_udp_open;
  irs_u8* mp_rx_buf;
  irs_u8* mp_tx_buf;
  irs_u8* m_user_rx_buf;
  irs_u8* m_user_tx_buf;
  bool m_rx_arp;
  bool m_send_arp;
  bool m_rx_icmp;
  bool m_send_icmp;
  bool m_send_udp;
  bool m_rx_hard;
  bool m_tx_hard;
  irs_u16 m_rx_len_hard;
  
  irs_u8 cash(irs_u8* a_dest_ip);
  irs_u16 ip_checksum(irs_u16 a_cs, irs_u8 a_dat, irs_u16 a_count);
  irs_u16 cheksum(irs_u16 a_count, irs_u8* a_addr);
  irs_u16 cheksumUDP(irs_u16 a_count, irs_u8* a_addr);
  void arp_ping(irs_u8* a_dest_ip);
  void arp_packet(void);
  void arp_cash(void);
  void arp();
  void send_arp(void);
  void icmp_packet();
  void send_icmp();
  void icmp();
  void server_udp();
  void udp_packet();
  void send_udp();
  void client_udp();
  void udp();
  void ip(void);
  
};

} //namespace hardflow

} //namespace irs

#endif //IRSUDPSTACKH
