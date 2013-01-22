//! \file
//! \ingroup drivers_group
//! \brief Коммуникационные потоки для Borland C++ Builder
//!
//! Дата: 14.04.2010\n
//! Дата создания: 27.03.2009

#ifndef hardflowH
#define hardflowH

#include <irsdefs.h>

#include <winsock2.h>

#include <irscpp.h>
#include <hardflowg.h>
#include <irsstd.h>
#include <timer.h>
#include <irserror.h>

#include <irsfinal.h>

/* com port
Описание структуры DCB
typedef struct _DCB {
       DWORD DCBlength;            // sizeof(DCB)
       DWORD BaudRate;             // current baud rate
       DWORD fBinary:1;            // binary mode, no EOF check
       DWORD fParity:1;            // enable parity checking
       DWORD fOutxCtsFlow:1;       // CTS output flow control
       DWORD fOutxDsrFlow:1;       // DSR output flow control
       DWORD fDtrControl:2;        // DTR flow control type
       DWORD fDsrSensitivity:1;    // DSR sensitivity
       DWORD fTXContinueOnXoff:1;  // XOFF continues Tx
       DWORD fOutX:1;              // XON/XOFF out flow control
       DWORD fInX:1;               // XON/XOFF in flow control
       DWORD fErrorChar:1;         // enable error replacement
       DWORD fNull:1;              // enable null stripping
       DWORD fRtsControl:2;        // RTS flow control
       DWORD fAbortOnError:1;      // abort reads/writes on error
       DWORD fDummy2:17;           // reserved
       WORD  wReserved;            // not currently used
       WORD  XonLim;               // transmit XON threshold
       WORD  XoffLim;              // transmit XOFF threshold
       BYTE  ByteSize;             // number of bits/byte, 4-8
       BYTE  Parity;               // 0-4=no,odd,even,mark,space
       BYTE  StopBits;             // 0,1,2 = 1, 1.5, 2
       char  XonChar;              // Tx and Rx XON character
       char  XoffChar;             // Tx and Rx XOFF character
       char  ErrorChar;            // error replacement character
       char  EofChar;              // end of input character
       char  EvtChar;              // received event character
       WORD  wReserved1;           // reserved; do not use
   } DCB;
*/

namespace irs {

//! \addtogroup drivers_group
//! @{

struct com_param_t
{
  irs_u32 baud_rate;
  irs_u32 f_parity;
  irs_u32 f_outx_cts_flow;
  irs_u32 f_outx_dsr_flow;
  irs_u32 f_dtr_control;
  irs_u32 f_dsr_sensitivity;
  irs_u32 f_tx_continue_on_xoff;
  irs_u32 f_out_x;
  irs_u32 f_in_x;
  irs_u32 f_error_char;
  irs_u32 f_null;
  irs_u32 f_rts_control;
  irs_u32 f_abort_on_error;
  irs_u16 xon_lim;
  irs_u16 xoff_lim;
  irs_u8 byte_size;
  irs_u8 parity;
  irs_u8 stop_bits;
  irs_i8 xon_char;
  irs_i8 xoff_char;
  irs_i8 error_char;
  irs_i8 eof_char;
  irs_i8 evt_char;

  com_param_t():
    baud_rate(0),
    f_parity(0),
    f_outx_cts_flow(0),
    f_outx_dsr_flow(0),
    f_dtr_control(0),
    f_dsr_sensitivity(0),
    f_tx_continue_on_xoff(0),
    f_out_x(0),
    f_in_x(0),
    f_error_char(0),
    f_null(0),
    f_rts_control(0),
    f_abort_on_error(0),
    xon_lim(0),
    byte_size(0),
    parity(0),
    stop_bits(0),
    xon_char(0),
    xoff_char(0),
    error_char(0),
    eof_char(0),
    evt_char(0)
  {}
};
struct com_param_str_t
{
  typedef irs::string_t string_type;
  const string_type baud_rate;
  const string_type f_parity;
  const string_type f_outx_cts_flow;
  const string_type f_outx_dsr_flow;
  const string_type f_dtr_control;
  const string_type f_dsr_sensitivity;
  const string_type f_tx_continue_on_xoff;
  const string_type f_out_x;
  const string_type f_in_x;
  const string_type f_error_char;
  const string_type f_null;
  const string_type f_rts_control;
  const string_type f_abort_on_error;
  const string_type xon_lim;
  const string_type xoff_lim;
  const string_type byte_size;
  const string_type parity;
  const string_type stop_bits;
  const string_type xon_char;
  const string_type xoff_char;
  const string_type error_char;
  const string_type eof_char;
  const string_type evt_char;

