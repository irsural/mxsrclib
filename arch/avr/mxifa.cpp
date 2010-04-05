// ��������� ���������� ��� ������� ������ (�����������)
// Max Interface Abstraction
// ���� 05.04.2010
// ������ ���� 16.04.2008

#include <irsdefs.h>

#include <string.h>

#include <mxifa.h>
#include <UDP_stack.h>
#include <timer.h>
#include <hardflowg.h>
#include <irserror.h>
//#include <irsavrutil.h>

#include <irsfinal.h>

// ������� �������� ������
#define TIMEOUT_CHANNEL TIME_TO_CNT(1, 1)

// ����������� ������ �������
static void mxifa_nulf(void *pchdata);
// ���������� �������� ������ ��� avr128_ether
static void avr128_ether_read(void *pchdata);
// ���������� �������� ������ ��� avr128_ether
static void avr128_ether_write(void *pchdata);

// ������������ ������� ������
typedef void (*mxifa_tick_func_t)(void *);
// ��� �������� ������ ������
typedef enum _mxifa_write_mode_t {
  mxifa_wm_free,
  mxifa_wm_write
} mxifa_write_mode_t;
// ��� �������� ������ ������
typedef enum _mxifa_read_mode_t {
  mxifa_rm_free,
  mxifa_rm_read
} mxifa_read_mode_t;

// ��������� ������ ������ ���� mxifa_ei_avr128_ether
typedef struct _mxifa_avr128_ether_t {
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
  // ������� ����� ������
  mxifa_write_mode_t write_mode;
  // �������� ��������� ������� ��� ������
  mxifa_dest_t *wr_dest;
  // ����� ������
  irs_u8 *wr_buf;
  // ������ ������ ������
  mxifa_sz_t wr_size;
  // ������� ����� ������
  mxifa_read_mode_t read_mode;
  // �������� ��������� ������� ��� ������
  mxifa_dest_t *rd_dest;
  // ����� ������
  irs_u8 *rd_buf;
  // ������ ������ ������
  mxifa_sz_t rd_size;
  // ������ ������ ������ UDP_stack
  irs_u16 buf_begin;
  // ������ ������ ������ UDP_stack
  irs_u16 buf_size;
  #ifndef IRS_LIB_UDP_RTL_STATIC_BUFS
  // ������ ������ ������ � �������� �� Ethernet
  irs_size_t ether_bufs_size;
  #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
  // ���� ������ AVR � �������� ���������� ����� ������ RTL
  irs_avr_port_t data_port;
  // ���� ������ AVR � �������� ���������� ����� ������ RTL
  irs_avr_port_t address_port;
} mxifa_avr128_ether_t;

// ��������� ������ ������ ���� mxifa_ei_hardflow
typedef struct _mxifa_hardflow_t {
  irs_uarc channel_id;
  irs_u8 *write_buffer;
  irs_u8 *read_buffer;
  irs_uarc rb_size;
  irs_uarc wb_size;
  irs_uarc rb_current_byte;
  irs_uarc wb_current_byte;
  bool read_process;
  bool write_process;
  irs::hardflow_t *hardflow;
} mxifa_hardflow_t;

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

// ���� ��������� mxnet
#define MXNET_PORT 5005
// Ethernet (MAC) �����
irs_u8 local_mac[MXIFA_MAC_SIZE] = {0x00, 0x01, 0xF1, 0x5C, 0xAF, 0x11};
// ��������� IP
mxip_t local_ip = {{192, 168, 0, 38}};
// ����������������� IP
const mxip_t broadcast_ip = {{255, 255, 255, 255}};
// ��������� IP ��� ������ 1
mxip_t dest_ip_1 = {{192, 168, 0, 16}};
// ����� ������� (�� ������������)
mxip_t local_mask = {{255, 255, 255, 0}};

// ������ ������ 1
mxifa_avr128_ether_t mxifa_avr128_ether_1 = {
  local_ip,             // ��������� IP
  MXNET_PORT,           // ��������� ����
  local_mask,           // ��������� �����
  dest_ip_1,            // ��������� IP
  MXNET_PORT,           // ��������� ����
  irs_false,            // ��������� �������������
  mxifa_wm_free,        // ������� ����� ������
  IRS_NULL,             // �������� ��������� ������� ��� ������
  IRS_NULL,             // ����� ������
  0,                    // ������ ������ ������
  mxifa_rm_free,        // ������� ����� ������
  IRS_NULL,             // �������� ��������� ������� ��� ������
  IRS_NULL,             // ����� ������
  0,                    // ������ ������ ������
  0,                    // ������ ������ ������ UDP_stack
  0,                    // ������ ������ ������ UDP_stack
  #ifndef IRS_LIB_UDP_RTL_STATIC_BUFS
  // ������ ������ ������ � �������� �� Ethernet
  250,
  #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
  // ���� ������ AVR � �������� ���������� ����� ������ RTL
  irs_avr_porta,
  // ���� ������ AVR � �������� ���������� ����� ������ RTL
  irs_avr_portf //irs_avr_portc �������???
};

