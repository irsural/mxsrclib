#include <irspch.h>

#include <irsdcp.h>

#include <irsfinal.h>

#ifndef __ICCAVR__

namespace {

char packet_id[] = {'i', 'r', 's', 'd', 'c', 'p'};

} // unnamed namespace

// struct packet_t
irs::dcp::packet_t::packet_t():
  //transaction_id(0),
  operation_code(device_request),
  device_code(device_code_u5023m),
  device_mac(mxmac_t::zero_mac()),
  device_ip(mxip_t::any_ip()),
  device_netmask(mxip_t::any_ip()),
  device_gateway(mxip_t::any_ip()),
  options(0)
{
  irs::memsetex(hardware_version, IRS_ARRAYSIZE(hardware_version));
  irs::memsetex(firmware_version, IRS_ARRAYSIZE(firmware_version));

  //IRS_LIB_ASSERT(IRS_ARRAYSIZE(packet_id) == id_size);
  //memcpy(protocol_id, packet_id, IRS_ARRAYSIZE(packet_id));
}

// class packet_flow_t
irs::dcp::packet_flow_t::packet_flow_t(
  irs::hardflow_t* ap_hardflow
):
  mp_hardflow(ap_hardflow),
  m_fixed_flow(mp_hardflow),
  //m_receive_timeout(),
  m_receive_status(irs_st_ready),
  m_receive_channel_id(0),
  m_send_buf(),
  m_receive_buf(),
  mp_receive_packet(NULL),
  mp_process(&packet_flow_t::process_wait_command)
{
  //IRS_LIB_ASSERT(ap_hardflow);
  m_send_buf.resize(sizeof(packet_t) + IRS_ARRAYSIZE(packet_id));
  m_fixed_flow.write_timeout(irs::make_cnt_s(2));
  //m_fixed_flow.read_timeout(irs::make_cnt_s(1));
}

void irs::dcp::packet_flow_t::hardflow(irs::hardflow_t* ap_hardflow)
{
  IRS_LIB_ASSERT(m_receive_status != irs_st_busy);
  IRS_LIB_ASSERT(m_fixed_flow.write_status() != fixed_flow_type::status_wait);
  mp_hardflow = ap_hardflow;
  m_fixed_flow.connect(ap_hardflow);
}

void irs::dcp::packet_flow_t::send(size_type a_channel,
  const packet_t& a_packet)
{
  IRS_LIB_ASSERT(m_send_buf.size() ==
    (sizeof(packet_t) + IRS_ARRAYSIZE(packet_id)));
  IRS_LIB_ASSERT(m_fixed_flow.write_status() !=
    hardflow::fixed_flow_t::status_wait);
  memcpyex(vector_data(m_send_buf), reinterpret_cast<const irs_u8*>(packet_id),
    IRS_ARRAYSIZE(packet_id));
  memcpyex(vector_data(m_send_buf, IRS_ARRAYSIZE(packet_id)),
    reinterpret_cast<const irs_u8*>(&a_packet), sizeof(packet_t));
  m_fixed_flow.write(a_channel, vector_data(m_send_buf), m_send_buf.size());
}

void irs::dcp::packet_flow_t::receive(size_type a_channel,
  packet_t* ap_receive_packet)
{
  IRS_LIB_ASSERT(m_fixed_flow.read_status() !=
    hardflow::fixed_flow_t::status_wait);
  m_receive_channel_id = a_channel;
  mp_receive_packet = ap_receive_packet;
  m_receive_buf.clear();
  // Выставляем максимальный тайм-аут, так как нам не нужно, чтобы он срабатывал
  //m_fixed_flow.read_timeout(std::numeric_limits<counter_t>::max());
  //m_receive_timeout.start();
  mp_process = &packet_flow_t::process_find_packet_begin;
  m_receive_status = irs_st_busy;
}

/*void irs::dcp::packet_flow_t::receive_timeout(double a_timeout)
{
  m_receive_timeout.set(make_cnt_s(a_timeout));
}*/

