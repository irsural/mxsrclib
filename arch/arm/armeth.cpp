#include <irsdefs.h>

#include <armeth.h>
#include <iolm3sxxxx.h>
#include <irscpu.h>

#include <irsfinal.h>

irs::arm::arm_ethernet_t::arm_ethernet_t(
  buffer_num_t a_buf_num,
  size_t a_buf_size,
  mxmac_t a_mac
):
  m_buf_num(a_buf_num),
  m_buf_size(a_buf_size),
  mp_read_buf(IRS_NULL),
  mp_write_buf(IRS_NULL)
{
  mp_read_buf 
    = new irs_u8[DA_size + SA_size + L_size + m_buf_size + FCS_size];
  if (m_buf_num == single_buf) mp_write_buf = mp_read_buf;
  else mp_write_buf 
    = new irs_u8[DA_size + SA_size + L_size + m_buf_size];
  
  RCGC2_bit.EPHY0 = 1;
  RCGC2_bit.EMAC0 = 1;
  for (irs_u8 i = 10; i; i--);
  
  const irs_u32 MII_freq = 2500000; //  max MII freq, Hz
  MACMDV_bit.DIV 
    = irs_u8((irs::cpu_traits_t::frequency() / (2 * MII_freq)) - 1);
  
  MACIA0 = *(irs_u32*)a_mac.val;
  MACIA1 = *(irs_u32*)&a_mac.val[mac_length];
  
  MACTCTL_bit.DUPLEX = 1;
  MACTCTL_bit.CRC = 1;
  MACTCTL_bit.PADEN = 1;
  MACTCTL_bit.TXEN = 1;
  
  MACRCTL_bit.RXEN = 0;
  MACRCTL_bit.RSTFIFO = 1;
  MACRCTL_bit.BADCRC = 1;
  MACRCTL_bit.PRMS = 0;
  MACRCTL_bit.AMUL = 0;
  MACRCTL_bit.RXEN = 1;
}

irs::arm::arm_ethernet_t::~arm_ethernet_t() 
{
  MACTCTL_bit.TXEN = 0;
  MACRCTL_bit.RXEN = 0;
  RCGC2_bit.EPHY0 = 0;
  RCGC2_bit.EMAC0 = 0;
}

void irs::arm::arm_ethernet_t::send_packet(irs_size_t a_size)
{
}

void irs::arm::arm_ethernet_t::set_recv_handled()
{
}

bool irs::arm::arm_ethernet_t::is_recv_buf_filled()
{
}

irs_u8* irs::arm::arm_ethernet_t::get_recv_buf()
{
}

irs_u8* irs::arm::arm_ethernet_t::get_send_buf()
{
}

irs_size_t irs::arm::arm_ethernet_t::recv_buf_size()
{
}

irs_size_t irs::arm::arm_ethernet_t::send_buf_max_size()
{
}

irs::simple_ethernet_t::buffer_num_t irs::arm::arm_ethernet_t::get_buf_num()
{
}

mxmac_t irs::arm::arm_ethernet_t::get_local_mac()
{
}

void irs::arm::arm_ethernet_t::tick()
{
}