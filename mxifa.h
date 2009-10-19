// Абстакция интерфейса для каналов обмена (интерфейсов)
// Max Interface Abstraction
// Дата: 19.10.2009
// Ранняя дата: 06.03.2008

#ifndef MXIFAH
#define MXIFAH

#include <irsdefs.h>
#include <mxifal.h>
#include <mxifar.h>
#include <irsstd.h>
#include <hardflowg.h>
#include <irserror.h>

//extern irs_bool dbg_write;

// Номера каналов закрепленных за программами или модулями
// zero - неиспользуемый канал
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
// Максимальный номер
#define MXIFA_CNT_CHANNEL  15

// Тип для адреса GPIB-устройства
typedef irs_i32 gpib_addr_t;

// Длина MAC(Ethernet)-адреса
#define MXIFA_MAC_SIZE 6

// Тип канала
typedef enum _mxifa_ei_t {
  // Неизвестен
  mxifa_ei_unknown,
  // UDP/IP Димы Уржумцева
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

// Структура для описания удаленной системы
typedef struct _mxifa_avr128_ether_dest_t {
  // Удаленный IP
  mxip_t ip;
  // Удаленный порт
  irs_u16 port;
} mxifa_avr128_ether_dest_t;

// Структура для описания удаленной системы
typedef struct _mxifa_win32_tcp_ip_dest_t {
  // Удаленный IP
  mxip_t ip;
  // Удаленный порт
  irs_u16 port;
} mxifa_win32_tcp_ip_dest_t;

// Структура для описания удаленной системы
typedef struct _mxifa_win32_ni_usb_gpib_dest_t {
  // Адрес GPIB-устройства
  gpib_addr_t address;
} mxifa_win32_ni_usb_gpib_dest_t;

//
typedef struct _mxifa_linux_dest_t {
  // Удаленный IP
  mxip_t ip;
  // Удаленный порт
  irs_u16 port;
} mxifa_linux_dest_t;

typedef struct _mxifa_linux_cl_dest_t {
  // Удаленный IP
  mxip_t ip;
  // Удаленный порт
  irs_u16 port;
} mxifa_linux_cl_dest_t;

// Структура для описания удаленной системы
typedef struct _mxifa_hardflow_dest_t {
  // Идентификатор
  irs_uarc channel_id;
} mxifa_hardflow_dest_t;

// Обобщенная структура для описания удаленной системы
typedef union _mxifa_dest_t {
  // Структура для описания удаленной системы канала типа mxifa_ei_avr128_ether
  mxifa_avr128_ether_dest_t avr128_ether;
  // Структура для описания удаленной системы канала типа mxifa_ei_win32_tcp_ip
  mxifa_win32_tcp_ip_dest_t win32_tcp_ip;
  // Структура для описания удаленной системы канала
  // типа mxifa_ei_win32_ni_usb_gpib
  mxifa_win32_ni_usb_gpib_dest_t win32_ni_usb_gpib;
  mxifa_linux_dest_t mxifa_linux_dest;
  mxifa_linux_cl_dest_t mxifa_linux_cl_dest;
  mxifa_hardflow_dest_t mxifa_hardflow_dest;
} mxifa_dest_t;

// Структура для установки конфигурации avr128_ether в функции mxifa_set_config
typedef struct _mxifa_avr128_cfg {
  // Локальный IP
  mxip_t *ip;
  // Маска подсети
  mxip_t *mask;
  // Локальный MAC(Ethernet)-адрес (6 байт)
  irs_u8 *mac;
  // Порт данных AVR к которому подключены линии данных RTL
  irs_avr_port_t data_port;
  // Порт данных AVR к которому подключены линии адреса RTL
  irs_avr_port_t address_port;
} mxifa_avr128_cfg;

// Структура для установки конфигурации win32_tcp_ip в функции mxifa_set_config
typedef struct _mxifa_win32_tcp_ip_cfg {
  // Удаленный IP
  mxip_t dest_ip;
  // Удаленный порт
  irs_u16 dest_port;
  // Локальный порт
  irs_u16 local_port;
} mxifa_win32_tcp_ip_cfg;

// Структура для установки конфигурации win32_ni_usb_gpib
// в функции mxifa_set_config
typedef struct _mxifa_win32_ni_usb_gpib_cfg {
  // Побитовая конфигурация
  irs_u32 bitcfg;
  // Адрес GPIB-устройства
  gpib_addr_t address;
  // Число действительно прочитаных байт, после прерывания операции чтения
  mxifa_sz_t read_count;
} mxifa_win32_ni_usb_gpib_cfg;

//
typedef struct _mxifa_linux_tcp_ip_cfg {
  // Удаленный IP
  mxip_t dest_ip;
  // Удаленный порт
  irs_u16 dest_port;
  // Локальный порт
  irs_u16 local_port;
  //Type of protocol IP or TCP
  int pro_type;

} mxifa_linux_tcp_ip_cfg;

typedef struct _mxifa_linux_tcp_ip_cl_cfg {
  // Удаленный IP
  mxip_t dest_ip;
  // Удаленный порт
  irs_u16 dest_port;
  // Локальный порт
  irs_u16 local_port;
  //Type of protocol IP or TCP
  int pro_type;

} mxifa_linux_tcp_ip_cl_cfg;

//  Структура для установки конфигурации hardflow
//  в функции mxifa_set_config
typedef struct _mxifa_hardflow_cfg {
  irs::hardflow_t *user_hardflow;
} mxifa_hardflow_cfg;

#if defined(__WATCOMC__) || defined(_MSC_VER)
// Определение функций для компиляторов для которых нет реализации mxifa
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
// Инициализация mxifa
void mxifa_init();
// Деинициализация mxifa
void mxifa_deinit();
// Элементарное действие
void mxifa_tick();
// Блокирующее открытие channel
// channel - номер канала, занимается в специальной таблице для каждой программы
// is_broadcast - открытие канала в широковещательном режиме
// Возврат - указатель на данные канала
void *mxifa_open(mxifa_ch_t channel, irs_bool is_broadcast);
// Неблокирующее открытие channel
// channel - номер канала, занимается в специальной таблице для каждой программы
// is_broadcast - открытие канала в широковещательном режиме
// Возврат - указатель на данные канала
void *mxifa_open_begin(mxifa_ch_t channel, irs_bool is_broadcast);
// Проверка окончания открытия channel
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// abort - немедленное завершение с отменой операции
// Возврат - операция окончена
irs_bool mxifa_open_end(void *pchdata, bool abort);
// Блокирующее закрытие channel
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// Возврат - операция окончена
irs_bool mxifa_close(void *pchdata);
// Неблокирующее закрытие channel
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// Возврат - операция окончена
irs_bool mxifa_close_begin(void *pchdata);
// Проверка окончания закрытия channel
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// abort - немедленное завершение с отменой операции
// Возврат - операция окончена
irs_bool mxifa_close_end(void *pchdata, bool abort);
// Запись буфера в интерфейс для передачи
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// dest - приемник данных, если IRS_NULL, то используется приемник канала
//   по умолчанию
// buf - укзатель на данные для передачи
// size - размер данных в байтах
// Возврат - операция окончена
irs_bool mxifa_write_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size);
// Проверка окончания записи
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// abort - немедленное завершение с отменой операции
// Возврат - операция окончена
irs_bool mxifa_write_end(void *pchdata, irs_bool abort);
// Чтение принятых данных из интерфейса в буфер
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// dest - источник данных, если IRS_NULL, то используется источник канала
//   по умолчанию
// buf - укзатель на буфер для приема
// size - размер буфера в байтах
// Возврат - операция окончена
irs_bool mxifa_read_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size);
// Проверка окончания чтения
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// abort - немедленное завершение с отменой операции
// Возврат - операция окончена
irs_bool mxifa_read_end(void *pchdata, irs_bool abort);
// Чтение типа канала
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// Возврат - тип канала
mxifa_ei_t mxifa_get_channel_type(void *pchdata);
// Установка конфигурации канала
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// config - указатель на структуру ..._cfg соответствующую данному типу канала
void mxifa_set_config(void *pchdata, void *config);
// Чтение конфигурации канала
// config - указатель на структуру ..._cfg соответствующую данному типу канала
void mxifa_get_config(void *pchdata, void *config);

