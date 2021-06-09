// @brief описание классов для работы с композиционным usb.
//
// Дата: 09.06.2021
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

#define MAX_USB_MODULES 6

class usb_module
{}

class usb_composite_device
{
  usb_composite_device(
    string ap_manufacturer,
    string ap_product,
    string ap_serial_number,
    isr_u16 a_vendor_id,
    irs_u16 a_product_id);
  ~usb_composite_device();

  string get_manufacturer() const;
  string get_product() const;
  string get_serial_number() const;

  irs_u16 get_vendor_id() const;
  irs_u16 get_product_id() const;

  bool start();
  void stop();

  // Я не знаю, для чего эта функция
  void clear();

  // В миллисекундах
  void set_disconnect_delay(irs_u32 delay = 500);

  bool is_ready() const;

  bool add_module(usb_module* module);

private:
  string mp_manufacturer;
  string mp_product;
  string mp_serial_number;

  irs_u16 m_vendor_id;
  irs_u16 m_product_id;

  bool m_is_enabled;

  list<usb_module*> m_modules_list;
}

} // namespace irs

#endif // COMPOSITE_USB_H
