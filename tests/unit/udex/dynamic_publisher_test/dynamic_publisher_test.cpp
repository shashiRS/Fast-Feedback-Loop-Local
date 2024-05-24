#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <atomic>
#include <cmath>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include <next/sdk/sdk.hpp>

#include "udex_dynamic_publisher.h"
#include "udex_dynamic_subscriber.h"

namespace next {
namespace udex {

constexpr const int waiting_time_thread = 1500;
constexpr const int waiting_time_ecal_callback = 500;

std::string makeStringFromVec(std::vector<char> _vec) {
  std::string result = "";
  result.append(_vec.data(), _vec.size());
  return result;
}

class DynamicPublisherTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  DynamicPublisherTestFixture() { this->instance_name_ = "DynamicPublisherTestFixture"; }
};

constexpr int kWaitTimeMs = 1500;
constexpr int kWaitIncrementMs = 100;

bool WaitSubscriber(eCAL::CPublisher *pub) {
  std::cout << "waiting for subscriber ";
  int waited_time = 0;
  while (pub->GetSubscriberCount() == 0) {
    if (waited_time > kWaitTimeMs * 5) {
      std::cerr << "wait_time expired\n" << std::flush;
      return false;
    }
    waited_time += kWaitIncrementMs;
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(kWaitIncrementMs));
  }

  std::cout << "subscriber connected\n" << std::flush;
  return true;
}

std::string generateUniqueName(const std::string &in) {
  return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch())
                                 .count());
}
TEST_F(DynamicPublisherTestFixture, DynamicPublisherSubscriberMulitMessageDecoding) {

  // ######################### this will be done by the recording ########################################
  std::string desc_string = "{asdf}";
  std::vector<char> data_desc_binary;
  data_desc_binary.insert(data_desc_binary.begin(), desc_string.begin(), desc_string.end());
  // #####################################################################################################

  ecal_util::DynamicPublisher dyn_publisher(true);

  SignalDescription signal_desc;
  signal_desc.basic_info.parent_url = "ADC4xx.TrafficFusion.StaticTraffic.HeadingList";
  signal_desc.binary_data_description = data_desc_binary;

  dyn_publisher.CreateTopic("Topic.DynamicPublisherSubscriberMultiMessageDecoding", signal_desc);

  std::atomic<int> check_count = 0;
  std::atomic<int> callback_count = 0;

  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  dyn_subscriber.SubscribeToUrl("ADC4xx.TrafficFusion.StaticTraffic.HeadingList",
                                [&check_count, &callback_count](const char *,
                                                                const ecal_util::TopicReceiveMessage &msg_, long long,
                                                                long long, long long) {
                                  EXPECT_EQ(check_count.load(), msg_.meta.package_id) << "package id failed";
                                  EXPECT_EQ(makeStringFromVec(msg_.package_info), "asdf") << "package info failed";
                                  std::string checkstring = "19237aeas ";
                                  checkstring += std::to_string(msg_.meta.package_id * 100);
                                  checkstring += "count: ";
                                  checkstring += std::to_string(msg_.meta.package_id);
                                  EXPECT_EQ(makeStringFromVec(msg_.binary_blob), checkstring) << "binary blob failed";

                                  callback_count++;
                                });

  ASSERT_TRUE(WaitSubscriber(&dyn_publisher)) << "Subscriber did not connect";

  int i_count = 0;
  while (i_count < 5) {
    std::string binary_blob = "19237aeas ";
    binary_blob += std::to_string(i_count * 100);
    binary_blob += "count: ";
    binary_blob += std::to_string(i_count);

    std::string package_info = "asdf";

    ecal_util::TopicSendMessage message;
    message.package_info_size = package_info.size();
    message.package_info_in = package_info.data();
    message.binary_blob_size = binary_blob.size();
    message.binary_blob_in = binary_blob.data();
    message.meta.mode = PACKAGE_MODE_CAN;
    message.meta.mts_timestamp = 1231235123;
    message.meta.package_id = i_count;
    check_count.store(i_count);
    dyn_publisher.SendTopic(message);
    i_count++;
  }
  EXPECT_EQ(callback_count, 5);
}

