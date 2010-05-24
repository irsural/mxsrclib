// Драйвер Ethernet для RTL8019AS Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Испорчен Поляковым М.
// Дата: 14.04.2010
// Ранняя дата: 30.05.2008

#define RTL_RESET_ON_TIMEOUT // Сброс RTL при таймауте в операции
//#define RTL_DISABLE_INT // Выключение прерываний во время операции
#define RTL_DISABLE_INT_BYTE //Запрет прерываний при чтении/записи байтов
//#define RTL_DBG_MSG // Отображение отладочных сообщений (нужен общий DBG)

#include <irsdefs.h>

#include <ioavr.h>
#include <inavr.h>

#include <mxdata.h>
#include <timer.h>
#include <irsconfig.h>
//#include <irsavrutil.h>
#include <RTL8019AS.h>
#include <irserror.h>

#include <irsfinal.h>

// Внешние переменные, поэтому вне namespace
irs_u8* tx_buf = IRS_NULL;
irs_u8* rx_buf = IRS_NULL;
irs_u8 rx_hard = 0;
irs_u8 tx_hard = 0;
irs_u16 rxlen_hard = 0;

namespace {

struct
{
  p_avr_port_t rtl_data_port_set;
  p_avr_port_t rtl_data_port_get;
  p_avr_port_t rtl_data_port_dir;
  p_avr_port_t rtl_address_port_set;
  p_avr_port_t rtl_address_port_get;
  p_avr_port_t rtl_address_port_dir;
} port_str;

class rtl_t
{
public:
  typedef rtl_t this_type;
  
  enum {
    // Регистры
    CR = 0x0,
    
    PSTART = 0x01,
    PSTOP = 0x02,
    BNRY = 0x03,
    // Адресс страницы в которой находится пакет для передачи
    TPSR = 0x04,
    TBCR0 = 0x05,
    TBCR1 = 0x06,
    ISR = 0x07,
    RSAR0 = 0x08,
    RSAR1 = 0x09,
    RBCR0 = 0x0a,
    RBCR1 = 0x0b,
    RCR = 0x0c,
    TCR = 0x0d,
    DCR = 0x0e,
    IMR = 0x0f,
    
    CURR = 0x07,

    CONFIG3 = 0x06,
      
    RDMAPORT = 0x10,
    RSTPORT = 0x18,
    
    // Биты CR
    STP = 0,
    STA = 1,
    TXP = 2,
    RD0 = 3,
    RD1 = 4,
    RD2 = 5,
    PS0 = 6,
    PS1 = 7,
    cr_page0 = ((0 << PS1)|(0 << PS0)),
    cr_page1 = ((0 << PS1)|(1 << PS0)),
    cr_page2 = ((1 << PS1)|(0 << PS0)),
    cr_page3 = ((1 << PS1)|(1 << PS0)),
    cr_remote_read = ((0 << RD2)|(0 << RD1)|(1 << RD0)),
    cr_remote_write = ((0 << RD2)|(1 << RD1)|(0 << RD0)),
    cr_remote_send = ((0 << RD2)|(1 << RD1)|(1 << RD0)),
    cr_dma_abort_complete = ((1 << RD2)|(1 << RD1)|(1 << RD0)),
    cr_rtl_start = ((1 << STA)|(0 << STP)),
    cr_rtl_stop = ((0 << STA)|(1 << STP)),

    // Биты ISR
    RDC = 0x40,
    
    // Биты DCR
    WTS = 0,
    BOS = 1,
    LAS = 2,
    LS = 3,
    ARM = 4,
    FT0 = 5,
    FT1 = 6,
    
    // Биты CONFIG3
    // Тип 0-го светодиода
    LEDS0 = 4,
    
    // Готовые значения для регистров
    //dcrval = 0x58,
    dcrval = ((1 << LS)|(1 << ARM)|(1 << FT1)),
    // Адрес для TPSR
    txstart = 0x40,
    rxstart = 0x46,
    rxstop = 0x60,
    imrval = 0x1b,
    tcrval = 0x00
  };
  // Пины AVR для RTL
  enum {
    IORB = 5,
    IOWB  = 6,
    RSTDRV = 7
  };
  enum {
    irs_rtl8019as_packet_def_mark = 0,
    irs_rtl8019as_packet_def = 300,
    irs_rtl8019as_packet_min = 60,
    irs_rtl8019as_packet_max = 1550
  };
  enum { mac_size = 6};

