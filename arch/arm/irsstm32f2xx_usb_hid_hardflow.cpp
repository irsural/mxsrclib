//! \file
//! \ingroup drivers_group
//! \brief Реализация интерфейса hardflow_t для 
//!   USB HID контроллеров stm32f2/stm32f4
//!
//! Дата создания: 16.10.2013

#include <irsdefs.h>


#include <irscpu.h>
#include <irsint.h>
#include <irsarchint.h>
#include <timer.h>
#include <irsalg.h>

#include <irscpp.h>
#include <irssysutils.h>
#include <armioregs.h>

#include <armcfg.h>
#include <irsstm32f2xx_usb_hid_hardflow.h>

#if IRS_USE_STM32F2_4_USB_HID_HARDFLOW
extern "C" {  
# include "usbd_hid_desc.h"
//# include <usbd_custom_hid_core.h>
# include <irs_usbd_hid_hardflow_core.h>
# include "usbd_usr.h"
# include <usbd_core.h>
# include <usb_dcd_int.h>
}
#endif // IRS_USE_STM32F2_4_USB_HID_HARDFLOW

#include <irsfinal.h>

#if IRS_USE_STM32F2_4_USB_HID_HARDFLOW

// class usb_hid_t
irs::hardflow::arm::usb_hid_t* 
irs::hardflow::arm::usb_hid_t::mp_usb_hid = NULL;

irs::hardflow::arm::usb_hid_t::usb_hid_t(
  size_type a_channel_start_index,
  size_type a_channel_count,
  size_type a_report_size
):  
  m_channel_start_index(a_channel_start_index),
  m_channel_end_index(a_channel_start_index + a_channel_count),
  m_channel_count(a_channel_count),
  //m_report_size(a_report_size),
  //m_packet_size(m_report_size),
  m_data_max_size(a_report_size - header_size),
  m_buffer_max_size(a_report_size*report_max_count),  
  m_usb_otg_dev(),
  m_otg_fs_event_connect(this, &irs::hardflow::arm::usb_hid_t::otg_fs_event),
  m_write_packet(),
  m_read_buffers(m_channel_count),
  m_write_buffers(m_channel_count),
  m_write_buf_index(0),
  m_channel(invalid_channel + 1),
  mp_dev(NULL),
  mp_rx_buffer(NULL),
  m_tx_buffer_is_empty(true),
  m_packet_received(false)
{  
  if ((m_channel_start_index < 1) ||
    (m_channel_start_index > static_cast<size_type>(
    std::numeric_limits<channel_field_type>::max() + 1))) {
    throw std::logic_error("Стартовый индекс должен быть от 1 до 256");
  }
  if ((a_channel_count < 1) || (a_channel_count > static_cast<size_type>(
    std::numeric_limits<channel_field_type>::max()))) {
    throw std::logic_error("Количество каналов должно быть от 1 до 255");
  }
  if ((a_report_size < 1) || (a_report_size > report_max_size)) {
    std::ostringstream msg;
    msg << "Значение a_report_size должно быть от 1 до " << report_max_size;
    throw std::logic_error(msg.str());
  }
  mp_usb_hid = this;
  irs::interrupt_array()->int_event_gen(irs::arm::otg_fs_int)->
    add(&m_otg_fs_event_connect);
  
  USBD_Init(&m_usb_otg_dev,
    #ifdef USE_USB_OTG_HS 
    USB_OTG_HS_CORE_ID,
    #else            
    USB_OTG_FS_CORE_ID,
    #endif
    &USR_HID_desc, 
    &usbd_hid_hardflow_cb, 
    &USR_cb);
}

irs::hardflow::arm::usb_hid_t::~usb_hid_t()
{
  USBD_DeInit(&m_usb_otg_dev);
}

void irs::hardflow::arm::usb_hid_t::release_resources()
{  
}

irs::hardflow::arm::usb_hid_t::string_type
irs::hardflow::arm::usb_hid_t::param(const string_type &a_name)
{
  return string_type();
}

void irs::hardflow::arm::usb_hid_t::set_param(const string_type &a_name,
  const string_type &a_value)
{
}

irs::hardflow::arm::usb_hid_t::size_type
irs::hardflow::arm::usb_hid_t::read(size_type a_channel_ident, irs_u8 *ap_buf,
  size_type a_size)
{
  size_type read_size = 0;
  const size_type buf_index = channel_id_to_buf_index(a_channel_ident);
  if (buf_index < m_channel_count) {
    buffers_iterator buf_it = m_read_buffers.begin() +
    channel_id_to_buf_index(a_channel_ident);
    read_size = read_from_buffer(&(*buf_it), ap_buf, a_size);    
  }
  return read_size;
}

irs::hardflow::arm::usb_hid_t::size_type
irs::hardflow::arm::usb_hid_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_size = 0;
  const size_type buf_index = channel_id_to_buf_index(a_channel_ident);
  if (buf_index < m_channel_count) {
    buffers_iterator buf_it = m_write_buffers.begin() + buf_index;
    write_size = write_to_buffer(&(*buf_it), ap_buf, a_size);    
  }
  return write_size;
}

