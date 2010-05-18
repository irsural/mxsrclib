// Стандартаная библиотека ИРС
// Дата: 18.05.2010
// Ранняя дата: 25.11.2007

#ifndef irsstdH
#define irsstdH

// Номер файла
#define IRSSTDH_IDX 16

#include <irsdefs.h>

#ifdef __ICCAVR__
#include <ioavr.h>
#endif //__ICCAVR__

#include <string.h>

//#include <irsstdl.h>
#include <irscpp.h>
#include <timer.h>
#include <irsint.h>
#include <irsstring.h>
#include <mxdatastd.h>
#include <irsconsolestd.h>
#include <irsstrmstd.h>
#include <irschartwin.h>
#include <irscpp.h>
#include <irscpp.h>
#include <irsarchint.h>

#include <irsfinal.h>

//#include <vector>

// Устарел
// Абстактный базовый класс драйвера консоли
class mxcondrv_t
{
public:
  virtual ~mxcondrv_t() {}
  virtual void out(const char *str) = 0;
};

// Класс для вывода отладочной информации
class mxconsole_t
{
private:
  mxcondrv_t *f_condrv;

  // Конструктор по умолчанию запрещен
  mxconsole_t();
public:
  // Конструктор
  mxconsole_t(mxcondrv_t *a_condrv);
  // Деструктор
  ~mxconsole_t();

  // Вывод на консоль по типу puts
  void puts(const char *a_msg);
  // Вывод на консоль по типу printf
  void printf(const char *a_format, ...);
};

#ifdef __embedded_cplusplus
// Приведение IAR-complex в нормальный complex
template <class T>
class complex
{
};
template<> class complex<double>;
template<>
class complex<float>: public float_complex
{
  typedef float T;
  typedef float_complex base;
public:
  complex(const T& a_real = 0, const T& a_imag = 0):
    base(a_real, a_imag)
  {
  }
  complex(const base& a_val):
    base(a_val)
  {
  }
  explicit complex(const complex<double>& a_val):
    base(reinterpret_cast<const double_complex&>(a_val))
  {
  }
  complex<T>& operator=(const T& a_val)
  {
    static_cast<base&>(*this) = a_val;
    return *this;
  }
};
template<>
class complex<double>: public double_complex
{
  typedef double T;
  typedef double_complex base;
public:
  complex(const T& a_real = 0, const T& a_imag = 0):
    base(a_real, a_imag)
  {
  }
  complex(const base& a_val):
    base(a_val)
  {
  }
  complex(const complex<float>& a_val):
    base(static_cast<const float_complex&>(a_val))
  {
  }
  complex<T>& operator=(const T& a_val)
  {
    static_cast<base&>(*this) = a_val;
    return *this;
  }
};
#endif //__embedded_cplusplus

// Устарел
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

// Перераспределение памяти с сохранением данных
void *irs_renew(void *pointer, size_t old_size, size_t new_size);

// Функция Макса Полякова для преобразования double в char*
void afloat_to_str(char *str, double N, size_t len, size_t accur);

extern mxapplication_t *mxapplication;

namespace irs {

// Число на выходе будет в заданных границах
template <class T>
T range(T val, T min_val, T max_val)
{
  return min(max(min_val, val), max_val);
}

} //namespace irs

#ifdef __ICCAVR__
// Указатель на порт AVR
typedef irs_u8 volatile __tiny avr_port_t;
typedef avr_port_t* p_avr_port_t;

typedef struct _irs_avr_port_map_t {
  //irs_avr_port_t name,
  p_avr_port_t set;
  p_avr_port_t get;
  p_avr_port_t dir;
} irs_avr_port_map_t;

extern const irs_avr_port_map_t avr_port_map[];

// Класс драйвера клавиатуры AVR
class mxkey_drv_avr_t: public mxkey_drv_t
{
  irs_u8 m_high_set_bit;
  irs_u8 m_high_get_bit;
  p_avr_port_t mp_set;
  p_avr_port_t mp_get;
  p_avr_port_t mp_dir;
  irskey_t __flash *mp_key_map;

  inline void sethor(irs_u8 num);
  inline void reshor(irs_u8 num);
  inline irs_u8 chkver(irs_u8 num);
public:
  mxkey_drv_avr_t(irskbd_t type, irskbd_map_t map_type, irs_avr_port_t a_port);
  ~mxkey_drv_avr_t();
  virtual irskey_t operator()();
};

inline void mxkey_drv_avr_t::sethor(irs_u8 num)
{
  (*mp_set) |= (1 << (m_high_set_bit - num));
}
inline void mxkey_drv_avr_t::reshor(irs_u8 num)
{
  (*mp_set) &= (0xFF^(1 << (m_high_set_bit - num)));
}
inline irs_u8 mxkey_drv_avr_t::chkver(irs_u8 num)
{
  return (*mp_get)&(1 << (m_high_get_bit - num));
}

// Класс драйвера дисплея AVR
class mxdisplay_drv_avr_t: public mxdisplay_drv_t
{
  typedef enum _lcd_status_t {
    LCD_BEGIN,
    LCD_RS,
    LCD_DATA_OUT,
    LCD_ADDRESS_OUT,
    LCD_E_RISE,
    LCD_E_FALL,
    LCD_FREE
  } lcd_status_t;

  p_avr_port_t mp_set_data;
  p_avr_port_t mp_get_data;
  p_avr_port_t mp_dir_data;
  p_avr_port_t mp_set_control;
  p_avr_port_t mp_get_control;
  p_avr_port_t mp_dir_control;
  irs_u8 m_LCD_E;
  irs_u8 m_LCD_RS;
  irs_u8 LINE_COUNT;
  irs_u8 LINE_LEN;
  irs_u8 LCD_SIZE;
  lcd_status_t lcd_status;
  irs_u8 *lcd_ram;
  irs_u8 lcd_current_symbol;
  irs_u8 lcd_current_symbol_address;
  irs_u8 lcd_init_counter;
  counter_t lcd_timer;
  irs_bool lcd_address_jump;

  //  Запись в регистр команд LCD
  void lcd_IR_enable() {
    (*mp_set_control) &= ((1 << m_LCD_RS)^0xFF);
  }
  //  Запись в регистр данных LCD
  void lcd_DR_enable() {
    (*mp_set_control) |= (1 << m_LCD_RS);
  }
  //  Выбор LCD
  void lcd_enable() {
    (*mp_set_control) |= (1 << m_LCD_E);
  }
  //  LCD недоступен для операций
  void lcd_disable() {
    (*mp_set_control) &= ((1 << m_LCD_E)^0xFF);
  }
  //  Вывод команды/данных в порт
  void lcd_out(irs_u8 data) {
    (*mp_set_data) = data;
  }
public:
  mxdisplay_drv_avr_t(irslcd_t a_type, irs_avr_port_t a_data_port,
    irs_avr_port_t a_control_port, irs_u8 a_control_pin);
  ~mxdisplay_drv_avr_t();
  virtual mxdisp_pos_t get_height();
  virtual mxdisp_pos_t get_width();
  virtual void outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
    const char *text);
  virtual void tick();
};
#endif //__ICCAVR__

#endif //irsstdH
