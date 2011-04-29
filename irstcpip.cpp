//! \file
//! \ingroup network_in_out_group
//! \brief UDP/IP-стек
//!
//! Дата: 23.07.2010\n
//! Дата создания: 16.03.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irscpp.h>
#include <irstcpip.h>
#include <timer.h>
#include <irsstrm.h>

#include <irsfinal.h>

#define IRS_TCPIP_MAC(data) (*(mxmac_t*)(data))
#define IRS_TCPIP_IP(data) (*(mxip_t*)(data))

// Переобразование данных в mxmac_t
#define IRS_UDP_MAC(data) (*(mxmac_t*)(&(data)))
template <class T>
inline mxmac_t& mac_from_data(T& data);
template <class T>
inline mxmac_t& mac_from_data(T* data);

// Преобразование данных в mxip_t
#define IRS_UDP_IP(data) (*(mxip_t*)(&(data)))
template <class T>
inline mxip_t& ip_from_data(T& data);
template <class T>
inline mxip_t& ip_from_data(T* data);

// Переобразование данных в mxmac_t
template <class T>
inline mxmac_t& mac_from_data(T& data)
{
  return IRS_UDP_MAC(data);
}
template <class T>
inline mxmac_t& mac_from_data(T* data)
{
  return IRS_UDP_MAC(*data);
}

// Преобразование данных в mxip_t
template <class T>
inline mxip_t& ip_from_data(T& data)
{
  return IRS_UDP_IP(data);
}
template <class T>
inline mxip_t& ip_from_data(T* data)
{
  return IRS_UDP_IP(*data);
}

irs_u16 irs::ntoh16(irs_u8* ap_network_in)
{
  irs_u16 host_out = 0;
  IRS_HIBYTE(host_out) = IRS_FIRSTBYTE((*(irs_u16*)(ap_network_in)));
  IRS_LOBYTE(host_out) = IRS_SECONDBYTE((*(irs_u16*)(ap_network_in)));
  return host_out;
}

void irs::hton16(irs_u8* ap_network_out, irs_u16 a_host_in)
{
  IRS_FIRSTBYTE((*(irs_u16*)(ap_network_out))) = IRS_HIBYTE(a_host_in);
  IRS_SECONDBYTE((*(irs_u16*)(ap_network_out))) = IRS_LOBYTE(a_host_in);
}

irs_u32 irs::ntoh32(irs_u8* ap_network_in)
{
  irs_u32 host_out = 0;
  IRS_HIBYTE(IRS_HIWORD(host_out)) =
    IRS_FIRSTBYTE(IRS_FIRSTWORD((*(irs_u32*)(ap_network_in))));
  IRS_LOBYTE(IRS_HIWORD(host_out)) =
    IRS_SECONDBYTE(IRS_FIRSTWORD((*(irs_u32*)(ap_network_in))));
  IRS_HIBYTE(IRS_LOWORD(host_out)) =
    IRS_FIRSTBYTE(IRS_SECONDWORD((*(irs_u32*)(ap_network_in))));
  IRS_LOBYTE(IRS_LOWORD(host_out)) =
    IRS_SECONDBYTE(IRS_SECONDWORD((*(irs_u32*)(ap_network_in))));
  return host_out;
}

void irs::hton32(irs_u8* ap_network_out, irs_u32 a_host_in)
{
  IRS_FIRSTBYTE(IRS_FIRSTWORD((*(irs_u32*)(ap_network_out)))) =
    IRS_HIBYTE(IRS_HIWORD(a_host_in));
  IRS_SECONDBYTE(IRS_FIRSTWORD((*(irs_u32*)(ap_network_out)))) =
    IRS_LOBYTE(IRS_HIWORD(a_host_in));
  IRS_FIRSTBYTE(IRS_SECONDWORD((*(irs_u32*)(ap_network_out)))) =
    IRS_HIBYTE(IRS_LOWORD(a_host_in));
  IRS_SECONDBYTE(IRS_SECONDWORD((*(irs_u32*)(ap_network_out)))) =
    IRS_LOBYTE(IRS_LOWORD(a_host_in));
}

// ARP-кэш
irs::arp_cash_t::arp_cash_t(irs_size_t a_size):
  m_cash(a_size),
  m_pos(0),
  m_reset_timer(irs::make_cnt_s(600))
{
}

bool irs::arp_cash_t::ip_to_mac(const mxip_t& a_ip, mxmac_t& a_mac)
{
  for (cash_item_const_it_t it = m_cash.begin(); it != m_cash.end(); ++it) {
    if (it->valid) {
      if (it->ip == a_ip) {
        a_mac = it->mac;
        return true;
      }
    }
  }
  if (m_reset_timer.check()) {
    irs_size_t save_size = m_cash.size();
    m_cash.clear();
    m_cash.resize(save_size);
  }
  return false;
}

// Добавление ip и mac в ARP-таблицу
void irs::arp_cash_t::add(const mxip_t& a_ip, const mxmac_t& a_mac)
{
  bool ip_finded = false;
  for (cash_item_it_t it = m_cash.begin(); it != m_cash.end(); ++it) {
    if (it->valid) {
      if (it->ip == a_ip) {
        it->mac = a_mac;
        ip_finded = true;
      }
    }
  }
  if (!ip_finded) {
    m_cash[m_pos].ip = a_ip;
    m_cash[m_pos].mac = a_mac;
    m_cash[m_pos].valid = true;
    ++m_pos;
    if (m_pos > m_cash.size()) {
      m_pos = 0;
    }
  }
}

inline irs_size_t irs::arp_cash_t::size() const
{
  return (irs_size_t)m_cash.size();
}

void irs::arp_cash_t::resize(irs_size_t a_size)
{
  if (a_size < 1) {
    a_size = 1;
  }
  m_cash.resize(a_size);
  if (m_pos > m_cash.size()) {
    m_pos = 0;
  }
}

void irs::arp_cash_t::tick()
{
}

//----------------- NEW ARP -----------------------

irs::arp_t::arp_t(
  simple_ethernet_t* ap_ethernet,
  const mxip_t& a_local_ip,
  irs_size_t a_size
):
  m_cash(a_size),
  m_pos(0),
  mp_ethernet(ap_ethernet),
  m_new_recv_packet(true),
  m_buf_num(mp_ethernet->get_buf_num()),
  mp_recv_buf(mp_ethernet->get_recv_buf()),
  mp_send_buf((m_buf_num == simple_ethernet_t::single_buf) ? 
    mp_ethernet->get_recv_buf() : mp_ethernet->get_send_buf()),
  m_local_ip(a_local_ip),
  m_mac(mp_ethernet->get_local_mac())
{
}

bool irs::arp_t::ip_to_mac(const mxip_t& a_ip, mxmac_t& a_mac)
{
  for (cash_item_const_it_t it = m_cash.begin(); it != m_cash.end(); ++it) {
    if (it->valid) {
      if (it->ip == a_ip) {
        a_mac = it->mac;
        return true;
      }
    }
  }
  return false;
}

// Добавление ip и mac в ARP-таблицу
void irs::arp_t::add(const mxip_t& a_ip, const mxmac_t& a_mac)
{
  bool ip_finded = false;
  for (cash_item_it_t it = m_cash.begin(); it != m_cash.end(); ++it) {
    if (it->valid) {
      if (it->ip == a_ip) {
        it->mac = a_mac;
        it->ttl.start();
        ip_finded = true;
      }
    }
  }
  if (!ip_finded) {
    m_cash[m_pos].ip = a_ip;
    m_cash[m_pos].mac = a_mac;
    m_cash[m_pos].ttl.start();
    m_cash[m_pos].reset_timer.start();
    m_cash[m_pos].valid = true;
    ++m_pos;
    if (m_pos >= m_cash.size()) {
      m_pos = 0;
    }
  }
}

void irs::arp_t::add_static(const mxip_t& a_ip, const mxmac_t& a_mac)
{
  bool ip_finded = false;
  for (cash_item_it_t it = m_cash.begin(); it != m_cash.end(); ++it) {
    if (it->valid) {
      if (it->ip == a_ip) {
        it->mac = a_mac;
        it->ttl.stop();
        it->reset_timer.stop();
        it->mode = static_note;
        ip_finded = true;
      }
    }
  }
  if (!ip_finded) {
    m_cash[m_pos].ip = a_ip;
    m_cash[m_pos].mac = a_mac;
    //m_cash[m_pos].ttl.stop();
    //m_cash[m_pos].reset_timer.stop();
    m_cash[m_pos].mode = static_note;
    m_cash[m_pos].valid = true;
    ++m_pos;
    if (m_pos >= m_cash.size()) {
      m_pos = 0;
    }
  }
}

irs_size_t irs::arp_t::size() const
{
  return (irs_size_t)m_cash.size();
}

void irs::arp_t::resize(irs_size_t a_size)
{
  if (a_size < 1) {
    a_size = 1;
  }
  m_cash.resize(a_size);
  if (m_pos >= m_cash.size()) {
    m_pos = 0;
  }
}

void irs::arp_t::tick()
{
  mp_ethernet->tick();
  for (cash_item_it_t it = m_cash.begin(); it != m_cash.end(); ++it) {
    if (it->mode == dinamic_note) {
      if(it->ttl.check() || it->reset_timer.check()) {
        it->valid = false;
      }
    }
  }
  
  /*if (mp_ethernet->is_recv_buf_filled() && m_new_recv_packet) {
    m_new_recv_packet = false;
    if((mp_recv_buf[ether_type_0] == IRS_CONST_HIBYTE(ARP)) &&
      (mp_recv_buf[ether_type_1] == IRS_CONST_LOBYTE(ARP)))
    {
      mxip_t ip = IRS_TCPIP_IP(mp_recv_buf + arp_target_ip);
      if(ip == m_local_ip) {
        
      }
    } else {
      m_new_recv_packet = true;
    }
  }*/
}

