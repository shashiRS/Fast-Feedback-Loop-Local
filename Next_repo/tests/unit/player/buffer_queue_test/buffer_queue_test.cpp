
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <future>

#include "mts/extensibility/package.hpp"
#include "plugin/core_lib_reader/core_lib_package.h"
#include "resource_manager/buffer_queue.hpp"

next::player::BufferQueue test_queue;

constexpr long long watchdog_s = 10;
constexpr size_t package_size = sizeof(next::sdk::types::IPackage);
constexpr uint64_t max_read_ahead_bytes = MAX_READ_AHEAD_BUFFER_SIZE_MB * 1024 * 1024;

void PushInBuffer(std::size_t size, uint64_t reference_timestamp, uint16_t cycleCounter) {
  mts_memory_allocator_t *allocator = mts_get_default_allocator();
  const auto format = mts::extensibility::get_format_identifier(mts::extensibility::reference_camera_is);
  mts::extensibility::package test_package(allocator, format, size);
  mts_package_t *mts_pkt = test_package.get();
  ASSERT_TRUE(mts_pkt) << "Failed to create mts_package_t";

  mts_pkt->cycle_counter = cycleCounter;
  mts_pkt->reference_timestamp = reference_timestamp;

  std::unique_ptr<next::sdk::types::IPackage> package = std::make_unique<CoreLibPackage>(std::move(test_package));

  test_queue.pushBuffer(std::move(package));
}

struct PackageSimulator {
  std::size_t package_size = 100;
  std::size_t total_packages_cnt = 0;
  uint16_t cycle_cnt = 0;
  uint64_t timestamp = 0;
};

TEST(buffer_queue_test, empty_pop) {
  test_queue.clearBuffer();
  test_queue.startPush();
  // create push task - will sleep for 5 seconds to allow pop to block
  auto push_buffer_future_ = std::async(std::launch::async, [this]() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    PushInBuffer(100, 1, 1);
  });

  auto pop_buffer_ = std::async(std::launch::async, []() {
    bool eof;
    std::unique_ptr<next::sdk::types::IPackage> samplePackage = test_queue.popBuffer(eof);
  });
  // pop will be in wait as there are no elements in the queue -> should result in timeout
  EXPECT_FALSE(pop_buffer_.wait_for(std::chrono::seconds(3)) != std::future_status::timeout);
  // should have already pushed ->pop is possible
  EXPECT_TRUE(pop_buffer_.wait_for(std::chrono::seconds(3)) != std::future_status::timeout);
}

TEST(buffer_queue_test, full_push) {
  test_queue.clearBuffer();
  test_queue.startPush();
  // fill the data buffer - duration condition - only 2 push should be possible
  auto push_buffer = std::async(std::launch::async, [this]() {
    PushInBuffer(100, 1, 1);
    PushInBuffer(100, MAXIMUM_READ_AHEAD_BUFFER_TIME + 2, 2);
    PushInBuffer(100, 2, 3);
  });

  // push should be blocked
  EXPECT_TRUE(push_buffer.wait_for(std::chrono::seconds(3)) == std::future_status::timeout);
  bool eof;
  auto samplePackage = test_queue.popBuffer(eof);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  // after the 3rd push should work and should have 2 elements
  EXPECT_TRUE(test_queue.size() == 2);
}

TEST(buffer_queue_test, max_read_ahead_data_size_test) {
  PackageSimulator test_package;

  test_queue.clearBuffer();
  test_queue.startPush();
  EXPECT_TRUE(test_queue.size() == 0);

  // fill the data buffer until maximum read ahead data size is reached
  auto fill_buffer = std::async(std::launch::async, [this, &test_package]() {
    while (test_package.total_packages_cnt + (test_queue.size() * package_size) < max_read_ahead_bytes) {
      test_package.cycle_cnt++;
      test_package.timestamp++;
      PushInBuffer(test_package.package_size, test_package.timestamp, test_package.cycle_cnt);
      test_package.total_packages_cnt += test_package.package_size;
    }
  });
  if (fill_buffer.wait_for(std::chrono::seconds(watchdog_s)) == std::future_status::timeout) {
    test_queue.stopPush(); // terminate fill_buffer async call
    ASSERT_TRUE(false) << "Failed to fill Buffer to maximum, push blocked to early or timeout";
  }

  const std::size_t nr_of_bytes_pushed = test_package.total_packages_cnt / test_package.package_size;
  EXPECT_EQ(test_queue.size(), nr_of_bytes_pushed) << "Buffer size differs, possible packages lost";

  const size_t queue_full = test_queue.size();
  auto try_overflow = std::async(std::launch::async, [this, &test_package]() {
    test_package.cycle_cnt++;
    test_package.timestamp++;
    PushInBuffer(test_package.package_size, test_package.timestamp, test_package.cycle_cnt);
  });
  EXPECT_EQ(try_overflow.wait_for(std::chrono::seconds(3)), std::future_status::timeout)
      << "Pushing to Buffer was not blocked or timeout";
  test_queue.stopPush(); // terminate try_overflow async call

  EXPECT_EQ(queue_full, test_queue.size()) << "Buffer overflow, buffer size increased";
}

