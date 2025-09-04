//! \file
//! \ingroup drivers_group
//! \brief Классы для работы с приборами
//!
//! Дата создания: 05.07.2013

#ifndef irsdevicesH
#define irsdevicesH

#include <irsdefs.h>

#include <tstlan4abs.h>
#include <hardflowg.h>
#include <mxdata.h>
#include <measmul.h>
#include <irsparamabs.h>
#include <simple_ftp_client.h>

#include <iptypes.h>
#include <iphlpapi.h>

#ifdef __BORLANDC__
#include <winsock2.h>
#endif // __BORLANDC__

#include <irsfinal.h>

namespace irs {

//! \addtogroup network_in_out_group
//! @{

class mxdata_assembly_t
{
public:
  typedef irs::string_t string_type;
  typedef deque<string_type> error_string_list_type;
  class options_base_t
  {
  public:
    typedef mxdata_assembly_t::string_type string_type;
    virtual ~options_base_t()
    {
    }
    virtual string_type get_option(const string_type& a_option_name) const = 0;
    virtual bool set_option(const string_type& a_option_name,
      const string_type& a_option_value) = 0;
  };
  typedef options_base_t options_type;
  enum status_t {
    //! \brief Статус не поддерживается
    //! \details Этот статус выдается, если работа со статусами не
    //!   поддерживается. Он выдается по умолчанию
    status_not_supported,
    //! \brief Соединение выключено
    status_disabled,
    //! \brief Соединение установлено
    status_connected,
    //! \brief Ожидание соединения
    //! \details Этот статус следует выдавать даже тогда, когда происходит
    //!   ошибка соединения, но это не мешает циклически пытаться установить
    //!   соединение
    status_busy,
    //! \brief Произошла ошибка и попыток соединения больше не происходит
    status_error
  };
  virtual ~mxdata_assembly_t() {}
  virtual bool enabled() const = 0;
  virtual void enabled(bool a_enabled) = 0;
  virtual irs::mxdata_t* mxdata() = 0;
  virtual void tick() = 0;
  virtual void show_options() = 0;
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4) = 0;
  virtual options_type* options();
  virtual status_t get_status();
  virtual error_string_list_type get_last_error_string_list();
};

class param_box_base_options_t: public mxdata_assembly_t::options_type
{
public:
  param_box_base_options_t(param_box_base_t* ap_param_box_base):
    mp_param_box_base(ap_param_box_base)
  {
  }
  virtual string_type get_option(const string_type& a_option_name) const
  {
    return mp_param_box_base->get_param(a_option_name);
  }
  virtual bool set_option(const string_type& a_option_name,
    const string_type& a_option_value)
  {
    return mp_param_box_base->set_param(a_option_name, a_option_value);
  }
private:
  param_box_base_options_t();
  param_box_base_t* mp_param_box_base;
};

class mxdata_assembly_params_t
{
public:
  typedef mxdata_assembly_t::string_type string_type;

  mxdata_assembly_params_t(string_type a_ini_name);
};

class mxdata_assembly_creator_t
{
public:
  typedef mxdata_assembly_t::string_type string_type;

  virtual ~mxdata_assembly_creator_t() {}
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name) = 0;
private:
};

#ifdef __BORLANDC__

class ip_collector_t
{
public:
  typedef std::string string_t;
  typedef std::vector<string_t> answer_type_t;

  ~ip_collector_t();

  void send_request(string_t a_request_string, uint16_t a_port);
  void tick();
  bool are_ip_collected();
  const answer_type_t& get_answers();

  static ip_collector_t* get_instance()
  {
    static ip_collector_t ip_collector;
    return &ip_collector;
  }

private:
  typedef std::vector<std::pair<mxip_t, mxip_t> > if_address_t;
  typedef std::vector<std::pair<string_t, int> > socket_t;

  ip_collector_t();

  if_address_t m_if_addresses;
  socket_t m_sockets;
  answer_type_t m_answers;

  irs::loop_timer_t m_recv_socket_timer;

  irs::timer_t m_collect_ip_timer;

  void get_interfaces_ip();
  void create_sockets();
};

#endif // __BORLANDC__