// ������ ������ 2
mxifa_hardflow_t mxifa_hardflow = {
  MXIFA_HARDFLOW,       //  channel id
  0,                    //  write_buffer
  0,                    //  read_buffer
  0,                    //  rb_size
  0,                    //  wb_size
  0,                    //  rb_current_byte
  0,                    //  wb_current_byte
  false,                //  read_process
  false,                //  write_process
  0                     //  ��������� �� �����
};

// ������ ������ ���� �������
mxifa_chdata_t mxifa_chdata[] = {
  // ����������������� ������� ����
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  // ���� 1 �� avr128_ether
  {
    mxifa_ei_avr128_ether,        // ��� ����������
    avr128_ether_read,            // ������ ���������
    true,                         // ������, ���� ���������� �� ������ ����������
    false,                        // ����� ������
    (void *)&mxifa_avr128_ether_1 // ������������� ������ ������
  },
  // ����������������� 2 ����
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  // ����������������� 3 ����
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  // ����������������� 4 ����
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  // ����������������� 5 ����
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  // ����������������� 6 ����
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  //  ���� 7 �� hardflow
  {
    mxifa_ei_hardflow,            // ��� ����������
    mxifa_nulf,                   // ������ ���������
    true,                         // ������, ���� ���������� �� ������ ����������
    false,                        // ����� ������
    (void*)&mxifa_hardflow        // ������������� ������ ������
  }
};
// ���������� ��������� ������ ������ 1
//mxifa_chdata_t mxifa_chdata_1 = {mxifa_ei_avr128_ether, mxifa_avr128_ether_1};
// ������ ���������� �� ������ ���� �������
//mxifa_chdata_t *mxifa_chdata[] = {IRS_NULL, &mxifa_chdata_1};
// ������ ������� ���������� �� ������ ���� �������
#define MXIFA_CHD_SIZE (IRS_ARRAYOFSIZE(mxifa_chdata))
// ����� ���������� ������
const mxifa_ch_t channel_begin = 1;
// ����� �������� ������
mxifa_ch_t channel_cur = channel_begin;
// ��������� �� ������ ������� ������� � ������� �������
mxifa_chdata_t *mxifa_chdata_begin = &mxifa_chdata[channel_begin];
// ��������� �� ������� ��������� �� ���������
mxifa_chdata_t *mxifa_chdata_end = &mxifa_chdata[MXIFA_CHD_SIZE];
// ������� �������������
static irs_u8 count_init = 0;

