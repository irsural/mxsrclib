// UDP/IP-стек 
// Дата: 17.03.2010
// дата создания: 16.03.2010

#include <irsdefs.h>

#include <irscpp.h>
#include <irstcpip.h>
#include <timer.h>

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

// ARP-кэш
class arp_cash_t
{
public:
  static const irs_uarc m_def_size = 3;
  
  arp_cash_t(irs_uarc a_size = m_def_size);
  bool ip_to_mac(const irs::ip_t& a_ip, irs::mac_t& a_mac);
  void add(const irs::ip_t& a_ip, const irs::mac_t& a_mac);
  inline irs_uarc size() const;
  void resize(irs_uarc a_size);
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
  irs_uarc m_pos;
  irs::loop_timer_t m_reset_timer;
};

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
arp_cash_t::arp_cash_t(irs_uarc a_size):
  m_cash(a_size),
  m_pos(0),
  m_reset_timer(60, 1)
{
}
bool arp_cash_t::ip_to_mac(const irs::ip_t& a_ip, irs::mac_t& a_mac)
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
    irs_uarc save_size = m_cash.size();
    m_cash.clear();
    m_cash.resize(save_size);
  }
  return false;
}
void arp_cash_t::add(const irs::ip_t& a_ip, const irs::mac_t& a_mac)
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
    if (m_pos > m_cash.size()) m_pos = 0;
  }
}

inline irs_uarc arp_cash_t::size() const
{
  return (irs_uarc)m_cash.size();
}

void arp_cash_t::resize(irs_uarc a_size)
{
  if (a_size < 1) a_size = 1;
  m_cash.resize(a_size);
  if (m_pos > m_cash.size()) m_pos = 0;
}

irs::hardflow::simple_udp_t::simple_udp_t(
  ethernet_rtl_t* ap_ethernet_rtl,
  const irs_u8* ap_mac, 
  const irs_u8* ap_ip,
  irs_avr_port_t a_data_port,
  irs_avr_port_t a_address_port
):
  mp_ethernet_rtl(ap_ethernet_rtl),
  m_ip(IRS_UDP_IP_SIZE),
  m_mac(IRS_UDP_MAC_SIZE),
  m_arp_buf(ARPBUF_SENDSIZE),
  m_icmp_buf(ICMPBUF_SIZE),
  m_arp_cash(10),
  m_rx_len_icmp(0),
  m_dest_ip(IRS_UDP_IP_SIZE),
  m_dest_ip_def(IRS_UDP_IP_SIZE),
  m_user_rx(false),
  m_user_tx(false),
  m_tx_udp(false),
  m_identif(0),
  m_rx_user_length(0),
  m_tx_user_length_hard(0),
  m_tx_user_length_udp(0),
  m_dest_port(0),
  m_dest_port_def(0),
  m_local_port(0),
  m_udp_open(false),
  mp_rx_buf(mp_ethernet_rtl->get_rx_buf()),
  mp_tx_buf(mp_ethernet_rtl->get_tx_buf()),
  m_user_rx_buf(mp_ethernet_rtl->get_rx_buf()),
  m_user_tx_buf(mp_ethernet_rtl->get_tx_buf()),
  m_rx_arp(false),
  m_send_arp(false),
  m_rx_icmp(false),
  m_send_icmp(false),
  m_send_udp(false),
  m_rx_hard(mp_ethernet_rtl->get_rx_hard()),
  m_tx_hard(mp_ethernet_rtl->get_tx_hard()),
  m_rx_len_hard(mp_ethernet_rtl->get_rx_len_hard())
{
  /*m_rx_buf = mp_ethernet_rtl->get_rx_buf();
  m_tx_buf = mp_ethernet_rtl->get_tx_buf();*/
  mp_ethernet_rtl->set_rtl_ports(a_data_port, a_address_port);
  mp_ethernet_rtl->init_rtl(ap_mac);
  
  m_ip[0] = ap_ip[0];
  m_ip[1] = ap_ip[1];
  m_ip[2] = ap_ip[2];
  m_ip[3] = ap_ip[3];

  m_mac[0] = ap_mac[0];
  m_mac[1] = ap_mac[1];
  m_mac[2] = ap_mac[2];
  m_mac[3] = ap_mac[3];
  m_mac[4] = ap_mac[4];
  m_mac[5] = ap_mac[5];

  memset(m_arp_buf.data(), 0, ARPBUF_SENDSIZE);
  memset(m_icmp_buf.data(), 0, ICMPBUF_SIZE);
}

