/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     config_comp_data_storage_tests.cpp
 *  @brief    Testing the config database storage class
 */

#include <string>
#include <vector>

#include <boost/json.hpp>

#include <next/sdk/sdk.hpp>

#include "config_comp_data_storage.hpp"

namespace next {
namespace appsupport {

TEST(ConfigCompDataStorageTests, put_and_get_simple_values_and_overwrite) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");
  {
    std::string key = "test:tst_int";
    int config_input_value = 10;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 0, true, value_exists),
              config_input_value);
    EXPECT_TRUE(value_exists);
  }
  {
    // overwrite with different value
    std::string key = "test:tst_int";
    int config_input_value = -10;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 0, false, value_exists),
              config_input_value);
    EXPECT_TRUE(value_exists);
  }

  {
    std::string key = "test:tst_float";
    float config_input_value = 10.1f;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 0.0f, true, value_exists),
              config_input_value);
  }
  {
    // overwrite with different value
    std::string key = "test:tst_float";
    float config_input_value = -10.1f;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 0.0f, true, value_exists),
              config_input_value);
  }

  {
    std::string key = "test:tst_string";
    std::string config_input_value = "testValue";
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              config_input_value);
  }
  {
    // overwrite with different value
    std::string key = "test:tst_string";
    std::string config_input_value = "";
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(
        config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "default", true, value_exists),
        config_input_value);
  }

  {
    std::string key = "test:tst_bool";
    bool config_input_value = true;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists),
              config_input_value);
  }

  {
    // overwrite with different value
    std::string key = "test:tst_bool";
    bool config_input_value = false;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index, config_input_value);

    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, true, true, value_exists),
              config_input_value);
  }
}

TEST(ConfigCompDataStorageTests, put_fill_array_from_the_back) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  bool config_input_value_bool = true;
  size_t array_index_bool = 4;
  config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index_bool, config_input_value_bool);

  int config_input_value_int = 2;
  size_t array_index_int = 3;
  config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index_int, config_input_value_int);

  float config_input_value_float = 3.5f;
  size_t array_index_float = 2;
  config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index_float, config_input_value_float);

  // check if array index 0 is not overwriting existing array
  std::string config_input_value_string = "string";
  size_t array_index_string = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index_string, config_input_value_string);

  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index_bool, false, true, value_exists),
            config_input_value_bool);
  EXPECT_TRUE(value_exists);

  value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index_int, 0, true, value_exists),
            config_input_value_int);
  EXPECT_TRUE(value_exists);

  value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index_float, 0.0f, true, value_exists),
            config_input_value_float);
  EXPECT_TRUE(value_exists);

  value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index_string, "", true, value_exists),
            config_input_value_string);
  EXPECT_TRUE(value_exists);
  // not defined array index
  value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, 1, "default", true, value_exists),
            "default");
  EXPECT_FALSE(value_exists);
}

TEST(ConfigCompDataStorageTests, dataInfo_valid) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  {
    std::string key = "test2";
    bool config_input_value_bool = true;
    size_t array_index_bool = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index_bool, config_input_value_bool);

    // get single element
    size_t array_size = 1;
    EXPECT_TRUE(config_comp_data_storage.dataInfo(key, array_size));
    EXPECT_EQ(array_size, 1);
  }
  {
    std::string key = "test";
    bool config_input_value_bool = true;
    size_t array_index_bool = 2; // create an array with 3 values where the first two elements are set to default values
    config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index_bool, config_input_value_bool);

    // get array element
    size_t array_size = 0;
    EXPECT_TRUE(config_comp_data_storage.dataInfo(key, array_size));
    EXPECT_EQ(array_size, 3);
  }
}

TEST(ConfigCompDataStorageTests, dataInfo_invalid) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  size_t array_size = 1;
  EXPECT_FALSE(config_comp_data_storage.dataInfo("non_existing", array_size));
  EXPECT_EQ(array_size, 0);
}

