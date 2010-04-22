// UDP/IP-стек Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Дата: 14.04.2010
// Ранняя дата: 30.05.2008

#include <irsdefs.h>

#include <timer.h>
#include <irscpp.h>
#include <mxdata.h>
//#include <irsavrutil.h>
#include <UDP_stack.h>
#include <irserror.h>

#include <irsfinal.h>

irs_u8 *user_rx_buf = 0;
irs_u8 *user_tx_buf = 0;

namespace irs {

namespace udp {
  
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

const mac_t zero_mac = {{0, 0, 0, 0, 0, 0}};
const mac_t broadcast_mac = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

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

const ip_t zero_ip = {{0, 0, 0, 0}};
const ip_t broadcast_ip = {{0xFF, 0xFF, 0xFF, 0xFF}};

// Переобразование данных в mac_t
#define IRS_UDP_MAC(data) (*(irs::udp::mac_t*)(&(data)))
template <class T>
inline mac_t& mac_from_data(T& data);
template <class T>
inline mac_t& mac_from_data(T* data);

// Преобразование данных в ip_t
#define IRS_UDP_IP(data) (*(irs::udp::ip_t*)(&(data)))
template <class T>
inline ip_t& ip_from_data(T& data);
template <class T>
inline ip_t& ip_from_data(T* data);

// ARP-кэш
class arp_cash_t
{
public:
  static const irs_uarc m_def_size = 3;
  
  arp_cash_t(irs_uarc a_size = m_def_size);
  bool ip_to_mac(const ip_t& a_ip, mac_t& a_mac);
  void add(const ip_t& a_ip, const mac_t& a_mac);
  inline irs_uarc size() const;
  void resize(irs_uarc a_size);
private:
  struct cash_item_t {
    ip_t ip;
    mac_t mac;
    bool valid;
    
    cash_item_t(ip_t a_ip = zero_ip, mac_t a_mac = zero_mac, 
      bool a_valid = false):
      ip(a_ip), mac(a_mac), valid(a_valid)
    {
    }
  };
  typedef vector<cash_item_t>::iterator cash_item_it_t;
  typedef vector<cash_item_t>::const_iterator cash_item_const_it_t;
  
  vector<cash_item_t> m_cash;
  irs_uarc m_pos;
  loop_timer_t m_reset_timer;
};
  
} //namespace udp
  
} //namespace irs

// Переобразование данных в mac_t
template <class T>
inline irs::udp::mac_t& irs::udp::mac_from_data(T& data)
{
  return IRS_UDP_MAC(data);
}
template <class T>
inline irs::udp::mac_t& irs::udp::mac_from_data(T* data)
{
  return IRS_UDP_MAC(*data);
}

// Преобразование данных в ip_t
template <class T>
inline irs::udp::ip_t& irs::udp::ip_from_data(T& data)
{
  return IRS_UDP_IP(data);
}
template <class T>
inline irs::udp::ip_t& irs::udp::ip_from_data(T* data)
{
  return IRS_UDP_IP(*data);
}

// ARP-кэш
irs::udp::arp_cash_t::arp_cash_t(irs_uarc a_size):
  m_cash(a_size),
  m_pos(0),
  m_reset_timer(60, 1)
{
}
bool irs::udp::arp_cash_t::ip_to_mac(const ip_t& a_ip, mac_t& a_mac)
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
void irs::udp::arp_cash_t::add(const ip_t& a_ip, const mac_t& a_mac)
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
inline irs_uarc irs::udp::arp_cash_t::size() const
{
  return (irs_uarc)m_cash.size();
}
void irs::udp::arp_cash_t::resize(irs_uarc a_size)
{
  if (a_size < 1) a_size = 1;
  m_cash.resize(a_size);
  if (m_pos > m_cash.size()) m_pos = 0;
}
  
