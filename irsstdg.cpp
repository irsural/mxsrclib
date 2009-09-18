// Стандартаная библиотека ИРС общая часть
// Дата: 9.09.2009

#include <irsstdg.h>
#include <ctype.h>
#include <irserror.h>

//---------------------------------------------------------------------------
// Класс для приложения

mxapplication_t::mxapplication_t():
  first_exec(irs_true),
  stop_exec(irs_false),
  f_init(IRS_NULL),
  f_deinit(IRS_NULL),
  f_tick(IRS_NULL),
  f_stop(IRS_NULL),
  f_stopped(IRS_NULL)
{
}
mxapplication_t::~mxapplication_t()
{
}
irs_bool mxapplication_t::run(irs_bool exec)
{
  if (exec)
  if (first_exec) {
    first_exec = irs_false;
    init();
  }
  if (!first_exec) {
    if (!exec && !stop_exec) {
      stop_exec = irs_true;
      stop();
    }
    tick();
  }
  if (stopped() && !first_exec) {
    deinit();
    first_exec = irs_true;
    stop_exec = irs_false;
    return irs_false;
  } else {
    return irs_true;
  }
}
void mxapplication_t::set_init_event(void (*a_init)())
{
  f_init = a_init;
}
void mxapplication_t::set_denit_event(void (*a_deinit)())
{
  f_deinit = a_deinit;
}
void mxapplication_t::set_tick_event(void (*a_tick)())
{
  f_tick = a_tick;
}
void mxapplication_t::set_stop_event(void (*a_stop)())
{
  f_stop = a_stop;
}
void mxapplication_t::set_stopped_event(irs_bool (*a_stopped)())
{
  f_stopped = a_stopped;
}
void mxapplication_t::init()
{
  if (f_init) f_init();
}
void mxapplication_t::deinit()
{
  if (f_deinit) f_deinit();
}
void mxapplication_t::tick()
{
  if (f_tick) f_tick();
}
void mxapplication_t::stop()
{
  if (f_stop) f_stop();
}
irs_bool mxapplication_t::stopped()
{
  if (f_stopped) return f_stopped();
  else return irs_true;
}
//---------------------------------------------------------------------------
// Перераспределение памяти с сохранением данных
void *irs_renew(void *pointer, size_t old_size, size_t new_size)
{
  if (old_size == new_size) return pointer;

  void *new_pointer = IRS_NULL;
  if (new_size) {
    new_pointer = (void*)new irs_u8 *[new_size];
    if (!new_pointer) return new_pointer;
  }
  if (pointer) {
    if (old_size && new_size) {
      memcpy(new_pointer, pointer, irs_min(old_size, new_size));
    }
    delete [](irs_u8 *)pointer;
    pointer = IRS_NULL;
  }
  return new_pointer;
}
mxapplication_t *mxapplication = IRS_NULL;
//---------------------------------------------------------------------------
void afloat_to_str(char *str, double N, size_t len, size_t accur)
{
  ostrstream strm(str, len + 1);
  strm << setiosflags(ios::fixed) << setw(len) << setprecision(accur) << N;
  strm << '\0';
  str[len] = '\0';
  //str[strm.pcount()] = 0;
}
//---------------------------------------------------------------------------
// Событие клавиатуры

mxkey_event_t::mxkey_event_t():
  f_prev_event(IRS_NULL),
  f_key(irskey_none)
{
}
mxkey_event_t::~mxkey_event_t()
{
}
void mxkey_event_t::exec(irskey_t key)
{
  f_key = key;
  if (f_prev_event) f_prev_event->exec(key);
}
irskey_t mxkey_event_t::check()
{
  irskey_t prev_key = f_key;
  f_key = irskey_none;
  return prev_key;
}
void mxkey_event_t::reset()
{
  f_key = irskey_none;
}
void mxkey_event_t::connect(mxkey_event_t *&a_event)
{
  f_prev_event = a_event;
  a_event = this;
}
//---------------------------------------------------------------------------
// Класс для выполнения антидребезга

