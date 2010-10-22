//! \file
//! \ingroup network_in_out_group
//! \brief ��������� ���������� ��� ������� ������ (�����������)
//!   ��� Borland C++ Builder
//!
//! ����: 12.05.2010\n
//! ������ ����: 18.03.2009

#define MXIFA_NEW

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <string.h>

#include <winsock2.h>

#include <mxifa.h>
#include <irsdefs.h>
#include <timer.h>
#include <irsalg.h>
#include <niusbgpib.h>
// ������ ������ � gpib-32.dll
#define GPIB_DIRECT_ACCESS
extern "C" {
#include <Decl-32.h>
}

#include <irsfinal.h>

// ������� �������� ������
#define TIMEOUT_CHANNEL TIME_TO_CNT(1, 1)

// ��������� GPIB
// ��������� ����� ����������
#define NO_SECONDARY_ADDR		  0
// �������
// TNONE, T10us, T30us, T100us, T300us, T1ms, T3ms, T10ms, T30ms, T100ms
// T300ms, T1s, T3s, T10s, T30s, T100s, T300s, T1000s
#define TIMEOUT					      T100ms
// ������� ����� ������ ��������
#define EOTMODE					      1
// ����� EOS ��������
#define EOSMODE					      1
//#define BUFLEN                30    // ������ ������ ������/��������

// ����� ��� ������ � UDP-�������� Windows
class udp_socket_t;
// ����� ��� ������ � National Instruments USB-GPIB
class ni_usb_gpib_t;

// ������������ ������� ������
typedef void (*mxifa_tick_func_t)(void *);

// ��������� ������ ������ ���� mxifa_ei_win32_tcp_ip
typedef struct _mxifa_win32_tcp_ip_t {
  // ��������� IP
  mxip_t local_ip;
  // ��������� ����
  irs_u16 local_port;
  // ��������� �����
  mxip_t local_mask;
  // ��������� IP
  mxip_t dest_ip;
  // ��������� ����
  irs_u16 dest_port;
  // ��������� �������������
  irs_bool is_broadcast;
  // �������� ��������� ������� ��� ������
  mxifa_win32_tcp_ip_dest_t wr_dest;
  // �������� ��������� ������� ��� ������
  mxifa_win32_tcp_ip_dest_t rd_dest;
  // �������� ��������� ������� ��� ������ ���������� �������
  mxifa_dest_t *rd_dest_caller;
  // ��������� �� UDP-����� Windows
  udp_socket_t *udp_socket;
  // ����� ������
  irs_u8 *rd_buf;
  // ������ ������ ������
  mxifa_sz_t rd_buf_size;
  // ������� � ������ ������
  mxifa_sz_t rd_buf_pos;
  // �������� ������ �������
  irs_bool read_proc;
  // ����� ������
  const irs_u8 *wr_buf;
  // ������ ������ ������
  mxifa_sz_t wr_buf_size;
  // ������� � ������ ������
  mxifa_sz_t wr_buf_pos;
  // �������� ������ �������
  irs_bool write_proc;
  // ������ ��������� ������
  mxifa_tick_func_t read_tick;
  // ������ ��������� ������
  mxifa_tick_func_t write_tick;
} mxifa_win32_tcp_ip_t;

// ��������� ������ ������ ���� mxifa_ei_win32_ni_usb_gpib
typedef struct _mxifa_win32_ni_usb_gpib_t {
  // ����� GPIB-����������
  gpib_addr_t address;
  // ������ ����� GPIB
  gpib_addr_t index;
  // ���������� GPIB-����������
  gpib_addr_t handle;
  // ��������� �������������
  irs_bool is_broadcast;
  // ����� ������
  irs_u8 *rd_buf;
  // ������ ������ ������
  mxifa_sz_t rd_buf_size;
  // ������� � ������ ������
  mxifa_sz_t rd_buf_pos;
  // �������� ������ �������
  irs_bool read_proc;
  // ����� ������
  const irs_u8 *wr_buf;
  // ������ ������ ������
  mxifa_sz_t wr_buf_size;
  // ������� � ������ ������
  mxifa_sz_t wr_buf_pos;
  // �������� ������ �������
  irs_bool write_proc;
  // ������ ��������� ������
  mxifa_tick_func_t read_tick;
  // ������ ��������� ������
  mxifa_tick_func_t write_tick;
} mxifa_win32_ni_usb_gpib_t;

// ���������� ��������� ������ ������
typedef struct _mxifa_chdata_t {
  // ��� ����������
  mxifa_ei_t enum_iface;
  // ������ ���������
  mxifa_tick_func_t tick;
  // ������, ���� ���������� �� ������ ����������
  irs_bool mode_free;
  // ����� ������
  irs_bool opened;
  // ������������� ������ ������
  void *ch_spec;
} mxifa_chdata_t;

// ����������� ������ �������
static void mxifa_nulf(void *pchdata);
// ������������� ���������� �������� ������ ��� win32_tcp_ip
static void win32_tcp_ip_read_init(void *pchdata);
// ���������� �������� ������ ��� avr128_ether
static void win32_tcp_ip_read(void *pchdata);
// ���������� �������� ������ ��� avr128_ether
static void win32_tcp_ip_write(void *pchdata);
// ��������� ������
static void win32_tcp_ip_stop(void *pchdata);
// ���������� �������� ������ win32_ni_usb_gpib
static void win32_ni_usb_gpib_read(void *pchdata);
// ���������� �������� ������ win32_ni_usb_gpib
static void win32_ni_usb_gpib_write(void *pchdata);
// ��������� ������ win32_ni_usb_gpib
static void win32_ni_usb_gpib_stop(void *pchdata);
// ����������� ������ ������ �� �����������
static mxifa_ch_t mxifa_get_channel_num(void *pchdata);
// ���������� ��������� ������������ ������
static void mxifa_set_config_in(void *pchdata, void *config);
#ifdef MXIFA_NEW
// ������������� ������ TCP/IP
static void win32_tcp_ip_init(mxifa_ch_t channel_type,
  mxifa_win32_tcp_ip_t *channel_spec_data);
#endif //MXIFA_NEW

// ������� IP
//const mxip_t zero_ip = {{0, 0, 0, 0}};

// ���� ��������� mxnet
#define MXNET_PORT 5005
// Ethernet (MAC) �����
//const irs_u8 local_mac[] = {0x50, 0x01, 0xF1, 0x5C, 0xAF, 0xF7};
// ��������� IP
mxip_t local_ip = {{192, 168, 0, 2}};
// ����������������� IP
//const mxip_t broadcast_ip = {{255, 255, 255, 255}};
// ��������� IP ��� ������ 1
mxip_t dest_ip_1 = {{192, 168, 0, 39}};
// ��������� IP ��� ������ 2
mxip_t dest_ip_2 = {{192, 168, 0, 38}};
// ����� ������� (�� ������������) ������ 1
mxip_t local_mask_1 = {{255, 255, 255, 0}};
// ����� ������� (�� ������������) ������ 2
mxip_t local_mask_2 = {{255, 255, 255, 0}};

