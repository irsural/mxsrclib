#include <irshfftp.h>

#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE test_irshfftp

namespace {

class memory_files_t: public irs::hfftp::files_t
{
public:
  memory_files_t(const string_type& a_file_name);
  virtual irs::hfftp::file_read_only_t* open_for_read(
    const string_type& a_file_name,
    irs::hfftp::file_error_t* ap_error_code);
  virtual irs::hfftp::file_write_only_t* open_for_write(
    const string_type& a_file_name,
    irs::hfftp::file_error_t* ap_error_code);
  virtual void close(irs::hfftp::file_base_t* ap_file);
  irs_status_t get_status();
  size_type data_size() const;
  void get_data(irs_u8* ap_buf, size_type a_size);
  void set_data(const irs_u8* ap_buf, size_type a_size);
private:
  memory_files_t();
  bool is_file_allowed(const string_type& a_file_name,
    irs::hfftp::file_error_t* ap_error_code);
  string_type m_file_name;
  irs::handle_t<irs::hfftp::memory_file_read_only_t> mp_memory_file_read_only;
  irs::handle_t<irs::hfftp::memory_file_write_only_t> mp_memory_file_write_only;
  std::vector<irs_u8> m_data;
};

memory_files_t::memory_files_t(const string_type& a_file_name):
  m_file_name(a_file_name),
  mp_memory_file_read_only(),
  mp_memory_file_write_only(),
  m_data()
{
}

irs::hfftp::file_read_only_t* memory_files_t::open_for_read(
  const string_type& a_file_name,
  irs::hfftp::file_error_t* ap_error_code)
{
  if (is_file_allowed(a_file_name, ap_error_code)) {
    mp_memory_file_read_only.reset(new irs::hfftp::memory_file_read_only_t(
      irs::vector_data(m_data), m_data.size()));
    return mp_memory_file_read_only.get();
  } else {
    return IRS_NULL;
  }
}

irs::hfftp::file_write_only_t* memory_files_t::open_for_write(
  const string_type& a_file_name,
  irs::hfftp::file_error_t* ap_error_code)
{
  if (is_file_allowed(a_file_name, ap_error_code)) {
    mp_memory_file_write_only.reset(new irs::hfftp::memory_file_write_only_t());
    return mp_memory_file_write_only.get();
  } else {
    return IRS_NULL;
  }
}

bool memory_files_t::is_file_allowed(const string_type& a_file_name,
  irs::hfftp::file_error_t* ap_error_code)
{
  if (m_file_name != a_file_name) {
    *ap_error_code = irs::hfftp::file_error_file_not_found;
    return false;
  }
  if (!mp_memory_file_read_only.is_empty() ||
    !mp_memory_file_write_only.is_empty()) {
    *ap_error_code = irs::hfftp::file_error_access_violation;
    return false;
  }
  *ap_error_code = irs::hfftp::file_error_no_error;
  return true;
}

void memory_files_t::close(irs::hfftp::file_base_t* ap_file)
{
  if (mp_memory_file_read_only.get() == ap_file) {
    mp_memory_file_read_only.reset();
  } else if (mp_memory_file_write_only.get() == ap_file) {
    m_data.resize(mp_memory_file_write_only->size());
    mp_memory_file_write_only->read(
      irs::vector_data(m_data), m_data.size());
    mp_memory_file_write_only.reset();
  } else {
    IRS_LIB_ASSERT_MSG("Недопустимый указатель");
  }
}

memory_files_t::size_type memory_files_t::data_size() const
{
  return m_data.size();
}

void memory_files_t::get_data(irs_u8* ap_buf, size_type a_size)
{
  const size_type count = std::min(a_size, m_data.size());
  irs::memcpyex(ap_buf, irs::vector_data(m_data), count);
}

void memory_files_t::set_data(const irs_u8* ap_buf, size_type a_size)
{
  m_data.resize(a_size);
  irs::memcpyex(irs::vector_data(m_data), ap_buf, a_size);
}

} // unnamed namespace

/*struct fixture {
    F() : i( 0 ) { BOOST_TEST_MESSAGE( "setup fixture" ); }
    ~F()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

    int i;
};*/

BOOST_AUTO_TEST_SUITE(test_hfftp)

