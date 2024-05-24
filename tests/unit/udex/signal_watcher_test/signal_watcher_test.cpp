#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <atomic>
#include <fstream>
#include <string>
#include <vector>

#include <next/sdk/sdk.hpp>

#include "signal_watcher.h"
#include "udex_dynamic_publisher.h"

namespace next {
namespace udex {
namespace ecal_util {

class SignalWatcherTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  SignalWatcherTestFixture() { this->instance_name_ = "SignalWatcherTestFixture"; }
};

/*
TEST_F(SignalWatcherTestFixture, SignalWatcherSingletonTest) {

  std::shared_ptr<SignalWatcher> instance_returned = SignalWatcher::GetInstance();
  auto thread_fun1 = [&instance_returned]() -> void {
    std::shared_ptr<SignalWatcher> SignalWatcherInstanceReturned = SignalWatcher::GetInstance();
    ASSERT_EQ(instance_returned.get(), SignalWatcherInstanceReturned.get())
        << "Multiple instance of SignalWatcher created";
  };

  auto thread_fun2 = [&instance_returned]() -> void {
    std::shared_ptr<SignalWatcher> SignalWatcherInstanceReturned = SignalWatcher::GetInstance();
    ASSERT_EQ(instance_returned.get(), SignalWatcherInstanceReturned.get())
        << "Multiple instance of SignalWatcher created";
  };

  auto thread_fun3 = [&instance_returned]() -> void {
    std::shared_ptr<SignalWatcher> SignalWatcherInstanceReturned = SignalWatcher::GetInstance();
    ASSERT_EQ(instance_returned.get(), SignalWatcherInstanceReturned.get())
        << "Multiple instance of SignalWatcher created";
  };

  std::thread test_thread_1(thread_fun1);
  std::thread test_thread_2(thread_fun2);
  std::thread test_thread_3(thread_fun3);

  test_thread_1.join();
  test_thread_2.join();
  test_thread_3.join();
}
*/
constexpr int kWaitTimeMs = 3000;
constexpr int kWaitIncrementMs = 150;

bool WaitUrl(SignalWatcher &signal_watcher, std::string url) {
  int waited_time = 0;
  while (!signal_watcher.checkUrlAvailable(url)) {
    if (waited_time > kWaitTimeMs) {
      std::cerr << "wait_time expired" << std::flush;
      return false;
    }
    waited_time += kWaitIncrementMs;
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(kWaitIncrementMs));
  }
  std::cout << "url is available n" << std::flush;
  return true;
}

std::string generateUniqueName(const std::string &in) {
  return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch())
                                 .count());
}

TEST_F(SignalWatcherTestFixture, checkUrlAfterCreation_test) {
  std::string test_url = generateUniqueName("device.group.port.checkUrlAvailable_test");
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  std::string topic_name = generateUniqueName("Topic_checkUrlAvailable_test");
  pub.CreateTopic(topic_name, desc, true);
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  EXPECT_EQ(signal_watcher.checkUrlAvailable(test_url), true) << "Update not found";
}

TEST_F(SignalWatcherTestFixture, checkUrlAvailable_test) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  std::string test_url = generateUniqueName("device.group.port.checkUrlAvailable_test");
  EXPECT_EQ(signal_watcher.checkUrlAvailable(test_url), false) << "Check empty list failed";
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  std::string topic_name = generateUniqueName("Topic_checkUrlAvailable_test");
  pub.CreateTopic(topic_name, desc, true);

  EXPECT_EQ(WaitUrl(signal_watcher, test_url), true) << "Update not found";
  EXPECT_EQ(signal_watcher.checkUrlAvailable(test_url), true) << "Update not found";
}

