// UDP/IP-стэк Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Дата: 26.04.2010
// Ранняя дата: 30.05.2008

#ifndef UDPSTACKH
#define UDPSTACKH

#include <irsdefs.h>

#include <RTL8019AS.h>
#include <irsdefs.h>
//#include <gdefs.h>
#include <irsconfig.h>

#include <irsfinal.h>

// Размер заголовков Ethernet + IP + UDP
#define HEADERS_SIZE 42

// Размер IP-адреса
#define IRS_UDP_IP_SIZE 4

// Инициализация UDP/IP
void Init_UDP(const irs_u8 *mymac, const irs_u8 *myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port,
  irs_size_t bufs_size = 250);
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
// Размер буферов Ethernet
irs_size_t udp_buf_size();

// Укзатель на буфер приема
extern irs_u8 *user_rx_buf;
// Укзатель на буфер передачи
extern irs_u8 *user_tx_buf;

#endif //UDPSTACKH
