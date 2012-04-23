//! \file
//! \ingroup drivers_group
//! \brief Работа с National Instruments USB-GPIB через интерфейс
//!   irs::hardflow_t
//!
//! Дата: 06.08.2010\n
//! Дата создания: 06.08.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <niusbgpib_hardflow.h>

#include <irsfinal.h>

#if defined(IRS_WIN32)

namespace {

double time_s_normalize(const double a_time_s);

int time_s_to_timecode(const double a_time_s);

irs::irs_string_t data_to_string_for_mlog(const irs::raw_data_t<irs_u8>& a_data);

} // empty namespace

// class ni_usb_gpib_flow_t
irs::hardflow::ni_usb_gpib_flow_t::ni_usb_gpib_flow_t(
  const size_type a_board_index,
  const size_type a_gpib_adress,
  const double a_session_read_timeout_s,
  const double a_session_write_timeout_s
):
  m_process(process_wait),
  m_proc_array(),
  mp_ni_usb_gpib(ni_usb_gpib_t::get_instance()),
  m_device_id(),
  m_channel(invalid_channel),
  m_read_buf(),
  m_write_buf(),
  m_session_read_timeout(a_session_read_timeout_s),
  m_session_write_timeout(a_session_write_timeout_s),
  m_session_write_timer(make_cnt_s(a_session_write_timeout_s)),
  m_session_read_timer(make_cnt_s(a_session_read_timeout_s)),
  m_timeout_delta(0.05)
{
  m_proc_array.push_back(&ni_usb_gpib_flow_t::proc_wait);
  m_proc_array.push_back(&ni_usb_gpib_flow_t::proc_read);
  m_proc_array.push_back(&ni_usb_gpib_flow_t::proc_write);
  mp_ni_usb_gpib->init();
  m_device_id = mp_ni_usb_gpib->ibdev(a_board_index, a_gpib_adress,
    m_no_secondary_addr, m_timeout, m_eotmode, m_eosmode);
  if (m_device_id != static_cast<size_type>(-1)) {
    //int ibclr_status =
    mp_ni_usb_gpib->ibclr(m_device_id);
    m_channel = invalid_channel + 1;
  } else {
    m_channel = invalid_channel;
  }
}

irs::hardflow::ni_usb_gpib_flow_t::~ni_usb_gpib_flow_t()
{
  if (m_channel != invalid_channel) {
    // Перевод устройства в offline
    mp_ni_usb_gpib->ibonl(m_device_id, false);
    mp_ni_usb_gpib->deinit();
  } else {
    // Канал не создан
  }
}

irs::hardflow::ni_usb_gpib_flow_t::string_type
irs::hardflow::ni_usb_gpib_flow_t::param(const string_type& /*a_name*/)
{
  return irst("");
}

void irs::hardflow::ni_usb_gpib_flow_t::set_param(const string_type& a_name,
  const string_type& a_value)
{
  if ((a_name == irst("read_timeout")) || (a_name == irst("write_timeout"))) {
    double value = 0;
    if (str_to_num(a_value, &value)) {
      if (a_name == irst("read_timeout")) {
        m_session_read_timeout = value;
        m_session_read_timer.set(make_cnt_s(time_s_normalize(value) +
          m_timeout_delta));
      } else {
        m_session_read_timeout = value;
        m_session_write_timer.set(make_cnt_s(time_s_normalize(value)));
      }
    } else {
      IRS_LIB_ERROR(ec_standard, "Недопустимое значение параметра");
    }
  } else if (a_name == irst("write_timeout")) {
  } else {
    IRS_LIB_ERROR(ec_standard, "Неизвестный параметр");
  }   
}

irs::hardflow::ni_usb_gpib_flow_t::size_type
irs::hardflow::ni_usb_gpib_flow_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type read_count = 0;
  if ((m_channel != invalid_channel) &&
    (m_channel == a_channel_ident) &&
    (m_process == process_wait) &&
    (a_size > 0)) {

    if (!m_read_buf.empty()) {
      read_count = min(a_size, m_read_buf.size());
      memcpy(ap_buf, m_read_buf.data(), read_count);
      m_read_buf.erase(m_read_buf.data(), m_read_buf.data() + read_count);
    } else {
      m_read_buf.resize(a_size);
      IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE("Запрашиваем чтение " << a_size <<
        " байт.");
      //const int set_timeout_status =
      mp_ni_usb_gpib->ibtmo(
        m_device_id, time_s_to_timecode(m_session_read_timeout));
      //const int read_status =
      mp_ni_usb_gpib->ibrda(m_device_id,
        m_read_buf.data(), m_read_buf.size());
      if (time_s_normalize(m_session_read_timeout) != 0.) {
        m_session_read_timer.start();
      } else {
        m_session_read_timer.stop();
      }
      m_process = process_read;
    }
  } else {
    // Канал не создан
  }
  return read_count;
}

