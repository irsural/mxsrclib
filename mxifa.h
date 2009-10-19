// ��������� ���������� ��� ������� ������ (�����������)
// Max Interface Abstraction
// ����: 19.10.2009
// ������ ����: 06.03.2008

#ifndef MXIFAH
#define MXIFAH

#include <irsdefs.h>
#include <mxifal.h>
#include <mxifar.h>
#include <irsstd.h>
#include <hardflowg.h>
#include <irserror.h>

//extern irs_bool dbg_write;

// ������ ������� ������������ �� ����������� ��� ��������
// zero - �������������� �����
#define MXIFA_ZERO          0
// mxnet
#define MXIFA_MXNET         1
// mxnetc
#define MXIFA_MXNETC        2
// messup
#define MXIFA_SUPPLY        3
// mesmul
#define MXIFA_MULTIMETER    4
// modbus serv
#define MXIFA_MODBUS        5
// modbus client
#define MXIFA_MODBUS_CL     6
// hardflow
#define MXIFA_HARDFLOW      7
// mxnetc 2
#define MXIFA_MXNETC_2      8
// mxnetc 3
#define MXIFA_MXNETC_3      9
// mxnetc 4
#define MXIFA_MXNETC_4     10
// mxnetc 5
#define MXIFA_MXNETC_5     11
// mxnetc 6
#define MXIFA_MXNETC_6     12
// zero 2
#define MXIFA_ZERO2        13
// zero 3
#define MXIFA_ZERO3        14
// ������������ �����
#define MXIFA_CNT_CHANNEL  15

// ��� ��� ������ GPIB-����������
typedef irs_i32 gpib_addr_t;

// ����� MAC(Ethernet)-������
#define MXIFA_MAC_SIZE 6

// ��� ������
typedef enum _mxifa_ei_t {
  // ����������
  mxifa_ei_unknown,
  // UDP/IP ���� ���������
  mxifa_ei_avr128_ether,
  // Win32 TCP/IP
  mxifa_ei_win32_tcp_ip,
  // Win32 National Instruments USB-GPIB
  mxifa_ei_win32_ni_usb_gpib,
  //linux TCP/IP
  mxifa_ei_linux_tcpip,
  //linux TCP/IP client
  mxifa_ei_linux_tcpip_cl,
  //  hardflow
  mxifa_ei_hardflow
} mxifa_ei_t;

// ��������� ��� �������� ��������� �������
typedef struct _mxifa_avr128_ether_dest_t {
  // ��������� IP
  mxip_t ip;
  // ��������� ����
  irs_u16 port;
} mxifa_avr128_ether_dest_t;

// ��������� ��� �������� ��������� �������
typedef struct _mxifa_win32_tcp_ip_dest_t {
  // ��������� IP
  mxip_t ip;
  // ��������� ����
  irs_u16 port;
} mxifa_win32_tcp_ip_dest_t;

// ��������� ��� �������� ��������� �������
typedef struct _mxifa_win32_ni_usb_gpib_dest_t {
  // ����� GPIB-����������
  gpib_addr_t address;
} mxifa_win32_ni_usb_gpib_dest_t;

//
typedef struct _mxifa_linux_dest_t {
  // ��������� IP
  mxip_t ip;
  // ��������� ����
  irs_u16 port;
} mxifa_linux_dest_t;

typedef struct _mxifa_linux_cl_dest_t {
  // ��������� IP
  mxip_t ip;
  // ��������� ����
  irs_u16 port;
} mxifa_linux_cl_dest_t;

// ��������� ��� �������� ��������� �������
typedef struct _mxifa_hardflow_dest_t {
  // �������������
  irs_uarc channel_id;
} mxifa_hardflow_dest_t;

// ���������� ��������� ��� �������� ��������� �������
typedef union _mxifa_dest_t {
  // ��������� ��� �������� ��������� ������� ������ ���� mxifa_ei_avr128_ether
  mxifa_avr128_ether_dest_t avr128_ether;
  // ��������� ��� �������� ��������� ������� ������ ���� mxifa_ei_win32_tcp_ip
  mxifa_win32_tcp_ip_dest_t win32_tcp_ip;
  // ��������� ��� �������� ��������� ������� ������
  // ���� mxifa_ei_win32_ni_usb_gpib
  mxifa_win32_ni_usb_gpib_dest_t win32_ni_usb_gpib;
  mxifa_linux_dest_t mxifa_linux_dest;
  mxifa_linux_cl_dest_t mxifa_linux_cl_dest;
  mxifa_hardflow_dest_t mxifa_hardflow_dest;
} mxifa_dest_t;

