#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <thread>

#include <next/bridgesdk/threadsafe_queue.h>

namespace next {
namespace bridgesdk {
namespace unit_test {

TEST(THREADSAFE_QUEUE_TEST, FIFOTest) {
  threadsafe_queue<int> queue;

  queue.push(1);
  queue.push(2);

  ASSERT_EQ(queue.size(), 2);
  ASSERT_EQ(queue.empty(), false);

  int value;
  ASSERT_TRUE(queue.try_and_pop(value));
  ASSERT_EQ(1, value);

  ASSERT_TRUE(queue.try_and_pop(value));
  ASSERT_EQ(2, value);

  ASSERT_EQ(queue.size(), 0);
  ASSERT_EQ(queue.empty(), true);
}

TEST(THREADSAFE_QUEUE_TEST, WaitAndPopValue) {
  threadsafe_queue<int> queue;
  const int producer_val{42};
  std::thread producer{[&]() {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0.5s);
    queue.push(producer_val);
  }};

  int consumer_val{0};
  std::thread consumer{[&]() { queue.wait_and_pop(consumer_val); }};

  producer.join();
  consumer.join();

  ASSERT_EQ(producer_val, consumer_val);
}

TEST(THREADSAFE_QUEUE_TEST, WaitAndPopMultipleValue) {
  threadsafe_queue<int> queue;
  using namespace std::chrono_literals;
  std::thread producer{[&]() {
    for (int i = 0; i <= 10; i++) {
      std::this_thread::sleep_for(0.5s);
      queue.push(i);
    }
  }};
  int consumer_val{0};
  int i = 0;
  std::thread consumer{[&]() {
    while (consumer_val != 10) {
      queue.wait_and_pop(consumer_val);
      ASSERT_EQ(i++, consumer_val);
    }
  }};

  producer.join();
  consumer.join();
}

} // namespace unit_test
} // namespace bridgesdk
} // namespace next