irs_status_t irs::dcp::packet_flow_t::get_send_status() const
{
  return irs::to_irs_status(m_fixed_flow.write_status());
}

irs_status_t irs::dcp::packet_flow_t::get_receive_status() const
{
  return m_receive_status;
}

void irs::dcp::packet_flow_t::abort()
{
  m_fixed_flow.write_abort();
  m_fixed_flow.read_abort();
}

void irs::dcp::packet_flow_t::tick()
{
  m_fixed_flow.tick();
  (this->*mp_process)();
}

void irs::dcp::packet_flow_t::process_wait_command()
{
  // Ожидаем команд
}

void irs::dcp::packet_flow_t::process_find_packet_begin()
{
  IRS_LIB_ASSERT(m_receive_buf.size() < IRS_ARRAYSIZE(packet_id));
  size_type pos = m_receive_buf.size();
  m_receive_buf.resize(IRS_ARRAYSIZE(packet_id));
  const size_type size = mp_hardflow->read(m_receive_channel_id,
    vector_data(m_receive_buf, pos), m_receive_buf.size() - pos);
  m_receive_buf.resize(pos + size);
  bool equal = false;
  if (m_receive_buf.size() == IRS_ARRAYSIZE(packet_id)) {
    if (memcmp(vector_data(m_receive_buf), packet_id,
      m_receive_buf.size()) == 0) {
      equal = true;
    }
  }
  if (equal) {
    m_receive_buf.resize(sizeof(packet_t));
    m_fixed_flow.read_timeout(make_cnt_s(1));
    m_fixed_flow.read(m_receive_channel_id, vector_data(m_receive_buf),
      m_receive_buf.size());
    mp_process = &packet_flow_t::process_receive_packet;
  }/* else if (m_receive_timeout.check()) {
    m_receive_status = irs_st_error;
    mp_process = &packet_flow_t::process_wait_command;
  }*/ else {
    if (m_receive_buf.size() == IRS_ARRAYSIZE(packet_id)) {
      m_receive_buf.erase(m_receive_buf.begin());
    }
  }
}

void irs::dcp::packet_flow_t::process_receive_packet()
{
  if (m_fixed_flow.read_status() == fixed_flow_type::status_success) {
    *mp_receive_packet =
      *reinterpret_cast<packet_t*>(vector_data(m_receive_buf));
    m_receive_buf.clear();
    mp_process = &packet_flow_t::process_wait_command;
    m_receive_status = irs_st_ready;
  } else if (/*!m_receive_timeout.check() &&*/
    m_fixed_flow.read_status() == fixed_flow_type::status_error) {
    m_receive_buf.clear();
    mp_process = &packet_flow_t::process_find_packet_begin;
  }/* else if (m_receive_timeout.check()) {
    m_fixed_flow.read_abort();
    m_receive_status = irs_st_error;
    mp_process = &packet_flow_t::process_wait_command;
  }*/
}

// class client_t
irs::dcp::client_t::client_t(irs::hardflow_t* ap_client_hardflow,
  irs::hardflow_t* ap_server_hardflow, size_type a_divece_max_count
):
  mp_client_hardflow(ap_client_hardflow),
  mp_server_hardflow(ap_server_hardflow),
  m_client_channel_id(irs::hardflow_t::invalid_channel),
  m_client_packet_flow(ap_client_hardflow),
  m_server_packet_flow(ap_server_hardflow),
  m_receive_packet_timeout(make_cnt_s(1)),
  m_mode(mode_active_passive),
  m_status(irs_st_ready),
  mp_process_receive_packet(&client_t::process_wait_create_channel),
  mp_process_send_packet(&client_t::process_wait_command),
  m_send_packets(),
  m_user_send_packet(),
  m_send_packet(),
  m_send_packet_type(spt_system_packet),
  m_receive_packet(),
  m_devices(),
  m_divece_max_count(a_divece_max_count)
{
  m_client_channel_id = mp_client_hardflow->channel_next();
}

