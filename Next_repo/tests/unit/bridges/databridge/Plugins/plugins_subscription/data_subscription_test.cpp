/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_unittest_test.cpp
 *  @brief    Testing some mock functionalities
 */

#include <cip_test_support/gtest_reporting.h>

#include <exception>
#include <thread>

#include <ecal/ecal.h>
#include <ecal/msg/string/publisher.h>

#include <next/sdk/sdk.hpp>

#include <next/udex/subscriber/data_subscriber.hpp>

#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>

#include "data_manager_mock.hpp"
#include "data_subscription_plugin.hpp"

#include "signal_explorer_mock.hpp"

#define CALLBACK_NOT_CALLED 0
#define CALLBACK_CALLED_ONCE 1
#define CALLBACK_CALLED_TWICE 2
#define CALLBACK_CALLED_3_TIMES 3
#define CALLBACK_CALLED_4_TIMES 4
#define CALLBACK_CALLED_5_TIMES 5

#define RECEIVED_DATA_VECTOR_SIZE_1 1
#define RECEIVED_DATA_VECTOR_SIZE_2 2
#define RECEIVED_DATA_VECTOR_SIZE_3 3
#define RECEIVED_DATA_VECTOR_SIZE_4 4
#define RECEIVED_DATA_VECTOR_SIZE_5 5

// giving the subscriber some time to recognize that there is now a publisher
constexpr int subscription_wait_time_ms = 3000;
// giving ecal time to send the data
constexpr int after_send_delay_ms = 1000;
constexpr int timeout_ms = 2000;
//! Function for create a publisher
/*!
 * This function will create publishers based on topics and payloads given as parameter.
 */
bool publishATopic(std::string topic, std::string payload) {
  std::atomic<bool> success(false);
  auto pub_thread = std::thread([&topic, &payload, &success] {
    eCAL::string::CPublisher<std::string> pub(topic);
    eCAL::Util::EnableLoopback(true);

    std::cout << "waiting for subscription";
    int waited_time = 0;
    const int wait_time = 100;
    while (!pub.IsSubscribed()) {
      if (waited_time > subscription_wait_time_ms) {
        std::cerr << "there is no subscription\n" << std::flush;
        return;
      }
      waited_time += wait_time;
      std::cout << "." << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
    }
    std::cout << "subscriber connected\n" << std::flush;
    const size_t n_bytes = pub.Send(payload);
    if (n_bytes != payload.size()) {
      std::cerr << "failed to send data!\n" << std::flush;
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(after_send_delay_ms));
    success = true;
  });

  pub_thread.join();
  return success;
}

class PluginSubscriptionTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  PluginSubscriptionTestFixture() { this->instance_name_ = "PluginSubscriptionTest"; }
};

//! Testing subscribing to a single URL without a publisher
/*!
 * This test is demonstrating how to subscribe to a topic without receiveing anything, because no publisher exists.
 */

// TODO: enable the test when SIMEN-6657 is resolved
TEST_F(PluginSubscriptionTestFixture, DISABLED_SubscribingToASingleURLWithoutPublisher) {
  // ------------------------ INIT START

  std::atomic<int> no_of_callback_called = 0;

  auto plugin = std::make_shared<next::plugins::DataSubscriptionTestPlugin>("dummy");

  // actual interface to be tested !!!!
  auto udex_subscriber = std::make_shared<next::udex::subscriber::DataSubscriber>("asdf");
  // mock intreface -> not tested
  auto dm = next::databridge::data_manager::CreateDataManager();
  // mock interface -> not tested

  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_mock =
      std::shared_ptr<next::test::SignalExplorerMock>();
  plugin->connectPlugin(udex_subscriber, dm, signal_explorer_mock);
  plugin->init();

  plugin->subscriberManualy("adsf", no_of_callback_called);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  EXPECT_EQ(no_of_callback_called, CALLBACK_NOT_CALLED);
  EXPECT_EQ(plugin->data_received.empty(), true);
}

//! Testing subscribing to multiple URL and receiving data from publisher
/*!
 * This test is demonstrating how to subscribe to multiple topics and then receiving several data from publishers and
 * thus callback function will be called.
 */

