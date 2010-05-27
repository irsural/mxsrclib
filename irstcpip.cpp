// UDP/IP-стек 
// Дата: 20.04.2010
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
  m_ip(a_local_ip),
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
  m_udp_wait_arp(false),
  m_recv_buf_filled(mp_ethernet->is_recv_buf_filled()),
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
  m_recv_arp(false),
  m_recv_icmp(false),
  m_port_list()
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
  return !m_user_send_status;
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
  m_user_send_buf_udp_size = a_size;
  m_user_send_status = true;
}

irs_size_t irs::simple_tcpip_t::read_udp(mxip_t* a_dest_ip,
  irs_u16* a_dest_port, irs_u16* a_local_port)
{
  irs_size_t data = 0;
  if (m_user_recv_status == true) {
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

irs_u16 irs::simple_tcpip_t::ip_checksum(irs_u16 a_check_sum, irs_u8 a_dat,
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
  irs_u16 ip_check_sum = 0;

  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < a_count; check_sum_cnt++)
  {
    ip_check_sum = 
      ip_checksum(ip_check_sum, a_addr[check_sum_cnt], check_sum_cnt);
  }

	ip_check_sum = static_cast<irs_u16>(0xffff^ip_check_sum);

  return ip_check_sum;
}

irs_u16 irs::simple_tcpip_t::cheksumUDP(irs_size_t a_count, irs_u8* a_addr)
{
  // Длина псевдозаголовка
  #define ph_count 12
  // Массив для псевдозаголовка
  irs_u8 pseudo_header[ph_count];

  /// Формирование псевдозаголовка
  IRS_TCPIP_IP(pseudo_header) = IRS_TCPIP_IP(mp_send_buf + udp_source_ip);

  IRS_TCPIP_IP(pseudo_header + 4) = IRS_TCPIP_IP(mp_send_buf + udp_dest_ip);

  pseudo_header[8] = 0;
  pseudo_header[9] = udp_proto;

  pseudo_header[10] = mp_send_buf[udp_length_0];
  pseudo_header[11] = mp_send_buf[udp_length_1];

  // Вычисление контрольной суммы
  irs_u16 ip_check_sum = 0;

  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < ph_count; check_sum_cnt++)
  {
    ip_check_sum = 
      ip_checksum(ip_check_sum, pseudo_header[check_sum_cnt], check_sum_cnt);
  }
  for (irs_size_t check_sum_cnt = 0; check_sum_cnt < a_count; check_sum_cnt++)
  {
    ip_check_sum = 
      ip_checksum(ip_check_sum, a_addr[check_sum_cnt], check_sum_cnt);
  }

  ip_check_sum = static_cast<irs_u16>(0xffff^ip_check_sum);

  return ip_check_sum;
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
  mp_send_buf[hardware_length] = mac_length;
  //Protocol lenght
  mp_send_buf[proto_length] = ip_length;
  //Operation Code
  mp_send_buf[arp_operation_code_0] = 0x0;
  mp_send_buf[arp_operation_code_1] = arp_operation_request;

  //Физический адрес отправителя
  IRS_TCPIP_MAC(mp_send_buf + arp_sender_mac) = m_mac;

  //Логический адрес отправителя
  IRS_TCPIP_IP(mp_send_buf + arp_sender_ip) = m_ip;

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

  IRS_TCPIP_IP(mp_send_buf + arp_sender_ip) = m_ip;

  IRS_TCPIP_MAC(mp_send_buf + arp_target_mac) = 
    IRS_TCPIP_MAC(mp_recv_buf + arp_sender_mac);

  IRS_TCPIP_IP(mp_send_buf + arp_target_ip) = 
    IRS_TCPIP_IP(mp_recv_buf + arp_sender_ip);

  m_send_arp = true;
}

//Заполнение ARP-таблицы:
void irs::simple_tcpip_t::arp_cash(void)
{
  mxip_t& arp_ip = ip_from_data(mp_recv_buf[arp_sender_ip]);
  mxmac_t& arp_mac = mac_from_data(mp_recv_buf[arp_sender_mac]);
  m_arp_cash.add(arp_ip, arp_mac);
    
  m_recv_arp = false;
}