//---------------------- END NEW ARP ---------------------

irs::simple_tcpip_t::simple_tcpip_t(
  simple_ethernet_t* ap_ethernet,
  const mxip_t& a_local_ip,
  const mxip_t& a_dest_ip,
  irs_size_t a_arp_cash_size
):
  mp_ethernet(ap_ethernet),
  m_buf_num(mp_ethernet->get_buf_num()),
  m_local_ip(a_local_ip),
  m_mac(mp_ethernet->get_local_mac()),
  m_recv_buf_size_icmp(0),
  m_dest_ip(a_dest_ip),
  m_dest_ip_def(a_dest_ip),
  m_user_recv_status(false),
  m_user_send_status(false),
  m_identif(0),
  m_user_recv_buf_size(0),
  m_user_send_buf_udp_size(0),
  m_user_send_buf_tcp_size(0),
  m_dest_port(0),
  m_dest_port_def(0),
  m_local_port(0),
  m_udp_open(false),
  m_tcp_open(false), // false
  mp_recv_buf(mp_ethernet->get_recv_buf()),
  mp_send_buf((m_buf_num == simple_ethernet_t::single_buf) ? 
    mp_ethernet->get_recv_buf() : mp_ethernet->get_send_buf()),
  mp_user_recv_buf(mp_ethernet->get_recv_buf()),
  mp_user_send_buf((m_buf_num == simple_ethernet_t::single_buf) ? 
    mp_ethernet->get_recv_buf() : mp_ethernet->get_send_buf()),
  m_send_buf_filled((m_buf_num == simple_ethernet_t::double_buf) ? false : 
    mp_ethernet->is_recv_buf_filled()),
  m_connection_wait_arp_timer(make_cnt_s(1)),
  m_arp_cash(a_arp_cash_size),
  //m_arp_cash(ap_ethernet, a_local_ip, a_arp_cash_size),
  m_dest_mac(IRS_TCPIP_MAC(mp_send_buf)),
  m_cur_dest_ip(mxip_t::zero_ip()),
  m_cur_dest_port(0),
  m_cur_local_port(0),
  m_send_arp(false),
  m_send_icmp(false),
  m_send_udp(false),
  m_port_list(),
  m_new_recv_packet(true),
  m_tcp_send_mode(wait_send_command_mode),
  m_udp_wait_arp(false),
  m_tcp_wait_arp(false),
  m_seq_num(0),
  m_ack_num(0),
  m_tcp_state(CLOSED),
  m_open_type(non_open), // non_open
  m_sock_max_count(a_arp_cash_size), // уточнить
  m_tcp_socket_list(0),
  m_socket(invalid_socket),
  m_list_index(0),
  m_cur_socket(invalid_socket),
  m_tcp_options_size(0),
  m_tcp_init_seq(0),
  m_disconnect_timer(make_cnt_s(20)),
  m_base_timer(make_cnt_ms(1))
{
  m_disconnect_timer.start();
}

irs::simple_tcpip_t::~simple_tcpip_t()
{

}

void irs::simple_tcpip_t::open_udp()
{
  m_udp_open = true;
}

void irs::simple_tcpip_t::close_udp()
{
  m_udp_open = false;
}

void irs::simple_tcpip_t::active_open_tcp()
{
  m_tcp_open = true;
  m_open_type = active_open;
}

void irs::simple_tcpip_t::passive_open_tcp()
{
  m_tcp_open = true;
  m_open_type = passive_open;
}

void irs::simple_tcpip_t::close_tcp()
{
  m_tcp_open = false;
}

bool irs::simple_tcpip_t::is_write_complete()
{
  return mp_ethernet->is_send_buf_empty();
}

void irs::simple_tcpip_t::write(mxip_t a_dest_ip, irs_u16 a_dest_port,
  irs_u16 a_local_port, irs_size_t a_size)
{
  if (a_dest_ip != mxip_t::zero_ip()) {
    m_dest_ip = a_dest_ip;
  } else {
    m_dest_ip = m_dest_ip_def;
  }
  if (a_dest_port) {
    m_dest_port = a_dest_port;
  } else {
    m_dest_port = m_dest_port_def;
  }
  if (a_local_port) {
    m_local_port = a_local_port;
  }
  
  if (mp_ethernet->is_send_buf_empty()) {
    if (m_buf_num == simple_ethernet_t::single_buf) {
      m_new_recv_packet = false;
    }
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("WRITE") << endl);
    // mp_ethernet->set_send_buf_locked();
    if (m_udp_open) {
      m_user_send_buf_udp_size = a_size;
    } else if (m_tcp_open) {
      m_user_send_buf_tcp_size = a_size;
    }
    m_user_send_status = true;
  }
}

irs_size_t irs::simple_tcpip_t::read(mxip_t* a_dest_ip,
  irs_u16* a_dest_port, irs_u16* a_local_port)
{
  irs_size_t read_data = 0;
  if (m_tcp_open && m_tcp_socket_list[m_list_index].state != ESTABLISHED) {
    return 0;
  }
  if (m_user_recv_status) {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("READ") << endl);
    read_data = m_user_recv_buf_size;
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_user_recv_buf_size = ") <<
      m_user_recv_buf_size << endl);
    if (a_dest_ip) {
      *a_dest_ip = m_cur_dest_ip;
    }
    if (a_dest_port) {
      *a_dest_port = m_cur_dest_port;
    }
    if (a_local_port) {
      *a_local_port = m_cur_local_port;
    }
  }
  return read_data;
}

void irs::simple_tcpip_t::read_complete()
{
  m_user_recv_status = false;
  //if (m_udp_open) {
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  //}
}

bool irs::simple_tcpip_t::tcp_cur_channel(irs_size_t* a_socket,
  mxip_t* a_dest_ip, irs_u16* a_dest_port)
{
  if (m_tcp_socket_list[m_list_index].state == ESTABLISHED) {
    *a_socket = m_tcp_socket_list[m_list_index].socket;
    *a_dest_ip = m_tcp_socket_list[m_list_index].remote_ip;
    *a_dest_port = m_tcp_socket_list[m_list_index].remote_port;
    return true;
  } else {
    return false;
  }
}

void irs::simple_tcpip_t::view_sockets_list()
{
  IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(
    mlog() << irsm("Sockets list updated: m_socket = ") << m_socket << endl;
    mlog() << irsm("m_tcp_socket_list.size() = ") <<
      m_tcp_socket_list.size() << endl;
    for (size_t list_idx = 0; list_idx < m_tcp_socket_list.size(); list_idx++)
    {
      mlog() << irsm("List index ") << list_idx;
      switch(m_tcp_socket_list[list_idx].state)
      {
        case CLOSED:
        {
          mlog() << irsm(" CLOSED ");
        } break;
        case LISTEN:
        {
          mlog() << irsm(" LISTEN ");
        } break;
        case SYN_SENT:
        {
          mlog() << irsm(" SYN_SENT ");
        } break;
        case SYN_RECEIVED:
        {
          mlog() << irsm(" SYN_RECEIVED ");
        } break;
        case ESTABLISHED:
        {
          mlog() << irsm(" ESTABLISHED ");
        } break;
        case FIN_WAIT_1:
        {
          mlog() << irsm(" FIN_WAIT_1 ");
        } break;
        case CLOSE_WAIT:
        {
          mlog() << irsm(" CLOSE_WAIT ");
        } break;
        case FIN_WAIT_2:
        {
          mlog() << irsm(" FIN_WAIT_2 ");
        } break;
        case LAST_ACK:
        {
          mlog() << irsm(" LAST_ACK");
        } break;
        case TIME_WAIT:
        {
          mlog() << irsm(" TIME_WAIT ");
        } break;
        case CLOSING:
        {
          mlog() << irsm(" CLOSING ");
        } break;
      }
      mlog() << irsm(" socket: ") <<
        m_tcp_socket_list[list_idx].socket << irsm(" remote_ip: ") <<
        m_tcp_socket_list[list_idx].remote_ip << irsm(" remote_port: ") <<
        m_tcp_socket_list[list_idx].remote_port << irsm(" local_port: ") <<
        m_tcp_socket_list[list_idx].local_port << endl;
    }
  );
}