// Версия 0.4
// Чтение принятых данных немедленно
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// dest - источник данных, если IRS_NULL, то используется источник канала
//   по умолчанию
// buf - укзатель на буфер для приема
// size - размер буфера в байтах
// Возврат - количество реально принятых данных
mxifa_sz_t mxifa_fast_read(void *pchdata, mxifa_dest_t *dest, irs_u8 *buf,
  mxifa_sz_t size);
// Блокирующее открытие channel c кофигурированием
// channel - номер канала, занимается в специальной таблице для каждой программы
// is_broadcast - открытие канала в широковещательном режиме
// config - указатель на структуру ..._cfg соответствующую данному типу канала
// Возврат - указатель на данные канала
void *mxifa_open_ex(mxifa_ch_t channel, void *config, irs_bool is_broadcast);
// Неблокирующее открытие channel с конфигурированием
// channel - номер канала, занимается в специальной таблице для каждой программы
// is_broadcast - открытие канала в широковещательном режиме
// config - указатель на структуру ..._cfg соответствующую данному типу канала
// Возврат - указатель на данные канала
void *mxifa_open_begin_ex(mxifa_ch_t channel, void *config,
  irs_bool is_broadcast);
// Проверка окончания открытия channel с кофигурированием
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// abort - немедленное завершение с отменой операции
// Возврат - операция окончена
irs_bool mxifa_open_end_ex(void *pchdata, bool abort);
// Чтение типа канала по номеру
// pchdata - указатель на данные канала, возвращаемые mxifa_open или
//   mxifa_open_begin
// Возврат - тип канала
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel);
#endif //defined(__WATCOMC__)

#endif //MXIFAH
