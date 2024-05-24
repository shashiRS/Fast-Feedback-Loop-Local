/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     config_comp_data_tests.cpp
 *  @brief    Testing the config database class
 */

#include <string>
#include <vector>

#include <boost/json.hpp>

#include <next/sdk/sdk.hpp>

#include "config_comp_data.hpp"

namespace next {
namespace appsupport {

TEST(ConfigCompDataTests, put_and_get_simple_values_and_overwrite) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int", 10);
  config_comp_data.put("test:tst_float", 20.0f);
  config_comp_data.put("test:tst_char_array", "teststring");
  const std::string lorem_ipsum{"Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt "
                                "ut labore et dolore magna aliqua."};
  config_comp_data.put("test:tst_string", lorem_ipsum);
  config_comp_data.put("tst_bool", true);

  bool value_exists = false;
  EXPECT_EQ(config_comp_data.getInt("test:tst_int", 0, value_exists), 10);
  EXPECT_TRUE(value_exists);

  value_exists = false;
  EXPECT_EQ(config_comp_data.getFloat("test:tst_float", 0.0f, value_exists), 20.0f);
  EXPECT_TRUE(value_exists);

  value_exists = false;
  EXPECT_EQ(config_comp_data.getString("test:tst_string", "", value_exists), lorem_ipsum);
  EXPECT_TRUE(value_exists);

  value_exists = false;
  EXPECT_EQ(config_comp_data.getString("test:tst_char_array", "", value_exists), "teststring");
  EXPECT_TRUE(value_exists);

  value_exists = false;
  EXPECT_EQ(config_comp_data.getBool("tst_bool", false, value_exists), true);
  EXPECT_TRUE(value_exists);

  // overwrite with different values
  config_comp_data.put("test:tst_int", -10);
  config_comp_data.put("test:tst_float", -20.0f);
  config_comp_data.put("test:tst_char_array", "newString");
  config_comp_data.put("test:tst_string", std::string("newString"));
  config_comp_data.put("tst_bool", false);

  EXPECT_EQ(config_comp_data.getInt("test:tst_int", 0), -10);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_float", 0.f), -20.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_string", ""), "newString");
  EXPECT_EQ(config_comp_data.getString("test:tst_char_array", ""), "newString");
  EXPECT_EQ(config_comp_data.getBool("tst_bool", true), false);

  // overwrite with empty value
  config_comp_data.put("test:tst_char_array", "");
  config_comp_data.put("test:tst_string", std::string(""));

  EXPECT_EQ(config_comp_data.getString("test:tst_string", "failed"), "");
  EXPECT_EQ(config_comp_data.getString("test:tst_char_array", "failed"), "");
}

TEST(ConfigCompDataTests, put_and_get_node_separator_in_config_value) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int", "test:tst_int");
  EXPECT_EQ(config_comp_data.getString("test:tst_int", ""), "test:tst_int");
}

TEST(ConfigCompDataTests, put_and_get_different_node_variations) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int", 10);

  // add value at root position, when tree exists already
  config_comp_data.put("test", 11);
  EXPECT_EQ(config_comp_data.getInt("test", 0), 11);

  // add additional child to final child where child name is equal to previous one
  config_comp_data.put("test:tst_int:tst_int", 12);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int:tst_int", 0), 12);

  // add additional child in the middle of the tree
  config_comp_data.put("test:tst_int2", 13);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int2", 0), 13);

  // add additional child at the root of the tree
  config_comp_data.put("test2", 14);
  EXPECT_EQ(config_comp_data.getInt("test2", 0), 14);
}

TEST(ConfigCompDataTests, put_and_get_overwriting_root_node_with_different_datatype) {
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("tst_int", 10);

  // overwrite root node
  config_comp_data.put("tst_int", 110);
  EXPECT_EQ(config_comp_data.getInt("tst_int", 0), 110);

  // overwrite value with different datatype
  config_comp_data.put("tst_int", "TestValue");
  EXPECT_EQ(config_comp_data.getString("tst_int", ""), "TestValue");
}