// ��������� ��� ��������� ������������ avr128_ether � ������� mxifa_set_config
typedef struct _mxifa_avr128_cfg {
  // ��������� IP
  mxip_t *ip;
  // ����� �������
  mxip_t *mask;
  // ��������� MAC(Ethernet)-����� (6 ����)
  irs_u8 *mac;
  // ���� ������ AVR � �������� ���������� ����� ������ RTL
  irs_avr_port_t data_port;
  // ���� ������ AVR � �������� ���������� ����� ������ RTL
  irs_avr_port_t address_port;
} mxifa_avr128_cfg;

// ��������� ��� ��������� ������������ win32_tcp_ip � ������� mxifa_set_config
typedef struct _mxifa_win32_tcp_ip_cfg {
  // ��������� IP
  mxip_t dest_ip;
  // ��������� ����
  irs_u16 dest_port;
  // ��������� ����
  irs_u16 local_port;
} mxifa_win32_tcp_ip_cfg;

// ��������� ��� ��������� ������������ win32_ni_usb_gpib
// � ������� mxifa_set_config
typedef struct _mxifa_win32_ni_usb_gpib_cfg {
  // ��������� ������������
  irs_u32 bitcfg;
  // ����� GPIB-����������
  gpib_addr_t address;
  // ����� ������������� ���������� ����, ����� ���������� �������� ������
  mxifa_sz_t read_count;
} mxifa_win32_ni_usb_gpib_cfg;

//
typedef struct _mxifa_linux_tcp_ip_cfg {
  // ��������� IP
  mxip_t dest_ip;
  // ��������� ����
  irs_u16 dest_port;
  // ��������� ����
  irs_u16 local_port;
  //Type of protocol IP or TCP
  int pro_type;

} mxifa_linux_tcp_ip_cfg;

typedef struct _mxifa_linux_tcp_ip_cl_cfg {
  // ��������� IP
  mxip_t dest_ip;
  // ��������� ����
  irs_u16 dest_port;
  // ��������� ����
  irs_u16 local_port;
  //Type of protocol IP or TCP
  int pro_type;

} mxifa_linux_tcp_ip_cl_cfg;

//  ��������� ��� ��������� ������������ hardflow
//  � ������� mxifa_set_config
typedef struct _mxifa_hardflow_cfg {
  irs::hardflow_t *user_hardflow;
} mxifa_hardflow_cfg;

#if defined(__WATCOMC__) || defined(_MSC_VER)
// ����������� ������� ��� ������������ ��� ������� ��� ���������� mxifa
inline void mxifa_init() {}
inline void mxifa_deinit() {}
inline void mxifa_tick() {}
inline void *mxifa_open(mxifa_ch_t, irs_bool) {  return IRS_NULL; }
inline void *mxifa_open_begin(mxifa_ch_t, irs_bool) {  return IRS_NULL; }
inline irs_bool mxifa_open_end(void*, bool) { return irs_false; }
inline irs_bool mxifa_close(void *) { return irs_false; }
inline irs_bool mxifa_close_begin(void*) { return irs_false; }
inline irs_bool mxifa_close_end(void*, bool) { return irs_false; }
inline irs_bool mxifa_write_begin(void*, mxifa_dest_t*, irs_u8*, 
  mxifa_sz_t) { return irs_false; }
inline irs_bool mxifa_write_end(void*, irs_bool) { return irs_false; }
inline irs_bool mxifa_read_begin(void*, mxifa_dest_t*, irs_u8*, 
  mxifa_sz_t) { return irs_false; }
inline irs_bool mxifa_read_end(void*, irs_bool) { return irs_false; }
inline mxifa_ei_t mxifa_get_channel_type(void*) { return mxifa_ei_unknown; }
inline irs_bool mxifa_set_config(void*, void *) { return irs_false; }
inline irs_bool mxifa_get_config(void *, void *) { return irs_false; }
inline mxifa_sz_t mxifa_fast_read(void *, mxifa_dest_t *, irs_u8 *,
  mxifa_sz_t) { return 0; }
