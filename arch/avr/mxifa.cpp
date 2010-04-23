// Абстакция интерфейса для каналов обмена (интерфейсов)
// Max Interface Abstraction
// Дата 23.04.2010
// Ранняя дата 16.04.2008

#include <irsdefs.h>

#include <string.h>

#include <mxifa.h>
#include <UDP_stack.h>
#include <timer.h>
#include <hardflowg.h>
#include <irserror.h>
//#include <irsavrutil.h>

#include <irsfinal.h>

// Таймаут открытия канала
#define TIMEOUT_CHANNEL TIME_TO_CNT(1, 1)

// Специальная пустая функция
static void mxifa_nulf(void *pchdata);
// Выполнение операции чтения для avr128_ether
static void avr128_ether_read(void *pchdata);
// Выполнение операции записи для avr128_ether
static void avr128_ether_write(void *pchdata);

// Элементарная функция канала
typedef void (*mxifa_tick_func_t)(void *);
// Тип текущего режима записи
typedef enum _mxifa_write_mode_t {
  mxifa_wm_free,
  mxifa_wm_write
} mxifa_write_mode_t;
// Тип текущего режима записи
typedef enum _mxifa_read_mode_t {
  mxifa_rm_free,
  mxifa_rm_read
} mxifa_read_mode_t;

// Структура данных канала типа mxifa_ei_avr128_ether
typedef struct _mxifa_avr128_ether_t {
  // Локальный IP
  mxip_t local_ip;
  // Локальный порт
  irs_u16 local_port;
  // Локальная маска
  mxip_t local_mask;
  // Удаленный IP
  mxip_t dest_ip;
  // Удаленный порт
  irs_u16 dest_port;
  // Включение широковещания
  irs_bool is_broadcast;
  // Текущий режим записи
  mxifa_write_mode_t write_mode;
  // Описание удаленной системы для записи
  mxifa_dest_t *wr_dest;
  // Буфер записи
  irs_u8 *wr_buf;
  // Размер буфера записи
  mxifa_sz_t wr_size;
  // Текущий режим записи
  mxifa_read_mode_t read_mode;
  // Описание удаленной системы для чтения
  mxifa_dest_t *rd_dest;
  // Буфер чтения
  irs_u8 *rd_buf;
  // Размер буфера чтения
  mxifa_sz_t rd_size;
  // Начало буфера чтения UDP_stack
  irs_u16 buf_begin;
  // Размер буфера чтения UDP_stack
  irs_u16 buf_size;
  // Размер буфера приема и передачи по Ethernet
  irs_size_t ether_bufs_size;
  // Порт данных AVR к которому подключены линии данных RTL
  irs_avr_port_t data_port;
  // Порт данных AVR к которому подключены линии адреса RTL
  irs_avr_port_t address_port;
  
} mxifa_avr128_ether_t;

// Структура данных канала типа mxifa_ei_hardflow
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

// Обобщенная структура данных канала
typedef struct _mxifa_chdata_t {
  // Тип интерфейса
  mxifa_ei_t enum_iface;
  // Фукции обработки
  mxifa_tick_func_t tick;
  // Истина, если устройство не занято операциями
  irs_bool mode_free;
  // Канал открыт
  irs_bool opened;
  // Специфические данные канала
  void *ch_spec;
} mxifa_chdata_t;

// Порт протокола mxnet
#define MXNET_PORT 5005
// Ethernet (MAC) адрес
irs_u8 local_mac[MXIFA_MAC_SIZE] = {0x00, 0x01, 0xF1, 0x5C, 0xAF, 0x11};
// Локальный IP
mxip_t local_ip = {{192, 168, 0, 38}}; // 4 bytes
// Широковещательный IP
const mxip_t broadcast_ip = {{255, 255, 255, 255}};
// Удаленный IP для канала 1
mxip_t dest_ip_1 = {{192, 168, 0, 16}}; // 4 bytes
// Маска подсети (не используется)
mxip_t local_mask = {{255, 255, 255, 0}}; // 4 bytes

enum { channel_max_cnt = 8 };

