// Стандартаная библиотека ИРС локальная часть
// Для Borland C++ Builder
// Дата: 14.04.2010
// Ранняя дата: 2.09.2009

#include <irsdefs.h>

#include <vcl.h>
#pragma hdrstop

#include <stdlib.h>
#include <string.h>

#include <irsstdl.h>

#include <irsfinal.h>
//---------------------------------------------------------------------------
// Вспомогательный класс драйвера клавиатуры С++ Builder
class mxkdb_keystate_t: TObject
{
  Word f_key;
  void __fastcall KeyDown(System::TObject* Sender, Word &Key,
    Classes::TShiftState Shift);
  void __fastcall KeyUp(System::TObject* Sender, Word &Key,
    Classes::TShiftState Shift);
public:
  mxkdb_keystate_t();
  virtual __fastcall ~mxkdb_keystate_t();
  void connect(TMemo *input);
  Word operator()();
};
#ifdef NOP
//---------------------------------------------------------------------------
// Преобразование числа double в строку в %g формате
irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer)
{
  return (irs_u8 *)gcvt(value, ndigits, (char *)buffer);
}
#endif //NOP
//---------------------------------------------------------------------------
// Класс драйвера консоли для объекта TStrings C++ Builder

mxbuilder_condrv_t::mxbuilder_condrv_t()
{
}
mxbuilder_condrv_t::mxbuilder_condrv_t(TStrings *Console):
  f_console(Console)
{
}
void mxbuilder_condrv_t::out(const char *str)
{
  int len_s = strlen((char *)str);
  //char *message = (char *)malloc(len_s + 1);
  char *message = new char[len_s + 1];
  if (!message) return;
  ::strcpy(message, (char *)str);
  char *message_line = message;
  //TStrings *Console = MainForm->Memo1->Lines;
  for (int i_msg_ch = 0; i_msg_ch < len_s; i_msg_ch++) {
    if ('\n' == message[i_msg_ch]) {
      message[i_msg_ch] = 0;
      if (f_console->Count <= 0) f_console->Add("");
      String ConsoleLine = f_console->Strings[f_console->Count - 1];
      ConsoleLine += String(message_line);
      f_console->Strings[f_console->Count - 1] = ConsoleLine;
      f_console->Add("");
      message_line = &message[i_msg_ch + 1];
    }
  }
  if (len_s > 0)
  if (message[len_s - 1] != '\n') {
    if (f_console->Count <= 0) f_console->Add("");
    String ConsoleLine = f_console->Strings[f_console->Count - 1];
    ConsoleLine += String(message_line);
    f_console->Strings[f_console->Count - 1] = ConsoleLine;
  }
  delete []message;
}
//---------------------------------------------------------------------------
// Класс для автоподключения mainprg к приложению на C++ Builder

__fastcall mpc_builder_t::mpc_builder_t():
  m_timer(IRS_NULL),
  m_SavedCloseEvent(IRS_NULL),
  m_form(IRS_NULL),
  m_app_proc(IRS_NULL),
  m_mxapplication(IRS_NULL)
{
}
__fastcall mpc_builder_t::~mpc_builder_t()
{
  if (m_timer) delete m_timer;
  m_timer = IRS_NULL;
  if (m_mxapplication) delete m_mxapplication;
  m_mxapplication = IRS_NULL;
}
void mpc_builder_t::TimerInit(TNotifyEvent Event)
{
  m_timer = new TTimer(IRS_NULL);
  m_timer->OnTimer = Event;
  m_timer->Interval = 10;
  m_timer->Enabled = true;
}

void mpc_builder_t::Connect(TForm *Form, void (*a_init)(), void (*a_deinit)(),
  void (*a_tick)(), void (*a_stop)(), irs_bool (*a_stopped)())
{
  m_form = Form;
  m_SavedCloseEvent = m_form->OnClose;
  m_form->OnClose = MainFormClose;

  m_mxapplication = new mxapplication_t();
  m_mxapplication->set_init_event(a_init);
  m_mxapplication->set_denit_event(a_deinit);
  m_mxapplication->set_tick_event(a_tick);
  m_mxapplication->set_stop_event(a_stop);
  m_mxapplication->set_stopped_event(a_stopped);
  m_mxapplication->run(irs_true);

  TimerInit(TimerEvent);
}
void __fastcall mpc_builder_t::MainFormClose(System::TObject* Sender,
  TCloseAction &Action)
{
  if (m_SavedCloseEvent) m_SavedCloseEvent(Sender, Action);
  while (m_mxapplication->run(irs_false));
}
void __fastcall mpc_builder_t::TimerEvent(System::TObject* Sender)
{
  if (!m_mxapplication->run(irs_true)) {
    m_timer->Enabled = false;
    m_form->Close();
    return;
  }
}

