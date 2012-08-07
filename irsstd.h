// ������������ ���������� ���
// ����: 12.07.2010
// ������ ����: 25.11.2007

#ifndef irsstdH
#define irsstdH

// ����� �����
#define IRSSTDH_IDX 16

#include <irsdefs.h>

#ifdef __ICCAVR__
#include <ioavr.h>
#include <irsarchint.h>
#endif //__ICCAVR__
#ifdef __BORLANDC__
#include <winsock2.h>
#include <System.hpp>
#include <Forms.hpp>
#include <Classes.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#endif //__BORLANDC__
#ifdef __WATCOMC__
#include <windows.h>
#endif //__WATCOMC__

#include <string.h>

#include <irscpp.h>
#include <timer.h>
#include <irsint.h>
#include <irsstring.h>
#include <mxdatastd.h>
#include <irsconsolestd.h>
#include <irsstrmstd.h>
#include <irschartwin.h>

#if defined(__ICCAVR__) || defined(__ICCARM__)
#include <irsgpio.h>
#endif //__ICCAVR__ || __ICCARM__

#include <irsfinal.h>

//#include <vector>

// �������
// ���������� ������� ����� �������� �������
class mxcondrv_t
{
public:
  virtual ~mxcondrv_t() {}
  virtual void out(const char *str) = 0;
};

// ����� ��� ������ ���������� ����������
class mxconsole_t
{
private:
  mxcondrv_t *f_condrv;

  // ����������� �� ��������� ��������
  mxconsole_t();
public:
  // �����������
  mxconsole_t(mxcondrv_t *a_condrv);
  // ����������
  ~mxconsole_t();

  // ����� �� ������� �� ���� puts
  void puts(const char *a_msg);
  // ����� �� ������� �� ���� printf
  void printf(const char *a_format, ...);
};

#ifdef __embedded_cplusplus
// ���������� IAR-complex � ���������� complex
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

// �������
// ����� ��� ����������
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

// ����������������� ������ � ����������� ������
void *irs_renew(void *pointer, size_t old_size, size_t new_size);

// ������� ����� �������� ��� �������������� double � char*
void afloat_to_str(char *str, double N, size_t len, size_t accur);

extern mxapplication_t *mxapplication;

namespace irs {

// ����� �� ������ ����� � �������� ��������
template <class T>
T range(T val, T min_val, T max_val)
{
  return min(max(min_val, val), max_val);
}

} //namespace irs

#ifdef __ICCAVR__

struct irs_avr_port_map_t {
  //irs_avr_port_t name,
  p_avr_port_t set;
  p_avr_port_t get;
  p_avr_port_t dir;
};

//extern const irs_avr_port_map_t avr_port_map[];

namespace irs {

#define avr_port_map (irs::get_avr_port_map())
const irs_avr_port_map_t* get_avr_port_map();

} //namespace irs

// ����� �������� ���������� AVR
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

// ����� �������� ������� AVR
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

  //  ������ � ������� ������ LCD
  void lcd_IR_enable() {
    (*mp_set_control) &= ((1 << m_LCD_RS)^0xFF);
  }
  //  ������ � ������� ������ LCD
  void lcd_DR_enable() {
    (*mp_set_control) |= (1 << m_LCD_RS);
  }
  //  ����� LCD
  void lcd_enable() {
    (*mp_set_control) |= (1 << m_LCD_E);
  }
  //  LCD ���������� ��� ��������
  void lcd_disable() {
    (*mp_set_control) &= ((1 << m_LCD_E)^0xFF);
  }
  //  ����� �������/������ � ����
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

#if defined(__ICCAVR__) || defined(__ICCARM__)
class mxdisplay_drv_gen_t: public mxdisplay_drv_t
{
public:
  mxdisplay_drv_gen_t(irslcd_t a_type, irs::gpio_port_t& a_data_port,
    irs::gpio_pin_t& a_rs_pin, irs::gpio_pin_t& a_e_pin);
  ~mxdisplay_drv_gen_t();
  virtual mxdisp_pos_t get_height();
  virtual mxdisp_pos_t get_width();
  virtual void outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
    const char *text);
  virtual void tick();