TEST_F(SignalWatcherTestFixture, checkDeviceFormatReferenceCamera) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;

  std::string url1 = generateUniqueName("url_checkDeviceFormatReferenceCamera1");
  std::string url2 = generateUniqueName("url_checkDeviceFormatReferenceCamera2");

  desc.basic_info.parent_url = url1;
  desc.basic_info.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_CAN;
  desc.basic_info.data_source_name = "canDevice";
  std::string topic_name = generateUniqueName("Topic_checkDeviceFormatReferenceCamera");
  pub.CreateTopic(topic_name, desc, true);

  ecal_util::DynamicPublisher pub1(true);
  desc.basic_info.parent_url = url2;
  desc.basic_info.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_REFCAM;
  desc.basic_info.data_source_name = "refCam";
  topic_name = generateUniqueName("Topic_checkDeviceFormatReferenceCamera1");
  pub1.CreateTopic(topic_name, desc, true);

  // wait for the topics to be there
  EXPECT_EQ(WaitUrl(signal_watcher, url1), true) << "Update not found";
  EXPECT_EQ(WaitUrl(signal_watcher, url2), true) << "Update not found";

  auto devices = signal_watcher.getDevicesByFormat("mts.can");
  ASSERT_EQ(devices.size(), 1) << "Device number not correct";
  EXPECT_EQ(devices[0], "canDevice") << "Device name wrong";

  devices = signal_watcher.getDevicesByFormat(next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_CAN);
  ASSERT_EQ(devices.size(), 1) << "Device number not correct";
  EXPECT_EQ(devices[0], "canDevice") << "Device name wrong";

  devices = signal_watcher.getDevicesByFormat(next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_REFCAM);
  ASSERT_EQ(devices.size(), 1) << "Device number not correct";
  EXPECT_EQ(devices[0], "refCam") << "Device name wrong";

  devices = signal_watcher.getDevicesByFormat("mts.refcam");
  ASSERT_EQ(devices.size(), 1) << "Device number not correct";
  EXPECT_EQ(devices[0], "refCam") << "Device name wrong";
}

TEST_F(SignalWatcherTestFixture, checkgetDeviceByFormat) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  std::string test_url = generateUniqueName("device.group.port.checkgetDeviceByFormat");
  EXPECT_EQ(signal_watcher.checkUrlAvailable(test_url), false) << "Check empty list failed";
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  desc.basic_info.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_CAN;
  desc.basic_info.data_source_name = "canDevice";
  std::string topic_name = generateUniqueName("Topic_checkgetDeviceByFormat");
  pub.CreateTopic(topic_name, desc, true);

  EXPECT_EQ(WaitUrl(signal_watcher, test_url), true) << "Update not found";
  auto devices = signal_watcher.getDevicesByFormat("mts.can");
  ASSERT_EQ(devices.size(), 1) << "Device number not correct";
  EXPECT_EQ(devices[0], "canDevice") << "Device name wrong";

  devices = signal_watcher.getDevicesByFormat(next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_CAN);
  ASSERT_EQ(devices.size(), 1) << "Device number not correct";
  EXPECT_EQ(devices[0], "canDevice") << "Device name wrong";
}