void irs::simple_tcpip_t::view_tcp_packet(irs_u8* ap_buf)
{
  ap_buf = ap_buf;
  IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(
    mlog() << irsm("__________________________________") << endl;
    mlog() << irsm("**********************************") << endl;
    mlog() << irsm("counter, c = ") << CNT_TO_DBLTIME(counter_get()) << endl;
    mxmac_t mac_dest = IRS_TCPIP_MAC(ap_buf);
    mlog() << irsm("dest_mac = ") << mac_dest << endl;
    mxmac_t mac_source = IRS_TCPIP_MAC(ap_buf + sourse_mac);
    mlog() << irsm("src_mac = ") << mac_source << endl;
    irs_u16 ether_type = 0;
    ether_type = ntoh16(&ap_buf[ether_type_0]);
    mlog() << irsm("ether_type = ");
    switch(ether_type)
    {
      case ARP:
      {
        mlog() << irsm("ARP") << endl; 
      } break;
      case IPv4:
      {
        mlog() << irsm("IPv4") << endl;
      } break;
    }
    mlog() << irsm("ip_version = ");
    out_hex(&mlog(), ap_buf[ip_version]);
    mlog() << endl;
    irs_size_t incoming_ip_header_length =
      (ap_buf[ip_version] & 0x0F) * 4;
    mlog() << irsm("Header len = ") << incoming_ip_header_length << endl;
    mlog() << irsm("Total len = ") << ntoh16(&ap_buf[ip_length]) << endl;
    mlog() << irsm("ID = ");    
    out_hex(&mlog(), ntoh16(&ap_buf[ip_ident]));
    mlog() << endl;
    mlog() << irsm("fragmentation = ") <<
      ntoh16(&ap_buf[ip_fragment]) << endl;
    mlog() << irsm("TTL = ") << int(ap_buf[TTL]) << endl;
    mlog() << irsm("Protocol = ");
    switch(ap_buf[ip_proto_type])
    {
      case tcp_proto:
      {
        mlog() << irsm("TCP") << endl;
      } break;
      case udp_proto:
      {
        mlog() << irsm("UDP") << endl;
      } break;
      case icmp_proto:
      {
        mlog() << irsm("ICMP") << endl;
      } break;
      default:
      {
        mlog() << irsm("Unknown proto") << endl;
      } break;
    }
    mlog() << irsm("Checksum ip = ");
    out_hex(&mlog(), ntoh16(&ap_buf[ip_check_sum]));
    mlog() << endl;
    mxip_t ip_src = IRS_TCPIP_IP(ap_buf + tcp_source_ip);
    mlog() << irsm("source ip = ") << ip_src << endl;
    mxip_t ip_dest = IRS_TCPIP_IP(ap_buf + tcp_dest_ip);
    mlog() << irsm("dest ip = ") << ip_dest << endl;
    mlog() << irsm("source port = ") <<
      ntoh16(&ap_buf[tcp_local_port]) << endl;
    mlog() << irsm("dest port = ") <<
      ntoh16(&ap_buf[tcp_dest_port]) << endl;
    mlog() << irsm("seq_num = ") <<
      ntoh32(&ap_buf[tcp_sequence_number]) << endl;
    mlog() << irsm("ack_num = ") <<
      ntoh32(&ap_buf[tcp_acknowledgment_number]) << endl;
    mlog() << irsm("data_offset = ") << (ap_buf[tcp_flags_0] >> 4)*4 << endl;
    mlog() << irsm("reserved = ");
    out_hex(&mlog(), ap_buf[tcp_flags_0]);
    out_hex(&mlog(), ap_buf[tcp_flags_1]);
    mlog() << endl;
    mlog() << irsm("tcp flags =");
    if (ap_buf[tcp_flags_1] & tcp_URG) {
      mlog() << irsm(" URG ");
    }
    if (ap_buf[tcp_flags_1] & tcp_ACK) {
      mlog() << irsm(" ACK ");
    }
    if (ap_buf[tcp_flags_1] & tcp_PSH) {
      mlog() << irsm(" PUSH ");
    }
    if (ap_buf[tcp_flags_1] & tcp_RST) {
      mlog() << irsm(" RST ");
    }
    if (ap_buf[tcp_flags_1] & tcp_SYN) {
      mlog() << irsm(" SYN ");
    }
    if (ap_buf[tcp_flags_1] & tcp_FIN) {
      mlog() << irsm(" FIN ");
    }
    mlog() << endl;
    mlog() << irsm("window = ") <<
      ntoh16(&ap_buf[tcp_window_size]) << endl;
    mlog() << irsm("checksum tcp = ");
    out_hex(&mlog(), ntoh16(&ap_buf[tcp_check_sum]));
    mlog() << endl;
    mlog() << irsm("**********************************") << endl;
    mlog() << irsm("----------------------------------") << endl;
  );
}

irs_size_t irs::simple_tcpip_t::new_socket(mxip_t a_remote_ip,
  irs_u16 a_remote_port, irs_u16 a_local_port)
{
  irs_size_t list_index = 0;
  if (find_if(m_tcp_socket_list.begin(), m_tcp_socket_list.end(),
    socket_equal_t(a_remote_ip, a_remote_port, a_local_port)) ==
    m_tcp_socket_list.end())
  {
    m_socket++;
    if (m_socket != invalid_socket) {
      if (m_tcp_socket_list.size() < m_sock_max_count) {
        m_tcp_socket_list.push_front(tcp_socket_t(m_socket, LISTEN,
          a_remote_ip, a_remote_port, a_local_port, 0, 0, 0));
        list_index = 0;
      } else {
        // При заполнении всего списка новые каналы не могут вытеснить старые,
        // т.к. требуется закрытие соединения
        list_index = m_list_index;
        m_socket--;
        m_new_recv_packet = true;
        mp_ethernet->set_recv_handled();
      }
    } else {
      m_tcp_socket_list.clear();
      list_index = m_socket;
      m_socket++;
      m_tcp_socket_list.push_front(tcp_socket_t(m_socket, LISTEN, a_remote_ip,
        a_remote_port, a_local_port, 0, 0, 0));
    }
    view_sockets_list();
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
      irsm("сокет уже есть в списке сокетов") << endl);
    socket_equal_t socket_equal(a_remote_ip, a_remote_port, a_local_port);
    typedef deque<tcp_socket_t>::iterator iterator_t;
    iterator_t finded_it = find_if(m_tcp_socket_list.begin(),
      m_tcp_socket_list.end(), socket_equal);
    list_index = irs_deque_distance(m_tcp_socket_list.begin(), finded_it);
  }
  return list_index;
}

bool irs::simple_tcpip_t::cash(mxip_t a_dest_ip)
{
  bool result = false;
  const mxip_t& ip = ip_from_data(a_dest_ip);
  if (ip != mxip_t::broadcast_ip()) {
    if (m_arp_cash.ip_to_mac(ip, m_dest_mac)) {
      result = true;
    }
  } else {
    result = true;
  }
  return result;
}

irs_u16 irs::simple_tcpip_t::check_sum_ip(irs_u16 a_check_sum, irs_u8 a_dat,
  irs_size_t a_count)
{
	irs_u8 check_sum_hi = IRS_HIBYTE(a_check_sum);
	irs_u8 check_sum_lo = IRS_LOBYTE(a_check_sum);

	if (IRS_LOBYTE(a_count) & 0x01) {
		//* We are processing LSB	
		if ((check_sum_lo = static_cast<irs_u8>(check_sum_lo + a_dat)) < a_dat) {
			if (++check_sum_hi == 0 ) {
				check_sum_lo++;
      }
		}
	} else {
		//* We are processing MSB	
		if ((check_sum_hi = static_cast<irs_u8>(check_sum_hi + a_dat)) < a_dat)	{
			if (++check_sum_lo == 0) {
				check_sum_hi++;
      }
		}
	}

  IRS_HIBYTE(a_check_sum) = check_sum_hi;
	IRS_LOBYTE(a_check_sum) = check_sum_lo;
	return a_check_sum;
}

irs_u16 irs::simple_tcpip_t::check_sum(irs_size_t a_count, irs_u8* a_addr)
{
  irs_u16 ip_checksum = 0;

  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < a_count; check_sum_cnt++)
  {
    ip_checksum = 
      check_sum_ip(ip_checksum, a_addr[check_sum_cnt], check_sum_cnt);
  }

	ip_checksum = static_cast<irs_u16>(~ip_checksum);

  return ip_checksum;
}

irs_u16 irs::simple_tcpip_t::check_sum_udp(irs_size_t a_count, irs_u8* a_addr)
{
  // Массив для псевдозаголовка
  irs_u8 pseudo_header[udp_pseudo_header_length];

  // Формирование псевдозаголовка
  IRS_TCPIP_IP(pseudo_header) = IRS_TCPIP_IP(mp_send_buf + udp_source_ip);

  IRS_TCPIP_IP(pseudo_header + 4) = IRS_TCPIP_IP(mp_send_buf + udp_dest_ip);

  pseudo_header[8] = 0;
  pseudo_header[9] = udp_proto;

  pseudo_header[10] = mp_send_buf[udp_length];
  pseudo_header[11] = mp_send_buf[udp_length + 1];

  // Вычисление контрольной суммы
  irs_u16 udp_checksum = 0;

  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < udp_pseudo_header_length;
    check_sum_cnt++)
  {
    udp_checksum = 
      check_sum_ip(udp_checksum, pseudo_header[check_sum_cnt], check_sum_cnt);
  }
  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < a_count; check_sum_cnt++)
  {
    udp_checksum = 
      check_sum_ip(udp_checksum, a_addr[check_sum_cnt], check_sum_cnt);
  }

  udp_checksum = static_cast<irs_u16>(~udp_checksum);

  return udp_checksum;
}

irs_u16 irs::simple_tcpip_t::check_sum_tcp(irs_size_t a_count,
  irs_u8* a_addr)
{
  // Массив для псевдозаголовка
  irs_u8 pseudo_header[tcp_pseudo_header_length];

  // Формирование псевдозаголовка
  IRS_TCPIP_IP(pseudo_header) = IRS_TCPIP_IP(mp_send_buf + tcp_source_ip);

  IRS_TCPIP_IP(pseudo_header + 4) = IRS_TCPIP_IP(mp_send_buf + tcp_dest_ip);

  pseudo_header[8] = 0;
  pseudo_header[9] = tcp_proto;

  irs_u16 tcp_length =
    static_cast<irs_u16>(ntoh16(&mp_send_buf[ip_length]) - ip_header_length);
  pseudo_header[10] = IRS_HIBYTE(tcp_length);
  pseudo_header[11] = IRS_LOBYTE(tcp_length);

  // Вычисление контрольной суммы
  irs_u16 tcp_checksum = 0;

  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < tcp_pseudo_header_length;
    check_sum_cnt++)
  {
    tcp_checksum = 
      check_sum_ip(tcp_checksum, pseudo_header[check_sum_cnt], check_sum_cnt);
  }
  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < a_count; check_sum_cnt++)
  {
    tcp_checksum = 
      check_sum_ip(tcp_checksum, a_addr[check_sum_cnt], check_sum_cnt);
  }

  tcp_checksum = static_cast<irs_u16>(~tcp_checksum);

  return tcp_checksum;
}

