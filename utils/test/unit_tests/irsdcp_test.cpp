#include <irsdcp.h>

#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE test_hardflowg

BOOST_AUTO_TEST_SUITE(test_dcp)

BOOST_AUTO_TEST_CASE(test_packet_flow_case_1)
{
  using namespace irs;
  using namespace irs::hardflow;
  using namespace irs::dcp;

  echo_t echo;

  packet_t packet_1;
  packet_1.operation_code = device_request;
  packet_1.device_code = device_code_u5023m;
  packet_1.device_mac = make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x1);
  packet_1.device_ip = make_mxip(192, 168, 0, 1);
  packet_1.options = 0;
  packet_t packet_2;
  packet_2.operation_code = status_inform;
  packet_2.device_code= device_code_itn_2400;
  packet_2.device_mac = make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x2);
  packet_2.device_ip = make_mxip(192, 168, 0, 2);
  packet_2.options = 1;

  std::size_t channel_id = echo.channel_next();
  packet_flow_t packet_flow(&echo);
  packet_flow.send(channel_id, packet_1);

  while (packet_flow.get_send_status() == irs_st_busy) {
    packet_flow.tick();
  }
  BOOST_CHECK_EQUAL(packet_flow.get_send_status(), irs_st_ready);
  packet_flow.send(channel_id, packet_2);
  while (packet_flow.get_send_status() == irs_st_busy) {
    packet_flow.tick();
  }
  BOOST_CHECK_EQUAL(packet_flow.get_send_status(), irs_st_ready);

  packet_t packet_recv;
  packet_flow.receive(channel_id, &packet_recv);
  while (packet_flow.get_receive_status() == irs_st_busy) {
    packet_flow.tick();
  }
  BOOST_CHECK_EQUAL(packet_flow.get_receive_status(), irs_st_ready);
  BOOST_CHECK(packet_recv == packet_1);
  packet_flow.receive(channel_id, &packet_recv);
  while (packet_flow.get_receive_status() == irs_st_busy) {
    packet_flow.tick();
  }
  BOOST_CHECK_EQUAL(packet_flow.get_receive_status(), irs_st_ready);
  BOOST_CHECK(packet_recv == packet_2);
}

BOOST_AUTO_TEST_CASE(test_device_command_inform_status_and_wait_ip)
{
  using namespace irs::hardflow;
  using namespace irs::dcp;

  echo_t client_echo;
  echo_t server_echo;

  irs::dcp::device_t::configuration_t u5023_config;
  u5023_config.client_hardflow = &client_echo;
  u5023_config.server_hardflow = &server_echo;
  u5023_config.mode = irs::dcp::mode_passive;
  u5023_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x1);
  u5023_config.device_configuration.ip = mxip_t::any_ip();
  u5023_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_device(u5023_config);
  u5023_device.inform_status_and_wait_configuration();
  /*while (u5023_device.get_status() == irs_st_busy) {
    u5023_device.tick();
  }
  BOOST_CHECK_EQUAL(u5023_device.get_status(), irs_st_ready);*/
  std::size_t channel_id = client_echo.channel_next();
  packet_flow_t packet_flow(&client_echo);
  packet_t packet;
  packet_flow.receive(channel_id, &packet);
  while (packet_flow.get_receive_status() == irs_st_busy) {
    u5023_device.tick();
    packet_flow.tick();
  }
  BOOST_CHECK_EQUAL(packet_flow.get_receive_status(), irs_st_ready);

  BOOST_CHECK_EQUAL(packet.device_code, u5023_config.device_code);
  BOOST_CHECK_EQUAL(packet.device_ip, u5023_config.device_configuration.ip);
  BOOST_CHECK_EQUAL(packet.device_mac, u5023_config.mac);
  BOOST_CHECK_EQUAL(packet.operation_code, status_inform);
  BOOST_CHECK_EQUAL(packet.options, option_wait_commands);
}