irs::hardflow::simple_udp_t::~simple_udp_t()
{

}

irs::hardflow::simple_udp_t::size_type
  irs::hardflow::simple_udp_t::read(size_type /*a_channel_ident*/, irs_u8* /*ap_buf*/,
  size_type /*a_size*/)
{
  return 0;
}

irs::hardflow::simple_udp_t::size_type
  irs::hardflow::simple_udp_t::write(size_type /*a_channel_ident*/, 
  const irs_u8* /*ap_buf*/, size_type /*a_size*/)
{
  return 0;
}

irs::hardflow::simple_udp_t::size_type
  irs::hardflow::simple_udp_t::channel_next()
{
  size_type channel = invalid_channel;
  return channel;
}

bool irs::hardflow::simple_udp_t::is_channel_exists(size_type /*a_channel_iden*/)
{
  return true;
}

irs::string irs::hardflow::simple_udp_t::param(const irs::string& /*a_name*/)
{
  irs::string param;
  return param;
}

void irs::hardflow::simple_udp_t::set_param(const irs::string& /*a_name*/,
  const irs::string& /*a_value*/)
{

}

void irs::hardflow::simple_udp_t::open_udp(irs_u16 a_local_port, 
  irs_u16 a_dest_port, const irs_u8* a_dest_ip)
{
  m_dest_ip[0] = a_dest_ip[0];
  m_dest_ip[1] = a_dest_ip[1];
  m_dest_ip[2] = a_dest_ip[2];
  m_dest_ip[3] = a_dest_ip[3];

  m_dest_ip_def[0] = a_dest_ip[0];
  m_dest_ip_def[1] = a_dest_ip[1];
  m_dest_ip_def[2] = a_dest_ip[2];
  m_dest_ip_def[3] = a_dest_ip[3];

  m_dest_port = a_dest_port;
  m_dest_port_def = a_dest_port;
  m_local_port = a_local_port;
  m_udp_open = true;
}

void irs::hardflow::simple_udp_t::close_udp()
{
  m_udp_open = false;
}

irs_u8 irs::hardflow::simple_udp_t::write_udp_begin()
{
  return m_user_tx^0x1;
}

void irs::hardflow::simple_udp_t::write_udp_end(irs_u8* a_dest_ip, 
  irs_u16* a_dest_port, irs_u16 a_size)
{
  if (a_dest_ip) {
    m_dest_ip[0] = a_dest_ip[0];
    m_dest_ip[1] = a_dest_ip[1];
    m_dest_ip[2] = a_dest_ip[2];
    m_dest_ip[3] = a_dest_ip[3];
  } else {
    m_dest_ip[0] = m_dest_ip_def[0];
    m_dest_ip[1] = m_dest_ip_def[1];
    m_dest_ip[2] = m_dest_ip_def[2];
    m_dest_ip[3] = m_dest_ip_def[3];
  }
  if (a_dest_port) {
    m_dest_port = *a_dest_port;
  } else {
    m_dest_port = m_dest_port_def;
  }
  if (a_size >= 18) {
    m_tx_user_length_hard = a_size;
  } else {
    m_tx_user_length_hard = 18;
  }
  m_tx_user_length_udp = a_size;
  m_user_tx = true;
}

irs_u16 irs::hardflow::simple_udp_t::read_udp_begin(irs_u8* a_dest_ip,
  irs_u16* a_dest_port)
{
  irs_u16 data = 0;
  if (m_user_rx == true) {
    data = m_rx_user_length;
    if (a_dest_ip) {
      IRS_LODWORD(*a_dest_ip) = IRS_LODWORD(mp_rx_buf[0x1A]);
    }
    if (a_dest_port) {
      IRS_HIBYTE(*a_dest_port) = mp_rx_buf[0x22];
      IRS_LOBYTE(*a_dest_port) = mp_rx_buf[0x23];
    }
  }
  return data;
}

