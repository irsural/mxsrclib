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
#include <niusbgpib.h>
#include <niusbgpib_hardflow.h>
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
#else // Other
irs::handle_t<irs::param_box_base_t> make_assembly_param_box(
  const irs::string_t&,
  const irs::string_t&)
{
  return irs::handle_t<irs::param_box_base_t>();
}
#endif // Other

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
#endif // __BORLANDC__
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
    mp_mxnet_client_hardflow->set_param(irst("remote_address"),
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

class modbus_usb_hid_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class modbus_assembly_t: public mxdata_assembly_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
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
private:
  void tune_param_box();
  void update_usb_hid_device_path_map();
  void update_param_box_devices_field();
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

  static handle_t<mxdata_t> make_client(handle_t<hardflow_t> ap_hardflow,
    handle_t<param_box_base_t> ap_param_box);
  handle_t<hardflow_t> make_hardflow();
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

irs::handle_t<irs::mxdata_assembly_t>
irs::modbus_usb_hid_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(new modbus_assembly_t(ap_tstlan4,
    a_name, modbus_assembly_t::usb_hid_protocol));
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
irs::handle_t<irs::hardflow_t> irs::modbus_assembly_t::make_hardflow()
{
  handle_t<hardflow_t> hardflow_ret = IRS_NULL;
  switch (m_protocol) {
    case udp_protocol: {
      hardflow_ret = hardflow::make_udp_flow_client(
        mp_param_box->get_param(irst("IP")),
        mp_param_box->get_param(irst("Порт")));
    } break;
    case tcp_protocol: {
      hardflow_ret.reset(new hardflow::tcp_client_t(
        make_mxip(mp_param_box->get_param(irst("IP")).c_str()),
        param_box_read_number<irs_u16>(*mp_param_box, irst("Порт"))));
    } break;
    case usb_hid_protocol: {
      const string_type device =
        mp_param_box->get_param(irst("Имя устройства"));
      map<string_type, string_type>::const_iterator it =
        m_usb_hid_device_path_map.find(device);
      string_type device_path;
      if (it != m_usb_hid_device_path_map.end()) {
        device_path = it->second;
      }
      const irs::hardflow_t::size_type channel_id =
        param_box_read_number<irs::hardflow_t::size_type>(
        *mp_param_box, (irst("Номер канала")));
      hardflow_ret.reset(new hardflow::usb_hid_t(device_path, channel_id));
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
    case usb_hid_protocol: {
      protocol_name_ret = irst("USB_HID");
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
  m_usb_hid_device_path_map(),
  //m_param_box_tune(this, mp_param_box.get(), a_protocol),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_modbus_client_hardflow(NULL),
  mp_modbus_client(NULL)
{
  tune_param_box();
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::modbus_assembly_t::~modbus_assembly_t()
{
  mp_param_box->save();
}

void irs::modbus_assembly_t::tune_param_box()
{
  if (m_protocol == usb_hid_protocol) {
    update_usb_hid_device_path_map();
    vector<string_type> devices_items;
    mp_param_box->add_edit(irst("Имя устройства"), string_type());
    mp_param_box->add_combo(irst("Имя устройства"), &devices_items);
    update_param_box_devices_field();
    mp_param_box->add_edit(irst("Номер канала"), irst("1"));
  } else {
    mp_param_box->add_edit(irst("IP"), irst("127.0.0.1"));
    mp_param_box->add_edit(irst("Порт"), irst("5005"));
  }
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

irs::modbus_assembly_t::param_box_tune_t::param_box_tune_t(
  modbus_assembly_t* ap_modbus_assembly,
  param_box_base_t* ap_param_box, protocol_t a_protocol
):
  mp_param_box(ap_param_box)
{
  if (a_protocol == usb_hid_protocol) {
    ap_modbus_assembly->update_usb_hid_device_path_map();
    vector<string_type> devices_items;
    map<string_type, string_type>::const_iterator it =
      ap_modbus_assembly->m_usb_hid_device_path_map.begin();
    while (it != ap_modbus_assembly->m_usb_hid_device_path_map.end()) {
      devices_items.push_back(it->first);
      ++it;
    }
    string_type default_device;
    if (!devices_items.empty()) {
      default_device = devices_items.front();
    }
    mp_param_box->add_edit(irst("Имя устройства"), default_device);
    mp_param_box->add_combo(irst("Имя устройства"), &devices_items);
    mp_param_box->add_edit(irst("Номер канала"), irst("1"));
  } else {
    mp_param_box->add_edit(irst("IP"), irst("127.0.0.1"));
    mp_param_box->add_edit(irst("Порт"), irst("5005"));
  }
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
void irs::modbus_assembly_t::update_usb_hid_device_path_map()
{
  m_usb_hid_device_path_map.clear();
  std::vector<irs::usb_hid_device_info_t> devs =
    irs::usb_hid_info_t::get_instance()->get_devices_info();
  for (size_type i = 0; i < devs.size(); i++) {
    string_type device;
    device += devs[i].attributes.manufacturer;
    if (!device.empty() && !devs[i].attributes.product.empty()) {
      device += irst(" ");
    }
    device += devs[i].attributes.product;
    if (!device.empty() && !devs[i].attributes.serial_number.empty()) {
      device += irst(" ");
    }
    device += devs[i].attributes.serial_number;
    if (!device.empty() && !devs[i].path.empty()) {
      device += irst(" ");
    }
    device += devs[i].path;

    m_usb_hid_device_path_map.insert(make_pair(device, devs[i].path));
  }
}

void irs::modbus_assembly_t::update_param_box_devices_field()
{
  string_type default_device = mp_param_box->get_param(irst("Имя устройства"));
  mp_param_box->clear_combo(irst("Имя устройства"));

  update_usb_hid_device_path_map();
  vector<string_type> devices_items;
  map<string_type, string_type>::const_iterator it =
    m_usb_hid_device_path_map.begin();
  while (it != m_usb_hid_device_path_map.end()) {
    devices_items.push_back(it->first);
    ++it;
  }
  if (default_device.empty() && !devices_items.empty()) {
    default_device = devices_items.front();
  }
  mp_param_box->add_combo(irst("Имя устройства"), &devices_items);
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
    mp_modbus_client_hardflow = make_hardflow();
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
  update_param_box_devices_field();
  if (mp_param_box->show() && m_enabled) {
    if (m_protocol == usb_hid_protocol) {
      mp_modbus_client_hardflow = make_hardflow();
    } else {
      mp_modbus_client_hardflow->set_param(irst("remote_address"),
        mp_param_box->get_param(irst("IP")));
      mp_modbus_client_hardflow->set_param(irst("remote_port"),
        mp_param_box->get_param(irst("Порт")));
    }
    mp_modbus_client = make_client(mp_modbus_client_hardflow, mp_param_box);
    mp_tstlan4->connect(mp_modbus_client.get());
  }
}
void irs::modbus_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}

#if defined(IRS_WIN32)

namespace irs {

struct gpib_param_box_tune_t {
  typedef mxdata_assembly_t::string_type string_type;
  param_box_base_t* mp_param_box;
  gpib_param_box_tune_t(param_box_base_t* ap_param_box);
};

handle_t<irs::hardflow_t> make_gpib_hardflow(
  param_box_base_t* ap_param_box,
  irs::hardflow::prologix_flow_t::end_line_t a_read_end_line,
  irs::hardflow::prologix_flow_t::end_line_t a_write_end_line,
  int a_prologix_timeout_ms
);

void add_gpib_options_to_param_box(param_box_base_t* ap_param_box);

} // namespace irs

void irs::add_gpib_options_to_param_box(param_box_base_t* ap_param_box)
{
  typedef mxdata_assembly_t::string_type string_type;
  vector<string_type> interface_items;
  interface_items.push_back(irst("NI USB-GPIB"));
  interface_items.push_back(irst("Agilent USB-GPIB"));
  interface_items.push_back(irst("Prologix COM-GPIB"));
  interface_items.push_back(irst("Prologix Ethernet-GPIB"));
  ap_param_box->add_edit(irst("Способ подключения"), interface_items.front());
  ap_param_box->add_combo(irst("Способ подключения"), &interface_items);

  ap_param_box->add_edit(irst("GPIB индекс"), irst("0"));
  ap_param_box->add_edit(irst("GPIB адрес"), irst("23"));
  ap_param_box->add_edit(irst("COM name"), irst("com1"));
  ap_param_box->add_edit(irst("IP адрес"), irst("127.0.0.1"));
  ap_param_box->add_edit(irst("Порт"), irst("1234"));
}

irs::handle_t<irs::hardflow_t> irs::make_gpib_hardflow(
  param_box_base_t* ap_param_box,
  irs::hardflow::prologix_flow_t::end_line_t a_read_end_line,
  irs::hardflow::prologix_flow_t::end_line_t a_write_end_line,
  int a_prologix_timeout_ms
  )
{
  typedef mxdata_assembly_t::string_type string_type;
  const string_type interface_name = ap_param_box->get_param(
    irst("Способ подключения"));
  const int gpib_index = param_box_read_number<int>(*ap_param_box,
    irst("GPIB индекс"));
  const int gpib_address = param_box_read_number<int>(*ap_param_box,
    irst("GPIB адрес"));
  const string_type com_name = ap_param_box->get_param(irst("COM name"));

  irs::handle_t<irs::hardflow_t> gpib_hardflow;
  if (interface_name == irst("NI USB-GPIB")) {
    const string_type dll_name = get_ni_usb_gpib_dll_file_name(ni_fn_gpib_32);
    gpib_hardflow.reset(new irs::hardflow::ni_usb_gpib_flow_t(
      gpib_index, gpib_address, 1, 1, dll_name));
  } else if (interface_name == irst("Agilent USB-GPIB")) {
    const string_type dll_name = get_ni_usb_gpib_dll_file_name(ni_fn_agtgpib32);
    gpib_hardflow.reset(new irs::hardflow::ni_usb_gpib_flow_t(
      gpib_index, gpib_address, 1, 1, dll_name));
  } else if (interface_name == irst("Prologix COM-GPIB")) {
    gpib_hardflow.reset(new irs::hardflow::prologix_flow_t(
      new irs::hardflow::com_flow_t(
      com_name, CBR_4800, FALSE, NOPARITY, 8,
      ONESTOPBIT, DTR_CONTROL_DISABLE),
      gpib_address, a_read_end_line, a_write_end_line, a_prologix_timeout_ms));
  } else {
    const string_type ip_str = ap_param_box->get_param(irst("IP адрес"));
    const string_type port_str = ap_param_box->get_param(irst("Порт"));
    mxip_t ip;
    if (!str_to_mxip(ip_str, &ip)) {
      throw std::runtime_error("Не удалось преобразовать ip-адрес");
    }
    irs_u16 port = 0;
    if (!str_to_num(port_str, &port)) {
      throw std::runtime_error("Не удалось преобразовать порт");
    }
    gpib_hardflow.reset(new irs::hardflow::prologix_flow_t(
      new irs::hardflow::tcp_client_t(ip, port),
      gpib_address, a_read_end_line, a_write_end_line, a_prologix_timeout_ms));
  }
  return gpib_hardflow;
}

#endif // IRS_WIN32

#if defined(IRS_WIN32)
namespace irs {

class agilent_3458a_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class agilent_3458a_assembly_t: public mxdata_assembly_t
{
public:
  agilent_3458a_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~agilent_3458a_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  void reset();
  void disable();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
  handle_t<multimeter_mxdata_t> mp_mxdata;
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t>
irs::agilent_3458a_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new agilent_3458a_assembly_t(ap_tstlan4, a_name));
}

irs::agilent_3458a_assembly_t::agilent_3458a_assembly_t(
  tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("Agilent 3458A"),
    m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_hardflow(),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::agilent_3458a_assembly_t::~agilent_3458a_assembly_t()
{
  mp_param_box->save();
}
irs::agilent_3458a_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::agilent_3458a_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::agilent_3458a_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    reset();
  } else {
    disable();
  }
  m_enabled = a_enabled;
}
void irs::agilent_3458a_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
  mp_hardflow.reset();
}

void irs::agilent_3458a_assembly_t::reset()
{
  disable();
  typedef irs::hardflow::prologix_flow_t prologix_flow_type;
  const prologix_flow_type::end_line_t read_end_line =
    prologix_flow_type::cr_lf;
  const prologix_flow_type::end_line_t write_end_line =
    prologix_flow_type::cr_lf;
  const int prologix_timeout_ms = 30;
  mp_hardflow = make_gpib_hardflow(mp_param_box.get(), read_end_line,
    write_end_line, prologix_timeout_ms);
  mp_multimeter.reset(new irs::agilent_3458a_t(mp_hardflow.get(),
    mul_mode_type_passive));

  const int update_time_ms =
    param_box_read_number<int>(*mp_param_box, irst("Время обновления, мс"));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    update_time_ms/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::agilent_3458a_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::agilent_3458a_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
    mp_mxdata->tick();
  }
}
void irs::agilent_3458a_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::agilent_3458a_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}
#endif // defined(IRS_WIN32)

#if defined(IRS_WIN32)
namespace irs {

class agilent_34420a_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class agilent_34420a_assembly_t: public mxdata_assembly_t
{
public:
  agilent_34420a_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~agilent_34420a_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  void reset();
  void disable();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
  handle_t<multimeter_mxdata_t> mp_mxdata;
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t>
irs::agilent_34420a_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new agilent_34420a_assembly_t(ap_tstlan4, a_name));
}

irs::agilent_34420a_assembly_t::agilent_34420a_assembly_t(
  tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("Agilent 34420A"),
    m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_hardflow(),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::agilent_34420a_assembly_t::~agilent_34420a_assembly_t()
{
  mp_param_box->save();
}
irs::agilent_34420a_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::agilent_34420a_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::agilent_34420a_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    reset();
  } else {
    disable();
  }
  m_enabled = a_enabled;
}
void irs::agilent_34420a_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
  mp_hardflow.reset();
}

void irs::agilent_34420a_assembly_t::reset()
{
  disable();
  typedef irs::hardflow::prologix_flow_t prologix_flow_type;
  const prologix_flow_type::end_line_t read_end_line =
    prologix_flow_type::lf;
  const prologix_flow_type::end_line_t write_end_line =
    prologix_flow_type::cr;
  const int prologix_timeout_ms = 3000;
  mp_hardflow = make_gpib_hardflow(mp_param_box.get(), read_end_line,
    write_end_line, prologix_timeout_ms);
  mp_multimeter.reset(new irs::agilent_34420a_t(mp_hardflow.get(),
    mul_mode_type_passive));

  const int update_time_ms =
    param_box_read_number<int>(*mp_param_box, irst("Время обновления, мс"));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    update_time_ms/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::agilent_34420a_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::agilent_34420a_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
    mp_mxdata->tick();
  }
}
void irs::agilent_34420a_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::agilent_34420a_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}
#endif // defined(IRS_WIN32)

