// Стандартаная библиотека ИРС общая часть
// Дата: 10.09.2009

#ifndef irsstdgH
#define irsstdgH

#include <string.h>

#include <irsdefs.h>
#include <irscpp.h>
#include <timer.h>
#include <irsexcept.h>
#include <irslimits.h>
#include <irsint.h>

//typedef string std_string;

// Тип для пина
typedef enum _irs_pin_t{
  irs_on = 0,
  irs_off = 1
} irs_pin_t;

// Абстрактный тип для портов AVR
typedef enum _irs_avr_port_t {
  irs_avr_porta = 0,
  irs_avr_portb = 1,
  irs_avr_portc = 2,
  irs_avr_portd = 3,
  irs_avr_porte = 4,
  irs_avr_portf = 5,
  irs_avr_portg = 6
} irs_avr_port_t;

// Тип для позции на дисплее
typedef size_t mxdisp_pos_t;

// Тип индикатора
typedef enum _irslcd_t {
  irslcd_unknown,
  irslcd_4x20
} irslcd_t;

// Тип подключения клавиатуры
typedef enum _irskbd_t {
  irskbd_high_pull_up,
  irskbd_low_pull_up
} irskbd_t;

// Тип раскладки клавиш клавиатуры
typedef enum _irskbd_map_t {
  irskbd_map_vertical,
  irskbd_map_horisontal,
  irskbd_map_gtch_rev_1
} irskbd_map_t;

// Коды клавиш
typedef enum _irskey_t {
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
} irskey_t;

// Класс для функций обработки
class mx_proc_t {
public:
  virtual ~mx_proc_t() {}
  virtual irs_bool tick() = 0;
  virtual void abort() = 0;
};

// Абстактный базовый класс драйвера консоли
class mxcondrv_t
{
public:
  virtual ~mxcondrv_t() {}
  virtual void out(const char *str) = 0;
};

// Класс для приложения
class mxapplication_t
{
  irs_bool first_exec;
  irs_bool stop_exec;
  void (*f_init)();
  void (*f_deinit)();
  void (*f_tick)();
  void (*f_stop)();
  irs_bool (*f_stopped)();

  void init();
  void deinit();
  void tick();
  void stop();
  irs_bool stopped();
public:
  mxapplication_t();
  ~mxapplication_t();
  void set_init_event(void (*a_init)());
  void set_denit_event(void (*a_deinit)());
  void set_tick_event(void (*a_tick)());
  void set_stop_event(void (*a_stop)());
  void set_stopped_event(irs_bool (*a_stopped)());
  irs_bool run(irs_bool exec);
};

// Абстрактный базовый класс драйвера клавиатуры
class mxkey_drv_t
{
public:
  virtual ~mxkey_drv_t() {}
  virtual irskey_t operator()() = 0;
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

// Перераспределение памяти с сохранением данных
void *irs_renew(void *pointer, size_t old_size, size_t new_size);

// Событие клавиатуры
class mxkey_event_t
{
  mxkey_event_t *f_prev_event;
  irskey_t f_key;
public:
  mxkey_event_t();
  virtual ~mxkey_event_t();
  virtual void exec(irskey_t key);
  virtual irskey_t check();
  virtual void reset();
  virtual void connect(mxkey_event_t *&a_event);
};

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
  mxantibounce_t();
  ~mxantibounce_t();
  void set_time(counter_t time);
  irs_pin_t operator()(irs_pin_t pin);
  // 25.03.2009 - добавлена проверка события при изменении на выходе
  bool check();
  bool check(irs_pin_t pin);
};

// Класс для генерации событий клавиатуры
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
  mxkey_event_t *f_event;
  irs_bool f_abort_request;
  counter_t f_antibounce_time;
  counter_t f_defence_time;
  counter_t f_rep_time;
  counter_t f_to;
  mxantibounce_t f_antibounce;
  irskey_t f_key;
  mode_t f_mode;