namespace {

class udp_t
{
public:
  udp_t();
  ~udp_t();
  void init(const irs_u8 *mymac, const irs_u8 *myip,
    irs_avr_port_t a_data_port, irs_avr_port_t a_address_port,
    irs_size_t bufs_size = 250);
  void open(irs_u16 localport_, irs_u16 destport_, const irs_u8 *ip_dest_);
  void close();
  irs_u8 write_begin();
  void write_end(irs_u8 *dest_ip_a, irs_u16 *dest_port_a, irs_u16 size);
  irs_u16 read_begin(irs_u8 *dest_ip_a, irs_u16 *dest_port_a);
  void read_end();
  irs_size_t udp_buf_size();
  void tick();
  
private:
  enum {
    prot_icmp = 0x01,
    prot_tcp = 0x06,
    prot_udp = 0x11,
    ip_size = 4,
    mac_size = 6,
    ARPBUF_SIZE = 42,
    ARPBUF_SENDSIZE = 60,
    ICMPBUF_SIZE = 100
  };
  
  irs_u8 m_ip[ip_size];
  irs_u8 m_mac[6];
  irs::raw_data_t<irs_u8> m_arpbuf_data;
  irs::raw_data_t<irs_u8> m_icmpbuf_data;
  irs_u8* m_arp_buf;
  irs_u8* m_icmp_buf;
  irs_size_t m_user_buf_size;
  irs_u8 m_arpcash[10];
  irs_u16 m_rxlenicmp;
  irs_u8 m_dest_ip[ip_size];
  irs_u8 m_dest_ip_def[ip_size];
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
  bool m_udp_is_open;
  bool m_rx_arp;
  bool m_send_arp;
  bool m_rx_icmp;
  bool m_send_icmp;
  bool m_send_udp;
  irs::udp::arp_cash_t m_arp_cash;
  irs::udp::ip_t& m_cash_ip;
  irs::udp::mac_t& m_cash_mac;
  