#if defined(IRS_WIN32)
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
  void disable();
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
    disable();
  }
  m_enabled = a_enabled;
}
void irs::v7_78_1_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
}
void irs::v7_78_1_assembly_t::reset()
{
  disable();
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
    mp_multimeter->tick();
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
#endif // defined(IRS_WIN32)

#if defined(IRS_WIN32)
namespace irs {

class ch3_85_3r_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class ch3_85_3r_assembly_t: public mxdata_assembly_t
{
public:
  ch3_85_3r_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~ch3_85_3r_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  void reset();
  void disable();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
  handle_t<multimeter_mxdata_t> mp_mxdata;
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t> irs::ch3_85_3r_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new ch3_85_3r_assembly_t(ap_tstlan4, a_name));
}

irs::ch3_85_3r_assembly_t::ch3_85_3r_assembly_t(tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("Ч3-85\/3R"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_hardflow(),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::ch3_85_3r_assembly_t::~ch3_85_3r_assembly_t()
{
  mp_param_box->save();
}
irs::ch3_85_3r_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_edit(irst("COM name"), irst("com1"));
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::ch3_85_3r_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::ch3_85_3r_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    reset();
  } else {
    disable();
  }
  m_enabled = a_enabled;
}
void irs::ch3_85_3r_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
  mp_hardflow.reset();
}
void irs::ch3_85_3r_assembly_t::reset()
{
  disable();
  const string_type com_name = mp_param_box->get_param(irst("COM name"));

  mp_hardflow.reset(new irs::hardflow::com_flow_t(
    com_name, CBR_9600, FALSE, NOPARITY, 8,
    ONESTOPBIT, DTR_CONTROL_DISABLE));

  mp_multimeter.reset(
    new irs::akip_ch3_85_3r_t(mp_hardflow.get(), mul_mode_type_passive));

  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    param_box_read_number<int>(*mp_param_box,
    irst("Время обновления, мс"))/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}