TEST(ConfigCompDataStorageTests, configValueExists_single_element) {
  ConfigCompDataStorage::ConfigCompDataStorage config_comp_data_storage("Test");

  std::string key = "test";
  bool config_input_value_bool = true;
  size_t array_index_bool = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index_bool, config_input_value_bool);

  EXPECT_TRUE(config_comp_data_storage.configValueExists(key, array_index_bool));
  // url with array index has to be resolved in the comp_data class
  EXPECT_FALSE(config_comp_data_storage.configValueExists(key + "[0]", array_index_bool));
  EXPECT_FALSE(config_comp_data_storage.configValueExists(key, array_index_bool + 1));
  EXPECT_FALSE(config_comp_data_storage.configValueExists("not_existing", array_index_bool));
}
TEST(ConfigCompDataStorageTests, configValueExists_array_element) {
  ConfigCompDataStorage::ConfigCompDataStorage config_comp_data_storage("Test");

  std::string key = "test";
  bool config_input_value_bool = true;
  size_t array_index_bool = 2;
  config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index_bool, config_input_value_bool);

  EXPECT_TRUE(config_comp_data_storage.configValueExists(key, array_index_bool));
  EXPECT_FALSE(config_comp_data_storage.configValueExists(key, array_index_bool - 1));
  EXPECT_FALSE(config_comp_data_storage.configValueExists("test2[1]", array_index_bool));
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_bool) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test_bool";
  bool config_input_value = true;
  size_t array_index = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index, config_input_value);

  // check datatype conversion when another datatype is chosen as the one used for storage
  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), true);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 1);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, -1.0f, true, value_exists), 1.0f);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists), "1");
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_int) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  {
    std::string key = "test_int_positive";
    int config_input_value = 1;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), true);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 1);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, -1.0f, true, value_exists), 1.0f);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists), "1");
  }
  {
    std::string key = "test_int_negative";
    int config_input_value = -1;
    size_t array_index = 1;
    config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = true;
    // no loss free conversion possible to the target type
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
    EXPECT_FALSE(value_exists);

    value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 1, true, value_exists), -1);
    EXPECT_TRUE(value_exists);

    value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 1.0f, true, value_exists), -1.0f);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              "-1");
  }
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_float) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  {
    std::string key = "test_float_positive";
    float config_input_value = 1.0f;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), true);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 1);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, -1.0f, true, value_exists), 1.0f);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists), "1");
  }
  {
    std::string key = "test_float_negative";
    float config_input_value = -1.0f;
    size_t array_index = 1;
    config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 1, true, value_exists), -1);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 1.0f, true, value_exists), -1.0f);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              "-1");
  }
  {
    std::string key = "test_float_non_integger";
    float config_input_value = -3.5f;
    size_t array_index = 1;
    config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 1, true, value_exists), 1);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 1.0f, true, value_exists), -3.5f);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              "-3.5");
  }
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_boolean_number_string) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test_string_bool";
  std::string config_input_value = "1";
  size_t array_index = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

  // check datatype conversion when another datatype is chosen as the one used for storage
  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), true);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 1);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, -1.0f, true, value_exists), 1.0f);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists), "1");
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_boolean_true_string) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");
  // special test cases for "true" string conversion
  {
    std::string key = "test_string_true";
    std::string config_input_value = "true"; // value can't be represented exactly by a floating point number
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), true);
    EXPECT_TRUE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 1);
    EXPECT_TRUE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 2.0f, true, value_exists), 1.0f);
    EXPECT_TRUE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              config_input_value);
    EXPECT_TRUE(value_exists);
  }
  {
    std::string key = "test_string_true2";
    std::string config_input_value = "TRUE"; // value can't be represented exactly by a floating point number
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), true);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 1);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 2.0f, true, value_exists), 1.0f);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              config_input_value);
  }
  {
    std::string key = "test_string_invalid_true";
    std::string config_input_value = "atrue"; // value can't be represented exactly by a floating point number
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
    EXPECT_FALSE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), -1);
    EXPECT_FALSE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 2.0f, true, value_exists), 2.0f);
    EXPECT_FALSE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              config_input_value);
    EXPECT_TRUE(value_exists);
  }
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_boolean_false_string) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");
  // special test cases for "false" string conversion
  {
    std::string key = "test_string_false";
    std::string config_input_value = "false"; // value can't be represented exactly by a floating point number
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
    EXPECT_TRUE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 0);
    EXPECT_TRUE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 2.0f, true, value_exists), 0.0f);
    EXPECT_TRUE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              config_input_value);
    EXPECT_TRUE(value_exists);
  }
  {
    std::string key = "test_string_false2";
    std::string config_input_value = "fALSE"; // value can't be represented exactly by a floating point number
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), 0);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 2.0f, true, value_exists), 0.0f);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              config_input_value);
  }
  {
    std::string key = "test_string_invalid_false";
    std::string config_input_value = "falsea"; // value can't be represented exactly by a floating point number
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

    // check datatype conversion when another datatype is chosen as the one used for storage
    bool value_exists = false;
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, true, true, value_exists), true);
    EXPECT_FALSE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), -1);
    EXPECT_FALSE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 2.0f, true, value_exists), 2.0f);
    EXPECT_FALSE(value_exists);
    EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
              config_input_value);
    EXPECT_TRUE(value_exists);
  }
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_negative_int_string) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test_string_negative";
  std::string config_input_value = "-1";
  size_t array_index = 1;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

  // check datatype conversion when another datatype is chosen as the one used for storage
  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 1, true, value_exists), -1);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 1.0f, true, value_exists), -1.0f);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists), "-1");
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_float_string) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");
  std::string key = "test_string_double";
  std::string config_input_value = "5.12345678987"; // value can't be represented exactly by a floating point number
  float expected_value = (float)5.12345678987;
  size_t array_index = 1;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

  // check datatype conversion when another datatype is chosen as the one used for storage
  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), -1);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 1.0f, true, value_exists),
            expected_value);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
            config_input_value);
}

