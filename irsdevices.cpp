//! \file
//! \ingroup drivers_group
//! \brief Классы для работы с приборами
//!
//! Дата создания: 05.07.2013

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsparamabs.h>
#include <irsmbus.h>
#include <mxnetc.h>
#include <measmul.h>
#ifdef __BORLANDC__
# include <cbsysutils.h>
# include <mxini.h>
# include <irsparam.h>
#endif // __BORLANDC__

#include <irsdevices.h>

#include <irsfinal.h>

namespace {

#ifdef __BORLANDC__
irs::handle_t<irs::param_box_base_t> make_assembly_param_box(
  const irs::string_t& a_assembly_type_name,
  const irs::string_t& a_assembly_name)
{
  irs::handle_t<irs::param_box_base_t> param_box = new irs::param_box_t(
    irst("Настройки ") + a_assembly_type_name +
      irst(" - ") + a_assembly_name,
    irst("device"),
    irst(""),
    a_assembly_name
  );
  return param_box;
}
#endif // __BORLANDC__

} // unnamed namespace

namespace irs {

class mxdata_assembly_params_container_t
{
public:
  typedef mxdata_assembly_t::string_type string_type;

  mxdata_assembly_params_container_t();
  void ini_name(const string_type& a_ini_name);
  string_type ini_name() const;
private:
  string_type m_ini_name;
};
mxdata_assembly_params_container_t::mxdata_assembly_params_container_t():
  m_ini_name(irs::default_ini_name())
{
}
void mxdata_assembly_params_container_t::ini_name(
  const string_type& a_ini_name)
{
  m_ini_name = a_ini_name;
}
mxdata_assembly_params_container_t::string_type
  mxdata_assembly_params_container_t::ini_name() const
{
  return m_ini_name;
}

mxdata_assembly_params_container_t* mxdata_assembly_params()
{
  static mxdata_assembly_params_container_t mxdata_assembly_params_container;
  return &mxdata_assembly_params_container;
}

mxdata_assembly_params_t::mxdata_assembly_params_t(
  mxdata_assembly_t::string_type a_ini_name)
{
  mxdata_assembly_params()->ini_name(a_ini_name);
}

} // namespace irs {

#ifdef __BORLANDC__

namespace irs {

class mxnet_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class mxnet_assembly_t: public mxdata_assembly_t
{
public:
  mxnet_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~mxnet_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  handle_t<hardflow_t> mp_mxnet_client_hardflow;
  handle_t<mxdata_t> mp_mxnet_client;
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t> irs::mxnet_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(new mxnet_assembly_t(ap_tstlan4, a_name));
}

irs::mxnet_assembly_t::mxnet_assembly_t(tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("mxnet"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_mxnet_client_hardflow(),
  mp_mxnet_client()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::mxnet_assembly_t::~mxnet_assembly_t()
{
  mp_param_box->save();
}
irs::mxnet_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_edit(irst("IP"), irst("127.0.0.1"));
  mp_param_box->add_edit(irst("Порт"), irst("5005"));
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::mxnet_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::mxnet_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    mp_mxnet_client_hardflow = irs::hardflow::make_udp_flow_client(
      mp_param_box->get_param(irst("IP")),
      mp_param_box->get_param(irst("Порт")));
    mp_mxnet_client.reset(new irs::mxnet_client_t(*mp_mxnet_client_hardflow,
      make_cnt_ms(param_box_read_number<int>(*mp_param_box,
      irst("Время обновления, мс")))));
    mp_tstlan4->connect(mp_mxnet_client.get());
  } else {
    mp_tstlan4->connect(NULL);
    mp_mxnet_client.reset();
    mp_mxnet_client_hardflow.reset();
  }
  m_enabled = a_enabled;
}
irs::mxdata_t* irs::mxnet_assembly_t::mxdata()
{
  return mp_mxnet_client.get();
}
void irs::mxnet_assembly_t::tick()
{
  if (!mp_mxnet_client.is_empty()) {
    mp_mxnet_client->tick();
  }
}
void irs::mxnet_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    mp_mxnet_client_hardflow->set_param(irst("remote_adress"),
      mp_param_box->get_param(irst("IP")));
    mp_mxnet_client_hardflow->set_param(irst("remote_port"),
      mp_param_box->get_param(irst("Порт")));
    mxnet_client_t* p_mxnet_client =
      static_cast<mxnet_client_t*>(mp_mxnet_client.get());
    p_mxnet_client->update_time(make_cnt_ms(
      param_box_read_number<int>(*mp_param_box,
      irst("Время обновления, мс"))));
  }
}
void irs::mxnet_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}

namespace irs {

class modbus_udp_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class modbus_tcp_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class modbus_assembly_t: public mxdata_assembly_t
{
public:
  enum protocol_t { udp_protocol, tcp_protocol };

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
private:
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };
  string_type m_conf_file_name;
  protocol_t m_protocol;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  handle_t<hardflow_t> mp_modbus_client_hardflow;
  handle_t<mxdata_t> mp_modbus_client;