void irs::hardflow::simple_udp_t::read_udp_end()
{
  m_user_rx = false;
  m_rx_hard = false;
}

irs_u8 irs::hardflow::simple_udp_t::cash(irs_u8* a_dest_ip)
{
  irs_u8 result = 0;
  
  #ifdef IRS_UDP_ARP_CASH_EXT
  const irs::udp::ip_t& ip = irs::udp::ip_from_data(a_dest_ip);
  if (ip != irs::udp::broadcast_ip) {
    if (arp_cash.ip_to_mac(ip, cash_mac)) {
      cash_ip = ip;
      result = 1;
    }
  } else {
    cash_ip = irs::broadcast_ip();
    cash_mac = irs::broadcast_mac();
    result = 1;
  }
  #else //IRS_UDP_ARP_CASH_EXT
  const irs_u32 broadcast_ip = 0xFFFFFFFF;
  if (IRS_LODWORD(a_dest_ip[0]) == broadcast_ip) {
    *(irs_u32*)(&m_arp_cash[0]) = 0xFFFFFFFF;
    m_arp_cash[4]=0xff;
    m_arp_cash[5]=0xff;
    m_arp_cash[6]=0xff;
    m_arp_cash[7]=0xff;
    m_arp_cash[8]=0xff;
    m_arp_cash[9]=0xff;
    result = 1;
  } else { 
    if (IRS_LODWORD(m_arp_cash[0]) != 0)
    if (IRS_LODWORD(a_dest_ip[0]) == IRS_LODWORD(m_arp_cash[0])) {
      result = 1;
    }
  }
  #endif //IRS_UDP_ARP_CASH_EXT
  
  return result;
}

irs_u16 irs::hardflow::simple_udp_t::ip_checksum(irs_u16 a_cs, irs_u8 a_dat,
  irs_u16 a_count)
{
	irs_u8 b = a_dat;
	irs_u8 cs_l;
	irs_u8 cs_h;
	
	cs_h = IRS_HIBYTE(a_cs);
	cs_l = IRS_LOBYTE(a_cs);

	if( IRS_LOBYTE(a_count) & 0x01 ) {
		//* We are processing LSB	
		if( (cs_l = cs_l + b) < b ) {
			if( ++cs_h == 0 ) {
				cs_l ++;
      }
		}
	} else {
		//* We are processing MSB	
		if( (cs_h = cs_h + b) < b )	{
			if( ++cs_l == 0 ) {
				cs_h ++;
      }
		}
	}

  IRS_HIBYTE(a_cs) = cs_h;
	IRS_LOBYTE(a_cs) = cs_l;

	return a_cs;
}

irs_u16 irs::hardflow::simple_udp_t::cheksum(irs_u16 a_count, irs_u8* a_addr)
{
  irs_u16 ip_cs = 0;

  for (irs_u16 cs_cnt = 0; cs_cnt < a_count; cs_cnt++) {
    ip_cs = ip_checksum(ip_cs, a_addr[cs_cnt], cs_cnt);
  }

	ip_cs = 0xffff^ip_cs;

  return ip_cs;
}