public:
  mxkey_event_gen_t();
  ~mxkey_event_gen_t();
  void connect(mxkey_drv_t *mxkey_drv);
  void add_event(mxkey_event_t *event);
  virtual irs_bool tick();
  virtual void abort();
  void set_antibounce_time(counter_t time);
  void set_defence_time(counter_t time);
  void set_rep_time(counter_t time);
};

// Сервисный класс драйвера дисплея
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
  mxdisplay_drv_service_t();
  ~mxdisplay_drv_service_t();
  inline void outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
    const char *ap_text);
  inline void outtext(const char *ap_text);
  inline void setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top);
  inline void getpos(mxdisp_pos_t &a_left, mxdisp_pos_t &a_top);
  inline mxdisp_pos_t get_height();
  inline mxdisp_pos_t get_width();
  inline void clear();
  inline void clear_line(mxdisp_pos_t a_top);
  void connect(mxdisplay_drv_t *ap_display_drv);
};
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

// Функция Макса Полякова для преобразования double в char*
void afloat_to_str(char *str, double N, size_t len, size_t accur);

//Класс драйвер-фильтр размера для консоли Windows
class irs_win32_filtrsize_console_display_t: public mxdisplay_drv_t
{
private:
  mxdisplay_drv_t *mp_driver;
  mxdisp_pos_t m_left_display_box; //X
  mxdisp_pos_t m_top_display_box;  //Y
  mxdisp_pos_t m_height_display_box; //высота
  mxdisp_pos_t m_width_display_box;  //ширина
  irs_win32_filtrsize_console_display_t();//конструктор без параметров запрещен
public:
  //конструктор с параметрами
  irs_win32_filtrsize_console_display_t(
    mxdisp_pos_t a_left,
    mxdisp_pos_t a_top,
    mxdisp_pos_t a_width,
    mxdisp_pos_t a_height);
  virtual mxdisp_pos_t get_height();
  virtual mxdisp_pos_t get_width();
  virtual void outtextpos(
    mxdisp_pos_t a_left,
    mxdisp_pos_t a_top,
    const char *ap_text);
  virtual void tick();
  void connect(mxdisplay_drv_t *ap_display_drv);
};

// класс буфер
class irs_strm_buf: public streambuf//filebuf
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
  irs_strm_buf();
  virtual ~irs_strm_buf();
  inline mxdisp_pos_t get_height();
  inline mxdisp_pos_t get_width();
  void connect(mxdisplay_drv_t *ap_driver);           // коннектимся к драйверу
  //коннектимся к обьекту класса mxkey_event_t
  void connect(mxkey_event_t* ap_mxkey_event);
  inline void clear();        //очистка всего буфера и экрана
  //очистка сроки с номером  a_top
  inline void clear_line(mxdisp_pos_t a_top);
  void setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top);//задаем позицию курсора
  virtual int sync();
  virtual int overflow( int = EOF );
  virtual int underflow();
  //задаем скорость вывода текста на экран
  void set_refresh_time_out_text(irs_u32 a_num=1, irs_u32 a_denom=10);
  //задаем скорость мигания курсора
  void set_refresh_time_blink_cursor(irs_u32 a_num=1, irs_u32 a_denom=2);
  void tick();
  inline void start_get();
  inline irs_bool check_get();
  void stop_get();
private:
  //начать мигать курсором
  void blink_cursor(irs_i8 a_parametr_immediate_blink_cursor=0);
  void stop_blink_cursor();     //прекратить мигать курсором
  void processing_key_event();  //обработка событий клавиатуры
  void outputbuf(irs_i32 a_ich);  //вывод в экранный буфер

};
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

