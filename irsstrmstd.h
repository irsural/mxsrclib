// Потоки ввода/вывода ИРС из irsstd
// Дата: 03.10.2011\n
// Дата создания: 27.09.2009

#ifndef IRSSTRMSTDH
#define IRSSTRMSTDH

#include <irsdefs.h>

#include <irscpp.h>
#include <irsconsolestd.h>
#include <irsstrdefs.h>
#include <irsstrconv.h>
#include <mxdata.h>

#ifdef __BORLANDC__
#include <StdCtrls.hpp>
#endif // __BORLANDC__

#ifdef QT_CORE_LIB
#include <QPlainTextEdit>
#endif // QT_CORE_LIB

#include <irsfinal.h>

#define IRSSTRM_NEW_MEMOBUF

//! \addtogroup text_user_interface_group
//! @{

// класс буфер
class irs_strm_buf: public streambuf
{
  //для связи с внешним обьекторм класса mxkey_event_t
  mxkey_event_t* mp_mxkey_event;
  irskey_t m_temporary_value_key;
  mxdisplay_drv_service_t m_display_drv_service;
  vector<char> m_BufTextDisplay;          //для скроллинга(экранная память)
  char* mp_InBuf;                    //буфер ввода
  char* mp_TemporaryInBuf;
  irs_u32 m_LengthInBuf;           //Длина буфера
  mxdisplay_drv_t *mp_driver;
  mxdisp_pos_t m_left_activecursor_buf;      //координата X курсора в буфере
  mxdisp_pos_t m_top_activecursor_buf;       //координата Y курсора в буфере
  mxdisp_pos_t m_width_display;     //ширина экрана
  mxdisp_pos_t m_height_display;    //высота экрана
  counter_t m_test_to;
  irs_u32 m_num;
  irs_u32 m_denom;
  irs_u32 m_num_nucl_cur;       //для мигания курсором
  irs_u32 m_denom_cur;          //для мигания курсором
  counter_t m_test_to_cur;      //для мигания курсором
  irs_bool m_cursor_visible;    //флаг, горит ли курсор в данный момент
  irs_bool m_cursor_blink;      //флаг, надо ли мигать курсором
  //переменная для временного хранения текстового символа
  char m_AlphaCharacter[2];
  irs_bool m_flag_processing_key_event;
  irs_bool m_flag_check_get;

public:
  inline irs_strm_buf();
  inline virtual ~irs_strm_buf();
  inline mxdisp_pos_t get_height();
  inline mxdisp_pos_t get_width();
  // коннектимся к драйверу
  inline void connect(mxdisplay_drv_t *ap_driver);
  //коннектимся к обьекту класса mxkey_event_t
  inline void connect(mxkey_event_t* ap_mxkey_event);
  //очистка всего буфера и экрана
  inline void clear();
  //очистка сроки с номером  a_top
  inline void clear_line(mxdisp_pos_t a_top);
  //задаем позицию курсора
  inline void setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top);
  inline virtual int sync();
  inline virtual int overflow( int = EOF );
  inline virtual int underflow();
  //задаем скорость вывода текста на экран
  inline void set_refresh_time_out_text(irs_u32 a_num=1, irs_u32 a_denom=10);
  //задаем скорость мигания курсора
  inline void set_refresh_time_blink_cursor(irs_u32 a_num=1, irs_u32 a_denom=2);
  inline void tick();
  inline void start_get();
  inline irs_bool check_get();
  inline void stop_get();
