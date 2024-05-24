#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <string>
#include <thread>
#include <vector>

#include <next/sdk/sdk.hpp>

#include "udex_dynamic_publisher.h"
#include "udex_dynamic_subscriber.h"

namespace next {
namespace udex {
namespace ecal_util {

constexpr const int waiting_time_thread = 2500;
constexpr int kWaitTimeMs = 1500;
constexpr int kWaitIncrementMs = 100;

class DynamicSubscriberTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  DynamicSubscriberTestFixture() { this->instance_name_ = "DynamicSubscriberTestFixture"; }
};

std::string makeStringFromVec(std::vector<char> _vec) {
  std::string result = "";
  result.append(_vec.data(), _vec.size());
  return result;
}

std::shared_ptr<ecal_util::DynamicPublisher> CreateTestPublisher(std::string url) {
  // ######################### this will be done by the recording ########################################
  std::string desc_string = "{asdf}";
  std::vector<char> data_desc_binary;
  data_desc_binary.insert(data_desc_binary.begin(), desc_string.begin(), desc_string.end());
  // #####################################################################################################

  auto dyn_publisher = std::make_shared<ecal_util::DynamicPublisher>(true);

  SignalDescription signal_desc;
  signal_desc.basic_info.parent_url = url;
  signal_desc.binary_data_description = data_desc_binary;

  dyn_publisher->CreateTopic(url + "changeTopicName", signal_desc, true);
  return dyn_publisher;
}

TEST_F(DynamicSubscriberTestFixture, DynamicSubscriberCheckDescriptionEmtpyWhenNoSubscription) {
  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  bool found;
  SignalDescription description = dyn_subscriber.GetSignalDescription(found);
  EXPECT_EQ(found, false) << "SignalDescription found without subscription";
  EXPECT_EQ(description.basic_info.parent_url, "") << "SignalDescription found without subscription";
  EXPECT_EQ(description.basic_info.mode, PACKAGE_MODE_NOTSET) << "SignalDescription found without subscription";
  EXPECT_EQ(description.basic_info.dummy, 0) << "SignalDescription found without subscription";
  EXPECT_EQ(description.binary_data_description.size(), 0) << "SignalDescription found without subscription";
}

TEST_F(DynamicSubscriberTestFixture, DynamicSubscribercheckSubscriptionActiveFalseWhenNoSubscription) {
  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  EXPECT_EQ(dyn_subscriber.checkSubscriptionActive(), false) << "subscripition reported active without subscription";
}

TEST_F(DynamicSubscriberTestFixture, DynamicSubscribercheckReset) {
  std::string test_url = "Test.Url.1.DynamicSubscribercheckReset_url";
  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  auto sub_bool = dyn_subscriber.SubscribeToUrl(
      test_url, [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  EXPECT_FALSE(sub_bool) << "subscriptoin positive wiht no subscription";
  EXPECT_EQ(dyn_subscriber.getSubscriptionUrl(), test_url);
  EXPECT_EQ(dyn_subscriber.Reset(), true);
  EXPECT_EQ(dyn_subscriber.getSubscriptionUrl(), "");
}

TEST_F(DynamicSubscriberTestFixture, DynamicSubscriberCheckNoSubscriptionReturn) {
  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  auto sub_bool =
      dyn_subscriber.SubscribeToUrl("DynamicSubscriberCheckNoSubscriptionReturn_url",
                                    [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  EXPECT_EQ(sub_bool, false) << "subscriptoin positive wiht no subscription";
  EXPECT_EQ(dyn_subscriber.checkSubscriptionActive(), false) << "subscripition reported active without subscription";
}

TEST_F(DynamicSubscriberTestFixture, DynamicSubscribercheckSubscriptionActiveWithDelay) {
  std::string test_url = "Test.Url.1.DynamicSubscribercheckSubscriptionActiveWithDelay_url";
  auto dyn_pub = CreateTestPublisher(test_url);

  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));

  auto subscribed = dyn_subscriber.SubscribeToUrl(
      test_url, [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  EXPECT_EQ(subscribed, true) << "subscripition reported not active";
  EXPECT_EQ(dyn_subscriber.checkSubscriptionActive(), true) << "subscripition reported not active";
}

TEST_F(DynamicSubscriberTestFixture, DynamicSubscribercheckSubscriptionActiveWithCallback) {
  std::string test_url = "Test.Url.1.Test.Url.1.DynamicSubscribercheckSubscriptionActiveWithCallback";
  auto dyn_pub = CreateTestPublisher(test_url);

  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  auto subscribed = dyn_subscriber.SubscribeToUrl(
      test_url, [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  EXPECT_EQ(subscribed, true) << "subscripition reported not active";

  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  bool test_sub = dyn_subscriber.checkSubscriptionActive();
  EXPECT_EQ(test_sub, true) << "subscripition reported not active";
}

TEST_F(DynamicSubscriberTestFixture, DISABLED_DynamicSubscribercheckSubscriptionDeactivated) {
  std::string test_url = "Test.Url.1.Test.Url.2.DynamicSubscribercheckSubscriptionDeactivated";

  auto dyn_subscriber = ecal_util::DynamicSubscriber(true);
  EXPECT_EQ(dyn_subscriber.checkSubscriptionActive(), false) << "subscripition reported active without subscription";

  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  auto sub_valid = dyn_subscriber.SubscribeToUrl(
      test_url, [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  EXPECT_EQ(sub_valid, false) << "subscripition not reported active";
  EXPECT_EQ(dyn_subscriber.checkSubscriptionActive(), false) << "subscripition reported active";

  auto dyn_pub = CreateTestPublisher(test_url);
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  EXPECT_EQ(dyn_subscriber.checkSubscriptionActive(), true) << "subscripition reported not active";

  dyn_pub->ResetTopic();
  std::this_thread::sleep_for(std::chrono::milliseconds(100000));
  EXPECT_EQ(dyn_subscriber.checkSubscriptionActive(), false) << "subscripition reported active without subscription";
}

namespace test {
class SubTest : public DynamicSubscriber {
public:
  SubTest(bool intra_process) : DynamicSubscriber(intra_process){};
  ~SubTest(){};

  eCAL::SDataTypeInformation GetTopicInfoTestDummy(void) {
    return (ecal_util::DynamicSubscriber::GetDataTypeInformation());
  }
  size_t GetPublisherCountDummy() { return (ecal_util::DynamicSubscriber::GetPublisherCount()); }
  bool WaitPublisher() {
    std::cout << "waiting for publisher ";
    int waited_time = 0;
    while (ecal_util::DynamicSubscriber::GetPublisherCount() == 0) {
      if (waited_time > kWaitTimeMs * 5) {
        std::cerr << "wait_time expired\n" << std::flush;
        return false;
      }
      waited_time += kWaitIncrementMs;
      std::cout << "." << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(kWaitIncrementMs));
    }

    std::cout << "publisher connected\n" << std::flush;
    return true;
  }
};
} // namespace test

TEST_F(DynamicSubscriberTestFixture, GetTypeTestEmptyParentUrl) {
  std::string test_url = "";
  auto dyn_pub = CreateTestPublisher(test_url);
  auto dyn_sub = test::SubTest(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  eCAL::SDataTypeInformation topic_info = dyn_sub.GetTopicInfoTestDummy();
  EXPECT_EQ("UdexDynamicTopic_NotDefined", topic_info.name);
}

TEST_F(DynamicSubscriberTestFixture, GetTypeTestNonEmptyParentUrl) {

  std::string test_url = "GetTypeTestNonEmptyParentUrl";
  auto dyn_pub = CreateTestPublisher(test_url);
  auto dyn_sub = test::SubTest(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(waiting_time_thread));
  dyn_sub.SubscribeToUrl(test_url, [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  eCAL::SDataTypeInformation topic_info = dyn_sub.GetTopicInfoTestDummy();
  EXPECT_EQ("UdexDynamicTopic_GetTypeTestNonEmptyParentUrl", topic_info.name);
}

TEST_F(DynamicSubscriberTestFixture, UrlUnregistrationTest) {

  std::string test_url = "UrlUnregistrationTest";

  test::SubTest *dyn_sub = new test::SubTest(true);
  test::SubTest *dyn_sub1 = new test::SubTest(true);
  dyn_sub->SubscribeToUrl(test_url, [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  dyn_sub1->SubscribeToUrl(test_url, [](const char *, const TopicReceiveMessage &, long long, long long, long long) {});
  delete dyn_sub;
  auto dyn_pub = CreateTestPublisher(test_url);

  ASSERT_TRUE(dyn_sub1->WaitPublisher()) << "Publisher did not connect";
  // no crash due to removed hook of dyn_sub expected
  eCAL::SDataTypeInformation topic_info = dyn_sub1->GetTopicInfoTestDummy();
  EXPECT_EQ("UdexDynamicTopic_" + test_url, topic_info.name);
  delete dyn_sub1;
}

} // namespace ecal_util
} // namespace udex
} // namespace next