TEST(ConfigCompDataStorageTests, get_datatype_conversion_from_text_string) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test_string_text";
  std::string config_input_value = "Test"; // value can't be represented exactly by a floating point number
  size_t array_index = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);

  // check datatype conversion when another datatype is chosen as the one used for storage
  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index, false, true, value_exists), false);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, -1, true, value_exists), -1);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 1.0f, true, value_exists), 1.0f);
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<std::string>(key, array_index, "", true, value_exists),
            config_input_value);
}

TEST(ConfigCompDataStorageTests, get_non_existing_key) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "not_existing";
  size_t array_index = 0;

  // Request non existing key at index 0 -> get default value
  bool value_exists = true;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 10, false, value_exists), 10);
  EXPECT_FALSE(value_exists);

  // Request non existing key at index 10 -> get default value
  size_t array_index_extended = 10;
  value_exists = true;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index_extended, 10, false, value_exists), 10);
  EXPECT_FALSE(value_exists);
}

TEST(ConfigCompDataStorageTests, get_non_existing_array_index) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  int config_input_value = 10;
  size_t array_index = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value);

  // non existing array index -> get default value
  size_t array_index_extended = 10;
  bool value_exists = true;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index_extended, 20, false, value_exists), 20);
  EXPECT_FALSE(value_exists);
}

TEST(ConfigCompDataStorageTests, get_not_defined_array_index) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  bool config_input_value = false;
  size_t array_index = 10;
  config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index, config_input_value);

  // not defined array value -> get default value
  size_t array_index_not_defined = 0;
  bool value_exists = true;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<bool>(key, array_index_not_defined, true, true, value_exists),
            true);
  EXPECT_FALSE(value_exists);
}

TEST(ConfigCompDataStorageTests, getValueListFromDatabase_different_data_types) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  bool config_input_value_bool = true;
  size_t array_index_bool = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index_bool, config_input_value_bool);

  int config_input_value_int = 2;
  size_t array_index_int = 2;
  config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index_int, config_input_value_int);

  float config_input_value_float = 3.5f;
  size_t array_index_float = 3;
  config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index_float, config_input_value_float);

  std::string config_input_value_string = "string";
  size_t array_index_string = 5;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index_string, config_input_value_string);

  std::vector<std::string> dataValues = config_comp_data_storage.getValueListFromDatabase(key);
  EXPECT_EQ(dataValues.size(), 4); // default value at index 1 and index 4 is ignored
  EXPECT_EQ(dataValues.at(array_index_bool), std::to_string(config_input_value_bool));
  EXPECT_EQ(dataValues.at(array_index_int - 1), std::to_string(config_input_value_int));
  EXPECT_EQ(dataValues.at(array_index_float - 1), std::to_string(config_input_value_float));
  EXPECT_EQ(dataValues.at(array_index_string - 2), config_input_value_string);
}

TEST(ConfigCompDataStorageTests, getValueListFromDatabase_invalid_key) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  std::vector<std::string> dataValues = config_comp_data_storage.getValueListFromDatabase(key);
  EXPECT_TRUE(dataValues.empty());
}

