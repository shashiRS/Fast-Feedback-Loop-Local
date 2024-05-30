/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_base_impl.cpp
 * @brief    funcctions for configuration handling
 *
 **/

#include "config_base_impl.hpp"

#include <exception>

#include <next/sdk/logger/logger.hpp>

#include <json/json.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "config_database_manager.hpp"
#include "config_utilities.hpp"

namespace pt = boost::property_tree;

namespace next {
namespace appsupport {

ConfigBase_Impl::ConfigBase_Impl(const std::string &name, bool filter_by_comp_name) {
  _component_name = std::make_shared<std::string>(name);

  // main ptree where we store all configs
  auto it_component_tree = map_config_database_configs_.try_emplace(ConfigSource::Component, name, filter_by_comp_name);
  // temporary ptree to store config from cmd arguments at init
  map_config_database_configs_.try_emplace(ConfigSource::CmdOptionsParameters, name, filter_by_comp_name);
  //// temporaty ptree to store config from cmd defined config at init
  map_config_database_configs_.try_emplace(ConfigSource::CmdOptionsConfig, name, filter_by_comp_name);
  ////  temporaty ptree to store config from server at init
  map_config_database_configs_.try_emplace(ConfigSource::ConfigServer, name, filter_by_comp_name);

  current_tree_ = &it_component_tree.first->second;
}

ConfigBase_Impl::~ConfigBase_Impl() {
  shutdown_flag_ = true;
  wait_for_response_cv_.notify_one();
}

void ConfigBase_Impl::insert(const std::string &json_str, bool /*filterOutPrivateKeys*/, bool /*resetActiveTag*/,
                             const ConfigSource &source) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  if (isInitFinished()) {
    current_tree_->insertJsonString(json_str);

    config_change_hook_handler_.callChangeHooks("full_cfg");
  } else {
    auto it = map_config_database_configs_.find(source);
    if (it != map_config_database_configs_.end()) {
      it->second.insertJsonString(json_str);
    }
  }
}

template <typename T>
void ConfigBase_Impl::put_template(const std::string &key, const T &value, const ConfigSource &source) {
  static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, std::string> ||
                    std::is_same_v<T, bool>,
                "put function only supports int,bool, float and std::string types");
  {
    std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
    if (isInitFinished()) {
      current_tree_->put(key, value);
    } else {
      auto it = map_config_database_configs_.find(source);
      if (it != map_config_database_configs_.end()) {
        it->second.put(key, value);
      }
    }
  }

  if (isInitFinished())
    config_change_hook_handler_.callChangeHooks(key);
}

void ConfigBase_Impl::put(const std::string &key, const int &value, const ConfigSource &source) {
  put_template(key, value, source);
}

void ConfigBase_Impl::put(const std::string &key, const float &value, const ConfigSource &source) {
  put_template(key, value, source);
}

void ConfigBase_Impl::put(const std::string &key, const std::string &value, const ConfigSource &source) {
  put_template(key, value, source);
}

void ConfigBase_Impl::put(const std::string &key, const bool &value, const ConfigSource &source) {
  put_template(key, value, source);
}
// void ConfigBase_Impl::putPrivateKey(const std::string &key) {
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  auto compNameInKey = getComponentNameFromKey(key);
//  auto localComponents = componentName();
//  if (std::find(localComponents.begin(), localComponents.end(), compNameInKey) == localComponents.end())
//    return;
//  std::string readonlyKey = compNameInKey + kConfigurationKeyDelimiter + kPrivateKeyName;
//  put(readonlyKey + "[" + std::to_string(getChildren(readonlyKey).size()) + "]", key);
//}

int ConfigBase_Impl::get_int(const std::string &key, const int &default_value, bool /*block_active*/,
                             bool &value_available) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getInt(key, default_value, value_available);
}

float ConfigBase_Impl::get_float(const std::string &key, const float &default_value, bool /*block_active*/,
                                 bool &value_available) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getFloat(key, default_value, value_available);
}

std::string ConfigBase_Impl::get_string(const std::string &key, const std::string &default_value, bool /*block_active*/,
                                        bool &value_available) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getString(key, default_value, value_available);
}

bool ConfigBase_Impl::get_option(const std::string &key, const bool &default_value, bool /*block_active*/,
                                 bool &value_available) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getBool(key, default_value, value_available);
}

void ConfigBase_Impl::putCfg(const std::string &filePath, const ConfigSource &source) {
  // Hint: putCfg shall not trigger _change_hook because putCfg must only be called during startup of the component
  //        (when_change_hook are not available). Later on it's not clear anymore which config values shall be used
  //        from the config file and which shall be used from the current configuration. CMD line argument parameter
  //        have for e.g. higher priority than the config file
  {
    std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
    if (isInitFinished()) {
      current_tree_->putCfg(filePath);
    } else {
      auto it = map_config_database_configs_.find(source);
      if (it != map_config_database_configs_.end()) {
        it->second.putCfg(filePath);
      }
    }
  }
}

