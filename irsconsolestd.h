// Консоль, выделенная из irsstd
// Дата: 30.10.2010
// Дата создания: 27.09.2009

#ifndef IRSCONSOLESTDH
#define IRSCONSOLESTDH

#include <irsdefs.h>

//#include <irsstrmstd.h>
#include <timer.h>

#include <irsfinal.h>

// Тип для позции на дисплее
typedef size_t mxdisp_pos_t;

// Тип индикатора
enum irslcd_t {
  irslcd_unknown,
  irslcd_4x20,
  irslcd_oled_4x20
};

// Тип подключения клавиатуры
enum irskbd_t {
  irskbd_high_pull_up,
  irskbd_low_pull_up
};

// Тип раскладки клавиш клавиатуры
enum irskbd_map_t {
  irskbd_map_vertical,
  irskbd_map_horisontal,
  irskbd_map_gtch_rev_1
};

// Коды клавиш
enum irskey_t {
  irskey_none     = 0,
  irskey_0        = 1,
  irskey_1        = 2,
  irskey_2        = 3,
  irskey_3        = 4,
  irskey_4        = 5,
  irskey_5        = 6,
  irskey_6        = 7,
  irskey_7        = 8,
  irskey_8        = 9,
  irskey_9        = 10,
  irskey_point    = 11,
  irskey_enter    = 12,
  irskey_up       = 13,
  irskey_down     = 14,
  irskey_backspace= 15,
  irskey_escape   = 16
};

// Абстрактный базовый класс драйвера клавиатуры
class mxkey_drv_t
{
public:
  virtual ~mxkey_drv_t() {}
  virtual irskey_t operator()() = 0;
};

class encoder_drv_t
{
public:
  virtual ~encoder_drv_t() {}
  virtual int get_press_count() = 0;
  virtual irskey_t get_key_encoder() = 0;
  virtual irskey_t get_key_button() = 0;
};

// Абстрактный базовый класс драйвера дисплея
class mxdisplay_drv_t
{
public:
  virtual ~mxdisplay_drv_t() {}
  virtual mxdisp_pos_t get_height() = 0;
  virtual mxdisp_pos_t get_width() = 0;
  virtual void outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
    const char *text) = 0;
  virtual void tick() = 0;
};


// Событие клавиатуры
class mxkey_event_t
{
  mxkey_event_t *f_prev_event;
  irskey_t f_key;
public:
  inline mxkey_event_t();
  inline virtual ~mxkey_event_t();
  inline virtual void exec(irskey_t key);
  inline virtual irskey_t check();
  inline virtual void reset();
  inline virtual void connect(mxkey_event_t *&a_event);
};
inline mxkey_event_t::mxkey_event_t():
  f_prev_event(IRS_NULL),
  f_key(irskey_none)
{
}
inline mxkey_event_t::~mxkey_event_t()
{
}
inline void mxkey_event_t::exec(irskey_t key)
{
  f_key = key;
  if (f_prev_event) f_prev_event->exec(key);
}
inline irskey_t mxkey_event_t::check()
{
  irskey_t prev_key = f_key;
  f_key = irskey_none;
  return prev_key;
}
inline void mxkey_event_t::reset()
{
  f_key = irskey_none;
}
inline void mxkey_event_t::connect(mxkey_event_t *&a_event)
{
  f_prev_event = a_event;
  a_event = this;
}


// Класс для выполнения антидребезга
class mxantibounce_t
{
  typedef enum _mode_t{
    mode_start,
    mode_wait_change,
    mode_wait_time
  } mode_t;

  irs_pin_t f_pin;
  counter_t f_time;
  counter_t f_to;
  mode_t f_mode;
  bool f_occur;
public:
  inline mxantibounce_t();
  inline ~mxantibounce_t();
  inline void set_time(counter_t time);
  inline irs_pin_t operator()(irs_pin_t pin);
  // 25.03.2009 - добавлена проверка события при изменении на выходе
  inline bool check();
  inline bool check(irs_pin_t pin);
};
inline mxantibounce_t::mxantibounce_t():
  f_pin(irs_off),
  f_time(0),
  f_to(0),
  f_mode(mode_start),
  f_occur(true)
{
  init_to_cnt();
}
inline mxantibounce_t::~mxantibounce_t()
{
  deinit_to_cnt();
}
inline void mxantibounce_t::set_time(counter_t time)
{
  f_time = time;
}
inline irs_pin_t mxantibounce_t::operator()(irs_pin_t pin)
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
inline bool mxantibounce_t::check()
{
  bool occur_save = f_occur;
  f_occur = false;
  return occur_save;
}
inline bool mxantibounce_t::check(irs_pin_t pin)
{
  operator()(pin);
  return check();
}


