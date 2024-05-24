#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <thread>

#include <next/sdk/sdk.hpp>

#include <next/udex/subscriber/data_subscriber.hpp>

namespace {

using next::udex::extractor::DataExtractor;
using next::udex::subscriber::DataSubscriber;
using next::udex::subscriber::SubscriptionInfo;

const std::string node_name = "generic_node_subscriber";
const std::string url_1 = "url_1_subscriber";
const std::string url_2 = "url_2_subscriber";
const std::string url_3 = "url_3_subscriber";
const std::string url_4 = "url_4_subscriber";
const std::string url_5 = "url_5_subscriber";
const std::string url_6 = "url_6_subscriber";
const std::string url_7 = "url_7_subscriber";
const std::string url_8 = "url_8_subscriber";
const std::string url_9 = "url_9_subscriber";
const std::string url_10 = "url_10_subscriber";
const std::string url_11 = "url_11_subscriber";
const std::string url_12 = "url_12_subscriber";
const std::string url_13 = "url_13_subscriber";
const std::string url_14 = "url_14_subscriber";
const std::string url_15 = "url_15_subscriber";

class DataSubscriberTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  DataSubscriberTestFixture() { this->instance_name_ = "DataSubscriberTestFixture"; }
};

std::map<size_t, SubscriptionInfo> subscriptions_info;

TEST_F(DataSubscriberTestFixture, ResetInCallback) {}

TEST_F(DataSubscriberTestFixture, SubscribingToMultipleURLsInParallel) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  std::thread subscriber1 = std::thread([&]() {
    for (int i = 0; i < 1000; i++)
      subscriber.Subscribe(url_1, extractor);
  });
  std::thread subscriber2 = std::thread([&]() {
    for (int i = 0; i < 1000; i++)
      subscriber.Subscribe(url_2, extractor);
  });
  std::thread subscriber3 = std::thread([&]() {
    for (int i = 0; i < 1000; i++)
      subscriber.Subscribe(url_3, extractor);
  });
  std::thread subscriber4 = std::thread([&]() {
    for (int i = 0; i < 1000; i++)
      subscriber.Subscribe(url_4, extractor);
  });
  std::thread subscriber5 = std::thread([&]() {
    for (int i = 0; i < 1000; i++)
      subscriber.Subscribe(url_5, extractor);
  });

  subscriber1.join();
  subscriber2.join();
  subscriber3.join();
  subscriber4.join();
  subscriber5.join();

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 5000);

  subscriber.Reset();

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

TEST_F(DataSubscriberTestFixture, noSubscribe_singleUnsubscribe) {
  auto subscriber = DataSubscriber(node_name);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);

  subscriber.Unsubscribe(999);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

TEST_F(DataSubscriberTestFixture, singleSubscribe_noUnsubscribe) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 1);
  EXPECT_EQ(id_1, 1);
  EXPECT_EQ(subscriptions_info[id_1].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_1].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_1].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_1].topic.c_str(), url_1.c_str());
}

TEST_F(DataSubscriberTestFixture, multiSubscribe_noUnsubscribe) {
  // usecaes of Databridge with single plugin and mulitple subscriptions
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);
  auto id_2 = subscriber.Subscribe(url_2, extractor);
  auto id_3 = subscriber.Subscribe(url_1, extractor);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 3);
  EXPECT_EQ(id_1, 1);
  EXPECT_EQ(subscriptions_info[id_1].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_1].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_1].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_1].topic.c_str(), url_1.c_str());

  EXPECT_EQ(id_2, 2);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 2);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_2.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_2.c_str());

  EXPECT_EQ(id_3, 3);
  EXPECT_EQ(subscriptions_info[id_3].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_3].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_3].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_3].topic.c_str(), url_1.c_str());

  subscriber.Unsubscribe(id_3);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 2);
  EXPECT_EQ(subscriptions_info[id_1].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_1].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_1].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_1].topic.c_str(), url_1.c_str());

  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 2);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_2.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_2.c_str());

  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 1);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 2);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_2.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_2.c_str());
}

TEST_F(DataSubscriberTestFixture, singleSubscribe_singleUnsubscribe) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 1);

  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

TEST_F(DataSubscriberTestFixture, singleSubscribe_multipleUnsubscribe) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);

  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);

  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

