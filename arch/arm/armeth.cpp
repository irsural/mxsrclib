#include <irsdefs.h>

#include <armeth.h>
#include <iolm3sxxxx.h>
#include <irscpu.h>
#include <irsint.h>
#include <irsarchint.h>

#include <irsfinal.h>

irs::arm::arm_ethernet_t::arm_ethernet_t(
  buffer_num_t a_buf_num,
  size_t a_buf_size,
  mxmac_t a_mac,
  use_int_t a_use_interrupt
):
  m_buf_num(a_buf_num),
  m_buf_size(a_buf_size),
  mp_rx_buf(IRS_NULL),
  mp_tx_buf(IRS_NULL),
  m_rx_buf_handled(true),
  m_rx_buf_filled(false),
  m_rx_size(0),
  m_rx_int_flag(false),
  m_rx_int_event(this, rx_interrupt),
  m_use_interrupt(a_use_interrupt)
{
  mp_rx_buf 
    = new irs_u8[DA_size + SA_size + L_size + m_buf_size + FCS_size];
  if (m_buf_num == single_buf) mp_tx_buf = mp_rx_buf;
  else mp_tx_buf 
    = new irs_u8[DA_size + SA_size + L_size + m_buf_size];
  
  enum
  {
    EMAC0 = 28,
    EPHY0 = 30
  };
  
  RCGC2 |= (1 << EMAC0)|(1 << EPHY0);
  //RCGC2_bit.EPHY0 = 1;  //  В iolm3sxxxx.h биты указаны неверно
  //RCGC2_bit.EMAC0 = 1;
  RCGC2_bit.PORTF = 1;
  for (irs_u8 i = 10; i; i--);
  //  Делитель частоты интерфейса MII
  const irs_u32 MII_freq = 2500000; //  max MII freq, Hz
  MACMDV_bit.DIV 
    = irs_u8((irs::cpu_traits_t::frequency() / (2 * MII_freq)) - 1);
  //  MAC-адрес
  MACIA0 = *(irs_u32*)a_mac.val;
  MACIA1_bit.MACOCT5 = a_mac.val[4];
  MACIA1_bit.MACOCT6 = a_mac.val[5];
  //
  //  Передатчик
  MACTCTL_bit.DUPLEX = 1;
  MACTCTL_bit.CRC = 1;
  MACTCTL_bit.PADEN = 1;
  MACTCTL_bit.TXEN = 1;
  //  Приёмник
  MACIM = 0;
  MACRCTL_bit.RXEN = 0;
  MACRCTL_bit.RSTFIFO = 1;
  MACRCTL_bit.BADCRC = 1;
  MACRCTL_bit.PRMS = 0;
  MACRCTL_bit.AMUL = 0;
  MACRCTL_bit.RXEN = 1;
  //
  //MACTCTL_bit.TXEN = 1;
  //  Светодиоды Ethernet
  GPIOFDEN_bit.no2 = 1;
  GPIOFDEN_bit.no3 = 1;
  GPIOFDIR_bit.no2 = 1;
  GPIOFDIR_bit.no3 = 1;
  GPIOFAFSEL_bit.no2 = 1;
  GPIOFAFSEL_bit.no3 = 1;
  //  Прерывание приёма пакета
  if (m_use_interrupt == USE_INT)
  {
    interrupt_array()->int_event_gen(ethernet_int)->add(&m_rx_int_event);
    MACIM_bit.RXINT = 1;
    SETENA1_bit.SETENA42 = 1;  //  Разрешение прерывания в NVIC
  }
  else
  {
    MACIM_bit.RXINT = 0;
    SETENA1_bit.SETENA42 = 0;
  }
}

irs::arm::arm_ethernet_t::~arm_ethernet_t() 
{
  delete []mp_rx_buf;
  if (m_buf_num == double_buf) delete []mp_tx_buf;
  SETENA1_bit.SETENA42 = 0;
  MACIM_bit.RXINT = 0;
  MACTCTL_bit.TXEN = 0;
  MACRCTL_bit.RXEN = 0;
  RCGC2_bit.EPHY0 = 0;
  RCGC2_bit.EMAC0 = 0;
}

