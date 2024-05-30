/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_comp_data_impl.hpp
 * @brief    Handles the config database
 *
 * Implementation of the functions for handling of the config database for one component
 *
 */

#ifndef CONFIG_COMP_DATA_IMPL_H_
#define CONFIG_COMP_DATA_IMPL_H_

#include <mutex>
#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <next/sdk/sdk_macros.h>

#include "config_comp_data_storage.hpp"
#include "config_utilities.hpp"

namespace next {
namespace appsupport {

using treeStorageType = boost::property_tree::ptree;

class ConfigCompData_Impl {
  // implement appropriate copy and move constructor if needed, e.g. for inserting object into map
  DISABLE_COPY_MOVE(ConfigCompData_Impl)

public:
  ConfigCompData_Impl(const std::string &comp_name);
  virtual ~ConfigCompData_Impl();

public:
  void put(const std::string &key, const int &value);
  void put(const std::string &key, const float &value);
  void put(const std::string &key, const std::string &value);
  void put(const std::string &key, const char *value);
  void put(const std::string &key, const bool &value);

  int getInt(const std::string &key, const int &default_value, bool &value_exist) const;
  float getFloat(const std::string &key, const float &default_value, bool &value_exist) const;
  std::string getString(const std::string &key, const std::string &default_value, bool &value_exist) const;
  bool getBool(const std::string &key, const bool &default_value, bool &value_exist) const;
  std::vector<std::string> getChildrenKeys(const std::string &key, bool full_path) const;
  std::vector<std::string> getValuesAsStringList(const std::string &key, const std::vector<std::string> &default_values,
                                                 bool &value_available) const;
  std::map<std::string, std::string>
  getValueKeyPairsAsStringList(const std::string &key, const std::map<std::string, std::string> &default_values,
                               bool &value_available) const;
  std::map<std::string, std::string> getCompData() const;
  void traverseTree(const std::string &key,
                    std::function<void(const std::string &, const std::string &)> call_for_config_values) const;
  bool isEmpty() const;
  bool configValueExists(const std::string &key) const;
  bool keyInfo(const std::string &key, bool &final_leaf, size_t &array_size) const;

  std::string getCompName() const { return comp_name_; }

private:
  template <typename T>
  T getTemplate(const std::string &key, const T &default_value, const bool &default_value_defined,
                bool &value_available) const;
  template <typename T>
  void putTemplate(const std::string &key, const T &value);
  void walkTreeAndCallFunc(const treeStorageType &tree, const std::string &key,
                           std::function<void(const std::string &, const std::string &)> &call_for_config_values) const;

  std::string comp_name_;

  treeStorageType config_data_tree_;
  std::shared_ptr<ConfigCompDataStorage::ConfigCompDataStorage> config_data_storage_;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_COMP_DATA_IMPL_H_
