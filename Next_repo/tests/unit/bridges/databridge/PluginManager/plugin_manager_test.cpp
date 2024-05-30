/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_manager_test.cpp
 *  @brief    PluginManager unit tests
 */

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <next/databridge/plugin_manager.h>
#include <boost/filesystem.hpp>
#include "OutputSchema/output_schema_checker.h"
#include "next/sdk/sdk.hpp"
#include "plugin_errorHandling_mock.hpp"
#include "unittest_plugin_manager_helper.hpp"

using next::test::PluginManagerTestHelper;
plugin_create_t create_plugin(const char *path) { return new next::bridgesdk::plugin::Plugin(path); }

class PluginManagerTestPluginTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  PluginManagerTestPluginTestFixture() { this->instance_name_ = "request_event_handler"; }
};
// check if player state is correctly evaluated to return if plugin shall be reset
TEST_F(PluginManagerTestPluginTestFixture, checkForPluginReset) {

  next::databridge::plugin_manager::PluginManager plugin_manager{};

  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);

  next::control::events::PlayerMetaInformationMessage pMessagePlugin;

  // positive test
  pMessagePlugin.status = next::control::events::PlayerState::EXIT_OPENING;
  EXPECT_EQ(PluginManagerTestHelper.checkForPluginReset(pMessagePlugin), true);

  pMessagePlugin.status = next::control::events::PlayerState::EXIT_SKIP_TO;
  pMessagePlugin.min_timestamp = 10;
  pMessagePlugin.timestamp = pMessagePlugin.min_timestamp;
  EXPECT_EQ(PluginManagerTestHelper.checkForPluginReset(pMessagePlugin), true);

  // negative test
  pMessagePlugin.status = next::control::events::PlayerState::EXIT_SKIP_TO;
  pMessagePlugin.min_timestamp = 9;
  pMessagePlugin.timestamp = 10;
  EXPECT_EQ(PluginManagerTestHelper.checkForPluginReset(pMessagePlugin), false);

  pMessagePlugin.status = next::control::events::PlayerState::EXIT_SKIP_TO;
  pMessagePlugin.min_timestamp = 11;
  pMessagePlugin.timestamp = 10;
  EXPECT_EQ(PluginManagerTestHelper.checkForPluginReset(pMessagePlugin), false);

  pMessagePlugin.status = next::control::events::PlayerState::ENTER_SKIP_TO;
  EXPECT_EQ(PluginManagerTestHelper.checkForPluginReset(pMessagePlugin), false);
}
// check updateconfig
TEST_F(PluginManagerTestPluginTestFixture, updatePluginConfigs_no_compatiable_plugin_test) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);
  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);

  const std::string data_class = "test_data_class";
  const std::string version = "test_version";
  std::string source_name = "test_source_name";
  const std::vector<std::string> user_parameter;
  EXPECT_FALSE(PluginManagerTestHelper.updatePluginConfigs(data_class, version, source_name, user_parameter));
}

// update config
TEST_F(PluginManagerTestPluginTestFixture, updatePluginConfigs_compatiable_plugin_test) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);
  // set up plugins in plugin manager by helper class
  PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);

  const std::string data_class = "GuiSchema.TrafficParticipant";
  const std::string version = "0.0.1";
  const std::vector<std::string> user_parameter;
  const std::string &plugin_name = "test_plugin";
  const std::string &source_name = "test_source_name";
  const std::string &instance_name_2 = "instance_name2";
  const std::string &instance_name_3 = "instance_name3";

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin3 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest3");

  next::databridge::plugin_manager::map_instances_per_source inst_2;
  next::databridge::plugin_manager::map_instances_per_source inst_3;
  inst_2.emplace(instance_name_2, plugin2);
  inst_3.emplace(instance_name_3, plugin3);

  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;

  boost::filesystem::path path_to_fbs = boost::filesystem::current_path() / "trafficparticipant.fbs";
  std::list<std::string> input_url = {"input_url1", "input_url2", "input_url3", "input_url4"};
  std::list<std::string> output_fbs = {path_to_fbs.string(), "fbs", "fbs", "fbs"};
  next::databridge::plugin_manager::plugin_information p_info2{create_plugin,
                                                               input_url,
                                                               output_fbs,
                                                               {
                                                                   {std::make_pair(std::string(source_name), inst_2)},
                                                                   {std::make_pair(std::string(source_name), inst_3)},
                                                               },
                                                               ""};
  pluginList[plugin_name] = p_info2;
  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);

  EXPECT_TRUE(PluginManagerTestHelper.updatePluginConfigs(data_class, version, source_name, user_parameter));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(plugin2->add_update_counter, 1);

  pluginList.clear();
  plugin2->add_update_counter = 0;
  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);
  p_info2 = {create_plugin,
             input_url,
             output_fbs,
             {
                 {std::make_pair(std::string("some_source"), inst_2)},
             },
             ""};
  pluginList[plugin_name] = p_info2;

  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);

  EXPECT_TRUE(PluginManagerTestHelper.updatePluginConfigs(data_class, version, source_name, user_parameter));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(plugin2->add_update_counter, 0);
}

