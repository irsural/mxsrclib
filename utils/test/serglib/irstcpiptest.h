// UDP/IP-стек 
// Дата: 19.03.2010
// дата создания: 16.03.2010

#ifndef IRSTCPIPH
#define IRSTCPIPH

#include <irsdefs.h>

#include <mxdata.h>

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

//Ethernet interface for RTL
class simple_ethernet_t
{
public:
  enum buffer_num_t{
    single_buf,
    double_buf
  };
  virtual ~simple_ethernet_t();
  virtual void send_packet(irs_u16 a_size);
  virtual void set_recv_status_completed();
  virtual bool is_recv_status_busy(); 
  virtual bool is_send_status_busy(); 
  virtual irs_u8* get_recv_buf();
  virtual irs_u8* get_send_buf();
  virtual size_t recv_buf_size();
  virtual buffer_num_t get_buf_num();
};

struct mac_t 
{
  irs_u8 val[IRS_UDP_MAC_SIZE];
  
  bool operator==(const mac_t& a_mac) const
  {
    return memcmp((void *)val, (void *)a_mac.val, IRS_UDP_MAC_SIZE) == 0;
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

// ARP-кэш
class arp_cash_t
{
public:
  enum {
    arp_table_size = 3
  };
  
  arp_cash_t(size_t a_size = arp_table_size);
  bool ip_to_mac(const irs::ip_t& a_ip, irs::mac_t& a_mac);
  void add(const irs::ip_t& a_ip, const irs::mac_t& a_mac);
  inline size_t size() const;
  void resize(size_t a_size);
private:
  struct cash_item_t {
    irs::ip_t ip;
    irs::mac_t mac;
    bool valid;
    
    cash_item_t(
      irs::ip_t a_ip = irs::zero_ip(),
      irs::mac_t a_mac = irs::zero_mac(), 
      bool a_valid = false
    ):
      ip(a_ip),
      mac(a_mac),
      valid(a_valid)
    {
    }
  };
  typedef vector<cash_item_t>::iterator cash_item_it_t;
  typedef vector<cash_item_t>::const_iterator cash_item_const_it_t;
  
  vector<cash_item_t> m_cash;
  size_t m_pos;
  irs::loop_timer_t m_reset_timer;
};

class simple_tcpip_t
{
public:
  typedef simple_ethernet_t::buffer_num_t buffer_num_t;

  enum {
    ARPBUF_SIZE = 42,
    ARPBUF_SENDSIZE = 60,
    ICMPBUF_SIZE = 200
  };
  
  simple_tcpip_t(
    simple_ethernet_t* ap_ethernet,
    arp_cash_t a_arp_cash,
    const irs_u8* ap_mac, 
    const irs_u8* ap_ip
  );
  ~simple_tcpip_t();
  void open_udp(irs_u16 a_local_port, irs_u16 a_dest_port, 
    const irs_u8* a_dest_ip);
  void close_udp();
  irs_u8 write_udp_begin();
  void write_udp_end(irs_u8* a_dest_ip, irs_u16* a_dest_port, irs_u16 a_size);
  irs_u16 read_udp_begin(irs_u8* a_dest_ip, irs_u16* a_dest_port);
  void read_udp_end();
  void tick();
  
private:
  simple_ethernet_t* mp_ethernet;
  buffer_num_t m_buf_num;
  raw_data_t<irs_u8> m_ip;
  raw_data_t<irs_u8> m_mac;
  raw_data_t<irs_u8> m_arp_cash;
  irs_u16 m_recv_buf_size_icmp;
  raw_data_t<irs_u8> m_dest_ip;
  raw_data_t<irs_u8> m_dest_ip_def;
  bool m_user_recv_status;
  bool m_user_send_status;
  bool m_udp_send_status;
  irs_u16 m_identif;
  irs_u16 m_user_recv_buf_size;
  irs_u16 m_user_send_buf_size;
  irs_u16 m_user_send_buf_udp_size;
  irs_u16 m_dest_port;
  irs_u16 m_dest_port_def;
  irs_u16 m_local_port;
  bool m_udp_open;
  irs_u8* mp_recv_buf;
  irs_u8* mp_send_buf;
  irs_u8* mp_user_recv_buf;
  irs_u8* mp_user_send_buf;
  bool m_recv_arp_status_busy;
  bool m_send_arp_status_busy;
  bool m_recv_icmp_status_busy;
  bool m_send_icmp_status_busy;
  bool m_send_udp_status_busy;
  bool m_recv_status;
  bool m_send_status;
  size_t m_recv_buf_size;
  arp_cash_t mp_arp_cash;
  ip_t& m_cash_ip;
  mac_t& m_cash_mac;
  
  bool cash(irs_u8* a_dest_ip);
  irs_u16 ip_checksum(irs_u16 a_cs, irs_u8 a_dat, irs_u16 a_count);
  irs_u16 check_sum(irs_u16 a_count, irs_u8* a_addr);
  irs_u16 cheksumUDP(irs_u16 a_count, irs_u8* a_addr);
  void arp_request(irs_u8* a_dest_ip);
  void arp_response(void);
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

} //namespace irs

#endif //IRSTCPIPH
