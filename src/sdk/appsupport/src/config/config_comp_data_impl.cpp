/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_data_impl.cpp
 * @brief    Handles the config database
 *
 * Implementation of the functions for handling of the config database.
 *
 **/

#include "config_comp_data_impl.hpp"

#include <exception>

#include <next/sdk/logger/logger.hpp>

#include "config_utilities.hpp"

namespace pt = boost::property_tree;

namespace next {
namespace appsupport {

ConfigCompData_Impl::ConfigCompData_Impl(const std::string &comp_name) {
  comp_name_ = comp_name;
  config_data_storage_ = std::make_shared<ConfigCompDataStorage::ConfigCompDataStorage>(comp_name);
}

ConfigCompData_Impl::~ConfigCompData_Impl() {}

template <typename T>
void ConfigCompData_Impl::putTemplate(const std::string &key, const T &value) {
  static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, std::string> ||
                    std::is_same_v<T, bool>,
                "put function only supports int,bool, float and std::string types");

  if (key.empty()) {
    // TODO how to handle if key is invalid? Expection or return value?
    warn(__FILE__, "Empty key '{}' not allowed for putting values into config.", key);
    return;
  }

  std::string key_path{""};
  size_t array_index{0};
  bool array_in_path{false};
  if (!ConfigUtilities::checkKeyPath(key, key_path, array_index, array_in_path)) {
    warn(__FILE__, "Invalid key '{}' for putting values into config. No value added to config", key);
    return;
  }

  // add key to config tree ->all missing sub childs will be created automatically.
  // Add boolean as small datatype. Data itself are stored in unordered_map. ptree is only used for hierarchical
  // structure of keys
  config_data_tree_.put(pt::ptree::path_type{key_path, kConfigurationKeyDelimiter}, true);

  config_data_storage_->putDataInDatabaseTemplate(key_path, array_index, value);
}

void ConfigCompData_Impl::put(const std::string &key, const int &value) { putTemplate(key, value); }

void ConfigCompData_Impl::put(const std::string &key, const float &value) { putTemplate(key, value); }

void ConfigCompData_Impl::put(const std::string &key, const std::string &value) { putTemplate(key, value); }

void ConfigCompData_Impl::put(const std::string &key, const char *value) { putTemplate(key, std::string(value)); }

void ConfigCompData_Impl::put(const std::string &key, const bool &value) { putTemplate(key, value); }

int ConfigCompData_Impl::getInt(const std::string &key, const int &default_value, bool &value_available) const {
  return getTemplate(key, default_value, true, value_available);
}

float ConfigCompData_Impl::getFloat(const std::string &key, const float &default_value, bool &value_available) const {
  return getTemplate(key, default_value, true, value_available);
}

std::string ConfigCompData_Impl::getString(const std::string &key, const std::string &default_value,
                                           bool &value_available) const {
  return getTemplate(key, default_value, true, value_available);
}

bool ConfigCompData_Impl::getBool(const std::string &key, const bool &default_value, bool &value_available) const {
  return getTemplate(key, default_value, true, value_available);
}

template <typename T>
T ConfigCompData_Impl::getTemplate(const std::string &key, const T &default_value, const bool &default_value_defined,
                                   bool &value_available) const {
  static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, std::string> ||
                    std::is_same_v<T, bool>,
                "get function only supports int, bool, float and std::string types");

  value_available = false;

  std::string key_path{""};
  size_t array_index{0};
  if (!ConfigUtilities::extractKeyPath(key, key_path, array_index)) {
    warn(__FILE__, "Invalid key '{}' for getting values from config. Default value returned.", key);
    return default_value;
  }

  return config_data_storage_->getDataFromDatabase<T>(key_path, array_index, default_value, default_value_defined,
                                                      value_available);
}

std::vector<std::string> ConfigCompData_Impl::getChildrenKeys(const std::string &key, bool full_path) const {
  std::vector<std::string> key_list{};

  auto child_ptree = config_data_tree_.get_child_optional(pt::ptree::path_type{key, kConfigurationKeyDelimiter});
  if (!child_ptree) {
    return key_list;
  }
  for (auto &it_childs : child_ptree.get()) {
    std::string children_key{""};
    if (!full_path) {
      children_key = it_childs.first;
    } else {
      children_key = key;
      // add a separator between key segments
      if (!key.empty()) {
        children_key += kConfigurationKeyDelimiter;
      }
      children_key += it_childs.first;
    }

    // add key of childs
    key_list.push_back(children_key);
  }

  return key_list;
}

std::map<std::string, std::string> ConfigCompData_Impl::getCompData() const {
  return config_data_storage_->getDatabase();
}