irs_u16 irs::hardflow::simple_udp_t::cheksumUDP(irs_u16 a_count, irs_u8* a_addr)
{
  #ifndef NOP
  // Длина псевдозаголовка
  #define ph_count 12
  // Массив для псевдозаголовка
  irs_u8 pseudo_header[ph_count];

  /// Формирование псевдозаголовка
  pseudo_header[0] = mp_tx_buf[0x1a];
  pseudo_header[1] = mp_tx_buf[0x1b];
  pseudo_header[2] = mp_tx_buf[0x1c];
  pseudo_header[3] = mp_tx_buf[0x1d];

  pseudo_header[4] = mp_tx_buf[0x1e];
  pseudo_header[5] = mp_tx_buf[0x1f];
  pseudo_header[6] = mp_tx_buf[0x20];
  pseudo_header[7] = mp_tx_buf[0x21];

  pseudo_header[8] = 0;
  pseudo_header[9] = 0x11;

  pseudo_header[10] = mp_tx_buf[0x26];
  pseudo_header[11] = mp_tx_buf[0x27];

  // Вычисление контрольной суммы
  irs_u16 ip_cs = 0;

  for (irs_u16 cs_cnt = 0; cs_cnt < ph_count; cs_cnt++) {
    ip_cs = ip_checksum(ip_cs, pseudo_header[cs_cnt], cs_cnt);
  }
  for (irs_u16 cs_cnt = 0; cs_cnt < a_count; cs_cnt++) {
    ip_cs = ip_checksum(ip_cs, a_addr[cs_cnt], cs_cnt);
  }

  ip_cs = 0xffff^ip_cs;

  return ip_cs;
    
  #else //NOP
  
  irs_u8 pseudo_header[12];
  irs_u8 count_head = 12;
  irs_u8 *adr_h;
  irs_u32 sum = 0;
  irs_u16 byte;
  ///psevdo zagolovok
  pseudo_header[0] = m_tx_buf[0x1a];
  pseudo_header[1] = m_tx_buf[0x1b];
  pseudo_header[2] = m_tx_buf[0x1c];
  pseudo_header[3] = m_tx_buf[0x1d];

  pseudo_header[4] = m_tx_buf[0x1e];
  pseudo_header[5] = m_tx_buf[0x1f];
  pseudo_header[6] = m_tx_buf[0x20];
  pseudo_header[7] = m_tx_buf[0x21];

  pseudo_header[8] = 0;
  pseudo_header[9] = 0x11;

  pseudo_header[10] = m_tx_buf[0x26];
  pseudo_header[11] = m_tx_buf[0x27];
  adr_h = &pseudo_header[0];
  while (count_head > 1) {
    byte = (((irs_u16)*adr_h) << 8) + *(adr_h + 1);
    sum += byte;
    adr_h += 2;
    count_head -= 2;
  }

  while (a_count > 1) {
    byte = (((irs_u16)*a_addr) << 8) + *(a_addr + 1);
    sum += byte;
    a_addr += 2;
    a_count -= 2;
  }

  if(a_count > 0) {
    sum += ((irs_u16)*a_addr) << 8;
  }
  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  return((0xFFFF ^ sum));
  
  #endif //NOP
}

void irs::hardflow::simple_udp_t::arp_ping(irs_u8* a_dest_ip)
{
  //Широковещательный запрос
  m_arp_buf[0x0] = 0xff;
  m_arp_buf[0x1] = 0xff;
  m_arp_buf[0x2] = 0xff;
  m_arp_buf[0x3] = 0xff;
  m_arp_buf[0x4] = 0xff;
  m_arp_buf[0x5] = 0xff;

  //Локальный МАС-адресс
  m_arp_buf[0x6] = m_mac[0x0];
  m_arp_buf[0x7] = m_mac[0x1];
  m_arp_buf[0x8] = m_mac[0x2];
  m_arp_buf[0x9] = m_mac[0x3];
  m_arp_buf[0xa] = m_mac[0x4];
  m_arp_buf[0xb] = m_mac[0x5];

  //EtherType пакетов запроса 0x0806 
  m_arp_buf[0xc] = 0x8;
  m_arp_buf[0xd] = 0x6;

  //Hardware type
  m_arp_buf[0xe] = 0x0;
  m_arp_buf[0xf] = 0x1;
  
  //Protocol type
  m_arp_buf[0x10] = 0x8;
  m_arp_buf[0x11] = 0x0;
  
  //Hardware lenght
  m_arp_buf[0x12] = 0x6;
  //Protocol lenght
  m_arp_buf[0x13] = 0x4;
  //Код операции
  m_arp_buf[0x15] = 0x1;

  //Физический адрес отправителя
  m_arp_buf[0x16] = m_mac[0x0];
  m_arp_buf[0x17] = m_mac[0x1];
  m_arp_buf[0x18] = m_mac[0x2];
  m_arp_buf[0x19] = m_mac[0x3];
  m_arp_buf[0x1a] = m_mac[0x4];
  m_arp_buf[0x1b] = m_mac[0x5];

  //Логический адрес отправителя
  m_arp_buf[0x1c] = m_ip[0x0];
  m_arp_buf[0x1d] = m_ip[0x1];
  m_arp_buf[0x1e] = m_ip[0x2];
  m_arp_buf[0x1f] = m_ip[0x3];

  //Физический адрес получателя
  m_arp_buf[0x20] = m_arp_buf[0x0];
  m_arp_buf[0x21] = m_arp_buf[0x0];
  m_arp_buf[0x22] = m_arp_buf[0x0];
  m_arp_buf[0x23] = m_arp_buf[0x0];
  m_arp_buf[0x24] = m_arp_buf[0x0];
  m_arp_buf[0x25] = m_arp_buf[0x0];

  //Логический адрес получателя
  m_arp_buf[0x26] = a_dest_ip[0x0];
  m_arp_buf[0x27] = a_dest_ip[0x1];
  m_arp_buf[0x28] = a_dest_ip[0x2];
  m_arp_buf[0x29] = a_dest_ip[0x3];

  m_send_arp = true;
}

