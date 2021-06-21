// @brief описание классов для работы с композиционным usb.
//
// Дата: 21.06.2021
// Дата создания: 23.05.2021

#pragma once

#ifndef COMPOSITE_USB_H
#define COMPOSITE_USB_H

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>
#include <irserror.h>
#include <irsusbgeneric.h>

#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_conf.h>

#include <usbd_core.h>

namespace irs
{

#define IRS_MAX_MODULES 6

// TODO: Определить значения по умолчанию для нижних дефайнов
#define IRS_VENDOR_ID
#define IRS_PRODUCT_ID

/*
 * Для работы USB устройства необходимо реализовать три дескриптора:
 * 1. Дескриптор устройства
 * 2. Дескриптор конфигурации (описывает возможности устройства)
 * 3. Дескриптор репорта (описывает протокол)
 *
 * Описание всех дескрипторов производиться в 16-ричной системе c типом данных irs_u8 (uint8).
 * Описание дескрипторов производится с помощью массива irs_u8, но для удобности программирования
 * было принято решение использовать структуры и классы.
 *
 * Описание дескрипторов:
 * - Дескриптор устройства:
 *   - irs_u8 length - общая длина дескриптора
 *   - irs_u8 descriptor_type - показывает, что за дескриптор – определяет его тип
 *   - irs_u8 lobyte_bcd_usb - младший байт версии usb, к-ую поддерживает устройство
 *   - irs_u8 hibyte_bcd_usb - страший байт версии usb, к-ую поддерживает устройство
 *   - irs_u8 device_class - класс по стандарту usb. Если указан 0x00, то класс отвечает сам за себя
 *   - irs_u8 device_subclass - сабкласс по стандарту usb. Если указан 0x00, то сабкласс отвечает сам за себя
 *   - irs_u8 device_protocol - протокол по стандарту usb. Если указан 0x00, то протокол отвечает сам за себя
 *   - irs_u8 max_packet_size - максимальный размер пакета для endpoint 0 (при конфигурировании)
 *   - irs_u8 lobyte_vid - младший байт vendor id
 *   - irs_u8 hibyte_vid - старший байт vendor id
 *   - irs_u8 lobyte_product_id - младший байт product id
 *   - irs_u8 hibyte_product_id - старший байт product id
 *   - irs_u8 lobyte_version - младший байт версии устройства (определяется программистом, как обычная версия программы)
 *   - irs_u8 hibyte_version - старший байт версии устройства (определяется программистом, как обычная версия программы)
 *   - irs_u8 index_manufacturer - индекс строки, описывающей производителя
 *   - irs_u8 index_product - индекс строки, описывающей продукт
 *   - irs_u8 index_serial - идекс строки, описывающей серию (продукта)
 * NOTE: индексы нужны для того, чтобы корректно работать с подключенными устройствами, у которых
 *       VID/PID одинаковы.
 *   - irs_u8 max_configuration_size - максимальное количество конфигураций
 *
 * - Дескриптор конфигурации (Последовательность описания: конфигурация, интерфейсы,
 *                            конечные точки (endpoints), классы, производитель (vendor)):
 *   - irs_u8 length - общая длина дескрипора конфигурации
 *   - irs_u8 configuration_type - тип дескриптора - конфигурация
 *   - irs_u8 lb_total_length - младший байт общего размера всего дерева под данной конфигурацией
 *   - irs_u8 hb_total_length - старший байт общего размера всего деерва под данной конфигурацией
 *   - irs_u8 count_interface - количество интерфейсов у данной конфигурации
 *   - irs_u8 configuration_value - индекс данной конфигурации
 *   - irs_u8 index_configuration - индекс строки, которая описывает данную конфигурацию
 *   - irs_u8 bm_attributes - признак того, что устройство будет питаться от шины USB
 *   - irs_u8 max_power - максимальное число ампер
 *   ---- Затем идет описание дескриптора интерфейса ----
 *   - irs_u8 interface_length - общая длина дескриптора интерфейса
 *   - irs_u8 interface_type - тип дескриптора – интерфейс
 *   - irs_u8 interface_number - порядковый номер интерфейса
 *   - irs_u8 interface_alternate_setting - признак альтернативного интерфейса
 *   - irs_u8 interface_num_endpoints - количество конечных точек
 *   - irs_u8 interface_class - класс интерфейса (например, HID)
 *   - irs_u8 interface_sublcass - сабкласс интерфейса
 *   - irs_u8 interface_protocol - протокол интерфейса
 * NOTE: если нужно показать, что данное устройство, к примеру, является мышкой, то необходимо указывать
 *       правильные класс и сабкласс.
 *   - irs_u8 interface_index - индекс строки, описывающей интерфейс
 *   ---- Затем идет описание дескриптора класса (если необходимо). В качестве примера, будет дескриптор HID ----
 *   - irs_u8 hid_length - общая длина дескриптора hid
 *   - irs_u8 hid_type - тип дескриптора - hid
 *   - irs_u8 lb_bcd_hid - младший байт версии hid
 *   - irs_u8 hb_bcd_hid - старший байт версии hid
 *   - irs_u8 hid_contry_code - код страны (если нужен)
 *   - irs_u8 hid_num_descriptors - сколько дальше будет report дескрипторов
 *   ---- Затем идет описание report дескрипторов, если необходимо ----
 *   - irs_u8 report_type - тип дескриптора - report
 *   - irs_u8 report_length - длина report дескриптора
 *   ---- Затем идет описание дескриптора конечных точек (endpoints) ----
 *   - irs_u8 endpoint_length - длина дескриптора конечных точек
 *   - irs_u8 endpoint_type - тип дескриптора - endpoint
 *   - irs_u8 endpoint_address - адрес конечной точки и направление 1 (IN) (be like 0x81)
 *   - irs_u8 endpoint_attributes - тип конечной точки
 *   - irs_u8 lb_max_packet_size - младший байт максимального размера пакета
 *   - irs_u8 hb_max_packet_size - старший байт максимального размера пакета
 * NOTE: по стандарту больше 0x40 указывать для размера пакета не стоит.
 *       Если передаваемый пакет будет отличаться по размеру, то будут проблемы!
 *   - irs_u8 endpoint_interval_polling - интервал опроса
 *   ---- Затем продолжается описание конечных точек также, как было определено описание предыдущей конечной точки ----
 *   - ...
 *
 * - Дескриптор репорта (далее для простоты понимания вместо названий переменных
 *                       будут указываться пары чисел с их описанием.
 *                       Числа для примера взяты из проекта по написанию custrom-hid
 *                       и их можно менять под свои задачи):
 *   - 0x06, 0x00, 0xff - usage_page (generic desktop)
 *   - 0x09, 0x01 - usage (vendor usage 1)
 *   - 0xa1, 0x01 - collection (application)
 *   - 0x85, 0x01 - report_id (1)
 *   - 0x09, 0x01 - usage (vendor usage 1)
 *   - 0x15, 0x00 - logical_minimum (0)
 *   - 0x25, 0x01 - logical_maximum (1)
 *   - 0x75, 0x08 - report_size (8)
 *   - 0x95, 0x01 - report_count (1)
 *   - 0xb1, 0x82 - feature (data, var, abs, vol)
 *   - 0x85, 0x01 - report_id (1)
 *   - 0x09, 0x01 - usage (vendor usage 1)
 *   - 0x91, 0x82 - output (data, var, abs, vol)
 *
 *   - 0x85, 0x02 - report_id (2)
 *   - 0x09, 0x02 - usage (vendor usage 2)
 *   - 0x15, 0x00 - logical_minimum (0)
 *   - 0x25, 0x01 - logical_maximum (1)
 *   - 0x75, 0x08 - report_size (8)
 *   - 0x95, 0x01 - report_count (1)
 *   - 0xb1, 0x82 - feature (data, var, abs, vol)
 *   - 0x85, 0x02 - report_id (2)
 *   - 0x09, 0x02 - usage (vendor usage 2)
 *   - 0x91, 0x82 - output (data, var, abs, vol)
 *
 *   - 0x85, 0x03 - report_id (3)
 *   - 0x09, 0x03 - usage (vendor usage 3)
 *   - 0x15, 0x00 - logical_minimum (0)
 *   - 0x26, 0xff, 0x00 - logical_maximum (255)
 *   - 0x75, 0x08 - report_size (8)
 *   - 0x95, RPT3_COUNT - report_count (N, RPT3_COUNT - некоторая константа)
 *   - 0xb1, 0x82 - feature (data, var, abs, vol)
 *   - 0x85, 0x03 - report_id (3)
 *   - 0x09, 0x03 - usage (vendor usage 3)
 *   - 0x91, 0x82 - output (data, var, abs, vol)
 *
 *   - 0x85, 0x04 - report_id (4)
 *   - 0x09, 0x04 - usage (vendor usage 4)
 *   - 0x75, 0x08 - report_size (8)
 *   - 0x95, RPT4_COUNT - report_count (N, RPT4_COUNT - некоторая константа)
 *   - 0x81, 0x82 - input (data, var, abs, vol)
 *   - 0xc0 - end_collection
 */

/* TODO: Упростить работу с парами переменных старший/младший байты
*       посредству написания приватной функции деления числа на старшую часть и младшую
*/

struct basic_desc
{
  irs_u8 length; // общая длина дескриптора
  irs_u8 desc_type; // показывает, что за дескриптор – определяет его тип
};

struct desc_string_handshake : public basic_desc
{
  /* Значения по умолчанию. */
  desc_type = USB_DESC_TYPE_STRING;

