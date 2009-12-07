// Коммуникационные потоки
// Для Borland C++ Builder
// Дата: 6.08.2009

#include <hardflow.h>
#include <mem.h>
// class com_flow_t
irs::com_flow_t::com_flow_t(
  const irs::string& a_portname,
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
  // открываем ком порт
  m_com = CreateFile(
    a_portname.c_str() ,
    GENERIC_READ|GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);
  if(m_com == INVALID_HANDLE_VALUE){
    fsuccess = FALSE;
    mp_error_trans->throw_error(
      ec_standard, __FILE__, __LINE__, "Ошибка открытия порта");
  }
  if(fsuccess){
    // настраиваем
    //устанавливаем размер внутренних буферов приема-передачи в драйвере порта
    fsuccess = SetupComm(m_com,m_max_size_write*2, m_max_size_write*2);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR();}
  }
  if(fsuccess) {
    get_param_dbc();
    m_com_param.baud_rate = a_baud_rate;
    m_com_param.f_parity = a_f_parity;
    m_com_param.parity = a_parity;
    m_com_param.byte_size = a_byte_size;
    m_com_param.stop_bits = a_stop_bits;
    m_com_param.f_dtr_control = a_f_dtr_control;

    set_and_get_param_dbc();
    fsuccess = GetCommTimeouts(m_com,&time_outs);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR();}
  }
  if (fsuccess) {
    time_outs.ReadIntervalTimeout = 10;
    time_outs.ReadTotalTimeoutMultiplier = 0;
    time_outs.ReadTotalTimeoutConstant = 10;
    time_outs.WriteTotalTimeoutMultiplier = 0;
    time_outs.WriteTotalTimeoutConstant = 10;
    fsuccess = SetCommTimeouts(m_com,&time_outs);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR();}
  }
  if (fsuccess) {
    // сброс порта
    fsuccess = PurgeComm(
      m_com, PURGE_RXABORT|PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    if (!fsuccess) { IRS_LIB_SEND_LAST_ERROR(); }
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

irs::string irs::com_flow_t::param(const irs::string &a_name)
{
  irs::string param_value_str = "";
  if(m_port_status == PS_PREFECT){
    if(a_name == m_com_param_str.baud_rate){
      param_value_str = m_com_param.baud_rate;
    }else if(a_name == m_com_param_str.f_parity){
      param_value_str = m_com_param.f_parity ;
    }else if(a_name == m_com_param_str.f_outx_cts_flow){
      param_value_str = m_com_param.f_outx_cts_flow;
    }else if(a_name == m_com_param_str.f_outx_dsr_flow){
      param_value_str = m_com_param.f_outx_dsr_flow;
    }else if(a_name == m_com_param_str.f_dtr_control){
      param_value_str = m_com_param.f_dtr_control;
    }else if(a_name == m_com_param_str.f_dsr_sensitivity){
      param_value_str = m_com_param.f_dsr_sensitivity;
    }else if(a_name == m_com_param_str.f_tx_continue_on_xoff){
      param_value_str = m_com_param.f_tx_continue_on_xoff;
    }else if(a_name == m_com_param_str.f_out_x){
      param_value_str = m_com_param.f_out_x;
    }else if(a_name == m_com_param_str.f_in_x){
      param_value_str = m_com_param.f_in_x;
    }else if(a_name == m_com_param_str.f_error_char){
      param_value_str = m_com_param.f_error_char;
    }else if(a_name == m_com_param_str.f_null){
      param_value_str = m_com_param.f_null;
    }else if(a_name == m_com_param_str.f_abort_on_error){
      param_value_str = m_com_param.f_abort_on_error;
    }else if(a_name == m_com_param_str.xon_lim){
      param_value_str = m_com_param.xon_lim;
    }else if(a_name == m_com_param_str.xoff_lim){
      param_value_str = m_com_param.xoff_lim;
    }else if(a_name == m_com_param_str.byte_size){
      param_value_str = m_com_param.byte_size;
    }else if(a_name == m_com_param_str.parity){
      param_value_str = m_com_param.parity;
    }else if(a_name == m_com_param_str.stop_bits){
      param_value_str = m_com_param.stop_bits;
    }else if(a_name == m_com_param_str.xon_char){
      param_value_str = m_com_param.xon_char;
    }else if(a_name == m_com_param_str.xoff_char){
      param_value_str = m_com_param.xoff_char;
    }else if(a_name == m_com_param_str.error_char){
      param_value_str = m_com_param.error_char;
    }else if(a_name == m_com_param_str.eof_char){
      param_value_str = m_com_param.eof_char;
    }else if(a_name == m_com_param_str.evt_char){
      param_value_str = m_com_param.evt_char;
    }
  }
  return param_value_str;
}

void irs::com_flow_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{
  bool fsuccess = true;
  if(m_port_status == PS_PREFECT){
    irs_u32 value = 0;
    fsuccess = a_value.to_number(value);
    if(a_name == m_com_param_str.baud_rate) {
      if(fsuccess){m_com_param.baud_rate = value;}
    }else if(a_name == m_com_param_str.f_parity) {
      if(fsuccess){m_com_param.f_parity = value;}
    }else if(a_name == m_com_param_str.f_outx_cts_flow){
      if(fsuccess){m_com_param.f_outx_cts_flow = value;}
    }else if(a_name == m_com_param_str.f_outx_dsr_flow){
      if(fsuccess){m_com_param.f_outx_dsr_flow = value;}
    }else if(a_name == m_com_param_str.f_dtr_control){
      if(fsuccess){m_com_param.f_dtr_control = value;}
    }else if(a_name == m_com_param_str.f_dsr_sensitivity){
      if(fsuccess){m_com_param.f_dsr_sensitivity = value;}
    }else if(a_name == m_com_param_str.f_tx_continue_on_xoff){
      if(fsuccess){m_com_param.f_tx_continue_on_xoff = value;}
    }else if(a_name == m_com_param_str.f_out_x){
      if(fsuccess){m_com_param.f_out_x = value;}
    }else if(a_name == m_com_param_str.f_in_x){
      if(fsuccess){m_com_param.f_in_x = value;}
    }else if(a_name == m_com_param_str.f_error_char){
      if(fsuccess){m_com_param.f_error_char = value;}
    }else if(a_name == m_com_param_str.f_null){
      if(fsuccess){m_com_param.f_null = value;}
    }else if(a_name == m_com_param_str.f_abort_on_error){
      if(fsuccess){m_com_param.f_abort_on_error = value;}
    }else if(a_name == m_com_param_str.xon_lim){
      if(fsuccess){m_com_param.xon_lim = static_cast<irs_i8>(value);}
    }else if(a_name == m_com_param_str.xoff_lim){
      if(fsuccess){m_com_param.xoff_lim = static_cast<irs_i8>(value);}
    }else if(a_name == m_com_param_str.byte_size){
      if(fsuccess){m_com_param.byte_size = static_cast<irs_u8>(value);}
    }else if(a_name == m_com_param_str.parity){
      if(fsuccess){m_com_param.parity = static_cast<irs_u8>(value);}
    }else if(a_name == m_com_param_str.stop_bits){
      if(fsuccess){m_com_param.stop_bits = static_cast<irs_u8>(value);}
    }else if(a_name == m_com_param_str.xon_char){
      if(fsuccess){m_com_param.xon_char = static_cast<irs_i8>(value);}
    }else if(a_name == m_com_param_str.xoff_char){
      if(fsuccess){m_com_param.xoff_char = static_cast<irs_i8>(value);}
    }else if(a_name == m_com_param_str.error_char){
      if(fsuccess){m_com_param.error_char = static_cast<irs_i8>(value);}
    }else if(a_name == m_com_param_str.eof_char){
      if(fsuccess){m_com_param.eof_char = static_cast<irs_i8>(value);}
    }else if(a_name == m_com_param_str.evt_char){
      if(fsuccess){m_com_param.evt_char = static_cast<irs_i8>(value);}
    }
    set_and_get_param_dbc();
  }
  IRS_ASSERT(fsuccess);
}

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
      fsuccess = WriteFile(m_com, ap_buf, size_wr, &num_of_bytes_written, NULL);
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
  if(m_port_status == PS_DEFECT){
    fsuccess = 0;
  }else{
    fsuccess = GetCommState(m_com, &dcb);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR();}
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
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR(); return;}
    get_param_dbc();
  }else{
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
  if(fsuccess){
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
    if (!fsuccess) { IRS_LIB_SEND_LAST_ERROR(); }
  }else{
    m_port_status = PS_DEFECT;
    resource_free();
  }
}
void irs::com_flow_t::resource_free()
{
  if(!m_on_resource_free){
    m_on_resource_free = true;
    PurgeComm(m_com, PURGE_RXABORT|PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    CloseHandle(m_com);
  }
}


//class named_pipe_t
irs::named_pipe_server_t::named_pipe_server_t(
  const irs::string& a_name,
  const irs_u32 a_count_instances
):
  m_max_size_write(65535),
  m_size_inbuf(65535),
  m_size_outbuf(65535),
  mp_error_trans(irs::error_trans()),
  m_name("\\\\.\\pipe\\"+a_name),
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
irs::string irs::named_pipe_server_t::param(const irs::string &a_name)
{
  return irs::string();
}
void irs::named_pipe_server_t::set_param(
  const irs::string &a_name, const irs::string &a_value)
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
irs::named_pipe_client_t::named_pipe_client_t(const irs::string& a_name
):
  m_max_size_write(65535),
  mp_error_trans(irs::error_trans()),
  m_name("\\\\.\\pipe\\"+a_name),
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
irs::string irs::named_pipe_client_t::param(const irs::string &a_name)
{
  return irs::string();
}
void irs::named_pipe_client_t::set_param(
  const irs::string &a_name, const irs::string &a_value)
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


// class tcp_server_t
irs::tcp_server_t::tcp_server_t(
):
  mp_error_trans(irs::error_trans()),
  m_state_info(),
  m_wsd(),
  m_server_listen_sock(),
  m_client_sock(),
  m_local_addr(),
  m_func_select_timeout(),
  m_sls_kit_read(),
  m_cs_kit_read(),
  m_cs_kit_write()
{
  m_local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  m_local_addr.sin_family = AF_INET;
  m_local_addr.sin_port = htons(5050);
  m_func_select_timeout.tv_sec = 0;
  m_func_select_timeout.tv_usec = 0;

  start();
}

irs::tcp_server_t::~tcp_server_t()
{
  if (m_state_info.slsock_created) {
    // Запрещаем прием и отправку данных
    //shutdown(m_server_listen_sock, SD_BOTH);
    closesocket(m_server_listen_sock);
  } else {
    //Сокет не создан
  }
  sock_container_t::iterator it_cl_sock = m_client_sock.begin();
  const sock_container_t::iterator it_cl_sock_end = m_client_sock.end();
  for ( ; it_cl_sock != it_cl_sock_end; it_cl_sock++) {
    // Запрещаем прием и отправку данных
    //shutdown(*it_cl_sock, SD_BOTH);
    closesocket(*it_cl_sock);
  }
}

void irs::tcp_server_t::start()
{
  if (m_state_info.lib_socket_loaded) {
    // Библиотека уже загружена
  } else {
    // загружаем библиотеку сокетов версии 2.2
    WORD version_requested = 0;
    IRS_LOBYTE(version_requested) = 2;
    IRS_HIBYTE(version_requested) = 2;
    if (WSAStartup(version_requested, &m_wsd) != 0) {
      // Ошибка при загрузке библиотеки
    } else {
      // библиотека удачно загружена
      m_state_info.lib_socket_loaded = true;
    }
  }
  if (m_state_info.lib_socket_loaded) {
    if (m_state_info.slsock_created) {
      // Сокет уже создан
    } else {
      m_server_listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
      if (m_server_listen_sock == static_cast<SOCKET>(SOCKET_ERROR)) {
        // Сокет создать не удалось
      } else {
        m_state_info.slsock_created = true;
      }
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.slsock_created) {
    if (m_state_info.set_io_mode_slsock_success) {
      // Сокет уже переведен в неблокирующий режим
    } else {
      // функция "ioctlsocket" используется для перевода сокета в неблокирующий
      // режим
      // для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      ULONG ulblock = 1;
      if (ioctlsocket(m_server_listen_sock, FIONBIO, &ulblock) == SOCKET_ERROR)
      {
        // функция завершилась неудачей
      } else {
        m_state_info.set_io_mode_slsock_success = true;
      }
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_slsock_success) {
    if (m_state_info.bind_slsock_and_ladr_success) {
      // Сокет уже привязан к локальному адресу
    } else {
      // !!! Необходимо добавить инициализацию структуры m_local_addr
      if (bind(m_server_listen_sock, (struct sockaddr *)&m_local_addr,
        sizeof(m_local_addr)) == SOCKET_ERROR)
      {
        // Ошибка при связывании сокета с локальным адресом
      } else {
        m_state_info.bind_slsock_and_ladr_success = true;
      }
    }
  } else {
    // Перевод сокета в неблокирующий режим завершился неудачей
  }
  if (m_state_info.bind_slsock_and_ladr_success) {
    if (m_state_info.listen_slsock_success) {
      // Прослушивание порта уже запущено
    } else {
      if (listen(m_server_listen_sock, 4) == SOCKET_ERROR) {
        // Ошибка при запуске прослушивания порта
      } else {
        m_state_info.listen_slsock_success = true;
      }
    }
  } else {
    // Ошибка при связывании сокета с локальным адресом
  }
}

irs::string irs::tcp_server_t::param(const irs::string &a_name)
{
  irs::string str = "";
  return str;
}

void irs::tcp_server_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{
}

irs_uarc irs::tcp_server_t::read(irs_uarc &a_channel_ident, irs_u8 *ap_buf,
  irs_uarc a_size)
{
  irs_uarc size_rd = 0;
  if (a_channel_ident < m_client_sock.size()) {
    sock_container_t::iterator it_cl_sock =
      m_client_sock.begin() + a_channel_ident;
    FD_ZERO(&m_cs_kit_read);
    FD_SET(*it_cl_sock, &m_cs_kit_read);
    int ready_sock_count = select(
      NULL, &m_cs_kit_read, NULL, NULL, &m_func_select_timeout);
    if (ready_sock_count == SOCKET_ERROR)
    {
      IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
      IRS_LIB_FATAL_ERROR("Select failed");
    } else if (ready_sock_count > 0) {
      if (FD_ISSET(*it_cl_sock, &m_cs_kit_read)) {
        int ret = recv(*it_cl_sock, reinterpret_cast<char*>(ap_buf), a_size, 0);
        if (ret == SOCKET_ERROR) {
          IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
          // Произошло отключение клиента, удаляем сокет
          closesocket(*it_cl_sock);
          m_client_sock.erase(it_cl_sock);
        } else if (ret == 0) {
          // Произошло отключение клиента, удаляем сокет
          closesocket(*it_cl_sock);
          m_client_sock.erase(it_cl_sock);
        } else {
          size_rd = static_cast<irs_uarc>(ret);
        }
      } else {
        // Сокет обязан входить в набор, так как в наборе был только один сокет
        IRS_LIB_FATAL_ERROR("Panic");
      }
    } else {
      // Сокет не готов отдавать данные
    }
  } else {
    //Канала с таким номером не существует
  }
  return size_rd;
}

irs_uarc irs::tcp_server_t::write(
  irs_uarc a_channel_ident,
  const irs_u8 *ap_buf,
  irs_uarc a_size)
{
  irs_uarc size_wr = 0;
  if (a_channel_ident < m_client_sock.size()) {
    sock_container_t::iterator it_cl_sock =
    m_client_sock.begin() + a_channel_ident;
    FD_ZERO(&m_cs_kit_write);
    FD_SET(*it_cl_sock, &m_cs_kit_write);
    int ready_sock_count = select(
      NULL, NULL, &m_cs_kit_write, NULL, &m_func_select_timeout);
    if (ready_sock_count == SOCKET_ERROR)
    {
      IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
      IRS_LIB_FATAL_ERROR("Select failed");
    } else if (ready_sock_count > 0) {
      if (FD_ISSET(*it_cl_sock, &m_cs_kit_write)) {
        int ret =
          send(*it_cl_sock, reinterpret_cast<const char*>(ap_buf), a_size, 0);
        if (ret == SOCKET_ERROR) {
          IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
          // Произошло отключение клиента, удаляем сокет
          closesocket(*it_cl_sock);
          m_client_sock.erase(it_cl_sock);
        } else {
          size_wr = static_cast<irs_uarc>(ret);
        }
      } else {
        // Сокет обязан входить в набор, так как в наборе был только один сокет
        IRS_LIB_FATAL_ERROR("Panic");
      }
    } else {
      // Сокет не готов записывать
    }
  } else {
    // Канала с таким номером не существует
  }
  return size_wr;
}

void irs::tcp_server_t::tick()
{
  if (m_state_info.listen_slsock_success) {
    FD_ZERO(&m_sls_kit_read);
    FD_SET(m_server_listen_sock, &m_sls_kit_read);
    int ready_sock_count = select(
      NULL, &m_sls_kit_read, NULL, NULL, &m_func_select_timeout);
    if (ready_sock_count == SOCKET_ERROR) {
      IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
      // Возможна ошибка WSAENOTSOCK. Это возникает по причине работы с
      // неблокирующим сокетом и это нормально для нашего алгоритма.
    } else if (ready_sock_count > 0) {
      // Проверяем вхождение сокета в набор
      if (FD_ISSET(m_server_listen_sock, &m_sls_kit_read)) {
        sockaddr_in client_addr;
        int addr_size = sizeof(client_addr);
        SOCKET client_sock = accept(
          m_server_listen_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock == INVALID_SOCKET) {
          IRS_LIB_FATAL_ERROR("Accept filed");
        } else {
          // Переводим сокет в неблокирующий режим
          ULONG ulblock = 1;
          if (ioctlsocket(client_sock, FIONBIO, &ulblock) == SOCKET_ERROR)
          {
            IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
            closesocket(client_sock);
          } else {
            m_client_sock.push_back(client_sock);
            IRS_LIB_SOCK_DBG_MSG("Произошло подключение клиента");
          }
        }
      } else {
        // Сокет обязан входить в набор, так как в наборе был только один сокет
        IRS_LIB_FATAL_ERROR("Panic");
      }

    } else {
      // Новых запросов на соединение не обнаружено
    }
  } else {
    // Не запущено прослушивание порта
    // Запускаем сервер
    start();
  }
}

irs::tcp_client_t::tcp_client_t(
):
  mp_error_trans(irs::error_trans()),
  m_state_info(),
  m_wsd(),
  m_client_sock(),
  m_server_addr(),
  m_func_select_timeout(),
  m_cs_kit_read(),
  m_cs_kit_write()
{
  m_server_addr.sin_family = AF_INET;
  m_server_addr.sin_port = htons(5050);
  m_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  m_func_select_timeout.tv_sec = 0;
  m_func_select_timeout.tv_usec = 0;
}

irs::tcp_client_t::~tcp_client_t()
{
  //shutdown(m_client_sock, SD_SEND);
  closesocket(m_client_sock);
}

void irs::tcp_client_t::start()
{
  if (m_state_info.lib_socket_loaded) {
    // Библиотека уже загружена
  } else {
    // загружаем библиотеку сокетов версии 2.2
    WORD version_requested = 0;
    IRS_LOBYTE(version_requested) = 2;
    IRS_HIBYTE(version_requested) = 2;
    if (WSAStartup(version_requested, &m_wsd) != 0) {
      // Ошибка при загрузке библиотеки
    } else {
      // Библиотека удачно загружена
      m_state_info.lib_socket_loaded = true;
    }
  }
  if (m_state_info.lib_socket_loaded) {
    if (m_state_info.csock_created) {
      // Сокет уже создан
    } else {
      m_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
      if (m_client_sock == static_cast<SOCKET>(SOCKET_ERROR)) {
        // Сокет создать не удалось
      } else {
        m_state_info.csock_created = true;
      }
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.csock_created) {
    if (m_state_info.set_io_mode_csock_success) {
      // Сокет уже переведен в неблокирующий режим
    } else {
      // функция "ioctlsocket" используется для перевода сокета в неблокирующий
      // режим
      // Для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      ULONG ulblock = 1;
      if (ioctlsocket(m_client_sock, FIONBIO, &ulblock) == SOCKET_ERROR)
      {
        // функция завершилась неудачей
      } else {
        m_state_info.set_io_mode_csock_success = true;
      }
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_csock_success) {
    if (m_state_info.csock_connected) {
      // Сокет уже соединен с сервером
    } else {
      if (connect(
        m_client_sock,
        (struct sockaddr *)&m_server_addr,
        sizeof(m_server_addr)) == SOCKET_ERROR)
      {
         const int Error = WSAGetLastError();
         if (Error == WSAEWOULDBLOCK) {
           // Сокет в неблокирующем режиме, а соединение с сервером не
           // может быть выполнено мгновенно.
           // Это для данного алгоритма не является ошибкой
         } else if (Error == WSAEISCONN) {
           // Сокет уже соединен с сервером
           m_state_info.csock_connected = true;
           IRS_LIB_SOCK_DBG_MSG("Произошло подключение к серверу");
         } else {
           // Другая ошибка
         }
      } else {
        m_state_info.csock_connected = true;
      }
    }
  } else {
    // Сокет не переведен в неблокирующий режим
  }
}

irs::string irs::tcp_client_t::param(const irs::string &a_name)
{
  irs::string str;
  return str;
}

void irs::tcp_client_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{ }

irs_uarc irs::tcp_client_t::read(
  irs_uarc &a_channel_ident,
  irs_u8 *ap_buf,
  irs_uarc a_size)
{
  irs_uarc size_rd = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    FD_ZERO(&m_cs_kit_read);
    FD_SET(m_client_sock, &m_cs_kit_read);
    int ready_sock_count = select(
      NULL,
      &m_cs_kit_read,
      NULL,
      NULL,
      &m_func_select_timeout);
    if (ready_sock_count == SOCKET_ERROR)
    {
      IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
      IRS_LIB_FATAL_ERROR("Select failed");
    } else if (ready_sock_count > 0) {
      if (FD_ISSET(m_client_sock, &m_cs_kit_read)) {
        int ret = recv(
          m_client_sock, reinterpret_cast<char*>(ap_buf), a_size, 0);
        if (ret == SOCKET_ERROR) {
          IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
          m_state_info.csock_connected = false;
        } else if (ret == 0) {
          m_state_info.csock_connected = false;
        } else {
          size_rd = static_cast<irs_uarc>(ret);
        }
      } else {
        // Сокет обязан входить в набор, так как в наборе был только один сокет
        IRS_LIB_FATAL_ERROR("Panic");
      }
    } else {
      // Сокет не готов для чтения
    }
  } else {
    // Клиент еще не запущен
  }
  return size_rd;
}

irs_uarc irs::tcp_client_t::write(
  irs_uarc a_channel_ident,
  const irs_u8 *ap_buf,
  irs_uarc a_size)
{
  irs_uarc size_wr = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    FD_ZERO(&m_cs_kit_write);
    FD_SET(m_client_sock, &m_cs_kit_write);
    int ready_sock_count = select(
      NULL,
      NULL,
      &m_cs_kit_write,
      NULL,
      &m_func_select_timeout);
    if (ready_sock_count == SOCKET_ERROR) {
      IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
    } else if (ready_sock_count > 0) {
      if (FD_ISSET(m_client_sock, &m_cs_kit_write)) {
        int ret = send(
          m_client_sock, reinterpret_cast<const char*>(ap_buf), a_size, 0);
        if (ret == SOCKET_ERROR) {
          IRS_LIB_SOCK_DBG_MSG("Произошла какая то ошибка при отправке"
            "сообщения. Пробуем снова подключиться к серверу");
          m_state_info.csock_connected = false;
        } else {
          size_wr = static_cast<irs_uarc>(ret);
        }
      } else {
        // Сокет обязан входить в набор, так как в наборе был только один сокет
        IRS_LIB_FATAL_ERROR("Panic");
      }
    } else {
      // Сокет не готов для записи
    }
  } else {
    // Клиент еще не запущен
  }
  return size_wr;
}

void irs::tcp_client_t::tick()
{
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    // Клиент успешно запущен
  } else {
    start();
  }
}

#ifdef UDP_FLOW_PREVIOUS_VERSION
// class udp_flow_t
irs::udp_flow_t::udp_flow_t(
  const string_type& a_recipient_name,
  const string_type& a_recipient_port,
  const string_type& a_local_name,
  const string_type& a_local_port
):
  mp_error_trans(irs::error_trans()),
  m_state_info(),
  m_wsd(),
  m_sock(),
  m_recipient_addr(),
  m_local_name(a_local_name),
  m_local_port(a_local_port),
  m_recipient_name(a_recipient_name),
  m_recipient_port(a_recipient_port),
  m_func_select_timeout(),
  m_s_kit(),
  m_send_msg_max_size(1024)
{
  m_func_select_timeout.tv_sec = 0;
  m_func_select_timeout.tv_usec = 0;
  bool init_success = true;
  recipient_addr_init(&m_recipient_addr, &init_success);
}

irs::udp_flow_t::~udp_flow_t()
{
  if (m_state_info.sock_created) {
    closesocket(m_sock);
  } else {
    // Сокет не создан
  }
}

void irs::udp_flow_t::start()
{
  if (m_state_info.lib_socket_loaded) {
    // Библиотека уже загружена
  } else {
    // загружаем библиотеку сокетов версии 2.2
    WORD version_requested = 0;
    IRS_LOBYTE(version_requested) = 2;
    IRS_HIBYTE(version_requested) = 2;
    if (WSAStartup(version_requested, &m_wsd) != 0) {
      // Ошибка при загрузке библиотеки
    } else {
      // библиотека удачно загружена
      m_state_info.lib_socket_loaded = true;
      m_state_info.sock_created = false;
    }
  }
  if (m_state_info.lib_socket_loaded) {
    if (m_state_info.sock_created) {
      // Сокет уже создан
    } else {
      m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (m_sock == INVALID_SOCKET) {
        // Сокет создать не удалось
      } else {
        m_state_info.sock_created = true;
        m_state_info.set_io_mode_sock_success = false;
        // Обновляем переменную, указывающую максимальный размер сообщения
        DWORD optval;
        int optval_size = sizeof(optval);
        int res = getsockopt(m_sock, SOL_SOCKET, SO_MAX_MSG_SIZE,
          reinterpret_cast<char*>(&optval),  &optval_size);
        if (res == SOCKET_ERROR) {
          IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
        } else {
          m_send_msg_max_size = optval;
        }
      }
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.sock_created) {
    if (m_state_info.set_io_mode_sock_success) {
      // Сокет уже переведен в неблокирующий режим
    } else {     
      // для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      ULONG ulblock = 1;
      if (ioctlsocket(m_sock, FIONBIO, &ulblock) == SOCKET_ERROR)
      {
        // функция завершилась неудачей
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_sock_success) {
    if (m_state_info.bind_sock_and_ladr_success) {
      // Сокет уже привязан к локальному адресу
    } else {
      sockaddr_in local_addr;
      bool init_success = true;
      local_addr_init(&local_addr, &init_success);
      if (init_success) { m_state_info.bind_sock_and_ladr_success = true;
        if (bind(m_sock, (sockaddr *)&local_addr,
          sizeof(local_addr)) == SOCKET_ERROR)
        {
          // Ошибка при связывании сокета с локальным адресом
        } else {
          m_state_info.bind_sock_and_ladr_success = true;
        }
      } else {
        // Ошибка инициализации
      }
    }
  } else {
    // Перевод сокета в неблокирующий режим завершился неудачей
  }
}

void irs::udp_flow_t::sock_close()
{
  closesocket(m_sock);
  m_state_info.sock_created = false;
  m_state_info.set_io_mode_sock_success = false;
  m_state_info.bind_sock_and_ladr_success = false;
}

void irs::udp_flow_t::local_addr_init(
  sockaddr_in* ap_sockaddr, bool* ap_init_success)
{
  *ap_init_success = true;
  if (m_local_name == "") {
    ap_sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    unsigned long addr = inet_addr(m_local_name.c_str());
    if (addr == INADDR_NONE) {
      hostent* p_host = gethostbyname(m_local_name.c_str());
      if (p_host == NULL) {
        // Ошибка получения адреса
        *ap_init_success = false;
      } else {
        // Берем первый адрес в списке
        memcpy(&ap_sockaddr->sin_addr,
          p_host->h_addr_list[0], p_host->h_length);
      }
    } else {
      ap_sockaddr->sin_addr.s_addr = addr;
    }
  }
  ap_sockaddr->sin_family = AF_INET;
  unsigned short port = 0;
  if (m_local_port.to_number(port)) {
    ap_sockaddr->sin_port = htons(port);
  } else {
    *ap_init_success = false;
  }
  #ifdef IRS_LIB_DEBUG
  int b1 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b1;
  int b2 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b2;
  int b3 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b3;
  int b4 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b4;
  #endif // IRS_LIB_DEBUG
}

void irs::udp_flow_t::recipient_addr_init(
  sockaddr_in* ap_sockaddr, bool* ap_init_success)
{
  *ap_init_success = true;
  unsigned long addr = inet_addr(m_recipient_name.c_str());
  if (addr == INADDR_NONE) {
    hostent* p_host = gethostbyname(m_recipient_name.c_str());
    if (p_host == NULL) {
      // Ошибка получения адреса
      *ap_init_success = false;
    } else {
      // Берем первый адрес в списке
      memcpy(&(ap_sockaddr->sin_addr), p_host->h_addr_list[0],
        p_host->h_length);
    }
  } else {
    ap_sockaddr->sin_addr.s_addr = addr;
  }
  ap_sockaddr->sin_family = AF_INET;
  unsigned short port = 0;
  if (m_recipient_port.to_number(port)) {
    ap_sockaddr->sin_port = htons(port);
  } else {
    *ap_init_success = false;
  }
  #ifdef IRS_LIB_DEBUG
  int b1 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b1;
  int b2 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b2;
  int b3 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b3;
  int b4 = ap_sockaddr->sin_addr.S_un.S_un_b.s_b4;
  #endif // IRS_LIB_DEBUG
}

irs::string irs::udp_flow_t::param(const irs::string &a_name)
{
  irs::string param;
  return param;
}

void irs::udp_flow_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{
}

irs_uarc irs::udp_flow_t::read(
  irs_uarc &a_channel_ident, irs_u8 *ap_buf, irs_uarc a_size)
{
  a_channel_ident = 0;
  irs_uarc size_rd = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    FD_ZERO(&m_s_kit);
    FD_SET(m_sock, &m_s_kit);
    int ready_sock_count = select(NULL, &m_s_kit, NULL, NULL,
      &m_func_select_timeout);
    if (ready_sock_count == SOCKET_ERROR)
    {
      IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
      IRS_LIB_FATAL_ERROR("Select failed");
    } else if (ready_sock_count > 0) {
      if (FD_ISSET(m_sock, &m_s_kit)) {
        sockaddr_in sender_addr;
        int sender_addr_size = sizeof(sender_addr);
        int ret = recvfrom(m_sock, reinterpret_cast<char*>(ap_buf), a_size, 0,
          (sockaddr*)(&sender_addr), &sender_addr_size);
        if (ret == SOCKET_ERROR) {
          switch (ret) {
            case WSAECONNRESET: {
              // Адресат не доступен
            } break;
            case WSAEMSGSIZE : {
              // Размер полученного сообщения больше размера буфера
            } break;
            default : {
              IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
              sock_close();
            }
          }
        } else {
          size_rd = static_cast<irs_uarc>(ret);
        }
      } else {
        // Сокет обязан входить в набор, так как в наборе был только один сокет
        IRS_LIB_FATAL_ERROR("Panic");
      }
    } else {
      // Сокет не готов для чтения
    }    
  } else {
    // Запуск не произошел
  }
  return size_rd;
}

irs_uarc irs::udp_flow_t::write(
  irs_uarc a_channel_ident, const irs_u8 *ap_buf, irs_uarc a_size)
{
  irs_uarc size_wr = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    size_type msg_size = min(a_size, m_send_msg_max_size);
    const int ret = sendto(m_sock, reinterpret_cast<const char*>(ap_buf),
      msg_size, 0, (sockaddr*)(&m_recipient_addr),
        sizeof(m_recipient_addr));
    if (ret == SOCKET_ERROR) {
      switch (ret) {
        case WSAEMSGSIZE : {
          // Размер отправляемого сообщения превышает допустимый
          IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
        } break;
        default : {
          IRS_LIB_SEND_WIN_WSA_LAST_ERROR();
          sock_close();
        }
      }  
    } else {
      size_wr = static_cast<irs_uarc>(ret);
    }
  } else {
    // Запуск не произошел
  }
  return size_wr;
}

void irs::udp_flow_t::tick()
{
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    // Клиент успешно запущен
  } else {
    start();
  }
}

#endif // UDP_FLOW_PREVIOUS_VERSION
