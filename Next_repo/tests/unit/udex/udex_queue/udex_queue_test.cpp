#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include "udex_queue.h"

constexpr int kSignal_Size = 156; // VDY.vdhDyn described in SDL

constexpr int kSmall_BinaryBlob_Size = 80;
constexpr int kEqual_BinaryBlob_Size = 156;
constexpr int kLarge_BinaryBlob_Size = 160; // measfreeze size due to OS limitations

constexpr int kSizeDiffThreshold = 8;

using namespace next::udex;

namespace next {
namespace udex {

class UDexQueueTester : public UDex_Queue {
public:
  UDexQueueTester(IConverter *converter, const std::string &schema, const PackageInfo &package_info,
                  const SignalInfo &signal_info, bool blocking_flag, bool drop_extra_input)
      : UDex_Queue(converter, schema, package_info, signal_info, blocking_flag, drop_extra_input) {}
  ~UDexQueueTester(){};

  void test_prepare_extraction_payload(const next::udex::UdexDataPackage *UdexDataPackage, void *&input, size_t &size) {
    prepare_extraction_payload(UdexDataPackage, input, size);
  }
  void SetBlocking(bool blocking_enabled) { blocking_flag_ = blocking_enabled; }
};

} // namespace udex
} // namespace next

struct TestTiming {
  unsigned short exec;
  unsigned short push;
};

class UdexQueueTestFixture : public testing::TestWithParam<TestTiming> {
public:
  UdexQueueTestFixture() : obj(nullptr, "", package_info_, signal_info_, false, false) { obj.setQueueMaxSize(100); }
  PackageInfo package_info_{"", 0, 0, 0, next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN};
  SignalInfo signal_info_{next::udex::SignalType::SIGNAL_TYPE_STRUCT, kSignal_Size, 1, 0};

  UdexDataPackage createPackage(size_t size) {
    memory_.resize(size);
    memset((void *)memory_.data(), 0xFF, size);

    UdexDataPackage udex_data_package;
    udex_data_package.info.PackageUrl = "device.view.group";
    udex_data_package.Data = memory_.data();
    udex_data_package.Size = size;
    udex_data_package.info.Time = 999999999;
    udex_data_package.info.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN;
    return udex_data_package;
  }

  void SetUp() override {
#ifdef BUILD_TESTING
    auto timings = GetParam();
    obj.sleep_ms_exec = timings.exec;
    obj.sleep_ms_push = timings.push;
#endif
  }

  bool WaitForThreadStart(std::atomic<bool> &thread_started) {
    int watchdog = 0;
    while (!thread_started && watchdog < 10) {
      watchdog++;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    // this makes sure even though the thread has started the call to push is done
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if (watchdog >= 10) {
      return false;
    }
    return true;
  }

  std::vector<char> memory_;
  UDexQueueTester obj;
};

TEST_P(UdexQueueTestFixture, Small_BinaryBlob_Size) {
  obj.setSizeDiffThreshold(kSizeDiffThreshold);
  UdexDataPackage udex_data_package = createPackage(kSmall_BinaryBlob_Size);
  void *input = nullptr;
  size_t size = 0;
  obj.test_prepare_extraction_payload(&udex_data_package, input, size);
  EXPECT_EQ(size, kSmall_BinaryBlob_Size);
}

TEST_P(UdexQueueTestFixture, Large_BinaryBlob_Size) {
  obj.setSizeDiffThreshold(kSizeDiffThreshold);
  UdexDataPackage udex_data_package = createPackage(kLarge_BinaryBlob_Size);
  void *input = nullptr;
  size_t size = 0;
  obj.test_prepare_extraction_payload(&udex_data_package, input, size);
  EXPECT_EQ(size, kSignal_Size);
}

TEST_P(UdexQueueTestFixture, Equal_BinaryBlob_Size) {
  obj.setSizeDiffThreshold(kSizeDiffThreshold);
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  void *input = nullptr;
  size_t size = 0;
  obj.test_prepare_extraction_payload(&udex_data_package, input, size);
  EXPECT_EQ(size, kSignal_Size);
}

TEST_P(UdexQueueTestFixture, TestPushAsync) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  obj.SetBlocking(false);
  obj.Push("device.view.group", &udex_data_package);
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
}

