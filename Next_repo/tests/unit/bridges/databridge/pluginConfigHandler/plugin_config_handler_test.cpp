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

#include <plugin_config_handler.h>

namespace next {
namespace databridge {
namespace plugin_util {

std::vector<next::bridgesdk::plugin_config_t> config_to_set = {
    {std::pair("[Data_VIEW]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view"}),
     std::pair("[color]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::EDITABLE, "#color"})},

    {std::pair("[Data_VIEW]",
               next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view 2nd"})}};

std::vector<next::bridgesdk::plugin_config_t> config_to_set_new = {
    {std::pair("[Data_VIEW]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view 2nd"})},

    {std::pair("[Data_VIEW]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view new"}),
     std::pair("[color]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::EDITABLE, "#color new"})}};

std::vector<next::bridgesdk::plugin_config_t> config_to_set_new_multiple_configs = {
    {std::pair("[Data_VIEW]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view 2nd"})},
    {std::pair("[Data_VIEW]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view 2nd"})},
    {std::pair("[Data_VIEW_1]",
               next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view 2nd"})},
    {std::pair("[Data_VIEW]",
               next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::EDITABLE, "Data view 2nd"})},
    {std::pair("[Data_VIEW]",
               next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view 3nd"})}};

class PluginConfigHandlerTest : public PluginConfigHandler {
public:
  PluginConfigHandlerTest(){};

  void testAddNewConfigCall(const plugin_util::map_plugin_configs &configs_to_set,
                            plugin_util::map_plugin_configs &plugin_configs_cpy, map_plugin_configs &configs_to_add) {
    addNewConfigCall(configs_to_set, plugin_configs_cpy, configs_to_add);
  }

  void testremoveConfigIfCfgChangedAndCall(const plugin_util::map_plugin_configs &configs_to_set,
                                           plugin_util::map_plugin_configs &plugin_configs_cpy,
                                           map_plugin_configs &configs_to_remove) {
    removeConfigIfCfgChangedAndCall(configs_to_set, plugin_configs_cpy, configs_to_remove);
  }

  std::string CreateKey(const next::bridgesdk::plugin_config_t &config) {
    std::string final_key = "";
    for (const auto &[key, values] : config) {
      final_key += key + "[" + std::to_string(static_cast<uint32_t>(values.config_type)) + "]:" + values.value + ";";
    }
    return final_key;
  }
};

TEST(PluginConfigTest, add_config_remove_config) {

  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs(config_to_set, plugin_name, source_name, plugin_config_updates);

  auto available_configs = plugin_config_handler.getAvailableConfigs();
  auto it_instance_remove = available_configs.find(plugin_name);
  ASSERT_NE(it_instance_remove, available_configs.end()) << "plugin not found";

  for (auto &[key, config_value] : plugin_config_updates.configs_to_add) {
    plugin_config_handler.removeConfig(plugin_name, source_name, key);
  }

  available_configs = plugin_config_handler.getAvailableConfigs();

  auto it_plugin_instance = available_configs.find(plugin_name);
  ASSERT_NE(it_plugin_instance, available_configs.end()) << "plugin not found";
  auto it_source = it_plugin_instance->second.find(source_name);
  ASSERT_NE(it_source, it_plugin_instance->second.end()) << "source not found";
  ASSERT_EQ(it_source->second.size(), 0) << "source size is different from 0";
}

TEST(PluginConfigTest, remove_config_with_empty_config) {

  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs(config_to_set, plugin_name, source_name, plugin_config_updates);

  auto available_configs = plugin_config_handler.getAvailableConfigs();

  auto it_plugin_instance = available_configs.find(plugin_name);
  ASSERT_NE(it_plugin_instance, available_configs.end()) << "plugin not found";
  ASSERT_EQ(plugin_config_updates.configs_to_add.size(), 2) << "plugin config to add size is different from 2";
  ASSERT_EQ(plugin_config_updates.configs_to_remove.size(), 0) << "plugin config to remove size is different from 0";
  plugin_config_handler.updateConfigs({}, plugin_name, source_name, plugin_config_updates);

  available_configs = plugin_config_handler.getAvailableConfigs();
  ASSERT_EQ(plugin_config_updates.configs_to_add.size(), 0)
      << "plugin config to add size is different from 0 (expected 0)";
  ASSERT_EQ(plugin_config_updates.configs_to_remove.size(), 2)
      << "plugin config to remove size is different from 2 (expected 2)";

  auto it_plugin_instance_new = available_configs.find(plugin_name);
  ASSERT_NE(it_plugin_instance_new, available_configs.end()) << "plugin not found";
}

TEST(PluginConfigTest, remove_config_and_update) {

  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs(config_to_set, plugin_name, source_name, plugin_config_updates);

  auto available_configs = plugin_config_handler.getAvailableConfigs();

  auto it_plugin_instance = available_configs.find(plugin_name);
  ASSERT_NE(it_plugin_instance, available_configs.end()) << "plugin not found";

  plugin_config_handler.updateConfigs(config_to_set_new, plugin_name, source_name, plugin_config_updates);

  available_configs = plugin_config_handler.getAvailableConfigs();
  ASSERT_EQ(plugin_config_updates.configs_to_add.size(), 1)
      << "plugin config to add size is different from 1 (expected 1)";
  ASSERT_EQ(plugin_config_updates.configs_to_remove.size(), 1)
      << "plugin config to remove size is different from 1 (expected 1)";

  std::string config_to_remove =
      plugin_config_handler.CreateKey(plugin_config_updates.configs_to_remove.begin()->second);
  std::string expected_config_to_removed = plugin_config_handler.CreateKey(config_to_set.at(0));
  ASSERT_EQ(config_to_remove, expected_config_to_removed);

  std::string config_to_add = plugin_config_handler.CreateKey(plugin_config_updates.configs_to_add.begin()->second);
  std::string expected_config_to_add = plugin_config_handler.CreateKey(config_to_set_new.at(1));
  ASSERT_EQ(config_to_add, expected_config_to_add);

  auto it_instance_new = available_configs.find(plugin_name);
  ASSERT_NE(it_instance_new, available_configs.end()) << "plugin not found";
  auto it_source_name_new = it_instance_new->second.find(source_name);
  ASSERT_NE(it_source_name_new, it_instance_new->second.end()) << "source not found";

  for (auto &it : it_source_name_new->second) {
    if (auto temp = it.second.find("[color]"); temp != it.second.end()) {
      ASSERT_EQ(temp->second.value, "#color new") << "config param did not change after config change";
    }
  }
}

TEST(PluginConfigTest, add_remove_config) {

  PluginConfigHandlerTest plugin_config_handler;
  plugin_util::map_plugin_configs plugin_configs_cpy{};
  plugin_util::map_plugin_configs configs_map;
  PluginConfigUpdates plugin_config_updates;

  const std::string plugin_name = "plugin";
  const std::string source_name = "source";

  auto available_configs = plugin_config_handler.getAvailableConfigs();
  if (auto it_plugin_configs_all = available_configs.find(plugin_name);
      it_plugin_configs_all != available_configs.end()) {
    if (auto itPlugin_configs_source = it_plugin_configs_all->second.find(source_name);
        itPlugin_configs_source != it_plugin_configs_all->second.end()) {
      plugin_configs_cpy = itPlugin_configs_source->second;
    }
  }

  for (auto &conf : config_to_set) {
    std::string config_key = plugin_config_handler.CreateKey(conf);
    configs_map.insert({config_key, conf});
  }

  plugin_config_handler.testremoveConfigIfCfgChangedAndCall(configs_map, plugin_configs_cpy,
                                                            plugin_config_updates.configs_to_remove);
  ASSERT_EQ(plugin_config_updates.configs_to_remove.size(), 0)
      << "plugin config to add size is different from 0 (expected 0)";

  plugin_config_handler.testAddNewConfigCall(configs_map, plugin_configs_cpy, plugin_config_updates.configs_to_add);
  ASSERT_EQ(plugin_config_updates.configs_to_add.size(), 2)
      << "plugin config to remove size is different from 2 (expected 2)";
}

TEST(PluginConfigTest, add_empty_config_add_new_config) {
  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs({}, plugin_name, source_name, plugin_config_updates);
  auto available_configs = plugin_config_handler.getAvailableConfigs();

  ASSERT_EQ(available_configs.size(), 0);

  auto plugin_config = available_configs.find(plugin_name);
  ASSERT_EQ(plugin_config, available_configs.end()) << "plugin not found";

  plugin_config_handler.updateConfigs(config_to_set, plugin_name, source_name, plugin_config_updates);
  available_configs = plugin_config_handler.getAvailableConfigs();

  plugin_config = available_configs.find(plugin_name);
  ASSERT_NE(plugin_config, available_configs.end()) << "plugin was not removed";

  auto source_name_config = plugin_config->second.find(source_name);
  ASSERT_EQ(source_name_config->second.size(), 2);
}

TEST(PluginConfigTest, add_remove_empty_config) {
  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs({}, plugin_name, source_name, plugin_config_updates);

  auto available_configs = plugin_config_handler.getAvailableConfigs();
  ASSERT_EQ(available_configs.size(), 0);

  for (auto &[key, config_value] : plugin_config_updates.configs_to_add) {
    plugin_config_handler.removeConfig(plugin_name, source_name, key);
  }
}

TEST(PluginConfigTest, add_remove_same_config) {
  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs(config_to_set, plugin_name, source_name, plugin_config_updates);
  auto available_configs = plugin_config_handler.getAvailableConfigs();

  auto plugin_configs = available_configs.find(plugin_name);
  auto config_source_name = plugin_configs->second.find(source_name);
  ASSERT_EQ(config_source_name->second.size(), 2);

  for (auto &[key, config_value] : plugin_config_updates.configs_to_add) {
    plugin_config_handler.removeConfig(plugin_name, source_name, key);
  }

  available_configs = plugin_config_handler.getAvailableConfigs();
  plugin_configs = available_configs.find(plugin_name);
  config_source_name = plugin_configs->second.find(source_name);
  ASSERT_EQ(config_source_name->second.size(), 0);

  plugin_config_handler.updateConfigs(config_to_set, plugin_name, source_name, plugin_config_updates);

  available_configs = plugin_config_handler.getAvailableConfigs();
  plugin_configs = available_configs.find(plugin_name);
  config_source_name = plugin_configs->second.find(source_name);
  ASSERT_EQ(config_source_name->second.size(), 2);
}

TEST(PluginConfigTest, add_config_two_sources) {
  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs(config_to_set, plugin_name, source_name, plugin_config_updates);
  auto available_configs = plugin_config_handler.getAvailableConfigs();

  ASSERT_EQ(available_configs.size(), 1);

  const std::string new_source_name = "new_source_name";

  plugin_config_handler.updateConfigs(config_to_set_new, plugin_name, new_source_name, plugin_config_updates);
  available_configs = plugin_config_handler.getAvailableConfigs();

  auto plugin_configs = available_configs.find(plugin_name);
  auto config_source_name = plugin_configs->second.find(source_name);
  auto config_new_source_name = plugin_configs->second.find(new_source_name);

  ASSERT_EQ(config_source_name->second.size(), 2);
  ASSERT_EQ(config_new_source_name->second.size(), 2);
}

TEST(PluginConfigTest, add_config_multiple_configs) {
  const std::string plugin_name = "plugin";
  const std::string source_name = "source_name";

  PluginConfigHandlerTest plugin_config_handler;
  PluginConfigUpdates plugin_config_updates;

  plugin_config_handler.updateConfigs(config_to_set_new_multiple_configs, plugin_name, source_name,
                                      plugin_config_updates);
  auto available_configs = plugin_config_handler.getAvailableConfigs();

  ASSERT_EQ(available_configs.size(), 1);

  auto plugin_configs = available_configs.find(plugin_name);
  auto config_source_name = plugin_configs->second.find(source_name);
  ASSERT_EQ(config_source_name->second.size(), 4);
}
} // namespace plugin_util
} // namespace databridge
} // namespace next