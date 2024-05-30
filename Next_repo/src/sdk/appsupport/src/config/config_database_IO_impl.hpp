/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_IO_impl.hpp
 * @brief    Functions for handling input and output in the config database
 */

#ifndef CONFIG_DATABASE_IO_IMPL_H_
#define CONFIG_DATABASE_IO_IMPL_H_

#include <map>
#include <string>
#include <vector>

#include <json/json.h>
#include <boost/property_tree/ptree.hpp>

#include "config_comp_data.hpp"
#include "config_database_IO.hpp"

namespace next {
namespace appsupport {

// contains the tree structure read in by file or string
// !!! has nothing to do with the tree structure used inside ConfigCompData !!!
using inputDataType = boost::property_tree::ptree;

class ConfigDatabaseIO_Impl {

public:
  ConfigDatabaseIO_Impl() = delete;
  virtual ~ConfigDatabaseIO_Impl() = delete;

public:
  static void putCfg(const std::string &file_path, std::map<std::string, ConfigCompData> &map_config_comp_data,
                     bool filter_by_comp_name);
  static void insertJsonString(const std::string &json_data,
                               std::map<std::string, ConfigCompData> &map_config_comp_data, bool filter_by_comp_name);
  static void insertConfigCompData(const std::map<std::string, ConfigCompData> &source,
                                   std::map<std::string, ConfigCompData> &destination, bool filter_by_comp_name);

  static std::string getCompConfigAsJsonString(ConfigCompData &config_comp_data, std::string key, bool formatted);
  static std::string getCompConfigAsJsonString(std::map<std::string, ConfigCompData> &map_config_comp_data,
                                               std::string key, bool formatted);

  static std::string getDifferences(std::string json_data, std::map<std::string, ConfigCompData> &map_config_comp_data);

private:
  // these function has nothing to do with the ptree used in ConfigCompData.
  // ptree here is only used for convenience to read in data, e.g. xml/json files
  // !!! don't use any ptree for exchanging data with ConfigCompData because the tree storage structure in
  // ConfigCompData can be replaced !!!
  static void insertInputData(const inputDataType &tree, std::map<std::string, ConfigCompData> &map_config_comp_data,
                              bool filter_by_comp_name);

  // these function has nothing to do with the walkTreeAndCallFunc used in ConfigCompData.
  // ptree here is only used for convenience to read in data, e.g. xml/json files
  // !!! don't use any ptree for exchanging data with ConfigCompData because the tree storage structure in
  // ConfigCompData can be replaced !!!
  static void walkInputDataAndInsertValues(const inputDataType &tree, const std::string &key,
                                           ConfigCompData &config_comp_data);

  static std::map<std::string, inputDataType> splitInputDataByComp(const inputDataType &tree);

  static void getConfigTreeAsJson(const ConfigCompData &config_comp_data, const std::string &key,
                                  Json::Value *config_tree);
  static Json::Value *addJsonValueFromKey(const std::string &key, Json::Value *config_tree);
  static void addValueToJson(const ConfigCompData &config_comp_data, const std::string &key, size_t array_size,
                             Json::Value *config_tree);

  static void
  extractCompDatabase(const std::map<std::string, ConfigCompData> &map_config_comp_data,
                      std::map<std::string, std::map<std::string, std::string>> &map_extracted_config_comp_database);

  static std::string createJsonString(Json::Value &value, bool formatted);
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_DATABASE_IO_IMPL_H_