irs::hardflow::ni_usb_gpib_flow_t::size_type
irs::hardflow::ni_usb_gpib_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_count = 0;
  if ((m_channel != invalid_channel) &&
    (m_channel == a_channel_ident) &&
    (m_process != process_write)) {

    if (m_process == process_read) {
      //const int clr_status =
      mp_ni_usb_gpib->ibclr(m_device_id);
      m_read_buf.clear();
    } else {
      // Операция чтения не происходит
    }  
    IRS_LIB_ASSERT(m_write_buf.empty());
    m_write_buf.insert(m_write_buf.data(), ap_buf, ap_buf + a_size);
    IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE("Запрашиваем запись " << a_size <<
      " байт.");
    IRS_LIB_NIUSBGPIBHF_DBG_MSG_DETAIL("Данные:" <<
      data_to_string_for_mlog(raw_data_t<irs_u8>(ap_buf, a_size)));
    //const int set_timeout_status =
    mp_ni_usb_gpib->ibtmo(m_device_id, TNONE);
    //const int write_status =
    mp_ni_usb_gpib->ibwrta(m_device_id,
      m_write_buf.data(), m_write_buf.size());
    write_count = a_size;
    if (time_s_normalize(m_session_write_timeout) != 0.) {
      m_session_write_timer.start();
    } else {
      m_session_write_timer.stop();
    }
    m_process = process_write;
  } else {
    // Канал не создан
  }
  return write_count;
}

irs::hardflow::ni_usb_gpib_flow_t::size_type
irs::hardflow::ni_usb_gpib_flow_t::channel_next()
{
  return m_channel;
}

bool irs::hardflow::ni_usb_gpib_flow_t::is_channel_exists(
  size_type a_channel_ident)
{
  return (m_channel == a_channel_ident);
}

void irs::hardflow::ni_usb_gpib_flow_t::tick()
{
  (this->*m_proc_array[m_process])();
}

void irs::hardflow::ni_usb_gpib_flow_t::proc_read()
{
  const bool session_read_timeout = m_session_read_timer.check();
  const int read_status = mp_ni_usb_gpib->ibwait(m_device_id, 0);
  if ((read_status & CMPL) ||
    (read_status & ERR) ||
    !m_write_buf.empty() ||
    session_read_timeout) {

    if (!m_write_buf.empty() || session_read_timeout) {
      const int stop_status = mp_ni_usb_gpib->ibstop(m_device_id);
      if (stop_status & ERR) {
        const int err_status = mp_ni_usb_gpib->get_iberr();
        if (!(err_status & EABO)) {
          // Возвращаем ошибку пользователю
          IRS_LIB_ASSERT("Остановить асинхронную операцию "
            "чтения не удалось");
        } else {
          // Чтение успешно завершено
        }
      } else {
        IRS_LIB_ASSERT("Остановить асинхронную операцию "
          "чтения не удалось");
      }
    } else {
      // Данные для записи не поступали
    }
    if (read_status & ERR) {
      IRS_LIB_ASSERT("Ошибка при асинхронной операции чтения");
    } else {
      // Чтение успешно завершено
    }

    //const size_type debug_ibcntl =
    mp_ni_usb_gpib->get_ibcntl();
    m_read_buf.resize(mp_ni_usb_gpib->get_ibcntl());
    m_process = process_wait;
    if ((read_status & ERR) ||
      !m_write_buf.empty() ||
      session_read_timeout) {
      if (read_status & ERR) {
        IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE("Чтение прервано по причине "
          "ошибки при чтении");
      } else if (!m_write_buf.empty()) {
        IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE("Чтение прервано, потому что есть "
          "данные на отправку");
      } else {
        IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE("Чтение прервано по тайм-ауту, "
          "прочитано: " << m_read_buf.size());
      }
    } else {
      IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE("Чтение завершено, прочитано: " <<
        m_read_buf.size());
    }
    IRS_LIB_NIUSBGPIBHF_DBG_MSG_DETAIL("Данные:" <<
      data_to_string_for_mlog(m_read_buf));
  } else {
    // Продолжаем чтение
  }
}