TEST_F(DynamicPublisherTestFixture, DynamicPublisherTestDelayedPublishing) {

  // ######################### this will be done by the recording ########################################
  std::string desc_string = "{asdf}";
  std::vector<char> data_desc_binary;
  data_desc_binary.insert(data_desc_binary.begin(), desc_string.begin(), desc_string.end());
  // #####################################################################################################

  ecal_util::DynamicPublisher dyn_publisher(true);
  std::string ref_name = generateUniqueName("DynamicPublisherTestDelayedPublishing");

  SignalDescription signal_desc;
  signal_desc.basic_info.parent_url = "parenturl.test." + ref_name;
  signal_desc.binary_data_description = data_desc_binary;

  dyn_publisher.CreateTopic("Topic." + ref_name, signal_desc);
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));

  std::atomic<int> callback_count = 0;

  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));

  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  dyn_subscriber.SubscribeToUrl(
      "parenturl.test." + ref_name,
      [&callback_count](const char *, const ecal_util::TopicReceiveMessage &, long long, long long, long long) {
        // mock 500 ms procesing time
        std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_ecal_callback));
        callback_count++;
      });

  // wait until the subscriber connected
  ASSERT_TRUE(WaitSubscriber(&dyn_publisher)) << "Subscriber did not connect";

  dyn_publisher.SetSyncSend(true);
  for (int i_count = 0; 5 > i_count; ++i_count) {
    std::string binary_blob = "19237aeas ";
    binary_blob += std::to_string(i_count * 100);
    binary_blob += "count: ";
    binary_blob += std::to_string(i_count);

    std::string package_info = "asdf";

    ecal_util::TopicSendMessage message;
    message.package_info_size = package_info.size();
    message.package_info_in = package_info.data();
    message.binary_blob_size = binary_blob.size();
    message.binary_blob_in = binary_blob.data();
    message.meta.mode = PACKAGE_MODE_CAN;
    message.meta.mts_timestamp = 1231235123;
    message.meta.package_id = i_count;
    dyn_publisher.SendTopic(message);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  EXPECT_EQ(callback_count, 5);

  dyn_publisher.SetSyncSend(false);
  callback_count = 0;
  for (int i_count = 0; 5 > i_count; ++i_count) {
    std::string binary_blob = "19237aeas ";
    binary_blob += std::to_string(i_count * 100);
    binary_blob += "count: ";
    binary_blob += std::to_string(i_count);

    std::string package_info = "asdf";

    ecal_util::TopicSendMessage message;
    message.package_info_size = package_info.size();
    message.package_info_in = package_info.data();
    message.binary_blob_size = binary_blob.size();
    message.binary_blob_in = binary_blob.data();
    message.meta.mode = PACKAGE_MODE_CAN;
    message.meta.mts_timestamp = 1231235123;
    message.meta.package_id = i_count;
    dyn_publisher.SendTopic(message);
  }
  EXPECT_EQ(callback_count, 0);

  // wait for the callback of the subscriber
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));

  dyn_publisher.SetSyncSend(true);
  callback_count = 0;
  for (int i_count = 0; 5 > i_count; ++i_count) {
    std::string binary_blob = "19237aeas ";
    binary_blob += std::to_string(i_count * 100);
    binary_blob += "count: ";
    binary_blob += std::to_string(i_count);

    std::string package_info = "asdf";

    ecal_util::TopicSendMessage message;
    message.package_info_size = package_info.size();
    message.package_info_in = package_info.data();
    message.binary_blob_size = binary_blob.size();
    message.binary_blob_in = binary_blob.data();
    message.meta.mode = PACKAGE_MODE_CAN;
    message.meta.mts_timestamp = 1231235123;
    message.meta.package_id = i_count;
    dyn_publisher.SendTopic(message);
  }
  EXPECT_EQ(callback_count, 5);
}