  com_param_str_t():
    baud_rate(irst("baud_rate")),
    f_parity(irst("f_parity")),
    f_outx_cts_flow(irst("f_outx_cts_flow")),
    f_outx_dsr_flow(irst("f_outx_dsr_flow")),
    f_dtr_control(irst("f_dtr_control")),
    f_dsr_sensitivity(irst("f_dsr_sensitivity")),
    f_tx_continue_on_xoff(irst("f_tx_continue_on_xoff")),
    f_out_x(irst("f_out_x")),
    f_in_x(irst("f_in_x")),
    f_error_char(irst("f_error_char")),
    f_null(irst("f_null")),
    f_rts_control(irst("f_rts_control")),
    f_abort_on_error(irst("f_abort_on_error")),
    xon_lim(irst("xon_lim")),
    xoff_lim(irst("xoff_lim")),
    byte_size(irst("byte_size")),
    stop_bits(irst("stop_bits")),
    parity(irst("parity")),
    xon_char(irst("xon_char")),
    xoff_char(irst("xoff_char")),
    error_char(irst("error_char")),
    eof_char(irst("eof_char")),
    evt_char(irst("evt_char"))
  {}
};

class com_flow_t: public hardflow_t
{
private:
  typedef hardflow_t::size_type size_type;
  enum port_status_t{PS_PREFECT, PS_DEFECT} m_port_status;
  enum { m_channel_id = 1};
  const size_type m_max_size_write;
  HANDLE m_com;
  irs::error_trans_base_t *mp_error_trans;
  com_param_t m_com_param;
  // Освобождены ли ресурсы
  bool m_on_resource_free;
  const com_param_str_t m_com_param_str;
  void set_and_get_param_dbc();
  void get_param_dbc();
  void resource_free();
public:
  com_flow_t(
    const string_type& a_portname,
    const irs_u32 a_baud_rate = CBR_9600,
    const irs_u32 a_f_parity = FALSE,
    const irs_u8 a_parity = NOPARITY,
    const irs_u8 a_byte_size = 8,
    const irs_u8 a_stop_bits = ONESTOPBIT,
    const irs_u32 a_f_dtr_control = DTR_CONTROL_DISABLE,
    const irs_u32 a_f_rts_control = RTS_CONTROL_DISABLE);
  virtual ~com_flow_t();
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name, const string_type &a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
};

//! @}


class named_pipe_server_t: public hardflow_t
{
public:
  typedef string_t string_type;
private:
  const irs_uarc m_max_size_write;
  irs_u32 m_size_inbuf;
  irs_u32 m_size_outbuf;
  error_trans_base_t *mp_error_trans;
  string_type m_name;
  irs_u32 m_count_instances;
  struct instance_named_pipe_t
  {
    HANDLE handle;
    OVERLAPPED ovl;
    irs_u8* p_inbuf;
    irs_u8* p_outbuf;
    irs_u32 count_inf_bytes_inbuf;
    irs_u32 count_inf_bytes_outbuf;
  };
  std::vector<instance_named_pipe_t> mv_named_pipe;
  named_pipe_server_t();
  counter_t m_delay_control;
  loop_timer_t m_control_timer;

public:
  named_pipe_server_t(
    const string_type& a_name,
    const irs_u32 a_count_instances = 1);
  virtual ~named_pipe_server_t();
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual irs_uarc read(irs_uarc &a_channel_ident, irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual irs_uarc write(irs_uarc a_channel_ident, const irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual void tick();
private:
  void add_named_pipe(const irs_u32 a_count);

};
class named_pipe_client_t: public hardflow_t
{
public:
  typedef string_t string_type;
private:
  const irs_uarc m_max_size_write;
  error_trans_base_t *mp_error_trans;
  string_type m_name;
  HANDLE m_handle_named_pipe;
  OVERLAPPED m_ovl;
  named_pipe_client_t();
  counter_t m_delay_control;
  loop_timer_t m_control_timer;
  bool m_connected;

public:
  named_pipe_client_t(const string_type& a_name);
  virtual ~named_pipe_client_t();
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual irs_uarc read(irs_uarc &a_channel_ident, irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual irs_uarc write(irs_uarc a_channel_ident, const irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual void tick();
private:
};

} // namespace irs

#endif //hardflowH