// ������ ������ 1 (MXIFA_MXNET)
mxifa_win32_tcp_ip_t mxifa_win32_tcp_ip_1;
// ������ ������ 2 (MXIFA_MXNETC)
mxifa_win32_tcp_ip_t mxifa_win32_tcp_ip_2;
// ������ ������ 3 (MXIFA_SUPPLY)
mxifa_win32_ni_usb_gpib_t mxifa_win32_ni_usb_gpib_3;
// ������ ������ 4 (MXIFA_MULTIMETER)
mxifa_win32_ni_usb_gpib_t mxifa_win32_ni_usb_gpib_4;
// ������ ������ 8 (MXIFA_MXNETC_2)
mxifa_win32_tcp_ip_t mxifa_win32_tcp_ip_8;
// ������ ������ 9 (MXIFA_MXNETC_3)
mxifa_win32_tcp_ip_t mxifa_win32_tcp_ip_9;
// ������ ������ 10 (MXIFA_MXNETC_4)
mxifa_win32_tcp_ip_t mxifa_win32_tcp_ip_10;
// ������ ������ 11 (MXIFA_MXNETC_5)
mxifa_win32_tcp_ip_t mxifa_win32_tcp_ip_11;
// ������ ������ 12 (MXIFA_MXNETC_6)
mxifa_win32_tcp_ip_t mxifa_win32_tcp_ip_12;
#ifdef MXIFA_NEW
// ������ ������ ���� �������
mxifa_chdata_t mxifa_chdata[MXIFA_CNT_CHANNEL];
// ����� ���������� ������
const mxifa_ch_t channel_begin = 1;
// ����� �������� ������
mxifa_ch_t channel_cur = channel_begin;
// ��������� �� ������ ������� ������� � ������� �������
mxifa_chdata_t *mxifa_chdata_begin = &mxifa_chdata[channel_begin];
// ��������� �� ������� ��������� �� ���������
mxifa_chdata_t *mxifa_chdata_end = &mxifa_chdata[MXIFA_CNT_CHANNEL];
// ������� ������������� ������
static irs_u8 count_init = 0;
#else //MXIFA_NEW
// ������ ������� ���������� �� ������ ���� �������
#define MXIFA_CHD_SIZE 5
// ������ ������ ���� �������
mxifa_chdata_t mxifa_chdata[MXIFA_CHD_SIZE];
// ����� �������� ������
mxifa_ch_t channel_cur = 1;
// ��������� �� ������ ������� ������� � ������� �������
mxifa_chdata_t *mxifa_chdata_begin = &mxifa_chdata[1];
// ��������� �� ������� ������� ������� � ������� �������
mxifa_chdata_t *mxifa_chdata_cur = mxifa_chdata_begin;
// ������� ������������� ������
static irs_u8 count_init = 0;
#endif //MXIFA_NEW

#ifdef MXIFA_NEW
// ������������� ������ TCP/IP
static void win32_tcp_ip_init(mxifa_ch_t channel_type,
  mxifa_win32_tcp_ip_t *channel_spec_data)
{
  // �������������� ������������� ������ ������
  // �������� ���������� ��� mxifa_win32_tcp_ip_2
  mxifa_win32_tcp_ip_t &spec_data = *channel_spec_data;
  // ������� ����������� ��������� spec_data
  memset(&spec_data, 0, sizeof(spec_data));
  // ��������� IP
  spec_data.local_ip = local_ip;
  // ��������� ����
  spec_data.local_port = MXNET_PORT;
  // ��������� �����
  spec_data.local_mask = local_mask_2;
  // ��������� IP
  spec_data.dest_ip = dest_ip_2;
  // ��������� ����
  spec_data.dest_port = MXNET_PORT;
  // ��������� �������������
  spec_data.is_broadcast = irs_false;
  // �������� ��������� ������� ��� ������
  spec_data.wr_dest.ip = spec_data.dest_ip;
  spec_data.wr_dest.port = spec_data.dest_port;
  // �������� ��������� ������� ��� ������
  spec_data.rd_dest.ip = spec_data.dest_ip;
  spec_data.rd_dest.port = spec_data.dest_port;
  // �������� ��������� ������� ��� ������ ���������� �������
  spec_data.rd_dest_caller = IRS_NULL;
  // ��������� �� UDP-����� Windows
  spec_data.udp_socket = IRS_NULL;
  // ����� ������
  spec_data.rd_buf = IRS_NULL;
  // ������ ������ ������
  spec_data.rd_buf_size = 0;
  // ������� � ������ ������
  spec_data.rd_buf_pos = 0;
  // �������� ������ �������
  spec_data.read_proc = irs_false;
  // ����� ������
  spec_data.wr_buf = IRS_NULL;
  // ������ ������ ������
  spec_data.wr_buf_size = 0;
  // ������� � ������ ������
  spec_data.wr_buf_pos = 0;
  // �������� ������ �������
  spec_data.write_proc = irs_false;
  // ������ ��������� ������
  spec_data.read_tick = win32_tcp_ip_read_init;
  // ������ ��������� ������
  spec_data.write_tick = win32_tcp_ip_write;

  // �������������� ������ ������
  // �������� ���������� ��� ������ ������
  mxifa_chdata_t &channel = mxifa_chdata[channel_type];
    // ��� ����������
  channel.enum_iface = mxifa_ei_win32_tcp_ip;
  // ������ ���������
  channel.tick = mxifa_nulf;
  // ������, ���� ���������� �� ������ ����������
  channel.mode_free = true;
  // ����� ������
  channel.opened = false;
  // ������������� ������ ������
  channel.ch_spec = &spec_data;
}
#endif //MXIFA_NEW

