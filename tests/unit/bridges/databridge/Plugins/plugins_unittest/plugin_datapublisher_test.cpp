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

#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>

#include "plugin_mock.hpp"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;

const std::string MOCK_URL("MockUrl");

TYPED_TEST_CASE(PluginTest, next::plugins::MockPlugin);

TYPED_TEST(PluginTest, create) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataPublisher Publisher(Plugin.get());
  int test_int = 5;
  Publisher.sendResultData((const uint8_t *)&test_int, sizeof(int));
  ASSERT_EQ(this->dm_mock_->data_.size(), 1);

  this->plugin_->callDummySend(6);
  EXPECT_EQ(this->dm_mock_->data_.size(), 2);
}

TYPED_TEST(PluginTest, create_callDummySend) {

  this->plugin_->callDummySend(6);
  EXPECT_EQ(this->dm_mock_->data_.size(), 1);
}

TYPED_TEST(PluginTest, sendResultData_nullptr_check) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataPublisher Publisher(Plugin.get());

  Publisher.sendResultData(nullptr, sizeof(int));
  ASSERT_EQ(this->dm_mock_->data_.size(), 0u);
}

TYPED_TEST(PluginTest, sendResultData_data_check) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataPublisher Publisher(Plugin.get());
  uint8_t test_int[6] = {0, 1, 2, 3, 4, 5}; /*test data*/

  Publisher.sendResultData((const uint8_t *)&test_int, sizeof(test_int));
  ASSERT_EQ(this->dm_mock_->data_.size(), 1);
  auto it = this->dm_mock_->data_.begin();
  ASSERT_EQ((*it)[0], 0);
  ASSERT_EQ((*it)[1], 1);
  ASSERT_EQ((*it)[2], 2);
  ASSERT_EQ((*it)[3], 3);
  ASSERT_EQ((*it)[4], 4);
  ASSERT_EQ((*it)[5], 5);
}

TYPED_TEST(PluginTest, Test_sendFlatbufferData) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataPublisher Publisher(Plugin.get());

  std::string path_to_fbs = "test";
  char flatbuffer_data[10] = {'T', 'E', 'S', 'T'}; // Sending test data
  next::bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = reinterpret_cast<const uint8_t *>(flatbuffer_data);
  flatbuffer.fbs_size = 10;
  Publisher.sendFlatbufferData(path_to_fbs, flatbuffer, {}, {});
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_.size(), flatbuffer.fbs_size);
  // sendFlatbufferData should update the flatbuffer_data_ with data being passed as argument
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[0], 'T');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[1], 'E');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[2], 'S');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[3], 'T');
}

TYPED_TEST(PluginTest, Test_sendFlatbufferData_zero_buffer) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");

  Plugin->connectPlugin(this->subscriber_mock_ptr_, this->dm_mock_ptr_, this->signal_explorer_ptr_);

  next::bridgesdk::plugin_addons::PluginDataPublisher Publisher(Plugin.get());

  std::string path_to_fbs = "test";
  char flatbuffer_data[10] = {'T', 'E', 'S', 'T'}; // Sending test data
  next::bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = reinterpret_cast<const uint8_t *>(flatbuffer_data);
  flatbuffer.fbs_size = 10;
  // Test 1.1 - Send test data with size of test buffer as argument
  Publisher.sendFlatbufferData(path_to_fbs, flatbuffer, {}, {});
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_.size(),
            flatbuffer.fbs_size); // flatbuffer_data_ size shoule be same as the size of test data being sent
  // sendFlatbufferData should update the flatbuffer_data_ with data being passed as argument
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[0], 'T');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[1], 'E');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[2], 'S');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[3], 'T');

  // Test 1.2 - Send test data with size of test buffer as Zero
  Publisher.sendFlatbufferData(path_to_fbs, flatbuffer, {}, {});
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_.size(),
            flatbuffer.fbs_size); // flatbuffer_data_ size shoule be same as the size of test data being sent
  // sendFlatbufferData should update the should remain same as last test
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[0], 'T');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[1], 'E');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[2], 'S');
  ASSERT_EQ(this->dm_mock_->flatbuffer_data_[3], 'T');
}