TEST_P(UdexQueueTestFixture, TestPushAsyncWithCallback) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  auto udex_callback = [&](const std::string) {
    EXPECT_EQ(obj.PullQueue().size(), 1);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");

  obj.SetBlocking(false);
  obj.setQueueCallbacks(callback_map);
  obj.Push("device.view.group", &udex_data_package);
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
}

TEST_P(UdexQueueTestFixture, TestPushAsyncPushFromDifferentThread) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  obj.SetBlocking(false);
  std::atomic<bool> thread_started = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
  thread.join();
}

TEST_P(UdexQueueTestFixture, TestPushAsyncPushFromDifferentThreadWithCallback) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  auto udex_callback = [&](const std::string) {
    EXPECT_EQ(obj.PullQueue().size(), 1);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");

  obj.SetBlocking(false);
  obj.setQueueCallbacks(callback_map);
  std::atomic<bool> thread_started = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
  thread.join();
}

TEST_P(UdexQueueTestFixture, TestPushSync) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  obj.SetBlocking(true);
  obj.Push("device.view.group", &udex_data_package);
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
}

TEST_P(UdexQueueTestFixture, TestPushSyncWithCallback) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  auto udex_callback = [&](const std::string) {
    EXPECT_EQ(obj.PullQueue().size(), 1);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");

  obj.SetBlocking(true);
  obj.setQueueCallbacks(callback_map);
  obj.Push("device.view.group", &udex_data_package);
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
}

TEST_P(UdexQueueTestFixture, TestPushSyncPushFromDifferentThread) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  obj.SetBlocking(true);
  std::atomic<bool> thread_started = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
  thread.join();
}

TEST_P(UdexQueueTestFixture, TestPushSyncPushFromDifferentThreadWithCallback) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  auto udex_callback = [&](const std::string) {
    EXPECT_EQ(obj.PullQueue().size(), 1);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");

  obj.SetBlocking(true);
  obj.setQueueCallbacks(callback_map);
  std::atomic<bool> thread_started = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 1);
  thread.join();
}

TEST_P(UdexQueueTestFixture, TestPushASyncMultiPushWithCallback) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  std::atomic<int> counter = 0;
  auto udex_callback = [&](const std::string) {
    EXPECT_EQ(obj.PullQueue().size(), ++counter);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");

  obj.SetBlocking(false);
  obj.setQueueCallbacks(callback_map);
  obj.Push("device.view.group", &udex_data_package);
  obj.Push("device.view.group", &udex_data_package);
  obj.Push("device.view.group", &udex_data_package);
  obj.Push("device.view.group", &udex_data_package);

  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(counter, 4);
  EXPECT_EQ(obj.PullQueue().size(), 4);
}

TEST_P(UdexQueueTestFixture, TestPushFromDifferentThreadWithLimitedQueueSize) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  std::atomic<int> counter = 0;
  unsigned short limit_queue_size = 2;
  auto udex_callback = [&](const std::string) {
    counter++;
    if (counter <= limit_queue_size)
      EXPECT_EQ(obj.PullQueue().size(), counter);
    else
      EXPECT_EQ(obj.PullQueue().size(), limit_queue_size);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");
  obj.setQueueMaxSize(limit_queue_size);
  obj.SetBlocking(true);
  obj.setQueueCallbacks(callback_map);
  std::atomic<bool> thread_started = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
    obj.Push("device.view.group", &udex_data_package);
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(obj.isDataAvailable());
  thread.join();
  EXPECT_EQ(obj.PullQueue().size(), limit_queue_size);
}