private:
  //начать мигать курсором
  inline void blink_cursor(irs_i8 a_parametr_immediate_blink_cursor = 0);
  inline void stop_blink_cursor();     //прекратить мигать курсором
  inline void processing_key_event();  //обработка событий клавиатуры
  inline void outputbuf(irs_i32 a_ich);  //вывод в экранный буфер
};
inline irs_strm_buf::irs_strm_buf(
):
  streambuf(),
  mp_mxkey_event(IRS_NULL),
  mp_InBuf(IRS_NULL),
  mp_TemporaryInBuf(IRS_NULL),
  m_LengthInBuf(0),
  mp_driver(IRS_NULL),
  m_left_activecursor_buf(0),     //текущая координата X курсора в буфере
  m_top_activecursor_buf(0),      //текущая координата Y курсора в буфере
  m_width_display(0),               //ширина экрана
  m_height_display(0),              //высота экрана(видимой части)
  m_test_to(0),
  m_num(1),
  m_denom(10),
  m_num_nucl_cur(1),                //для мигания курсором
  m_denom_cur(2),              //для мигания курсором
  m_test_to_cur(0),                 //для мигания курсором
  m_cursor_visible(irs_false),
  m_cursor_blink(irs_false),
  m_flag_processing_key_event(IRS_NULL),
  m_flag_check_get(irs_false)
{
  m_AlphaCharacter[0]='\0';
  m_AlphaCharacter[1]='\0';
  setg(NULL, NULL, NULL);         //обнуляем внутренний буфер filebuf
  init_to_cnt();
  set_to_cnt(m_test_to, TIME_TO_CNT(m_num, m_denom));
  set_to_cnt(m_test_to_cur, TIME_TO_CNT(m_num_nucl_cur, m_denom_cur));
}
inline irs_strm_buf::~irs_strm_buf()
{
  deinit_to_cnt();
}
inline mxdisp_pos_t  irs_strm_buf::get_height()
{
  return m_display_drv_service.get_height();// mp_driver->get_height();
}
inline mxdisp_pos_t  irs_strm_buf::get_width()
{
  return m_display_drv_service.get_width(); //mp_driver->get_width();
}
inline void irs_strm_buf::clear()
{
  m_display_drv_service.clear();
}
inline void irs_strm_buf::clear_line(mxdisp_pos_t a_top)
{
  m_display_drv_service.clear_line(a_top);
}
inline void irs_strm_buf::start_get()
{
  if(m_LengthInBuf>1)
    delete []mp_InBuf;
  m_LengthInBuf=0;
  m_cursor_blink=irs_true;
  mp_mxkey_event->reset();
  m_flag_processing_key_event=irs_true;    //начать запись входного буфера
}
inline irs_bool irs_strm_buf::check_get()
{
  return m_flag_check_get;
}
inline void irs_strm_buf::connect(mxdisplay_drv_t *ap_driver)
{
  m_display_drv_service.connect(ap_driver);
  mp_driver=ap_driver;
  m_width_display=get_width();        //запоминаем ширину экрана
  m_height_display=get_height();      //запоминаем высоту экрана(видимой части)
  //создаем экранную память
  for(irs_u32 W=0; W<m_height_display; W++)
  {
    m_BufTextDisplay.insert(m_BufTextDisplay.end(), m_width_display ,' ');
    m_BufTextDisplay.push_back('\0');
  }
}
// коннектимся к обьекту класса mxkey_event_t)(коннектимся к драйверу)
inline void irs_strm_buf::connect(mxkey_event_t* ap_mxkey_event)
{
  mp_mxkey_event=ap_mxkey_event;
}
inline int irs_strm_buf::sync()
{
  for(mxdisp_pos_t G=0;G<m_height_display;G++)
  {
    m_display_drv_service.outtextpos(
      0,
      G,
      &(m_BufTextDisplay[(m_width_display+1)*G])
    );
  }
  return 0;
}
inline int irs_strm_buf::overflow(int ich)
{
  outputbuf(ich);
  return ich;
}
inline int irs_strm_buf::underflow()
{
  return EOF;
}
inline void irs_strm_buf::set_refresh_time_out_text(irs_u32 a_num, irs_u32 a_denom)
{
  m_num=a_num;
  m_denom=a_denom;
}
inline void irs_strm_buf::set_refresh_time_blink_cursor(irs_u32 a_num, irs_u32 a_denom)
{
  m_num_nucl_cur=a_num;
  m_denom_cur=a_denom;
}
inline void irs_strm_buf::tick()
{
  if(test_to_cnt(m_test_to))
  {
    set_to_cnt(m_test_to, TIME_TO_CNT(m_num, m_denom));
  }
  if(m_cursor_blink==irs_true)       //надо ли мигать курсором
    blink_cursor();
  if(m_flag_processing_key_event==irs_true)
    processing_key_event();
} //функция - начать мигать курсором
inline void irs_strm_buf::blink_cursor(irs_i8 a_parametr_immediate_blink_cursor)
{
  char ChCursor[2]; //вид курсора
  ChCursor[0] = '_';
  ChCursor[1] = '\0';
  irs_u8 one=0;
  if(test_to_cnt(m_test_to_cur) || a_parametr_immediate_blink_cursor != 0)
  {
    //если достигнут правый край экрана
    if(m_left_activecursor_buf >= m_width_display && m_width_display > 0)
      one=1;
    else
      one=0;
    if((m_cursor_visible == irs_false ||
    a_parametr_immediate_blink_cursor == 1)&&
    a_parametr_immediate_blink_cursor != -1)  //отобразить курсор
    {
      m_display_drv_service.outtextpos(
      static_cast<mxdisp_pos_t>(m_left_activecursor_buf-one),
      m_top_activecursor_buf,
      ChCursor);
      if(m_top_activecursor_buf>0)
        m_AlphaCharacter[0]=
          m_BufTextDisplay[(m_top_activecursor_buf)*(m_width_display+1)+
          m_left_activecursor_buf-one];
      else
        m_AlphaCharacter[0]=m_BufTextDisplay[m_left_activecursor_buf-one];
      m_cursor_visible=irs_true;
    }
    //погасить курсор
    else if(m_cursor_visible == irs_true ||
      a_parametr_immediate_blink_cursor == -1)
    {
      m_display_drv_service.outtextpos(
        static_cast<mxdisp_pos_t>(m_left_activecursor_buf-one),
        m_top_activecursor_buf,
        m_AlphaCharacter);
        m_cursor_visible=irs_false;
    }
    set_to_cnt(m_test_to_cur, TIME_TO_CNT(m_num_nucl_cur, m_denom_cur));
  }
}
inline void irs_strm_buf::stop_blink_cursor()           //прекратить мигать курсором
{
  if(m_left_activecursor_buf >= m_width_display && m_width_display > 0)
    m_display_drv_service.outtextpos(
      static_cast<mxdisp_pos_t>(m_left_activecursor_buf-1),
      m_top_activecursor_buf,
      m_AlphaCharacter);
  else
    m_display_drv_service.outtextpos(
      m_left_activecursor_buf,
      m_top_activecursor_buf,
      m_AlphaCharacter);
  m_cursor_visible=irs_false;
}
inline void irs_strm_buf::processing_key_event()  //обработка событий клавиатуры
{
  char ch='\0';
  // параметр выделения памяти: "0" - не выделять;
  // "1" - выделить на 1 байт больше; "-1" -выделить на 1 байт меньше
  irs_i8 parameter_new_memory=0;
  m_temporary_value_key=mp_mxkey_event->check();
  switch(m_temporary_value_key)
  {
    case irskey_0: parameter_new_memory=1; ch='0';break;
    case irskey_1: parameter_new_memory=1; ch='1';break;
    case irskey_2: parameter_new_memory=1; ch='2';break;
    case irskey_3: parameter_new_memory=1; ch='3';break;
    case irskey_4: parameter_new_memory=1; ch='4';break;
    case irskey_5: parameter_new_memory=1; ch='5';break;
    case irskey_6: parameter_new_memory=1; ch='6';break;
    case irskey_7: parameter_new_memory=1; ch='7';break;
    case irskey_8: parameter_new_memory=1; ch='8';break;
    case irskey_9: parameter_new_memory=1; ch='9';break;
    case irskey_point: parameter_new_memory=1; ch='.';break;
    case irskey_enter: parameter_new_memory=0; ch='\n';
      m_flag_check_get=irs_true; break;
    case irskey_up: parameter_new_memory=0; mp_InBuf[m_LengthInBuf-1]='9';break;
    case irskey_backspace:  parameter_new_memory=-1; break;
    case irskey_escape: parameter_new_memory=0; break;
    default:  parameter_new_memory=0;
  }
  if(parameter_new_memory == 1)
  {
    blink_cursor(-1);                       //немедленно погасить курсор
    mp_TemporaryInBuf=mp_InBuf;
    mp_InBuf=new char[++m_LengthInBuf];
    outputbuf(ch);                          //пишем символ в экранную память
    for(irs_u32 i=0; i<m_LengthInBuf-1; i++)
      mp_InBuf[i]=mp_TemporaryInBuf[i];
    mp_InBuf[m_LengthInBuf-1]=ch;
    if(m_LengthInBuf > 1)
      delete []mp_TemporaryInBuf;
    blink_cursor(1);                        //немедленно отобразить курсор
  }
  else if(parameter_new_memory == -1 && m_LengthInBuf > 0)
  {
    blink_cursor(-1);                 //немедленно погасить курсор
    mp_TemporaryInBuf=mp_InBuf;
    mp_InBuf=new char[--m_LengthInBuf];
    if(m_left_activecursor_buf > 0)
      m_left_activecursor_buf--;
    else
    {
      if(m_top_activecursor_buf > 0)
      {
        m_top_activecursor_buf--; //переходим на предыдущую строку
        m_left_activecursor_buf=
          static_cast<mxdisp_pos_t>(m_width_display-1);
      }
    }
    outputbuf(' ');
    if(m_left_activecursor_buf > 0)
      m_left_activecursor_buf--;
    else
    {
      if(m_top_activecursor_buf>0)
      {
        m_top_activecursor_buf--; //переходим на предыдущую строку
        m_left_activecursor_buf=
          static_cast<mxdisp_pos_t>(m_width_display-1);
      }
    }
    for(irs_u32 i=0; i<m_LengthInBuf; i++)
      mp_InBuf[i]=mp_TemporaryInBuf[i];
    delete []mp_TemporaryInBuf;
    blink_cursor(1);                     //немедленно отобразить курсор
  }
}
inline void irs_strm_buf::outputbuf(irs_i32 a_ich)      //вывод в экранный буфер
{
  char ChTime[2];
  ChTime[0] = static_cast<char>(a_ich);
  ChTime[1] = '\0';
  if(m_left_activecursor_buf >= m_width_display || ChTime[0] == '\n')
  { //если текущая строка последняя, делаем скроллинг
    if(m_top_activecursor_buf > m_height_display-2)
    {
      //сдвигаемн на одну строчку вверх
      irs_u32 LeftShiftStr=m_width_display+1;
      irs_u32 LenBufTextDisplay=m_height_display*(m_width_display+1)-
        LeftShiftStr;
      for(irs_u32 PositioinCur=0;PositioinCur<LenBufTextDisplay;PositioinCur++)
        m_BufTextDisplay[PositioinCur]=
          m_BufTextDisplay[LeftShiftStr+PositioinCur];
      irs_u32 LeftShiftAtEndStr=LeftShiftStr*(m_height_display-1);
      //очищаем буфер,заполняя его пробелами
      for(irs_u32 PositioinCur=0; PositioinCur<m_width_display;PositioinCur++)
        m_BufTextDisplay[LeftShiftAtEndStr+PositioinCur]=' ';
      m_left_activecursor_buf=0;
      if(ChTime[0] != '\0' && ChTime[0] != '\n')
      {
        if(m_top_activecursor_buf>0)
          m_BufTextDisplay[(m_top_activecursor_buf)*(m_width_display+1)+
            m_left_activecursor_buf]=ChTime[0];
        else
          m_BufTextDisplay[m_left_activecursor_buf]=ChTime[0];
        m_left_activecursor_buf++;
      }
      for(mxdisp_pos_t G=0; G<m_height_display; G++)
      {
        m_display_drv_service.outtextpos(
        0,
        G,
        &(m_BufTextDisplay[(m_width_display+1)*G]));
      }
    }
    else                            //если текущая строка не последняя
    {
      m_top_activecursor_buf++;     //переходим на следующую строку
      m_left_activecursor_buf=0;    //переходим в начало строки
      if(ChTime[0] != '\0' && ChTime[0] != '\n')
      {
        if(m_top_activecursor_buf>0)
          m_BufTextDisplay[(m_top_activecursor_buf)*(m_width_display+1)+
            m_left_activecursor_buf]=ChTime[0];
        else
          m_BufTextDisplay[m_left_activecursor_buf]=ChTime[0];
          m_display_drv_service.outtextpos(
          m_left_activecursor_buf,
          m_top_activecursor_buf,
          ChTime);
        m_left_activecursor_buf++;
      }
    }
  }
  else if(ChTime[0] != '\0')
  {
    if(m_top_activecursor_buf>0)
      m_BufTextDisplay[(m_top_activecursor_buf)*(m_width_display+1)+
      m_left_activecursor_buf]=ChTime[0];
    else
      m_BufTextDisplay[m_left_activecursor_buf]=ChTime[0];
    m_display_drv_service.outtextpos(
      m_left_activecursor_buf,
      m_top_activecursor_buf,
      ChTime);
    m_left_activecursor_buf++;
  }
}
inline void irs_strm_buf::setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top)
{
  if(a_left > m_width_display-1)
    a_left=static_cast<mxdisp_pos_t>(m_width_display-1);
  if(a_top > m_height_display-1)
    a_top=static_cast<mxdisp_pos_t>(m_height_display-1);
  m_left_activecursor_buf=a_left;      //текущая координата X курсора в буфере
  m_top_activecursor_buf=a_top;        //текущая координата Y курсора в буфере
  m_display_drv_service.setpos(a_left, a_top);
}
inline void irs_strm_buf::stop_get()
{
  m_cursor_blink=irs_false;             //прекращаем мигать курсором
  stop_blink_cursor();                  //перестать мигать курсором
  m_flag_check_get=irs_false;
  mp_TemporaryInBuf=mp_InBuf;
  mp_InBuf=new char[++m_LengthInBuf];
  for(irs_u32 i=0; i < m_LengthInBuf-1; i++)
    mp_InBuf[i]=mp_TemporaryInBuf[i];
  mp_InBuf[m_LengthInBuf-1]='\0';
  if(m_LengthInBuf > 1)
    delete []mp_TemporaryInBuf;
  m_flag_processing_key_event=irs_false;    //остановить запись входного буфера
  setg(mp_InBuf,mp_InBuf,mp_InBuf+m_LengthInBuf);//устанавлиаем ссылку на буфер
}