//Формирование ARP-запроса
void irs::simple_tcpip_t::arp_request(mxip_t a_dest_ip)
{
  //Заголовок Ethernet:
  
  // Destination MAC
  IRS_TCPIP_MAC(mp_send_buf + dest_mac) = mxmac_t::broadcast_mac();

  // Source MAC
  IRS_TCPIP_MAC(mp_send_buf + sourse_mac) = m_mac;

  //EtherType
  mp_send_buf[ether_type_0] = IRS_CONST_HIBYTE(ARP);
  mp_send_buf[ether_type_1] = IRS_CONST_LOBYTE(ARP);
  //-----------------------------------
  
  //ARP-запрос:
  
  //Hardware type
  mp_send_buf[hardware_type_0] = IRS_CONST_HIBYTE(Ethernet);
  mp_send_buf[hardware_type_1] = IRS_CONST_LOBYTE(Ethernet);
  
  //Protocol type
  mp_send_buf[proto_type_0] = IRS_CONST_HIBYTE(IPv4);
  mp_send_buf[proto_type_1] = IRS_CONST_LOBYTE(IPv4);
  
  //Hardware lenght
  mp_send_buf[hardware_length] = mac_len;
  //Protocol lenght
  mp_send_buf[proto_length] = ip_len;
  //Operation Code
  mp_send_buf[arp_operation_code_0] = 0x0;
  mp_send_buf[arp_operation_code_1] = arp_operation_request;

  //Физический адрес отправителя
  IRS_TCPIP_MAC(mp_send_buf + arp_sender_mac) = m_mac;

  //Логический адрес отправителя
  IRS_TCPIP_IP(mp_send_buf + arp_sender_ip) = m_local_ip;

  //Физический адрес получателя
  IRS_TCPIP_MAC(mp_send_buf + arp_target_mac) = mxmac_t::broadcast_mac();

  //Логический адрес получателя
  IRS_TCPIP_IP(mp_send_buf + arp_target_ip) = a_dest_ip;

  m_send_arp = true;
}

void irs::simple_tcpip_t::arp_response()
{
  //Destination MAC
  IRS_TCPIP_MAC(mp_send_buf + dest_mac) = 
    IRS_TCPIP_MAC(mp_recv_buf + sourse_mac);

  //Source MAC
  IRS_TCPIP_MAC(mp_send_buf + sourse_mac) = m_mac;
  

  //Operation Code
  mp_send_buf[arp_operation_code_0] = IRS_CONST_HIBYTE(arp_operation_response);
  mp_send_buf[arp_operation_code_1] = IRS_CONST_LOBYTE(arp_operation_response);

  IRS_TCPIP_MAC(mp_send_buf + arp_sender_mac) = m_mac;

  IRS_TCPIP_IP(mp_send_buf + arp_sender_ip) = m_local_ip;

  IRS_TCPIP_MAC(mp_send_buf + arp_target_mac) = 
    IRS_TCPIP_MAC(mp_recv_buf + arp_sender_mac);

  IRS_TCPIP_IP(mp_send_buf + arp_target_ip) = 
    IRS_TCPIP_IP(mp_recv_buf + arp_sender_ip);

  m_send_arp = true;
}

void irs::simple_tcpip_t::arp()
{
  mxip_t ip = IRS_TCPIP_IP(mp_recv_buf + arp_target_ip);
  if(ip == m_local_ip) {
    if (mp_recv_buf[arp_operation_code_1] == arp_operation_response) {
      //ARP-ответ
      //добавляем ip и mac запрашиваемой цели в ARP-таблицу
      mxip_t& arp_ip = ip_from_data(mp_recv_buf[arp_sender_ip]);
      mxmac_t& arp_mac = mac_from_data(mp_recv_buf[arp_sender_mac]);
      m_arp_cash.add(arp_ip, arp_mac);
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
        irsm("добавляем ip и mac в ARP-таблицу") << endl);
      mp_ethernet->set_recv_handled();
      m_new_recv_packet = true;
    } else if (mp_recv_buf[arp_operation_code_1] == arp_operation_request) {
      //ARP-запрос
      //добавляем ip и mac запрашивающего в ARP-таблицу
      mxip_t& arp_ip = ip_from_data(mp_recv_buf[arp_target_ip]);
      if (!cash(arp_ip)) {
        mxmac_t& arp_mac = mac_from_data(mp_recv_buf[arp_target_mac]);
        m_arp_cash.add(arp_ip, arp_mac);
      }
      if (m_buf_num == simple_ethernet_t::double_buf) {
        memcpyex(mp_send_buf, mp_recv_buf, ARPBUF_SIZE);
      }
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
        irsm("формируем ответ на пришедший ARP-запрос") << endl);
      mp_ethernet->set_recv_handled();
      mp_ethernet->set_send_buf_locked();
      //формируем ответ на пришедший ARP-запрос
      arp_response();
    }
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("arp-запрос отклонен по ip: ") <<
      ip << endl);
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::send_arp()
{
  mp_ethernet->send_packet(ARPBUF_SENDSIZE);
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("send_arp() size = ") <<
    int(ARPBUF_SENDSIZE) << endl);
  m_send_arp = false;
  m_new_recv_packet = true;
  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("отправлен arp-пакет") << endl);
  irs::mlog() << irsm("отправлен arp-пакет") << endl;
}

void irs::simple_tcpip_t::icmp_packet()
{
  // Destination MAC
  IRS_TCPIP_MAC(mp_send_buf + dest_mac) = 
    IRS_TCPIP_MAC(mp_send_buf + sourse_mac);

  // Source MAC
  IRS_TCPIP_MAC(mp_send_buf + sourse_mac) = m_mac;
  
  // Обнуляем контрольную сумму IP
  hton16(&mp_send_buf[ip_check_sum], 0);
  
  // Контрольная сумма заголовка IP
  irs_u16 chksum_ip = check_sum(20, &mp_send_buf[0xe]);
  hton16(&mp_send_buf[ip_check_sum], chksum_ip);

  // IP-адрес приемника
  IRS_TCPIP_IP(mp_send_buf + icmp_target_ip) = 
    IRS_TCPIP_IP(mp_send_buf + icmp_sender_ip);

  // IP-адрес источника
  IRS_TCPIP_IP(mp_send_buf + icmp_sender_ip) = m_local_ip;

  // Тип ICMP - эхо-ответ
  mp_send_buf[icmp_type] = icmp_echo_response;
  // Код ICMP
  mp_send_buf[icmp_code] = 0x00;

  // Обнуляем контрольную сумму ICMP
  hton16(&mp_send_buf[icmp_check_sum], 0);

  // Контрольная сумма ICMP
  irs_u16 check_sum_icmp = check_sum(m_recv_buf_size_icmp - 0x22, 
    &mp_send_buf[0x22]);
  hton16(&mp_send_buf[icmp_check_sum], check_sum_icmp);
  
  //-------------------------------------------------------
  m_send_icmp = true;
}

void irs::simple_tcpip_t::send_icmp()
{
  IRS_LIB_ASSERT(m_recv_buf_size_icmp <= mp_ethernet->send_buf_max_size());
  mp_ethernet->send_packet(static_cast<irs_u16>(m_recv_buf_size_icmp));
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("send_icmp() size = ") <<
    int(m_recv_buf_size_icmp) << endl);
  m_send_icmp = false;
  m_new_recv_packet = true;
  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("отправлен icmp-ответ") << endl);
}

void irs::simple_tcpip_t::icmp()
{
  if ((mp_recv_buf[icmp_type] == icmp_echo_request) && 
    (mp_recv_buf[icmp_code] == 0)) // Эхо-запрос
  {
    m_recv_buf_size_icmp = mp_ethernet->recv_buf_size() - 4;
    if (m_recv_buf_size_icmp <= ICMPBUF_SIZE) {
      if (m_buf_num == simple_ethernet_t::double_buf) {
        memcpyex(mp_send_buf, mp_recv_buf, m_recv_buf_size_icmp);
      }
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("icmp запрос обработан") << endl);
      mp_ethernet->set_recv_handled();
      mp_ethernet->set_send_buf_locked();
      icmp_packet();
    }
  } else {
    m_new_recv_packet = true;
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("неправильный icmp-пакет") << endl);
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::udp_packet()
{
  IRS_TCPIP_MAC(mp_send_buf) = m_dest_mac;

  IRS_TCPIP_MAC(mp_send_buf + sourse_mac) = m_mac;

  //type
  mp_send_buf[ether_type_0] = IRS_CONST_HIBYTE(IPv4);
  mp_send_buf[ether_type_1] = IRS_CONST_LOBYTE(IPv4);
  //ver//tos
  mp_send_buf[ip_version] = 0x45;
  mp_send_buf[ip_type_of_service] = 0x10;
  //all length
  irs_size_t length_ip = m_user_send_buf_udp_size + ip_header_length +
    udp_header_length;
  hton16(&mp_send_buf[ip_length], static_cast<irs_u16>(length_ip));
  //рисуем идентификатор
  m_identif++;
  hton16(&mp_send_buf[ip_ident], m_identif);
  //fragment
  hton16(&mp_send_buf[ip_fragment], 0);
  //рисуем TTL
  mp_send_buf[TTL] = 128;
  //protocol
  mp_send_buf[ip_proto_type] = udp_proto;
  //obnulenie check_sum
  hton16(&mp_send_buf[ip_check_sum], 0);

  //ip sourse
  IRS_TCPIP_IP(mp_send_buf + udp_source_ip) = m_local_ip;

  //ip dest
  IRS_TCPIP_IP(mp_send_buf + udp_dest_ip) = m_dest_ip;
  
  //checksum_ip
  irs_u16 chksum_ip = check_sum(20, &mp_send_buf[0xe]);
  hton16(&mp_send_buf[ip_check_sum], chksum_ip);

  //ports udp
  hton16(&mp_send_buf[udp_dest_port], m_dest_port);

  hton16(&mp_send_buf[udp_local_port], m_local_port);
  //udp length
  irs_size_t length_udp = m_user_send_buf_udp_size + udp_header_length;
  hton16(&mp_send_buf[udp_length], static_cast<irs_u16>(length_udp));
  //checsum
  hton16(&mp_send_buf[udp_check_sum], 0);

  //checsum_udp
  irs_u16 chksum_udp = check_sum_udp(length_udp,
    &mp_send_buf[udp_local_port]);
  hton16(&mp_send_buf[udp_check_sum], chksum_udp);
  
  m_send_udp = true;
}