std::vector<std::string> ConfigBase_Impl::getStringList(const std::string &key, const std::vector<std::string> defaults,
                                                        bool /*block_active*/, bool &value_available) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getValuesAsStringList(key, defaults, value_available);
}

std::vector<std::string> ConfigBase_Impl::getChildren(const std::string &key, bool full_path) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getChildrenKeys(key, full_path);
}

std::string ConfigBase_Impl::getAllConfigurationsAsJson(bool /*filterOutPrivateKeys*/, bool /*filterOutGenericKeys*/,
                                                        bool /*filterOutReadOnly*/, bool formatted) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getAllCompConfigsAsJsonString("", formatted);
}

std::string ConfigBase_Impl::getComponentConfigurationsAsJson(const std::string &component,
                                                              bool /*filterOutPrivateKeys*/, bool formatted) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getOneCompConfigAsJsonString(component, "", formatted);
}

void ConfigBase_Impl::finishInitialization() {
  // block all changes in the configuration during combining the different config sources
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  ConfigDatabaseManager &component_tree = map_config_database_configs_.at(ConfigSource::Component);
  ConfigDatabaseManager &tree_from_command_line_arguments =
      map_config_database_configs_.at(ConfigSource::CmdOptionsParameters);
  ConfigDatabaseManager &tree_from_command_line_config =
      map_config_database_configs_.at(ConfigSource::CmdOptionsConfig);
  ConfigDatabaseManager &tree_from_server = map_config_database_configs_.at(ConfigSource::ConfigServer);

  // combine all config sources in component tree (overwrite existing values)
  component_tree.insertConfig(tree_from_server);

  // TODO activate when tag system is implemented
  /*
  auto currentComponents = componentName();
  for (auto component : currentComponents) {
    // try to remove the private values from the input json
    boost::optional<pt::ptree &> componentTree = (*current_tree_).get_child_optional(component);
    // in the initialization phase all blacklisted values from external sources should be filtered
    // check if we have something in the config ptree for the current component
    if (componentTree.is_initialized()) {
      std::vector<std::string> currentPrivate = getPrivateKeys((*current_tree_).get_child(component));

      if (currentPrivate.size()) {
        removePrivateValuesFromPtree(_tree_from_command_line_config, currentPrivate);
        removePrivateValuesFromPtree(_tree_from_command_line_arguments, currentPrivate);
      }
    }
  }
  */
  component_tree.insertConfig(tree_from_command_line_config);
  component_tree.insertConfig(tree_from_command_line_arguments);

  // point current tree to component tree
  current_tree_ = &component_tree;

  tree_from_command_line_arguments.clear();
  tree_from_command_line_config.clear();
  tree_from_server.clear();
  initialization_done_ = true;
}

void ConfigBase_Impl::enableFilterByComponent() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  for (auto &it_config_database_manager : map_config_database_configs_) {
    it_config_database_manager.second.enableFilterByComponent();
  }
}

void ConfigBase_Impl::disableFilterByComponent() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  for (auto &it_config_database_manager : map_config_database_configs_) {
    it_config_database_manager.second.disableFilterByComponent();
  }
}

bool ConfigBase_Impl::getCurrentFilterByComponentStatus() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getFilterByComponent();
}

void ConfigBase_Impl::setCurrentFilterByComponentStatus(bool filter_by_comp) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  current_tree_->setFilterByComponent(filter_by_comp);
}

void ConfigBase_Impl::waitForResponse() {
  std::unique_lock<std::mutex> lock(wait_for_response_mutex_);
  wait_for_response_cv_.wait_for(lock, std::chrono::milliseconds(kWaitForMessageTimeout_), [&] {
    bool retVal = false;
    if (shutdown_flag_)
      retVal = true;
    if (wait_for_response_flag_ == true)
      retVal = true;
    return retVal;
  });
  wait_for_response_flag_ = false;
  lock.unlock();
}

void ConfigBase_Impl::setWaitForResponseFlag(bool value) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  wait_for_response_flag_ = value;
  wait_for_response_cv_.notify_one();
}

void ConfigBase_Impl::setConfigValueFromServer(const std::string &value) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  configuration_value_from_server_ = value;
}

std::string ConfigBase_Impl::getConfigValueFromServer() {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return configuration_value_from_server_;
}

// void ConfigBase_Impl::definePrivateConfigKeys(std::vector<std::string> privateKeys) {
//  if (privateKeys.size() == 0)
//    return;
//
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  std::string readonlyKey = getComponentNameFromKey(privateKeys[0]) + kConfigurationKeyDelimiter + kPrivateKeyName;
//  size_t initialIdx = getChildren(readonlyKey).size();
//  for (size_t idx = 0; idx < privateKeys.size(); idx++) {
//    put(readonlyKey + "[" + std::to_string(initialIdx + idx) + "]", privateKeys[idx]);
//  }
//}
// void ConfigBase_Impl::removePrivateKeysFromChild(boost::property_tree::ptree &pt, std::string url,
//                                                 const std::vector<std::string> &privateKeys) {
//  std::string childUrl = "";
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  boost::property_tree::ptree::iterator it = pt.begin();
//  for (; it != pt.end();) {
//    childUrl = url + it->first;
//    if (std::find(privateKeys.begin(), privateKeys.end(), childUrl) != privateKeys.end()) {
//      it = pt.erase(it);
//    } else {
//      removePrivateKeysFromChild(it->second, childUrl + kConfigurationKeyDelimiter, privateKeys);
//      ++it;
//    }
//  }
//}

