/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_comp_data.cpp
 * @brief    Handles the config database from one component.
 **/

#include "config_comp_data.hpp"

#include "config_comp_data_impl.hpp"

namespace next {
namespace appsupport {

ConfigCompData::ConfigCompData(const std::string &comp_name) { impl_ = new ConfigCompData_Impl(comp_name); }

ConfigCompData::~ConfigCompData() {
  if (impl_)
    delete impl_;
}

void ConfigCompData::put(const std::string &key, const int &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigCompData::put(const std::string &key, const float &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigCompData::put(const std::string &key, const std::string &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigCompData::put(const std::string &key, const char *value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

void ConfigCompData::put(const std::string &key, const bool &value) {
  if (impl_) {
    impl_->put(key, value);
  }
}

int ConfigCompData::getInt(const std::string &key, const int &default_value, bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getInt(key, default_value, value_available);
}
float ConfigCompData::getFloat(const std::string &key, const float &default_value, bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getFloat(key, default_value, value_available);
}

std::string ConfigCompData::getString(const std::string &key, const std::string &default_value,
                                      bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getString(key, default_value, value_available);
}

bool ConfigCompData::getBool(const std::string &key, const bool &default_value, bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return default_value;
  }
  return impl_->getBool(key, default_value, value_available);
}

std::vector<std::string> ConfigCompData::getChildrenKeys(const std::string &key, bool full_path) const {
  if (!impl_) {
    return std::vector<std::string>();
  }
  return impl_->getChildrenKeys(key, full_path);
}

std::vector<std::string> ConfigCompData::getValuesAsStringList(const std::string &key,
                                                               const std::vector<std::string> defaults,
                                                               bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return std::vector<std::string>();
  }
  return impl_->getValuesAsStringList(key, defaults, value_available);
}

std::map<std::string, std::string>
ConfigCompData::getValueKeyPairsAsStringList(const std::string &key, const std::map<std::string, std::string> defaults,
                                             bool &value_available) const {
  if (!impl_) {
    value_available = false;
    return std::map<std::string, std::string>();
  }
  return impl_->getValueKeyPairsAsStringList(key, defaults, value_available);
}

std::map<std::string, std::string> ConfigCompData::getCompData() const {
  std::map<std::string, std::string> map_comp_data;
  if (impl_) {
    map_comp_data = impl_->getCompData();
  }
  return map_comp_data;
}

void ConfigCompData::traverseTree(
    const std::string &key,
    std::function<void(const std::string &, const std::string &)> call_for_config_values) const {
  if (impl_) {
    impl_->traverseTree(key, call_for_config_values);
  }
}

bool ConfigCompData::isEmpty() const {
  bool retVal{true};
  if (impl_) {
    retVal = impl_->isEmpty();
  }
  return retVal;
}

bool ConfigCompData::configValueExists(const std::string &key) const {
  bool retVal{false};
  if (impl_) {
    retVal = impl_->configValueExists(key);
  }
  return retVal;
}

bool ConfigCompData::keyInfo(const std::string &key, bool &final_leaf, size_t &array_size) const {
  bool retVal{false};
  if (impl_) {
    retVal = impl_->keyInfo(key, final_leaf, array_size);
  }
  return retVal;
}

std::string ConfigCompData::getCompName() const {
  std::string comp_name{""};
  if (impl_) {
    comp_name = impl_->getCompName();
  }
  return comp_name;
}

} // namespace appsupport
} // namespace next