void irs::dcp::client_t::command_offer_configuration(const mxmac_t& a_mac,
  const device_configuration_t& a_config)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_status = irs_st_busy;
  m_user_send_packet.operation_code = address_offer;
  //m_user_send_packet.transaction_id = rand();
  m_user_send_packet.device_ip = a_config.ip;
  m_user_send_packet.device_netmask = a_config.netmask;
  m_user_send_packet.device_gateway = a_config.gateway;
  m_user_send_packet.device_mac = a_mac;
  m_user_packet_exists = true;
  send_packet();
}

void irs::dcp::client_t::command_get_device_status()
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_status = irs_st_busy;
  m_user_send_packet.operation_code = device_status_request;
  //m_user_send_packet.transaction_id = rand();
  m_user_packet_exists = true;
  send_packet();
}

irs_status_t irs::dcp::client_t::get_status() const
{
  return m_status;
}

void irs::dcp::client_t::abort()
{
  m_server_packet_flow.abort();
  m_status = irs_st_ready;
  mp_process_send_packet = &client_t::process_wait_command;
}

const irs::dcp::client_t::devices_type&
irs::dcp::client_t::get_devices() const
{
  return m_devices;
}

void irs::dcp::client_t::delete_devices()
{
  m_devices.clear();
}

void irs::dcp::client_t::tick()
{
  m_server_packet_flow.tick();
  m_client_packet_flow.tick();
  (this->*mp_process_send_packet)();
  (this->*mp_process_receive_packet)();
}

void irs::dcp::client_t::send_packet(const packet_t& a_packet)
{
  if (m_send_packets.size() >= send_packet_queue_max_size) {
    m_send_packets.pop_front();
  }
  m_send_packets.push_back(a_packet);
  send_packet();
}

void irs::dcp::client_t::send_packet()
{
  if (mp_process_send_packet == &client_t::process_wait_command) {
    if (select_packet_for_send()) {
      m_client_packet_flow.send(m_client_channel_id, m_send_packet);
      mp_process_send_packet = &client_t::process_send_packet;
    }
  }
}

bool irs::dcp::client_t::select_packet_for_send()
{
  if (m_user_packet_exists) {
    m_send_packet = m_user_send_packet;
    m_user_packet_exists = false;
    m_send_packet_type = spt_user_packet;
    return true;
  } else if (!m_send_packets.empty()) {
    m_send_packet = m_send_packets.front();
    m_send_packets.pop_front();
    m_send_packet_type = spt_system_packet;
    return true;
  }
  return false;
}

void irs::dcp::client_t::process_wait_command()
{
  // Ожидаем команд
}

void irs::dcp::client_t::process_send_packet()
{
  if ((m_client_packet_flow.get_send_status() == irs_st_ready) ||
    (m_client_packet_flow.get_send_status() == irs_st_error)) {
    if (m_send_packet_type == spt_user_packet) {
      if (m_client_packet_flow.get_send_status() == irs_st_ready) {
        m_status = irs_st_ready;
      } else {
        m_status = irs_st_error;
      }
    }
    if (select_packet_for_send()) {
      m_client_packet_flow.send(m_client_channel_id, m_send_packet);
    } else {
      mp_process_send_packet = &client_t::process_wait_command;
    }
  }
}

void irs::dcp::client_t::process_wait_create_channel()
{
  size_type channel_id = mp_server_hardflow->channel_next();
  if (channel_id != irs::hardflow_t::invalid_channel) {
    m_server_packet_flow.receive(channel_id, &m_receive_packet);
    m_receive_packet_timeout.start();
    mp_process_receive_packet =
      &client_t::process_receive_status_inform;
  }
}

