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

BOOST_AUTO_TEST_SUITE(student_t_inverse_distribution_2x)

BOOST_AUTO_TEST_CASE(test_confidence_level_0_95_n_20)
{
  BOOST_CHECK_CLOSE(
    irs::student_t_inverse_distribution_2x(irs::confidence_level_0_95, 20),
    2.0860, 0.01);
}

BOOST_AUTO_TEST_CASE(test_confidence_level_0_99_n_25)
{
  BOOST_CHECK_CLOSE(
    irs::student_t_inverse_distribution_2x(irs::confidence_level_0_99, 25),
    2.7874, 0.01);
}

BOOST_AUTO_TEST_CASE(test_confidence_level_0_999_n_30)
{
  BOOST_CHECK_CLOSE(
    irs::student_t_inverse_distribution_2x(irs::confidence_level_0_999, 30),
    3.646, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(critical_values_for_t_a_criterion)

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_01_n_20)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_t_a_criterion(irs::level_of_significance_0_01, 20),
    3.21, 0.01);
}

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_01_n_30)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_t_a_criterion(irs::level_of_significance_0_01, 30),
    3.38454, 0.0001);
}

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_05_n_20)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_t_a_criterion(irs::level_of_significance_0_05, 20),
    2.73, 0.01);
}

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_05_n_30)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_t_a_criterion(irs::level_of_significance_0_05, 30),
    2.92070, 0.0001);
}

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_1_n_20)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_t_a_criterion(irs::level_of_significance_0_1, 20),
    2.50, 0.01);
}

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_1_n_30)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_t_a_criterion(irs::level_of_significance_0_1, 30),
    2.6897, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(critical_values_for_smirnov_criterion)

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_01_n_10)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_smirnov_criterion(
    irs::level_of_significance_0_01, 10),
    2.41, 0.0001);
}

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_05_n_20)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_smirnov_criterion(
    irs::level_of_significance_0_05, 20),
    2.56, 0.0001);
}

BOOST_AUTO_TEST_CASE(test_level_of_significance_0_1_n_25)
{
  BOOST_CHECK_CLOSE(
    irs::critical_values_for_smirnov_criterion(
    irs::level_of_significance_0_1, 25),
    2.49, 0.0001);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(eliminating_outliers_t_a_criterion)

BOOST_AUTO_TEST_CASE(test_case_no_outliers)
{
  samples_no_outliers_t samples;
  std::vector<double> values = samples.get();
  const std::vector<double> result(values);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_t_a_criterion(values.begin(),
    values.end(), irs::level_of_significance_0_01,
    samples.sample_standard_deviation);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_no_outliers_calc_type_long_double)
{
  samples_no_outliers_t samples;
  std::vector<double> values = samples.get();
  const std::vector<double> result(values);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_t_a_criterion<
    std::vector<double>::iterator, long double>(values.begin(),
    values.end(), irs::level_of_significance_0_01,
    samples.sample_standard_deviation);
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
    values.end(), irs::level_of_significance_0_01, sd);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

namespace {

template <class T>
class offset_t
{
public:
  offset_t(T a_offset):
    m_offset(a_offset)
  {
  }
  T operator() (T a_value)
  {
    return a_value + m_offset;
  }
private:
  offset_t();
  const T m_offset;
};

} // unnamed namespace

BOOST_AUTO_TEST_CASE(test_case_one_outlier_shifted_upwards)
{
  samples_one_outlier_t samples;
  const double sd = samples.sample_standard_deviation_without_outliers;
  std::vector<double> values = samples.get();
  std::transform(values.begin(), values.end(), values.begin(),
    offset_t<double>(+10.));

  std::vector<double> result(values);
  result.erase(result.begin() + samples.bad_value_pos);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_t_a_criterion(values.begin(),
    values.end(), irs::level_of_significance_0_01, sd);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_one_outlier_shifted_downwards)
{
  samples_one_outlier_t samples;
  const double sd = samples.sample_standard_deviation_without_outliers;
  std::vector<double> values = samples.get();
  std::transform(values.begin(), values.end(), values.begin(),
    offset_t<double>(-10.));

  std::vector<double> result(values);
  result.erase(result.begin() + samples.bad_value_pos);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_t_a_criterion(values.begin(),
    values.end(), irs::level_of_significance_0_01, sd);
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
    values.end(), irs::level_of_significance_0_01);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_no_outliers_calc_type_long_double)
{
  samples_no_outliers_t samples;
  std::vector<double> values = samples.get();
  const std::vector<double> result(values);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_smirnov_criterion<
    std::vector<double>::iterator, long double>(values.begin(),
    values.end(), irs::level_of_significance_0_01);
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
    values.end(), irs::level_of_significance_0_01);
  values.erase(it_end, values.end());

  BOOST_CHECK(values == result);
}

BOOST_AUTO_TEST_CASE(test_case_two_outlier)
{
  samples_two_outliers_t samples;
  std::vector<double> values = samples.get();

  std::vector<double> result(values);
  // Используемый метод детектирует только одну ошибку
  result.erase(result.begin() + samples.bad_value_2_pos);

  std::vector<double>::iterator it_end =
    irs::eliminating_outliers_smirnov_criterion(values.begin(),
    values.end(), irs::level_of_significance_0_01);
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
    values.end(), irs::level_of_significance_0_01);
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
    values.end(), irs::level_of_significance_0_01);
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
    values.end(), irs::level_of_significance_0_01);
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
  BOOST_CHECK_CLOSE(result, 1/sqrt(2.), 1e-13);
  BOOST_CHECK_SMALL(average, 1e-15);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(fast_average_as_t)

BOOST_AUTO_TEST_CASE(test_case1)
{
  const double f = 50.17;
  const double t = 1/f;
  const double f_adc = 8203.125;
  const double t_adc = 1/f_adc;
  const double pi = IRS_PI;

  vector<double> y;
  const double x_max = 50*t;
  const double window_size = (48*t)/t_adc;
  y.reserve(static_cast<std::size_t>(x_max/t_adc));
  double x = 0;
  while (x <= x_max) {
    y.push_back(1 + sin(pi/2 + 2*pi*f*x));
    x += t_adc;
  }

  irs::fast_average_as_t<double, double> fast_average(window_size);
  for (std::size_t i = 0; i < y.size(); i++) {
    fast_average.add(y[i]);
  }

  const double result = fast_average.get();

  BOOST_CHECK_CLOSE(result, 1., 0.0001);
}

BOOST_AUTO_TEST_CASE(test_case2)
{
  const double f = 50.17;
  const double t = 1/f;
  const double f_adc = 8203.125;
  const double t_adc = 1/f_adc;
  const double pi = IRS_PI;

  vector<double> y;
  const double x_max = 50*t;
  const double window_size = (48*t)/t_adc;
  y.reserve(static_cast<std::size_t>(x_max/t_adc));
  double x = 0;
  while (x <= x_max) {
    y.push_back(1 + sin(pi/2 + 2*pi*f*x));
    x += t_adc;
  }

  irs::fast_average_as_t<double, double> fast_average(window_size);
  for (std::size_t i = 0; i < y.size(); i++) {
    fast_average.add(y[i]);
  }

  const double result = fast_average.get();

  BOOST_CHECK_CLOSE(result, 1., 0.0001);

  {
    const double f = 50.5;
    const double t = 1/f;
    const double f_adc = 8203.125;
    const double t_adc = 1/f_adc;
    const double pi = IRS_PI;

    vector<double> y;
    const double x_max = 200*t;
    const double window_size = (48*t)/t_adc;
    y.reserve(static_cast<std::size_t>(x_max/t_adc));
    double x = 0;
    while (x <= x_max) {
      y.push_back(1 + sin(pi/2 + 2*pi*f*x));
      x += t_adc;
    }

    fast_average.resize(window_size);
    fast_average.clear();
    for (std::size_t i = 0; i < y.size(); i++) {
      fast_average.add(y[i]);
    }

    const double result = fast_average.get();

    BOOST_CHECK_CLOSE(result, 1., 0.0001);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(fast_multi_sko_with_single_average_as_t)

BOOST_AUTO_TEST_CASE(test_case1)
{
  //const double f = 50.1752;
  const double f = 50;
  const double t = 1/f;
  const double f_adc = 8203.125;
  const double t_adc = 1/f_adc;
  const double pi = IRS_PI;

  vector<double> y;
  const double x_max = 200*t;
  const double average_size = (35*t)/t_adc;
  const double window_size = (48*t)/t_adc;
  const double short_window_size = (5*t)/t_adc;
  y.reserve(static_cast<std::size_t>(x_max/t_adc));
  double x = 0;
  while (x <= x_max) {
    y.push_back(3 + sin(pi/2 + 2*pi*f*x));
    x += t_adc;
  }

  vector<double> sizes;
  sizes.push_back(window_size);
  sizes.push_back(short_window_size);

  irs::fast_multi_sko_with_single_average_as_t<double, double> sko(sizes,
    average_size);
  for (std::size_t i = 0; i < y.size(); i++) {
    sko.add(y[i]);
    if (i > y.size()/2) {
      const double result = sko.get(0);
      BOOST_CHECK_CLOSE(result, 0.7071067811865, 0.00007);
    }
  }

  /*irs::fast_multi_sko_with_single_average_as_t<float, float> sko_float(sizes,
    window_size);
  for (std::size_t i = 0; i < y.size(); i++) {
    sko_float.add(y[i]);
    if (sko_float.is_full(0)) {
      const double result = sko_float.get(0);
      BOOST_CHECK_CLOSE(result, 0.7071067811865, 0.00015);
    }
  }*/
}

BOOST_AUTO_TEST_CASE(test_case2)
{
  //const double f = 50.1752;
  const double f = 50.25;
  const double t = 1/f;
  const double f_adc = 8203.125;
  const double t_adc = 1/f_adc;
  const double pi = IRS_PI;

  vector<double> y;
  const double x_max = 200*t;
  const double average_size = (35*t)/t_adc;
  const double window_size = (48*t)/t_adc;
  const double short_window_size = (5*t)/t_adc;
  y.reserve(static_cast<std::size_t>(x_max/t_adc));
  double x = 0;
  while (x <= x_max) {
    y.push_back(3 + sin(pi/2 + 2*pi*f*x));
    x += t_adc;
  }

  vector<double> sizes;
  sizes.push_back(window_size);
  sizes.push_back(short_window_size);

  irs::fast_multi_sko_with_single_average_as_t<double, double> sko(sizes,
    average_size);
  for (std::size_t i = 0; i < y.size(); i++) {
    sko.add(y[i]);
    if (i > y.size()/2) {
      const double result = sko.get(0);
      BOOST_CHECK_CLOSE(result, 0.7071067811865, 0.00007);
    }
  }
}


BOOST_AUTO_TEST_CASE(test_case3)
{
  const double f = 50;
  const double t = 1/f;
  const double f_adc = 8203.125;
  const double t_adc = 1/f_adc;
  const double pi = IRS_PI;

  vector<double> y;
  const double x_max = 200*t;
  const double average_size = (35*t)/t_adc;
  const double window_size = (48*t)/t_adc;
  const double short_window_size = (5*t)/t_adc;
  y.reserve(static_cast<std::size_t>(x_max/t_adc));
  double x = 0;
  while (x <= x_max) {
    y.push_back(3 + sin(pi/2 + 2*pi*f*x));
    x += t_adc;
  }

  vector<double> sizes;
  sizes.push_back(window_size);
  sizes.push_back(short_window_size);

  irs::fast_multi_sko_with_single_average_as_t<double, double> sko(sizes,
    average_size);
  for (std::size_t i = 0; i < y.size(); i++) {
    sko.add(y[i]);
    if (i > y.size()/2) {
      const double result = sko.get(0);
      BOOST_CHECK_CLOSE(result, 0.7071067811865, 0.00007);
    }
  }

  {
    const double f = 50.25;
    const double t = 1/f;
    const double f_adc = 8203.125;
    const double t_adc = 1/f_adc;
    const double pi = IRS_PI;

    vector<double> y;
    const double x_max = 200*t;
    const double average_size = (35*t)/t_adc;
    const double window_size = (48*t)/t_adc;
    const double short_window_size = (5*t)/t_adc;
    y.reserve(static_cast<std::size_t>(x_max/t_adc));
    double x = 0;
    while (x <= x_max) {
      y.push_back(3 + sin(pi/2 + 2*pi*f*x));
      x += t_adc;
    }

    sko.resize(0, window_size);
    sko.resize(1, short_window_size);
    sko.resize_average(average_size);

    for (std::size_t i = 0; i < y.size(); i++) {
      sko.add(y[i]);
      if (i > y.size()/2) {
        const double result = sko.get(0);
        if (i > (y.size(  ) - 10)) {
          BOOST_CHECK_CLOSE(result, 0.7071067811865, 0.00007);
        }
      }
    }
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
  BOOST_CHECK_CLOSE(result, 1/sqrt(2.), 1e-13);
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

BOOST_AUTO_TEST_SUITE(test_median_filter_t)

template<class data_t, class calc_t>
class median_filter_base_implementation_t
{
public:
  typedef size_t size_type;
  median_filter_base_implementation_t(size_type a_max_count);
  ~median_filter_base_implementation_t();
  void clear();
  void add(data_t a_val);
  calc_t get() const;
  void resize(size_type a_size);
  size_type size() const;
  size_type max_size() const;
  bool is_full();
private:
  median_filter_base_implementation_t();
  void update_result();
  size_type m_max_count;
  deque<data_t> m_deque;
  multiset<data_t> m_sort_values;
  calc_t m_result;
};

template<class data_t, class calc_t>
median_filter_base_implementation_t<data_t, calc_t>::
median_filter_base_implementation_t(size_type a_max_count
):
  m_max_count(a_max_count),
  m_deque(),
  m_sort_values(),
  m_result(0)
{
}

template<class data_t, class calc_t>
median_filter_base_implementation_t<data_t, calc_t>::
~median_filter_base_implementation_t()
{
}

template<class data_t, class calc_t>
void median_filter_base_implementation_t<data_t, calc_t>::clear()
{
  m_deque.clear();
  m_sort_values.clear();
  m_result = 0;
}

template<class data_t, class calc_t>
void median_filter_base_implementation_t<data_t, calc_t>::
add(data_t a_val)
{
  if (m_max_count == 0) {
    return;
  }
  if (m_deque.size() >= m_max_count) {
    typename multiset<data_t>::iterator it =
      m_sort_values.find(m_deque.front());
    m_sort_values.erase(it);
    m_deque.pop_front();
  }
  m_deque.push_back(a_val);
  m_sort_values.insert(a_val);
  update_result();
}

template<class data_t, class calc_t>
void median_filter_base_implementation_t<data_t, calc_t>::
update_result()
{
  const std::size_t size_2 = m_sort_values.size()/2;
  if (m_sort_values.size()%2 == 0) {
    const std::size_t pos = size_2 == 0 ? 0: size_2 - 1;
    typename multiset<data_t>::iterator it_1 = m_sort_values.begin();
    advance(it_1, pos);
    typename multiset<data_t>::iterator it_2 = it_1;
    ++it_2;
    m_result = (static_cast<calc_t>(*it_1) +
      static_cast<calc_t>(*it_2))/2;
  } else {
    const std::size_t pos = size_2;
    typename multiset<data_t>::iterator it = m_sort_values.begin();
    advance(it, pos);
    m_result = *it;
  }
}

template<class data_t, class calc_t>
calc_t median_filter_base_implementation_t<data_t, calc_t>::
get() const
{
  return m_result;
}

template<class data_t, class calc_t>
void median_filter_base_implementation_t<data_t, calc_t>::
resize(size_type a_size)
{
  m_max_count = a_size;
  while (m_deque.size() >= m_max_count) {
    typename multiset<data_t>::iterator it =
      m_sort_values.find(m_deque.front());
    m_sort_values.erase(it);
    m_deque.pop_front();
  }
  update_result();
}

template<class data_t, class calc_t>
typename median_filter_base_implementation_t<data_t, calc_t>::size_type
median_filter_base_implementation_t<data_t, calc_t>::size() const
{
  return m_deque.size();
}

template<class data_t, class calc_t>
typename median_filter_base_implementation_t<data_t, calc_t>::size_type
median_filter_base_implementation_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template<class data_t, class calc_t>
bool median_filter_base_implementation_t<data_t, calc_t>::is_full()
{
  return (m_deque.size() == m_max_count);
}


std::vector<double> make_values()
{
  std::vector<double> values;
  values.push_back(1);
  values.push_back(2);
  values.push_back(3);
  values.push_back(3);
  values.push_back(4);
  values.push_back(5);
  values.push_back(6);
  values.push_back(7);
  values.push_back(7);
  values.push_back(7);
  values.push_back(8);
  values.push_back(8);
  values.push_back(8);
  values.push_back(8);
  values.push_back(9);
  values.push_back(9);
  values.push_back(9);
  values.push_back(9);
  values.push_back(9);
  values.push_back(7);
  values.push_back(6);
  values.push_back(5);
  values.push_back(4);
  values.push_back(3);
  values.push_back(2);
  values.push_back(1);
  return values;
}

void test_median_filter(const std::size_t a_window_max_size,
  const std::vector<double>& a_values)
{
  irs::median_filter_t<double, double> filter(a_window_max_size);
  median_filter_base_implementation_t<double, double>
    filter_base(a_window_max_size);

  for (std::size_t i = 0; i < a_values.size(); i++) {
    filter.add(a_values[i]);
    filter_base.add(a_values[i]);

    const double filter_value = filter.get();
    const double filter_base_value = filter_base.get();
    BOOST_CHECK_EQUAL(filter_value, filter_base_value);
  }
}

BOOST_AUTO_TEST_CASE(test_add_with_window_add_size)
{
  std::vector<double> values = make_values();
  const std::size_t window_max_size = 5;
  test_median_filter(window_max_size, values);
}

BOOST_AUTO_TEST_CASE(test_add_with_window_even_size)
{
  std::vector<double> values = make_values();
  const std::size_t window_max_size = 4;
  test_median_filter(window_max_size, values);
}

BOOST_AUTO_TEST_CASE(test_add_with_window_size_0)
{
  std::vector<double> values = make_values();
  const std::size_t window_max_size = 0;
  test_median_filter(window_max_size, values);
}

BOOST_AUTO_TEST_CASE(test_add_with_window_size_1)
{
  std::vector<double> values = make_values();
  const std::size_t window_max_size = 1;
  test_median_filter(window_max_size, values);
}

BOOST_AUTO_TEST_CASE(test_add_with_window_size_2)
{
  std::vector<double> values = make_values();
  const std::size_t window_max_size = 2;
  test_median_filter(window_max_size, values);
}

BOOST_AUTO_TEST_CASE(test_add_with_bipolar_values)
{
  std::vector<double> values;
  values.push_back(-1);
  values.push_back(-2);
  values.push_back(-3);
  values.push_back(-3);
  values.push_back(4);
  values.push_back(5);
  values.push_back(-6);
  values.push_back(-7);
  values.push_back(7);
  values.push_back(7);
  values.push_back(-8);
  values.push_back(-8);
  values.push_back(-8);
  values.push_back(-8);
  values.push_back(9);
  values.push_back(9);
  values.push_back(-9);
  values.push_back(9);
  values.push_back(9);
  values.push_back(7);
  values.push_back(-6);
  values.push_back(5);
  values.push_back(4);
  values.push_back(3);
  values.push_back(-2);
  values.push_back(1);

  const std::size_t window_max_size = 4;
  test_median_filter(window_max_size, values);
}

BOOST_AUTO_TEST_CASE(test_resize)
{
  std::vector<double> values = make_values();

  std::size_t window_max_size = 5;
  const std::size_t window_new_max_size = 3;
  irs::median_filter_t<double, double> filter(window_max_size);
  median_filter_base_implementation_t<double, double>
    filter_base(window_max_size);

  for (std::size_t i = 0; i < values.size(); i++) {
    if (i == values.size()/2) {
      window_max_size = window_new_max_size;
      filter.resize(window_max_size);
      filter_base.resize(window_max_size);
    }
    filter.add(values[i]);
    filter_base.add(values[i]);

    const double filter_value = filter.get();
    const double filter_base_value = filter_base.get();
    BOOST_CHECK_EQUAL(filter_value, filter_base_value);
  }
}

BOOST_AUTO_TEST_CASE(test_size_and_full)
{
  std::vector<double> values = make_values();

  std::size_t window_max_size = 3;
  irs::median_filter_t<double, double> filter(window_max_size);
  filter.add(values[0]);
  BOOST_CHECK_EQUAL(filter.size(), 1);
  BOOST_CHECK_EQUAL(filter.is_full(), false);
  filter.add(values[1]);
  BOOST_CHECK_EQUAL(filter.size(), 2);
  BOOST_CHECK_EQUAL(filter.is_full(), false);
  filter.add(values[2]);
  BOOST_CHECK_EQUAL(filter.size(), 3);
  BOOST_CHECK_EQUAL(filter.is_full(), true);
  filter.add(values[3]);
  BOOST_CHECK_EQUAL(filter.size(), 3);
  BOOST_CHECK_EQUAL(filter.is_full(), true);
  filter.clear();
  BOOST_CHECK_EQUAL(filter.size(), 0);
  BOOST_CHECK_EQUAL(filter.is_full(), false);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_crc16)

BOOST_AUTO_TEST_CASE(test_case1)
{
  BOOST_CHECK_EQUAL(irs::crc16(
    reinterpret_cast<const irs_u8*>("123456789"), 9), 0x4B37);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_crc16_table)

BOOST_AUTO_TEST_CASE(test_case1)
{
  BOOST_CHECK_EQUAL(irs::crc16_table(
    reinterpret_cast<const irs_u8*>("123456789"), 9), 0x4B37);
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