BOOST_AUTO_TEST_CASE(test_memory_file_read_only)
{
  std::vector<irs_u8> data(100);
  generate(data.begin(), data.end(), std::rand);
  irs::hfftp::memory_file_read_only_t file(irs::vector_data(data), data.size());
  BOOST_CHECK_EQUAL(file.size(), data.size());
  BOOST_CHECK(file.size_is_known());
  BOOST_CHECK(!file.fail());
  std::vector<irs_u8> buf(file.size(), 1);
  std::size_t count =
    file.read(irs::vector_data(buf), buf.size());
  BOOST_CHECK_EQUAL(count, data.size());
  BOOST_CHECK(file.end_of_file() == false);
  count = file.read(irs::vector_data(buf), buf.size());
  BOOST_CHECK_EQUAL(count, 0);
  BOOST_CHECK(file.end_of_file() == true);
  BOOST_CHECK(!file.fail());
  BOOST_CHECK(data == buf);
}

BOOST_AUTO_TEST_CASE(test_memory_file_write_only)
{
  std::vector<irs_u8> data(100);
  const std::size_t max_size = 150;
  generate(data.begin(), data.end(), std::rand);
  irs::hfftp::memory_file_write_only_t file(max_size);
  BOOST_CHECK_EQUAL(file.size(), 0);
  BOOST_CHECK(file.max_size_is_known());
  BOOST_CHECK_EQUAL(file.max_size(), max_size);
  BOOST_CHECK(!file.fail());
  std::size_t current_count = file.write(irs::vector_data(data), data.size());
  BOOST_CHECK(!file.fail());
  std::size_t count = current_count;
  BOOST_CHECK_EQUAL(current_count, data.size());
  current_count = file.write(irs::vector_data(data), data.size());
  count += current_count;
  BOOST_CHECK_EQUAL(count, max_size);
  BOOST_CHECK(file.fail());
  BOOST_CHECK_EQUAL(file.size(), max_size);
  std::vector<irs_u8> buf(data.size(), 1);
  std::size_t read_count = file.read(irs::vector_data(buf), buf.size());
  BOOST_CHECK_EQUAL(read_count, buf.size());
  BOOST_CHECK(buf == data);
  file.write_abort();
  BOOST_CHECK_EQUAL(file.size(), 0);
}

BOOST_AUTO_TEST_CASE(test_field_simple_type)
{
  const int value_src = 9;
  int value = value_src;
  irs::hfftp::field_simple_type_t<int> field(&value);
  std::vector<irs_u8> buf;
  field.push_back(&buf);
  std::size_t pos = 0;
  field.read(buf, &pos);
  BOOST_CHECK(pos == sizeof(value));
  BOOST_CHECK(value_src == value);
}

BOOST_AUTO_TEST_CASE(test_field_data)
{
  std::vector<irs_u8> data_src(3);
  data_src[0] = 0;
  data_src[1] = 10;
  data_src[2] = 20;
  std::vector<irs_u8> data = data_src;
  irs::hfftp::field_data_t field(&data);
  std::vector<irs_u8> buf;
  field.push_back(&buf);
  std::size_t pos = 0;
  field.read(buf, &pos);
  BOOST_CHECK(pos == (data.size() + sizeof(irs::hfftp::data_size_t)));
  BOOST_CHECK(data_src == data);
}

BOOST_AUTO_TEST_CASE(test_field_string)
{
  irs::string_t str_src = 192;
  irs::string_t str = str_src;
  irs::hfftp::field_string_t field(&str);
  std::vector<irs_u8> buf;
  field.push_back(&buf);
  std::size_t pos = 0;
  field.read(buf, &pos);
  BOOST_CHECK(pos == (str.size() + sizeof(irs::hfftp::data_size_t)));
  BOOST_CHECK(str_src == str);
}

