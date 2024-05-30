#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <atomic>
#include <string>
#include <vector>

#include "signal_watcher.h"
#include "udex_dynamic_publisher.h"
#define MAX_PUBLISHER_COUNT 3000

namespace next {
namespace udex {
constexpr const int waiting_time_thread = 10000;
void fill_binaryinfo(std::vector<char> &binary_data_description, unsigned int size) {
  char fill_data = 0;
  for (unsigned int count = 0; count < size; count++)
    binary_data_description.push_back(fill_data++); // fill some data
}
std::shared_ptr<ecal_util::DynamicPublisher> CreateTestPublisher([[maybe_unused]] std::string url) {
  auto dyn_publisher = std::make_shared<ecal_util::DynamicPublisher>(true);
  return dyn_publisher;
}

TEST(EcalStressTest, DISABLED_EcalCreateMultiPublishers) {
  std::atomic<int> call_back_count = 0;
  auto callbac_fun =
      [&call_back_count]([[maybe_unused]] std::string topic_name,
                         [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count++;
  };
  ecal_util::SignalWatcher &signal_watcher = ecal_util::SignalWatcher::GetInstance();
  std::string topic_name = "Topic.EcalCreateMultiPublishers";
  std::string test_url = "Test.EcalCreateMultiPublishers";
  std::string node_name = "Node.EcalCreateMultiPublishers";

  SignalDescription desc;
  fill_binaryinfo(desc.binary_data_description, 1024); // fill ikb data
  desc.basic_info.mode = PackageMode::PACKAGE_MODE_BINARY;

  for (size_t count = 0; count < MAX_PUBLISHER_COUNT; count++) {
    signal_watcher.requestUrl(test_url + std::to_string(count), callbac_fun);
  }

  std::vector<ecal_util::DynamicPublisher *> publisher;
  for (int pub_count = 0; pub_count < MAX_PUBLISHER_COUNT; pub_count++) {
    auto pub = new ecal_util::DynamicPublisher(true);
    publisher.push_back(pub);
    desc.basic_info.parent_url = test_url + std::to_string(pub_count);
    desc.basic_info.dummy = pub_count;
    pub->CreateTopic(topic_name + std::to_string(pub_count), desc);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  EXPECT_EQ(call_back_count, MAX_PUBLISHER_COUNT) << "callback not called correct amout of times ";

  for (size_t pub_count = 0; pub_count < MAX_PUBLISHER_COUNT; pub_count++) {
    delete (publisher[pub_count]);
  }
}
} // namespace udex
} // namespace next
