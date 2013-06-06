#include <hardflowg.h>

#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE test_hardflowg

BOOST_AUTO_TEST_SUITE(test_memory_flow)

std::set<irs::hardflow_t::size_type>
get_channels(irs::hardflow_t* ap_hardflow, irs::hardflow_t::size_type a_count)
{
  typedef irs::hardflow_t::size_type size_type;
  std::set<size_type> channels;
  for (size_type i = 0; i < a_count; i++) {
    channels.insert(ap_hardflow->channel_next());
  }
  return channels;
}

BOOST_AUTO_TEST_CASE(test_channel_next)
{
  using namespace irs::hardflow;
  typedef irs::hardflow_t::size_type size_type;

  memory_flow_t memory_flow_1;
  memory_flow_t memory_flow_2;
  memory_flow_t memory_flow_3;
  memory_flow_t memory_flow_4;

  BOOST_CHECK(memory_flow_1.channel_next() == irs::hardflow_t::invalid_channel);

  memory_flow_1.add_connection(&memory_flow_2, 1, 1);
  memory_flow_1.add_connection(&memory_flow_3, 2, 1);
  memory_flow_1.add_connection(&memory_flow_4, 2, 1);

  std::set<size_type> channels = get_channels(&memory_flow_1, 3);

  BOOST_CHECK_EQUAL(channels.size(), 2);
  BOOST_CHECK(channels.find(1) != channels.end());
  BOOST_CHECK(channels.find(2) != channels.end());

  memory_flow_1.delete_connection(&memory_flow_4, 2, 1);

  channels = get_channels(&memory_flow_1, 3);

  BOOST_CHECK_EQUAL(channels.size(), 2);
  BOOST_CHECK(channels.find(1) != channels.end());
  BOOST_CHECK(channels.find(2) != channels.end());

  memory_flow_1.delete_connection(&memory_flow_3, 2, 1);

  channels = get_channels(&memory_flow_1, 3);

  BOOST_CHECK_EQUAL(channels.size(), 1);
  BOOST_CHECK(channels.find(1) != channels.end());
}

