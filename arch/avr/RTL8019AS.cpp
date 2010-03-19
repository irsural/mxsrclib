// Драйвер Ethernet для RTL8019AS Димы Уржумцева
// Откорректирован Крашенинников М. В.
// Испорчен Поляковым М.
// Дата: 26.02.2009

#define RTL_RESET_ON_TIMEOUT // Сброс RTL при таймауте в операции
//#define RTL_DISABLE_INT // Выключение прерываний во время операции
#define RTL_DISABLE_INT_BYTE //Запрет прерываний при чтении/записи байтов

#include <ioavr.h>
#include <inavr.h>
#include <RTL8019AS.h>
#include <timer.h>
//#include <irsavrutil.h>

////объявления выводов
////////////////defines hardware
#define ETHERNET_PACKET_MIN_RX 64
#define ETHERNET_PACKET_MAX_RX 1554
#define ETHERNET_PACKET_MIN_TX 60
#define ETHERNET_PACKET_MAX_TX 1550
#if ETHERNET_PACKET_MAX_RX<ETHERNET_PACKET_RX
#undef ETHERNET_PACKET_RX
#define ETHERNET_PACKET_RX ETHERNET_PACKET_MAX_RX
#endif
#if ETHERNET_PACKET_MIN_RX>ETHERNET_PACKET_RX
#undef ETHERNET_PACKET_RX
#define ETHERNET_PACKET_RX ETHERNET_PACKET_MIN_RX
#endif
#if ETHERNET_PACKET_MAX_TX<ETHERNET_PACKET_TX
#undef ETHERNET_PACKET_TX
#define ETHERNET_PACKET_TX ETHERNET_PACKET_MAX_TX
#endif
#if ETHERNET_PACKET_MIN_TX>ETHERNET_PACKET_TX
#undef ETHERNET_PACKET_TX
#define ETHERNET_PACKET_TX ETHERNET_PACKET_MIN_TX
#endif

//ETHERNET_PORT2|=0x20
#define IOR_HI {*port_str.rtl_address_port_set |= (1 << IORB);}
//ETHERNET_PORT2&=(0xFF^0x20)
#define IOR_LO {*port_str.rtl_address_port_set &= (0xFF^(1 << IORB));}
//ETHERNET_PORT2|=0x40
#define IOW_HI {*port_str.rtl_address_port_set |= (1 << IOWB);}
//ETHERNET_PORT2&=(0xFF^0x40)
#define IOW_LO {*port_str.rtl_address_port_set &= (0xFF^(1 << IOWB));}
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
/////дефайны сетевушки
const irs_u8 rstport=0x18;
const irs_u8 isr=0x07;
const irs_u8 cr=0x0;
const irs_u8 dcrval=0x58;
const irs_u8 dcr=0x0e;
const irs_u8 rbcr0=0x0a;
const irs_u8 rbcr1=0x0b;
const irs_u8 rcr=0x0c;
const irs_u8 tpsr=0x04;
const irs_u8 txtstart=0x40;
const irs_u8 tcr=0x0d;
const irs_u8 pstart=0x01;
const irs_u8 rxstart=0x46;
const irs_u8 bnry=0x03;
const irs_u8 pstop=0x02;
const irs_u8 rxstop=0x60;
const irs_u8 curr=0x07;
const irs_u8 imr=0x0f;
const irs_u8 imrval=0x1b;
const irs_u8 tcrval=0x00;
const irs_u8 rsar0=0x8;
const irs_u8 rsar1=0x9;
const irs_u8 tbcr0=0x5;
const irs_u8 tbcr1=0x6;
const irs_u8 rdmaport=0x10;//Const Rdmaport = &H10
const irs_u8 rdc=0x40;//Const Rdc = &H40
///////peremennie setevushki
//int i, rxlen_hard=0;//Dim I as Integer
//irs_u16 i;//Dim I as Integer
irs_u16 rxlen_hard = 0;
irs_u8 rx_hard=0, tx_hard=0; // flags of hardware transmission and resieving
irs_u8 rx_buf[ETHERNET_PACKET_RX];
irs_u8 tx_buf[ETHERNET_PACKET_TX];

struct
{
  p_avr_port_t rtl_data_port_set;
  p_avr_port_t rtl_data_port_get;
  p_avr_port_t rtl_data_port_dir;
  p_avr_port_t rtl_address_port_set;
  p_avr_port_t rtl_address_port_get;
  p_avr_port_t rtl_address_port_dir;
} port_str;