  rtl_t();
  ~rtl_t();
  void init(const irs_u8 *ap_mac, irs_size_t a_bufs_size);
  void send_packet(irs_u16 length, irs_u8 *ext_tx_buf);
  void set_ports(irs_avr_port_t a_data_port, irs_avr_port_t a_address_port);
  irs_size_t tx_buf_size();
  
private:
  irs_u8 m_mac_save[mac_size];
  irs_size_t m_buf_size;
  irs::event_connect_t<this_type> m_rtl_interrupt_event;
  irs::raw_data_t<irs_u8> m_tx_buf_data;
  irs::raw_data_t<irs_u8> m_rx_buf_data;
  bool m_is_initialized;
  
  void rtl_interrupt();
  void reset_rtl();
  bool wait_dma();
  irs_u8 readrtl(irs_u8 regaddr);
  void writertl(irs_u8 regaddr, irs_u8 regdata);
  void overrun();
  void getpacket();
};

rtl_t::rtl_t():
  m_buf_size(0),
  m_rtl_interrupt_event(this, rtl_interrupt),
  m_tx_buf_data(0),
  m_rx_buf_data(0),
  m_is_initialized(false)
{
  memset(m_mac_save, 0, mac_size);
  irs_avr_int4_int.add(&m_rtl_interrupt_event);
}

rtl_t::~rtl_t()
{

}

void rtl_t::init(const irs_u8 *ap_mac, irs_size_t a_bufs_size)
{
  #ifdef RTL_DBG_MSG
  IRS_LIB_DBG_RAW_MSG(irsm("rtl - a_bufs_size перед: ") <<
    a_bufs_size << endl);
  #endif //RTL_DBG_MSG
  
  #ifdef IRS_LIB_DEBUG
  bool is_bufs_size_valid =
    (a_bufs_size == irs_rtl8019as_packet_def_mark) || 
    (
      (irs_rtl8019as_packet_min <= a_bufs_size) &&
      (a_bufs_size <= irs_rtl8019as_packet_max)
    );
  IRS_LIB_ASSERT(is_bufs_size_valid);
  #endif //IRS_LIB_DEBUG
  
  if (a_bufs_size == irs_rtl8019as_packet_def_mark) {
    a_bufs_size = irs_rtl8019as_packet_def;
  } else if (irs_rtl8019as_packet_min > a_bufs_size) {
    a_bufs_size = irs_rtl8019as_packet_min;
  } else if (a_bufs_size > irs_rtl8019as_packet_max) {
    a_bufs_size = irs_rtl8019as_packet_max;
  }
  
  m_tx_buf_data.resize(a_bufs_size);
  m_rx_buf_data.resize(a_bufs_size + 4);
  tx_buf = m_tx_buf_data.data();
  rx_buf = m_rx_buf_data.data();
  
  #ifdef RTL_DBG_MSG
  IRS_LIB_DBG_RAW_MSG(irsm("rtl - a_bufs_size после: ") <<
    a_bufs_size << endl);
  #endif //RTL_DBG_MSG
  
  memcpy(m_mac_save, ap_mac, mac_size);
  
  // Сброс RTL
  reset_rtl();
  
  m_is_initialized = true;
}

irs_u8 rtl_t::readrtl(irs_u8 regaddr)
{
  #ifdef RTL_DISABLE_INT_BYTE
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE
  
  irs_u8 READ=0x0;

  *port_str.rtl_data_port_dir = 0x00;
  *port_str.rtl_data_port_set = 0xFF;
  *port_str.rtl_address_port_set &= (0xFF^0x1F);
  *port_str.rtl_address_port_set |= regaddr;
  *port_str.rtl_address_port_set &= (0xFF^(1 << IORB));
  __no_operation();
  READ = *port_str.rtl_data_port_get;
  __no_operation();
  *port_str.rtl_address_port_set |= (1 << IORB);
  __no_operation();
  //__no_operation();
  //__no_operation();
  //__no_operation();
  //__no_operation();

  #ifdef RTL_DISABLE_INT_BYTE
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE

  return READ;
}

void rtl_t::writertl(irs_u8 regaddr, irs_u8 regdata)
{
  #ifdef RTL_DISABLE_INT_BYTE
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE

  *port_str.rtl_address_port_set &= (0xFF^0x1F);
  *port_str.rtl_address_port_set |= regaddr;
  *port_str.rtl_data_port_dir = 0xFF;
  *port_str.rtl_data_port_set = regdata;
  __no_operation();
  //__no_operation();
  //__no_operation();
  *port_str.rtl_address_port_set &= (0xFF^(1 << IOWB));
  __no_operation();
  //__no_operation();
  //__no_operation();
  //__no_operation();
  *port_str.rtl_address_port_set |= (1 << IOWB);
  __no_operation();
  //__no_operation();
  //__no_operation();
  //__no_operation();
  *port_str.rtl_data_port_dir = 0x00;
  *port_str.rtl_data_port_set = 0xFF;

  #ifdef RTL_DISABLE_INT_BYTE
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE
}

void rtl_t::reset_rtl()
{
//  rtl_port_str = a_port_struct;
  irs_u8 byte;
  // Запрет прерываний
  irs_disable_interrupt();
  // Все линии PORT2 выходы
  *port_str.rtl_address_port_dir = 0xFF;

  // Линию RTL RESDRV выставляем в 1
  *port_str.rtl_address_port_set |= (1 << RSTDRV);
  // Линию RTL RESDRV выставляем в 0
  *port_str.rtl_address_port_set &= (0xFF^(1 << RSTDRV));
  // Задержка после сброса RTL
  counter_t to_wait_rst;
  set_to_cnt(to_wait_rst, TIME_TO_CNT(1,320));
  //set_to_cnt(to_wait_rst, TIME_TO_CNT(1, 1));
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
  *port_str.rtl_data_port_dir = 0x00;
  // Все линии PORT1 заполняются 1
  *port_str.rtl_data_port_set = 0xFF;
  // Адресные биты сбрасываем в 0
  *port_str.rtl_address_port_set &= 0xE0;
  // Линию RTL IORB выставляем в 1
  *port_str.rtl_address_port_set |= (1 << IORB);
  // Линию RTL IOWB выставляем в 1
  *port_str.rtl_address_port_set |= (1 << IOWB);

  byte = readrtl(RSTPORT);////читаем порт резета
  writertl(RSTPORT,byte);
  __no_operation();
  //if((readrtl(ISR)&0x80)==0x0);
  //////////!!!!!!!!!!

  writertl(CR,0x21);
  __no_operation();
  writertl(DCR,dcrval);
  writertl(RBCR0,0x0);
  writertl(RBCR1,0x0);
  writertl(RCR,0x04);
  writertl(TPSR,txstart);
  writertl(TCR,0x02);
  writertl(PSTART,rxstart);
  writertl(BNRY,rxstart);
  writertl(PSTOP,rxstop);
  writertl(CR,0x61);
  __no_operation();
  writertl(CURR,rxstart);

  writertl(0x1,m_mac_save[0]);
  writertl(0x2,m_mac_save[1]);
  writertl(0x3,m_mac_save[2]);
  writertl(0x4,m_mac_save[3]);
  writertl(0x5,m_mac_save[4]);
  writertl(0x6,m_mac_save[5]);

  writertl(CR,0x21);
  writertl(DCR,dcrval);
  writertl(CR,0x22);
  writertl(ISR,0xff);///обнуляем иср
  writertl(IMR,imrval);
  writertl(TCR,tcrval);

  // writertl(CR,0x22);///go!

  // Разрешение прерываний
  irs_enable_interrupt();
}

void rtl_t::rtl_interrupt()
{
  if (!m_is_initialized) return;
  
  #ifdef NOP
  static irs::blink_t blink_14(irs_avr_portd, 3);
  blink_14.flip();
  #endif //NOP

  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #else //RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT

  irs_u8 byte, _byte;
  byte = readrtl(ISR);
  if ((byte & 0x10) == 0x10) {
    overrun();
  }
  if ((byte & 0x1) == 0x1) {
    getpacket();
  }
  if (byte & 0xA) {
    tx_hard = 0;
  }

  byte = readrtl(BNRY); /////Call Read_rtl8019as(BNRY)
                      ///Data_l = Byte_read
  writertl(CR,0x62);   ////Call Write_rtl8019as(CR , &H62)
  _byte = readrtl(CURR);   ///Call Read_rtl8019as(CURR)
                         ///Data_h = Byte_read
  writertl(CR, 0x22); ////Call Write_rtl8019as(CR , &H22)

  //'buffer is not empty, get next packet
  if (byte != _byte) {
    overrun();
  }

  writertl(ISR,0xff);
  writertl(CR,0x22);

  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

bool rtl_t::wait_dma()
{
  counter_t to_wait_end_dma;
  set_to_cnt(to_wait_end_dma, TIME_TO_CNT(1, 2));
  irs_u8 isr_cont = readrtl(ISR);
  while ((isr_cont&RDC) == 0) {
    isr_cont = readrtl(ISR);
    if (test_to_cnt(to_wait_end_dma)) {

  //static irs::blink_t blink_red(irs_avr_porte, 3);
  //blink_red.set();

      #ifdef RTL_RESET_ON_TIMEOUT
      reset_rtl();
      return false;
      #else //RTL_RESET_ON_TIMEOUT
      return true;
      #endif //RTL_RESET_ON_TIMEOUT
    }
  }
  return true;
}

void rtl_t::overrun()
{
  irs_u8 cr_save = readrtl(CR);
  writertl(CR,0x21);
  __no_operation();
  writertl(RBCR0,0x0);
  writertl(RBCR1,0x0);

  if((cr_save&0x4)==0) readrtl(ISR);

  writertl(TCR,0x2);
  writertl(CR,0x22);
  writertl(BNRY,rxstart);
  writertl(CR,0x62);
  writertl(CURR,rxstart);
  writertl(CR,0x22);
  writertl(ISR,0x10);
  writertl(TCR,tcrval);
}

void rtl_t::getpacket()
{
  writertl(CR, 0x1a);
  
  readrtl(RDMAPORT);
  readrtl(RDMAPORT);
  irs_u16 rxlen_hard_cur = 0;
  IRS_LOBYTE(rxlen_hard_cur) = readrtl(RDMAPORT);
  IRS_HIBYTE(rxlen_hard_cur) = readrtl(RDMAPORT);
  
  #ifdef RTL_DBG_MSG
  IRS_LIB_DBG_RAW_MSG(irsm("rtl - receive size = ") << rxlen_hard_cur);
  IRS_LIB_DBG_RAW_MSG(endl);
  #endif //RTL_DBG_MSG
  
  if (rx_hard == 0) {
    if (rxlen_hard_cur <= m_rx_buf_data.size()) {
      // Если размер пакета в норме, то принимаем его
      rx_hard = 1;
      rxlen_hard = rxlen_hard_cur;
      for (irs_u16 i = 0; i < rxlen_hard; i++) {
        rx_buf[i] = readrtl(RDMAPORT);
      }
    } else {
      // Если принятый пакет превышает размер буфера, то отбрасываем его
      for (irs_u16 i = 0; i < rxlen_hard_cur; i++) {
        readrtl(RDMAPORT);
      }
    }
  } else {
    // Если буфер занят, то отбрасываем пакет
    for (irs_u16 i = 0; i < rxlen_hard_cur; i++) {
      readrtl(RDMAPORT);
    }
  }
  
  //if ((byte&RDC) != 64) byte = readrtl(ISR);
  writertl(ISR, 0xF5);
}

void rtl_t::send_packet(irs_u16 length, irs_u8 *ext_tx_buf)
{
  #ifdef RTL_DBG_MSG
  IRS_LIB_DBG_RAW_MSG(irsm("rtl - send size = ") << length << endl);
  #endif //RTL_DBG_MSG
  
  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT
  
  //STA RD2
  //writertl(CR,0x22);
  writertl(CR, cr_rtl_start|cr_dma_abort_complete|cr_page0);
  writertl(TPSR,txstart);
  writertl(RSAR0,0x00);
  writertl(RSAR1,0x40);
  writertl(ISR,0xff);

  writertl(RBCR0, IRS_LOBYTE(length));
  writertl(RBCR1, IRS_HIBYTE(length));
  //writertl(RBCR0,(irs_u8)(length&0xFF));
  //writertl(RBCR1,length>>8);
  writertl(CR,0x12);
  for (irs_u16 i = 0; i < length; i++) writertl(RDMAPORT, ext_tx_buf[i]);

  if (!wait_dma()) return;

  writertl(TBCR0, IRS_LOBYTE(length));
  writertl(TBCR1, IRS_HIBYTE(length));
  //writertl(TBCR0,(irs_u8)(length&0xFF));
  //writertl(TBCR1,length>>8);
  writertl(CR,0x24);
  
  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}

void rtl_t::set_ports(irs_avr_port_t a_data_port, 
  irs_avr_port_t a_address_port)
{
  port_str.rtl_data_port_set = avr_port_map[a_data_port].set;
  port_str.rtl_data_port_get = avr_port_map[a_data_port].get;
  port_str.rtl_data_port_dir = avr_port_map[a_data_port].dir;
  port_str.rtl_address_port_set = avr_port_map[a_address_port].set;
  port_str.rtl_address_port_get = avr_port_map[a_address_port].get;
  port_str.rtl_address_port_dir = avr_port_map[a_address_port].dir;
}

irs_size_t rtl_t::tx_buf_size()
{
  return m_tx_buf_data.size();
}

rtl_t* get_rtl()
{
  static rtl_t rtl;
  return &rtl;
}

}; // namespace 

void initrtl(const irs_u8 *mac, irs_size_t bufs_size)
{
  get_rtl()->init(mac, bufs_size);
}

irs_size_t tx_buf_size()
{
  return get_rtl()->tx_buf_size();
}

void sendpacket(irs_u16 length, irs_u8 *ext_tx_buf)
{
  get_rtl()->send_packet(length, ext_tx_buf);
}

void rtl_set_ports(irs_avr_port_t a_data_port,
  irs_avr_port_t a_address_port)
{
  get_rtl()->set_ports(a_data_port, a_address_port);
}
