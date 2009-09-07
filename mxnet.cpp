// Протокол MxNet (Max Network)
// Дата: 18.06.2008

#include <gdefs.h>
#include <string.h>
#include <mxnet.h>
#include <mxnetd.h>
#include <irsdefs.h>
//#include <irsavrutil.h>

// Пуск выполнения обработки
static void mxn_tick_start(mxn_data_t &data);
// Запуск чтения заголовка
static void mxn_read_head(mxn_data_t &data);
// Ожидание чтения заголовка и его анализ
static void mxn_wait_read_head_and_analysis(mxn_data_t &data);
// Чтение остальных данных
static void mxn_read_data(mxn_data_t &data);
// Проверка контрольной суммы
static void mxn_checksum(mxn_data_t &data);
// Декодирование команды
static void mxn_decode_comm(mxn_data_t &data);
// Формирование пакета для ответа на команду MXN_READ_COUNT
static void mxn_read_count_packet(mxn_data_t &data);
// Расчет контрольной суммы
static void mxn_calc_checksum(irs_i32 &chksum, mxn_packet_t *packet,
  mxn_cnt_t var_count, irs::mxn_checksum_t checksum_type);
// Отправка ответа
static void mxn_write(mxn_data_t &data);
// Ожидание записи
static void mxn_write_wait(mxn_data_t &data);
// Обработка пакета MXN_READ
static void mxn_read_proc(mxn_data_t &data);
// Формирование пакета для ответа на команду MXN_READ
static void mxn_read_packet(mxn_data_t &data);
// Обработка пакета MXN_WRITE
static void mxn_write_proc(mxn_data_t &data);
// Формирование пакета для ответа на команду MXN_WRITE
static void mxn_write_packet(mxn_data_t &data);
// Формирование пакета для ответа на команду MXN_GET_VERSION
static void mxn_get_version_packet(mxn_data_t &data);
// Обработка пакета MXN_SET_BROADCAST
static void mxn_set_broadcast_proc(mxn_data_t &data);
// Формирование пакета для ответа на команду MXN_SET_BROADCAST
static void mxn_set_broadcast_packet(mxn_data_t &data);
// Отправка широковещательного пакета MXN_WRITE_BROADCAST
static void mxn_write_broadcast_packet(mxn_data_t &data);
// Ожидание открытия канала
static void mxn_open_wait(mxn_data_t &data);

