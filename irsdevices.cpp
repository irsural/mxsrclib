//! \file
//! \ingroup drivers_group
//! \brief Классы для работы с приборами
//!
//! Дата создания: 05.07.2013

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#pragma warn -8061
#endif // __BORLANDC__

#include <irsparamabs.h>
#include <irsmbus.h>
#include <mxnetc.h>
#include <measmul.h>
#include <niusbgpib.h>
#include <niusbgpib_hardflow.h>
#ifdef __BORLANDC__
#include <cbsysutils.h>
#include <mxini.h>
#include <irsparam.h>
#include <ws2tcpip.h>
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

// class mxdata_assembly_t
mxdata_assembly_t::options_type *irs::mxdata_assembly_t::options()
{
  return NULL;
}

irs::mxdata_assembly_t::status_t irs::mxdata_assembly_t::get_status()
{
  return status_not_supported;
}

irs::mxdata_assembly_t::error_string_list_type
irs::mxdata_assembly_t::get_last_error_string_list()
{
  return error_string_list_type();
}

mxdata_assembly_params_t::mxdata_assembly_params_t(
  mxdata_assembly_t::string_type a_ini_name)
{
  mxdata_assembly_params()->ini_name(a_ini_name);
}

} // namespace irs {


#if defined(IRS_WIN32) || defined(IRS_LINUX)

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
  virtual options_type* options();
private:
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  param_box_base_options_t m_param_box_base_options;
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
  m_param_box_base_options(mp_param_box.get()),
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

irs::mxnet_assembly_t::options_type* irs::mxnet_assembly_t::options()
{
  return &m_param_box_base_options;
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
      #if !IRS_USE_HID_WIN_API
      throw std::logic_error(
        "Не включен IRS_USE_HID_WIN_API. См. mxsrclib desc");
      #endif // !IRS_USE_HID_WIN_API

      #ifdef IRS_WIN32
      const string_type device = mp_param_box->get_param(irst("Имя устройства"));
      map<string_type, device_open_data_t>::const_iterator it =
      m_usb_hid_device_path_map.find(device);
      string_type device_path;
      if (it != m_usb_hid_device_path_map.end()) {
        device_path = it->second.path;
      }
      const irs::hardflow_t::size_type channel_id =
        param_box_read_number<irs::hardflow_t::size_type>(
		*mp_param_box, (irst("Номер канала")));
	  if (mp_hardflow_create_foo == NULL) {
		hardflow_ret.reset(new irs::hardflow::usb_hid_t(device_path, channel_id));
	  } else {
		hardflow_ret.reset(mp_hardflow_create_foo(
		  it->second.pid, it->second.vid));
	  }

      #endif // IRS_WIN32
	} break;
  }
  return hardflow_ret;
}

#ifdef __BORLANDC__

irs::ip_collector_t::ip_collector_t() :
  m_if_addresses(),
  m_sockets(),
  m_answers(),
  m_recv_socket_timer(irs::make_cnt_ms(200)),
  m_collect_ip_timer(irs::make_cnt_ms(1200))
{
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2,2), &wsa) == 0) {
    get_interfaces_ip();
    if (!m_if_addresses.empty()) {
      create_sockets();
      for (size_t i = 0; i < m_if_addresses.size(); ++i) {
        irs::mlog() << "Сетевой интерфейс №" << i << ": " <<
          m_if_addresses[i].first << "; " << m_if_addresses[i].second << endl;
      }
    } else {
      irs::mlog() << "Не найдено ни одного сетевого интерфейса" << endl;
    }
  } else {
    irs::mlog() << "Не удалось инициализировать Winsock" << endl;
  }

}

irs::ip_collector_t::~ip_collector_t()        
{
  socket_t::iterator it;
  for (it = m_sockets.begin(); it != m_sockets.end(); ++it) {
    closesocket(it->second);
  }
	WSACleanup();
}

void irs::ip_collector_t::send_request(string_t a_request_string, uint16_t a_port)
{
  for (socket_t::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
    string_t& ip_str = it->first;
    int sock = it->second;

    sockaddr_in sa_in = {0};
	  sa_in.sin_family = AF_INET;
    sa_in.sin_addr.s_addr = inet_addr(ip_str.c_str());
    sa_in.sin_port = htons(a_port);

    if (sendto(sock, a_request_string.c_str(), a_request_string.size(), 0,
      reinterpret_cast<sockaddr*>(&sa_in), sizeof(sa_in)) == SOCKET_ERROR)
    {
      irs::mlog() << "Не удалось отправить с сокета на адрес ip: " << ip_str << endl;
    }
  }
  m_answers.clear();
  m_collect_ip_timer.start();
}

void irs::ip_collector_t::tick()
{
  if (m_recv_socket_timer.check()) {
    for (socket_t::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it) {
      int sock = it->second;

      char buf[50];
      int bytes_read = recv(sock, buf, sizeof(buf), 0);

      if (bytes_read > 0) {
        m_answers.push_back(string_t(buf, buf + bytes_read));
      }
    }
  }
}