TEST_F(SignalWatcherTestFixture, invoke_callback_test) {
  std::atomic<int> call_back_count = 0;
  std::atomic<int> call_back_count1 = 0;
  auto callbac_fun =
      [&call_back_count]([[maybe_unused]] std::string topic_name,
                         [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count++;
  };

  auto callbac_fun1 =
      [&call_back_count1]([[maybe_unused]] std::string topic_name,
                          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count1++;
  };

  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  std::string test_url = generateUniqueName("device.group.port.urltest,.invoke_callback_test");

  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  std::string topic_name = generateUniqueName("Topic_invoke_callback_test");
  pub.CreateTopic(topic_name, desc, true);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  signal_watcher.requestUrl(test_url, callbac_fun);
  signal_watcher.requestUrl(test_url, callbac_fun);
  signal_watcher.requestUrl(test_url, callbac_fun1);
  signal_watcher.requestUrl(test_url, callbac_fun);
  EXPECT_EQ(WaitUrl(signal_watcher, test_url), true) << "Update not found";
  EXPECT_EQ(call_back_count, 3) << "callback not called correct amout of times";
  EXPECT_EQ(call_back_count1, 1) << "callback not called correct amout of times ";
}

TEST_F(SignalWatcherTestFixture, requestUrl_MultiTopicName) {
  // distiguish to different descriptions with the same topic name
  std::atomic<int> call_back_count_description_1 = 0;
  std::atomic<int> call_back_count_description_2 = 0;
  auto callbac_fun = [&call_back_count_description_1](
                         [[maybe_unused]] std::string topic_name,
                         [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_1++;
  };

  auto callbac_fun1 = [&call_back_count_description_2](
                          [[maybe_unused]] std::string topic_name,
                          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_2++;
  };

  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();

  std::string topic_name = generateUniqueName("Diff_desccriptions_same_topic_name_test");
  std::string test_url_1 = generateUniqueName("requestUrl_MultiTopicName.device.group.port.urltest1");
  ecal_util::DynamicPublisher pub1(true);
  SignalDescription desc1;
  desc1.basic_info.parent_url = test_url_1;
  pub1.CreateTopic(topic_name, desc1, true);

  std::string test_url_2 = "device.group.port.urltest2";
  ecal_util::DynamicPublisher pub2(true);
  SignalDescription desc2;
  desc2.basic_info.parent_url = test_url_2;
  pub2.CreateTopic(topic_name + "1", desc2, true);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  signal_watcher.requestUrl(test_url_1, callbac_fun);
  signal_watcher.requestUrl(test_url_2, callbac_fun1);
  EXPECT_EQ(WaitUrl(signal_watcher, test_url_2), true) << "Update not found";
  EXPECT_EQ(call_back_count_description_1, 1) << "callback not called correct amout of times";
  EXPECT_EQ(call_back_count_description_2, 1) << "callback not called correct amout of times";
}

TEST_F(SignalWatcherTestFixture, checkDescriptionInCallback) {
  std::atomic<int> call_back_count = 0;
  struct SignalDescription test_input;
  test_input.basic_info.parent_url = generateUniqueName("checkDescriptionInCallback");
  test_input.basic_info.dummy = 100;
  test_input.basic_info.mode = PackageMode::PACKAGE_MODE_SDL;
  test_input.binary_data_description = {'c', 'h', 'e', 'c', 'k', 'D', 'e', 's', 'c', 'r', 'i', 'p', 't',
                                        'i', 'o', 'n', 'I', 'n', 'C', 'a', 'l', 'l', 'b', 'a', 'c', 'k'};

  auto callbac_fun = [test_input,
                      &call_back_count]([[maybe_unused]] std::string topic_name,
                                        next::udex::ecal_util::SignalDescriptionHandler desc_handler) -> void {
    auto signal_description_hanlder = ecal_util::SignalDescriptionHandler();
    call_back_count++;
    EXPECT_EQ(desc_handler.GetParentUrl(), test_input.basic_info.parent_url) << "Parent url is not received correctly";
    EXPECT_EQ(desc_handler.GetBasicInformation().dummy, test_input.basic_info.dummy)
        << "dummy is not received correctly";
    EXPECT_EQ(desc_handler.GetBasicInformation().mode, test_input.basic_info.mode) << "mode is not received correctly";
    EXPECT_EQ(desc_handler.GetDescription().binary_data_description, test_input.binary_data_description)
        << "binary description is not received correctly";
  };

  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();

  SignalDescription desc;
  desc.basic_info = test_input.basic_info;
  desc.binary_data_description = test_input.binary_data_description;

  ecal_util::DynamicPublisher pub(true);
  std::string topic_name = generateUniqueName("Topic_checkDescriptionInCallback");
  pub.CreateTopic(topic_name, desc, true);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  signal_watcher.requestUrl(test_input.basic_info.parent_url, callbac_fun);

  ASSERT_EQ(WaitUrl(signal_watcher, test_input.basic_info.parent_url), true) << "Update not found";
  EXPECT_EQ(call_back_count, 1) << "callback not called correct amout of times";
}

TEST_F(SignalWatcherTestFixture, checkDescription) {

  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();

  bool description_valid = false;
  struct SignalDescription test_input;
  test_input.basic_info.parent_url = generateUniqueName("device.group.port.checkDescription");
  test_input.basic_info.dummy = 100;
  test_input.basic_info.mode = PackageMode::PACKAGE_MODE_SDL;
  test_input.binary_data_description = {'c', 'h', 'e', 'c', 'k', 'D', 'e', 's', 'c', 'r', 'i', 'p', 't',
                                        'i', 'o', 'n', 'I', 'n', 'C', 'a', 'l', 'l', 'b', 'a', 'c', 'k'};

  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_input.basic_info.parent_url;
  desc.basic_info.dummy = test_input.basic_info.dummy;
  desc.basic_info.mode = test_input.basic_info.mode;
  desc.binary_data_description = test_input.binary_data_description;
  std::string topic_name = generateUniqueName("Topic_checkDescription");
  pub.CreateTopic(topic_name, desc, true);
  ASSERT_EQ(WaitUrl(signal_watcher, test_input.basic_info.parent_url), true) << "Update not found";

  auto description_handler =
      signal_watcher.getTopicDescriptionHandler(test_input.basic_info.parent_url, description_valid);
  EXPECT_EQ(description_valid, true) << "signal description not valid ";
  EXPECT_EQ(description_handler.GetParentUrl(), test_input.basic_info.parent_url)
      << "Parent url is not received correctly";
  EXPECT_EQ(description_handler.GetBasicInformation().dummy, test_input.basic_info.dummy)
      << "dummy is not received correctly";
  EXPECT_EQ(description_handler.GetDescription().binary_data_description, test_input.binary_data_description)
      << "binary description is not received correctly";
}

TEST_F(SignalWatcherTestFixture, checkGetTopicName) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();

  bool topic_valid = false;
  struct SignalDescription test_input;
  test_input.basic_info.parent_url = generateUniqueName("device.group.port.checkGetTopicName");
  test_input.basic_info.dummy = 100;
  test_input.basic_info.mode = PackageMode::PACKAGE_MODE_SDL;
  test_input.binary_data_description = {'c', 'h', 'e', 'c', 'k', 'D', 'e', 's', 'c', 'r', 'i', 'p', 't',
                                        'i', 'o', 'n', 'I', 'n', 'C', 'a', 'l', 'l', 'b', 'a', 'c', 'k'};

  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_input.basic_info.parent_url;
  desc.basic_info.dummy = test_input.basic_info.dummy;
  desc.basic_info.mode = test_input.basic_info.mode;
  desc.binary_data_description = test_input.binary_data_description;
  std::string topic_name = generateUniqueName("Topic_checkGetTopicName");
  pub.CreateTopic(topic_name, desc, true);
  ASSERT_EQ(WaitUrl(signal_watcher, test_input.basic_info.parent_url), true) << "Update not found";
  auto description_received = signal_watcher.getTopicName(test_input.basic_info.parent_url, topic_valid);
  EXPECT_EQ(topic_valid, true) << "topic name not valid ";
  EXPECT_EQ(description_received, topic_name) << "topic name not received correctly ";
}

TEST_F(SignalWatcherTestFixture, GetRootUrlList_test) {

  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();

  std::string topic_name1 = generateUniqueName("GetRootUrlList_test.Topic_1");
  std::string topic_name2 = generateUniqueName("GetRootUrlList_test.Topic_2");
  std::string topic_name3 = generateUniqueName("GetRootUrlList_test.Topic_3");

  std::string test_url1 = generateUniqueName("GetRootUrlList_test.device.group.port.urltest1");
  std::string test_url2 = generateUniqueName("GetRootUrlList_test.device.group.port.urltest2");
  std::string test_url3 = generateUniqueName("GetRootUrlList_test.device.group.port.urltest3");

  ecal_util::DynamicPublisher pub1(true);
  ecal_util::DynamicPublisher pub2(true);
  ecal_util::DynamicPublisher pub3(true);

  SignalDescription desc1;
  SignalDescription desc2;
  SignalDescription desc3;

  desc1.basic_info.parent_url = test_url1;
  desc1.basic_info.data_source_name = "GetRootUrlList_test";
  pub1.CreateTopic(topic_name1, desc1, true);
  desc2.basic_info.parent_url = test_url2;
  desc2.basic_info.data_source_name = "GetRootUrlList_test";
  pub2.CreateTopic(topic_name2, desc2, true);
  desc3.basic_info.parent_url = test_url3;
  desc3.basic_info.data_source_name = "GetRootUrlList_test";
  pub3.CreateTopic(topic_name3, desc3, true);

  ASSERT_EQ(WaitUrl(signal_watcher, desc3.basic_info.parent_url), true) << "Update not found";
  auto root_url_list = signal_watcher.GetRootUrlList(SignalWatcher::DEVICE_LEVEL, "");

  EXPECT_EQ(root_url_list[0].name, "GetRootUrlList_test") << "Root url not updated correctly";
}
/*
TEST_F(SignalWatcherTestFixture, checkResetAndInstances) {
  constexpr const int instance_count_max = 1000;
  std::shared_ptr<SignalWatcher> test_instance = SignalWatcher::GetInstance();
  std::vector<std::shared_ptr<SignalWatcher>> signal_watcher_instances;
  for (size_t count = 0; count < instance_count_max; count++) {
    std::shared_ptr<SignalWatcher> SignalWatcherInstanceReturned = SignalWatcher::GetInstance();
    ASSERT_EQ(SignalWatcherInstanceReturned.get(), test_instance.get()) << "Multiple instance of SignalWatcher created";
  }
}*/

TEST_F(SignalWatcherTestFixture, getSignalDescription) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();

  bool description_valid = false;
  struct SignalDescription test_input;
  test_input.basic_info.parent_url = generateUniqueName("device.group.port.getSignalDescription");
  test_input.basic_info.dummy = 100;
  test_input.basic_info.mode = PackageMode::PACKAGE_MODE_SDL;
  test_input.binary_data_description = {'c', 'h', 'e', 'c', 'k', 'D', 'e', 's', 'c', 'r', 'i', 'p', 't',
                                        'i', 'o', 'n', 'I', 'n', 'C', 'a', 'l', 'l', 'b', 'a', 'c', 'k'};

  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_input.basic_info.parent_url;
  desc.basic_info.dummy = test_input.basic_info.dummy;
  desc.basic_info.mode = test_input.basic_info.mode;
  desc.binary_data_description = test_input.binary_data_description;
  std::string topic_name = generateUniqueName("Topic_getSignalDescription");
  pub.CreateTopic(topic_name, desc, true);

  ASSERT_EQ(WaitUrl(signal_watcher, desc.basic_info.parent_url), true) << "Update not found";

  auto description_received = signal_watcher.getSignalDescription(test_input.basic_info.parent_url, description_valid);
  EXPECT_EQ(description_valid, true) << "signal description not valid ";
  EXPECT_EQ(description_received.basic_info.parent_url, test_input.basic_info.parent_url)
      << "Parent url is not received correctly";
  EXPECT_EQ(description_received.basic_info.dummy, test_input.basic_info.dummy) << "dummy is not received correctly";
  EXPECT_EQ(description_received.binary_data_description, test_input.binary_data_description)
      << "binary description is not received correctly";
}
TEST_F(SignalWatcherTestFixture, requestUrl_MultiRequestAndRemoval) {
  // distiguish to different descriptions with the same topic name
  std::atomic<int> call_back_count_description_0 = 0;
  std::atomic<int> call_back_count_description_1 = 0;
  std::atomic<int> call_back_count_description_2 = 0;
  std::atomic<int> call_back_count_description_3 = 0;
  std::atomic<int> call_back_count_description_4 = 0;
  std::atomic<int> call_back_count_description_5 = 0;
  std::atomic<int> call_back_count_description_6 = 0;

  auto callbac_fun = [&call_back_count_description_0](
                         [[maybe_unused]] std::string topic_name,
                         [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_0++;
  };
  (void)callbac_fun; // fake usage

  auto callbac_fun1 = [&call_back_count_description_1](
                          [[maybe_unused]] std::string topic_name,
                          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_1++;
  };
  (void)callbac_fun1; // fake usage

  auto callbac_fun2 = [&call_back_count_description_2](
                          [[maybe_unused]] std::string topic_name,
                          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_2++;
  };
  (void)callbac_fun2; // fake usage

  auto callbac_fun3 = [&call_back_count_description_3](
                          [[maybe_unused]] std::string topic_name,
                          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_3++;
  };
  (void)callbac_fun3; // fake usage

  auto callbac_fun4 = [&call_back_count_description_4](
                          [[maybe_unused]] std::string topic_name,
                          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_4++;
  };
  (void)callbac_fun4; // fake usage

  auto callbac_fun5 = [&call_back_count_description_5](
                          [[maybe_unused]] std::string topic_name,
                          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_5++;
  };
  (void)callbac_fun5; // fake usage

  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();

  std::string test_url_1 = generateUniqueName("requestUrl_MultiTopicName.device.group.port.urltest1");
  std::string test_url_2 = generateUniqueName("requestUrl_MultiTopicName.device.group.port.urltest2");
  std::string test_url_3 = generateUniqueName("requestUrl_MultiTopicName.device.group.port.urltest3");
  std::string test_url_4 = generateUniqueName("requestUrl_MultiTopicName.device.group.port.urltest4");
  std::string test_url_5 = generateUniqueName("requestUrl_MultiTopicName.device.group.port.urltest5");

  std::vector<size_t> requestedUrl_ids;

  std::string topic_name1 = generateUniqueName("Topic1");
  std::string topic_name2 = generateUniqueName("Topic2");
  std::string topic_name3 = generateUniqueName("Topic3");
  std::string topic_name4 = generateUniqueName("Topic4");

  ecal_util::DynamicPublisher pub1(true);
  SignalDescription desc1;
  desc1.basic_info.parent_url = test_url_1;
  pub1.CreateTopic(topic_name1, desc1, true);
  ASSERT_EQ(WaitUrl(signal_watcher, desc1.basic_info.parent_url), true) << "Update not found";

  ecal_util::DynamicPublisher pub2(true);
  SignalDescription desc2;
  desc2.basic_info.parent_url = test_url_4;
  pub2.CreateTopic(topic_name2, desc2, true);

  ASSERT_EQ(WaitUrl(signal_watcher, desc2.basic_info.parent_url), true) << "Update not found";

  ecal_util::DynamicPublisher pub3(true);
  SignalDescription desc3;
  desc3.basic_info.parent_url = test_url_5;
  pub3.CreateTopic(topic_name3, desc3, true);

  ASSERT_EQ(WaitUrl(signal_watcher, desc3.basic_info.parent_url), true) << "Update not found";

  // Request for urls and verify the urls are availble after specified delay
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun1));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_5, callbac_fun5));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun2));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_4, callbac_fun4));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun3));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_4, callbac_fun3));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun4));

  EXPECT_EQ(call_back_count_description_1, 1) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_2, 1) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_3, 2) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_5, 1) << "callback not called correct amout of times ";

  desc1.basic_info.parent_url = test_url_1;
  pub1.CreateTopic(topic_name1, desc1, true);

  desc2.basic_info.parent_url = test_url_4;
  pub2.CreateTopic(topic_name2, desc2, true);

  desc3.basic_info.parent_url = test_url_5;
  pub3.CreateTopic(topic_name3, desc3, true);

  // Request for the same urls and check the count increated only the specified requested times
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun1));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_5, callbac_fun5));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun2));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_4, callbac_fun4));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun3));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_4, callbac_fun3));
  requestedUrl_ids.push_back(signal_watcher.requestUrl(test_url_1, callbac_fun4));

  EXPECT_EQ(call_back_count_description_1, 2) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_2, 2) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_3, 4) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_5, 2) << "callback not called correct amout of times ";
}