void irs::simple_tcpip_t::send_udp()
{
  if (m_user_send_buf_udp_size < 18) {
    m_user_send_buf_udp_size = 18;
  }
  IRS_LIB_ASSERT((udp_data + m_user_send_buf_udp_size) <=
    mp_ethernet->send_buf_max_size());
  mp_ethernet->send_packet(udp_data + m_user_send_buf_udp_size);
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("send_udp() size = ") <<
    int(udp_data + m_user_send_buf_udp_size) << endl);
  m_send_udp = false;
  m_user_send_status = false;
  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("send_udp() size = ") <<
    int(udp_data + m_user_send_buf_udp_size) << endl;);
  m_new_recv_packet = true;
}

void irs::simple_tcpip_t::server_udp()
{
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("recv: server_udp()") << endl);
  irs_u16 local_port = ntoh16(&mp_recv_buf[udp_dest_port]);
  if (m_port_list.find(local_port) != m_port_list.end()) {
    m_cur_local_port = local_port;
    m_cur_dest_port = ntoh16(&mp_recv_buf[udp_local_port]);
    m_cur_local_port = local_port;
    irs_u16 udp_size = ntoh16(&mp_recv_buf[udp_length]);
    m_user_recv_buf_size = static_cast<irs_u16>(udp_size - 8);
    
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("размер полученного udp пакета = ") <<
      m_user_recv_buf_size << endl);
    
    mxip_t& ip = ip_from_data(mp_recv_buf[udp_source_ip]);
    mxmac_t& mac = mac_from_data(mp_recv_buf[sourse_mac]);
    m_cur_dest_ip = ip;
    if (!cash(ip)) {
      m_arp_cash.add(ip, mac);
    }
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("port: ") << local_port <<
      irsm(" пакет принят") << endl);
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_user_recv_status = true") << endl);
    m_user_recv_status = true;
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("port: ") << local_port <<
      irsm(" пакет отклонен") << endl);
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::client_udp()
{
  if (m_user_send_status) {
    if (cash(m_dest_ip)) {
      m_udp_wait_arp = false;
      if (mp_ethernet->is_send_buf_empty()) {
        mp_ethernet->set_send_buf_locked();
        udp_packet();
      }
    } else {
      if (m_udp_wait_arp) {
        if (m_connection_wait_arp_timer.check()) {
          m_udp_wait_arp = false;
          m_user_send_status = false;
        }
      } else {
        arp_request(m_dest_ip);
        m_udp_wait_arp = true;
        m_connection_wait_arp_timer.start();
      }
    }
  }
}

void irs::simple_tcpip_t::udp()
{
  if (m_udp_open) {
    server_udp();
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("udp не открыт") << endl);
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::tcp_packet()
{
  IRS_TCPIP_MAC(mp_send_buf) = m_dest_mac;

  IRS_TCPIP_MAC(mp_send_buf + sourse_mac) = m_mac;
  
  //type
  mp_send_buf[ether_type_0] = IRS_CONST_HIBYTE(IPv4);
  mp_send_buf[ether_type_1] = IRS_CONST_LOBYTE(IPv4);
  //ver//tos
  mp_send_buf[ip_version] = 0x45;
  mp_send_buf[ip_type_of_service] = 0x10;
  //all length
  irs_size_t length_ip = m_user_send_buf_tcp_size + ip_header_length +
    tcp_header_length + m_tcp_options_size;
  hton16(&mp_send_buf[ip_length], static_cast<irs_u16>(length_ip));
  //рисуем идентификатор
  m_identif++;
  hton16(&mp_send_buf[ip_ident], m_identif);
  //fragment
  hton16(&mp_send_buf[ip_fragment], 0);
  //рисуем TTL
  mp_send_buf[TTL] = 128;
  //protocol
  mp_send_buf[ip_proto_type] = tcp_proto;
  //obnulenie check_sum
  hton16(&mp_send_buf[ip_check_sum], 0);

  //ip sourse
  IRS_TCPIP_IP(mp_send_buf + tcp_source_ip) = m_local_ip;

  //ip dest
  IRS_TCPIP_IP(mp_send_buf + tcp_dest_ip) = m_dest_ip;
  
  //checksum_ip
  irs_u16 chksum_ip = check_sum(20, &mp_send_buf[0xe]);
  hton16(&mp_send_buf[ip_check_sum], chksum_ip);
  
  //TCP packet:
  //hton16(&mp_send_buf[tcp_dest_port], m_dest_port);
  hton16(&mp_send_buf[tcp_dest_port], m_cur_dest_port);

  //hton16(&mp_send_buf[tcp_local_port], m_local_port);
  hton16(&mp_send_buf[tcp_local_port], m_cur_local_port);
  
  irs_size_t tcp_length = m_user_send_buf_tcp_size + tcp_header_length +
    m_tcp_options_size;
    //ntoh16(&mp_send_buf[ip_length]) - ip_header_length;
    //m_user_send_buf_tcp_size + tcp_header_length;
  //hton16(&mp_send_buf[tcp_window_size], static_cast<irs_u16>(tcp_length));
  
  hton16(&mp_send_buf[tcp_window_size],
    static_cast<irs_u16>(send_data_size_max()));
  
  // обнуляем check_sum_tcp
  hton16(&mp_send_buf[tcp_check_sum], 0);
  
  irs_u16 checksum_tcp = check_sum_tcp(tcp_length,
    &mp_send_buf[tcp_local_port]);
  hton16(&mp_send_buf[tcp_check_sum], checksum_tcp);
}

void irs::simple_tcpip_t::send_tcp()
{
  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("tcp send") << endl);
  view_tcp_packet(mp_send_buf);
  mp_ethernet->send_packet(m_user_send_buf_tcp_size + tcp_data +
    m_tcp_options_size);
  m_new_recv_packet = true;
  m_user_send_status = false;
}

void irs::simple_tcpip_t::tcp()
{
  if (m_tcp_open) {
    server_tcp();
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("tcp не открыт") << endl);
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::tcp_send_control(send_mode_t a_send_mode)
{
  switch(a_send_mode)
  {
    case send_SYN:
    {
      mp_ethernet->set_send_buf_locked();
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("посылаем SYN") << endl);
      mp_send_buf[tcp_flags_1] = tcp_SYN;
      m_user_send_buf_tcp_size = 0;
    } break;
    case send_ACK_SYN:
    {
      mp_ethernet->set_send_buf_locked();
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("посылаем ACK + SYN") << endl);
      mp_send_buf[tcp_flags_1] = tcp_SYN|tcp_ACK;
      m_user_send_buf_tcp_size = 0;
    } break;
    case send_ACK:
    {
      mp_ethernet->set_send_buf_locked();
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("посылаем ACK") << endl);
      mp_send_buf[tcp_flags_1] = tcp_ACK;
    } break;
    case send_FIN:
    {
      mp_ethernet->set_send_buf_locked();
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("посылаем ACK + FIN, ответ на "
        "запрос об окончании передачи данных TCP") << endl);
      mp_send_buf[tcp_flags_1] = tcp_ACK|tcp_FIN;
      //m_tcp_socket_list[m_list_index].send_unacked++;
    } break;
    case send_DATA:
    {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("send DATA") << endl);
      mp_send_buf[tcp_flags_1] = tcp_ACK|tcp_PSH;
      m_tcp_socket_list[m_list_index].send_next += m_user_send_buf_tcp_size;
    } break;
    case send_RST:
    {
      if (mp_recv_buf[tcp_flags_1] & tcp_ACK) {
        mp_send_buf[tcp_flags_1] = tcp_RST;
        m_tcp_socket_list[m_list_index].send_unacked = m_ack_num;
        m_tcp_socket_list[m_list_index].receive_next = m_seq_num;
      } else {
        mp_send_buf[tcp_flags_1] = tcp_ACK|tcp_RST;
        m_tcp_socket_list[m_list_index].send_unacked = 0;
        m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
      }
    } break;
    default:
    {
    
    } break;
  }
  hton32(&mp_send_buf[tcp_sequence_number],
    m_tcp_socket_list[m_list_index].send_unacked);
  hton32(&mp_send_buf[tcp_acknowledgment_number],
    m_tcp_socket_list[m_list_index].receive_next);
  tcp_packet();
  send_tcp();
}

