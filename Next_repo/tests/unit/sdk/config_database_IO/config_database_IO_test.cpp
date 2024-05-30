/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     config_database_IO_tests.cpp
 *  @brief    Testing the config database IO class
 */

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <json/json.h>

#include <next/sdk/sdk.hpp>

#include "config_database_IO.hpp"

namespace next {
namespace appsupport {

TEST(ConfigDatabaseIO_Tests, put_cfg_json) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  // invalid urls are ignored
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data);

  EXPECT_EQ(map_config_comp_data.size(), 3);

  auto map_list_check_test_config_data = map_config_comp_data.at("list_check_test").getCompData();
  // arrays on intermediate level are ignored like list_check_test:object_array[0]:dummy
  EXPECT_EQ(map_list_check_test_config_data.size(), 5);
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:normal_variable"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[0]"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[1]"], "val2");
  // empty array will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_empty[0]"], "");
  // empty json will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:empty_json"], "");

  auto map_local_config_data = map_config_comp_data.at("local").getCompData();
  EXPECT_EQ(map_local_config_data.size(), 2);
  EXPECT_EQ(map_local_config_data["local:active_state"], "init");
  EXPECT_EQ(map_local_config_data["local:active_name"], "CEM200_COH");

  auto map_CEM_config_data = map_config_comp_data.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 33);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:version"], "0.0.1");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[0]"], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:out_port:m_dcrInterface:port_vaddr"], "0x9000");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectList");
}

TEST(ConfigDatabaseIO_Tests, put_cfg_xml) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  ConfigDatabaseIO::putCfg("test.xml", map_config_comp_data);

  auto map_CEM_config_data = map_config_comp_data.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 6);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:Settings:ID"], "1");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:Settings:LogLevel"], "debug");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:Settings:Position:X"], "2.5");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:Settings:Position:Z"], "0");
  // when key exists multiple times, previous one are overwritten by last one
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:Settings:Banlist:IP"], "192.168.1.0");
}

TEST(ConfigDatabaseIO_Tests, put_cfg_xml_with_attributes_at_nodes) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  ConfigDatabaseIO::putCfg("test_nodes_with_attr.xml", map_config_comp_data);

  ASSERT_EQ(map_config_comp_data.size(), 1);
  auto map_CEM_config_data = map_config_comp_data.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 5);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:node_attr:<xmlattr>:attr1"], "test");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:node_attr:<xmlattr>:attr2"], "test2");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:node_attr:elem_attr:<xmlattr>:attr1"], "1");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:node_attr:elem_attr:<xmlattr>:attr2"], "2");
  // node is ignored beause elem_attr is not a final node and values at this level are not supported by json
  EXPECT_FALSE(map_CEM_config_data.find("CEM200_COH:node_attr:elem_attr") != map_CEM_config_data.end());
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:node_attr:elem_simple"], "val2");
}

TEST(ConfigDatabaseIO_Tests, put_cfg_wrong_format) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  ConfigDatabaseIO::putCfg("test.x", map_config_comp_data);

  EXPECT_EQ(map_config_comp_data.size(), 0);
}

TEST(ConfigDatabaseIO_Tests, put_cfg_file_not_existing) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  ConfigDatabaseIO::putCfg("not_existing.xml", map_config_comp_data);

  EXPECT_EQ(map_config_comp_data.size(), 0);
}

TEST(ConfigDatabaseIO_Tests, put_cfg_not_supported_entries) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  ConfigDatabaseIO::putCfg("node_cfg_invalid.json", map_config_comp_data);

  ASSERT_EQ(map_config_comp_data.size(), 2);

  auto map_local_config_data = map_config_comp_data.at("local").getCompData();
  EXPECT_TRUE(map_local_config_data.empty());

  auto map_nested_config_data = map_config_comp_data.at("nested_arrays").getCompData();
  EXPECT_EQ(map_nested_config_data.size(), 4);
  EXPECT_EQ(map_nested_config_data["nested_arrays:normal_variable"], "val1");
  // no index added because only one valid element found for nested_array1
  EXPECT_EQ(map_nested_config_data["nested_arrays:nested_array1"], "entry_elem1");
  // index added because nested_array2 is specifiying an array
  EXPECT_EQ(map_nested_config_data["nested_arrays:nested_array2[0]"], "entry_elem1");
  EXPECT_EQ(map_nested_config_data["nested_arrays:nested_array2[3]"], "entry_elem2");
}

