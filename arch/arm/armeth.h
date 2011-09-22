// Интерфейс Ethernet (MAC) для ARM
// Дата: 24.11.2010
// Дата создания: 20.05.2010

#ifndef armethH
#define armethH

#include <irsdefs.h>

#include <irstcpip.h>

#include <irsfinal.h>

//#define NEW_21092011

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
  virtual void set_mac(mxmac_t& a_mac);
  virtual void tick();
private:
  enum
  {
    // Размер поля адреса приемника
    DA_size = mac_length,
    // Размер поля адреса источника
    SA_size = mac_length,
    // Размер поля длины пакета
    L_size = 2,
    // Размер контрольной суммы
    FCS_size = 4,
    // Добавочный размер для выравнивания на 4 байта
    align_size = 4,
    // Минимальный размер пакета в FIFO
    fifo_min_size = L_size + DA_size + SA_size
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
  #ifndef NEW_21092011
  irs_size_t m_buf_size;
  #endif //NEW_21092011
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
  irs_size_t m_rx_buf_size;
  irs_size_t m_tx_buf_size;

  void rx_interrupt();
};

} //  arm

} //  irs

#endif  //  armethH
