/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     config_database_manager_tests.cpp
 *  @brief    Testing the config database manager class
 */

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <json/json.h>

#include <next/sdk/sdk.hpp>

#include "config_database_manager.hpp"

namespace next {
namespace appsupport {

TEST(ConfigDatabaseManager_Tests, put_int_filtering_deactivated) {
  ConfigDatabaseManager config_database_manager("test", false);

  int expected_value = 1;
  config_database_manager.put("key", expected_value);
  int ret_value = config_database_manager.getInt("key");
  EXPECT_EQ(expected_value, ret_value);
}

TEST(ConfigDatabaseManager_Tests, put_int_filtering_active_value_added) {
  ConfigDatabaseManager config_database_manager("test", true);
  config_database_manager.addNewComponentConfig("comp");

  int expected_value = 1;
  config_database_manager.put("comp:test", expected_value);
  bool value_available = false;
  int ret_value = config_database_manager.getInt("comp:test", 0, value_available);
  EXPECT_EQ(expected_value, ret_value);
  EXPECT_TRUE(value_available);
}

TEST(ConfigDatabaseManager_Tests, put_int_filtering_active_value_ignored) {
  ConfigDatabaseManager config_database_manager("test", true);

  int value = 1;
  // value shall be ignored because comp is not available
  config_database_manager.put("comp:test", value);

  bool value_available = false;
  int ret_value = config_database_manager.getInt("comp:test", 1, value_available);
  EXPECT_EQ(1, ret_value);
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, put_float_filtering_deactivated) {
  ConfigDatabaseManager config_database_manager("test", false);

  float expected_value = 1.1f;
  config_database_manager.put("key", expected_value);
  float ret_value = config_database_manager.getFloat("key");
  EXPECT_EQ(expected_value, ret_value);
}

TEST(ConfigDatabaseManager_Tests, put_string_filtering_deactivated) {
  ConfigDatabaseManager config_database_manager("test", false);

  std::string expected_value = "test";
  config_database_manager.put("key", expected_value);
  std::string ret_value = config_database_manager.getString("key");
  EXPECT_EQ(expected_value, ret_value);
}

TEST(ConfigDatabaseManager_Tests, put_char_filtering_deactivated) {
  ConfigDatabaseManager config_database_manager("test", false);

  char expected_value[] = "test";
  config_database_manager.put("key", expected_value);
  std::string ret_value = config_database_manager.getString("key");
  EXPECT_EQ(expected_value, ret_value);
}

TEST(ConfigDatabaseManager_Tests, put_bool_filtering_deactivated) {
  ConfigDatabaseManager config_database_manager("test", false);

  bool expected_value = true;
  config_database_manager.put("key", expected_value);
  bool ret_value = config_database_manager.getBool("key");
  EXPECT_EQ(expected_value, ret_value);
}

TEST(ConfigDatabaseManager_Tests, put_int_invalid_comp_name_filtering_activated) {
  ConfigDatabaseManager config_database_manager("test", true);

  bool expected_value = true;
  config_database_manager.put("invalid_comp_name[0]", expected_value);
  bool value_available{true};
  bool ret_value = config_database_manager.getBool("invalid_comp_name[0]", false, value_available);
  EXPECT_EQ(ret_value, false); // return default value
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, put_int_invalid_comp_name_filtering_deactivated) {
  ConfigDatabaseManager config_database_manager("test", false);

  bool expected_value = true;
  config_database_manager.put("invalid_comp_name[0]", expected_value);
  bool value_available{true};
  bool ret_value = config_database_manager.getBool("invalid_comp_name[0]", false, value_available);
  EXPECT_EQ(ret_value, false); // return default value
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, insertConfig_empty_config) {
  ConfigDatabaseManager config_database_manager_dest("dest", false);
  ConfigDatabaseManager config_database_manager_source("source", false);

  int expected_value = 1;
  config_database_manager_dest.put("key", expected_value); // check that file is not overwritten by source
  // insert config
  config_database_manager_dest.insertConfig(config_database_manager_source);

  bool value_available = false;
  int ret_value = config_database_manager_dest.getInt("key", 0, value_available);
  EXPECT_EQ(expected_value, ret_value);
  EXPECT_TRUE(value_available);
}

TEST(ConfigDatabaseManager_Tests, insertConfig_filtering_deactivated) {
  ConfigDatabaseManager config_database_manager_dest("dest", false);
  ConfigDatabaseManager config_database_manager_source("source", false);

  int expected_value = 1;
  config_database_manager_source.put("key", expected_value); // add value to source
  // insert config
  config_database_manager_dest.insertConfig(config_database_manager_source);

  bool value_available = false;
  int ret_value = config_database_manager_dest.getInt("key", 0, value_available);
  EXPECT_EQ(expected_value, ret_value);
  EXPECT_TRUE(value_available);
}

TEST(ConfigDatabaseManager_Tests, insertConfig_filtering_activated) {
  ConfigDatabaseManager config_database_manager_dest("dest", true);
  ConfigDatabaseManager config_database_manager_source("source", false);

  int expected_value = 1;
  config_database_manager_source.put("comp", expected_value);
  // source is not inserted because "comp" doesn't exist in "config_database_manager_dest"
  config_database_manager_dest.insertConfig(config_database_manager_source);

  bool value_available = true;
  int ret_value = config_database_manager_dest.getInt("comp", -1, value_available);
  EXPECT_EQ(ret_value, -1);
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, get_int_non_existing_component_default_value) {
  ConfigDatabaseManager config_database_manager("test");

  int default_value = 1;
  bool value_available = true;
  int ret_value = config_database_manager.getInt("key", default_value, value_available);
  EXPECT_EQ(default_value, ret_value);
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, get_int_valid_value) {
  ConfigDatabaseManager config_database_manager("test");

  int expected_value = 1;
  config_database_manager.put("comp", expected_value);

  {
    bool value_available = false;
    int ret_value = config_database_manager.getInt("comp", 0, value_available);
    EXPECT_EQ(expected_value, ret_value);
    EXPECT_TRUE(value_available);
  }
  {
    int ret_value = config_database_manager.getInt("comp");
    EXPECT_EQ(expected_value, ret_value);
  }
}

TEST(ConfigDatabaseManager_Tests, get_float_non_existing_component_default_value) {
  ConfigDatabaseManager config_database_manager("test");

  float default_value = -1.1f;
  bool value_available = true;
  float ret_value = config_database_manager.getFloat("key", default_value, value_available);
  EXPECT_EQ(default_value, ret_value);
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, get_float_valid_value) {
  ConfigDatabaseManager config_database_manager("test");

  float expected_value = 3.2f;
  config_database_manager.put("comp", expected_value);

  {
    bool value_available = false;
    float ret_value = config_database_manager.getFloat("comp", 0, value_available);
    EXPECT_EQ(expected_value, ret_value);
    EXPECT_TRUE(value_available);
  }
  {
    float ret_value = config_database_manager.getFloat("comp");
    EXPECT_EQ(expected_value, ret_value);
  }
}

TEST(ConfigDatabaseManager_Tests, get_string_non_existing_component_default_value) {
  ConfigDatabaseManager config_database_manager("test");

  std::string default_value = "test";
  bool value_available = true;
  std::string ret_value = config_database_manager.getString("key", default_value, value_available);
  EXPECT_EQ(default_value, ret_value);
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, get_string_valid_value) {
  ConfigDatabaseManager config_database_manager("test");

  std::string expected_value = "test";
  config_database_manager.put("comp:a", expected_value);

  {
    bool value_available = false;
    std::string ret_value = config_database_manager.getString("comp:a", "empty", value_available);
    EXPECT_EQ(expected_value, ret_value);
    EXPECT_TRUE(value_available);
  }
  {
    std::string ret_value = config_database_manager.getString("comp:a");
    EXPECT_EQ(expected_value, ret_value);
  }
}

TEST(ConfigDatabaseManager_Tests, get_bool_non_existing_component_default_value) {
  ConfigDatabaseManager config_database_manager("test");

  bool default_value = true;
  bool value_available = true;
  bool ret_value = config_database_manager.getBool("key", default_value, value_available);
  EXPECT_EQ(default_value, ret_value);
  EXPECT_FALSE(value_available);
}

TEST(ConfigDatabaseManager_Tests, get_bool_valid_value) {
  ConfigDatabaseManager config_database_manager("test");

  bool expected_value = "test";
  config_database_manager.put("comp:a", expected_value);

  {
    bool value_available = false;
    bool ret_value = config_database_manager.getBool("comp:a", 0, value_available);
    EXPECT_EQ(expected_value, ret_value);
    EXPECT_TRUE(value_available);
  }
  {
    bool ret_value = config_database_manager.getBool("comp:a");
    EXPECT_EQ(expected_value, ret_value);
  }
}

TEST(ConfigDatabaseManager_Tests, getChildrenKeys_non_existing_component) {
  ConfigDatabaseManager config_database_manager("test");

  bool comp_available = true;
  std::vector<std::string> ret_value =
      config_database_manager.getChildrenKeys("compNotExisting", false, comp_available);
  EXPECT_TRUE(ret_value.empty());
  EXPECT_FALSE(comp_available);
}

TEST(ConfigDatabaseManager_Tests, getChildrenKeys_valid_keys) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a", expected_value1);
  std::string expected_value2 = "test2";
  config_database_manager.put("comp:b", expected_value2);