void irs::simple_tcpip_t::arp()
{
  if(IRS_TCPIP_IP(mp_recv_buf + arp_target_ip) == m_ip) {
    if (!m_recv_arp) {
      m_recv_arp = true;
      for (irs_u8 i = 0; i < ARPBUF_SIZE; i++) {
        mp_send_buf[i] = mp_recv_buf[i];
      }
      mp_ethernet->set_recv_handled();
      if (mp_send_buf[arp_operation_code_1] == arp_operation_response) {
        //ARP-ответ
        //добавляем ip и mac в ARP-таблицу
        arp_cash(); 
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("добавляем ip и mac в ARP-таблицу") << endl);
      }
      if (mp_send_buf[arp_operation_code_1] == arp_operation_request) { 
        //ARP-запрос
        //формируем ответ на пришедший ARP-запрос 
        arp_response();
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(
          irsm("формируем ответ на пришедший ARP-запрос") << endl);
      }
    } else {
      mp_ethernet->set_recv_handled();
    }
    /*mlog() << irsm("TCPIP TEST ARP recieve buffer:") << endl;
    for(int buf_idx = 0; buf_idx < 20; buf_idx++) {
      irs::mlog() << irsm("arp_buf[") << buf_idx << irsm("] = ") <<
        int(mp_recv_buf[0x2a + buf_idx]) << endl;
    }*/
  }
}

void irs::simple_tcpip_t::send_arp(void)
{
  mp_ethernet->send_packet(ARPBUF_SENDSIZE);
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("send_arp() size = ") <<
    int(ARPBUF_SENDSIZE) << endl);
  m_recv_arp = false;
  m_send_arp = false;
}

void irs::simple_tcpip_t::icmp_packet()
{
  // Destination MAC
  IRS_TCPIP_MAC(mp_send_buf + dest_mac) = 
    IRS_TCPIP_MAC(mp_send_buf + sourse_mac);

  // Source MAC
  IRS_TCPIP_MAC(mp_send_buf + sourse_mac) = m_mac;
  
  // Обнуляем контрольную сумму IP
  mp_send_buf[check_sum_ip_0] = 0x00;
  mp_send_buf[check_sum_ip_1] = 0x00;
  
  // Контрольная сумма заголовка IP
  irs_u16 chksum_ip = check_sum(20, &mp_send_buf[0xe]);
  mp_send_buf[check_sum_ip_0] = IRS_HIBYTE(chksum_ip);
  mp_send_buf[check_sum_ip_1] = IRS_LOBYTE(chksum_ip);

  // IP-адрес приемника
  IRS_TCPIP_IP(mp_send_buf + icmp_target_ip) = 
    IRS_TCPIP_IP(mp_send_buf + icmp_sender_ip);

  // IP-адрес источника
  IRS_TCPIP_IP(mp_send_buf + icmp_sender_ip) = m_ip;

  // Тип ICMP - эхо-ответ
  mp_send_buf[icmp_type] = icmp_echo_response;
  // Код ICMP
  mp_send_buf[icmp_code] = 0x00;

  // Обнуляем контрольную сумму ICMP
  mp_send_buf[check_sum_icmp_0] = 0x00;
  mp_send_buf[check_sum_icmp_1] = 0x00;

  // Контрольная сумма ICMP
  irs_u16 check_sum_icmp = check_sum(m_recv_buf_size_icmp - 0x22, 
    &mp_send_buf[0x22]);
  mp_send_buf[check_sum_icmp_0] = IRS_HIBYTE(check_sum_icmp);
  mp_send_buf[check_sum_icmp_1] = IRS_LOBYTE(check_sum_icmp);
  
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
  m_recv_icmp = false;
}