TEST(ConfigCompDataTests, put_and_get_default_values_for_invalid_key) {
  auto config_comp_data = ConfigCompData("Test");

  // check if ":" is used as separator instead of "."
  config_comp_data.put("invalid:key", 110);

  // check default values for invalid keys
  EXPECT_EQ(config_comp_data.getInt("invalid.key", 0), 0);
  EXPECT_EQ(config_comp_data.getInt("invalid.key", -21), -21);

  EXPECT_EQ(config_comp_data.getFloat("invalid.key", 0.f), 0.f);
  EXPECT_EQ(config_comp_data.getFloat("invalid.key", 6.6743f), 6.6743f);

  EXPECT_EQ(config_comp_data.getString("invalid.key", ""), "");
  EXPECT_EQ(config_comp_data.getString("invalid.key", "NotAvailable"), "NotAvailable");

  EXPECT_EQ(config_comp_data.getBool("invalid.key", true), true);
  EXPECT_EQ(config_comp_data.getBool("invalid.key", false), false);

  // check empty key
  config_comp_data.put("", 110);
  EXPECT_EQ(config_comp_data.getInt("", 0), 0);

  // put and get invalid component name (array as componet name) -> not supported
  config_comp_data.put("test[a]", 110);
  EXPECT_EQ(config_comp_data.getInt("test[a]", 0), 0);
}

TEST(ConfigCompDataTests, get_for_invalid_keys) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("invalid:key", 110);

  EXPECT_EQ(config_comp_data.getInt("invalid::key", 0), 0);
  EXPECT_EQ(config_comp_data.getInt("invalid::key[0]", 0), 0);
  EXPECT_EQ(config_comp_data.getInt("invalid[0]::key", 0), 0);
  EXPECT_EQ(config_comp_data.getInt(":key", 0), 0);
  EXPECT_EQ(config_comp_data.getInt("", 0), 0);
}

TEST(ConfigCompDataTests, put_and_get_value_array_datatypes) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int[0]", 10);
  config_comp_data.put("test:tst_float[0]", 20.5f);
  config_comp_data.put("test:tst_char_array[0]", "teststring");
  config_comp_data.put("test:tst_string[0]", std::string("teststring"));
  config_comp_data.put("b:tst_bool[0]", true);

  EXPECT_EQ(config_comp_data.getInt("test:tst_int[0]", 0), 10);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_float[0]", 0.0f), 20.5f);
  EXPECT_EQ(config_comp_data.getString("test:tst_char_array[0]", ""), "teststring");
  EXPECT_EQ(config_comp_data.getString("test:tst_string[0]", ""), "teststring");
  EXPECT_EQ(config_comp_data.getBool("b:tst_bool[0]", false), true);

  // test consecutive array index
  config_comp_data.put("test:tst_int[1]", 110);
  config_comp_data.put("test:tst_float[1]", 120.0f);
  config_comp_data.put("test:tst_string[1]", "teststring1");
  config_comp_data.put("b:tst_bool[1]", false);

  EXPECT_EQ(config_comp_data.getInt("test:tst_int[1]", 0), 110);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_float[1]", 0.0f), 120.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_string[1]", ""), "teststring1");
  EXPECT_EQ(config_comp_data.getBool("b:tst_bool[1]", true), false);
}

TEST(ConfigCompDataTests, put_and_get_value_array_indexes) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int[0]", 10);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[0]", 0), 10);

  // test consecutive array index
  config_comp_data.put("test:tst_int[1]", 110);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[1]", 0), 110);

  // test non consecutive array index
  config_comp_data.put("test:tst_int[10]", 1110);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[10]", 0), 1110);

  // test initial array index larger zero at root node -> not supported
  config_comp_data.put("test[10]", 9110);
  EXPECT_EQ(config_comp_data.getInt("test[10]", 0), 0);

  // test inserting in the middle of an array after it's extension
  config_comp_data.put("test:tst_int[9]", 2110);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[9]", 0), 2110);

  // test overwriting array index
  config_comp_data.put("test:tst_int[9]", 3110);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[9]", 0), 3110);

  // test out of bounds array index
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[11]", 0), 0);

  // test not defined array index
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[8]", 0), 0);
}

TEST(ConfigCompDataTests, put_different_datatypes_in_array) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int[0]", true);
  config_comp_data.put("test:tst_int[1]", 11);
  config_comp_data.put("test:tst_int[2]", 2.5f);
  config_comp_data.put("test:tst_int[3]", "text");

  EXPECT_EQ(config_comp_data.getBool("test:tst_int[0]", false), true);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[1]", 0), 11);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_int[2]", 0.f), 2.5f);
  EXPECT_EQ(config_comp_data.getString("test:tst_int[3]", ""), "text");
}