void irs::simple_tcpip_t::server_tcp()
{
  irs_u16 local_port = ntoh16(&mp_recv_buf[tcp_dest_port]);
  if (m_port_list.find(local_port) != m_port_list.end())
  {
    view_tcp_packet(mp_recv_buf);
    m_cur_local_port = local_port;
    m_cur_dest_port = ntoh16(&mp_recv_buf[tcp_local_port]);
    
    irs_size_t length_ip = ntoh16(&mp_recv_buf[ip_length]);
    irs_size_t incoming_ip_header_length =
      (mp_recv_buf[ip_version] & 0x0F) * 4;
    irs_size_t incoming_tcp_header_length =
      ((mp_recv_buf[tcp_flags_0] & 0xF0) >> 4) * 4;
    m_tcp_options_size = incoming_tcp_header_length - tcp_header_length;
      
    m_user_recv_buf_size = static_cast<irs_size_t>(length_ip -
      incoming_ip_header_length - incoming_tcp_header_length);
    
    mxip_t& ip = ip_from_data(mp_recv_buf[tcp_source_ip]);
    m_cur_dest_ip = ip;
    m_dest_ip = ip;
    mxmac_t& mac = mac_from_data(mp_recv_buf[sourse_mac]);
    if (!cash(ip)) {
      m_arp_cash.add(ip, mac);
    }
    #ifdef NOP
    if (m_tcp_socket_list.size() < m_sock_max_count) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
        irsm("m_tcp_socket_list.size() < m_sock_max_count") << endl);
      m_list_index = new_socket(m_cur_dest_ip, m_cur_dest_port,
        m_cur_local_port);
    } else {
      deque<tcp_socket_t>::iterator tcp_sock_it_cur =
        find_if(m_tcp_socket_list.begin(), m_tcp_socket_list.end(),
        socket_equal_t(m_cur_dest_ip, m_cur_dest_port, m_cur_local_port));
      if (tcp_sock_it_cur != m_tcp_socket_list.end())
      {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("tcp_sock_it_cur != m_tcp_socket_list.end()") << endl);
        m_list_index = irs_deque_distance(m_tcp_socket_list.begin(),
          tcp_sock_it_cur);
      } else {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("В списке нет места для нового сокета") << endl);
        m_new_recv_packet = true;
        mp_ethernet->set_recv_handled();
        return;
      }
    }
    #endif // NOP
    m_list_index = get_tcp_connection();
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("socket = ") <<
      (m_tcp_socket_list[m_list_index].socket) << endl);
    
    m_seq_num = ntoh32(&mp_recv_buf[tcp_sequence_number]);
    m_ack_num = ntoh32(&mp_recv_buf[tcp_acknowledgment_number]);
    if (m_buf_num == simple_ethernet_t::double_buf) {
      memcpyex(mp_send_buf, mp_recv_buf, TCP_HANDSHAKE_SIZE);
    }
    m_dest_mac = mac;
    switch(m_tcp_socket_list[m_list_index].state)
    {
      case CLOSED:
      {
        if (m_open_type == passive_open) {
          m_tcp_socket_list[m_list_index].state = LISTEN;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("LISTEN") << endl);
        } else if (m_open_type == active_open) {
          m_tcp_socket_list[m_list_index].state = SYN_SENT;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("SYN_SENT") << endl);
          tcp_send_control(send_SYN);
        }
      } break;
      case LISTEN:
      {
        view_sockets_list();
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          //
        } else if (mp_recv_buf[tcp_flags_1] & tcp_ACK) {
          tcp_send_control(send_RST);
        } else if (!(mp_recv_buf[tcp_flags_1] & tcp_SYN)) {
          tcp_send_control(send_RST);
        } else if (mp_recv_buf[tcp_flags_1] & tcp_SYN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("SYN_RECEIVED") << endl);
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].send_unacked = 123456789;
          tcp_send_control(send_ACK_SYN);
          m_tcp_socket_list[m_list_index].state = SYN_RECEIVED;
          m_tcp_socket_list[m_list_index].send_next =
            m_tcp_socket_list[m_list_index].send_unacked + 1;
        } else if (m_user_send_status) { // отправка данных
          tcp_send_control(send_SYN);
          m_tcp_socket_list[m_list_index].state = SYN_SENT;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("SYN_SENT, активное открытие") << endl);
        } else if (!m_tcp_open) {
          #ifdef NOP
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
          m_tcp_socket_list.erase(m_tcp_socket_list.begin() + m_list_index);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("сокет удален из списка") << endl);
          #endif // NOP
          socket_close();
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("CLOSED, закрытие приложения") << endl);
          view_sockets_list();
        } else {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("Пришла ненужная фигня :D") << endl);
        }
      } break;
      case SYN_SENT:
      {
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ERROR:Reset received") << endl);
        } else if ((mp_recv_buf[tcp_flags_1] & tcp_SYN) &&
          (mp_recv_buf[tcp_flags_1] & tcp_ACK))
        {
          if (m_ack_num != m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("SYN+ACK received but wrong Ack") << endl);
          } else {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("Получены SYN + ACK, "
              "переходим в ESTABLISHED") << endl);
            m_tcp_socket_list[m_list_index].state = ESTABLISHED;
            m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
            m_tcp_socket_list[m_list_index].send_unacked =
              m_tcp_socket_list[m_list_index].send_next;
            tcp_send_control(send_ACK);
          }
        } else if (mp_recv_buf[tcp_flags_1] & tcp_SYN) {
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].state = SYN_RECEIVED;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("SYN_RECEIVED") << endl);
          tcp_send_control(send_ACK_SYN);
        } else if (!m_tcp_open) { // закрытие приложения или по таймауту
          #ifdef NOP
          m_tcp_socket_list[m_list_index].state = CLOSED;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
          m_tcp_socket_list.erase(m_tcp_socket_list.begin() + m_list_index);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("сокет удален из списка") << endl);
          #endif // NOP
          socket_close();
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("CLOSED, закрытие приложения") << endl);
          view_sockets_list();
        } else {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("TCP packet out of nowhere received...") << endl);
          tcp_send_control(send_RST);
        }
      } break;
      case SYN_RECEIVED:
      {
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получен флаг RST") << endl);
          m_tcp_socket_list[m_list_index].state = LISTEN;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("LISTEN") << endl);
        } else if ((mp_recv_buf[tcp_flags_1] & tcp_SYN) &&
          (mp_recv_buf[tcp_flags_1] & tcp_ACK))
        {
          if (m_ack_num != m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("SYN+ACK received but wrong Ack") << endl);
          } else {
            m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
            m_tcp_socket_list[m_list_index].send_unacked =
              m_tcp_socket_list[m_list_index].send_next;
            m_tcp_socket_list[m_list_index].state = ESTABLISHED;
            tcp_send_control(send_ACK);
          }
        } else if (mp_recv_buf[tcp_flags_1] & tcp_ACK) {
          if (m_ack_num != m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("SYN+ACK received but wrong Ack") << endl);
          } else if (m_seq_num != m_tcp_socket_list[m_list_index].receive_next)
          {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("ACK received but Wrong SEQ number") << endl);
          } else {
            m_tcp_socket_list[m_list_index].send_unacked =
              m_tcp_socket_list[m_list_index].send_next;
            m_tcp_socket_list[m_list_index].state = ESTABLISHED;
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("ACK received, connection ESTABLISHED") << endl);
          }
        } else if (mp_recv_buf[tcp_flags_1] & tcp_SYN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("Repeated SYN") << endl);
        } else if (!m_tcp_open) { // закрытие приложения
          m_tcp_socket_list[m_list_index].send_unacked++;
          m_tcp_socket_list[m_list_index].send_next++;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("FIN_WAIT_1, закрытие приложения") << endl);
          m_tcp_socket_list[m_list_index].state = FIN_WAIT_1;
          tcp_send_control(send_FIN);
        } else {
          tcp_send_control(send_RST);
        }
      } break;
      case ESTABLISHED:
      {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ESTABLISHED") << endl);
        #ifndef NOP
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("send_unacked = ") <<
          m_tcp_socket_list[m_list_index].send_unacked << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("send_next = ") <<
          m_tcp_socket_list[m_list_index].send_next << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("receive_next = ") <<
          m_tcp_socket_list[m_list_index].receive_next << endl);
        #endif // NOP
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("ERROR: Reset received") << endl);
          m_tcp_socket_list[m_list_index].state = LISTEN;
          m_new_recv_packet = true;
          mp_ethernet->set_recv_handled();
          return;
        }
        if (mp_recv_buf[tcp_flags_1] & tcp_SYN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("received SYN") << endl);
          if (mp_recv_buf[tcp_flags_1] & tcp_ACK) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("received ACK") << endl);
            if ((m_seq_num + 1) ==
              m_tcp_socket_list[m_list_index].receive_next)
            {
              if (m_ack_num == m_tcp_socket_list[m_list_index].send_next) {
                tcp_send_control(send_ACK);
                m_new_recv_packet = true;
                mp_ethernet->set_recv_handled();
                return;
              }
            }
            m_new_recv_packet = true;
            mp_ethernet->set_recv_handled();
            return;
          }
        }
        if (m_tcp_socket_list[m_list_index].send_unacked !=
          m_tcp_socket_list[m_list_index].send_next)
        {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("send_unacked != send_next") << endl);
          if (!(mp_recv_buf[tcp_flags_1] & tcp_ACK)) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("Packet without ACK and "
              "unacked data. Packet not processed") << endl);
            m_new_recv_packet = true;
            mp_ethernet->set_recv_handled();
            return;
          }
          if (m_ack_num == m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("We don't have unacked data now") << endl);
            m_tcp_socket_list[m_list_index].send_unacked =
              m_tcp_socket_list[m_list_index].send_next;
          }
        }
        if (m_tcp_socket_list[m_list_index].receive_next != m_seq_num) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("Too big sequence number received") << endl);
          tcp_send_control(send_ACK);
          m_new_recv_packet = true;
          mp_ethernet->set_recv_handled();
          return;
        }
        m_tcp_socket_list[m_list_index].receive_next +=
          m_user_recv_buf_size;
        if (mp_recv_buf[tcp_flags_1] & tcp_FIN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("Other end want's to close") << endl);
          if (m_tcp_socket_list[m_list_index].send_unacked ==
            m_tcp_socket_list[m_list_index].send_next)
          {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("send_unacked == send_next") << endl);
            m_tcp_socket_list[m_list_index].receive_next++;
            m_tcp_socket_list[m_list_index].send_next++;
            m_tcp_socket_list[m_list_index].send_unacked++;
            tcp_send_control(send_FIN);
            m_tcp_socket_list[m_list_index].state = CLOSE_WAIT;
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("CLOSE_WAIT") << endl);
            m_new_recv_packet = true;
            mp_ethernet->set_recv_handled();
            return;
          }
        }
        if (m_user_recv_buf_size) {
          m_cur_socket = m_tcp_socket_list[m_list_index].socket;
          m_user_recv_status = true;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("GET DATA") << endl);
          return;
        } else {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("Получено подтверждение о "
            "получении данных TCP") << endl);
        }
        if (!m_tcp_open) {
          if (m_tcp_socket_list[m_list_index].send_unacked ==
            m_tcp_socket_list[m_list_index].send_next)
          {
            m_tcp_socket_list[m_list_index].send_next++;
            tcp_send_control(send_FIN);
            m_tcp_socket_list[m_list_index].state = FIN_WAIT_1;
          }
        }
        m_new_recv_packet = true;
        mp_ethernet->set_recv_handled();
      } break;
      case FIN_WAIT_1:
      {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("send_unacked = ") <<
          m_tcp_socket_list[m_list_index].send_unacked << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("send_next = ") <<
          m_tcp_socket_list[m_list_index].send_next << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("receive_next = ") <<
          m_tcp_socket_list[m_list_index].receive_next << endl);
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ERROR: Reset received") << endl);
          m_tcp_socket_list[m_list_index].state = LISTEN;
        } else if ((mp_recv_buf[tcp_flags_1] & tcp_FIN) &&
          (mp_recv_buf[tcp_flags_1] & tcp_ACK))
        {
          if (m_ack_num != m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("FIN+ACK received but wrong Ack") << endl);
            m_new_recv_packet = true;
            mp_ethernet->set_recv_handled();
            return;
          }
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].receive_next += m_user_recv_buf_size;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получен ACK + FIN") << endl);
          m_tcp_socket_list[m_list_index].state = TIME_WAIT;
          tcp_send_control(send_ACK);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("TIME_WAIT") << endl);
        } else if ((mp_recv_buf[tcp_flags_1] & tcp_FIN) &&
          !(mp_recv_buf[tcp_flags_1] & tcp_ACK))
        {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("Simultaneous close, next CLOSING") << endl);
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].receive_next +=m_user_recv_buf_size;
          m_tcp_socket_list[m_list_index].state = CLOSING;
        } else if ((mp_recv_buf[tcp_flags_1] & tcp_ACK) &&
          !(mp_recv_buf[tcp_flags_1] & tcp_FIN))
        {
          if (m_ack_num != m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("FIN_WAIT_1 -> FIN_WAIT_2,"
              " ACK received but wrong Ack") << endl);
            m_new_recv_packet = true;
            mp_ethernet->set_recv_handled();
            return;
          }
          m_tcp_socket_list[m_list_index].send_unacked =
            m_tcp_socket_list[m_list_index].send_next;
          m_tcp_socket_list[m_list_index].state = FIN_WAIT_2;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("FIN_WAIT_2") << endl);
        }
      } break;
      case CLOSE_WAIT:
      {
        if (mp_recv_buf[tcp_flags_1] & tcp_ACK) {
          #ifdef NOP
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("send_unacked = ") <<
            m_tcp_socket_list[m_list_index].send_unacked << endl);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("send_next = ") <<
            m_tcp_socket_list[m_list_index].send_next << endl);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("receive_next = ") <<
            m_tcp_socket_list[m_list_index].receive_next << endl);
          #endif // NOP
          m_user_send_buf_tcp_size = 0;
          mp_send_buf[tcp_flags_1] = tcp_ACK|tcp_FIN;
          m_tcp_socket_list[m_list_index].send_next +=
            m_user_send_buf_tcp_size;
          hton32(&mp_send_buf[tcp_sequence_number],
            m_tcp_socket_list[m_list_index].send_unacked - 1);
          hton32(&mp_send_buf[tcp_acknowledgment_number],
            m_tcp_socket_list[m_list_index].receive_next++);
          tcp_packet();
          send_tcp();
          m_tcp_socket_list[m_list_index].state = LAST_ACK;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("LAST_ACK") << endl);
        }
      } break;
      case FIN_WAIT_2:
      {
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ERROR: Reset received") << endl);
          m_tcp_socket_list[m_list_index].state = LISTEN;
        } else if (mp_recv_buf[tcp_flags_1] & tcp_FIN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получен FIN") << endl);
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].receive_next +=m_user_recv_buf_size;
          m_tcp_socket_list[m_list_index].state = TIME_WAIT;
          tcp_send_control(send_ACK);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("TIME_WAIT") << endl);
          #ifdef NOP
          m_tcp_socket_list[m_list_index].state = CLOSED;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
          m_tcp_socket_list.erase(m_tcp_socket_list.begin() + m_list_index);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("сокет удален из списка") << endl);
          #endif // NOP
          socket_close();
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("CLOSED") << endl);
          view_sockets_list();
        }
      } break;
      case LAST_ACK:
      {
        #ifndef NOP
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("send_unacked = ") <<
          m_tcp_socket_list[m_list_index].send_unacked << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("send_next = ") <<
          m_tcp_socket_list[m_list_index].send_next << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("receive_next = ") <<
          m_tcp_socket_list[m_list_index].receive_next << endl);
        #endif // NOP
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ERROR: Reset received") << endl);
          m_tcp_socket_list[m_list_index].state = LISTEN;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("LISTEN") << endl);
        } else if (mp_recv_buf[tcp_flags_1] & tcp_ACK) {
          if (m_ack_num != m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("ACK received but wrong Ack") << endl);
            m_new_recv_packet = true;
            mp_ethernet->set_recv_handled();
            return;
          }
          #ifdef NOP
          m_tcp_socket_list[m_list_index].send_unacked =
            m_tcp_socket_list[m_list_index].send_next;
          m_tcp_socket_list[m_list_index].state = CLOSED;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("CLOSED") << endl);
          m_tcp_socket_list.erase(m_tcp_socket_list.begin() + m_list_index);
          #endif // NOP
          socket_close();
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("CLOSED") << endl);
          view_sockets_list();
        } else if (mp_recv_buf[tcp_flags_1] & tcp_FIN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("Repeated FIN, repeat ACK") << endl);
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].receive_next += m_user_recv_buf_size;
          m_tcp_socket_list[m_list_index].send_unacked++;
          tcp_send_control(send_FIN);
        }
      } break;
      case TIME_WAIT:
      {
        #ifdef NOP
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ERROR:Reset received") << endl);
          m_tcp_socket_list[m_list_index].state = LISTEN;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("LISTEN") << endl);
        } else if (mp_recv_buf[tcp_flags_1] & tcp_FIN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("Repeated FIN, repeat ACK") << endl);
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].receive_next += m_user_recv_buf_size;
          tcp_send_control(send_ACK);
        }
        #endif // NOP
        #ifndef NOP
        // по таймауту
        #ifdef NOP
        m_tcp_socket_list[m_list_index].state = CLOSED;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
        m_tcp_socket_list.erase(m_tcp_socket_list.begin() + m_list_index);
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("m_tcp_socket_list.size() = ") <<
            m_tcp_socket_list.size() << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("сокет удален из списка") << endl);
        #endif // NOP
        socket_close();
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("CLOSED") << endl);
        view_sockets_list();
        #endif // NOP
      } break;
      case CLOSING:
      {
        if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ERROR:Reset received") << endl);
          m_tcp_socket_list[m_list_index].state = LISTEN;
        } else if (mp_recv_buf[tcp_flags_1] & tcp_ACK) {
          if (m_ack_num != m_tcp_socket_list[m_list_index].send_next) {
            IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
              irsm("ACK received but wrong Ack") << endl);
            m_new_recv_packet = true;
            mp_ethernet->set_recv_handled();
            return;
          }
          m_tcp_socket_list[m_list_index].send_unacked =
            m_tcp_socket_list[m_list_index].send_next;
          m_tcp_socket_list[m_list_index].state = TIME_WAIT;
        } else if (mp_recv_buf[tcp_flags_1] & tcp_FIN) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("Repeated FIN, repeat ACK") << endl);
          m_tcp_socket_list[m_list_index].receive_next = m_seq_num + 1;
          m_tcp_socket_list[m_list_index].receive_next += m_user_recv_buf_size;
          tcp_send_control(send_ACK);
        }
      } break;
    } // switch(m_tcp_state)
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("port: ") << local_port <<
      irsm(" пакет отклонен") << endl);
  }
  m_new_recv_packet = true;
  mp_ethernet->set_recv_handled();
}

