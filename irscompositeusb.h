// @brief описание классов для работы с композиционным usb.
//
// Дата: 26.07.2021
// Дата создания: 23.05.2021

#pragma once

#ifndef COMPOSITE_USB_H
#define COMPOSITE_USB_H

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>
#include <irserror.h>

#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_conf.h>

#include <usbd_core.h>
#include <usbd_def.h>

#ifndef USB_CONFIG_DESC_SIZE
#define USB_CONFIG_DESC_SIZE 0x09
#endif // USB_CONFIG_DESC_SIZE

#ifndef USB_MAX_PACKET_SIZE
#define USB_MAX_PACKET_SIZE 0x64U
#endif // USB_MAX_PACKET_SIZE

/* Тег для непротестированных объектов */
#define _NO_TESTED_

/* Тег для объектов, для которых не написана реализация */
#define _NO_IMPL_

#define MAX_ENDPOINTS 16

/* Конечная точка активна (используется) */
#define EP_IS_USED 1U

namespace irs
{

/* Главная структура, объединяющая все модули */
typedef struct
{
  USBD_HandleTypeDef device;

  USBD_HandleTypeDef** modules;
  size_t count_modules;

  irs_u16 vid;
  irs_u16 pid;
  irs_u16 langid;

  irs_u8* manufacturer_str;
  irs_u8* product_str;
  irs_u8* config_str;
  irs_u8* interface_str;
  irs_u8* serial_str;

  irs_u8 buffer_str[USBD_MAX_STR_DESC_SIZ];

  irs_u8 mb_attributes;
  irs_u8 max_power;

} composite_usb_t;

enum TYPE_SPEED
{
  HIGH,
  FULL,
  OTHER
};

composite_usb_t composite_usb;

/* Функции для класса */
static irs_u8 init(USBD_HandleTypeDef* ap_dev, irs_u8 a_cfgidx);
static irs_u8 deinit(USBD_HandleTypeDef* ap_dev, irs_u8 a_cfgidx);

_NO_TESTED_ static irs_u8 setup(USBD_HandleTypeDef* ap_dev, USBD_SetupReqTypedef* ap_req);
_NO_TESTED_ static irs_u8 ep0_tx_sent(USBD_HandleTypeDef* ap_dev);
_NO_TESTED_ static irs_u8 ep0_rx_ready(USBD_HandleTypeDef* ap_dev);

_NO_TESTED_ static irs_u8 data_in(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum);
_NO_TESTED_ static irs_u8 data_out(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum);
_NO_IMPL_ static irs_u8 sof(USBD_HandleTypeDef* ap_dev);
_NO_IMPL_ static irs_u8 iso_in_incomplete(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum);
_NO_IMPL_ static irs_u8 iso_out_incomplete(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum);

static irs_u8* get_hs_config_desc(irs_u16* ap_length);
static irs_u8* get_fs_config_desc(irs_u16* ap_length);
static irs_u8* get_other_speed_config_desc(irs_u16* ap_length);
static irs_u8* get_device_qualifier_desc(irs_u16* ap_length);

/* Функции для дескриптора */

static irs_u8* get_device_desc(USBD_SpeedTypeDef speed, irs_u16* length);
static irs_u8* get_lang_id_str_desc(USBD_SpeedTypeDef speed, irs_u16* length);
static irs_u8* get_manufacturer_str_desc(USBD_SpeedTypeDef speed, irs_u16* length);
static irs_u8* get_product_str_desc(USBD_SpeedTypeDef speed, irs_u16* length);
static irs_u8* get_serial_str_desc(USBD_SpeedTypeDef speed, irs_u16* length);
static irs_u8* get_configuration_str_desc(USBD_SpeedTypeDef speed, irs_u16* length);
static irs_u8* get_interface_str_desc(USBD_SpeedTypeDef speed, irs_u16* length);

void init_composite_usb(USBD_HandleTypeDef** ap_handles, size_t a_count_handles);

static irs_u8 init(USBD_HandleTypeDef* ap_dev, irs_u8 a_cfgidx)
{
  irs_u8 ret = USBD_OK;

  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    if (composite_usb.modules[i]->pClass) {
      ret = composite_usb.modules[i]->pClass->Init(ap_dev, a_cfgidx);
      if (ret != USBD_OK) { return ret; }
    }
  }