void irs::arm::arm_ethernet_t::send_packet(irs_size_t a_size)
{
  if (a_size < DA_size + SA_size + L_size) return;
  
  volatile irs_u32 fifo_data = a_size - DA_size - SA_size - L_size;
  IRS_HIWORD(fifo_data) = *(irs_u16*)mp_tx_buf;
  MACDATA = fifo_data;
  
  const irs_size_t shift = sizeof(irs_u16);
  irs_u32* p_shifted_buf = (irs_u32*)(mp_tx_buf + shift);
  const irs_size_t whole_dwords_cnt = (a_size - shift) / sizeof(fifo_data);
  irs_size_t current_dword = 0; 
  for (; current_dword < whole_dwords_cnt; current_dword++)
  {
    MACDATA = p_shifted_buf[current_dword];
  }
  irs_size_t current_byte = current_dword * sizeof(fifo_data) + shift;
  fifo_data = 0;
  for (irs_u8 i = 0; current_byte < a_size; current_byte++, i++)
  {
    *((irs_u8*)&fifo_data + i) = mp_tx_buf[current_byte];
  }
  MACDATA = fifo_data;
  MACTR_bit.NEWTX = 1;
}

void irs::arm::arm_ethernet_t::set_recv_handled()
{
  m_rx_buf_handled = true;
  m_rx_buf_filled = false;
}

bool irs::arm::arm_ethernet_t::is_recv_buf_filled()
{
  return m_rx_buf_filled;
}

irs_u8* irs::arm::arm_ethernet_t::get_recv_buf()
{
  return mp_rx_buf;
}

irs_u8* irs::arm::arm_ethernet_t::get_send_buf()
{
  return mp_tx_buf;
}

irs_size_t irs::arm::arm_ethernet_t::recv_buf_size()
{
  return m_rx_size;
}

irs_size_t irs::arm::arm_ethernet_t::send_buf_max_size()
{
  return m_buf_size;
}

irs::simple_ethernet_t::buffer_num_t irs::arm::arm_ethernet_t::get_buf_num()
{
  return m_buf_num;
}

mxmac_t irs::arm::arm_ethernet_t::get_local_mac()
{
  mxmac_t mac;
  *(irs_u32*)mac.val = MACIA0;
  mac.val[4] = MACIA1_bit.MACOCT5;
  mac.val[5] = MACIA1_bit.MACOCT6;
  return mac;
}

void irs::arm::arm_ethernet_t::tick()
{
  if (m_rx_buf_handled)
  {
    if (m_use_interrupt == NO_USE_INT) m_rx_int_flag = (MACIS_bit.RXINT);
    if (m_rx_int_flag)
    {
      m_rx_int_flag = false;
      irs_u32 fifo_data = 0;
      fifo_data = MACDATA;
      m_rx_size = IRS_LOWORD(fifo_data) - L_size;
      
      irs::mlog() << irsm("RX size : ") << m_rx_size << endl;
      
      if (m_rx_size > m_buf_size && MACNP_bit.NPR)
      {
        if (m_rx_size > max_packet_size)
        {
          MACRCTL_bit.RSTFIFO = 1;
        }
        else
        {
          const irs_size_t skip_dwords_cnt = m_rx_size / sizeof(fifo_data);
          for (irs_size_t i = 0; i < skip_dwords_cnt; i++)
          {
            fifo_data = MACDATA;
          }
        }
      }
      else
      {
        *(irs_u16*)mp_rx_buf = IRS_HIWORD(fifo_data);
        
        const irs_size_t shift = sizeof(irs_u16);
        irs_u32* p_shifted_buf = (irs_u32*)(mp_rx_buf + shift);
        
        const irs_size_t whole_dwords_cnt 
          = (m_rx_size - shift) / sizeof(fifo_data);
        irs_size_t current_dword = 0;
        
        for (; current_dword < whole_dwords_cnt; current_dword++)
        {
          p_shifted_buf[current_dword] = MACDATA;
        }
        irs_size_t current_byte = current_dword * sizeof(fifo_data) + shift;
        fifo_data = MACDATA;
        for (irs_u8 i = 0; current_byte < m_rx_size; current_byte++, i++)
        {
          mp_rx_buf[current_byte] = *((irs_u8*)&fifo_data + i);
        }
        m_rx_buf_filled = true;
        m_rx_buf_handled = false;
      }
      if (m_use_interrupt == USE_INT) MACIM_bit.RXINT = 1;
    }
  }
}

void irs::arm::arm_ethernet_t::rx_interrupt()
{
  MACIS_bit.RXINT = 1;
  MACIM_bit.RXINT = 0;
  m_rx_int_flag = true;
}
