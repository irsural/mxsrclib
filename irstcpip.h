// UDP/IP-стек
// Дата: 07.06.2010
// дата создания: 16.03.2010

#ifndef IRSTCPIPH
#define IRSTCPIPH

#include <irsdefs.h>

#include <mxdata.h>
#ifdef __ICCAVR__
#include <irsavrutil.h>
#endif //__ICCAVR__
#include <irsnetdefs.h>

#include <irsfinal.h>

//define TCP_ENABLED

#ifdef IRS_LIB_IRSTCPIP_DEBUG_TYPE
# if (IRS_LIB_IRSTCPIP_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
#   define IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
#   define IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(msg) msg
#   define IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(msg)
# elif (IRS_LIB_IRSTCPIP_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
#   define IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
#   define IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(msg) msg
#   define IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(msg) IRS_LIB_DBG_RAW_MSG(msg)
# endif
#else // IRS_LIB_IRSTCPIP_DEBUG_TYPE
# define IRS_LIB_IRSTCPIP_DEBUG_TYPE IRS_LIB_DEBUG_NONE
# define IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(msg)
#   define IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(msg)
#endif // IRS_LIB_IRSTCPIP_DEBUG_TYPE

namespace irs {

// Ethernet interface
class simple_ethernet_t
{
public:
  enum buffer_num_t{
    single_buf,
    double_buf
  };
  virtual ~simple_ethernet_t() {}
  //  Отправить пакет с размером a_size ( = DA + SA + L + DATA)
  virtual void send_packet(irs_size_t a_size) = 0;
  //  Очистить буфер для приёма нового пакета
  virtual void set_recv_handled() = 0;
  //  Заблокировать буфер для отправки пакета
  virtual void set_send_buf_locked() = 0;
  //  Есть ли в приёмном буфере пакет
  virtual bool is_recv_buf_filled() = 0;
  //  Свободен ли буфер на запись
  virtual bool is_send_buf_empty() = 0;
  //  Указатель на буфер приёма
  virtual irs_u8* get_recv_buf() = 0;
  //  Указатель на буфер передачи
  virtual irs_u8* get_send_buf() = 0;
  //  Размер принятого пакета ( = DA + SA + L + DATA + FCS)
  virtual irs_size_t recv_buf_size() = 0;
  //  Размер буфера передачи ( = DA + SA + L + DATA)
  virtual irs_size_t send_buf_max_size() = 0;
  //  Один общий буфер или два отдельных буфера на приём и передачу
  virtual buffer_num_t get_buf_num() = 0;
  //  Собственный MAC-адрес
  virtual mxmac_t get_local_mac() = 0;
  virtual void tick() = 0;
};

// ARP-кэш
class arp_cash_t
{
public:
  enum {
    arp_table_size = 3
  };
  
  arp_cash_t(irs_size_t a_size = arp_table_size);
  bool ip_to_mac(const mxip_t& a_ip, mxmac_t& a_mac);
  void add(const mxip_t& a_ip, const mxmac_t& a_mac);
  inline irs_size_t size() const;
  void resize(irs_size_t a_size);
private:
  struct cash_item_t {
    mxip_t ip;
    mxmac_t mac;
    bool valid;
    
    cash_item_t(
      mxip_t a_ip = mxip_t::zero_ip(),
      mxmac_t a_mac = mxmac_t::zero_mac(), 
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
    HEADERS_SIZE = 42,
    ARPBUF_SIZE = 42,
    ARPBUF_SENDSIZE = 60,
    ICMPBUF_SIZE = 200,
    mac_length = 0x6,
    ip_length = 0x4,
    arp_operation_request = 0x0001,
    arp_operation_response = 0x0002,
    udp_proto = 0x11,
    icmp_proto = 0x01,
    tcp_proto = 0x6,
    ether_type = 0x0806,
    ARP = 0x0806,
    IPv4 = 0x0800,
    Ethernet = 0x0001,
    ip_header_length = 20,
    udp_header_length = 8,
    tcp_header_length = 20,
    
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
    ip_proto_type = 0x17,
    ip_version = 0xe,
    ip_type_of_service = 0xf,
    ip_length_0 = 0x10,
    ip_length_1 = 0x11,
    udp_ident_0 = 0x12,
    udp_ident_1 = 0x13,
    udp_fragment_0 = 0x14,
    udp_fragment_1 = 0x15,
    TTL = 0x16,
    udp_pseudo_header_length = 12,
    tcp_dest_ip = 0x1e,
    tcp_source_ip = 0x1a,
    tcp_dest_port_0 = 0x24,
    tcp_dest_port_1 = 0x25,
    tcp_local_port_0 = 0x22,
    tcp_local_port_1 = 0x23,
    tcp_sequence_number = 0x26, // 4 byte
    tcp_acknowledgment_number = 0x2a, // 4 byte
    tcp_flags_0 = 0x2e,
    tcp_flags_1 = 0x2f,
    tcp_FIN = 0x01,
    tcp_SYN = 0x02,
    tcp_RST = 0x04,
    tcp_PSH = 0x08,
    tcp_ACK = 0x10,
    tcp_URG = 0x20,
    tcp_window_size_0 = 0x30,
    tcp_window_size_1 = 0x31,
    tcp_check_sum_0 = 0x32,
    tcp_check_sum_1 = 0x33,
    tcp_urgent_pointer_0 = 0x34, // указатель срочности
    tcp_urgent_pointer_1 = 0x35,
    tcp_data = 0x36 // options(если есть) + data
  };
  
