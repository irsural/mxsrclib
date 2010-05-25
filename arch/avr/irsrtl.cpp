// Драйвер Ethernet для RTL8019AS 
// Дата: 25.05.2010
// Дата создания: 15.03.2010

#include <irsdefs.h>

#include <ioavr.h>
#include <inavr.h>

#include <irsint.h>
#include <timer.h>
#include <irserror.h>
#include <irsrtl.h>

#include <irsfinal.h>

//Сброс RTL при таймауте в операции:
#define RTL_RESET_ON_TIMEOUT 

//Выключение прерываний во время операции:
//#define RTL_DISABLE_INT 

//Запрет прерываний при чтении/записи байтов:
#define RTL_DISABLE_INT_BYTE 

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
  mxmac_t a_mac,
  counter_t a_recv_timeout_cnt
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
  m_is_recv_buf_filled(false),
  m_send_status(false),
  m_recv_buf_size(0),
  mp_recv_buf(m_recv_buf.data()),
  mp_send_buf((a_buf_num == single_buf) ? mp_recv_buf : m_send_buf.data()),
  m_recv_timeout(a_recv_timeout_cnt)
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
  #ifdef RTL_DISABLE_INT_BYTE
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE
  
  *rtl_port_str.rtl_data_port_dir = 0x00;
  *rtl_port_str.rtl_data_port_set = 0xFF;
  *rtl_port_str.rtl_address_port_set &= (0xFF^0x1F);
  *rtl_port_str.rtl_address_port_set |= a_reg_addr;
  *rtl_port_str.rtl_address_port_set &= (0xFF^(1 << IORB));
  __no_operation();
  irs_u8 READ = *rtl_port_str.rtl_data_port_get;
  __no_operation();
  *rtl_port_str.rtl_address_port_set |= (1 << IORB);
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
  *rtl_port_str.rtl_address_port_set &= (0xFF^(1 << IOWB));
  __no_operation();
  *rtl_port_str.rtl_address_port_set |= (1 << IOWB);
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
  
  read_rtl(rdmaport);
  read_rtl(rdmaport);
  
  size_t recv_size_cur = 0;
  IRS_LOBYTE(recv_size_cur) = read_rtl(rdmaport);
  IRS_HIBYTE(recv_size_cur) = read_rtl(rdmaport);
  
  if (m_is_recv_buf_filled == false) {
    if (recv_size_cur <= m_size_buf) {
      m_recv_buf_size = recv_size_cur;
      IRS_LIB_ASSERT((m_recv_buf_size < ETHERNET_PACKET_MAX) &&
        (m_recv_buf_size <= m_recv_buf.size()));
      for (irs_size_t i = 0; i < m_recv_buf_size; i++) {
        mp_recv_buf[i] = read_rtl(rdmaport);
      }
      m_is_recv_buf_filled = true;
      m_recv_timeout.start();
    } else {
      for (irs_size_t i = 0; i < recv_size_cur; i++) {
        read_rtl(rdmaport);
      }
    }
  } else {
    for (irs_size_t i = 0; i < recv_size_cur; i++) {
      read_rtl(rdmaport);
    }
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
  if(byte&0x10) { //буфер приема переполнен
    overrun();
  }
  if(byte&0x01) { //получено без ошибок
    recv_packet();
  }
  if (byte&0xA) { //данные отправлены
    m_send_status = false;
  }

  byte = read_rtl(bnry);
  write_rtl(cr, 0x62);
  irs_u8 _byte = read_rtl(curr);
  write_rtl(cr, 0x22);

  //buffer is not empty, get next packet
  if (byte != _byte) {
    //mlog() << irsm("buffer is not empty, get next packet") << endl;
    overrun();//overrun(); //recv_packet(); 
  }

  write_rtl(isr, 0xff);
  write_rtl(cr, 0x22);

  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

void irs::rtl8019as_t::init_rtl() 
{
  // Запрет прерываний
  irs_disable_interrupt();

  // Все линии PORTA выходы
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
  *rtl_port_str.rtl_address_port_set |= (1 << IORB);
  // Линию RTL IOWB выставляем в 1
  *rtl_port_str.rtl_address_port_set |= (1 << IOWB);
  
  irs_u8 byte = read_rtl(rstport);
  write_rtl(rstport, byte);
  __no_operation();
  
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
      init_rtl();
      return false;
      #else //RTL_RESET_ON_TIMEOUT
      return true;
      #endif //RTL_RESET_ON_TIMEOUT
    }
  }
  return true;
}

void irs::rtl8019as_t::send_packet(irs_size_t a_size) 
{
  #ifdef IRS_LIB_CHECK
  if (a_size > m_send_buf.size()) {
    a_size = m_send_buf.size();
  }
  #endif //IRS_LIB_CHECK
  
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
  
  for (irs_size_t i = 0; i < a_size; i++) {
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

void irs::rtl8019as_t::set_recv_handled()
{
  m_is_recv_buf_filled = false;
  m_recv_timeout.stop();
  
}

bool irs::rtl8019as_t::is_recv_buf_filled()
{
  return m_is_recv_buf_filled;
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

mxmac_t irs::rtl8019as_t::get_local_mac()
{
  return m_mac;
}

void irs::rtl8019as_t::tick()
{
  if (m_recv_timeout.check()) {
    set_recv_handled();
  }
}