void irs::simple_tcpip_t::client_tcp()
{
  if (m_user_send_status && m_tcp_socket_list[m_list_index].state ==
    ESTABLISHED)
  {
    if (cash(m_dest_ip)) {
      m_tcp_wait_arp = false;
      if (mp_ethernet->is_send_buf_empty()) {
        tcp_send_control(send_DATA);
      } else {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("буфер записи занят, данные отправятся позже") << endl);
      }
    } else {
      if (m_tcp_wait_arp) {
        if (m_connection_wait_arp_timer.check()) {
          m_tcp_wait_arp = false;
          m_user_send_status = false;
        }
      } else {
        arp_request(m_dest_ip);
        m_tcp_wait_arp = true;
        m_connection_wait_arp_timer.start();
      }
    }
  }
}

irs_size_t irs::simple_tcpip_t::get_tcp_connection()
{
  if (mp_recv_buf[tcp_flags_1] & tcp_SYN) {
    // Запрос нового соединения
    for (irs_size_t list_index = 0; list_index < m_sock_max_count;
      list_index++)
    {
      if (m_tcp_socket_list[list_index].state == CLOSED) {
        m_tcp_socket_list[list_index].state = LISTEN;
        m_tcp_socket_list[list_index].remote_ip = m_dest_ip;
        m_tcp_socket_list[list_index].remote_port = m_cur_dest_port;
        m_tcp_socket_list[list_index].local_port = m_cur_local_port;
        m_tcp_socket_list[list_index].send_unacked = 0;
        m_tcp_socket_list[list_index].send_next = 0xFFFFFFFF;
        m_tcp_socket_list[list_index].receive_next = 0;
        return list_index;
      }
    }
    return 0;
  } else {
    // Сокет для соединения уже создан
    socket_equal_t socket_equal(m_dest_ip, m_cur_dest_port, m_cur_local_port);
    typedef deque<tcp_socket_t>::iterator iterator_t;
    iterator_t finded_it = find_if(m_tcp_socket_list.begin(),
      m_tcp_socket_list.end(), socket_equal);
    return irs_deque_distance(m_tcp_socket_list.begin(), finded_it);
  }
}