bool irs::ip_collector_t::are_ip_collected()
{
  m_collect_ip_timer.check();
  return m_collect_ip_timer.stopped();
}

const irs::ip_collector_t::answer_type_t& irs::ip_collector_t::get_answers()
{
  return m_answers;
}

void irs::ip_collector_t::get_interfaces_ip()
{
  m_if_addresses.clear();

  ULONG info_buffer_size = sizeof (IP_ADAPTER_INFO);
  PIP_ADAPTER_INFO p_adapter_info = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
  DWORD res;

  if (p_adapter_info != NULL) {
    res = GetAdaptersInfo(p_adapter_info, &info_buffer_size);
    if (res == ERROR_BUFFER_OVERFLOW) {
      free(p_adapter_info);
      p_adapter_info = (IP_ADAPTER_INFO *) malloc(info_buffer_size);

      if (p_adapter_info != NULL) {
        res = GetAdaptersInfo(p_adapter_info, &info_buffer_size);
      }
    }
  }

  if (res == NO_ERROR) {
    while (p_adapter_info) {
      mxip_t ip = {0};
      cstr_to_mxip(ip, p_adapter_info->IpAddressList.IpAddress.String);
      mxip_t mask = {0};
      cstr_to_mxip(mask, p_adapter_info->IpAddressList.IpMask.String);

      if (p_adapter_info->Type == MIB_IF_TYPE_ETHERNET) {
        m_if_addresses.push_back(make_pair(ip, mask));
      } else {
        irs::mlog() << "other if: " << ip << "; " << mask << endl;
      }
      p_adapter_info = p_adapter_info->Next;
    }
  } else {
    irs::mlog() << "GetAdaptersInfo error code " << res << endl;
  }
}

void irs::ip_collector_t::create_sockets()
{
  if_address_t::iterator it;
  for (it = m_if_addresses.begin(); it != m_if_addresses.end(); ++it) {

    const mxip_t& ip = it->first;
    const mxip_t& mask = it->second;

    mxip_t bcast_ip = {0};
    bcast_ip.addr = ip.addr | ~(mask.addr);
    char bcast_ip_str[IP_STR_LEN] = {0};
    mxip_to_cstr(bcast_ip_str, bcast_ip);

    char str_ip[IP_STR_LEN] = {0};
    mxip_to_cstr(str_ip, ip);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock != SOCKET_ERROR) {
      sockaddr_in server = {0};
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = inet_addr(str_ip);
      server.sin_port = htons(0);

      if (bind(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server)) !=
        SOCKET_ERROR)
      {
        unsigned long non_blocking_mode = 1;
        ioctlsocket(sock, FIONBIO, &non_blocking_mode);

        m_sockets.push_back(make_pair(bcast_ip_str, sock));
      } else {
        irs::mlog() << "Не удалось забиндить socket, ip: " << str_ip << endl;
      }
    } else {
      irs::mlog() << "Не удалось создать socket :" << str_ip << endl;
    }
  }
}

#endif // __BORLANDC__

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
  mp_modbus_client(NULL),
  m_activated(false),
  m_activation_timer(irs::make_cnt_s(1)),
  mp_hardflow_create_foo(NULL)
  #ifdef __BORLANDC__
  ,
  m_wait_response(false),
  mp_ip_collector(ip_collector_t::get_instance())
  #endif // __BORLANDC__
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
    vector<string_type> devices_items;
    mp_param_box->add_edit(irst("IP"), irst("127.0.0.1"));
    mp_param_box->add_combo(irst("IP"), &devices_items);
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
	map<string_type, device_open_data_t>::const_iterator it =
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
  #if IRS_USE_HID_WIN_API
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

	m_usb_hid_device_path_map.insert(make_pair(device, device_open_data_t(
	  devs[i].path, devs[i].attributes.product_id,
	  devs[i].attributes.vendor_id)));
  }
  #else //IRS_USE_HID_WIN_API
  m_usb_hid_device_path_map.insert(
	make_pair(irst("define IRS_USE_HID_WIN_API выключен!"),
	device_open_data_t(irst(""), 0, 0)));
  #endif //IRS_USE_HID_WIN_API
}