TEST_F(SignalWatcherTestFixture, checkTopicListUpdateOnNewType) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  std::string test_url = generateUniqueName("device.group.port.checkTopicListUpdateOnNewType");

  std::vector<size_t> requestedUrl_ids;

  std::string topic_name = generateUniqueName("Topic_checkTopicListUpdateOnNewType");

  ecal_util::DynamicPublisher pub1(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  desc.binary_data_description = {'a', 'b', 'c', 'd'};

  pub1.CreateTopic(topic_name, desc, true);

  ASSERT_EQ(WaitUrl(signal_watcher, desc.basic_info.parent_url), true) << "Update not found";

  bool available;
  SignalDescriptionHandler port_desc = signal_watcher.getTopicDescriptionHandler(desc.basic_info.parent_url, available);
  ASSERT_EQ(available, true) << "topic not available";
  EXPECT_EQ(port_desc.GetSignalDescription(), desc.binary_data_description);

  SignalDescription desc1;
  desc1.basic_info.parent_url = test_url;
  desc1.binary_data_description = {'d', 'c', 'b', 'a'};
  bool reset = pub1.ResetTopic();
  ASSERT_EQ(reset, true) << "Publisher not reset";
  std::this_thread::sleep_for(std::chrono::milliseconds(6000));
  bool created = pub1.CreateTopic(topic_name, desc1, true);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  ASSERT_EQ(created, true) << "Topic not created";
  ASSERT_EQ(WaitUrl(signal_watcher, desc1.basic_info.parent_url), true) << "Update not found";

  SignalDescriptionHandler port_desc2 =
      signal_watcher.getTopicDescriptionHandler(desc.basic_info.parent_url, available);
  ASSERT_EQ(available, true) << "topic not available";
  EXPECT_EQ(port_desc2.GetSignalDescription(), desc1.binary_data_description);
}