TEST(ConfigDatabaseIO_Tests, put_cfg_update) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  // invalid urls are ignored
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data);
  // insert additional config file. Insert new value, overwrite different values
  ConfigDatabaseIO::putCfg("node_cfg_diff.json", map_config_comp_data);

  EXPECT_EQ(map_config_comp_data.size(), 4);

  auto map_list_check_test_config_data = map_config_comp_data.at("list_check_test").getCompData();
  // arrays on intermediate level are ignored like list_check_test:object_array[0]:dummy
  EXPECT_EQ(map_list_check_test_config_data.size(), 6);
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:normal_variable"], "val1New");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:normal_variableNew"], "val");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[0]"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[1]"], "val2New");
  // empty array will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_empty[0]"], "");
  // empty json will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:empty_json"], "");

  auto map_local_config_data = map_config_comp_data.at("local").getCompData();
  EXPECT_EQ(map_local_config_data.size(), 2);
  EXPECT_EQ(map_local_config_data["local:active_state"], "init");

  auto map_localnew_config_data = map_config_comp_data.at("localnew").getCompData();
  EXPECT_EQ(map_localnew_config_data.size(), 2);
  EXPECT_EQ(map_localnew_config_data["localnew:active_state"], "init");

  auto map_CEM_config_data = map_config_comp_data.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 54);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:version"], "0 .0.1");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[0]"], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:in_port:m_pSefInputIf:port_version"], "0.0.1");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:in_portNew:m_pSefInputIf:port_versionNew"], "0.0.1");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:out_port:m_dcrInterface:port_vaddr"], "0x9000");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pEmlInputIfNew[1]"],
            "CEM200_FCU.ARSSepTuple3_Fcu");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectListNew");
}

TEST(ConfigDatabaseIO_Tests, put_cfg_filtering) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  // invalid urls are ignored
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data);
  // insert additional config file (insert only already existing componentes)
  ConfigDatabaseIO::putCfg("node_cfg_diff.json", map_config_comp_data, true);

  EXPECT_EQ(map_config_comp_data.size(), 3);

  auto map_list_check_test_config_data = map_config_comp_data.at("list_check_test").getCompData();
  // arrays on intermediate level are ignored like list_check_test:object_array[0]:dummy
  EXPECT_EQ(map_list_check_test_config_data.size(), 6);
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:normal_variable"], "val1New");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:normal_variableNew"], "val");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[0]"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[1]"], "val2New");
  // empty array will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_empty[0]"], "");
  // empty json will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:empty_json"], "");

  auto map_local_config_data = map_config_comp_data.at("local").getCompData();
  EXPECT_EQ(map_local_config_data.size(), 2);
  EXPECT_EQ(map_local_config_data["local:active_state"], "init");

  // filtered out component
  EXPECT_TRUE(map_config_comp_data.find("localnew") == map_config_comp_data.end());

  auto map_CEM_config_data = map_config_comp_data.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 54);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:version"], "0 .0.1");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[0]"], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:in_port:m_pSefInputIf:port_version"], "0.0.1");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:in_portNew:m_pSefInputIf:port_versionNew"], "0.0.1");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:out_port:m_dcrInterface:port_vaddr"], "0x9000");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pEmlInputIfNew[1]"],
            "CEM200_FCU.ARSSepTuple3_Fcu");

  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectListNew");
}

