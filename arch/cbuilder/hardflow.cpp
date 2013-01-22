//! \file
//! \ingroup in_out_group
//! \brief Коммуникационные потоки для Borland C++ Builder
//!
//! Дата: 14.04.2010\n
//! Дата создания: 6.08.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <mem.h>

#include <hardflow.h>

#include <irsfinal.h>

//#define IRS_HARDFLOW_LOG_COM_PORT

// class com_flow_t
irs::com_flow_t::com_flow_t(
  const string_type& a_portname,
  const irs_u32 a_baud_rate,      //CBR_9600
  const irs_u32 a_f_parity,       //NOPARITY
  const irs_u8 a_parity,
  const irs_u8 a_byte_size,      //8
  const irs_u8 a_stop_bits,
  const irs_u32 a_f_dtr_control): //ONESTOPBIT
  m_max_size_write(256),
  m_com(IRS_NULL),
  mp_error_trans(irs::error_trans()),
  m_com_param(),
  m_port_status(PS_PREFECT),
  m_on_resource_free(false)
{                                             
  BOOL fsuccess = TRUE;
  COMMTIMEOUTS time_outs;
  m_com = CreateFile(
    (irst("\\\\.\\") + a_portname).c_str(),
    GENERIC_READ|GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);
  if (m_com == INVALID_HANDLE_VALUE) {
    fsuccess = FALSE;
    IRS_LIB_ERROR(ec_standard,
      ("Ошибка открытия COM-порта. " + last_error_str()).c_str());
  }
  if (fsuccess) {
    // Настраиваем
    // устанавливаем размер внутренних буферов приема-передачи в драйвере порта
    fsuccess = SetupComm(m_com,m_max_size_write*2, m_max_size_write*2);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR();}
  }
  if (fsuccess) {
    get_param_dbc();
    m_com_param.baud_rate = a_baud_rate;
    m_com_param.f_parity = a_f_parity;
    m_com_param.parity = a_parity;
    m_com_param.byte_size = a_byte_size;
    m_com_param.stop_bits = a_stop_bits;
    m_com_param.f_dtr_control = a_f_dtr_control;

    set_and_get_param_dbc();
    fsuccess = GetCommTimeouts(m_com,&time_outs);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if (fsuccess) {
    time_outs.ReadIntervalTimeout = 10;
    time_outs.ReadTotalTimeoutMultiplier = 0;
    time_outs.ReadTotalTimeoutConstant = 10;
    time_outs.WriteTotalTimeoutMultiplier = 0;
    time_outs.WriteTotalTimeoutConstant = 10;
    fsuccess = SetCommTimeouts(m_com,&time_outs);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if (fsuccess) {
    // Сброс порта
    fsuccess = PurgeComm(
      m_com, PURGE_RXABORT|PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if (!fsuccess) {
    m_port_status = PS_DEFECT;
    resource_free();
  }
}

irs::com_flow_t::~com_flow_t()
{
  resource_free();
}

irs::com_flow_t::string_type irs::com_flow_t::param(const string_type &a_name)
{
  string_type param_value_str = irst("");
  if(m_port_status == PS_PREFECT){
    if (a_name == m_com_param_str.baud_rate) {
      param_value_str = m_com_param.baud_rate;
    } else if(a_name == m_com_param_str.f_parity) {
      param_value_str = m_com_param.f_parity ;
    } else if(a_name == m_com_param_str.f_outx_cts_flow) {
      param_value_str = m_com_param.f_outx_cts_flow;
    } else if(a_name == m_com_param_str.f_outx_dsr_flow) {
      param_value_str = m_com_param.f_outx_dsr_flow;
    } else if(a_name == m_com_param_str.f_dtr_control) {
      param_value_str = m_com_param.f_dtr_control;
    } else if(a_name == m_com_param_str.f_dsr_sensitivity) {
      param_value_str = m_com_param.f_dsr_sensitivity;
    } else if(a_name == m_com_param_str.f_tx_continue_on_xoff) {
      param_value_str = m_com_param.f_tx_continue_on_xoff;
    } else if(a_name == m_com_param_str.f_out_x) {
      param_value_str = m_com_param.f_out_x;
    } else if(a_name == m_com_param_str.f_in_x) {
      param_value_str = m_com_param.f_in_x;
    } else if(a_name == m_com_param_str.f_error_char) {
      param_value_str = m_com_param.f_error_char;
    } else if(a_name == m_com_param_str.f_null) {
      param_value_str = m_com_param.f_null;
    } else if (a_name == m_com_param_str.f_rts_control) {
      param_value_str = m_com_param.f_rts_control;
    } else if(a_name == m_com_param_str.f_abort_on_error) {
      param_value_str = m_com_param.f_abort_on_error;
    } else if(a_name == m_com_param_str.xon_lim) {
      param_value_str = m_com_param.xon_lim;
    } else if(a_name == m_com_param_str.xoff_lim) {
      param_value_str = m_com_param.xoff_lim;
    } else if(a_name == m_com_param_str.byte_size) {
      param_value_str = m_com_param.byte_size;
    } else if(a_name == m_com_param_str.parity) {
      param_value_str = m_com_param.parity;
    } else if(a_name == m_com_param_str.stop_bits) {
      param_value_str = m_com_param.stop_bits;
    } else if(a_name == m_com_param_str.xon_char) {
      param_value_str = m_com_param.xon_char;
    } else if(a_name == m_com_param_str.xoff_char) {
      param_value_str = m_com_param.xoff_char;
    } else if(a_name == m_com_param_str.error_char) {
      param_value_str = m_com_param.error_char;
    } else if(a_name == m_com_param_str.eof_char) {
      param_value_str = m_com_param.eof_char;
    } else if(a_name == m_com_param_str.evt_char) {
      param_value_str = m_com_param.evt_char;
    }
  }
  return param_value_str;
}

void irs::com_flow_t::set_param(const string_type &a_name,
  const string_type &a_value)
{
  bool fsuccess = true;
  if (m_port_status == PS_PREFECT) {
    irs_u32 value = 0;
    fsuccess = a_value.to_number(value);
    if (a_name == m_com_param_str.baud_rate) {
      if (fsuccess) {
        m_com_param.baud_rate = value;
      }
    } else if(a_name == m_com_param_str.f_parity) {
      if (fsuccess) {
        m_com_param.f_parity = value;
      }
    } else if(a_name == m_com_param_str.f_outx_cts_flow) {
      if(fsuccess){m_com_param.f_outx_cts_flow = value;}
    } else if(a_name == m_com_param_str.f_outx_dsr_flow) {
      if (fsuccess) {
        m_com_param.f_outx_dsr_flow = value;
      }
    } else if (a_name == m_com_param_str.f_dtr_control) {
      if (fsuccess) {
        m_com_param.f_dtr_control = value;
      }
    } else if (a_name == m_com_param_str.f_dsr_sensitivity) {
      if (fsuccess) {
        m_com_param.f_dsr_sensitivity = value;
      }
    } else if (a_name == m_com_param_str.f_tx_continue_on_xoff) {
      if (fsuccess) {
        m_com_param.f_tx_continue_on_xoff = value;
      }
    } else if (a_name == m_com_param_str.f_out_x) {
      if (fsuccess) {
        m_com_param.f_out_x = value;
      }
    } else if (a_name == m_com_param_str.f_in_x) {
      if (fsuccess) { m_com_param.f_in_x = value;
      }
    } else if (a_name == m_com_param_str.f_error_char) {
      if (fsuccess) {
        m_com_param.f_error_char = value;
      }
    } else if (a_name == m_com_param_str.f_null) {
      if (fsuccess) {
        m_com_param.f_null = value;
      }
    } else if (a_name == m_com_param_str.f_rts_control) {
      if (fsuccess) {
        m_com_param.f_rts_control = value;
      }
    } else if (a_name == m_com_param_str.f_abort_on_error) {
      if (fsuccess) {
        m_com_param.f_abort_on_error = value; }
    } else if (a_name == m_com_param_str.xon_lim) {
      if (fsuccess) {
        m_com_param.xon_lim = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.xoff_lim) {
      if (fsuccess) {
        m_com_param.xoff_lim = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.byte_size) {
      if (fsuccess) {
        m_com_param.byte_size = static_cast<irs_u8>(value);
      }
    } else if (a_name == m_com_param_str.parity) {
      if (fsuccess) {
        m_com_param.parity = static_cast<irs_u8>(value);
      }
    } else if (a_name == m_com_param_str.stop_bits) {
      if (fsuccess) {
        m_com_param.stop_bits = static_cast<irs_u8>(value);
      }
    } else if (a_name == m_com_param_str.xon_char) {
      if (fsuccess) {
        m_com_param.xon_char = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.xoff_char) {
      if (fsuccess) {
        m_com_param.xoff_char = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.error_char) {
      if (fsuccess) {
        m_com_param.error_char = static_cast<irs_i8>(value);
      }
    } else if(a_name == m_com_param_str.eof_char) {
      if (fsuccess) {
        m_com_param.eof_char = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.evt_char) {
      if (fsuccess) {
        m_com_param.evt_char = static_cast<irs_i8>(value);
      }
    }
    set_and_get_param_dbc();
  }
  IRS_ASSERT(fsuccess);
}

#ifdef IRS_HARDFLOW_LOG_COM_PORT
namespace irs {
void com_flow_log(const irs_u8* ap_buf, size_t a_size, DWORD result)
{
  irs_string_t buf_str(reinterpret_cast<const char*>(ap_buf), a_size);
  buf_str = conv_notprintable_to_hex(buf_str);
  stringstream slog;
  slog << "com_flow_t::write(\"" << buf_str << "\", " << a_size;
  slog << ") == " << result;
  slog << endl;
  static irs_string_t prev_log_line = "";
  static int counter = 0;
  fstream flog("S:\\Files\\ASUTP\\U309M\\Основное\\log.txt",
    ios::app | ios::out);
  if (slog.str() != prev_log_line) {
    if (counter) {
      flog << "Предыдущая строка повторялась ";
      flog << counter << " раз" << endl;
      counter = 0;
    }
    flog << slog.str();
  } else {
    counter++;
  }
  prev_log_line = slog.str();
}
} //namespace irs
#endif //IRS_HARDFLOW_LOG_COM_PORT

irs::com_flow_t::size_type irs::com_flow_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  DWORD fsuccess = 1;
  if (a_channel_ident != m_channel_id) {
    fsuccess = 0;
  } else {
    // Указан существующий канал
  }
  COMSTAT com_stat;
  memset((void*)&com_stat, 0, sizeof(COMSTAT));
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  }
  if (fsuccess) {
    DWORD errors = 0;
    fsuccess = ClearCommError(m_com, &errors, &com_stat);
    if (!fsuccess) { IRS_LIB_SEND_LAST_ERROR(); return 0;}
  }
  size_type size_rd = 0;
  if (fsuccess) {
    irs_uarc num_bytes_buf_rd = com_stat.cbInQue;
    if(num_bytes_buf_rd > 0){
      DWORD num_of_bytes_read = 0;
      size_rd = min(a_size, num_bytes_buf_rd);
      fsuccess = ReadFile(m_com, ap_buf, size_rd, &num_of_bytes_read, NULL);

      #ifdef IRS_HARDFLOW_LOG_COM_PORT
      com_flow_log(ap_buf, num_of_bytes_read, fsuccess);
      #endif //IRS_HARDFLOW_LOG_COM_PORT

      if (fsuccess) {
        size_rd = num_of_bytes_read;
      } else {
        size_rd = 0;
        IRS_LIB_SEND_LAST_ERROR();
      }
    }
  } else {
    m_port_status = PS_DEFECT;
    resource_free();
  }
  return size_rd;
}

irs::com_flow_t::size_type irs::com_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  DWORD fsuccess = 1;
  if (a_channel_ident != m_channel_id) {
    fsuccess = 0;
  } else {
    // Указан существующий канал
  }
  COMSTAT com_stat;
  memset((void*)&com_stat, 0, sizeof(COMSTAT));
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  }
  if (fsuccess) {
    DWORD errors = 0;
    fsuccess = ClearCommError(m_com, &errors, &com_stat);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR(); return 0;}
  }
  irs_u32 size_wr = 0;
  if(fsuccess){
    irs_uarc num_bytes_buf_wr = com_stat.cbOutQue;
    if(num_bytes_buf_wr == 0){
      DWORD num_of_bytes_written = 0;
      size_wr = min(a_size, m_max_size_write);
      fsuccess = WriteFile(m_com, ap_buf, size_wr,
        &num_of_bytes_written, NULL);

      #ifdef IRS_HARDFLOW_LOG_COM_PORT
      com_flow_log(ap_buf, num_of_bytes_written, fsuccess);
      #endif //IRS_HARDFLOW_LOG_COM_PORT

      if (fsuccess) {
        size_wr = num_of_bytes_written;
      } else {
        size_wr = 0;
        IRS_LIB_SEND_LAST_ERROR();
      }
    }
  }else{
    m_port_status = PS_DEFECT;
    resource_free();
  }
  return size_wr;
}

irs::com_flow_t::size_type irs::com_flow_t::channel_next()
{
  return m_channel_id;
}

bool irs::com_flow_t::is_channel_exists(size_type a_channel_ident)
{
  return (m_channel_id == a_channel_ident);
}

void irs::com_flow_t::tick()
{}

void irs::com_flow_t::set_and_get_param_dbc()
{
  DWORD fsuccess = 1;
  DCB dcb;
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  } else {
    fsuccess = GetCommState(m_com, &dcb);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if(fsuccess){
    dcb.BaudRate = m_com_param.baud_rate;
    dcb.fParity = m_com_param.f_parity;
    dcb.fOutxCtsFlow = m_com_param.f_outx_cts_flow;
    dcb.fOutxDsrFlow = m_com_param.f_outx_dsr_flow;
    dcb.fDtrControl = m_com_param.f_dtr_control;
    dcb.fDsrSensitivity = m_com_param.f_dsr_sensitivity;
    dcb.fTXContinueOnXoff = m_com_param.f_tx_continue_on_xoff;
    dcb.fOutX = m_com_param.f_out_x;
    dcb.fInX = m_com_param.f_in_x;
    dcb.fErrorChar = m_com_param.f_error_char;
    dcb.fNull = m_com_param.f_null;
    dcb.fRtsControl = m_com_param.f_rts_control;
    dcb.fAbortOnError = m_com_param.f_abort_on_error;
    dcb.XonLim = m_com_param.xon_lim;
    dcb.XoffLim = m_com_param.xoff_lim;
    dcb.ByteSize = m_com_param.byte_size;
    dcb.Parity = m_com_param.parity;
    dcb.StopBits = m_com_param.stop_bits;
    dcb.XonChar = m_com_param.xon_char;
    dcb.XoffChar = m_com_param.xoff_char;
    dcb.ErrorChar = m_com_param.error_char;
    dcb.EofChar = m_com_param.eof_char;
    dcb.EvtChar = m_com_param.evt_char;
    fsuccess = SetCommState(m_com, &dcb);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
      return;
    }
    get_param_dbc();
  } else {
    m_port_status = PS_DEFECT;
    resource_free();
  }
}
void irs::com_flow_t::get_param_dbc()
{
  DWORD fsuccess = 0;
  DCB dcb;
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  } else {
    fsuccess = GetCommState(m_com, &dcb);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR();}
  }
  if (fsuccess) {
    m_com_param.baud_rate = dcb.BaudRate;
    m_com_param.f_parity = dcb.fParity;
    m_com_param.f_outx_cts_flow = dcb.fOutxCtsFlow;
    m_com_param.f_outx_dsr_flow = dcb.fOutxDsrFlow;
    m_com_param.f_dtr_control = dcb.fDtrControl;
    m_com_param.f_dsr_sensitivity = dcb.fDsrSensitivity;
    m_com_param.f_tx_continue_on_xoff = dcb.fTXContinueOnXoff;
    m_com_param.f_out_x = dcb.fOutX;
    m_com_param.f_in_x = dcb.fInX;
    m_com_param.f_error_char = dcb.fErrorChar;
    m_com_param.f_null = dcb.fNull;
    m_com_param.f_rts_control = dcb.fRtsControl;
    m_com_param.f_abort_on_error = dcb.fAbortOnError;
    m_com_param.xon_lim = dcb.XonLim;
    m_com_param.xoff_lim = dcb.XoffLim;
    m_com_param.byte_size = dcb.ByteSize;
    m_com_param.parity = dcb.Parity;
    m_com_param.stop_bits = dcb.StopBits;
    m_com_param.xon_char = dcb.XonChar;
    m_com_param.xoff_char = dcb.XoffChar;
    m_com_param.error_char = dcb.ErrorChar;
    m_com_param.eof_char = dcb.EofChar;
    m_com_param.evt_char = dcb.EvtChar;

    //fsuccess = SetCommState(m_com, &dcb);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  } else {
    m_port_status = PS_DEFECT;
    resource_free();
  }
}
void irs::com_flow_t::resource_free()
{
  if (!m_on_resource_free) {
    m_on_resource_free = true;
    PurgeComm(m_com, PURGE_RXABORT|PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    CloseHandle(m_com);
  }
}


//class named_pipe_t
irs::named_pipe_server_t::named_pipe_server_t(
  const string_type& a_name,
  const irs_u32 a_count_instances
):
  m_max_size_write(65535),
  m_size_inbuf(65535),
  m_size_outbuf(65535),
  mp_error_trans(irs::error_trans()),
  m_name(irst("\\\\.\\pipe\\") + a_name),
  m_count_instances(a_count_instances),
  mv_named_pipe(),
  m_delay_control(make_cnt_ms(3000)),
  m_control_timer(m_delay_control)
{
  m_control_timer.start();
  BOOL fsuccess = TRUE;
  if (m_count_instances == 0){
    fsuccess = FALSE;
    IRS_FATAL_ERROR("Неверное значение входного параметра");
  }
  if(fsuccess){
    add_named_pipe(m_count_instances);
  }
}

irs::named_pipe_server_t::~named_pipe_server_t()
{
  irs_u32 count = mv_named_pipe.size();
  for (irs_u32 i = 0; i < count; i++){
    HANDLE handle = mv_named_pipe[i].handle;
    delete [] mv_named_pipe[i].p_inbuf;
    delete [] mv_named_pipe[i].p_outbuf;
    CloseHandle(handle);
  }
}
irs::named_pipe_server_t::string_type
irs::named_pipe_server_t::param(const string_type &a_name)
{
  return string_type();
}
void irs::named_pipe_server_t::set_param(
  const string_type &a_name, const string_type &a_value)
{
}
irs_uarc irs::named_pipe_server_t::read(
  irs_uarc &a_channel_ident, irs_u8 *ap_buf, irs_uarc a_size)
{
  irs_u32 size_rd = 0;
  if (a_channel_ident >= m_count_instances){
    IRS_FATAL_ERROR("Неверное значение входного параметра");
  }else if(a_channel_ident < mv_named_pipe.size()){
    irs_uarc size_bytes_not_free =
      mv_named_pipe[a_channel_ident].count_inf_bytes_inbuf;
    size_rd = min(a_size, size_bytes_not_free);
    irs_u8* p_inbuf = mv_named_pipe[a_channel_ident].p_inbuf;
    memcpy(ap_buf, p_inbuf, size_rd);
    size_bytes_not_free -= size_rd;
    memcpy(p_inbuf, p_inbuf+size_bytes_not_free, size_rd);
    mv_named_pipe[a_channel_ident].count_inf_bytes_inbuf = size_bytes_not_free;
    ////


    HANDLE handle_named_pipe = mv_named_pipe[a_channel_ident].handle;
    OVERLAPPED* p_ovl = &(mv_named_pipe[a_channel_ident].ovl);

    if (!ReadFile(handle_named_pipe, ap_buf,  a_size, &size_rd, p_ovl))
    {
      DWORD dw = GetLastError();
      IRS_LIB_SEND_LAST_ERROR();
      if (dw == 109){
        DisconnectNamedPipe(handle_named_pipe);
        ConnectNamedPipe(handle_named_pipe, p_ovl);
      }
    }
  }
  return size_rd;
}
irs_uarc irs::named_pipe_server_t::write(
  irs_uarc a_channel_ident, const irs_u8 *ap_buf, irs_uarc a_size)
{
  irs_u32 size_wr = 0;
  if (a_channel_ident >= m_count_instances){
    IRS_FATAL_ERROR("Неверное значение входного параметра");
  }else{
    irs_uarc size_bytes_not_free =
      mv_named_pipe[a_channel_ident].count_inf_bytes_outbuf;
    irs_uarc size_bytes_free =
      m_size_outbuf - size_bytes_not_free;
    size_wr = min(a_size, size_bytes_free);
    irs_u8* p_input =  mv_named_pipe[a_channel_ident].p_outbuf +
      size_bytes_not_free;
    memcpy(p_input, ap_buf, size_wr);
    mv_named_pipe[a_channel_ident].count_inf_bytes_outbuf += size_wr;
  }

  if (a_channel_ident >= m_count_instances){
    IRS_FATAL_ERROR("Неверное значение входного параметра");
  }else if (a_channel_ident < mv_named_pipe.size()){
    HANDLE handl_named_pipe = mv_named_pipe[a_channel_ident].handle;
    OVERLAPPED* p_ovl = &(mv_named_pipe[a_channel_ident].ovl);
    irs_u32 size = min(a_size, m_max_size_write);
    irs_u32 dw_bytes_writte = 0;
    if (!WriteFile(handl_named_pipe, ap_buf, size, &dw_bytes_writte, p_ovl))
    {
      DWORD dw = GetLastError();
      if (dw != 109){
        DisconnectNamedPipe(handl_named_pipe);
        ConnectNamedPipe(handl_named_pipe, p_ovl);
      }
    }else{
      size_wr = a_size;
    }
  }
  return size_wr;
}
void irs::named_pipe_server_t::tick()
{
  #ifdef NOP
  for (irs_u32 i = 0; i < m_count_instances; i++){
    HANDLE handl_named_pipe = mv_named_pipe[i].handle;
    OVERLAPPED* p_ovl = &(mv_named_pipe[i].ovl);
    /*irs_u32 size = min(a_size, m_max_size_write);
    irs_u32 dw_bytes_writte = 0;
    if (!WriteFile(handl_named_pipe, ap_buf, size, &dw_bytes_writte, p_ovl))
    {
      DWORD dw = GetLastError();
      if (dw != 109){
        DisconnectNamedPipe(handl_named_pipe);
        ConnectNamedPipe(handl_named_pipe, p_ovl);
      }
    }else{
      size_wr = a_size;
    }
      */
  }
  if (m_control_timer.check()){
    irs_u32 count_named_pipe = mv_named_pipe.size();
    if (count_named_pipe < m_count_instances){
      irs_u32 need_count_named_pipe = count_named_pipe;
      add_named_pipe(need_count_named_pipe);
    }
  }
  #endif //NOP
}

void irs::named_pipe_server_t::add_named_pipe(const irs_u32 a_count)
{
  irs_u32 outbuf_size = m_max_size_write*2;
  irs_u32 inbuf_size = m_max_size_write*2;
  for (irs_u32 i = 0; i < a_count; i++){
    HANDLE handle_named_pipe = CreateNamedPipe(
      m_name.c_str(),
      PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
      PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT,
      m_count_instances+5,
      outbuf_size,
      inbuf_size,
      0,
      NULL);
    if (handle_named_pipe == INVALID_HANDLE_VALUE){
      IRS_LIB_SEND_LAST_ERROR();
      break;
    }else{
      instance_named_pipe_t instance_named_pipe;
      instance_named_pipe.handle = handle_named_pipe;
      instance_named_pipe.ovl.Offset = 0;
      instance_named_pipe.ovl.OffsetHigh = 0;
      instance_named_pipe.ovl.hEvent = 0;
      instance_named_pipe.p_inbuf = new irs_u8[m_size_inbuf];
      instance_named_pipe.p_outbuf = new irs_u8[m_size_outbuf];
      instance_named_pipe.count_inf_bytes_inbuf = 0;
      instance_named_pipe.count_inf_bytes_outbuf = 0;
      mv_named_pipe.push_back(instance_named_pipe);
      OVERLAPPED* p_ovl = &((*(mv_named_pipe.end())).ovl);
      ConnectNamedPipe(handle_named_pipe, p_ovl);
    }
  }
}

//class named_pipe_client_t
irs::named_pipe_client_t::named_pipe_client_t(const string_type& a_name
):
  m_max_size_write(65535),
  mp_error_trans(irs::error_trans()),
  m_name(irst("\\\\.\\pipe\\") + a_name),
  m_delay_control(make_cnt_ms(300)),
  m_control_timer(m_delay_control),
  m_connected(false)
{
  m_control_timer.start();
  m_ovl.Offset = 0;
  m_ovl.OffsetHigh = 0;
  m_ovl.hEvent = 0;
  m_handle_named_pipe = 0;
}

irs::named_pipe_client_t::~named_pipe_client_t()
{
  CloseHandle(m_handle_named_pipe);
}
irs::named_pipe_client_t::string_type
irs::named_pipe_client_t::param(const string_type &a_name)
{
  return string_type();
}
void irs::named_pipe_client_t::set_param(
  const string_type &a_name, const string_type &a_value)
{
}
irs_uarc irs::named_pipe_client_t::read(
  irs_uarc &a_channel_ident, irs_u8 *ap_buf, irs_uarc a_size)
{
  irs_u32 size_rd = 0;
  if (m_connected){
    if (!ReadFile(m_handle_named_pipe, ap_buf,  a_size, &size_rd, &m_ovl))
    {
      CloseHandle(m_handle_named_pipe);
      m_connected = false;
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  return size_rd;
}
irs_uarc irs::named_pipe_client_t::write(
  irs_uarc a_channel_ident, const irs_u8 *ap_buf, irs_uarc a_size)
{
  irs_u32 size_wr = 0;
  if (m_connected){
    irs_u32 size = min(a_size, m_max_size_write);
    irs_u32 dw_bytes_writte = 0;
    BOOL fsuccess = FALSE;
    fsuccess =
      WriteFile(m_handle_named_pipe, ap_buf, size, &dw_bytes_writte, &m_ovl);
    if ((!fsuccess))
    {
      DWORD dw = GetLastError();
      if (dw != ERROR_IO_PENDING){
        CloseHandle(m_handle_named_pipe);
        m_connected = false;
        IRS_LIB_SEND_LAST_ERROR();
      }else{
        size_wr = a_size;
      }
    }else{
      size_wr = a_size;
    }
  }
  return size_wr;
}
void irs::named_pipe_client_t::tick()
{
  BOOL fsuccess = FALSE;
  if (m_control_timer.check() && (!m_connected)) {
    fsuccess = WaitNamedPipe(m_name.c_str(), 0);
    if (fsuccess){
      m_handle_named_pipe = CreateFile(
        m_name.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        &m_ovl);
      if (m_handle_named_pipe != INVALID_HANDLE_VALUE)
      {
        m_connected = true;
      }
    }
  }
}