TEST_F(DynamicPublisherTestFixture, DynamicPublisherGetDescription) {

  std::string ref_name = generateUniqueName("DynamicPublisherGetDescription");
  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc_in;
  desc_in.basic_info.parent_url = "parent";
  desc_in.basic_info.mode = PACKAGE_MODE_CAN;
  desc_in.basic_info.dummy = 1;
  desc_in.binary_data_description = {1, 1, 3, 5, 23, 12, 3};
  pub.CreateTopic("Topic." + ref_name, desc_in);
  auto desc = pub.GetSignalDescription();

  EXPECT_EQ(desc_in.basic_info.parent_url, desc.basic_info.parent_url) << "description failed";
  EXPECT_EQ(desc_in.basic_info.mode, desc.basic_info.mode) << "description failed";
  EXPECT_EQ(desc_in.basic_info.dummy, desc.basic_info.dummy) << "description failed";
  EXPECT_EQ(desc_in.binary_data_description, desc.binary_data_description) << "description failed";
}
TEST_F(DynamicPublisherTestFixture, DynamicPublisherGetEmptyDescription) {

  ecal_util::DynamicPublisher pub(true);
  std::string ref_name = generateUniqueName("DynamicPublisherGetEmptyDescription");
  pub.CreateTopic("Topic." + ref_name, {});
  EXPECT_EQ(true, true) << "empty description crashed";
  auto desc = pub.GetSignalDescription();

  EXPECT_EQ(desc.basic_info.parent_url, "") << "empty string failed";
  EXPECT_EQ(desc.basic_info.dummy, 0) << "empty int failed";
  EXPECT_EQ(desc.basic_info.mode, PACKAGE_MODE_NOTSET) << "empty mode failed";
  ASSERT_EQ(desc.binary_data_description.size(), 0) << "binary desc failed";
}

TEST_F(DynamicPublisherTestFixture, DynamicPublisherCheckResetEmptyDescription) {

  ecal_util::DynamicPublisher pub(true);
  SignalDescription desc_in;
  std::string ref_name = generateUniqueName("DynamicPublisherCheckResetEmptyDescription");
  desc_in.basic_info.parent_url = "parent";
  desc_in.basic_info.mode = PACKAGE_MODE_CAN;
  desc_in.basic_info.dummy = 1;
  desc_in.binary_data_description = {1, 1, 3, 5, 23, 12, 3};
  pub.CreateTopic("Topic." + ref_name, desc_in);
  auto desc = pub.GetSignalDescription();

  EXPECT_EQ(desc_in.basic_info.parent_url, desc.basic_info.parent_url) << "description failed";
  EXPECT_EQ(desc_in.basic_info.mode, desc.basic_info.mode) << "description failed";
  EXPECT_EQ(desc_in.basic_info.dummy, desc.basic_info.dummy) << "description failed";
  EXPECT_EQ(desc_in.binary_data_description, desc.binary_data_description) << "description failed";

  pub.ResetTopic();
  desc = pub.GetSignalDescription();
  EXPECT_EQ(desc.basic_info.parent_url, "") << "empty string failed";
  EXPECT_EQ(desc.basic_info.dummy, 0) << "empty int failed";
  EXPECT_EQ(desc.basic_info.mode, PACKAGE_MODE_NOTSET) << "empty mode failed";
  ASSERT_EQ(desc.binary_data_description.size(), 0) << "binary desc failed";
}

