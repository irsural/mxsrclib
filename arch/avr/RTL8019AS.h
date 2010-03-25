// Драйвер Ethernet для RTL8019AS Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Испорчен Поляковым М.
// Дата: 24.03.2010
// Ранняя дата: 30.05.2008

#ifndef RTL8019ASH
#define RTL8019ASH

#include <irsdefs.h>

#include <gdefs.h>
#include <irsstd.h>
#include <irsconfig.h>

#include <irsfinal.h>

#define ETHERNET_PACKET_TX 250
#define ETHERNET_PACKET_RX 254
//#define ETHERNET_PACKET_TX 300
//#define ETHERNET_PACKET_RX 304
//#define ETHERNET_PACKET_TX 1550
//#define ETHERNET_PACKET_RX 1554

// Длина MAC(Ethernet)-адреса
#define IRS_RTL_MAC_SIZE 6

#ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
void initrtl(const irs_u8 *mac);
#else //IRS_LIB_UDP_RTL_STATIC_BUFS
void initrtl(const irs_u8 *mac, irs_size_t bufs_size);
irs_size_t tx_buf_size();
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS
void sendpacket(irs_u16 length, irs_u8 *ext_tx_buf);
extern irs_u8 rx_hard;
extern irs_u8 tx_hard;
#ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
extern irs_u8 rx_buf[];
extern irs_u8 tx_buf[];
#else //IRS_LIB_UDP_RTL_STATIC_BUFS
extern irs_u8* tx_buf;
extern irs_u8* rx_buf;
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS
extern irs_u16 rxlen_hard;

void rtl_set_ports(irs_avr_port_t a_data_port, irs_avr_port_t a_address_port);

#endif //RTL8019ASH