// check findAndSetConfig empty and non empty input config tests
TEST_F(PluginManagerTestPluginTestFixture, findAndSetConfig_test) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);
  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  next::bridgesdk::plugin_config_t config;
  std::vector<next::bridgesdk::plugin_config_t> configs;
  next::bridgesdk::PluginConfigParam test_param;

  test_param.config_type = next::bridgesdk::ConfigType::EDITABLE;
  test_param.value = "test_1";
  config.emplace(std::make_pair("Config_1", test_param));
  configs.push_back(config);
  std::string plugin_name = "Test_plugin_name";
  std::string source_name = "Test_source_name";

  EXPECT_TRUE(PluginManagerTestHelper.findAndSetConfig(configs, plugin_name, source_name));
  configs.clear();
  EXPECT_TRUE(PluginManagerTestHelper.findAndSetConfig(configs, plugin_name, source_name));
}

// check addPluginInstance add new plugin instance
TEST_F(PluginManagerTestPluginTestFixture, createInstance_test_empty_plugins_input) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  const std::string &plugin_name = "test_plugin";
  const std::string &source_name = "test_source_name";
  const std::string &instance_name = "instance_name";
  EXPECT_FALSE(PluginManagerTestHelper.addPluginInstance(plugin_name, source_name, instance_name));
}

// check addPluginInstance
TEST_F(PluginManagerTestPluginTestFixture, createInstance_test_existing_instance) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  const std::string &plugin_name = "test_plugin";
  const std::string &source_name = "test_source_name";
  const std::string &instance_name_2 = "PluginTest2_name";
  const std::string &instance_name_3 = "PluginTest3_name";

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin3 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest3");

  next::databridge::plugin_manager::map_instances_per_source inst_2;
  next::databridge::plugin_manager::map_instances_per_source inst_3;
  inst_2.emplace(instance_name_2, plugin2);
  inst_3.emplace(instance_name_3, plugin3);

  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;
  next::databridge::plugin_manager::plugin_information p_info2{
      create_plugin, {}, {}, {{std::make_pair(std::string(source_name), inst_2)}}, ""};
  pluginList[plugin_name] = p_info2;
  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);
  // add already existing plugin instance for the existing source
  EXPECT_TRUE(PluginManagerTestHelper.addPluginInstance(plugin_name, source_name, instance_name_2));
  // add new plugin instance to the same pluging for the same source
  EXPECT_TRUE(PluginManagerTestHelper.addPluginInstance(plugin_name, source_name, instance_name_3));
  // add new plugin instance to the same pluging for the different source
  EXPECT_TRUE(PluginManagerTestHelper.addPluginInstance(plugin_name, "some_source_name", instance_name_3));
  // add new plugin instance to non existing plugin
  EXPECT_FALSE(PluginManagerTestHelper.addPluginInstance("some_plugin_name", source_name, instance_name_3));
}

// check addPluginInstance
TEST_F(PluginManagerTestPluginTestFixture, createInstance_test_new_instance) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  const std::string &plugin_name = "test_plugin";
  const std::string &source_name = "test_source_name";
  const std::string &instance_name_2 = "instance_name2";
  const std::string &instance_name_3 = "instance_name3";

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin3 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest3");

  next::databridge::plugin_manager::map_instances_per_source inst_2;
  next::databridge::plugin_manager::map_instances_per_source inst_3;
  inst_2.emplace(instance_name_2, plugin2);
  inst_3.emplace(instance_name_3, plugin3);

  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;
  next::databridge::plugin_manager::plugin_information p_info2{create_plugin,
                                                               {},
                                                               {},
                                                               {
                                                                   {std::make_pair(std::string(source_name), inst_2)},
                                                                   {std::make_pair(std::string(source_name), inst_3)},
                                                               },
                                                               ""};
  pluginList[plugin_name] = p_info2;
  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);

  EXPECT_TRUE(PluginManagerTestHelper.addPluginInstance(plugin_name, source_name, "new_instance"));
}