void irs::dcp::client_t::process_receive_status_inform()
{
  const bool timeout = m_receive_packet_timeout.check();
  if (m_server_packet_flow.get_receive_status() == irs_st_ready) {
    if (m_receive_packet.operation_code == status_inform) {
      if (m_devices.size() < m_divece_max_count) {
        mxmac_t mac = m_receive_packet.device_mac;
        device_info_t device_info;
        device_info.code = convert_to_device_code(m_receive_packet.device_code);
        device_info.hardware_version =
          irs::str_conv<string_type>(
          std::string(m_receive_packet.hardware_version));
        device_info.firmware_version =
          irs::str_conv<string_type>(
          std::string(m_receive_packet.firmware_version));
        device_configuration_t config;
        config.ip = m_receive_packet.device_ip;
        config.netmask = m_receive_packet.device_netmask;
        config.gateway = m_receive_packet.device_gateway;
        device_info.device_configuration = config;
        m_devices[mac] = device_info;
        mp_process_receive_packet =
          &client_t::process_wait_create_channel;
        if (m_receive_packet.options == option_wait_commands_timeout) {
          packet_t packet = m_receive_packet;
          packet.operation_code = wait_command;
          packet.options = 0;
          send_packet(packet);
        }
      }
    }
    mp_process_receive_packet = &client_t::process_wait_create_channel;
  } else if ((m_server_packet_flow.get_receive_status() == irs_st_error) ||
    timeout) {
    m_server_packet_flow.abort();
    mp_process_receive_packet =
      &client_t::process_wait_create_channel;
  }
}

irs::device_code_t
irs::dcp::client_t::convert_to_device_code(irs_u32 a_code)
{
  device_code_t device_code = device_code_unknown;
  if (is_known_device_code(a_code)) {
    device_code = static_cast<device_code_t>(a_code);
  }
  return device_code;
}

// class device_t
irs::dcp::device_t::device_t(const configuration_t& a_configuration
):
  mp_hardflow(NULL),
  m_channel_id(irs::hardflow_t::invalid_channel),
  m_client_channel_id(irs::hardflow_t::invalid_channel),
  m_server_packet_flow(NULL),
  m_client_packet_flow(NULL),
  m_config(a_configuration),
  m_timeout(std::numeric_limits<counter_t>::max()/2),
  m_receive_packet_timeout(make_cnt_s(1)),
  //m_offered_ip(mxip_t::any_ip()),
  //m_use_dhcp(false),
  m_status(irs_st_ready),
  mp_process(&device_t::process_wait_command),
  m_send_packet(),
  m_receive_packet()
{
  IRS_LIB_ASSERT(m_config.client_hardflow);
  IRS_LIB_ASSERT(m_config.server_hardflow);
  /*if (((m_config.mode == mode_active) ||
    (m_config.mode == mode_active_passive)) && !m_config.client_hardflow) {
    throw std::logic_error("В конфигурации указан режим, "
      "при котором требуется указать configuration_t::client_hardflow");
  }
  if (((m_config.mode == mode_passive) ||
    (m_config.mode == mode_active_passive)) && !m_config.server_hardflow) {
    throw std::logic_error("В конфигурации указан режим, "
      "при котором требуется configuration_t::server_hardflow");
  }*/
  //m_channel_id = m_config.server_hardflow->channel_next();
  m_client_channel_id = m_config.client_hardflow->channel_next();
  m_server_packet_flow.hardflow(m_config.server_hardflow);
  m_client_packet_flow.hardflow(m_config.client_hardflow);
  mp_hardflow = m_config.server_hardflow;
}

void irs::dcp::device_t::wait_configuration()
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_status = irs_st_busy;
  //m_timeout.start();
  mp_process = &device_t::process_wait_create_channel;
}