void irs::simple_tcpip_t::socket_close()
{
  m_tcp_socket_list[m_list_index].state = CLOSED;
  m_tcp_socket_list[m_list_index].remote_ip = mxip_t::zero_ip();
  m_tcp_socket_list[m_list_index].remote_port = 0;
  m_tcp_socket_list[m_list_index].local_port = 0;
  m_tcp_socket_list[m_list_index].send_unacked = 0;
  m_tcp_socket_list[m_list_index].send_next = 0xFFFFFFFF;
  m_tcp_socket_list[m_list_index].receive_next = 0;
}

void irs::simple_tcpip_t::tcp_init()
{
  m_tcp_socket_list.resize(m_sock_max_count, tcp_socket_t());
  for (irs_size_t list_index = 0; list_index < m_sock_max_count; list_index++)
  {
    m_tcp_socket_list[list_index].socket = list_index + 1;
  }
}

/*irs_size_t irs::simple_tcpip_t::tcp_get_socket()
{
  irs_size_t cur_socket = invalid_socket;
  
  return cur_socket;
}*/

bool irs::simple_tcpip_t::tcp_connect(irs_size_t a_socket,
  irs_u16 a_local_port, mxip_t a_dest_ip, irs_u16 a_dest_port)
{
  if ((a_socket <= 0) || (a_socket > m_sock_max_count)) {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("Socket not valid") << endl);
    return false;
  }
  if ((m_tcp_socket_list[a_socket - 1].state != CLOSED) &&
    (m_tcp_socket_list[a_socket - 1].state != LISTEN) &&
    (m_tcp_socket_list[a_socket - 1].state != TIME_WAIT))
  {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("Socket on unvalid state to"
      " initialize CONNECT") << endl);
    return false;
  }

  open_port(a_local_port);
  
  m_tcp_socket_list[a_socket - 1].remote_ip = a_dest_ip;
  m_tcp_socket_list[a_socket - 1].remote_port = a_dest_port;
  m_tcp_socket_list[a_socket - 1].local_port = a_local_port;
  m_tcp_socket_list[a_socket - 1].send_unacked = m_tcp_init_seq;
  m_tcp_socket_list[a_socket - 1].send_next =
    m_tcp_socket_list[a_socket - 1].send_unacked + 1;
  tcp_send_control(send_SYN);
  return true;
}

bool irs::simple_tcpip_t::tcp_listen(irs_size_t a_socket, irs_u16 a_port)
{
  if ((m_tcp_socket_list[a_socket - 1].state != CLOSED) &&
    (m_tcp_socket_list[a_socket - 1].state != LISTEN) &&
    (m_tcp_socket_list[a_socket - 1].state != TIME_WAIT))
  {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("Not possible to listen,"
      " socket on connected state") << endl);
    return false;
  }

  open_port(a_port);
  
  m_tcp_socket_list[a_socket - 1].remote_ip = mxip_t::zero_ip();
  m_tcp_socket_list[a_socket - 1].remote_port = 0;
  m_tcp_socket_list[a_socket - 1].local_port = a_port;
  m_tcp_socket_list[a_socket - 1].send_unacked = 0;
  m_tcp_socket_list[a_socket - 1].send_next = 0xFFFFFFFF;
  m_tcp_socket_list[a_socket - 1].receive_next = 0;
  
  return true;
}

void irs::simple_tcpip_t::tcp_close()
{
  switch(m_tcp_socket_list[m_list_index].state)
  {
    case CLOSED:
    {
    } break;
    case LISTEN:
    {
      m_tcp_socket_list[m_list_index].state = CLOSED;
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("CLOSED") << endl);
    } break;
    case SYN_SENT:
    {
      m_tcp_socket_list[m_list_index].state = CLOSED;
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("CLOSED") << endl);
    } break;
    case SYN_RECEIVED:
    {
      m_tcp_socket_list[m_list_index].send_unacked++;
      m_tcp_socket_list[m_list_index].send_next++;
      tcp_send_control(send_FIN);
      m_tcp_socket_list[m_list_index].state = FIN_WAIT_1;
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("FIN_WAIT_1") << endl);
    } break;
    case ESTABLISHED:
    {
      if (m_tcp_socket_list[m_list_index].send_unacked ==
        m_tcp_socket_list[m_list_index].send_next)
      {
        m_tcp_socket_list[m_list_index].send_next+= m_user_send_buf_tcp_size;
        m_tcp_socket_list[m_list_index].send_next++;
        tcp_send_control(send_FIN);
        m_tcp_socket_list[m_list_index].state = FIN_WAIT_1;
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("FIN_WAIT_1") << endl);
      }
    } break;
    case FIN_WAIT_1:
    case CLOSE_WAIT:
    case FIN_WAIT_2:
    case LAST_ACK:
    case TIME_WAIT:
    case CLOSING:
    {
      /*already closed*/
    } break;
  } // switch(tcp state)
}

void irs::simple_tcpip_t::ip()
{
  if(IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) == m_local_ip) {
    if (mp_recv_buf[ip_proto_type] == icmp_proto) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("recv: ip() -> icmp()") << endl);
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ip: ") <<
        IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) <<
        irsm(" пакет принят как icmp") << endl);
      icmp(); 
    } else if (mp_recv_buf[ip_proto_type] == udp_proto) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("recv: ip() -> udp()") << endl);
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ip: ") <<
        IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) <<
        irsm(" пакет принят как udp") << endl);
      udp();
    } else if (mp_recv_buf[ip_proto_type] == tcp_proto) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ip: ") <<
        IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) <<
        irsm(" пакет получен как tcp") << endl);
      tcp();
    } else {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ip: ") <<
        IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) <<
        irsm(" пакет отклонен как неизвестный протокол") << endl);
      m_new_recv_packet = true;
      mp_ethernet->set_recv_handled();
    }
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ip: ") <<
      IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) <<
      irsm(" пакет отклонен") << endl);
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  }
}

irs_u8* irs::simple_tcpip_t::get_recv_buf()
{
  if (m_udp_open) {
    return mp_recv_buf + udp_data;
  } else if (m_tcp_open) {
    return mp_recv_buf + m_tcp_options_size + tcp_data;
  } else {
    return IRS_NULL;
  }
}

irs_u8* irs::simple_tcpip_t::get_send_buf()
{
  if (m_udp_open) {
    return mp_send_buf + udp_data;
  } else if (m_tcp_open) {
    return mp_send_buf + m_tcp_options_size + tcp_data;
  } else {
    return IRS_NULL;
  }
}

bool irs::simple_tcpip_t::open_port(irs_u16 a_port)
{
  pair<set<irs_u16>::iterator, bool> insert_port =
    m_port_list.insert(a_port);
  return insert_port.second;
}

void irs::simple_tcpip_t::close_port(irs_u16 a_port)
{
  m_port_list.erase(a_port);
}

irs_size_t irs::simple_tcpip_t::recv_data_size()
{
  return m_user_recv_buf_size;
}

irs_size_t irs::simple_tcpip_t::send_data_size_max()
{
  return (mp_ethernet->send_buf_max_size() - udp_data - 4);
}

void irs::simple_tcpip_t::tick()
{
  mp_ethernet->tick();
  m_arp_cash.tick();
  
  if (mp_ethernet->is_recv_buf_filled() && m_new_recv_packet)
  {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
      irsm("обрабатываем полученный пакет") << endl);
    m_new_recv_packet = false;
    if((mp_recv_buf[ether_type_0] == IRS_CONST_HIBYTE(ARP)) &&
      (mp_recv_buf[ether_type_1] == IRS_CONST_LOBYTE(ARP)))
    {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получен arp - запрос") << endl);
      arp();
    } else if((mp_recv_buf[ether_type_0] == IRS_CONST_HIBYTE(IPv4)) &&
      (mp_recv_buf[ether_type_1] == IRS_CONST_LOBYTE(IPv4)))
    {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получен ip - пакет") << endl);
      ip();
    } else {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
        irsm("получен неизвестный пакет") << endl);
      m_new_recv_packet = true;
      mp_ethernet->set_recv_handled();
    }
  }

  if (m_udp_open) {
    client_udp();
  }

  if (!mp_ethernet->is_send_buf_empty()) {
    if (m_send_arp) {
      send_arp();
    } else if (m_send_icmp) {
      send_icmp();
    } else if (m_send_udp) {
      send_udp();
    }
  }
  
  if (m_tcp_open) {
    client_tcp();
  }
  
  // Проверка закрытия соединения TCP при инициации закрытия со стороны
  // контроллера:
  #ifdef NOP 
  static bool close_tcp = false;
  if (m_disconnect_timer.check()) {
    close_tcp = true;
  }
  if (close_tcp) {
    if (m_tcp_socket_list[m_list_index].state == ESTABLISHED) {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("TCP_INIT_CLOSE") << endl);
      tcp_close();
      close_tcp = false;
    }
  }
  #endif // NOP
  
  if (m_base_timer.check()) {
    m_tcp_init_seq++;
  }
}

void irs::simple_tcpip_t::set_ip(mxip_t& a_ip)
{
  m_local_ip = a_ip;
}

void irs::simple_tcpip_t::set_mac(mxmac_t& a_mac)
{
  mp_ethernet->set_mac(a_mac);
  m_mac = a_mac;
}
