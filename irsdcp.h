//! \file
//! \ingroup network_in_out_group
//! \brief Реализация DCP (Device Configuration Protocol) - протокола.
//!
//! Дата создания: 06.05.2013

#ifndef IRSDCP_H
#define IRSDCP_H

#include <irsdefs.h>

#include <hardflowg.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup network_in_out_group
//! @{

namespace dcp {

/*enum device_code_t {
  device_code_any = 0,
  device_code_u5023 = 1
};*/

enum operation_code_t
{
  device_request = 1, //!<  \brief Запрос адресов устройств
  address_request = 2, //!< \brief Устройство запрашивает адрес для себя
  address_offer = 3,  //!< \brief Устройству предлагается адрес
  address_inform = 4, //!< \brief Устройство сообщает свой адрес
  device_status_request = 5, //!< \brief Запрос устройств и их состояния
  status_inform = 6, // !< \brief Устройство сообщает свой статус
  wait_command = 7 //!<\brief Команда устройству "ждать команд бесконечно"
};

enum options_t {
  //! \brief Использовать DHCP для настройки ip-адреса
  option_use_dhcp = 1,
  //! \brief Ожидание команд
  option_wait_commands = 2,
  //! \brief Ожидание команд ограниченное время
  option_wait_commands_timeout = 4
};

enum { hardware_version_max_size = 20 };
enum { firmware_version_max_size = 20 };

#pragma pack(push, 1)
struct packet_t
{
  //enum { id_size = 6 };
  //char protocol_id[id_size];
  //irs_u32 transaction_id;
  irs_u8 operation_code;
  irs_u32 device_code;
  mxmac_t device_mac;
  mxip_t device_ip;
  mxip_t device_netmask;
  mxip_t device_gateway;
  char hardware_version[hardware_version_max_size + 1];
  char firmware_version[firmware_version_max_size + 1];
  irs_u32 options;
  packet_t();
  bool operator==(const packet_t& a_packet) const
  {
    return /*(transaction_id == a_packet.transaction_id) &&*/
      (operation_code == a_packet.operation_code) &&
      (device_code == a_packet.device_code) &&
      (device_mac == a_packet.device_mac) &&
      (device_ip == a_packet.device_ip) &&
      (device_netmask == a_packet.device_netmask) &&
      (device_gateway == a_packet.device_gateway) &&
      (0 == memcmp(hardware_version, a_packet.hardware_version,
        IRS_ARRAYSIZE(hardware_version))) &&
      (0 == memcmp(firmware_version, a_packet.firmware_version,
        IRS_ARRAYSIZE(firmware_version))) &&
      (options == a_packet.options);
  }
};
#pragma pack(pop)

class packet_flow_t
{
public:
  typedef std::size_t size_type;
  packet_flow_t(irs::hardflow_t* ap_hardflow);
  void hardflow(irs::hardflow_t* ap_hardflow);
  void send(size_type a_channel, const packet_t& a_packet);
  void receive(size_type a_channel, packet_t* ap_receive_packet);
  //void receive_timeout(double a_timeout);
  irs_status_t get_send_status() const;
  irs_status_t get_receive_status() const;
  void abort();
  void tick();
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  typedef void (packet_flow_t::*process_fun_t) ();
  void process_wait_command();
  void process_find_packet_begin();
  void process_receive_packet();
  hardflow_t* mp_hardflow;
  hardflow::fixed_flow_t m_fixed_flow;
  //timer_t m_receive_timeout;
  irs_status_t m_receive_status;
  size_type m_receive_channel_id;
  vector<irs_u8> m_send_buf;
  vector<irs_u8> m_receive_buf;
  packet_t* mp_receive_packet;
  process_fun_t mp_process;
};

enum mode_t {
  mode_active,
  mode_passive,
  mode_active_passive
};

struct device_configuration_t
{
  mxip_t ip;
  mxip_t netmask;
  mxip_t gateway;
  bool dhcp_enabled;
  device_configuration_t():
    ip(mxip_t::any_ip()),
    netmask(mxip_t::any_ip()),
    gateway(mxip_t::any_ip()),
    dhcp_enabled(false)
  {
  }
  bool operator==(const device_configuration_t& a_config) const
  {
    return (ip == a_config.ip) &&
      (netmask == a_config.netmask) &&
      (gateway == a_config.gateway) &&
      (dhcp_enabled == a_config.dhcp_enabled);
  }
};

struct device_info_t
{
  device_code_t code;
  irs::string_t hardware_version;
  irs::string_t firmware_version;
  device_configuration_t device_configuration;
  bool operator==(const device_info_t& a_device_info) const
  {
    return (code == a_device_info.code) &&
      (hardware_version == a_device_info.hardware_version) &&
      (firmware_version == a_device_info.firmware_version) &&
      (device_configuration == a_device_info.device_configuration);
  }
};

class client_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  typedef std::map<mxmac_t, device_info_t> devices_type;
  client_t(irs::hardflow_t* ap_client_hardflow,
    irs::hardflow_t* ap_server_hardflow,
    size_type a_divece_max_count = 255);
  void command_offer_configuration(const mxmac_t& a_mac,
    const device_configuration_t& a_config);
  void command_get_device_status();
  irs_status_t get_status() const;
  const devices_type& get_devices() const;
  void delete_devices();
  void abort();
  void tick();
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  typedef void (client_t::*process_fun_t) ();
  void send_packet(const packet_t& a_packet);
  void send_packet();
  bool select_packet_for_send();
  void process_wait_command();
  //void process_wait_send_packets();
  void process_send_packet();
  void process_wait_create_channel();
  void process_receive_status_inform();
  device_code_t convert_to_device_code(irs_u32 a_code);
  irs::hardflow_t* mp_client_hardflow;
  irs::hardflow_t* mp_server_hardflow;
  size_type m_client_channel_id;
  packet_flow_t m_client_packet_flow;
  packet_flow_t m_server_packet_flow;
  timer_t m_receive_packet_timeout;
  mode_t m_mode;
  irs_status_t m_status;
  process_fun_t mp_process_receive_packet;
  process_fun_t mp_process_send_packet;
  enum { send_packet_queue_max_size = 10 };
  enum send_packet_type_t {
    spt_system_packet,
    spt_user_packet
  };
  deque<packet_t> m_send_packets;
  packet_t m_user_send_packet;
  packet_t m_send_packet;
  send_packet_type_t m_send_packet_type;
  bool m_user_packet_exists;
  packet_t m_receive_packet;
  devices_type m_devices;
  size_type m_divece_max_count;
};

