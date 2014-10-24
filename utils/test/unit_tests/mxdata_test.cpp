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
  if (deque_data.size() == count) {
    for (int i = 0; i < count; i++) {
      BOOST_CHECK(deque_data[i] == i);
    }
  }
}

BOOST_AUTO_TEST_CASE(test_push_back_sequence_case_1)
{
  const std::size_t data_size = 1000;
  std::vector<int> test_data(data_size);
  for (std::size_t i = 0; i < data_size; i++) {
    test_data[i] = i;
  }
  irs::deque_data_t<int> deque_data;
  deque_data.push_back(&test_data.front(), &test_data.back() + 1);
  BOOST_CHECK(deque_data.size() == test_data.size());
  if (deque_data.size() == test_data.size()) {
    for (std::size_t i = 0; i < test_data.size(); i++) {
      BOOST_CHECK(deque_data[i] == test_data[i]);
    }
  }
}

BOOST_AUTO_TEST_CASE(test_push_back_sequence_case_2)
{
  const std::size_t test_data_1_size = 1000;
  std::vector<int> test_data_1(test_data_1_size);
  for (std::size_t i = 0; i < test_data_1_size; i++) {
    test_data_1[i] = i;
  }
  const std::size_t test_data_2_size = 2000;
  std::vector<int> test_data_2(test_data_2_size);
  for (std::size_t i = 0; i < test_data_2_size; i++) {
    test_data_2[i] = i*10000 + i;
  }
  irs::deque_data_t<int> deque_data;
  deque_data.push_back(&test_data_1.front(), &test_data_1.back() + 1);
  deque_data.push_back(&test_data_2.front(), &test_data_2.back() + 1);
  BOOST_CHECK(deque_data.size() == (test_data_1.size() + test_data_2.size()));
  if (deque_data.size() == (test_data_1.size() + test_data_2.size())) {
    for (std::size_t i = 0; i < test_data_1.size(); i++) {
      BOOST_CHECK(deque_data[i] == test_data_1[i]);
    }
    for (std::size_t i = 0; i < test_data_2.size(); i++) {
      BOOST_CHECK(deque_data[i + test_data_1.size()] == test_data_2[i]);
    }
  }
}

BOOST_AUTO_TEST_CASE(test_push_back_pop_front_sequence)
{
  const std::size_t data_size = 1000;

  std::vector<int> test_data(data_size);
  for (std::size_t i = 0; i < data_size; i++) {
    test_data[i] = i;
  }
  irs::deque_data_t<int> deque_data;
  deque_data.push_back(&test_data.front(), &test_data.back() + 1);

  const std::size_t pop_size = data_size/2;
  deque_data.pop_front(pop_size);
  BOOST_CHECK(deque_data.size() == (test_data.size() - pop_size));
  if (deque_data.size() == (test_data.size() - pop_size)) {
    for (std::size_t i = 0; i < deque_data.size(); i++) {
      BOOST_CHECK(deque_data[i] == test_data[pop_size + i]);
    }
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
