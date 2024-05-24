/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_base_impl.hpp
 * @brief    Including functions for configuration handling
 *
 * Implementation of the functions for configuration handling.
 *
 */

#ifndef CONFIG_BASE_IMPL_H_
#define CONFIG_BASE_IMPL_H_

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include <next/sdk/sdk_macros.h>
#include <next/appsupport/config/config_base.hpp>

#include "config_change_hook_handler.hpp"
#include "config_database_manager.hpp"

namespace next {
namespace appsupport {

class ConfigBase_Impl {

  DISABLE_COPY_MOVE(ConfigBase_Impl)

public:
  ConfigBase_Impl(const std::string &name, bool filter_by_comp_name);
  virtual ~ConfigBase_Impl();

public:
  // TODO remove comments after cleanup of ConfigBase_Impl class
  // in CompData
  void put(const std::string &key, const int &value, const ConfigSource &source);
  void put(const std::string &key, const float &value, const ConfigSource &source);
  void put(const std::string &key, const std::string &value, const ConfigSource &source);
  void put(const std::string &key, const bool &value, const ConfigSource &source);
  int get_int(const std::string &key, const int &default_value, bool block_active, bool &value_available);
  float get_float(const std::string &key, const float &default_value, bool block_active, bool &value_available);
  std::string get_string(const std::string &key, const std::string &default_value, bool block_active,
                         bool &value_available);
  bool get_option(const std::string &key, const bool &default_value, bool block_active, bool &value_available);
  std::vector<std::string> getChildren(const std::string &key, bool full_path);
  std::vector<std::string> getStringList(const std::string &key, const std::vector<std::string> defaults,

                                         bool block_active, bool &value_available);
  // ENd

  // in ConfigManager
  // void checkGenericConfig(const std::string &json_str);
  // void checkGenericConfig(const boost::property_tree::ptree &source);
  // END

  // in IO
  void putCfg(const std::string &filePath, const ConfigSource &source);
  std::string getComponentConfigurationsAsJson(const std::string &component, bool filterOutPrivateKeys = false,
                                               bool formatted = false);
  void insert(const std::string &json_data, bool filterOutPrivateKeys = false, bool resetActiveTag = false,
              const ConfigSource &source = ConfigSource::Component);
  // end IO

  // todo get and set json data from and to ptree
  // conf Manager
  std::string getAllConfigurationsAsJson(bool filterOutPrivateKeys = false, bool filterOutGenericKeys = false,
                                         bool filterOutReadOnly = false, bool formatted = false);
  // conf Manager

  // void putPrivateKey(const std::string &key);
  std::vector<std::string> componentName() const;
  std::string getRootName() const;

  void clearTree();
  void finishInitialization();
  void enableFilterByComponent();
  void disableFilterByComponent();
  bool getCurrentFilterByComponentStatus();
  void setCurrentFilterByComponentStatus(bool filter_by_comp);
  void waitForResponse();
  void setWaitForResponseFlag(bool value);
  void setConfigValueFromServer(const std::string &value);
  std::string getConfigValueFromServer();
  // void definePrivateConfigKeys(std::vector<std::string>);
  std::string getDifferences(std::string inputJson);
  bool isComponentInCurrentTree(const std::string &componentName);
  bool addNewComponentToLocalTree(const std::string &componentName);
  /*void setTag(const std::string &key, next::appsupport::configtags::TAG tag);
  void setTag(const std::string &key, const std::string &tag);*/
  // std::vector<std::string> getTags(const std::string &key);
  /*void removeTag(const std::string &key, next::appsupport::configtags::TAG tag, bool removeFormQuickList = true);
  void removeTag(const std::string &key, const std::string &tag, bool removeFormQuickList = true);*/
  // void removeAllTagsForKey(const std::string &key);
  // void addTagToQuickList(const std::string &key, const std::string &tag);
  // void removeTagFromQuickList(const std::string &key, const std::string &tag);
  // void removeTagFromQuickList(const std::string &tag);
  // void removeAllTagsForKeyFromQuickList(const std::string &key);
  // std::string getTagsAsJsonStr();
  // std::string getTagsAsJsonStrForGUI();
  // void updateTagsFromJsonString(std::string jsonStr);
  // std::string getTreeFilteredByTagAsJson(std::string tag);
  // std::string getTreeFilteredByTagsIntersectionAsJson(std::vector<std::string> tags);
  // std::string getTreeFilteredByTagsUnionAsJson(std::vector<std::string> tags);
  bool isList(const std::string &key);
  // void setActive(const std::string &key, const std::string &value, const bool &reset = true);
  // void resetActive(const std::string &key, const std::string &value);

  bool isInitFinished() const;

  size_t addChangeHook(configtypes::change_hook hook);
  void removeChangeHook(size_t id);

private:
  // void removeTagFromConfiguration(const std::string &tag);
  // void updatePtreeTagsFromQuickMap();
  // void setTagForList(const std::string &key, const std::string &tag);
  std::string getComponentNameFromKey(const std::string &key);
  template <typename T>
  void put_template(const std::string &key, const T &value, const ConfigSource &source);

  // bool removeReadOnlyValuesFromComponents(boost::property_tree::ptree &pt);
  // size_t getNumberOfChildren(const boost::property_tree::ptree &pt);
  // void removeGenericValuesFromComponents(boost::property_tree::ptree &pt);
  /*void removePrivateValuesFromPtree(boost::property_tree::ptree &pt,
                                    const std::vector<std::string> &inputPrivateKeys = std::vector<std::string>());*/
  /*void removePrivateKeysFromChild(boost::property_tree::ptree &pt, std::string url,
                                  const std::vector<std::string> &privateKeys);*/
  // std::vector<std::string> getPrivateKeys(const boost::property_tree::ptree &pt);
  bool isValidKeyInTree(const std::string &key);
  // boost::optional<boost::property_tree::ptree *> getChildFromKey(const std::string &key);

  ConfigDatabaseManager *current_tree_;

  // contains the
  // - main ptree where we store all config
  // - temporaty ptree to store config from cmd parameters at init
  // - temporaty ptree to store config from cmd configuration at init
  // - temporaty ptree to store config from server at init
  //
  // Hint: Don't use [] oeprator for accessing data. Use only .at operator because default constructor of
  // ConfigDatabaseManager is deleted
  std::map<ConfigSource, ConfigDatabaseManager> map_config_database_configs_;

  ConfigChangeHookHandler config_change_hook_handler_;

  std::shared_ptr<std::string> _component_name;
  // bool filter_by_comp_name_ = false;
  bool initialization_done_ = false;
  mutable std::mutex wait_for_response_mutex_;
  std::condition_variable wait_for_response_cv_;
  std::atomic_bool wait_for_response_flag_{false};
  std::atomic_bool shutdown_flag_ = false; //! Will wake up sleeping threads on shutdown
  const int kWaitForMessageTimeout_{2000};
  std::string configuration_value_from_server_;
  mutable std::recursive_mutex configuration_tree_handling_mutex_{};
  // tag quick access
  // component [tag [url list]]
  // std::map<std::string, std::map<std::string, std::set<std::string>>> tag_map_;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_BASE_IMPL_H_
