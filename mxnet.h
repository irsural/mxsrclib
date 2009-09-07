// Протокол MxNet (Max Network)
// Дата 16.04.2008

#ifndef MXNETH
#define MXNETH

#include <irsdefs.h>
#include <mxifa.h>
#include <mxnetda.h>
#include <mxnetr.h>
#include <timer.h>

// Версия (<старший байт>.<младший байт>)
#define MXN_VERSION 0x0003

// Предварительное объявление данных для протокола mxnet
struct _mxn_data_t;
typedef _mxn_data_t mxn_data_t;
// Тип для элементарной функции
typedef void (*mxn_tick_f)(mxn_data_t &data);
// Данные для протокола mxnet
struct _mxn_data_t {
  // Количество переменных
  mxn_cnt_t count;
  // Внешний массив переменных
  irs_i32 *vars_ext;
  // Внутренний массив переменных
  mxn_packet_t *packet;
  // Массив помечающий переменные только для чтения
  irs_bool *read_only;
  // Работа в режиме широковещания
  irs_bool is_broadcast;
  // Переменная таймаута широковещательных рассылок
  counter_t to_broadcast;
  // Изменение режима широковещания на новый
  irs_bool is_broadcast_new;
  // Канал по которому работает протокол
  mxifa_ch_t channel;
  // Данные канала
  void *data_ch;
  // Данные об удаленном клиенте
  mxifa_dest_t dest_info;
  // Количество переменных в принятом пакете
  mxn_cnt_t var_cnt_packet;
  // Следующая элементарная функция
  mxn_tick_f mxn_tick_next;
  // Элементарная функция
  mxn_tick_f mxn_tick;
  // Количество отсылаемых байт
  mxn_sz_t count_send;
  // Широковещательная передача
  irs_bool broadcast_send;
  // Ошибка в аргументах записи
  irs_bool write_error;
  // Поиск начала пакета
  irs::mx_beg_pack_proc_t *beg_pack_proc;
  // Текущий тип контрольной суммы
  irs::mxn_checksum_t checksum_type;
};
struct mxn_ext_param_t
{
  // Структура для установки конфигурации avr128_ether в
  // функции mxifa_set_config
  mxifa_avr128_cfg *avr128_cfg;
};

// Инициализация протокола
void mxn_init(mxn_data_t &data, mxifa_ch_t channel, irs_i32 *vars,
  mxn_cnt_t count, mxn_ext_param_t *ext_param = 0);
// Деинициализация протокола
void mxn_deinit(mxn_data_t &data);
// Установка только для чтение переменная или нет
void mxn_set_read_only(mxn_data_t &data, mxn_cnt_t ind_var, irs_bool read_only);
// Установка нового режима широковещания
void mxn_set_broadcast(mxn_data_t &data, irs_bool is_broadcast);
// Элементарная функция
void mxn_tick(mxn_data_t &data);
// Чтение указателя на дескриптор канала
//void *mxn_get_data_channel(mxn_data_t &data);
// Установка локального IP
void mxn_set_ip(mxn_data_t &data, mxip_t ip);
// Чтение локального IP
mxip_t mxn_get_ip(mxn_data_t &data);
// Установка локальной маски
void mxn_set_mask(mxn_data_t &data, mxip_t mask);
// Чтение локальной маски
mxip_t mxn_get_mask(mxn_data_t &data);
// Установка MAC(Ethernet)-адреса
void mxn_set_mac(mxn_data_t &data, irs_u8 *mac);
// Чтение MAC(Ethernet)-адреса
void mxn_get_mac(mxn_data_t &data, irs_u8 *mac);
// Установка портов AVR к которым подключена RTL
void mxn_set_avrport(mxn_data_t &data, irs_avr_port_t data_port, 
  irs_avr_port_t address_port);
// Чтение портов AVR к которым подключена RTL
void mxn_get_avrport(mxn_data_t &data, irs_avr_port_t &data_port, 
  irs_avr_port_t &address_port);
// Установка типа контрольной суммы
void mxn_set_checksum_type(mxn_data_t &data, irs::mxn_checksum_t checksum_type); 
// Чтение типа контрольной суммы
irs::mxn_checksum_t mxn_get_checksum_type(mxn_data_t &data); 

namespace irs {

// Преобразование из внешнего массива irs_u8 в mxdata_t
class mxdata_to_u8_t : public mxdata_t
{
  irs_bool m_is_connected;
  irs_u8 *mp_u8_data;
  irs_uarc m_u8_size;
public:
  mxdata_to_u8_t(irs_u8 *ap_u8_data, irs_uarc a_u8_size);
  ~mxdata_to_u8_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void tick();
};

} //namespace irs

#endif //MXNETH
