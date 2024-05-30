/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_base.cpp
 * @brief    functions for configuration handling
 *
 **/

#include <next/appsupport/config/config_base.hpp>

#include <chrono>

#include "config_base_impl.hpp"

namespace next {
namespace appsupport {

ConfigBase::ConfigBase(const std::string &name, bool filter_by_comp_name) {
  _impl = new ConfigBase_Impl(name, filter_by_comp_name);
}

ConfigBase::~ConfigBase() {
  if (_impl)
    delete _impl;
}

size_t ConfigBase::addChangeHook(configtypes::change_hook hook) {
  if (_impl) {
    return _impl->addChangeHook(hook);
  }
  return 0;
}

void ConfigBase::removeChangeHook(size_t id) {
  if (_impl) {
    return _impl->removeChangeHook(id);
  }
}

void ConfigBase::put(const std::string &key, const int &value, const ConfigSource &source) {
  if (_impl) {
    _impl->put(key, value, source);
  }
}

void ConfigBase::put(const std::string &key, const float &value, const ConfigSource &source) {
  if (_impl) {
    _impl->put(key, value, source);
  }
}

void ConfigBase::put(const std::string &key, const std::string &value, const ConfigSource &source) {
  if (_impl) {
    _impl->put(key, value, source);
  }
}

void ConfigBase::put_option(const std::string &key, const bool &value, const ConfigSource &source) {
  if (_impl) {
    _impl->put(key, value, source);
  }
}

void ConfigBase::putCfg(const std::string &filePath, const ConfigSource &source) {
  if (_impl) {
    _impl->putCfg(filePath, source);
  }
}

int ConfigBase::get_int(const std::string &key, const int &default_value, bool block_active,
                        bool &value_available) const {
  int retVal = default_value;
  if (_impl) {
    retVal = _impl->get_int(key, default_value, block_active, value_available);
  }
  return retVal;
}
float ConfigBase::get_float(const std::string &key, const float &default_value, bool block_active,
                            bool &value_available) const {
  float retVal = default_value;
  if (_impl) {
    retVal = _impl->get_float(key, default_value, block_active, value_available);
  }
  return retVal;
}

std::string ConfigBase::get_string(const std::string &key, const std::string &default_value, bool block_active,
                                   bool &value_available) const {
  std::string returnValue = default_value;
  if (_impl) {
    returnValue = _impl->get_string(key, default_value, block_active, value_available);
  }
  return returnValue;
}

bool ConfigBase::get_option(const std::string &key, const bool &default_value, bool block_active,
                            bool &value_available) const {
  bool retVal = false;
  if (_impl) {
    retVal = _impl->get_option(key, default_value, block_active, value_available);
  }
  return retVal;
}

std::vector<std::string> ConfigBase::getChildren(const std::string &key, bool full_path) {
  std::vector<std::string> retVal;
  if (_impl) {

    retVal = _impl->getChildren(key, full_path);
  }
  return retVal;
}

std::vector<std::string> ConfigBase::getStringList(const std::string &key, const std::vector<std::string> &defaults,
                                                   bool block_active, bool &value_available) const {
  std::vector<std::string> retVal;
  if (_impl) {

    retVal = _impl->getStringList(key, defaults, block_active, value_available);
  }
  return retVal;
}

std::string ConfigBase::getAllConfigurationsAsJson(bool filterOutPrivateKeys, bool filterOutGenericKeys,
                                                   bool filterOutReadOnly, bool formatted) const {
  std::string retVal = "";
  if (_impl) {
    retVal =
        _impl->getAllConfigurationsAsJson(filterOutPrivateKeys, filterOutGenericKeys, filterOutReadOnly, formatted);
  }
  return retVal;
}
std::string ConfigBase::getComponentConfigurationsAsJson(const std::string &component, bool filterOutPrivateKeys,
                                                         bool formatted) const {
  std::string retVal = "";
  if (_impl) {
    retVal = _impl->getComponentConfigurationsAsJson(component, filterOutPrivateKeys, formatted);
  }
  return retVal;
}

void ConfigBase::insert(const std::string &json_str, bool filterOutPrivateKeys, bool resetActiveTag,
                        const ConfigSource &source) {
  if (_impl) {
    _impl->insert(json_str, filterOutPrivateKeys, resetActiveTag, source);
  }
}

// void ConfigBase::checkGenericConfig(const std::string &json_str) {
//  if (_impl) {
//    _impl->checkGenericConfig(json_str);
//  }
//}

std::string ConfigBase::getRootName() const {
  std::string retVal = {""};
  if (_impl) {
    retVal = _impl->getRootName();
  }
  return retVal;
}
std::vector<std::string> ConfigBase::componentName() const {
  std::vector<std::string> retVal = {""};
  if (_impl) {
    retVal = _impl->componentName();
  }
  return retVal;
}

void ConfigBase::finishInitialization() {
  if (_impl) {
    _impl->finishInitialization();
  }
}

void ConfigBase::clearTree() const {
  if (_impl) {
    _impl->clearTree();
  }
}

void ConfigBase::enableFilterByComponent() {
  if (_impl) {
    _impl->enableFilterByComponent();
  }
}

void ConfigBase::disableFilterByComponent() {
  if (_impl) {
    _impl->disableFilterByComponent();
  }
}

bool ConfigBase::getCurrentFilterByComponentStatus() {
  if (_impl) {
    return _impl->getCurrentFilterByComponentStatus();
  }
  return false;
}

void ConfigBase::setCurrentFilterByComponentStatus(bool filter_by_comp) {
  if (_impl) {
    _impl->setCurrentFilterByComponentStatus(filter_by_comp);
  }
}

void ConfigBase::waitForResponse() {
  if (_impl) {
    _impl->waitForResponse();
  }
}

void ConfigBase::setWaitForResponseFlag(bool value) {
  if (_impl) {
    _impl->setWaitForResponseFlag(value);
  }
}

void ConfigBase::setConfigValueFromServer(const std::string &value) {
  if (_impl) {
    _impl->setConfigValueFromServer(value);
  }
}

std::string ConfigBase::getConfigValueFromServer() {
  std::string retVal = "";
  if (_impl) {
    retVal = _impl->getConfigValueFromServer();
  }
  return retVal;
}

// void ConfigBase::definePrivateConfigKeys(std::vector<std::string> configKeys) {
//  if (_impl) {
//    _impl->definePrivateConfigKeys(configKeys);
//  }
//}

// void ConfigBase::putPrivateKey(std::string configKey) {
//  if (_impl) {
//    _impl->putPrivateKey(configKey);
//  }
//}

std::string ConfigBase::getDifferences(std::string inputJson) {
  std::string retVal = "";
  if (_impl) {
    retVal = _impl->getDifferences(inputJson);
  }
  return retVal;
}
bool ConfigBase::isComponentInCurrentTree(const std::string &componentName) {
  bool retVal = false;
  if (_impl) {
    retVal = _impl->isComponentInCurrentTree(componentName);
  }
  return retVal;
}

bool ConfigBase::addNewComponentToLocalTree(const std::string &componentName) {
  bool retVal = false;
  if (_impl) {
    retVal = _impl->addNewComponentToLocalTree(componentName);
  }
  return retVal;
}

bool ConfigBase::isInitFinished() const {
  bool retVal = true;
  if (_impl) {
    retVal = _impl->isInitFinished();
  }
  return retVal;
}
// void ConfigBase::setTag(const std::string &key, next::appsupport::configtags::TAG tag) {
//  if (_impl) {
//    _impl->setTag(key, tag);
//  }
//}
// void ConfigBase::setTag(const std::string &key, const std::string &tag) {
//  if (_impl) {
//    _impl->setTag(key, tag);
//  }
//}
/*std::vector<std::string> ConfigBase::getTags(const std::string &key) {
  std::vector<std::string> retVal;
  if (_impl) {

    retVal = _impl->getTags(key);
  }
  return retVal;
}*/
// std::string ConfigBase::getTreeFilteredByTagAsJson(std::string tag) {
//  std::string retVal = "";
//  if (_impl) {
//
//    retVal = _impl->getTreeFilteredByTagAsJson(tag);
//  }
//  return retVal;
//}

// std::string ConfigBase::getTreeFilteredByTagsIntersectionAsJson(std::vector<std::string> tagList) {
//  std::string retVal = "";
//  if (_impl) {
//
//    retVal = _impl->getTreeFilteredByTagsIntersectionAsJson(tagList);
//  }
//  return retVal;
//}
// std::string ConfigBase::getTreeFilteredByTagsUnionAsJson(std::vector<std::string> tagList) {
//  std::string retVal = "";
//  if (_impl) {
//
//    retVal = _impl->getTreeFilteredByTagsUnionAsJson(tagList);
//  }
//  return retVal;
//}

// void ConfigBase::removeTag(const std::string &key, next::appsupport::configtags::TAG tag) {
//  if (_impl) {
//    _impl->removeTag(key, tag);
//  }
//}
// void ConfigBase::removeTag(const std::string &key, const std::string &tag) {
//  if (_impl) {
//    _impl->removeTag(key, tag);
//  }
//}
/*void ConfigBase::removeAllTagsForKey(const std::string &key) {
  if (_impl) {
    _impl->removeAllTagsForKey(key);
  }
}*/

/*std::string ConfigBase::getTagsAsJsonStr() {
  std::string retVal = "";
  if (_impl) {
    retVal = _impl->getTagsAsJsonStr();
  }
  return retVal;
}*/

/*std::string ConfigBase::getTagsAsJsonStrForGUI() {
  std::string retVal = "";
  if (_impl) {
    retVal = _impl->getTagsAsJsonStrForGUI();
  }
  return retVal;
}*/

/*void ConfigBase::updateTagsFromJsonString(std::string jsonStr) {
  if (_impl) {
    _impl->updateTagsFromJsonString(jsonStr);
  }
}*/

bool ConfigBase::isList(const std::string &key) {
  bool retVal = false;
  if (_impl) {
    retVal = _impl->isList(key);
  }
  return retVal;
}

/*void ConfigBase::setActive(const std::string &key, const std::string &value, const bool &reset) {
  if (_impl) {
    _impl->setActive(key, value, reset);
  }
}*/
/*void ConfigBase::resetActive(const std::string &key, const std::string &value) {
  if (_impl) {
    _impl->resetActive(key, value);
  }
}*/

} // namespace appsupport
} // namespace next
