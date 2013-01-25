//! \file
//! \ingroup drivers_group
//! \brief Работа с National Instruments USB-GPIB через интерфейс
//!   irs::hardflow_t
//!
//! Дата: 06.08.2010\n
//! Дата создания: 06.08.2010

#ifndef niusbgpib_hardflowH
#define niusbgpib_hardflowH

#include <irsdefs.h>

#include <hardflowg.h>
#include <niusbgpib.h>

// Прямой доступ к gpib-32.dll
//#define GPIB_DIRECT_ACCESS

#if defined(IRS_WIN32)
extern "C" {
#include <Decl-32.h>
}
#endif // IRS_WIN32

#include <irsfinal.h>

#ifndef IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE
# define IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE IRS_LIB_DEBUG_NONE
#endif // !IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE

#if (IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
# define IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_NIUSBGPIBHF_DBG_MSG_DETAIL(msg)
#elif (IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
# define IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_NIUSBGPIBHF_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG(msg)
#else // IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE == IRS_LIB_DEBUG_NONE
# define IRS_LIB_NIUSBGPIBHF_DBG_MSG_DETAIL(msg)
# define IRS_LIB_NIUSBGPIBHF_DBG_MSG_BASE(msg)
#endif // IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE == IRS_LIB_DEBUG_NONE

namespace irs {

namespace hardflow {

//! \addtogroup drivers_group
//! @{

#if defined(IRS_WIN32)

//#define SYNCHRONOUS_IO

//! \brief Класс для работы с National Instruments USB-GPIB
//!   в асинхронном режиме
//! \author Lyashchov Maxim
class ni_usb_gpib_flow_t: public hardflow_t
{
public:
  ni_usb_gpib_flow_t(
    const size_type a_board_index,
    const size_type a_gpib_adress,
    const double a_session_read_timeout_s = 1,
    const double a_session_write_timeout_s = 1,
    const string_type& a_dll_file_name = string_type()
  );
  virtual ~ni_usb_gpib_flow_t();
  virtual string_type param(const string_type&);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
private:
  void proc_read();
  void proc_write();
  void proc_wait();
  //enum { m_gpib_index = 1 };
  enum { m_no_secondary_addr = 0 };
  enum { m_timeout = T10s };
  enum { m_eotmode = 1 };
  enum { m_eosmode = 1 };
  enum process_t {
    process_wait,
    process_read,
    process_write
  };   
  process_t m_process;
  typedef void (ni_usb_gpib_flow_t::*p_process)();
  vector<p_process> m_proc_array;
  ni_usb_gpib_t m_ni_usb_gpib;
  size_type m_device_id;
  size_type m_channel;
  raw_data_t<irs_u8> m_read_buf;
  raw_data_t<irs_u8> m_write_buf;
  double m_session_read_timeout;
  double m_session_write_timeout;
  timer_t m_session_write_timer;
  timer_t m_session_read_timer;
  // Это значение нужно чтобы таймер срабатывал не раньше, чем произойдет
  // тайм-аут операции GPIB
  const double m_timeout_delta;
};

//! \brief Класс для работы с National Instruments USB-GPIB
//!   в синхронном режиме
//! \author Lyashchov Maxim
class ni_usb_gpib_flow_syn_t: public hardflow_t
{
public:
  ni_usb_gpib_flow_syn_t(
    const size_type a_board_index,
    const size_type a_gpib_adress,
    const double read_timeout_s = 100e-3,
    const double write_timeout_s = 100e-3
  );
  virtual ~ni_usb_gpib_flow_syn_t();
  virtual string_type param(const string_type &);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
private:
  enum { m_no_secondary_addr = 0 };   
  enum { m_eotmode = 1 };
  enum { m_eosmode = 1 };
  ni_usb_gpib_t m_ni_usb_gpib;
  size_type m_device_id;
  size_type m_channel;
  double m_read_timeout;
  double m_write_timeout;
};

#endif // defined(IRS_WIN32)

//! @}

} // namespace hardflow_t

} // namespace irs

#endif // niusbgpib_hardflowH