  /* Кодs поддерживаемых языков. */
  list<irs_u16> langids;
};

struct desc_string : public basic_desc
{
  /* Значения по умолчанию. */
  desc_type = USB_DESC_TYPE_STRING;  

  /* Строка, закодированная в Unicode. */
  string str;
};

struct desc_class : public basic_desc
{
  irs_u8 lb_bcd; // младший байт версии класса
  irs_u8 hb_bcd; // страший байт версии класса
  irs_u8 contry_code; // код страны
  irs_u8 count_reports; // сколько будет дескрипторов report
};

struct desc_report : public basic_desc
{
  /* TODO: Описатб структуру для report дескриптора */
};

struct desc_endpoint : public basic_desc
{
  /* Значения по умолчанию */
  length = 7;
  desc_type = USB_DESC_TYPE_ENDPOINT;

  /* Адрес конечной точки.
   * Биты 0..3 - номер конечной точки,
   * Биты 4..6 - зарезервированы, установлены в 0,
   * Бит 7 - направление 0 = Out, 1 = In (для конечных точек игнорируется).
   */
  irs_u8 address;

  /* Набор битов, определяющие тип конечной точки.
   * Биты 0..1 - тип передачи:
   *   - 00 - Control,
   *   - 01 - Isochronous,
   *   - 10 - Bulk,
   *   - 11 - Interrupt.
   * Биты 2..7 - зарезервированы. Если конечная точка изохронная, то:
   *   Биты 3..2 - тип синхронизации (режим Iso),
   *     00 - No synchronisation,
   *     01 - Asynchronous,
   *     10 - Adaptive,
   *     11 - Synchronous.
   *   Биты 5..4 - тип использования Usage Type (режим Iso):
   *     00 - коненая точка данных,
   *     01 - конечная точка обратной связи (Feedback Endpoint),
   *     10 - явная конечная точка обратной связи данных,
   *     11 - зарезервировано.
   */
  irs_u8 attributes;

