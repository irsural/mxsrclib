//! \file
//! \ingroup network_in_out_group
//! \brief UDP/IP-стек
//!
//! Дата: 23.07.2010\n
//! дата создания: 16.03.2010

#ifndef IRSTCPIPH
#define IRSTCPIPH

#include <irsdefs.h>

#include <mxdata.h>
#ifdef __ICCAVR__
#include <irsavrutil.h>
#endif //__ICCAVR__
#include <irsnetdefs.h>

#include <irsfinal.h>

#ifndef IRS_LIB_IRSTCPIP_DEBUG_TYPE
# define IRS_LIB_IRSTCPIP_DEBUG_TYPE IRS_LIB_DEBUG_NONE
#endif // !IRS_LIB_IRSTCPIP_DEBUG_TYPE

#if (IRS_LIB_IRSTCPIP_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
# define IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(msg) msg
# define IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(msg)
#elif (IRS_LIB_IRSTCPIP_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
# define IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(msg) msg
# define IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(msg) IRS_LIB_DBG_RAW_MSG(msg)
#else // IRS_LIB_IRSTCPIP_DEBUG_TYPE == IRS_LIB_DEBUG_NONE
# define IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(msg)
# define IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(msg)
#endif // IRS_LIB_IRSTCPIP_DEBUG_TYPE == IRS_LIB_DEBUG_NONE

namespace irs {

//! \addtogroup network_in_out_group
//! @{

irs_u16 ntoh16(irs_u8* ap_network_in);
void hton16(irs_u8* ap_network_out, irs_u16 a_host_in);
irs_u32 ntoh32(irs_u8* ap_network_in);
void hton32(irs_u8* ap_network_out, irs_u32 a_host_in);

//! \brief Ethernet interface
//! \author Sergeev Sergey
class simple_ethernet_t
{
public:
  enum {
    //! \brief 6 байт адрес назначения, 6 байт адрес источника, 2 байта длина
    ethernet_header_size = 14
  };
  enum buffer_num_t{
    single_buf,
    double_buf
  };
  virtual ~simple_ethernet_t() {}
  //! \brief  Отправить пакет с размером a_size ( = DA + SA + L + DATA)
  virtual void send_packet(irs_size_t a_size) = 0;
  //! \brief  Очистить буфер для приёма нового пакета
  virtual void set_recv_handled() = 0;
  //! \brief  Заблокировать буфер для отправки пакета
  virtual void set_send_buf_locked() = 0;
  //! \brief  Есть ли в приёмном буфере пакет
  virtual bool is_recv_buf_filled() const = 0;
  //! \brief  Свободен ли буфер на запись
  virtual bool is_send_buf_empty() const = 0;
  //! \brief  Указатель на буфер приёма
  virtual irs_u8* get_recv_buf() = 0;
  //! \brief  Указатель на буфер передачи
  virtual irs_u8* get_send_buf() = 0;
  //! \brief  Размер принятого пакета ( = DA + SA + L + DATA + FCS)
  //! \details Контрольная сумма не обрабатывается, но ее размер включен в
  //!   размер пакета для обратной совместимости
  virtual irs_size_t recv_buf_size() const = 0;
  //! \brief  Размер буфера передачи ( = DA + SA + L + DATA)
  virtual irs_size_t send_buf_max_size() const = 0;
  //! \brief  Один общий буфер или два отдельных буфера на приём и передачу
  virtual buffer_num_t get_buf_num() const = 0;
  //! \brief  Собственный MAC-адрес
  virtual mxmac_t get_local_mac() const = 0;
  //! \brief  Установка нового МАС-адреса
  virtual void set_mac(mxmac_t& a_mac) = 0;
  //! \brief  Возвращает статус завершения запуска
  //! \details Конфигурирование Ethernet-модуля требует некоторого времени.
  //!   Конфигурирование может происходить не только при первой инициализации,
  //!   но и в случае изменения некоторых параметров соединения, таких как
  //!   скорость соединения(10/100 Мбит/с) или
  //!   режим соединения (half duplex/full duplex), поэтому функция может
  //!   возвращать \c false в процессе работы программы.
  //!   Функция должна возвращать \c true в том случае, если Ethernet-модуль
  //!   нормально функционирует и позволяет начать производить обмен данными.
  //!   Фукция должна возвращать \c false, если функция get_linked_status
  //!   возвращает \c false
  virtual bool get_ready_status() const
  {
    return true;
  }
  //! \brief  Возвращает статус соединения
  //! \details Эта функция возвращает \c false, если сетевой кабель
  //!   не подсоединен
  virtual bool get_linked_status() const
  {
    return true;
  }
  virtual void tick() = 0;
};

//! \brief ARP-cash
//! \author Sergeev Sergey
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
  void tick();
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

class arp_t
{
public:
  typedef simple_ethernet_t::buffer_num_t buffer_num_t;