void irs::modbus_assembly_t::update_param_box_devices_field()
{
  string_type default_device = mp_param_box->get_param(irst("Имя устройства"));
  mp_param_box->clear_combo(irst("Имя устройства"));

  update_usb_hid_device_path_map();
  vector<string_type> devices_items;
  map<string_type, device_open_data_t>::const_iterator it =
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

#ifdef __BORLANDC__
void irs::modbus_assembly_t::add_collected_ip()
{
  vector<string_type> collected_ips;

  irs::ip_collector_t::answer_type_t answers = mp_ip_collector->get_answers();
  irs::ip_collector_t::answer_type_t::iterator it;
  for (it = answers.begin(); it != answers.end(); ++it) {
    // Формат ответа: ip;factory_number
    irs::ip_collector_t::string_t::size_type n = it->find(';');
    if (n != irs::ip_collector_t::string_t::npos) {
      *it = irs::ip_collector_t::string_t(it->begin(), it->begin() + n);
    }

    // Приходить должно только в ASCII
    wstringstream wide_answer;
    wide_answer << it->c_str();
    collected_ips.push_back(wide_answer.str());

    irs::mlog() << it->c_str() << endl;
  }

  string_type default_ip = mp_param_box->get_param(irst("IP"));
  if (default_ip.empty() && !collected_ips.empty()) {
    default_ip = collected_ips.front();
  }

  mp_param_box->clear_combo(irst("IP"));
  mp_param_box->add_combo(irst("IP"), &collected_ips);
}
#endif // __BORLANDC__

void irs::modbus_assembly_t::add_error(const string_type& a_error)
{
  stringstream s;
  s << irs::sdatetime;
  string_type msg = irs::str_conv<string_type>(s.str()) + a_error;
  m_error_list.push_back(msg);
  if (m_error_list.size() > error_list_max_size) {
    m_error_list.pop_front();
  }
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
    if (m_protocol == usb_hid_protocol) {
      try_create_modbus();
    } else {
      try {
        create_modbus();
      } catch (...) {
        destroy_modbus();
        throw;
      }
    }
  } else {
    destroy_modbus();
  }
  m_enabled = a_enabled;
}

void irs::modbus_assembly_t::try_create_modbus()
{
  try {
    create_modbus();
  } catch (std::runtime_error& e) {
    const string_type s = str_conv<string_type>(string(e.what()));
    add_error(s);
    destroy_modbus();
  } catch (std::logic_error& /*e*/) {
    throw;
  } catch (...) {
    add_error(irst("Неизвестная ошибка"));
    destroy_modbus();
  }
}

void irs::modbus_assembly_t::create_modbus()
{
  mp_modbus_client_hardflow = make_hardflow();
  mp_modbus_client = make_client(mp_modbus_client_hardflow, mp_param_box);
  mp_tstlan4->connect(mp_modbus_client.get());
  m_activated = true;
}

void irs::modbus_assembly_t::destroy_modbus()
{
  mp_tstlan4->connect(NULL);
  mp_modbus_client.reset();
  mp_modbus_client_hardflow.reset();
  m_activated = false;
}

irs::mxdata_t* irs::modbus_assembly_t::mxdata()
{
  return mp_modbus_client.get();
}
void irs::modbus_assembly_t::tick()
{
  if (!mp_modbus_client.is_empty()) {
    mp_modbus_client->tick();
    const string_type error_string =
      mp_modbus_client_hardflow->param(irst("error_string"));
    if (!error_string.empty()) {
      add_error(error_string);
      destroy_modbus();
    }
  }
  if ((m_protocol == usb_hid_protocol) &&
      m_enabled && !m_activated && m_activation_timer.check()) {
    try_create_modbus();
  }

  #ifdef __BORLANDC__
  if (m_protocol != usb_hid_protocol) {
    mp_ip_collector->tick();

    if (mp_ip_collector->are_ip_collected() && m_wait_response) {
      m_wait_response = false;
      add_collected_ip();
    }
  }
  #endif // __BORLANDC__
}
void irs::modbus_assembly_t::show_options()
{
  #ifdef __BORLANDC__
  if (m_protocol != usb_hid_protocol) {
    mp_ip_collector->send_request("pokrov_get_ip", 5007);
    // Одновременно в тике крутятся несколько экземпляров modbus_assembly_t,
    // ответы должен принимать только тот, который послал запрос
    m_wait_response = true;
  }
  #endif // __BORLANDC__

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

irs::modbus_assembly_t::status_t irs::modbus_assembly_t::get_status()
{
  if (m_activated) {
    return status_connected;
  }
  if (m_enabled) {
    return status_busy;
  }
  return status_disabled;
}

irs::modbus_assembly_t::error_string_list_type
irs::modbus_assembly_t::get_last_error_string_list()
{
  error_string_list_type error_list = m_error_list;
  m_error_list.clear();
  return error_list;
}

void irs::modbus_assembly_t::set_hardwlof_create_foo(
  irs::modbus_assembly_t::hardflow_create_foo_t ap_hardflow_create_foo)
{
  mp_hardflow_create_foo = ap_hardflow_create_foo;
}

#endif // defined(IRS_WIN32) || defined(IRS_LINUX)

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

class agilent_3458a_mxmultimeter_creator_t:
  public mxmultimeter_assembly_creator_t
{
public:
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name);
};

class agilent_3458a_mxmultimeter_t: public mxmultimeter_assembly_t
{
public:
  agilent_3458a_mxmultimeter_t(const string_type& a_conf_file_name);
  ~agilent_3458a_mxmultimeter_t();
  virtual bool enabled() const;
  virtual void enable(multimeter_mode_type_t a_mul_mode_type);
  virtual void disable();
  virtual mxmultimeter_t* mxmultimeter();
  virtual void tick();
  virtual void show_options();
private:
  void reset();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;
    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  bool m_enabled;
  multimeter_mode_type_t m_mul_mode_type;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
};