void ConfigCompData_Impl::traverseTree(
    const std::string &key,
    std::function<void(const std::string &, const std::string &)> call_for_config_values) const {
  std::string key_path{""};
  size_t array_index{0};
  if (!ConfigUtilities::extractKeyPath(key, key_path, array_index)) {
    warn(__FILE__, "Invalid key '{}' for getting values from config. Callback not called.", key);
    return;
  }

  // check for root node
  if (key.empty()) {
    walkTreeAndCallFunc(config_data_tree_, key, call_for_config_values);
  } else {
    // get node for this key
    auto child_ptree = config_data_tree_.get_child_optional(pt::ptree::path_type{key_path, kConfigurationKeyDelimiter});
    if (child_ptree) {
      walkTreeAndCallFunc(child_ptree.get(), key, call_for_config_values);
    } else {
      warn(__FILE__, "Key '{}' doesn't exist in config. Callback not called.", key);
    }
  }
}

void ConfigCompData_Impl::walkTreeAndCallFunc(
    const treeStorageType &tree, const std::string &key,
    std::function<void(const std::string &, const std::string &)> &call_for_config_values) const {
  // call "call_for_config_values" if key is a final leaf -> config can have only values at the final leaf
  // Hint: tree.data().empty() returns as well empty for an empty string
  if (tree.empty()) {
    bool _;
    std::map<std::string, std::string> config_key_value_pairs =
        getValueKeyPairsAsStringList(key, std::map<std::string, std::string>{}, _);
    for (const auto &[config_key, config_value] : config_key_value_pairs) {
      call_for_config_values(config_key, config_value);
    }
    return;
  }

  size_t array_index = 0;
  for (auto &it_child : tree) {
    std::string child_key = key;
    // check if child contains an array
    if (!it_child.first.empty()) {
      // add a separator between key segments if it's not the root node
      if (!child_key.empty()) {
        child_key += kConfigurationKeyDelimiter;
      }
      child_key += it_child.first;
    } else {
      child_key += "[" + std::to_string(array_index) + "]";
      ++array_index;
    }

    // walk child nodes
    walkTreeAndCallFunc(it_child.second, child_key, call_for_config_values);
  }
}

bool ConfigCompData_Impl::isEmpty() const { return config_data_storage_->isEmpty(); }

std::vector<std::string> ConfigCompData_Impl::getValuesAsStringList(const std::string &key,
                                                                    const std::vector<std::string> &default_values,
                                                                    bool &value_available) const {
  value_available = false;

  std::string key_path{""};
  size_t array_index{0};
  bool array_in_key_path;
  if (!ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path)) {
    warn(__FILE__, "Invalid key '{}' for getting values from config. Default value returned.", key);
    return default_values;
  }

  std::vector<std::string> string_list;
  // check if array index is in the path
  if (!array_in_key_path) {
    // get all available data for this key (single data or array)
    string_list = config_data_storage_->getValueListFromDatabase(key_path);
  } else {
    // get only the data for the specified index
    bool value_found{false};
    std::string config_value =
        config_data_storage_->getDataFromDatabase<std::string>(key_path, array_index, "", true, value_found);
    if (value_found) {
      string_list.push_back(config_value);
    }
  }

  if (!string_list.empty()) {
    value_available = true;
    return string_list;
  } else {
    return default_values;
  }
}

std::map<std::string, std::string> ConfigCompData_Impl::getValueKeyPairsAsStringList(
    const std::string &key, const std::map<std::string, std::string> &default_values, bool &value_available) const {
  value_available = false;

  std::string key_path{""};
  size_t array_index{0};
  bool array_in_key_path;
  if (!ConfigUtilities::extractKeyPath(key, key_path, array_index, array_in_key_path)) {
    warn(__FILE__, "Invalid key '{}' for getting values from config. Default value returned.", key);
    return default_values;
  }

  std::map<std::string, std::string> map_key_value_pair;
  if (!array_in_key_path) {
    // get all available data for this key (single data or array)
    map_key_value_pair = config_data_storage_->getValueKeyPairListFromDatabase(key_path);
  } else {
    // get only the data for the specified index
    bool value_found{false};
    std::string config_value =
        config_data_storage_->getDataFromDatabase<std::string>(key_path, array_index, "", true, value_found);
    if (value_found) {
      map_key_value_pair.emplace(key, config_value);
    }
  }

  if (!map_key_value_pair.empty()) {
    value_available = true;
    return map_key_value_pair;
  } else {
    return default_values;
  }
}

bool ConfigCompData_Impl::configValueExists(const std::string &key) const {
  std::string key_path{""};
  size_t array_index{0};
  if (!ConfigUtilities::extractKeyPath(key, key_path, array_index)) {
    return false;
  }
  return config_data_storage_->configValueExists(key_path, array_index);
}

bool ConfigCompData_Impl::keyInfo(const std::string &key, bool &final_leaf, size_t &array_size) const {
  final_leaf = false;
  array_size = 0;
  std::string key_path{""};
  size_t array_index{0};
  if (!ConfigUtilities::extractKeyPath(key, key_path, array_index)) {
    return false;
  }

  auto child_ptree = config_data_tree_.get_child_optional(pt::ptree::path_type{key_path, kConfigurationKeyDelimiter});
  if (!child_ptree) {
    return false;
  }
  if (child_ptree.get().empty()) {
    final_leaf = true;
  }
  return config_data_storage_->dataInfo(key_path, array_size);
}

} // namespace appsupport
} // namespace next