inline irs_bool *mxifa_open_ex(mxifa_ch_t, void *, irs_bool) 
{ return IRS_NULL; }
inline irs_bool *mxifa_open_begin_ex(mxifa_ch_t, void *, irs_bool)
{ return IRS_NULL; }
inline irs_bool mxifa_open_end_ex(void *, bool) { return irs_false; }
inline mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t) 
{ return mxifa_ei_unknown; }
#else //defined(__WATCOMC__)
// ������������� mxifa
void mxifa_init();
// ��������������� mxifa
void mxifa_deinit();
// ������������ ��������
void mxifa_tick();
// ����������� �������� channel
// channel - ����� ������, ���������� � ����������� ������� ��� ������ ���������
// is_broadcast - �������� ������ � ����������������� ������
// ������� - ��������� �� ������ ������
void *mxifa_open(mxifa_ch_t channel, irs_bool is_broadcast);
// ������������� �������� channel
// channel - ����� ������, ���������� � ����������� ������� ��� ������ ���������
// is_broadcast - �������� ������ � ����������������� ������
// ������� - ��������� �� ������ ������
void *mxifa_open_begin(mxifa_ch_t channel, irs_bool is_broadcast);
// �������� ��������� �������� channel
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// abort - ����������� ���������� � ������� ��������
// ������� - �������� ��������
irs_bool mxifa_open_end(void *pchdata, bool abort);
// ����������� �������� channel
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// ������� - �������� ��������
irs_bool mxifa_close(void *pchdata);
// ������������� �������� channel
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// ������� - �������� ��������
irs_bool mxifa_close_begin(void *pchdata);
// �������� ��������� �������� channel
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// abort - ����������� ���������� � ������� ��������
// ������� - �������� ��������
irs_bool mxifa_close_end(void *pchdata, bool abort);
// ������ ������ � ��������� ��� ��������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// dest - �������� ������, ���� IRS_NULL, �� ������������ �������� ������
//   �� ���������
// buf - �������� �� ������ ��� ��������
// size - ������ ������ � ������
// ������� - �������� ��������
irs_bool mxifa_write_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size);
// �������� ��������� ������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// abort - ����������� ���������� � ������� ��������
// ������� - �������� ��������
irs_bool mxifa_write_end(void *pchdata, irs_bool abort);
// ������ �������� ������ �� ���������� � �����
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// dest - �������� ������, ���� IRS_NULL, �� ������������ �������� ������
//   �� ���������
// buf - �������� �� ����� ��� ������
// size - ������ ������ � ������
// ������� - �������� ��������
irs_bool mxifa_read_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size);
// �������� ��������� ������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// abort - ����������� ���������� � ������� ��������
// ������� - �������� ��������
irs_bool mxifa_read_end(void *pchdata, irs_bool abort);
// ������ ���� ������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// ������� - ��� ������
mxifa_ei_t mxifa_get_channel_type(void *pchdata);
// ��������� ������������ ������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// config - ��������� �� ��������� ..._cfg ��������������� ������� ���� ������
void mxifa_set_config(void *pchdata, void *config);
// ������ ������������ ������
// config - ��������� �� ��������� ..._cfg ��������������� ������� ���� ������
void mxifa_get_config(void *pchdata, void *config);

// ������ 0.4
// ������ �������� ������ ����������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// dest - �������� ������, ���� IRS_NULL, �� ������������ �������� ������
//   �� ���������
// buf - �������� �� ����� ��� ������
// size - ������ ������ � ������
// ������� - ���������� ������� �������� ������
mxifa_sz_t mxifa_fast_read(void *pchdata, mxifa_dest_t *dest, irs_u8 *buf,
  mxifa_sz_t size);
// ����������� �������� channel c ����������������
// channel - ����� ������, ���������� � ����������� ������� ��� ������ ���������
// is_broadcast - �������� ������ � ����������������� ������
// config - ��������� �� ��������� ..._cfg ��������������� ������� ���� ������
// ������� - ��������� �� ������ ������
void *mxifa_open_ex(mxifa_ch_t channel, void *config, irs_bool is_broadcast);
// ������������� �������� channel � �����������������
// channel - ����� ������, ���������� � ����������� ������� ��� ������ ���������
// is_broadcast - �������� ������ � ����������������� ������
// config - ��������� �� ��������� ..._cfg ��������������� ������� ���� ������
// ������� - ��������� �� ������ ������
void *mxifa_open_begin_ex(mxifa_ch_t channel, void *config,
  irs_bool is_broadcast);
// �������� ��������� �������� channel � ����������������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// abort - ����������� ���������� � ������� ��������
// ������� - �������� ��������
irs_bool mxifa_open_end_ex(void *pchdata, bool abort);
// ������ ���� ������ �� ������
// pchdata - ��������� �� ������ ������, ������������ mxifa_open ���
//   mxifa_open_begin
// ������� - ��� ������
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel);
#endif //defined(__WATCOMC__)

#endif //MXIFAH