namespace {

const irs_u8 mac_size = 6;
irs_u8 mac_save[mac_size] = {0, 0, 0, 0, 0, 0};

void reset_rtl();
bool wait_dma();
irs_u8 readrtl(irs_u8 regaddr);
void writertl(irs_u8 regaddr, irs_u8 regdata);
void overrun();
void getpacket();

/////interrupt/////////////////////////////////////////////
class rtl_interrupt_t : public mxfact_event_t
{
public:
  virtual void exec()
  {
    #ifdef RTL_DISABLE_INT
    irs_disable_interrupt();
    #else //RTL_DISABLE_INT
    irs_enable_interrupt();
    #endif //RTL_DISABLE_INT

    irs_u8 byte, _byte;
    byte=readrtl(isr);//
    if((byte&0x10)==0x10) overrun(); ///
    if((byte&0x1)==0x1) getpacket();//
    if(byte&0xA) tx_hard=0;

    byte=readrtl(bnry); /////Call Read_rtl8019as(bnry)
                        ///Data_l = Byte_read
    writertl(cr,0x62);   ////Call Write_rtl8019as(cr , &H62)
    _byte=readrtl(curr);   ///Call Read_rtl8019as(curr)
                           ///Data_h = Byte_read
    writertl(cr, 0x22); ////Call Write_rtl8019as(cr , &H22)

    //'buffer is not empty, get next packet
    if (byte != _byte) {
      overrun();
    }

    writertl(isr,0xff);
    writertl(cr,0x22);

    #ifdef RTL_DISABLE_INT
    irs_enable_interrupt();
    #endif //RTL_DISABLE_INT
  }
};

#ifdef NOP
#pragma vector=INT4_vect
__interrupt void int_etherhet(void)
{
  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #else //RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT

  irs_u8 byte, _byte;
  byte=readrtl(isr);//
  if((byte&0x10)==0x10) overrun(); ///
  if((byte&0x1)==0x1) getpacket();//
  if(byte&0xA) tx_hard=0;

  byte=readrtl(bnry); /////Call Read_rtl8019as(bnry)
                      ///Data_l = Byte_read
  writertl(cr,0x62);   ////Call Write_rtl8019as(cr , &H62)
  _byte=readrtl(curr);   ///Call Read_rtl8019as(curr)
                         ///Data_h = Byte_read
  writertl(cr, 0x22); ////Call Write_rtl8019as(cr , &H22)

  //'buffer is not empty, get next packet
  if (byte != _byte) {
    overrun();
  }

  writertl(isr,0xff);
  writertl(cr,0x22);
  
  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}
#endif //NOP

////////////////////////////////////////read and write/////////////
irs_u8 readrtl(irs_u8 regaddr)
{
  #ifdef RTL_DISABLE_INT_BYTE
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT_BYTE
  
  irs_u8 READ=0x0;

  *port_str.rtl_data_port_dir = 0x00;
  *port_str.rtl_data_port_set = 0xFF;
  *port_str.rtl_address_port_set &= (0xFF^0x1F);
  *port_str.rtl_address_port_set |= regaddr;
  IOR_LO;
  __no_operation();
  READ = *port_str.rtl_data_port_get;
  __no_operation();
  IOR_HI;
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

void writertl(irs_u8 regaddr, irs_u8 regdata)
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
  IOW_LO;
  __no_operation();
  //__no_operation();
  //__no_operation();
  //__no_operation();
  IOW_HI;
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

void overrun()
{
  irs_u8 cr_save = readrtl(cr);
  writertl(cr,0x21);
  __no_operation();
  writertl(rbcr0,0x0);
  writertl(rbcr1,0x0);

  if((cr_save&0x4)==0) readrtl(isr);

  writertl(tcr,0x2);
  writertl(cr,0x22);
  writertl(bnry,rxstart);
  writertl(cr,0x62);
  writertl(curr,rxstart);
  writertl(cr,0x22);
  writertl(isr,0x10);
  writertl(tcr,tcrval);
}

void getpacket()
{
  irs_u8 byte;
  writertl(cr, 0x1a);
  byte = readrtl(rdmaport);
  byte = readrtl(rdmaport);
  byte = readrtl(rdmaport);
  irs_u16 rxlen_hard_cur = 0;
  //IRS_HIBYTE(rxlen_hard_cur) = byte;
  IRS_LOBYTE(rxlen_hard_cur) = byte;
  byte = readrtl(rdmaport);
  //rxlen_hard += (byte*256);
  //rxlen_hard |= ((int)byte) << 8;
  //IRS_LOBYTE(rxlen_hard_cur) = byte;
  IRS_HIBYTE(rxlen_hard_cur) = byte;
  //if (!IRS_HIBYTE(rxlen_hard_cur)) PORTB ^= 8;
  //if (IRS_HIBYTE(rxlen_hard_cur)) PORTB |= 4;
  if (rx_hard == 0) {
    if (rxlen_hard_cur > ETHERNET_PACKET_RX) {
      for (irs_u16 i = 0; i < rxlen_hard_cur; i++) {
        byte = readrtl(rdmaport);
      }
    } else {
      rx_hard = 1;
      rxlen_hard = rxlen_hard_cur;
      for (irs_u16 i = 0; i < rxlen_hard; i++) {
        rx_buf[i] = readrtl(rdmaport);
      }
    }
  } else {
    for (irs_u16 i = 0; i < rxlen_hard_cur; i++) {
      byte = readrtl(rdmaport);
    }
  }
  if ((byte&rdc) != 64) byte = readrtl(isr);
  writertl(isr, 0xF5);
}

// Сброс RTL
void reset_rtl()
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
  IOR_HI;
  // Линию RTL IOWB выставляем в 1
  IOW_HI;

  byte = readrtl(rstport);////читаем порт резета
  writertl(rstport,byte);
  __no_operation();
  //if((readrtl(isr)&0x80)==0x0);
  //////////!!!!!!!!!!

  writertl(cr,0x21);
  __no_operation();
  writertl(dcr,dcrval);
  writertl(rbcr0,0x0);
  writertl(rbcr1,0x0);
  writertl(rcr,0x04);
  writertl(tpsr,txtstart);
  writertl(tcr,0x02);
  writertl(pstart,rxstart);
  writertl(bnry,rxstart);
  writertl(pstop,rxstop);
  writertl(cr,0x61);
  __no_operation();
  writertl(curr,rxstart);

  writertl(0x1,mac_save[0]);
  writertl(0x2,mac_save[1]);
  writertl(0x3,mac_save[2]);
  writertl(0x4,mac_save[3]);
  writertl(0x5,mac_save[4]);
  writertl(0x6,mac_save[5]);

  writertl(cr,0x21);
  writertl(dcr,dcrval);
  writertl(cr,0x22);
  writertl(isr,0xff);///обнуляем иср
  writertl(imr,imrval);
  writertl(tcr,tcrval);

  // writertl(cr,0x22);///go!

  // Разрешение прерываний
  irs_enable_interrupt();
}
// Ожидание завершения работы DMA
bool wait_dma()
{
  counter_t to_wait_end_dma;
  set_to_cnt(to_wait_end_dma, TIME_TO_CNT(1, 2));
  irs_u8 isr_cont = readrtl(isr);
  while ((isr_cont&rdc) == 0) {
    isr_cont = readrtl(isr);
    if (test_to_cnt(to_wait_end_dma)) {

  //static irs::blink_t blink_red(irs_avr_porte, 3);
  //blink_red.set();

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

} //namespace

void initrtl(const irs_u8 *mac)
{
  memcpy(mac_save, mac, mac_size);

  static rtl_interrupt_t rtl_interrupt;
  irs_avr_int4_int.add(&rtl_interrupt);

  // Сброс RTL
  reset_rtl();
}

void sendpacket(irs_u16 length, irs_u8 *tx_buf)
{
  #ifdef RTL_DISABLE_INT
  irs_disable_interrupt();
  #endif //RTL_DISABLE_INT
  
  writertl(cr,0x22);
  writertl(tpsr,txtstart);
  writertl(rsar0,0x00);
  writertl(rsar1,0x40);
  writertl(isr,0xff);

  writertl(rbcr0, IRS_LOBYTE(length));
  writertl(rbcr1, IRS_HIBYTE(length));
  //writertl(rbcr0,(irs_u8)(length&0xFF));
  //writertl(rbcr1,length>>8);
  writertl(cr,0x12);
  for (irs_u16 i = 0; i < length; i++) writertl(rdmaport, tx_buf[i]);

  if (!wait_dma()) return;

  writertl(tbcr0, IRS_LOBYTE(length));
  writertl(tbcr1, IRS_HIBYTE(length));
  //writertl(tbcr0,(irs_u8)(length&0xFF));
  //writertl(tbcr1,length>>8);
  writertl(cr,0x24);
  
  #ifdef RTL_DISABLE_INT
  irs_enable_interrupt();
  #endif //RTL_DISABLE_INT
}


void rtl_set_ports(irs_avr_port_t a_data_port, irs_avr_port_t a_address_port)
{
  port_str.rtl_data_port_set = avr_port_map[a_data_port].set;
  port_str.rtl_data_port_get = avr_port_map[a_data_port].get;
  port_str.rtl_data_port_dir = avr_port_map[a_data_port].dir;
  port_str.rtl_address_port_set = avr_port_map[a_address_port].set;
  port_str.rtl_address_port_get = avr_port_map[a_address_port].get;
  port_str.rtl_address_port_dir = avr_port_map[a_address_port].dir;
}