TEST(ConfigCompDataTests, put_array_of_arrays) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int", 10);
  config_comp_data.put("test:tst_int[2]", 11);
  config_comp_data.put("test:tst_int:a", 12);
  config_comp_data.put("test:tst_int[3]", 13);
  config_comp_data.put("test:tst_int[3]:a", 14); // invalid url
  config_comp_data.put("test:tst_int[2]:a", 15); // invalid url

  EXPECT_EQ(config_comp_data.getInt("test:tst_int", 0), 10);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[2]", 0), 11);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int:a", 0), 12);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[3]", 0), 13);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[3]:a", 0), 0);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[2]:a", 0), 0);

  // add array of arrays in different order
  config_comp_data.put("test2:tst_int[3]", 20);
  config_comp_data.put("test2:tst_int", 21);
  config_comp_data.put("test2:tst_int:a", 22);
  config_comp_data.put("test2:tst_int[3]:a", 23); // invalid url

  EXPECT_EQ(config_comp_data.getInt("test2:tst_int[3]", 0), 20);
  EXPECT_EQ(config_comp_data.getInt("test2:tst_int", 0), 21);
  EXPECT_EQ(config_comp_data.getInt("test2:tst_int:a", 0), 22);
  EXPECT_EQ(config_comp_data.getInt("test2:tst_int[3]:a", 0), 0); // invalid url

  // add array of arrays in different order
  config_comp_data.put("test3:tst_int[3]:a", 30); // invalid url
  config_comp_data.put("test3:tst_int[3]", 31);
  config_comp_data.put("test3:tst_int:a", 32);
  config_comp_data.put("test3:tst_int", 33);

  EXPECT_EQ(config_comp_data.getInt("test3:tst_int[3]:a", 0), 0); // invalid url
  EXPECT_EQ(config_comp_data.getInt("test3:tst_int[3]", 0), 31);
  EXPECT_EQ(config_comp_data.getInt("test3:tst_int:a", 0), 32);
  EXPECT_EQ(config_comp_data.getInt("test3:tst_int", 0), 33);
}

TEST(ConfigCompDataTests, getSimpleKeyFromArray) {
  auto config_comp_data = ConfigCompData("Test");

  // array index 0 and no array index are treated equally
  config_comp_data.put("test:tst_int[0]", 10);
  config_comp_data.put("test:tst_float", 20.5f);

  EXPECT_EQ(config_comp_data.getInt("test:tst_int", 0), 10);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_float[0]", 0.0f), 20.5f);
}

TEST(ConfigCompDataTests, get_datatype_conversion_from_bool) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_bool", true);
  EXPECT_EQ(config_comp_data.getBool("test:tst_bool", false), true);
  EXPECT_EQ(config_comp_data.getInt("test:tst_bool", 0), 1);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_bool", 0.0f), 1.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_bool", "default"), "1");
}

TEST(ConfigCompDataTests, get_datatype_conversion_from_int) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_int", 1);
  EXPECT_EQ(config_comp_data.getBool("test:tst_int", false), true);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int", 0), 1);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_int", 0.0f), 1.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_int", "default"), "1");

  config_comp_data.put("test:tst_int", -1);
  bool value_exists = true;
  EXPECT_EQ(config_comp_data.getBool("test:tst_int", false, value_exists), false);
  EXPECT_FALSE(value_exists);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int", 0), -1);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_int", 0.0f), -1.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_int", "default"), "-1");
}

TEST(ConfigCompDataTests, get_datatype_conversion_from_float) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_float", 10.0f);
  EXPECT_EQ(config_comp_data.getBool("test:tst_float", false), false);
  EXPECT_EQ(config_comp_data.getInt("test:tst_float", -1), 10);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_float", 0.0f), 10.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_float", "default"), "10");

  config_comp_data.put("test:tst_float", 10.1f);
  EXPECT_EQ(config_comp_data.getBool("test:tst_float", false), false);
  EXPECT_EQ(config_comp_data.getInt("test:tst_float", -1), -1);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_float", 0.0f), 10.1f);
  EXPECT_EQ(config_comp_data.getString("test:tst_float", "default"), "10.1");
}