  /* Младший байт максимального размера пакета этой конечной точки,
   * подходящий для отправки или приема.
   */
  irs_u8 lb_max_packet_size;

  /* Старший байт максимального размера пакета этой конечной точки,
   * подходящий для отправки или приема.
   */
  irs_u8 hb_max_packet_size;
  
  /* Интервал для того, чтобы опросить передачи данных конечной точки.
   * Указывается в количестве фейрмов. Поле игнорируется для конечных точек
   * Bulk и Control. Для конечных точек Isochronous должно быть равно 1 и
   * для конечных точек interrupt может лежать в диапазоне 1..255. */
  irs_u8 interval_polling;
};

struct desc_interface : public basic_desc
{
  /* Значения по умолчанию */
  length = 9;
  desc_type = USB_DESC_TYPE_INTERFACE;

  /* Индекс (порядковый номер) интерфейса. */
  irs_u8 number;

  /* Величина, используемая для выбора лальтернативной установки.
   * Нужен для того, чтобы показать, что настройка заимствуется
   * у другого интерфейса. В качестве значения указывается
   * индекс (порядковый номер) интерфейса, у к-ого заимствуется
   * настройка.
   */
  irs_u8 alternate_setting;

  /* Количество конечных точек, используемых в интерфейсе.
   * Конечная точка 0 не учитывается.
   */
  irs_u8 count_endpoints;

