// @brief описание классов для работы с композиционным usb.
//
// Дата: 19.07.2021
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
#include <usbd_def.h>

/* Тег для непротестированных объектов */
#define _NO_TESTED_

/* Тег для объектов, для которых не написана реализация */
#define _NO_IMPL_

namespace irs
{

typedef struct
{
/* Главная структура, объединяющая все модули */
USBD_HandleTypeDef device;

USBD_HandleTypeDef** modules;
size_t count_modules;

} composite_usb_t;

composite_usb_t composite_usb;

_NO_TESTED_ static irs_u8 init(USBD_HandleTypeDef* pdev, irs_u8 cfgidx)
{
  irs_u8 ret = USBD_OK; 

  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    ret = composite_usb.modules[i]->pClass->Init(pdev, cfgidx);
    if (ret != USBD_OK) { return ret; }
  }

  return ret;
}

_NO_TESTED_ static irs_u8 deinit(USBD_HandleTypeDef* pdev, irs_u8 cfgidx)
{
  for (size_t i = 0; i < composite_usb.count_modules; i++) {
    composite_usb.modules[i]->pClass->DeInit(pdev, cfgidx);
  }

  if (composite_usb.device.pClass) {
    delete composite_usb.device.pClass;
  }

  return USBD_OK;
}

_NO_IMPL_ static irs_u8 setup(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef  *req)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 ep0_tx_sent(USBD_HandleTypeDef* pdev)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 ep0_rx_ready(USBD_HandleTypeDef* pdev)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 data_in(USBD_HandleTypeDef* pdev, irs_u8 epnum)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 data_out(USBD_HandleTypeDef* pdev, irs_u8 epnum)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 sof(USBD_HandleTypeDef* pdev)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 iso_in_incomplete(USBD_HandleTypeDef* pdev, irs_u8 epnum)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8 iso_out_incomplete(USBD_HandleTypeDef* pdev, irs_u8 epnum)
{
  return USBD_OK;
}

_NO_IMPL_ static irs_u8* get_hs_config_desc(irs_u16* length)
{
  return NULL;
}

_NO_IMPL_ static irs_u8* get_fs_config_desc(irs_u16* length)
{
  return NULL;
}

_NO_IMPL_ static irs_u8* get_other_speed_config_desc(irs_u16* length)
{
  return NULL;
}

_NO_IMPL_ static irs_u8* get_device_qualifier_desc(irs_u16* length)
{
  return NULL;
}

#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
_NO_IMPL_ static irs_u8* get_usr_str_desc(USBD_HandleTypeDef* pdev, irs_u8 idx, irs_u16* length)
{
  return NULL;
}
#endif

_NO_TESTED_ void setup_handle(USBD_HandleTypeDef** ap_handles,
                  size_t a_count_handles)
{
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
#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
    get_usr_str_desc,
#endif
  };

  composite_usb.device.pClass = pclass;
}

} // namespace irs

#endif // COMPOSITE_USB_H