void irs::hardflow::simple_udp_t::arp_packet(void)
{
  //формируем пакет
  m_arp_buf[0x0] = m_arp_buf[0x6];
  m_arp_buf[0x1] = m_arp_buf[0x7];
  m_arp_buf[0x2] = m_arp_buf[0x8];
  m_arp_buf[0x3] = m_arp_buf[0x9];
  m_arp_buf[0x4] = m_arp_buf[0xa];
  m_arp_buf[0x5] = m_arp_buf[0xb];

  m_arp_buf[0x6] = m_mac[0x0];
  m_arp_buf[0x7] = m_mac[0x1];
  m_arp_buf[0x8] = m_mac[0x2];
  m_arp_buf[0x9] = m_mac[0x3];
  m_arp_buf[0xa] = m_mac[0x4];
  m_arp_buf[0xb] = m_mac[0x5];

  m_arp_buf[0x15] = 0x2;

  m_arp_buf[0x20] = m_arp_buf[0x16];
  m_arp_buf[0x21] = m_arp_buf[0x17];
  m_arp_buf[0x22] = m_arp_buf[0x18];
  m_arp_buf[0x23] = m_arp_buf[0x19];
  m_arp_buf[0x24] = m_arp_buf[0x1a];
  m_arp_buf[0x25] = m_arp_buf[0x1b];

  m_arp_buf[0x26] = m_arp_buf[0x1c];
  m_arp_buf[0x27] = m_arp_buf[0x1d];
  m_arp_buf[0x28] = m_arp_buf[0x1e];
  m_arp_buf[0x29] = m_arp_buf[0x1f];

  m_arp_buf[0x16] = m_mac[0x0];
  m_arp_buf[0x17] = m_mac[0x1];
  m_arp_buf[0x18] = m_mac[0x2];
  m_arp_buf[0x19] = m_mac[0x3];
  m_arp_buf[0x1a] = m_mac[0x4];
  m_arp_buf[0x1b] = m_mac[0x5];

  m_arp_buf[0x1c] = m_ip[0x0];
  m_arp_buf[0x1d] = m_ip[0x1];
  m_arp_buf[0x1e] = m_ip[0x2];
  m_arp_buf[0x1f] = m_ip[0x3];

  m_send_arp = true;
}

void irs::hardflow::simple_udp_t::arp_cash(void)
{
  #ifdef IRS_UDP_ARP_CASH_EXT
  irs::ip_t& arp_ip = ip_from_data(m_arp_buf[0x1c]);
  irs::mac_t& arp_mac = mac_from_data(m_arp_buf[0x16]);
  arp_cash.add(arp_ip, arp_mac);
  #else //IRS_UDP_ARP_CASH_EXT
  m_arp_cash[4] = m_arp_buf[0x16];
  m_arp_cash[5] = m_arp_buf[0x17];
  m_arp_cash[6] = m_arp_buf[0x18];
  m_arp_cash[7] = m_arp_buf[0x19];
  m_arp_cash[8] = m_arp_buf[0x1a];
  m_arp_cash[9] = m_arp_buf[0x1b];

  m_arp_cash[0] = m_arp_buf[0x1c];
  m_arp_cash[1] = m_arp_buf[0x1d];
  m_arp_cash[2] = m_arp_buf[0x1e];
  m_arp_cash[3] = m_arp_buf[0x1f];
  #endif //IRS_UDP_ARP_CASH_EXT
  m_rx_arp = false;
}