namespace test {
class SignalWatcherTest : public SignalWatcher {

public:
  SignalWatcherTest() {}
  size_t setCallbacks(const std::string &url, const registration_callback call_back) {
    static size_t request_counter = 0;
    request_counter++;
    SignalWatcher::requested_urls[url].push_back(std::make_pair(call_back, request_counter));
    return request_counter;
  }
  void eraseCallbacks(std::string &url, size_t id) {
    SignalWatcher::eraseCallbacks(SignalWatcher::requested_urls[url], id);
  }
  void invokeCallbacks(const std::string &url) { SignalWatcher::InvokeCallback(url); }

  bool isRequestedIdPreset(size_t id) {
    for (auto it : requested_urls) {
      for (auto callbacks : it.second) {
        if (id == callbacks.second) {
          return true;
        }
      }
    }
    return false;
  }
};
} // namespace test

TEST_F(SignalWatcherTestFixture, setCallbacksTest) {
  std::atomic<int> call_back_count_description_1 = 0;
  std::atomic<int> call_back_count_description_2 = 0;
  std::atomic<int> call_back_count_description_3 = 0;

  auto callback_func1 =
      [&call_back_count_description_1](
          [[maybe_unused]] std::string topic_name,
          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_1++;
  };
  auto callback_func2 =
      [&call_back_count_description_2](
          [[maybe_unused]] std::string topic_name,
          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_2++;
  };
  auto callback_func3 =
      [&call_back_count_description_3](
          [[maybe_unused]] std::string topic_name,
          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_3++;
  };
  std::string url_1 = generateUniqueName("url1");
  std::string url_2 = generateUniqueName("url2");
  std::string url_3 = generateUniqueName("url3");
  std::string url_4 = generateUniqueName("url4");

  test::SignalWatcherTest signalwatcher;
  std::vector<size_t> req_ids;
  // Test 1 : different callbacks for same urls
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func1));
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func2));
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func3));
  for (auto it : req_ids) {
    EXPECT_EQ(signalwatcher.isRequestedIdPreset(it), true) << "Callback not registred with id";
  }
  signalwatcher.invokeCallbacks(url_1);
  EXPECT_EQ(call_back_count_description_1, 1) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_2, 1) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_3, 1) << "callback not called correct amout of times ";

  // Test 2 : different url same callback function
  req_ids.clear();
  call_back_count_description_1 = 0;
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func1));
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func1));
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func1));

  EXPECT_EQ(signalwatcher.isRequestedIdPreset(req_ids[0]), true) << "Callback not registred with id";

  signalwatcher.invokeCallbacks(url_1);
  EXPECT_EQ(call_back_count_description_1, 3) << "callback not called correct amout of times ";
}