mxantibounce_t::mxantibounce_t():
  f_pin(irs_off),
  f_time(0),
  f_to(0),
  f_mode(mode_start),
  f_occur(true)
{
  init_to_cnt();
}
mxantibounce_t::~mxantibounce_t()
{
  deinit_to_cnt();
}
void mxantibounce_t::set_time(counter_t time)
{
  f_time = time;
}
irs_pin_t mxantibounce_t::operator()(irs_pin_t pin)
{
  switch (f_mode){
    case mode_start: {
      f_pin = pin;
      f_mode = mode_wait_change;
    } break;
    case mode_wait_change: {
      if (pin != f_pin) {
        set_to_cnt(f_to, f_time);
        f_mode = mode_wait_time;
      }
    } break;
    case mode_wait_time: {
      if (test_to_cnt(f_to)) {
        f_pin = pin;
        f_occur = true;
        f_mode = mode_wait_change;
      }
    } break;
  }

  return f_pin;
}
bool mxantibounce_t::check()
{
  bool occur_save = f_occur;
  f_occur = false;
  return occur_save;
}
bool mxantibounce_t::check(irs_pin_t pin)
{
  operator()(pin);
  return check();
}
//---------------------------------------------------------------------------
// Класс для генерации событий клавиатуры

// Изменены 24.03.2009. Взяты из УПМС
// Время антидребезга по умолчанию
#define mxkey_antibounce_time_def TIME_TO_CNT(1, 20)
// Время защитного интервала по умолчанию
#define mxkey_defence_time_def TIME_TO_CNT(1, 1)
// Время интервала повтора по умолчанию
#define mxkey_rep_time_def TIME_TO_CNT(1, 6)

mxkey_event_gen_t::mxkey_event_gen_t():
  f_mxkey_drv(IRS_NULL),
  f_event(IRS_NULL),
  f_abort_request(irs_false),
  f_antibounce_time(0),
  f_defence_time(0),
  f_rep_time(0),
  f_to(0),
  f_antibounce(),
  f_key(irskey_none),
  f_mode(mode_start)
{
  init_to_cnt();
  f_antibounce_time = mxkey_antibounce_time_def;
  f_antibounce.set_time(f_antibounce_time);
  f_defence_time = mxkey_defence_time_def;
  f_rep_time = mxkey_rep_time_def;
}
mxkey_event_gen_t::~mxkey_event_gen_t()
{
  deinit_to_cnt();
}
void mxkey_event_gen_t::connect(mxkey_drv_t *mxkey_drv)
{
  f_mxkey_drv = mxkey_drv;
}
void mxkey_event_gen_t::add_event(mxkey_event_t *event)
{
  event->connect(f_event);
}
irs_bool mxkey_event_gen_t::tick()
{
  irskey_t cur_key = (*f_mxkey_drv)();

  irs_pin_t pin = irs_off;
  if (cur_key != irskey_none) pin = irs_on;

  bool is_key_down = false;
  if (f_antibounce.check(pin)) {
    f_key = cur_key;
    if (f_key == irskey_none) {
      f_mode = mode_wait_front;
    } else {
      is_key_down = true;
    }
  }

  switch (f_mode) {
    case mode_start: {
      f_mode = mode_wait_front;
    } break;
    case mode_wait_front: {
      if (is_key_down) {
        set_to_cnt(f_to, f_defence_time);
        f_event->exec(f_key);
        f_mode = mode_wait_defence;
      }
    } break;
    case mode_wait_defence: {
      if (test_to_cnt(f_to)) {
        set_to_cnt(f_to, f_rep_time);
        f_event->exec(f_key);
        f_mode = mode_wait_rep;
      }
    } break;
    case mode_wait_rep: {
      if (test_to_cnt(f_to)) {
        set_to_cnt(f_to, f_rep_time);
        f_event->exec(f_key);
      }
    } break;
    case mode_stop: {
    } break;
  }

  return !f_abort_request;
}
void mxkey_event_gen_t::abort()
{
  f_abort_request = irs_true;
}
void mxkey_event_gen_t::set_defence_time(counter_t time)
{
  f_defence_time = time;
}
void mxkey_event_gen_t::set_rep_time(counter_t time)
{
  f_rep_time = time;
}
void mxkey_event_gen_t::set_antibounce_time(counter_t time)
{
  f_antibounce_time = time;
  f_antibounce.set_time(f_antibounce_time);
}
//---------------------------------------------------------------------------
// Сервисный класс драйвера дисплея

// Используется для выполнения сервисных функции: очистка, сохранение текущей
// позиции, вывод в текущую позицию
// Внешний драйвер подключается с помощью connect

