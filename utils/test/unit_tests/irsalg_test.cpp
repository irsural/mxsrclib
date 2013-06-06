#include <irsalg.h>

#include <boost/test/unit_test.hpp>
//#include <boost/test/floating_point_comparison.hpp>

#define BOOST_TEST_MODULE test_irsalg

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
