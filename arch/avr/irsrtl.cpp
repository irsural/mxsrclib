//! \file
//! \ingroup drivers_group
//! \brief Драйвер Ethernet для RTL8019AS
//!
//! Дата: 01.07.2010\n
//! Дата создания: 15.03.2010

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
  //m_rtl_interrupt_event(this, rtl_interrupt),
  m_recv_buf_locked(false),
  m_recv_buf_size(0),
  mp_recv_buf(m_recv_buf.data()),
  mp_send_buf((a_buf_num == single_buf) ? mp_recv_buf : m_send_buf.data()),
  m_recv_timeout(a_recv_timeout_cnt),
  m_send_buf_empty(true),
  m_FIFO_new_packets(0),
  m_cur_pack_pointer(0),
  m_next_pack_pointer(0)
{
  m_rtl_port_str.rtl_data_port_set = avr_port_map[a_data_port].set;
  m_rtl_port_str.rtl_data_port_get = avr_port_map[a_data_port].get;
  m_rtl_port_str.rtl_data_port_dir = avr_port_map[a_data_port].dir;
  m_rtl_port_str.rtl_address_port_set = avr_port_map[a_address_port].set;
  m_rtl_port_str.rtl_address_port_get = avr_port_map[a_address_port].get;
  m_rtl_port_str.rtl_address_port_dir = avr_port_map[a_address_port].dir;
  
  //irs_avr_int4_int.add(&m_rtl_interrupt_event);
  
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
  
  *m_rtl_port_str.rtl_data_port_dir = 0x00;
  *m_rtl_port_str.rtl_data_port_set = 0xFF;
  *m_rtl_port_str.rtl_address_port_set &= (0xFF^0x1F);
  *m_rtl_port_str.rtl_address_port_set |= a_reg_addr;
  *m_rtl_port_str.rtl_address_port_set &= (0xFF^(1 << IORB));
  __no_operation();
  irs_u8 READ = *m_rtl_port_str.rtl_data_port_get;
  __no_operation();
  *m_rtl_port_str.rtl_address_port_set |= (1 << IORB);
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

  *m_rtl_port_str.rtl_address_port_set &= (0xFF^0x1F);
  *m_rtl_port_str.rtl_address_port_set |= a_reg_addr;
  *m_rtl_port_str.rtl_data_port_dir = 0xFF;
  *m_rtl_port_str.rtl_data_port_set = a_reg_data;
  __no_operation();
  *m_rtl_port_str.rtl_address_port_set &= (0xFF^(1 << IOWB));
  __no_operation();
  *m_rtl_port_str.rtl_address_port_set |= (1 << IOWB);
  __no_operation();
  *m_rtl_port_str.rtl_data_port_dir = 0x00;
  *m_rtl_port_str.rtl_data_port_set = 0xFF;

  #ifdef RTL_DISABLE_INT_BYTE
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE
}

void irs::rtl8019as_t::overrun() 
{
  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);
  
  if (read_rtl(ISR) & 0x10) {
    write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_stop);
    write_rtl(RBCR0, 0x00);
    write_rtl(RBCR1, 0x00);
    while((read_rtl(ISR) & 0x80) == 0);
    write_rtl(TCR, 0x02);
    write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);
    if (read_rtl(BNRY) != m_next_pack_pointer) {
      IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("Normal Overflow") << endl);
      write_rtl(BNRY, m_next_pack_pointer);
    } else {
      IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("Unrecognized Overflow") << endl);
      write_rtl(CR, cr_page1|cr_dma_abort_complete|cr_rtl_start);
      irs_u8 byte = read_rtl(CURR);
      write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);
      write_rtl(BNRY, byte);
      m_next_pack_pointer = byte;
      m_cur_pack_pointer = byte;
    }
    write_rtl(ISR, 0xFF);
    write_rtl(TCR, 0xE0);
  }
}

void irs::rtl8019as_t::recv_packet() 
{
  if (new_rx_packet()) {
    if (!m_recv_buf_locked) {
      m_cur_pack_pointer = read_rtl(BNRY);
      IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("m_cur_pack_pointer = ") <<
        int(m_cur_pack_pointer) << endl);
      write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);
      
      write_rtl(RSAR0, 0);
      write_rtl(RSAR1, m_cur_pack_pointer);
      write_rtl(RBCR0, 0xFF);
      write_rtl(RBCR1, 0x0F);
      
      write_rtl(CR, cr_page0|cr_remote_read|cr_rtl_start);
      
      read_rtl(RDMAPORT);
      m_next_pack_pointer = read_rtl(RDMAPORT);
      IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("m_next_pack_pointer = ") <<
        int(m_next_pack_pointer) << endl);
      size_t recv_size_cur = 0;
      IRS_LOBYTE(recv_size_cur) = read_rtl(RDMAPORT);
      IRS_HIBYTE(recv_size_cur) = read_rtl(RDMAPORT);
      
      if (recv_size_cur <= m_size_buf) {
        m_recv_buf_size = recv_size_cur;
        IRS_LIB_ASSERT((m_recv_buf_size < ETHERNET_PACKET_MAX) &&
          (m_recv_buf_size <= m_recv_buf.size()));
        for (irs_size_t i = 0; i < m_recv_buf_size; i++) {
          mp_recv_buf[i] = read_rtl(RDMAPORT);
        }
        m_recv_buf_locked = true;
        IRS_LIB_RTL_DBG_RAW_MSG_BASE(
          irsm("ethernet принимает пакет и блокирует его на запись") << endl);
        m_recv_timeout.start();
      } else {
        for (irs_size_t i = 0; i < recv_size_cur; i++) {
          read_rtl(RDMAPORT);
        }
        IRS_LIB_RTL_DBG_RAW_MSG_BASE(
          irsm("принятый пакет размером = ") << recv_size_cur <<
          irsm(" превышает размер буфера - отбрасываем") << endl);
      }
    } else {
      IRS_LIB_RTL_DBG_RAW_MSG_BASE(
        irsm("Буфер занят, пакет будет обработан позже") << endl);
    }
  }
}