void irs::hardflow::ni_usb_gpib_flow_t::proc_write()
{
  const bool session_write_timeout = m_session_write_timer.check();
  const int write_status = mp_ni_usb_gpib->ibwait(m_device_id, 0);
  if ((write_status & CMPL) ||
    (write_status & ERR) ||
    session_write_timeout) {
      
    m_write_buf.erase(m_write_buf.data(),
      m_write_buf.data() + min(m_write_buf.size(),
      static_cast<size_t>(mp_ni_usb_gpib->get_ibcntl())));
    if (write_status & ERR) {
      IRS_LIB_ASSERT("Ошибка при асинхронной операции записи");
    } else {
      // Ошибок небыло
    }
    if (m_write_buf.empty()) {
      m_process = process_wait;
      IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE("Запись завершена");
    } else {
      //const int new_write_status =
      mp_ni_usb_gpib->ibwrta(m_device_id,
        m_write_buf.data(), m_write_buf.size());
      if (time_s_normalize(m_session_write_timeout) != 0.) {
        m_session_write_timer.start();
      } else {
        m_session_write_timer.stop();
      }
    }                                 
  } else {
    // Продолжаем запись
  }
}

void irs::hardflow::ni_usb_gpib_flow_t::proc_wait()
{
}

// class ni_usb_gpib_flow_syn_t
irs::hardflow::ni_usb_gpib_flow_syn_t::ni_usb_gpib_flow_syn_t(
  const size_type a_board_index,
  const size_type a_gpib_adress,
  const double a_read_timeout_s,
  const double a_write_timeout_s
):
  mp_ni_usb_gpib(ni_usb_gpib_t::get_instance()),
  m_device_id(),
  m_channel(invalid_channel),
  m_read_timeout(a_read_timeout_s),
  m_write_timeout(a_write_timeout_s)
{
  mp_ni_usb_gpib->init();
  m_device_id = mp_ni_usb_gpib->ibdev(a_board_index, a_gpib_adress,
    m_no_secondary_addr, T100ms, m_eotmode, m_eosmode);
  if (m_device_id != static_cast<size_type>(-1)) {
    //int ibclr_status =
    mp_ni_usb_gpib->ibclr(m_device_id);
    m_channel = invalid_channel + 1;
  } else {
    m_channel = invalid_channel;
  }
}

irs::hardflow::ni_usb_gpib_flow_syn_t::~ni_usb_gpib_flow_syn_t()
{
  if (m_channel != invalid_channel) {
    // Перевод устройства в offline
    mp_ni_usb_gpib->ibonl(m_device_id, false);
    mp_ni_usb_gpib->deinit();
  } else {
    // Канал не создан
  }
}

irs::hardflow::ni_usb_gpib_flow_syn_t::string_type
irs::hardflow::ni_usb_gpib_flow_syn_t::param(const string_type& /*a_name*/)
{
  return irst("");
}

void irs::hardflow::ni_usb_gpib_flow_syn_t::set_param(const string_type& a_name,
  const string_type& a_value)
{
  if ((a_name == irst("read_timeout")) ||
    (a_name == irst("write_timeout"))) {

    double value = 0;
    if (str_to_num(a_value, &value)) {
      if (a_name == irst("read_timeout")) {
        m_read_timeout = value;
      } else if (a_name == irst("write_timeout")) {
        m_write_timeout = value;
      } else {
        IRS_LIB_ASSERT_MSG("Недопустимый параметр");
      }
    } else {
      IRS_LIB_ERROR(ec_standard, "Недопустимое значение параметра");
    }
  } else {
    IRS_LIB_ERROR(ec_standard, "Неизвестный параметр");
  }   
}

irs::hardflow::ni_usb_gpib_flow_syn_t::size_type
irs::hardflow::ni_usb_gpib_flow_syn_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type read_count = 0;
  if ((m_channel != invalid_channel) && (m_channel == a_channel_ident)) {
    //const int set_timeout_status =
    mp_ni_usb_gpib->ibtmo(
      m_device_id, time_s_to_timecode(m_read_timeout));
    //const int read_status =
    mp_ni_usb_gpib->ibrd(m_device_id,
      ap_buf, a_size);
    read_count = mp_ni_usb_gpib->get_ibcntl();
  } else {
    // Канал не создан
  }
  return read_count;
}

irs::hardflow::ni_usb_gpib_flow_syn_t::size_type
irs::hardflow::ni_usb_gpib_flow_syn_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_count = 0;
  if ((m_channel != invalid_channel) && (m_channel == a_channel_ident)) {
    //const int set_timeout_status =
    mp_ni_usb_gpib->ibtmo(
      m_device_id, time_s_to_timecode(m_write_timeout));
    mp_ni_usb_gpib->ibwrt(m_device_id,
      const_cast<unsigned char*>(ap_buf), a_size);
    write_count = mp_ni_usb_gpib->get_ibcntl();
  } else {
    // Канал не создан
  }
  return write_count;
}