irs::handle_t<irs::mxmultimeter_assembly_t>
irs::agilent_3458a_mxmultimeter_creator_t::make(
  const string_type& a_name)
{
  return handle_t<mxmultimeter_assembly_t>(
    new agilent_3458a_mxmultimeter_t(a_name));
}

irs::agilent_3458a_mxmultimeter_t::agilent_3458a_mxmultimeter_t(
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("Agilent 3458A"),
    m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  m_enabled(false),
  m_mul_mode_type(mul_mode_type_passive),
  mp_hardflow(),
  mp_multimeter()
{
}
irs::agilent_3458a_mxmultimeter_t::~agilent_3458a_mxmultimeter_t()
{
  mp_param_box->save();
}
irs::agilent_3458a_mxmultimeter_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->load();
}
bool irs::agilent_3458a_mxmultimeter_t::enabled() const
{
  return m_enabled;
}
void irs::agilent_3458a_mxmultimeter_t::enable(
  multimeter_mode_type_t a_mul_mode_type)
{
  if (m_enabled) {
    return;
  }
  m_mul_mode_type = a_mul_mode_type;
  reset();
}
void irs::agilent_3458a_mxmultimeter_t::disable()
{
  mp_multimeter.reset();
  mp_hardflow.reset();
  m_enabled = false;
}

void irs::agilent_3458a_mxmultimeter_t::reset()
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
    m_mul_mode_type));
  m_enabled = true;
}
mxmultimeter_t* irs::agilent_3458a_mxmultimeter_t::mxmultimeter()
{
  return mp_multimeter.get();
}
void irs::agilent_3458a_mxmultimeter_t::tick()
{
  if (!mp_multimeter.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
  }
}
void irs::agilent_3458a_mxmultimeter_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}

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

class agilent_34420a_mxmultimeter_creator_t:
  public mxmultimeter_assembly_creator_t
{
public:
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name);
};

class agilent_34420a_mxmultimeter_t: public mxmultimeter_assembly_t
{
public:
  agilent_34420a_mxmultimeter_t(const string_type& a_conf_file_name);
  virtual ~agilent_34420a_mxmultimeter_t();
  virtual bool enabled() const;
  virtual void enable(multimeter_mode_type_t a_mul_mode_type);
  virtual void disable();
  virtual mxmultimeter_t* mxmultimeter();
  virtual void tick();
  virtual void show_options();
private:
  void reset();

  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  bool m_enabled;
  multimeter_mode_type_t m_mul_mode_type;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
};

} // namespace irs

irs::handle_t<irs::mxmultimeter_assembly_t>
irs::agilent_34420a_mxmultimeter_creator_t::make(
  const string_type& a_name)
{
  return handle_t<mxmultimeter_assembly_t>(
    new agilent_34420a_mxmultimeter_t(a_name));
}

irs::agilent_34420a_mxmultimeter_t::agilent_34420a_mxmultimeter_t(
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("Agilent 34420A"),
    m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  m_enabled(false),
  m_mul_mode_type(mul_mode_type_passive),
  mp_hardflow(),
  mp_multimeter()
{

}
irs::agilent_34420a_mxmultimeter_t::~agilent_34420a_mxmultimeter_t()
{
  mp_param_box->save();
}
irs::agilent_34420a_mxmultimeter_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->load();
}
bool irs::agilent_34420a_mxmultimeter_t::enabled() const
{
  return m_enabled;
}
void irs::agilent_34420a_mxmultimeter_t::enable(
  multimeter_mode_type_t a_mul_mode_type)
{
  if (m_enabled) {
    return;
  }
  m_mul_mode_type = a_mul_mode_type;
  reset();
}
void irs::agilent_34420a_mxmultimeter_t::disable()
{
  mp_multimeter.reset();
  mp_hardflow.reset();
  m_enabled = false;
}

void irs::agilent_34420a_mxmultimeter_t::reset()
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
    m_mul_mode_type));
  m_enabled = true;
}
mxmultimeter_t* irs::agilent_34420a_mxmultimeter_t::mxmultimeter()
{
  return mp_multimeter.get();
}
void irs::agilent_34420a_mxmultimeter_t::tick()
{
  if (!mp_multimeter.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
  }
}
void irs::agilent_34420a_mxmultimeter_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}

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

class v7_78_1_mxmultimeter_creator_t: public mxmultimeter_assembly_creator_t
{
public:
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name);
};

class v7_78_1_mxmultimeter_t: public mxmultimeter_assembly_t
{
public:
  v7_78_1_mxmultimeter_t(const string_type& a_conf_file_name);
  virtual ~v7_78_1_mxmultimeter_t();
  virtual bool enabled() const;
  virtual void enable(multimeter_mode_type_t a_mul_mode_type);
  virtual void disable();
  virtual mxmultimeter_t* mxmultimeter();
  virtual void tick();
  virtual void show_options();
private:
  void reset();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  bool m_enabled;
  multimeter_mode_type_t m_mul_mode_type;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
};

} // namespace irs

irs::handle_t<irs::mxmultimeter_assembly_t>
irs::v7_78_1_mxmultimeter_creator_t::make(
  const string_type& a_name)
{
  return handle_t<mxmultimeter_assembly_t>(new v7_78_1_mxmultimeter_t(a_name));
}