  irs_u8 cash(irs_u8 a_dest_ip[4]);
  irs_u16 ip_checksum (irs_u16 cs, irs_u8 dat, irs_u16 count);
  irs_u16 cheksum(irs_u16 count, irs_u8 *adr);
  irs_u16 cheksum_udp(irs_u16 count, irs_u8 *adr);
  void arp_ping(irs_u8 dest_ip_[4]);
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

udp_t::udp_t():
  m_arpbuf_data(0),
  m_icmpbuf_data(0),
  m_arp_buf(IRS_NULL),
  m_icmp_buf(IRS_NULL),
  m_user_buf_size(0),
  m_rxlenicmp(0),
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
  m_udp_is_open(false),
  m_rx_arp(false),
  m_send_arp(false),
  m_rx_icmp(false),
  m_send_icmp(false),
  m_send_udp(false),
  m_arp_cash(),
  m_cash_ip(irs::udp::ip_from_data(m_arpcash[0])),
  m_cash_mac(irs::udp::mac_from_data(m_arpcash[4]))
{
  memset(m_ip, 0, ip_size);
  memset(m_mac, 0, mac_size);
  memset(m_dest_ip, 0, ip_size);
  memset(m_dest_ip_def, 0, ip_size);
}

void udp_t::init(const irs_u8 *mymac,const irs_u8 *myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port,
  irs_size_t bufs_size)
{
  rtl_set_ports(a_data_port, a_address_port);

  initrtl(mymac, bufs_size);
  m_arpbuf_data.resize(ARPBUF_SENDSIZE);
  m_icmpbuf_data.resize(ICMPBUF_SIZE);
  m_arp_buf = m_arpbuf_data.data();
  m_icmp_buf = m_icmpbuf_data.data();
  m_user_buf_size = tx_buf_size() - HEADERS_SIZE;

  m_ip[0]=myip[0];
  m_ip[1]=myip[1];
  m_ip[2]=myip[2];
  m_ip[3]=myip[3];

  m_mac[0]=mymac[0];
  m_mac[1]=mymac[1];
  m_mac[2]=mymac[2];
  m_mac[3]=mymac[3];
  m_mac[4]=mymac[4];
  m_mac[5]=mymac[5];
  
  user_rx_buf=&rx_buf[HEADERS_SIZE];
  user_tx_buf=&tx_buf[HEADERS_SIZE];
  
  IRS_LIB_ASSERT(m_arp_buf != IRS_NULL);
  IRS_LIB_ASSERT(m_icmp_buf != IRS_NULL);
}

udp_t::~udp_t()
{
}

irs_u8 udp_t::cash(irs_u8 a_dest_ip[4])
{ 
  irs_u8 result = 0;
  
  const irs::udp::ip_t& ip = irs::udp::ip_from_data(a_dest_ip);
  if (ip != irs::udp::broadcast_ip) {
    if (m_arp_cash.ip_to_mac(ip, m_cash_mac)) {
      m_cash_ip = ip;
      result = 1;
    }
  } else {
    m_cash_ip = irs::udp::broadcast_ip;
    m_cash_mac = irs::udp::broadcast_mac;
    result = 1;
  }
  
  return result;
}

irs_u16 udp_t::ip_checksum (irs_u16 cs, irs_u8 dat, irs_u16 count)
{
	irs_u8 b = dat;
	irs_u8 cs_l;
	irs_u8 cs_h;
	
	cs_h = IRS_HIBYTE(cs);
	cs_l = IRS_LOBYTE(cs);

	if( IRS_LOBYTE(count) & 0x01 ) {
		///* We are processing LSB	
		
		if( (cs_l = cs_l + b) < b ) {
			if( ++cs_h == 0 )
				cs_l++;
		}
		
	} else {
		///* We are processing MSB	
		
		if( (cs_h = cs_h + b) < b )	{
			if( ++cs_l == 0 )
				cs_h++;
		}
	}

  IRS_HIBYTE(cs) = cs_h;
	IRS_LOBYTE(cs) = cs_l;

	return cs;
}
irs_u16 udp_t::cheksum(irs_u16 count, irs_u8 *adr)
{
  irs_u16 ip_cs = 0;

  for (irs_u16 cs_cnt = 0; cs_cnt < count; cs_cnt++) {
    ip_cs = ip_checksum(ip_cs, adr[cs_cnt], cs_cnt);
  }

	ip_cs = 0xffff^ip_cs;

  return ip_cs;
}

irs_u16 udp_t::cheksum_udp(irs_u16 count, irs_u8 *adr)
{
  // Длина псевдозаголовка
  #define ph_count 12
  // Массив для псевдозаголовка
  irs_u8 pseudo_header[ph_count];

  /// Формирование псевдозаголовка
  pseudo_header[0]=tx_buf[0x1a];
  pseudo_header[1]=tx_buf[0x1b];
  pseudo_header[2]=tx_buf[0x1c];
  pseudo_header[3]=tx_buf[0x1d];

  pseudo_header[4]=tx_buf[0x1e];
  pseudo_header[5]=tx_buf[0x1f];
  pseudo_header[6]=tx_buf[0x20];
  pseudo_header[7]=tx_buf[0x21];

  pseudo_header[8]=0;
  pseudo_header[9]=0x11;

  pseudo_header[10]=tx_buf[0x26];
  pseudo_header[11]=tx_buf[0x27];

  // Вычисление контрольной суммы
  irs_u16 ip_cs = 0;

  for (irs_u16 cs_cnt = 0; cs_cnt < ph_count; cs_cnt++) {
    ip_cs = ip_checksum(ip_cs, pseudo_header[cs_cnt], cs_cnt);
  }
  for (irs_u16 cs_cnt = 0; cs_cnt < count; cs_cnt++) {
    ip_cs = ip_checksum(ip_cs, adr[cs_cnt], cs_cnt);
  }

	ip_cs = 0xffff^ip_cs;

  return ip_cs;
}

void udp_t::arp_ping(irs_u8 a_dest_ip[4])
{
  //Широковещательный запрос
  m_arp_buf[0x0]=0xff;
  m_arp_buf[0x1]=0xff;
  m_arp_buf[0x2]=0xff;
  m_arp_buf[0x3]=0xff;
  m_arp_buf[0x4]=0xff;
  m_arp_buf[0x5]=0xff;

  //Локальный МАС-адресс
  m_arp_buf[0x6]=m_mac[0x0];
  m_arp_buf[0x7]=m_mac[0x1];
  m_arp_buf[0x8]=m_mac[0x2];
  m_arp_buf[0x9]=m_mac[0x3];
  m_arp_buf[0xa]=m_mac[0x4];
  m_arp_buf[0xb]=m_mac[0x5];

  //EtherType пакетов запроса 0x0806 
  m_arp_buf[0xc]=0x8;
  m_arp_buf[0xd]=0x6;

  //Hardware type
  m_arp_buf[0xe]=0x0;
  m_arp_buf[0xf]=0x1;
  
  //Protocol type
  m_arp_buf[0x10]=0x8;
  m_arp_buf[0x11]=0x0;
  
  //Hardware lenght
  m_arp_buf[0x12]=0x6;
  //Protocol lenght
  m_arp_buf[0x13]=0x4;
  //Код операции
  m_arp_buf[0x15]=0x1;

  //Физический адрес отправителя
  m_arp_buf[0x16]=m_mac[0x0];
  m_arp_buf[0x17]=m_mac[0x1];
  m_arp_buf[0x18]=m_mac[0x2];
  m_arp_buf[0x19]=m_mac[0x3];
  m_arp_buf[0x1a]=m_mac[0x4];
  m_arp_buf[0x1b]=m_mac[0x5];

  //Логический адрес отправителя
  m_arp_buf[0x1c]=m_ip[0x0];
  m_arp_buf[0x1d]=m_ip[0x1];
  m_arp_buf[0x1e]=m_ip[0x2];
  m_arp_buf[0x1f]=m_ip[0x3];

  //Физический адрес получателя
  m_arp_buf[0x20]=m_arp_buf[0x0];
  m_arp_buf[0x21]=m_arp_buf[0x0];
  m_arp_buf[0x22]=m_arp_buf[0x0];
  m_arp_buf[0x23]=m_arp_buf[0x0];
  m_arp_buf[0x24]=m_arp_buf[0x0];
  m_arp_buf[0x25]=m_arp_buf[0x0];

  //Логический адрес получателя
  m_arp_buf[0x26]=a_dest_ip[0x0];
  m_arp_buf[0x27]=a_dest_ip[0x1];
  m_arp_buf[0x28]=a_dest_ip[0x2];
  m_arp_buf[0x29]=a_dest_ip[0x3];

  m_send_arp=1;
}

void udp_t::arp_packet(void)
{ //irs_u8 byte=0;

/////формируем пакет
  m_arp_buf[0x0]=m_arp_buf[0x6];
  m_arp_buf[0x1]=m_arp_buf[0x7];
  m_arp_buf[0x2]=m_arp_buf[0x8];
  m_arp_buf[0x3]=m_arp_buf[0x9];
  m_arp_buf[0x4]=m_arp_buf[0xa];
  m_arp_buf[0x5]=m_arp_buf[0xb];

  m_arp_buf[0x6]=m_mac[0x0];
  m_arp_buf[0x7]=m_mac[0x1];
  m_arp_buf[0x8]=m_mac[0x2];
  m_arp_buf[0x9]=m_mac[0x3];
  m_arp_buf[0xa]=m_mac[0x4];
  m_arp_buf[0xb]=m_mac[0x5];

  m_arp_buf[0x15]=0x2;

  m_arp_buf[0x20]=m_arp_buf[0x16];
  m_arp_buf[0x21]=m_arp_buf[0x17];
  m_arp_buf[0x22]=m_arp_buf[0x18];
  m_arp_buf[0x23]=m_arp_buf[0x19];
  m_arp_buf[0x24]=m_arp_buf[0x1a];
  m_arp_buf[0x25]=m_arp_buf[0x1b];

  m_arp_buf[0x26]=m_arp_buf[0x1c];
  m_arp_buf[0x27]=m_arp_buf[0x1d];
  m_arp_buf[0x28]=m_arp_buf[0x1e];
  m_arp_buf[0x29]=m_arp_buf[0x1f];

  m_arp_buf[0x16]=m_mac[0x0];
  m_arp_buf[0x17]=m_mac[0x1];
  m_arp_buf[0x18]=m_mac[0x2];
  m_arp_buf[0x19]=m_mac[0x3];
  m_arp_buf[0x1a]=m_mac[0x4];
  m_arp_buf[0x1b]=m_mac[0x5];

  m_arp_buf[0x1c]=m_ip[0x0];
  m_arp_buf[0x1d]=m_ip[0x1];
  m_arp_buf[0x1e]=m_ip[0x2];
  m_arp_buf[0x1f]=m_ip[0x3];

  m_send_arp=1;
}

void udp_t::arp_cash(void)
{
  irs::udp::ip_t& arp_ip = irs::udp::ip_from_data(m_arp_buf[0x1c]);
  irs::udp::mac_t& arp_mac = irs::udp::mac_from_data(m_arp_buf[0x16]);
  m_arp_cash.add(arp_ip, arp_mac);
  m_rx_arp = false;
}

void udp_t::arp()
{
  
#ifdef NOP
    static irs::blink_t blink_2(irs_avr_porte, 2);
    blink_2.set();
    //for(;;);
#endif //NOP
    
  //irs_u8 i=0;
  if((rx_buf[0x26]==m_ip[0])&&(rx_buf[0x27]==m_ip[1])&&
    (rx_buf[0x28]==m_ip[2])&&(rx_buf[0x29]==m_ip[3]))
  {
    if (!m_rx_arp) {
      m_rx_arp = true;
      for (irs_u8 i = 0; i < ARPBUF_SIZE; i++) m_arp_buf[i] = rx_buf[i];
      rx_hard = 0;
      if (m_arp_buf[0x15]==2) arp_cash();
      if (m_arp_buf[0x15]==1) arp_packet();
    }
  } else {
    rx_hard=0;
  }
}

void udp_t::send_arp(void)
{ 
  //sendpacketARP(&m_arp_buf[0]);
  sendpacket(ARPBUF_SENDSIZE, m_arp_buf);
  //sendpacket(60, m_arp_buf);
  m_send_arp = false;
  m_rx_arp = false;
}
///////////////////////////ICMP/////////////////////////////////////////////
void udp_t::icmp_packet()
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
  //m_icmp_buf[0x16] -= 1;

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
  irs_u16 chksum_icmp = cheksum(m_rxlenicmp - 0x22, &m_icmp_buf[0x22]);
  m_icmp_buf[0x24] = IRS_HIBYTE(chksum_icmp);
  m_icmp_buf[0x25] = IRS_LOBYTE(chksum_icmp);

