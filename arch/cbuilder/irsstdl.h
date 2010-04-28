// ������������ ���������� ��� ��������� �����
// ��� Borland C++ Builder
// ����: 14.04.2010
// ������ ����: 10.04.2009

#ifndef irsstdlH
#define irsstdlH

#include <irsdefs.h>

#include <winsock2.h>

#include <System.hpp>
#include <Forms.hpp>
#include <Classes.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>

#include <irsstdg.h>
#include <timer.h>
#include <irscpp.h>

#include <irsfinal.h>

// �������������� ����� double � ������ � %g �������
//irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer);

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

} //namespace irs

#endif //irsstdlH
