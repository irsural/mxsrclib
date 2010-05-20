// Интерфейс Ethernet (MAC) для ARM
// Дата: 20.05.2010
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
  enum
  {
    DA_size = mac_length,
    SA_size = mac_length,
    L_size = 2,
    FCS_size = 4
  };
  buffer_num_t m_buf_num;
  size_t m_buf_size;
  irs_u8* mp_read_buf;
  irs_u8* mp_write_buf;
public:
  arm_ethernet_t(
    buffer_num_t a_buf_num,
    size_t a_buf_size,
    mxmac_t a_mac);
  virtual ~arm_ethernet_t();
  virtual void send_packet(irs_size_t a_size);
  virtual void set_recv_handled();
  virtual bool is_recv_buf_filled();
  virtual irs_u8* get_recv_buf();
  virtual irs_u8* get_send_buf();
  virtual irs_size_t recv_buf_size();
  virtual irs_size_t send_buf_max_size();
  virtual buffer_num_t get_buf_num();
  virtual mxmac_t get_local_mac();
  virtual void tick();
};

} //  arm

} //  irs

#endif  //  armethH