TEST(ConfigCompDataTests, get_datatype_conversion_from_string) {
  auto config_comp_data = ConfigCompData("Test");

  config_comp_data.put("test:tst_string", "0");
  EXPECT_EQ(config_comp_data.getBool("test:tst_string", true), false);
  EXPECT_EQ(config_comp_data.getInt("test:tst_string", -1), 0);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_string", 0.0f), 0.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_string", "default"), "0");

  config_comp_data.put("test:tst_string", "test");
  EXPECT_EQ(config_comp_data.getBool("test:tst_string", false), false);
  EXPECT_EQ(config_comp_data.getInt("test:tst_string", -1), -1);
  EXPECT_EQ(config_comp_data.getFloat("test:tst_string", 0.0f), 0.0f);
  EXPECT_EQ(config_comp_data.getString("test:tst_string", "default"), "test");
}

TEST(ConfigCompDataTests, configValueExists) {

  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test:tst_int[0]", 10);

  EXPECT_TRUE(config_comp_data.configValueExists("test:tst_int[0]"));
  EXPECT_TRUE(config_comp_data.configValueExists("test:tst_int"));
  EXPECT_FALSE(config_comp_data.configValueExists("test:tst_in"));
  EXPECT_FALSE(config_comp_data.configValueExists("test:tst_int[a]"));
}

TEST(ConfigCompDataTests, getCompData) {

  ConfigCompData config_comp_data("Test");
  config_comp_data.put("test:tst_int", 1);
  config_comp_data.put("test:tst_int:b[2]:c", 12); // will be ignored -> invalid url
  std::map<std::string, std::string> map_data = config_comp_data.getCompData();

  EXPECT_EQ(map_data.size(), 1);
  EXPECT_EQ(map_data["test:tst_int"], "1");
}

TEST(ConfigCompDataTests, getCompName) {
  ConfigCompData config_comp_data("Test");
  EXPECT_EQ(config_comp_data.getCompName(), "Test");
}

TEST(ConfigCompDataTests, get_children_valid_fullpath) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test:tst_int:b:c", 12);
  config_comp_data.put("test:tst_int[0]", 10);
  config_comp_data.put("test:tst_int[1]", 13);
  config_comp_data.put("test:tst_int[3]:c", 14); // invalid url
  config_comp_data.put("test:tst_int:a", 11);
  config_comp_data.put("test2", 13);

  // nodes with values
  EXPECT_EQ(config_comp_data.getInt("test:tst_int:b:c", 0), 12);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int", 0), 10);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[1]", 0), 13);
  bool value_exists = true;
  EXPECT_EQ(config_comp_data.getInt("test:tst_int[3]:c", 0, value_exists), 0); // invalid url
  EXPECT_FALSE(value_exists);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int:a", 0), 11);
  // nodes with no values
  EXPECT_EQ(config_comp_data.getInt("test", 0), 0);
  EXPECT_EQ(config_comp_data.getInt("test:tst_int:b", 0), 0);

  // check children at different levels
  std::vector<std::string> children_root = config_comp_data.getChildrenKeys("", true);
  ASSERT_EQ(children_root.size(), 2);
  EXPECT_EQ(children_root.at(0), "test");
  EXPECT_EQ(children_root.at(1), "test2");

  std::vector<std::string> children_test = config_comp_data.getChildrenKeys("test", true);
  ASSERT_EQ(children_test.size(), 1);
  EXPECT_EQ(children_test.at(0), "test:tst_int");

  std::vector<std::string> children_multiple = config_comp_data.getChildrenKeys("test:tst_int", true);
  ASSERT_EQ(children_multiple.size(), 2);
  EXPECT_EQ(children_multiple.at(0), "test:tst_int:b");
  EXPECT_EQ(children_multiple.at(1), "test:tst_int:a");

  std::vector<std::string> children_array = config_comp_data.getChildrenKeys("test:tst_int[1]", true);
  EXPECT_TRUE(children_array.empty());

  std::vector<std::string> children_final = config_comp_data.getChildrenKeys("test:tst_int:b:c", true);
  EXPECT_TRUE(children_final.empty());
}

TEST(ConfigCompDataTests, get_children_valid_onlyNodeNames) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test:tst_int:b:c", 12);
  config_comp_data.put("test:tst_int[0]", 10);
  config_comp_data.put("test:tst_int[1]", 13);
  config_comp_data.put("test:tst_int[3]:c", 14); // invalid url
  config_comp_data.put("test:tst_int:a", 11);
  config_comp_data.put("test2", 13);

  // check children
  std::vector<std::string> children_root = config_comp_data.getChildrenKeys("");
  ASSERT_EQ(children_root.size(), 2);
  EXPECT_EQ(children_root.at(0), "test");
  EXPECT_EQ(children_root.at(1), "test2");

  // check to get only node names
  std::vector<std::string> children_test = config_comp_data.getChildrenKeys("test");
  ASSERT_EQ(children_test.size(), 1);
  EXPECT_EQ(children_test.at(0), "tst_int");

  // check to get only node names
  std::vector<std::string> children_multiple = config_comp_data.getChildrenKeys("test:tst_int", false);
  ASSERT_EQ(children_multiple.size(), 2);
  EXPECT_EQ(children_multiple.at(0), "b");
  EXPECT_EQ(children_multiple.at(1), "a");
}