irs::v7_78_1_mxmultimeter_t::v7_78_1_mxmultimeter_t(
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("В7-78/1"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  m_enabled(false),
  m_mul_mode_type(mul_mode_type_passive),
  mp_hardflow(),
  mp_multimeter()
{
}
irs::v7_78_1_mxmultimeter_t::~v7_78_1_mxmultimeter_t()
{
  mp_param_box->save();
}
irs::v7_78_1_mxmultimeter_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->load();
}
bool irs::v7_78_1_mxmultimeter_t::enabled() const
{
  return m_enabled;
}
void irs::v7_78_1_mxmultimeter_t::enable(
  multimeter_mode_type_t a_mul_mode_type)
{
  if (m_enabled) {
    return;
  }
  m_mul_mode_type = a_mul_mode_type;
  reset();
}
void irs::v7_78_1_mxmultimeter_t::disable()
{
  mp_multimeter.reset();
  mp_hardflow.reset();
  m_enabled = false;
}
void irs::v7_78_1_mxmultimeter_t::reset()
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
  mp_multimeter.reset(new irs::akip_v7_78_1_t(mp_hardflow.get(),
    m_mul_mode_type));
  m_enabled = true;
}
mxmultimeter_t* irs::v7_78_1_mxmultimeter_t::mxmultimeter()
{
  return mp_multimeter.get();
}
void irs::v7_78_1_mxmultimeter_t::tick()
{
  if (!mp_multimeter.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
  }
}
void irs::v7_78_1_mxmultimeter_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
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
  mp_param_box(make_assembly_param_box(irst("В7-78/1"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_hardflow(),
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
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();

  //mp_param_box->add_edit(irst("GPIB адрес"), irst("23"));
  //mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  //mp_param_box->load();
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
  mp_hardflow.reset();
}
void irs::v7_78_1_assembly_t::reset()
{
  /*disable();
  const int gpib_address = param_box_read_number<int>(*mp_param_box,
    irst("GPIB адрес"));
  mp_multimeter.reset(new irs::v7_78_1_t(
    MXIFA_MULTIMETER, gpib_address, mul_mode_type_passive));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    param_box_read_number<int>(*mp_param_box,
    irst("Время обновления, мс"))/1000.));
  mp_tstlan4->connect(mp_mxdata.get());*/


  disable();
  typedef irs::hardflow::prologix_flow_t prologix_flow_type;
  const prologix_flow_type::end_line_t read_end_line =
    prologix_flow_type::lf;
  const prologix_flow_type::end_line_t write_end_line =
    prologix_flow_type::cr;
  const int prologix_timeout_ms = 3000;
  mp_hardflow = make_gpib_hardflow(mp_param_box.get(), read_end_line,
    write_end_line, prologix_timeout_ms);
  mp_multimeter.reset(new irs::akip_v7_78_1_t(mp_hardflow.get(),
    mul_mode_type_passive));

  const int update_time_ms =
    param_box_read_number<int>(*mp_param_box, irst("Время обновления, мс"));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    update_time_ms/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::v7_78_1_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::v7_78_1_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_hardflow->tick();
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

class keithley_2015_mxmultimeter_creator_t:
  public mxmultimeter_assembly_creator_t
{
public:
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name);
};

class keithley_2015_mxmultimeter_t: public mxmultimeter_assembly_t
{
public:
  keithley_2015_mxmultimeter_t(const string_type& a_conf_file_name);
  virtual ~keithley_2015_mxmultimeter_t();
  virtual bool enabled() const;
  virtual void enable(multimeter_mode_type_t a_mul_mode_type);
  virtual void disable();
  virtual mxmultimeter_t* mxmultimeter();
  virtual void tick();
  virtual void show_options();
private:
  void reset();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  bool m_enabled;
  multimeter_mode_type_t m_mul_mode_type;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
};

} // namespace irs

irs::handle_t<irs::mxmultimeter_assembly_t>
irs::keithley_2015_mxmultimeter_creator_t::make(
  const string_type& a_name)
{
  return handle_t<mxmultimeter_assembly_t>(
    new keithley_2015_mxmultimeter_t(a_name));
}

irs::keithley_2015_mxmultimeter_t::keithley_2015_mxmultimeter_t(
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(
    make_assembly_param_box(irst("Keithley 2015"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  m_enabled(false),
  m_mul_mode_type(mul_mode_type_passive),
  mp_hardflow(),
  mp_multimeter()
{
}
irs::keithley_2015_mxmultimeter_t::~keithley_2015_mxmultimeter_t()
{
  mp_param_box->save();
}
irs::keithley_2015_mxmultimeter_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->load();
}
bool irs::keithley_2015_mxmultimeter_t::enabled() const
{
  return m_enabled;
}
void irs::keithley_2015_mxmultimeter_t::enable(
  multimeter_mode_type_t a_mul_mode_type)
{
  if (m_enabled) {
    return;
  }
  m_mul_mode_type = a_mul_mode_type;
  reset();
}
void irs::keithley_2015_mxmultimeter_t::disable()
{
  mp_multimeter.reset();
  mp_hardflow.reset();
  m_enabled = false;
}
void irs::keithley_2015_mxmultimeter_t::reset()
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
  mp_multimeter.reset(new irs::keithley_2015_t(mp_hardflow.get(),
    m_mul_mode_type));
  m_enabled = true;
}
mxmultimeter_t* irs::keithley_2015_mxmultimeter_t::mxmultimeter()
{
  return mp_multimeter.get();
}
void irs::keithley_2015_mxmultimeter_t::tick()
{
  if (!mp_multimeter.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
  }
}
void irs::keithley_2015_mxmultimeter_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}

namespace irs {

class keithley_2015_assembly_creator_t: public mxdata_assembly_creator_t
{
public:
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name);
private:
};

class keithley_2015_assembly_t: public mxdata_assembly_t
{
public:
  keithley_2015_assembly_t(tstlan4_base_t* ap_tstlan4,
    const string_type& a_conf_file_name);
  virtual ~keithley_2015_assembly_t();
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
irs::keithley_2015_assembly_creator_t::make(
  tstlan4_base_t* ap_tstlan4, const string_type& a_name)
{
  return handle_t<mxdata_assembly_t>(
    new keithley_2015_assembly_t(ap_tstlan4, a_name));
}

irs::keithley_2015_assembly_t::keithley_2015_assembly_t(
  tstlan4_base_t* ap_tstlan4, const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(
    make_assembly_param_box(irst("Keithley 2015"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  mp_tstlan4(ap_tstlan4),
  m_enabled(false),
  mp_hardflow(),
  mp_multimeter(),
  mp_mxdata()
{
  mp_tstlan4->ini_name(m_conf_file_name);
}
irs::keithley_2015_assembly_t::~keithley_2015_assembly_t()
{
  mp_param_box->save();
}
irs::keithley_2015_assembly_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  add_gpib_options_to_param_box(ap_param_box);
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->load();

  //mp_param_box->add_edit(irst("GPIB адрес"), irst("23"));
  //mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  //mp_param_box->load();
}
bool irs::keithley_2015_assembly_t::enabled() const
{
  return m_enabled;
}
void irs::keithley_2015_assembly_t::enabled(bool a_enabled)
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
void irs::keithley_2015_assembly_t::disable()
{
  mp_tstlan4->connect(NULL);
  mp_mxdata.reset();
  mp_multimeter.reset();
  mp_hardflow.reset();
}
void irs::keithley_2015_assembly_t::reset()
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
  mp_multimeter.reset(new irs::keithley_2015_t(mp_hardflow.get(),
    mul_mode_type_passive));

  const int update_time_ms =
    param_box_read_number<int>(*mp_param_box, irst("Время обновления, мс"));
  mp_mxdata.reset(new irs::multimeter_mxdata_t(mp_multimeter.get(),
    update_time_ms/1000.));

  mp_tstlan4->connect(mp_mxdata.get());
}
irs::mxdata_t* irs::keithley_2015_assembly_t::mxdata()
{
  return mp_mxdata.get();
}
void irs::keithley_2015_assembly_t::tick()
{
  if (!mp_mxdata.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
    mp_mxdata->tick();
  }
}
void irs::keithley_2015_assembly_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}
void irs::keithley_2015_assembly_t::tstlan4(tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}
#endif // defined(IRS_WIN32)

#if defined(IRS_WIN32)
namespace irs {

class ch3_85_3r_mxmultimeter_creator_t: public mxmultimeter_assembly_creator_t
{
public:
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name);
};

class ch3_85_3r_mxmultimeter_t: public mxmultimeter_assembly_t
{
public:
  ch3_85_3r_mxmultimeter_t(const string_type& a_conf_file_name);
  virtual ~ch3_85_3r_mxmultimeter_t();
  virtual bool enabled() const;
  virtual void enable(multimeter_mode_type_t a_mul_mode_type);
  virtual void disable();
  virtual mxmultimeter_t* mxmultimeter();
  virtual void tick();
  virtual void show_options();
  private:
  void reset();
  struct param_box_tune_t {
    param_box_base_t* mp_param_box;

    param_box_tune_t(param_box_base_t* ap_param_box);
  };

  string_type m_conf_file_name;
  handle_t<param_box_base_t> mp_param_box;
  param_box_tune_t m_param_box_tune;
  bool m_enabled;
  multimeter_mode_type_t m_mul_mode_type;
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  handle_t<mxmultimeter_t> mp_multimeter;
};

} // namespace irs

irs::handle_t<irs::mxmultimeter_assembly_t>
irs::ch3_85_3r_mxmultimeter_creator_t::make(
  const string_type& a_name)
{
  return handle_t<mxmultimeter_assembly_t>(
    new ch3_85_3r_mxmultimeter_t(a_name));
}

irs::ch3_85_3r_mxmultimeter_t::ch3_85_3r_mxmultimeter_t(
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(irst("Ч3-85/3R"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),

  m_enabled(false),
  m_mul_mode_type(mul_mode_type_passive),
  mp_hardflow(),
  mp_multimeter()
{
}
irs::ch3_85_3r_mxmultimeter_t::~ch3_85_3r_mxmultimeter_t()
{
  mp_param_box->save();
}
irs::ch3_85_3r_mxmultimeter_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_edit(irst("COM name"), irst("com1"));
  mp_param_box->load();
}
bool irs::ch3_85_3r_mxmultimeter_t::enabled() const
{
  return m_enabled;
}
void irs::ch3_85_3r_mxmultimeter_t::enable(
  multimeter_mode_type_t a_mul_mode_type)
{
  if (m_enabled) {
    return;
  }
  m_mul_mode_type = a_mul_mode_type;
  reset();
}
void irs::ch3_85_3r_mxmultimeter_t::disable()
{
  mp_multimeter.reset();
  mp_hardflow.reset();
  m_enabled = false;
}
void irs::ch3_85_3r_mxmultimeter_t::reset()
{
  disable();
  const string_type com_name = mp_param_box->get_param(irst("COM name"));

  mp_hardflow.reset(new irs::hardflow::com_flow_t(
    com_name, CBR_9600, FALSE, NOPARITY, 8,
    ONESTOPBIT, DTR_CONTROL_DISABLE));

  mp_multimeter.reset(
    new irs::akip_ch3_85_3r_t(mp_hardflow.get(), m_mul_mode_type));
  m_enabled = true;
}

mxmultimeter_t* irs::ch3_85_3r_mxmultimeter_t::mxmultimeter()
{
  return mp_multimeter.get();
}
void irs::ch3_85_3r_mxmultimeter_t::tick()
{
  if (!mp_multimeter.is_empty()) {
    mp_hardflow->tick();
    mp_multimeter->tick();
  }
}
void irs::ch3_85_3r_mxmultimeter_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}

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
  mp_param_box(make_assembly_param_box(irst("Ч3-85/3R"), m_conf_file_name)),
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

class test_multimeter_mxmultimeter_creator_t:
  public mxmultimeter_assembly_creator_t
{
public:
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name);
private:
};

class test_multimeter_mxmultimeter_t: public mxmultimeter_assembly_t
{
public:
  test_multimeter_mxmultimeter_t(const string_type& a_conf_file_name);
  virtual ~test_multimeter_mxmultimeter_t();
  virtual bool enabled() const;
  virtual void enable(multimeter_mode_type_t a_mul_mode_type);
  virtual void disable();
  virtual mxmultimeter_t* mxmultimeter();
  virtual void tick();
  virtual void show_options();
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
};

} // namespace irs

irs::handle_t<irs::mxmultimeter_assembly_t>
irs::test_multimeter_mxmultimeter_creator_t::make(
  const string_type& a_name)
{
  return handle_t<mxmultimeter_assembly_t>(
    new test_multimeter_mxmultimeter_t(a_name));
}

irs::test_multimeter_mxmultimeter_t::test_multimeter_mxmultimeter_t(
  const string_type& a_conf_file_name
):
  m_conf_file_name(a_conf_file_name),
  mp_param_box(make_assembly_param_box(
    irst("Тестовый мультиметр"), m_conf_file_name)),
  m_param_box_tune(mp_param_box.get()),
  m_enabled(false),
  mp_multimeter()
{
}
irs::test_multimeter_mxmultimeter_t::~test_multimeter_mxmultimeter_t()
{
  mp_param_box->save();
}
irs::test_multimeter_mxmultimeter_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->load();
}
bool irs::test_multimeter_mxmultimeter_t::enabled() const
{
  return m_enabled;
}
void irs::test_multimeter_mxmultimeter_t::enable(
  multimeter_mode_type_t /*a_mul_mode_type*/)
{
  if (m_enabled) {
    return;
  }
  reset();
}
void irs::test_multimeter_mxmultimeter_t::disable()
{
  mp_multimeter.reset();
  m_enabled = false;
}
void irs::test_multimeter_mxmultimeter_t::reset()
{
  disable();

  mp_multimeter.reset(new irs::dummy_multimeter_t());
  m_enabled = true;
}
mxmultimeter_t* irs::test_multimeter_mxmultimeter_t::mxmultimeter()
{
  return mp_multimeter.get();
}
void irs::test_multimeter_mxmultimeter_t::tick()
{
  if (!mp_multimeter.is_empty()) {
    mp_multimeter->tick();
  }
}
void irs::test_multimeter_mxmultimeter_t::show_options()
{
  if (mp_param_box->show() && m_enabled) {
    reset();
  }
}


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
	const string_type& a_name, modbus_assembly_t::hardflow_create_foo_t
	a_modbus_hid_create_foo);