TEST(ConfigDatabaseIO_Tests, insertJsonString_new) {
  std::ifstream jsonFile("node_cfg.json");
  std::ostringstream ss;
  ss << jsonFile.rdbuf();

  std::map<std::string, ConfigCompData> map_config_comp_data;
  // invalid urls are ignored
  ConfigDatabaseIO::insertJsonString(ss.str(), map_config_comp_data);
  EXPECT_EQ(map_config_comp_data.size(), 3);

  auto map_list_check_test_config_data = map_config_comp_data.at("list_check_test").getCompData();
  // arrays on intermediate level are ignored like list_check_test:object_array[0]:dummy
  EXPECT_EQ(map_list_check_test_config_data.size(), 5);
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:normal_variable"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[0]"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[1]"], "val2");
  // empty array will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_empty[0]"], "");
  // empty json will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:empty_json"], "");

  auto map_local_config_data = map_config_comp_data.at("local").getCompData();
  EXPECT_EQ(map_local_config_data.size(), 2);
  EXPECT_EQ(map_local_config_data["local:active_state"], "init");
  EXPECT_EQ(map_local_config_data["local:active_name"], "CEM200_COH");

  auto map_CEM_config_data = map_config_comp_data.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 33);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:version"], "0.0.1");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[0]"], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:out_port:m_dcrInterface:port_vaddr"], "0x9000");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectList");
}

TEST(ConfigDatabaseIO_Tests, insertJsonString_empty) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  ConfigDatabaseIO::insertJsonString("", map_config_comp_data);

  EXPECT_EQ(map_config_comp_data.size(), 0);
}

TEST(ConfigDatabaseIO_Tests, insertJsonString_invalid_node_names) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  std::string json_string_invalid_content = "{\"array[0]\":{\"sample_1\":\"init\"},\"\":{\"empty_object\":1}}";
  ConfigDatabaseIO::insertJsonString(json_string_invalid_content, map_config_comp_data);

  EXPECT_EQ(map_config_comp_data.size(), 0);
}

TEST(ConfigDatabaseIO_Tests, insertConfigCompData_emptyCompData) {
  std::map<std::string, ConfigCompData> map_config_comp_data_source;
  std::map<std::string, ConfigCompData> map_config_comp_data_destination;

  ConfigDatabaseIO::insertConfigCompData(map_config_comp_data_source, map_config_comp_data_destination, false);

  EXPECT_EQ(map_config_comp_data_destination.size(), 0);
}

TEST(ConfigDatabaseIO_Tests, insertConfigCompData_AddOneValueWithoutFilteringNewComp) {
  std::map<std::string, ConfigCompData> map_config_comp_data_source;
  std::map<std::string, ConfigCompData> map_config_comp_data_destination;

  map_config_comp_data_source.emplace("comp", "comp");
  map_config_comp_data_source.at("comp").put("value", 2);

  ConfigDatabaseIO::insertConfigCompData(map_config_comp_data_source, map_config_comp_data_destination, false);

  ASSERT_FALSE(map_config_comp_data_destination.at("comp").isEmpty());
  EXPECT_EQ(map_config_comp_data_destination.at("comp").getInt("value", 0), 2);
}

TEST(ConfigDatabaseIO_Tests, insertConfigCompData_ReplacingOneValueFilteringActive) {
  std::map<std::string, ConfigCompData> map_config_comp_data_source;
  std::map<std::string, ConfigCompData> map_config_comp_data_destination;

  map_config_comp_data_destination.emplace("comp", "comp");
  map_config_comp_data_destination.at("comp").put("value", 1);

  map_config_comp_data_source.emplace("comp", "comp");
  map_config_comp_data_source.at("comp").put("value", 2);

  ConfigDatabaseIO::insertConfigCompData(map_config_comp_data_source, map_config_comp_data_destination, true);

  ASSERT_FALSE(map_config_comp_data_destination.at("comp").isEmpty());
  EXPECT_EQ(map_config_comp_data_destination.at("comp").getInt("value", 0), 2);
}

