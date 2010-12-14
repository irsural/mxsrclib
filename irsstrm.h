//! \file
//! \ingroup in_out_group
//! \brief Потоки ввода/вывода ИРС
//!
//! Дата: 08.10.2010\n
//! Ранняя дата: 14.09.2009

#ifndef IRSSTRMH
#define IRSSTRMH

//#define NEW

#include <irsdefs.h>

#include <irscpp.h>
//#include <irsstd.h>
#include <mxdata.h>
#include <irsstrmstd.h>
#include <irsconsole.h>
#include <irscpu.h>

#ifdef __ICCAVR__
#include <ioavr.h>
#endif //__ICCAVR__

#ifdef __ICCARM__
#include <armioregs.h>
#endif //__ICCARM__

#ifdef IRS_LINUX
#include <fcntl.h>
#include <sys/termios.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#endif //IRS_LINUX

#include <irsfinal.h>

namespace irs {

//! \ingroup in_out_group
//typedef<class container_type = vector<streambuf*> >
class union_streambuf: public streambuf
{
public:
  typedef size_t size_type;
  typedef vector<streambuf*> container_type;
  inline union_streambuf(size_type a_outbuf_size = 0);
  inline virtual ~union_streambuf();
  inline virtual int overflow(int c = EOF);
  inline virtual int sync();
  inline void insert(streambuf* ap_buf);
  inline void erase(streambuf* ap_buf);
  inline void clear();
  //inline void part_overflow(streambuf* ap_streambuf);
private:
  raw_data_t<char> m_outbuf;
  container_type m_streambufs;
  //static char m_overflow_end_symbol;
};
inline union_streambuf::union_streambuf(size_type a_outbuf_size):
  m_outbuf(a_outbuf_size),
  m_streambufs()
  //m_overflow_end_symbol(0)
{
  setp(m_outbuf.data(), m_outbuf.data() + m_outbuf.size());
}
inline union_streambuf::~union_streambuf()
{ }
/*inline void union_streambuf::part_overflow(streambuf* ap_streambuf)
{
  size_type put_byte_count = 0;
  while (put_byte_count < m_outbuf.size()) {
    put_byte_count +=
      ap_streambuf->sputn(m_outbuf.data()+put_byte_count,
      m_outbuf.size()-put_byte_count);
    ap_streambuf->pubsync();
  }
  const size_type symbol_count = 1;
  int res = ap_streambuf->sputc(m_overflow_end_symbol);
  if (res) {
    IRS_LIB_DBG_MSG_SRC("Функция завершилась неудачно");
  }
}*/
inline int union_streambuf::overflow(int c)
{
  int result = 0;
  const size_type streambuf_count = m_streambufs.size();
  for (size_type buf_i = 0; buf_i < streambuf_count; buf_i++) {
    size_type put_byte_count = 0;
    size_type buf_fill_size = pptr() - pbase();
    while (put_byte_count < buf_fill_size) {
      int sended_count =
        m_streambufs[buf_i]->sputn(m_outbuf.data()+put_byte_count,
        buf_fill_size-put_byte_count);
      put_byte_count += sended_count;
      if (sended_count == 0) {
        // Проверяем можно ли еще что-нибудь записать
        if (m_streambufs[buf_i]->sputc(m_outbuf[put_byte_count]) == EOF) {
          result = EOF;
          break;
        } else {
          put_byte_count++;
        }
      } else {
        // Запись успешна
      }
    }
    if (c != EOF) {
      result = m_streambufs[buf_i]->sputc(static_cast<char>(c));
    }
    #ifdef __WATCOMC__
    m_streambufs[buf_i]->sync();
    #else //__WATCOMC__
    m_streambufs[buf_i]->pubsync();
    #endif //__WATCOMC__
  }
  setp(m_outbuf.data(), m_outbuf.data() + m_outbuf.size());
  return result;
}
inline int union_streambuf::sync()
{
  return overflow();
}
inline void union_streambuf::insert(streambuf* ap_buf)
{
  m_streambufs.insert(m_streambufs.end(), ap_buf);
}
inline void union_streambuf::erase(streambuf* ap_buf)
{
  container_type::iterator it_buf = find(
    m_streambufs.begin(), m_streambufs.end(), ap_buf);
  IRS_LIB_ASSERT(it_buf != m_streambufs.end());
  m_streambufs.erase(it_buf);
}
inline void union_streambuf::clear()
{
  m_streambufs.clear();
}

#ifdef __ICCAVR__

namespace avr {

// Буфер потоков для COM-порта AVR
class com_buf: public streambuf
{
public:
  inline com_buf(const com_buf& a_buf);
  // В AVR com_index == 1 для регистров с индексом 0
  //       com_index == 2 для регистров с индексом 1
  inline com_buf(
    int a_com_index = 1,
    int a_outbuf_size = 10, 
    const irs_u32 a_baud_rate = 9600
  );
  //inline virtual ~com_buf();
  inline virtual int overflow(int c = EOF);
  inline virtual int sync();
  inline void trans (char data);
  inline void trans_simple (char data);
private:
  int m_outbuf_size;
  auto_arr<char> m_outbuf;
  const irs_u32 m_baud_rate;
};
inline com_buf::com_buf(const com_buf& a_buf):
  m_outbuf_size(a_buf.m_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  m_baud_rate(a_buf.m_baud_rate)
{
  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
}
inline com_buf::com_buf(
  int a_com_index,
  int a_outbuf_size,
  const irs_u32 a_baud_rate
):
  m_outbuf_size(a_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  m_baud_rate(a_baud_rate)
{
  const irs_u32 FOSC = 16000000;//Частота процессора
  irs_u16 ubrr = FOSC/16/m_baud_rate - 1;

  if (a_com_index == 1) {
    PRR0_PRUSART0 = 0; //Включение питания USART0
    
    UBRR0H = IRS_HIBYTE(ubrr);
    UBRR0L = IRS_LOBYTE(ubrr);
    UCSR0B_RXEN0 = 0;//Разрешение приема
    UCSR0B_TXEN0 = 1;//Разрешение передачи

    UCSR0B_RXCIE0 = 0;//разрешение прерывания по завершению приема
    UCSR0B_TXCIE0 = 0;//разрешение прерывания по завершении передачи
    UCSR0B_UDRIE0 = 0;//разрешение прерывания при очистке регистра данных UART
    UCSR0B_UCSZ02 = 0;//размер передаваемого слова
    UCSR0B_RXB80 = 0;//8 бит принимаемых данных
    UCSR0B_TXB80 = 0;//8 бит передаваемых данных

    #ifdef __ATmega128__
    UCSR0C_UMSEL0 = 0;//режим работы USART
    #else //__ATmega128__
    UCSR0C_UMSEL01 = 0;//режим работы USART
    UCSR0C_UMSEL00 = 0;//режим работы USART
    #endif //__ATmega128__
    UCSR0C_UPM01 = 0;//режим работы схемы контроля и формирования бита четности
    UCSR0C_UPM00 = 0;//режим работы схемы контроля и формирования бита четности
    UCSR0C_USBS0 = 0;//количество стоп - битов
    UCSR0C_UCSZ01 = 1;//размер передаваемого слова
    UCSR0C_UCSZ00 = 1;//размер передаваемого слова
    UCSR0C_UCPOL0 = 0;//полярность тактового сигнала

    UCSR0A_MPCM0 = 0;//режим мультипроцессорного обмена
    UCSR0A_U2X0 = 0;//удвоение скорости обмена
  } else {
    PRR1_PRUSART1 = 0; //Включение питания USART1
    
    UBRR1H = IRS_HIBYTE(ubrr);
    UBRR1L = IRS_LOBYTE(ubrr);
    UCSR1B_RXEN1 = 0;//Разрешение приема
    UCSR1B_TXEN1 = 1;//Разрешение передачи

    UCSR1B_RXCIE1 = 0;//разрешение прерывания по завершению приема
    UCSR1B_TXCIE1 = 0;//разрешение прерывания по завершении передачи
    UCSR1B_UDRIE1 = 0;//разрешение прерывания при очистке регистра данных UART
    UCSR1B_UCSZ12 = 0;//размер передаваемого слова
    UCSR1B_RXB81 = 0;//8 бит принимаемых данных
    UCSR1B_TXB81 = 0;//8 бит передаваемых данных

    #ifdef __ATmega128__
    UCSR1C_UMSEL1 = 0;//режим работы USART
    #else //__ATmega128__
    UCSR1C_UMSEL11 = 0;//режим работы USART
    UCSR1C_UMSEL10 = 0;//режим работы USART
    #endif //__ATmega128__
    UCSR1C_UPM11 = 0;//режим работы схемы контроля и формирования бита четности
    UCSR1C_UPM10 = 0;//режим работы схемы контроля и формирования бита четности
    UCSR1C_USBS1 = 0;//количество стоп - битов
    UCSR1C_UCSZ11 = 1;//размер передаваемого слова
    UCSR1C_UCSZ10 = 1;//размер передаваемого слова
    UCSR1C_UCPOL1 = 0;//полярность тактового сигнала

    UCSR1A_MPCM1 = 0;//режим мультипроцессорного обмена
    UCSR1A_U2X1 = 0;//удвоение скорости обмена
  }

  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
  
  // Задержка необходимая для того, чтобы COM-порт успел инициализироватся
  __delay_cycles(4000);
}
/*inline irs::com_buf::~com_buf()
{
}*/
inline void com_buf::trans (char data)
{
  if (data == '\n'){
    trans_simple('\r');
    trans_simple('\n');
  } else {
    trans_simple(data);
  }
}
inline void com_buf::trans_simple (char data)
{
  while (!UCSR0A_UDRE0);
  UCSR0A_UDRE0 = 1;
  UDR0 = data;
}
inline int com_buf::overflow(int c)
{
  int len_s = pptr() - pbase();
  if (len_s > 0) {
    *pptr() = 0;
    char* pend = pptr();
    for(char *message = pbase(); message<pend; message++ ) {
      trans(*message);
    }
  }
  if (c != EOF) {
    trans(c);
  }
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
  return 0;
}
inline int com_buf::sync()
{
  return overflow();
}

// Поток COM-порта AVR
class com_ostream: public ostream
{
public:
  inline com_ostream(int a_com_index = 1, int a_outbuf_size = 10);
private:
  com_buf m_buf;
};
inline com_ostream::com_ostream(int a_com_index, int a_outbuf_size):
  ostream(&m_buf),
  m_buf(a_com_index, a_outbuf_size)
{
  //rdbuf(&m_buf);
  init(&m_buf);
}

} //namespace avr

#endif //__ICCAVR__

#ifdef IRS_LINUX
#ifdef NEW
// Поток ввода для Linux без блокировки (ожидания)
class linux_terminal_buf: public streambuf
{
public:
  typedef char char_type;

  linux_terminal_buf(streamsize a_size = 10);
  ~linux_terminal_buf();

  virtual int underflow();
private:
  raw_data_t<char_type> m_raw_buf;
  char_type *m_raw_buf_first = m_raw_buf.data();
  char_type *m_raw_buf_last = raw_buf_first + m_raw_buf.size();
};
inline linux_terminal_buf(streamsize a_size):
  m_raw_buf(a_size),
  m_raw_buf_first(m_raw_buf.data()),
  m_raw_buf_last(raw_buf_first + m_raw_buf.size())
{
  setg(m_raw_buf_first, m_raw_buf_last, m_raw_buf_last);
}
inline ~linux_terminal_buf()
{
}
int underflow()
{
}

class console_t
{
public:
  console_t(mxdisplay_drv_t* mp_display_drv,
    mxkey_drv_t* ap_key_drv = IRS_NULL);
  console_t(streambuf* ap_out_buf,
    streambuf* ap_in_buf = IRS_NULL);
  void tick();
  void pos(size_t a_horizontal, size_t a_vertical);
  void in_start(size_t a_max_chars);
  void in_stop();
  bool in_ready();
private:
  mxdisplay_drv_t* mp_display_drv;
  mxkey_drv_t* mp_key_drv;
  streambuf* mp_out_buf;
  streambuf* mp_in_buf;
  ostream m_out;
  istream m_in;
};
#endif //NEW
#endif //IRS_LINUX

// Сохранение/восстановление формата потоков
class ostream_format_save_t
{
private:
  ostream *mp_ostream;
  ios::fmtflags m_format_flags;
  irs_streamsize_t m_width;
  irs_streamsize_t m_precision;
public:
  ostream_format_save_t(ostream *ap_ostream = IRS_NULL):
    mp_ostream(ap_ostream),
    m_format_flags(ios::fmtflags()),
    m_width(0),
    m_precision(0)
  {
    save_format();
  }
  ~ostream_format_save_t()
  {
    restore_format();
  }
  ostream* out_stream()
  {
    return mp_ostream;
  }
  void out_stream(ostream* ap_ostream)
  {
    mp_ostream = ap_ostream;
  }
  void save_format()
  {
    if (mp_ostream) {
      m_format_flags = mp_ostream->flags();
      m_width = mp_ostream->width();
      m_precision = mp_ostream->precision();
    }
  }
  void restore_format()
  {
    if (mp_ostream) {
      mp_ostream->flags(m_format_flags);
      mp_ostream->width(m_width);
      mp_ostream->precision(m_precision);
    }
  }
};

template <class T>
void out_hex(ostream *ap_strm, const T& a_value)
{
  ostream_format_save_t format_save(ap_strm);
  enum { hex_chars_in_byte = 2 };
  (*ap_strm) << hex << setw(hex_chars_in_byte*sizeof(T));
  (*ap_strm) << uppercase << setfill('0');
  if (sizeof(T) != sizeof(char)) {
    (*ap_strm) << a_value;
  } else {
    (*ap_strm) << static_cast<int>(a_value);
  }
}

template <class T>
void out_hex_0x(ostream *ap_strm, const T& a_value)
{
  (*ap_strm) << "0x";
  out_hex(ap_strm, a_value);
}

#ifdef __ICCARM__

namespace arm {

// Буфер потоков для COM-порта ARM (LM3S8971)
class com_buf: public streambuf
{
public:
  inline com_buf(const com_buf& a_buf);
  // В LM3S8971 UART один, com_index не используется и оставлен на будущее
  inline com_buf(
    int a_com_index = 1,
    int a_outbuf_size = 16, 
    const irs_u32 a_baud_rate = 9600
  );
  //inline virtual ~com_buf();
  inline virtual int overflow(int c = EOF);
  inline virtual int sync();
  inline void trans (char data);
  inline void trans_simple (char data);
private:
  enum {
    PORTA1_UART0Tx = 0x10
  };
  
  int m_outbuf_size;
  auto_arr<char> m_outbuf;
  const irs_u32 m_baud_rate;
};
inline com_buf::com_buf(const com_buf& a_buf):
  m_outbuf_size(a_buf.m_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  m_baud_rate(a_buf.m_baud_rate)
{
  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
}
inline com_buf::com_buf(
  int /*a_com_index*/,
  int a_outbuf_size,
  const irs_u32 a_baud_rate
):
  m_outbuf_size(a_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  m_baud_rate(a_baud_rate)
{
  const float FOSC = cpu_traits_t::frequency(); //  Частота процессора
  float BRD = FOSC / 16.f / float(m_baud_rate);
  irs_u16 BRDI = irs_u16(BRD);  //  Целая часть делителя
  irs_u16 BRDF = irs_u16((BRD - float(BRDI))*64.f + 0.5f); //  Дробная часть
  
  RCGC1_bit.UART0 = 1;      //  Подача тактовой частоты на модуль UART0
  RCGC2_bit.PORTA = 1;      //  Подача тактовой частоты на PORTA (нога TX)
  for (irs_u8 i = 10; i; i--);
  
  UART0CTL_bit.UARTEN = 0;  //  Отключение UART0
  UART0CTL_bit.HSE = 0;     //  ClkDiv = 16
  UART0IBRD = BRDI;
  UART0FBRD = BRDF;
  UART0LCRH_bit.SPS = 0;    //  Проверка чётности выключена
  UART0LCRH_bit.WLEN = 0x3; //  8 бит
  UART0LCRH_bit.FEN = 1;    //  FIFO включен
  UART0LCRH_bit.STP2 = 0;   //  2 стоп-бита выключено
  UART0LCRH_bit.EPS = 0;    //  Проверка чётности выключена
  UART0LCRH_bit.PEN = 0;    //  Проверка чётности выключена
  UART0LCRH_bit.BRK = 0;    //  Лень писать что, но тоже выключено
  UART0CTL_bit.RXE = 0;     //  Приём выключен
  UART0CTL_bit.TXE = 1;     //  Передача включена
  UART0CTL_bit.LBE = 0;     //  Loop-back выключен
  //UART0CTL_bit.no2 = 0;     //  Экономный режим ИК-трансивера выключен
  //UART0CTL_bit.no1 = 0;     //  И сам трансивер тоже
  UART0IM = 0;              //  Прерывания выключены
  UART0CTL_bit.UARTEN = 1;  //  UART0 включен
  
  GPIOADEN_bit.no1 = 1;     //  Нога TX включена
  GPIOADIR_bit.no1 = 1;     //  Нога TX выход
  GPIOAAFSEL_bit.no1 = 1;   //  Альтернативная функция включена
  //
  #ifdef __LM3SxBxx__
  (*((volatile irs_u32*)(PORTA_BASE + GPIO_PCTL))) |= PORTA1_UART0Tx;
  #endif // __LM3SxBxx__

  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
  
  // Задержка необходимая для того, чтобы COM-порт успел инициализироватся
  //__delay_cycles(4000);
}
/*inline irs::com_buf::~com_buf()
{
}*/
inline void com_buf::trans (char data)
{
  if (data == '\n'){
    trans_simple('\r');
    trans_simple('\n');
  } else {
    trans_simple(data);
  }
}
inline void com_buf::trans_simple (char data)
{
  #ifdef __LM3Sx9xx__
  UART0DR = data;
  #endif // __LM3Sx9xx__
  #ifdef __LM3SxBxx__
  UART0DR_bit.DATA = data;
  #endif // __LM3SxBxx__
  while (UART0FR_bit.TXFF);
}
inline int com_buf::overflow(int c)
{
  int len_s = pptr() - pbase();
  if (len_s > 0) {
    *pptr() = 0;
    char* pend = pptr();
    for(char *message = pbase(); message<pend; message++ ) {
      trans(*message);
    }
  }
  if (c != EOF) {
    trans(c);
  }
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
  return 0;
}
inline int com_buf::sync()
{
  return overflow();
}

// Поток COM-порта ARM
class com_ostream: public ostream
{
public:
  inline com_ostream(int a_com_index = 1, int a_outbuf_size = 16);
private:
  com_buf m_buf;
};
inline com_ostream::com_ostream(int a_com_index, int a_outbuf_size):
  ostream(&m_buf),
  m_buf(a_com_index, a_outbuf_size)
{
  //rdbuf(&m_buf);
  init(&m_buf);
}

} //namespace arm

#endif //__ICCAVR__

} //namespace irs

#endif //IRSSTRMH