class modbus_assembly_t: public mxdata_assembly_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  typedef hardflow_t* (*hardflow_create_foo_t)(uint16_t, uint16_t);
  enum protocol_t { udp_protocol, tcp_protocol, usb_hid_protocol };

  modbus_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name,
    protocol_t a_protocol);
  virtual ~modbus_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  std::string param_to_utf8(string_type a_param);
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
  virtual status_t get_status();
  virtual error_string_list_type get_last_error_string_list();
  void set_hardwlof_create_foo(hardflow_create_foo_t ap_hardflow_create_foo);
private:
  struct device_open_data_t
  {
    string_type path;
    uint16_t pid;
    uint16_t vid;

    device_open_data_t(string_type a_path, uint16_t a_pid, uint16_t a_vid) :
      path(a_path),
      pid(a_pid),
      vid(a_vid)
    {}
  };

  void tune_param_box();
  void update_usb_hid_device_path_map();
  void update_param_box_devices_field();
  void add_error(const string_type& a_error);

  string_type m_conf_file_name;
  protocol_t m_protocol;
  handle_t<param_box_base_t> mp_param_box;
  map<string_type, device_open_data_t> m_usb_hid_device_path_map;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  handle_t<hardflow_t> mp_modbus_client_hardflow;
  handle_t<mxdata_t> mp_modbus_client;
  bool m_activated;
  irs::loop_timer_t m_activation_timer;
  enum { error_list_max_size = 100 };
  error_string_list_type m_error_list;
  hardflow_create_foo_t mp_hardflow_create_foo;
  handle_t<simple_ftp_client_t> mp_simple_ftp_client;
  irs::loop_timer_t progress_timer;
  bool m_is_progress_update_on;

  #ifdef __BORLANDC__
  ip_collector_t* mp_ip_collector;
  bool m_wait_response;
  void add_collected_ip();
  #endif // __BORLANDC__

  static handle_t<mxdata_t> make_client(handle_t<hardflow_t> ap_hardflow,
    handle_t<param_box_base_t> ap_param_box);
  void make_simple_ftp_client(handle_t<hardflow_t> ap_hardflow);
  handle_t<hardflow_t> make_hardflow();
  void try_create_modbus();
  void create_modbus();
  void destroy_modbus();
  static string_type protocol_name(protocol_t a_protocol);
};

class mxdata_assembly_types_t
{
public:
  typedef mxdata_assembly_t::string_type string_type;

  virtual ~mxdata_assembly_types_t() {}
  virtual void enum_types(vector<string_type>* ap_types) const = 0;
  virtual handle_t<mxdata_assembly_t> make_assembly(
    const string_type& a_assembly_type, tstlan4_base_t* ap_tstlan4,
    const string_type& a_name, modbus_assembly_t::hardflow_create_foo_t
    a_modbus_hid_create_foo = NULL) = 0;
};

//mxdata_assembly_names_base_t* mxdata_assembly_names();

mxdata_assembly_types_t* mxdata_assembly_types();

class mxmultimeter_assembly_t
{
public:
  typedef string_t string_type;
  virtual ~mxmultimeter_assembly_t() {}
  virtual bool enabled() const = 0;
  virtual void enable(multimeter_mode_type_t a_mul_mode_type) = 0;
  virtual void disable() = 0;
  virtual mxmultimeter_t* mxmultimeter() = 0;
  virtual void tick() = 0;
  virtual void show_options() = 0;
};

class mxmultimeter_assembly_creator_t
{
public:
  typedef mxmultimeter_assembly_t::string_type string_type;

  virtual ~mxmultimeter_assembly_creator_t() {}
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name) = 0;
private:
};

class mxmultimeter_assembly_types_t
{
public:
  typedef mxmultimeter_assembly_t::string_type string_type;

  virtual ~mxmultimeter_assembly_types_t() {}
  virtual void enum_types(vector<string_type>* ap_types) const = 0;
  virtual handle_t<mxmultimeter_assembly_t> make_assembly(
    const string_type& a_assembly_type, const string_type& a_name) = 0;
};

//mxdata_assembly_names_base_t* mxdata_assembly_names();

mxmultimeter_assembly_types_t* mxmultimeter_assembly_types();

//! @}

} // namespace irs

#include <irsdefs.h>

#endif // irsdevicesH