class device_t
{
public:
  typedef std::size_t size_type;
  struct configuration_t
  {
    irs::hardflow_t* client_hardflow;
    irs::hardflow_t* server_hardflow;
    mode_t mode;
    device_code_t device_code;
    irs::string_t hardware_version;
    irs::string_t firmware_version;
    mxmac_t mac;
    device_configuration_t device_configuration;
    configuration_t():
      client_hardflow(NULL),
      server_hardflow(NULL),
      mode(mode_active_passive),
      device_code(device_code_u5023m),
      hardware_version(),
      firmware_version(),
      mac(mxmac_t::broadcast_mac()),
      device_configuration()
    {
    }
  };
  device_t(const configuration_t& a_configuration);
  void wait_configuration();
  void inform_status_and_wait_configuration();
  void inform_status_and_wait_configuration(double a_timeout);
  irs_status_t get_status() const;
  void tick();
  const device_configuration_t& get_offered_configuration() const;
  //bool use_dhcp() const;
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  typedef void (device_t::*process_fun_t) ();
  void process_wait_command();
  void process_wait_create_channel();
  void process_receive_status_request();
  void process_send_status_inform();
  void init_packet_send_from_configuration();
  irs::hardflow_t* mp_hardflow;
  size_type m_channel_id;
  size_type m_client_channel_id;
  packet_flow_t m_server_packet_flow;
  packet_flow_t m_client_packet_flow;
  configuration_t m_config;
  timer_t m_timeout;
  timer_t m_receive_packet_timeout;
  device_configuration_t m_offered_device_configuration;
  //bool m_use_dhcp;
  irs_status_t m_status;
  process_fun_t mp_process;
  packet_t m_send_packet;
  packet_t m_receive_packet;
};

} // dcp

//! @}

} // irs

#endif // IRSDCP_H
