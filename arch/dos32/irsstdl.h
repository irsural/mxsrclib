// Стандартаная библиотека ИРС локальная часть
// Для watcom
// Дата 6.11.2007

#ifndef irsstdlH
#define irsstdlH

#include <windows.h>
#include <irsdefs.h>
#include <irsstdg.h>

// Преобразование числа double в строку в %g формате
irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer);

//Класс драйвер для консоли Windows
class irs_win32_console_display_t: public mxdisplay_drv_t
{
private:
  DWORD m_Written;
  HANDLE m_InpHandle, m_OutHandle;        //дескрипторы окон
  COORD m_Coord;                        // координаты вывода(x,y)
  //CHAR chFillChar;
  //WORD wColors[3], wColor;          //цвет ячеек, и текста
  //переменная для хранения  параметров буфера;
  CONSOLE_SCREEN_BUFFER_INFO m_BufInfo;
  //так как текст, поступающий в буфер неЛьзя изменить
  //(например для обрезки нуль-символом), приходится ввести свой буфер
  char* m_textbuf;
  int m_top_displey; //координата Y текущего положения видимой области на экране
public:
  irs_win32_console_display_t(); //конструктор без параметров
  virtual mxdisp_pos_t get_height();
  virtual mxdisp_pos_t get_width();
  virtual void outtextpos(
    mxdisp_pos_t a_left,
    mxdisp_pos_t a_top,
    const char *text);
  virtual void tick();
};

//Класс драйвер для влавиатуры
class irs_win32_console_key_drv_t: public mxkey_drv_t
{
private:
  irs_bool m_flag_event_KeyDown;
  irskey_t m_temporary_irs_key;              //храним код нажатой клавиши
  DWORD m_Written, m_fdwMode, m_cNumRead;
  HANDLE  m_InpHandle;                  //дескриптор окна
  static const irs_u32 m_len_buf = 128;
  INPUT_RECORD m_irInBuf[m_len_buf];
public:
  irs_win32_console_key_drv_t();
  virtual irskey_t operator()();
};

#endif //irsstdlH