mxdisplay_drv_service_t::mxdisplay_drv_service_t():
  mp_display_drv(IRS_NULL),
  m_left(0),
  m_top(0),
  m_width(0),
  m_height(0),
  m_clear_char(' '),
  mp_clear_line(IRS_NULL)
{
}
mxdisplay_drv_service_t::~mxdisplay_drv_service_t()
{
  IRS_ARDELETE(mp_clear_line);
}
void mxdisplay_drv_service_t::connect(mxdisplay_drv_t *ap_display_drv)
{
  mp_display_drv = ap_display_drv;
  m_width = mp_display_drv->get_width();
  m_height = mp_display_drv->get_height();
  IRS_ARDELETE(mp_clear_line);
  mp_clear_line = new char[m_width + 1];
  memset(mp_clear_line, m_clear_char, m_width);
  mp_clear_line[m_width] = 0;
}

//---------------------------------------------------------------------------
// Класс драйвер-фильтр размера для консоли Windows

//конструктор без параметров
irs_win32_filtrsize_console_display_t::irs_win32_filtrsize_console_display_t()
{}
//конструктор с параметрами
irs_win32_filtrsize_console_display_t::
irs_win32_filtrsize_console_display_t(
  mxdisp_pos_t a_left,
  mxdisp_pos_t a_top,
  mxdisp_pos_t a_width,
  mxdisp_pos_t a_height):
  mp_driver(IRS_NULL),
  m_left_display_box(a_left),  //X
  m_top_display_box(a_top),    //Y
  m_height_display_box(a_height),//высота
  m_width_display_box(a_width)   //ширина
{}
mxdisp_pos_t irs_win32_filtrsize_console_display_t::get_height()
{
  return m_height_display_box;
}
mxdisp_pos_t irs_win32_filtrsize_console_display_t::get_width()
{
  return m_width_display_box;
}
void irs_win32_filtrsize_console_display_t::outtextpos(
  mxdisp_pos_t a_left,
  mxdisp_pos_t a_top,
  const char *ap_text)
{
  char* buftext = IRS_NULL;
  if(strlen(ap_text)<=static_cast<irs_u32>(m_width_display_box-a_left))
    mp_driver->outtextpos(
      static_cast<mxdisp_pos_t>(a_left+m_left_display_box),
      static_cast<mxdisp_pos_t>(a_top+m_top_display_box),
      ap_text);
  else
  {
    buftext=new char[static_cast<irs_u32>(m_width_display_box-a_left+1)];
    strncpy(buftext, ap_text, static_cast<irs_u32>(m_width_display_box-a_left));
    buftext[static_cast<irs_u32>(m_width_display_box-a_left+1)]='\0';
    mp_driver->outtextpos(
      static_cast<mxdisp_pos_t>(a_left+m_left_display_box),
      static_cast<mxdisp_pos_t>(a_top+m_top_display_box),
      buftext);
  }
  delete []buftext;
}
void irs_win32_filtrsize_console_display_t::tick()
{
}
void irs_win32_filtrsize_console_display_t::
connect(mxdisplay_drv_t *ap_display_drv)
{
  mp_driver=ap_display_drv;
  //пользователь задал длину и ширину выделяемой области.
  //проверяем, входят ли они в диапазон [1, max],
  //где max - длина или ширана родительской области
  //если выходят за пределы диапазона, то корректируем их
  if(m_height_display_box<1)
    m_height_display_box=1;
  else if(m_height_display_box>mp_driver->get_height())
    m_height_display_box=mp_driver->get_height();
  if(m_width_display_box<1)
    m_width_display_box=1;
  else if(m_width_display_box>mp_driver->get_width())
    m_width_display_box=mp_driver->get_width();
  if(m_width_display_box>mp_driver->get_width()-m_left_display_box)
    m_left_display_box=static_cast<mxdisp_pos_t>
      (mp_driver->get_width()-m_width_display_box);
  if(m_height_display_box>mp_driver->get_height()-m_top_display_box)
    m_top_display_box=static_cast<mxdisp_pos_t>
      (mp_driver->get_height()-m_height_display_box);

}

//---------------------------------------------------------------------------
// класс буфер  irs_strm_buf