TEST_F(DynamicPublisherTestFixture, DynamicPublisherCheckNoInfoPackage) {

  std::string test_url = "testurlCheckNoInfoPackage";

  ecal_util::DynamicPublisher pub(true);
  pub.CreateTopic("Topic.DynamicPublisherCheckNoInfoPackage", {{"testurlCheckNoInfoPackage"}, {}});

  std::atomic<int> received = 0;

  ecal_util::DynamicSubscriber dyn_subscriber(true);
  // wait for publisher to be in ecalLayer
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));

  auto check_subscription = dyn_subscriber.SubscribeToUrl(
      "testurlCheckNoInfoPackage", [&received](const char *, const ecal_util::TopicReceiveMessage &, long long,
                                               long long, long long) { received++; });
  EXPECT_EQ(check_subscription, true) << "Subscription did not work";
  // wait for subscripton to be active
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));

  std::vector<char> test_vec = {'a', 'b', 'c', '3', '5'};
  ecal_util::TopicSendMessage test_false;
  test_false.package_info_size = 0;
  test_false.package_info_in = test_vec.data();
  test_false.binary_blob_in = test_vec.data();
  test_false.binary_blob_size = 5;
  pub.SendTopic(test_false);

  // wait for callback to be called
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_ecal_callback * 3));
  EXPECT_EQ(received, 1) << "Dynamic data with zero size info package was not received";
}

TEST_F(DynamicPublisherTestFixture, DynamicPublisherCheckNoBinaryData) {

  std::string test_url = "testurlNoBinaryData";

  ecal_util::DynamicPublisher pub(true);
  EXPECT_EQ(pub.CreateTopic("Topic.DynamicPublisherCheckNoBinaryData", {{test_url}, {}}), true);
  // wait for publisher to be in ecalLayer
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  std::atomic<int> received = 0;
  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  // wait for publisher to be in ecalLayer
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  auto check_subscription =
      dyn_subscriber.SubscribeToUrl(test_url, [&received](const char *, const ecal_util::TopicReceiveMessage &,
                                                          long long, long long, long long) { received++; });
  EXPECT_EQ(check_subscription, true) << "Subscription did not work";
  // wait for subscripton to be active
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));

  std::vector<char> test_vec = {'a', 'b', 'c', '3', '5'};
  ecal_util::TopicSendMessage test_false;
  test_false.package_info_size = 5;
  test_false.package_info_in = test_vec.data();
  test_false.binary_blob_in = test_vec.data();
  test_false.binary_blob_size = 0;
  pub.SendTopic(test_false);

  // wait for callback to be called
  std::this_thread::sleep_for(std::chrono::milliseconds(3 * waiting_time_ecal_callback));
  EXPECT_EQ(received, 1) << "Dynamic data with zero size info package was not received";
}