// класс irs_ostream
class irs_iostream: public istream, public ostream
{
  irs_strm_buf m_strm_buf;
  mxkey_event_t* mp_mxkey_event;
public:
  irs_iostream();
  inline mxdisp_pos_t get_height();
  inline mxdisp_pos_t get_width();
  inline void connect(mxdisplay_drv_t *ap_driver);
  inline void connect(mxkey_event_t* ap_mxkey_event);
  inline void clear_display();
  inline void clear_line(mxdisp_pos_t a_top);
  inline ostream& setpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top);
  void tick();
  inline void start_get();
  inline irs_bool check_get();
  inline void stop_get();
};
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


extern mxapplication_t *mxapplication;

namespace irs {

const int npos = -1;

// Прямоугольник
struct rect_t {
  irs_i32 left;
  irs_i32 top;
  irs_i32 width;
  irs_i32 height;
  rect_t(
    irs_i32 a_left = 0,
    irs_i32 a_top = 0,
    irs_i32 a_width = 0,
    irs_i32 a_height = 0
  ):
    left(a_left),
    top(a_top),
    width(a_width),
    height(a_height)
  {}
};

// Число на выходе будет в заданных границах
template <class T>
T range(T val, T min_val, T max_val)
{
  return min(max(min_val, val), max_val);
}

// Базовый класс для исключений утверждений всего семейства классов,
// работающих с mxdata_t
class data_assert_e: public assert_e
{
  virtual const char *what() const throw()
  {
    return "data_assert_e";
  }
};

// Абстактный базовый класс для массива данных
class mxdata_t
{
public:
  virtual ~mxdata_t() {}
  virtual irs_uarc size() = 0;
  virtual irs_bool connected() = 0;
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size) = 0;
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size) = 0;
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index) = 0;
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index) = 0;
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index) = 0;
  virtual void tick() = 0;
};

// Локальные данные
class local_data_t: public mxdata_t
{
public:
  local_data_t(irs_uarc a_size);
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
private:
  vector<irs_u8> m_data;

  local_data_t();
};
template <class T, int size = sizeof(T), bool check = false>
class conn_data_t
{
public:
  conn_data_t();
  conn_data_t(irs::mxdata_t *a_data, irs_uarc a_index);
  conn_data_t(irs::mxdata_t *a_data, irs_uarc a_index, irs_uarc &a_ret_index);
  irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_index);
  irs::mxdata_t *data() const;
  irs_uarc index() const;

  const T &operator=(const T &a_elem);
  const conn_data_t& operator=(const conn_data_t& a_conn_data);
  operator T();
private:
  irs_uarc m_index;
  irs::mxdata_t *m_data;
  //T m_elem;
  //irs_bool m_connected;
};

template <class T, int size, bool check>
conn_data_t<T, size, check>::conn_data_t():
  m_index(0),
  m_data(IRS_NULL)//,
  //m_elem()
  //m_connected(irs_false)
{
}
template <class T, int size, bool check>
conn_data_t<T, size, check>::conn_data_t(irs::mxdata_t *a_data,
  irs_uarc a_index):
  m_index(0),
  m_data(IRS_NULL)
  //m_elem()
  //m_connected(irs_false)
{
  connect(a_data, a_index);
}
template <class T, int size, bool check>
conn_data_t<T, size, check>::conn_data_t(irs::mxdata_t *a_data,
  irs_uarc a_index, irs_uarc &a_ret_index):
  m_index(0),
  m_data(IRS_NULL)//,
  //m_elem()
  //m_connected(irs_false)
{
  a_ret_index = connect(a_data, a_index);
}
template <class T, int size, bool check>
irs_uarc conn_data_t<T, size, check>::connect(irs::mxdata_t *a_data,
  irs_uarc a_index)
{
  //if (a_data) {
    //if (a_index + size <= a_data->size()) {
      //m_connected = irs_true;
      m_index = a_index;
      m_data = a_data;
      return a_index + size;
    //} else {
      //m_connected = irs_true;
    //}
  //}
  //return a_index;
}
template <class T, int size, bool check>
const T& conn_data_t<T, size, check>::operator=(const T &a_elem)
{
  //if (check) if (!m_connected) return m_elem;
  m_data->write(reinterpret_cast<const irs_u8 *>(&a_elem), m_index, size);
  return a_elem;
}
template <class T, int size, bool check>
const conn_data_t<T, size, check>& conn_data_t<T, size, check>::operator=(
  const conn_data_t<T, size, check>& a_conn_data)
{
  T elem = a_conn_data;
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  m_data->write(p_elem_u8, m_index, size);
  return elem;
}
template <class T, int size, bool check>
conn_data_t<T, size, check>::operator T()
{
  //if (check) if (!m_connected) return m_elem;
  T elem = T();
  irs_u8* p_elem_u8 = reinterpret_cast<irs_u8*>(&elem);
  if (irs::numeric_limits<T>::is_floating) {
    m_data->read(p_elem_u8, m_index, size);
    if (irs::numeric_limits<T>::is_inf_or_nan(p_elem_u8)) {
      fill(p_elem_u8, p_elem_u8 + sizeof(T), irs_u8());
      elem = irs::numeric_limits<T>::mark_bad();
    }
  } else {
    m_data->read(p_elem_u8, m_index, size);
  }
  return elem;
}
template <class T, int size, bool check>
irs::mxdata_t *conn_data_t<T, size, check>::data() const
{
  return m_data;
}
template <class T, int size, bool check>
irs_uarc conn_data_t<T, size, check>::index() const
{
  return m_index;
}

