// UDP/IP-���� 
// ����: 31.03.2010
// ���� ��������: 16.03.2010

#ifndef IRSTCPIPH
#define IRSTCPIPH

#include <irsdefs.h>

#include <mxdata.h>
#include <timer.h>
#include <irsavrutil.h>

#include <irsfinal.h>

// ������ ���������� Ethernet + IP + UDP
#define HEADERS_SIZE 42
// ������ ������ ��������
#define UDP_BUFFER_SIZE_TX (ETHERNET_PACKET_TX - HEADERS_SIZE)
// ������ ������ ������
#define UDP_BUFFER_SIZE_RX (ETHERNET_PACKET_RX - HEADERS_SIZE)

// ������ IP-������
#define IRS_UDP_IP_SIZE 4

// ������ MAC-������
#define IRS_UDP_MAC_SIZE 6

namespace irs {

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

//Ethernet interface for RTL
class simple_ethernet_t
{
public:
  enum buffer_num_t{
    single_buf,
    double_buf
  };
  virtual ~simple_ethernet_t() {}
  virtual void send_packet(irs_u16 a_size) = 0;
  virtual void set_recv_handled() = 0;
  virtual bool is_recv_buf_filled() = 0;
  virtual irs_u8* get_recv_buf() = 0;
  virtual irs_u8* get_send_buf() = 0;
  virtual irs_size_t recv_buf_size() = 0;
  virtual irs_size_t send_buf_max_size() = 0;
  virtual buffer_num_t get_buf_num() = 0;
  virtual mac_t get_local_mac() = 0;
  virtual void tick() = 0;
};

// ARP-���
class arp_cash_t
{
public:
  enum {
    arp_table_size = 3
  };
  
  arp_cash_t(irs_size_t a_size = arp_table_size);
  bool ip_to_mac(const irs::ip_t& a_ip, irs::mac_t& a_mac);
  void add(const irs::ip_t& a_ip, const irs::mac_t& a_mac);
  inline irs_size_t size() const;
  void resize(irs_size_t a_size);
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
  irs_size_t m_pos;
  irs::loop_timer_t m_reset_timer;
};

class simple_tcpip_t
{
public:
  typedef simple_ethernet_t::buffer_num_t buffer_num_t;

  enum {
    ARPBUF_SIZE = 42,
    ARPBUF_SENDSIZE = 60,
    ICMPBUF_SIZE = 200,
    mac_length = 0x6,
    ip_length = 0x4,
    arp_operation_request = 0x0001,
    arp_operation_response = 0x0002,
    udp_proto = 0x11,
    icmp_proto = 0x01,
    //tcp_proto = 0x6,
    ether_type = 0x0806,
    IPv4 = 0x0800,
    Ethernet = 0x0001,
    
    dest_mac = 0x0,
    sourse_mac = 0x6,
    ether_type_0 = 0xc,
    ether_type_1 = 0xd,
    hardware_type_0 = 0xe,
    hardware_type_1 = 0xf,
    proto_type_0 = 0x10,
    proto_type_1 = 0x11,
    hardware_length = 0x12,
    proto_length = 0x13,
    arp_operation_code_0 = 0x14,
    arp_operation_code_1 = 0x15,
    arp_sender_mac = 0x16,
    arp_sender_ip = 0x1c,
    arp_target_mac = 0x20,
    arp_target_ip = 0x26,
    check_sum_ip_0 = 0x18,
    check_sum_ip_1 = 0x19,
    check_sum_icmp_0 = 0x24,
    check_sum_icmp_1 = 0x25,
    icmp_target_ip = 0x1e,
    icmp_sender_ip = 0x1a,
    icmp_type = 0x22,
    icmp_code = 0x23,
    icmp_echo_request = 8,
    icmp_echo_response = 0,
    udp_dest_ip = 0x1e,
    udp_source_ip = 0x1a,
    udp_dest_port_0 = 0x24,
    udp_dest_port_1 = 0x25,
    udp_local_port_0 = 0x22,
    udp_local_port_1 = 0x23,
    udp_length_0 = 0x26,
    udp_length_1 = 0x27,
    udp_check_sum_0 = 0x28,
    udp_check_sum_1 = 0x29,
    ip_proto_type = 0x17
  };
  
  simple_tcpip_t(
    simple_ethernet_t* ap_ethernet,
    const ip_t& a_ip,
    irs_size_t a_arp_cash_size = arp_cash_t::arp_table_size
  );
  ~simple_tcpip_t();
  void open_udp();
  void close_udp();
  bool is_write_udp_complete();
  void write_udp(ip_t a_dest_ip, irs_u16 a_dest_port,
    irs_u16 a_local_port, irs_size_t a_size);
  irs_size_t read_udp(ip_t* a_dest_ip, irs_u16* a_dest_port,
    irs_u16* a_local_port);
  void is_read_udp_complete();
  void tick();
  
private:
  simple_ethernet_t* mp_ethernet;
  buffer_num_t m_buf_num;
  ip_t m_ip;
  mac_t m_mac;
  irs_u16 m_recv_buf_size_icmp;
  ip_t m_dest_ip;
  ip_t m_dest_ip_def;
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
  bool m_udp_wait_arp;
  bool m_recv_buf_filled;
  bool m_send_buf_filled;
  timer_t m_udp_wait_arp_time;
  arp_cash_t m_arp_cash;
  mac_t& m_dest_mac;
  blink_t m_blink_0;
  blink_t m_blink_1;
  blink_t m_blink_2;
  blink_t m_blink_3;
  bool m_send_arp;
  bool m_send_icmp;
  bool m_send_udp;
  bool m_recv_arp;
  bool m_recv_icmp;
  
  bool cash(ip_t a_dest_ip);
  irs_u16 ip_checksum(irs_u16 a_cs, irs_u8 a_dat, irs_u16 a_count);
  irs_u16 check_sum(irs_u16 a_count, irs_u8* a_addr);
  irs_u16 cheksumUDP(irs_u16 a_count, irs_u8* a_addr);
  void arp_request(ip_t a_dest_ip);
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