  enum {
    arp_table_size = 3
  };
  enum mode_t {
    static_note,
    dinamic_note
  };

  arp_t(
    simple_ethernet_t* ap_ethernet,
    const mxip_t& a_local_ip,
    irs_size_t a_size = arp_table_size);
  bool ip_to_mac(const mxip_t& a_ip, mxmac_t& a_mac);
  void add(const mxip_t& a_ip, const mxmac_t& a_mac);
  void add_static(const mxip_t& a_ip, const mxmac_t& a_mac);
  irs_size_t size() const;
  void resize(irs_size_t a_size);
  void tick();
private:
  enum {
    ARP = 0x0806,
    ether_type_0 = 0xc,
    ether_type_1 = 0xd,
    arp_operation_code_0 = 0x14,
    arp_operation_code_1 = 0x15,
    arp_sender_mac = 0x16,
    arp_sender_ip = 0x1c,
    arp_target_mac = 0x20,
    arp_target_ip = 0x26
  };
  struct cash_item_t {
    mxip_t ip;
    mxmac_t mac;
    irs::timer_t ttl;
    irs::timer_t reset_timer;
    mode_t mode;
    bool valid;

    cash_item_t(
      mxip_t a_ip = mxip_t::zero_ip(),
      mxmac_t a_mac = mxmac_t::zero_mac(),
      irs::timer_t a_ttl = irs::make_cnt_s(120),
      irs::timer_t a_reset_timer = irs::make_cnt_s(600),
      mode_t a_mode = dinamic_note,
      bool a_valid = false
    ):
      ip(a_ip),
      mac(a_mac),
      ttl(a_ttl),
      reset_timer(a_reset_timer),
      mode(a_mode),
      valid(a_valid)
    {
    }
  };
  typedef vector<cash_item_t>::iterator cash_item_it_t;
  typedef vector<cash_item_t>::const_iterator cash_item_const_it_t;

  vector<cash_item_t> m_cash;
  irs_size_t m_pos;
  simple_ethernet_t* mp_ethernet;
  bool m_new_recv_packet;
  buffer_num_t m_buf_num;
  irs_u8* mp_recv_buf;
  irs_u8* mp_send_buf;
  mxip_t m_local_ip;
  mxmac_t m_mac;
}; // class arp_t

//! \brief Стек протоколов TCP/IP
//! \author Sergeev Sergey
class simple_tcpip_t
{
public:
  typedef simple_ethernet_t::buffer_num_t buffer_num_t;

  enum {
    ARPBUF_SIZE = 42,
    ARPBUF_SENDSIZE = 60,
    ICMPBUF_SIZE = 200,
    TCP_HANDSHAKE_SIZE = 62,
    mac_len = 0x6,
    ip_len = 0x4,
    arp_operation_request = 0x0001,
    arp_operation_response = 0x0002,
    udp_proto = 0x11,
    icmp_proto = 0x01,
    tcp_proto = 0x6,
    ARP = 0x0806,
    IPv4 = 0x0800,
    Ethernet = 0x0001,
    ip_header_length = 20,
    udp_header_length = 8,
    udp_pseudo_header_length = 12,
    tcp_header_length = 20,
    tcp_pseudo_header_length = 12,

