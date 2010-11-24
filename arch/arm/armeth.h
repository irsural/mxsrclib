// Интерфейс Ethernet (MAC) для ARM
// Дата: 24.11.2010
// Дата создания: 20.05.2010

#ifndef armethH
#define armethH

#include <irsdefs.h>

#include <irstcpip.h>

#include <irsfinal.h>

namespace irs
{

namespace arm
{

class arm_ethernet_t : public simple_ethernet_t
{
public:
  enum use_int_t
  {
    USE_INT,
    NO_USE_INT
  };
  arm_ethernet_t(
    buffer_num_t a_buf_num,
    size_t a_buf_size,
    mxmac_t a_mac,
    use_int_t a_use_interrupt = NO_USE_INT);
  virtual ~arm_ethernet_t();
  virtual void send_packet(irs_size_t a_size);
  virtual void set_recv_handled();
  virtual void set_send_buf_locked();
  virtual bool is_recv_buf_filled();
  virtual bool is_send_buf_empty();
  virtual irs_u8* get_recv_buf();
  virtual irs_u8* get_send_buf();
  virtual irs_size_t recv_buf_size();
  virtual irs_size_t send_buf_max_size();
  virtual buffer_num_t get_buf_num();
  virtual mxmac_t get_local_mac();
  virtual void tick();
private:
  enum
  {
    DA_size = mac_length,
    SA_size = mac_length,
    L_size = 2,
    FCS_size = 4
  };
  enum
  {
    max_packet_size = 2032
  };
  enum {
    PORTF2_LED1 = 0x100,
    PORTF3_LED0 = 0x1000
  };

  buffer_num_t m_buf_num;
  irs_size_t m_buf_size;
  irs_u8* mp_rx_buf;
  irs_u8* mp_tx_buf;
  bool m_rx_buf_handled;
  bool m_rx_buf_filled;
  irs_size_t m_rx_size;
  bool m_rx_int_flag;
  event_connect_t<arm_ethernet_t> m_rx_int_event;
  const use_int_t m_use_interrupt;
  bool m_send_buf_locked;
  bool m_send_packet_action;

  void rx_interrupt();
};

} //  arm

} //  irs

#endif  //  armethH