//Данные  канала 1 (MXIFA_MXNET)
mxifa_avr128_ether_t mxifa_avr128_ether_1;
// Данные канала 7 (MXIFA_HARDFLOW)
mxifa_hardflow_t mxifa_hardflow_7;

mxifa_chdata_t mxifa_chdata[channel_max_cnt];

static void mxifa_unknown_channel_init(mxifa_ch_t a_channel_ident)
{
  // Инициализируем данные канала
  // Создание псевдонима для данных канала
  mxifa_chdata_t &channel = mxifa_chdata[a_channel_ident];
    // Тип интерфейса
  channel.enum_iface = mxifa_ei_unknown;
  // Фукции обработки
  channel.tick = mxifa_nulf;
  // Истина, если устройство не занято операциями
  channel.mode_free = true;
  // Канал открыт
  channel.opened = false;
  // Специфические данные канала
  channel.ch_spec = IRS_NULL;  
}

static void mxifa_hardflow_channel_init(mxifa_ch_t a_channel_ident,
  mxifa_hardflow_t* ap_channel_spec_data)
{
  mxifa_hardflow_t& spec_data = *ap_channel_spec_data;
  memset(&spec_data, 0, sizeof(spec_data));
 
  spec_data.channel_id = MXIFA_HARDFLOW;
  spec_data.write_buffer = 0;
  spec_data.read_buffer = 0;
  spec_data.rb_size = 0;
  spec_data.wb_size = 0;
  spec_data.rb_current_byte = 0;
  spec_data.wb_current_byte = 0;
  spec_data.read_process = false;
  spec_data.write_process = false;
  spec_data.hardflow = 0;
  
  // Инициализируем данные канала
  // Создание псевдонима для данных канала
  mxifa_chdata_t &channel = mxifa_chdata[a_channel_ident];
    // Тип интерфейса
  channel.enum_iface = mxifa_ei_hardflow;
  // Фукции обработки
  channel.tick = mxifa_nulf;
  // Истина, если устройство не занято операциями
  channel.mode_free = true;
  // Канал открыт
  channel.opened = false;
  // Специфические данные канала
  channel.ch_spec = &spec_data;  
}

static void mxifa_avr128_ether_channel_init(mxifa_ch_t a_channel_ident,
  mxifa_avr128_ether_t* ap_channel_spec_data)
{
  mxifa_avr128_ether_t& spec_data = *ap_channel_spec_data;
  memset(&spec_data, 0, sizeof(spec_data));
  
  spec_data.local_ip = local_ip;
  spec_data.local_port = MXNET_PORT;
  spec_data.local_mask = local_mask;
  spec_data.dest_ip = dest_ip_1;
  spec_data.dest_port = MXNET_PORT;
  spec_data.is_broadcast = irs_false;
  spec_data.write_mode = mxifa_wm_free;
  spec_data.wr_dest = IRS_NULL;
  spec_data.wr_buf = IRS_NULL;
  spec_data.wr_size = 0;
  spec_data.read_mode = mxifa_rm_free;
  spec_data.rd_dest = IRS_NULL;
  spec_data.rd_buf = IRS_NULL;
  spec_data.rd_size = 0;
  spec_data.buf_begin = 0;
  spec_data.buf_size = 0;
  spec_data.ether_bufs_size = 250;
  spec_data.data_port = irs_avr_porta;
  spec_data.address_port = irs_avr_portf;
  
  // Инициализируем данные канала
  // Создание псевдонима для данных канала
  mxifa_chdata_t &channel = mxifa_chdata[a_channel_ident];
    // Тип интерфейса
  channel.enum_iface = mxifa_ei_avr128_ether;
  // Фукции обработки
  channel.tick = avr128_ether_read;
  // Истина, если устройство не занято операциями
  channel.mode_free = true;
  // Канал открыт
  channel.opened = false;
  // Специфические данные канала
  channel.ch_spec = &spec_data;
}