void mpc_builder_t::Connect(TForm *a_Form, mx_proc_t *a_app_proc)
{
  m_form = a_Form;
  m_SavedCloseEvent = m_form->OnClose;
  m_form->OnClose = ProcClose;

  m_app_proc = a_app_proc;

  TimerInit(ProcTimer);
}
void __fastcall mpc_builder_t::ProcClose(System::TObject* Sender,
  TCloseAction &Action)
{
  if (m_SavedCloseEvent) m_SavedCloseEvent(Sender, Action);
  m_app_proc->abort();
  while (m_app_proc->tick());
}
void __fastcall mpc_builder_t::ProcTimer(System::TObject* Sender)
{
  if (!m_app_proc->tick()) {
    m_timer->Enabled = false;
    m_form->Close();
    return;
  }
}
//---------------------------------------------------------------------------
// Класс драйвера клавиатуры С++ Builder

mxkey_drv_builder_t::mxkey_drv_builder_t():
  keystate(IRS_NULL)
{
  keystate = (void *)new mxkdb_keystate_t();
}
mxkey_drv_builder_t::~mxkey_drv_builder_t()
{
  delete (mxkdb_keystate_t *)keystate;
}
void mxkey_drv_builder_t::connect(TMemo *a_display)
{
  ((mxkdb_keystate_t *)keystate)->connect(a_display);
}
irskey_t mxkey_drv_builder_t::operator()()
{
  typedef struct _key_map_t {
    Word win_key;
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

  for (irs_u8 key_map_index = 0; key_map_index < key_map_size;
    key_map_index++) {
    if (key_map[key_map_index].win_key == (*(mxkdb_keystate_t *)keystate)())
    {
      return key_map[key_map_index].irs_key;
    }
  }
  return irskey_none;
}
//---------------------------------------------------------------------------
// Вспомогательный класс драйвера клавиатуры С++ Builder

mxkdb_keystate_t::mxkdb_keystate_t():
  f_key(0)
{
}
__fastcall mxkdb_keystate_t::~mxkdb_keystate_t()
{
}
Word mxkdb_keystate_t::operator()()
{
  return f_key;
}
void __fastcall mxkdb_keystate_t::KeyDown(System::TObject* Sender, Word &Key,
  Classes::TShiftState Shift)
{
  f_key = Key;
}
void __fastcall mxkdb_keystate_t::KeyUp(System::TObject* Sender, Word &Key,
  Classes::TShiftState Shift)
{
  f_key = 0;
}
void mxkdb_keystate_t::connect(TMemo *input)
{
  input->OnKeyDown = KeyDown;
  input->OnKeyUp = KeyUp;
}
//---------------------------------------------------------------------------
// Класс драйвера дисплея С++ Builder

// Время интервала повтора по умолчанию
#define mxdisplay_refresh_time_def TIME_TO_CNT(1, 20)

mxdisplay_drv_builder_t::mxdisplay_drv_builder_t(mxdisp_pos_t a_height,
  mxdisp_pos_t a_width):
  mp_display(IRS_NULL),
  mp_console(IRS_NULL),
  m_height(a_height),
  m_width(a_width),
  m_length(mxdisp_pos_t(m_height*m_width)),
  mp_display_ram(new char[m_length]),
  m_display_ram_changed(irs_true),
  m_refresh_time(mxdisplay_refresh_time_def),
  m_refresh_to(0)
{
  //lcd_clear();
  init_to_cnt();
  set_to_cnt(m_refresh_to, m_refresh_time);
}
mxdisplay_drv_builder_t::~mxdisplay_drv_builder_t()
{
  deinit_to_cnt();
  delete []mp_display_ram;
}
mxdisp_pos_t mxdisplay_drv_builder_t::get_height()
{
  return m_height;
}
mxdisp_pos_t mxdisplay_drv_builder_t::get_width()
{
  return m_width;
}
void mxdisplay_drv_builder_t::outtextpos(
  mxdisp_pos_t a_left, mxdisp_pos_t a_top, const char *ap_text)
{
  if ((a_left < m_width) && (a_top < m_height)) {
    m_display_ram_changed = irs_true;
    mxdisp_pos_t text_len = (mxdisp_pos_t)strlen(ap_text);
    if (a_left + text_len > m_width) text_len = mxdisp_pos_t(m_width - a_left);
    memcpy((void *)(mp_display_ram + a_top*m_width + a_left),
      (void *)ap_text, text_len);
  }
}
void mxdisplay_drv_builder_t::tick()
{
  if (test_to_cnt(m_refresh_to)) {
    set_to_cnt(m_refresh_to, m_refresh_time);
    if (m_display_ram_changed)
    if (mp_console) {
      m_display_ram_changed = irs_false;
      mp_console->BeginUpdate();
      mp_console->Clear();
      const irs_u32 add_chr_cnt = 2;
      const irs_u32 line_cur_size = m_width + add_chr_cnt + 1;
      char *line_cur = new char [line_cur_size];
      memset((void *)line_cur, '-', line_cur_size - 1);
      line_cur[line_cur_size - 1] = 0;
      mp_console->Add(line_cur);
      for (mxdisp_pos_t line_ind = 0; line_ind < m_height; line_ind++) {
        line_cur[0] = '|';
        memcpy((void *)&line_cur[1], mp_display_ram + line_ind*m_width,
          m_width);
        line_cur[line_cur_size - 2] = '|';
        line_cur[line_cur_size - 1] = 0;
        mp_console->Add(line_cur);
      }
      memset((void *)line_cur, '-', line_cur_size - 1);
      line_cur[line_cur_size - 1] = 0;
      mp_console->Add(line_cur);
      mp_console->EndUpdate();
      delete []line_cur;
    }
  }
}
void mxdisplay_drv_builder_t::connect(TMemo *ap_display)
{
  mp_display = ap_display;
  mp_console = mp_display->Lines;
}
void mxdisplay_drv_builder_t::set_refresh_time(counter_t a_refresh_time)
{
  m_refresh_time = a_refresh_time;
}
//--------------------------------------------------------------------
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
  //m_textbuf=newx char[get_width()];
  //if (SetConsoleCP(1252));
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
  if((a_left < static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Right+1)) &&
    (a_top < static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Bottom+1)))
  {
    irs_u32 width=get_width();
	  m_Coord.X=static_cast<short>(a_left);
	  m_Coord.Y=static_cast<short>(a_top+m_top_displey);
    irs_u32 lentext=min(strlen(text), static_cast<size_t>(width-a_left));
    irs::auto_arr<wchar_t> unicode_text(new wchar_t[lentext+1]);
    MultiByteToWideChar(
      1251/*UINT CodePage*/,
      MB_PRECOMPOSED /*DWORD dwFlags*/,
      text/*LPCSTR lpMultiByteStr*/,
      lentext/*int cbMultiByte*/,
      unicode_text.get()/*LPWSTR lpWideCharStr*/,
      lentext/*int cchWideChar*/);
    WriteConsoleOutputCharacterW(
      m_OutHandle,              // screen buffer handle
      unicode_text.get(),        // pointer to source string
      lentext,                  // length of string
      m_Coord,                  // first cell to write to
      &m_Written);              // actual number written
  }
}
void irs_win32_console_display_t::tick()
{
}
//---------------------------------------------------------------------------
//Класс драйвер для влавиатуры

