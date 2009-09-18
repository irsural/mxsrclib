// Стандартаная библиотека ИРС локальная часть
// Для watcom
// Дата: 17.09.2009
// Дата ранее: 3.12.2007

#include <irsstdl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//---------------------------------------------------------------------------
// Преобразование числа double в строку в %g формате
irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer)
{
  return (irs_u8 *)gcvt(value, ndigits, (char *)buffer);
}
//---------------------------------------------------------------------------
//Класс драйвер для консоли Windows
irs_win32_console_display_t::irs_win32_console_display_t():
  m_Written(0),
  m_top_displey(0)
{
  m_Coord.X=0;   // координаты вывода(x,y)
  m_Coord.Y=0;   // координаты вывода(x,y)
  //просим десриптор стандартного ввода
  m_InpHandle=GetStdHandle(STD_INPUT_HANDLE);
  //просим дескрипртор стандартного вывода
  m_OutHandle=GetStdHandle(STD_OUTPUT_HANDLE);
  //считываем параметры буфера
  GetConsoleScreenBufferInfo(m_OutHandle, &m_BufInfo);
  //щас погасим курсор
  CONSOLE_CURSOR_INFO lpConsoleCursorInfo;
  GetConsoleCursorInfo(m_OutHandle,&lpConsoleCursorInfo);
  lpConsoleCursorInfo.bVisible=false;   //гасим курсор
  SetConsoleCursorInfo(m_OutHandle,&lpConsoleCursorInfo);
  m_textbuf=new char[get_width()];           
}

mxdisp_pos_t irs_win32_console_display_t::get_height()
{
  return static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Bottom+1);//.dwSize.Y;
}
mxdisp_pos_t irs_win32_console_display_t::get_width()
{
  return static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Right+1);//.dwSize.X;
}
void irs_win32_console_display_t::outtextpos(
  mxdisp_pos_t a_left,
  mxdisp_pos_t a_top,
  const char *text)
{
  if((a_left < m_BufInfo.srWindow.Right+1) &&
    (a_top < m_BufInfo.srWindow.Bottom+1))
  {
    irs_u32 width=get_width();
    m_Coord.X = static_cast<SHORT>(a_left);
    m_Coord.Y = static_cast<SHORT>(a_top+m_top_displey);
    irs_u32 lentext=min(strlen(text), static_cast<unsigned int>(width-a_left));
    WriteConsoleOutputCharacter(
      m_OutHandle,              // screen buffer handle
      text,                     // pointer to source string
      lentext,                  // length of string
      m_Coord,                  // first cell to write to
      &m_Written);              // actual number written
  }
}

void irs_win32_console_display_t::tick()
{
}
//---------------------------------------------------------------------------
// Класс драйвер для влавиатуры
irs_win32_console_key_drv_t::irs_win32_console_key_drv_t():
  m_flag_event_KeyDown(irs_false),
  m_temporary_irs_key(irskey_none),
  m_Written(0)
{ 
  //просим десриптор стандартного ввода
  m_InpHandle=GetStdHandle(STD_INPUT_HANDLE);
  m_fdwMode = ENABLE_WINDOW_INPUT;
  SetConsoleMode(m_InpHandle, m_fdwMode);
}
irskey_t irs_win32_console_key_drv_t::operator()()
{

  typedef struct _key_map_t {
    irs_i16 win_key;
    irskey_t irs_key;
  } key_map_t;
  key_map_t key_map[] = {
    {VK_RETURN,     irskey_enter},
    {VK_SEPARATOR,  irskey_enter},
    {VK_ESCAPE,     irskey_escape},
    {VK_UP,         irskey_up},
    {VK_DOWN,       irskey_down},
    {'0',           irskey_0},
    {'1',           irskey_1},
    {'2',           irskey_2},
    {'3',           irskey_3},
    {'4',           irskey_4},
    {'5',           irskey_5},
    {'6',           irskey_6},
    {'7',           irskey_7},
    {'8',           irskey_8},
    {'9',           irskey_9},
    {VK_NUMPAD0,    irskey_0},
    {VK_NUMPAD1,    irskey_1},
    {VK_NUMPAD2,    irskey_2},
    {VK_NUMPAD3,    irskey_3},
    {VK_NUMPAD4,    irskey_4},
    {VK_NUMPAD5,    irskey_5},
    {VK_NUMPAD6,    irskey_6},
    {VK_NUMPAD7,    irskey_7},
    {VK_NUMPAD8,    irskey_8},
    {VK_NUMPAD9,    irskey_9},
    {VK_DECIMAL,    irskey_point},
    {VK_BACK,       irskey_backspace}
  };
  const irs_u8 key_map_size = sizeof(key_map)/sizeof(key_map_t);

  GetNumberOfConsoleInputEvents(m_InpHandle,&m_cNumRead);
  if(m_cNumRead>0)
  {
    //irInBuf-buffer to read into
    //128-size of read buffer
    //cNumRead-number of records read
    ReadConsoleInput(m_InpHandle, m_irInBuf, m_len_buf, &m_cNumRead);
    for(irs_u32 i=0;i< m_cNumRead; i++)
      if(m_irInBuf[i].EventType==KEY_EVENT)    //если событие с клавиатуры
      {
        //если событие при нажатии клавиши и клавиша не была нажата
        if(m_irInBuf[i].Event.KeyEvent.bKeyDown==irs_true
          && m_flag_event_KeyDown==irs_false)
        {
          //смотрим, совпадает ли с одной из кнопок
          for (irs_u8 key_map_index = 0; key_map_index < key_map_size;
          key_map_index++)
          {
            //если совпадает, отправляем ее
            if(key_map[key_map_index].win_key==
              m_irInBuf[i].Event.KeyEvent.wVirtualKeyCode)  //виртуальный код
            {
              m_flag_event_KeyDown=irs_true;
              m_temporary_irs_key=key_map[key_map_index].irs_key;
              return m_temporary_irs_key;
            }
          }
        }
        //если произошло отпускание кнопки, и клавиша была нажата
        else  if(m_irInBuf[i].Event.KeyEvent.bKeyDown==irs_false &&
          m_flag_event_KeyDown==irs_true)
        {
          m_flag_event_KeyDown=irs_false;
          m_temporary_irs_key=irskey_none;
          return m_temporary_irs_key;
        }
        else  if(m_flag_event_KeyDown==irs_true)  //если все еще клавиша нажата
        {
          return m_temporary_irs_key;
        }             
      }
  }
  else  if(m_flag_event_KeyDown==irs_true)        //если все еще клавиша нажата
  {
    return m_temporary_irs_key;
  }

  m_temporary_irs_key=irskey_none;
  return m_temporary_irs_key;
}
//---------------------------------------------------------------------------
// Конфигурация консоли для Watcom C++
irs::conio_cfg_t& irs::arch_conio_cfg::def()
{
  static irs_win32_console_key_drv_t key_drv;
  static irs_win32_console_display_t display;
  static conio_cfg_t conio_cfg_i(key_drv, display);
  return conio_cfg_i;
}