// check addPluginInstance create empty instance if no instance for the specified source available
TEST_F(PluginManagerTestPluginTestFixture, addPluginInstance_create_empty_instance_test) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  const std::string &plugin_name = "test_plugin";
  const std::string &source_name = "test_source_name";
  const std::string &instance_name_2 = "instance_name2";
  const std::string &instance_name_3 = "instance_name3";

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin3 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest3");

  next::databridge::plugin_manager::map_instances_per_source inst_2;
  next::databridge::plugin_manager::map_instances_per_source inst_3;
  inst_2.emplace(instance_name_2, plugin2);
  inst_3.emplace(instance_name_3, plugin3);

  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;
  next::databridge::plugin_manager::plugin_information p_info2{create_plugin,
                                                               {},
                                                               {},
                                                               {
                                                                   {std::make_pair(std::string(source_name), inst_2)},
                                                                   {std::make_pair(std::string(source_name), inst_3)},
                                                               },
                                                               ""};
  pluginList[plugin_name] = p_info2;
  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);

  EXPECT_TRUE(PluginManagerTestHelper.addPluginInstance(plugin_name, "some_source_name", instance_name_2));
}

// check getPluginVersion
TEST_F(PluginManagerTestPluginTestFixture, getPluginVersion_test) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  const std::string &plugin_name = "test_plugin";
  const std::string &source_name = "test_source_name";
  const std::string &instance_name_2 = "instance_name2";
  const std::string &instance_name_3 = "instance_name3";

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin3 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest3");

  next::databridge::plugin_manager::map_instances_per_source inst_2;
  next::databridge::plugin_manager::map_instances_per_source inst_3;
  inst_2.emplace(instance_name_2, plugin2);
  inst_3.emplace(instance_name_3, plugin3);

  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;
  next::databridge::plugin_manager::plugin_information p_info2{create_plugin,
                                                               {},
                                                               {},
                                                               {
                                                                   {std::make_pair(std::string(source_name), inst_2)},
                                                                   {std::make_pair(std::string(source_name), inst_3)},
                                                               },
                                                               ""};
  pluginList[plugin_name] = p_info2;
  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);

  std::string plugin_version;
  PluginManagerTestHelper.getPluginVersion(plugin_name, source_name, instance_name_2, plugin_version);
  EXPECT_EQ(plugin_version, "0.0.1");
}

// check initPlugin
TEST_F(PluginManagerTestPluginTestFixture, initPlugin_test) {
  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  const std::string &plugin_name = "test_plugin";
  const std::string &source_name = "test_source_name";
  const std::string &instance_name_2 = "instance_name2";
  const std::string &instance_name_3 = "instance_name3";

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin3 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest3");

  next::databridge::plugin_manager::map_instances_per_source inst_2;
  next::databridge::plugin_manager::map_instances_per_source inst_3;
  inst_2.emplace(instance_name_2, plugin2);
  inst_3.emplace(instance_name_3, plugin3);

  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;
  next::databridge::plugin_manager::plugin_information p_info2{{},
                                                               {},
                                                               {},
                                                               {
                                                                   {std::make_pair(std::string(source_name), inst_2)},
                                                                   {std::make_pair(std::string(source_name), inst_3)},
                                                               },
                                                               ""};
  pluginList[plugin_name] = p_info2;
  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);

  std::string plugin_version;
  EXPECT_TRUE(PluginManagerTestHelper.initPlugin(plugin_name, source_name, instance_name_2));
}