// Наложение массива на mxdata_t
template <class T, int elem_size = sizeof(T), bool check = false>
class array_data_t
{
public:

  array_data_t();
  // a_data_index - смещение в mxdata_t в байтах
  // a_array_size - количество элементов типа T
  array_data_t(irs::mxdata_t *a_data, irs_uarc a_data_index,
    irs_uarc a_array_size);
  array_data_t(irs::mxdata_t *a_data, irs_uarc a_data_index,
    irs_uarc a_array_size, irs_uarc &a_ret_data_index);
  irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_data_index,
    irs_uarc a_array_size);
  conn_data_t<T, elem_size, check> &operator[](irs_uarc a_array_index);
private:
  irs_uarc m_data_index;
  irs_uarc m_array_size;
  irs::mxdata_t *m_data;
  conn_data_t<T, elem_size, check> m_elem;
  //irs_bool m_connected;
};
template <class T, int elem_size, bool check>
array_data_t<T, elem_size, check>::array_data_t():
  m_data_index(0),
  m_array_size(0),
  m_data(IRS_NULL),
  m_elem()//,
  //m_connected(irs_false)
{
}
template <class T, int elem_size, bool check>
array_data_t<T, elem_size, check>::array_data_t(irs::mxdata_t *a_data,
  irs_uarc a_data_index, irs_uarc a_array_size):
  m_data_index(0),
  m_array_size(0),
  m_data(IRS_NULL),
  m_elem()//,
  //m_connected(irs_false)
{
  connect(a_data, a_data_index, a_array_size);
}
template <class T, int elem_size, bool check>
array_data_t<T, elem_size, check>::array_data_t(irs::mxdata_t *a_data,
  irs_uarc a_data_index, irs_uarc a_array_size, irs_uarc &a_ret_data_index):
  m_data_index(0),
  m_array_size(0),
  m_data(IRS_NULL),
  m_elem()//,
  //m_connected(irs_false)
{
  a_ret_data_index = connect(a_data, a_data_index, a_array_size);
}
template <class T, int elem_size, bool check>
irs_uarc array_data_t<T, elem_size, check>::connect(irs::mxdata_t *a_data,
  irs_uarc a_data_index, irs_uarc a_array_size)
{
  //if (a_data) {
    //if (a_data_index + elem_size*a_array_size <= a_data->size()) {
      //m_connected = irs_true;
      m_data_index = a_data_index;
      m_array_size = a_array_size;
      m_data = a_data;
      return a_data_index + a_array_size*elem_size;
    //} else {
      //m_connected = irs_true;
    //}
  //}
  //return a_data_index;
}
template <class T, int elem_size, bool check>
conn_data_t<T, elem_size, check> &array_data_t<T, elem_size, check>::
operator[](irs_uarc a_array_index)
{
  m_elem.connect(m_data, m_data_index + a_array_index*elem_size);
  return m_elem;
}
// Наложение бита на mxdata_t
class bit_data_t
{
public:
  typedef int bit_t;

