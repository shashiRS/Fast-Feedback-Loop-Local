/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugins_utils_test.cpp
 *  @brief    Testing the plugin utils functions (helper functions)
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/plugins_utils/helper_functions.h>

using namespace next::plugins_utils;

//! Testing the string split function
TEST(PluginUtilsTest, StringSplit) {
  const std::string hello("hello");
  const std::string world("world");
  const std::string hw(hello + "." + world);

  auto vec = split_string(hw, ".");

  ASSERT_EQ(vec.size(), 2);
  EXPECT_EQ(vec[0], hello) << "split of 'hello.world' should give 'hello'";
  EXPECT_EQ(vec[1], world) << "split of 'hello.world' should give 'world'";
  vec.clear();

  const std::string empty("");
  vec = split_string(empty, ".");
  ASSERT_EQ(vec.size(), 1) << "splitting an empty string should give a vector with one empty element";
  EXPECT_EQ(vec[0].size(), 0) << "splitting an empty string should give a vector with one empty element";

  const std::string simple("this is only a sentence without any dot");
  vec = split_string(simple, ".");
  ASSERT_EQ(vec.size(), 1) << "splitting a string by dot without a dot in the string should give one string back";
  EXPECT_EQ(vec[0], simple) << "splitting a string by dot without a dot in the string should give the original string";

  const std::string doubleDot(hello + ".." + world);
  vec = split_string(doubleDot, ".");
  ASSERT_EQ(vec.size(), 3) << "splitting a string with two dots should give 3 strings back";
  EXPECT_EQ(vec[0], hello);
  EXPECT_TRUE(vec[1].empty()) << "splitting a string by dot with '..' in give a empty string";
  EXPECT_EQ(vec[2], world);

  EXPECT_THROW(split_string(hello, ""), std::invalid_argument)
      << "For an empty delimiter a invalid argument exception shall be thrown";

  const std::string trailingDelim(world + "/" + hello + "//");
  vec = split_string(trailingDelim, "/");
  ASSERT_EQ(vec.size(), 4) << "for having double trailing delimiter it should give two empty strings at the end";
  ASSERT_EQ(vec[0], world);
  ASSERT_EQ(vec[1], hello);
  ASSERT_TRUE(vec[2].empty()) << "for having double trailing delimiter it should give two empty strings at the end";
  ASSERT_TRUE(vec[3].empty()) << "for having double trailing delimiter it should give two empty strings at the end";
}

TEST(PluginUtilsTest, Trim) {
  const std::string hw("hello world");
  std::string input1(" " + hw + " ");
  trim(input1);
  EXPECT_EQ(input1, hw);

  std::string input2(" .a" + hw + ".a. ");
  trim(input2, "a. ");
  EXPECT_EQ(input2, hw);

  std::string input3(" " + hw + "\t\n");
  trim(input3, " \n");
  EXPECT_EQ(input3, std::string(hw + "\t"));
  trim(input3, "\t");
  EXPECT_EQ(input3, hw);
}

TEST(PluginUtilsTest, CreateKeyOutOfPluginConfig) {
  std::vector<next::bridgesdk::plugin_config_t> config_to_set = {
      {std::pair("[Data_VIEW]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view"})}};

  std::vector<next::bridgesdk::plugin_config_t> config_to_set_new = {{std::pair(
      "[Data_VIEW]", next::bridgesdk::PluginConfigParam{next::bridgesdk::ConfigType::FIXED, "Data view 2nd"})}};

  auto result = next::plugins_utils::CreateKeyOutOfPluginConfig(config_to_set.at(0));
  auto new_result = next::plugins_utils::CreateKeyOutOfPluginConfig(config_to_set_new.at(0));

  std::string expected_result = "[Data_VIEW][1]:Data view;";

  ASSERT_EQ(result, expected_result) << "Key is not equal to expected key";
  ASSERT_NE(result, new_result) << "Unit test fail, configurations are equal";
}