void irs::dcp::device_t::inform_status_and_wait_configuration()
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_status = irs_st_busy;
  m_send_packet.operation_code = status_inform;

  init_packet_send_from_configuration();
  /*m_send_packet.device_mac = m_config.mac;
  m_send_packet.device_code =
    static_cast<irs_u32>(m_config.device_code);

  const std::string hardware_ver = irs::str_conv<std::string>(
    m_config.hardware_version);
  const size_type hardware_ver_size =
    std::min<size_type>(hardware_version_max_size, hardware_ver.size());
  std::memcpy(m_send_packet.hardware_version, hardware_ver.c_str(),
    hardware_ver_size);
  m_send_packet.hardware_version[hardware_ver_size] = '\0';

  const std::string firmware_ver = irs::str_conv<std::string>(
    m_config.firmware_version);
  const size_type firmware_ver_size =
    std::min<size_type>(firmware_version_max_size, firmware_ver.size());
  std::memcpy(m_send_packet.firmware_version, firmware_ver.c_str(),
    firmware_ver_size);
  m_send_packet.hardware_version[firmware_ver_size] = '\0';

  m_send_packet.device_ip = m_config.device_configuration.ip;
  m_send_packet.device_netmask = m_config.device_configuration.netmask;
  m_send_packet.device_gateway = m_config.device_configuration.gateway;
  if (m_config.device_configuration.dhcp_enabled) {
    m_send_packet.options |= option_use_dhcp;
  }
  */
  m_send_packet.options |= option_wait_commands;
  m_client_packet_flow.send(m_client_channel_id, m_send_packet);
  mp_process = &device_t::process_send_status_inform;
}

void irs::dcp::device_t::inform_status_and_wait_configuration(double a_timeout)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_status = irs_st_busy;
  m_send_packet.operation_code = status_inform;
  init_packet_send_from_configuration();
  /*
  m_send_packet.device_mac = m_config.mac;
  m_send_packet.device_code =
    static_cast<irs_u32>(m_config.device_code);
  m_send_packet.device_ip = m_config.device_configuration.ip;
  m_send_packet.device_netmask = m_config.device_configuration.netmask;
  m_send_packet.device_gateway = m_config.device_configuration.gateway;
  if (m_config.device_configuration.dhcp_enabled) {
    m_send_packet.options |= option_use_dhcp;
  }
  */
  m_send_packet.options |= option_wait_commands_timeout;
  m_client_packet_flow.send(m_client_channel_id, m_send_packet);
  m_timeout.set(make_cnt_s(a_timeout));
  m_timeout.start();
  mp_process = &device_t::process_send_status_inform;
}

irs_status_t irs::dcp::device_t::get_status() const
{
  return m_status;
}

void irs::dcp::device_t::tick()
{
  m_server_packet_flow.tick();
  m_client_packet_flow.tick();
  (this->*mp_process)();
}

const irs::dcp::device_configuration_t&
irs::dcp::device_t::get_offered_configuration() const
{
  return m_offered_device_configuration;
}

/*bool irs::dcp::device_t::use_dhcp() const
{
  return m_use_dhcp;
}*/

void irs::dcp::device_t::process_wait_command()
{
  // Ожидаем команд
}

void irs::dcp::device_t::process_wait_create_channel()
{
  m_channel_id = m_config.server_hardflow->channel_next();
  if (m_timeout.check()) {
    m_status = irs_st_error;
    mp_process = &device_t::process_wait_command;
    return;
  }
  if (m_channel_id != irs::hardflow_t::invalid_channel) {
    m_server_packet_flow.receive(m_channel_id, &m_receive_packet);
    m_receive_packet_timeout.start();
    mp_process = &device_t::process_receive_status_request;
  }
}