TEST(ConfigCompDataTests, get_children_empty) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test:tst_int:b:c", 12);

  // check non existing key
  std::vector<std::string> children_empty = config_comp_data.getChildrenKeys("tes");
  EXPECT_TRUE(children_empty.empty());

  // check invalid keys
  {
    std::vector<std::string> children_invalid_key = config_comp_data.getChildrenKeys("test[a]");
    EXPECT_TRUE(children_invalid_key.empty());
  }
  {
    std::vector<std::string> children_invalid_key = config_comp_data.getChildrenKeys("test:tst_int:b[0]:c");
    EXPECT_TRUE(children_invalid_key.empty());
  }
  {
    std::vector<std::string> children_invalid_key = config_comp_data.getChildrenKeys("test::tst_int:b:c");
    EXPECT_TRUE(children_invalid_key.empty());
  }
}

TEST(ConfigCompDataTests, isEmpty) {
  auto config_comp_data = ConfigCompData("Test");
  EXPECT_TRUE(config_comp_data.isEmpty());

  config_comp_data.put("test:tst_int:b:c", 12);
  EXPECT_FALSE(config_comp_data.isEmpty());
}

TEST(ConfigCompDataStorageTests, keyInfo_valid) {
  auto config_comp_data = ConfigCompData("Test");

  {
    std::string key = "test";
    bool config_input_value_bool = true;
    config_comp_data.put(key, config_input_value_bool);

    // get single element
    size_t array_size = 0;
    bool final_leaf = false;
    EXPECT_TRUE(config_comp_data.keyInfo(key, final_leaf, array_size));
    EXPECT_EQ(array_size, 1);
    EXPECT_TRUE(final_leaf);
  }
  {
    std::string key = "test:int";
    bool config_input_value_bool = true;
    config_comp_data.put(key, config_input_value_bool);

    // get single element
    size_t array_size = 0;
    bool final_leaf = true;
    EXPECT_TRUE(config_comp_data.keyInfo("test", final_leaf, array_size));
    EXPECT_EQ(array_size, 1);
    EXPECT_FALSE(final_leaf);
  }
  {
    std::string key1 = "a:test2[0]";
    bool config_input_value_bool = true;
    config_comp_data.put(key1, config_input_value_bool);
    std::string key2 = "a:test2[1]";
    config_comp_data.put(key2, config_input_value_bool);

    // get array element
    size_t array_size = 1;
    bool final_leaf = false;
    EXPECT_TRUE(config_comp_data.keyInfo(key1, final_leaf, array_size));
    EXPECT_EQ(array_size, 2);
    EXPECT_TRUE(final_leaf);

    array_size = 1;
    final_leaf = false;
    EXPECT_TRUE(config_comp_data.keyInfo(key2, final_leaf, array_size));
    EXPECT_EQ(array_size, 2);
    EXPECT_TRUE(final_leaf);

    array_size = 1;
    final_leaf = false;
    EXPECT_TRUE(config_comp_data.keyInfo("a:test2", final_leaf, array_size));
    EXPECT_EQ(array_size, 2);
    EXPECT_TRUE(final_leaf);
  }
}

TEST(ConfigCompDataTests, keyInfo_invalid) {
  auto config_comp_data = ConfigCompData("Test");
  {
    size_t array_size = 1;
    bool final_leaf = true;
    EXPECT_FALSE(config_comp_data.keyInfo("non_existing", final_leaf, array_size));
    EXPECT_EQ(array_size, 0);
    EXPECT_FALSE(final_leaf);
  }
  config_comp_data.put("dummy", 1);
  {
    // invalid key
    size_t array_size = 1;
    bool final_leaf = true;
    EXPECT_FALSE(config_comp_data.keyInfo("invalid_key[a]", final_leaf, array_size));
    EXPECT_EQ(array_size, 0);
    EXPECT_FALSE(final_leaf);
  }
}