TEST_F(SignalWatcherTestFixture, removeCallbackTest) {
  std::atomic<int> call_back_count_description_1 = 0;
  std::atomic<int> call_back_count_description_2 = 0;
  std::atomic<int> call_back_count_description_3 = 0;
  auto callback_func1 =
      [&call_back_count_description_1](
          [[maybe_unused]] std::string topic_name,
          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_1++;
  };
  auto callback_func2 =
      [&call_back_count_description_2](
          [[maybe_unused]] std::string topic_name,
          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_2++;
  };
  auto callback_func3 =
      [&call_back_count_description_3](
          [[maybe_unused]] std::string topic_name,
          [[maybe_unused]] next::udex::ecal_util::SignalDescriptionHandler topic_description) -> void {
    call_back_count_description_3++;
  };

  std::string url_1 = generateUniqueName("url1");
  std::string url_2 = generateUniqueName("url2");
  test::SignalWatcherTest signalwatcher;
  std::vector<size_t> req_ids;

  // Set callbacks
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func1));
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func2));
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func3));
  req_ids.push_back(signalwatcher.setCallbacks(url_2, callback_func2));
  req_ids.push_back(signalwatcher.setCallbacks(url_2, callback_func3));
  req_ids.push_back(signalwatcher.setCallbacks(url_2, callback_func1));
  req_ids.push_back(signalwatcher.setCallbacks(url_1, callback_func2));
  req_ids.push_back(signalwatcher.setCallbacks(url_2, callback_func3));

  // check ids registred
  for (auto it : req_ids) {
    EXPECT_EQ(signalwatcher.isRequestedIdPreset(it), true) << "Callback not registred with id";
  }

  // erase  callback
  signalwatcher.eraseCallbacks(url_1, req_ids[0]);
  signalwatcher.eraseCallbacks(url_1, req_ids[2]);
  signalwatcher.eraseCallbacks(url_2, req_ids[7]);
  EXPECT_EQ(signalwatcher.isRequestedIdPreset(req_ids[0]), false) << "Callback not removed";
  EXPECT_EQ(signalwatcher.isRequestedIdPreset(req_ids[2]), false) << "Callback not removed";
  EXPECT_EQ(signalwatcher.isRequestedIdPreset(req_ids[7]), false) << "Callback not removed";

  // invoke callbacks only one callback will remain
  signalwatcher.invokeCallbacks(url_1);
  signalwatcher.invokeCallbacks(url_2);

  EXPECT_EQ(call_back_count_description_1, 1) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_3, 1) << "callback not called correct amout of times ";
  EXPECT_EQ(call_back_count_description_3, 1) << "callback not called correct amout of times ";

  // check ids registred
  for (auto it : req_ids) {
    EXPECT_EQ(signalwatcher.isRequestedIdPreset(it), false) << "Callback not removed properly";
  }
}