BOOST_AUTO_TEST_CASE(test_device_command_inform_status_and_wait_ip_case_2)
{
  using namespace irs::hardflow;

  const std::size_t device_count = 3;
  memory_flow_t memory_flow_pc_client(sizeof(irs::dcp::packet_t)*device_count);
  memory_flow_t memory_flow_pc_server(sizeof(irs::dcp::packet_t)*device_count);
  memory_flow_t memory_flow_device_1_client(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_1_server(sizeof(irs::dcp::packet_t));

  memory_flow_pc_client.add_connection(&memory_flow_device_1_server, 1, 1);
  memory_flow_pc_server.add_connection(&memory_flow_device_1_client, 1, 1);

  irs::dcp::client_t client(&memory_flow_pc_client, &memory_flow_pc_server);

  irs::dcp::device_t::configuration_t u5023_config;
  u5023_config.client_hardflow = &memory_flow_device_1_client;
  u5023_config.server_hardflow = &memory_flow_device_1_server;
  u5023_config.mode = irs::dcp::mode_passive;
  u5023_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x1);
  u5023_config.device_configuration.ip = mxip_t::any_ip();
  u5023_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_device(u5023_config);
  u5023_device.inform_status_and_wait_configuration();

  std::map<mxmac_t, irs::dcp::device_info_t> devices;

  irs::timer_t timeout(irs::make_cnt_s(2));
  timeout.start();

  while (!timeout.check() && (devices.size() < 1)) {
    memory_flow_pc_client.tick();
    memory_flow_pc_server.tick();
    memory_flow_device_1_client.tick();
    memory_flow_device_1_server.tick();
    client.tick();
    u5023_device.tick();
    devices = client.get_devices();
  }
  BOOST_CHECK_EQUAL(client.get_status(), irs_st_ready);
  BOOST_CHECK_EQUAL(devices.size(), 1);
  BOOST_CHECK(devices.find(u5023_config.mac) != devices.end());

  irs::dcp::device_info_t device_info = devices.begin()->second;

  BOOST_CHECK_EQUAL(device_info.code, u5023_config.device_code);
  BOOST_CHECK_EQUAL(device_info.device_configuration.ip,
    u5023_config.device_configuration.ip);
}

BOOST_AUTO_TEST_CASE(
  test_device_command_inform_status_and_wait_ip_timeout_case_1)
{
  using namespace irs::hardflow;

  memory_flow_t memory_flow_device_1_client(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_1_server(sizeof(irs::dcp::packet_t));

  irs::dcp::device_t::configuration_t u5023_config;
  u5023_config.client_hardflow = &memory_flow_device_1_client;
  u5023_config.server_hardflow = &memory_flow_device_1_server;
  u5023_config.mode = irs::dcp::mode_passive;
  u5023_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x1);
  u5023_config.device_configuration.ip = mxip_t::any_ip();
  u5023_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_device(u5023_config);
  const double timeout_s = 0.001;
  u5023_device.inform_status_and_wait_configuration(timeout_s);

  irs::timer_t timeout(irs::make_cnt_s(2));
  timeout.start();
  while (!timeout.check() && (u5023_device.get_status() == irs_st_busy)) {
    memory_flow_device_1_client.tick();
    memory_flow_device_1_server.tick();
    u5023_device.tick();
  }
  BOOST_CHECK_EQUAL(u5023_device.get_status(), irs_st_error);
}