void irs::rtl8019as_t::rtl_interrupt() 
{
  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #else //RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
  
  IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("rtl_interrupt") << endl);
  
  irs_u8 byte = read_rtl(ISR);
  if(byte&0x10) { //буфер приема переполнен
    overrun();
  }
  if(byte&0x01) { //получено без ошибок
    recv_packet();
  }

  byte = read_rtl(BNRY);
  write_rtl(CR, cr_page1|cr_dma_abort_complete|cr_rtl_start);
  irs_u8 _byte = read_rtl(CURR);
  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);

  //buffer is not empty, get next packet/
  if (byte != _byte) {
    //mlog() << irsm("buffer is not empty, get next packet") << endl;
    overrun();//overrun();//recv_packet();
  }

  write_rtl(ISR, 0xff);
  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);

  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

bool irs::rtl8019as_t::new_rx_packet()
{
  write_rtl(CR, cr_page1|cr_dma_abort_complete|cr_rtl_start);
  irs_u8 byte = read_rtl(CURR);
  write_rtl(CR, cr_page0|cr_rtl_start);
  if (byte != read_rtl(BNRY)) {
    if (read_rtl(RCR) & 0x01) {
      return true;
    }
  }
  return false;
}

void irs::rtl8019as_t::init_rtl() 
{
  // Запрет прерываний
  irs_disable_interrupt();

  // Все линии PORTA выходы
  *m_rtl_port_str.rtl_address_port_dir = 0xFF;

  // Линию RTL RESDRV выставляем в 1
  *m_rtl_port_str.rtl_address_port_set |= (1 << RSTDRV);
  pause(make_cnt_ms(3));
  // Линию RTL RESDRV выставляем в 0
  *m_rtl_port_str.rtl_address_port_set &= (0xFF^(1 << RSTDRV));
  pause(make_cnt_ms(3));
  
  // Сброс RTL
  write_rtl(RSTPORT, 0x00);
  // Задержка после сброса RTL
  pause(make_cnt_ms(9));
  
  // Все линии PORT1 входы
  *m_rtl_port_str.rtl_data_port_dir = 0x00;
  // Все линии PORT1 заполняются 1
  *m_rtl_port_str.rtl_data_port_set = 0xFF;
  // Адресные биты сбрасываем в 0
  *m_rtl_port_str.rtl_address_port_set &= 0xE0;
  // Линию RTL IORB выставляем в 1
  *m_rtl_port_str.rtl_address_port_set |= (1 << IORB);
  // Линию RTL IOWB выставляем в 1
  *m_rtl_port_str.rtl_address_port_set |= (1 << IOWB);
  
  #ifdef NOP
  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_stop);//stop, page0
  __no_operation();
  write_rtl(DCR, dcrval);
  write_rtl(RBCR0, 0x0);
  write_rtl(RBCR1, 0x0);
  write_rtl(RCR, 0x04);
  write_rtl(TPSR, txstart);
  write_rtl(TCR, 0x02);
  write_rtl(PSTART, rxstart);
  write_rtl(BNRY, rxstart);
  write_rtl(PSTOP, rxstop);
  write_rtl(CR, cr_page1|cr_dma_abort_complete|cr_rtl_stop); //stop, page1
  __no_operation();
  write_rtl(CURR, rxstart);

  for (int mac_index = 0; mac_index < mac_size; mac_index++) {
    write_rtl(PAR0 + mac_index, m_mac.val[mac_index]);
  }

  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_stop); //stop, page0
  write_rtl(DCR, dcrval);
  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start); //start
  write_rtl(ISR, 0xff);
  write_rtl(IMR, imrval);
  write_rtl(TCR, tcrval);
  #endif // NOP
  
  /* Goto page 3 and set registers */
  write_rtl(CR, cr_page3|cr_rtl_stop); /* page3, stop */
 	write_rtl(0x01, 0xC0);		/* config reg. write enble */
 	write_rtl(0x05, 0);			/* link test enable */
 	/* write_rtl( 0x06, 0x70 );		//Full duplex, leds */
 	
 	/* Goto page 1 and set registers */
 	
 	write_rtl(CR, cr_page1|cr_rtl_stop); /* page1, stop */
  /* Set MAC Address  */
  for (int mac_index = 0; mac_index < mac_size; mac_index++) {
    write_rtl(PAR0 + mac_index, m_mac.val[mac_index]);
  }
	
 	write_rtl( CURR, rxstart );	/* Current address */
 	m_next_pack_pointer = rxstart;
 	m_cur_pack_pointer = rxstart;
 	
 	/* Goto page 0 and set registers */
 	
 	write_rtl(CR, 0);				 		/* page0, Stop */
 	write_rtl(PSTART, rxstart); 		/* Rx buffer start address */
 	write_rtl(PSTOP, rxstop);	 		/* Rx buffer end address */
 	write_rtl(BNRY, rxstart);	 	/* Boundary */
 	write_rtl(ISR, 0xFF );			 		/* Interrupt services */
 	write_rtl(RCR, 0xC4);			 		/* Rx config (Accept all), was C4 */
 	write_rtl(TCR, 0xE0);			 		/* Tx config */
 	write_rtl(DCR, 0xB8);			 		/* Dataconfig */
 	
 	/* Start action ! */
 	
 	write_rtl( CR, cr_page0|cr_dma_abort_complete|cr_rtl_start );
  
  m_send_buf_empty = true;
  // Разрешение прерываний
  irs_enable_interrupt();
}