// void ConfigBase_Impl::removeGenericValuesFromComponents(boost::property_tree::ptree &pt) {
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  boost::optional<pt::ptree &> genericComponentTree = pt.get_child_optional(kGenericKeyName);
//  if (genericComponentTree.is_initialized()) {
//    boost::property_tree::ptree temporaryReturn;
//    boost::property_tree::ptree temporaryTree;
//    boost::property_tree::ptree::iterator it = pt.begin();
//    // check first level children - this are the components
//    for (; it != pt.end(); it++) {
//      // skip GENERIC component
//      if (it->first.compare(kGenericKeyName) == 0)
//        continue;
//      boost::property_tree::ptree temp = comparePTree(it->second, genericComponentTree.get());
//      if (temp.size()) {
//        pt.put_child(it->first, temp);
//      }
//    }
//  }
//}

// void ConfigBase_Impl::removePrivateValuesFromPtree(boost::property_tree::ptree &pt,
//                                                   const std::vector<std::string> &inputPrivateKeys) {
//  std::string childUrl = "";
//  std::vector<std::string> privateKeys;
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  boost::property_tree::ptree::iterator it = pt.begin();
//  // check children recursivly
//  for (; it != pt.end();) {
//    if (inputPrivateKeys.size())
//      privateKeys = inputPrivateKeys;
//    else
//      privateKeys = getPrivateKeys(it->second);
//    if (privateKeys.size()) {
//      childUrl = it->first;
//      if (std::find(privateKeys.begin(), privateKeys.end(), childUrl) != privateKeys.end()) {
//        it = pt.erase(it);
//        continue;
//      } else {
//        removePrivateKeysFromChild(it->second, childUrl + kConfigurationKeyDelimiter, privateKeys);
//      }
//      // remove the blacklist entry
//      pt.get_child(it->first).erase(kPrivateKeyName);
//    }
//    it++;
//  }
//}

// std::vector<std::string> ConfigBase_Impl::getPrivateKeys(const boost::property_tree::ptree &pt) {
//  std::vector<std::string> retVal{};
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  boost::optional<const pt::ptree &> privateChild = pt.get_child_optional(kPrivateKeyName);
//  if (privateChild.is_initialized()) {
//    const auto children = getSubtreeAsArray(kPrivateKeyName, pt);
//    for (auto child : children) {
//      if (child.isValue) {
//        retVal.push_back(child.value);
//      }
//    }
//  }
//  return retVal;
//}

// void ConfigBase_Impl::checkGenericConfig(const boost::property_tree::ptree &source) {
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  boost::optional<const pt::ptree &> genericComponentTree = source.get_child_optional(kGenericKeyName);
//  if (genericComponentTree.is_initialized()) {
//    boost::property_tree::ptree temporaryTree;
//    // here we should update the generic in all components
//    auto components = componentName();
//    for (auto componentNameStr : components) {
//      temporaryTree.add_child(componentNameStr, genericComponentTree.get());
//      if (initialization_done_) {
//        // we only do this here if initialization is done already as for the init phase privet key filtering is done
//        // after
//        std::vector<std::string> currentPrivate = getPrivateKeys((*current_tree_).get_child(componentNameStr));
//        if (currentPrivate.size()) {
//          removePrivateValuesFromPtree(temporaryTree, currentPrivate);
//        }
//      }
//    }
//    auto json = getTreeAsJson(temporaryTree);
//    mergePTree(temporaryTree, *current_tree_, filter_by_comp_name);
//    json = getTreeAsJson(*current_tree_);
//  } else {
//    return;
//  }
//}
// void ConfigBase_Impl::checkGenericConfig(const std::string &json_str) {
//  if (!json_str.empty()) {
//    std::stringstream ss;
//    ss << json_str;
//    pt::ptree tempPtree;
//    try {
//      boost::property_tree::read_json(ss, tempPtree);
//      // try to get the generic component and rename it to our current component name
//      checkGenericConfig(tempPtree);
//    } catch (const std::exception &e) {
//      next::sdk::logger::warn(__FILE__, "Error parsing json string {0} : {1}", json_str, e.what());
//    }
//  }
//}

std::string ConfigBase_Impl::getDifferences(std::string inputJson) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->getDifferences(inputJson);
}

