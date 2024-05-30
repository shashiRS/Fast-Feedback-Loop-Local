/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_manager_impl.cpp
 * @brief    Manages the configurations of different components
 *
 **/

#include "config_database_manager_impl.hpp"

#include <next/sdk/logger/logger.hpp>

#include "config_database_IO.hpp"
#include "config_utilities.hpp"

namespace next {
namespace appsupport {

ConfigDatabaseManager_Impl::ConfigDatabaseManager_Impl(const std::string &root_name, bool filter_by_comp_name)
    : root_name_(root_name), filter_by_comp_name_(filter_by_comp_name) {}

ConfigDatabaseManager_Impl::~ConfigDatabaseManager_Impl() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
}

template <typename T>
void ConfigDatabaseManager_Impl::putTemplate(const std::string &key, const T &value) {
  static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, std::string> ||
                    std::is_same_v<T, bool>,
                "put function only supports int,bool, float and std::string types");
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  std::string comp_name = ConfigUtilities::getComponentNameFromKey(key);

  auto it_config_comp_data = map_config_comp_data_.find(comp_name);
  // skip if filtering is activated and component doesn't exist already
  if (it_config_comp_data == map_config_comp_data_.end() && true == filter_by_comp_name_) {
    debug(__FILE__, "Config key '{}' is skipped because component is not existing in ConfigDatabaseManager.", key);
    return;
  } else if (it_config_comp_data == map_config_comp_data_.end() && false == filter_by_comp_name_) {
    // check if component name is valid
    if (!ConfigUtilities::checkComponentName(comp_name)) {
      return;
    }
    // create new component if it doesn't exist and filtering is deactivated
    auto res = map_config_comp_data_.try_emplace(comp_name, comp_name);
    it_config_comp_data = res.first;
  }

  it_config_comp_data->second.put(key, value);
}

void ConfigDatabaseManager_Impl::put(const std::string &key, const int &value) { putTemplate(key, value); }

void ConfigDatabaseManager_Impl::put(const std::string &key, const float &value) { putTemplate(key, value); }

void ConfigDatabaseManager_Impl::put(const std::string &key, const std::string &value) { putTemplate(key, value); }

void ConfigDatabaseManager_Impl::put(const std::string &key, const char *value) {
  putTemplate(key, std::string(value));
}

void ConfigDatabaseManager_Impl::put(const std::string &key, const bool &value) { putTemplate(key, value); }

void ConfigDatabaseManager_Impl::insertConfig(ConfigDatabaseManager_Impl *config_data_base_source) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  ConfigDatabaseIO::insertConfigCompData(config_data_base_source->map_config_comp_data_, map_config_comp_data_,
                                         filter_by_comp_name_);
}

int ConfigDatabaseManager_Impl::getInt(const std::string &key, const int &default_value, bool &value_available) {
  value_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    return it_config_comp_data->second.getInt(key, default_value, value_available);
  }

  return default_value;
}

float ConfigDatabaseManager_Impl::getFloat(const std::string &key, const float &default_value, bool &value_available) {
  value_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    return it_config_comp_data->second.getFloat(key, default_value, value_available);
  }

  return default_value;
}

std::string ConfigDatabaseManager_Impl::getString(const std::string &key, const std::string &default_value,
                                                  bool &value_available) {
  value_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    return it_config_comp_data->second.getString(key, default_value, value_available);
  }

  return default_value;
}

bool ConfigDatabaseManager_Impl::getBool(const std::string &key, const bool &default_value, bool &value_available) {
  value_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    return it_config_comp_data->second.getBool(key, default_value, value_available);
  }

  return default_value;
}

std::vector<std::string> ConfigDatabaseManager_Impl::getChildrenKeys(const std::string &key, bool full_path,
                                                                     bool &comp_available) {
  comp_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    comp_available = true;
    return it_config_comp_data->second.getChildrenKeys(key, full_path);
  }

  return std::vector<std::string>{};
}

std::vector<std::string> ConfigDatabaseManager_Impl::getValuesAsStringList(
    const std::string &key, const std::vector<std::string> &default_values, bool &value_available) {
  value_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    return it_config_comp_data->second.getValuesAsStringList(key, default_values, value_available);
  }

  return default_values;
}

