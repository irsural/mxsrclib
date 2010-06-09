// UDP/IP-стек 
// Дата: 09.06.2010
// Дата создания: 16.03.2010

#include <irsdefs.h>

#include <irscpp.h>
#include <irstcpip.h>
#include <timer.h>

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
  m_reset_timer(60, 1)
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

//Добавление ip и mac в ARP-таблицу
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
  m_udp_send_status(false),
  m_identif(0),
  m_user_recv_buf_size(0),
  m_user_send_buf_size(0),
  m_user_send_buf_udp_size(0),
  m_user_send_buf_tcp_size(0),
  m_dest_port(0),
  m_dest_port_def(0),
  m_local_port(0),
  m_udp_open(false),
  mp_recv_buf(mp_ethernet->get_recv_buf()),
  mp_send_buf((m_buf_num == simple_ethernet_t::single_buf) ? 
    mp_ethernet->get_recv_buf() : mp_ethernet->get_send_buf()),
  mp_user_recv_buf(mp_ethernet->get_recv_buf()),
  mp_user_send_buf((m_buf_num == simple_ethernet_t::single_buf) ? 
    mp_ethernet->get_recv_buf() : mp_ethernet->get_send_buf()),
  m_send_buf_filled((m_buf_num == simple_ethernet_t::double_buf) ? false : 
    mp_ethernet->is_recv_buf_filled()),
  m_udp_wait_arp_time(make_cnt_s(1)),
  m_arp_cash(a_arp_cash_size),
  m_dest_mac(IRS_TCPIP_MAC(mp_send_buf)),
  m_cur_dest_ip(mxip_t::zero_ip()),
  m_cur_dest_port(0),
  m_cur_local_port(0),
  m_send_arp(false),
  m_send_icmp(false),
  m_send_udp(false),
  m_send_tcp(false),
  m_recv_arp(false),
  m_port_list(),
  m_new_recv_packet(true),
  m_tcp_connected(false),
  m_client_sequence_num(0),
  m_server_sequence_num(0),
  m_tcp_client_mode(disconnected_mode),
  m_tcp_server_mode(disconnected_mode),
  m_udp_wait_arp(false),
  m_tcp_wait_arp(false)
{
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

bool irs::simple_tcpip_t::is_write_udp_complete()
{
  //return !m_user_send_status;
  return mp_ethernet->is_send_buf_empty();
}

void irs::simple_tcpip_t::write_udp(mxip_t a_dest_ip, 
  irs_u16 a_dest_port, irs_u16 a_local_port, irs_size_t a_size)
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
  if (a_size >= 18) {
    m_user_send_buf_size = a_size;
  } else {
    m_user_send_buf_size = 18;
  }

  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("вызов write") << endl);
  if (mp_ethernet->is_send_buf_empty()) {
    if (m_buf_num == simple_ethernet_t::single_buf) {
      m_new_recv_packet = false;
    }
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("старт записи") << endl);
    mp_ethernet->set_send_buf_locked();
    m_user_send_buf_udp_size = a_size;
    m_user_send_status = true;
  }
}

irs_size_t irs::simple_tcpip_t::read_udp(mxip_t* a_dest_ip,
  irs_u16* a_dest_port, irs_u16* a_local_port)
{
  irs_size_t data = 0;
  if (m_user_recv_status) {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("read_udp") << endl);
    data = m_user_recv_buf_size;
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
  return data;
}

void irs::simple_tcpip_t::read_udp_complete()
{
  m_user_recv_status = false;
  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
    irsm("read_udp_complete, m_user_recv_status = false;") << endl);
  IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(
    if (mp_recv_buf[ip_proto_type] == udp_proto) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получен udp packet"));
      irs_u16 local_port = ntoh16(&mp_recv_buf[udp_dest_port]);
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm(" ip:") <<
        IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) << irsm(" port: ") <<
        local_port << endl);
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("входящий пакет обработан") << endl);
    } else {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получен не udp!!!!!!!!!!") << endl);
    }
  );
  m_new_recv_packet = true;
  mp_ethernet->set_recv_handled();
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

  pseudo_header[10] = mp_send_buf[tcp_window_size];
  pseudo_header[11] = mp_send_buf[tcp_window_size + 1];

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
  mp_send_buf[ether_type_0] = IRS_CONST_HIBYTE(ether_type);
  mp_send_buf[ether_type_1] = IRS_CONST_LOBYTE(ether_type);
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