// void ConfigBase_Impl::setTag([[maybe_unused]] const std::string &key,
//                             [[maybe_unused]] next::appsupport::configtags::TAG tag) {
//  /*if (next::appsupport::configtags::kTagStr.find(tag) == next::appsupport::configtags::kTagStr.end()) {
//    next::sdk::logger::warn(__FILE__, "Tag {0} is not valid. Ignored.", (int)tag);
//  }
//  setTag(key, next::appsupport::configtags::kTagStr.at(tag));*/
//}

// void ConfigBase_Impl::setTag([[maybe_unused]] const std::string &key, [[maybe_unused]] const std::string &tag) {
//  /*if (!isValidKeyInTree(key)) {
//    next::sdk::logger::warn(__FILE__, "Could not add Tag {0} for missing key {1}", tag, key);
//    return;
//  }
//  boost::optional<pt::ptree &> treeElement =
//      current_tree_->get_child_optional(pt::ptree::path_type{key, kConfigurationKeyDelimiter});
//  if (treeElement.is_initialized() || hasArrayChild(key)) {
//    std::string tagKey = key + kConfigurationKeyDelimiter + kTagKeyName;
//    auto tags = getStringList(tagKey, {}, true);
//    if (std::find(tags.begin(), tags.end(), tag) != tags.end()) {
//      next::sdk::logger::debug(__FILE__, "Tag {0} for key {1} already exists", tag, key);
//      return;
//    }
//    size_t initialIdx = tags.size();
//    bool temp = config_change_hook_handler_.getChangeHooksActivationStatus;
//    config_change_hook_handler_.setChangeHooksActivationStatus(false);
//    put(tagKey + "[" + std::to_string(initialIdx) + "]", tag);
//    config_change_hook_handler_.setChangeHooksActivationStatus(temp);
//    // add to tag list
//    addTagToQuickList(key, tag);
//  } else {
//    next::sdk::logger::warn(__FILE__, "Could not add Tag {0} for missing key {1}", tag, key);
//  }*/
//}

// void ConfigBase_Impl::addTagToQuickList(const std::string &key, const std::string &tag) {
//  auto compName = getComponentNameFromKey(key);
//  tag_map_[compName][tag].insert(key);
//}

// void ConfigBase_Impl::removeAllTagsForKeyFromQuickList(const std::string &key) {
//  auto compName = getComponentNameFromKey(key);
//  std::map<std::string, std::set<std::string>>::iterator it = tag_map_[compName].begin();
//  for (; it != tag_map_[compName].end();) {
//    std::set<std::string>::iterator it_url = it->second.find(key);
//    if (it_url != it->second.end())
//      it->second.erase(it_url);
//    if (it->second.size() == 0) {
//      it = tag_map_[compName].erase(it);
//    } else
//      ++it;
//  }
//}

/*void ConfigBase_Impl::removeTagFromQuickList(const std::string &key, const std::string &tag) {
  auto compName = getComponentNameFromKey(key);
  if (tag_map_.find(compName) != tag_map_.end()) {
    if (tag_map_[compName].find(tag) != tag_map_[compName].end()) {
      if (tag_map_[compName][tag].find(key) != tag_map_[compName][tag].end()) {
        tag_map_[compName][tag].erase(tag_map_[compName][tag].find(key));
        if (tag_map_[compName][tag].size() == 0)
          tag_map_[compName].erase(tag);
      }
    }
  }
}*/

/*void ConfigBase_Impl::updateTagsFromJsonString(std::string jsonStr) {
  if (jsonStr.size() == 0)
    return;
  Json::Value jsonTagMap;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(jsonStr.c_str(), jsonTagMap);
  if (!parsingSuccessful) {
    next::sdk::logger::warn(__FILE__, "there was an error parsing json string {0}", jsonStr);
    return;
  }
  for (Json::Value::iterator itr = jsonTagMap.begin(); itr != jsonTagMap.end(); itr++) {
    auto componentName = itr.name();
    Json::Value component = jsonTagMap[componentName];
    for (Json::Value::iterator itrTag = component.begin(); itrTag != component.end(); itrTag++) {
      auto tagName = itrTag.name();
      std::set<std::string> urlList;
      for (Json::Value::iterator itrURL = (*itrTag).begin(); itrURL != (*itrTag).end(); itrURL++) {
        urlList.insert(itrURL->asString());
      }
      if (urlList.size())
        tag_map_[componentName][tagName] = urlList;
    }
  }
  updatePtreeTagsFromQuickMap();
}*/