TEST(ConfigDatabaseIO_Tests, insertConfigCompData_FilteringActiveNoneExistingCompIgnored) {
  std::map<std::string, ConfigCompData> map_config_comp_data_source;
  std::map<std::string, ConfigCompData> map_config_comp_data_destination;

  map_config_comp_data_destination.emplace("comp", "comp");
  map_config_comp_data_destination.at("comp").put("value", 1);

  // new component which shall be ignored
  // use key name which is at the first place in the map to test skipping it in insertConfigCompData
  map_config_comp_data_source.emplace("coma", "comp");
  map_config_comp_data_source.at("coma").put("value", 2);

  // already existing component whose values shall be added
  map_config_comp_data_source.emplace("comp", "comp");
  map_config_comp_data_source.at("comp").put("value2", 2);

  ConfigDatabaseIO::insertConfigCompData(map_config_comp_data_source, map_config_comp_data_destination, true);

  ASSERT_FALSE(map_config_comp_data_destination.at("comp").isEmpty());
  EXPECT_EQ(map_config_comp_data_destination.at("comp").getInt("value", 0), 1);
  EXPECT_EQ(map_config_comp_data_destination.at("comp").getInt("value2", 0), 2);
  // component was filtered out
  ASSERT_TRUE(map_config_comp_data_destination.find("coma") == map_config_comp_data_destination.end());
}