void irs::simple_tcpip_t::arp_response(void)
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
      IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(
        irsm("добавляем ip и mac в ARP-таблицу") << endl);
      mp_ethernet->set_recv_handled();
      m_new_recv_packet = true;
    } else if (mp_recv_buf[arp_operation_code_1] == arp_operation_request) { 
      //ARP-запрос
      //добавляем ip и mac запрашивающего в ARP-таблицу
      mxip_t& arp_ip = ip_from_data(mp_recv_buf[arp_target_ip]);
      if (cash(arp_ip)) {
        mxmac_t& arp_mac = mac_from_data(mp_recv_buf[arp_target_mac]);
        m_arp_cash.add(arp_ip, arp_mac);
      }
      if (m_buf_num == simple_ethernet_t::double_buf) {
        memcpyex(mp_send_buf, mp_recv_buf, ARPBUF_SIZE);
      }
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
        irsm("формируем ответ на пришедший ARP-запрос") << endl);
      mp_ethernet->set_send_buf_locked();
      //формируем ответ на пришедший ARP-запрос
      arp_response();
      IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(
        irsm("формируем ответ на пришедший ARP-запрос") << endl);
      if (m_buf_num == simple_ethernet_t::double_buf) {
        mp_ethernet->set_recv_handled();
      }
    }
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("arp-запрос отклонен по ip: ") <<
      ip << endl);
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::send_arp(void)
{
  mp_ethernet->send_packet(ARPBUF_SENDSIZE);
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("send_arp() size = ") <<
    int(ARPBUF_SENDSIZE) << endl);
  m_send_arp = false;
  m_new_recv_packet = true;
  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("отправлен arp-пакет") << endl);
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
      mp_ethernet->set_send_buf_locked();
      icmp_packet();
      if (m_buf_num == simple_ethernet_t::double_buf) {
        mp_ethernet->set_recv_handled();
      }
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
  IRS_LIB_ASSERT((m_user_send_buf_size + HEADERS_SIZE) <=
    mp_ethernet->send_buf_max_size());
  mp_ethernet->send_packet(m_user_send_buf_size +
    HEADERS_SIZE);
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("send_udp() size = ") <<
    int(m_user_send_buf_size + HEADERS_SIZE) << endl);
  m_send_udp = false;
  m_udp_send_status = false;
  m_user_send_status = false;
  IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("send_udp() size = ") <<
    int(m_user_send_buf_size + HEADERS_SIZE) << endl;);
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
    m_arp_cash.add(ip, mac);
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
  #ifdef NOP
  #ifdef NOP
  if (m_user_send_status) {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("client_udp: write") << endl);
    if (cash(m_dest_ip)) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
        irsm("dest_ip содержится в arp_cash") << endl);
      if (!m_udp_send_status) {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("заполнение udp-пакета") << endl);
        IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("send: udp_packet()") << endl);
        udp_packet();
        m_udp_send_status = true;
        m_udp_wait_arp = false;
      }
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("udp не ждет arp-запрос") << endl);
    } else {
      if (m_udp_wait_arp) {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("udp ждет arp-запрос") << endl);
        if (m_udp_wait_arp_time.check()) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
            irsm("отмена отправки udp-пакета") << endl);
          m_udp_wait_arp = false;
          m_send_udp = false;
          m_udp_send_status = false;
          m_user_send_status = false;
        }
      } else {
        if (!m_recv_arp) {
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("отправка arp-запроса") << endl);
          m_recv_arp = true;
          arp_request(m_dest_ip);
          m_udp_wait_arp = true;
          m_udp_wait_arp_time.start();
        }
      }
    }
  }
  #else // NOP
  if (m_user_send_status) {
    if (cash(m_dest_ip)) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("заполнение udp-пакета") << endl);
      udp_packet();
      m_user_send_status = false;
    } else {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("заполнение arp_request") << endl);
      arp_request(m_dest_ip);
      m_udp_wait_arp = true;
    }
  }
  #endif // NOP
  #endif // NOP
  
  #ifndef NOP
  if (m_user_send_status) {
    if (cash(m_dest_ip)) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("заполнение udp-пакета") << endl);
      if (m_udp_wait_arp) {
        if (mp_ethernet->is_send_buf_empty()) {
          if (m_buf_num == simple_ethernet_t::single_buf) {
            m_new_recv_packet = false;
          }
          mp_ethernet->set_send_buf_locked();
          udp_packet();
        }
      } else {
        udp_packet();
      }
      m_udp_wait_arp = false;
    } else {
      if (m_udp_wait_arp) {
        if (m_udp_wait_arp_time.check()) {
          m_udp_wait_arp = false;
          m_user_send_status = false;
        }
      } else {
        arp_request(m_dest_ip);
        m_udp_wait_arp = true;
        m_udp_wait_arp_time.start();
      }
    }
  }
  #endif // NOP
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
  mlog() << "dest_mac = " << m_dest_mac << endl;

  IRS_TCPIP_MAC(mp_send_buf + sourse_mac) = m_mac;
  mlog() << "source_mac = " << m_mac << endl;
  
  //type
  mp_send_buf[ether_type_0] = IRS_CONST_HIBYTE(IPv4);
  mp_send_buf[ether_type_1] = IRS_CONST_LOBYTE(IPv4);
  //ver//tos
  mp_send_buf[ip_version] = 0x45;
  mp_send_buf[ip_type_of_service] = 0x10;
  //all length
  irs_size_t length_ip = m_user_send_buf_tcp_size + ip_header_length +
    tcp_header_length;
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
  
  irs_size_t tcp_length = m_user_send_buf_tcp_size + tcp_header_length;
  hton16(&mp_send_buf[tcp_window_size], static_cast<irs_u16>(tcp_length));
  
  // обнуляем check_sum_tcp
  hton16(&mp_send_buf[tcp_check_sum], 0);
  
  irs_u16 checksum_tcp = check_sum_tcp(tcp_length,
    &mp_send_buf[tcp_local_port]);
  hton16(&mp_send_buf[tcp_check_sum], checksum_tcp);
    
  m_send_tcp = true;
}