/*std::string ConfigBase_Impl::getTagsAsJsonStrForGUI() {
  std::map<std::string, std::map<std::string, std::set<std::string>>>::const_iterator componentIt = tag_map_.begin();
  std::map<std::string, std::vector<std::string>> guiMApValues;
  while (componentIt != tag_map_.end()) {
    std::map<std::string, std::set<std::string>>::const_iterator tagIt = componentIt->second.begin();

    while (tagIt != componentIt->second.end()) {
      std::set<std::string>::const_iterator urlIt = tagIt->second.begin();
      Json::Value jsonUrlList;
      while (urlIt != tagIt->second.end()) {
        guiMApValues[*urlIt].push_back(tagIt->first);
        urlIt++;
      }
      tagIt++;
    }
    componentIt++;
  }
  std::map<std::string, std::vector<std::string>>::const_iterator itGuiMap = guiMApValues.begin();

  Json::Value jsonurl;
  Json::Value jsonTag;
  while (itGuiMap != guiMApValues.end()) {

    Json::Value jsonUrlList;
    for (auto tag : itGuiMap->second) {
      jsonUrlList.append(tag);
    }
    jsonTag[itGuiMap->first] = jsonUrlList;
    itGuiMap++;
  }
  Json::StyledWriter writer;
  std::string output = writer.write(jsonTag);
  return output;
}*/

/*std::string ConfigBase_Impl::getTagsAsJsonStr() {
  std::map<std::string, std::map<std::string, std::set<std::string>>>::const_iterator componentIt = tag_map_.begin();
  Json::Value jsonComponentMap;
  while (componentIt != tag_map_.end()) {
    std::map<std::string, std::set<std::string>>::const_iterator tagIt = componentIt->second.begin();
    Json::Value jsonTag;
    while (tagIt != componentIt->second.end()) {
      std::set<std::string>::const_iterator urlIt = tagIt->second.begin();
      Json::Value jsonUrlList;
      while (urlIt != tagIt->second.end()) {
        jsonUrlList.append(*urlIt);
        urlIt++;
      }
      jsonTag[tagIt->first] = jsonUrlList;
      tagIt++;
    }
    jsonComponentMap[componentIt->first] = jsonTag;

    componentIt++;
  }
  Json::StyledWriter writer;
  std::string output = writer.write(jsonComponentMap);
  return output;
}*/

/*std::vector<std::string> ConfigBase_Impl::getTags(const std::string &key) {
  std::vector<std::string> retval;
  boost::optional<pt::ptree &> treeElement =
      current_tree_->get_child_optional(pt::ptree::path_type{key, kConfigurationKeyDelimiter});
  if (treeElement.is_initialized() || hasArrayChild(key)) {
    std::string tagKey = key + kConfigurationKeyDelimiter + kTagKeyName;
    const auto children = getStringList(tagKey, {}, true);
    for (auto child : children) {
      retval.push_back(child);
    }
  } else {
    next::sdk::logger::warn(__FILE__, "Could not get Tags for missing key {0}", key);
  }
  return retval;
}*/

// void ConfigBase_Impl::removeTag(const std::string &key, next::appsupport::configtags::TAG tag,
//                                bool removeFormQuickList) {
//  if (next::appsupport::configtags::kTagStr.find(tag) == next::appsupport::configtags::kTagStr.end()) {
//    next::sdk::logger::warn(__FILE__, "Tag {0} is not valid. Ignored.", (int)tag);
//  } else {
//    removeTag(key, next::appsupport::configtags::kTagStr.at(tag), removeFormQuickList);
//  }
//}
// void ConfigBase_Impl::removeTag(const std::string &key, const std::string &tag, bool removeFormQuickList) {
//  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
//  if (!isValidKeyInTree(key + kConfigurationKeyDelimiter + kTagKeyName)) {
//    next::sdk::logger::warn(__FILE__, "Could not remove Tag {0} for key {1}", tag, key);
//    removeTagFromQuickList(key, tag);
//    return;
//  }
//  typedef typename boost::property_tree::ptree::key_type::value_type Ch;
//  typedef typename std::basic_string<Ch> Str;
//  boost::optional<pt::ptree *> treeElement;
//  treeElement.reset();
//  if (hasArrayChild(key)) {
//    treeElement = getChildFromKey(key + kConfigurationKeyDelimiter + kTagKeyName);
//  }
//  if (!treeElement.is_initialized()) {
//    treeElement = &current_tree_->get_child(
//        pt::ptree::path_type{key + kConfigurationKeyDelimiter + kTagKeyName, kConfigurationKeyDelimiter});
//  }
//
//  auto strVal = getTags(key);
//  if (treeElement.is_initialized()) {
//    auto it = (treeElement.get())->begin();
//    for (; it != (treeElement.get())->end();) {
//      if (it->first.empty() && it->second.template get_value<Str>().compare(tag) == 0) {
//        it = (treeElement.get())->erase(it);
//        if (removeFormQuickList) {
//          removeTagFromQuickList(key, tag);
//        }
//      } else {
//        ++it;
//      }
//    }
//    // remove the tag node if empty
//    if ((treeElement.get())->size() == 0) {
//      boost::optional<pt::ptree *> currentNode = getChildFromKey(key);
//      if (currentNode.is_initialized()) {
//        auto it_tag = (currentNode.get())->begin();
//        for (; it_tag != (currentNode.get())->end(); it_tag++) {
//          if (it_tag->first.compare(kTagKeyName) == 0) {
//            (currentNode.get())->erase(it_tag);
//            break;
//          }
//        }
//      }
//    }
//
//  } else {
//    next::sdk::logger::warn(__FILE__, "Could not remove Tag {0} for key {1}", tag, key);
//  }
//}
// void ConfigBase_Impl::removeAllTagsForKey(const std::string &key) {
//  if (!isValidKeyInTree(key)) {
//    next::sdk::logger::warn(__FILE__, "Could not remove Tags for key {0}", key);
//    return;
//  }
//  boost::optional<pt::ptree *> treeElement;
//  treeElement.reset();
//  if (hasArrayChild(key)) {
//    treeElement = getChildFromKey(key);
//  }
//  if (!treeElement.is_initialized())
//    treeElement = &current_tree_->get_child(pt::ptree::path_type{key, kConfigurationKeyDelimiter});
//
//  if (treeElement.is_initialized()) {
//    auto it = (treeElement.get())->begin();
//    for (; it != (treeElement.get())->end();) {
//
//      if (it->first.compare(kTagKeyName) == 0) {
//        it = (treeElement.get())->erase(it);
//      } else {
//        ++it;
//      }
//    }
//    removeAllTagsForKeyFromQuickList(key);
//  } else {
//    next::sdk::logger::warn(__FILE__, "Could not remove tags for missing key {0}", key);
//  }
//}