// Класс для генерации событий клавиатуры

// Изменены 24.03.2009. Взяты из УПМС
// Время антидребезга по умолчанию
#define mxkey_antibounce_time_def TIME_TO_CNT(1, 20)
// Время антидребезга по умолчанию
#define mxkey_antibounce_encoder_time_def TIME_TO_CNT(1, 20)
// Время защитного интервала по умолчанию
#define mxkey_defence_time_def TIME_TO_CNT(1, 1)
// Время интервала повтора по умолчанию
#define mxkey_rep_time_def TIME_TO_CNT(1, 6)

class mxkey_event_gen_t: public mx_proc_t
{
  typedef enum _mode_t {
    mode_start,
    mode_wait_front,
    mode_wait_defence,
    mode_wait_rep,
    mode_stop
  } mode_t;

  mxkey_drv_t *f_mxkey_drv;
  encoder_drv_t *mp_encoder_drv;
  mxkey_event_t *f_event;
  irs_bool f_abort_request;
  counter_t f_defence_time;
  counter_t f_rep_time;
  counter_t f_to;
  mxantibounce_t f_antibounce;
  mxantibounce_t m_antibounce_encoder;
  irskey_t f_key;
  irskey_t m_encoder_button_key;
  mode_t f_mode;
  mode_t m_encoder_mode;
  counter_t m_encoder_button_time;
public:
  inline mxkey_event_gen_t();
  inline ~mxkey_event_gen_t();
  inline void connect(mxkey_drv_t *mxkey_drv);
  inline void connect_encoder(encoder_drv_t *ap_encoder_drv);
  inline void add_event(mxkey_event_t *event);
  inline virtual irs_bool tick();
  inline virtual void abort();
  inline void set_antibounce_time(counter_t time);
  inline void set_antibounce_encoder_time(counter_t a_time);
  inline void set_defence_time(counter_t time);
  inline void set_rep_time(counter_t time);
};
inline mxkey_event_gen_t::mxkey_event_gen_t():
  f_mxkey_drv(IRS_NULL),
  mp_encoder_drv(IRS_NULL),
  f_event(IRS_NULL),
  f_abort_request(irs_false),
  f_defence_time(0),
  f_rep_time(0),
  f_to(0),
  f_antibounce(),
  m_antibounce_encoder(),
  f_key(irskey_none),
  m_encoder_button_key(irskey_none),
  f_mode(mode_start),
  m_encoder_mode(mode_start),
  m_encoder_button_time(0)
{
  init_to_cnt();
  f_antibounce.set_time(mxkey_antibounce_time_def);
  m_antibounce_encoder.set_time(mxkey_antibounce_encoder_time_def);
  f_defence_time = mxkey_defence_time_def;
  f_rep_time = mxkey_rep_time_def;
}
inline mxkey_event_gen_t::~mxkey_event_gen_t()
{
  deinit_to_cnt();
}
inline void mxkey_event_gen_t::connect(mxkey_drv_t *mxkey_drv)
{
  f_mxkey_drv = mxkey_drv;
}
inline void mxkey_event_gen_t::connect_encoder(encoder_drv_t *ap_encoder_drv)
{
  mp_encoder_drv = ap_encoder_drv;
}
inline void mxkey_event_gen_t::add_event(mxkey_event_t *event)
{
  event->connect(f_event);
}
inline irs_bool mxkey_event_gen_t::tick()
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
  
  if (mp_encoder_drv != IRS_NULL) {
    cur_key = mp_encoder_drv->get_key_button();
    pin = irs_off;
    if (cur_key != irskey_none) pin = irs_on;
    is_key_down = false;
    if (m_antibounce_encoder.check(pin)) {
      m_encoder_button_key = cur_key;
      if (m_encoder_button_key == irskey_none) {
        m_encoder_mode = mode_wait_front;
      } else {
        is_key_down = true;
      }
    }
    switch (m_encoder_mode) {
      case mode_start: {
        f_mode = mode_wait_front;
      } break;
      case mode_wait_front: {
        if (is_key_down) {
          set_to_cnt(m_encoder_button_time, f_defence_time);
          f_event->exec(m_encoder_button_key);
          m_encoder_mode = mode_wait_defence;
        }
      } break;
      case mode_wait_defence: {
        if (test_to_cnt(m_encoder_button_time)) {
          set_to_cnt(m_encoder_button_time, f_rep_time);
          f_event->exec(m_encoder_button_key);
          m_encoder_mode = mode_wait_rep;
        }
      } break;
      case mode_wait_rep: {
        if (test_to_cnt(m_encoder_button_time)) {
          set_to_cnt(m_encoder_button_time, f_rep_time);
          f_event->exec(m_encoder_button_key);
        }
      } break;
      case mode_stop: {
      } break;
    }
    
    cur_key = mp_encoder_drv->get_key_encoder();
    if (cur_key != irskey_none) {
      int count = mp_encoder_drv->get_press_count();
      for (int i = 0; i < count; i++) {
        f_event->exec(cur_key);  
      }
    }
  }
  return !f_abort_request;
}
inline void mxkey_event_gen_t::abort()
{
  f_abort_request = irs_true;
}
inline void mxkey_event_gen_t::set_defence_time(counter_t time)
{
  f_defence_time = time;
}
inline void mxkey_event_gen_t::set_rep_time(counter_t time)
{
  f_rep_time = time;
}
inline void mxkey_event_gen_t::set_antibounce_time(counter_t time)
{
  f_antibounce.set_time(time);
}
inline void mxkey_event_gen_t::set_antibounce_encoder_time(counter_t a_time)
{
  m_antibounce_encoder.set_time(a_time);
}

