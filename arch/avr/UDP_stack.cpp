// UDP/IP-стек Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Дата: 30.05.2008

#define IRS_UDP_ARP_CASH_EXT // Расширеный ARP-кэш

#include <irsdefs.h>

#include <timer.h>
#include <irscpp.h>
#include <mxdata.h>
//#include <irsavrutil.h>
#include <udp_stack.h>

#include <irsfinal.h>

#ifdef NOP
#define MAX_IP_OPTLEN		40				/* Max IP Header option field length	*/
#define IP_MIN_HLEN			20		
#endif //NOP


#ifdef IRS_UDP_ARP_CASH_EXT
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
#endif //IRS_UDP_ARP_CASH_EXT
  


irs_u8 __flash prot_icmp = 0x01;
irs_u8 __flash prot_tcp = 0x06;
irs_u8 __flash prot_udp = 0x11;
irs_u8 ip[4] = {0, 0, 0, 0};
irs_u8 mac[6] = {0, 0, 0, 0, 0, 0};
const irs_u8 ARPBUF_SIZE = 42;
const irs_u8 ARPBUF_SENDSIZE = 60;
const irs_u8 ICMPBUF_SIZE = 200;
#ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
irs_u8 arpbuf[ARPBUF_SENDSIZE];
irs_u8 icmpbuf[ICMPBUF_SIZE];
#else //IRS_LIB_UDP_RTL_STATIC_BUFS
irs::raw_data_t<irs_u8> arpbuf_data;
irs::raw_data_t<irs_u8> icmpbuf_data;
irs_u8* arpbuf = IRS_NULL;
irs_u8* icmpbuf = IRS_NULL;
irs_size_t user_buf_size = 0;
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS
irs_u8 arpcash[10];///первые 4 под IP остальные MAC
irs_u16 rxlenudp = 0;
irs_u16 rxlenicmp = 0;
////flags
irs_u8 dest_ip[4] = {0, 0, 0, 0};
irs_u8 dest_ip_def[4] = {0, 0, 0, 0};
irs_u8 user_rx=0;
irs_u8 user_tx=0;
irs_u8 tx_udp=0;
irs_u16 identif = 0;
irs_u16 rx_user_length=0;
//irs_u16 rx_user_length=0;
irs_u16 tx_user_length_hard=0;
irs_u16 tx_user_length_udp=0;

irs_u16 destport=0;
irs_u16 destport_def=0;
irs_u16 localport=0;
irs_u8 openUDPflag=0;
irs_u8 *user_rx_buf = 0;
irs_u8 *user_tx_buf = 0;

irs_u8 rxarp=0,send_arp=0,rxicmp=0,send_icmp=0,send_udp=0;

#ifdef IRS_UDP_ARP_CASH_EXT
namespace {
// Экземпдяр ARP-кэша
irs::udp::arp_cash_t arp_cash;
// Ссылка на IP из кэша
irs::udp::ip_t& cash_ip = irs::udp::ip_from_data(arpcash[0]);
// Ссылка на MAC из кэша
irs::udp::mac_t& cash_mac = irs::udp::mac_from_data(arpcash[4]);
} //namespace
#endif //IRS_UDP_ARP_CASH_EXT

#ifdef NOP
typedef unsigned char UINT8;
typedef unsigned int UINT16;
typedef unsigned long UINT32;

struct ip_frame
{
	char	vihl;					/**< Version & Header Length field	*/
	char	tos;					/**< Type Of Service				*/
	unsigned int  tlen;					/**< Total Length					*/
	unsigned int	id;						/**< IP Identification number		*/
	unsigned int  frags;					/**< Flags & Fragment offsett		*/
	char	ttl;					/**< Time to live					*/
	char	protocol;				/**< Protocol over IP				*/
	unsigned int 	checksum;				/**< Header Checksum				*/
	long	sip;					/**< Source IP address				*/
	long	dip;					/**< Destination IP address			*/
	char	opt[MAX_IP_OPTLEN + 1]; /**< Option field					*/
	unsigned int	buf_index;				/**< Next offset from the start of
									 * 	 network buffer				
									 */
};
#endif //NOP