std::string ConfigBase_Impl::getComponentNameFromKey(const std::string &key) {
  return ConfigUtilities::getComponentNameFromKey(key);
}

bool ConfigBase_Impl::isComponentInCurrentTree(const std::string &componentName) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->isComponentInConfig(componentName);
}

bool ConfigBase_Impl::addNewComponentToLocalTree(const std::string &componentName) {
  bool retVal = true;
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};

  // update for all config components
  for (auto &it_config_database_manager : map_config_database_configs_) {
    if (it_config_database_manager.second.isComponentInConfig(componentName)) {
      retVal = false;
      next::sdk::logger::warn(__FILE__, "Component {0} already exists in config database!", componentName);
    } else {
      it_config_database_manager.second.addNewComponentConfig(componentName);
    }
  }
  return retVal;
}

std::vector<std::string> ConfigBase_Impl::componentName() const {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  // addNewComponentToLocalTree make sure that all ConfigSource have the same component names
  return current_tree_->getConfigCompNames();
}

std::string ConfigBase_Impl::getRootName() const {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  // addNewComponentToLocalTree make sure that all ConfigSource have the same component names
  return current_tree_->getRootName();
}

// std::string ConfigBase_Impl::getTreeFilteredByTagAsJson(std::string tag) {
//
//  boost::property_tree::ptree tree;
//  auto componentNameList = componentName();
//  for (auto currentComponent : componentNameList) {
//    if (tag_map_.find(currentComponent) == tag_map_.end())
//      continue;
//    if (tag_map_[currentComponent].find(tag) == tag_map_[currentComponent].end())
//      continue;
//    std::set<std::string> tagURLs = tag_map_[currentComponent].at(tag);
//    for (auto key : tagURLs) {
//      const auto vec = construct_vector(key);
//      ::next::appsupport::put(tree, key, get_string(key, "", true), true);
//    }
//  }
//  auto jsonVal = getTreeAsJson(tree);
//  return jsonVal;
//}

// std::string ConfigBase_Impl::getTreeFilteredByTagsIntersectionAsJson(std::vector<std::string> tags) {
//  std::string retVal = "{}";
//  if (tags.size() >= 2) {
//    boost::property_tree::ptree tree;
//    std::set<std::string> result;
//    auto componentNameList = componentName();
//    for (auto currentComponent : componentNameList) {
//      std::vector<std::string> tagsForCurrentComponent;
//      for (auto tag : tags) {
//        if (tag_map_[currentComponent].find(tag) != tag_map_[currentComponent].end()) {
//          tagsForCurrentComponent.push_back(tag);
//        }
//      }
//      // we need all tags available for intersection
//      if (tagsForCurrentComponent.size() != tags.size())
//        continue;
//      std::set<std::string> firstOperand = tag_map_[currentComponent].at(tagsForCurrentComponent[0]);
//      std::set<std::string> secondOperand = tag_map_[currentComponent].at(tagsForCurrentComponent[1]);
//      std::set_intersection(firstOperand.begin(), firstOperand.end(), secondOperand.begin(), secondOperand.end(),
//                            std::inserter(result, result.begin()));
//      for (size_t pos = 2; pos < tagsForCurrentComponent.size(); pos++) {
//        firstOperand = result;
//        result.clear();
//        secondOperand = tag_map_[currentComponent].at(tagsForCurrentComponent[pos]);
//        std::set_intersection(firstOperand.begin(), firstOperand.end(), secondOperand.begin(), secondOperand.end(),
//                              std::inserter(result, result.begin()));
//      }
//      for (auto key : result) {
//        const auto vec = construct_vector(key);
//        ::next::appsupport::put(tree, key, get_string(key, "", true), true);
//      }
//    }
//    retVal = getTreeAsJson(tree);
//  } else {
//    if (tags.size())
//      retVal = getTreeFilteredByTagAsJson(tags[0]);
//  }
//  return retVal;
//}