// TODO: enable the test when SIMEN-6657 is resolved
TEST_F(PluginSubscriptionTestFixture, DISABLED_SubscribingToMultipleURLs) {
  std::atomic<int> no_of_callback_called = 0;

  auto plugin = std::make_shared<next::plugins::DataSubscriptionTestPlugin>("dummy2");
  auto udex_subscriber = std::make_shared<next::udex::subscriber::DataSubscriber>("asdf");
  auto dm = next::databridge::data_manager::CreateDataManager();
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_mock =
      std::shared_ptr<next::test::SignalExplorerMock>();

  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  plugin->connectPlugin(udex_subscriber, dm, signal_explorer_mock);
  plugin->init();

  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  plugin->subscriberManualy("url1", no_of_callback_called);
  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  plugin->subscriberManualy("url2", no_of_callback_called);
  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  plugin->subscriberManualy("url3", no_of_callback_called);
  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  ASSERT_TRUE(publishATopic("url1", "payload1")) << "sending data failed";
  ASSERT_EQ(no_of_callback_called, CALLBACK_CALLED_ONCE);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_1);
  EXPECT_STREQ(plugin->data_received[0].c_str(), "payload1");

  ASSERT_TRUE(publishATopic("url2", "payload2")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_TWICE);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_2);
  EXPECT_STREQ(plugin->data_received[1].c_str(), "payload2");

  ASSERT_TRUE(publishATopic("url3", "payload3")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_3_TIMES);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_3);
  EXPECT_STREQ(plugin->data_received[2].c_str(), "payload3");
}

//! Testing unsubscribing from same URLs
/*!
 * This test is demonstrating how to unsubscribe from same topics and how callback functions are not called anymore
 * after unsubscribing.
 */

// TODO: enable the test when SIMEN-6657 is resolved
TEST_F(PluginSubscriptionTestFixture, DISABLED_UnsubscribeFromSameURLs) {
  std::atomic<int> no_of_callback_called = 0;

  auto plugin = std::make_shared<next::plugins::DataSubscriptionTestPlugin>("dummy3");
  auto udex_subscriber = std::make_shared<next::udex::subscriber::DataSubscriber>("asdf");
  auto dm = next::databridge::data_manager::CreateDataManager();
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_mock =
      std::shared_ptr<next::test::SignalExplorerMock>();
  plugin->connectPlugin(udex_subscriber, dm, signal_explorer_mock);
  plugin->init();

  size_t id = plugin->subscriberManualy("url_1_for_unsubscribe", no_of_callback_called);
  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  size_t id2 = plugin->subscriberManualy("url_1_for_unsubscribe", no_of_callback_called);
  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  ASSERT_TRUE(publishATopic("url_1_for_unsubscribe", "payload1")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_TWICE);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_2);
  EXPECT_STREQ(plugin->data_received[0].c_str(), "payload1");
  EXPECT_STREQ(plugin->data_received[1].c_str(), "payload1");

  ASSERT_TRUE(publishATopic("url_1_for_unsubscribe", "payload2")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_4_TIMES);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_4);
  EXPECT_STREQ(plugin->data_received[2].c_str(), "payload2");
  EXPECT_STREQ(plugin->data_received[3].c_str(), "payload2");

  plugin->removeSubscription(id);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  ASSERT_TRUE(publishATopic("url_1_for_unsubscribe", "payload1")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_5_TIMES);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_5);
  EXPECT_STREQ(plugin->data_received[4].c_str(), "payload1");

  plugin->removeSubscription(id2);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  EXPECT_FALSE(publishATopic("url_1_for_unsubscribe", "payload1"))
      << "should return false, there should not be a subscription anymore";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_5_TIMES);
  EXPECT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_5);
}

//! Testing unsubscribing from multiple different URLs
/*!
 * This test is demonstrating how to unsubscribe from different topics and how callback functions are not called anymore
 * after unsubscribing.
 */