TEST(buffer_queue_test, normal_pop) {
  test_queue.clearBuffer();
  test_queue.startPush();
  PushInBuffer(100, 1, 1);
  PushInBuffer(100, 10, 2);
  PushInBuffer(100, 20, 3);
  PushInBuffer(100, 30, 4);
  EXPECT_TRUE(test_queue.size() == 4);
  bool eof;
  auto samplePackage = test_queue.popBuffer(eof);
  EXPECT_TRUE(test_queue.size() == 3);
}

TEST(buffer_queue_test, check_end_of_file) {
  test_queue.clearBuffer();
  test_queue.startPush();
  PushInBuffer(100, 1, 1);
  PushInBuffer(100, 10, 2);
  PushInBuffer(100, 20, 3);
  PushInBuffer(100, 30, 4);
  EXPECT_TRUE(test_queue.size() == 4);
  // signal the buffer that the file reached the end
  test_queue.setEndOfFileFlag();
  PushInBuffer(100, 40, 5);
  // this push should not be allowed, buffer size should still be 4
  EXPECT_TRUE(test_queue.size() == 4);

  bool eof;
  auto samplePackage = test_queue.popBuffer(eof);
  EXPECT_TRUE(test_queue.size() == 3);
  EXPECT_TRUE(eof == false);
  test_queue.popBuffer(eof);
  EXPECT_TRUE(test_queue.size() == 2);
  EXPECT_TRUE(eof == false);
  test_queue.popBuffer(eof);
  EXPECT_TRUE(test_queue.size() == 1);
  EXPECT_TRUE(eof == false);
  test_queue.popBuffer(eof);
  EXPECT_TRUE(test_queue.size() == 0);
  EXPECT_TRUE(eof == false);
  // now end of file should be true
  test_queue.popBuffer(eof);
  EXPECT_TRUE(eof == true);
}

TEST(buffer_queue_test, shutdown_unblock_empty_pop) {
  test_queue.clearBuffer();
  test_queue.startPush();
  // create shutdown task - will sleep for 5 seconds to allow pop to block
  auto push_buffer_future_ = std::async(std::launch::async, [this]() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    test_queue.shutdown();
  });
  bool eof;
  auto pop_buffer_ = std::async(std::launch::async, [&eof]() {
    std::unique_ptr<next::sdk::types::IPackage> samplePackage = test_queue.popBuffer(eof);
  });
  // pop will be in wait as there are no elements in the queue -> should result in timeout
  EXPECT_FALSE(pop_buffer_.wait_for(std::chrono::seconds(3)) != std::future_status::timeout);
  // should have already pushed ->pop is possible
  EXPECT_TRUE(pop_buffer_.wait_for(std::chrono::seconds(3)) != std::future_status::timeout);
}
// Enable this test once the ticket SIMEN-7492 resolved
TEST(buffer_queue_test, shutdown_unblock_full_push) {
  test_queue.clearBuffer();
  test_queue.startPush();
  // fill the data buffer - duration condition - only 2 push should be possible
  auto push_buffer = std::async(std::launch::async, [this]() {
    PushInBuffer(100, 1, 1);
    PushInBuffer(100, MAXIMUM_READ_AHEAD_BUFFER_TIME + 2, 2);
    PushInBuffer(100, 2, 3);
  });

  // push should be blocked
  EXPECT_TRUE(push_buffer.wait_for(std::chrono::seconds(3)) == std::future_status::timeout);
  test_queue.shutdown();
  // shutdown should unblock and clear the queue
  EXPECT_TRUE(test_queue.size() == 0);
}