irs_strm_buf::irs_strm_buf(
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
                                  //координата X, координата Y, длина, высота

irs_strm_buf::~irs_strm_buf()
{
  deinit_to_cnt();
}

void irs_strm_buf::connect(mxdisplay_drv_t *ap_driver)
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
void irs_strm_buf::connect(mxkey_event_t* ap_mxkey_event)
{
  mp_mxkey_event=ap_mxkey_event;
}
int irs_strm_buf::sync()
{
  for(mxdisp_pos_t G=0;G<m_height_display;G++)
  {
    m_display_drv_service.outtextpos(
    0,
    G,
    &(m_BufTextDisplay[(m_width_display+1)*G]) );
  }
  return 0;
}
int irs_strm_buf::overflow(int ich)
{
  outputbuf(ich);
  return ich;
}
int irs_strm_buf::underflow()
{
  return EOF;
}
void irs_strm_buf::set_refresh_time_out_text(irs_u32 a_num, irs_u32 a_denom)
{
  m_num=a_num;
  m_denom=a_denom;
}
void irs_strm_buf::set_refresh_time_blink_cursor(irs_u32 a_num, irs_u32 a_denom)
{
  m_num_nucl_cur=a_num;
  m_denom_cur=a_denom;
}
void irs_strm_buf::tick()
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
void irs_strm_buf::blink_cursor(irs_i8 a_parametr_immediate_blink_cursor)
{
  const char ChCursor[2]="_";       //вид курсора
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
void irs_strm_buf::stop_blink_cursor()           //прекратить мигать курсором
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
void irs_strm_buf::processing_key_event()  //обработка событий клавиатуры
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
void irs_strm_buf::outputbuf(irs_i32 a_ich)      //вывод в экранный буфер
{
  char ChTime[2]="\0";
  ChTime[0]=(char)a_ich;
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
void irs_strm_buf::setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top)
{
  if(a_left > m_width_display-1)
    a_left=static_cast<mxdisp_pos_t>(m_width_display-1);
  if(a_top > m_height_display-1)
    a_top=static_cast<mxdisp_pos_t>(m_height_display-1);
  m_left_activecursor_buf=a_left;      //текущая координата X курсора в буфере
  m_top_activecursor_buf=a_top;        //текущая координата Y курсора в буфере
  m_display_drv_service.setpos(a_left, a_top);
}
void irs_strm_buf::stop_get()
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

//---------------------------------------------------------------------------
//класс irs_ostream

irs_iostream::irs_iostream():
  istream(&m_strm_buf),
  ostream(&m_strm_buf)
{
  ostream::init(&m_strm_buf);
  istream::init(&m_strm_buf);
}
void irs_iostream::tick()
{
  m_strm_buf.tick();
}
//---------------------------------------------------------------------------
// Локальные данные

irs::local_data_t::local_data_t(irs_uarc a_size):
  m_data(a_size)
{
}
irs_uarc irs::local_data_t::size()
{
  return m_data.size();
}
irs_bool irs::local_data_t::connected()
{
  return irs_true;
}
void irs::local_data_t::read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
  if (a_index + a_size > m_data.size()) {
    fill(ap_buf, ap_buf + a_size, 0);
    return;
  }
  vector<irs_u8>::iterator it_begin = m_data.begin() + a_index;
  copy(it_begin, it_begin + a_size, ap_buf);
}
void irs::local_data_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index + a_size > m_data.size()) return;
  vector<irs_u8>::iterator it_begin = m_data.begin() + a_index;
  copy(ap_buf, ap_buf + a_size, it_begin);
}
irs_bool irs::local_data_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  return (m_data[a_index]&(1 << a_bit_index)) ? irs_true : irs_false;
}
void irs::local_data_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  m_data[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
}
void irs::local_data_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  m_data[a_index] &= static_cast<irs_u8>(~(1 << a_bit_index));
}
void irs::local_data_t::tick()
{
}
//---------------------------------------------------------------------------
// Общая конфигурация консоли
irs::conio_cfg_t::
conio_cfg_t(mxkey_drv_t& a_key_drv, mxdisplay_drv_t& a_display_drv):
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
irs_bool irs::conio_cfg_t::
tick()
{
  m_key_event_gen.tick();
  m_display_drv.tick();
  m_strm.tick();
  return !m_abort_request;
}
void irs::conio_cfg_t::
abort()
{
  m_abort_request = irs_false;
}
irs_iostream& irs::conio_cfg_t::
stream()
{
  return m_strm;
}
mxkey_event_t& irs::conio_cfg_t::
key_event()
{
  return m_app_key_event;
}