// Инициализация UDP/IP
#ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
void Init_UDP(const irs_u8 *mymac,const irs_u8 *myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port)
#else //IRS_LIB_UDP_RTL_STATIC_BUFS
void Init_UDP(const irs_u8 *mymac,const irs_u8 *myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port,
  irs_size_t bufs_size)
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS
{
  rtl_set_ports(a_data_port, a_address_port);

  #ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
  initrtl(mymac);
  #else //IRS_LIB_UDP_RTL_STATIC_BUFS
  initrtl(mymac, bufs_size);
  arpbuf_data.resize(ARPBUF_SENDSIZE);
  icmpbuf_data.resize(ICMPBUF_SIZE);
  arpbuf = arpbuf_data.data();
  icmpbuf = icmpbuf_data.data();
  user_buf_size = tx_buf_size() - HEADERS_SIZE;
  #endif //IRS_LIB_UDP_RTL_STATIC_BUFS

  ip[0]=myip[0];
  ip[1]=myip[1];
  ip[2]=myip[2];
  ip[3]=myip[3];

  mac[0]=mymac[0];
  mac[1]=mymac[1];
  mac[2]=mymac[2];
  mac[3]=mymac[3];
  mac[4]=mymac[4];
  mac[5]=mymac[5];
  user_rx_buf=&rx_buf[HEADERS_SIZE];
  user_tx_buf=&tx_buf[HEADERS_SIZE];
  
  memset(arpbuf, 0, ARPBUF_SENDSIZE);
  memset(icmpbuf, 0, ICMPBUF_SIZE);
}
// Деинициализация UDP/IP
void Deinit_UDP()
{
}

irs_u8 cash(irs_u8 dest_ip[4])
{ 
  irs_u8 result = 0;
  
  #ifdef IRS_UDP_ARP_CASH_EXT
  const irs::udp::ip_t& ip = irs::udp::ip_from_data(dest_ip);
  if (ip != irs::udp::broadcast_ip) {
    if (arp_cash.ip_to_mac(ip, cash_mac)) {
      cash_ip = ip;
      result = 1;
    }
  } else {
    cash_ip = irs::udp::broadcast_ip;
    cash_mac = irs::udp::broadcast_mac;
    result = 1;
  }
  #else //IRS_UDP_ARP_CASH_EXT
  const irs_u32 broadcast_ip = 0xFFFFFFFF;
  if (IRS_LODWORD(dest_ip[0]) == broadcast_ip) {
    *(irs_u32*)(&arpcash[0]) = 0xFFFFFFFF;
    arpcash[4]=0xff;
    arpcash[5]=0xff;
    arpcash[6]=0xff;
    arpcash[7]=0xff;
    arpcash[8]=0xff;
    arpcash[9]=0xff;
    result = 1;
  } else { 
    if (IRS_LODWORD(arpcash[0]) != 0)
    if (IRS_LODWORD(dest_ip[0]) == IRS_LODWORD(arpcash[0])) {
      result = 1;
    }
  }
  #endif //IRS_UDP_ARP_CASH_EXT
  
  return result;
}


/////////////////////////////////CHEKSUM////////////////////////
irs_u16 ip_checksum (irs_u16 cs, irs_u8 dat, irs_u16 count)
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
irs_u16 cheksum(irs_u16 count, irs_u8 *adr)
{
  irs_u16 ip_cs = 0;

  for (irs_u16 cs_cnt = 0; cs_cnt < count; cs_cnt++) {
    ip_cs = ip_checksum(ip_cs, adr[cs_cnt], cs_cnt);
  }

	ip_cs = 0xffff^ip_cs;

  return ip_cs;
}
#ifdef NOP
irs_u16 cheksum(irs_u16 count, irs_u8 *adr)
{
  long sum=0;
  irs_u16 byte;
  while (count > 1) {
    byte = (((irs_u16)*adr) << 8) + *(adr + 1);

    //sum+=*(irs_u16*) adr;
    sum += byte;
    adr += 2;
    count -= 2;
  }

  if (count > 0) sum += *(irs_u8*) adr;
  while (sum >> 16) sum = (sum&0xffff) + (sum >> 16);
  return((0xFFFF^sum));
}
#endif //NOP