namespace {

using namespace irs::hfftp;
bool options_equals(
  const std::map<option_name_t, irs::handle_t<option_value_t> >& options1,
  const std::map<option_name_t, irs::handle_t<option_value_t> >& options2)
{
  if (options1.size() != options2.size()) {
    return false;
  }
  std::map<option_name_t, irs::handle_t<option_value_t> >::const_iterator it1 =
    options1.begin();
  std::map<option_name_t, irs::handle_t<option_value_t> >::const_iterator it2 =
    options2.begin();
  while (it1 != options1.end()) {
    if (it1->first != it2->first) {
      return false;
    }
    if (it1->second.is_empty() || it2->second.is_empty()) {
      if (it1->second.is_empty() != it2->second.is_empty()) {
        return false;
      }
    } else if (!it1->second->equals(it2->second.get())) {
      return false;
    }
    ++it1;
    ++it2;
  }
  return true;
}

}

BOOST_AUTO_TEST_CASE(test_field_options)
{
  using namespace irs::hfftp;
  std::map<option_name_t, irs::handle_t<option_value_t> > options_src;

  irs::handle_t<option_value_t> get_file_size_option_value;
  options_src.insert(make_pair(option_get_file_size,
    get_file_size_option_value));
  irs::handle_t<option_value_t> file_size_option_value(
    new option_value_simple_type_t<irs_u32>());
  static_cast<option_value_simple_type_t<irs_u32>* >(
    file_size_option_value.get())->set(10);
  options_src.insert(make_pair(option_file_size, file_size_option_value));

  std::map<option_name_t, irs::handle_t<option_value_t> > options = options_src;
  irs::hfftp::field_options_t field(&options);
  std::vector<irs_u8> buf;
  field.push_back(&buf);
  std::size_t pos = 0;
  field.read(buf, &pos);
  BOOST_CHECK(pos == sizeof(option_count_t) + sizeof(option_code_t) +
    sizeof(irs_u32) + sizeof(option_code_t));
  BOOST_CHECK(options_equals(options_src, options));
}

BOOST_AUTO_TEST_CASE(test_send_read_request)
{
  irs::string_t remote_file_name = irst("test");

  irs::hfftp::packet_read_request_t::fields_t fields;
  fields.file_name = irst("test");
  fields.data_max_size = 1234;
  fields.timeout_ms = 9;
  fields.retransmit_count = 4;

  /*protocol_configuration_t protocol_configuration;
  protocol_configuration.transmission_mode =
    static_cast<irs::hfftp::transmission_mode_t>(fields.transmission_mode);
  protocol_configuration.data_max_size =
    fields.data_max_size;
  protocol_configuration.timeout =
    fields.timeout_ms/1000.;
  protocol_configuration.retransmit_max_count =
    fields.retransmit_count;*/

  irs::hardflow::echo_t echo;
  std::size_t channel = echo.channel_next();
  irs::hfftp::packet_flow_t sender(&echo);
  sender.enable_send_timeout(1);
  sender.send_read_request(channel, fields);
  while (sender.status() == irs_st_busy) {
    sender.tick();
  }
  BOOST_CHECK(sender.status() == irs_st_ready);

  irs::hfftp::packet_flow_t receiver(&echo);
  receiver.enable_receive_timeout(1);
  receiver.receive_packet(channel);
  while (receiver.status() == irs_st_busy) {
    receiver.tick();
  }
  BOOST_CHECK(receiver.status() == irs_st_ready);
  irs::hfftp::packet_t* packet = receiver.get_packet();
  BOOST_CHECK(packet);
  irs::hfftp::packet_read_request_t* packet_read_request =
    dynamic_cast<irs::hfftp::packet_read_request_t*>(packet);
  BOOST_CHECK(packet_read_request);

  irs::hfftp::packet_read_request_t::fields_t recv_fields =
    packet_read_request->get_fields();
  BOOST_CHECK(fields == recv_fields);
  BOOST_CHECK(fields.data_max_size == recv_fields.data_max_size);
  BOOST_CHECK(fields.file_name == recv_fields.file_name);
  BOOST_CHECK(fields.retransmit_count == recv_fields.retransmit_count);
  BOOST_CHECK(fields.timeout_ms == recv_fields.timeout_ms);
}

