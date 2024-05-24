/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_manager.cpp
 * @brief    Manages the configurations of different components
 *
 **/

#include "config_database_manager.hpp"

#include "config_database_manager_impl.hpp"

namespace next {
namespace appsupport {

ConfigDatabaseManager::ConfigDatabaseManager(const std::string &root_name, bool filter_by_comp_name) {
  impl_ = new ConfigDatabaseManager_Impl(root_name, filter_by_comp_name);
}

ConfigDatabaseManager::~ConfigDatabaseManager() {
  if (impl_)
    delete impl_;
}

void ConfigDatabaseManager::put(const std::string &key, const int &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigDatabaseManager::put(const std::string &key, const float &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigDatabaseManager::put(const std::string &key, const std::string &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigDatabaseManager::put(const std::string &key, const char *value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigDatabaseManager::put(const std::string &key, const bool &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

int ConfigDatabaseManager::getInt(const std::string &key, const int &default_value, bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getInt(key, default_value, value_available);
}
float ConfigDatabaseManager::getFloat(const std::string &key, const float &default_value, bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getFloat(key, default_value, value_available);
}

std::string ConfigDatabaseManager::getString(const std::string &key, const std::string &default_value,
                                             bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getString(key, default_value, value_available);
}

bool ConfigDatabaseManager::getBool(const std::string &key, const bool &default_value, bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getBool(key, default_value, value_available);
}

std::vector<std::string> ConfigDatabaseManager::getChildrenKeys(const std::string &key, bool full_path,
                                                                bool &comp_available) const {
  if (!impl_) {
    comp_available = false;
    return std::vector<std::string>();
  }
  return impl_->getChildrenKeys(key, full_path, comp_available);
}

std::vector<std::string> ConfigDatabaseManager::getValuesAsStringList(const std::string &key,
                                                                      const std::vector<std::string> defaults,
                                                                      bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return std::vector<std::string>();
  }
  return impl_->getValuesAsStringList(key, defaults, value_available);
}

std::map<std::string, std::string> ConfigDatabaseManager::getCompData(const std::string &comp_name,
                                                                      bool &comp_available) const {
  std::map<std::string, std::string> map_comp_data;
  comp_available = false;
  if (impl_) {
    map_comp_data = impl_->getCompData(comp_name, comp_available);
  }
  return map_comp_data;
}

bool ConfigDatabaseManager::isCompEmpty(const std::string &comp_name, bool &comp_available) const {
  bool retVal{true};
  comp_available = false;
  if (impl_) {
    retVal = impl_->isCompEmpty(comp_name, comp_available);
  }
  return retVal;
}

bool ConfigDatabaseManager::configValueExists(const std::string &key, bool &comp_available) const {
  bool retVal{false};
  comp_available = false;
  if (impl_) {
    retVal = impl_->configValueExists(key, comp_available);
  }
  return retVal;
}

bool ConfigDatabaseManager::keyInfo(const std::string &key, bool &final_leaf, size_t &array_size,
                                    bool &comp_available) const {
  bool retVal{false};
  comp_available = false;
  if (impl_) {
    retVal = impl_->keyInfo(key, final_leaf, array_size, comp_available);
  }
  return retVal;
}

void ConfigDatabaseManager::putCfg(const std::string &filePath) {
  if (impl_) {
    impl_->putCfg(filePath);
  }
}

void ConfigDatabaseManager::insertJsonString(const std::string &json_data) {
  if (impl_) {
    impl_->insertJsonString(json_data);
  }
}

std::string ConfigDatabaseManager::getOneCompConfigAsJsonString(const std::string &comp_name, const std::string &key,
                                                                bool &comp_available, bool formatted) {
  std::string return_value{""};
  comp_available = false;
  if (impl_) {
    return impl_->getOneCompConfigAsJsonString(comp_name, key, comp_available, formatted);
  }
  return return_value;
}

std::string ConfigDatabaseManager::getAllCompConfigsAsJsonString(std::string key, bool formatted) {
  std::string return_value{""};
  if (impl_) {
    return impl_->getAllCompConfigsAsJsonString(key, formatted);
  }
  return return_value;
}

std::string ConfigDatabaseManager::getDifferences(std::string json_data) {
  std::string return_value{""};
  if (impl_) {
    return impl_->getDifferences(json_data);
  }
  return return_value;
}

std::string ConfigDatabaseManager::getRootName() const {
  std::string retVal{""};
  if (impl_) {
    retVal = impl_->getRootName();
  }
  return retVal;
}

std::vector<std::string> ConfigDatabaseManager::getConfigCompNames() const {
  std::vector<std::string> retVal{};
  if (impl_) {
    retVal = impl_->getConfigCompNames();
  }
  return retVal;
}

bool ConfigDatabaseManager::isComponentInConfig(const std::string &comp_name) {
  bool retVal = false;
  if (impl_) {
    retVal = impl_->isComponentInConfig(comp_name);
  }
  return retVal;
}

bool ConfigDatabaseManager::addNewComponentConfig(const std::string &comp_name) {
  bool retVal = false;
  if (impl_) {
    retVal = impl_->addNewComponentConfig(comp_name);
  }
  return retVal;
}

void ConfigDatabaseManager::clear() {
  if (impl_) {
    return impl_->clear();
  }
}

void ConfigDatabaseManager::insertConfig(ConfigDatabaseManager &config_data_base_source) {
  if (impl_) {
    return impl_->insertConfig(config_data_base_source.impl_);
  }
}

void ConfigDatabaseManager::enableFilterByComponent() {
  if (impl_) {
    impl_->enableFilterByComponent();
  }
}

void ConfigDatabaseManager::disableFilterByComponent() {
  if (impl_) {
    impl_->disableFilterByComponent();
  }
}

bool ConfigDatabaseManager::getFilterByComponent() {
  if (impl_) {
    return impl_->getFilterByComponent();
  }
  return false;
}

void ConfigDatabaseManager::setFilterByComponent(bool filter_by_comp) {
  if (impl_) {
    impl_->setFilterByComponent(filter_by_comp);
  }
}

} // namespace appsupport
} // namespace next