#ifndef NOP
irs_u16 cheksumUDP(irs_u16 count, irs_u8 *adr)
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
#else //NOP
irs_u16 cheksumUDP(irs_u16 count,irs_u8 *adr)
{
  irs_u8 pseudo_header[12],count_head=12;
irs_u8 *adr_h;
irs_u32 sum=0;
irs_u16 byte;
 ///psevdo zagolovok
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
 adr_h=&pseudo_header[0];
 while (count_head>1) {
                 byte = (((irs_u16)*adr_h) << 8) + *(adr_h + 1);

                 //sum+=*(irs_u16*) adr;
                 sum+=byte;
                 adr_h+=2;
                 count_head-=2;
                }

 while (count>1) {
                 byte = (((irs_u16)*adr) << 8) + *(adr + 1);

                 //sum+=*(irs_u16*) adr;
                 sum+=byte;
                 adr+=2;
                 count-=2;
                }

if(count>0) sum+= ((irs_u16)*adr)<<8;
while (sum>>16)
sum=(sum&0xffff) + (sum>>16);
return((0xFFFF^sum));
}
#endif //NOP

////////////////////////////////////ARP/////////////////////////////////////////////////
void arp_ping(irs_u8 dest_ip_[4])
{
  //Широковещательный запрос
  arpbuf[0x0]=0xff;
  arpbuf[0x1]=0xff;
  arpbuf[0x2]=0xff;
  arpbuf[0x3]=0xff;
  arpbuf[0x4]=0xff;
  arpbuf[0x5]=0xff;

  //Локальный МАС-адресс
  arpbuf[0x6]=mac[0x0];
  arpbuf[0x7]=mac[0x1];
  arpbuf[0x8]=mac[0x2];
  arpbuf[0x9]=mac[0x3];
  arpbuf[0xa]=mac[0x4];
  arpbuf[0xb]=mac[0x5];

  //EtherType пакетов запроса 0x0806 
  arpbuf[0xc]=0x8;
  arpbuf[0xd]=0x6;

  //Hardware type
  arpbuf[0xe]=0x0;
  arpbuf[0xf]=0x1;
  
  //Protocol type
  arpbuf[0x10]=0x8;
  arpbuf[0x11]=0x0;
  
  //Hardware lenght
  arpbuf[0x12]=0x6;
  //Protocol lenght
  arpbuf[0x13]=0x4;
  //Код операции
  arpbuf[0x15]=0x1;

  //Физический адрес отправителя
  arpbuf[0x16]=mac[0x0];
  arpbuf[0x17]=mac[0x1];
  arpbuf[0x18]=mac[0x2];
  arpbuf[0x19]=mac[0x3];
  arpbuf[0x1a]=mac[0x4];
  arpbuf[0x1b]=mac[0x5];

  //Логический адрес отправителя
  arpbuf[0x1c]=ip[0x0];
  arpbuf[0x1d]=ip[0x1];
  arpbuf[0x1e]=ip[0x2];
  arpbuf[0x1f]=ip[0x3];

  //Физический адрес получателя
  arpbuf[0x20]=arpbuf[0x0];
  arpbuf[0x21]=arpbuf[0x0];
  arpbuf[0x22]=arpbuf[0x0];
  arpbuf[0x23]=arpbuf[0x0];
  arpbuf[0x24]=arpbuf[0x0];
  arpbuf[0x25]=arpbuf[0x0];

  //Логический адрес получателя
  arpbuf[0x26]=dest_ip_[0x0];
  arpbuf[0x27]=dest_ip_[0x1];
  arpbuf[0x28]=dest_ip_[0x2];
  arpbuf[0x29]=dest_ip_[0x3];

  send_arp=1;
}