TEST(ConfigCompDataTests, getStringList_valid) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test:tst_int:b:c", 12);
  config_comp_data.put("test:tst_int[0]", 10);
  config_comp_data.put("test:tst_int[10]", "test");
  config_comp_data.put("test:tst_int[11]", 3.1f);
  config_comp_data.put("test:tst_int[13]", true);
  config_comp_data.put("test:tst_int:a", 11);
  config_comp_data.put("test2", 13);

  // get tree values
  bool value_available{false};
  // empty key -> return default values
  std::vector<std::string> tree_values_empty = config_comp_data.getValuesAsStringList("", {}, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_empty.size(), 0);

  // get not existing value
  std::vector<std::string> tree_values_root =
      config_comp_data.getValuesAsStringList("test", std::vector<std::string>{"empty"}, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_root.size(), 1);
  EXPECT_EQ(tree_values_root.at(0), "empty");

  std::vector<std::string> tree_values_test_int =
      config_comp_data.getValuesAsStringList("test:tst_int", std::vector<std::string>{"empty"});
  EXPECT_EQ(tree_values_test_int.size(), 4);
  EXPECT_EQ(tree_values_test_int.at(0), "10");
  EXPECT_EQ(tree_values_test_int.at(1), "test");
  EXPECT_EQ(tree_values_test_int.at(2), "3.100000");
  EXPECT_EQ(tree_values_test_int.at(3), "1");

  std::vector<std::string> tree_values_test_int_elem_zero =
      config_comp_data.getValuesAsStringList("test:tst_int[0]", std::vector<std::string>{"empty"});
  EXPECT_EQ(tree_values_test_int_elem_zero.size(), 1);
  EXPECT_EQ(tree_values_test_int_elem_zero.at(0), "10");

  std::vector<std::string> tree_values_test_int_elem_one =
      config_comp_data.getValuesAsStringList("test:tst_int[1]", std::vector<std::string>{"empty"});
  EXPECT_EQ(tree_values_test_int_elem_one.size(), 1);
  EXPECT_EQ(tree_values_test_int_elem_one.at(0), "empty");

  std::vector<std::string> tree_values_test_int_elem_ten =
      config_comp_data.getValuesAsStringList("test:tst_int[10]", std::vector<std::string>{"empty"});
  EXPECT_EQ(tree_values_test_int_elem_ten.size(), 1);
  EXPECT_EQ(tree_values_test_int_elem_ten.at(0), "test");

  std::vector<std::string> tree_values_leaf_a =
      config_comp_data.getValuesAsStringList("test:tst_int:a", std::vector<std::string>{"empty"});
  EXPECT_EQ(tree_values_leaf_a.size(), 1);
  EXPECT_EQ(tree_values_leaf_a.at(0), "11");

  std::vector<std::string> tree_values_leaf_b =
      config_comp_data.getValuesAsStringList("test:tst_int:b:c", std::vector<std::string>{"empty"});
  EXPECT_EQ(tree_values_leaf_b.size(), 1);
  EXPECT_EQ(tree_values_leaf_b.at(0), "12");

  std::vector<std::string> tree_values_test2 = config_comp_data.getValuesAsStringList("test2", {}, value_available);
  EXPECT_TRUE(value_available);
  EXPECT_EQ(tree_values_test2.size(), 1);
  EXPECT_EQ(tree_values_test2.at(0), "13");
}

TEST(ConfigCompDataTests, getStringList_empty) {
  auto config_comp_data = ConfigCompData("Test");

  // get non existing key
  bool value_available{true};
  std::vector<std::string> tree_values_empty =
      config_comp_data.getValuesAsStringList("test", std::vector<std::string>{"empty"}, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_empty.size(), 1);
  EXPECT_EQ(tree_values_empty.at(0), "empty");

  // get invalid key
  value_available = true;
  std::vector<std::string> tree_values_invalid =
      config_comp_data.getValuesAsStringList("test[a]", std::vector<std::string>{"empty"}, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_empty.size(), 1);
  EXPECT_EQ(tree_values_empty.at(0), "empty");
}