  class conn_fail_e: public data_assert_e
  {
    virtual const char *what() const throw()
    {
      return "bit_data_t::conn_fail_e";
    }
  };

  inline bit_data_t(irs::mxdata_t *a_data = 0, irs_uarc a_index = 0,
    irs_uarc a_bit_index = 0);
  inline irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_index,
    irs_uarc a_bit_index);
  inline irs::mxdata_t *data() const;
  inline irs_uarc index() const;
  inline irs_uarc bit_index() const;

  inline bit_t operator=(bit_t a_elem);
  inline operator bit_t();
private:
  static const irs_uarc m_max_bit_count = 8;

  irs::mxdata_t *mp_data;
  irs_uarc m_index;
  irs_uarc m_bit_index;
};
inline bit_data_t::bit_data_t(mxdata_t *ap_data, irs_uarc a_index,
  irs_uarc a_bit_index):
  mp_data(ap_data),
  m_index(a_index),
  m_bit_index(a_bit_index)
{
}
inline irs_uarc bit_data_t::connect(mxdata_t *ap_data, irs_uarc a_index,
  irs_uarc a_bit_index)
{
  //bit_t data_valid = (ap_data != 0);
  //bit_t index_valid = a_index < ap_data->size();
  //bit_t bit_index_valid = a_bit_index < m_max_bit_count;
  //if (data_valid && index_valid && bit_index_valid) {
    mp_data = ap_data;
    m_index = a_index;
    m_bit_index = a_bit_index;
  //} else {
    //throw conn_fail_e();
  //}
  return a_index;
}
inline bit_data_t::bit_t bit_data_t::operator=
  (bit_data_t::bit_t a_elem)
{
  if (a_elem) {
    mp_data->set_bit(m_index, m_bit_index);
  } else {
    mp_data->clear_bit(m_index, m_bit_index);
  }
  return a_elem;
}
inline bit_data_t::operator bit_t()
{
  bit_t ret_bit = 0;
  if (mp_data->bit(m_index, m_bit_index)) ret_bit = 1;
  return ret_bit;
}
inline irs::mxdata_t *bit_data_t::data() const
{
  return mp_data;
}
inline irs_uarc bit_data_t::index() const
{
  return m_index;
}
inline irs_uarc bit_data_t::bit_index() const
{
  return m_bit_index;
}


// Работа с массивами по типу auto_ptr
template <class T>
class auto_arr
{
  T* m_arr;
public:
  auto_arr(T* a_arr = IRS_NULL): m_arr(a_arr) {}
  ~auto_arr() { delete []m_arr; }
  T *get() const { return m_arr; }
  T& operator[](int a_index) const { return m_arr[a_index]; }
  T& operator*() const { return *m_arr; }
  T* operator->() const { return m_arr; }
  T* release() { T* arr = m_arr; m_arr = IRS_NULL; return arr; }
  void reset(T* a_arr = IRS_NULL)
  {
    if (a_arr != m_arr) { delete []m_arr; m_arr = a_arr; }
  }
};