void irs::simple_tcpip_t::send_tcp()
{
  mp_ethernet->send_packet(200);
  m_new_recv_packet = true;
}

void irs::simple_tcpip_t::server_tcp()
{
  /*if () { // получение пакета SYN для установки соединения
  
  } else if () { // получение пакета ACK
  
  } else if () { // получение запроса от клиента
  
  } else if () { // получение FIN, сигнализирующего об окончании передачи
  
  }*/
  
  irs_u16 local_port = ntoh16(&mp_recv_buf[tcp_dest_port]);
  if (m_port_list.find(local_port) != m_port_list.end()) {
    m_cur_local_port = local_port;
    m_cur_dest_port = ntoh16(&mp_recv_buf[udp_local_port]);
    
    irs_size_t length_ip = ntoh16(&mp_recv_buf[ip_length]);
    irs_size_t incoming_ip_header_length =
      (mp_recv_buf[ip_version] & 0x0F) * 4;
    irs_size_t incoming_tcp_header_length =
      (mp_recv_buf[tcp_flags_1] & 0xF0) * 4; ///????
      
    m_tcp_data_length_in = static_cast<irs_size_t>(length_ip -
      incoming_ip_header_length - incoming_tcp_header_length);

    if (m_tcp_connected) {  
      if (mp_recv_buf[tcp_flags_1] & tcp_FIN) {
        // сигнализирует об окончании передачи
      }
    } else { // Режим установления соединения
      if ((mp_recv_buf[tcp_flags_1] & tcp_SYN) &&
        (mp_recv_buf[tcp_flags_1] & tcp_ACK))
      {
        // переход в состояние ESTABLISHED
        m_tcp_connected = true;
      } else if (mp_recv_buf[tcp_flags_1] & tcp_SYN) {
        // запоминаем номер последовательности и посылаем сегмент с флагом ACK
        m_client_sequence_num = ntoh32(&mp_recv_buf[tcp_sequence_number]);
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("получен пакет TCP с флагом SYN, sequence_num = ") <<
          m_client_sequence_num << endl);
        memcpyex(mp_send_buf, mp_recv_buf, TCP_HANDSHAKE_SIZE);
        m_tcp_client_mode = send_ACK_SYN;
        mp_ethernet->set_recv_handled();
      } else if ((mp_recv_buf[tcp_flags_1] & tcp_ACK) && m_tcp_data_length_in) {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("получаем данные TCP") << endl);
      } else if (mp_recv_buf[tcp_flags_1] & tcp_RST) {
        // прекращение попыток соединиться
      }
    }
  } else {
    IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("port: ") << local_port <<
      irsm(" пакет отклонен") << endl);
    m_new_recv_packet = true;
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::client_tcp()
{
  if (cash(m_dest_ip)) {
    if (m_tcp_connected) {
    
    } else { // Режим установления соединения
      switch (m_tcp_client_mode)
      {
        case disconnected_mode:
        {
        
        } break;
        case send_SYN:
        {
          mp_ethernet->set_send_buf_locked();
          mp_send_buf[tcp_flags_1] |= tcp_SYN;
          hton32(&mp_send_buf[tcp_sequence_number], 0);
          tcp_packet();
          send_tcp();
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("посылаем SYN") << endl);
          m_tcp_client_mode = disconnected_mode;
        } break;
        case send_ACK_SYN:
        {
          mp_ethernet->set_send_buf_locked();
          mp_send_buf[tcp_flags_1] |= tcp_SYN;
          mp_send_buf[tcp_flags_1] |= tcp_ACK;
          hton32(&mp_send_buf[tcp_sequence_number], m_server_sequence_num);
          hton32(&mp_send_buf[tcp_acknowledgment_number],
            m_client_sequence_num + 1);
          m_user_send_buf_tcp_size = 8;
          tcp_packet();
          /*hton16(&mp_send_buf[tcp_dest_port], m_cur_dest_port);
          hton16(&mp_send_buf[tcp_local_port], m_cur_local_port);*/
          send_tcp();
          m_tcp_client_mode = disconnected_mode;
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("посылаем ответ на SYN") << endl);
          /*IRS_LIB_TCPIP_DBG_RAW_MSG_BLOCK_BASE(
            for (irs_size_t i = 0; i < TCP_HANDSHAKE_SIZE; i++) {
              mlog() << "mp_send_buf[" << i << "] = " <<
                int(mp_send_buf[i]) << endl;
            }
          );*/
        } break;
        case send_ACK_data:
        {
          mp_ethernet->set_send_buf_locked();
          mp_send_buf[tcp_flags_1] |= tcp_ACK;
          hton32(&mp_send_buf[tcp_sequence_number], m_client_sequence_num + 1);
          hton32(&mp_send_buf[tcp_acknowledgment_number],
            m_server_sequence_num + 1);
          tcp_packet();
          send_tcp();
          IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("посылаем данные TCP") << endl);
        } break;
        default:
        {
        } break;
      }
    }
  } else {
    if (m_tcp_wait_arp) {
      if (m_udp_wait_arp_time.check()) {
        m_tcp_wait_arp = false;
      }
    } else {
      if (mp_ethernet->is_send_buf_empty()) {
        if (m_buf_num == simple_ethernet_t::single_buf) {
          m_new_recv_packet = false;
        }
        mp_ethernet->set_send_buf_locked();
        arp_request(m_dest_ip);
        m_tcp_wait_arp = true;
        m_udp_wait_arp_time.start();
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("отправка arp_request") << endl);
      }
    }
  }
}

