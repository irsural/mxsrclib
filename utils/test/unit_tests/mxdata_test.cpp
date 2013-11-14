#include <mxdata.h>

#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE test_mxdata

BOOST_AUTO_TEST_SUITE(test_deque_data_t)

BOOST_AUTO_TEST_CASE(test_push_back)
{
  const std::size_t count = 1000;
  irs::deque_data_t<int> deque_data;
  for (int i = 0; i < count; i++) {
    deque_data.push_back(i);
  }
  BOOST_CHECK(deque_data.size() == count);
  for (int i = 0; i < count; i++) {
    BOOST_CHECK(deque_data[i] == i);
  }
}

namespace {

void pop_front_push_back(irs::deque_data_t<int>* ap_deque_data)
{
  const std::size_t count = 1000;
  const std::size_t max_size = 100;

  for (int i = 0; i < 1000; i++) {
    if (ap_deque_data->size() >= 100) {
      ap_deque_data->pop_front();
    }
    ap_deque_data->push_back(i);
  }
  BOOST_CHECK(ap_deque_data->size() == max_size);
  const int shift = count - max_size;
  for (int i = 0; i < max_size; i++) {
    BOOST_CHECK((*ap_deque_data)[i] == (i + shift));
  }
}

} // unnamed namespace

BOOST_AUTO_TEST_CASE(test_pop_front_push_back)
{
  irs::deque_data_t<int> deque_data;
  pop_front_push_back(&deque_data);
}

BOOST_AUTO_TEST_CASE(test_reserve)
{
  irs::deque_data_t<int> deque_data;
  deque_data.reserve(1000);
  pop_front_push_back(&deque_data);
}

BOOST_AUTO_TEST_SUITE_END()