private:
  typedef map<string_type, handle_t<mxdata_assembly_creator_t> > ac_list_type;
  typedef ac_list_type::iterator ac_list_it_type;
  typedef ac_list_type::const_iterator ac_list_const_it_type;

  ac_list_type m_ac_list;
};

} //namespace irs

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
    new modbus_tcp_assembly_creator_t);
  m_ac_list[irst("modbus usb hid")] = handle_t<mxdata_assembly_creator_t>(
	new modbus_usb_hid_assembly_creator_t);
  m_ac_list[irst("Agilent 3458A")] = handle_t<mxdata_assembly_creator_t>(
    new agilent_3458a_assembly_creator_t);
  m_ac_list[irst("Agilent 34420A")] = handle_t<mxdata_assembly_creator_t>(
    new agilent_34420a_assembly_creator_t);
  m_ac_list[irst("В7-78/1")] = handle_t<mxdata_assembly_creator_t>(
    new v7_78_1_assembly_creator_t);
  m_ac_list[irst("Keithley 2015")] = handle_t<mxdata_assembly_creator_t>(
    new keithley_2015_assembly_creator_t);
  m_ac_list[irst("Ч3-85/3R")] = handle_t<mxdata_assembly_creator_t>(
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
  const string_type& a_name, modbus_assembly_t::hardflow_create_foo_t
  a_modbus_hid_create_foo)
{
  handle_t<mxdata_assembly_t> result_assembly(IRS_NULL);
  ac_list_it_type it = m_ac_list.find(a_assembly_type);
  if (it != m_ac_list.end()) {
	result_assembly = IRS_NULL;
	result_assembly = it->second->make(ap_tstlan4, a_name);
  }
  if (a_assembly_type == irst("modbus usb hid")) {
	if (modbus_assembly_t* modbus_assembly = dynamic_cast<modbus_assembly_t*>(result_assembly.get())) {
	  if (a_modbus_hid_create_foo != NULL) {
		modbus_assembly->set_hardwlof_create_foo(a_modbus_hid_create_foo);
	  }
	}

  }
  return result_assembly;
}

