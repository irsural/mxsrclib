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

/* Выравнивание структур данных для IAR */
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif

namespace irs
{

#define IRS_MAX_MODULES 6

// TODO: Определить значения по умолчанию для нижних дефайнов
#define IRS_VENDOR_ID
#define IRS_PRODUCT_ID

/* <---------ДЕСКРИПТОРЫ--------->  */

/*
 * Для работы USB устройства необходимо реализовать три дескриптора:
 * 1. Дескриптор устройства
 * 2. Дескриптор конфигурации (описывает возможности устройства)
 * 3. Дескриптор репорта (описывает протокол)
 *
 * Описание всех дескрипторов производиться в 16-ричной системе c типом данных irs_u8 (uint8).
 * Описание дескрипторов производится с помощью массива irs_u8, но для удобности программирования
 * было принято решение использовать структуры и классы.
 */

 /* Размерности дескрипторов:
  * #define  USB_LEN_DEV_QUALIFIER_DESC                     0x0AU
  * #define  USB_LEN_DEV_DESC                               0x12U
  * #define  USB_LEN_CFG_DESC                               0x09U
  * #define  USB_LEN_IF_DESC                                0x09U
  * #define  USB_LEN_EP_DESC                                0x07U
  * #define  USB_LEN_OTG_DESC                               0x03U
  * #define  USB_LEN_LANGID_STR_DESC                        0x04U
  * #define  USB_LEN_OTHER_SPEED_DESC_SIZ                   0x09U
  */

struct basic_desc
{
  /* Общая длина дескриптора */
  irs_u8 length;

  /* Тип дескриптора */
  irs_u8 desc_type;
};

struct desc_string_handshake : public basic_desc
{
  /* Значения по умолчанию. */
  length = USB_LEN_LANGID_STR_DESC;
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
  /* Версия класса */
  irs_u16 bcd;

  /* Код страны */
  irs_u8 contry_code;

  /* Количество дескрипторов типа report */
  irs_u8 count_reports;
};

struct desc_report : public basic_desc
{
  /* TODO: Описать структуру для report дескриптора */
};

struct desc_endpoint : public basic_desc
{
  /* Значения по умолчанию */
  length = USB_LEN_EP_DESC;
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

  /* Максимальный размер пакета этой конечной точки,
   * подходящий для отправки или приема.
   */
  irs_u16 max_packet_size;

  /* Интервал для того, чтобы опросить передачи данных конечной точки.
   * Указывается в количестве фейрмов. Поле игнорируется для конечных точек
   * Bulk и Control. Для конечных точек Isochronous должно быть равно 1 и
   * для конечных точек interrupt может лежать в диапазоне 1..255. */
  irs_u8 interval_polling;
};

struct desc_interface : public basic_desc
{
  /* Значения по умолчанию */
  length = USB_LEN_IF_DESC;
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
  /* Значения по умолчанию.
   * Из length вычитается 3, поскольку вместо разделения bcd_usb, vendor_id, product_id
   * на младшие и старшие биты посредству использования irs_u8 используется irs_u16.
   */
  length = USB_LEN_DEV_DESC - 3;
  desc_type = USB_DESC_TYPE_DEVICE;

  /* Номер спецификации USB, с к-ой совместимо устройство. */
  irs_u16 bcd_usb;

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

  /* Идентификатор вендора (VID)
   * (назначается организацией USB Org).
   */
  irs_u16 vendor_id;

  /* Идентификатор  продукта (PID)
   * (назначается организацией - производителем)
   */
  irs_u16 product_id;

  /* Номер версии устройства (задается разработчиком устройства). */
  irs_u16 version;

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
  length = USB_LEN_CFG_DESC;
  desc_type = USB_DESC_TYPE_CONFIGURATION;

  /* Полная длина возвращаемых данных в байтах.
   * (общий размер всего деерва под данной конфигурацией)
   */
  irs_u16 total_length;

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

/* <---------МОДУЛЬ--------->  */

class basic_usb_module
{
public:
  friend class usb_composite_device;

  basic_usb_module(desc_configuration* desc_conf = nullptr);
  ~basic_usb_module();

  desc_configuration& get_desc_conf();

private:
  /* TODO: Конфигурация определяется модулем или девайсом? */
  desc_configuration m_desc_conf;
};

basic_usb_module::basic_usb_module(desc_configuration* desc_conf)
{
  if (desc_conf) { m_desc_conf = *desc_conf; }
}

basic_usb_module::~basic_usb_module()
{}

desc_configuration basic_usb_module::get_desc_conf()
{ return m_desc_conf; }

/* <---------КОМПОЗИТНОЕ УСТРОЙСТВО--------->  */

class usb_composite_device
{
  static usb_composite_device* get_instance();

