// ������� Ethernet ��� RTL8019AS 
// ����: 29.03.2010
// ���� ��������: 15.03.2010

#include <irsdefs.h>

#include <ioavr.h>
#include <inavr.h>

#include <irsint.h>
#include <timer.h>
#include <irserror.h>
//#include <irsrtl.h>
#include <irsrtltest.h>

#include <irsfinal.h>

//����� RTL ��� �������� � ��������:
#define RTL_RESET_ON_TIMEOUT 

//���������� ���������� �� ����� ��������:
//#define RTL_DISABLE_INT 

//������ ���������� ��� ������/������ ������:
#define RTL_DISABLE_INT_BYTE 

//ETHERNET_PORT2|=0x20
#define IOR_HI {*rtl_port_str.rtl_address_port_set |= (1 << IORB);}
//ETHERNET_PORT2&=(0xFF^0x20)
#define IOR_LO {*rtl_port_str.rtl_address_port_set &= (0xFF^(1 << IORB));}
//ETHERNET_PORT2|=0x40
#define IOW_HI {*rtl_port_str.rtl_address_port_set |= (1 << IOWB);}
//ETHERNET_PORT2&=(0xFF^0x40)
#define IOW_LO {*rtl_port_str.rtl_address_port_set &= (0xFF^(1 << IOWB));}
#define IORB 5
#define IOWB 6
#define RSTDRV 7
// �������� RTL
#define config3 0x06
// ���� CONFIG3
// ��� 0-�� ����������
#define LEDS0 4
// ���� CR
// ����� ����������� ��������
#define PS0 6
#define PS1 7

struct rtl_port_str_t
{
  p_avr_port_t rtl_data_port_set;
  p_avr_port_t rtl_data_port_get;
  p_avr_port_t rtl_data_port_dir;
  p_avr_port_t rtl_address_port_set;
  p_avr_port_t rtl_address_port_get;
  p_avr_port_t rtl_address_port_dir;
} rtl_port_str;

irs::rtl8019as_t::rtl8019as_t(
  buffer_num_t a_buf_num,
  size_t a_buf_size,
  irs_avr_port_t a_data_port,
  irs_avr_port_t a_address_port,
  mac_t a_mac
):
  m_buf_num(a_buf_num),
  m_size_buf((a_buf_size < ETHERNET_PACKET_MAX) ? 
    ((a_buf_size > ETHERNET_PACKET_MIN) ? a_buf_size : ETHERNET_PACKET_MIN) : 
    ETHERNET_PACKET_MAX),
  m_recv_buf(m_size_buf),
  m_send_buf((a_buf_num == single_buf) ? 0 : m_size_buf),
  m_mac(a_mac),
  #ifndef SERGEY_OFF_INT4
  m_rtl_interrupt_event(this, rtl_interrupt),
  #endif //SERGEY_OFF_INT4
  m_recv_status(false),
  m_send_status(false),
  m_recv_buf_size(0),
  mp_recv_buf(m_recv_buf.data()),
  mp_send_buf((a_buf_num == single_buf) ? mp_recv_buf : m_send_buf.data()),
  m_blink_15(irs_avr_portd, 1),
  m_blink_16(irs_avr_portb, 6),
  m_blink_17(irs_avr_portb, 5),
  m_blink_18(irs_avr_portb, 4),
  m_blink_19(irs_avr_portb, 2),
  m_blink_20(irs_avr_portb, 7)
{
  rtl_port_str.rtl_data_port_set = avr_port_map[a_data_port].set;
  rtl_port_str.rtl_data_port_get = avr_port_map[a_data_port].get;
  rtl_port_str.rtl_data_port_dir = avr_port_map[a_data_port].dir;
  rtl_port_str.rtl_address_port_set = avr_port_map[a_address_port].set;
  rtl_port_str.rtl_address_port_get = avr_port_map[a_address_port].get;
  rtl_port_str.rtl_address_port_dir = avr_port_map[a_address_port].dir;
  
  #ifndef SERGEY_OFF_INT4
  irs_avr_int4_int.add(&m_rtl_interrupt_event);
  #endif // SERGEY_OFF_INT4
  
  init_rtl();
}

