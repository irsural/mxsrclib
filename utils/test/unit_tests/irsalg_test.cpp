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

struct samples_no_outliers_t
{
  const double standard_deviation;
  const double sample_standard_deviation;
  const double average;
  samples_no_outliers_t():
    standard_deviation(0.89059917505702),
    sample_standard_deviation(0.961957053293416),
    average(-0.115371428571429)
  {
  }
  std::vector<double> get()
  {
    double array[] = {
      -0.7549, 1.3703, -1.7115, -0.1022, -0.2414, 0.3192, 0.3129
    };
    return std::vector<double>(array, array + IRS_ARRAYSIZE(array));
  }
};

struct samples_one_outlier_t
{
  const double standard_deviation;
  const double sample_standard_deviation;
  const double sample_standard_deviation_without_outliers;
  const double average;
  const std::size_t bad_value_pos;
  samples_one_outlier_t():
    standard_deviation(1.40135521759296),
    sample_standard_deviation(1.43776011683195),
    sample_standard_deviation_without_outliers(0.982809705238266),
    average(-0.207175),
    bad_value_pos(10)
  {
  }
  std::vector<double> get()
  {
    double array[] = {
      0.8404,
      -0.8880,
      0.1001,
      -0.5445,
      0.3035,
      -0.6003,
      0.4900,
      0.7394,
      1.7119,
      -0.1941,
      -4.7,    // Грубая ошибка
      -0.8396,
      1.3546,
      -1.0722,
      0.9610,
      0.1240,
      1.4367,
      -1.9609,
      -0.1977,
      -1.2078
    };
    return std::vector<double>(array, array + IRS_ARRAYSIZE(array));
  }
};

struct samples_two_outliers_t
{
  const double standard_deviation;
  const double sample_standard_deviation;
  const double average;
  const std::size_t bad_value_1_pos;
  const std::size_t bad_value_2_pos;
  samples_two_outliers_t():
    standard_deviation(1.90446865841237),
    sample_standard_deviation(1.95394361575574),
    average(-0.056675),
    bad_value_1_pos(6),
    bad_value_2_pos(10)
  {
  }
  std::vector<double> get()
  {
    double array[] = {
      0.8404,
      -0.8880,
      0.1001,
      -0.5445,
      0.3035,
      -0.6003,
      4.600,   // Грубая ошибка
      0.7394,
      1.7119,
      -0.1941,
      -5.8,    // Грубая ошибка
      -0.8396,
      1.3546,
      -1.0722,
      0.9610,
      0.1240,
      1.4367,
      -1.9609,
      -0.1977,
      -1.2078
    };
    return std::vector<double>(array, array + IRS_ARRAYSIZE(array));
  }
};

} // unnamed namespace

BOOST_AUTO_TEST_SUITE(eliminating_outliers_t_a_criterion)