BOOST_AUTO_TEST_CASE(
  test_device_command_inform_status_and_wait_ip_timeout_case_2)
{
  using namespace irs::hardflow;

  memory_flow_t memory_flow_pc_client(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_pc_server(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_1_client(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_1_server(sizeof(irs::dcp::packet_t));

  memory_flow_pc_client.add_connection(&memory_flow_device_1_server, 1, 1);
  memory_flow_pc_server.add_connection(&memory_flow_device_1_client, 1, 1);

  irs::dcp::client_t client(&memory_flow_pc_client, &memory_flow_pc_server);

  irs::dcp::device_t::configuration_t u5023_config;
  u5023_config.client_hardflow = &memory_flow_device_1_client;
  u5023_config.server_hardflow = &memory_flow_device_1_server;
  u5023_config.mode = irs::dcp::mode_passive;
  u5023_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x1);
  u5023_config.device_configuration.ip = mxip_t::any_ip();
  u5023_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_device(u5023_config);
  const double timeout_s = 0.01;
  u5023_device.inform_status_and_wait_configuration(timeout_s);

  irs::timer_t timeout(irs::make_cnt_s(0.03));
  timeout.start();
  while (!timeout.check() && (u5023_device.get_status() == irs_st_busy)) {
    memory_flow_pc_client.tick();
    memory_flow_pc_server.tick();
    memory_flow_device_1_client.tick();
    memory_flow_device_1_server.tick();
    client.tick();
    u5023_device.tick();
  }
  BOOST_CHECK_EQUAL(u5023_device.get_status(), irs_st_busy);
}

BOOST_AUTO_TEST_CASE(test_client_command_get_device_status)
{
  using namespace irs::hardflow;

  const std::size_t device_count = 3;
  memory_flow_t memory_flow_pc_client(sizeof(irs::dcp::packet_t)*device_count);
  memory_flow_t memory_flow_pc_server(sizeof(irs::dcp::packet_t)*device_count);
  memory_flow_t memory_flow_device_1_client(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_1_server(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_2_client(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_2_server(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_3_client(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_3_server(sizeof(irs::dcp::packet_t));
  memory_flow_pc_client.add_connection(&memory_flow_device_1_server, 1, 1);
  memory_flow_pc_client.add_connection(&memory_flow_device_2_server, 1, 1);
  memory_flow_pc_client.add_connection(&memory_flow_device_3_server, 1, 1);
  memory_flow_pc_server.add_connection(&memory_flow_device_1_client, 1, 1);
  memory_flow_pc_server.add_connection(&memory_flow_device_2_client, 1, 1);
  memory_flow_pc_server.add_connection(&memory_flow_device_3_client, 1, 1);

  irs::dcp::client_t client(&memory_flow_pc_client, &memory_flow_pc_server);

  irs::dcp::device_t::configuration_t u5023_config;
  u5023_config.client_hardflow = &memory_flow_device_1_client;
  u5023_config.server_hardflow = &memory_flow_device_1_server;
  u5023_config.mode = irs::dcp::mode_passive;
  u5023_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x1);
  u5023_config.device_configuration.ip = mxip_t::any_ip();
  u5023_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_device(u5023_config);
  u5023_device.inform_status_and_wait_configuration();

  irs::dcp::device_t::configuration_t u5023_2_config;
  u5023_2_config.client_hardflow = &memory_flow_device_2_client;
  u5023_2_config.server_hardflow = &memory_flow_device_2_server;
  u5023_2_config.mode = irs::dcp::mode_passive;
  u5023_2_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x2);
  u5023_2_config.device_configuration.ip = mxip_t::any_ip();
  u5023_2_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_2_device(u5023_2_config);
  u5023_2_device.inform_status_and_wait_configuration();

  irs::dcp::device_t::configuration_t itn_config;
  itn_config.client_hardflow = &memory_flow_device_3_client;
  itn_config.server_hardflow = &memory_flow_device_3_server;
  itn_config.mode = irs::dcp::mode_passive;
  itn_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x3);
  itn_config.device_configuration.ip = mxip_t::any_ip();
  itn_config.device_code = irs::device_code_itn_2400;
  irs::dcp::device_t itn_device(itn_config);
  itn_device.inform_status_and_wait_configuration();

  client.command_get_device_status();
  std::map<mxmac_t, irs::dcp::device_info_t> devices;
  irs::timer_t timeout(irs::make_cnt_s(2));
  timeout.start();
  while (!timeout.check() && (devices.size() < device_count)) {
    memory_flow_pc_client.tick();
    memory_flow_pc_server.tick();
    memory_flow_device_1_client.tick();
    memory_flow_device_1_server.tick();
    memory_flow_device_2_client.tick();
    memory_flow_device_2_server.tick();
    memory_flow_device_3_client.tick();
    memory_flow_device_3_server.tick();
    client.tick();
    u5023_device.tick();
    u5023_2_device.tick();
    itn_device.tick();
    devices = client.get_devices();
  }
  BOOST_CHECK_EQUAL(client.get_status(), irs_st_ready);
  BOOST_CHECK_EQUAL(devices.size(), device_count);
  BOOST_CHECK(devices.find(u5023_config.mac) != devices.end());
  BOOST_CHECK(devices.find(u5023_2_config.mac) != devices.end());
  BOOST_CHECK(devices.find(itn_config.mac) != devices.end());

  irs::dcp::device_info_t device_info = devices.find(u5023_config.mac)->second;
  BOOST_CHECK_EQUAL(device_info.code, u5023_config.device_code);
  BOOST_CHECK_EQUAL(device_info.device_configuration.ip,
    u5023_config.device_configuration.ip);

  device_info = devices.find(u5023_2_config.mac)->second;
  BOOST_CHECK_EQUAL(device_info.code, u5023_2_config.device_code);
  BOOST_CHECK_EQUAL(device_info.device_configuration.ip,
    u5023_2_config.device_configuration.ip);

  device_info = devices.find(itn_config.mac)->second;
  BOOST_CHECK_EQUAL(device_info.code, itn_config.device_code);
  BOOST_CHECK_EQUAL(device_info.device_configuration.ip,
    itn_config.device_configuration.ip);
}

/*BOOST_AUTO_TEST_CASE(test_client_command_configure_and_get_device_address)
{
  using namespace irs::hardflow;

  const std::size_t device_count = 3;
  memory_flow_t memory_flow_client(sizeof(irs::dcp::packet_t)*device_count);
  memory_flow_t memory_flow_device_1(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_2(sizeof(irs::dcp::packet_t));
  memory_flow_t memory_flow_device_3(sizeof(irs::dcp::packet_t));
  memory_flow_client.add_connection(&memory_flow_device_1, 1, 1);
  memory_flow_client.add_connection(&memory_flow_device_2, 1, 1);
  memory_flow_client.add_connection(&memory_flow_device_3, 1, 1);
  irs::dcp::client_t client(&memory_flow_client, irs::dcp::mode_active, 1);

  irs::dcp::device_t::configuration_t u5023_config;
  u5023_config.server_hardflow = &memory_flow_device_1;
  u5023_config.mode = irs::dcp::mode_passive;
  u5023_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x1);
  u5023_config.ip = mxip_t::any_ip();
  u5023_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_device(u5023_config);
  u5023_device.start();

  irs::dcp::device_t::configuration_t u5023_2_config;
  u5023_2_config.server_hardflow = &memory_flow_device_2;
  u5023_2_config.mode = irs::dcp::mode_passive;
  u5023_2_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x2);
  u5023_2_config.ip = mxip_t::any_ip();
  u5023_2_config.device_code = irs::device_code_u5023m;
  irs::dcp::device_t u5023_2_device(u5023_2_config);
  u5023_2_device.start();

  irs::dcp::device_t::configuration_t itn_config;
  itn_config.server_hardflow = &memory_flow_device_3;
  itn_config.mode = irs::dcp::mode_passive;
  itn_config.mac = irs::make_mxmac(0x2, 0x10, 0x10, 0x10, 0x10, 0x3);
  itn_config.ip = mxip_t::any_ip();
  itn_config.device_code = irs::device_code_itn_2400;
  irs::dcp::device_t itn_device(itn_config);
  itn_device.start();

  client.command_configure_and_get_device_address();
  irs::timer_t timeout(irs::make_cnt_s(2));
  timeout.start();
  while (!timeout.check() && (client.get_status() == irs_st_busy)) {
    client.tick();
    u5023_device.tick();
    u5023_2_device.tick();
    itn_device.tick();
  }
  BOOST_CHECK_EQUAL(client.get_status(), irs_st_ready);
  std::vector<irs::dcp::device_info_t> devices = client.get_devices();
  BOOST_CHECK_EQUAL(devices.size(), device_count);

  BOOST_CHECK_EQUAL(devices[0].device_code, u5023_config.device_code);
  BOOST_CHECK_EQUAL(devices[0].device_mac, u5023_config.mac);
  BOOST_CHECK_EQUAL(devices[0].device_ip, u5023_config.ip);

  BOOST_CHECK_EQUAL(devices[1].device_code, u5023_2_config.device_code);
  BOOST_CHECK_EQUAL(devices[1].device_mac, u5023_2_config.mac);
  BOOST_CHECK_EQUAL(devices[1].device_ip, u5023_2_config.ip);

  BOOST_CHECK_EQUAL(devices[2].device_code, itn_config.device_code);
  BOOST_CHECK_EQUAL(devices[2].device_mac, itn_config.mac);
  BOOST_CHECK_EQUAL(devices[2].device_ip, itn_config.ip);
}*/

BOOST_AUTO_TEST_SUITE_END()