    dest_mac = 0x0,
    sourse_mac = 0x6,
    ether_type_0 = 0xc,
    ether_type_1 = 0xd,
    ip_begin = 0xe,
    hardware_type_0 = ip_begin,
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
    ip_check_sum = 0x18,
    icmp_check_sum = 0x24,
    icmp_target_ip = 0x1e,
    icmp_sender_ip = 0x1a,
    icmp_type = 0x22,
    icmp_code = 0x23,
    icmp_echo_request = 8,
    icmp_echo_response = 0,
    udp_dest_ip = 0x1e,
    udp_source_ip = 0x1a,
    udp_local_port = 0x22,
    udp_dest_port = 0x24,
    udp_length = 0x26,
    udp_check_sum = 0x28,
    udp_data = 0x2a,
    ip_proto_type = 0x17,
    ip_version = 0xe,
    ip_type_of_service = 0xf,
    ip_length = 0x10,
    ip_ident = 0x12,
    ip_fragment = 0x14,
    TTL = 0x16,
    tcp_dest_ip = 0x1e,
    tcp_source_ip = 0x1a,
    tcp_dest_port = 0x24,
    tcp_local_port = 0x22,
    tcp_sequence_number = 0x26, // 4 byte
    tcp_acknowledgment_number = 0x2a, // 4 byte
    tcp_flags_0 = 0x2e,
    tcp_flags_1 = 0x2f,
    tcp_FIN = 0x01, // передача окончена
    tcp_SYN = 0x02, // синхронизация при установлении соединения
    tcp_RST = 0x04, // запрос на восстановление соединения
    tcp_PSH = 0x08, // запрос на отправку сообщения без ожидания
                      // заполнения буфера
    tcp_ACK = 0x10, // квитанция на принятый сегмент
    tcp_URG = 0x20, // срочное сообщение
    tcp_window_size = 0x30,
    tcp_check_sum = 0x32,
    tcp_urgent_pointer = 0x34, // указатель срочности
    tcp_data = 0x36 // options(если есть) + data
  };
  enum {
    invalid_socket = 0
  };

  simple_tcpip_t(
    simple_ethernet_t* ap_ethernet,
    const mxip_t& a_ip,
    const mxip_t& a_dest_ip = mxip_t::zero_ip(),
    irs_size_t a_arp_cash_size = arp_cash_t::arp_table_size
  );
  ~simple_tcpip_t();

  // UDP
  void open_udp();
  void close_udp();

  // TCP
  void active_open_tcp();
  void passive_open_tcp();
  void close_tcp();
  void tcp_init();
  bool tcp_connect(irs_size_t a_socket, irs_u16 a_local_port,
    mxip_t a_dest_ip, irs_u16 a_dest_port);
  bool tcp_listen(irs_size_t a_socket, irs_u16 a_port);

  bool is_write_complete();
  void write(mxip_t a_dest_ip, irs_u16 a_dest_port,
    irs_u16 a_local_port, irs_size_t a_size);
  irs_size_t read(mxip_t* a_dest_ip, irs_u16* a_dest_port,
    irs_u16* a_local_port);
  void read_complete();
  bool tcp_cur_channel(irs_size_t* a_socket, mxip_t* a_dest_ip,
    irs_u16* a_dest_port);

  irs_u8* get_recv_buf();
  irs_u8* get_send_buf();
  bool open_port(irs_u16 a_port);
  void close_port(irs_u16 a_port);
  irs_size_t recv_data_size();
  irs_size_t send_data_size_max();
  void tick();
  void set_ip(mxip_t& a_ip);
  void set_mac(mxmac_t& a_mac);

public:
  enum send_mode_t{
    wait_send_command_mode,
    send_SYN,
    send_ACK_SYN,
    send_ACK,
    send_FIN,
    send_DATA,
    send_RST
  };
  enum tcp_state_t {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    CLOSE_WAIT,
    FIN_WAIT_2,
    LAST_ACK,
    TIME_WAIT,
    CLOSING
  };
  enum open_t {
    // соединение не открыто
    non_open,
    // открытие приложения и ожидание запроса на создание соединения
    passive_open,
    // открытие приложения и инициация соединения
    active_open
  };
private:
  struct tcp_socket_t {
    irs_size_t socket;
    tcp_state_t state;
    mxip_t remote_ip;
    irs_u16 remote_port;
    irs_u16 local_port;
    irs_u32 send_unacked;
    irs_u32 send_next;
    irs_u32 receive_next;