BOOST_AUTO_TEST_CASE(test_case_no_outliers)
{
  samples_no_outliers_t samples;
  std::vector<double> values = samples.get();
  const std::vector<double> result(values);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_t_a_criterion(values.begin(),
    values.end(), 0.01, samples.sample_standard_deviation);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_one_outlier)
{
  samples_one_outlier_t samples;
  const double sd = samples.sample_standard_deviation_without_outliers;
  std::vector<double> values = samples.get();

  std::vector<double> result(values);
  result.erase(result.begin() + samples.bad_value_pos);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_t_a_criterion(values.begin(),
    values.end(), 0.01, sd);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(eliminating_outliers_smirnov_criterion)

BOOST_AUTO_TEST_CASE(test_case_no_outliers)
{
  samples_no_outliers_t samples;
  std::vector<double> values = samples.get();
  const std::vector<double> result(values);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_smirnov_criterion(values.begin(),
    values.end(), 0.01);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_one_outlier)
{
  samples_one_outlier_t samples;
  std::vector<double> values = samples.get();

  std::vector<double> result(values);
  result.erase(result.begin() + samples.bad_value_pos);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_smirnov_criterion(values.begin(),
    values.end(), 0.01);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(eliminating_outliers_smirnov_criterion_multiple_pass)

BOOST_AUTO_TEST_CASE(test_case_no_ountiers)
{
  samples_no_outliers_t samples;
  std::vector<double> values = samples.get();

  std::vector<double> result(values);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_smirnov_criterion_multiple_pass(values.begin(),
    values.end(), 0.01);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_one_outlier)
{
  samples_one_outlier_t samples;
  std::vector<double> values = samples.get();

  std::vector<double> result(values);
  result.erase(result.begin() + samples.bad_value_pos);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_smirnov_criterion_multiple_pass(values.begin(),
    values.end(), 0.01);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_two_outliers)
{
  samples_two_outliers_t samples;
  std::vector<double> values = samples.get();

  std::vector<double> result(values);
  result.erase(result.begin() + samples.bad_value_2_pos);
  result.erase(result.begin() + samples.bad_value_1_pos);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_smirnov_criterion_multiple_pass(
    values.begin(),
    values.end(), 0.01);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_SUITE_END()
/*
BOOST_AUTO_TEST_SUITE(remove_errors_criterion_smirov_single_pass)

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
    irs::remove_errors_criterion_smirov_single_pass(values.begin(),
    values.end(), 0.95);
  values.erase(it_end, values.end());
  BOOST_CHECK_MESSAGE(values.size() == 19, "size = " << values.size());
  std::vector<double>::iterator it = values.begin();
  while (it != values.end()) {
    const double value = *it;
    BOOST_CHECK(fabs(value - average)/sko <= student_coef);
    ++it;
  }
}

BOOST_AUTO_TEST_SUITE_END()


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
    irs::remove_errors_criterion_smirov(values.begin(), values.end(), 0.95);
  values.erase(it_end, values.end());
  BOOST_CHECK_MESSAGE(values.size() == 19, "size = " << values.size());
  std::vector<double>::iterator it = values.begin();
  while (it != values.end()) {
    const double value = *it;
    BOOST_CHECK(fabs(value - average)/sko <= student_coef);
    ++it;
  }
}

BOOST_AUTO_TEST_CASE(test_case2)
{
  std::vector<double> values;
  values.push_back(0.8404);
  values.push_back(-0.8880);
  values.push_back(0.1001);
  values.push_back(-0.5445);
  values.push_back(0.3035);
  values.push_back(-0.6003);
  values.push_back(2.60);     // Грубая ошибка
  values.push_back(0.7394);
  values.push_back(1.7119);
  values.push_back(-0.1941);
  values.push_back(-2.77);    // Грубая ошибка
  values.push_back(-0.8396);
  values.push_back(1.3546);
  values.push_back(-1.0722);
  values.push_back(0.9610);
  values.push_back(0.1240);
  values.push_back(1.4367);
  values.push_back(-1.9609);
  values.push_back(-0.1977);
  values.push_back(-1.2078);

  const double sko = 1.2024295;
  const double average = -0.1035500;
  const double student_coef = 2.085963447; // 95 %

  std::vector<double>::iterator it_end =
    irs::remove_errors_criterion_smirov(values.begin(), values.end(), 0.95);
  values.erase(it_end, values.end());
  BOOST_CHECK(values.size() == 18);
  std::vector<double>::iterator it = values.begin();
  while (it != values.end()) {
    const double value = *it;
    const double t = fabs(value - average)/sko;
    BOOST_CHECK_MESSAGE(t <= student_coef, "value = " << value <<
      ", t = " << t << ", student_coef = " << student_coef);
    ++it;
  }
}

BOOST_AUTO_TEST_SUITE_END()
*/
BOOST_AUTO_TEST_SUITE(standard_deviation)

BOOST_AUTO_TEST_CASE(test_case1)
{
  const std::size_t period_size = 1000;
  const std::size_t period_count = 5;
  const std::vector<double> sinus = make_sinus(period_size, period_count);

  irs::sko_calc_t<double, double> sko_calc(period_size*period_count);

  for (std::size_t i = 0; i < sinus.size(); i++) {
    sko_calc.add(sinus[i]);
  }
  double average = 0;
  double result = 0;
  irs::standard_deviation(sinus.begin(), sinus.end(), &result, &average);
  BOOST_CHECK_EQUAL(result, 1/sqrt(2.));
  BOOST_CHECK_SMALL(average, 1e-15);
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
  BOOST_CHECK_EQUAL(result, 1/sqrt(2.));
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
  BOOST_CHECK_CLOSE(result, 1/sqrt(2.), 0.02);
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