// ������������� mxifa
void mxifa_init()
{
  if (!count_init) {
    init_to_cnt();
  }
  count_init++;
}
// ��������������� mxifa
void mxifa_deinit()
{
  count_init--;
  if (!count_init) {
    deinit_to_cnt();
  }
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
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return irs_false;
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  switch (cur_ch->enum_iface) {
    case mxifa_ei_avr128_ether: {
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)cur_ch->ch_spec;
      const mxip_t *dest_ip = &avr128_ether->dest_ip;
      avr128_ether->is_broadcast = is_broadcast;
      if (is_broadcast) {
        dest_ip = &broadcast_ip;
      }
      #ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port);
      #else //IRS_LIB_UDP_RTL_STATIC_BUFS
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port,
        avr128_ether->ether_bufs_size);
      #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
      OpenUDP(avr128_ether->local_port, avr128_ether->dest_port,
        (irs_u8 *)dest_ip);
      cur_ch->opened = irs_true;
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)cur_ch->ch_spec;
      if (hardflow->hardflow) cur_ch->opened = irs_true;
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
    case mxifa_ei_avr128_ether: {
      abort = abort;
      opened = irs_true;
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      if (hardflow->hardflow) opened = irs_true;
    } break;
  }
  return opened;
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
// ������������� �������� channel
irs_bool mxifa_close_begin(void *pchdata)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool closed = irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_avr128_ether: {
      closed = pchdatas->mode_free;
      if (closed) {
        CloseUDP();
        pchdatas->opened = irs_false;
        Deinit_UDP();
      }
    } break;
    case mxifa_ei_hardflow: {
      pchdatas->opened = irs_false;
      closed = true;  
    }
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
    case mxifa_ei_avr128_ether: {
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)pchdatas->ch_spec;
      if (abort) {
        avr128_ether->read_mode = mxifa_rm_free;
        avr128_ether->write_mode = mxifa_wm_free;
        pchdatas->mode_free = irs_true;
      }
      closed = pchdatas->mode_free;
      if (closed) {
        CloseUDP();
        pchdatas->opened = irs_false;
        Deinit_UDP();
      }
    } break;
    case mxifa_ei_hardflow: {
      closed = true;
    }
  }
  return closed;
}
// ������ ������ � ��������� ��� ��������
irs_bool mxifa_write_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size)
{
  if (pchdata == IRS_NULL) return irs_false;
  if (buf == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_avr128_ether: {
      pchdatas->mode_free = false;
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)pchdatas->ch_spec;
      avr128_ether->write_mode = mxifa_wm_write;
      avr128_ether->wr_dest = dest;
      avr128_ether->wr_buf = buf;
      avr128_ether->wr_size = size;
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      hardflow->wb_size = size;
      hardflow->wb_current_byte = 0;
      hardflow->write_buffer = buf;
      hardflow->write_process = true;
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
    case mxifa_ei_avr128_ether: {
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)pchdatas->ch_spec;
      if (avr128_ether->write_mode == mxifa_wm_free) return irs_true;
      if (abort) {
        avr128_ether->write_mode = mxifa_wm_free;
        return irs_true;
      }
    } break;
    case mxifa_ei_hardflow:
    {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      if (!hardflow->write_process || abort)
      {
        hardflow->write_process = false;
        return true;
      }
      break;
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
    case mxifa_ei_avr128_ether: {
      pchdatas->mode_free = false;
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)pchdatas->ch_spec;
      avr128_ether->read_mode = mxifa_rm_read;
      avr128_ether->rd_dest = dest;
      avr128_ether->rd_buf = buf;
      avr128_ether->rd_size = size;
      //avr128_ether->buf_begin = 0;
      //avr128_ether->buf_size = 0;
    } break;
    case mxifa_ei_hardflow:
    {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      hardflow->rb_size = size;
      hardflow->read_buffer = buf;
      hardflow->rb_current_byte = 0;
      hardflow->read_process = true;
      break;
    }
  }
  return irs_false;
}
// �������� ��������� ������
irs_bool mxifa_read_end(void *pchdata, irs_bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_avr128_ether: {
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)pchdatas->ch_spec;
      if (avr128_ether->read_mode == mxifa_rm_free) return irs_true;
      if (abort) {
        avr128_ether->read_mode = mxifa_rm_free;
        return irs_true;
      }
    } break;
    case mxifa_ei_hardflow:
    {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      if (!hardflow->read_process || abort)
      {
        hardflow->read_process = false;
        return true;
      }
      break;
    }
  }
  return irs_false;
}
// ������������ ��������
void mxifa_tick()
{
  for (
    mxifa_chdata_t *mxifa_chdata_cur = mxifa_chdata_begin;
    mxifa_chdata_cur < mxifa_chdata_end;
    mxifa_chdata_cur++
  ) {
    if (mxifa_chdata_cur->opened) {
      switch (mxifa_chdata_cur->enum_iface) {
        case mxifa_ei_avr128_ether: {
          Tick_UDP();
          mxifa_chdata_cur->tick(mxifa_chdata_cur);
        } break;
        case mxifa_ei_hardflow:
        {
          mxifa_hardflow_t *hardflow =
            (mxifa_hardflow_t*)mxifa_chdata_cur->ch_spec;
          hardflow->hardflow->tick();
  
          if (hardflow->write_process)
          {
            irs_u8 *buf = &hardflow->write_buffer[hardflow->wb_current_byte];
            irs_u8 write_count = hardflow->hardflow->write(
              hardflow->channel_id, buf, 
              hardflow->wb_size - hardflow->wb_current_byte);
            hardflow->wb_current_byte += write_count;
            if (hardflow->wb_current_byte >= hardflow->wb_size)
            {
              hardflow->wb_size = 0;
              hardflow->wb_current_byte = 0;
              hardflow->write_process = false;
            }
          }
          if (hardflow->read_process)
          {
            irs_u8 *buf = &hardflow->read_buffer[hardflow->rb_current_byte];
            irs_uarc size = hardflow->rb_size - hardflow->rb_current_byte;
            irs_u8 read_count = hardflow->hardflow->read(
              hardflow->channel_id, buf, size);
            hardflow->rb_current_byte += read_count;
            if (hardflow->rb_current_byte >= hardflow->rb_size)
            {
              hardflow->rb_size = 0;
              hardflow->rb_current_byte = 0;
              hardflow->read_process = false;
            }
          }
        } break;
      }
    }
  }
}
// ������ ���� ������
mxifa_ei_t mxifa_get_channel_type(void *pchdata)
{
  if (pchdata == IRS_NULL) return mxifa_ei_unknown;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  return pchdatas->enum_iface;
}
// ����� ���������������� ��� avr128_ether
void mxifa_avr128_ether_config(void *pchdata, void *config)
{
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_avr128_cfg *cfg = (mxifa_avr128_cfg *)config;
  mxifa_avr128_ether_t *avr128_ether =
    (mxifa_avr128_ether_t *)pchdatas->ch_spec;
  avr128_ether->local_ip = *cfg->ip;
  memcpy(local_mac, cfg->mac, sizeof(local_mac));
  avr128_ether->data_port = cfg->data_port;
  avr128_ether->address_port = cfg->address_port;
  #ifndef IRS_LIB_UDP_RTL_STATIC_BUFS
  avr128_ether->ether_bufs_size = cfg->ether_bufs_size;
  #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
}
// ��������� ������������ ������
void mxifa_set_config(void *pchdata, void *config)
{
  if (pchdata == IRS_NULL) return;
  if (config == IRS_NULL) return;
  //mxip_t ip, mxip_t mask
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_avr128_ether: {
      //mxifa_avr128_cfg *cfg = (mxifa_avr128_cfg *)config;
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)pchdatas->ch_spec;
      mxifa_avr128_ether_config(pchdata, config);
      if (pchdatas->opened) CloseUDP();
      Deinit_UDP();
      const mxip_t *dest_ip = &avr128_ether->dest_ip;
      if (avr128_ether->is_broadcast) {
        dest_ip = &broadcast_ip;
      }
      #ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port);
      #else //IRS_LIB_UDP_RTL_STATIC_BUFS
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port,
        avr128_ether->ether_bufs_size);
      #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
      if (pchdatas->opened)
        OpenUDP(avr128_ether->local_port, avr128_ether->dest_port,
          (irs_u8 *)dest_ip);
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      mxifa_hardflow_cfg *cfg = (mxifa_hardflow_cfg*)config;
      hardflow->hardflow = cfg->user_hardflow;
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
    case mxifa_ei_avr128_ether: {
      mxifa_avr128_cfg *cfg = (mxifa_avr128_cfg *)config;
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)pchdatas->ch_spec;
      //avr128_ether->local_ip = *cfg->ip;
      cfg->ip = &avr128_ether->local_ip;
      cfg->mask = &avr128_ether->local_mask;
      cfg->mac = local_mac;
      cfg->data_port = avr128_ether->data_port;
      cfg->address_port = avr128_ether->address_port;
      #ifndef IRS_LIB_UDP_RTL_STATIC_BUFS
      cfg->ether_bufs_size = avr128_ether->ether_bufs_size;
      #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      mxifa_hardflow_cfg *cfg = (mxifa_hardflow_cfg*)config;
      cfg->user_hardflow = hardflow->hardflow;
    } break;
  }
}
// ����������� �������� channel c ����������������
void *mxifa_open_ex(mxifa_ch_t channel, void *config, irs_bool is_broadcast)
{
  void *pchdata = mxifa_open_begin_ex(channel, config, is_broadcast);
  counter_t var_timeout_channel_open;
  set_to_cnt(var_timeout_channel_open, TIMEOUT_CHANNEL);
  for (;;) {
    if (mxifa_open_end_ex(pchdata, irs_false)) return pchdata;
    if (test_to_cnt(var_timeout_channel_open)) {
      mxifa_open_end(pchdata, irs_true);
      break;
    }
  }
  return IRS_NULL;
}
// ������������� �������� channel � �����������������
void *mxifa_open_begin_ex(mxifa_ch_t channel, void *config,
  irs_bool is_broadcast)
{
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return irs_false;
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  switch (cur_ch->enum_iface) {
    case mxifa_ei_avr128_ether: {
      mxifa_avr128_ether_t *avr128_ether =
        (mxifa_avr128_ether_t *)cur_ch->ch_spec;
      mxifa_avr128_ether_config(cur_ch, config);
      const mxip_t *dest_ip = &avr128_ether->dest_ip;
      avr128_ether->is_broadcast = is_broadcast;
      if (is_broadcast) {
        dest_ip = &broadcast_ip;
      }
      #ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port);
      #else //IRS_LIB_UDP_RTL_STATIC_BUFS
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port,
        avr128_ether->ether_bufs_size);
      #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
      OpenUDP(avr128_ether->local_port, avr128_ether->dest_port,
        (irs_u8 *)dest_ip);
      cur_ch->opened = irs_true;
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)cur_ch->ch_spec;
      mxifa_hardflow_cfg *cfg = (mxifa_hardflow_cfg*)config;
      hardflow->hardflow = cfg->user_hardflow;
      if (hardflow->hardflow) cur_ch->opened = irs_true;
    } break;
  }
  return cur_ch;
}
// �������� ��������� �������� channel � ����������������
irs_bool mxifa_open_end_ex(void *pchdata, bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool opened = irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_avr128_ether: {
      abort = abort;
      opened = irs_true;
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      if (hardflow->hardflow) opened = irs_true;
    } break;
  }
  return opened;
}
// ������ ���� ������ �� ������
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel)
{
  return mxifa_chdata[channel].enum_iface;
}
//---------------------------------------------------------------------------
// ����������� ������ �������
static void mxifa_nulf(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
}
// ���������� �������� ������ ��� avr128_ether
static void avr128_ether_read(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_avr128_ether_t *avr128_ether =
    (mxifa_avr128_ether_t *)pchdatas->ch_spec;
  if (avr128_ether->read_mode == mxifa_rm_read) {
    if (avr128_ether->buf_size) {
      irs_u16 size = avr128_ether->buf_size;
      if (size > avr128_ether->rd_size) size = avr128_ether->rd_size;
      memcpy((void *)avr128_ether->rd_buf, (void *)(user_rx_buf +
        avr128_ether->buf_begin), size);
      avr128_ether->buf_size -= size;
      avr128_ether->buf_begin += size;
      avr128_ether->read_mode = mxifa_rm_free;
      irs_bool read_mode_free = (avr128_ether->read_mode == mxifa_rm_free);
      irs_bool write_mode_free = (avr128_ether->write_mode == mxifa_wm_free);
      pchdatas->mode_free = (read_mode_free && write_mode_free);
      if (!avr128_ether->buf_size) ReadUDP_end();
    } else {
      irs_u8 *p_ip = IRS_NULL;
      irs_u16 *p_port = IRS_NULL;
      if (avr128_ether->rd_dest) {
        p_ip = (irs_u8 *)&avr128_ether->rd_dest->avr128_ether.ip;
        p_port = &avr128_ether->rd_dest->avr128_ether.port;
      }
      irs_u16 bytes_recieved =  ReadUDP_begin(p_ip, p_port);
      if (bytes_recieved < MXIFA_SZ_T_MAX) {
        avr128_ether->buf_size = bytes_recieved;
      } else {
        avr128_ether->buf_size = MXIFA_SZ_T_MAX;
      }
      avr128_ether->buf_begin = 0;
    }
  }
  pchdatas->tick = avr128_ether_write;
}
// ���������� �������� ������ ��� avr128_ether
static void avr128_ether_write(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_avr128_ether_t *avr128_ether =
    (mxifa_avr128_ether_t *)pchdatas->ch_spec;
  if (avr128_ether->write_mode == mxifa_wm_write) {
    if (WriteUDP_begin()) {
      mxifa_sz_t size = avr128_ether->wr_size;
      #ifdef IRS_LIB_UDP_RTL_STATIC_BUFS
      #if (MXIFA_SZ_T_MAX > UDP_BUFFER_SIZE_TX)
      if (size > UDP_BUFFER_SIZE_TX) size = UDP_BUFFER_SIZE_TX;
      #endif //(MXIFA_SZ_T_MAX > UDP_BUFFER_SIZE_TX)
      #else //IRS_LIB_UDP_RTL_STATIC_BUFS
      IRS_LIB_ASSERT(size <= udp_buf_size());
      if (size > udp_buf_size()) size = udp_buf_size();
      #endif //IRS_LIB_UDP_RTL_STATIC_BUFS
      memcpy((void *)user_tx_buf, (void *)avr128_ether->wr_buf, size);
      avr128_ether->write_mode = mxifa_wm_free;
      irs_bool read_mode_free = (avr128_ether->read_mode == mxifa_rm_free);
      irs_bool write_mode_free = (avr128_ether->write_mode == mxifa_wm_free);
      pchdatas->mode_free = (read_mode_free && write_mode_free);
      if (avr128_ether->wr_dest) {
        WriteUDP_end((irs_u8 *)&avr128_ether->wr_dest->avr128_ether.ip,
          &avr128_ether->wr_dest->avr128_ether.port, size);
      } else {
        WriteUDP_end(IRS_NULL, IRS_NULL, size);
      }
    }
  }
  pchdatas->tick = avr128_ether_read;
}