  /* Код класса (назначается организацией, следящей за стандартами USB). */
  irs_u8 iclass;

  /* Код подкласса (назначается организацией, следящей за стандартами USB). */
  irs_u8 subclass;

  /* Код протокола (назначается организацией, следящей за стандартами USB). */
  irs_u8 protocol;

  /* Индекс строкового дескриптора, описывающей этот интерфейс */
  irs_u8 index_str;

  /* Список конечных точек */
  list<desc_endpoint> endpoints;
};

struct desc_device : public basic_desc
{
  /* Значения по умолчанию. */
  length = 18;
  desc_type = USB_DESC_TYPE_DEVICE;

  /* Младший байт номера спецификации USB, с к-ой совместимо устройство. */
  irs_u8 lobyte_bcd_usb;

  /* Старший байт номера спецификации USB, с к-ой совместимо устройство. */
  irs_u8 hibyte_bcd_usb;

  /* Код класса (назначается организацией USB Org).
   * Если равно 0, то каждый интерфейс указывает свой собственный код класса.
   * Если равен 0xFF, то код класса определяяется вендором, иначе поле содержит
   * код стандартного класса.
   */
  irs_u8 device_class;

  /* Код подкласса (назначается организацией USB Org). */
  irs_u8 device_subclass;

  /* Код протокола (назначается организацией USB Org). */
  irs_u8 device_protocol;

  /* Максимальный размер пакета для конечной точки 0.
   * Допустимые размеры: 8, 16, 32, 64.
   */
  irs_u8 max_packet_size;

  /* Младший байт идентификатора вендора (VID)
   * (назначается организацией USB Org).
   */
  irs_u8 lobyte_vid;

  /* Старший байт идентификатора вендора (VID)
   * (назначается организацией USB Org).
   */
  irs_u8 hibyte_vid;

  /* Младший байт идентификатора продукта (PID)
   * (назначается организацией - производителем)
   */
  irs_u8 lobyte_product_id;

  /* Старший байт идентификатора продукта (PID)
   * (назначается организацией - производителем)
   */
  irs_u8 hibyte_product_id;

  /* Младший байт номера версии устройства (задается разработчиком устройства). */
  irs_u8 lobyte_version;

  /* Страший байт номера версии устройства (задается разработчиком устройства). */
  irs_u8 hibyte_version;

  /* Индекс строки, описывающей прозиводителя
   * (необязательное поле, можно указать 0)
   */
  irs_u8 index_manufacturer;

  /* Индекс строки, описывающей продукт
   * (необязательное поле, можно указать 0)
   */
  irs_u8 index_product;

  /* Индекс строки, содержащей серийный номер
   * (необязательное поле, можно указать 0)
   */
  irs_u8 index_serial;

  /* Количество возможных конфигураций */
  irs_u8 max_configuration_size;
};

struct desc_configuration : public basic_desc
{
  /* Значения по умолчанию */
  desc_type = USB_DESC_TYPE_CONFIGURATION;

