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
#include "plugin_config_handler.h"
#include "plugin_mock.hpp"
#include "process_manager.h"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;

const std::string MOCK_URL("MockUrl");

TYPED_TEST_CASE(PluginTest, next::plugins::MockPlugin);

//! Testing plugin creation
/*!
 * This test is demonstrating how to create a instance of a plugin. Afterwards the name is set and checked.
 */
TEST(PluginUnittestTest, PluginCreation) {
  next::plugins::MockPlugin plugin("notExisting");

  const std::string plugin_name("MockPluginName");
  plugin.setName(plugin_name);
  EXPECT_EQ(plugin.getName(), plugin_name);
}

//! Connecting the plugin and providing dummy data
/*!
 * This test is showing how to provide data to the plugin.
 */
TYPED_TEST(PluginTest, CheckConnection) {
  std::vector<std::byte> input_data(512); // just 512 bytes

  EXPECT_EQ(this->subscriber_mock_->getSubscriptionsInfo().size(), 1);
  EXPECT_EQ(this->plugin_->getTotalReceivedMessagesCount(), 0u);

  this->subscriber_mock_->forwardData(MOCK_URL, input_data);

  EXPECT_EQ(this->plugin_->getTotalReceivedMessagesCount(), 1u);
}

//! Connecting the plugin, sending some data and expecting some result
TYPED_TEST(PluginTest, CheckDataProcessing) {
  std::vector<std::byte> input_data(sizeof(int) * 512, (std::byte)0); // 512 ints

  ASSERT_EQ(this->subscriber_mock_->getSubscriptionsInfo().size(), 1u);
  ASSERT_EQ(this->dm_mock_->data_.size(), 0);

  this->subscriber_mock_->forwardData(MOCK_URL, input_data);

  ASSERT_EQ(this->dm_mock_->data_.size(), 1);
  EXPECT_EQ(this->dm_mock_->data_.front().size(), input_data.size());
  const int *ints = reinterpret_cast<const int *>(this->dm_mock_->data_.front().data());
  for (size_t i = 0; this->dm_mock_->data_.front().size() / sizeof(int) > i; ++i) {
    EXPECT_EQ(ints[i], 1);
  }
}

//! Adding two sensors, making sure that we get two ids
TYPED_TEST(PluginTest, RegisterSensors) {
  const std::vector<std::string> sensors{"sensor1", "sensor2"};
  const std::vector<next::bridgesdk::plugin_addons::uid_t> &sensor_ids = this->plugin_->sensor_ids_;

  EXPECT_EQ(sensor_ids.empty(), true);

  EXPECT_EQ(this->plugin_->update(sensors), true);
  EXPECT_EQ(sensor_ids.size(), sensors.size());

  EXPECT_EQ(this->plugin_->update(std::vector<std::string>()), true);
  EXPECT_EQ(sensor_ids.empty(), true);
}

//! Adding two sensors, keeping only the second, adding another one and assuring that the id of the always used sensor
//! is constant
TYPED_TEST(PluginTest, ChangeSensorRegstrations) {

  const std::vector<std::string> sensors1{"sensor1", "sensor2"};
  const std::vector<std::string> sensors2{"sensor2"};
  const std::vector<std::string> sensors3{"sensor2", "sensor3"};
  const std::vector<std::string> sensors_empty{};
  const std::vector<next::bridgesdk::plugin_addons::uid_t> &sensor_ids = this->plugin_->sensor_ids_;

  EXPECT_EQ(sensor_ids.empty(), true);

  EXPECT_EQ(this->plugin_->update(sensors1), true);
  EXPECT_EQ(sensor_ids.size(), sensors1.size());

  EXPECT_EQ(this->plugin_->update(sensors2), true);
  ASSERT_EQ(sensor_ids.size(), sensors2.size());
  const auto id = this->plugin_->sensor_ids_.front();

  EXPECT_EQ(this->plugin_->update(sensors3), true);
  EXPECT_EQ(sensor_ids.size(), sensors3.size());
  EXPECT_EQ(std::find(sensor_ids.begin(), sensor_ids.end(), id), sensor_ids.begin());

  EXPECT_EQ(this->plugin_->update(sensors_empty), true);
  EXPECT_EQ(sensor_ids.empty(), true);
}
