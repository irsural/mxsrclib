#include <irsalg.h>

#include <boost/test/unit_test.hpp>
//#include <boost/test/floating_point_comparison.hpp>

#define BOOST_TEST_MODULE test_irsalg

namespace {

std::vector<double> make_sinus(const std::size_t a_period_point_count,
  const std::size_t a_period_count)
{
  std::size_t point_count = a_period_point_count*a_period_count;
  std::vector<double> buffer(point_count);
  double delta = 2*IRS_PI/a_period_point_count;
  for (std::size_t i = 0; i < point_count; i++) {
    buffer[i] = sin(delta*i);
  }
  return buffer;
}

} // unnamed namespace

BOOST_AUTO_TEST_SUITE(remove_errors_metod_smirov)

BOOST_AUTO_TEST_CASE(test_case1)
{
  std::vector<double> values;
  values.push_back(0.8404);
  values.push_back(-0.8880);
  values.push_back(0.1001);
  values.push_back(-0.5445);
  values.push_back(0.3035);
  values.push_back(-0.6003);
  values.push_back(0.4900);
  values.push_back(0.7394);
  values.push_back(1.7119);
  values.push_back(-0.1941);
  values.push_back(-2.7);    // Грубая ошибка
  values.push_back(-0.8396);
  values.push_back(1.3546);
  values.push_back(-1.0722);
  values.push_back(0.9610);
  values.push_back(0.1240);
  values.push_back(1.4367);
  values.push_back(-1.9609);
  values.push_back(-0.1977);
  values.push_back(-1.2078);

  const double sko = 1.12037111970766;
  const double average = -0.107175;
  const double student_coef = 2.085963447; // 95 %

  std::vector<double>::iterator it_end =
    irs::remove_errors_metod_smirov(values.begin(), values.end(), 0.95);
  values.erase(it_end, values.end());
  BOOST_CHECK(values.size() == 19);
  std::vector<double>::iterator it = values.begin();
  while (it != values.end()) {
    const double value = *it;
    BOOST_CHECK(fabs(value - average)/sko <= student_coef);
    ++it;
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sko_calc_t)

BOOST_AUTO_TEST_CASE(test_case1)
{
  const std::size_t period_size = 1000;
  const std::size_t period_count = 5;
  const std::vector<double> sinus = make_sinus(period_size, period_count);

  irs::sko_calc_t<double, double> sko_calc(period_size*period_count);

  for (std::size_t i = 0; i < sinus.size(); i++) {
    sko_calc.add(sinus[i]);
  }
  double result = sko_calc;
  BOOST_CHECK_EQUAL(result, 1/sqrt(2));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_fast_sko_t)

BOOST_AUTO_TEST_CASE(test_case1)
{
  const std::size_t period_size = 1000;
  const std::size_t period_count = 50;
  const std::vector<double> sinus = make_sinus(period_size, period_count);

  irs::fast_sko_t<double, double> fast_sko(period_size,
    period_size*period_count);
  fast_sko.resize_average(period_size);
  fast_sko.resize(period_size*period_count);

  for (std::size_t i = 0; i < sinus.size(); i++) {
    fast_sko.add(sinus[i]);
  }
  double result = fast_sko;
  BOOST_CHECK_CLOSE(result, 1/sqrt(2), 0.02);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_crc16)

BOOST_AUTO_TEST_CASE(test_case1)
{
  BOOST_CHECK_EQUAL(irs::crc16(
    reinterpret_cast<irs_u8*>("123456789"), 9), 0x4B37);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_crc16_table)

BOOST_AUTO_TEST_CASE(test_case1)
{
  BOOST_CHECK_EQUAL(irs::crc16_table(
    reinterpret_cast<irs_u8*>("123456789"), 9), 0x4B37);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_crc32_table)

BOOST_AUTO_TEST_CASE(test_case1)
{
  BOOST_CHECK_EQUAL(irs::crc32_table("1", 1), 0x83dcefb7);
}

BOOST_AUTO_TEST_CASE(test_case2)
{
  BOOST_CHECK_EQUAL(irs::crc32_table("123", 3), 0x884863d2);
}

BOOST_AUTO_TEST_CASE(test_case3)
{
  BOOST_CHECK_EQUAL(irs::crc32_table("1234", 4), 0x9be3e0a3);
}

BOOST_AUTO_TEST_CASE(test_case4)
{
  BOOST_CHECK_EQUAL(irs::crc32_table("123456789", 9), 0xcbf43926);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_crc32_table_stream)

BOOST_AUTO_TEST_CASE(test_case1)
{
  irs::crc32_table_stream_t crc32_table_stream;
  crc32_table_stream.put("1", 1);
  BOOST_CHECK_EQUAL(crc32_table_stream.get_crc(), 0x83dcefb7);
}

BOOST_AUTO_TEST_CASE(test_case2)
{
  char data[] = "123";
  irs::crc32_table_stream_t crc32_table_stream;
  crc32_table_stream.put(data, 1);
  crc32_table_stream.put(&data[1], 2);
  BOOST_CHECK_EQUAL(crc32_table_stream.get_crc(), 0x884863d2);
}

BOOST_AUTO_TEST_CASE(test_case3)
{
  char data[] = "123456789";
  irs::crc32_table_stream_t crc32_table_stream;
  crc32_table_stream.put(data, 3);
  crc32_table_stream.reset();
  crc32_table_stream.put(data, 9);
  BOOST_CHECK_EQUAL(crc32_table_stream.get_crc(), 0xcbf43926);
}

BOOST_AUTO_TEST_SUITE_END()