  // only nodes
  {
    std::vector<std::string> ret_value = config_database_manager.getChildrenKeys("comp");
    ASSERT_EQ(ret_value.size(), 2);
    EXPECT_EQ(ret_value.at(0), "a");
    EXPECT_EQ(ret_value.at(1), "b");
  }
  // full path
  {
    bool comp_available = false;
    std::vector<std::string> ret_value = config_database_manager.getChildrenKeys("comp", true, comp_available);
    ASSERT_TRUE(comp_available);
    ASSERT_EQ(ret_value.size(), 2);
    EXPECT_EQ(ret_value.at(0), "comp:a");
    EXPECT_EQ(ret_value.at(1), "comp:b");
  }
}

TEST(ConfigDatabaseManager_Tests, getValuesAsStringList_non_existing_component) {
  ConfigDatabaseManager config_database_manager("test");

  bool comp_available = true;
  std::vector<std::string> defaults = {"a"};
  std::vector<std::string> ret_value =
      config_database_manager.getValuesAsStringList("compNotExisting", defaults, comp_available);
  ASSERT_EQ(ret_value.size(), 1);
  EXPECT_EQ(ret_value.at(0), "a");
  EXPECT_FALSE(comp_available);
}

TEST(ConfigDatabaseManager_Tests, getValuesAsStringList_valid_keys) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[0]", expected_value1);
  std::string expected_value2 = "test2";
  config_database_manager.put("comp:a[1]", expected_value2);

