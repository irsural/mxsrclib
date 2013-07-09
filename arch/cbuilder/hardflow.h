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

namespace irs {

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