  /* Младший байт полной длины возвращаемых данных в байтах.
   * (общий размер всего деерва под данной конфигурацией)
   */
  irs_u8 lb_total_length;

  /* Старший байт полной длины возвращаемых данных в байтах.
   * (общий размер всего деерва под данной конфигурацией)
   */
  irs_u8 hb_total_length;

  /* Количество интерфейсов. */
  irs_u8 count_interfaces;

  /* Величина, используемая как аргумент для выбора этой конфигурации. */
  irs_u8 value;

  /* Индекс строкового дескриптора, описывающего эту конфигурацию. */
  irs_u8 index_str;

  /* Признак того, что устройство будет питаться от шины USB.
   * D7 зарезервировано, установлено в 1. (USB 1.0 Bs Powered),
   * D6 самозапитываемое (Self Powered),
   * D5 удаленное пробуждение (Remote Wakeup),
   * D4..0 зарезервировано, установлено в 0.
   */
  irs_u8* bm_attributes;

  /* Максимальное энергопотребление в единицах 2 мА. */
  irs_u8* max_power;

  /* Список интерфейсов данной кофигурации. */
  list<desc_interface> interfaces;

  /* Список классов данной конфигурации. */
  list<desc_class> classes;
};

class basic_usb_module
{
public:
  basic_usb_module(desc_configuration desc_conf);
  ~basic_usb_module();

  desc_configuration get_desc_conf() const;

private:
  /* TODO: Конфигурация определяется модулем или девайсом? */
  desc_configuration m_desc_conf;
};

basic_usb_module::basic_usb_module(desc_configuration desc_conf)
  : m_desc_conf(desc_conf)
{}

basic_usb_module::~basic_usb_module()
{}

desc_configuration basic_usb_module::get_desc_conf() const
{ return m_desc_conf; }

class usb_composite_device
{
  static usb_composite_device* get_instance();

  void set_manufacturer(const string a_manufacturer);
  void set_product(const string a_product);
  void set_serial_number(const string a_serial_number);

  void set_vendor_id(const irs_u16 a_vendor_id);
  void set_product_id(const irs_u16 a_product_id);

  string get_manufacturer() const;
  string get_product() const;
  string get_serial_number() const;

  irs_u16 get_vendor_id() const;
  irs_u16 get_product_id() const;

  bool start();
  void stop();

  /* Функция очистки подключенных модулей */
  void clear();

  // В миллисекундах
  void set_disconnect_delay(irs_u32 delay = 500);

  bool is_ready() const;

  bool add_module(usb_module* module);

private:
  usb_composite_device();
  ~usb_composite_device();

  static usb_composite_device* mp_instance;

  string m_manufacturer;
  string m_product;
  string m_serial_number;

  irs_u16 m_vendor_id;
  irs_u16 m_product_id;

  bool m_is_enabled;

  list<basic_usb_module*> m_modules_list;
  desc_device m_desc_device;
};

usb_composite_device::usb_composite_device()
  : m_vendor_id(IRS_VENDOR_ID)
  , m_product_id(IRS_PRODUCT_ID)
{
  // TODO: Нужна ли здесь инициализация модуля USB?
}

usb_composite_device::~usb_composite_device()
{
  if (mp_instance) { delete mp_instance; }
}

usb_composite_device* usb_composite_device::get_instance()
{
  if (!mp_instance) { mp_instance = new usb_composite_device(); }
  return mp_instance;
}

void usb_composite_device::set_manufacturer(const string a_manufacturer)
{ m_manufacturer = a_manufacturer; }

void usb_composite_device::set_product(const string a_product)
{ m_product = a_product; }

void usb_composite_device::set_serial_number(const string a_serial_number)
{ m_serial_number = a_serial_number; }

void usb_composite_device::set_vendor_id(const string a_vendor_id)
{ m_vendor_id = a_vendor_id; }

void usb_composite_device::set_product_id(const string a_product_id)
{ m_product_id = a_product_id; }

void usb_composite_device::clear()
{ m_modules_list.clear(); }

} // namespace irs

#endif // COMPOSITE_USB_H