void arppacket(void)
{ //irs_u8 byte=0;

/////формируем пакет
  arpbuf[0x0]=arpbuf[0x6];
  arpbuf[0x1]=arpbuf[0x7];
  arpbuf[0x2]=arpbuf[0x8];
  arpbuf[0x3]=arpbuf[0x9];
  arpbuf[0x4]=arpbuf[0xa];
  arpbuf[0x5]=arpbuf[0xb];

  arpbuf[0x6]=mac[0x0];
  arpbuf[0x7]=mac[0x1];
  arpbuf[0x8]=mac[0x2];
  arpbuf[0x9]=mac[0x3];
  arpbuf[0xa]=mac[0x4];
  arpbuf[0xb]=mac[0x5];

  arpbuf[0x15]=0x2;

  arpbuf[0x20]=arpbuf[0x16];
  arpbuf[0x21]=arpbuf[0x17];
  arpbuf[0x22]=arpbuf[0x18];
  arpbuf[0x23]=arpbuf[0x19];
  arpbuf[0x24]=arpbuf[0x1a];
  arpbuf[0x25]=arpbuf[0x1b];

  arpbuf[0x26]=arpbuf[0x1c];
  arpbuf[0x27]=arpbuf[0x1d];
  arpbuf[0x28]=arpbuf[0x1e];
  arpbuf[0x29]=arpbuf[0x1f];

  arpbuf[0x16]=mac[0x0];
  arpbuf[0x17]=mac[0x1];
  arpbuf[0x18]=mac[0x2];
  arpbuf[0x19]=mac[0x3];
  arpbuf[0x1a]=mac[0x4];
  arpbuf[0x1b]=mac[0x5];

  arpbuf[0x1c]=ip[0x0];
  arpbuf[0x1d]=ip[0x1];
  arpbuf[0x1e]=ip[0x2];
  arpbuf[0x1f]=ip[0x3];

  send_arp=1;
}

void arpcash_(void)
{
  #ifdef IRS_UDP_ARP_CASH_EXT
  irs::udp::ip_t& arp_ip = irs::udp::ip_from_data(arpbuf[0x1c]);
  irs::udp::mac_t& arp_mac = irs::udp::mac_from_data(arpbuf[0x16]);
  arp_cash.add(arp_ip, arp_mac);
  #else //IRS_UDP_ARP_CASH_EXT
  arpcash[4]=arpbuf[0x16];
  arpcash[5]=arpbuf[0x17];
  arpcash[6]=arpbuf[0x18];
  arpcash[7]=arpbuf[0x19];
  arpcash[8]=arpbuf[0x1a];
  arpcash[9]=arpbuf[0x1b];

  arpcash[0]=arpbuf[0x1c];
  arpcash[1]=arpbuf[0x1d];
  arpcash[2]=arpbuf[0x1e];
  arpcash[3]=arpbuf[0x1f];
  #endif //IRS_UDP_ARP_CASH_EXT
  rxarp=0;
}

void arp()
{
  //irs_u8 i=0;
  if((rx_buf[0x26]==ip[0])&&(rx_buf[0x27]==ip[1])&&
    (rx_buf[0x28]==ip[2])&&(rx_buf[0x29]==ip[3]))
  {
    if (rxarp==0) {
      rxarp = 1;
      for (irs_u8 i = 0; i < ARPBUF_SIZE; i++) arpbuf[i] = rx_buf[i];
      rx_hard = 0;
      if (arpbuf[0x15]==2) arpcash_();
      if (arpbuf[0x15]==1) arppacket();
    }
  } else {
    rx_hard=0;
  }
}

