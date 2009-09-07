// UDP/IP-���� ���� ���������
// ��������������� ������������� �. �.
// ����: 30.05.2008

#ifndef UDPSTACKH
#define UDPSTACKH

#include <RTL8019AS.h>
#include <irsdefs.h>
#include <gdefs.h>

// ������ ���������� Ethernet + IP + UDP
#define HEADERS_SIZE 42
// ������ ������ ��������
#define UDP_BUFFER_SIZE_TX (ETHERNET_PACKET_TX - HEADERS_SIZE)
// ������ ������ ������
#define UDP_BUFFER_SIZE_RX (ETHERNET_PACKET_RX - HEADERS_SIZE)

// ������ IP-������
#define IRS_UDP_IP_SIZE 4

// ������������� UDP/IP
void Init_UDP(const irs_u8 *mymac, const irs_u8 *myip,
  irs_avr_port_t a_data_port, irs_avr_port_t a_address_port);
// ��������������� UDP/IP
void Deinit_UDP();
// ������������ ��������
void Tick_UDP();
// �������� ������
void OpenUDP(irs_u16 localport_, irs_u16 destport_, const irs_u8 *ip_dest_);
// �������� ������
void CloseUDP();
// �������� ��������� ������ ��������
irs_u8 WriteUDP_begin();
// ������ ������� �� ��������
void WriteUDP_end(irs_u8 *dest_ip_a, irs_u16 *dest_port_a, irs_u16 size);
// �������� ������ ������
irs_u16 ReadUDP_begin(irs_u8 *dest_ip_a, irs_u16 *dest_port_a);
// ������������ ������ ������
void ReadUDP_end();

// �������� �� ����� ������
extern irs_u8 *user_rx_buf;
// �������� �� ����� ��������
extern irs_u8 *user_tx_buf;

#endif //UDPSTACKH