TEST_F(DataSubscriberTestFixture, multipleSubscribe_noUnsubscribe) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);
  auto id_2 = subscriber.Subscribe(url_1, extractor);
  auto id_3 = subscriber.Subscribe(url_1, extractor);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 3);
  EXPECT_EQ(id_1, 1);
  EXPECT_EQ(subscriptions_info[id_1].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_1].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_1].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_1].topic.c_str(), url_1.c_str());

  EXPECT_EQ(id_2, 2);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_1.c_str());

  EXPECT_EQ(id_3, 3);
  EXPECT_EQ(subscriptions_info[id_3].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_3].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_3].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_3].topic.c_str(), url_1.c_str());
}

TEST_F(DataSubscriberTestFixture, multipleSubscribe_singleUnsubscribe) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);
  auto id_2 = subscriber.Subscribe(url_1, extractor);
  auto id_3 = subscriber.Subscribe(url_1, extractor);

  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 2);

  EXPECT_EQ(id_2, 2);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_1.c_str());

  EXPECT_EQ(id_3, 3);
  EXPECT_EQ(subscriptions_info[id_3].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_3].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_3].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_3].topic.c_str(), url_1.c_str());
}

TEST_F(DataSubscriberTestFixture, multipleSubscribe_multipleUnsubscribe) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);
  auto id_2 = subscriber.Subscribe(url_1, extractor);
  auto id_3 = subscriber.Subscribe(url_1, extractor);

  subscriber.Unsubscribe(id_1);

  subscriber.Unsubscribe(id_2);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 1);
  EXPECT_EQ(id_3, 3);
  EXPECT_EQ(subscriptions_info[id_3].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_3].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_3].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_3].topic.c_str(), url_1.c_str());

  subscriber.Unsubscribe(id_3);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

TEST_F(DataSubscriberTestFixture, singleTopic_multipleExtractors) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor1 = std::make_shared<DataExtractor>();
  auto extractor2 = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor1);
  auto id_2 = subscriber.Subscribe(url_1, extractor2);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 2);
  EXPECT_EQ(id_1, 1);
  EXPECT_EQ(subscriptions_info[id_1].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_1].data_extractor, extractor1);
  EXPECT_STREQ(subscriptions_info[id_1].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_1].topic.c_str(), url_1.c_str());

  EXPECT_EQ(id_2, 2);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor2);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_1.c_str());

  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 1);
  EXPECT_EQ(id_2, 2);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor2);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_1.c_str());

  subscriber.Unsubscribe(id_2);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

TEST_F(DataSubscriberTestFixture, multipleTopic_singleExtractor) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor);
  auto id_2 = subscriber.Subscribe(url_2, extractor);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 2);
  EXPECT_EQ(id_1, 1);
  EXPECT_EQ(subscriptions_info[id_1].ecal_subscriber_id, 1);
  EXPECT_EQ(subscriptions_info[id_1].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_1].url.c_str(), url_1.c_str());
  EXPECT_STREQ(subscriptions_info[id_1].topic.c_str(), url_1.c_str());

  EXPECT_EQ(id_2, 2);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 2);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_2.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_2.c_str());

  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  ASSERT_EQ(subscriptions_info.size(), 1);
  EXPECT_EQ(id_2, 2);
  EXPECT_EQ(subscriptions_info[id_2].ecal_subscriber_id, 2);
  EXPECT_EQ(subscriptions_info[id_2].data_extractor, extractor);
  EXPECT_STREQ(subscriptions_info[id_2].url.c_str(), url_2.c_str());
  EXPECT_STREQ(subscriptions_info[id_2].topic.c_str(), url_2.c_str());

  subscriber.Unsubscribe(id_2);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

TEST_F(DataSubscriberTestFixture, combined_test) {
  auto subscriber = DataSubscriber(node_name);
  auto extractor1 = std::make_shared<DataExtractor>();
  auto extractor2 = std::make_shared<DataExtractor>();

  auto id_1 = subscriber.Subscribe(url_1, extractor1);
  auto id_2 = subscriber.Subscribe(url_1, extractor2);
  auto id_3 = subscriber.Subscribe(url_2, extractor1);
  auto id_4 = subscriber.Subscribe(url_2, extractor2);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 4);

  subscriber.Unsubscribe(id_2);
  subscriber.Unsubscribe(id_2);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 3);

  subscriber.Unsubscribe(id_3);
  subscriber.Unsubscribe(id_3);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 2);

  subscriber.Unsubscribe(id_1);
  subscriber.Unsubscribe(id_1);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 1);

  subscriber.Unsubscribe(id_4);
  subscriber.Unsubscribe(id_4);

  subscriptions_info = subscriber.getSubscriptionsInfo();
  EXPECT_EQ(subscriptions_info.size(), 0);
}

} // namespace
