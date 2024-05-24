/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_IO.cpp
 * @brief    Functions for handling input and output in the config database
 *
 **/

#include "config_database_IO.hpp"

#include "config_database_IO_impl.hpp"

namespace next {
namespace appsupport {

void ConfigDatabaseIO::putCfg(const std::string &filePath, std::map<std::string, ConfigCompData> &map_config_comp_data,
                              bool filter_by_comp_name) {
  ConfigDatabaseIO_Impl::putCfg(filePath, map_config_comp_data, filter_by_comp_name);
}

std::string ConfigDatabaseIO::getCompConfigAsJsonString(ConfigCompData &config_comp_data, std::string key,
                                                        bool formatted) {
  return ConfigDatabaseIO_Impl::getCompConfigAsJsonString(config_comp_data, key, formatted);
}
std::string ConfigDatabaseIO::getCompConfigAsJsonString(std::map<std::string, ConfigCompData> &map_config_comp_data,
                                                        std::string key, bool formatted) {
  return ConfigDatabaseIO_Impl::getCompConfigAsJsonString(map_config_comp_data, key, formatted);
}

void ConfigDatabaseIO::insertJsonString(const std::string &json_data,
                                        std::map<std::string, ConfigCompData> &map_config_comp_data,
                                        bool filter_by_comp_name) {
  ConfigDatabaseIO_Impl::insertJsonString(json_data, map_config_comp_data, filter_by_comp_name);
}

void ConfigDatabaseIO::insertConfigCompData(const std::map<std::string, ConfigCompData> &source,
                                            std::map<std::string, ConfigCompData> &destination,
                                            bool filter_by_comp_name) {
  ConfigDatabaseIO_Impl::insertConfigCompData(source, destination, filter_by_comp_name);
}

std::string ConfigDatabaseIO::getDifferences(std::string json_data,
                                             std::map<std::string, ConfigCompData> &map_config_comp_data) {
  return ConfigDatabaseIO_Impl::getDifferences(json_data, map_config_comp_data);
}

} // namespace appsupport
} // namespace next