// std::string ConfigBase_Impl::getTreeFilteredByTagsUnionAsJson(std::vector<std::string> tags) {
//  std::string retVal = "{}";
//  // filter valid tags
//  if (tags.size() >= 2) {
//
//    boost::property_tree::ptree tree;
//    std::set<std::string> result;
//
//    auto componentNameList = componentName();
//    for (auto currentComponent : componentNameList) {
//      std::vector<std::string> tagsForCurrentComponent;
//      for (auto tag : tags) {
//        if (tag_map_[currentComponent].find(tag) != tag_map_[currentComponent].end()) {
//          tagsForCurrentComponent.push_back(tag);
//        }
//      }
//      if (tagsForCurrentComponent.size() >= 2) {
//        std::set<std::string> firstOperand = tag_map_[currentComponent].at(tagsForCurrentComponent[0]);
//        std::set<std::string> secondOperand = tag_map_[currentComponent].at(tagsForCurrentComponent[1]);
//        std::set_union(firstOperand.begin(), firstOperand.end(), secondOperand.begin(), secondOperand.end(),
//                       std::inserter(result, result.begin()));
//        for (size_t pos = 2; pos < tagsForCurrentComponent.size(); pos++) {
//          firstOperand = result;
//          result.clear();
//          secondOperand = tag_map_[currentComponent].at(tagsForCurrentComponent[pos]);
//          std::set_union(firstOperand.begin(), firstOperand.end(), secondOperand.begin(), secondOperand.end(),
//                         std::inserter(result, result.begin()));
//        }
//        for (auto key : result) {
//          const auto vec = construct_vector(key);
//          ::next::appsupport::put(tree, key, get_string(key, "", true), true);
//        }
//      } else if (tagsForCurrentComponent.size() == 1) {
//
//        std::set<std::string> tagURLs = tag_map_[currentComponent].at(tagsForCurrentComponent[0]);
//        for (auto key : tagURLs) {
//          const auto vec = construct_vector(key);
//          ::next::appsupport::put(tree, key, get_string(key, "", true), true);
//        }
//      }
//    }
//    retVal = getTreeAsJson(tree);
//  } else {
//    if (tags.size())
//      retVal = getTreeFilteredByTagAsJson(tags[0]);
//  }
//  return retVal;
//}

// boost::optional<boost::property_tree::ptree *> ConfigBase_Impl::getChildFromKey(const std::string &key) {
//  boost::optional<boost::property_tree::ptree *> retVal;
//  retVal.reset();
//  const auto vec = construct_vector(key);
//  pt::ptree *tmp = current_tree_;
//  bool found = true;
//  for (size_t p = 0; vec.size() > p; ++p) {
//    auto &ve = vec[p]; // easier access
//    if (!ve.name.empty()) {
//      auto tmp2 = tmp->get_child_optional(pt::ptree::path_type{ve.name, kConfigurationKeyDelimiter});
//      if (tmp2.is_initialized()) {
//        tmp = &tmp->get_child(pt::ptree::path_type{ve.name, kConfigurationKeyDelimiter});
//      } else {
//        found = false;
//        break;
//      }
//    }
//    if (ve.is_array) {
//      size_t pos = ve.array_index;
//      for (auto &e : (*tmp)) {
//        if (pos == 0u) {
//          tmp = &e.second;
//          break;
//        }
//        pos--;
//      }
//    }
//  }
//  if (found) {
//    retVal = tmp;
//  }
//  return retVal;
//}

bool ConfigBase_Impl::isValidKeyInTree(const std::string &key) {
  std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
  return current_tree_->configValueExists(key);
}

bool ConfigBase_Impl::isList(const std::string &key) {
  size_t array_size{0};
  bool final_leaf{false};
  {
    std::lock_guard<std::recursive_mutex> guard{configuration_tree_handling_mutex_};
    current_tree_->keyInfo(key, final_leaf, array_size);
  }
  if (array_size > 1) {
    return true;
  }
  return false;
}

void ConfigBase_Impl::clearTree() {
  for (auto &it_config_database_manager : map_config_database_configs_) {
    it_config_database_manager.second.clear();
  }

  // point current_tree to component tree
  current_tree_ = &map_config_database_configs_.at(ConfigSource::Component);
}

bool ConfigBase_Impl::isInitFinished() const { return initialization_done_; }

size_t ConfigBase_Impl::addChangeHook(configtypes::change_hook hook) {
  return config_change_hook_handler_.addChangeHook(hook);
}

void ConfigBase_Impl::removeChangeHook(size_t id) { config_change_hook_handler_.removeChangeHook(id); }