// Инициализация протокола
void mxn_init(mxn_data_t &data, mxifa_ch_t channel, irs_i32 *vars,
  mxn_cnt_t count, mxn_ext_param_t *ext_param)
{
  init_to_cnt();
  mxifa_init();
  data.count = count;
  data.vars_ext = vars;
  data.packet = (mxn_packet_t *)new irs_i32[data.count + SIZE_OF_HEADER + 1];
  data.read_only = new irs_bool[data.count];
  for (mxn_cnt_t ind_var = 0; ind_var < data.count; ind_var++) {
    data.read_only[ind_var] = irs_false;
  }
  memset((void *)&data.dest_info, 0, sizeof(data.dest_info));
  data.channel = channel;
  data.is_broadcast = irs_false;
  data.is_broadcast_new = irs_false;
  data.var_cnt_packet = 0;
  data.mxn_tick = mxn_tick_start;
  bool open_ex_performed = false;
  if (ext_param) {
    mxifa_ei_t channel_type = mxifa_get_channel_type_ex(channel);
    if (channel_type == mxifa_ei_avr128_ether) {
      open_ex_performed = true;
      data.data_ch = mxifa_open_ex(data.channel, ext_param->avr128_cfg, 
        data.is_broadcast);
    } 
  }
  if (!open_ex_performed)
    data.data_ch = mxifa_open(data.channel, data.is_broadcast);
  data.beg_pack_proc = new irs::mx_beg_pack_proc_t(data.data_ch);
  data.count_send = 0;
  data.broadcast_send = irs_false;
  data.write_error = irs_false;
  data.checksum_type = irs::mxncs_reduced_direct_sum;
}
// Деинициализация протокола
void mxn_deinit(mxn_data_t &data)
{
  delete data.beg_pack_proc;
  mxifa_close(data.data_ch);
  delete []data.read_only;
  delete [](irs_i32 *)data.packet;
  mxifa_deinit();
  deinit_to_cnt();
}
// Установка только для чтение переменная или нет
void mxn_set_read_only(mxn_data_t &data, mxn_cnt_t ind_var, irs_bool read_only)
{
  if (ind_var < data.count)
    data.read_only[ind_var] = read_only;
}
// Установка нового режима широковещания
void mxn_set_broadcast(mxn_data_t &data, irs_bool is_broadcast)
{
  data.is_broadcast_new = is_broadcast;
}
// Элементарная функция
void mxn_tick(mxn_data_t &data)
{
  data.mxn_tick(data);
}
#ifdef NOP
// Чтение указателя на дескриптор канала
void *mxn_get_data_channel(mxn_data_t &data)
{
  return data.data_ch;
}
#endif //NOP
// Установка локального IP
void mxn_set_ip(mxn_data_t &data, mxip_t ip)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    // Текущая конфигурация канала
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    chan_cfg.ip = &ip;
    mxifa_set_config(data.data_ch, &chan_cfg);
  }
}
// Чтение локального IP
mxip_t mxn_get_ip(mxn_data_t &data)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    return *chan_cfg.ip;
  }
  mxip_t error_ip = {{0, 0, 0, 0}};
  return error_ip;
}
// Установка локальной маски
void mxn_set_mask(mxn_data_t &data, mxip_t mask)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    chan_cfg.mask = &mask;
    mxifa_set_config(data.data_ch, &chan_cfg);
  }
}
// Чтение локальной маски
mxip_t mxn_get_mask(mxn_data_t &data)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    return *chan_cfg.mask;
  }
  mxip_t error_mask = {{0, 0, 0, 0}};
  return error_mask;
}
// Установка MAC(Ethernet)-адреса
void mxn_set_mac(mxn_data_t &data, irs_u8 *mac)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    chan_cfg.mac = mac;
    mxifa_set_config(data.data_ch, &chan_cfg);
  }
}
// Чтение MAC(Ethernet)-адреса
void mxn_get_mac(mxn_data_t &data, irs_u8 *mac)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    memcpy(mac, chan_cfg.mac, MXIFA_MAC_SIZE);
    return;
  }
  // В случае ошибки возвращаем MAC заполненый нулями
  memset(mac, 0, MXIFA_MAC_SIZE);
}
// Установка портов AVR к которым подключена RTL
void mxn_set_avrport(mxn_data_t &data, irs_avr_port_t data_port, 
  irs_avr_port_t address_port)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    chan_cfg.data_port = data_port;
    chan_cfg.address_port = address_port;
    mxifa_set_config(data.data_ch, &chan_cfg);
  }
}
// Чтение портов AVR к которым подключена RTL
void mxn_get_avrport(mxn_data_t &data, irs_avr_port_t &data_port, 
  irs_avr_port_t &address_port)
{
  if (mxifa_get_channel_type(data.data_ch) == mxifa_ei_avr128_ether) {
    mxifa_avr128_cfg chan_cfg;
    mxifa_get_config(data.data_ch, &chan_cfg);
    data_port = chan_cfg.data_port;
    address_port = chan_cfg.address_port;
  }
}
// Установка типа контрольной суммы
void mxn_set_checksum_type(mxn_data_t &data, irs::mxn_checksum_t checksum_type)
{
  data.checksum_type = checksum_type;
}
// Чтение типа контрольной суммы
irs::mxn_checksum_t mxn_get_checksum_type(mxn_data_t &data)
{
  return data.checksum_type;
}
// Пуск выполнения обработки
static void mxn_tick_start(mxn_data_t &data)
{
  mxifa_tick();
  set_to_cnt(data.to_broadcast, MXN_BROADCAST_INTERVAL);
  data.mxn_tick = mxn_read_head;
}
// Запуск чтения заголовка
static void mxn_read_head(mxn_data_t &data)
{
  mxifa_tick();
  (*data.beg_pack_proc)((irs_u8 *)data.packet, &data.dest_info);
  data.mxn_tick = mxn_wait_read_head_and_analysis;
}
// Ожидание чтения заголовка и его анализ
static void mxn_wait_read_head_and_analysis(mxn_data_t &data)
{

  //static irs::blink_t blink2(irs_avr_porte, 2);
  //blink2();

  mxifa_tick();
  if (!data.beg_pack_proc->tick()) {
    if (data.packet->code_comm == MXN_WRITE) {
      if (data.packet->var_count <= data.count) {
        data.var_cnt_packet = data.packet->var_count;
        data.mxn_tick = mxn_read_data;
      } else {
        data.mxn_tick = mxn_read_head;
      }
    } else {
      data.var_cnt_packet = 0;
      //data.mxn_tick_next = mxn_read_head;
      //data.mxn_tick = mxn_checksum;
      data.mxn_tick = mxn_read_data;
    }
  } else {
    if (test_to_cnt(data.to_broadcast)) {
      set_to_cnt(data.to_broadcast, MXN_BROADCAST_INTERVAL);
      if (data.is_broadcast != data.is_broadcast_new) {
        data.is_broadcast = data.is_broadcast_new;
        mxifa_close_begin(data.data_ch);
        mxifa_close_end(data.data_ch, irs_true);
        data.data_ch = mxifa_open_begin(data.channel, data.is_broadcast);
        data.mxn_tick = mxn_open_wait;
      } else {
        if (data.is_broadcast) {
          mxifa_read_end(data.data_ch, irs_true);
          data.mxn_tick = mxn_write_broadcast_packet;
        }
      }
    }
  }
}
// Чтение остальных данных
static void mxn_read_data(mxn_data_t &data)
{
  mxifa_tick();
  mxifa_read_begin(data.data_ch, &data.dest_info, ((irs_u8 *)data.packet) +
    mxn_header_size, sizeof(irs_i32)*(data.var_cnt_packet + 1));
  data.mxn_tick = mxn_checksum;
}
// Проверка контрольной суммы
static void mxn_checksum(mxn_data_t &data)
{
  mxifa_tick();
  if (mxifa_read_end(data.data_ch, irs_false)) {
    irs_i32 chksum;
    mxn_calc_checksum(chksum, data.packet, data.var_cnt_packet,
      data.checksum_type);
    if (chksum == data.packet->var[data.var_cnt_packet]) {
      data.mxn_tick = mxn_decode_comm;
      //data.mxn_tick = data.mxn_tick_next;
    } else {
      data.mxn_tick = mxn_read_head;
    }
  }
}
// Декодирование команды
static void mxn_decode_comm(mxn_data_t &data)
{
  mxifa_tick();
  switch (data.packet->code_comm) {
    case MXN_READ_COUNT: {
      data.mxn_tick = mxn_read_count_packet;
    } break;
    case MXN_READ: {
      data.mxn_tick = mxn_read_proc;
    } break;
    case MXN_WRITE: {
      data.mxn_tick = mxn_write_proc;
    } break;
    case MXN_GET_VERSION: {
      data.mxn_tick = mxn_get_version_packet;
    } break;
    case MXN_SET_BROADCAST: {
      data.mxn_tick = mxn_set_broadcast_proc;
    } break;
    default: {
      data.mxn_tick = mxn_read_head;
    } break;
  }
}
// Формирование пакета для ответа на команду MXN_READ_COUNT
static void mxn_read_count_packet(mxn_data_t &data)
{
  mxifa_tick();
  data.packet->var_count = data.count;
  mxn_calc_checksum(data.packet->var[0], data.packet, 0, data.checksum_type);
  data.count_send = sizeof(irs_i32)*(SIZE_OF_HEADER + 1);
  //data.mxn_tick = mxn_read_count_write;
  data.mxn_tick = mxn_write;
  data.mxn_tick_next = mxn_read_head;
}
// ***Расчет контрольной суммы
static void mxn_calc_checksum(irs_i32 &chksum, mxn_packet_t *packet,
  mxn_cnt_t var_count, irs::mxn_checksum_t checksum_type)
{
  chksum = irs::checksum_calc(checksum_type, packet, var_count);
}
// Отправка ответа
static void mxn_write(mxn_data_t &data)
{
  mxifa_tick();
  mxifa_dest_t *p_dest_info = &data.dest_info;
  if (data.broadcast_send) {
    data.broadcast_send = irs_false;
    p_dest_info = IRS_NULL;
  }
  mxifa_write_begin(data.data_ch, p_dest_info, (irs_u8 *)data.packet,
    data.count_send);
  data.mxn_tick = mxn_write_wait;
}
// Ожидание записи
static void mxn_write_wait(mxn_data_t &data)
{
  mxifa_tick();
  if (mxifa_write_end(data.data_ch, irs_false)) {
    data.mxn_tick = data.mxn_tick_next;
  }
}
// Обработка пакета MXN_READ
static void mxn_read_proc(mxn_data_t &data)
{
  mxifa_tick();
  data.mxn_tick = mxn_read_packet;
  if (data.packet->var_count > MXN_CNT_MAX - data.packet->var_ind_first)
    data.mxn_tick = mxn_read_head;
  if (data.packet->var_ind_first + data.packet->var_count > data.count)
    data.mxn_tick = mxn_read_head;
}
// Формирование пакета для ответа на команду MXN_READ
static void mxn_read_packet(mxn_data_t &data)
{
  mxifa_tick();
  memcpy((void *)data.packet->var,
         (void *)(data.vars_ext + data.packet->var_ind_first),
         sizeof(irs_i32)*data.packet->var_count);
  mxn_calc_checksum(data.packet->var[data.packet->var_count], data.packet, 
    data.packet->var_count, data.checksum_type);
  data.count_send = sizeof(irs_i32)*(data.packet->var_count + SIZE_OF_HEADER + 1);
  data.mxn_tick = mxn_write;
  data.mxn_tick_next = mxn_read_head;
}
// Обработка пакета MXN_WRITE
static void mxn_write_proc(mxn_data_t &data)
{
  mxifa_tick();
  irs_bool over_max =
    (data.packet->var_count > (MXN_CNT_MAX - data.packet->var_ind_first));
  irs_bool over_range =
    ((data.packet->var_ind_first + data.packet->var_count) > data.count);
  data.write_error = (over_max || over_range);

  if (!data.write_error) {
    for (mxn_cnt_t ind_var = 0, ind_var_ext = data.packet->var_ind_first;
         ind_var < data.packet->var_count;
         ind_var++, ind_var_ext++) {
      if (!data.read_only[ind_var_ext])
        data.vars_ext[ind_var_ext] = data.packet->var[ind_var];
    }
  }
  data.mxn_tick = mxn_write_packet;
}
// Формирование пакета для ответа на команду MXN_WRITE
static void mxn_write_packet(mxn_data_t &data)
{
  mxifa_tick();
  //data.packet->code_comm = MXN_WRITE_ACK;
  data.packet->var_ind_first = data.write_error;
  data.packet->var_count = 0;
  mxn_calc_checksum(data.packet->var[0], data.packet, 0, data.checksum_type);
  data.count_send = sizeof(irs_i32)*(SIZE_OF_HEADER + 1);
  data.mxn_tick = mxn_write;
  data.mxn_tick_next = mxn_read_head;
}
// Формирование пакета для ответа на команду MXN_GET_VERSION
static void mxn_get_version_packet(mxn_data_t &data)
{
  mxifa_tick();
  data.packet->var_ind_first = 0;
  IRS_LOWORD(data.packet->var_count) = MXN_VERSION;
  mxn_calc_checksum(data.packet->var[0], data.packet, 0, data.checksum_type);
  data.count_send = sizeof(irs_i32)*(SIZE_OF_HEADER + 1);
  data.mxn_tick = mxn_write;
  data.mxn_tick_next = mxn_read_head;
}
// Обработка пакета MXN_SET_BROADCAST
static void mxn_set_broadcast_proc(mxn_data_t &data)
{
  mxifa_tick();
  switch (data.packet->var_ind_first) {
    case irs_true:
    case irs_false: {
      data.is_broadcast_new = (irs_bool)data.packet->var_ind_first;
      data.mxn_tick = mxn_set_broadcast_packet;
    } break;
    default: {
      data.mxn_tick = mxn_read_head;
    } break;
  }
}
// Формирование пакета для ответа на команду MXN_SET_BROADCAST
static void mxn_set_broadcast_packet(mxn_data_t &data)
{
  mxifa_tick();
  data.count_send = sizeof(irs_i32)*(SIZE_OF_HEADER + 1);
  data.mxn_tick = mxn_write;
  data.mxn_tick_next = mxn_read_head;
}
// Отправка широковещательного пакета MXN_WRITE_BROADCAST
static void mxn_write_broadcast_packet(mxn_data_t &data)
{
  mxifa_tick();
  data.packet->ident_beg_pack_first = MXN_CONST_IDENT_BEG_PACK_FIRST;
  data.packet->ident_beg_pack_second = MXN_CONST_IDENT_BEG_PACK_SECOND;
  data.packet->code_comm = MXN_WRITE_BROADCAST;
  data.packet->var_ind_first = 0;
  data.packet->var_count = data.count;
  memcpy((void *)data.packet->var, (void *)data.vars_ext,
    sizeof(irs_i32)*data.count);
  mxn_calc_checksum(data.packet->var[data.count], data.packet, data.count,
    data.checksum_type);
  data.count_send = sizeof(irs_i32)*(data.count + SIZE_OF_HEADER + 1);
  data.broadcast_send = irs_true;
  data.mxn_tick = mxn_write;
  data.mxn_tick_next = mxn_read_head;
}
// Ожидание открытия канала
static void mxn_open_wait(mxn_data_t &data)
{
  mxifa_tick();
  if (mxifa_open_end(data.data_ch, irs_false)) {
    data.mxn_tick = mxn_read_head;
  }
}
//---------------------------------------------------------------------------
// Преобразование из внешнего массива irs_u8 в mxdata_t