  static handle_t<mxdata_t> make_client(handle_t<hardflow_t> ap_hardflow,
    handle_t<param_box_base_t> ap_param_box);
  static handle_t<hardflow_t> make_hardflow(protocol_t a_protocol,
    handle_t<param_box_base_t> ap_param_box);
  static string_type protocol_name(protocol_t a_protocol);
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t> irs::modbus_udp_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(new modbus_assembly_t(ap_tstlan4,
    a_name, modbus_assembly_t::udp_protocol));
}

irs::handle_t<irs::mxdata_assembly_t> irs::modbus_tcp_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(new modbus_assembly_t(ap_tstlan4,
    a_name, modbus_assembly_t::tcp_protocol));
}

irs::handle_t<irs::mxdata_t> irs::modbus_assembly_t::make_client(
  handle_t<hardflow_t> ap_hardflow,
  handle_t<param_box_base_t> ap_param_box)
{
  return new irs::modbus_client_t(
    ap_hardflow.get(),
    modbus_client_t::mode_refresh_auto,
    param_box_read_number<size_t>(*ap_param_box,
      irst("Биты, только чтение (Discret inputs), байт")),
    param_box_read_number<size_t>(*ap_param_box,
      irst("Биты, чтение/запись (Coils), байт")),
    param_box_read_number<size_t>(*ap_param_box,
      irst("Регистры, чтение/запись (Holding Registers), кол-во")),
    param_box_read_number<size_t>(*ap_param_box,
      irst("Регистры, только чтение (Input Registers), кол-во")),
    make_cnt_ms(param_box_read_number<int>(*ap_param_box,
      irst("Время обновления, мс")))
  );
}
irs::handle_t<irs::hardflow_t> irs::modbus_assembly_t::make_hardflow(
  protocol_t a_protocol,
  handle_t<param_box_base_t> ap_param_box)
{
  handle_t<hardflow_t> hardflow_ret = IRS_NULL;
  switch (a_protocol) {
    case udp_protocol: {
      hardflow_ret = hardflow::make_udp_flow_client(
        ap_param_box->get_param(irst("IP")),
        ap_param_box->get_param(irst("Порт")));
    } break;
    case tcp_protocol: {
      hardflow_ret.reset(new hardflow::tcp_client_t(
        make_mxip(ap_param_box->get_param(irst("IP")).c_str()),
        param_box_read_number<irs_u16>(*ap_param_box, irst("Порт"))));
    } break;
  }
  return hardflow_ret;
}
irs::modbus_assembly_t::string_type irs::modbus_assembly_t::protocol_name(
  protocol_t a_protocol)
{
  modbus_assembly_t::string_type protocol_name_ret = irst("***");
  switch (a_protocol) {
    case udp_protocol: {
      protocol_name_ret = irst("UDP");
    } break;
    case tcp_protocol: {
      protocol_name_ret = irst("TCP");
    } break;
  }
  return protocol_name_ret;
}
irs::modbus_assembly_t::modbus_assembly_t(tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name, protocol_t a_protocol
):
  m_conf_file_name(a_conf_file_name),
  m_protocol(a_protocol),
  mp_param_box(make_assembly_param_box(irst("MODBUS ") +
    protocol_name(a_protocol), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_modbus_client_hardflow(NULL),
  mp_modbus_client(NULL)
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::modbus_assembly_t::~modbus_assembly_t()
{
  mp_param_box->save();
}
irs::modbus_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_edit(irst("IP"), irst("127.0.0.1"));
  mp_param_box->add_edit(irst("Порт"), irst("5005"));
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->add_edit(irst("Биты, только чтение (Discret inputs), байт"),
    irst("0"));
  mp_param_box->add_edit(irst("Биты, чтение/запись (Coils), байт"),
    irst("0"));
  mp_param_box->add_edit(irst("Регистры, чтение/запись ")
    irst("(Holding Registers), кол-во"), irst("10"));
  mp_param_box->add_edit(irst("Регистры, только чтение ")
    irst("(Input Registers), кол-во"), irst("0"));
  mp_param_box->load();
}
bool irs::modbus_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::modbus_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    mp_modbus_client_hardflow = make_hardflow(m_protocol, mp_param_box);
    mp_modbus_client = make_client(mp_modbus_client_hardflow, mp_param_box);
    mp_tstlan4->connect(mp_modbus_client.get());
  } else {
    mp_tstlan4->connect(NULL);
    mp_modbus_client.reset();
    mp_modbus_client_hardflow.reset();
  }
  m_enabled = a_enabled;
}
irs::mxdata_t* irs::modbus_assembly_t::mxdata()
{
  return mp_modbus_client.get();
}
void irs::modbus_assembly_t::tick()
{
  if (!mp_modbus_client.is_empty()) {
    mp_modbus_client->tick();
  }
}
void irs::modbus_assembly_t::show_options()
{
  if (mp_param_box->show()) {
    mp_modbus_client_hardflow->set_param(irst("remote_adress"),
      mp_param_box->get_param(irst("IP")));
    mp_modbus_client_hardflow->set_param(irst("remote_port"),
      mp_param_box->get_param(irst("Порт")));
    mp_modbus_client = make_client(mp_modbus_client_hardflow, mp_param_box);
    mp_tstlan4->connect(mp_modbus_client.get());
  }
}
void irs::modbus_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}