irs::mxdata_assembly_types_t* irs::mxdata_assembly_types()
{
  static mxdata_assembly_types_implementation_t
	mxdata_assembly_types_implementation;
  return &mxdata_assembly_types_implementation;
}
///

namespace irs {

class mxmultimeter_assembly_types_implementation_t:
  public mxmultimeter_assembly_types_t
{
public:
  mxmultimeter_assembly_types_implementation_t();
  virtual void enum_types(vector<string_type>* ap_types) const;
  virtual handle_t<mxmultimeter_assembly_t> make_assembly(
    const string_type& a_assembly_type, const string_type& a_name);
private:
  typedef map<string_type, handle_t<mxmultimeter_assembly_creator_t> >
    ac_list_type;
  typedef ac_list_type::iterator ac_list_it_type;
  typedef ac_list_type::const_iterator ac_list_const_it_type;

  ac_list_type m_ac_list;
};

} //namespace irs

irs::mxmultimeter_assembly_types_t* irs::mxmultimeter_assembly_types()
{
  static mxmultimeter_assembly_types_implementation_t
    mxmultimeter_assembly_types_implementation;
  return &mxmultimeter_assembly_types_implementation;
}

irs::mxmultimeter_assembly_types_implementation_t::
  mxmultimeter_assembly_types_implementation_t(
):
  m_ac_list()
{
  #ifdef __BORLANDC__
  m_ac_list[irst("Agilent 3458A")] = handle_t<mxmultimeter_assembly_creator_t>(
    new agilent_3458a_mxmultimeter_creator_t());
  m_ac_list[irst("Agilent 34420A")] = handle_t<mxmultimeter_assembly_creator_t>(
    new agilent_34420a_mxmultimeter_creator_t());
  m_ac_list[irst("В7-78/1")] = handle_t<mxmultimeter_assembly_creator_t>(
    new v7_78_1_mxmultimeter_creator_t());
  m_ac_list[irst("Keithley 2015")] = handle_t<mxmultimeter_assembly_creator_t>(
    new keithley_2015_mxmultimeter_creator_t());
  m_ac_list[irst("Ч3-85/3R")] = handle_t<mxmultimeter_assembly_creator_t>(
    new ch3_85_3r_mxmultimeter_creator_t());
  #endif // __BORLANDC__
  m_ac_list[irst("Тестовый мультиметр")] =
    handle_t<mxmultimeter_assembly_creator_t>(
    new test_multimeter_mxmultimeter_creator_t());
}
void irs::mxmultimeter_assembly_types_implementation_t::
  enum_types(vector<string_type>* ap_types) const
{
  ap_types->clear();
  for (ac_list_const_it_type it = m_ac_list.begin();
    it != m_ac_list.end(); it++)
  {
    ap_types->push_back(it->first);
  }
}
irs::handle_t<irs::mxmultimeter_assembly_t>
  irs::mxmultimeter_assembly_types_implementation_t::make_assembly
(
  const string_type& a_assembly_type,
  const string_type& a_name)
{
  handle_t<mxmultimeter_assembly_t> result_assembly(IRS_NULL);
  ac_list_it_type it = m_ac_list.find(a_assembly_type);
  if (it != m_ac_list.end()) {
    result_assembly = IRS_NULL;
    result_assembly = it->second->make(a_name);
  }
  return result_assembly;
}

