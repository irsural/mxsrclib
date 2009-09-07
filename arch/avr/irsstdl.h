// Стандартаная библиотека ИРС локальная часть
// Для AVR2561
// Дата: 16.02.2009

#ifndef irsstdlH
#define irsstdlH

#include <ioavr.h>

#include <irsdefs.h>
#include <irsstdg.h>
#include <timer.h>
#include <irscpp.h>
//#include <vector>
#include <irsarchint.h>

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

// Приведение IAR-complex в нормальный complex
template <class T>
class complex
{
};
class complex<double>;
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

// Инициализация AVR при старте
inline void irs_avr_init()
{
  //PINA = 0;
  //DDRA = 0;
  //PORTA = 0;
  //PINB = 0;
  //WDTCSR = 0;
  //UDR3 = 0;
}

#ifdef NOP
// Внешний интерфейс для прерывания AVR
extern irs_int_event_gen_t& irs_avr_int0_int;
extern irs_int_event_gen_t& irs_avr_int1_int;
extern irs_int_event_gen_t& irs_avr_int2_int;
extern irs_int_event_gen_t& irs_avr_int3_int;
extern irs_int_event_gen_t& irs_avr_int4_int;
extern irs_int_event_gen_t& irs_avr_int5_int;
extern irs_int_event_gen_t& irs_avr_int6_int;
extern irs_int_event_gen_t& irs_avr_int7_int;
extern irs_int_event_gen_t& irs_avr_pcint0_int;
extern irs_int_event_gen_t& irs_avr_pcint1_int;
extern irs_int_event_gen_t& irs_avr_wdt_int;
extern irs_int_event_gen_t& irs_avr_timer2_compa_int;
extern irs_int_event_gen_t& irs_avr_timer2_compb_int;
extern irs_int_event_gen_t& irs_avr_timer2_ovf_int;
extern irs_int_event_gen_t& irs_avr_timer1_capt_int;
extern irs_int_event_gen_t& irs_avr_timer1_compa_int;
extern irs_int_event_gen_t& irs_avr_timer1_compb_int;
extern irs_int_event_gen_t& irs_avr_timer1_compc_int;
extern irs_int_event_gen_t& irs_avr_timer1_ovf_int;
extern irs_int_event_gen_t& irs_avr_timer0_compa_int;
extern irs_int_event_gen_t& irs_avr_timer0_compb_int;
extern irs_int_event_gen_t& irs_avr_timer0_ovf_int;
extern irs_int_event_gen_t& irs_avr_spi_stc_int;
// Неправильный имя прерывания созданного ранее
extern irs_int_event_gen_t& irs_avr_spi_int;
extern irs_int_event_gen_t& irs_avr_usart0_rx_int;
extern irs_int_event_gen_t& irs_avr_usart0_udre_int;
extern irs_int_event_gen_t& irs_avr_usart0_tx_int;
extern irs_int_event_gen_t& irs_avr_analog_comp_int;
extern irs_int_event_gen_t& irs_avr_adc_int;
extern irs_int_event_gen_t& irs_avr_ee_ready_int;
extern irs_int_event_gen_t& irs_avr_timer3_capt_int;
extern irs_int_event_gen_t& irs_avr_timer3_compa_int;
extern irs_int_event_gen_t& irs_avr_timer3_compb_int;
extern irs_int_event_gen_t& irs_avr_timer3_compc_int;
extern irs_int_event_gen_t& irs_avr_timer3_ovf_int;
extern irs_int_event_gen_t& irs_avr_usart1_rx_int;
extern irs_int_event_gen_t& irs_avr_usart1_udre_int;
extern irs_int_event_gen_t& irs_avr_usart1_tx_int;
extern irs_int_event_gen_t& irs_avr_twi_int;
extern irs_int_event_gen_t& irs_avr_spm_ready_int;
extern irs_int_event_gen_t& irs_avr_timer4_compa_int;
extern irs_int_event_gen_t& irs_avr_timer4_compb_int;
extern irs_int_event_gen_t& irs_avr_timer4_compc_int;
extern irs_int_event_gen_t& irs_avr_timer4_ovf_int;
extern irs_int_event_gen_t& irs_avr_timer5_compa_int;
extern irs_int_event_gen_t& irs_avr_timer5_compb_int;
extern irs_int_event_gen_t& irs_avr_timer5_compc_int;
extern irs_int_event_gen_t& irs_avr_timer5_ovf_int;

extern irs_int_event_gen_t& irs_avr_timer1_compa_int;
extern irs_int_event_gen_t irs_avr_timer1_ovf_int;
extern irs_int_event_gen_t irs_avr_timer3_compa_int;
extern irs_int_event_gen_t irs_avr_timer3_ovf_int;
extern irs_int_event_gen_t irs_avr_spi_int;
extern irs_int_event_gen_t irs_avr_pcint0_int;
extern irs_int_event_gen_t irs_avr_analog_comp_int;

// Инициализация прерываний AVR
// Для правильной инициализации независимо от порядка инициализации модулей
namespace irs {
class avr_int_event_gen_init_t
{
public:
  typedef irs_u8 count_type;
  
  avr_int_event_gen_init_t();
  ~avr_int_event_gen_init_t();
private:
  static count_type m_count;
  static vector<irs_int_event_gen_t*> m_itps;
  
  static void itp_add(irs_int_event_gen_t* a_int_gen);
  static void itp_init(irs_int_event_gen_t* a_int_gen);
  static void itp_deinit(irs_int_event_gen_t* a_int_gen);
};
static avr_int_event_gen_init_t avr_int_event_gen_init;
} //namespace irs
#endif //NOP

namespace irs {

#ifdef NOP
// Конфигурация консоли для AVR
namespace arch_conio_cfg {
  conio_cfg_t& def();
} //namespace arch_conio_cfg
#endif //NOP

} //namespace irs

#endif //irsstdlH