namespace irs {

class v7_78_1_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class v7_78_1_assembly_t: public mxdata_assembly_t
{
public:
  v7_78_1_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~v7_78_1_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  void reset();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  handle_t<mxmultimeter_t> mp_multimeter;
  handle_t<multimeter_mxdata_t> mp_mxdata;
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t> irs::v7_78_1_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new v7_78_1_assembly_t(ap_tstlan4, a_name));
}

irs::v7_78_1_assembly_t::v7_78_1_assembly_t(tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("В7-78\/1"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::v7_78_1_assembly_t::~v7_78_1_assembly_t()
{
  mp_param_box->save();
}
irs::v7_78_1_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_edit(irst("GPIB адрес"), irst("23"));
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::v7_78_1_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::v7_78_1_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    reset();
  } else {
    mp_tstlan4->connect(NULL);
    mp_mxdata.reset();
    mp_multimeter.reset();
  }
  m_enabled = a_enabled;
}
void irs::v7_78_1_assembly_t::reset()
{
  const int gpib_address = param_box_read_number<int>(*mp_param_box,
    irst("GPIB адрес"));
  mp_multimeter.reset(new irs::v7_78_1_t(
    MXIFA_MULTIMETER, gpib_address, mul_mode_type_passive));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    param_box_read_number<int>(*mp_param_box,
    irst("Время обновления, мс"))/1000.));
  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::v7_78_1_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::v7_78_1_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_mxdata->tick();
  }
}
void irs::v7_78_1_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::v7_78_1_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}

#endif // __BORLANDC__

namespace irs {

class mxdata_assembly_types_implementation_t: public mxdata_assembly_types_t
{
public:
  mxdata_assembly_types_implementation_t();
  virtual void enum_types(vector<string_type>* ap_types) const;
  virtual handle_t<mxdata_assembly_t> make_assembly(
    const string_type& a_assembly_type, tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
  typedef map<string_type, handle_t<mxdata_assembly_creator_t> > ac_list_type;
  typedef ac_list_type::iterator ac_list_it_type;
  typedef ac_list_type::const_iterator ac_list_const_it_type;

  ac_list_type m_ac_list;
};

} //namespace irs

irs::mxdata_assembly_types_t* irs::mxdata_assembly_types()
{
  static mxdata_assembly_types_implementation_t
    mxdata_assembly_types_implementation;
  return &mxdata_assembly_types_implementation;
}

irs::mxdata_assembly_types_implementation_t::
  mxdata_assembly_types_implementation_t(
):
  m_ac_list()
{
  #ifdef __BORLANDC__
  m_ac_list[irst("mxnet")] = handle_t<mxdata_assembly_creator_t>(
    new mxnet_assembly_creator_t);
  m_ac_list[irst("modbus udp")] = handle_t<mxdata_assembly_creator_t>(
    new modbus_udp_assembly_creator_t);
  m_ac_list[irst("modbus tcp")] = handle_t<mxdata_assembly_creator_t>(
    new modbus_udp_assembly_creator_t);
  m_ac_list[irst("В7-78\/1")] = handle_t<mxdata_assembly_creator_t>(
    new v7_78_1_assembly_creator_t);
  #endif // __BORLANDC__
}
void irs::mxdata_assembly_types_implementation_t::
  enum_types(vector<string_type>* ap_types) const
{
  ap_types->clear();
  for (ac_list_const_it_type it = m_ac_list.begin();
    it != m_ac_list.end(); it++)
  {
    ap_types->push_back(it->first);
  }
}
irs::handle_t<irs::mxdata_assembly_t>
  irs::mxdata_assembly_types_implementation_t::make_assembly
(
  const string_type& a_assembly_type, tstlan4_base_t* ap_tstlan4,
  const string_type& a_name)
{
  handle_t<mxdata_assembly_t> result_assembly(IRS_NULL);
  ac_list_it_type it = m_ac_list.find(a_assembly_type);
  if (it != m_ac_list.end()) {
    result_assembly = IRS_NULL;
    result_assembly = it->second->make(ap_tstlan4, a_name);
  }
  return result_assembly;
}