  return ret;
}

static irs_u8 deinit(USBD_HandleTypeDef* ap_dev, irs_u8 a_cfgidx)
{
  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    if (composite_usb.modules[i]->pClass) {
      composite_usb.modules[i]->pClass->DeInit(ap_dev, a_cfgidx);
    }
  }

  if (composite_usb.device.pClass) {
    delete composite_usb.device.pClass;
  }

  static irs_u8* desc = get_hs_config_desc(nullptr);
  if (desc) { delete desc; }

  desc = get_fs_config_desc(nullptr);
  if (desc) { delete desc; }

  desc = get_other_speed_config_desc(nullptr);
  if (desc) { delete desc; }

  return USBD_OK;
}

_NO_TESTED_ static irs_u8 setup(USBD_HandleTypeDef* ap_dev, USBD_SetupReqTypedef* ap_req)
{
  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    /* Сравниваем номера индекстов интерфейсов. i = index of interface */
    if ((ap_req->bmRequest & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE &&
         ap_req->wIndex == i) {

      if (composite_usb.modules[i]->pClass) {
        return composite_usb.modules[i]->pClass->Setup(ap_dev, ap_req);
      }
    }

    for (size_t j = 0; j < MAX_ENDPOINTS; j++) {
      if ((ap_req->bmRequest & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT &&
          (ap_req->wIndex & 0x7F) == j &&
          composite_usb.modules[i]->ep_in[j].is_used == EP_IS_USED) {

        if (composite_usb.modules[i]->pClass) {
          return composite_usb.modules[i]->pClass->Setup(ap_dev, ap_req);
        }
      }
    }
  }

  return USBD_FAIL;
}

_NO_TESTED_ static irs_u8 ep0_tx_sent(USBD_HandleTypeDef* ap_dev)
{
  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    if (composite_usb.modules[i]->pClass) {
      if (composite_usb.modules[i]->pClass->EP0_RxReady) {
        return composite_usb.modules[i]->pClass->EP0_TxSent(ap_dev);
      }
    }
  }
  
  return USBD_OK;
}

_NO_TESTED_ static irs_u8 ep0_rx_ready(USBD_HandleTypeDef* ap_dev)
{
  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    if (composite_usb.modules[i]->pClass) {
      if (composite_usb.modules[i]->pClass->EP0_RxReady) {
        return composite_usb.modules[i]->pClass->EP0_RxReady(ap_dev);
      }
    }
  }

  return USBD_OK;
}

_NO_TESTED_ static irs_u8 data_in(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum)
{
  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    for (size_t j = 0; j < MAX_ENDPOINTS; j++) {
      if (a_epnum == j &&
          composite_usb.modules[i]->ep_in[j].is_used == EP_IS_USED) {

        if (composite_usb.modules[i]->pClass) {
          return composite_usb.modules[i]->pClass->DataIn(ap_dev, a_epnum);
        }
      }
    }
  }

  return USBD_FAIL;
}

_NO_TESTED_ static irs_u8 data_out(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum)
{
  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    for (size_t j = 0; j < MAX_ENDPOINTS; j++) {
      if (a_epnum == j &&
          composite_usb.modules[i]->ep_in[j].is_used == EP_IS_USED) {

        if (composite_usb.modules[i]->pClass) {
          return composite_usb.modules[i]->pClass->DataOut(ap_dev, a_epnum);
        }
      }
    }
  }

  return USBD_FAIL;
}

