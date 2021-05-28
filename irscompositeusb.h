// @brief описание классов для работы с композиционным usb.
//
// Дата: 28.05.2021
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

namespace irs
{
  
typedef USBD_StatusTypeDef usb_statusns_t;
  
enum USB_ENDPOINT_TYPES {
  USB_GENERIC_ENDPOINT_TYPE_BULK = 0,
  USB_GENERIC_ENDPOINT_TYPE_CONTROL,
  USB_GENERIC_ENDPOINT_TYPE_ISO,
  USB_GENERIC_ENDPOINT_TYPE_INTERRUPT
};
  
typedef struct usb_endpoint
{
  void (*callback)(void);
  void* pma;
  irs_u16 pma_size;
  irs_u8 address;
  irs_u8 type : 2;
  irs_u8 tx : 1; // 1 - TX, 0 - RX
  irs_u8 exclusive : 1; // 1 - если невозможно использовать точку для tx и rx
  irs_u8 align : 1; // если следующая конечная точка противоположного 
                    // типа имеет тот же номер конечной точки, что и эта
} usb_endpoint;
  
typedef struct usb_module
{
  irs_u8 count_interfaces;
  irs_u8 count_endpoints;
  
  irs_u8 start_inferface;
  irs_u16 descriptor_size;
  
  void (*get_descriptor)(irs_u8* out);
  void (*module_init)(void);
  void (*module_reset)(void);
  void (*module_set_configuration)(void);
  void (*module_clear_feature)(void);
  void (*clear)(void);
  usb_statusns_t (*module_data_setup)(irs_u8 request, irs_u8 interface, 
                                      irs_u8 request_type, irs_u8 w_value_0,
                                      irs_u8 w_value_1, irs_u16 w_index,
                                      irs_u16 w_length);
  usb_statusns_t (*module_no_data_setup)(irs_u8 request, irs_u8 interface, 
                                         irs_u8 request_type, irs_u8 w_value_0,
                                         irs_u8 w_value_1, irs_u16 w_index,
                                         irs_u16 w_length);
  list<usb_endpoint*> endpoints_list;
} usb_module;

class composite_usb
{
public:
  composite_usb(irs_u16 a_vendor_id = 0,
                irs_u16 a_product_id = 0,
                const char* ap_manufacturer = nullptr,
                const char* ap_product = nullptr,
                const char* ap_serial_number = nullptr,
                list<usb_module*> a_modules_list = list<usb_module*>());
  
  virtual ~composite_usb();
  
  virtual void tick();
  
  void set_vendor_id(irs_u16 a_vendor_id);
  
  void set_product_id(irs_u16 a_product_id);
  
  void set_manufacturer(const char* ap_manufacturer);
  
  void set_product(const char* ap_product);
  
  void set_serial_number(const char* ap_serial_number);
  
  size_t count_modules() const;
  
  bool is_ready() const;
  
  void reset_modules();
  
  bool add_module(usb_module* module);
  
private:
  /*Приватные функции*/
  
  int usb_init();
  
  /* Приватные поля */
  
  irs_u16 m_vendor_id;
  irs_u16 m_product_id;
  
  const char* mp_manufacturer;
  const char* mp_product;
  const char* mp_serial_number;
  
  bool m_is_enabled;
  
  list<usb_module*> m_modules_list;
  
  USBD_HandleTypeDef m_handle_device;
};

composite_usb::composite_usb(irs_u16 a_vendor_id,
                             irs_u16 a_product_id,
                             const char* ap_manufacturer,
                             const char* ap_product,
                             const char* ap_serial_number,
                             list<usb_module*> a_modules_list)
  : m_vendor_id(a_vendor_id)
  , m_product_id(a_product_id)
  , mp_manufacturer(ap_manufacturer)
  , mp_product(ap_product)
  , mp_serial_number(ap_serial_number)
  , m_modules_list(a_modules_list)
{ IRS_ASSERT(usb_init() < 0); }

composite_usb::~composite_usb()
{}

void composite_usb::tick()
{
}

int composite_usb::usb_init()
{
  // Нужно ли инициализировать HAL здесь?
  // HAL_PWREx_EnableUSBVoltageDetector();
  

  USBD_Start(&m_handle_device);
  
  return 0;
}

void composite_usb::set_vendor_id(irs_u16 a_vendor_id)
{ m_vendor_id = a_vendor_id; }

void composite_usb::set_product_id(irs_u16 a_product_id)
{ m_product_id = a_product_id; }

void composite_usb::set_manufacturer(const char* ap_manufacturer)
{ mp_manufacturer = ap_manufacturer; }

void composite_usb::set_product(const char* ap_product)
{ mp_product = ap_product; }

void composite_usb::set_serial_number(cosnt char* ap_serial_number)
{ mp_serial_number = ap_serial_number; }

size_t composite_usb::count_modules() const
{ return m_modules_list.size(); }

bool composite_usb::is_ready() const
{ return is_enabled; }

void composite_usb::reset_modules()
{ m_modules_list.clear(); }

bool composite_usb::add_module(usb_module* module)
{
  IRS_ASSERT(module == nullptr);
  
  return true;
}

} // namespace irs

#endif // COMPOSITE_USB_H