/*BOOST_AUTO_TEST_CASE(test_case1)
{
  const std::string source_file_name = "source_hfftp_test_data";
  const std::string destination_file_name = "destination_hfftp_test_data";
  typedef irs_u32 buf_elem_type;
  std::vector<buf_elem_type> source_buf(1000);
  std::ofstream ofile(source_file_name, ios::binary|ios::trunc);
  BOOST_CHECK(ofile.good());
  for (std::size_t i = 0; i < source_buf.size(); i++) {
    source_buf[i] = i;
  }
  ofile.write(reinterpret_cast<char*>(&source_buf.front()),
    source_buf.size()*sizeof(buf_elem_type));
  BOOST_CHECK(ofile.good());
  ofile.close();

  irs::handle_t<irs::hfftp::ofstream_file_t> ofstream_file(
    new irs::hfftp::ofstream_file_t(irs::str_conv<irs::string_t>(
    destination_file_name)));

  irs::hfftp::fstream_files_t fstream_files;

  const irs::string_t server_port = irst("5015");

  irs::hardflow::udp_flow_t server_hardflow(irst(""), server_port,
    irst(""), irst(""), irs::hardflow::udplc_mode_queue, 1000,
    0xFFFF, false, 24*60*60, true, 10);

  irs::hardflow::udp_flow_t client_hardflow(irst(""), irst(""),
    irst("127.0.0.1"), server_port,
    irs::hardflow::udplc_mode_limited, 1, 0xFFFF, false, 24*60*60, false, 10);

  irs::hfftp::client_t hfftp_client(&client_hardflow);
  irs::hfftp::server_t hfftp_server(&server_hardflow, &fstream_files, 3);

  hfftp_client.get(irs::str_conv<irs::string_t>(source_file_name),
    ofstream_file.get());

  BOOST_CHECK(hfftp_client.status() == irs_st_busy);

  while (hfftp_client.status() == irs_st_busy) {
    client_hardflow.tick();
    server_hardflow.tick();
    hfftp_client.tick();
    hfftp_server.tick();
  }
  ofstream_file.reset();
  BOOST_CHECK(hfftp_client.status() == irs_st_ready);

  std::ifstream ifile(destination_file_name, ios::binary);
  BOOST_CHECK(ifile.good());

  std::vector<irs_u32> destination_buf(source_buf.size(), 0);
  ifile.read(reinterpret_cast<char*>(&destination_buf.front()),
    source_buf.size()*sizeof(buf_elem_type));
  BOOST_CHECK(ifile.good());
  irs_u8 ch;
  ifile.read(reinterpret_cast<char*>(&ch), 1);
  BOOST_CHECK(ifile.eof());
  ifile.close();
  BOOST_CHECK(destination_buf == source_buf);
}*/

BOOST_AUTO_TEST_CASE(test_case2)
{
  const irs::string_t source_file_name = irst("source_file");
  const irs::string_t destination_file_name = irst("destination_file");
  typedef irs_u8 buf_elem_type;
  std::vector<buf_elem_type> source_buf(3000);
  generate(source_buf.begin(), source_buf.end(), std::rand);

  irs::hfftp::memory_file_write_only_t memory_file_write_only;

  memory_files_t memory_files(source_file_name);
  memory_files.set_data(irs::vector_data(source_buf), source_buf.size());

  const irs::string_t server_port = irst("5015");

  irs::hardflow::udp_flow_t server_hardflow(irst(""), server_port,
    irst(""), irst(""), irs::hardflow::udplc_mode_queue, 1000,
    0xFFFF, false, 24*60*60, true, 10);

  irs::hardflow::udp_flow_t client_hardflow(irst(""), irst(""),
    irst("127.0.0.1"), server_port,
    irs::hardflow::udplc_mode_limited, 1, 0xFFFF, false, 24*60*60, false, 10);

  irs::hfftp::client_t hfftp_client(&client_hardflow);
  irs::hfftp::server_t hfftp_server(&server_hardflow, &memory_files, 1);

  hfftp_client.get(irs::str_conv<irs::string_t>(source_file_name),
    &memory_file_write_only);

  BOOST_CHECK(hfftp_client.status() == irs_st_busy);

  while (hfftp_client.status() == irs_st_busy) {
    client_hardflow.tick();
    server_hardflow.tick();
    hfftp_client.tick();
    hfftp_server.tick();
  }

  BOOST_CHECK(hfftp_client.status() == irs_st_ready);

  std::vector<buf_elem_type> destination_buf(memory_file_write_only.size());
  memory_file_write_only.read(irs::vector_data(destination_buf),
    destination_buf.size());

  BOOST_CHECK(destination_buf == source_buf);
}