    tcp_socket_t():
      socket(invalid_socket),
      state(CLOSED),
      remote_ip(mxip_t::zero_ip()),
      remote_port(0),
      local_port(0),
      send_unacked(0),
      send_next(0),
      receive_next(0)
    {
    }
    tcp_socket_t(
      irs_size_t a_socket_number,
      tcp_state_t a_state,
      mxip_t a_remote_ip,
      irs_u16 a_remote_port,
      irs_u16 a_local_port,
      irs_u32 a_send_unacked,
      irs_u32 a_send_next,
      irs_u32 a_receive_next
    ):
      socket(a_socket_number),
      state(a_state),
      remote_ip(a_remote_ip),
      remote_port(a_remote_port),
      local_port(a_local_port),
      send_unacked(a_send_unacked),
      send_next(a_send_next),
      receive_next(a_receive_next)
    {
    }
    bool operator==(tcp_socket_t a_tcp_socket)
    {
      return ((remote_ip == a_tcp_socket.remote_ip) &&
        (remote_port == a_tcp_socket.remote_port) &&
        (local_port == a_tcp_socket.local_port));
    }
  };
  class socket_equal_t
  {
  public:
    socket_equal_t(
      mxip_t a_remote_ip,
      irs_u16 a_remote_port,
      irs_u16 a_local_port
    ):
      m_remote_ip(a_remote_ip),
      m_remote_port(a_remote_port),
      m_local_port(a_local_port)
    {

    }
    bool operator()(tcp_socket_t a_tcp_socket)
    {
      if (a_tcp_socket.remote_port == m_remote_port) {
        if (a_tcp_socket.remote_ip == m_remote_ip) {
          if (a_tcp_socket.local_port == m_local_port) {
            return true;
          }
        }
      }
      return false;
    }
  private:
    mxip_t m_remote_ip;
    irs_u16 m_remote_port;
    irs_u16 m_local_port;
  };

  simple_ethernet_t* mp_ethernet;
  buffer_num_t m_buf_num;
  mxip_t m_local_ip;
  mxmac_t m_mac;
  irs_size_t m_recv_buf_size_icmp;
  mxip_t m_dest_ip;
  mxip_t m_dest_ip_def;
  bool m_user_recv_status;
  bool m_user_send_status;
  irs_u16 m_identif;
  irs_size_t m_user_recv_buf_size;
  irs_size_t m_user_send_buf_udp_size;
  irs_size_t m_user_send_buf_tcp_size;
  irs_u16 m_dest_port;
  irs_u16 m_dest_port_def;
  irs_u16 m_local_port;
  bool m_udp_open;
  bool m_tcp_open;
  irs_u8* mp_recv_buf;
  irs_u8* mp_send_buf;
  irs_u8* mp_user_recv_buf;
  irs_u8* mp_user_send_buf;
  bool m_send_buf_filled;
  timer_t m_connection_wait_arp_timer;
  arp_cash_t m_arp_cash;
  //arp_t m_arp_cash;
  mxmac_t& m_dest_mac;
  mxip_t m_cur_dest_ip;
  irs_u16 m_cur_dest_port;
  irs_u16 m_cur_local_port;
  bool m_send_arp;
  bool m_send_icmp;
  bool m_send_udp;
  set<irs_u16> m_port_list;
  bool m_new_recv_packet;
  send_mode_t m_tcp_send_mode;
  bool m_udp_wait_arp;
  bool m_tcp_wait_arp;
  irs_u32 m_seq_num;
  irs_u32 m_ack_num;
  tcp_state_t m_tcp_state;
  open_t m_open_type;
  irs_size_t m_sock_max_count;
  deque<tcp_socket_t> m_tcp_socket_list;
  irs_size_t m_socket;
  irs_size_t m_list_index;
  irs_size_t m_cur_socket;
  irs_size_t m_tcp_options_size;
  irs_u32 m_tcp_init_seq;
  timer_t m_disconnect_timer;
  loop_timer_t m_base_timer;
  loop_timer_t m_recv_timout;

  bool cash(mxip_t a_dest_ip);
  irs_u16 check_sum_ip(irs_u16 a_cs, irs_u8 a_dat, irs_size_t a_count);
  irs_u16 check_sum(irs_size_t a_count, irs_u8* a_addr);
  irs_u16 check_sum_udp(irs_size_t a_count, irs_u8* a_addr);
  irs_u16 check_sum_tcp(irs_size_t a_count, irs_u8* a_addr);
  void arp_request(mxip_t a_dest_ip);
  void arp_response();
  void arp();
  void send_arp();
  void icmp_packet();
  void send_icmp();
  void icmp();
  void server_udp();
  void udp_packet();
  void send_udp();
  void client_udp();
  void udp();
  void tcp();
  void ip();
  void tcp_packet();
  void server_tcp();
  void client_tcp();
  void send_tcp();
  irs_size_t new_socket(mxip_t a_remote_ip, irs_u16 a_remote_port,
    irs_u16 a_local_port);
  void view_sockets_list();
  void view_tcp_packet(irs_u8* ap_buf);
  void tcp_send_control(send_mode_t a_send_mode);
  void tcp_close();
  void socket_close();
  irs_size_t get_tcp_connection();
};

//! @}

} //namespace irs

#endif //IRSTCPIPH
