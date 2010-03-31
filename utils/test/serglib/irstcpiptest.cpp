// UDP/IP-стек 
// Дата: 23.03.2010
// дата создания: 16.03.2010

/************************************************
Задачи:

*Таймаут на время обработки принятого UDP-пакета

*************************************************/

#include <irsdefs.h>

#include <irscpp.h>
//#include <irstcpip.h>
#include <irstcpiptest.h>

#include <irsfinal.h>

const irs::ip_t& irs::zero_ip()
{
  static const ip_t zero_ip_var = {{0, 0, 0, 0}};
  return zero_ip_var;
}

const irs::ip_t& irs::broadcast_ip()
{
  static const ip_t broadcast_ip_var = {{0xFF, 0xFF, 0xFF, 0xFF}};
  return broadcast_ip_var;
}

const irs::mac_t& irs::zero_mac()
{
  static const mac_t zero_mac_var = {{0, 0, 0, 0, 0, 0}};
  return zero_mac_var;
}

const irs::mac_t& irs::broadcast_mac()
{
  static const mac_t broadcast_mac_var = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
  return broadcast_mac_var;
}

#define IRS_TCPIP_MAC(data) (*(irs::mac_t*)(data))
#define IRS_TCPIP_IP(data) (*(irs::ip_t*)(data))

// Переобразование данных в mac_t
#define IRS_UDP_MAC(data) (*(irs::mac_t*)(&(data)))
template <class T>
inline irs::mac_t& mac_from_data(T& data);
template <class T>
inline irs::mac_t& mac_from_data(T* data);

// Преобразование данных в ip_t
#define IRS_UDP_IP(data) (*(irs::ip_t*)(&(data)))
template <class T>
inline irs::ip_t& ip_from_data(T& data);
template <class T>
inline irs::ip_t& ip_from_data(T* data);

// Переобразование данных в mac_t
template <class T>
inline irs::mac_t& mac_from_data(T& data)
{
  return IRS_UDP_MAC(data);
}
template <class T>
inline irs::mac_t& mac_from_data(T* data)
{
  return IRS_UDP_MAC(*data);
}

// Преобразование данных в ip_t
template <class T>
inline irs::ip_t& ip_from_data(T& data)
{
  return IRS_UDP_IP(data);
}
template <class T>
inline irs::ip_t& ip_from_data(T* data)
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
bool irs::arp_cash_t::ip_to_mac(const irs::ip_t& a_ip, irs::mac_t& a_mac)
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
void irs::arp_cash_t::add(const irs::ip_t& a_ip, const irs::mac_t& a_mac)
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
  const ip_t& a_ip,
  irs_size_t a_arp_cash_size
):
  mp_ethernet(ap_ethernet),
  m_buf_num(mp_ethernet->get_buf_num()),
  m_ip(a_ip),
  m_mac(mp_ethernet->get_local_mac()),
  m_recv_buf_size_icmp(0),
  m_dest_ip(a_ip),
  m_dest_ip_def(a_ip),
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
  m_recv_buf_filled(mp_ethernet->is_recv_buf_filled()),
  m_send_buf_filled((m_buf_num == simple_ethernet_t::double_buf) ? false : 
    mp_ethernet->is_recv_buf_filled()),
  m_udp_wait_arp(false),
  m_udp_wait_arp_time(make_cnt_s(1)),
  m_recv_buf_size(mp_ethernet->recv_buf_size()),
  m_arp_cash(a_arp_cash_size),
  m_dest_mac(IRS_TCPIP_MAC(mp_send_buf)),
  m_blink_0(irs_avr_porte, 7),
  m_blink_1(irs_avr_porte, 5),
  m_blink_2(irs_avr_porte, 2),
  m_blink_3(irs_avr_portf, 0),
  m_send_arp(false),
  m_send_icmp(false),
  m_send_udp(false),
  m_recv_arp(false),
  m_recv_icmp(false)
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

