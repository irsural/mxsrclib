// @brief описание классов для работы с композиционным usb.
//
// Дата: 23.05.2021
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
  
class usb_module
{
public:
  usb_module(USBD_DescriptorsTypeDef* ap_descriptor, 
             USBD_ClassTypeDef* ap_definition,
             USBD_CDC_ItfTypeDef* ap_interface,
             void (*ap_register_interface)(USBD_HandleTypeDef*,
                                           USBD_CDC_ItfTypeDef*),
             irs_u8 a_id);
  
  virtual ~usb_module() = default;
  
  virtual void tick(USBD_HandleTypeDef* ap_handle_device) = 0;
  
  USBD_DescriptorsTypeDef* get_descriptor() const;
  
  USBD_ClassTypeDef* get_definition() const;
  
  USBD_CDC_ItfTypeDef* get_interface() const;
  
  void (*mp_register_interface)(USBD_HandleTypeDef*,
                                USBD_CDC_ItfTypeDef*);
  
  irs_u8 get_id() const;
  
private:
  USBD_DescriptorsTypeDef* mp_descriptor;
  USBD_ClassTypeDef* mp_definition;
  USBD_CDC_ItfTypeDef* mp_interface;
  
  irs_u8 m_id;
};
  
usb_module::usb_module(USBD_DescriptorsTypeDef* ap_descriptor, 
                       USBD_ClassTypeDef* ap_definition,
                       USBD_CDC_ItfTypeDef* ap_interface,
                       void (*ap_register_interface)(USBD_HandleTypeDef*,
                                                     USBD_CDC_ItfTypeDef*),
                       irs_u8 a_id)
  : mp_descriptor(ap_descriptor)  
  , mp_definition(ap_definition)
  , mp_interface(ap_interface)
  , m_id(a_id)
{ mp_register_interface = ap_register_interface; }

USBD_DescriptorsTypeDef* usb_module::get_descriptor() const
{ return mp_descriptor; }

USBD_ClassTypeDef* usb_module::get_definition() const
{ return mp_definition; }

USBD_CDC_ItfTypeDef* usb_module::get_interface() const
{ return mp_interface; }

irs_u8 usb_module::get_id() const
{ return m_id; }


class composite_usb
{
public:
  composite_usb(list<usb_module*> modules_list);
  
  virtual ~composite_usb();
  
  virtual void tick();
  
private:
  int usb_init();
  
  list<usb_module*> m_modules_list;
  
  USBD_HandleTypeDef m_handle_device;
};

composite_usb::composite_usb(list<usb_module*> modules_list)
  : m_modules_list(modules_list)
{ IRS_ASSERT(usb_init() < 0); }

composite_usb::~composite_usb()
{}

void composite_usb::tick()
{
  for (list<usb_module*>::iterator iter = m_modules_list.begin();
       iter != m_modules_list.end();
       iter++) 
  { 
    (*iter)->tick(&m_handle_device); 
  }
}

int composite_usb::usb_init()
{
  // Нужно ли инициализировать HAL здесь?
  // HAL_PWREx_EnableUSBVoltageDetector();
  
  for (list<usb_module*>::iterator iter = m_modules_list.begin();
       iter != m_modules_list.end();
       iter++) 
  {
    if ((*iter)->get_descriptor()) {
      USBD_Init(&m_handle_device, (*iter)->get_descriptor(), (*iter)->get_id());
    }
  }
  
  for (list<usb_module*>::iterator iter = m_modules_list.begin();
       iter != m_modules_list.end();
       iter++)
  {
    if ((*iter)->get_definition()) {
      USBD_RegisterClass(&m_handle_device, (*iter)->get_definition());
    }
  }
  
  for (list<usb_module*>::iterator iter = m_modules_list.begin();
       iter != m_modules_list.end();
       iter++)
  {
    if ((*iter)->get_interface()) {
      (*iter)->mp_register_interface(&m_handle_device, (*iter)->get_interface());
    }
  }
  
  USBD_Start(&m_handle_device);
  
  return 0;
}

} // namespace irs

#endif // COMPOSITE_USB_H