private:
  enum {
    m_start_interval = 100,
    m_clear_interval = 10,
    m_trans_interval = 1,
    m_line_0 = 0,
    m_line_1 = 20,
    m_line_2 = 40,
    m_line_3 = 60,
    m_line_0_addr = 0x00,
    m_line_1_addr = 0x40,
    m_line_2_addr = 0x14,
    m_line_3_addr = 0x54
  };
  enum {
    m_com_null = 0x00,
    m_com_function_set = 0x3A,
    m_com_disp_on = 0x0C,
    m_com_disp_clear = 0x01,
    m_com_entry_mode_set = 0x06
  };
  enum lcd_status_t {
    lcd_init_wait,
    lcd_init,
    lcd_e_rise,
    lcd_data_out,
    lcd_e_fall,
    lcd_data_clear,
    lcd_begin
  };
  irs::gpio_port_t& m_data_port;
  irs::gpio_pin_t& m_rs_pin;
  irs::gpio_pin_t& m_e_pin;
  irs_u8 m_line_cnt;
  irs_u8 m_line_len;
  irs_u8 m_lcd_size;
  lcd_status_t m_status;
  lcd_status_t m_target_status;
  vector<irs_u8> m_ram_vector;
  irs_u8 m_current_symbol;
  irs_u8 m_current_symbol_address;
  irs_u8 m_lcd_init_counter;
  irs::timer_t m_timer;
  bool m_address_jump;
  irs_u8 m_command;
};
#endif  //  avr and arm

#ifdef __BORLANDC__
// ����� �������� ������� ��� ������� TStrings C++ Builder
class mxbuilder_condrv_t: public mxcondrv_t
{
  TStrings *f_console;
  mxbuilder_condrv_t();
public:
  mxbuilder_condrv_t(TStrings *Console);
  virtual void out(const char *str);
};

// ����� ��� ��������������� mainprg � ���������� �� C++ Builder
class mpc_builder_t: public TObject
{
  TTimer *m_timer;
  TCloseEvent m_SavedCloseEvent;
  TForm *m_form;
  mx_proc_t *m_app_proc;
  mxapplication_t *m_mxapplication;

  void __fastcall MainFormClose(System::TObject* Sender, TCloseAction &Action);
  void __fastcall TimerEvent(System::TObject* Sender);
  void __fastcall ProcClose(System::TObject* Sender, TCloseAction &Action);
  void __fastcall ProcTimer(System::TObject* Sender);
  void TimerInit(TNotifyEvent Event);
public:
  __fastcall mpc_builder_t();
  virtual __fastcall ~mpc_builder_t();
  void Connect(TForm *Form, void (*a_init)(), void (*a_deinit)(),
    void (*a_tick)(), void (*a_stop)(), irs_bool (*a_stopped)());
  void Connect(TForm *a_Form, mx_proc_t *a_app_proc);
};

// ����� �������� ���������� �++ Builder
class mxkey_drv_builder_t: public mxkey_drv_t
{
  void *keystate;
public:
  mxkey_drv_builder_t();
  ~mxkey_drv_builder_t();
  void connect(TMemo *a_display);
  virtual irskey_t operator()();
};

// ����� �������� ������� �++ Builder
class mxdisplay_drv_builder_t: public mxdisplay_drv_t
{
  TMemo *mp_display;
  TStrings *mp_console;
  mxdisp_pos_t m_height;
  mxdisp_pos_t m_width;
  mxdisp_pos_t m_length;
  char *mp_display_ram;
  irs_bool m_display_ram_changed;
  counter_t m_refresh_time;
  counter_t m_refresh_to;
public:
  mxdisplay_drv_builder_t(mxdisp_pos_t a_height, mxdisp_pos_t a_width);
  ~mxdisplay_drv_builder_t();
  virtual mxdisp_pos_t get_height();
  virtual mxdisp_pos_t get_width();
  virtual void outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
    const char *ap_text);
  virtual void tick();
  void connect(TMemo *ap_display);
  void set_refresh_time(counter_t a_refresh_time);
};