void irs::simple_tcpip_t::write_udp(ip_t a_dest_ip, 
  irs_u16 a_dest_port, irs_u16 a_local_port, irs_size_t a_size)
{
  if (a_dest_ip != zero_ip()) {
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

irs_size_t irs::simple_tcpip_t::read_udp(ip_t* a_dest_ip,
  irs_u16* a_dest_port, irs_u16* a_local_port)
{
  irs_size_t data = 0;
  if (m_user_recv_status == true) {
    data = m_user_recv_buf_size;
    if (a_dest_ip) {
      IRS_LODWORD(*a_dest_ip) = IRS_LODWORD(mp_recv_buf[0x1A]);
    }
    if (a_dest_port) {
      IRS_HIBYTE(*a_dest_port) = mp_recv_buf[0x22];
      IRS_LOBYTE(*a_dest_port) = mp_recv_buf[0x23];
    }
    if (a_local_port) {
      IRS_HIBYTE(*a_local_port) = mp_recv_buf[0x24];
      IRS_LOBYTE(*a_local_port) = mp_recv_buf[0x25];
    }
  }
  return data;
}

void irs::simple_tcpip_t::is_read_udp_complete()
{
  m_user_recv_status = false;
  mp_ethernet->set_recv_handled();
}

bool irs::simple_tcpip_t::cash(ip_t a_dest_ip)
{
  bool result = false;
  const ip_t& ip = ip_from_data(a_dest_ip);
  if (ip != broadcast_ip()) {
    if (m_arp_cash.ip_to_mac(ip, m_dest_mac)) {
      result = true;
    }
  } else {
    result = true;
  }
  return result;
}

irs_u16 irs::simple_tcpip_t::ip_checksum(irs_u16 a_check_sum, irs_u8 a_dat,
  irs_u16 a_count)
{
	irs_u8 check_sum_hi = IRS_HIBYTE(a_check_sum);
	irs_u8 check_sum_lo = IRS_LOBYTE(a_check_sum);

	if (IRS_LOBYTE(a_count) & 0x01) {
		//* We are processing LSB	
		if ((check_sum_lo = check_sum_lo + a_dat) < a_dat) {
			if (++check_sum_hi == 0 ) {
				check_sum_lo++;
      }
		}
	} else {
		//* We are processing MSB	
		if ((check_sum_hi = check_sum_hi + a_dat) < a_dat)	{
			if (++check_sum_lo == 0) {
				check_sum_hi++;
      }
		}
	}

  IRS_HIBYTE(a_check_sum) = check_sum_hi;
	IRS_LOBYTE(a_check_sum) = check_sum_lo;

	return a_check_sum;
}

irs_u16 irs::simple_tcpip_t::check_sum(irs_u16 a_count, irs_u8* a_addr)
{
  irs_u16 ip_check_sum = 0;

  for (irs_u16 check_sum_cnt = 0; check_sum_cnt < a_count; check_sum_cnt++) {
    ip_check_sum = 
      ip_checksum(ip_check_sum, a_addr[check_sum_cnt], check_sum_cnt);
  }

	ip_check_sum = 0xffff^ip_check_sum;

  return ip_check_sum;
}

irs_u16 irs::simple_tcpip_t::cheksumUDP(irs_u16 a_count, irs_u8* a_addr)
{
  // Длина псевдозаголовка
  #define ph_count 12
  // Массив для псевдозаголовка
  irs_u8 pseudo_header[ph_count];

  /// Формирование псевдозаголовка
  pseudo_header[0] = mp_send_buf[0x1a];
  pseudo_header[1] = mp_send_buf[0x1b];
  pseudo_header[2] = mp_send_buf[0x1c];
  pseudo_header[3] = mp_send_buf[0x1d];

  pseudo_header[4] = mp_send_buf[0x1e];
  pseudo_header[5] = mp_send_buf[0x1f];
  pseudo_header[6] = mp_send_buf[0x20];
  pseudo_header[7] = mp_send_buf[0x21];

  pseudo_header[8] = 0;
  pseudo_header[9] = udp_proto;

  pseudo_header[10] = mp_send_buf[0x26];
  pseudo_header[11] = mp_send_buf[0x27];

  // Вычисление контрольной суммы
  irs_u16 ip_check_sum = 0;

  for (irs_u16 check_sum_cnt = 0; check_sum_cnt < ph_count; check_sum_cnt++) {
    ip_check_sum = 
      ip_checksum(ip_check_sum, pseudo_header[check_sum_cnt], check_sum_cnt);
  }
  for (irs_u16 check_sum_cnt = 0; check_sum_cnt < a_count; check_sum_cnt++) {
    ip_check_sum = 
      ip_checksum(ip_check_sum, a_addr[check_sum_cnt], check_sum_cnt);
  }

  ip_check_sum = 0xffff^ip_check_sum;

  return ip_check_sum;
    
}

//Формирование ARP-запроса
void irs::simple_tcpip_t::arp_request(ip_t a_dest_ip)
{
  //Заголовок Ethernet:
  
  // Destination MAC
  IRS_TCPIP_MAC(mp_send_buf + dest_mac) = broadcast_mac();

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
  mp_send_buf[arp_operation_code_0] = IRS_CONST_HIBYTE(arp_operation_request);
  mp_send_buf[arp_operation_code_1] = IRS_CONST_LOBYTE(arp_operation_request);

  //Физический адрес отправителя
  IRS_TCPIP_MAC(mp_send_buf + sender_mac) = m_mac;

  //Логический адрес отправителя
  IRS_TCPIP_IP(mp_send_buf + sender_ip) = m_ip;

  //Физический адрес получателя
  IRS_TCPIP_MAC(mp_send_buf + target_mac) = broadcast_mac();

  //Логический адрес получателя
  IRS_TCPIP_IP(mp_send_buf + target_ip) = a_dest_ip;

  m_send_arp = true;
}

void irs::simple_tcpip_t::arp_response(void)
{
  //Destination MAC
  IRS_TCPIP_MAC(mp_send_buf + dest_mac) = 
    IRS_TCPIP_MAC(mp_send_buf + sourse_mac);
  /*mp_send_buf[0x0] = mp_recv_buf[0x6];
  mp_send_buf[0x1] = mp_recv_buf[0x7];
  mp_send_buf[0x2] = mp_recv_buf[0x8];
  mp_send_buf[0x3] = mp_recv_buf[0x9];
  mp_send_buf[0x4] = mp_recv_buf[0xa];
  mp_send_buf[0x5] = mp_recv_buf[0xb];*/

  //Source MAC
  IRS_TCPIP_MAC(mp_send_buf + 0x6) = m_mac;
  

  //Operation Code
  mp_send_buf[0x15] = arp_operation_response;

  IRS_TCPIP_MAC(mp_send_buf + 0x16) = m_mac;

  IRS_TCPIP_IP(mp_send_buf + 0x1c) = m_ip;

  mp_send_buf[0x20] = mp_recv_buf[0x16];
  mp_send_buf[0x21] = mp_recv_buf[0x17];
  mp_send_buf[0x22] = mp_recv_buf[0x18];
  mp_send_buf[0x23] = mp_recv_buf[0x19];
  mp_send_buf[0x24] = mp_recv_buf[0x1a];
  mp_send_buf[0x25] = mp_recv_buf[0x1b];

  mp_send_buf[0x26] = mp_recv_buf[0x1c];
  mp_send_buf[0x27] = mp_recv_buf[0x1d];
  mp_send_buf[0x28] = mp_recv_buf[0x1e];
  mp_send_buf[0x29] = mp_recv_buf[0x1f];

  m_send_arp = true;
}

//Заполнение ARP-таблицы:
void irs::simple_tcpip_t::arp_cash(void)
{
  irs::ip_t& arp_ip = ip_from_data(mp_recv_buf[0x1c]);
  irs::mac_t& arp_mac = mac_from_data(mp_recv_buf[0x16]);
  m_arp_cash.add(arp_ip, arp_mac);
    
  m_recv_arp = false;
}

void irs::simple_tcpip_t::arp()
{
  if (m_recv_arp == false) {
    m_recv_arp = true;
    for (irs_u8 i = 0; i < ARPBUF_SIZE; i++) {
      mp_send_buf[i] = mp_recv_buf[i];
    }
    mp_ethernet->set_recv_handled();
    if (mp_send_buf[0x15] == arp_operation_response) { //ARP-ответ
      //добавляем ip и mac в ARP-таблицу
      m_blink_1();
      arp_cash(); 
    }
    if (mp_send_buf[0x15] == arp_operation_request) { //ARP-запрос
      //формируем ответ на пришедший ARP-запрос 
      m_blink_2();
      arp_response();
    }
  }
}

void irs::simple_tcpip_t::send_arp(void)
{
  mp_ethernet->send_packet(ARPBUF_SENDSIZE);
  m_recv_arp = false;
  m_send_arp = false;
}

void irs::simple_tcpip_t::icmp_packet()
{
  // Destination MAC
  mp_send_buf[0x0] = mp_send_buf[0x6];
  mp_send_buf[0x1] = mp_send_buf[0x7];
  mp_send_buf[0x2] = mp_send_buf[0x8];
  mp_send_buf[0x3] = mp_send_buf[0x9];
  mp_send_buf[0x4] = mp_send_buf[0xa];
  mp_send_buf[0x5] = mp_send_buf[0xb];

  // Source MAC
  IRS_TCPIP_MAC(mp_send_buf + 0x6) = m_mac;
  
  // Обнуляем контрольную сумму IP
  mp_send_buf[0x18] = 0x00;
  mp_send_buf[0x19] = 0x00;
  
  // Контрольная сумма заголовка IP
  irs_u16 chksum_ip = check_sum(20, &mp_send_buf[0xe]);
  mp_send_buf[0x18] = IRS_HIBYTE(chksum_ip);
  mp_send_buf[0x19] = IRS_LOBYTE(chksum_ip);

  // IP-адрес приемника
  mp_send_buf[0x1e] = mp_send_buf[0x1a];
  mp_send_buf[0x1f] = mp_send_buf[0x1b];
  mp_send_buf[0x20] = mp_send_buf[0x1c];
  mp_send_buf[0x21] = mp_send_buf[0x1d];

  // IP-адрес источника
  IRS_TCPIP_IP(mp_send_buf + 0x1a) = m_ip;

  // Тип ICMP - эхо-ответ
  mp_send_buf[0x22] = 0x00;
  // Код ICMP
  mp_send_buf[0x23] = 0x00;

  // Обнуляем контрольную сумму ICMP
  mp_send_buf[0x24] = 0x00;
  mp_send_buf[0x25] = 0x00;

  // Контрольная сумма ICMP
  irs_u16 check_sum_icmp = check_sum(m_recv_buf_size_icmp - 0x22, 
    &mp_send_buf[0x22]);
  mp_send_buf[0x24] = IRS_HIBYTE(check_sum_icmp);
  mp_send_buf[0x25] = IRS_LOBYTE(check_sum_icmp);
  
  //-------------------------------------------------------
  m_send_icmp = true;
}

void irs::simple_tcpip_t::send_icmp()
{
  mp_ethernet->send_packet(m_recv_buf_size_icmp);
  #ifdef DBGMODE
  PORTB ^= (1 << PORTB2);
  #endif //DBGMODE
  m_send_icmp = false;
  m_recv_icmp = false;
}

void irs::simple_tcpip_t::icmp()
{
  if ((mp_recv_buf[0x22] == 8) && (mp_recv_buf[0x23] == 0)) // Эхо-запрос
  {
    if (m_recv_icmp == false) {
      m_recv_buf_size_icmp = mp_ethernet->recv_buf_size() - 4;
      if (m_recv_buf_size_icmp <= ICMPBUF_SIZE) {
        m_recv_icmp = true;
        if (m_buf_num == simple_ethernet_t::double_buf) {
          for (irs_i16 i = 0; i < m_recv_buf_size_icmp; i++) {
            mp_send_buf[i] = mp_recv_buf[i];
          }
        }
        m_blink_3();
        icmp_packet();
      }
    }
  }
  mp_ethernet->set_recv_handled();
}

void irs::simple_tcpip_t::server_udp()
{
  if ((mp_recv_buf[0x24] == IRS_HIBYTE(m_local_port)) &&
      (mp_recv_buf[0x25] == IRS_LOBYTE(m_local_port)))
  {
    m_user_recv_status = true;
    size_t udp_size = 0;
    IRS_LOBYTE(udp_size) = mp_recv_buf[0x27];
    IRS_HIBYTE(udp_size) = mp_recv_buf[0x26];
    m_user_recv_buf_size = udp_size - 8;
    
    irs::ip_t& ip = ip_from_data(mp_recv_buf[0x1A]);
    irs::mac_t& mac = mac_from_data(mp_recv_buf[0x06]);
    m_arp_cash.add(ip, mac);
  } else {
    mp_ethernet->set_recv_handled();
  }
}

void irs::simple_tcpip_t::udp_packet()
{
  IRS_TCPIP_MAC(mp_send_buf) = m_dest_mac;

  IRS_TCPIP_MAC(mp_send_buf + 0x6) = m_mac;

  //type
  mp_send_buf[0xc] = static_cast<irs_u8>(IPv4 >> 8); //0x08
  mp_send_buf[0xd] = static_cast<irs_u8>(IPv4 & 0xFF); //0x00
  //ver//tos
  mp_send_buf[0xe] = 0x45;
  mp_send_buf[0xf] = 0x10;
  //all_lenth
  irs_u16 length_ip = m_user_send_buf_udp_size + 28;
  mp_send_buf[0x11] = IRS_LOBYTE(length_ip);
  mp_send_buf[0x10] = IRS_HIBYTE(length_ip);
  //рисуем идентификатор
  m_identif++;
  mp_send_buf[0x13] = IRS_LOBYTE(m_identif);
  mp_send_buf[0x12] = IRS_HIBYTE(m_identif);
  //fragment
  mp_send_buf[0x14] = 0;
  mp_send_buf[0x15] = 0;
  //рисуем TTL
  mp_send_buf[0x16] = 128;
  //protocol
  mp_send_buf[0x17] = udp_proto;
  //obnulenie check_sum
  mp_send_buf[0x18] = 0;
  mp_send_buf[0x19] = 0;

  //ip dest
  IRS_TCPIP_IP(mp_send_buf + 0x1e) = m_dest_ip;
  
  //ip sourse
  IRS_TCPIP_IP(mp_send_buf + 0x1a) = m_ip;
  //clear checsum

  //checksum
  irs_u16 chksum_ip = check_sum(20, &mp_send_buf[0xe]);
  mp_send_buf[0x18] = IRS_HIBYTE(chksum_ip);
  mp_send_buf[0x19] = IRS_LOBYTE(chksum_ip);

  //ports udp
  mp_send_buf[0x24] = IRS_HIBYTE(m_dest_port);
  mp_send_buf[0x25] = IRS_LOBYTE(m_dest_port);

  mp_send_buf[0x22] = IRS_HIBYTE(m_local_port);
  mp_send_buf[0x23] = IRS_LOBYTE(m_local_port);
  //udp length
  irs_u16 udp_length = m_user_send_buf_udp_size + 8;
  mp_send_buf[0x27] = IRS_LOBYTE(udp_length);
  mp_send_buf[0x26] = IRS_HIBYTE(udp_length);
  //checsum
  mp_send_buf[0x28] = 0;
  mp_send_buf[0x29] = 0;

  //checsum
  irs_u16 chksum_udp = cheksumUDP(m_user_send_buf_udp_size + 8, 
    &mp_send_buf[0x22]);
  mp_send_buf[0x28] = IRS_HIBYTE(chksum_udp);
  mp_send_buf[0x29] = IRS_LOBYTE(chksum_udp);
  
  m_send_udp = true;
}

void irs::simple_tcpip_t::send_udp()
{
  mp_ethernet->send_packet(m_user_send_buf_size + HEADERS_SIZE);
  m_send_udp = false;
  m_udp_send_status = false;
  m_user_send_status = false;
}

void irs::simple_tcpip_t::client_udp()
{
  if (m_user_send_status == true) {
    if (cash(m_dest_ip)) {
      if (m_udp_send_status == false) {
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
  if (mp_recv_buf[0x17] == icmp_proto) {
    icmp(); 
  }
  if (mp_recv_buf[0x17] == udp_proto) {
    udp();
  }
}

void irs::simple_tcpip_t::tick()
{
  mp_ethernet->tick();
  m_blink_0();
  if (mp_ethernet->is_recv_buf_filled() == true)
  {
    if((mp_recv_buf[0x26] == m_ip.val[0]) && 
      (mp_recv_buf[0x27] == m_ip.val[1]) &&
      (mp_recv_buf[0x28] == m_ip.val[2]) && 
      (mp_recv_buf[0x29] == m_ip.val[3])) 
    {
      arp();
    }
    if ((mp_recv_buf[0x1e] == m_ip.val[0]) && 
      (mp_recv_buf[0x1f] == m_ip.val[1]) && 
      (mp_recv_buf[0x20] == m_ip.val[2]) && 
      (mp_recv_buf[0x21] == m_ip.val[3]))
    {
      ip();
    }
    mp_ethernet->set_recv_handled();
  }

  if (m_udp_open == true) {
    client_udp();
  }

  if (m_send_arp) {
    if (m_send_buf_filled == false) {
      send_arp();
    }
  }
  if (m_send_icmp) {
    if (m_send_buf_filled == false) {
      static blink_t blink_4(irs_avr_porte, 3);
      blink_4.set();
      send_icmp();
    }
  }
  if (m_send_udp) {
    if (m_send_buf_filled == false) {
      send_udp();
    }
  }
}