void sendARP(void)
{ 
  //sendpacketARP(&arpbuf[0]);
  sendpacket(ARPBUF_SENDSIZE, arpbuf);
  //sendpacket(60, arpbuf);
  send_arp=0;
  rxarp=0;
}
///////////////////////////ICMP/////////////////////////////////////////////////////////////
void icmppacket()
{
  // MAC-приемника
  icmpbuf[0x0] = icmpbuf[0x6];
  icmpbuf[0x1] = icmpbuf[0x7];
  icmpbuf[0x2] = icmpbuf[0x8];
  icmpbuf[0x3] = icmpbuf[0x9];
  icmpbuf[0x4] = icmpbuf[0xa];
  icmpbuf[0x5] = icmpbuf[0xb];

  // MAC-передатчика
  icmpbuf[0x6] = mac[0x0];
  icmpbuf[0x7] = mac[0x1];
  icmpbuf[0x8] = mac[0x2];
  icmpbuf[0x9] = mac[0x3];
  icmpbuf[0xa] = mac[0x4];
  icmpbuf[0xb] = mac[0x5];

  // Уменьшаем TTL на 1
  // Обычно при ответе на ping стек TCP/IP не уменьшает TTL
  //icmpbuf[0x16] -= 1;

  // Обнуляем контрольную сумму IP
  icmpbuf[0x18] = 0x00;
  icmpbuf[0x19] = 0x00;

  // IP-адрес приемника
  icmpbuf[0x1E] = icmpbuf[0x1a];
  icmpbuf[0x1f] = icmpbuf[0x1b];
  icmpbuf[0x20] = icmpbuf[0x1c];
  icmpbuf[0x21] = icmpbuf[0x1d];

  // IP-адрес источника
  icmpbuf[0x1a] = ip[0x0];
  icmpbuf[0x1b] = ip[0x1];
  icmpbuf[0x1c] = ip[0x2];
  icmpbuf[0x1d] = ip[0x3];

  // Контрольная сумма IP
  irs_u16 chksum_ip = cheksum(20, &icmpbuf[0xe]);
  icmpbuf[0x18] = IRS_HIBYTE(chksum_ip);
  icmpbuf[0x19] = IRS_LOBYTE(chksum_ip);

  // Тип ICMP - эхо-ответ
  icmpbuf[0x22] = 0x00;
  // Код ICMP
  icmpbuf[0x23] = 0x00;

  // Обнуляем контрольную сумму ICMP
  icmpbuf[0x24] = 0x00;
  icmpbuf[0x25] = 0x00;

  // Контрольная сумма ICMP
  irs_u16 chksum_icmp = cheksum(rxlenicmp - 0x22, &icmpbuf[0x22]);
  icmpbuf[0x24] = IRS_HIBYTE(chksum_icmp);
  icmpbuf[0x25] = IRS_LOBYTE(chksum_icmp);

  send_icmp = 1;
}
void sendICMP()
{
  //sendpacketICMP(rxlenicmp, &icmpbuf[0]);
  sendpacket(rxlenicmp, icmpbuf);
  #ifdef DBGMODE
  PORTB ^= (1 << PORTB2);
  #endif //DBGMODE
  send_icmp=0;
  rxicmp=0;
}
void icmp()
{
  if ((rx_buf[0x22] == 8)&&(rx_buf[0x23] == 0))
  {
    if (rxicmp == 0) {
      rxlenicmp = rxlen_hard - 4;
      if (rxlenicmp <= ICMPBUF_SIZE) {
        rxicmp = 1;
        for (irs_i16 i = 0; i < rxlenicmp; i++) icmpbuf[i] = rx_buf[i];
        icmppacket();
      }
    }
  }
  rx_hard = 0;
}


////////////////////////////////UDP////////////////////////