TEST(ConfigCompDataStorageTests, getValueKeyPairListFromDatabase_simpleValue) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "simpleValue";
  std::string config_input_value_simple = "value";
  size_t array_index_simple = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index_simple, config_input_value_simple);

  std::map<std::string, std::string> dataValues = config_comp_data_storage.getValueKeyPairListFromDatabase(key);
  EXPECT_EQ(dataValues.size(), 1);
  EXPECT_EQ(dataValues.at("simpleValue"), config_input_value_simple);
}

TEST(ConfigCompDataStorageTests, getValueKeyPairListFromDatabase_different_data_types_array) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  bool config_input_value_bool = true;
  size_t array_index_bool = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index_bool, config_input_value_bool);

  int config_input_value_int = 2;
  size_t array_index_int = 2;
  config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index_int, config_input_value_int);

  float config_input_value_float = 3.5f;
  size_t array_index_float = 3;
  config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index_float, config_input_value_float);

  std::string config_input_value_string = "string";
  size_t array_index_string = 5;
  config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index_string, config_input_value_string);

  std::map<std::string, std::string> dataValues = config_comp_data_storage.getValueKeyPairListFromDatabase(key);
  EXPECT_EQ(dataValues.size(), 4); // default value at index 1 and index 4 is ignored
  EXPECT_EQ(dataValues.at("test[0]"), std::to_string(config_input_value_bool));
  EXPECT_EQ(dataValues.at("test[2]"), std::to_string(config_input_value_int));
  EXPECT_EQ(dataValues.at("test[3]"), std::to_string(config_input_value_float));
  EXPECT_EQ(dataValues.at("test[5]"), config_input_value_string);
}

TEST(ConfigCompDataStorageTests, getValueKeyPairListFromDatabase_invalid_key) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  std::map<std::string, std::string> dataValues = config_comp_data_storage.getValueKeyPairListFromDatabase(key);
  EXPECT_TRUE(dataValues.empty());
}

TEST(ConfigCompDataStorageTests, put_and_get_overwriting_with_different_datatype_simple_value) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  float config_input_value_float = 5.5f;
  size_t array_index = 0;
  config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value_float);

  ConfigValue config_output_value;
  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 0.f, true, value_exists),
            config_input_value_float);
  EXPECT_TRUE(value_exists);

  // overwrite existing value with different data type
  int config_input_value_int = 10;
  config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value_int);

  value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 0, true, value_exists),
            config_input_value_int);
  EXPECT_TRUE(value_exists);
}

TEST(ConfigCompDataStorageTests, put_and_get_overwriting_with_different_datatype_array_value) {
  auto config_comp_data_storage = ConfigCompDataStorage::ConfigCompDataStorage("Test");

  std::string key = "test";
  float config_input_value_float = 10.5f;
  size_t array_index = 10;
  config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value_float);

  bool value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<float>(key, array_index, 0.f, true, value_exists),
            config_input_value_float);
  EXPECT_TRUE(value_exists);

  // overwrite existing value with different data type
  int config_input_value_int = -20;
  config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value_int);
  value_exists = false;
  EXPECT_EQ(config_comp_data_storage.getDataFromDatabase<int>(key, array_index, 0, true, value_exists),
            config_input_value_int);
  EXPECT_TRUE(value_exists);
}

TEST(ConfigCompDataStorageTests, getDatabase) {
  ConfigCompDataStorage::ConfigCompDataStorage config_comp_data_storage("Test");
  {
    std::string key = "testbool";
    bool config_input_value = true;
    size_t array_index = 4;
    config_comp_data_storage.putDataInDatabaseTemplate<bool>(key, array_index, config_input_value);
  }
  {
    std::string key = "testint[2]";
    int config_input_value = -1;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<int>(key, array_index, config_input_value);
  }
  {
    std::string key = "testfloat";
    float config_input_value = -1.1f;
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<float>(key, array_index, config_input_value);
  }
  {
    std::string key = "teststring";
    std::string config_input_value = "string";
    size_t array_index = 0;
    config_comp_data_storage.putDataInDatabaseTemplate<std::string>(key, array_index, config_input_value);
  }

  std::map<std::string, std::string> map_database = config_comp_data_storage.getDatabase();
  EXPECT_EQ(map_database.size(), 4);
  EXPECT_EQ(map_database["testbool[4]"], "1");
  EXPECT_EQ(map_database["testint[2]"], "-1");
  EXPECT_EQ(map_database["testfloat"], "-1.100000");
  EXPECT_EQ(map_database["teststring"], "string");
}

} // namespace appsupport
} // namespace next
