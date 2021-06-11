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

#define IRS_MAX_MODULES 6
#define IRS_VENDOR_ID
#define IRS_PRODUCT_ID

class usb_module
{

}

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

  list<usb_module*> m_modules_list;
}

usb_composite_device::usb_composite_device()
  : m_vendor_id(IRS_VENDOR_ID)
  , m_product_id(IRS_PRODUCT_ID)
{}

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