TEST(ConfigDatabaseIO_Tests, insertConfigCompData_invalid_comp_names_ignored) {
  std::map<std::string, ConfigCompData> map_config_comp_data_source;
  std::map<std::string, ConfigCompData> map_config_comp_data_destination;

  // invalid comp names are ignored
  map_config_comp_data_source.emplace("comp[0]", "comp");
  map_config_comp_data_source.at("comp[0]").put("value", 2);

  ConfigDatabaseIO::insertConfigCompData(map_config_comp_data_source, map_config_comp_data_destination, false);

  ASSERT_TRUE(map_config_comp_data_destination.empty());
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_one_component_empty) {
  Json::Value empty;
  Json::FastWriter fastWriter;
  std::string empty_json = fastWriter.write(empty);

  ConfigCompData comp_data("Test");
  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(comp_data, "");
  EXPECT_EQ(res, empty_json);
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_multiple_component_empty) {
  Json::Value empty;
  Json::FastWriter fastWriter;
  std::string empty_json = fastWriter.write(empty);

  std::map<std::string, ConfigCompData> map_config_comp_data;
  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(map_config_comp_data, "");
  EXPECT_EQ(res, empty_json);
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_one_component_one_entry) {
  Json::Value singleValue;
  singleValue["value1"] = "1";
  Json::FastWriter fastWriter;
  std::string single_value_json = fastWriter.write(singleValue);

  ConfigCompData comp_data("Test");
  comp_data.put("value1", 1);
  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(comp_data, "");
  EXPECT_EQ(res, single_value_json);
  res = ConfigDatabaseIO::getCompConfigAsJsonString(comp_data, "value1");
  EXPECT_EQ(res, single_value_json);
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_one_component_config_value) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  // invalid urls are ignored
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data);

  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(map_config_comp_data.find("CEM200_COH")->second, "");

  std::map<std::string, ConfigCompData> map_config_comp_data_inserted;
  ConfigDatabaseIO::insertJsonString(res, map_config_comp_data_inserted);
  EXPECT_EQ(map_config_comp_data_inserted.size(), 1);

  auto map_CEM_config_data = map_config_comp_data_inserted.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 33);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:version"], "0.0.1");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[0]"], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:out_port:m_dcrInterface:port_vaddr"], "0x9000");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectList");
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_multiple_component) {
  std::map<std::string, ConfigCompData> map_config_comp_data_cfg_file;
  // invalid urls are ignored
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data_cfg_file);

  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(map_config_comp_data_cfg_file, "");

  // insert string to comp data for easier evaluation
  std::map<std::string, ConfigCompData> map_config_comp_data_inserted;
  ConfigDatabaseIO::insertJsonString(res, map_config_comp_data_inserted);
  EXPECT_EQ(map_config_comp_data_inserted.size(), 3);

  auto map_list_check_test_config_data = map_config_comp_data_inserted.at("list_check_test").getCompData();
  // arrays on intermediate level are ignored like list_check_test:object_array[0]:dummy
  EXPECT_EQ(map_list_check_test_config_data.size(), 5);
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:normal_variable"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[0]"], "val1");
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_value[1]"], "val2");
  // empty array will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:array_empty[0]"], "");
  // empty json will be treated as one element with empty string
  EXPECT_EQ(map_list_check_test_config_data["list_check_test:empty_json"], "");

  auto map_local_config_data = map_config_comp_data_inserted.at("local").getCompData();
  EXPECT_EQ(map_local_config_data.size(), 2);
  EXPECT_EQ(map_local_config_data["local:active_state"], "init");
  EXPECT_EQ(map_local_config_data["local:active_name"], "CEM200_COH");

  auto map_CEM_config_data = map_config_comp_data_inserted.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 33);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:version"], "0.0.1");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[0]"], "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:m_pEmlInputIf[1]"], "CEM200_FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:out_port:m_dcrInterface:port_vaddr"], "0x9000");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectList");
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_subTree) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  // invalid urls are ignored
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data);

  // filter out only "CEM200_COH:in_port" node by preserving the tree structure
  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(map_config_comp_data, "CEM200_COH:in_port");

  // insert string to comp data for easier evaluation
  std::map<std::string, ConfigCompData> map_config_comp_data_inserted;
  ConfigDatabaseIO::insertJsonString(res, map_config_comp_data_inserted);
  EXPECT_EQ(map_config_comp_data_inserted.size(), 1);

  auto map_CEM_config_data = map_config_comp_data_inserted.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 19);
  EXPECT_TRUE(map_CEM_config_data.find("CEM200_COH:version") == map_CEM_config_data.end());
  EXPECT_TRUE(map_CEM_config_data.find("CEM200_COH:m_pEmlInputIf[0]") == map_CEM_config_data.end());
  EXPECT_TRUE(map_CEM_config_data.find("CEM200_COH:m_pEmlInputIf[1]") == map_CEM_config_data.end());
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:in_port:m_pSefInputIf:sync:sync_type"], "timestamp");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:in_port:m_pEmlInputIf:sync:sync_type"], "timestamp");
  EXPECT_TRUE(map_CEM_config_data.find("CEM200_COH:out_port:m_dcrInterface:port_vaddr") == map_CEM_config_data.end());
  EXPECT_TRUE(map_CEM_config_data.find("CEM200_COH:composition:alternates:m_pSefInputIf[1]") ==
              map_CEM_config_data.end());
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_subTree_invalid_key) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data);

  // check non existing key
  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(map_config_comp_data, "CEM200_COH:in_portTest");

  Json::Value empty;
  Json::FastWriter fastWriter;
  std::string empty_json = fastWriter.write(empty);

  EXPECT_EQ(res, empty_json);
}

TEST(ConfigDatabaseIO_Tests, getCompConfig_with_single_value) {
  ConfigCompData comp_data("Test");
  comp_data.put("Test", true);
  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(comp_data, "");

  // insert string to comp data for easier evaluation
  std::map<std::string, ConfigCompData> map_config_comp_data_inserted;
  ConfigDatabaseIO::insertJsonString(res, map_config_comp_data_inserted);
  EXPECT_EQ(map_config_comp_data_inserted.size(), 1);

  auto map_CEM_config_data = map_config_comp_data_inserted.at("Test").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 1);
  EXPECT_EQ(map_CEM_config_data["Test"], "1");
}

