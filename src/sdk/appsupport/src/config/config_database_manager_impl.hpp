/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_manager_impl.hpp
 * @brief    Manages the configurations of different components
 */

#ifndef CONFIG_DATABASE_MANAGER_IMPL_H_
#define CONFIG_DATABASE_MANAGER_IMPL_H_

#include <map>
#include <mutex>

#include "config_comp_data.hpp"
#include "config_database_IO.hpp"

namespace next {
namespace appsupport {

class ConfigDatabaseManager_Impl {

public:
  ConfigDatabaseManager_Impl(const std::string &root_name, bool filter_by_comp_name);
  virtual ~ConfigDatabaseManager_Impl();

public:
  //**** FUNC handling ConfigCompData ****/
  void put(const std::string &key, const int &value);
  void put(const std::string &key, const float &value);
  void put(const std::string &key, const std::string &value);
  void put(const std::string &key, const char *value);
  void put(const std::string &key, const bool &value);

  int getInt(const std::string &key, const int &default_value, bool &value_exist);
  float getFloat(const std::string &key, const float &default_value, bool &value_exist);
  std::string getString(const std::string &key, const std::string &default_value, bool &value_exist);
  bool getBool(const std::string &key, const bool &default_value, bool &value_exist);
  std::vector<std::string> getChildrenKeys(const std::string &key, bool full_path, bool &comp_available);
  std::vector<std::string> getValuesAsStringList(const std::string &key, const std::vector<std::string> &default_values,
                                                 bool &value_available);
  std::map<std::string, std::string> getCompData(const std::string &comp_name, bool &comp_available);
  bool isCompEmpty(const std::string &comp_name, bool &comp_available);
  bool configValueExists(const std::string &key, bool &comp_available);
  bool keyInfo(const std::string &key, bool &final_leaf, size_t &array_size, bool &comp_available);
  void clear();
  ///////////////////////////////////////

  //**** FUNC handling ConfigCompDataIO ****/
  void putCfg(const std::string &filePath);
  void insertJsonString(const std::string &json_data);
  void insertConfig(ConfigDatabaseManager_Impl *config_data_base_source);
  std::string getOneCompConfigAsJsonString(const std::string &comp_name, const std::string &key, bool &comp_available,
                                           bool formatted = false);
  std::string getAllCompConfigsAsJsonString(std::string key = "", bool formatted = false);
  std::string getDifferences(std::string json_data);
  ///////////////////////////////////////

  std::string getRootName() const;
  std::vector<std::string> getConfigCompNames() const;

  void enableFilterByComponent();
  void disableFilterByComponent();
  bool getFilterByComponent();
  void setFilterByComponent(bool filter_by_comp);

  bool isComponentInConfig(const std::string &comp_name);

  bool addNewComponentConfig(const std::string &comp_name);

private:
  template <typename T>
  void putTemplate(const std::string &key, const T &value);

  mutable std::recursive_mutex configuration_tree_handling_mutex_;
  std::string root_name_{""};
  bool filter_by_comp_name_{false};
  std::map<std::string, ConfigCompData> map_config_comp_data_;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_DATABASE_MANAGER_IMPL_H_