// класс irs_ostream
class irs_iostream: public istream, public ostream
{
  irs_strm_buf m_strm_buf;
  mxkey_event_t* mp_mxkey_event;
public:
  inline irs_iostream();
  inline mxdisp_pos_t get_height();
  inline mxdisp_pos_t get_width();
  inline void connect(mxdisplay_drv_t *ap_driver);
  inline void connect(mxkey_event_t* ap_mxkey_event);
  inline void clear_display();
  inline void clear_line(mxdisp_pos_t a_top);
  inline ostream& setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top);
  inline void tick();
  inline void start_get();
  inline irs_bool check_get();
  inline void stop_get();
};
inline irs_iostream::irs_iostream():
  istream(&m_strm_buf),
  ostream(&m_strm_buf)
{
  ostream::init(&m_strm_buf);
  istream::init(&m_strm_buf);
}
inline void irs_iostream::tick()
{
  m_strm_buf.tick();
}
inline mxdisp_pos_t irs_iostream::get_height()
{
  return m_strm_buf.get_height();
}
inline mxdisp_pos_t irs_iostream::get_width()
{
  return m_strm_buf.get_width();
}
inline void irs_iostream::connect(mxdisplay_drv_t *ap_driver)
{
  m_strm_buf.connect(ap_driver);
}
inline void irs_iostream::connect(mxkey_event_t* ap_mxkey_event)
{
  mp_mxkey_event = ap_mxkey_event;
  m_strm_buf.connect(ap_mxkey_event);
}
inline void irs_iostream::clear_display()
{
  m_strm_buf.clear();
}
inline void irs_iostream::clear_line(mxdisp_pos_t a_top)
{
  m_strm_buf.clear_line(a_top);
}
inline ostream& irs_iostream::setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top)
{
  m_strm_buf.setpos(a_left, a_top);
  return *this;
}
inline void irs_iostream::start_get()
{
  m_strm_buf.start_get();
}
inline irs_bool irs_iostream::check_get()
{
  irs_bool check_get = m_strm_buf.check_get();
  if (check_get) m_strm_buf.stop_get();
  return check_get;
}
inline void irs_iostream::stop_get()
{
  m_strm_buf.stop_get();
}