// TODO: enable the test when SIMEN-6657 is resolved
TEST_F(PluginSubscriptionTestFixture, DISABLED_UnsubscribeFromDifferentURLs) {
  std::atomic<int> no_of_callback_called = 0;

  auto plugin = std::make_shared<next::plugins::DataSubscriptionTestPlugin>("dummy4");
  auto udex_subscriber = std::make_shared<next::udex::subscriber::DataSubscriber>("asdf");
  auto dm = next::databridge::data_manager::CreateDataManager();
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_mock =
      std::shared_ptr<next::test::SignalExplorerMock>();
  plugin->connectPlugin(udex_subscriber, dm, signal_explorer_mock);
  plugin->init();

  size_t id = plugin->subscriberManualy("url_1_for_unsubscribe", no_of_callback_called);
  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  size_t id2 = plugin->subscriberManualy("url_2_for_unsubscribe", no_of_callback_called);
  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  ASSERT_TRUE(publishATopic("url_1_for_unsubscribe", "payload1")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_ONCE);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_1);
  EXPECT_STREQ(plugin->data_received[0].c_str(), "payload1");

  ASSERT_TRUE(publishATopic("url_2_for_unsubscribe", "payload2")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_TWICE);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_2);
  EXPECT_STREQ(plugin->data_received[1].c_str(), "payload2");

  plugin->removeSubscription(id);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  EXPECT_FALSE(publishATopic("url_1_for_unsubscribe", "payload1"))
      << "should return false, there should not be a subscription anymore";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_TWICE);
  EXPECT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_2);

  ASSERT_TRUE(publishATopic("url_2_for_unsubscribe", "payload2")) << "sending data failed";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_3_TIMES);
  ASSERT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_3);
  EXPECT_STREQ(plugin->data_received[2].c_str(), "payload2");

  plugin->removeSubscription(id2);
  std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

  EXPECT_FALSE(publishATopic("url_1_for_unsubscribe", "payload1"))
      << "should return false, there should not be a subscription anymore";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_3_TIMES);
  EXPECT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_3);

  EXPECT_FALSE(publishATopic("url_2_for_unsubscribe", "payload2"))
      << "should return false, there should not be a subscription anymore";
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_3_TIMES);
  EXPECT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_3);
}

//! Testing subscribing to multplie URLs at the same time
/*!
 * This test is demonstrating how to subscribe to multiple topics at the same time and then receiving several data from
 * publishers and thus callback functions will be called.
 */

// TODO: enable the test when SIMEN-6657 is resolved
TEST_F(PluginSubscriptionTestFixture, DISABLED_SubscribingToMultipleURLsInParallel) {

  std::atomic<int> no_of_callback_called = 0;
  auto plugin = std::make_shared<next::plugins::DataSubscriptionTestPlugin>("plugin1");

  auto udex_subscriber = std::make_shared<next::udex::subscriber::DataSubscriber>("asdf");
  auto dm = next::databridge::data_manager::CreateDataManager();
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_mock =
      std::shared_ptr<next::test::SignalExplorerMock>();
  plugin->connectPlugin(udex_subscriber, dm, signal_explorer_mock);
  plugin->init();

  try {
    std::thread subscriber1 = std::thread([&]() { plugin->subscriberManualy("url1", no_of_callback_called); });
    std::thread subscriber2 = std::thread([&]() { plugin->subscriberManualy("url2", no_of_callback_called); });
    std::thread subscriber3 = std::thread([&]() { plugin->subscriberManualy("url3", no_of_callback_called); });
    std::thread subscriber4 = std::thread([&]() { plugin->subscriberManualy("url3", no_of_callback_called); });

    subscriber1.join();
    subscriber2.join();
    subscriber3.join();
    subscriber4.join();
  } catch (std::exception &e) {
    std::cout << e.what() << '\n';
  }

  EXPECT_EQ(plugin->data_received.empty(), true);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  ASSERT_TRUE(publishATopic("url1", "payload1")) << "sending data failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_ONCE);
  EXPECT_STREQ(plugin->data_received[0].c_str(), "payload1");
  EXPECT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_1);

  ASSERT_TRUE(publishATopic("url2", "payload2")) << "sending data failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_TWICE);
  EXPECT_STREQ(plugin->data_received[1].c_str(), "payload2");
  EXPECT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_2);

  ASSERT_TRUE(publishATopic("url3", "payload1")) << "sending data failed";
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_EQ(no_of_callback_called, CALLBACK_CALLED_4_TIMES);
  EXPECT_STREQ(plugin->data_received[2].c_str(), "payload1");
  EXPECT_STREQ(plugin->data_received[3].c_str(), "payload1");
  EXPECT_EQ(plugin->data_received.size(), RECEIVED_DATA_VECTOR_SIZE_4);
}
