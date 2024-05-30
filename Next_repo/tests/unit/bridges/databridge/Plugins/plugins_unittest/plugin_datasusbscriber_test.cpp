/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_datasusbscriber_test.cpp
 *  @brief    Testing some mock functionalities
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>

#include "plugin_mock.hpp"
#include "unittest_plugin_helper.hpp"

#include <atomic>

using next::test::PluginTest;

const std::string MOCK_URL("MockUrl");
TYPED_TEST_CASE(PluginTest, next::plugins::MockPlugin);

void test_callback(const std::string, const next::bridgesdk::ChannelMessagePointers *ptr, char *test) {
  if (ptr->size != 0)
    *test = ((char *)ptr->data)[0];
}

TYPED_TEST(PluginTest, subscribeUrl_check_return_value) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataSubscriber Subscriber(Plugin.get());

  std::string url_name = "test1";
  size_t returnvalue = 0u;

  /*Test 1.1*/
  returnvalue = Subscriber.subscribeUrl(url_name, {}, {});
  EXPECT_EQ(returnvalue, 2u) << "subscriber id not incremented after subscription";

  /*Test 1.2 to check for incremental value of subscription id*/
  url_name = "test2";
  returnvalue = Subscriber.subscribeUrl(url_name, {}, {});
  ASSERT_EQ(returnvalue, 3u);
}

TYPED_TEST(PluginTest, subscribeUrl_callbacktest) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataSubscriber Subscriber(Plugin.get());

  std::string url_name = "test1";
  size_t returnvalue;
  char test;
  const std::string test_str = "test_callback";
  next::bridgesdk::ChannelMessagePointers test_Channel_Msg;
  test_Channel_Msg.channel_name = "ch1";
  test_Channel_Msg.data = "A";
  test_Channel_Msg.size = 10;
  test_Channel_Msg.time = 100;
  auto CallBackTest = std::bind(&test_callback, std::placeholders::_1, std::placeholders::_2, &test);
  returnvalue = Subscriber.subscribeUrl(url_name, {}, CallBackTest);
  this->subscriber_mock_->forwardData(url_name, test_Channel_Msg);
  EXPECT_EQ(test, 65);
  ASSERT_EQ(returnvalue, 2u);
  ASSERT_EQ(this->subscriber_mock_->getNumberOfSubscriptionsForUrl(url_name),
            1u); /*check for the callback registered with subscribeUrl*/
}

TYPED_TEST(PluginTest, unsubscribe_test_count) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataSubscriber Subscriber(Plugin.get());

  std::string url_name = "test1";
  size_t returnvalue = 0u;

  /*subscribe url*/
  returnvalue = Subscriber.subscribeUrl(url_name, {}, {});
  ASSERT_EQ(returnvalue, 2u);

  /*check subscritption count*/
  ASSERT_EQ(returnvalue, Subscriber.getSubscriptionsInfo().size());
  /*unsubscribe url */
  Subscriber.unsubscribe(1);
  /*check subscription count again ,count should be one less*/
  ASSERT_EQ(Subscriber.getSubscriptionsInfo().size(), 1u);
}

TYPED_TEST(PluginTest, unsubscribe_test_callback) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);
  next::bridgesdk::plugin_addons::PluginDataSubscriber Subscriber(Plugin.get());
  std::string url_name = "test1";
  char test;
  const std::string test_str = "test_callback";
  auto CallBackTest = std::bind(&test_callback, std::placeholders::_1, std::placeholders::_2, &test);
  Subscriber.subscribeUrl(url_name, {}, CallBackTest);

  /*unsubscribe url */
  Subscriber.unsubscribe(2u);

  /*check subscription count again ,count should be one less*/
  ASSERT_EQ(this->subscriber_mock_->getSubscriptionsInfo().size(),
            1u); /*check for the callback registered with subscribeUrl*/
  ASSERT_EQ(this->subscriber_mock_->getNumberOfSubscriptionsForUrl(url_name), 0u);
}