bool irs::rtl8019as_t::wait_dma()
{
  #ifdef RTL_RESET_ON_TIMEOUT
  counter_t to_wait_end_dma;
  set_to_cnt(to_wait_end_dma, TIME_TO_CNT(1, 2));
  irs_u8 isr_cont = read_rtl(ISR);
  while ((isr_cont&RDC) == 0) {
    isr_cont = read_rtl(ISR);
    if (test_to_cnt(to_wait_end_dma)) {
      init_rtl();
      return false;
    }
  }
  #endif //RTL_RESET_ON_TIMEOUT
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
  
  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);
  write_rtl(TPSR, txstart);
  write_rtl(RSAR0, 0x00);
  write_rtl(RSAR1, 0x40);
  write_rtl(ISR, 0xff);

  write_rtl(RBCR0, IRS_LOBYTE(a_size));
  write_rtl(RBCR1, IRS_HIBYTE(a_size));
  write_rtl(CR, cr_page0|cr_remote_write|cr_rtl_start);
  
  for (irs_size_t i = 0; i < a_size; i++) {
    write_rtl(RDMAPORT, mp_send_buf[i]);
  }
  
  if (!wait_dma()) {
    IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("!wait_dma()") << endl);
    mlog() << irsm("!wait_dma()") << endl;
    return;
  }

  write_rtl(TBCR0, IRS_LOBYTE(a_size));
  write_rtl(TBCR1, IRS_HIBYTE(a_size));
  write_rtl(CR, cr_page0|cr_dma_abort_complete|(1 << TXP));
  
  switch(m_buf_num) {
    case single_buf:
    {
      m_recv_buf_locked = false;
      m_recv_timeout.stop();
    } break;
    case double_buf:
    {
      m_send_buf_empty = true;
    } break;
    default:
    {
      IRS_LIB_ASSERT_MSG("неверно указан параметр: число буферов");
    } break;
  }
  IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("буфер записи разблокирован") << endl);
  
  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

void irs::rtl8019as_t::set_recv_handled()
{
  m_recv_buf_locked = false;
  IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("буфер чтения обработан") << endl);
  m_recv_timeout.stop();
  write_rtl(CR, cr_page0|cr_dma_abort_complete|cr_rtl_start);
  write_rtl(BNRY, m_next_pack_pointer);
}

void irs::rtl8019as_t::set_send_buf_locked()
{
  switch(m_buf_num) {
    case single_buf:
    {
      m_recv_buf_locked = true;
    } break;
    case double_buf:
    {
      m_send_buf_empty = false;
    } break;
    default:
    {
      IRS_LIB_ASSERT_MSG("неверно указан параметр: число буферов");
    } break;
  }
  IRS_LIB_RTL_DBG_RAW_MSG_BASE(irsm("буфер записи заблокирован") << endl);
}

bool irs::rtl8019as_t::is_recv_buf_filled()
{
  return m_recv_buf_locked;
}

bool irs::rtl8019as_t::is_send_buf_empty()
{
  switch(m_buf_num) {
    case single_buf:
    {
      return !m_recv_buf_locked;
    }
    case double_buf:
    {
      return m_send_buf_empty;
    }
    default:
    {
      IRS_LIB_ASSERT_MSG("неверно указан параметр: число буферов");
    }
  }
  return false;
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
    IRS_LIB_RTL_DBG_RAW_MSG_BASE(
      irsm("буфер разблокирован по таймауту") << endl);
    set_recv_handled();
  }
  recv_packet();
  overrun();
}