void irs::simple_tcpip_t::icmp()
{
  if ((mp_recv_buf[icmp_type] == icmp_echo_request) && 
    (mp_recv_buf[icmp_code] == 0)) // Эхо-запрос
  {
    if (!m_recv_icmp) {
      m_recv_buf_size_icmp = mp_ethernet->recv_buf_size() - 4;
      if (m_recv_buf_size_icmp <= ICMPBUF_SIZE) {
        m_recv_icmp = true;
        if (m_buf_num == simple_ethernet_t::double_buf) {
          IRS_LIB_ASSERT(m_recv_buf_size_icmp <=
            mp_ethernet->send_buf_max_size());
          for (irs_size_t i = 0; i < m_recv_buf_size_icmp; i++) {
            mp_send_buf[i] = mp_recv_buf[i];
          }
        }
        icmp_packet();
      }
    }
  }
  mp_ethernet->set_recv_handled();
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
  //alllenth
  irs_size_t length_ip = m_user_send_buf_udp_size + 28;
  mp_send_buf[ip_length_0] = IRS_HIBYTE(length_ip);
  mp_send_buf[ip_length_1] = IRS_LOBYTE(length_ip);
  //рисуем идентификатор
  m_identif++;
  mp_send_buf[udp_ident_0] = IRS_HIBYTE(m_identif);
  mp_send_buf[udp_ident_1] = IRS_LOBYTE(m_identif);
  //fragment
  mp_send_buf[udp_fragment_0] = 0;
  mp_send_buf[udp_fragment_1] = 0;
  //рисуем TTL
  mp_send_buf[TTL] = 128;
  //protocol
  mp_send_buf[ip_proto_type] = udp_proto;
  //obnulenie check_sum
  mp_send_buf[check_sum_ip_0] = 0;
  mp_send_buf[check_sum_ip_1] = 0;

  //ip dest
  IRS_TCPIP_IP(mp_send_buf + udp_dest_ip) = m_dest_ip;
  
  //ip sourse
  IRS_TCPIP_IP(mp_send_buf + udp_source_ip) = m_ip;
  //clear checsum

  //checksum
  irs_u16 chksum_ip = check_sum(20, &mp_send_buf[0xe]);
  mp_send_buf[check_sum_ip_0] = IRS_HIBYTE(chksum_ip);
  mp_send_buf[check_sum_ip_1] = IRS_LOBYTE(chksum_ip);

  //ports udp
  mp_send_buf[udp_dest_port_0] = IRS_HIBYTE(m_dest_port);
  mp_send_buf[udp_dest_port_1] = IRS_LOBYTE(m_dest_port);

  mp_send_buf[udp_local_port_0] = IRS_HIBYTE(m_local_port);
  mp_send_buf[udp_local_port_1] = IRS_LOBYTE(m_local_port);
  //udp length
  irs_size_t udp_length = m_user_send_buf_udp_size + 8;
  mp_send_buf[udp_length_0] = IRS_HIBYTE(udp_length);
  mp_send_buf[udp_length_1] = IRS_LOBYTE(udp_length);
  //checsum
  mp_send_buf[udp_check_sum_0] = 0;
  mp_send_buf[udp_check_sum_1] = 0;

  //checsum
  irs_u16 chksum_udp = cheksumUDP(m_user_send_buf_udp_size + 8, 
    &mp_send_buf[0x22]);
  mp_send_buf[udp_check_sum_0] = IRS_HIBYTE(chksum_udp);
  mp_send_buf[udp_check_sum_1] = IRS_LOBYTE(chksum_udp);
  
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
}