//! @}

namespace irs {

#ifdef __BORLANDC__

// Буфер стандартных потоков для Memo
template <class char_type>
class basic_memobuf: public basic_streambuf<char_type>
{
public:
  typedef typename basic_streambuf<char_type>::int_type int_type;
  typedef typename basic_streambuf<char_type>::traits_type traits_type;

  basic_memobuf(const basic_memobuf& a_buf);
  basic_memobuf(TMemo *ap_memo = 0, size_t a_outbuf_size = 0);
  void connect(TMemo *ap_memo);
  virtual int_type overflow(int_type a_char = traits_type::eof());
  virtual int sync();
private:
  enum { zero_term_size = 1 };

  raw_data_t<char_type> m_outbuf;
  TMemo *mp_memo;
  const char_type m_end_of_line_char;
  const char_type m_zero_term_char;
};
template <class char_type>
basic_memobuf<char_type>::basic_memobuf(
  const basic_memobuf<char_type>& a_buf
):
  m_outbuf(a_buf.m_outbuf),
  mp_memo(a_buf.mp_memo),
  m_end_of_line_char(a_buf.m_end_of_line_char),
  m_zero_term_char(a_buf.m_zero_term_char)
{
  setp(m_outbuf.begin(), m_outbuf.end() - zero_term_size);
}
template <class char_type>
basic_memobuf<char_type>::basic_memobuf(TMemo *ap_memo, size_t a_outbuf_size):
  m_outbuf(a_outbuf_size + zero_term_size),
  mp_memo(ap_memo),
  m_end_of_line_char(static_cast<char_type>('\n')),
  m_zero_term_char(static_cast<char_type>('\0'))
{
  setp(m_outbuf.begin(), m_outbuf.end() - zero_term_size);
}
template <class char_type>
void basic_memobuf<char_type>::connect(TMemo *ap_memo)
{
  mp_memo = ap_memo;
}
template <class char_type>
basic_memobuf<char_type>::int_type
  basic_memobuf<char_type>::overflow(int_type a_char)
{
  if (!mp_memo) return 0;

  int msg_size = pptr() - pbase();
  TStrings* Console = mp_memo->Lines;
  if (msg_size > 0) {
    *pptr() = m_zero_term_char;

    char_type* message = pbase();
    char_type* message_line = message;
    for (int msg_char_idx = 0; msg_char_idx < msg_size; msg_char_idx++) {
      if (message[msg_char_idx] == m_end_of_line_char) {
        message[msg_char_idx] = m_zero_term_char;
        if (Console->Count <= 0) {
          Console->Add("");
        }
        String ConsoleLine = Console->Strings[Console->Count - 1];
        ConsoleLine += String(message_line);
        Console->Strings[Console->Count - 1] = ConsoleLine;
        Console->Add("");
        message_line = &message[msg_char_idx + 1];
      }
    }
    if (message[msg_size - 1] != m_end_of_line_char) {
      if (Console->Count <= 0) {
        Console->Add("");
      }
      String ConsoleLine = Console->Strings[Console->Count - 1];
      ConsoleLine += String(message_line);
      Console->Strings[Console->Count - 1] = ConsoleLine;
    }
  }
  if (a_char != traits_type::eof())
  {
    if (a_char != m_end_of_line_char) {
      if (Console->Count <= 0) Console->Add("");
      String ConsoleLine = Console->Strings[Console->Count - 1];
      enum { char_str_size = 2 };
      char_type char_str[char_str_size];
      enum { char_pos = 0, zero_char_pos = 1 };
      char_str[char_pos] = traits_type::to_char_type(a_char);
      char_str[zero_char_pos] = 0;
      ConsoleLine += char_str;
      Console->Strings[Console->Count - 1] = ConsoleLine;
    } else {
      Console->Add("");
    }
  }
  setp(m_outbuf.begin(), m_outbuf.end() - zero_term_size);
  return 0;
}
template <class char_type>
int basic_memobuf<char_type>::sync()
{
  return overflow();
}

typedef basic_memobuf<char_t> memobuf_t;
#ifdef IRSSTRM_NEW_MEMOBUF
typedef basic_memobuf<char> memobuf;
#endif //IRSSTRM_NEW_MEMOBUF

#endif //__BORLANDC__

#ifdef QT_CORE_LIB
// Буфер стандартных потоков для QPlainTextEdit
template <class char_type>
class basic_plain_text_edit_buf_t: public basic_streambuf<char_type>
{
public:
  typedef std::size_t size_type;
  typedef typename basic_streambuf<char_type>::int_type int_type;
  typedef typename basic_streambuf<char_type>::traits_type traits_type;