irs::mxdata_t* irs::ch3_85_3r_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::ch3_85_3r_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
    mp_mxdata->tick();
  }
}
void irs::ch3_85_3r_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::ch3_85_3r_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}
#endif // defined(IRS_WIN32)

#if defined(IRS_WIN32)
namespace irs {

class ni_pxi_4071_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class ni_pxi_4071_assembly_t: public mxdata_assembly_t
{
public:
  ni_pxi_4071_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~ni_pxi_4071_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  void reset();
  void disable();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
  handle_t<multimeter_mxdata_t> mp_mxdata;
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t> irs::ni_pxi_4071_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new ni_pxi_4071_assembly_t(ap_tstlan4, a_name));
}

irs::ni_pxi_4071_assembly_t::ni_pxi_4071_assembly_t(tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("NI PXI 4071"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_hardflow(),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::ni_pxi_4071_assembly_t::~ni_pxi_4071_assembly_t()
{
  mp_param_box->save();
}
irs::ni_pxi_4071_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  vector<string_type> protocol_items;
  protocol_items.push_back(irst("TCP"));
  protocol_items.push_back(irst("UDP"));
  mp_param_box->add_edit(irst("Протокол"), protocol_items.front());
  mp_param_box->add_combo(irst("Протокол"), &protocol_items);
  mp_param_box->add_edit(irst("IP адрес"), irst("127.0.0.1"));
  mp_param_box->add_edit(irst("Порт"), irst("1234"));
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::ni_pxi_4071_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::ni_pxi_4071_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    reset();
  } else {
    disable();
  }
  m_enabled = a_enabled;
}
void irs::ni_pxi_4071_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
  mp_hardflow.reset();
}
void irs::ni_pxi_4071_assembly_t::reset()
{
  disable();
  const string_type protocol_name = mp_param_box->get_param(irst("Протокол"));
  const string_type ip_str = mp_param_box->get_param(irst("IP адрес"));
  const string_type port_str = mp_param_box->get_param(irst("Порт"));
  if (protocol_name == irst("TCP")) {
    mxip_t ip;
    if (!str_to_mxip(ip_str, &ip)) {
      throw std::runtime_error("Не удалось преобразовать ip-адрес");
    }
    irs_u16 port = 0;
    if (!str_to_num(port_str, &port)) {
      throw std::runtime_error("Не удалось преобразовать порт");
    }
    mp_hardflow.reset(new irs::hardflow::tcp_client_t(ip, port));
  } else {
    mp_hardflow.reset(new irs::hardflow::udp_flow_t(irst(""), irst(""),
      ip_str, port_str));
  }

  const int update_time_ms =
    param_box_read_number<int>(*mp_param_box, irst("Время обновления, мс"));

  counter_t update_time = irs::make_cnt_ms(update_time_ms);
  filter_settings_t filter_settings;
  mp_multimeter.reset(new irs::ni_pxi_4071_t(mp_hardflow.get(),
    filter_settings, update_time, mul_mode_type_passive));

  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    update_time_ms/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::ni_pxi_4071_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::ni_pxi_4071_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
    mp_mxdata->tick();
  }
}
void irs::ni_pxi_4071_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::ni_pxi_4071_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}
#endif // defined(IRS_WIN32)