_NO_IMPL_ static irs_u8 sof(USBD_HandleTypeDef* ap_dev)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 iso_in_incomplete(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 iso_out_incomplete(USBD_HandleTypeDef* ap_dev, irs_u8 a_epnum)
{
  return USBD_OK;
}

static irs_u8* get_hs_config_desc(irs_u16* ap_length)
{
  static irs_u8* desc = nullptr;

  if (!desc) {
    irs_u16 size_desc = 0;

    irs_u8* temp = nullptr;
    irs_u16 length = 0;
    irs_u16 sum_length = 0;

    for (size_t i = 0; i < composite_usb.count_modules; i++) {
      if (composite_usb.modules[i]->pClass) {
        (void)composite_usb.modules[i]->pClass->GetHSConfigDescriptor(&length);

        /**
         * Суммируем размерность промежуточного конфига,
         * вычитая первые 9 байт дескриптора конфигурации
         */
        if (length > USB_CONFIG_DESC_SIZE) {
          sum_length += length - USB_CONFIG_DESC_SIZE;
        }
      }
    }

    size_desc = sum_length + USB_CONFIG_DESC_SIZE;

    if (ap_length) {
      *ap_length = size_desc;
    }

    desc = new irs_u8[size_desc];

    desc[0] = USB_CONFIG_DESC_SIZE; /* bLength: Configuration Descriptor size */
    desc[1] = USB_DESC_TYPE_CONFIGURATION; /* bDescriptorType */
    desc[2] = LOBYTE(size_desc); /* wTotalLength */
    desc[3] = HIBYTE(size_desc);
    desc[4] = composite_usb.count_modules; /* bNumInterfaces */
    desc[5] = 0x01; /* bConfigurationValue */
    desc[6] = 0x00; /* iConfiguration: Index of string descriptor describing the configuration */
    desc[7] = composite_usb.mb_attributes; /* mbAttributes: bus powered and Supports Remote Wakeup */
    desc[8] = composite_usb.max_power; /* MaxPower: this current is used for detectring Vbus */

    size_t i_desc = 0x09;
    for (size_t i = 0; i < composite_usb.count_modules; i++) {
      if (composite_usb.modules[i]->pClass) {
        temp = composite_usb.modules[i]->pClass->GetHSConfigDescriptor(&length);

        if (temp) {
          for (size_t j = 0x09; j < length; j++) {
            desc[i_desc++] = temp[j];
          }
        }
      }
    }
  }

  return desc;
}

static irs_u8* get_fs_config_desc(irs_u16* ap_length)
{
  static irs_u8* desc = nullptr;

  if (!desc) {
    irs_u16 size_desc = 0;

    irs_u8* temp = nullptr;
    irs_u16 length = 0;
    irs_u16 sum_length = 0;

    for (size_t i = 0; i < composite_usb.count_modules; i++) {
      if (composite_usb.modules[i]->pClass) {
        (void)composite_usb.modules[i]->pClass->GetFSConfigDescriptor(&length);

        /**
         * Суммируем размерность промежуточного конфига,
         * вычитая первые 9 байт дескриптора конфигурации
         */
        if (length > USB_CONFIG_DESC_SIZE) {
          sum_length += length - USB_CONFIG_DESC_SIZE;
        }
      }
    }

    size_desc = sum_length + USB_CONFIG_DESC_SIZE;

    if (ap_length) {
      *ap_length = size_desc;
    }

    desc = new irs_u8[size_desc];

    desc[0] = USB_CONFIG_DESC_SIZE; /* bLength: Configuration Descriptor size */
    desc[1] = USB_DESC_TYPE_CONFIGURATION; /* bDescriptorType */
    desc[2] = LOBYTE(size_desc); /* wTotalLength */
    desc[3] = HIBYTE(size_desc);
    desc[4] = composite_usb.count_modules; /* bNumInterfaces */
    desc[5] = 0x01; /* bConfigurationValue */
    desc[6] = 0x00; /* iConfiguration: Index of string descriptor describing the configuration */
    desc[7] = composite_usb.mb_attributes; /* mbAttributes: bus powered and Supports Remote Wakeup */
    desc[8] = composite_usb.max_power; /* MaxPower: this current is used for detectring Vbus */

    /* TODO: В случае если считываем CDC конфигурацию, необходимо добавлять
       дескриптор ассоциации интерфейсов. */
    size_t i_desc = 0x09;
    for (size_t i = 0; i < composite_usb.count_modules; i++) {
      if (composite_usb.modules[i]->pClass) {
        temp = composite_usb.modules[i]->pClass->GetFSConfigDescriptor(&length);

        if (temp) {
          for (size_t j = 0x09; j < length; j++) {
            desc[i_desc++] = temp[j];
          }
        }
      }
    }
  }

  return desc;
}

static irs_u8* get_other_speed_config_desc(irs_u16* ap_length)
{ 
  static irs_u8* desc = nullptr;

  if (!desc) {
    irs_u16 size_desc = 0;

    irs_u8* temp = nullptr;
    irs_u16 length = 0;
    irs_u16 sum_length = 0;

    for (size_t i = 0; i < composite_usb.count_modules; i++) {
      if (composite_usb.modules[i]->pClass) {
        (void)composite_usb.modules[i]->pClass->GetOtherSpeedConfigDescriptor(&length);

        /**
         * Суммируем размерность промежуточного конфига,
         * вычитая первые 9 байт дескриптора конфигурации
         */
        if (length > USB_CONFIG_DESC_SIZE) {
          sum_length += length - USB_CONFIG_DESC_SIZE;
        }
      }
    }

    size_desc = sum_length + USB_CONFIG_DESC_SIZE;

    if (ap_length) {
      *ap_length = size_desc;
    }

    desc = new irs_u8[size_desc];

    desc[0] = USB_CONFIG_DESC_SIZE; /* bLength: Configuration Descriptor size */
    desc[1] = USB_DESC_TYPE_CONFIGURATION; /* bDescriptorType */
    desc[2] = LOBYTE(size_desc); /* wTotalLength */
    desc[3] = HIBYTE(size_desc);
    desc[4] = composite_usb.count_modules; /* bNumInterfaces */
    desc[5] = 0x01; /* bConfigurationValue */
    desc[6] = 0x00; /* iConfiguration: Index of string descriptor describing the configuration */
    desc[7] = composite_usb.mb_attributes; /* mbAttributes: bus powered and Supports Remote Wakeup */
    desc[8] = composite_usb.max_power; /* MaxPower: this current is used for detectring Vbus */

    size_t i_desc = 0x09;
    for (size_t i = 0; i < composite_usb.count_modules; i++) {
      if (composite_usb.modules[i]->pClass) {
        temp = composite_usb.modules[i]->pClass->GetOtherSpeedConfigDescriptor(&length);

        if (temp) {
          for (size_t j = 0x09; j < length; j++) {
            desc[i_desc++] = temp[j];
          }
        }
      }
    }
  }

  return desc; 
}

static irs_u8* get_device_qualifier_desc(irs_u16* ap_length)
{
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
  #endif
  static irs_u8 qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] =
  {
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    USB_MAX_PACKET_SIZE,
    0x01,
    0x00,
  };

  *ap_length = sizeof(qualifier_desc);
  return static_cast<irs_u8*>(qualifier_desc);
}