TEST_F(DynamicPublisherTestFixture, DynamicPublisherReset) {

  // ######################### this will be done by the recording ########################################
  std::string desc_string = "{asdf}";
  std::vector<char> data_desc_binary;
  data_desc_binary.insert(data_desc_binary.begin(), desc_string.begin(), desc_string.end());
  // #####################################################################################################

  auto dyn_publisher = new ecal_util::DynamicPublisher(true);

  SignalDescription signal_desc;
  signal_desc.basic_info.parent_url = "ADC4xx.TrafficFusion.StaticTraffic.HeadingList";
  signal_desc.binary_data_description = data_desc_binary;

  dyn_publisher->CreateTopic("Topic.DynamicPublisherReset", signal_desc);

  std::atomic<int> check_count;
  std::atomic<int> callback_count = 0;

  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);

  dyn_subscriber.SubscribeToUrl("ADC4xx.TrafficFusion.StaticTraffic.HeadingList",
                                [&check_count, &callback_count](const char *,
                                                                const ecal_util::TopicReceiveMessage &msg_, long long,
                                                                long long, long long) {
                                  EXPECT_EQ(check_count.load(), msg_.meta.package_id) << "package id failed";
                                  EXPECT_EQ(makeStringFromVec(msg_.package_info), "asdf") << "package info failed";
                                  std::string checkstring = "19237aeas ";
                                  checkstring += std::to_string(msg_.meta.package_id * 100);
                                  checkstring += "count: ";
                                  checkstring += std::to_string(msg_.meta.package_id);
                                  EXPECT_EQ(makeStringFromVec(msg_.binary_blob), checkstring) << "binary blob failed";
                                  callback_count++;
                                });

  ASSERT_TRUE(WaitSubscriber(dyn_publisher)) << "Subscriber did not connect";

  int i_count = 0;
  while (i_count < 2) {
    std::string binary_blob = "19237aeas ";
    binary_blob += std::to_string(i_count * 100);
    binary_blob += "count: ";
    binary_blob += std::to_string(i_count);

    std::string package_info = "asdf";

    ecal_util::TopicSendMessage message;
    message.package_info_size = package_info.size();
    message.package_info_in = package_info.data();
    message.binary_blob_size = binary_blob.size();
    message.binary_blob_in = binary_blob.data();
    message.meta.mode = PACKAGE_MODE_CAN;
    message.meta.mts_timestamp = 1231235123;
    message.meta.package_id = i_count;
    check_count.store(i_count);
    dyn_publisher->SendTopic(message);
    i_count++;
  }
  ASSERT_EQ(callback_count, 2);

  ASSERT_TRUE(dyn_publisher->ResetTopic());
  SignalDescription signal_desc1;
  signal_desc1.basic_info.parent_url = "ADC4xx.TrafficFusion.StaticTraffic.HeadingList1";
  signal_desc1.binary_data_description = data_desc_binary;

  dyn_publisher->CreateTopic("Topic.DynamicPublisherReset1", signal_desc1, true);
  // wait for publisher to be in ecalLayer

  std::atomic<int> check_count_1;
  std::atomic<int> callback_call_count = 0;
  auto dyn_subscriber1 = ecal_util::DynamicSubscriber(true);
  // wait for publisher to be in ecalLayer
  dyn_subscriber1.SubscribeToUrl("ADC4xx.TrafficFusion.StaticTraffic.HeadingList1",
                                 [&check_count_1, &callback_call_count](const char *,
                                                                        const ecal_util::TopicReceiveMessage &msg_,
                                                                        long long, long long, long long) {
                                   EXPECT_EQ(check_count_1.load(), msg_.meta.package_id) << "package id failed";
                                   EXPECT_EQ(makeStringFromVec(msg_.package_info), "asdf") << "package info failed";
                                   std::string checkstring = "17aeas 13fvswe";
                                   checkstring += std::to_string(msg_.meta.package_id * 101);
                                   checkstring += "count: ";
                                   checkstring += std::to_string(msg_.meta.package_id);
                                   EXPECT_EQ(makeStringFromVec(msg_.binary_blob), checkstring) << "binary blob failed";
                                   callback_call_count++;
                                 });

  EXPECT_TRUE(WaitSubscriber(dyn_publisher)) << "Subscriber did not connect";

  int i_count_1 = 0;
  while (i_count_1 < 2) {
    std::string binary_blob = "17aeas 13fvswe";
    binary_blob += std::to_string(i_count_1 * 101);
    binary_blob += "count: ";
    binary_blob += std::to_string(i_count_1);

    std::string package_info = "asdf";

    ecal_util::TopicSendMessage message;
    message.package_info_size = package_info.size();
    message.package_info_in = package_info.data();
    message.binary_blob_size = binary_blob.size();
    message.binary_blob_in = binary_blob.data();
    message.meta.mode = PACKAGE_MODE_CAN;
    message.meta.mts_timestamp = 1223;
    message.meta.package_id = i_count_1;
    check_count_1.store(i_count_1);
    dyn_publisher->SendTopic(message);
    i_count_1++;
  }
  if (dyn_publisher)
    delete dyn_publisher;
  EXPECT_EQ(callback_call_count, 2);
}

namespace test {
class PubTest : public ecal_util::DynamicPublisher {
public:
  PubTest(const std::string & /* pubName */, bool intra_process) : ecal_util::DynamicPublisher(intra_process){};
  ~PubTest(){};

  std::string GetTypeTestDummy(void) { return (ecal_util::DynamicPublisher::GetTypeName()); }
};
} // namespace test

TEST_F(DynamicPublisherTestFixture, GetTypeTestEmptyParentUrl) {
  test::PubTest pub("asdf", true);
  pub.CreateTopic("asdf", {{""}, {}});
  EXPECT_EQ("UdexDynamicTopic_NotDefined", pub.GetTypeTestDummy());
}