#if defined(IRS_WIN32)
namespace irs {

class termex_lt_300_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class termex_lt_300_assembly_t: public mxdata_assembly_t
{
public:
  termex_lt_300_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~termex_lt_300_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  void reset();
  void disable();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  tstlan4_base_t* mp_tstlan4;
  bool m_enabled;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
  handle_t<multimeter_mxdata_t> mp_mxdata;
};

} // namespace irs

irs::handle_t<irs::mxdata_assembly_t>
irs::termex_lt_300_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new termex_lt_300_assembly_t(ap_tstlan4, a_name));
}

irs::termex_lt_300_assembly_t::termex_lt_300_assembly_t(
  tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(
    irst("Termex ЛТ-300"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_hardflow(),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::termex_lt_300_assembly_t::~termex_lt_300_assembly_t()
{
  mp_param_box->save();
}
irs::termex_lt_300_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_edit(irst("COM name"), irst("com1"));
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::termex_lt_300_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::termex_lt_300_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    reset();
  } else {
    disable();
  }
  m_enabled = a_enabled;
}
void irs::termex_lt_300_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
  mp_hardflow.reset();
}
void irs::termex_lt_300_assembly_t::reset()
{
  disable();
  const string_type com_name = mp_param_box->get_param(irst("COM name"));

  mp_hardflow.reset(new irs::hardflow::com_flow_t(
    com_name, CBR_4800, FALSE, NOPARITY, 8,
    ONESTOPBIT, DTR_CONTROL_ENABLE));
  mp_hardflow->set_param(irst("f_rts_control"),
    irs::num_to_str(RTS_CONTROL_DISABLE));
  mp_multimeter.reset(new irs::termex_lt_300_t(mp_hardflow.get()));

  const int update_time_ms =
    param_box_read_number<int>(*mp_param_box, irst("Время обновления, мс"));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    update_time_ms/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::termex_lt_300_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::termex_lt_300_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
    mp_mxdata->tick();
  }
}
void irs::termex_lt_300_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::termex_lt_300_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}
#endif // defined(IRS_WIN32)

