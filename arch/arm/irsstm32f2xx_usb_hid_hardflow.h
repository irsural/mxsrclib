//! \file
//! \ingroup drivers_group
//! \brief Реализация интерфейса hardflow_t для 
//!   USB HID контроллеров stm32f2xx/stm32f4xx
//!
//! Дата создания: 16.10.2013

#ifndef irsstm32f2xx_usb_hid_hardflowH
#define irsstm32f2xx_usb_hid_hardflowH

#include <irsdefs.h>

#include <hardflowg.h>

#include <armioregs.h>
#include <armregs_stm32f2xx.h>
#include <armiomacros.h>

#if defined(IRS_STM32F2xx)
# include <stm32f2x7_eth.h>
#elif defined(IRS_STM32F4xx)
# include <stm32f4x7_eth.h>
#endif // defined(IRS_STM32F4xx)

#if IRS_USE_STM32F2_4_USB_HID_HARDFLOW
extern "C" {
# include <usb_core.h>
# include <usbd_core.h>
}
#endif // IRS_USE_STM32F2_4_USB_HID_HARDFLOW

#include <irsfinal.h>

#if IRS_USE_STM32F2_4_USB_HID_HARDFLOW

namespace irs {

namespace hardflow {
  
namespace arm {

#if defined(IRS_STM32F_2_AND_4)

class usb_hid_t: public hardflow_t
{
public:
  typedef hardflow_t::size_type size_type;
  typedef hardflow_t::string_type string_type;
  usb_hid_t(size_type a_channel_start_index = invalid_channel + 1,
    size_type a_channel_count = 1, size_type a_report_size = 64);
  virtual ~usb_hid_t();
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
  static void rx_buffer_is_empty_event(USB_OTG_CORE_HANDLE* ap_dev, 
    irs_u8* ap_rx_buffer, size_type a_size);
  static void tx_buffer_is_empty_event();
  static usb_hid_t* get_instance();
  enum { report_max_count = 5 };
  typedef irs_u8 channel_field_type;
  typedef irs_u16 size_field_type;
  typedef std::vector< vector<irs_u8> > buffers_type;
  typedef std::vector< vector<irs_u8> >::iterator buffers_iterator;
  typedef std::vector< vector<irs_u8> >::const_iterator buffers_const_iterator; 
  enum { channel_field_size = sizeof(channel_field_type) };
  enum { size_field_size = sizeof(size_field_type) };
  enum { header_size = sizeof(channel_field_type) + sizeof(size_field_type) };
  enum { report_max_size = 64 };
  enum { packet_max_size = report_max_size };
  enum { data_max_size = packet_max_size -  header_size};
private:  
  #pragma pack(push, 1)
  struct packet_t
  {    
    channel_field_type channel_id;
    size_field_type data_size;
    irs_u8 data[data_max_size];
    packet_t():      
      channel_id(0),
      data_size(0),
      data()
    {
      memsetex(data, irs_u8(0), sizeof(data));
    }
  };
  #pragma pack(pop)
  void release_resources();
  inline size_type channel_id_to_buf_index(size_type a_channel_id)
  {
    return a_channel_id - 1;
  }
  inline size_type buf_index_to_channel_id(size_type a_buf_index)
  {
    return a_buf_index + 1;
  }
  inline size_type packet_channel_id_to_buf_index(
    channel_field_type a_channel_id)
  {
    return a_channel_id - (m_channel_start_index - 1);
  }
  inline channel_field_type buf_index_to_packet_channel_id(
    size_type a_buf_index)
  {
    return static_cast<channel_field_type>(
      a_buf_index + (m_channel_start_index - 1));
  }
  size_type read_from_buffer(vector<irs_u8>* ap_buffer, irs_u8 *ap_buf,
    size_type a_size);
  size_type write_to_buffer(vector<irs_u8>* ap_buffer, const irs_u8 *ap_buf,
    size_type a_size);
  void otg_fs_event();
  //static write_report(void* ap_params);
  usb_hid_t();
  const size_type m_channel_start_index;
  const size_type m_channel_end_index;
  const size_type m_channel_count;
  //size_type m_report_size;
  //size_type m_packet_size;
  size_type m_data_max_size;
  size_type m_buffer_max_size;
  #ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
    #if defined ( __ICCARM__ ) /*!< IAR Compiler */
      #pragma data_alignment=4   
    #endif
  #endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */  
  __ALIGN_BEGIN USB_OTG_CORE_HANDLE m_usb_otg_dev __ALIGN_END;
  #ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
    #if defined ( __ICCARM__ ) /*!< IAR Compiler */
      #pragma data_alignment=4   
    #endif
  #endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */ 
  event_connect_t<usb_hid_t> m_otg_fs_event_connect;
  packet_t m_write_packet;
  buffers_type m_read_buffers;
  buffers_type m_write_buffers;
  size_type m_write_buf_index;
  size_type m_channel;
  bool m_packet_received;
  USB_OTG_CORE_HANDLE* mp_dev;
  irs_u8* mp_rx_buffer;
  bool m_tx_buffer_is_empty;
  static usb_hid_t* mp_usb_hid;  
};
  
#else
# error Тип контроллера не определён
#endif // mcu type
  
} // namespace arm

} // namespace hardflow

} // namespace irs
  
#endif // IRS_USE_STM32F2_4_USB_HID_HARDFLOW

#endif // irsstm32f2xx_usb_hid_hardflowH