// Драйвер Ethernet для RTL8019AS Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Испорчен Поляковым М.
// Дата: 30.05.2008

#ifndef RTL8019ASH
#define RTL8019ASH

#include <irsdefs.h>
#include <gdefs.h>
#include <irsstd.h>

#define ETHERNET_PACKET_TX 250
#define ETHERNET_PACKET_RX 254
//#define ETHERNET_PACKET_TX 300
//#define ETHERNET_PACKET_RX 304
//#define ETHERNET_PACKET_TX 1550
//#define ETHERNET_PACKET_RX 1554

// Длина MAC(Ethernet)-адреса
#define IRS_RTL_MAC_SIZE 6

void initrtl(const irs_u8 *mac);
void sendpacket(irs_u16 length, irs_u8 *tx_buf);
extern irs_u8 rx_hard;
extern irs_u8 tx_hard;
extern irs_u8 rx_buf[];
extern irs_u8 tx_buf[];
extern irs_u16 rxlen_hard;

void rtl_set_ports(irs_avr_port_t a_data_port, irs_avr_port_t a_address_port);

#endif //RTL8019ASH