  // default input parameters
  {
    std::vector<std::string> ret_value = config_database_manager.getValuesAsStringList("comp:a");
    ASSERT_EQ(ret_value.size(), 2);
    EXPECT_EQ(ret_value.at(0), "test1");
    EXPECT_EQ(ret_value.at(1), "test2");
  }
  // all input parameters defined
  {
    bool comp_available = false;
    std::vector<std::string> defaults = {"a"};
    std::vector<std::string> ret_value =
        config_database_manager.getValuesAsStringList("comp:a", defaults, comp_available);
    ASSERT_TRUE(comp_available);
    ASSERT_EQ(ret_value.size(), 2);
    EXPECT_EQ(ret_value.at(0), "test1");
    EXPECT_EQ(ret_value.at(1), "test2");
  }
}

TEST(ConfigDatabaseManager_Tests, getCompData_non_existing_component) {
  ConfigDatabaseManager config_database_manager("test");

  bool comp_available = true;
  std::map<std::string, std::string> ret_value = config_database_manager.getCompData("compNotExisting", comp_available);
  ASSERT_TRUE(ret_value.empty());
  EXPECT_FALSE(comp_available);
}

TEST(ConfigDatabaseManager_Tests, getCompData_full_key_path_return_no_data) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:a", 1);

  bool comp_available = true;
  // using the path to a specific config value to get the component data is not supported -> will return empty
  // -> use only component name instead
  std::map<std::string, std::string> ret_value = config_database_manager.getCompData("comp:a", comp_available);
  ASSERT_TRUE(ret_value.empty());
  EXPECT_FALSE(comp_available);
}

