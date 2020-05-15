//! \file
//! \ingroup drivers_group
//! \brief ������ ��� ������ � ���������
//!
//! ���� ��������: 05.07.2013

#ifndef irsdevicesH
#define irsdevicesH

#include <irsdefs.h>

#include <tstlan4abs.h>
#include <hardflowg.h>
#include <mxdata.h>
#include <measmul.h>
#include <irsparamabs.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup network_in_out_group
//! @{

class mxdata_assembly_t
{
public:
  typedef string_t string_type;
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
    //! \brief ������ �� ��������������
    //! \details ���� ������ ��������, ���� ������ �� ��������� ��
    //!   ��������������. �� �������� �� ���������
    status_not_supported,
    //! \brief ���������� ���������
    status_disabled,
    //! \brief ���������� �����������
    status_connected,
    //! \brief �������� ����������
    //! \details ���� ������ ������� �������� ���� �����, ����� ����������
    //!   ������ ����������, �� ��� �� ������ ���������� �������� ����������
    //!   ����������
    status_busy,
    //! \brief ��������� ������ � ������� ���������� ������ �� ����������
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


class modbus_assembly_t: public mxdata_assembly_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  typedef hardflow_t* (*hardflow_create_foo_t)(string_type, size_type);
  enum protocol_t { udp_protocol, tcp_protocol, usb_hid_protocol };

  modbus_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name,
    protocol_t a_protocol);
  virtual ~modbus_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
  virtual status_t get_status();
  virtual error_string_list_type get_last_error_string_list();
  void set_hardwlof_create_foo(hardflow_create_foo_t ap_hardflow_create_foo);
private:
  void tune_param_box();
  void update_usb_hid_device_path_map();
  void update_param_box_devices_field();
  void add_error(const string_type& a_error);
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;
    param_box_tune_t(modbus_assembly_t* ap_modbus_assembly,
      param_box_base_t* ap_param_box, protocol_t a_protocol);
  };
  friend struct param_box_tune_t;
  string_type m_conf_file_name;
  protocol_t m_protocol;
  handle_t<param_box_base_t> mp_param_box;
  map<string_type, string_type> m_usb_hid_device_path_map;
  //param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  handle_t<hardflow_t> mp_modbus_client_hardflow;
  handle_t<mxdata_t> mp_modbus_client;
  bool m_activated;
  irs::loop_timer_t m_activation_timer;
  enum { error_list_max_size = 100 };
  error_string_list_type m_error_list;
  hardflow_create_foo_t mp_hardflow_create_foo;

  static handle_t<mxdata_t> make_client(handle_t<hardflow_t> ap_hardflow,
    handle_t<param_box_base_t> ap_param_box);
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
	a_modbus_hid_create_foo=NULL) = 0;
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