irs::hardflow::arm::usb_hid_t::size_type
irs::hardflow::arm::usb_hid_t::channel_next()
{
  size_type channel = m_channel;
  m_channel++;
  if (m_channel > m_channel_end_index) {
    m_channel = m_channel_start_index;
  }
  return channel;
}

bool irs::hardflow::arm::usb_hid_t::is_channel_exists(size_type a_channel_ident)
{
  if ((a_channel_ident >= m_channel_start_index) &&
    (a_channel_ident <= m_channel_end_index)) {
    return true;
  }
  return false;
}

void irs::hardflow::arm::usb_hid_t::tick()
{
  if (m_packet_received) {
    bool prepare_buffer = false;
    const packet_t* packet = reinterpret_cast<packet_t*>(mp_rx_buffer);
    const size_type buf_index =
      packet_channel_id_to_buf_index(packet->channel_id);
    if (buf_index < m_channel_count) {      
      buffers_iterator it = m_read_buffers.begin() + buf_index;
      if ((m_buffer_max_size - it->size()) >=
        static_cast<size_type>(packet->data_size)) {
        size_type write_size = write_to_buffer(&(*it),
          packet->data, packet->data_size);
        IRS_LIB_ASSERT(write_size ==
          static_cast<size_type>(packet->data_size));
        prepare_buffer = true;
      }            
    } else {
      IRS_LIB_DBG_MSG("Получен пакет с недопустимым индексом канала");
      prepare_buffer = true;
    }
    if (prepare_buffer) {
      DCD_EP_PrepareRx(mp_dev, HID_OUT_EP, mp_rx_buffer, HID_OUT_PACKET);
      m_packet_received = false;
    }
  }
  if (m_tx_buffer_is_empty && 
    (m_usb_otg_dev.dev.device_status == USB_OTG_CONFIGURED)) {
    size_type start_index = m_write_buf_index;
    do {
      if (!m_write_buffers[m_write_buf_index].empty()) {
        break;
      }
      m_write_buf_index++;
      if (m_write_buf_index >= m_channel_count) {
        m_write_buf_index = 0;
      }
    } while (m_write_buf_index != start_index);

    if (!m_write_buffers[m_write_buf_index].empty()) {
      m_write_packet.channel_id =
        buf_index_to_packet_channel_id(m_write_buf_index);
      const size_type size = std::min<size_type>(
        m_write_buffers[m_write_buf_index].size(),
        m_data_max_size);
      m_write_packet.data_size = static_cast<size_field_type>(size);
      const size_type read_count = read_from_buffer(
        &m_write_buffers[m_write_buf_index],
        m_write_packet.data, size);
      IRS_LIB_ASSERT(read_count == size);  
      m_tx_buffer_is_empty = false;    
      usbd_hid_hardflow_send_report(&m_usb_otg_dev, 
        reinterpret_cast<irs_u8*>(&m_write_packet), packet_max_size);            
    }
    m_write_buf_index++;
    if (m_write_buf_index >= m_channel_count) {
      m_write_buf_index = 0;
    }
  }
}

irs::hardflow::arm::usb_hid_t::size_type
irs::hardflow::arm::usb_hid_t::read_from_buffer(vector<irs_u8>* ap_buffer,
  irs_u8 *ap_buf, size_type a_size)
{
  const size_type size = std::min(a_size, ap_buffer->size());
  memcpyex(ap_buf, vector_data(*ap_buffer), size);
  ap_buffer->erase(ap_buffer->begin(), ap_buffer->begin() + size);
  return size;
}

irs::hardflow::arm::usb_hid_t::size_type
irs::hardflow::arm::usb_hid_t::write_to_buffer(
  vector<irs_u8>* ap_buffer, const irs_u8 *ap_buf, size_type a_size)
{
  const size_type size = std::min(a_size,
    m_buffer_max_size - ap_buffer->size());
  size_type pos = ap_buffer->size();
  ap_buffer->resize(ap_buffer->size() + size);
  irs_u8* dest = vector_data(*ap_buffer, pos);
  memcpyex(dest, ap_buf, size);
  return size;
}

void irs::hardflow::arm::usb_hid_t::otg_fs_event()
{
  USBD_OTG_ISR_Handler(&m_usb_otg_dev);
}

void irs::hardflow::arm::usb_hid_t::tx_buffer_is_empty_event()
{
  IRS_LIB_ASSERT(mp_usb_hid); 
  mp_usb_hid->m_tx_buffer_is_empty = true;  
}

void irs::hardflow::arm::usb_hid_t::rx_buffer_is_empty_event(
  USB_OTG_CORE_HANDLE* ap_dev, 
  irs_u8* ap_rx_buffer, size_type a_size)
{
  IRS_LIB_ASSERT(mp_usb_hid);  
  if (a_size != report_max_size) {
    DCD_EP_PrepareRx(ap_dev, HID_OUT_EP, ap_rx_buffer, HID_OUT_PACKET);
    return;
  }  
  IRS_LIB_ASSERT(ap_dev == &mp_usb_hid->m_usb_otg_dev);
  mp_usb_hid->mp_dev = ap_dev;
  mp_usb_hid->mp_rx_buffer = ap_rx_buffer;
  mp_usb_hid->m_packet_received = true;  
}

#endif // IRS_USE_STM32F2_4_USB_HID_HARDFLOW