TEST(ConfigDatabaseManager_Tests, getCompData_valid_comp) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[0]", expected_value1);
  std::string expected_value2 = "test2";
  config_database_manager.put("comp:a[1]", expected_value2);

  // default input parameters
  {
    std::map<std::string, std::string> ret_value = config_database_manager.getCompData("comp");
    ASSERT_EQ(ret_value.size(), 2);
    EXPECT_EQ(ret_value.at("comp:a[0]"), "test1");
    EXPECT_EQ(ret_value.at("comp:a[1]"), "test2");
  }
  // all input parameters defined
  {
    bool comp_available = false;
    std::map<std::string, std::string> ret_value = config_database_manager.getCompData("comp", comp_available);
    ASSERT_TRUE(comp_available);
    ASSERT_EQ(ret_value.size(), 2);
    EXPECT_EQ(ret_value.at("comp:a[0]"), "test1");
    EXPECT_EQ(ret_value.at("comp:a[1]"), "test2");
  }
}

TEST(ConfigDatabaseManager_Tests, isEmpty_non_existing_component) {
  ConfigDatabaseManager config_database_manager("test");

  bool comp_available = true;
  bool ret_value = config_database_manager.isCompEmpty("compNotExisting", comp_available);
  EXPECT_TRUE(ret_value);
  EXPECT_FALSE(comp_available);
}

TEST(ConfigDatabaseManager_Tests, isEmpty_valid_comp) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[0]", expected_value1);

  // default input parameters
  {
    bool ret_value = config_database_manager.isCompEmpty("comp");
    EXPECT_FALSE(ret_value);
  }
  // all input parameters defined
  {
    bool comp_available = false;
    bool ret_value = config_database_manager.isCompEmpty("comp", comp_available);
    EXPECT_FALSE(ret_value);
    EXPECT_TRUE(comp_available);
  }
}

TEST(ConfigDatabaseManager_Tests, configValueExists_non_existing_value) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[0]", expected_value1);

  // non existing component
  {
    bool comp_available = true;
    bool ret_value = config_database_manager.configValueExists("compNotExisting", comp_available);
    EXPECT_FALSE(ret_value);
    EXPECT_FALSE(comp_available);
  }
  // existing component but config value doesn't exist
  {
    bool comp_available = false;
    bool ret_value = config_database_manager.configValueExists("comp", comp_available);
    EXPECT_FALSE(ret_value);
    EXPECT_TRUE(comp_available);
  }
}

TEST(ConfigDatabaseManager_Tests, configValueExists_valid_comp) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[0]", expected_value1);

  // default input parameters
  // Hint: array index 0 is handled equal to a single config value
  {
    bool ret_value = config_database_manager.configValueExists("comp:a");
    EXPECT_TRUE(ret_value);
  }
  // all input parameters defined
  {
    bool comp_available = false;
    bool ret_value = config_database_manager.configValueExists("comp:a", comp_available);
    EXPECT_TRUE(ret_value);
    EXPECT_TRUE(comp_available);
  }
}

TEST(ConfigDatabaseManager_Tests, keyInfo_non_existing_value) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[0]", expected_value1);

  // non existing component
  {
    bool final_leaf = true;
    size_t array_size = 1;
    bool comp_available = true;
    bool ret_value = config_database_manager.keyInfo("compNotExisting", final_leaf, array_size, comp_available);
    EXPECT_FALSE(ret_value);
    EXPECT_FALSE(final_leaf);
    EXPECT_EQ(array_size, 0);
    EXPECT_FALSE(comp_available);
  }
  // existing component but config value doesn't exist
  {
    bool final_leaf = true;
    size_t array_size = 1;
    bool ret_value = config_database_manager.keyInfo("comp", final_leaf, array_size);
    EXPECT_FALSE(ret_value);
    EXPECT_FALSE(final_leaf);
    EXPECT_EQ(array_size, 0);
  }
}