// ������������� mxifa
void mxifa_init()
{
  if (!count_init) {
    init_to_cnt();

    // ������������� ���� ������� ��� ��������������
    for (
      int channel_index = 0;
      channel_index < MXIFA_CNT_CHANNEL;
      channel_index++
    ) {
      // �������� ���������� ��� ������ ������
      mxifa_chdata_t &channel = mxifa_chdata[channel_index];
      // ��� ����������
      channel.enum_iface = mxifa_ei_unknown;
      // ������ ���������
      channel.tick = mxifa_nulf;
      // ������, ���� ���������� �� ������ ����������
      channel.mode_free = true;
      // ����� ������
      channel.opened = false;
      // ������������� ������ ������
      channel.ch_spec = IRS_NULL;
    }

    // �������������� ������������� ������ ������ 1  (MXIFA_MXNET)
    #ifdef MXIFA_NEW
    win32_tcp_ip_init(MXIFA_MXNET, &mxifa_win32_tcp_ip_1);
    #else //MXIFA_NEW
    // �������� ���������� ��� mxifa_win32_tcp_ip_1
    mxifa_win32_tcp_ip_t &win32_tcp_ip_1 = mxifa_win32_tcp_ip_1;
    // ������� ����������� ��������� win32_tcp_ip_1
    memset(&win32_tcp_ip_1, 0, sizeof(win32_tcp_ip_1));
    // ��������� IP
    win32_tcp_ip_1.local_ip = local_ip;
    // ��������� ����
    win32_tcp_ip_1.local_port = MXNET_PORT;
    // ��������� �����
    win32_tcp_ip_1.local_mask = local_mask_1;
    // ��������� IP
    win32_tcp_ip_1.dest_ip = dest_ip_1;
    // ��������� ����
    win32_tcp_ip_1.dest_port = MXNET_PORT;
    // ��������� �������������
    win32_tcp_ip_1.is_broadcast = irs_false;
    // �������� ��������� ������� ��� ������
    win32_tcp_ip_1.wr_dest.ip = win32_tcp_ip_1.dest_ip;
    win32_tcp_ip_1.wr_dest.port = win32_tcp_ip_1.dest_port;
    // �������� ��������� ������� ��� ������
    win32_tcp_ip_1.rd_dest.ip = win32_tcp_ip_1.dest_ip;
    win32_tcp_ip_1.rd_dest.port = win32_tcp_ip_1.dest_port;
    // �������� ��������� ������� ��� ������ ���������� �������
    win32_tcp_ip_1.rd_dest_caller = IRS_NULL;
    // ��������� �� UDP-����� Windows
    win32_tcp_ip_1.udp_socket = IRS_NULL;
    // ����� ������
    win32_tcp_ip_1.rd_buf = IRS_NULL;
    // ������ ������ ������
    win32_tcp_ip_1.rd_buf_size = 0;
    // ������� � ������ ������
    win32_tcp_ip_1.rd_buf_pos = 0;
    // �������� ������ �������
    win32_tcp_ip_1.read_proc = irs_false;
    // ����� ������
    win32_tcp_ip_1.wr_buf = IRS_NULL;
    // ������ ������ ������
    win32_tcp_ip_1.wr_buf_size = 0;
    // ������� � ������ ������
    win32_tcp_ip_1.wr_buf_pos = 0;
    // �������� ������ �������
    win32_tcp_ip_1.write_proc = irs_false;
    // ������ ��������� ������
    win32_tcp_ip_1.read_tick = win32_tcp_ip_read_init;
    // ������ ��������� ������
    win32_tcp_ip_1.write_tick = win32_tcp_ip_write;

    // �������������� ������ ������ 1 (MXIFA_MXNET)
    // �������� ���������� ��� ������ ������
    mxifa_chdata_t &chdata_1 = mxifa_chdata[1];
      // ��� ����������
    chdata_1.enum_iface = mxifa_ei_win32_tcp_ip;
    // ������ ���������
    chdata_1.tick = mxifa_nulf;
    // ������, ���� ���������� �� ������ ����������
    chdata_1.mode_free = true;
    // ����� ������
    chdata_1.opened = false;
    // ������������� ������ ������
    chdata_1.ch_spec = &win32_tcp_ip_1;
    #endif //MXIFA_NEW

    // �������������� ������������� ������ ������ 2 (MXIFA_MXNETC)
    #ifdef MXIFA_NEW
    win32_tcp_ip_init(MXIFA_MXNETC, &mxifa_win32_tcp_ip_2);
    #else //MXIFA_NEW
    // �������� ���������� ��� mxifa_win32_tcp_ip_2
    mxifa_win32_tcp_ip_t &win32_tcp_ip_2 = mxifa_win32_tcp_ip_2;
    // ������� ����������� ��������� win32_tcp_ip_2
    memset(&win32_tcp_ip_2, 0, sizeof(win32_tcp_ip_2));
    // ��������� IP
    win32_tcp_ip_2.local_ip = local_ip;
    // ��������� ����
    win32_tcp_ip_2.local_port = MXNET_PORT;
    // ��������� �����
    win32_tcp_ip_2.local_mask = local_mask_2;
    // ��������� IP
    win32_tcp_ip_2.dest_ip = dest_ip_2;
    // ��������� ����
    win32_tcp_ip_2.dest_port = MXNET_PORT;
    // ��������� �������������
    win32_tcp_ip_2.is_broadcast = irs_false;
    // �������� ��������� ������� ��� ������
    win32_tcp_ip_2.wr_dest.ip = win32_tcp_ip_2.dest_ip;
    win32_tcp_ip_2.wr_dest.port = win32_tcp_ip_2.dest_port;
    // �������� ��������� ������� ��� ������
    win32_tcp_ip_2.rd_dest.ip = win32_tcp_ip_2.dest_ip;
    win32_tcp_ip_2.rd_dest.port = win32_tcp_ip_2.dest_port;
    // �������� ��������� ������� ��� ������ ���������� �������
    win32_tcp_ip_2.rd_dest_caller = IRS_NULL;
    // ��������� �� UDP-����� Windows
    win32_tcp_ip_2.udp_socket = IRS_NULL;
    // ����� ������
    win32_tcp_ip_2.rd_buf = IRS_NULL;
    // ������ ������ ������
    win32_tcp_ip_2.rd_buf_size = 0;
    // ������� � ������ ������
    win32_tcp_ip_2.rd_buf_pos = 0;
    // �������� ������ �������
    win32_tcp_ip_2.read_proc = irs_false;
    // ����� ������
    win32_tcp_ip_2.wr_buf = IRS_NULL;
    // ������ ������ ������
    win32_tcp_ip_2.wr_buf_size = 0;
    // ������� � ������ ������
    win32_tcp_ip_2.wr_buf_pos = 0;
    // �������� ������ �������
    win32_tcp_ip_2.write_proc = irs_false;
    // ������ ��������� ������
    win32_tcp_ip_2.read_tick = win32_tcp_ip_read_init;
    // ������ ��������� ������
    win32_tcp_ip_2.write_tick = win32_tcp_ip_write;

    // �������������� ������ ������ 2 (MXIFA_MXNETC)
    // �������� ���������� ��� ������ ������
    mxifa_chdata_t &chdata_2 = mxifa_chdata[2];
      // ��� ����������
    chdata_2.enum_iface = mxifa_ei_win32_tcp_ip;
    // ������ ���������
    chdata_2.tick = mxifa_nulf;
    // ������, ���� ���������� �� ������ ����������
    chdata_2.mode_free = true;
    // ����� ������
    chdata_2.opened = false;
    // ������������� ������ ������
    chdata_2.ch_spec = &win32_tcp_ip_2;
    #endif //MXIFA_NEW

    // �������������� ������������� ������ ������ 3 (MXIFA_SUPPLY)
    // �������� ���������� ��� mxifa_win32_ni_usb_gpib_3
    mxifa_win32_ni_usb_gpib_t &win32_ni_usb_gpib_3 = mxifa_win32_ni_usb_gpib_3;
    // ������� ����������� ��������� win32_ni_usb_gpib_3
    memset(&win32_ni_usb_gpib_3, 0, sizeof(win32_ni_usb_gpib_3));
    // ����� GPIB-����������
    win32_ni_usb_gpib_3.address = 0;
    // ������ ����� GPIB
    win32_ni_usb_gpib_3.index = 0;
    // ���������� GPIB-����������
    win32_ni_usb_gpib_3.handle = 0;
    // ��������� �������������
    win32_ni_usb_gpib_3.is_broadcast = irs_false;
    // ����� ������
    win32_ni_usb_gpib_3.rd_buf = IRS_NULL;
    // ������ ������ ������
    win32_ni_usb_gpib_3.rd_buf_size = 0;
    // ������� � ������ ������
    win32_ni_usb_gpib_3.rd_buf_pos = 0;
    // �������� ������ �������
    win32_ni_usb_gpib_3.read_proc = irs_false;
    // ����� ������
    win32_ni_usb_gpib_3.wr_buf = IRS_NULL;
    // ������ ������ ������
    win32_ni_usb_gpib_3.wr_buf_size = 0;
    // ������� � ������ ������
    win32_ni_usb_gpib_3.wr_buf_pos = 0;
    // �������� ������ �������
    win32_ni_usb_gpib_3.write_proc = irs_false;
    // ������ ��������� ������
    win32_ni_usb_gpib_3.read_tick = win32_ni_usb_gpib_read;
    // ������ ��������� ������
    win32_ni_usb_gpib_3.write_tick = win32_ni_usb_gpib_write;

    // �������������� ������ ������ 3 (MXIFA_SUPPLY)
    // �������� ���������� ��� ������ ������
    mxifa_chdata_t &chdata_3 = mxifa_chdata[3];
      // ��� ����������
    chdata_3.enum_iface = mxifa_ei_win32_ni_usb_gpib;
    // ������ ���������
    chdata_3.tick = mxifa_nulf;
    // ������, ���� ���������� �� ������ ����������
    chdata_3.mode_free = true;
    // ����� ������
    chdata_3.opened = false;
    // ������������� ������ ������
    chdata_3.ch_spec = &mxifa_win32_ni_usb_gpib_3;

    // �������������� ������������� ������ ������ 4 (MXIFA_MULTIMETER)
    // �������� ���������� ��� mxifa_win32_ni_usb_gpib_4
    mxifa_win32_ni_usb_gpib_t &win32_ni_usb_gpib_4 = mxifa_win32_ni_usb_gpib_4;
    // ������� ����������� ��������� win32_ni_usb_gpib_4
    memset(&win32_ni_usb_gpib_4, 0, sizeof(win32_ni_usb_gpib_4));
    // ����� GPIB-����������
    win32_ni_usb_gpib_4.address = 0;
    // ������ ����� GPIB
    win32_ni_usb_gpib_4.index = 0;
    // ���������� GPIB-����������
    win32_ni_usb_gpib_4.handle = 0;
    // ��������� �������������
    win32_ni_usb_gpib_4.is_broadcast = irs_false;
    // ����� ������
    win32_ni_usb_gpib_4.rd_buf = IRS_NULL;
    // ������ ������ ������
    win32_ni_usb_gpib_4.rd_buf_size = 0;
    // ������� � ������ ������
    win32_ni_usb_gpib_4.rd_buf_pos = 0;
    // �������� ������ �������
    win32_ni_usb_gpib_4.read_proc = irs_false;
    // ����� ������
    win32_ni_usb_gpib_4.wr_buf = IRS_NULL;
    // ������ ������ ������
    win32_ni_usb_gpib_4.wr_buf_size = 0;
    // ������� � ������ ������
    win32_ni_usb_gpib_4.wr_buf_pos = 0;
    // �������� ������ �������
    win32_ni_usb_gpib_4.write_proc = irs_false;
    // ������ ��������� ������
    win32_ni_usb_gpib_4.read_tick = win32_ni_usb_gpib_read;
    // ������ ��������� ������
    win32_ni_usb_gpib_4.write_tick = win32_ni_usb_gpib_write;

    // �������������� ������ ������ 4 (MXIFA_MULTIMETER)
    // �������� ���������� ��� ������ ������
    mxifa_chdata_t &chdata_4 = mxifa_chdata[4];
      // ��� ����������
    chdata_4.enum_iface = mxifa_ei_win32_ni_usb_gpib;
    // ������ ���������
    chdata_4.tick = mxifa_nulf;
    // ������, ���� ���������� �� ������ ����������
    chdata_4.mode_free = true;
    // ����� ������
    chdata_4.opened = false;
    // ������������� ������ ������
    chdata_4.ch_spec = &mxifa_win32_ni_usb_gpib_4;

    #ifdef MXIFA_NEW
    // �������������� ������������� ������ ������ 8 (MXIFA_MXNETC_2)
    win32_tcp_ip_init(MXIFA_MXNETC_2, &mxifa_win32_tcp_ip_8);
    // �������������� ������������� ������ ������ 9 (MXIFA_MXNETC_3)
    win32_tcp_ip_init(MXIFA_MXNETC_3, &mxifa_win32_tcp_ip_9);
    // �������������� ������������� ������ ������ 10 (MXIFA_MXNETC_4)
    win32_tcp_ip_init(MXIFA_MXNETC_4, &mxifa_win32_tcp_ip_10);
    // �������������� ������������� ������ ������ 11 (MXIFA_MXNETC_5)
    win32_tcp_ip_init(MXIFA_MXNETC_5, &mxifa_win32_tcp_ip_11);
    // �������������� ������������� ������ ������ 12 (MXIFA_MXNETC_6)
    win32_tcp_ip_init(MXIFA_MXNETC_6, &mxifa_win32_tcp_ip_12);
    #endif //MXIFA_NEW
  }
  count_init++;
}
// ��������������� mxifa
void mxifa_deinit()
{
  count_init--;
  if (!count_init) {

    #ifdef MXIFA_NEW
    for (mxifa_ch_t ch_ind = 1; ch_ind < MXIFA_CNT_CHANNEL; ch_ind++) {
    #else //MXIFA_NEW
    for (mxifa_ch_t ch_ind = 1; ch_ind < MXIFA_CHD_SIZE; ch_ind++) {
    #endif //MXIFA_NEW
      mxifa_chdata_t &chdata = mxifa_chdata[ch_ind];
      if (chdata.opened) {
        mxifa_close_begin(&chdata);
        mxifa_close_end(&chdata, irs_true);
      }
    }

    deinit_to_cnt();
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ������ ������ ������
#define udp_socket_recv_buf_size 65535
// ����� ��� ������ � UDP-�������� Windows
class udp_socket_t//: public TObject
{
private:
  // �������� ����� ������
  class read_buf_ring_t
  {
    // ��� �������� ��������� ������ ������
    typedef struct _read_buf_ring_elem_t {
      mxip_t ip;
      irs_u16 port;
      mxifa_sz_t size;
      irs_u8 *buf;
    } read_buf_ring_elem_t;
    // ��������� �� ������� ������� ���������� ������ ������
    read_buf_ring_elem_t *f_read_buf_ring_elem_cur;
    // ��������� �� ��������� ������� ���������� ������ ������
    read_buf_ring_elem_t *f_rbre_remove;
    // ������ ���������� ������ ������
    irs_u32 f_read_buf_ring_size;
    // ��������� ����� ������
    alg_ring f_read_buf_ring;
    // �������� ���������� ��������
    void clear_rbre(read_buf_ring_elem_t *&rbre)
    {
      if (rbre) {
        if (rbre->buf) {
          delete []rbre->buf;
          rbre->buf = IRS_NULL;
        }
        delete rbre;
        rbre = IRS_NULL;
      }
    }
  public:
    read_buf_ring_t():
      f_read_buf_ring_elem_cur(IRS_NULL),
      f_rbre_remove(IRS_NULL),
      f_read_buf_ring_size(10),
      f_read_buf_ring(f_read_buf_ring_size)
    {
    }
    ~read_buf_ring_t()
    {
      clear_rbre(f_rbre_remove);
      for (;;) {
        f_read_buf_ring_elem_cur = (read_buf_ring_elem_t *)
          f_read_buf_ring.read_and_remove_first();
        if (!f_read_buf_ring_elem_cur) break;
        clear_rbre(f_read_buf_ring_elem_cur);
      }
    }
    void *add_last(mxip_t ip, irs_u16 port, mxifa_sz_t size)
    {
      //clear_rbre(f_rbre_remove);
      f_read_buf_ring_elem_cur = new read_buf_ring_elem_t;
      if (f_read_buf_ring_elem_cur) {
        f_read_buf_ring_elem_cur->ip = ip;
        f_read_buf_ring_elem_cur->port = port;
        f_read_buf_ring_elem_cur->size = size;
        f_read_buf_ring_elem_cur->buf = new irs_u8[size];
        if (f_read_buf_ring_elem_cur->buf) {
          read_buf_ring_elem_t *rem_ring_elem = (read_buf_ring_elem_t *)
            f_read_buf_ring.add_last_and_get_removed(f_read_buf_ring_elem_cur);
          clear_rbre(rem_ring_elem);
        } else {
          delete f_read_buf_ring_elem_cur;
          return IRS_NULL;
        }
        return f_read_buf_ring_elem_cur->buf;
      } else {
        return IRS_NULL;
      }
    }
    void *read_first(mxip_t &ip, irs_u16 &port, mxifa_sz_t &size)
    {
      clear_rbre(f_rbre_remove);
      void *buf = IRS_NULL;
      size = 0;
      const mxip_t zero_ip = {{0, 0, 0, 0}};
      ip = zero_ip;
      f_read_buf_ring_elem_cur = (read_buf_ring_elem_t *)
        f_read_buf_ring.read_and_remove_first();
      if (f_read_buf_ring_elem_cur) {
        buf = f_read_buf_ring_elem_cur->buf;
        size = f_read_buf_ring_elem_cur->size;
        ip = f_read_buf_ring_elem_cur->ip;
        port = f_read_buf_ring_elem_cur->port;
        f_rbre_remove = f_read_buf_ring_elem_cur;
      }
      return buf;
    }
  } f_read_buf_ring;
  // ��������� VCL ��� ������ � UDP
  //TNMUDP *f_UDPS;
  // ������ � ������������
  irs_bool f_create_error;
  // ����� ������
  irs_u8 *f_read_buf;
  // IP ��������������� ������ ������
  mxip_t f_read_ip;
  // ������� ������ ������ ������
  mxifa_sz_t f_read_buf_size;
  // ������� ������� � ������ ������
  mxifa_sz_t f_read_buf_pos;
  // ����� ������
  irs_u8 recv_buf[udp_socket_recv_buf_size];
  // ���������� ������
  SOCKET sockfd;
  // ����� �������� ������� select
  struct timeval tv;
  // ������ ������� select
  fd_set readfds;
  // ��������� ���� ��������� ��������� ������
  irs_u16 read_port;
  // ������� ��������� ����
  //irs_u16 f_local_port;

  void open_socket(irs_u16 port)
  {
    //f_local_port = port;

    for (;;) {
      sockfd = socket(PF_INET, SOCK_DGRAM, 0);
      if (sockfd == INVALID_SOCKET) {
        f_create_error = irs_true;
        break;
      }

      struct sockaddr_in my_addr; // my address information
      my_addr.sin_family = AF_INET; // host byte order
      my_addr.sin_port = htons(port); // short, network byte order
      my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
      memset(&(my_addr.sin_zero), 0, 8); // zero the rest of the struct
      //int bind_res =
      bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
      #ifdef NOP
      if (bind_res == SOCKET_ERROR) {
        f_create_error = irs_true;
        break;
      }
      #endif //NOP

      break;
    }
    //FD_SET(sockfd, &readfds);
  }
public:
  // �����������
  udp_socket_t(irs_u16 port)
  {
    f_create_error = irs_false;
    f_read_buf = IRS_NULL;
    f_read_ip = zero_ip;
    f_read_buf_size = 0;
    f_read_buf_pos = 0;
    memset(recv_buf, 0, udp_socket_recv_buf_size);
    sockfd = 0;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    read_port = 0;
    //f_local_port = 0;

    const WORD ws_ver = 0x0102;
    WSADATA wsaData;
    if (WSAStartup(ws_ver, &wsaData)) {
      f_create_error = irs_true;
    }

    open_socket(port);
  }
  // ����������
  __fastcall ~udp_socket_t()
  {
    //if (f_UDPS) delete f_UDPS;
    if (f_read_buf) delete []f_read_buf;
    closesocket(sockfd);
    WSACleanup();
  }
  // ��������� �����
  void set_local_port(irs_u16 port)
  {
    closesocket(sockfd);
    open_socket(port);
  }
  // ������������ ��������
  void tick()
  {
    if (f_create_error) return;
    FD_SET(sockfd, &readfds);
    int sel_res = select(sockfd + 1, &readfds, NULL, NULL, &tv);
    #ifndef NOP
    if (sel_res == SOCKET_ERROR) {
      switch (WSAGetLastError()) {
        case WSANOTINITIALISED: {
          sel_res++;
        } break;
        case WSAEFAULT: {
          sel_res++;
        } break;
        case WSAENETDOWN: {
          sel_res++;
        } break;
        case WSAEINVAL: {//*********
          sel_res++;
        } break;
        case WSAEINTR: {
          sel_res++;
        } break;
        case WSAEINPROGRESS: {
          sel_res++;
        } break;
        case WSAENOTSOCK: {
          sel_res++;
        } break;
      }
    }
    #endif //NOP
    if (FD_ISSET(sockfd, &readfds)) {
      struct sockaddr_in their_addr; // connector�s address information
      int addr_len = sizeof(their_addr);
      int numbytes = recvfrom(sockfd, (char *)recv_buf,
        udp_socket_recv_buf_size, 0, (struct sockaddr *)&their_addr, &addr_len);
      if (numbytes != SOCKET_ERROR) {
        mxip_t ip = *(mxip_t *)&their_addr.sin_addr.s_addr;
        irs_u16 port = ntohs(their_addr.sin_port);
        void *buf = f_read_buf_ring.add_last(ip, port, numbytes);
        memcpy(buf, recv_buf, numbytes);

        #ifdef NOP
        if (dbg_write) {
          dbg_write = irs_false;
          irs_i32 *buf32 = (irs_i32 *)buf;
          irs_u32 i = (irs_u32)buf32;
          i++;
        }
        #endif //NOP

      }
    }
  }
  // ������ � �����
  mxifa_sz_t write(mxip_t ip, irs_u16 port, const irs_u8 *buf, mxifa_sz_t size)
  {
    struct sockaddr_in their_addr;
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    their_addr.sin_addr.s_addr = *(unsigned long *)&ip;
    memset(&(their_addr.sin_zero), 0, 8);
    mxifa_sz_t bytes_send = sendto(sockfd, (char *)buf, (int )size, 0,
      (struct sockaddr *)&their_addr, sizeof(their_addr));

    //irs_u16 local_port = f_local_port;
    //local_port++;

    return bytes_send;
  }
  // ����������������� ������ � �����
  mxifa_sz_t write_broadcast(irs_u16 port, const irs_u8 *buf, mxifa_sz_t size)
  {
    struct sockaddr_in their_addr;
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    mxip_t broadcast_ip = {{255, 255, 255, 255}};
    their_addr.sin_addr.s_addr = *(unsigned long *)&broadcast_ip;
    memset(&(their_addr.sin_zero), 0, 8);
    mxifa_sz_t bytes_send = sendto(sockfd, (char *)buf, (int )size, 0,
      (struct sockaddr *)&their_addr, sizeof(their_addr));

    return size;
  }
  // ������ �� ������
  mxifa_sz_t read(mxip_t &ip, irs_u8 *buf, mxifa_sz_t size)
  {
    if (f_create_error) return 0;
    //const mxip_t zero_ip = {{0, 0, 0, 0}};
    ip = zero_ip;
    mxifa_sz_t size_returned = 0;
    if (f_read_buf) {
      mxifa_sz_t read_buf_size_cur = f_read_buf_size - f_read_buf_pos;
      ip = f_read_ip;
      if (read_buf_size_cur > size) {
        size_returned = size;
        memcpy((void *)buf, (void *)(f_read_buf + f_read_buf_pos),
          (size_t)size_returned);
        f_read_buf_pos += size_returned;
      } else {
        size_returned = read_buf_size_cur;
        memcpy((void *)buf, (void *)(f_read_buf + f_read_buf_pos),
          (size_t)size_returned);
        f_read_ip = zero_ip;
        f_read_buf_size = 0;
        f_read_buf_pos = 0;
        delete []f_read_buf;
        f_read_buf = IRS_NULL;
      }
    } else {
      mxifa_sz_t size_int = 0;
      void *buf_int = f_read_buf_ring.read_first(ip, read_port, size_int);
      if (buf_int) {
        if (size_int > size) {
          size_returned = size;
          memcpy((void *)buf, buf_int, size_returned);
          f_read_buf_size = size_int - size_returned;
          f_read_buf = new irs_u8[f_read_buf_size];
          f_read_ip = ip;
          f_read_buf_pos = 0;
          memcpy(f_read_buf, ((irs_u8 *)buf_int) + size_returned,
            f_read_buf_size);
        } else {
          size_returned = size_int;
          memcpy((void *)buf, buf_int, size_returned);
        }
      } else {
        size_returned = 0;
      }
    }
    return size_returned;
  }
  // �������� �� ����������� ������ ����� ��������� �������
  irs_bool read_port_available()
  {
    if (f_create_error) return irs_false;
    return irs_true;
  }
  // ������ ����� ��������� �������
  irs_u16 get_read_port()
  {
    if (f_create_error) return 0;
    return read_port;
  }
  // ������ ��� �������� ������
  irs_bool create_error()
  {
    return f_create_error;
  }
};
//---------------------------------------------------------------------------
// ������������ ��������
void mxifa_tick()
{
  #ifdef MXIFA_NEW
  for (
    mxifa_chdata_t *mxifa_chdata_cur = mxifa_chdata_begin;
    mxifa_chdata_cur < mxifa_chdata_end;
    mxifa_chdata_cur++
  ) {
  #endif //MXIFA_NEW
    switch (mxifa_chdata_cur->enum_iface) {
      case mxifa_ei_win32_tcp_ip: {
        mxifa_win32_tcp_ip_t *win32_tcp_ip =
          (mxifa_win32_tcp_ip_t *)mxifa_chdata_cur->ch_spec;
        win32_tcp_ip->read_tick(mxifa_chdata_cur);
        win32_tcp_ip->write_tick(mxifa_chdata_cur);
      } break;
      case mxifa_ei_win32_ni_usb_gpib: {
        mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
          (mxifa_win32_ni_usb_gpib_t *)mxifa_chdata_cur->ch_spec;
        win32_ni_usb_gpib->read_tick(mxifa_chdata_cur);
        win32_ni_usb_gpib->write_tick(mxifa_chdata_cur);
      } break;
    }
  #ifdef MXIFA_NEW
  }
  #endif //MXIFA_NEW
  
  #ifndef MXIFA_NEW
  #if (MXIFA_CHD_SIZE > 2)
  if (channel_cur >= MXIFA_CHD_SIZE - 1) {
    channel_cur = 1;
    mxifa_chdata_cur = mxifa_chdata_begin;
  } else {
    channel_cur++;
    mxifa_chdata_cur++;
  }
  #endif //(MXIFA_CHD_SIZE > 2)
  #endif //MXIFA_NEW
}
// ����������� �������� channel
void *mxifa_open(mxifa_ch_t channel, irs_bool is_broadcast)
{
  void *pchdata = mxifa_open_begin(channel, is_broadcast);
  counter_t var_timeout_channel_open;
  set_to_cnt(var_timeout_channel_open, TIMEOUT_CHANNEL);
  for (;;) {
    if (mxifa_open_end(pchdata, irs_false)) return pchdata;
    if (test_to_cnt(var_timeout_channel_open)) {
      mxifa_open_end(pchdata, irs_true);
      break;
    }
  }
  return IRS_NULL;
}
// ������������� �������� channel
void *mxifa_open_begin(mxifa_ch_t channel, irs_bool is_broadcast)
{
  #ifdef MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CNT_CHANNEL)) return irs_false;
  #else //MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return irs_false;
  #endif //MXIFA_NEW
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  switch (cur_ch->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)cur_ch->ch_spec;
      win32_tcp_ip->udp_socket = new udp_socket_t(win32_tcp_ip->local_port);
      if (win32_tcp_ip->udp_socket) {
        win32_tcp_ip->is_broadcast = is_broadcast;
        cur_ch->opened = irs_true;
      } else {
        cur_ch->opened = irs_false;
      }
    } break;
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)cur_ch->ch_spec;
      ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
      ni_usb_gpib->init();
      win32_ni_usb_gpib->handle = ni_usb_gpib->ibdev(win32_ni_usb_gpib->index,
        win32_ni_usb_gpib->address, NO_SECONDARY_ADDR, TIMEOUT, EOTMODE,
        EOSMODE);
      if (win32_ni_usb_gpib->handle != -1) {
        cur_ch->opened = irs_true;
        ni_usb_gpib->ibonl(win32_ni_usb_gpib->handle, irs_true);
        ni_usb_gpib->ibclr(win32_ni_usb_gpib->handle);
        //ni_usb_gpib->ibconfig(win32_ni_usb_gpib->handle, IbcEOSrd, 1);
        win32_ni_usb_gpib->is_broadcast = is_broadcast;
      }
    } break;
  }
  return cur_ch;
}
// �������� ��������� �������� channel
irs_bool mxifa_open_end(void *pchdata, bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool opened = irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      abort = abort;
      opened = pchdatas->opened;
    } break;
    case mxifa_ei_win32_ni_usb_gpib: {
      opened = pchdatas->opened;
    } break;
  }
  return opened;
}
// ����������� �������� channel c ����������������
void *mxifa_open_ex(mxifa_ch_t channel, void *config, irs_bool is_broadcast)
{
  #ifdef MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CNT_CHANNEL)) return irs_false;
  #else //MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return irs_false;
  #endif //MXIFA_NEW
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  mxifa_set_config_in(cur_ch, config);
  return mxifa_open(channel, is_broadcast);
}
// ������������� �������� channel � �����������������
void *mxifa_open_begin_ex(mxifa_ch_t channel, void *config,
  irs_bool is_broadcast)
{
  #ifdef MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CNT_CHANNEL)) return irs_false;
  #else //MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return irs_false;
  #endif //MXIFA_NEW
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  mxifa_set_config_in(cur_ch, config);
  return mxifa_open_begin(channel, is_broadcast);
}
// �������� ��������� �������� channel � ����������������
irs_bool mxifa_open_end_ex(void *pchdata, bool abort)
{
  return mxifa_open_end(pchdata, abort);
}
// ����������� �������� channel
irs_bool mxifa_close(void *pchdata)
{
  if (pchdata == IRS_NULL) return irs_false;
  if (mxifa_close_begin(pchdata)) return irs_true;
  counter_t var_timeout_channel_close;
  set_to_cnt(var_timeout_channel_close, TIMEOUT_CHANNEL);
  for (;;) {
    if (mxifa_close_end(pchdata, irs_false)) return irs_true;
    if (test_to_cnt(var_timeout_channel_close)) {
      mxifa_close_end(pchdata, irs_true);
      break;
    }
  }
  return irs_false;
}
// ��������� ������ win32_tcp_ip
static void win32_tcp_ip_stop(void *pchdata)
{
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  if (pchdatas->opened) {
    mxifa_win32_tcp_ip_t *win32_tcp_ip =
      (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
    if (win32_tcp_ip->udp_socket) delete win32_tcp_ip->udp_socket;
    win32_tcp_ip->udp_socket = IRS_NULL;
    pchdatas->opened = irs_false;
    win32_tcp_ip->read_proc = irs_false;
    win32_tcp_ip->write_proc = irs_false;
  }
}
// ��������� ������ win32_ni_usb_gpib
static void win32_ni_usb_gpib_stop(void *pchdata)
{
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  if (pchdatas->opened) {
    pchdatas->opened = irs_false;
    mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
      (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
    win32_ni_usb_gpib->read_proc = irs_false;
    win32_ni_usb_gpib->write_proc = irs_false;
    ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
    // ������� ���������� � offline
    ni_usb_gpib->ibonl(win32_ni_usb_gpib->handle, irs_false);
    ni_usb_gpib->deinit();
  }
}
// ������������� �������� channel
irs_bool mxifa_close_begin(void *pchdata)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool closed = irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      if (pchdatas->mode_free) {
        win32_tcp_ip_stop(pchdata);
      }
    } break;
    case mxifa_ei_win32_ni_usb_gpib: {
      if (pchdatas->mode_free) {
        win32_ni_usb_gpib_stop(pchdata);
      }
    } break;
  }
  return closed;
}
// �������� ��������� �������� channel
irs_bool mxifa_close_end(void *pchdata, bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool closed = irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      if (pchdatas->mode_free || abort) {
        win32_tcp_ip_stop(pchdata);
      }
      closed = !pchdatas->opened;
    } break;
    case mxifa_ei_win32_ni_usb_gpib: {
      if (pchdatas->mode_free || abort) {
        win32_ni_usb_gpib_stop(pchdata);
      }
      closed = !pchdatas->opened;
    } break;
  }
  return closed;
}
// ������ ������ � ��������� ��� ��������
irs_bool mxifa_write_begin(void *pchdata, mxifa_dest_t *dest,
  const irs_u8 *buf, mxifa_sz_t size)
{
  if (pchdata == IRS_NULL) return irs_false;
  if (buf == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
      // ����� ������
      win32_tcp_ip->wr_buf = buf;
      // ������ ������ ������
      win32_tcp_ip->wr_buf_size = size;
      // ������� � ������ ������
      win32_tcp_ip->wr_buf_pos = 0;
      // �������� ��������� ������� ��� ������
      if (dest) {
        win32_tcp_ip->wr_dest = dest->win32_tcp_ip;
      } else {
        win32_tcp_ip->wr_dest.ip = win32_tcp_ip->dest_ip;
        win32_tcp_ip->wr_dest.port = win32_tcp_ip->dest_port;
      }

      // �������� ������ �������
      win32_tcp_ip->write_proc = irs_true;
      // ������������� � ���������
      pchdatas->mode_free = irs_false;
    } return irs_true;
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      // ����� ������
      win32_ni_usb_gpib->wr_buf = buf;
      // ������ ������ ������
      win32_ni_usb_gpib->wr_buf_size = size;
      // ������� � ������ ������
      win32_ni_usb_gpib->wr_buf_pos = 0;

      // ��������� ������������� ��������
      win32_ni_usb_gpib->write_tick = win32_ni_usb_gpib_write;
      // �������� ������ �������
      win32_ni_usb_gpib->write_proc = irs_true;
      // ������������� � ���������
      pchdatas->mode_free = irs_false;

      gpib_addr_t handle = win32_ni_usb_gpib->handle;
      ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
      ni_usb_gpib->ibclr(handle);
    } break;
  }
  return irs_false;
}
// �������� ��������� ������
irs_bool mxifa_write_end(void *pchdata, irs_bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
      if (abort) {
        win32_tcp_ip->write_proc = irs_false;
        pchdatas->mode_free = ((!win32_tcp_ip->read_proc) &&
          (!win32_tcp_ip->write_proc));
      }
      return !win32_tcp_ip->write_proc;
    }
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      if (abort) {
        win32_ni_usb_gpib->write_proc = irs_false;
        pchdatas->mode_free = ((!win32_ni_usb_gpib->read_proc) &&
          (!win32_ni_usb_gpib->write_proc));
        // ��������� ������������� ��������
        win32_ni_usb_gpib->write_tick = win32_ni_usb_gpib_write;

        // ����� GPIB
        ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
        ni_usb_gpib->ibclr(win32_ni_usb_gpib->handle);
      }
      return !win32_ni_usb_gpib->write_proc;
    }
  }
  return irs_false;
}
// ������ �������� ������ �� ���������� � �����
irs_bool mxifa_read_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size)
{
  if (pchdata == IRS_NULL) return irs_false;
  if (buf == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
      // ����� ������
      win32_tcp_ip->rd_buf = buf;
      // ������ ������ ������
      win32_tcp_ip->rd_buf_size = size;
      // ������� � ������ ������
      win32_tcp_ip->rd_buf_pos = 0;
      // ��������� ������
      memset(win32_tcp_ip->rd_buf, 0, win32_tcp_ip->rd_buf_size);
      // �������� ��������� ������� ��� ������
      win32_tcp_ip->rd_dest_caller = dest;
      win32_tcp_ip->rd_dest.ip = zero_ip;
      win32_tcp_ip->rd_dest.port = 0;
      if (dest) {
        dest->win32_tcp_ip = win32_tcp_ip->rd_dest;
      }

      win32_tcp_ip->read_tick = win32_tcp_ip_read_init;
      // �������� ������ �������
      win32_tcp_ip->read_proc = irs_true;
      // ������������� � ���������
      pchdatas->mode_free = irs_false;
    } return irs_true;
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      // ����� ������
      win32_ni_usb_gpib->rd_buf = buf;
      // ������ ������ ������
      win32_ni_usb_gpib->rd_buf_size = size;
      // ������� � ������ ������
      win32_ni_usb_gpib->rd_buf_pos = 0;
      // ��������� ������
      memset(win32_ni_usb_gpib->rd_buf, 0, win32_ni_usb_gpib->rd_buf_size);
      if (dest) {
        // �������� ��������� ������� ��� ������
        dest->win32_ni_usb_gpib.address = win32_ni_usb_gpib->address;
      }

      // ��������� ������������� ��������
      win32_ni_usb_gpib->read_tick = win32_ni_usb_gpib_read;
      // �������� ������ �������
      win32_ni_usb_gpib->read_proc = irs_true;
      // ������������� � ���������
      pchdatas->mode_free = irs_false;
    } break;
  }
  return irs_false;
}
// �������� ��������� ������
irs_bool mxifa_read_end(void *pchdata, irs_bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
      if (abort) {
        win32_tcp_ip->read_proc = irs_false;
        pchdatas->mode_free = ((!win32_tcp_ip->read_proc) &&
          (!win32_tcp_ip->write_proc));
        win32_tcp_ip->read_tick = win32_tcp_ip_read_init;
      }
      return !win32_tcp_ip->read_proc;
    }
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      if (abort) {
        win32_ni_usb_gpib->read_proc = irs_false;
        pchdatas->mode_free = ((!win32_ni_usb_gpib->read_proc) &&
          (!win32_ni_usb_gpib->write_proc));
        // ��������� ������������� ��������
        win32_ni_usb_gpib->read_tick = win32_ni_usb_gpib_read;

        // ����� GPIB
        ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
        ni_usb_gpib->ibclr(win32_ni_usb_gpib->handle);
      }
      return !win32_ni_usb_gpib->read_proc;
    }
  }
  return irs_false;
}
// ������ �������� ������ ����������
mxifa_sz_t mxifa_fast_read(void *pchdata, mxifa_dest_t *dest, irs_u8 *buf,
  mxifa_sz_t size)
{
  if (pchdata == IRS_NULL) return irs_false;
  if (buf == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
      udp_socket_t *udp_socket = win32_tcp_ip->udp_socket;
      win32_tcp_ip->rd_dest.ip = zero_ip;
      win32_tcp_ip->rd_dest.port = 0;
      mxip_t &ip = win32_tcp_ip->rd_dest.ip;
      irs_u8 *buf = win32_tcp_ip->rd_buf;
      mxifa_sz_t size = win32_tcp_ip->rd_buf_size;
      mxifa_sz_t read_bytes = udp_socket->read(ip, buf, size);
      if (read_bytes > 0) {
        if (udp_socket->read_port_available()) {
          win32_tcp_ip->rd_dest.port = udp_socket->get_read_port();
        } else {
          win32_tcp_ip->rd_dest.port = win32_tcp_ip->dest_port;
        }
      }
      if (dest) {
        dest->win32_tcp_ip = win32_tcp_ip->rd_dest;
      }
      return read_bytes;
    }
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
      gpib_addr_t handle = win32_ni_usb_gpib->handle;
      ni_usb_gpib->ibrd(handle, (void *)buf, size);
      return ni_usb_gpib->get_ibcntl();
    } //break;
  }
  return 0;
}
// ������ ���� ������ �� ������
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel)
{
  #ifdef MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CNT_CHANNEL)) return mxifa_ei_unknown;
  #else //MXIFA_NEW
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return mxifa_ei_unknown;
  #endif //MXIFA_NEW
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  return mxifa_get_channel_type(cur_ch);
}
// ������ ���� ������
mxifa_ei_t mxifa_get_channel_type(void *pchdata)
{
  if (pchdata == IRS_NULL) return mxifa_ei_unknown;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  return pchdatas->enum_iface;
}
// ����������� ������ ������ �� �����������
static mxifa_ch_t mxifa_get_channel_num(void *pchdata)
{
  irs_i32 ch_num = (((mxifa_chdata_t *)pchdata) - &mxifa_chdata[0]);
  return ch_num;
}
// ���������� ��������� ������������ ������
static void mxifa_set_config_in(void *pchdata, void *config)
{
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
      mxifa_win32_tcp_ip_cfg *cfg = (mxifa_win32_tcp_ip_cfg *)config;
      win32_tcp_ip->dest_ip = cfg->dest_ip;
      win32_tcp_ip->dest_port = cfg->dest_port;
      win32_tcp_ip->local_port = cfg->local_port;
      win32_tcp_ip->udp_socket->set_local_port(cfg->local_port);
    } break;
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      mxifa_win32_ni_usb_gpib_cfg *cfg = (mxifa_win32_ni_usb_gpib_cfg *)config;
      win32_ni_usb_gpib->address = cfg->address;
    } break;
  }
}
// ��������� ������������ ������
void mxifa_set_config(void *pchdata, void *config)
{
  if (pchdata == IRS_NULL) return;
  if (config == IRS_NULL) return;

  mxifa_set_config_in(pchdata, config);

  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      // ��� ��������������� � ������� mxifa_set_config_in
    } break;
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      if (pchdatas->opened) {
        mxifa_close_begin(pchdata);
        mxifa_close_end(pchdata, irs_true);
      }
      mxifa_open_begin(mxifa_get_channel_num(pchdata),
        win32_ni_usb_gpib->is_broadcast);
      mxifa_open_end(pchdata, irs_true);
    } break;
  }
}
// ������ ������������ ������
void mxifa_get_config(void *pchdata, void *config)
{
  if (pchdata == IRS_NULL) return;
  if (config == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_win32_tcp_ip: {
      mxifa_win32_tcp_ip_t *win32_tcp_ip =
        (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;
      mxifa_win32_tcp_ip_cfg *cfg = (mxifa_win32_tcp_ip_cfg *)config;
      cfg->dest_ip = win32_tcp_ip->dest_ip;
      cfg->dest_port = win32_tcp_ip->dest_port;
      cfg->local_port = win32_tcp_ip->local_port;
    } break;
    case mxifa_ei_win32_ni_usb_gpib: {
      mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
        (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;
      mxifa_win32_ni_usb_gpib_cfg *cfg = (mxifa_win32_ni_usb_gpib_cfg *)config;
      cfg->address = win32_ni_usb_gpib->address;
      cfg->bitcfg = 0;
      cfg->read_count = win32_ni_usb_gpib->rd_buf_pos;
    } break;
  }
}
//---------------------------------------------------------------------------
// �����

// ����������� ������ �������
static void mxifa_nulf(void *pchdata)
{
}
//---------------------------------------------------------------------------
// win32_tcp_ip

// ������������� ���������� �������� ������ ��� win32_tcp_ip
static void win32_tcp_ip_read_init(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_win32_tcp_ip_t *win32_tcp_ip =
    (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;

  if (win32_tcp_ip->read_proc) {
    udp_socket_t *udp_socket = win32_tcp_ip->udp_socket;

    udp_socket->tick();

    mxifa_win32_tcp_ip_dest_t &dest = win32_tcp_ip->rd_dest;
    irs_u8 *buf = win32_tcp_ip->rd_buf;
    mxifa_sz_t size = win32_tcp_ip->rd_buf_size;
    mxifa_sz_t read_bytes = udp_socket->read(dest.ip, buf, size);
    if (read_bytes > 0) {
      if (udp_socket->read_port_available()) {
        dest.port = udp_socket->get_read_port();
      } else {
        dest.port = win32_tcp_ip->dest_port;
      }
      if (read_bytes < win32_tcp_ip->rd_buf_size) {
        win32_tcp_ip->rd_buf_pos += read_bytes;
        win32_tcp_ip->read_tick = win32_tcp_ip_read;
      } else {
        win32_tcp_ip->rd_buf_pos = win32_tcp_ip->rd_buf_size;
        if (win32_tcp_ip->rd_dest_caller) {
          win32_tcp_ip->rd_dest_caller->win32_tcp_ip = dest;
        }
        win32_tcp_ip->read_proc = irs_false;
        pchdatas->mode_free = ((!win32_tcp_ip->read_proc) &&
          (!win32_tcp_ip->write_proc));
      }
    }
  }
}
// ���������� �������� ������ ��� win32_tcp_ip
static void win32_tcp_ip_read(void *pchdata)
{
  if (pchdata == IRS_NULL) return;

  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_win32_tcp_ip_t *win32_tcp_ip =
    (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;

  if (win32_tcp_ip->read_proc) {
    udp_socket_t *udp_socket = win32_tcp_ip->udp_socket;

    udp_socket->tick();

    mxip_t ip = zero_ip;
    irs_u8 *buf_rest = win32_tcp_ip->rd_buf + win32_tcp_ip->rd_buf_pos;
    mxifa_sz_t size_rest = win32_tcp_ip->rd_buf_size -
      win32_tcp_ip->rd_buf_pos;
    mxifa_sz_t read_bytes = udp_socket->read(ip, buf_rest, size_rest);

    if (read_bytes > 0) {
      irs_u16 port = 7;
      if (udp_socket->read_port_available()) {
        port = udp_socket->get_read_port();
      } else {
        port = win32_tcp_ip->dest_port;
      }
      mxifa_win32_tcp_ip_dest_t &dest = win32_tcp_ip->rd_dest;
      if ((ip == dest.ip) && (port == dest.port)) {
        if (read_bytes < size_rest) {
          win32_tcp_ip->rd_buf_pos += read_bytes;
        } else {
          win32_tcp_ip->rd_buf_pos = win32_tcp_ip->rd_buf_size;
          if (win32_tcp_ip->rd_dest_caller) {
            win32_tcp_ip->rd_dest_caller->win32_tcp_ip = dest;
          }
          win32_tcp_ip->read_proc = irs_false;
          pchdatas->mode_free = ((!win32_tcp_ip->read_proc) &&
            (!win32_tcp_ip->write_proc));
          win32_tcp_ip->read_tick = win32_tcp_ip_read_init;
        }
      }
    }
  }
}
// ���������� �������� ������ ��� win32_tcp_ip
static void win32_tcp_ip_write(void *pchdata)
{
  if (pchdata == IRS_NULL) return;

  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_win32_tcp_ip_t *win32_tcp_ip =
    (mxifa_win32_tcp_ip_t *)pchdatas->ch_spec;

  if (win32_tcp_ip->write_proc) {
    udp_socket_t *udp_socket = win32_tcp_ip->udp_socket;

    udp_socket->tick();

    mxip_t ip = win32_tcp_ip->wr_dest.ip;
    irs_u16 port = win32_tcp_ip->wr_dest.port;
    const irs_u8 *buf = win32_tcp_ip->wr_buf + win32_tcp_ip->wr_buf_pos;
    mxifa_sz_t size = win32_tcp_ip->wr_buf_size - win32_tcp_ip->wr_buf_pos;
    mxifa_sz_t writed_bytes = 0;

    if (win32_tcp_ip->is_broadcast) {
      writed_bytes = udp_socket->write_broadcast(port, buf, size);
    } else {
      writed_bytes = udp_socket->write(ip, port, buf, size);
    }
    win32_tcp_ip->wr_buf_pos += writed_bytes;

    if (win32_tcp_ip->wr_buf_pos >= win32_tcp_ip->wr_buf_size) {
      win32_tcp_ip->write_proc = irs_false;
      pchdatas->mode_free = ((!win32_tcp_ip->read_proc) &&
        (!win32_tcp_ip->write_proc));
    }
  }
}
//---------------------------------------------------------------------------
// win32_ni_usb_gpib

// ���������� �������� ������ win32_ni_usb_gpib
static void win32_ni_usb_gpib_read(void *pchdata)
{
  if (pchdata == IRS_NULL) return;

  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
    (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;

  if (win32_ni_usb_gpib->read_proc) {

    ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
    gpib_addr_t handle = win32_ni_usb_gpib->handle;
    void *buf = (void *)(win32_ni_usb_gpib->rd_buf +
      win32_ni_usb_gpib->rd_buf_pos);
    irs_i32 size = (irs_i32)(win32_ni_usb_gpib->rd_buf_size -
      win32_ni_usb_gpib->rd_buf_pos);
    ni_usb_gpib->ibrd(handle, buf, size);
    win32_ni_usb_gpib->rd_buf_pos += ni_usb_gpib->get_ibcntl();
    if (win32_ni_usb_gpib->rd_buf_pos >= win32_ni_usb_gpib->rd_buf_size) {
      win32_ni_usb_gpib->read_proc = irs_false;
      pchdatas->mode_free = ((!win32_ni_usb_gpib->read_proc) &&
        (!win32_ni_usb_gpib->write_proc));
    }
    //win32_ni_usb_gpib->read_tick = win32_ni_usb_gpib_read_wait;

  }
}
// ���������� �������� ������ win32_ni_usb_gpib
static void win32_ni_usb_gpib_write(void *pchdata)
{
  if (pchdata == IRS_NULL) return;

  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_win32_ni_usb_gpib_t *win32_ni_usb_gpib =
    (mxifa_win32_ni_usb_gpib_t *)pchdatas->ch_spec;

  if (win32_ni_usb_gpib->write_proc) {

    gpib_addr_t handle = win32_ni_usb_gpib->handle;
    ni_usb_gpib_t *ni_usb_gpib = ni_usb_gpib_t::get_instance();
    //ni_usb_gpib->ibclr(handle);
    void *buf = (void *)(win32_ni_usb_gpib->wr_buf +
      win32_ni_usb_gpib->wr_buf_pos);
    irs_i32 size = (irs_i32)(win32_ni_usb_gpib->wr_buf_size -
      win32_ni_usb_gpib->wr_buf_pos);
    ni_usb_gpib->ibwrt(handle, buf, size);
    win32_ni_usb_gpib->wr_buf_pos += ni_usb_gpib->get_ibcntl();
    if (win32_ni_usb_gpib->wr_buf_pos >= win32_ni_usb_gpib->wr_buf_size) {
      win32_ni_usb_gpib->write_proc = irs_false;
      pchdatas->mode_free = ((!win32_ni_usb_gpib->read_proc) &&
        (!win32_ni_usb_gpib->write_proc));
    }

  }
}

//---------------------------------------------------------------------------