  m_send_icmp = true;
}

void udp_t::send_icmp()
{
  //sendpacketICMP(m_rxlenicmp, &m_icmp_buf[0]);
  sendpacket(m_rxlenicmp, m_icmp_buf);
  #ifdef DBGMODE
  PORTB ^= (1 << PORTB2);
  #endif //DBGMODE
  m_send_icmp = false;
  m_rx_icmp = false;
}

void udp_t::icmp()
{
  if ((rx_buf[0x22] == 8)&&(rx_buf[0x23] == 0))
  {
    if (!m_rx_icmp) {
      m_rxlenicmp = rxlen_hard - 4;
      if (m_rxlenicmp <= ICMPBUF_SIZE) {
        m_rx_icmp = true;
        for (irs_i16 i = 0; i < m_rxlenicmp; i++) m_icmp_buf[i] = rx_buf[i];
        icmp_packet();
      }
    }
  }
  rx_hard = 0;
}

void udp_t::server_udp()
{
  //if ((rx_buf[0x24] == (m_local_port>>8))&&(rx_buf[0x25] == (m_local_port&0xff))) {
  if ((rx_buf[0x24] == IRS_HIBYTE(m_local_port)) &&
      (rx_buf[0x25] == IRS_LOBYTE(m_local_port))) {
    m_user_rx = true;
    irs_u16 udp_size = 0;
    IRS_LOBYTE(udp_size) = rx_buf[0x27];
    IRS_HIBYTE(udp_size) = rx_buf[0x26];
    m_rx_user_length = udp_size - 8;
    irs::udp::ip_t& ip = irs::udp::ip_from_data(rx_buf[0x1A]);
    irs::udp::mac_t& mac = irs::udp::mac_from_data(rx_buf[0x06]);
    m_arp_cash.add(ip, mac);
  } else {
    rx_hard = 0;
  }

}

void udp_t::udp_packet()
{
  tx_buf[0x0]=m_arpcash[0x4];
  tx_buf[0x1]=m_arpcash[0x5];
  tx_buf[0x2]=m_arpcash[0x6];
  tx_buf[0x3]=m_arpcash[0x7];
  tx_buf[0x4]=m_arpcash[0x8];
  tx_buf[0x5]=m_arpcash[0x9];

  tx_buf[0x6]=m_mac[0x0];
  tx_buf[0x7]=m_mac[0x1];
  tx_buf[0x8]=m_mac[0x2];
  tx_buf[0x9]=m_mac[0x3];
  tx_buf[0xa]=m_mac[0x4];
  tx_buf[0xb]=m_mac[0x5];

  ///type
  tx_buf[0xc]=0x8;
  tx_buf[0xd]=0x0;
  //ver//tos
  tx_buf[0xe]=0x45;
  tx_buf[0xf]=0x10; 
  ///alllenth
  irs_u16 ip_length = m_tx_user_length_udp + 28;
  tx_buf[0x11]=IRS_LOBYTE(ip_length);
  tx_buf[0x10]=IRS_HIBYTE(ip_length);
  //рисуем идентификатор
  m_identif++;
  tx_buf[0x13] = IRS_LOBYTE(m_identif);
  tx_buf[0x12] = IRS_HIBYTE(m_identif);
  //if (m_identif==65000) m_identif=0;
  //tx_buf[0x13]=m_identif&0xff;
  //tx_buf[0x12]=m_identif>>8;
  //fragment
  tx_buf[0x14]=0;
  tx_buf[0x15]=0;
  //рисуем TTL
  tx_buf[0x16]=128;
  ///protocol
  tx_buf[0x17]=0x11;
  //obnulenie cheksum
  tx_buf[0x18]=0;
  tx_buf[0x19]=0;

  ///ip dest
  tx_buf[0x1e]=m_arpcash[0x0];
  tx_buf[0x1f]=m_arpcash[0x1];
  tx_buf[0x20]=m_arpcash[0x2];
  tx_buf[0x21]=m_arpcash[0x3];
  //ip sourse
  tx_buf[0x1a]=m_ip[0x0];
  tx_buf[0x1b]=m_ip[0x1];
  tx_buf[0x1c]=m_ip[0x2];
  tx_buf[0x1d]=m_ip[0x3];
  //clear checsum

  ///checksum
  irs_u16 chksum_ip = cheksum(20, &tx_buf[0xe]);
  tx_buf[0x18] = IRS_HIBYTE(chksum_ip);
  tx_buf[0x19] = IRS_LOBYTE(chksum_ip);
  //sum=cheksum(20,&tx_buf[0xe]);
  //tx_buf[0x18]=(sum>>8);
  //tx_buf[0x19]=(sum&0xff);

  //ports udp
  tx_buf[0x24]=IRS_HIBYTE(m_dest_port);
  tx_buf[0x25]=IRS_LOBYTE(m_dest_port);

  tx_buf[0x22]=IRS_HIBYTE(m_local_port);
  tx_buf[0x23]=IRS_LOBYTE(m_local_port);
  //udp lenyh
  irs_u16 udp_length = m_tx_user_length_udp + 8;
  tx_buf[0x27] = IRS_LOBYTE(udp_length);
  tx_buf[0x26] = IRS_HIBYTE(udp_length);
  //tx_buf[0x27]=((m_tx_user_length_udp+8)&0xFF);
  //tx_buf[0x26]=((m_tx_user_length_udp+8)>>8);
  //checsum
  tx_buf[0x28]=0;
  tx_buf[0x29]=0;

  ///checsum
  irs_u16 chksum_udp = cheksum_udp(m_tx_user_length_udp + 8, &tx_buf[0x22]);
  tx_buf[0x28]=IRS_HIBYTE(chksum_udp);
  tx_buf[0x29]=IRS_LOBYTE(chksum_udp);
  //tx_buf[0x28]=(sum>>8);
  //tx_buf[0x29]=(sum&0xff);
  m_send_udp = true;
}

void udp_t::send_udp()
{
  sendpacket(m_tx_user_length_hard+HEADERS_SIZE,&tx_buf[0]);
  m_send_udp = false;
  m_tx_udp = false;
  m_user_tx = false;
}

void udp_t::client_udp()
{
  if (m_user_tx) {
    static irs_bool udp_wait_arp = irs_false;
    static counter_t to_udp_wait_arp;
    #define t_udp_wait_arp TIME_TO_CNT(1, 1)

    if (cash(m_dest_ip) == 1) {
      if (!m_tx_udp) {
        udp_packet();
        m_tx_udp = true;
        //sendUDP();
        udp_wait_arp = irs_false;
      }
    } else {
      if (udp_wait_arp) {
        if (test_to_cnt(to_udp_wait_arp)) {
          udp_wait_arp = irs_false;
          m_send_udp = false;
          m_tx_udp = false;
          m_user_tx = false;
        }
      } else {
        if (!m_rx_arp) {
          m_rx_arp = true;
          arp_ping(m_dest_ip);
          udp_wait_arp = irs_true;
          set_to_cnt(to_udp_wait_arp, t_udp_wait_arp);
        }
      }
    }
  }
}

// Открытие сокета
void udp_t::open(irs_u16 a_local_port, irs_u16 a_dest_port,
  const irs_u8 *ap_dest_ip)
{
  m_dest_ip[0] = ap_dest_ip[0];
  m_dest_ip[1] = ap_dest_ip[1];
  m_dest_ip[2] = ap_dest_ip[2];
  m_dest_ip[3] = ap_dest_ip[3];

  m_dest_ip_def[0] = ap_dest_ip[0];
  m_dest_ip_def[1] = ap_dest_ip[1];
  m_dest_ip_def[2] = ap_dest_ip[2];
  m_dest_ip_def[3] = ap_dest_ip[3];

  m_dest_port = a_dest_port;
  m_dest_port_def = a_dest_port;
  m_local_port = a_local_port;
  m_udp_is_open = true;
}

// Закрытие сокета
void udp_t::close()
{
 m_udp_is_open = false;
}

// Проверка занятости буфера передачи
irs_u8 udp_t::write_begin()
{
  return m_user_tx^0x1;
}

// Выдача команды на передачу
void udp_t::write_end(irs_u8* ap_dest_ip, irs_u16* ap_dest_port, irs_u16 a_size)
{
  if (ap_dest_ip) {
    m_dest_ip[0] = ap_dest_ip[0];
    m_dest_ip[1] = ap_dest_ip[1];
    m_dest_ip[2] = ap_dest_ip[2];
    m_dest_ip[3] = ap_dest_ip[3];
  } else {
    m_dest_ip[0] = m_dest_ip_def[0];
    m_dest_ip[1] = m_dest_ip_def[1];
    m_dest_ip[2] = m_dest_ip_def[2];
    m_dest_ip[3] = m_dest_ip_def[3];
  }
  if (ap_dest_port) {
    m_dest_port = *ap_dest_port;
  } else {
    m_dest_port = m_dest_port_def;
  }
  if (a_size >= 18) m_tx_user_length_hard = a_size;
  else m_tx_user_length_hard = 18;
  m_tx_user_length_udp = a_size;
  m_user_tx = true;
}

// Проверка буфера приема
irs_u16 udp_t::read_begin(irs_u8* ap_dest_ip, irs_u16 *ap_dest_port)
{
  irs_u16 data;
  if (m_user_rx) {
    data = m_rx_user_length;
    if (ap_dest_ip) {
      IRS_LODWORD(*ap_dest_ip) = IRS_LODWORD(rx_buf[0x1A]);
    }
    if (ap_dest_port) {
      IRS_HIBYTE(*ap_dest_port) = rx_buf[0x22];
      IRS_LOBYTE(*ap_dest_port) = rx_buf[0x23];
    }
  } else {
    data = 0;
  }
  return data;
}

// Освобождение буфера приема
void udp_t::read_end()
{
  m_user_rx = false;
  rx_hard = 0;
}

void udp_t::udp()
{
  if (m_udp_is_open) {
    server_udp();
  } else {
    rx_hard = 0;
  }
}

void udp_t::ip(void)
{
  if ((rx_buf[0x1e] == m_ip[0]) && (rx_buf[0x1f] == m_ip[1])&&
    (rx_buf[0x20] == m_ip[2]) && (rx_buf[0x21] == m_ip[3]))
  {
    if (rx_buf[0x17] == 0x01) { 
      icmp(); 
    }
    if(rx_buf[0x17] == 0x11) { 
      udp();
    }
    //if (packet[0x17]==0x6) tcp();
  } else {
    rx_hard = 0;
  }
}

void udp_t::tick()
{
  if (rx_hard == 1)
  {
    if ((rx_buf[0xc] == 0x8)&&(rx_buf[0xd] == 0)) ip();
    if ((rx_buf[0xc] == 0x8)&&(rx_buf[0xd] == 6)) arp();
    rx_hard = 0;
  }

  if (m_udp_is_open) {
    client_udp();
  }

  if (m_send_arp) {
    if (tx_hard == 0) send_arp();
  }
  if (m_send_icmp) {
    if (tx_hard == 0) send_icmp();
  }
  if (m_send_udp) {
    if (tx_hard == 0) send_udp();
  }
}

irs_size_t udp_t::udp_buf_size()
{
  return m_user_buf_size;
}

udp_t* get_udp()
{
  static udp_t udp;
  return &udp;
}

}; //namespace