TEST(ConfigDatabaseManager_Tests, keyInfo_valid_comp) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[1]", expected_value1);

  // default input parameters
  // Hint: array index 0 is handled equal to a single config value
  {
    bool final_leaf = false;
    size_t array_size = 0;
    bool ret_value = config_database_manager.keyInfo("comp:a", final_leaf, array_size);
    EXPECT_TRUE(ret_value);
    EXPECT_TRUE(final_leaf);
    EXPECT_EQ(array_size, 2);
  }
  // all input parameters defined
  {
    bool final_leaf = false;
    size_t array_size = 0;
    bool comp_available = false;
    bool ret_value = config_database_manager.keyInfo("comp:a", final_leaf, array_size, comp_available);
    EXPECT_TRUE(ret_value);
    EXPECT_TRUE(comp_available);
    EXPECT_TRUE(final_leaf);
    EXPECT_EQ(array_size, 2);
  }
}

TEST(ConfigDatabaseManager_Tests, getOneCompConfigAsJsonString_non_existing) {
  ConfigDatabaseManager config_database_manager("test");
  std::string expected_value1 = "test1";
  config_database_manager.put("comp:a[0]", expected_value1);

  // non existing component
  {
    bool comp_available = true;
    std::string ret_value = config_database_manager.getOneCompConfigAsJsonString("compNotExisting", "", comp_available);
    EXPECT_TRUE(ret_value.empty());
    EXPECT_FALSE(comp_available);
  }
  // non existing key
  {
    bool comp_available = false;
    std::string ret_value = config_database_manager.getOneCompConfigAsJsonString("comp", "comp:b", comp_available);
    EXPECT_EQ(ret_value, "null\n"); // empty string "" is not a valid json
    EXPECT_TRUE(comp_available);
  }
}

TEST(ConfigDatabaseManager_Tests, getOneCompConfigAsJsonString_full_key_path_return_no_data) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:a", 1);

  std::string ret_value = config_database_manager.getOneCompConfigAsJsonString("comp:a", "comp:a");
  ASSERT_TRUE(ret_value.empty());
}

TEST(ConfigDatabaseManager_Tests, getOneCompConfigAsJsonString_valid_comp) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:value1", 1);
  config_database_manager.put("comp2:value1", 2); // shall not be considered

  Json::Value expected_config_value;
  expected_config_value["value1"] = "1";

  Json::Value expected_comp;
  expected_comp["comp"] = expected_config_value;
  Json::FastWriter fastWriter;
  std::string expected_comp_json = fastWriter.write(expected_comp);

  // default input parameters
  {
    std::string ret_value = config_database_manager.getOneCompConfigAsJsonString("comp", "");
    EXPECT_EQ(ret_value, expected_comp_json);
  }
  // all input parameters defined
  {
    bool comp_available = false;
    std::string ret_value = config_database_manager.getOneCompConfigAsJsonString("comp", "comp:value1", comp_available);
    EXPECT_EQ(ret_value, expected_comp_json);
    EXPECT_TRUE(comp_available);
  }
}

TEST(ConfigDatabaseManager_Tests, getAllCompConfigsAsJsonString_full_tree) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:value1:a", 1);
  config_database_manager.put("comp:value2:a", 2);
  config_database_manager.put("comp2:value1:a", 3);

  Json::Value expected_config_comp_value_1;
  expected_config_comp_value_1["a"] = "1";

  Json::Value expected_config_comp_value_2;
  expected_config_comp_value_2["a"] = "2";

  Json::Value expected_comp;
  expected_comp["value1"] = expected_config_comp_value_1;
  expected_comp["value2"] = expected_config_comp_value_2;

  Json::Value expected_config_comp2_value_1;
  expected_config_comp2_value_1["a"] = "3";

  Json::Value expected_comp2;
  expected_comp2["value1"] = expected_config_comp2_value_1;

  Json::Value expected_tree;
  expected_tree["comp"] = expected_comp;
  expected_tree["comp2"] = expected_comp2;
  Json::FastWriter fastWriter;
  std::string expected_tree_json = fastWriter.write(expected_tree);

  std::string ret_value = config_database_manager.getAllCompConfigsAsJsonString();
  EXPECT_EQ(ret_value, expected_tree_json);
}