BOOST_AUTO_TEST_CASE(test_case3)
{
  const irs::string_t source_file_name = irst("source_file");
  const irs::string_t destination_file_name = irst("destination_file");
  typedef irs_u8 buf_elem_type;
  std::vector<buf_elem_type> source_buf(3000);
  generate(source_buf.begin(), source_buf.end(), std::rand);

  irs::hfftp::memory_file_read_only_t memory_file_read_only(
    irs::vector_data(source_buf), source_buf.size());

  memory_files_t memory_files(destination_file_name);
  //memory_files.set_data(irs::vector_data(source_buf), source_buf.size());

  const irs::string_t server_port = irst("5016");

  irs::hardflow::udp_flow_t server_hardflow(irst(""), server_port,
    irst(""), irst(""), irs::hardflow::udplc_mode_queue, 1000,
    0xFFFF, false, 24*60*60, true, 10);

  irs::hardflow::udp_flow_t client_hardflow(irst(""), irst(""),
    irst("127.0.0.1"), server_port,
    irs::hardflow::udplc_mode_limited, 1, 0xFFFF, false, 24*60*60, false, 10);

  irs::hfftp::client_t hfftp_client(&client_hardflow);
  irs::hfftp::server_t hfftp_server(&server_hardflow, &memory_files, 1);

  hfftp_client.put(irs::str_conv<irs::string_t>(destination_file_name),
    &memory_file_read_only);

  BOOST_CHECK(hfftp_client.status() == irs_st_busy);

  irs::timer_t timer(irs::make_cnt_s(2));
  timer.start();
  while ((hfftp_client.status() == irs_st_busy) && !timer.check()) {
    client_hardflow.tick();
    server_hardflow.tick();
    hfftp_client.tick();
    hfftp_server.tick();
  }

  BOOST_CHECK_EQUAL(hfftp_client.status(), irs_st_ready);

  std::vector<buf_elem_type> destination_buf(memory_files.data_size());
  memory_files.get_data(irs::vector_data(destination_buf),
    destination_buf.size());
  //memory_file_write_only.read(irs::vector_data(destination_buf),
    //destination_buf.size());

  BOOST_CHECK(destination_buf == source_buf);
}

BOOST_AUTO_TEST_CASE(test_case4)
{
  const irs::string_t source_file_name = irst("source_file");
  const irs::string_t destination_file_name = irst("destination_file");
  typedef irs_u8 buf_elem_type;
  std::vector<buf_elem_type> source_buf(3000);
  generate(source_buf.begin(), source_buf.end(), std::rand);

  irs::hfftp::memory_file_read_only_t memory_file_read_only(
    irs::vector_data(source_buf), source_buf.size());

  memory_files_t memory_files(destination_file_name);
  //memory_files.set_data(irs::vector_data(source_buf), source_buf.size());

  const irs_u16 server_port = 5016;

  irs::hardflow::tcp_server_t server_hardflow(server_port);

  irs::hardflow::tcp_client_t client_hardflow(
    irs::make_mxip(127, 0, 0, 1), server_port);

  irs::hfftp::client_t hfftp_client(&client_hardflow);
  irs::hfftp::server_t hfftp_server(&server_hardflow, &memory_files, 1);

  hfftp_client.put(irs::str_conv<irs::string_t>(destination_file_name),
    &memory_file_read_only);

  BOOST_CHECK(hfftp_client.status() == irs_st_busy);

  irs::timer_t timer(irs::make_cnt_s(2));
  timer.start();
  while ((hfftp_client.status() == irs_st_busy) && !timer.check()) {
    client_hardflow.tick();
    server_hardflow.tick();
    hfftp_client.tick();
    hfftp_server.tick();
  }

  BOOST_CHECK_EQUAL(hfftp_client.status(), irs_st_ready);

  std::vector<buf_elem_type> destination_buf(memory_files.data_size());
  memory_files.get_data(irs::vector_data(destination_buf),
    destination_buf.size());
  //memory_file_write_only.read(irs::vector_data(destination_buf),
    //destination_buf.size());

  BOOST_CHECK(destination_buf == source_buf);
}
BOOST_AUTO_TEST_SUITE_END() // test_hfftp