// Добавление операций ввода/вывода для типа string
template <class T>
inline ostrstream& string_assign_helper(ostrstream& strm, const T& val)
{
  const int sizeofT = sizeof(T);
  int prec = 15;
  if (sizeofT <= 4) prec = 6;
  else if (sizeofT <= 8) prec = 15;
  else if (sizeofT <= 10) prec = 19;
  strm.precision(prec);
  strm << val;
  return strm;
}
inline ostrstream& string_assign_helper(ostrstream& strm, const irs_u8& val)
{
  //strm.precision(30);
  int val_int = val;
  strm << val_int;
  return strm;
}
inline ostrstream& string_assign_helper(ostrstream& strm, const irs_i8& val)
{
  //strm.precision(30);
  int val_int = val;
  strm << val_int;
  return strm;
}
template <class S, class T>
inline S& string_assign(S& strg, const T& val)
{
  ostrstream strm;
  string_assign_helper(strm, val);
  strg.assign(strm.str(), strm.pcount());
  strm.rdbuf()->freeze(false);
  return strg;
}
class string: public ::string
{
public:
  string& operator=(const ::string& strg)
  {
    ::string& base = ( ::string&)(*this);
    base = strg;
    return *this;
  }
  string& operator=(const char *cstr)
  {
    ::string& base = ( ::string&)(*this);
    base = cstr;
    return *this;
  }
  string& operator=(const wchar_t *cstr);
  string& operator=(const bool& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const char& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed char& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned char& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed short& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned short& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed int& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned int& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const signed long& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const unsigned long& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const float& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const double& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const long double& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const irs_i64& val)
  {
    return string_assign(*this, val);
  }
  string& operator=(const irs_u64& val)
  {
    return string_assign(*this, val);
  }

  #ifdef NOP
  operator ::string&()
  {
    ::string& base = ( ::string&)(*this);
    return base;
  }
  #endif //NOP

  string()
  {
  }
  string(const ::string& strg)
  {
    ::string& base = ( ::string&)(*this);
    base = strg;
  }
  string(const char *cstr)
  {
    *this = cstr;
  }
  string(const wchar_t *cstr);

  string(const bool& val)
  {
    *this = val;
  }
  string(const char& val)
  {
    *this = val;
  }
  string(const signed char& val)
  {
    *this = val;
  }
  string(const unsigned char& val)
  {
    *this = val;
  }
  string(const signed short& val)
  {
    *this = val;
  }
  string(const unsigned short& val)
  {
    *this = val;
  }
  string(const signed int& val)
  {
    *this = val;
  }
  string(const unsigned int& val)
  {
    *this = val;
  }
  string(const signed long& val)
  {
    *this = val;
  }
  string(const unsigned long& val)
  {
    *this = val;
  }
  string(const float& val)
  {
    *this = val;
  }
  string(const double& val)
  {
    *this = val;
  }
  string(const long double& val)
  {
    *this = val;
  }
  string(const irs_i64& val)
  {
    *this = val;
  }
  string(const irs_u64& val)
  {
    *this = val;
  }

  template <class T>
  irs_bool to_number(T& val) const
  {
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    strm >> val;
    if (strm) return irs_true;
    else return irs_false;
  }
  irs_bool to_number(irs_u8& val) const
  {
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    int val_int = 0;
    strm >> val_int;
    val = static_cast<irs_u8>(val_int);
    if (strm) return irs_true;
    else return irs_false;
  }
  irs_bool to_number(irs_i8& val) const
  {
    auto_arr<char> buf(new char[size()]);
    copy(buf.get(), size());
    ::replace(buf.get(), buf.get() + size(), ',', '.');
    istrstream strm(buf.get(), size());
    int val_int = 0;
    strm >> val_int;
    val = static_cast<irs_u8>(val_int);
    if (strm) return irs_true;
    else return irs_false;
  }
};
inline ostream& operator<<(ostream& strm, const irs::string& strg)
{
  return strm << strg.c_str();
}
istream& operator>>(istream& strm, irs::string& strg);