void irs::dcp::device_t::process_receive_status_request()
{
  if (m_timeout.check()) {
    m_server_packet_flow.abort();
    m_status = irs_st_error;
    mp_process = &device_t::process_wait_command;
    return;
  }
  if (m_server_packet_flow.get_receive_status() == irs_st_ready) {
    if (m_receive_packet.operation_code == device_status_request) {
      m_send_packet = m_receive_packet;
      m_send_packet.operation_code = status_inform;
      init_packet_send_from_configuration();
      /*
      m_send_packet.device_mac = m_config.mac;
      m_send_packet.device_code =
        static_cast<irs_u32>(m_config.device_code);
      m_send_packet.device_ip = m_config.device_configuration.ip;
      m_send_packet.device_netmask = m_config.device_configuration.netmask;
      m_send_packet.device_gateway = m_config.device_configuration.gateway;
      if (m_config.device_configuration.dhcp_enabled) {
        m_send_packet.options |= option_use_dhcp;
      }*/
      m_client_packet_flow.send(m_client_channel_id, m_send_packet);
      mp_process = &device_t::process_send_status_inform;
    } else if ((m_receive_packet.operation_code == address_offer) &&
      (m_receive_packet.device_mac == m_config.mac)) {
      m_offered_device_configuration.ip = m_receive_packet.device_ip;
      m_offered_device_configuration.netmask = m_receive_packet.device_netmask;
      m_offered_device_configuration.gateway = m_receive_packet.device_gateway;
      m_offered_device_configuration.dhcp_enabled =
        (m_receive_packet.options & option_use_dhcp);
      if ((m_offered_device_configuration.ip != mxip_t::any_ip()) ||
        m_offered_device_configuration.dhcp_enabled) {
        m_status = irs_st_ready;
        mp_process = &device_t::process_wait_command;
      } else {
        mp_process = &device_t::process_wait_create_channel;
      }
    } else if ((m_receive_packet.operation_code == wait_command) &&
      (m_receive_packet.device_mac == m_config.mac)) {
      m_timeout.stop();
      mp_process = &device_t::process_wait_create_channel;
    } else {
      mp_process = &device_t::process_wait_create_channel;
    }
  } else if ((m_server_packet_flow.get_receive_status() == irs_st_error) ||
    m_receive_packet_timeout.check()) {
    m_server_packet_flow.abort();
    mp_process = &device_t::process_wait_create_channel;
  }
}

void irs::dcp::device_t::process_send_status_inform()
{
  if (m_timeout.check()) {
    m_client_packet_flow.abort();
    m_status = irs_st_error;
    mp_process = &device_t::process_wait_command;
    return;
  }
  if ((m_client_packet_flow.get_send_status() == irs_st_ready) ||
    (m_client_packet_flow.get_send_status() == irs_st_error)) {
    /*if (m_client_packet_flow.get_send_status() == irs_st_ready) {
      m_status = irs_st_ready;
    } else {
      m_status = irs_st_error;
    }*/
    mp_process = &device_t::process_wait_create_channel;
  }
}

void irs::dcp::device_t::init_packet_send_from_configuration()
{
  m_send_packet.device_mac = m_config.mac;
  m_send_packet.device_code =
    static_cast<irs_u32>(m_config.device_code);

  const std::string hardware_ver = irs::str_conv<std::string>(
    m_config.hardware_version);
  const size_type hardware_ver_size =
    std::min<size_type>(hardware_version_max_size, hardware_ver.size());
  memcpy(m_send_packet.hardware_version, hardware_ver.c_str(),
    hardware_ver_size);
  m_send_packet.hardware_version[hardware_ver_size] = '\0';

  const std::string firmware_ver = irs::str_conv<std::string>(
    m_config.firmware_version);
  const size_type firmware_ver_size =
    std::min<size_type>(firmware_version_max_size, firmware_ver.size());
  memcpy(m_send_packet.firmware_version, firmware_ver.c_str(),
    firmware_ver_size);
  m_send_packet.firmware_version[firmware_ver_size] = '\0';

  m_send_packet.device_ip = m_config.device_configuration.ip;
  m_send_packet.device_netmask = m_config.device_configuration.netmask;
  m_send_packet.device_gateway = m_config.device_configuration.gateway;
  if (m_config.device_configuration.dhcp_enabled) {
    m_send_packet.options |= option_use_dhcp;
  }
}

#endif //__ICCAVR__
