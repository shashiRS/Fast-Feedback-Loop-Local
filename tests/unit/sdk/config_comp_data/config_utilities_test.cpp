/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     config_comp_data_utilities_tests.cpp
 *  @brief    Testing utility functionality
 */

#include <string>
#include <vector>

#include <next/sdk/sdk.hpp>

#include "config_utilities.hpp"

namespace next {
namespace appsupport {

TEST(ConfigUtilitiesTests, extractKeyPath_simple_string) {
  // test valid simple string
  std::string key = "test:tst_int";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path{""};
  size_t array_index{0};
  EXPECT_EQ(true, ConfigUtilities::extractKeyPath(key, key_path, array_index));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
}

TEST(ConfigUtilitiesTests, extractKeyPath_empty_string) {
  // test empty string
  std::string key = "";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path = "";
  size_t array_index = 0;
  bool array_in_key_path{true};
  EXPECT_EQ(true, ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_key_path);
}

TEST(ConfigUtilitiesTests, extractKeyPath_array_key) {
  // test array key
  {
    std::string key = "test:tst_int[0]";
    std::string expected_key_path = "test:tst_int";
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{false};
    EXPECT_EQ(true, ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_TRUE(array_in_key_path);
  }

  // test array key
  {
    std::string key = "test:tst_int[1]";
    std::string expected_key_path = "test:tst_int";
    size_t expected_array_index = 1;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{false};
    EXPECT_EQ(true, ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_TRUE(array_in_key_path);
  }
}

TEST(ConfigUtilitiesTests, extractKeyPath_whitespaces_valid) {
  {
    // check ignoring whitespaces
    std::string key = " test : tst_ in[1] ";
    std::string expected_key_path = "test : tst_ in";
    size_t expected_array_index = 1;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{false};
    EXPECT_EQ(true, ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_TRUE(array_in_key_path);
  }
}
TEST(ConfigUtilitiesTests, extractKeyPath_whitespaces_invalid_url) {
  {
    // check invalid url with whitespaces: key index not at the end -> don't adapt url
    std::string key = " test : tst_ in[0]a ";
    std::string expected_key_path = " test : tst_ in[0]a ";
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{false};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    // check invalid url with whitespaces: no opening bracket -> don't adapt url
    std::string key = " test : tst_ in] ";
    std::string expected_key_path = " test : tst_ in] ";
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    // check invalid url with whitespaces: no array index -> don't adapt url
    std::string key = " test : tst_ in[] ";
    std::string expected_key_path = " test : tst_ in[] ";
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    // check invalid url with whitespaces: invalid array index -> don't adapt url
    std::string key = " test : tst_ in[a] ";
    std::string expected_key_path = " test : tst_ in[a] ";
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }

  {
    // check invalid url with whitespaces: multiple brackets -> don't adapt url
    std::string key = " test : tst_ in[1]0] ";
    std::string expected_key_path = " test : tst_ in[1]0] ";
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
}

TEST(ConfigUtilitiesTests, extractKeyPath_emptyArrayIndex) {
  // handle empty array index -> ignore array index
  std::string key = "test:tst_int[]";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path = "";
  size_t array_index = 0;
  bool array_in_key_path{true};
  EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_key_path);
}

TEST(ConfigUtilitiesTests, extractKeyPath_emptyFinalNodeArrayIndex) {
  // handle empty array index -> ignore array index
  std::string key = "test:[1]";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path = "";
  size_t array_index = 0;
  bool array_in_key_path{true};
  EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_key_path);
}

TEST(ConfigUtilitiesTests, extractKeyPath_ignoreArrayNotAtTheEnd) {
  // handle array index is not at the end ->invalid url
  {
    std::string key = "test:tst_in[1]a";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    // array specification at an intermediate level is not supported
    std::string key = "test[1]:tst_int";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path{""};
    size_t array_index{0};
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
}

TEST(ConfigUtilitiesTests, extractKeyPath_incompleteOpeningBracket) {
  // handling incomplete array specification
  // -> opening bracket is considered as part of the string for performance reasons.
  //    -> Responsibility of the user.
  {
    std::string key = "test:tst_int[";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_TRUE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    std::string key = "test:tst_in[t";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_TRUE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    std::string key = "test:tst_int[[";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_TRUE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
}

TEST(ConfigUtilitiesTests, extractKeyPath_ignoreIncompleteClosingBracket) {
  // handling incomplete array specification -> invalid url
  {
    std::string key = "test:tst_int]";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    std::string key = "test:tst_in]t";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    std::string key = "test:tst_int]]";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    std::string key = "test:tst_int][";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
}

TEST(ConfigUtilitiesTests, extractKeyPath_ignoreMultipleArrays) {
  // test multiple arrays in path -> invalid url
  std::string key = "test[1]:tst_int[11]";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path = "";
  size_t array_index = 0;
  bool array_in_key_path{true};
  EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_key_path);

  // test multiple arrays in path -> invalid url
  key = "test:tst_int[1][11]";
  expected_key_path = key;
  expected_array_index = 0;

  key_path = "";
  array_index = 0;
  array_in_key_path = true;
  EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_key_path);
}

TEST(ConfigUtilitiesTests, extractKeyPath_ignoreAdditionalBrackets) {
  {
    // handling incomplete array specification -> invalid url
    std::string key = "test:tst_in[[1]";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    // handling incomplete array specification -> invalid url
    std::string key = "test:tst_in[1]]";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    // handling incomplete array specification -> invalid url
    std::string key = "test:tst_in][1]";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path = "";
    size_t array_index = 0;
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
}

TEST(ConfigUtilitiesTests, extractKeyPath_invalidArrayIndex) {
  {
    // handling invalid array specification
    std::string key = "test:tst_int[a]";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path{""};
    size_t array_index{0};
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
  {
    // array specification at an intermediate level is not supported
    std::string key = "test[a]:tst_int[1]";
    std::string expected_key_path = key;
    size_t expected_array_index = 0;

    std::string key_path{""};
    size_t array_index{0};
    bool array_in_key_path{true};
    EXPECT_FALSE(ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path));
    EXPECT_EQ(expected_key_path, key_path);
    EXPECT_EQ(expected_array_index, array_index);
    EXPECT_FALSE(array_in_key_path);
  }
}

TEST(ConfigUtilitiesTests, checkKeyPath_simple_key_valid) {
  // test valid simple string
  std::string key = "test:tst_int";
  EXPECT_TRUE(ConfigUtilities::checkKeyPath(key));
}

TEST(ConfigUtilitiesTests, checkKeyPath_array_valid) {
  // test valid array
  std::string key = "test:tst_int[1]";
  std::string expected_key_path = "test:tst_int";
  size_t expected_array_index = 1;

  std::string key_path{""};
  size_t array_index{0};
  bool array_in_path{false};
  EXPECT_TRUE(ConfigUtilities::checkKeyPath(key, key_path, array_index, array_in_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_TRUE(array_in_path);
}

TEST(ConfigUtilitiesTests, checkKeyPath_empty_invalid) {
  // empty component name not supported
  std::string key = "";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path{""};
  size_t array_index{1};
  bool array_in_path{true};
  EXPECT_FALSE(ConfigUtilities::checkKeyPath(key, key_path, array_index, array_in_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_path);
}

TEST(ConfigUtilitiesTests, checkKeyPath_empty_component_name_invalid) {
  // empty component name not supported
  std::string key = ":test";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path{""};
  size_t array_index{1};
  bool array_in_path{true};
  EXPECT_FALSE(ConfigUtilities::checkKeyPath(key, key_path, array_index, array_in_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_path);
}

TEST(ConfigUtilitiesTests, checkKeyPath_empty_node_name_invalid) {
  // empty node name at sublevel not supported
  std::string key = "a::test";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path{""};
  size_t array_index{1};
  bool array_in_path{true};
  EXPECT_FALSE(ConfigUtilities::checkKeyPath(key, key_path, array_index, array_in_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_path);
}

TEST(ConfigUtilitiesTests, checkKeyPath_nested_array_invalid) {
  // nested array not supported
  std::string key = "a:b[0]:test[1]";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path{""};
  size_t array_index{1};
  bool array_in_path{true};
  EXPECT_FALSE(ConfigUtilities::checkKeyPath(key, key_path, array_index, array_in_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_path);
}

TEST(ConfigUtilitiesTests, checkComponentName_compName_valid) {
  EXPECT_TRUE(ConfigUtilities::checkComponentName("comp"));
}

TEST(ConfigUtilitiesTests, checkComponentName_emptyCompName_invalid) {
  EXPECT_FALSE(ConfigUtilities::checkComponentName(""));
}

TEST(ConfigUtilitiesTests, checkComponentName_separatorInCompName_invalid) {
  EXPECT_FALSE(ConfigUtilities::checkComponentName("co:mp"));
}

TEST(ConfigUtilitiesTests, checkComponentName_bracketInCompName_invalid) {
  EXPECT_FALSE(ConfigUtilities::checkComponentName("[comp"));
  EXPECT_FALSE(ConfigUtilities::checkComponentName("comp["));
  EXPECT_FALSE(ConfigUtilities::checkComponentName("comp[0]"));
}

TEST(ConfigUtilitiesTests, checkKeyPath_array_component_invalid) {
  // array at component level not supported
  std::string key = "test[1]";
  std::string expected_key_path = key;
  size_t expected_array_index = 0;

  std::string key_path{""};
  size_t array_index{1};
  bool array_in_path{true};
  EXPECT_FALSE(ConfigUtilities::checkKeyPath(key, key_path, array_index, array_in_path));
  EXPECT_EQ(expected_key_path, key_path);
  EXPECT_EQ(expected_array_index, array_index);
  EXPECT_FALSE(array_in_path);
}

TEST(ConfigUtilitiesTests, getComponentNameFromKey_emptyKey) {
  std::string key = "";
  std::string comp_name = ConfigUtilities::getComponentNameFromKey(key);
  EXPECT_EQ(key, comp_name);
}
TEST(ConfigUtilitiesTests, getComponentNameFromKey_noSubComp) {
  std::string key = "test";
  std::string comp_name = ConfigUtilities::getComponentNameFromKey(key);
  EXPECT_EQ(key, comp_name);
}

TEST(ConfigUtilitiesTests, getComponentNameFromKey_subComp) {
  std::string key = "test:a:b:c";
  std::string comp_name = ConfigUtilities::getComponentNameFromKey(key);
  EXPECT_EQ("test", comp_name);
}

TEST(ConfigUtilitiesTests, getComponentNameFromKey_subCompArray) {
  std::string key = "test:a:b:c[3]";
  std::string comp_name = ConfigUtilities::getComponentNameFromKey(key);
  EXPECT_EQ("test", comp_name);
}

} // namespace appsupport
} // namespace next
