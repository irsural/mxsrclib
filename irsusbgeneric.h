// @brief общие классы и структуры для удобной работы с usb.
//
// Дата: 09.06.2021
// Дата создания: 09.06.2021

#pragma once

#ifndef USB_GENERIC_H
#define USB_GENERIC_H

namespace irs
{

// TODO: Понять, для чего эти дефайны 
#define USB_CONTROL_DONE 1

#define PMA_MEMORY_SIZE 512
#define MAX_USB_DESCRIPTOR_DATA_SIZE 200
#define USB_MAX_STRING_DESCRIPTOR_LENGTH 36

#define USB_EP0_BUFFER_SIZE       0x40
#define USB_EP0_TX_BUFFER_ADDRESS 0x40
#define USB_EP0_RX_BUFFER_ADDRESS (USB_EP0_TX_BUFFER_ADDRESS+USB_EP0_BUFFER_SIZE) 



} // namespace irs

#endif // USB_GENERIC_H