TEST_P(UdexQueueTestFixture, TestPushASyncDifferentThreadWithLimitedQueueSize) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  std::atomic<int> counter = 0;
  unsigned short limit_queue_size = 2;

  auto udex_callback = [&](const std::string) {
    counter++;
    if (counter <= limit_queue_size)
      EXPECT_EQ(obj.PullQueue().size(), counter);
    else
      EXPECT_EQ(obj.PullQueue().size(), limit_queue_size);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");
  obj.setQueueMaxSize(limit_queue_size);
  obj.SetBlocking(false);
  obj.setQueueCallbacks(callback_map);

  std::atomic<bool> thread_started = false;
  std::atomic<bool> thread_started1 = false;
  std::atomic<bool> thread_started2 = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  auto thread1 = std::thread([&]() {
    thread_started1 = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  auto thread2 = std::thread([&]() {
    thread_started2 = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(WaitForThreadStart(thread_started1));
  EXPECT_TRUE(WaitForThreadStart(thread_started2));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), limit_queue_size);
  thread.join();
  thread1.join();
  thread2.join();
}
TEST_P(UdexQueueTestFixture, TestPushSyncDifferentThreadWithLimitedQueueSize) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  std::atomic<int> counter = 0;
  unsigned short limit_queue_size = 2;

  auto udex_callback = [&](const std::string) {
    counter++;
    if (counter < limit_queue_size)
      EXPECT_EQ(obj.PullQueue().size(), counter);
    else
      EXPECT_EQ(obj.PullQueue().size(), limit_queue_size);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");
  obj.setQueueMaxSize(limit_queue_size);
  obj.SetBlocking(true);
  obj.setQueueCallbacks(callback_map);

  std::atomic<bool> thread_started = false;
  std::atomic<bool> thread_started1 = false;
  std::atomic<bool> thread_started2 = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  auto thread1 = std::thread([&]() {
    thread_started1 = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  auto thread2 = std::thread([&]() {
    thread_started2 = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(WaitForThreadStart(thread_started1));
  EXPECT_TRUE(WaitForThreadStart(thread_started2));
  EXPECT_TRUE(obj.isDataAvailable());
  thread.join();
  thread1.join();
  thread2.join();
  EXPECT_EQ(obj.PullQueue().size(), limit_queue_size);
}

TEST_P(UdexQueueTestFixture, TestPushASyncMultiPushFromDifferentThreadWithCallback) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  std::atomic<int> counter = 0;
  auto udex_callback = [&](const std::string) {
    EXPECT_EQ(obj.PullQueue().size(), ++counter);
    EXPECT_TRUE(obj.isDataAvailable());
  };
  callback_map[0] = std::make_pair(udex_callback, "test");

  obj.SetBlocking(false);
  obj.setQueueCallbacks(callback_map);
  std::atomic<bool> thread_started = false;
  std::atomic<bool> thread_started1 = false;
  std::atomic<bool> thread_started2 = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  auto thread1 = std::thread([&]() {
    thread_started1 = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  auto thread2 = std::thread([&]() {
    thread_started2 = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(WaitForThreadStart(thread_started1));
  EXPECT_TRUE(WaitForThreadStart(thread_started2));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), 3);
  thread.join();
  thread1.join();
  thread2.join();
}

TEST_P(UdexQueueTestFixture, TestSwitchAsyncSync) {
  UdexDataPackage udex_data_package = createPackage(kEqual_BinaryBlob_Size);
  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> callback_map;
  std::atomic<size_t> num_elements = 1;

  auto udex_callback = [&](const std::string) { EXPECT_EQ(obj.PullQueue().size(), num_elements); };
  callback_map[0] = std::make_pair(udex_callback, "test");

  obj.SetBlocking(true);
  obj.setQueueCallbacks(callback_map);
  std::atomic<bool> thread_started = false;
  auto thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_EQ(obj.PullQueue().size(), num_elements);
  thread.join();

  obj.SetBlocking(false);
  num_elements++;
  thread_started = false;
  thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_EQ(obj.PullQueue().size(), num_elements);
  thread.join();

  obj.SetBlocking(true);
  num_elements++;
  thread_started = false;
  thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), num_elements);
  thread.join();

  obj.SetBlocking(false);
  num_elements++;
  thread_started = false;
  thread = std::thread([&]() {
    thread_started = true;
    obj.Push("device.view.group", &udex_data_package);
  });
  EXPECT_TRUE(WaitForThreadStart(thread_started));
  EXPECT_TRUE(obj.isDataAvailable());
  EXPECT_EQ(obj.PullQueue().size(), num_elements);
  thread.join();
}

constexpr TestTiming k_delays[] = {{50, 150}, {150, 50}, {50, 50}};

INSTANTIATE_TEST_CASE_P(UdexQueueTest, UdexQueueTestFixture, ::testing::ValuesIn(k_delays));