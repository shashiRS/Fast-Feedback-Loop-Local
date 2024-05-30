#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <thread>

#include <next/bridgesdk/threadsafe_list.h>

namespace next {
namespace bridgesdk {
namespace unit_test {

TEST(THREADSAFE_LIST_TEST, FIFOTest) {
  threadsafe_list<int> list;

  list.push_back(1);
  list.push_back(2);

  ASSERT_EQ(list.size(), 2);
  ASSERT_EQ(list.empty(), false);

  int value;
  ASSERT_TRUE(list.try_and_pop_back(value));
  ASSERT_EQ(2, value);

  ASSERT_TRUE(list.try_and_pop_back(value));
  ASSERT_EQ(1, value);

  ASSERT_EQ(list.size(), 0);
  ASSERT_EQ(list.empty(), true);
}

TEST(THREADSAFE_LIST_TEST, WaitAndPopValue) {
  threadsafe_list<int> list;
  const int producer_val{42};
  std::thread producer{[&]() {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0.5s);
    list.push_back(producer_val);
  }};

  int consumer_val{0};
  std::thread consumer{[&]() { list.wait_and_pop_back(consumer_val); }};

  producer.join();
  consumer.join();

  ASSERT_EQ(producer_val, consumer_val);
}

TEST(THREADSAFE_LIST_TEST, WaitAndPopMultipleValue) {
  threadsafe_list<int> list;
  using namespace std::chrono_literals;
  std::thread producer{[&]() {
    for (int i = 0; i <= 10; i++) {
      std::this_thread::sleep_for(0.5s);
      list.push_back(i);
    }
  }};
  int consumer_val{0};
  int i = 0;
  std::thread consumer{[&]() {
    while (consumer_val != 10) {
      list.wait_and_pop_back(consumer_val);
      ASSERT_EQ(i++, consumer_val);
    }
  }};

  producer.join();
  consumer.join();
}

} // namespace unit_test
} // namespace bridgesdk
} // namespace next