// Сервисный класс драйвера дисплея

// Используется для выполнения сервисных функции: очистка, сохранение текущей
// позиции, вывод в текущую позицию
// Внешний драйвер подключается с помощью connect

class mxdisplay_drv_service_t
{
  mxdisplay_drv_t *mp_display_drv;
  mxdisp_pos_t m_left;
  mxdisp_pos_t m_top;
  mxdisp_pos_t m_width;
  mxdisp_pos_t m_height;
  const char m_clear_char;
  char *mp_clear_line;
public:
  inline mxdisplay_drv_service_t();
  inline ~mxdisplay_drv_service_t();
  inline void outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
    const char *ap_text);
  inline void outtext(const char *ap_text);
  inline void setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top);
  inline void getpos(mxdisp_pos_t &a_left, mxdisp_pos_t &a_top);
  inline mxdisp_pos_t get_height();
  inline mxdisp_pos_t get_width();
  inline void clear();
  inline void clear_line(mxdisp_pos_t a_top);
  inline void connect(mxdisplay_drv_t *ap_display_drv);
};
inline mxdisplay_drv_service_t::mxdisplay_drv_service_t():
  mp_display_drv(IRS_NULL),
  m_left(0),
  m_top(0),
  m_width(0),
  m_height(0),
  m_clear_char(' '),
  mp_clear_line(IRS_NULL)
{
}
inline mxdisplay_drv_service_t::~mxdisplay_drv_service_t()
{
  IRS_ARDELETE(mp_clear_line);
}
inline void mxdisplay_drv_service_t::connect(mxdisplay_drv_t *ap_display_drv)
{
  mp_display_drv = ap_display_drv;
  m_width = mp_display_drv->get_width();
  m_height = mp_display_drv->get_height();
  IRS_ARDELETE(mp_clear_line);
  mp_clear_line = new char[m_width + 1];
  memset(mp_clear_line, m_clear_char, m_width);
  mp_clear_line[m_width] = 0;
}
inline void mxdisplay_drv_service_t::outtextpos(mxdisp_pos_t a_left,
  mxdisp_pos_t a_top, const char *ap_text)
{
  if (mp_display_drv) mp_display_drv->outtextpos(a_left, a_top, ap_text);
}
inline void mxdisplay_drv_service_t::outtext(const char *ap_text)
{
  if (mp_display_drv) mp_display_drv->outtextpos(m_left, m_top, ap_text);
}
inline void mxdisplay_drv_service_t::setpos(mxdisp_pos_t a_left,
  mxdisp_pos_t a_top)
{
  m_left = a_left;
  m_top = a_top;
}
inline void mxdisplay_drv_service_t::getpos(mxdisp_pos_t &a_left,
  mxdisp_pos_t &a_top)
{
  a_left = m_left;
  a_top = m_top;
}
inline mxdisp_pos_t mxdisplay_drv_service_t::get_height()
{
  return m_height;
}
inline mxdisp_pos_t mxdisplay_drv_service_t::get_width()
{
  return m_width;
}
inline void mxdisplay_drv_service_t::clear()
{
  for (mxdisp_pos_t top = 0; top < m_height; top++) clear_line(top);
}
inline void mxdisplay_drv_service_t::clear_line(mxdisp_pos_t a_top)
{
  if (mp_clear_line) outtextpos(0, a_top, mp_clear_line);
}