BOOST_AUTO_TEST_CASE(test_read_write)
{
  using namespace irs::hardflow;

  const std::size_t channel_receive_buffer_max_size = 123;

  memory_flow_t memory_flow_1(channel_receive_buffer_max_size);
  memory_flow_t memory_flow_2(channel_receive_buffer_max_size);
  memory_flow_t memory_flow_3(channel_receive_buffer_max_size);
  memory_flow_t memory_flow_4(channel_receive_buffer_max_size);

  memory_flow_1.add_connection(&memory_flow_2, 1, 1);
  memory_flow_1.add_connection(&memory_flow_3, 2, 1);
  memory_flow_1.add_connection(&memory_flow_4, 2, 1);

  BOOST_CHECK(memory_flow_1.is_channel_exists(1));
  BOOST_CHECK(memory_flow_1.is_channel_exists(2));
  BOOST_CHECK(memory_flow_2.is_channel_exists(1));
  BOOST_CHECK(memory_flow_3.is_channel_exists(1));
  BOOST_CHECK(memory_flow_4.is_channel_exists(1));

  const std::size_t buffer_size = 1000;

  vector<irs_u8> data_1(buffer_size);
  for (std::size_t i = 0; i < data_1.size(); i++) {
    data_1[i] = i;
  }
  vector<irs_u8> data_2(buffer_size);
  for (std::size_t i = 0; i < data_2.size(); i++) {
    data_2[i] = i*2;
  }
  vector<irs_u8> data_3(buffer_size);
  for (std::size_t i = 0; i < data_3.size(); i++) {
    data_3[i] = i*3;
  }
  vector<irs_u8> data_4(buffer_size*2, 7);

  vector<irs_u8> buffer1_1(buffer_size);
  vector<irs_u8> buffer1_2(buffer_size*2);
  vector<irs_u8> buffer_2(buffer_size);
  vector<irs_u8> buffer_3(buffer_size);
  vector<irs_u8> buffer_4(buffer_size);

  fixed_flow_t fixed_flow1_1(&memory_flow_1);
  fixed_flow_t fixed_flow1_2(&memory_flow_1);
  fixed_flow_t fixed_flow2(&memory_flow_2);
  fixed_flow_t fixed_flow3(&memory_flow_3);
  fixed_flow_t fixed_flow4(&memory_flow_4);

  const double timeout = 1;
  fixed_flow1_1.read_timeout(irs::make_cnt_s(timeout));
  fixed_flow1_1.write_timeout(irs::make_cnt_s(timeout));
  fixed_flow1_2.read_timeout(irs::make_cnt_s(timeout));
  fixed_flow1_2.write_timeout(irs::make_cnt_s(timeout));
  fixed_flow2.read_timeout(irs::make_cnt_s(timeout));
  fixed_flow2.write_timeout(irs::make_cnt_s(timeout));
  fixed_flow3.read_timeout(irs::make_cnt_s(timeout));
  fixed_flow3.write_timeout(irs::make_cnt_s(timeout));
  fixed_flow4.read_timeout(irs::make_cnt_s(timeout));
  fixed_flow4.write_timeout(irs::make_cnt_s(timeout));

  fixed_flow1_1.read(1, irs::vector_data(buffer1_1), buffer1_1.size());
  fixed_flow1_1.write(1, irs::vector_data(data_1), data_1.size());
  fixed_flow1_2.read(2, irs::vector_data(buffer1_2), buffer1_2.size());
  fixed_flow1_2.write(2, irs::vector_data(data_2), data_2.size());
  fixed_flow2.read(1, irs::vector_data(buffer_2), buffer_2.size());
  fixed_flow2.write(1, irs::vector_data(data_3), data_3.size());
  fixed_flow3.read(1, irs::vector_data(buffer_3), buffer_3.size());
  fixed_flow3.write(1, irs::vector_data(data_4), data_4.size()/2);
  fixed_flow4.read(1, irs::vector_data(buffer_4), buffer_4.size());
  fixed_flow4.write(1, irs::vector_data(data_4), data_4.size()/2);
  while (
    (fixed_flow1_1.read_status() == fixed_flow_t::status_wait) ||
    (fixed_flow1_1.write_status() == fixed_flow_t::status_wait) ||
    (fixed_flow1_2.read_status() == fixed_flow_t::status_wait) ||
    (fixed_flow1_2.write_status() == fixed_flow_t::status_wait) ||
    (fixed_flow2.read_status() == fixed_flow_t::status_wait) ||
    (fixed_flow2.write_status() == fixed_flow_t::status_wait) ||
    (fixed_flow3.read_status() == fixed_flow_t::status_wait) ||
    (fixed_flow3.write_status() == fixed_flow_t::status_wait) ||
    (fixed_flow4.read_status() == fixed_flow_t::status_wait) ||
    (fixed_flow4.write_status() == fixed_flow_t::status_wait)) {

    memory_flow_1.tick();
    memory_flow_2.tick();
    memory_flow_3.tick();
    memory_flow_4.tick();

    fixed_flow1_1.tick();
    fixed_flow1_2.tick();
    fixed_flow2.tick();
    fixed_flow3.tick();
    fixed_flow4.tick();
  }

  BOOST_CHECK_EQUAL(fixed_flow1_1.read_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow1_1.write_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow1_2.read_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow1_2.write_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow2.read_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow2.write_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow3.read_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow3.write_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow4.read_status(), fixed_flow_t::status_success);
  BOOST_CHECK_EQUAL(fixed_flow4.write_status(), fixed_flow_t::status_success);

  BOOST_CHECK(buffer1_1 == data_3);
  BOOST_CHECK(buffer1_2 == data_4);
  BOOST_CHECK(buffer_2 == data_1);
  BOOST_CHECK(buffer_3 == data_2);
  BOOST_CHECK(buffer_4 == data_2);

  BOOST_CHECK_EQUAL(memory_flow_2.received_data_size(1), 0);
  BOOST_CHECK_EQUAL(memory_flow_3.received_data_size(1), 0);
  BOOST_CHECK_EQUAL(memory_flow_4.received_data_size(1), 0);
}

BOOST_AUTO_TEST_SUITE_END()