void serverUDP()
{
  //if ((rx_buf[0x24] == (localport>>8))&&(rx_buf[0x25] == (localport&0xff))) {
  if ((rx_buf[0x24] == IRS_HIBYTE(localport)) &&
      (rx_buf[0x25] == IRS_LOBYTE(localport))) {
    user_rx=1;
    irs_u16 udp_size = 0;
    IRS_LOBYTE(udp_size) = rx_buf[0x27];
    IRS_HIBYTE(udp_size) = rx_buf[0x26];
    rx_user_length = udp_size - 8;
    #ifdef IRS_UDP_ARP_CASH_EXT
    irs::udp::ip_t& ip = irs::udp::ip_from_data(rx_buf[0x1A]);
    irs::udp::mac_t& mac = irs::udp::mac_from_data(rx_buf[0x06]);
    arp_cash.add(ip, mac);

#ifdef NOP
    irs::udp::mac_t my_mac = {{0x00, 0x14, 0x85, 0x3E, 0x30, 0x7A}};
    if (mac == my_mac) {
  static irs::blink_t red_blink(irs_avr_porte, 3);
  red_blink.set();
    }
#endif //NOP
  
    #endif //IRS_UDP_ARP_CASH_EXT
  } else {
    rx_hard = 0;
  }

}
void udppacket()
{
  tx_buf[0x0]=arpcash[0x4];
  tx_buf[0x1]=arpcash[0x5];
  tx_buf[0x2]=arpcash[0x6];
  tx_buf[0x3]=arpcash[0x7];
  tx_buf[0x4]=arpcash[0x8];
  tx_buf[0x5]=arpcash[0x9];

  tx_buf[0x6]=mac[0x0];
  tx_buf[0x7]=mac[0x1];
  tx_buf[0x8]=mac[0x2];
  tx_buf[0x9]=mac[0x3];
  tx_buf[0xa]=mac[0x4];
  tx_buf[0xb]=mac[0x5];

  ///type
  tx_buf[0xc]=0x8;
  tx_buf[0xd]=0x0;
  //ver//tos
  tx_buf[0xe]=0x45;
  tx_buf[0xf]=0x10; 
  ///alllenth
  irs_u16 ip_length = tx_user_length_udp + 28;
  tx_buf[0x11]=IRS_LOBYTE(ip_length);
  tx_buf[0x10]=IRS_HIBYTE(ip_length);
  //рисуем идентификатор
  identif++;
  tx_buf[0x13] = IRS_LOBYTE(identif);
  tx_buf[0x12] = IRS_HIBYTE(identif);
  //if (identif==65000) identif=0;
  //tx_buf[0x13]=identif&0xff;
  //tx_buf[0x12]=identif>>8;
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
  tx_buf[0x1e]=arpcash[0x0];
  tx_buf[0x1f]=arpcash[0x1];
  tx_buf[0x20]=arpcash[0x2];
  tx_buf[0x21]=arpcash[0x3];
  //ip sourse
  tx_buf[0x1a]=ip[0x0];
  tx_buf[0x1b]=ip[0x1];
  tx_buf[0x1c]=ip[0x2];
  tx_buf[0x1d]=ip[0x3];
  //clear checsum

  ///checksum
  irs_u16 chksum_ip = cheksum(20, &tx_buf[0xe]);
  tx_buf[0x18] = IRS_HIBYTE(chksum_ip);
  tx_buf[0x19] = IRS_LOBYTE(chksum_ip);
  //sum=cheksum(20,&tx_buf[0xe]);
  //tx_buf[0x18]=(sum>>8);
  //tx_buf[0x19]=(sum&0xff);

  //ports udp
  tx_buf[0x24]=IRS_HIBYTE(destport);
  tx_buf[0x25]=IRS_LOBYTE(destport);

  tx_buf[0x22]=IRS_HIBYTE(localport);
  tx_buf[0x23]=IRS_LOBYTE(localport);
  //udp lenyh
  irs_u16 udp_length = tx_user_length_udp + 8;
  tx_buf[0x27] = IRS_LOBYTE(udp_length);
  tx_buf[0x26] = IRS_HIBYTE(udp_length);
  //tx_buf[0x27]=((tx_user_length_udp+8)&0xFF);
  //tx_buf[0x26]=((tx_user_length_udp+8)>>8);
  //checsum
  tx_buf[0x28]=0;
  tx_buf[0x29]=0;

  ///checsum
  irs_u16 chksum_udp = cheksumUDP(tx_user_length_udp + 8, &tx_buf[0x22]);
  tx_buf[0x28]=IRS_HIBYTE(chksum_udp);
  tx_buf[0x29]=IRS_LOBYTE(chksum_udp);
  //tx_buf[0x28]=(sum>>8);
  //tx_buf[0x29]=(sum&0xff);
  send_udp=1;
}
void sendUDP()
{
  sendpacket(tx_user_length_hard+HEADERS_SIZE,&tx_buf[0]);
  send_udp=0;
  tx_udp=0;
  user_tx=0;
}
void clientUDP()
{
  if (user_tx==1) {
    static irs_bool udp_wait_arp = irs_false;
    static counter_t to_udp_wait_arp;
    #define t_udp_wait_arp TIME_TO_CNT(1, 1)

    if (cash(dest_ip) == 1) {
      if (tx_udp == 0) {
        udppacket();
        tx_udp=1;
        //sendUDP();
        udp_wait_arp = irs_false;
      }
    } else {
      if (udp_wait_arp) {
        if (test_to_cnt(to_udp_wait_arp)) {
          udp_wait_arp = irs_false;
          send_udp = 0;
          tx_udp = 0;
          user_tx = 0;
        }
      } else {
        if (rxarp == 0) {
          rxarp = 1;
          arp_ping(dest_ip);
          udp_wait_arp = irs_true;
          set_to_cnt(to_udp_wait_arp, t_udp_wait_arp);
        }
      }
    }

  }
}