TEST_F(SignalWatcherTestFixture, checkInitOrderSwPub) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  std::string topic_name = generateUniqueName("GetRootUrlList_test.Topic_1");
  std::string test_url = generateUniqueName("GetRootUrlList_test.device.group.port.urltest1");
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  desc.basic_info.data_source_name = "GetRootUrlList_test";
  pub.CreateTopic(topic_name, desc, true);
  ASSERT_EQ(WaitUrl(signal_watcher, desc.basic_info.parent_url), true) << "Update not found";
  auto root_url_list = signal_watcher.GetRootUrlList(SignalWatcher::DEVICE_LEVEL, "");
  EXPECT_EQ(root_url_list[0].name, "GetRootUrlList_test") << "Root url not updated correctly";
}

TEST_F(SignalWatcherTestFixture, checkInitOrderPubSw) {
  std::string topic_name = generateUniqueName("GetRootUrlList_test.Topic_1");
  std::string test_url = generateUniqueName("GetRootUrlList_test.device.group.port.urltest1");
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  desc.basic_info.data_source_name = "GetRootUrlList_test";
  pub.CreateTopic(topic_name, desc, true);
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  ASSERT_EQ(WaitUrl(signal_watcher, desc.basic_info.parent_url), true) << "Update not found";
  auto root_url_list = signal_watcher.GetRootUrlList(SignalWatcher::DEVICE_LEVEL, "");
  EXPECT_EQ(root_url_list[0].name, "GetRootUrlList_test") << "Root url not updated correctly";
}