irs::rtl8019as_t::~rtl8019as_t()
{

}

irs_u8 irs::rtl8019as_t::read_rtl(irs_u8 a_reg_addr)
{
  m_blink_17.set();
  #ifdef RTL_DISABLE_INT_BYTE
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE
  
  *rtl_port_str.rtl_data_port_dir = 0x00;
  *rtl_port_str.rtl_data_port_set = 0xFF;
  *rtl_port_str.rtl_address_port_set &= (0xFF^0x1F);
  *rtl_port_str.rtl_address_port_set |= a_reg_addr;
  IOR_LO;
  __no_operation();
  irs_u8 READ = *rtl_port_str.rtl_data_port_get;
  __no_operation();
  IOR_HI;
  __no_operation();

  #ifdef RTL_DISABLE_INT_BYTE
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE

  return READ;
}

void irs::rtl8019as_t::write_rtl(irs_u8 a_reg_addr, 
  irs_u8 a_reg_data)
{
  m_blink_18.set();
  #ifdef RTL_DISABLE_INT_BYTE
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE

  *rtl_port_str.rtl_address_port_set &= (0xFF^0x1F);
  *rtl_port_str.rtl_address_port_set |= a_reg_addr;
  *rtl_port_str.rtl_data_port_dir = 0xFF;
  *rtl_port_str.rtl_data_port_set = a_reg_data;
  __no_operation();
  IOW_LO;
  __no_operation();
  IOW_HI;
  __no_operation();
  *rtl_port_str.rtl_data_port_dir = 0x00;
  *rtl_port_str.rtl_data_port_set = 0xFF;

  #ifdef RTL_DISABLE_INT_BYTE
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE
}

void irs::rtl8019as_t::overrun() 
{
  irs_u8 cr_save = read_rtl(cr);
  write_rtl(cr, 0x21);
  __no_operation();
  write_rtl(rbcr0, 0x0);
  write_rtl(rbcr1, 0x0);

  if((cr_save&0x4) == 0) {
    read_rtl(isr);
  }
  write_rtl(tcr, 0x2);
  write_rtl(cr, 0x22);
  write_rtl(bnry, rxstart);
  write_rtl(cr, 0x62);
  write_rtl(curr, rxstart);
  write_rtl(cr, 0x22);
  write_rtl(isr, 0x10);
  write_rtl(tcr, tcrval);
}

void irs::rtl8019as_t::recv_packet() 
{
  write_rtl(cr, 0x1a);
  irs_u8 byte = read_rtl(rdmaport);
  byte = read_rtl(rdmaport);
  byte = read_rtl(rdmaport);
  size_t recv_size_cur = 0;
  IRS_LOBYTE(recv_size_cur) = byte;
  byte = read_rtl(rdmaport);
  IRS_HIBYTE(recv_size_cur) = byte;
  if (m_recv_status == false) {
    if (recv_size_cur > ETHERNET_PACKET_RX) {
      for (irs_u16 i = 0; i < recv_size_cur; i++) {
        byte = read_rtl(rdmaport);
      }
    } else {
      m_recv_status = true;
      m_recv_buf_size = recv_size_cur;
      for (irs_u16 i = 0; i < m_recv_buf_size; i++) {
        mp_recv_buf[i] = read_rtl(rdmaport);
      }
      
      #ifndef NOP
      // IP ����������
      static ip_t local_ip = {{ 192, 168, 0, 37}};
      if((m_recv_buf[0xC] == 0x8) && (m_recv_buf[0xD] == 0x6)) { //EtherType
        if((m_recv_buf[0x26] == local_ip.val[0]) && 
          (m_recv_buf[0x27] == local_ip.val[1]) && 
          (m_recv_buf[0x28] == local_ip.val[2]) && 
          (m_recv_buf[0x29] == local_ip.val[3]) &&
          (m_recv_buf[0x15] == 0x1)) //arp-request
        {
          m_blink_15();
        }
      } 
      #endif //NOP
    }
  } else {
    for (irs_u16 i = 0; i < recv_size_cur; i++) {
      byte = read_rtl(rdmaport);
    }
  }
  if ((byte&rdc) != 64) {
    byte = read_rtl(isr);
  }
  write_rtl(isr, 0xF5);
}