//Класс драйвер-фильтр размера для консоли Windows
class irs_win32_filtrsize_console_display_t: public mxdisplay_drv_t
{
private:
  mxdisplay_drv_t *mp_driver;
  mxdisp_pos_t m_left_display_box; //X
  mxdisp_pos_t m_top_display_box;  //Y
  mxdisp_pos_t m_height_display_box; //высота
  mxdisp_pos_t m_width_display_box;  //ширина
  //конструктор без параметров запрещен
  irs_win32_filtrsize_console_display_t();
public:
  inline irs_win32_filtrsize_console_display_t(
    mxdisp_pos_t a_left,
    mxdisp_pos_t a_top,
    mxdisp_pos_t a_width,
    mxdisp_pos_t a_height
  );
  inline virtual mxdisp_pos_t get_height();
  inline virtual mxdisp_pos_t get_width();
  inline virtual void outtextpos(
    mxdisp_pos_t a_left,
    mxdisp_pos_t a_top,
    const char *ap_text);
  inline virtual void tick();
  inline void connect(mxdisplay_drv_t *ap_display_drv);
};
inline irs_win32_filtrsize_console_display_t::
  irs_win32_filtrsize_console_display_t
(
  mxdisp_pos_t a_left,
  mxdisp_pos_t a_top,
  mxdisp_pos_t a_width,
  mxdisp_pos_t a_height
):
  mp_driver(IRS_NULL),
  m_left_display_box(a_left),  //X
  m_top_display_box(a_top),    //Y
  m_height_display_box(a_height),//высота
  m_width_display_box(a_width)   //ширина
{
}
inline mxdisp_pos_t irs_win32_filtrsize_console_display_t::get_height()
{
  return m_height_display_box;
}
inline mxdisp_pos_t irs_win32_filtrsize_console_display_t::get_width()
{
  return m_width_display_box;
}
inline void irs_win32_filtrsize_console_display_t::outtextpos(
  mxdisp_pos_t a_left,
  mxdisp_pos_t a_top,
  const char *ap_text)
{
  if(strlen(ap_text)<=static_cast<irs_u32>(m_width_display_box-a_left))
    mp_driver->outtextpos(
      static_cast<mxdisp_pos_t>(a_left+m_left_display_box),
      static_cast<mxdisp_pos_t>(a_top+m_top_display_box),
      ap_text);
  else
  {
    char* buftext=new char[static_cast<irs_u32>(m_width_display_box-a_left+1)];
    strncpy(buftext, ap_text, static_cast<irs_u32>(m_width_display_box-a_left));
    buftext[static_cast<irs_u32>(m_width_display_box-a_left+1)]='\0';
    mp_driver->outtextpos(
      static_cast<mxdisp_pos_t>(a_left+m_left_display_box),
      static_cast<mxdisp_pos_t>(a_top+m_top_display_box),
      buftext);
    delete []buftext;
  }
}
inline void irs_win32_filtrsize_console_display_t::tick()
{
}
inline void irs_win32_filtrsize_console_display_t::
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

#endif //IRSCONSOLESTDH