TEST_F(SignalWatcherTestFixture, checkInitOrderSwPausePub) {
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  std::string topic_name = generateUniqueName("GetRootUrlList_test.Topic_1");
  std::string test_url = generateUniqueName("GetRootUrlList_test.device.group.port.urltest1");
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  desc.basic_info.data_source_name = "GetRootUrlList_test";
  pub.CreateTopic(topic_name, desc, true);
  ASSERT_EQ(WaitUrl(signal_watcher, desc.basic_info.parent_url), true) << "Update not found";
  auto root_url_list = signal_watcher.GetRootUrlList(SignalWatcher::DEVICE_LEVEL, "");
  EXPECT_EQ(root_url_list[0].name, "GetRootUrlList_test") << "Root url not updated correctly";
}

TEST_F(SignalWatcherTestFixture, checkInitOrderPubWaitSw) {
  std::string topic_name = generateUniqueName("GetRootUrlList_test.Topic_1");
  std::string test_url = generateUniqueName("GetRootUrlList_test.device.group.port.urltest1");
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc;
  desc.basic_info.parent_url = test_url;
  desc.basic_info.data_source_name = "GetRootUrlList_test";
  pub.CreateTopic(topic_name, desc, true);
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  SignalWatcher &signal_watcher = SignalWatcher::GetInstance();
  ASSERT_EQ(WaitUrl(signal_watcher, desc.basic_info.parent_url), true) << "Update not found";
  auto root_url_list = signal_watcher.GetRootUrlList(SignalWatcher::DEVICE_LEVEL, "");
  EXPECT_EQ(root_url_list[0].name, "GetRootUrlList_test") << "Root url not updated correctly";
}

} // namespace ecal_util
} // namespace udex
} // namespace next