std::map<std::string, std::string> ConfigDatabaseManager_Impl::getCompData(const std::string &comp_name,
                                                                           bool &comp_available) {
  comp_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(comp_name);
  if (it_config_comp_data != map_config_comp_data_.end()) {
    comp_available = true;
    return it_config_comp_data->second.getCompData();
  }

  return std::map<std::string, std::string>{};
}

bool ConfigDatabaseManager_Impl::isCompEmpty(const std::string &comp_name, bool &comp_available) {
  comp_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(comp_name);
  if (it_config_comp_data != map_config_comp_data_.end()) {
    comp_available = true;
    return it_config_comp_data->second.isEmpty();
  }

  return true;
}

bool ConfigDatabaseManager_Impl::configValueExists(const std::string &key, bool &comp_available) {
  comp_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    comp_available = true;
    return it_config_comp_data->second.configValueExists(key);
  }

  return false;
}

bool ConfigDatabaseManager_Impl::keyInfo(const std::string &key, bool &final_leaf, size_t &array_size,
                                         bool &comp_available) {
  // set default return values
  comp_available = false;
  final_leaf = false;
  array_size = 0;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(ConfigUtilities::getComponentNameFromKey(key));
  if (it_config_comp_data != map_config_comp_data_.end()) {
    comp_available = true;
    return it_config_comp_data->second.keyInfo(key, final_leaf, array_size);
  }

  return false;
}

void ConfigDatabaseManager_Impl::clear() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  map_config_comp_data_.clear();
}

void ConfigDatabaseManager_Impl::putCfg(const std::string &filePath) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  ConfigDatabaseIO::putCfg(filePath, map_config_comp_data_, filter_by_comp_name_);
}

void ConfigDatabaseManager_Impl::insertJsonString(const std::string &json_data) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  ConfigDatabaseIO::insertJsonString(json_data, map_config_comp_data_, filter_by_comp_name_);
}

std::string ConfigDatabaseManager_Impl::getOneCompConfigAsJsonString(const std::string &comp_name,
                                                                     const std::string &key, bool &comp_available,
                                                                     bool formatted) {
  comp_available = false;

  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  auto it_config_comp_data = map_config_comp_data_.find(comp_name);
  if (it_config_comp_data != map_config_comp_data_.end()) {
    comp_available = true;
    return ConfigDatabaseIO::getCompConfigAsJsonString(it_config_comp_data->second, key, formatted);
  }

  return "";
}
std::string ConfigDatabaseManager_Impl::getAllCompConfigsAsJsonString(std::string key, bool formatted) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return ConfigDatabaseIO::getCompConfigAsJsonString(map_config_comp_data_, key, formatted);
}

std::string ConfigDatabaseManager_Impl::getDifferences(std::string json_data) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return ConfigDatabaseIO::getDifferences(json_data, map_config_comp_data_);
}

std::string ConfigDatabaseManager_Impl::getRootName() const { return root_name_; }

std::vector<std::string> ConfigDatabaseManager_Impl::getConfigCompNames() const {
  std::vector<std::string> comp_name_list{};
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  for (const auto &it_comp_data : map_config_comp_data_) {
    comp_name_list.push_back(it_comp_data.first);
  }
  return comp_name_list;
}

bool ConfigDatabaseManager_Impl::isComponentInConfig(const std::string &comp_name) {
  bool retVal = false;
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  if (map_config_comp_data_.find(comp_name) != map_config_comp_data_.end())
    retVal = true;
  return retVal;
}

bool ConfigDatabaseManager_Impl::addNewComponentConfig(const std::string &comp_name) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  if (!ConfigUtilities::checkComponentName(comp_name)) {
    next::sdk::logger::warn(__FILE__, "Component name {0} is not supported! Nothing added.", comp_name);
    return false;
  }

  auto res = map_config_comp_data_.try_emplace(comp_name, comp_name);
  if (false == res.second) {
    next::sdk::logger::warn(__FILE__, "Component {0} already exists! Nothing added.", comp_name);
  }
  return res.second;
}

void ConfigDatabaseManager_Impl::enableFilterByComponent() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  filter_by_comp_name_ = true;
}

void ConfigDatabaseManager_Impl::disableFilterByComponent() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  filter_by_comp_name_ = false;
}

bool ConfigDatabaseManager_Impl::getFilterByComponent() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return filter_by_comp_name_;
}

void ConfigDatabaseManager_Impl::setFilterByComponent(bool filter_by_comp) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  filter_by_comp_name_ = filter_by_comp;
}

} // namespace appsupport
} // namespace next