  basic_plain_text_edit_buf_t(const basic_plain_text_edit_buf_t& a_buf);
  basic_plain_text_edit_buf_t(QPlainTextEdit *ap_edit = 0,
    size_type a_outbuf_size = 0);
  void connect(QPlainTextEdit *ap_edit);
  virtual int_type overflow(int_type a_char = traits_type::eof());
  virtual int sync();
private:
  enum { zero_term_size = 1 };

  raw_data_t<char_type> m_outbuf;
  QPlainTextEdit *mp_edit;
  const char_type m_end_of_line_char;
  const char_type m_zero_term_char;
};
template <class char_type>
basic_plain_text_edit_buf_t<char_type>::basic_plain_text_edit_buf_t(
  const basic_plain_text_edit_buf_t<char_type>& a_buf
):
  m_outbuf(a_buf.m_outbuf),
  mp_edit(a_buf.mp_edit),
  m_end_of_line_char(a_buf.m_end_of_line_char),
  m_zero_term_char(a_buf.m_zero_term_char)
{
  setp(m_outbuf.begin(), m_outbuf.end() - zero_term_size);
}
template <class char_type>
basic_plain_text_edit_buf_t<char_type>::basic_plain_text_edit_buf_t(
  QPlainTextEdit *ap_edit, size_t a_outbuf_size
):
  m_outbuf(a_outbuf_size + zero_term_size),
  mp_edit(ap_edit),
  m_end_of_line_char(static_cast<char_type>('\n')),
  m_zero_term_char(static_cast<char_type>('\0'))
{
  setp(m_outbuf.begin(), m_outbuf.end() - zero_term_size);
}
template <class char_type>
void basic_plain_text_edit_buf_t<char_type>::connect(QPlainTextEdit *ap_edit)
{
  mp_edit = ap_edit;
}
template <class char_type>
typename basic_plain_text_edit_buf_t<char_type>::int_type
basic_plain_text_edit_buf_t<char_type>::overflow(int_type a_char)
{
  if (!mp_edit) {
    return 0;
  }
  const int msg_size = this->pptr() - this->pbase();
  QString s;
  if (msg_size > 0) {
    *this->pptr() = m_zero_term_char;
    std::basic_string<char_type> message(this->pbase());
    s = irs::str_conv<QString>(message);
  }
  if (a_char != traits_type::eof()) {
    enum { char_str_size = 2 };
    char_type char_str[char_str_size];
    enum { char_pos = 0, zero_char_pos = 1 };
    char_str[char_pos] = traits_type::to_char_type(a_char);
    char_str[zero_char_pos] = 0;
    s = irs::str_conv<QString>(std::string(char_str));
  }
  if (!s.isEmpty()) {
    QTextCursor c = mp_edit->textCursor();
    c.movePosition(QTextCursor::End);
    mp_edit->setTextCursor(c);
    mp_edit->insertPlainText(s);
    c.movePosition(QTextCursor::End);
    mp_edit->setTextCursor(c);
  }

  setp(m_outbuf.begin(), m_outbuf.end() - zero_term_size);
  return 0;
}
template <class char_type>
int basic_plain_text_edit_buf_t<char_type>::sync()
{
  return overflow();
}

typedef basic_plain_text_edit_buf_t<char_t> plain_text_edit_buf_t;
typedef basic_plain_text_edit_buf_t<char> plain_text_edit_buf;
#endif // QT_CORE_LIB

// Стандартная конфигурация консоли
class conio_cfg_t: public mx_proc_t
{
public:
  inline conio_cfg_t(mxkey_drv_t& a_key_drv, mxdisplay_drv_t& a_display_drv);
  inline virtual irs_bool tick();
  inline virtual void abort();
  inline irs_iostream &stream();
  inline mxkey_event_t &key_event();
private:
  mxkey_drv_t &m_key_drv;
  mxkey_event_gen_t m_key_event_gen;
  mxkey_event_t m_strm_key_event;
  mxkey_event_t m_app_key_event;
  mxdisplay_drv_t &m_display_drv;
  irs_iostream m_strm;
  irs_bool m_abort_request;
};
inline conio_cfg_t::conio_cfg_t(
  mxkey_drv_t& a_key_drv,
  mxdisplay_drv_t& a_display_drv
):
  m_key_drv(a_key_drv),
  m_key_event_gen(),
  m_strm_key_event(),
  m_app_key_event(),
  m_display_drv(a_display_drv),
  m_strm(),
  m_abort_request(irs_false)
{
  m_key_event_gen.connect(&m_key_drv);
  m_key_event_gen.add_event(&m_strm_key_event);
  m_key_event_gen.add_event(&m_app_key_event);
  m_strm.connect(&m_display_drv);
  m_strm.connect(&m_strm_key_event);
}
inline irs_bool conio_cfg_t::tick()
{
  m_key_event_gen.tick();
  m_display_drv.tick();
  m_strm.tick();
  return !m_abort_request;
}
inline void conio_cfg_t::abort()
{
  m_abort_request = irs_false;
}
inline irs_iostream& conio_cfg_t::stream()
{
  return m_strm;
}
inline mxkey_event_t& conio_cfg_t::key_event()
{
  return m_app_key_event;
}

#ifdef NOP
// Специализированный тип потока вывода
inline void copyformat(ostream &dest, const ostream &src)
{
  dest.setf(src.flags());
  dest.precision(src.precision());
  dest.width(src.width());
  dest.fill(src.fill());
  dest.exceptions(src.exceptions());
  dest.clear(src.rdstate());
}
class mxostream: public ostream
{
  char m_def_decimal_point;
  char m_new_decimal_point;

