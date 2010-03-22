// Драйвер Ethernet для RTL8019AS 
// Дата: 17.03.2010
// Дата создания: 15.03.2010

#include <irsdefs.h>

#include <ioavr.h>
#include <inavr.h>

#include <irsint.h>
#include <timer.h>
#include <irserror.h>
//#include <irsrtl.h>
#include <irsrtltest.h>

#include <irsfinal.h>

//Сброс RTL при таймауте в операции:
#define RTL_RESET_ON_TIMEOUT 

//Выключение прерываний во время операции:
//#define RTL_DISABLE_INT 

//Запрет прерываний при чтении/записи байтов:
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
// Регистры RTL
#define config3 0x06
// Биты CONFIG3
// Тип 0-го светодиода
#define LEDS0 4
// Биты CR
// Номер регистровой страницы
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
  const irs_u8 *ap_mac
):
  m_buf_num(a_buf_num),
  m_size_buf((a_buf_size < ETHERNET_PACKET_MAX) ? 
    ((a_buf_size > ETHERNET_PACKET_MIN) ? a_buf_size : ETHERNET_PACKET_MIN) : 
    ETHERNET_PACKET_MAX),
  m_recv_buf(m_size_buf + 4),
  m_send_buf((a_buf_num == single_buf) ? 0 : m_size_buf),
  m_mac_save(mac_size),
  m_rtl_interrupt_event(this, rtl_interrupt),
  m_recv_status(false),
  m_send_status(false),
  m_recv_buf_size(0),
  mp_recv_buf(m_recv_buf.data()),
  mp_send_buf((a_buf_num == single_buf) ? mp_recv_buf : m_send_buf.data())
{
  irs_avr_int4_int.add(&m_rtl_interrupt_event);
  set_rtl_ports(a_data_port, a_address_port);
  init_rtl(ap_mac);
}

irs::rtl8019as_t::~rtl8019as_t()
{

}

irs_u8 irs::rtl8019as_t::read_rtl(irs_u8 a_reg_addr)
{
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
  if (!m_recv_status) {
    if (recv_size_cur > ETHERNET_PACKET_RX) {
      for (irs_u16 i = 0; i < recv_size_cur; i++) {
        byte = read_rtl(rdmaport);
      }
    } else {
      m_recv_status = true;
      /*if(m_buf_num == single_buf) {
        m_send_status = true;
      }*/
      m_recv_buf_size = recv_size_cur;
      for (irs_u16 i = 0; i < m_recv_buf_size; i++) {
        mp_recv_buf[i] = read_rtl(rdmaport);
      }
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
  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #else //RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT

  irs_u8 byte = read_rtl(isr);
  if((byte&0x10) == 0x10) {
    overrun(); 
  }
  if((byte&0x1) == 0x1) {
    recv_packet();
  }

  byte = read_rtl(bnry);
  write_rtl(cr, 0x62);
  irs_u8 _byte = read_rtl(curr);
  write_rtl(cr, 0x22);

  //'buffer is not empty, get next packet
  if (byte != _byte) {
    overrun();
  }

  write_rtl(isr, 0xff);
  write_rtl(cr, 0x22);

  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

void irs::rtl8019as_t::set_rtl_ports(irs_avr_port_t a_data_port, 
  irs_avr_port_t a_address_port)
{  
  rtl_port_str.rtl_data_port_set = avr_port_map[a_data_port].set;
  rtl_port_str.rtl_data_port_get = avr_port_map[a_data_port].get;
  rtl_port_str.rtl_data_port_dir = avr_port_map[a_data_port].dir;
  rtl_port_str.rtl_address_port_set = avr_port_map[a_address_port].set;
  rtl_port_str.rtl_address_port_get = avr_port_map[a_address_port].get;
  rtl_port_str.rtl_address_port_dir = avr_port_map[a_address_port].dir;
}

void irs::rtl8019as_t::reset_rtl() 
{
  // Запрет прерываний
  irs_disable_interrupt();
  // Все линии PORT2 выходы
  *rtl_port_str.rtl_address_port_dir = 0xFF;

  // Линию RTL RESDRV выставляем в 1
  *rtl_port_str.rtl_address_port_set |= (1 << RSTDRV);
  // Линию RTL RESDRV выставляем в 0
  *rtl_port_str.rtl_address_port_set &= (0xFF^(1 << RSTDRV));
  // Задержка после сброса RTL
  counter_t to_wait_rst;
  set_to_cnt(to_wait_rst, TIME_TO_CNT(1,320));
  while (!test_to_cnt(to_wait_rst));

  // Линия прерывания INT4 - вход
  DDRE &= 0xFF^((1 << DDE4));
  // Линия прерывания INT4 - подтянута к 5 В
  PORTE |= (1 << PORTE4);
  // INT4 генерируется по заднему фронту
  EICRB |= ((1 << ISC41)|(1 << ISC40));
  // Демаскирование INT4
  EIMSK |= ((1 << INT4));
  // Все линии PORT1 входы
  *rtl_port_str.rtl_data_port_dir = 0x00;
  // Все линии PORT1 заполняются 1
  *rtl_port_str.rtl_data_port_set = 0xFF;
  // Адресные биты сбрасываем в 0
  *rtl_port_str.rtl_address_port_set &= 0xE0;
  // Линию RTL IORB выставляем в 1
  IOR_HI;
  // Линию RTL IOWB выставляем в 1
  IOW_HI;

  irs_u8 byte = read_rtl(rstport);
  write_rtl(rstport, byte);
  __no_operation();

  write_rtl(cr, 0x21);
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
  write_rtl(cr, 0x61);
  __no_operation();
  write_rtl(curr, rxstart);

  write_rtl(0x1, m_mac_save[0]);
  write_rtl(0x2, m_mac_save[1]);
  write_rtl(0x3, m_mac_save[2]);
  write_rtl(0x4, m_mac_save[3]);
  write_rtl(0x5, m_mac_save[4]);
  write_rtl(0x6, m_mac_save[5]);

  write_rtl(cr, 0x21);
  write_rtl(dcr, dcrval);
  write_rtl(cr, 0x22);
  write_rtl(isr, 0xff);
  write_rtl(imr, imrval);
  write_rtl(tcr, tcrval);

  // Разрешение прерываний
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
      reset_rtl();
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

void irs::rtl8019as_t::init_rtl(const irs_u8 *ap_mac)
{
  memcpy(m_mac_save.data(), ap_mac, mac_size);
  irs_avr_int4_int.add(&m_rtl_interrupt_event);
  // Сброс RTL
  reset_rtl();
}

void irs::rtl8019as_t::set_recv_status_completed()
{
  m_recv_status = false;
}

bool irs::rtl8019as_t::is_recv_status_busy()
{
  return m_recv_status;
}

bool irs::rtl8019as_t::is_send_status_busy()
{
  return m_send_status;
}

irs_u8* irs::rtl8019as_t::get_recv_buf()
{
  return m_recv_buf.data();
}

irs_u8* irs::rtl8019as_t::get_send_buf()
{
  return m_send_buf.data();
}

size_t irs::rtl8019as_t::recv_buf_size()
{
  return m_recv_buf_size;
}

irs::rtl8019as_t::buffer_num_t irs::rtl8019as_t::get_buf_num()
{
  return m_buf_num;
}