irs_win32_console_key_drv_t::irs_win32_console_key_drv_t():
  m_flag_event_KeyDown(false),
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
    ReadConsoleInput(m_InpHandle, m_irInBuf, m_len_buf, &m_cNumRead);
    for(irs_u32 i=0;i< m_cNumRead; i++)
      if(m_irInBuf[i].EventType==KEY_EVENT)    //если событие с клавиатуры
      {
        //если событие при нажатии клавиши и клавиша не была нажата
        if(m_irInBuf[i].Event.KeyEvent.bKeyDown==true
          && m_flag_event_KeyDown==false)
        {
          //смотрим, совпадает ли с одной из кнопок
          for (irs_u8 key_map_index = 0; key_map_index < key_map_size;
          key_map_index++)
          {
            //если совпадает, отправляем ее
            if(key_map[key_map_index].win_key==
              m_irInBuf[i].Event.KeyEvent.wVirtualKeyCode)  //виртуальный код
            {
              m_flag_event_KeyDown=true;
              m_temporary_irs_key=key_map[key_map_index].irs_key;
              return m_temporary_irs_key;
            }
          }
        }
        //если произошло отпускание кнопки, и клавиша была нажата
        else  if(m_irInBuf[i].Event.KeyEvent.bKeyDown==false &&
          m_flag_event_KeyDown==true)
        {
          m_flag_event_KeyDown=false;
          m_temporary_irs_key=irskey_none;
          return m_temporary_irs_key;
        }
        else  if(m_flag_event_KeyDown==true)  //если все еще клавиша нажата
        {
          return m_temporary_irs_key;
        }             
      }
  }
  else  if(m_flag_event_KeyDown==true)        //если все еще клавиша нажата
  {
    return m_temporary_irs_key;
  }

  m_temporary_irs_key=irskey_none;
  return m_temporary_irs_key;
}
//---------------------------------------------------------------------------
// Конфигурация консоли для C++ Builder
irs::conio_cfg_t& irs::arch_conio_cfg::def()
{
  static irs_win32_console_key_drv_t key_drv;
  static irs_win32_console_display_t display;
  static conio_cfg_t conio_cfg_i(key_drv, display);
  return conio_cfg_i;
}
// Конфигурация консоли для Memo C++ Builder
irs::conio_cfg_t& irs::arch_conio_cfg::memo(TMemo *ap_memo,
  mxdisp_pos_t a_height, mxdisp_pos_t a_width)
{
  static mxkey_drv_builder_t key_drv;
  static mxdisplay_drv_builder_t display(a_height, a_width);
  static conio_cfg_t conio_cfg_i(key_drv, display);
  key_drv.connect(ap_memo);
  display.connect(ap_memo);
  return conio_cfg_i;
}