TEST(ConfigDatabaseManager_Tests, getAllCompConfigsAsJsonString_part_of_tree) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:value1:a", 1);
  config_database_manager.put("comp:value2:a", 2);
  config_database_manager.put("comp2:value1:a", 3);

  Json::Value expected_config_comp_value_1;
  expected_config_comp_value_1["a"] = "1";

  Json::Value expected_config_comp_value_2;
  expected_config_comp_value_2["a"] = "2";

  Json::Value expected_comp;
  expected_comp["value1"] = expected_config_comp_value_1;
  expected_comp["value2"] = expected_config_comp_value_2;

  Json::Value expected_tree;
  expected_tree["comp"] = expected_comp;
  Json::FastWriter fastWriter;
  std::string expected_tree_json = fastWriter.write(expected_tree);

  std::string ret_value = config_database_manager.getAllCompConfigsAsJsonString("comp");
  EXPECT_EQ(ret_value, expected_tree_json);
}

TEST(ConfigDatabaseManager_Tests, get_differences) {
  ConfigDatabaseManager config_database_manager("test");
  // invalid urls are ignored
  config_database_manager.putCfg("node_cfg.json");

  // check that differences are properly returned even when filtering is active
  config_database_manager.enableFilterByComponent();

  // read diff file
  std::ifstream node_diff_file("node_cfg_diff.json");
  std::ostringstream ss;
  ss << node_diff_file.rdbuf();

  // calculate diff
  std::string diff_string = config_database_manager.getDifferences(ss.str());

  // insert string to comp data for easier evaluation
  ConfigDatabaseManager config_database_manager_diff("diff");
  config_database_manager_diff.insertJsonString(diff_string);

  // check component with differences
  std::vector<std::string> comp_names = config_database_manager_diff.getConfigCompNames();
  EXPECT_EQ(comp_names.size(), 3);
  EXPECT_EQ(comp_names.at(0), "CEM200_COH");
  EXPECT_EQ(comp_names.at(1), "list_check_test");
  EXPECT_EQ(comp_names.at(2), "localnew");

  // check content of differences
  std::map<std::string, std::string> diff_data_list_check_test =
      config_database_manager_diff.getCompData("list_check_test");

  // arrays on intermediate level are ignored like list_check_test:object_array[0]:dummy
  EXPECT_EQ(diff_data_list_check_test.size(), 3);
  EXPECT_EQ(diff_data_list_check_test["list_check_test:normal_variableNew"], "val");
  EXPECT_EQ(diff_data_list_check_test["list_check_test:normal_variable"], "val1New");
  EXPECT_EQ(diff_data_list_check_test["list_check_test:array_value[1]"], "val2New");

  std::map<std::string, std::string> diff_data_localnew = config_database_manager_diff.getCompData("localnew");
  EXPECT_EQ(diff_data_localnew.size(), 2);
  EXPECT_EQ(diff_data_localnew["localnew:active_state"], "init");
  EXPECT_EQ(diff_data_localnew["localnew:active_name"], "CEM200_COH");

  std::map<std::string, std::string> diff_data_CEM200_COH = config_database_manager_diff.getCompData("CEM200_COH");
  EXPECT_EQ(diff_data_CEM200_COH.size(), 23);
  EXPECT_EQ(diff_data_CEM200_COH["CEM200_COH:version"], "0 .0.1");
  EXPECT_EQ(diff_data_CEM200_COH["CEM200_COH:in_portNew:m_pEmlInputIf:sync:sync_type"], "timestamp");
  EXPECT_EQ(diff_data_CEM200_COH["CEM200_COH:composition:alternates:m_pEmlInputIfNew[0]"],
            "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(diff_data_CEM200_COH["CEM200_COH:composition:alternates:m_pEmlInputIfNew[1]"],
            "CEM200_FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(diff_data_CEM200_COH["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectListNew");
}

TEST(ConfigDatabaseManager_Tests, getRootName) {
  ConfigDatabaseManager config_database_manager("test");
  EXPECT_EQ(config_database_manager.getRootName(), "test");
}

TEST(ConfigDatabaseManager_Tests, getConfigCompNames_empty) {
  ConfigDatabaseManager config_database_manager("test");

  std::vector<std::string> comp_names = config_database_manager.getConfigCompNames();
  EXPECT_TRUE(comp_names.empty());
}

TEST(ConfigDatabaseManager_Tests, getConfigCompNames_valid) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:a:b", 1);
  config_database_manager.put("comp2:a:b", 1);

  std::vector<std::string> comp_names = config_database_manager.getConfigCompNames();
  EXPECT_EQ(comp_names.at(0), "comp");
  EXPECT_EQ(comp_names.at(1), "comp2");
}