TEST(ConfigCompDataTests, getValueKeyPairsAsStringList_valid) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test:tst_int:b:c", 12);
  config_comp_data.put("test:tst_int[0]", 10);
  config_comp_data.put("test:tst_int[10]", "test");
  config_comp_data.put("test:tst_int[11]", 3.1f);
  config_comp_data.put("test:tst_int[13]", true);
  config_comp_data.put("test:tst_int:a", 11);
  config_comp_data.put("test2", 13);

  // get tree values
  bool value_available{false};
  std::map<std::string, std::string> default_values = std::map<std::string, std::string>{{"emptyKey", "emptyValue"}};

  // empty key -> return default values
  std::map<std::string, std::string> tree_values_empty_key =
      config_comp_data.getValueKeyPairsAsStringList("", default_values, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_empty_key.size(), 1);
  EXPECT_EQ(tree_values_empty_key.at("emptyKey"), "emptyValue");

  // get not existing config value at this level
  std::map<std::string, std::string> tree_values_root =
      config_comp_data.getValueKeyPairsAsStringList("test", {}, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_root.size(), 0);

  // valid key
  std::map<std::string, std::string> tree_values_test_int =
      config_comp_data.getValueKeyPairsAsStringList("test:tst_int", default_values);
  EXPECT_EQ(tree_values_test_int.size(), 4);
  EXPECT_EQ(tree_values_test_int.at("test:tst_int[0]"), "10");
  EXPECT_EQ(tree_values_test_int.at("test:tst_int[10]"), "test");
  EXPECT_EQ(tree_values_test_int.at("test:tst_int[11]"), "3.100000");
  EXPECT_EQ(tree_values_test_int.at("test:tst_int[13]"), "1");

  // valid key
  std::map<std::string, std::string> tree_values_test_int_elem_zero =
      config_comp_data.getValueKeyPairsAsStringList("test:tst_int[0]", default_values);
  EXPECT_EQ(tree_values_test_int_elem_zero.size(), 1);
  EXPECT_EQ(tree_values_test_int_elem_zero.at("test:tst_int[0]"), "10");

  // not set array value -> return empty because no default values are set
  std::map<std::string, std::string> tree_values_test_int_elem_one =
      config_comp_data.getValueKeyPairsAsStringList("test:tst_int[1]", {});
  EXPECT_EQ(tree_values_test_int_elem_one.size(), 0);

  // valid key
  std::map<std::string, std::string> tree_values_test_int_elem_ten =
      config_comp_data.getValueKeyPairsAsStringList("test:tst_int[10]", default_values);
  EXPECT_EQ(tree_values_test_int_elem_ten.size(), 1);
  EXPECT_EQ(tree_values_test_int_elem_ten.at("test:tst_int[10]"), "test");

  // valid key
  std::map<std::string, std::string> tree_values_leaf_a =
      config_comp_data.getValueKeyPairsAsStringList("test:tst_int:a", default_values);
  EXPECT_EQ(tree_values_leaf_a.size(), 1);
  EXPECT_EQ(tree_values_leaf_a.at("test:tst_int:a"), "11");

  // valid key
  std::map<std::string, std::string> tree_values_leaf_b =
      config_comp_data.getValueKeyPairsAsStringList("test:tst_int:b:c", default_values);
  EXPECT_EQ(tree_values_leaf_b.size(), 1);
  EXPECT_EQ(tree_values_leaf_b.at("test:tst_int:b:c"), "12");

  // valid key
  std::map<std::string, std::string> tree_values_test2 =
      config_comp_data.getValueKeyPairsAsStringList("test2", default_values, value_available);
  EXPECT_TRUE(value_available);
  EXPECT_EQ(tree_values_test2.size(), 1);
  EXPECT_EQ(tree_values_test2.at("test2"), "13");
}

TEST(ConfigCompDataTests, getValueKeyPairsAsStringList_empty) {
  auto config_comp_data = ConfigCompData("Test");

  // get non existing key
  bool value_available{true};
  std::map<std::string, std::string> tree_values_empty =
      config_comp_data.getValueKeyPairsAsStringList("test", std::map<std::string, std::string>{}, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_empty.size(), 0);

  // get invalid key
  value_available = true;
  std::map<std::string, std::string> tree_values_invalid =
      config_comp_data.getValueKeyPairsAsStringList("test[a]", std::map<std::string, std::string>{}, value_available);
  EXPECT_FALSE(value_available);
  EXPECT_EQ(tree_values_empty.size(), 0);
}