  simple_tcpip_t(
    simple_ethernet_t* ap_ethernet,
    const mxip_t& a_ip,
    const mxip_t& a_dest_ip = mxip_t::zero_ip(),
    irs_size_t a_arp_cash_size = arp_cash_t::arp_table_size
  );
  ~simple_tcpip_t();
  void open_udp();
  void close_udp();
  bool is_write_udp_complete();
  void write_udp(mxip_t a_dest_ip, irs_u16 a_dest_port,
    irs_u16 a_local_port, irs_size_t a_size);
  irs_size_t read_udp(mxip_t* a_dest_ip, irs_u16* a_dest_port,
    irs_u16* a_local_port);
  void read_udp_complete();
  irs_u8* get_recv_buf();
  irs_u8* get_send_buf();
  bool open_port(irs_u16 a_port);
  void close_port(irs_u16 a_port);
  irs_size_t recv_buf_size();
  irs_size_t send_data_size_max();
  void tick();
  
private:  
  simple_ethernet_t* mp_ethernet;
  buffer_num_t m_buf_num;
  mxip_t m_local_ip;
  mxmac_t m_mac;
  irs_size_t m_recv_buf_size_icmp;
  mxip_t m_dest_ip;
  mxip_t m_dest_ip_def;
  bool m_user_recv_status;
  bool m_user_send_status;
  bool m_udp_send_status;
  irs_u16 m_identif;
  irs_size_t m_user_recv_buf_size;
  irs_size_t m_user_send_buf_size;
  irs_size_t m_user_send_buf_udp_size;
  irs_size_t m_user_send_buf_tcp_size;
  irs_u16 m_dest_port;
  irs_u16 m_dest_port_def;
  irs_u16 m_local_port;
  bool m_udp_open;
  irs_u8* mp_recv_buf;
  irs_u8* mp_send_buf;
  irs_u8* mp_user_recv_buf;
  irs_u8* mp_user_send_buf;
  bool m_udp_wait_arp;
  bool m_send_buf_filled;
  timer_t m_udp_wait_arp_time;
  arp_cash_t m_arp_cash;
  mxmac_t& m_dest_mac;
  mxip_t m_cur_dest_ip;
  irs_u16 m_cur_dest_port;
  irs_u16 m_cur_local_port;
  bool m_send_arp;
  bool m_send_icmp;
  bool m_send_udp;
  bool m_send_tcp;
  bool m_recv_arp;
  set<irs_u16> m_port_list;
  bool m_new_recv_packet;
  bool m_tcp_connect;
  irs_size_t m_tcp_data_length_in;
  
  bool cash(mxip_t a_dest_ip);
  irs_u16 check_sum_ip(irs_u16 a_cs, irs_u8 a_dat, irs_size_t a_count);
  irs_u16 check_sum(irs_size_t a_count, irs_u8* a_addr);
  irs_u16 check_sum_udp(irs_size_t a_count, irs_u8* a_addr);
  irs_u16 check_sum_tcp(irs_size_t a_count, irs_u8* a_addr);
  void arp_request(mxip_t a_dest_ip);
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
  void tcp_packet();
  void recv_tcp();
  void send_tcp();
};

} //namespace irs
#endif //IRSTCPIPH