  template <class T>
  void floatout(T a_val)
  {
    ostrstream sstrm;
    copyformat(sstrm, *this);
    sstrm << a_val << "\0";
    char *sbeg = sstrm.str();
    char *send = sbeg + sstrm.pcount();
    replace(sbeg, send, m_def_decimal_point, m_new_decimal_point);
    *this << sbeg;
    sstrm.freeze(false);
  }
public:
  mxostream(streambuf *ap_rdbuf, char a_def_decimal_point = '.',
    char a_new_decimal_point = ','):
    ostream(ap_rdbuf),
    m_def_decimal_point(a_def_decimal_point),
    m_new_decimal_point(a_new_decimal_point)
  {
    init(ap_rdbuf);
  }
  void set_decimal_point(char a_new_decimal_point)
  {
    m_new_decimal_point = a_new_decimal_point;
  }
  mxostream &operator<<(const char *a_strg)
  {
    ostream &strm = *this;
    strm << a_strg;
    return *this;
  }
  mxostream &operator<<(const irs::string &a_strg)
  {
    ostream &strm = *this;
    strm << a_strg.c_str();
    return *this;
  }
  mxostream &operator<<(const char& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const signed char& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const unsigned char& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const signed short& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const unsigned short& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const signed int& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const unsigned int& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const signed long& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const unsigned long& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const irs_i64& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const irs_u64& val)
  {
    ostream &strm = *this;
    strm << val;
    return *this;
  }
  mxostream &operator<<(const float &a_val)
  {
    //ostream &strm = *this;
    floatout(a_val);
    return *this;
  }
  mxostream &operator<<(const double &a_val)
  {
    //ostream &strm = *this;
    floatout(a_val);
    return *this;
  }
  mxostream &operator<<(const long double &a_val)
  {
    //ostream &strm = *this;
    floatout(a_val);
    return *this;
  }
  mxostream& operator<<(mxostream& (*f)(mxostream&))
  {
    return f(*this);
  }
};
inline mxostream &endl(mxostream &strm)
{
  strm << "\n";
  strm.flush();
  return strm;
}
inline mxostream &ends(mxostream &strm)
{
  strm.put('\0');
  strm.flush();
  return strm;
}
inline mxostream &flush(mxostream &strm)
{
  strm.flush();
  return strm;
}
inline mxostream &oct(mxostream &strm)
{
  strm.setf(ios::oct, ios::basefield);
  return strm;
}
inline mxostream &dec(mxostream &strm)
{
  strm.setf(ios::dec, ios::basefield);
  return strm;
}
inline mxostream &hex(mxostream &strm)
{
  strm.setf(ios::hex, ios::basefield);
  return strm;
}
inline mxostream &general(mxostream &strm)
{
  strm.setf(static_cast<ios::fmtflags>(0), ios::floatfield);
  return strm;
}
inline mxostream &fixed(mxostream &strm)
{
  strm.setf(ios::fixed, ios::floatfield);
  return strm;
}
inline mxostream &scientific(mxostream &strm)
{
  strm.setf(ios::scientific, ios::floatfield);
  return strm;
}
inline mxostream &left(mxostream &strm)
{
  strm.setf(ios::left, ios::adjustfield);
  return strm;
}
inline mxostream &right(mxostream &strm)
{
  strm.setf(ios::right, ios::adjustfield);
  return strm;
}
inline mxostream &internal(mxostream &strm)
{
  strm.setf(ios::internal, ios::adjustfield);
  return strm;
}
inline mxostream &showbase(mxostream &strm)
{
  strm.setf(ios::showbase);
  return strm;
}
inline mxostream &noshowbase(mxostream &strm)
{
  strm.unsetf(ios::showbase);
  return strm;
}
inline mxostream &showpos(mxostream &strm)
{
  strm.setf(ios::showpos);
  return strm;
}
inline mxostream &noshowpos(mxostream &strm)
{
  strm.unsetf(ios::showpos);
  return strm;
}
inline mxostream &skipws(mxostream &strm)
{
  strm.setf(ios::skipws);
  return strm;
}
inline mxostream &noskipws(mxostream &strm)
{
  strm.unsetf(ios::skipws);
  return strm;
}
inline mxostream &uppercase(mxostream &strm)
{
  strm.setf(ios::uppercase);
  return strm;
}
inline mxostream &nouppercase(mxostream &strm)
{
  strm.unsetf(ios::uppercase);
  return strm;
}
#endif //NOP

} //namespace irs

#endif //IRSSTRMSTDH