//---------------------------------------------------------------------------
// Буфер стандартных потоков для Memo
irs::memobuf::memobuf(const memobuf& a_buf):
  m_outbuf_size(a_buf.m_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  mp_memo(a_buf.mp_memo)
{
  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
}
irs::memobuf::memobuf(TMemo *ap_memo, int a_outbuf_size):
  m_outbuf_size(a_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  mp_memo(ap_memo)
{
  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
}
irs::memobuf::~memobuf()
{
}
void irs::memobuf::connect(TMemo *ap_memo)
{
  mp_memo = ap_memo;
}
int irs::memobuf::overflow(int c)
{
  if (!mp_memo) return 0;
  
  int len_s = pptr() - pbase();
  TStrings *Console = mp_memo->Lines;
  if (len_s > 0) {
    *pptr() = 0;

    char *message = pbase();
    char *message_line = message;
    for (int i_msg_ch = 0; i_msg_ch < len_s; i_msg_ch++) {
      if ('\n' == message[i_msg_ch]) {
        message[i_msg_ch] = 0;
        if (Console->Count <= 0) Console->Add("");
        String ConsoleLine = Console->Strings[Console->Count - 1];
        ConsoleLine += String(message_line);
        Console->Strings[Console->Count - 1] = ConsoleLine;
        Console->Add("");
        message_line = &message[i_msg_ch + 1];
      }
    }
    if (message[len_s - 1] != '\n') {
      if (Console->Count <= 0) Console->Add("");
      String ConsoleLine = Console->Strings[Console->Count - 1];
      ConsoleLine += String(message_line);
      Console->Strings[Console->Count - 1] = ConsoleLine;
    }
  }
  if (c != EOF)
  {
    if (c != '\n') {
      if (Console->Count <= 0) Console->Add("");
      String ConsoleLine = Console->Strings[Console->Count - 1];
      ConsoleLine += String((char)c);
      Console->Strings[Console->Count - 1] = ConsoleLine;
    } else {
      Console->Add("");
    }
  }
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
  return 0;
}
int irs::memobuf::sync()
{
  return overflow();
}