// ����� ������� ��� ������� Windows
class irs_win32_console_display_t: public mxdisplay_drv_t
{
private:
  DWORD m_Written;
  HANDLE m_InpHandle, m_OutHandle;  //����������� ����
  COORD m_Coord;                    // ���������� ������(x,y)
  //���������� ��� ��������  ���������� ������;
  CONSOLE_SCREEN_BUFFER_INFO m_BufInfo;
  //��� ��� �����, ����������� � ����� ������ ��������
  //(�������� ��� ������� ����-��������), ���������� ������ ���� �����
  //char* m_textbuf;
  int m_top_displey; //���������� Y �������� ��������� ������� ������� �� ������
public:
  irs_win32_console_display_t(); //����������� ��� ����������
  virtual mxdisp_pos_t get_height();
  virtual mxdisp_pos_t get_width();
  virtual void outtextpos(
    mxdisp_pos_t a_left,
    mxdisp_pos_t a_top,
    const char *text);
  virtual void tick();
  };

// ����� ������� ��� ����������
class irs_win32_console_key_drv_t: public mxkey_drv_t
{
private:
  irs_bool m_flag_event_KeyDown;
  irskey_t m_temporary_irs_key;         //������ ��� ������� �������
  DWORD m_Written, m_fdwMode, m_cNumRead;
  HANDLE  m_InpHandle;                  //���������� ����
  static const irs_u32 m_len_buf = 128;
  INPUT_RECORD m_irInBuf[m_len_buf];
public:
  irs_win32_console_key_drv_t();
  virtual irskey_t operator()();
};

namespace irs {

// ������������ ������� ��� C++ Builder
namespace arch_conio_cfg {
  conio_cfg_t& def();
  conio_cfg_t& memo(TMemo *ap_memo, mxdisp_pos_t a_height,
    mxdisp_pos_t a_width);
} //namespace arch_conio_cfg

#ifndef IRSSTRM_NEW_MEMOBUF
// ����� ����������� ������� ��� Memo
class memobuf: public streambuf
{
public:
  memobuf(const memobuf& a_buf);
  memobuf(TMemo *ap_memo = 0, int a_outbuf_size = 0);
  virtual ~memobuf();
  void connect(TMemo *ap_memo);
  virtual int overflow(int c = EOF);
  virtual int sync();
private:
  int m_outbuf_size;
  auto_arr<char> m_outbuf;
  TMemo *mp_memo;
};
#endif //IRSSTRM_NEW_MEMOBUF

} //namespace irs
#endif //__BORLANDC__

#if defined(__WATCOMC__)
// �������������� ����� double � ������ � %g �������
irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer);

//����� ������� ��� ������� Windows
class irs_win32_console_display_t: public mxdisplay_drv_t
{
private:
  DWORD m_Written;
  HANDLE m_InpHandle, m_OutHandle;        //����������� ����
  COORD m_Coord;                        // ���������� ������(x,y)
  //CHAR chFillChar;
  //WORD wColors[3], wColor;          //���� �����, � ������
  //���������� ��� ��������  ���������� ������;
  CONSOLE_SCREEN_BUFFER_INFO m_BufInfo;
  //��� ��� �����, ����������� � ����� ������ ��������
  //(�������� ��� ������� ����-��������), ���������� ������ ���� �����
  char* m_textbuf;
  int m_top_displey; //���������� Y �������� ��������� ������� ������� �� ������
public:
  irs_win32_console_display_t(); //����������� ��� ����������
  virtual mxdisp_pos_t get_height();
  virtual mxdisp_pos_t get_width();
  virtual void outtextpos(
    mxdisp_pos_t a_left,
    mxdisp_pos_t a_top,
    const char *text);
  virtual void tick();
};

//����� ������� ��� ����������
class irs_win32_console_key_drv_t: public mxkey_drv_t
{
private:
  irs_bool m_flag_event_KeyDown;
  irskey_t m_temporary_irs_key;              //������ ��� ������� �������
  DWORD m_Written, m_fdwMode, m_cNumRead;
  HANDLE  m_InpHandle;                  //���������� ����
  static const irs_u32 m_len_buf = 128;
  INPUT_RECORD m_irInBuf[m_len_buf];
public:
  irs_win32_console_key_drv_t();
  virtual irskey_t operator()();
};
#endif //defined(__WATCOMC__)

#ifdef NOP
#ifdef __WATCOMC__
namespace irs {

// ������������ ������� ��� Watcom C++
namespace arch_conio_cfg {
  conio_cfg_t& def();
} //namespace arch_conio_cfg

} //namespace irs
#endif //__WATCOMC__
#endif //NOP

#endif //irsstdH
