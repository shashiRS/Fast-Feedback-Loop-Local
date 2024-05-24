/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_IO.hpp
 * @brief    Functions for handling input and output in the config database
 *
 *
 */

#ifndef CONFIG_DATABASE_IO_H_
#define CONFIG_DATABASE_IO_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "config_comp_data.hpp"

namespace next {
namespace appsupport {

class ConfigDatabaseIO_Impl;

/*!
 * @brief Class to handle the input and output in the comp_data class
 * @brief It gets the comp_data class as input and operates on it by inserting config values or extracting it.
 *
 * The data in the comp_data class are organized in a tree like structure and can be accessed by a 'URL' (: separated
 * string)
 */

class ConfigDatabaseIO {
public:
  ConfigDatabaseIO() = delete;
  virtual ~ConfigDatabaseIO() = delete;

public: // configuration public interface
  //! Load a configuration file into the config
  /*!
   * The loading is determining the file format automatically based on the trailing text (after the dot)
   * Valid fileformats are .json; .xml
   * The files have to be formated according to the definition of the key structure of the configuration
   * The top level node has to describe the component name of the config.
   * Only values at the final leafs are considered.
   *
   * @param filePath                  The filepath, including the filename of the config file to be loaded
   * @param map_config_comp_data      Map containing the config databases config file is added to
   * @param filter_by_comp_name       true: consider only the components of the config file which are already present
                                      in map_config_comp_data
                                      false: consider all components in the config file
   */
  static void putCfg(const std::string &filePath, std::map<std::string, ConfigCompData> &map_config_comp_data,
                     bool filter_by_comp_name = false);

  //! inserts JSON string into config tree.
  /*!
  * Config values of keys which are as well in map_config_comp_data will be overwritten
  * Config values of keys which are not present in map_config_comp_data will be added.
  * Config values of keys which are only present in map_config_comp_data will be not adapted.
  *
  * For each component name (key on the first level), a ConfigCompData instance is created (if it doesn't exist already)
  *
  * @param json_data                  String in json format, which shall be inserted into the config database
  * @param map_config_comp_data       json_data inserted in the config database
  * @param filter_by_comp_name        true: consider only the components in "json_data" which are already present
                                      in map_config_comp_data
                                      false: consider all components from "json_data"
  */
  static void insertJsonString(const std::string &json_data,
                               std::map<std::string, ConfigCompData> &map_config_comp_data,
                               bool filter_by_comp_name = false);

  //! insert source configuration into destination configuration
  /*!
  * Config values of keys which are as well in destination will be overwritten
  * Config values of keys which are not present in destination will be added.
  * Config values of keys which are only present in destination will be not adapted.
  *
  * For each component name (key on the first level), a ConfigCompData instance is created (if it doesn't exist already
  * and "filter_by_comp_name" is false)
  *
  * @param source                     Map containing ConfigCompData to insert
  * @param destination                Map containing ConfigCompData where the "source" shall be added to
  * @param filter_by_comp_name        true: consider only the components of the "source" which are already present
                                      in the destination.
                                      false: consider all components of "source"
  */
  static void insertConfigCompData(const std::map<std::string, ConfigCompData> &source,
                                   std::map<std::string, ConfigCompData> &destination,
                                   bool filter_by_comp_name = false);

  //! Get configurations from config tree as JSON string
  /*!
   * @param config_comp_data   Database to get config values as string
   * @param key                Extract all config values which are stored under this key name preserving the config tree
   *                           structure. For an empty key, the root node is used.
   * @param formatted          true: provide json string formatted. false: use json fast writer
   *
   * @return                    json string representing config data. In case key doesn't exists, an empty json
   *                            structure is returned
   */
  static std::string getCompConfigAsJsonString(ConfigCompData &config_comp_data, std::string key = "",
                                               bool formatted = false);

  //! Get configuration from list of config trees as JSON string
  /*!
   * @param map_config_comp_data    Map containing the config databases to get config values as string
   * @param key                     Extract all config values which are stored under this key name preserving the config
   *                                tree structure. For an empty key, the root node is used.
   * @param formatted               true: provide json string formatted. false: use json fast writer
   *
   * @return                        json string representing config data from all databases. In case key doesn't exists,
   *                                an empty json structure is returned
   */
  static std::string getCompConfigAsJsonString(std::map<std::string, ConfigCompData> &map_config_comp_data,
                                               std::string key = "", bool formatted = false);

  //! Compares the input json to the stored config tree and returns the differences as a json string
  /*!
   * Checks only which values of "json_data" are different or not available in "map_config_comp_data"
   * and not the other way around. That means values which are only in "map_config_comp_data" but not in
   * "json_data" won't be returned as differences.
   *
   * @param json_data               input json to be compared with the stored information
   * @return map_config_comp_data   Map containing the config databases with the config values to compare with
   */
  static std::string getDifferences(std::string json_data, std::map<std::string, ConfigCompData> &map_config_comp_data);
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_DATABASE_IO_H_