// check plugin reset
TEST_F(PluginManagerTestPluginTestFixture, PluginManagerResetPluginDirectCall) {

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin1 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest1");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin3 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest3");

  next::databridge::plugin_manager::PluginManager plugin_manager{};
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  plugin_manager.init("");
  plugin_manager.connectToOutputDataManager(output_data_manager);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;

  // check if empty plugin doesn't crash execution
  next::databridge::plugin_manager::plugin_information p_infoEmpty{{}, {}, {}, {}, ""};
  pluginList["PluginListEmpty"] = p_infoEmpty;

  // single plugin instance
  next::databridge::plugin_manager::map_instances_per_source inst_1;
  inst_1.emplace("instance1", plugin1);
  next::databridge::plugin_manager::plugin_information p_info1{
      {}, {}, {}, {std::pair{std::string("PluginTest1"), inst_1}}, ""};
  pluginList["PluginListTest1"] = p_info1;

  next::databridge::plugin_manager::map_instances_per_source inst_empty;
  next::databridge::plugin_manager::map_instances_per_source inst_2;
  next::databridge::plugin_manager::map_instances_per_source inst_3;
  inst_empty.emplace("instance_empty", nullptr);
  inst_2.emplace("instance2", plugin2);
  inst_3.emplace("instance3", plugin3);

  // multiple plugin instances, one with a null pointer
  next::databridge::plugin_manager::plugin_information p_info2{
      {},
      {},
      {},
      {
          {std::make_pair(std::string("PluginTest1"), inst_empty)},
          {std::make_pair(std::string("PluginTest2"), inst_2)},
          {std::make_pair(std::string("PluginTest3"), inst_3)},
      },
      ""};
  pluginList["PluginListTest2"] = p_info2;

  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);

  PluginManagerTestHelper.resetAllPlugins();

  // wait till invokeCall of processManager called all enterReset methods
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // check reset of plugins
  EXPECT_EQ(plugin1->enter_reset_counter, 1);
  EXPECT_EQ(plugin1->exit_reset_counter, 1);
  EXPECT_EQ(plugin1->exit_reset_called_after_enter_reset, true);

  EXPECT_EQ(plugin2->enter_reset_counter, 1);
  EXPECT_EQ(plugin2->exit_reset_counter, 1);
  EXPECT_EQ(plugin2->exit_reset_called_after_enter_reset, true);

  EXPECT_EQ(plugin3->enter_reset_counter, 1);
  EXPECT_EQ(plugin3->exit_reset_counter, 1);
  EXPECT_EQ(plugin3->exit_reset_called_after_enter_reset, true);
}

#if 0 
//could not check the expected values after testing addAndRemoveConfigs
//Test for addAndRemoveConfigs
TEST(PluginManagerTestPlugin, addAndRemoveConfigsTest) {

	  next::databridge::plugin_manager::PluginManager plugin_manager{};
  plugin_manager.init("");
          std::shared_ptr<next::databridge::plugin_util::PluginConfigHandler> plugin_config_handler =
              std::make_shared<next::databridge::plugin_util::PluginConfigHandler>();
  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin1 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest1");

  std::shared_ptr<next::plugins::MockErrorHandlingPlugin> plugin2 =
      std::make_shared<next::plugins::MockErrorHandlingPlugin>("PluginTest2");

  std::map<std::string, next::databridge::plugin_manager::plugin_information> pluginList;
    next::databridge::plugin_manager::map_instances_per_source inst_1;
  next::databridge::plugin_manager::map_instances_per_source inst_2;
  inst_1.emplace("instance1", plugin1);
  inst_2.emplace("instance2", plugin2);

  // set up plugins in plugin manager by helper class
  next::test::PluginManagerTestHelper PluginManagerTestHelper(plugin_manager);
  std::string plugin_name = "Test_plugin_name";
  std::string source_name = "Test_source_name";
  next::bridgesdk::plugin_config_t config;
  std::vector<next::bridgesdk::plugin_config_t> configs;

  next::bridgesdk::PluginConfigParam test_param;

  test_param.config_type = next::bridgesdk::ConfigType::EDITABLE;
  test_param.value = "test_1";
  config.emplace(std::make_pair("Config_1", test_param));
  configs.push_back(config);
  //set pluginconfig hadler
  PluginManagerTestHelper.setPluginConfigHanlder(plugin_config_handler);
  //case 1 Plugins do not exist 
  PluginManagerTestHelper.addAndRemoveConfigs(configs, plugin_name,source_name);
  auto available_configs = plugin_config_handler->getAvailableConfigs();
  EXPECT_EQ(available_configs.size(),0u);

  //Add some plugins

  next::databridge::plugin_manager::plugin_information p_info{{},
                                                              {},
                                                              {},
                                                              {
                                                                  {std::make_pair(std::string(source_name), inst_1)},
                                                                  {std::make_pair(std::string("PluginTest2"), inst_2)},
                                                              },
                                                              ""};
  pluginList["PluginListTest"] = p_info;

  PluginManagerTestHelper.SetUpPluginsOfPluginManager(pluginList);
  // case 1 Add configs to existing plugins
  PluginManagerTestHelper.addAndRemoveConfigs(configs, plugin_name, source_name);
  available_configs = plugin_config_handler->getAvailableConfigs();
  EXPECT_EQ(available_configs.size(), 0u);

}
#endif