void irs::hardflow::simple_udp_t::arp()
{
  if((mp_rx_buf[0x26] == m_ip[0]) && (mp_rx_buf[0x27] == m_ip[1]) &&
    (mp_rx_buf[0x28] == m_ip[2]) && (mp_rx_buf[0x29] == m_ip[3]))
  {
    if (m_rx_arp == false) {
      m_rx_arp = true;
      for (irs_u8 i = 0; i < ARPBUF_SIZE; i++) {
        m_arp_buf[i] = mp_rx_buf[i];
      }
      m_rx_hard = false;
      if (m_arp_buf[0x15] == 2) {
        arp_cash();
      }
      if (m_arp_buf[0x15] == 1) {
        arp_packet();
      }
    }
  } else {
    m_rx_hard = false;
  }
}

void irs::hardflow::simple_udp_t::send_arp(void)
{
  mp_ethernet_rtl->send_packet(ARPBUF_SENDSIZE, m_arp_buf.data());
  m_send_arp = false;
  m_rx_arp = false;
}

void irs::hardflow::simple_udp_t::icmp_packet()
{
  // MAC-приемника
  m_icmp_buf[0x0] = m_icmp_buf[0x6];
  m_icmp_buf[0x1] = m_icmp_buf[0x7];
  m_icmp_buf[0x2] = m_icmp_buf[0x8];
  m_icmp_buf[0x3] = m_icmp_buf[0x9];
  m_icmp_buf[0x4] = m_icmp_buf[0xa];
  m_icmp_buf[0x5] = m_icmp_buf[0xb];

  // MAC-передатчика
  m_icmp_buf[0x6] = m_mac[0x0];
  m_icmp_buf[0x7] = m_mac[0x1];
  m_icmp_buf[0x8] = m_mac[0x2];
  m_icmp_buf[0x9] = m_mac[0x3];
  m_icmp_buf[0xa] = m_mac[0x4];
  m_icmp_buf[0xb] = m_mac[0x5];

  // Уменьшаем TTL на 1
  // Обычно при ответе на ping стек TCP/IP не уменьшает TTL
  //icmpbuf[0x16] -= 1;

  // Обнуляем контрольную сумму IP
  m_icmp_buf[0x18] = 0x00;
  m_icmp_buf[0x19] = 0x00;

  // IP-адрес приемника
  m_icmp_buf[0x1E] = m_icmp_buf[0x1a];
  m_icmp_buf[0x1f] = m_icmp_buf[0x1b];
  m_icmp_buf[0x20] = m_icmp_buf[0x1c];
  m_icmp_buf[0x21] = m_icmp_buf[0x1d];

  // IP-адрес источника
  m_icmp_buf[0x1a] = m_ip[0x0];
  m_icmp_buf[0x1b] = m_ip[0x1];
  m_icmp_buf[0x1c] = m_ip[0x2];
  m_icmp_buf[0x1d] = m_ip[0x3];

  // Контрольная сумма IP
  irs_u16 chksum_ip = cheksum(20, &m_icmp_buf[0xe]);
  m_icmp_buf[0x18] = IRS_HIBYTE(chksum_ip);
  m_icmp_buf[0x19] = IRS_LOBYTE(chksum_ip);

  // Тип ICMP - эхо-ответ
  m_icmp_buf[0x22] = 0x00;
  // Код ICMP
  m_icmp_buf[0x23] = 0x00;

  // Обнуляем контрольную сумму ICMP
  m_icmp_buf[0x24] = 0x00;
  m_icmp_buf[0x25] = 0x00;

  // Контрольная сумма ICMP
  irs_u16 chksum_icmp = cheksum(m_rx_len_icmp - 0x22, &m_icmp_buf[0x22]);
  m_icmp_buf[0x24] = IRS_HIBYTE(chksum_icmp);
  m_icmp_buf[0x25] = IRS_LOBYTE(chksum_icmp);

  m_send_icmp = true;
}