TEST_F(DynamicPublisherTestFixture, GetTypeTestNonEmptyParentUrl) {
  test::PubTest pub("asdf", true);
  pub.CreateTopic("asdf", {{"TestUrl"}, {}});
  const std::string type = pub.GetTypeTestDummy();
  std::string type_name = type.substr(0, type.rfind("<"));
  EXPECT_EQ("next:TestUrl", type_name);
  // the last part of the type is the ts in us when the publisher has been created
  std::regex rgx("<(\\d+)>");
  std::smatch match_ts;
  ASSERT_EQ(std::regex_search(type.begin(), type.end(), match_ts, rgx), true);
}

TEST_F(DynamicPublisherTestFixture, MetaInfoTest) {

  // ######################### this will be done by the recording ########################################
  std::string desc_string = "{asdf}";
  std::vector<char> data_desc_binary;
  data_desc_binary.insert(data_desc_binary.begin(), desc_string.begin(), desc_string.end());
  // #####################################################################################################

  auto dyn_publisher = new ecal_util::DynamicPublisher(true);

  SignalDescription signal_desc;
  signal_desc.basic_info.parent_url = "ADC4xx.TrafficFusion.StaticTraffic.HeadingList";
  signal_desc.binary_data_description = data_desc_binary;

  dyn_publisher->CreateTopic("Topic.DynamicPublisherSubscriberMulitMessageDecoding", signal_desc);

  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  MetaInfo test_meta_info;
  constexpr const int meta_info_size = 24;
  std::atomic<bool> callback_hit = false;
  dyn_subscriber.SubscribeToUrl(
      "ADC4xx.TrafficFusion.StaticTraffic.HeadingList",
      [&test_meta_info, &meta_info_size, &callback_hit](const char *, const ecal_util::TopicReceiveMessage &msg_,
                                                        long long, long long, long long) {
        EXPECT_EQ(meta_info_size, sizeof(MetaInfo)) << "MetaInfo size changed ,Test not updates";
        EXPECT_EQ(makeStringFromVec(msg_.package_info), "asdf") << "package info failed";
        std::string checkstring = "19237aeas ";
        EXPECT_EQ(makeStringFromVec(msg_.binary_blob), checkstring) << "binary blob failed";
        EXPECT_EQ(msg_.meta.mts_timestamp, test_meta_info.mts_timestamp) << "meta info read failed";
        EXPECT_EQ(msg_.meta.format_type, test_meta_info.format_type) << "meta info read failed";
        EXPECT_EQ(msg_.meta.cycle_state, test_meta_info.cycle_state) << "meta info read failed";
        EXPECT_EQ(msg_.meta.dynamic_package, test_meta_info.dynamic_package) << "meta info read failed";
        EXPECT_EQ(msg_.meta.package_id, test_meta_info.package_id) << "meta info read failed";
        EXPECT_EQ(msg_.meta.mode, test_meta_info.mode) << "meta info read failed";
        callback_hit = true;
      });

  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  std::string binary_blob = "19237aeas ";
  std::string package_info = "asdf";
  ecal_util::TopicSendMessage message;
  message.package_info_size = package_info.size();
  message.package_info_in = package_info.data();
  message.binary_blob_size = binary_blob.size();
  message.binary_blob_in = binary_blob.data();
  /*Fill Meta Data*/
  test_meta_info.mts_timestamp = 1231235123;
  test_meta_info.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_CAN;
  test_meta_info.cycle_state = 100;
  test_meta_info.dynamic_package = true;
  test_meta_info.package_id = 1010;
  test_meta_info.mode = PACKAGE_MODE_CAN;

  memcpy(&message.meta, &test_meta_info, sizeof(MetaInfo));
  ASSERT_TRUE(WaitSubscriber(dyn_publisher)) << "Subscriber did not connect";

  dyn_publisher->SendTopic(message);
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  EXPECT_TRUE(callback_hit);
  delete dyn_publisher;
}

} // namespace udex
} // namespace next
