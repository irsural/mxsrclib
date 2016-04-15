//! \file
//! \ingroup drivers_group
//! \brief Реализация интерфейса hardflow_t для
//!   USB HID в контроллерах stm32.
//! \details Работает через библиотеку STM32 USB Device Library 
//!   (версия V2.0.0 или выше)
//!
//! Дата создания: 28.01.2016

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
#include <irsstm32_usb_hid_hardflow.h>

#if IRS_USE_STM32_USB_HID_HARDFLOW
extern "C" {
//# include <usbd_customhid.h>
//# include "usbd_hid_desc.h"
//# include "usbd_customhid.h"
# include "usbd_customhid_if.h"
//# include <usbd_custom_hid_core.h>
//# include <irs_usbd_hid_hardflow_core.h>
//# include "usbd_usr.h"
//# include <usbd_core.h>
//# include <usb_dcd_int.h>
#include <usbd_desc.h>

extern PCD_HandleTypeDef hpcd;
}
#endif // IRS_USE_STM32_USB_HID_HARDFLOW

#include <irsfinal.h>

#if IRS_USE_STM32_USB_HID_HARDFLOW

// class usb_hid_t
irs::handle_t<irs::hardflow::arm::usb_hid_t>
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
  m_otg_event_connect(this, &irs::hardflow::arm::usb_hid_t::otg_event),
  m_write_packet(),
  m_read_buffers(m_channel_count),
  m_write_buffers(m_channel_count),
  m_write_buf_index(0),
  m_channel(invalid_channel + 1),
  m_packet_received(false),
  mp_dev(NULL),
  mp_rx_buffer(NULL)/*,
  m_tx_buffer_is_empty(true)*/
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
  //mp_usb_hid = this;



  #ifdef USE_USB_HS
  const int USB_OTG_HS_CORE_ID = 0;
  const int core_id = USB_OTG_HS_CORE_ID;

  irs::interrupt_array()->int_gen_events(irs::arm::otg_hs_int)->push_back(
    &m_otg_event_connect);
  #else
  const int USB_OTG_FS_CORE_ID = 0; //1
  const int core_id = USB_OTG_FS_CORE_ID;

  irs::interrupt_array()->int_gen_events(irs::arm::otg_fs_int)->push_back(
    &m_otg_event_connect);
  #endif

  USBD_Init(&m_usb_otg_dev, &HID_Desc, core_id);

   /* Add Supported Class */
  USBD_RegisterClass(&m_usb_otg_dev, &USBD_CUSTOM_HID);

  USBD_CustomHID_fops.OutEvent = &irs::hardflow::arm::usb_hid_t::rx_buffer_is_empty_event;

  /* Add Custom HID callbacks */
  USBD_CUSTOM_HID_RegisterInterface(&m_usb_otg_dev, &USBD_CustomHID_fops);

  /* Start Device Process */
  USBD_Start(&m_usb_otg_dev);
}

irs::hardflow::arm::usb_hid_t::~usb_hid_t()
{
  USBD_Stop(&m_usb_otg_dev);
  #ifdef USE_USB_HS
  irs::interrupt_array()->int_gen_events(irs::arm::otg_hs_int)->erase(
    &m_otg_event_connect);
  #else
  irs::interrupt_array()->int_gen_events(irs::arm::otg_fs_int)->erase(
    &m_otg_event_connect);
  #endif
}

void irs::hardflow::arm::usb_hid_t::release_resources()
{
}

irs::hardflow::arm::usb_hid_t::string_type
irs::hardflow::arm::usb_hid_t::param(const string_type& /*a_name*/)
{
  return string_type();
}

void irs::hardflow::arm::usb_hid_t::set_param(const string_type& /*a_name*/,
  const string_type& /*a_value*/)
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
      //DCD_EP_PrepareRx(mp_dev, CUSTOM_HID_EPOUT_ADDR, mp_rx_buffer, CUSTOM_HID_EPOUT_SIZE);

      USBD_CUSTOM_HID_HandleTypeDef *hhid =
        (USBD_CUSTOM_HID_HandleTypeDef*)m_usb_otg_dev.pClassData;
      USBD_LL_PrepareReceive(&m_usb_otg_dev, CUSTOM_HID_EPOUT_ADDR , hhid->Report_buf,
        USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

      m_packet_received = false;
    }
  }
  USBD_CUSTOM_HID_HandleTypeDef *hhid =
    (USBD_CUSTOM_HID_HandleTypeDef*)m_usb_otg_dev.pClassData;

  if ((hhid->state == CUSTOM_HID_IDLE) &&
      (m_usb_otg_dev.dev_state == USBD_STATE_CONFIGURED)) {
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
      //m_tx_buffer_is_empty = false;
      //usbd_hid_hardflow_send_report(&m_usb_otg_dev,
        //reinterpret_cast<irs_u8*>(&m_write_packet), packet_max_size);

      USBD_CUSTOM_HID_SendReport(&m_usb_otg_dev,
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

void irs::hardflow::arm::usb_hid_t::otg_event()
{  
  //USBD_OTG_ISR_Handler(&m_usb_otg_dev);
  HAL_PCD_IRQHandler(&hpcd);   
}

void irs::hardflow::arm::usb_hid_t::tx_buffer_is_empty_event()
{
  IRS_LIB_ASSERT(!mp_usb_hid.is_empty());
  //mp_usb_hid->m_tx_buffer_is_empty = true;
}

int8_t irs::hardflow::arm::usb_hid_t::rx_buffer_is_empty_event(
  //USBD_HandleTypeDef* ap_dev,
  irs_u8* ap_rx_buffer, uint32_t a_size)
{
  IRS_LIB_ASSERT(!mp_usb_hid.is_empty());
  if (a_size != report_max_size) {
    //DCD_EP_PrepareRx(ap_dev, CUSTOM_HID_EPOUT_ADDR, ap_rx_buffer, HID_OUT_PACKET);

    USBD_CUSTOM_HID_HandleTypeDef *hhid =
      (USBD_CUSTOM_HID_HandleTypeDef*)mp_usb_hid->m_usb_otg_dev.pClassData;
    USBD_LL_PrepareReceive(&mp_usb_hid->m_usb_otg_dev, CUSTOM_HID_EPOUT_ADDR , hhid->Report_buf,
      USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    return USBD_OK;
  }
  //IRS_LIB_ASSERT(ap_dev == &mp_usb_hid->m_usb_otg_dev);
  //mp_usb_hid->mp_dev = ap_dev;
  mp_usb_hid->mp_rx_buffer = ap_rx_buffer;
  mp_usb_hid->m_packet_received = true;
  return USBD_OK;
}

irs::hardflow::arm::usb_hid_t*
irs::hardflow::arm::usb_hid_t::reset(
  size_type a_channel_start_index,
  size_type a_channel_count, size_type a_report_size)
{
  mp_usb_hid.reset();
  mp_usb_hid.reset(new usb_hid_t(a_channel_start_index, a_channel_count,
    a_report_size));
  return mp_usb_hid.get();
}

irs::hardflow::arm::usb_hid_t*
irs::hardflow::arm::usb_hid_t::get_instance()
{
  return mp_usb_hid.get();
}

#endif // IRS_USE_STM32_USB_HID_HARDFLOW