irs::hardflow::ni_usb_gpib_flow_syn_t::size_type
irs::hardflow::ni_usb_gpib_flow_syn_t::channel_next()
{
  return m_channel;
}

bool irs::hardflow::ni_usb_gpib_flow_syn_t::is_channel_exists(
  size_type a_channel_ident)
{
  return (m_channel == a_channel_ident);
}

void irs::hardflow::ni_usb_gpib_flow_syn_t::tick()
{

}

namespace {

double time_s_normalize(const double a_time_s)
{
  double time_normalize = 0.;
  if (a_time_s > 1000.) {
    time_normalize = 0.;
  } else if (a_time_s > 300.) {
    time_normalize = 1000;
  } else if (a_time_s > 100.) {
    time_normalize = 300.;
  } else if (a_time_s > 30.) {
    time_normalize = 100.;
  } else if (a_time_s > 10.) {
    time_normalize = 30.;
  } else if (a_time_s > 3.) {
    time_normalize = 10.;
  } else if (a_time_s > 1.) {
    time_normalize = 3.;
  } else if (a_time_s > 300e-3) {
    time_normalize = 1.;
  } else if (a_time_s > 100e-3) {
    time_normalize = 300e-3;
  } else if (a_time_s > 30e-3) {
    time_normalize = 100e-3;
  } else if (a_time_s > 10e-3) {
    time_normalize = 30e-3;
  } else if (a_time_s > 10e-3) {
    time_normalize = 30e-3;
  } else if (a_time_s > 3e-3) {
    time_normalize =  10e-3;
  } else if (a_time_s > 1e-3) {
    time_normalize = 3e-3;
  } else if (a_time_s > 300e-6) {
    time_normalize = 1e-3;
  } else if (a_time_s > 100e-6) {
    time_normalize = 300e-6;
  } else if (a_time_s > 30e-6) {
    time_normalize = T100us;
  } else if (a_time_s > 10e-6) {
    time_normalize = 30e-6;
  } else if (a_time_s == 10e-6) {
    time_normalize = 10e-6;
  } else {
    time_normalize = TNONE;
  }
  return time_normalize;
}

int time_s_to_timecode(const double a_time_s)
{
  int timecode = TNONE;
  if (a_time_s > 1000.) {
    timecode = TNONE;
  } else if (a_time_s > 300.) {
    timecode = T1000s;
  } else if (a_time_s > 100.) {
    timecode = T300s;
  } else if (a_time_s > 30.) {
    timecode = T100s;
  } else if (a_time_s > 10.) {
    timecode = T30s;
  } else if (a_time_s > 3.) {
    timecode = T10s;
  } else if (a_time_s > 1.) {
    timecode = T3s;
  } else if (a_time_s > 300e-3) {
    timecode = T1s;
  } else if (a_time_s > 100e-3) {
    timecode = T300ms;
  } else if (a_time_s > 30e-3) {
    timecode = T100ms;
  } else if (a_time_s > 10e-3) {
    timecode = T30ms;
  } else if (a_time_s > 10e-3) {
    timecode = T10ms;
  } else if (a_time_s > 3e-3) {
    timecode = T30ms;
  } else if (a_time_s > 1e-3) {
    timecode = T3ms;
  } else if (a_time_s > 300e-6) {
    timecode = T1ms;
  } else if (a_time_s > 100e-6) {
    timecode = T300us;
  } else if (a_time_s > 30e-6) {
    timecode = T100us;
  } else if (a_time_s > 10e-6) {
    timecode = T30us;
  } else if (a_time_s == 10e-6) {
    timecode = T10us;
  } else {
    timecode = TNONE;
  }
  return timecode;
}

irs::irs_string_t data_to_string_for_mlog(const irs::raw_data_t<irs_u8>& a_data)
{
  strstream ostr;
  for (size_t i = 0; i < a_data.size(); i++) {
    const char ch = a_data[i];

    if (irs::isprintt(ch) && !irs::iscntrlt(ch)) {
      ostr << ch;
    } else {
      const int width_not_graph_char = 2;
      ostr << "\\x" << setfill('0') << setw(width_not_graph_char) << hex <<
        uppercase << static_cast<int>(a_data[i]);
    }
  }
  ostr << '\0';
  ostr.rdbuf()->freeze(false);
  return ostr.str();
}

} // empty namespace

#endif // defined(IRS_WIN32)
