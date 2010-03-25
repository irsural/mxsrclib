// UDP/IP-стэк Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Дата: 30.05.2008

#ifndef UDPSTACKH
#define UDPSTACKH

#include <irsdefs.h>

#include <RTL8019AS.h>
#include <irsdefs.h>
#include <gdefs.h>
#include <irsconfig.h>

#include <irsfinal.h>

// Размер заголовков Ethernet + IP + UDP
#define HEADERS_SIZE 42
#ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
// Размер буфера передачи
#define UDP_BUFFER_SIZE_TX (ETHERNET_PACKET_TX - HEADERS_SIZE)
// Размер буфера приема
#define UDP_BUFFER_SIZE_RX (ETHERNET_PACKET_RX - HEADERS_SIZE)
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS

// Размер IP-адреса
#define IRS_UDP_IP_SIZE 4

// Инициализация UDP/IP
#ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
void Init_UDP(const irs_u8 *mymac, const irs_u8 *myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port);
#else //IRS_LIB_UDP_RTL_STATIC_BUFS
void Init_UDP(const irs_u8 *mymac, const irs_u8 *myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port,
  irs_size_t bufs_size = 250);
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS
// Деинициализация UDP/IP
void Deinit_UDP();
// Элементарное действие
void Tick_UDP();
// Открытие сокета
void OpenUDP(irs_u16 localport_, irs_u16 destport_, const irs_u8 *ip_dest_);
// Закрытие сокета
void CloseUDP();
// Проверка занятости буфера передачи
irs_u8 WriteUDP_begin();
// Выдача команды на передачу
void WriteUDP_end(irs_u8 *dest_ip_a, irs_u16 *dest_port_a, irs_u16 size);
// Проверка буфера чтения
irs_u16 ReadUDP_begin(irs_u8 *dest_ip_a, irs_u16 *dest_port_a);
// Освобождение буфера приема
void ReadUDP_end();
#ifndef IRS_LIB_UDP_RTL_STATIC_BUFS
irs_size_t udp_buf_size();
#endif //IRS_LIB_UDP_RTL_STATIC_BUFS

// Укзатель на буфер приема
extern irs_u8 *user_rx_buf;
// Укзатель на буфер передачи
extern irs_u8 *user_tx_buf;

#endif //UDPSTACKH