void irs::simple_tcpip_t::server_udp()
{
  IRS_LIB_TCPIP_DBG_RAW_MSG_DETAIL(irsm("recv: server_udp()") << endl);
  irs_u16 local_port = 0;
  IRS_HIBYTE(local_port) = mp_recv_buf[udp_dest_port_0];
  IRS_LOBYTE(local_port) = mp_recv_buf[udp_dest_port_1];
  if (m_port_list.find(local_port) != m_port_list.end())
  {
    IRS_HIBYTE(m_cur_dest_port) = mp_recv_buf[udp_local_port_0];
    IRS_LOBYTE(m_cur_dest_port) = mp_recv_buf[udp_local_port_1];
    m_cur_local_port = local_port;
    irs_u16 udp_size = 0;
    IRS_HIBYTE(udp_size) = mp_recv_buf[udp_length_0];
    IRS_LOBYTE(udp_size) = mp_recv_buf[udp_length_1];
    m_user_recv_buf_size = static_cast<irs_u16>(udp_size - 8);
    
    mxip_t& ip = ip_from_data(mp_recv_buf[udp_source_ip]);
    mxmac_t& mac = mac_from_data(mp_recv_buf[sourse_mac]);
    m_cur_dest_ip = ip;
    m_arp_cash.add(ip, mac);
    m_user_recv_status = true;
  } else {
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::client_udp()
{
  if (m_user_send_status == true) {
    if (cash(m_dest_ip)) {
      if (m_udp_send_status == false) {
        IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("send: udp_packet()") << endl);
        udp_packet();
        m_udp_send_status = true;
        m_udp_wait_arp = false;
      }
    } else {
      if (m_udp_wait_arp) {
        if (m_udp_wait_arp_time.check()) {
          m_udp_wait_arp = false;
          m_send_udp = false;
          m_udp_send_status = false;
          m_user_send_status = false;
        }
      } else {
        if (m_recv_arp == false) {
          m_recv_arp = true;
          arp_request(m_dest_ip);
          m_udp_wait_arp = true;
          m_udp_wait_arp_time.start();
        }
      }
    }
  }
}

void irs::simple_tcpip_t::udp()
{
  if (m_udp_open == true) {
    server_udp();
  } else {
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::ip(void)
{
  if(IRS_TCPIP_IP(mp_recv_buf + udp_dest_ip) == m_ip) {
    if (mp_recv_buf[ip_proto_type] == icmp_proto) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("recv: ip() -> icmp()") << endl);
      icmp(); 
    }
    if (mp_recv_buf[ip_proto_type] == udp_proto) {
      IRS_LIB_TCPIP_DBG_RAW_MSG_BASE(irsm("recv: ip() -> udp()") << endl);
      udp();
    }
    if (mp_recv_buf[ip_proto_type] == tcp_proto) {
      
    }
  } else {
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
  
  if (mp_ethernet->is_recv_buf_filled() == true)
  {
    if((mp_recv_buf[ether_type_0] == IRS_CONST_HIBYTE(ether_type)) && 
      (mp_recv_buf[ether_type_1] == IRS_CONST_LOBYTE(ether_type)))
    {
      arp();
    }
    if((mp_recv_buf[ether_type_0] == IRS_CONST_HIBYTE(IPv4)) && 
      (mp_recv_buf[ether_type_1] == IRS_CONST_LOBYTE(IPv4)))
    {
      ip();
    } else {
      mp_ethernet->set_recv_handled();
    }
  }  

  if (m_send_arp) {
    if (((m_buf_num == simple_ethernet_t::double_buf) && 
      (m_send_buf_filled == false)) || 
      ((m_buf_num == simple_ethernet_t::single_buf) && 
      (mp_ethernet->is_recv_buf_filled() == false)))
    {
      send_arp();
    }
  }
  if (m_send_icmp) {
    if (((m_buf_num == simple_ethernet_t::double_buf) &&
      (m_send_buf_filled == false)) || 
      ((m_buf_num == simple_ethernet_t::single_buf) &&
      (mp_ethernet->is_recv_buf_filled() == false)))
    {
      send_icmp();
    }
  }
  if (m_send_udp) {
    if (((m_buf_num == simple_ethernet_t::double_buf) &&
      (m_send_buf_filled == false)) ||
      ((m_buf_num == simple_ethernet_t::single_buf) &&
      (mp_ethernet->is_recv_buf_filled() == false)))
    {
      send_udp();
    }
  }
  
  if (m_udp_open == true) {
    client_udp();
  }
}
