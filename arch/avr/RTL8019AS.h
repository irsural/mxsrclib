// Драйвер Ethernet для RTL8019AS Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Испорчен Поляковым М.
// Дата: 14.04.2010
// Ранняя дата: 30.05.2008

#ifndef RTL8019ASH
#define RTL8019ASH

#include <irsdefs.h>

#include <gdefs.h>
#include <irsstd.h>
#include <irsconfig.h>
#include <irsint.h>

#include <irsfinal.h>

// Длина MAC(Ethernet)-адреса
#define IRS_RTL_MAC_SIZE 6

void initrtl(const irs_u8 *mac, irs_size_t bufs_size);
irs_size_t tx_buf_size();
void sendpacket(irs_u16 length, irs_u8 *ext_tx_buf);
extern irs_u8 rx_hard;
extern irs_u8 tx_hard;
extern irs_u8* tx_buf;
extern irs_u8* rx_buf;
extern irs_u16 rxlen_hard;

void rtl_set_ports(irs_avr_port_t a_data_port, irs_avr_port_t a_address_port);

#endif //RTL8019ASH