// Открытие сокета
void OpenUDP(irs_u16 localport_, irs_u16 destport_, const irs_u8 *ip_dest_)
{
  dest_ip[0]=ip_dest_[0];
  dest_ip[1]=ip_dest_[1];
  dest_ip[2]=ip_dest_[2];
  dest_ip[3]=ip_dest_[3];

  dest_ip_def[0]=ip_dest_[0];
  dest_ip_def[1]=ip_dest_[1];
  dest_ip_def[2]=ip_dest_[2];
  dest_ip_def[3]=ip_dest_[3];

  destport=destport_;
  destport_def = destport_;
  localport=localport_;
  openUDPflag=1;
}
// Закрытие сокета
void CloseUDP()
{
 openUDPflag=0;
}
// Проверка занятости буфера передачи
irs_u8 WriteUDP_begin()
{
  return user_tx^0x1;
}
// Выдача команды на передачу
void WriteUDP_end(irs_u8 *dest_ip_a, irs_u16 *dest_port_a, irs_u16 size)
{
  if (dest_ip_a) {
    dest_ip[0] = dest_ip_a[0];
    dest_ip[1] = dest_ip_a[1];
    dest_ip[2] = dest_ip_a[2];
    dest_ip[3] = dest_ip_a[3];
  } else {
    dest_ip[0] = dest_ip_def[0];
    dest_ip[1] = dest_ip_def[1];
    dest_ip[2] = dest_ip_def[2];
    dest_ip[3] = dest_ip_def[3];
  }
  if (dest_port_a) {
    destport = *dest_port_a;
  } else {
    destport = destport_def;
  }
  if (size >= 18) tx_user_length_hard = size;
  else tx_user_length_hard = 18;
  tx_user_length_udp = size;
  user_tx = 1;
}
// Проверка буфера приема
irs_u16 ReadUDP_begin(irs_u8 *dest_ip_a, irs_u16 *dest_port_a)
{
  irs_u16 data;
  if (user_rx == 1) {
    data = rx_user_length;
    if (dest_ip_a) {
      IRS_LODWORD(*dest_ip_a) = IRS_LODWORD(rx_buf[0x1A]);
    }
    if (dest_port_a) {
      IRS_HIBYTE(*dest_port_a) = rx_buf[0x22];
      IRS_LOBYTE(*dest_port_a) = rx_buf[0x23];
    }
  } else {
    data = 0;
  }
  return data;
}
// Освобождение буфера приема
void ReadUDP_end()
{
  user_rx=0;
  rx_hard=0;
}

void udp()
{
  if (openUDPflag == 1) {
    serverUDP();
  } else {
    rx_hard = 0;
  }
}

/////////////////////////////////////////////IP///////////////////////////////////////////////
void ip_(void)
{
 if((rx_buf[0x1e]==ip[0])&&(rx_buf[0x1f]==ip[1])&&(rx_buf[0x20]==ip[2])&&(rx_buf[0x21]==ip[3]))
                       {


                         if(rx_buf[0x17]==0x01){ icmp(); }
                         if(rx_buf[0x17]==0x11){ udp();}
                       // if (packet[0x17]==0x6) tcp();

                       }

else rx_hard=0;

}


////////////////////////////////////////////////
// Элементарное действие
void Tick_UDP()
{
  if (rx_hard == 1)
  {
    if ((rx_buf[0xc] == 0x8)&&(rx_buf[0xd] == 0)) ip_();
    if ((rx_buf[0xc] == 0x8)&&(rx_buf[0xd] == 6)) arp();
    rx_hard = 0;
  }

  if (openUDPflag == 1) {
    clientUDP();
  }

  if (send_arp) {
    if (tx_hard == 0) sendARP();
  }
  if (send_icmp) {
    if (tx_hard == 0) sendICMP();
  }
  if (send_udp) {
    if (tx_hard == 0) sendUDP();
  }
}

#ifndef IRS_LIB_UDP_RTL_STATIC_BUFS
irs_size_t udp_buf_size()
{
  return user_buf_size;
}
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS
