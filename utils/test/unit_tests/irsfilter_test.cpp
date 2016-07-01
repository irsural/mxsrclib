#include <irsfilter.h>

#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE test_irsfilter

BOOST_AUTO_TEST_SUITE(test_filter)

BOOST_AUTO_TEST_CASE(test_chebyshev_3_order_ripple_pass)
{
  // Тест фильтра нижних частот чебышева 3 порядка
  irs::filter_settings_t filter_settings;
  typedef size_t size_type;
  typedef double value_type;
  const double epsilon = 1e-11;
  vector<value_type> num_coef_list;
  vector<value_type> denom_coef_list;

  filter_settings.family = irs::ff_chebyshev_ripple_pass;
  filter_settings.bandform = irs::fb_low_pass;
  size_type order = 3;
  filter_settings.order = order;
  filter_settings.sampling_time_s = 10e-6;
  filter_settings.low_cutoff_freq_hz = 10000;
  filter_settings.passband_ripple_db = 0.1;

  BOOST_REQUIRE(irs::get_coef_iir_filter(
    filter_settings, &num_coef_list, &denom_coef_list));
  BOOST_REQUIRE(num_coef_list.size() == denom_coef_list.size());
  BOOST_REQUIRE(num_coef_list.size() == (order + 1));

  BOOST_CHECK_CLOSE(num_coef_list[0], 2.861337762404367e-02, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[1], 8.584013287213102e-02, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[2], 8.584013287213102e-02, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[3], 2.861337762404367e-02, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[0], 1.000000000000000e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[1], -1.621278205051458e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[2], 1.151371927426478e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[3], -3.011867013826700e-01, epsilon);
}

BOOST_AUTO_TEST_CASE(test_butterworth_2_order_low_pass)
{
  // Тест фильтра нижних частот баттерворда 2 порядка
  irs::filter_settings_t filter_settings;
  typedef size_t size_type;
  typedef double value_type;
  const double epsilon = 1e-11;
  vector<value_type> num_coef_list;
  vector<value_type> denom_coef_list;

  const size_type order = 2;
  filter_settings.family = irs::ff_butterworth;
  filter_settings.bandform = irs::fb_low_pass;
  filter_settings.order = order;
  filter_settings.sampling_time_s = 10e-6;
  filter_settings.low_cutoff_freq_hz = 10000;
  filter_settings.passband_ripple_db = 0.1;

  BOOST_REQUIRE(irs::get_coef_iir_filter(
    filter_settings, &num_coef_list, &denom_coef_list));
  BOOST_REQUIRE(num_coef_list.size() == denom_coef_list.size());
  BOOST_REQUIRE(num_coef_list.size() == (order + 1));

  BOOST_CHECK_CLOSE(num_coef_list[0], 6.745527388907192e-02, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[1], 1.349105477781438e-01, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[2], 6.745527388907192e-02, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[0], 1.000000000000000e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[1], -1.142980502539901e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[2], 4.128015980961886e-01, epsilon);
}

/*BOOST_AUTO_TEST_CASE(test_cauer_2_order_high_pass)
{
  // Тест фильтра верхних частот кауэра 2 порядка
  irs::filter_settings_t filter_settings;
  typedef size_t size_type;
  typedef double value_type;
  const double epsilon = 1e-11;
  vector<value_type> num_coef_list;
  vector<value_type> denom_coef_list;

  const size_type order = 2;
  filter_settings.family = irs::ff_cauer;
  filter_settings.bandform = irs::fb_high_pass;
  filter_settings.order = order;
  filter_settings.sampling_time_s = 10e-6;
  filter_settings.low_cutoff_freq_hz = 10000;
  filter_settings.passband_ripple_db = 0.1;
  filter_settings.stopband_ripple_db = 80;

  BOOST_REQUIRE(irs::get_coef_iir_filter(
    filter_settings, &num_coef_list, &denom_coef_list));
  BOOST_REQUIRE(num_coef_list.size() == denom_coef_list.size());
  BOOST_REQUIRE(num_coef_list.size() == (order + 1));

  BOOST_CHECK_CLOSE(num_coef_list[0], 7.818169187532143e-01, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[1], -1.563623760031081e+00, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[2], 7.818169187532143e-01, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[0], 1.000000000000000e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[1], -1.531347390185103e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[2], 6.321221436620125e-01, epsilon);
}*/

/*BOOST_AUTO_TEST_CASE(test_cauer_2_order_band_pass)
{
  // Тест полосового фильтра кауэра 2 порядка
  irs::filter_settings_t filter_settings;
  typedef size_t size_type;
  typedef double value_type;
  const double epsilon = 1e-11;
  vector<value_type> num_coef_list;
  vector<value_type> denom_coef_list;

  const size_type order = 2;
  filter_settings.family = irs::ff_cauer;
  filter_settings.bandform = irs::fb_band_pass;
  filter_settings.order = order;
  filter_settings.sampling_time_s = 10e-6;
  filter_settings.low_cutoff_freq_hz = 10000;
  filter_settings.high_cutoff_freq_hz = 20000;
  filter_settings.passband_ripple_db = 0.1;
  filter_settings.stopband_ripple_db = 80;

  BOOST_REQUIRE(irs::get_coef_iir_filter(
    filter_settings, &num_coef_list, &denom_coef_list));
  BOOST_REQUIRE(num_coef_list.size() == denom_coef_list.size());
  BOOST_REQUIRE(num_coef_list.size() == (order + 1));

  BOOST_CHECK_CLOSE(num_coef_list[0], 1.631435038562584e-01, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[1], -1.165736846345882e-04, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[2], -3.260263410292863e-01, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[3], -1.165736846345881e-04, epsilon);
  BOOST_CHECK_CLOSE(num_coef_list[4], 1.631435038562584e-01, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[0], 1.000000000000000e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[1], -1.615003829826376e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[2], 1.333596081600205e+00, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[3], -7.164698628654187e-01, epsilon);
  BOOST_CHECK_CLOSE(denom_coef_list[4], 2.730707507054483e-01, epsilon);
}*/

BOOST_AUTO_TEST_SUITE_END()