static irs_u8* get_device_desc(USBD_SpeedTypeDef speed, irs_u16* length)
{
  UNUSED(speed);

  /* USB Standard Device Descriptor */
  #if defined ( __ICCARM__ )      /* !< IAR Compiler */
  #pragma data_alignment=4
  #endif
  __ALIGN_BEGIN static irs_u8 device_desc[USB_LEN_DEV_DESC] __ALIGN_END = {
    0x12,                         /* bLength */
    USB_DESC_TYPE_DEVICE,         /* bDescriptorType */
    0x00,                         /* bcdUSB */
    0x02,
    0xEF,                         /* bDeviceClass */
    0x02,                         /* bDeviceSubClass */
    0x01,                         /* bDeviceProtocol */
    USB_MAX_EP0_SIZE,             /* bMaxPacketSize */
    LOBYTE(composite_usb.vid),    /* idVendor */
    HIBYTE(composite_usb.vid),    /* idVendor */
    LOBYTE(composite_usb.pid),    /* idVendor */
    HIBYTE(composite_usb.pid),    /* idVendor */
    0x00,                         /* bcdDevice rel. 2.00 */
    0x02,
    USBD_IDX_MFC_STR,             /* Index of manufacturer string */
    USBD_IDX_PRODUCT_STR,         /* Index of product string */
    USBD_IDX_SERIAL_STR,          /* Index of serial number string */
    USBD_MAX_NUM_CONFIGURATION    /* bNumConfigurations */
  };                              /* USB_DeviceDescriptor */

  *length = sizeof(device_desc);
  return static_cast<irs_u8*>(device_desc);
}