TEST(ConfigDatabaseIO_Tests, getJson_ignored_values) {
  ConfigCompData comp_data("Test");
  // values which are not at leaf nodes, are ignored for json export
  // check that empty strings are considered as well
  comp_data.put("Test", 1);      // will be ignored for json export
  comp_data.put("Test:a", 2);    // will be ignored for json export
  comp_data.put("Test:c", "");   // will be ignored for json export
  comp_data.put("Test:c:f", ""); // check that empty string is not ignored
  comp_data.put("Test:d:e", 3);
  comp_data.put("Test:a[2]", 4); // will be ignored for json export
  comp_data.put("Test:a:b:c[2]", 5);
  // arrays on the middle level are ignored as invalid urls
  comp_data.put("Test:a[2]:z", 6); // will be ignored for json export
  comp_data.put("Test:a[3]:z", 7); // will be ignored for json export
  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(comp_data, "");

  Json::Value root;
  Json::Reader reader;
  reader.parse(res, root);

  EXPECT_FALSE(root["Test"].isInt());         // values which are not at the final leaf are ignored
  EXPECT_FALSE(root["Test"]["a"].isInt());    // values which are not at the final leaf are ignored
  EXPECT_FALSE(root["Test"]["c"].isString()); // values which are not at the final leaf are ignored
  EXPECT_EQ(root["Test"]["c"]["f"].asString(), "");
  EXPECT_EQ(root["Test"]["d"]["e"].asString(), "3");
  EXPECT_TRUE(root["Test"]["a"].isObject());
  EXPECT_FALSE(root["Test"]["a"].isArray());
  EXPECT_FALSE(root["Test"].isMember("a[2]"));
  EXPECT_TRUE(root["Test"]["a"]["b"]["c"].isArray());
  EXPECT_FALSE(root["Test"]["a"]["b"]["c"].isObject());
  EXPECT_EQ(root["Test"]["a"]["b"]["c"][0].asString(), "");
  EXPECT_EQ(root["Test"]["a"]["b"]["c"][1].asString(), "");
  EXPECT_EQ(root["Test"]["a"]["b"]["c"][2].asString(), "5");

  // insert string to check if that's supported and empty strings are not ignored
  std::map<std::string, ConfigCompData> map_config_comp_data_inserted;
  ConfigDatabaseIO::insertJsonString(res, map_config_comp_data_inserted);
  EXPECT_EQ(map_config_comp_data_inserted.size(), 1);

  auto map_CEM_config_data = map_config_comp_data_inserted.at("Test").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 5);
  EXPECT_EQ(map_CEM_config_data["Test:d:e"], "3");
  // check that empty string is considered
  EXPECT_EQ(map_CEM_config_data["Test:c:f"], "");
  // check that empty string for not specified array entries are considered
  EXPECT_EQ(map_CEM_config_data["Test:a:b:c[0]"], "");
  EXPECT_EQ(map_CEM_config_data["Test:a:b:c[1]"], "");
  EXPECT_EQ(map_CEM_config_data["Test:a:b:c[2]"], "5");
}