TEST(ConfigDatabaseManager_Tests, isComponentInConfig_not_existing) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:a:b", 1);

  EXPECT_FALSE(config_database_manager.isComponentInConfig("test"));
}

TEST(ConfigDatabaseManager_Tests, getConfigCompNames_existing) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:a:b", 1);
  config_database_manager.put("comp2:a:b", 1);

  EXPECT_TRUE(config_database_manager.isComponentInConfig("comp2"));
}

TEST(ConfigDatabaseManager_Tests, addNewComponentConfig_added) {
  ConfigDatabaseManager config_database_manager("test", true);
  EXPECT_TRUE(config_database_manager.addNewComponentConfig("comp"));

  EXPECT_TRUE(config_database_manager.isComponentInConfig("comp"));
}

TEST(ConfigDatabaseManager_Tests, addNewComponentConfig_already_existing) {
  ConfigDatabaseManager config_database_manager("test", true);
  EXPECT_TRUE(config_database_manager.addNewComponentConfig("comp"));
  EXPECT_FALSE(config_database_manager.addNewComponentConfig("comp"));

  EXPECT_TRUE(config_database_manager.isComponentInConfig("comp"));
}

TEST(ConfigDatabaseManager_Tests, addNewComponentConfig_invalid_comp_name) {
  ConfigDatabaseManager config_database_manager("test", true);
  EXPECT_FALSE(config_database_manager.addNewComponentConfig("invalid_comp_name[0]"));

  EXPECT_FALSE(config_database_manager.isComponentInConfig("invalid_comp_name[0]"));
}

TEST(ConfigDatabaseManager_Tests, check_filtering) {
  ConfigDatabaseManager config_database_manager("test", true);
  // check that value is not added
  config_database_manager.put("comp:a", 1);
  EXPECT_FALSE(config_database_manager.configValueExists("comp:a"));

  // check that filtering new components is deactived
  config_database_manager.disableFilterByComponent();
  config_database_manager.put("comp:a", 1);
  EXPECT_TRUE(config_database_manager.configValueExists("comp:a"));

  // check that filtering new components is active again
  config_database_manager.enableFilterByComponent();
  config_database_manager.put("comp2:a", 1);
  EXPECT_FALSE(config_database_manager.configValueExists("comp2:a"));
}

TEST(ConfigDatabaseManager_Tests, get_and_set_status_filtering) {
  ConfigDatabaseManager config_database_manager("test", true);

  config_database_manager.setFilterByComponent(true);
  EXPECT_TRUE(config_database_manager.getFilterByComponent());
  config_database_manager.setFilterByComponent(false);
  EXPECT_FALSE(config_database_manager.getFilterByComponent());
}

TEST(ConfigDatabaseManager_Tests, clear_comp) {
  ConfigDatabaseManager config_database_manager("test");
  config_database_manager.put("comp:a[1]", 1);
  config_database_manager.put("comp2", 2);

  config_database_manager.clear();
  std::vector<std::string> comp_names = config_database_manager.getConfigCompNames();
  EXPECT_TRUE(comp_names.empty());
}

} // namespace appsupport
} // namespace next