// Переносимые манипуляторы
inline ostream& operator<<(ostream& strm, ios& (*f)(ios&, int))
{
  f(strm, 1);
  return strm;
}
inline ios &oct(ios &strm, int)
{
  strm.setf(ios::oct, ios::basefield);
  return strm;
}
inline ios &dec(ios &strm, int)
{
  strm.setf(ios::dec, ios::basefield);
  return strm;
}
inline ios &hex(ios &strm, int)
{
  strm.setf(ios::hex, ios::basefield);
  return strm;
}
inline ios &general(ios &strm, int)
{
  strm.setf(static_cast<ios::fmtflags>(0), ios::floatfield);
  return strm;
}
inline ios &fixed(ios &strm, int)
{
  strm.setf(ios::fixed, ios::floatfield);
  return strm;
}
inline ios &scientific(ios &strm, int)
{
  strm.setf(ios::scientific, ios::floatfield);
  return strm;
}
inline ios &left(ios &strm, int)
{
  strm.setf(ios::left, ios::adjustfield);
  return strm;
}
inline ios &right(ios &strm, int)
{
  strm.setf(ios::right, ios::adjustfield);
  return strm;
}
inline ios &internal(ios &strm, int)
{
  strm.setf(ios::internal, ios::adjustfield);
  return strm;
}
inline ios &showbase(ios &strm, int)
{
  strm.setf(ios::showbase);
  return strm;
}
inline ios &noshowbase(ios &strm, int)
{
  strm.unsetf(ios::showbase);
  return strm;
}
#ifdef NOP
inline ios &boolalpha(ios &strm, int)
{
  strm.setf(ios::boolalpha);
  return strm;
}
inline ios &noboolalpha(ios &strm, int)
{
  strm.unsetf(ios::boolalpha);
  return strm;
}
inline ios &showpoint(ios &strm, int)
{
  strm.setf(ios::showpoint);
  return strm;
}
inline ios &noshowpoint(ios &strm, int)
{
  strm.unsetf(ios::showpoint);
  return strm;
}
#endif //NOP
inline ios &showpos(ios &strm, int)
{
  strm.setf(ios::showpos);
  return strm;
}
inline ios &noshowpos(ios &strm, int)
{
  strm.unsetf(ios::showpos);
  return strm;
}
inline ios &skipws(ios &strm, int)
{
  strm.setf(ios::skipws);
  return strm;
}
inline ios &noskipws(ios &strm, int)
{
  strm.unsetf(ios::skipws);
  return strm;
}
inline ios &uppercase(ios &strm, int)
{
  strm.setf(ios::uppercase);
  return strm;
}
inline ios &nouppercase(ios &strm, int)
{
  strm.unsetf(ios::uppercase);
  return strm;
}

// Стандартная конфигурация консоли
class conio_cfg_t: public mx_proc_t
{
public:
  conio_cfg_t(mxkey_drv_t& a_key_drv, mxdisplay_drv_t& a_display_drv);
  virtual irs_bool tick();
  virtual void abort();
  irs_iostream &stream();
  mxkey_event_t &key_event();
private:
  mxkey_drv_t &m_key_drv;
  mxkey_event_gen_t m_key_event_gen;
  mxkey_event_t m_strm_key_event;
  mxkey_event_t m_app_key_event;
  mxdisplay_drv_t &m_display_drv;
  irs_iostream m_strm;
  irs_bool m_abort_request;
};

// Окно с графиком
class chart_window_t {
public:
  virtual ~chart_window_t() {}
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual rect_t position() const = 0;
  virtual void set_position(const rect_t &a_position) = 0;
  virtual void add_param(const irs::string &a_name) = 0;
  virtual void delete_param(const irs::string &a_name) = 0;
  virtual void clear_param() = 0;
  virtual void set_value(const irs::string &a_name, double a_value) = 0;
  virtual void set_value(const irs::string &a_name, double a_time,
    double a_value) = 0;
  virtual void set_time(double a_time) = 0;
  virtual void add() = 0;
  virtual void add(const irs::string &a_name, double a_time,
    double a_value) = 0;
  virtual void clear() = 0;
  virtual void resize(irs_u32 a_size) = 0;
  virtual irs_u32 size() const = 0;
  virtual void group_all() = 0;
  virtual void ungroup_all() = 0;
};

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

#endif //irsstdgH