//Global func:

void Init_UDP(const irs_u8* ap_mymac, const irs_u8* ap_myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port,
  irs_size_t a_bufs_size)
{
  get_udp()->init(ap_mymac, ap_myip, a_data_port, a_address_port, a_bufs_size);
}

void Deinit_UDP()
{
  get_udp()->~udp_t();
}

void Tick_UDP()
{
  get_udp()->tick();
}

void OpenUDP(irs_u16 a_local_port, irs_u16 a_dest_port, 
  const irs_u8* ap_dest_ip)
{
  get_udp()->open(a_local_port, a_dest_port, ap_dest_ip);
}

void CloseUDP()
{
  get_udp()->close();
}

irs_u8 WriteUDP_begin()
{
  return get_udp()->write_begin();
}

void WriteUDP_end(irs_u8* ap_dest_ip, irs_u16* ap_dest_port, irs_u16 a_size)
{
  get_udp()->write_end(ap_dest_ip, ap_dest_port, a_size);
}

irs_u16 ReadUDP_begin(irs_u8* ap_dest_ip, irs_u16* ap_dest_port)
{
  return get_udp()->read_begin(ap_dest_ip, ap_dest_port);
}

void ReadUDP_end()
{
  get_udp()->read_end();
}

irs_size_t udp_buf_size()
{
  return get_udp()->udp_buf_size();
}