TEST(ConfigDatabaseIO_Tests, get_differences) {
  std::map<std::string, ConfigCompData> map_config_comp_data;
  // invalid urls are ignored
  ConfigDatabaseIO::putCfg("node_cfg.json", map_config_comp_data);

  std::ifstream node_diff_file("node_cfg_diff.json");
  std::ostringstream ss;
  ss << node_diff_file.rdbuf();

  // calculate diff
  std::string diff_string = ConfigDatabaseIO::getDifferences(ss.str(), map_config_comp_data);

  // insert string to comp data for easier evaluation
  std::map<std::string, ConfigCompData> map_config_comp_data_inserted;
  ConfigDatabaseIO::insertJsonString(diff_string, map_config_comp_data_inserted);
  EXPECT_EQ(map_config_comp_data_inserted.size(), 3);

  auto map_list_check_config_data = map_config_comp_data_inserted.at("list_check_test").getCompData();
  // arrays on intermediate level are ignored like list_check_test:object_array[0]:dummy
  EXPECT_EQ(map_list_check_config_data.size(), 3);
  EXPECT_EQ(map_list_check_config_data["list_check_test:normal_variableNew"], "val");
  EXPECT_EQ(map_list_check_config_data["list_check_test:normal_variable"], "val1New");
  EXPECT_EQ(map_list_check_config_data["list_check_test:array_value[1]"], "val2New");

  auto map_local_new_config_data = map_config_comp_data_inserted.at("localnew").getCompData();
  EXPECT_EQ(map_local_new_config_data.size(), 2);
  EXPECT_EQ(map_local_new_config_data["localnew:active_state"], "init");
  EXPECT_EQ(map_local_new_config_data["localnew:active_name"], "CEM200_COH");

  auto map_CEM_config_data = map_config_comp_data_inserted.at("CEM200_COH").getCompData();
  EXPECT_EQ(map_CEM_config_data.size(), 23);
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:version"], "0 .0.1");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:in_portNew:m_pEmlInputIf:sync:sync_type"], "timestamp");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pEmlInputIfNew[0]"],
            "MFC5xx Device.FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pEmlInputIfNew[1]"],
            "CEM200_FCU.ARSSepTuple3_Fcu");
  EXPECT_EQ(map_CEM_config_data["CEM200_COH:composition:alternates:m_pSefInputIf[1]"],
            "MFC5xx Device.TPF2.m_tpObjectListNew");
}

TEST(ConfigDatabaseIO_Tests, DISABLED_get_json_performance_test) {
  auto config_comp_data = ConfigCompData("Test");

  unsigned int size_keys = 100000;
  unsigned int level_increase = 10000;
  std::vector<std::string> key_list_int;
  key_list_int.resize(size_keys);

  std::vector<std::string> key_list_float;
  key_list_float.resize(size_keys);

  std::vector<std::string> key_list_string;
  key_list_string.resize(size_keys);

  std::vector<std::string> key_list_char_array;
  key_list_char_array.resize(size_keys);

  std::vector<std::string> key_list_bool;
  key_list_bool.resize(size_keys);

  std::string key_int = "testabcint";
  std::string key_float = "testabcfloat";
  std::string key_string = "testabcstring";
  std::string key_char_array = "testabcchar_array";
  std::string key_bool = "testabcbool";

  std::string string_value{"teststring"};

  for (unsigned int i = 0; i < size_keys; ++i) {
    if (i % level_increase == 0 && i != 0) {
      key_int += ":testnew";
      key_float += ":testnew";
      key_string += ":testnew";
      key_char_array += ":testnew";
      key_bool += ":testnew";
    }
    key_list_int[i] = key_int + std::to_string(i);
    key_list_float[i] = key_float + std::to_string(i);
    key_list_string[i] = key_string + std::to_string(i);
    key_list_char_array[i] = key_char_array + std::to_string(i);
    key_list_bool[i] = key_bool + std::to_string(i);
  }

  for (const auto &key_elem : key_list_int) {
    config_comp_data.put(key_elem, 10);
  }
  for (const auto &key_elem : key_list_float) {
    config_comp_data.put(key_elem, 20.0f);
  }
  for (const auto &key_elem : key_list_string) {
    config_comp_data.put(key_elem, string_value);
  }
  for (const auto &key_elem : key_list_char_array) {
    config_comp_data.put(key_elem, "teststring");
  }
  for (const auto &key_elem : key_list_bool) {
    config_comp_data.put(key_elem, true);
  }

  std::string res = ConfigDatabaseIO::getCompConfigAsJsonString(config_comp_data, "");
}

} // namespace appsupport
} // namespace next