void irs::hardflow::simple_udp_t::send_icmp()
{
  mp_ethernet_rtl->send_packet(m_rx_len_icmp, m_icmp_buf.data());
  #ifdef DBGMODE
  PORTB ^= (1 << PORTB2);
  #endif //DBGMODE
  m_send_icmp = false;
  m_rx_icmp = false;
}

void irs::hardflow::simple_udp_t::icmp()
{
  if ((mp_rx_buf[0x22] == 8)&&(mp_rx_buf[0x23] == 0))
  {
    if (m_rx_icmp == false) {
      m_rx_len_icmp = m_rx_len_hard - 4;
      if (m_rx_len_icmp <= ICMPBUF_SIZE) {
        m_rx_icmp = true;
        for (irs_i16 i = 0; i < m_rx_len_icmp; i++) {
          m_icmp_buf[i] = mp_rx_buf[i];
        }
        icmp_packet();
      }
    }
  }
  m_rx_hard = false;
}

void irs::hardflow::simple_udp_t::server_udp()
{
  if((mp_rx_buf[0x24] == IRS_HIBYTE(m_local_port)) &&
      (mp_rx_buf[0x25] == IRS_LOBYTE(m_local_port)))
  {
    m_user_rx = true;
    irs_u16 udp_size = 0;
    IRS_LOBYTE(udp_size) = mp_rx_buf[0x27];
    IRS_HIBYTE(udp_size) = mp_rx_buf[0x26];
    m_rx_user_length = udp_size - 8;
    
    #ifdef IRS_UDP_ARP_CASH_EXT
    irs::ip_t& ip = ip_from_data(m_rx_buf[0x1A]);
    irs::mac_t& mac = mac_from_data(m_rx_buf[0x06]);
    arp_cash.add(ip, mac);
    #endif //IRS_UDP_ARP_CASH_EXT
  } else {
    m_rx_hard = false;
  }
}

void irs::hardflow::simple_udp_t::udp_packet()
{
  mp_tx_buf[0x0] = m_arp_cash[0x4];
  mp_tx_buf[0x1] = m_arp_cash[0x5];
  mp_tx_buf[0x2] = m_arp_cash[0x6];
  mp_tx_buf[0x3] = m_arp_cash[0x7];
  mp_tx_buf[0x4] = m_arp_cash[0x8];
  mp_tx_buf[0x5] = m_arp_cash[0x9];

  mp_tx_buf[0x6] = m_mac[0x0];
  mp_tx_buf[0x7] = m_mac[0x1];
  mp_tx_buf[0x8] = m_mac[0x2];
  mp_tx_buf[0x9] = m_mac[0x3];
  mp_tx_buf[0xa] = m_mac[0x4];
  mp_tx_buf[0xb] = m_mac[0x5];

  //type
  mp_tx_buf[0xc] = 0x8;
  mp_tx_buf[0xd] = 0x0;
  //ver//tos
  mp_tx_buf[0xe] = 0x45;
  mp_tx_buf[0xf] = 0x10;
  //alllenth
  irs_u16 ip_length = m_tx_user_length_udp + 28;
  mp_tx_buf[0x11] = IRS_LOBYTE(ip_length);
  mp_tx_buf[0x10] = IRS_HIBYTE(ip_length);
  //рисуем идентификатор
  m_identif++;
  mp_tx_buf[0x13] = IRS_LOBYTE(m_identif);
  mp_tx_buf[0x12] = IRS_HIBYTE(m_identif);
  //fragment
  mp_tx_buf[0x14] = 0;
  mp_tx_buf[0x15] = 0;
  //рисуем TTL
  mp_tx_buf[0x16] = 128;
  //protocol
  mp_tx_buf[0x17] = 0x11;
  //obnulenie cheksum
  mp_tx_buf[0x18] = 0;
  mp_tx_buf[0x19] = 0;

  //ip dest
  mp_tx_buf[0x1e] = m_arp_cash[0x0];
  mp_tx_buf[0x1f] = m_arp_cash[0x1];
  mp_tx_buf[0x20] = m_arp_cash[0x2];
  mp_tx_buf[0x21] = m_arp_cash[0x3];
  //ip sourse
  mp_tx_buf[0x1a] = m_ip[0x0];
  mp_tx_buf[0x1b] = m_ip[0x1];
  mp_tx_buf[0x1c] = m_ip[0x2];
  mp_tx_buf[0x1d] = m_ip[0x3];
  //clear checsum

  //checksum
  irs_u16 chksum_ip = cheksum(20, &mp_tx_buf[0xe]);
  mp_tx_buf[0x18] = IRS_HIBYTE(chksum_ip);
  mp_tx_buf[0x19] = IRS_LOBYTE(chksum_ip);

  //ports udp
  mp_tx_buf[0x24] = IRS_HIBYTE(m_dest_port);
  mp_tx_buf[0x25] = IRS_LOBYTE(m_dest_port);

  mp_tx_buf[0x22] = IRS_HIBYTE(m_local_port);
  mp_tx_buf[0x23] = IRS_LOBYTE(m_local_port);
  //udp lenyh
  irs_u16 udp_length = m_tx_user_length_udp + 8;
  mp_tx_buf[0x27] = IRS_LOBYTE(udp_length);
  mp_tx_buf[0x26] = IRS_HIBYTE(udp_length);
  //checsum
  mp_tx_buf[0x28]=0;
  mp_tx_buf[0x29]=0;

  //checsum
  irs_u16 chksum_udp = cheksumUDP(m_tx_user_length_udp + 8, &mp_tx_buf[0x22]);
  mp_tx_buf[0x28] = IRS_HIBYTE(chksum_udp);
  mp_tx_buf[0x29] = IRS_LOBYTE(chksum_udp);
  m_send_udp = true;
}