// Обобщенная структура данных канала 1
//mxifa_chdata_t mxifa_chdata_1 = {mxifa_ei_avr128_ether, mxifa_avr128_ether_1};
// Массив указателей на данные всех каналов
//mxifa_chdata_t *mxifa_chdata[] = {IRS_NULL, &mxifa_chdata_1};
// Размер массива указателей на данные всех каналов
#define MXIFA_CHD_SIZE (IRS_ARRAYOFSIZE(mxifa_chdata))
// Номер начального канала
const mxifa_ch_t channel_begin = 1;
// Номер текущего канала
mxifa_ch_t channel_cur = channel_begin;
// Указатель на первый элемент массива с данными каналов
mxifa_chdata_t *mxifa_chdata_begin = &mxifa_chdata[channel_begin];
// Указатель на элемент следующий за последним
mxifa_chdata_t *mxifa_chdata_end = &mxifa_chdata[MXIFA_CHD_SIZE];
// Счетчик инициализаций
static irs_u8 count_init = 0;

// Инициализация mxifa
void mxifa_init()
{
  if (!count_init) {
    init_to_cnt();
    
    mxifa_unknown_channel_init(MXIFA_ZERO);
    mxifa_avr128_ether_channel_init(MXIFA_MXNET, &mxifa_avr128_ether_1);
    mxifa_unknown_channel_init(MXIFA_MXNETC);
    mxifa_unknown_channel_init(MXIFA_SUPPLY);
    mxifa_unknown_channel_init(MXIFA_MULTIMETER);
    mxifa_unknown_channel_init(MXIFA_MODBUS);
    mxifa_unknown_channel_init(MXIFA_MODBUS_CL);
    mxifa_hardflow_channel_init(MXIFA_HARDFLOW, &mxifa_hardflow_7);
  }
  count_init++;
}
// Деинициализация mxifa
void mxifa_deinit()
{
  count_init--;
  if (!count_init) {
    deinit_to_cnt();
  }
}
// Блокирующее открытие channel
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
// Неблокирующее открытие channel
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
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port,
        avr128_ether->ether_bufs_size);
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

// Проверка окончания открытия channel
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
// Блокирующее закрытие channel
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
// Неблокирующее закрытие channel
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
// Проверка окончания закрытия channel
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
// Запись буфера в интерфейс для передачи
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
// Проверка окончания записи
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
// Чтение принятых данных из интерфейса в буфер
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
// Проверка окончания чтения
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
// Элементарное действие
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
// Чтение типа канала
mxifa_ei_t mxifa_get_channel_type(void *pchdata)
{
  if (pchdata == IRS_NULL) return mxifa_ei_unknown;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  return pchdatas->enum_iface;
}
// Общее конфигурирование для avr128_ether
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
  avr128_ether->ether_bufs_size = cfg->ether_bufs_size;
}
// Установка конфигурации канала
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
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port,
        avr128_ether->ether_bufs_size);
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
// Чтение конфигурации канала
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
      cfg->ether_bufs_size = avr128_ether->ether_bufs_size;
    } break;
    case mxifa_ei_hardflow: {
      mxifa_hardflow_t *hardflow = (mxifa_hardflow_t*)pchdatas->ch_spec;
      mxifa_hardflow_cfg *cfg = (mxifa_hardflow_cfg*)config;
      cfg->user_hardflow = hardflow->hardflow;
    } break;
  }
}
// Блокирующее открытие channel c кофигурированием
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
// Неблокирующее открытие channel с конфигурированием
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
      Init_UDP(local_mac, (irs_u8 *)&avr128_ether->local_ip,
        avr128_ether->data_port, avr128_ether->address_port,
        avr128_ether->ether_bufs_size);
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
// Проверка окончания открытия channel с кофигурированием
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
// Чтение типа канала по номеру
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel)
{
  return mxifa_chdata[channel].enum_iface;
}
//---------------------------------------------------------------------------
// Специальная пустая функция
static void mxifa_nulf(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
}
// Выполнение операции чтения для avr128_ether
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
// Выполнение операции записи для avr128_ether
static void avr128_ether_write(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_avr128_ether_t *avr128_ether =
    (mxifa_avr128_ether_t *)pchdatas->ch_spec;
  if (avr128_ether->write_mode == mxifa_wm_write) {
    if (WriteUDP_begin()) {
      mxifa_sz_t size = avr128_ether->wr_size;
      IRS_LIB_ASSERT(size <= udp_buf_size());
      if (size > udp_buf_size()) size = udp_buf_size();
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