TEST(ConfigCompDataTests, DISABLED_put_and_get_performance_test) {
  auto config_comp_data = ConfigCompData("Test");

  unsigned int size_keys = 100000;
  unsigned int level_increase = 100;
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

  for (const auto &key_elem : key_list_int) {
    config_comp_data.getInt(key_elem, 0);
  }
  for (const auto &key_elem : key_list_float) {
    config_comp_data.getFloat(key_elem, 0.0f);
  }
  for (const auto &key_elem : key_list_string) {
    config_comp_data.getString(key_elem, "");
  }
  for (const auto &key_elem : key_list_bool) {
    config_comp_data.getBool(key_elem, false);
  }
}

TEST(ConfigCompDataTests, traverseTree_emptyConfig) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");

  std::map<std::string, std::string> config_data;
  config_comp_data.traverseTree(
      "", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

  // get tree values
  EXPECT_TRUE(config_data.empty());
}

TEST(ConfigCompDataTests, traverseTree_nonExistingKey) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test", 1);

  std::map<std::string, std::string> config_data;
  config_comp_data.traverseTree(
      "abc", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

  // get tree values
  EXPECT_TRUE(config_data.empty());
}

TEST(ConfigCompDataTests, traverseTree_invalidKey) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test:a:b", 1);

  {
    std::map<std::string, std::string> config_data;
    // check with invalid key
    config_comp_data.traverseTree(
        "abc[1].a", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    EXPECT_TRUE(config_data.empty());
  }
  {
    std::map<std::string, std::string> config_data;
    // check with invalid key
    config_comp_data.traverseTree(
        "test:a::b", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    EXPECT_TRUE(config_data.empty());
  }
}

TEST(ConfigCompDataTests, traverseTree_singleValue) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("test", 1);

  {
    // use root as entry
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data.at("test"), "1");
  }
  {
    // use config value as entry
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "test", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data.at("test"), "1");
  }
  {
    // use config value as entry
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "test[0]", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data.at("test[0]"), "1");
  }
}

TEST(ConfigCompDataTests, traverseTree_arrayValue_index0) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("a:test[0]", 1);
  // check with single value -> should be considered as non array value
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data["a:test"], "1");
  }
}

TEST(ConfigCompDataTests, traverseTree_arrayValue_index2) {
  // test setup -> add nodes
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("a:test[2]", 1); // shall be considered as array entry

  // access via root
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data["a:test[2]"], "1");
  }
  // access via array name -> return only valid entries
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "a:test", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data["a:test[2]"], "1");
  }
  // access via array index -> return only the specified index
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "a:test[2]", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data["a:test[2]"], "1");
  }
}

TEST(ConfigCompDataTests, traverseTree_nestedStructureWithArray) {
  auto config_comp_data = ConfigCompData("Test");
  config_comp_data.put("comp:a:test[0]", 1);
  config_comp_data.put("comp:a:test[2]", 2);
  config_comp_data.put("coma:a:test[2]", 3);

  // check from root
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 3);
    EXPECT_EQ(config_data["comp:a:test[0]"], "1");
    EXPECT_EQ(config_data["comp:a:test[2]"], "2");
    EXPECT_EQ(config_data["coma:a:test[2]"], "3");
  }
  // check getting subtree
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "comp", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 2);
    EXPECT_EQ(config_data["comp:a:test[0]"], "1");
    EXPECT_EQ(config_data["comp:a:test[2]"], "2");
  }
  // check getting array which doesn't contain a value set for index 1
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree(
        "comp:a:test", [&config_data](const std::string &key, const std::string &value) { config_data[key] = value; });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 2);
    EXPECT_EQ(config_data["comp:a:test[0]"], "1");
    EXPECT_EQ(config_data["comp:a:test[2]"], "2");
  }
  // check getting array index 0
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree("comp:a:test[0]", [&config_data](const std::string &key, const std::string &value) {
      config_data[key] = value;
    });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data["comp:a:test[0]"], "1");
  }
  // check getting not set array index 1
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree("comp:a:test[1]", [&config_data](const std::string &key, const std::string &value) {
      config_data[key] = value;
    });

    // get tree values
    ASSERT_TRUE(config_data.empty());
  }
  // check getting array index 2
  {
    std::map<std::string, std::string> config_data;
    config_comp_data.traverseTree("comp:a:test[2]", [&config_data](const std::string &key, const std::string &value) {
      config_data[key] = value;
    });

    // get tree values
    ASSERT_FALSE(config_data.empty());
    EXPECT_EQ(config_data.size(), 1);
    EXPECT_EQ(config_data["comp:a:test[2]"], "2");
  }
}

} // namespace appsupport
} // namespace next