namespace irs {

class test_multimeter_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class test_multimeter_assembly_t: public mxdata_assembly_t
{
public:
  test_multimeter_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~test_multimeter_assembly_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  virtual void show_options();
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4);
private:
  void reset();
  void disable();
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

irs::handle_t<irs::mxdata_assembly_t>
irs::test_multimeter_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new test_multimeter_assembly_t(ap_tstlan4, a_name));
}

irs::test_multimeter_assembly_t::test_multimeter_assembly_t(
  tstlan4_base_t* ap_tstlan4,
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(
    irst("Тестовый мультиметр"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::test_multimeter_assembly_t::~test_multimeter_assembly_t()
{
  mp_param_box->save();
}
irs::test_multimeter_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();
}
bool irs::test_multimeter_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::test_multimeter_assembly_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    reset();
  } else {
    disable();
  }
  m_enabled = a_enabled;
}
void irs::test_multimeter_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
}
void irs::test_multimeter_assembly_t::reset()
{
  disable();

  mp_multimeter.reset(new irs::dummy_multimeter_t());

  const int update_time_ms =
    param_box_read_number<int>(*mp_param_box, irst("Время обновления, мс"));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    update_time_ms/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::test_multimeter_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::test_multimeter_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_multimeter->tick();
    mp_mxdata->tick();
  }
}
void irs::test_multimeter_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::test_multimeter_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}

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
  m_ac_list[irst("modbus usb hid")] = handle_t<mxdata_assembly_creator_t>(
    new modbus_usb_hid_assembly_creator_t);
  m_ac_list[irst("Agilent 3458A")] = handle_t<mxdata_assembly_creator_t>(
    new agilent_3458a_assembly_creator_t);
  m_ac_list[irst("Agilent 34420A")] = handle_t<mxdata_assembly_creator_t>(
    new agilent_34420a_assembly_creator_t);
  m_ac_list[irst("В7-78\/1")] = handle_t<mxdata_assembly_creator_t>(
    new v7_78_1_assembly_creator_t);
  m_ac_list[irst("Ч3-85\/3R")] = handle_t<mxdata_assembly_creator_t>(
    new ch3_85_3r_assembly_creator_t);
  m_ac_list[irst("NI PXI 4071")] = handle_t<mxdata_assembly_creator_t>(
    new ni_pxi_4071_assembly_creator_t);
  m_ac_list[irst("Termex ЛТ-300")] = handle_t<mxdata_assembly_creator_t>(
    new termex_lt_300_assembly_creator_t);
  #endif // __BORLANDC__
  m_ac_list[irst("Тестовый мультиметр")] =
    handle_t<mxdata_assembly_creator_t>(new test_multimeter_assembly_creator_t);
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
