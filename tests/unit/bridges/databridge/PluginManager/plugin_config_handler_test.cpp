/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_config_handler_test.cpp
 *  @brief    PluginConfigHandler unit tests
 */

#include <chrono>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>

#include <next/databridge/plugin_manager.h>

#include "plugin_config_handler.h"
#include "plugin_errorHandling_mock.hpp"
#include "plugin_sdl_mock.hpp"
#include "unittest_mock_plugin_manager.hpp"

using namespace next;
using namespace next::databridge::plugin_util;
using next::test::MockPluginManager;

using namespace next::databridge::plugin_manager;
TYPED_TEST_CASE(MockPluginManager, next::plugins::MockSdlPlugin);

// incluging the helper_functions.h cases linking error so dfine a new function here
std::string CreateKeyOutOfPluginConfigTest(const next::bridgesdk::plugin_config_t &config) {
  std::string final_key = "";
  for (const auto &[key, values] : config) {
    final_key += key + "[" + std::to_string(static_cast<uint32_t>(values.config_type)) + "]:" + values.value + ";";
  }
  return final_key;
}

//! Testing plugin description / config setting
/*!
 * This is adding and updating configs
 */
TEST(PluginUnittestTest, updateConfigs_test) {
  std::shared_ptr<next::databridge::plugin_util::PluginConfigHandler> plugin_config_handler_;
  next::bridgesdk::plugin_config_t config;
  std::vector<next::bridgesdk::plugin_config_t> configs;
  next::bridgesdk::PluginConfigParam test_param;

  test_param.config_type = next::bridgesdk::ConfigType::EDITABLE;
  test_param.value = "test_1";
  config.emplace(std::make_pair("Config_1", test_param));
  configs.push_back(config);
  std::string plugin_name = "Test_plugin_name";
  std::string source_name = "Test_source_name";

  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates;
  plugin_config_handler_ = std::make_shared<next::databridge::plugin_util::PluginConfigHandler>();
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates);
  EXPECT_EQ(plugin_config_updates.configs_to_add.size(), 1u);
  EXPECT_EQ(plugin_config_updates.configs_to_remove.size(), 0u);

  // adding same config again
  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates1;
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates1);
  EXPECT_EQ(plugin_config_updates1.configs_to_add.size(), 0u);
  EXPECT_EQ(plugin_config_updates1.configs_to_remove.size(), 0u);

  // adding different config
  test_param.config_type = next::bridgesdk::ConfigType::FIXED;
  test_param.value = "test_2";
  config.clear();
  config.emplace(std::make_pair("Config_2", test_param));
  configs.push_back(config);
  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates2;
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates2);
  EXPECT_EQ(plugin_config_updates2.configs_to_add.size(), 1u);
  EXPECT_EQ(plugin_config_updates2.configs_to_remove.size(), 0u);

  // Removed one config from the input, now the input contains only one new config to add
  configs.erase(configs.begin());
  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates3;
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates3);
  EXPECT_EQ(plugin_config_updates3.configs_to_add.size(), 1u);
  EXPECT_EQ(plugin_config_updates3.configs_to_remove.size(), 1u);

  // configs to set is empty , all existign confisgs exepected to add to the configs to remove
  // test for removeConfigIfCfgListEmptyAndCall
  configs.clear();
  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates4;
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates4);
  EXPECT_EQ(plugin_config_updates4.configs_to_add.size(), 0u);
  EXPECT_EQ(plugin_config_updates4.configs_to_remove.size(), 1u);
}

TEST(PluginUnittestTest, removeInstance_test) {

  std::shared_ptr<next::databridge::plugin_util::PluginConfigHandler> plugin_config_handler_;
  next::bridgesdk::plugin_config_t config;
  std::vector<next::bridgesdk::plugin_config_t> configs;
  next::bridgesdk::PluginConfigParam test_param;

  test_param.config_type = next::bridgesdk::ConfigType::EDITABLE;
  test_param.value = "test_1";
  config.emplace(std::make_pair("Config_1", test_param));
  configs.push_back(config);
  std::string plugin_name = "Test_plugin_name";
  std::string source_name = "Test_source_name";

  // add first config
  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates;
  plugin_config_handler_ = std::make_shared<next::databridge::plugin_util::PluginConfigHandler>();
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates);
  EXPECT_EQ(plugin_config_updates.configs_to_add.size(), 1u);
  EXPECT_EQ(plugin_config_updates.configs_to_remove.size(), 0u);

  // adding different config

  test_param.config_type = next::bridgesdk::ConfigType::FIXED;
  test_param.value = "test_2";
  config.clear();
  config.emplace(std::make_pair("Config_2", test_param));
  configs.push_back(config);
  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates2;
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates2);
  EXPECT_EQ(plugin_config_updates2.configs_to_add.size(), 1u);
  EXPECT_EQ(plugin_config_updates2.configs_to_remove.size(), 0u);

  // remove one plugin config from the above added 2 plugin configs
  std::string config_key = CreateKeyOutOfPluginConfigTest(config);
  plugin_config_handler_->removeConfig(plugin_name, source_name, config_key);

  // one plugin removed already , one plugin will remain
  configs.clear();
  next::databridge::plugin_util::PluginConfigUpdates plugin_config_updates4;
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates4);
  EXPECT_EQ(plugin_config_updates4.configs_to_add.size(), 0u);
  EXPECT_EQ(plugin_config_updates4.configs_to_remove.size(), 1u);

  // now available_configs_ is emppty,remove config from it again
  plugin_config_updates4.configs_to_add.clear();
  plugin_config_updates4.configs_to_remove.clear();
  plugin_config_handler_->removeConfig(plugin_name, source_name, config_key);
  plugin_config_handler_->updateConfigs(configs, plugin_name, source_name, plugin_config_updates4);
  EXPECT_EQ(plugin_config_updates4.configs_to_add.size(), 0u);
  EXPECT_EQ(plugin_config_updates4.configs_to_remove.size(), 0u);
}