// void ConfigBase_Impl::setTagForList([[maybe_unused]] const std::string &key, [[maybe_unused]] const std::string &tag)
// {
/*if (tag.size() == 0)
  return;
if (!isList(key))
  return;
auto listChild = getChildFromKey(key);
if (listChild.is_initialized()) {
  size_t numChildren = listChild.get()->size();
  for (size_t i = 0; i < numChildren; i++) {
    std::string keyChild = key + "[" + std::to_string(i) + "]";
    setTag(keyChild, tag);
  }
      */
//}
//}

// void ConfigBase_Impl::removeTagFromConfiguration(const std::string &tag) {
//  std::map<std::string, std::map<std::string, std::set<std::string>>>::iterator itComponent = tag_map_.begin();
//  while (itComponent != tag_map_.end()) {
//    std::map<std::string, std::set<std::string>>::iterator itTag = itComponent->second.begin();
//    while (itTag != itComponent->second.end()) {
//      if (itTag->first.compare(tag) == 0) {
//        for (auto url : itTag->second) {
//          removeTag(url, tag, false);
//        }
//      }
//      itTag++;
//    }
//    itComponent++;
//  }
//  removeTagFromQuickList(tag);
//}
// void ConfigBase_Impl::updatePtreeTagsFromQuickMap() {

/*std::map<std::string, std::map<std::string, std::set<std::string>>>::iterator itComponent = tag_map_.begin();
while (itComponent != tag_map_.end()) {
  std::map<std::string, std::set<std::string>>::iterator itTag = itComponent->second.begin();
  while (itTag != itComponent->second.end()) {

    for (auto url : itTag->second) {
      setTag(url, itTag->first);
    }

    itTag++;
  }
  itComponent++;
}*/
//}

// void ConfigBase_Impl::resetActive(const std::string &key, const std::string &value) {
//  if (isList(key)) {
//    auto node = getChildFromKey(key);
//    if (node.is_initialized()) {
//      size_t numChilds = (node.get())->size();
//      for (size_t i = 0; i < numChilds; i++) {
//        std::string url = key + "[" + std::to_string(i) + "]";
//        auto val = get_string(url, "", true);
//        if (value.compare(val) == 0) {
//          removeTag(url, next::appsupport::configtags::TAG::ACTIVE);
//        }
//      }
//    }
//  }
//}

// void ConfigBase_Impl::setActive([[maybe_unused]] const std::string &key, [[maybe_unused]] const std::string &value,
//                                [[maybe_unused]] const bool &reset) {
/*if (isList(key)) {
  auto node = getChildFromKey(key);
  if (node.is_initialized()) {
    size_t numChilds = (node.get())->size();
    if (reset) {
      for (size_t i = 0; i < numChilds; i++) {
        std::string url = key + "[" + std::to_string(i) + "]";
        auto val = get_string(url, "", true);
        if (value.compare(val) == 0) {
          setTag(url, next::appsupport::configtags::TAG::ACTIVE);
        } else {
          removeTag(url, next::appsupport::configtags::TAG::ACTIVE);
        }
      }
    }
  }
}*/
//}

// size_t ConfigBase_Impl::getNumberOfChildren(const boost::property_tree::ptree &pt) {
//  size_t retVal = 0;
//  auto it = pt.begin();
//  for (; it != pt.end(); it++) {
//    if (it->first.compare(kTagKeyName) == 0)
//      continue;
//    retVal++;
//  }
//  return retVal;
//}

// bool ConfigBase_Impl::removeReadOnlyValuesFromComponents(boost::property_tree::ptree &pt) {
//
//  typename boost::property_tree::ptree::iterator it = pt.begin();
//  auto childCnt = getNumberOfChildren(pt);
//  size_t deletedCnt = 0;
//  if (childCnt) {
//    for (; it != pt.end();) {
//      if (removeReadOnlyValuesFromComponents(it->second)) {
//        it = pt.erase(it);
//        deletedCnt++;
//      } else
//        it++;
//    }
//    if (deletedCnt == childCnt)
//      return true;
//  }
//  const auto children = getSubtreeAsArray(kTagKeyName, pt);
//  for (auto child : children) {
//    if (child.value.compare(next::appsupport::configtags::kTagStr.at(next::appsupport::configtags::TAG::READONLY)) ==
//        0) {
//      pt.erase(pt.begin(), pt.end());
//      return true;
//    }
//  }
//  return false;
//}

/*void ConfigBase_Impl::removeTagFromQuickList(const std::string &tag) {
  std::map<std::string, std::map<std::string, std::set<std::string>>>::iterator itComponent = tag_map_.begin();
  while (itComponent != tag_map_.end()) {
    std::map<std::string, std::set<std::string>>::iterator itTag = itComponent->second.begin();
    while (itTag != itComponent->second.end()) {
      if (itTag->first.compare(tag) == 0) {
        itTag = itComponent->second.erase(itTag);
        continue;
      }
      itTag++;
    }
    itComponent++;
  }
}*/
} // namespace appsupport
} // namespace next