static irs_u8* get_lang_id_str_desc(USBD_SpeedTypeDef speed, irs_u16* length)
{
  UNUSED(speed);

  #if defined ( __ICCARM__ )      /* !< IAR Compiler */
  #pragma data_alignment=4
  #endif
  __ALIGN_BEGIN static irs_u8 langid_desc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
    USB_LEN_LANGID_STR_DESC,
    USB_DESC_TYPE_STRING,
    LOBYTE(composite_usb.langid),
    HIBYTE(composite_usb.langid),
  };

  *length = sizeof(langid_desc);
  return static_cast<irs_u8*>(langid_desc);
}

static irs_u8* get_manufacturer_str_desc(USBD_SpeedTypeDef speed, irs_u16* length)
{
  UNUSED(speed);

  USBD_GetString(composite_usb.manufacturer_str, composite_usb.buffer_str, length);
  return composite_usb.buffer_str;
}

static irs_u8* get_product_str_desc(USBD_SpeedTypeDef speed, irs_u16* length)
{
  UNUSED(speed);

  USBD_GetString(composite_usb.product_str, composite_usb.buffer_str, length);
  return composite_usb.buffer_str;
}

static irs_u8* get_serial_str_desc(USBD_SpeedTypeDef speed, irs_u16* length)
{
  UNUSED(speed);

  USBD_GetString(composite_usb.serial_str, composite_usb.buffer_str, length);
  return composite_usb.buffer_str;
}

static irs_u8* get_configuration_str_desc(USBD_SpeedTypeDef speed, irs_u16* length)
{
  UNUSED(speed);

  USBD_GetString(composite_usb.config_str, composite_usb.buffer_str, length);
  return composite_usb.buffer_str;
}

static irs_u8* get_interface_str_desc(USBD_SpeedTypeDef speed, irs_u16* length)
{
  UNUSED(speed);

  USBD_GetString(composite_usb.interface_str, composite_usb.buffer_str, length);
  return composite_usb.buffer_str;
}

void init_composite_usb(USBD_HandleTypeDef** ap_modules, size_t a_count_modules)
{
  composite_usb.modules = ap_modules;
  composite_usb.count_modules = a_count_modules;

  if (composite_usb.device.pDesc) {
    /* Уверены в том, что инициализация нашего девайса
       происходило с помощью функции init_composite_usb */
    delete composite_usb.device.pDesc;
    composite_usb.device.pData = nullptr;
  }

  if (composite_usb.device.pClass) {
    /* Уверены в том, что инициализация нашего девайса
       происходило с помощью функции init_composite_usb */
    delete composite_usb.device.pClass;
    composite_usb.device.pClass = nullptr;
  }

  USBD_ClassTypeDef* pclass = new USBD_ClassTypeDef {
    init,
    deinit,

    /* Control Endpints */
    setup,
    ep0_tx_sent,
    ep0_rx_ready,

    /* Class Specific Endpoints */
    data_in,
    data_out,
    sof,
    iso_in_incomplete,
    iso_out_incomplete,

    get_hs_config_desc,
    get_fs_config_desc,
    get_other_speed_config_desc,
    get_device_qualifier_desc,
  };

  USBD_DescriptorsTypeDef* pdesc = new USBD_DescriptorsTypeDef {
    get_device_desc,
    get_lang_id_str_desc,
    get_manufacturer_str_desc,
    get_product_str_desc,
    get_serial_str_desc,
    get_configuration_str_desc,
    get_interface_str_desc,
  };

  irs_u8 id_desc = 0;

  USBD_Init(&composite_usb.device, pdesc, id_desc);
  USBD_RegisterClass(&composite_usb.device, pclass);
}

} // namespace irs

#endif // COMPOSITE_USB_H
