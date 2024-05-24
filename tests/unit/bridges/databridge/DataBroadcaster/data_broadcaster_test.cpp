#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/logger/logger.hpp>
#include "data_broadcaster_impl.hpp"

namespace next::databridge::testing {

void thread_test_function(databridge::data_broadcaster::DataBroadcasterImpl &data_broadcaster_test,
                          std::atomic<size_t> &threads_finished) {
  std::uint8_t num_requests{20};

  for (std::uint8_t i = 0; i < num_requests; ++i) {
    auto future = data_broadcaster_test.RequestNewMemory();
    future.wait();
  }
  threads_finished++;
}

TEST(DataBroadcasterTest, TestingDataValidityAfterExchangeWithMultipleThreads) {

  const size_t num_of_threads = 4u;
  std::vector<std::thread> multi_threads(num_of_threads);
  databridge::data_broadcaster::DataBroadcasterImpl data_broadcaster_test{};
  std::atomic<size_t> threads_finished{0};

  for (size_t i = 0u; i < num_of_threads; ++i) {
    multi_threads[i] = std::thread(thread_test_function, std::ref(data_broadcaster_test), std::ref(threads_finished));
  }

  while (threads_finished.load() != num_of_threads)
    ;

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(10ms);
  auto future = data_broadcaster_test.RequestNewMemory();
  future.wait();

  // Check if all memory slots are free after data have been published.
  int num_free_slot{0};
  for (auto const mem_slot : data_broadcaster_test.GetSignalPool()) {
    if (mem_slot.status_ == databridge::BroadcastingStatus::FILLING) {
      num_free_slot++;
    }
  }

  EXPECT_EQ(num_free_slot, 81);

  for (auto &mt : multi_threads) {
    if (mt.joinable()) {
      mt.join();
    }
  }
}

/*! \brief The test try to add 6 requests to queue and expecting 6 published message with certain ID to be received.
 */
TEST(DataBroadcasterTest, TestingDataValidityAfterExchange) {

  databridge::data_broadcaster::DataBroadcasterImpl data_broadcaster_test{};
  using namespace std::chrono_literals;

  size_t msg_counter{0};
  size_t payload{0};

  std::uint8_t num_requests{24};
  for (std::uint8_t i = 1; i < num_requests; ++i) {
    payload = i;
    auto future = data_broadcaster_test.RequestNewMemory();

    // Get reference to the provided memory slot via data broadcastere
    auto sig_value = future.get();

    // Fill memory with values
    sig_value->data_.resize(100);
    std::fill(std::begin(sig_value->data_), std::end(sig_value->data_), static_cast<std::uint8_t>(payload));

    // Check the ID of message after its published with original one.
    EXPECT_EQ(data_broadcaster_test.GetProcessedSignalTracker().begin()->first, msg_counter);

    EXPECT_EQ(data_broadcaster_test.GetSignalPool().begin()->status_, databridge::BroadcastingStatus::FILLING);

    data_broadcaster_test.DataPublishingFinished(msg_counter);
    msg_counter++;

    // The sleep give the Req.Res thread enough time to handle the requests.
    std::this_thread::sleep_for(3ms);

    EXPECT_EQ(std::next(data_broadcaster_test.GetSignalPool().begin(), i)->status_,
              databridge::BroadcastingStatus::FREE);

    std::this_thread::sleep_for(5ms);

    // Check the state of signal tracker map after publishing the data.
    EXPECT_TRUE(data_broadcaster_test.GetProcessedSignalTracker().empty());
  }

  // Check if all memory slots are free after data have been published.
  for (auto const mem_slot : data_broadcaster_test.GetSignalPool()) {
    EXPECT_EQ(mem_slot.status_, databridge::BroadcastingStatus::FREE);
  }
}

// /*! \brief The test try to send number of requests more than the size of message list, to check if new memory slot
// can be taken from the begining of the list.
// */
TEST(DataBroadcasterTest, TestingOverflowHandling) {
  using std::chrono::milliseconds;
  using namespace std::literals::chrono_literals;

  databridge::data_broadcaster::DataBroadcasterImpl data_broadcaster_test{};
  const auto signals_overflow = databridge::kNumSignals + 1;

  // Sending Requst for memory slot more than size of the list.
  // request max signals + 1(promise left)
  for (std::size_t i = 0; i < signals_overflow; ++i) {
    data_broadcaster_test.RequestNewMemory();
  }

  auto memory_future = data_broadcaster_test.RequestNewMemory();
  std::this_thread::sleep_for(1000ms);

  std::future_status memory_status = memory_future.wait_for(milliseconds(kMaxDurationValidMemInMs));
  ASSERT_FALSE(memory_status == std::future_status::timeout);

  auto signals = data_broadcaster_test.GetSignalPool();
  for (const auto &signal : signals) {
    EXPECT_EQ(signal.status_, BroadcastingStatus::FILLING);
  }

  // free broadcast for 1, 1 promise left-> fill one
  data_broadcaster_test.DataPublishingFinished(0);
  EXPECT_EQ(std::next(data_broadcaster_test.GetSignalPool().begin(), 0)->status_,
            databridge::BroadcastingStatus::FILLING);

  if (memory_future.valid()) {
    try {
      auto memory = memory_future.get();
      EXPECT_EQ(memory->status_, BroadcastingStatus::FILLING);
      EXPECT_EQ(memory->id_, 1001);
    } catch (const std::future_error &e) {
      sdk::logger::error(__FILE__, "Failed to get future memory {}", e.what());
    }
  }

  data_broadcaster_test.DataPublishingFinished(0);
  bool success = false;
  for (int i = 0; i < 100; i++) {
    std::this_thread::sleep_for(10ms);
    if (std::next(data_broadcaster_test.GetSignalPool().begin(), 0)->status_ == databridge::BroadcastingStatus::FREE) {
      success = true;
      break;
    }
  }
  EXPECT_TRUE(success) << "memory slot opening failed";
}

// /*! \brief When all memory slot is consumed, the new request for memory should wait till one got free by publisher.
// */
TEST(DataBroadcasterTest, TestingNoFreeMemorySlotAvailableForSomeTime) {
  databridge::data_broadcaster::DataBroadcasterImpl data_broadcaster_test{};

  using namespace std::chrono_literals;
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  for (std::size_t i = 0; i < (databridge::kNumSignals); ++i) {
    data_broadcaster_test.RequestNewMemory();
  }

  std::this_thread::sleep_for(100ms);

  // All other memory slot's state should be set to FILLING.
  auto signal_pool = data_broadcaster_test.GetSignalPool();
  auto ref_to_member = std::find_if(std::begin(signal_pool), std::end(signal_pool), [](const auto &ref_to_member) {
    return (ref_to_member.status_ == databridge::BroadcastingStatus::FREE);
  });

  // No free memory slot available
  EXPECT_TRUE(std::end(signal_pool) == ref_to_member);

  data_broadcaster_test.DataPublishingFinished(0);
  std::this_thread::sleep_for(100ms);

  auto future = data_broadcaster_test.RequestNewMemory();

  // Waiting for the new memory slot
  auto sig_value = future.get();

  // State of new memory slot.
  EXPECT_EQ(sig_value->status_, databridge::BroadcastingStatus::FILLING);

  // ID of new memory slot shall be the same as memory pool list size.
  EXPECT_EQ(sig_value->id_, databridge::kNumSignals);
}

} // namespace next::databridge::testing