void irs::hardflow::simple_udp_t::send_udp()
{
  mp_ethernet_rtl->send_packet(m_tx_user_length_hard + HEADERS_SIZE, 
    &mp_tx_buf[0]);
  m_send_udp = false;
  m_tx_udp = false;
  m_user_tx = false;
}

void irs::hardflow::simple_udp_t::client_udp()
{
  if (m_user_tx == true) {
    static bool udp_wait_arp = false;
    static counter_t to_udp_wait_arp;
    #define t_udp_wait_arp TIME_TO_CNT(1, 1)

    if (cash(m_dest_ip.data()) == 1) {
      if (m_tx_udp == false) {
        udp_packet();
        m_tx_udp = true;
        udp_wait_arp = false;
      }
    } else {
      if (udp_wait_arp) {
        if (test_to_cnt(to_udp_wait_arp)) {
          udp_wait_arp = false;
          m_send_udp = false;
          m_tx_udp = false;
          m_user_tx = false;
        }
      } else {
        if (m_rx_arp == false) {
          m_rx_arp = true;
          arp_ping(m_dest_ip.data());
          udp_wait_arp = true;
          set_to_cnt(to_udp_wait_arp, t_udp_wait_arp);
        }
      }
    }
  }
}

void irs::hardflow::simple_udp_t::udp()
{
  if (m_udp_open == true) {
    server_udp();
  } else {
    m_rx_hard = false;
  }
}

void irs::hardflow::simple_udp_t::ip(void)
{
  if((mp_rx_buf[0x1e] == m_ip[0]) && (mp_rx_buf[0x1f] == m_ip[1]) && 
    (mp_rx_buf[0x20] == m_ip[2]) && (mp_rx_buf[0x21] == m_ip[3]))
  {
      if(mp_rx_buf[0x17] == 0x01) {
        icmp(); 
      }
      if(mp_rx_buf[0x17] == 0x11) {
        udp();
      }
  } else {
    m_rx_hard = false;
  }  
}

void irs::hardflow::simple_udp_t::tick()
{
  if (m_rx_hard == true)
  {
    if ((mp_rx_buf[0xc] == 0x8)&&(mp_rx_buf[0xd] == 0)) ip();
    if ((mp_rx_buf[0xc] == 0x8)&&(mp_rx_buf[0xd] == 6)) arp();
    m_rx_hard = false;
  }

  if (m_udp_open == true) {
    client_udp();
  }

  if (m_send_arp) {
    if (m_tx_hard == false) {
      send_arp();
    }
  }
  
  if (m_send_icmp) {
    if (m_tx_hard == false) {
      send_icmp();
    }
  }
  
  if (m_send_udp) {
    if (m_tx_hard == false) {
      send_udp();
    }
  }
}