irs::mxdata_to_u8_t::mxdata_to_u8_t(irs_u8 *ap_u8_data, irs_uarc a_u8_size):
  m_is_connected(irs_false),
  mp_u8_data(ap_u8_data),
  m_u8_size(a_u8_size)
{
  if (mp_u8_data && m_u8_size) m_is_connected = irs_true;
}

irs::mxdata_to_u8_t::~mxdata_to_u8_t()
{
  //  даже не знаю, что тут такого плохого сделать...
  //  delete Windows;
}

irs_uarc irs::mxdata_to_u8_t::size()
{
  return m_u8_size;
}

irs_bool irs::mxdata_to_u8_t::connected()
{
  return m_is_connected;
}

void irs::mxdata_to_u8_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (m_is_connected)
  {
    irs_uarc real_index = a_index;
    irs_uarc real_size = a_size;
    if (real_index > m_u8_size - 1) real_index = m_u8_size - 1;
    if (real_size > m_u8_size - real_index)
      real_size = m_u8_size - real_index;
    memcpy((void*)ap_buf, (void*)(mp_u8_data + real_index), real_size);
  }
}

void irs::mxdata_to_u8_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (m_is_connected)
  {
    irs_uarc real_index = a_index;
    irs_uarc real_size = a_size;
    if (real_index > m_u8_size - 1) real_index = m_u8_size - 1;
    if (real_size > m_u8_size - real_index)
      real_size = m_u8_size - real_index;
    memcpy((void*)(mp_u8_data + real_index), (void*)ap_buf, real_size);
  }
}

irs_bool irs::mxdata_to_u8_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (m_is_connected)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_u8_size) index = m_u8_size - 1;
    if (bit_index > 7) bit_index = 7;
    return (mp_u8_data[index] & irs_u8(1 << bit_index));
  }
  else return irs_false;
}

void irs::mxdata_to_u8_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (m_is_connected)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_u8_size) index = m_u8_size - 1;
    if (bit_index > 7) bit_index = 7;
    mp_u8_data[index] |= irs_u8(1 << bit_index);
  }
}

void irs::mxdata_to_u8_t::clear_bit(irs_uarc a_index,irs_uarc a_bit_index)
{
  if (m_is_connected)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_u8_size) index = m_u8_size - 1;
    if (bit_index > 7) bit_index = 7;
    mp_u8_data[index] &= irs_u8((1 << bit_index)^0xFF);
  }
}

void irs::mxdata_to_u8_t::tick()
{
  //
}

