/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_signal_extractor_test.cpp
 *  @brief    Testing some mock functionalities
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include <atomic>
#include "data_types.hpp"
#include "plugin_mock.hpp"
#include "unittest_plugin_helper.hpp"

using next::test::PluginTest;
const std::string MOCK_URL("MockUrl");
TYPED_TEST_CASE(PluginTest, next::plugins::MockPlugin);

TYPED_TEST(PluginTest, extractData_test) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");
  Plugin->connectPlugin(ipc_mock_ptr_, dm_mock_ptr_, udex_monitor_ptr_.get(), signal_explorer_ptr_);
  next::bridgesdk::plugin_addons::PluginSignalExtractor Extractor(Plugin);
  const std::string test_topic = "test_topic";
  const std::string request_url = "test_url";
  next::bridgesdk::plugin_addons::Signals test_signal_info;
  int test_raw_data;
  size_t test_data_size = 4;
  next::udex::Signal test_signal;
  auto udex_monitor_mock = std::dynamic_pointer_cast<next::test::IUDex_Monitor_MOC>(udex_monitor_ptr_);
  test_signal.data_["data1"] = {1, next::udex::SignalType::SIGNAL_TYPE_BOOL, 4, 0, {'d', 'a', 't', 'a', '1'}};
  test_signal.data_["data1"] = {2, next::udex::SignalType::SIGNAL_TYPE_BOOL, 10, 5, {'d', 'a', 't', 'a', '2'}};

  udex_monitor_mock->setSignalInfo(test_signal);
  Extractor.extractData(test_topic, request_url, test_signal_info, &test_raw_data, test_data_size);

  ASSERT_EQ(test_signal_info.raw_signal_info["data1"].array_length_, test_signal.data_["data1"].array_length_);
  ASSERT_EQ(test_signal_info.raw_signal_info["data1"].signal_offset, test_signal.data_["data1"].start_offset_);
  ASSERT_EQ(test_signal_info.raw_signal_info["data1"].payload_size_, test_signal.data_["data1"].payload_size_);
  for (int i = 0; i < next::bridgesdk::plugin::N_BYTES_CAN_DATA; i++) {
    ASSERT_EQ(test_signal_info.raw_signal_info["data1"].can_data[i], test_signal.data_["data1"].can_data[i]);
  }
  ASSERT_EQ(test_signal_info.raw_signal_info["data2"].array_length_, test_signal.data_["data2"].array_length_);
  ASSERT_EQ(test_signal_info.raw_signal_info["data2"].signal_offset, test_signal.data_["data2"].start_offset_);
  ASSERT_EQ(test_signal_info.raw_signal_info["data2"].payload_size_, test_signal.data_["data2"].payload_size_);
  for (int i = 0; i < next::bridgesdk::plugin::N_BYTES_CAN_DATA; i++) {
    ASSERT_EQ(test_signal_info.raw_signal_info["data2"].can_data[i], test_signal.data_["data2"].can_data[i]);
  }
}

TYPED_TEST(PluginTest, extractData_nullptr_check) {

  auto Plugin = std::make_shared<next::plugins::MockPlugin>("dummy");
  Plugin->connectPlugin(ipc_mock_ptr_, dm_mock_ptr_, nullptr, signal_explorer_ptr_);
  next::bridgesdk::plugin_addons::PluginSignalExtractor Extractor(Plugin);
  const std::string test_topic = "test_topic";
  const std::string request_url = "test_url";
  next::bridgesdk::plugin_addons::Signals test_signal_info;
  int test_raw_data;
  size_t test_data_size = 4;
  next::udex::Signal test_signal;
  auto udex_monitor_mock = std::dynamic_pointer_cast<next::test::IUDex_Monitor_MOC>(udex_monitor_ptr_);
  test_signal.data_["data1"] = {1, next::udex::SignalType::SIGNAL_TYPE_BOOL, 4, 0, {'d', 'a', 't', 'a', '1'}};
  test_signal.data_["data1"] = {2, next::udex::SignalType::SIGNAL_TYPE_BOOL, 10, 5, {'d', 'a', 't', 'a', '2'}};

  udex_monitor_mock->setSignalInfo(test_signal);
  Extractor.extractData(test_topic, request_url, test_signal_info, &test_raw_data, test_data_size);

  ASSERT_EQ(test_signal_info.raw_signal_info["data1"].array_length_, 0);
  ASSERT_EQ(test_signal_info.raw_signal_info["data1"].signal_offset, 0);
  ASSERT_EQ(test_signal_info.raw_signal_info["data1"].payload_size_, 0);
  for (int i = 0; i < next::bridgesdk::plugin::N_BYTES_CAN_DATA; i++) {
    ASSERT_EQ(test_signal_info.raw_signal_info["data1"].can_data[i], 0);
  }
  ASSERT_EQ(test_signal_info.raw_signal_info["data2"].array_length_, 0);
  ASSERT_EQ(test_signal_info.raw_signal_info["data2"].signal_offset, 0);
  ASSERT_EQ(test_signal_info.raw_signal_info["data2"].payload_size_, 0);
  for (int i = 0; i < next::bridgesdk::plugin::N_BYTES_CAN_DATA; i++) {
    ASSERT_EQ(test_signal_info.raw_signal_info["data2"].can_data[i], 0);
  }
}