void irs::simple_tcpip_t::ip(void)
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
      #ifndef TCP_ENABLED
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ip: ") <<
        IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) <<
        irsm(" пакет отклонен как tcp") << endl);
      m_new_recv_packet = true;
      mp_ethernet->set_recv_handled();
      #else // TCP_ENABLED
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("ip: ") <<
        IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) <<
        irsm(" пакет получен как tcp") << endl);
      server_tcp();
      #endif // TCP_ENABLED
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
  return mp_recv_buf + 0x2a;
}

irs_u8* irs::simple_tcpip_t::get_send_buf()
{
  return mp_send_buf + 0x2a;
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

irs_size_t irs::simple_tcpip_t::recv_buf_size()
{
  return m_user_recv_buf_size;
}

irs_size_t irs::simple_tcpip_t::send_data_size_max()
{
  return (mp_ethernet->send_buf_max_size() - 0x2a - 4);
}

void irs::simple_tcpip_t::tick()
{
  mp_ethernet->tick();
  
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
      #ifdef NOP
      if (m_udp_wait_arp) {
        m_udp_wait_arp = false;
        if (m_buf_num == simple_ethernet_t::single_buf) {
          m_new_recv_packet = false;
        }
        mp_ethernet->set_send_buf_locked();
      }
      #endif // NOP
    } else if (m_send_icmp) {
      send_icmp();
    } else if (m_send_udp) {
      send_udp();
    }
  }
  
  #ifdef TCP_ENABLED
  client_tcp();
  #endif // TCP_ENABLED
}