void irs::rtl8019as_t::rtl_interrupt() 
{
  m_blink_20();
  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #else //RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
  
  irs_u8 byte = read_rtl(isr);
  if(byte&0x10) { //����� ������ ����������
    //m_blink_19.set();
    overrun(); 
  }
  if(byte&0x01) { //�������� ��� ������
    recv_packet();
  }
  if (byte&0xA) { //������ ����������
    //m_blink_19.set();
    m_send_status = false;
  }
  /*if (byte&0x2) { //������ ���������� ��� ������
    m_blink_19.set(); 
  }*/

  byte = read_rtl(bnry);
  write_rtl(cr, 0x62);
  irs_u8 _byte = read_rtl(curr);
  write_rtl(cr, 0x22);

  //buffer is not empty, get next packet
  if (byte != _byte) {
    overrun(); //recv_packet(); 
  }

  write_rtl(isr, 0xff);
  write_rtl(cr, 0x22);

  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

void irs::rtl8019as_t::init_rtl() 
{
  // ������ ����������
  irs_disable_interrupt();

  // ��� ����� PORTA ������
  *rtl_port_str.rtl_address_port_dir = 0xFF;

  // ����� RTL RESDRV ���������� � 1
  *rtl_port_str.rtl_address_port_set |= (1 << RSTDRV);
  // ����� RTL RESDRV ���������� � 0
  *rtl_port_str.rtl_address_port_set &= (0xFF^(1 << RSTDRV));
  // �������� ����� ������ RTL
  counter_t to_wait_rst;
  set_to_cnt(to_wait_rst, TIME_TO_CNT(1,320));
  while (!test_to_cnt(to_wait_rst));

  // ����� ���������� INT4 - ����
  DDRE &= 0xFF^((1 << DDE4));
  // ����� ���������� INT4 - ��������� � 5 �
  PORTE |= (1 << PORTE4);
  // INT4 ������������ �� ������� ������
  EICRB |= ((1 << ISC41)|(1 << ISC40));
  // �������������� INT4
  EIMSK |= ((1 << INT4));
  // ��� ����� PORT1 �����
  *rtl_port_str.rtl_data_port_dir = 0x00;
  // ��� ����� PORT1 ����������� 1
  *rtl_port_str.rtl_data_port_set = 0xFF;
  // �������� ���� ���������� � 0
  *rtl_port_str.rtl_address_port_set &= 0xE0;
  // ����� RTL IORB ���������� � 1
  IOR_HI;
  // ����� RTL IOWB ���������� � 1
  IOW_HI;
  
  irs_u8 byte = read_rtl(rstport);
  write_rtl(rstport, byte);
  __no_operation();

  //check for good soft reset
  if(!(read_rtl(isr) & 0x80))
  {
    //fail
    static irs::blink_t blink_7(irs_avr_portb, 3);
    blink_7.set();
  }
  
  write_rtl(cr, 0x21);//stop, page0
  __no_operation();
  write_rtl(dcr, dcrval);
  write_rtl(rbcr0, 0x0);
  write_rtl(rbcr1, 0x0);
  write_rtl(rcr, 0x04);
  write_rtl(tpsr, txtstart);
  write_rtl(tcr, 0x02);
  write_rtl(pstart, rxstart);
  write_rtl(bnry, rxstart);
  write_rtl(pstop, rxstop);
  write_rtl(cr, 0x61); //stop, page1
  __no_operation();
  write_rtl(curr, rxstart);

  for (int mac_index = 0; mac_index < mac_size; mac_index++) {
    write_rtl(par0 + mac_index, m_mac.val[mac_index]);
  }

  write_rtl(cr, 0x21); //stop, page0
  write_rtl(dcr, dcrval);
  write_rtl(cr, 0x22); //start
  write_rtl(isr, 0xff);
  write_rtl(imr, imrval);
  write_rtl(tcr, tcrval);

  // ���������� ����������
  irs_enable_interrupt();
}

bool irs::rtl8019as_t::wait_dma()
{
  counter_t to_wait_end_dma;
  set_to_cnt(to_wait_end_dma, TIME_TO_CNT(1, 2));
  irs_u8 isr_cont = read_rtl(isr);
  while ((isr_cont&rdc) == 0) {
    isr_cont = read_rtl(isr);
    if (test_to_cnt(to_wait_end_dma)) {
      #ifdef RTL_RESET_ON_TIMEOUT
      init_rtl();
      return false;
      #else //RTL_RESET_ON_TIMEOUT
      return true
      #endif //RTL_RESET_ON_TIMEOUT
    }
  }
  return true;
}

void irs::rtl8019as_t::send_packet(irs_u16 a_size) 
{
  #ifdef IRS_LIB_CHECK
  if (a_size > ETHERNET_PACKET_MAX) {
    a_size = ETHERNET_PACKET_MAX;
  } else if (a_size < ETHERNET_PACKET_MIN) {
    a_size = ETHERNET_PACKET_MIN;
  }
  #endif //IRS_LIB_CHECK

  IRS_LIB_ASSERT(a_size > ETHERNET_PACKET_MAX);
  
  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT
  
  write_rtl(cr, 0x22);
  write_rtl(tpsr, txtstart);
  write_rtl(rsar0, 0x00);
  write_rtl(rsar1, 0x40);
  write_rtl(isr, 0xff);

  write_rtl(rbcr0, IRS_LOBYTE(a_size));
  write_rtl(rbcr1, IRS_HIBYTE(a_size));
  write_rtl(cr, 0x12);
  for (irs_u16 i = 0; i < a_size; i++) {
    write_rtl(rdmaport, mp_send_buf[i]);
  }
  
  #ifndef NOP
  //IP �����������
  static ip_t local_ip = {{ 192, 168, 0, 37}};
  if((m_send_buf[0x1c] == local_ip.val[0]) &&
    (m_send_buf[0x1d] == local_ip.val[1]) &&
    (m_send_buf[0x1e] == local_ip.val[2]) &&
    (m_send_buf[0x1f] == local_ip.val[3]))
  {
    //IP ����������
    static ip_t dest_ip = {{ 192, 168, 0, 28}};
    if((m_send_buf[0x26] == dest_ip.val[0]) &&
      (m_send_buf[0x27] == dest_ip.val[1]) &&
      (m_send_buf[0x28] == dest_ip.val[2]) &&
      (m_send_buf[0x29] == dest_ip.val[3]))
    {
      m_blink_16();
    }
  }
  #endif //NOP
  
  if (!wait_dma()) {
    return;
  }

  write_rtl(tbcr0, IRS_LOBYTE(a_size));
  write_rtl(tbcr1, IRS_HIBYTE(a_size));
  write_rtl(cr, 0x24);
  
  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

void irs::rtl8019as_t::set_recv_handled()
{
  m_recv_status = false;
}

bool irs::rtl8019as_t::is_recv_buf_filled()
{
  return m_recv_status;
}

irs_u8* irs::rtl8019as_t::get_recv_buf()
{
  return m_recv_buf.data();
}

irs_u8* irs::rtl8019as_t::get_send_buf()
{
  return m_send_buf.data();
}

irs_size_t irs::rtl8019as_t::recv_buf_size()
{
  return m_recv_buf_size;
}

irs_size_t irs::rtl8019as_t::send_buf_max_size()
{
  return m_size_buf;
}

irs::rtl8019as_t::buffer_num_t irs::rtl8019as_t::get_buf_num()
{
  return m_buf_num;
}

irs::mac_t irs::rtl8019as_t::get_local_mac()
{
  return m_mac;
}

void irs::rtl8019as_t::tick()
{
}