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
    IRS_FATAL_ERROR("Valor de parбmetro de entrada no permitido.");
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
    IRS_FATAL_ERROR("Valor de parбmetro de entrada no permitido.");
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
    IRS_FATAL_ERROR("Valor de parбmetro de entrada no permitido.");
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
    IRS_FATAL_ERROR("Valor de parбmetro de entrada no permitido.");
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