  bool start();
  void stop();

  /* Функция очистки подключенных модулей */
  void clear();

  desc_device& get_desc_device();
  desc_configuration& get_desc_conf();

  string get_manufacturer() const;
  string get_product() const;
  string get_serial_number() const;

  void set_manufacturer(string manufacturer);
  void set_product(string product);
  void set_serial_number(string serial_number);

  /* Delay в миллисекундах */
  void set_disconnect_delay(irs_u32 delay = 500);

  /* Проверка на готовность работы устройства */
  bool is_ready() const;

  /* Функция добавления нового модуля */
  bool add_module(usb_module* module);

private:
  usb_composite_device();
  ~usb_composite_device();

  irs_u8 serialise_desc_device(desc_device* device);
  irs_u8 serialise_desc_conf(desc_configuration* conf);

  static usb_composite_device* mp_instance;

  list<basic_usb_module*> m_modules_list;
  
  desc_device m_desc_device;
  desc_configuration m_desc_conf;

  string m_manufacturer;
  string m_product;
  string m_serial_number;

  bool m_is_enabled;
};

usb_composite_device::usb_composite_device()
  : m_desc_device.vendor_id(IRS_VENDOR_ID)
  , m_desc_device.product_id(IRS_PRODUCT_ID)
{
  // TODO: Нужна ли здесь инициализация модуля USB?
}

usb_composite_device::~usb_composite_device()
{ if (mp_instance) { delete mp_instance; } }

void usb_composite_device::start()
{
  /* TODO: Реализовать функцию запуска модулей */
}

void usb_composite_device::stop()
{
  /* TODO: Реализовать функцию оставноки модулей */
}

usb_composite_device* usb_composite_device::get_instance()
{
  if (!mp_instance) { mp_instance = new usb_composite_device(); }
  return mp_instance;
}

desc_device& usb_composite_device::get_desc_device()
{ return m_desc_device; }

desc_configuration& usb_composite_device::get_desc_conf()
{ return m_desc_conf; }

string usb_composite_device::get_manufacturer() const
{ return m_manufacturer; }

string usb_composite_device::get_product() const
{ return m_product; }

string usb_composite_device::get_serial_number() const
{ return serial_number; }

void usb_composite_device::set_manufacturer(string manufacturer)
{ m_manufacturer = manufacturer; }

void usb_composite_device::set_product(string product)
{ m_product = product; }

void usb_composite_device::set_serial_number(string serial_number)
{ m_serial_number = serial_number; }

irs_u8* usb_composite_device::serialise_desc_device(desc_device* device)
{
  if (device) {
    __ALIGN_BEGIN irs_u8* usbd_device_desc __ALIGN_END = new irs_u8[device->length];

    usbd_device_desc[0] = device->length;
    usbd_device_desc[1] = device->type;
    usbd_device_desc[2] = device->bcd_usb;
    usbd_device_desc[3] = device->iclass;
    usbd_device_desc[4] = device->subclass;
    usbd_device_desc[5] = device->protocol;
    usbd_device_desc[6] = device->vendor_id;
    usbd_device_desc[7] = device->product_id;
    usbd_device_desc[8] = device->bcd;
    usbd_device_desc[9] = device->index_manufacturer;
    usbd_device_desc[10] = device->index_product;
    usbd_device_desc[11] = device->index_serial;
    usbd_device_desc[12] = device->max_configuration_size;

    return usbd_device_desc;
  }

  return nullptr;
}

irs_u8* usb_composite_device::serialise_desc_conf(desc_configuration* conf)
{
  if(conf) {
    /* TODO: Решить, каким образом проводить сериализацию конфигурации,
     * поскольку необходимо в один массив запихнуть данные о конфиге,
     * данные об интерфейсах, о конечных точках интерфейсов. 
     */
  }

  return nullptr;
};

void usb_composite_device::clear()
{ m_modules_list.clear(); }

void usb_composite_device::add_module(usb_module* module)
{
  if (m_modules_list.size() < IRS_MAX_MODULES) {
    if (m_is_enabled) { this->stop(); }
    m_modules_list.add(module);
  }

  /* TODO: Добавить инициализацию модуля */
}

} // namespace irs

#endif // COMPOSITE_USB_H
