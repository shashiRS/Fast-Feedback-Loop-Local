/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_manager.hpp
 * @brief    Manages the configurations of different components
 *
 */

#ifndef CONFIG_DATABASE_MANAGER_H_
#define CONFIG_DATABASE_MANAGER_H_

#include <map>
#include <string>
#include <vector>

#include <next/sdk/sdk_macros.h>
#include <next/appsupport/appsupport_config.hpp>

#include "config_comp_data.hpp"

namespace next {
namespace appsupport {

class ConfigDatabaseManager_Impl;

/*!
 * @brief Class to manager the configurations of different components
 * @brief It uses the comp_data class to store the configurations and the ConfigDatabaseIO class for operations on it
 *
 * The data in the comp_data class are organized in a tree like structure and can be accessed by a 'URL' (":" separated
 * string)
 */

class DECLSPEC_appsupport ConfigDatabaseManager {
public:
  //! Constructor
  /*!
   * @param root_name                     Name of the component for which this ConfigDatabaseManager shall be used.
   * @param filter_by_comp_name           true: Consider only existing components in configuration for all
                                                input operation
                                          false: Consider all config values for input independent of the component
   */
  ConfigDatabaseManager() = delete;
  ConfigDatabaseManager(const std::string &root_name, bool filter_by_comp_name = false);
  virtual ~ConfigDatabaseManager();

public: // configuration public interface
  //! Store integer data into the config database
  /*!
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The integer value, to be stored
   */
  virtual void put(const std::string &key, const int &value);

  //! Store float data into the config database
  /*!
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The float value, to be stored
   */
  virtual void put(const std::string &key, const float &value);

  //! Store string data into the config database
  /*!
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The string value, to be stored.
   */
  virtual void put(const std::string &key, const std::string &value);

  //! Store char array into the config database
  /*!
   * Prevents casting of char array like "<text>" to boolean value
   *
   * Will be internally stored as string and can be retrieved with the getString function
   *
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The char array value, to be stored
   */
  virtual void put(const std::string &key, const char *value);

  //! Store boolean data into the config database
  /*!
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The boolean value, to be stored
   */
  virtual void put(const std::string &key, const bool &value);

  //! Load a configuration file into the config
  /*!
   * The loading is determining the file format automatically based on the trailing text (after the dot)
   * Valid fileformats are .json; .xml
   * The files have to be formated according to the definition of the key structure of the configuration
   * The top level node has to describe the component name of the config.
   * Only values at the final leafs are considered.
   *
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param filePath              The filepath, including the filename of the config file to be loaded
   */
  void putCfg(const std::string &filePath);

  //! inserts JSON string into config tree
  /*!
   * For each component name (key on the first level), a ConfigCompData instance is created
   *
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param json_data             String in json format, which shall be inserted into the config database
   */
  void insertJsonString(const std::string &json_data);

  //! inserts configuration in the current configuration
  /*!
   * Config values of keys which are as well in this config will be overwritten
   * Config values of keys which are not present in this config will be added.
   * Config values of keys which are only present in this config will be not adapted.
   *
   * Hint: If enableFilterByComponent() is activated, only components which are already
   *       present in this config are considered.
   *
   * @param config_data_base_source    configuration shall be added to
   */
  virtual void insertConfig(ConfigDatabaseManager &config_data_base_source);

  // TODO fetch multiple values per key

  //! Retrieve integer data from the config database
  /*!
   * Get the integer value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   *
   * @return        The value retrieved from the config storage, or the default, if key not found
   */
  inline virtual int getInt(const std::string &key, const int &default_value = 0) const {
    bool _;
    return getInt(key, default_value, _);
  }

  //! Retrieve integer data from the config database
  /*!
   * Get the integer value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   * @param value_available  Returns false if value doesn't exist or value can't be converted to the target type.
   *                         Otherwise true.
   *
   * @return        The value retrieved from the config storage, or the default, if key not found
   */
  virtual int getInt(const std::string &key, const int &default_value, bool &value_available) const;

  //! Retrieve float data from the config database
  /*!
   * Get the float value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   *
   * @return        The value retrieved from the config storage, or the default, if key not found
   */
  inline virtual float getFloat(const std::string &key, const float &default_value = 0.0f) const {
    bool _;
    return getFloat(key, default_value, _);
  }

  //! Retrieve float data from the config database
  /*!
   * Get the float value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   * @param value_available  Returns false if value doesn't exist or value can't be converted to the target type.
   *                         Otherwise true.
   *
   * @return        The value retrieved from the config storage, or the default, if key not found
   */
  virtual float getFloat(const std::string &key, const float &default_value, bool &value_available) const;

  //! Retrieve string data from the config database
  /*!
   * Get the string value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   *
   * @return        The value retrieved from the config storage, or the default, if key not found
   */
  inline virtual std::string getString(const std::string &key, const std::string &default_value = "") const {
    bool _;
    return getString(key, default_value, _);
  }

  //! Retrieve string data from the config database
  /*!
   * Get the string value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   * @param value_available  Returns false if value doesn't exist or value can't be converted to the target type.
   *                         Otherwise true.
   *
   * @return        The value retrieved from the config storage, or the default, if key not found
   */
  virtual std::string getString(const std::string &key, const std::string &default_value, bool &value_available) const;

  //! Retrieve boolean data from the config database
  /*!
   * Get the boolean value stored under the key before.
   * If the key is not found, it will return false. So this can also be used to query, if a key is available.
   *
   * @param key     The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   *
   * @return        The value retrieved from the config storage, or false, if key not found
   */
  inline virtual bool getBool(const std::string &key, const bool &default_value = true) const {
    bool _;
    return getBool(key, default_value, _);
  }

  //! Retrieve boolean data from the config database
  /*!
   * Get the boolean value stored under the key before.
   * If the key is not found, it will return false. So this can also be used to query, if a key is available.
   *
   * @param key     The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   * @param value_available  Returns false if value doesn't exist or value can't be converted to the target type.
   *                         Otherwise true.
   *
   * @return        The value retrieved from the config storage, or false, if key not found
   */
  virtual bool getBool(const std::string &key, const bool &default_value, bool &value_available) const;

  //! Get all keys of a child node for a given key
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   *
   * @param key         The key / part of a value key for which the child keys should be returned
   * @param full_path   true: provides the full path to the child node(s)
   *                    false: return only the child node name
   *
   * @return        A list of urls to the child keys. Contains the full path
   */
  inline virtual std::vector<std::string> getChildrenKeys(const std::string &key, bool full_path = false) const {
    {
      bool _;
      return getChildrenKeys(key, full_path, _);
    }
  }

  //! Get all keys of a child node for a given key
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   *
   * @param key                 The key / part of a value key for which the child keys should be returned
   * @param full_path           true: provides the full path to the child node(s)
   *                            false: return only the child node name
   * @param comp_available      true if the component specified in the key is available, otherwise false
   *
   * @return                    A list of urls to the child keys. Contains the full path
   */
  virtual std::vector<std::string> getChildrenKeys(const std::string &key, bool full_path, bool &comp_available) const;

  //! Access the config using lists as results for more flexible request values. Will convert all values to a string
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   * Will return all config values for this key.
   * In case an array is stored under this key, all config values are returned.
   * If the key contains an array index, only this specific config value is returned, if existent.
   *
   * Hint: Values of child nodes of this key are not returned
   *
   * @param key             The key / part of a value key for which the config values shall be returned
   * @param default_value   Will be returned, if the key is not found
   *
   * @return                List of config values stored for this key.
   */
  inline virtual std::vector<std::string> getValuesAsStringList(const std::string &key,
                                                                const std::vector<std::string> defaults = {}) const {
    {
      bool _;
      return getValuesAsStringList(key, defaults, _);
    }
  }

  //! Access the config using lists as results for more flexible request values. Will convert all values to a string
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   * Will return all config values for this key.
   * In case an array is stored under this key, all config values are returned.
   * If the key contains an array index, only this specific config value is returned, if existent.
   *
   * Hint: Values of child nodes of this key are not returned
   *
   * @param key               The key / part of a value key for which the config values shall be returned
   * @param default_value     Will be returned, if the key is not found
   * @param value_available   Returns false if value doesn't exist or value can't be converted to the target type.
   *                          Otherwise true.
   *
   * @return                  List of config values stored for this key.
   */
  virtual std::vector<std::string>
  getValuesAsStringList(const std::string &key, const std::vector<std::string> defaults, bool &value_available) const;

  //! Get all config keys and values as string stored in this class
  /*!
   * @param comp_name   Name of the component to retrieve the data from
   *
   * @return            Map containing as key the configs keys and as value the config values as string
   */
  inline std::map<std::string, std::string> getCompData(const std::string &comp_name) const {
    {
      bool _;
      return getCompData(comp_name, _);
    }
  }

  //! Get all config keys and values as string stored in this class
  /*!
   * @param comp_name       Name of the component to retrieve the data from
   * @param comp_available  true if the specified component is available, otherwise false
   *
   * @return                Map containing as key the configs keys and as value the config values as string
   */
  std::map<std::string, std::string> getCompData(const std::string &comp_name, bool &comp_available) const;

  //! Get if config values are stored for a component
  /*!
   * @param comp_name   Name of the component to retrieve the data from
   *
   * @return            True: No config values are stored, otherwise: False
   */
  inline bool isCompEmpty(const std::string &comp_name) const {
    {
      bool _;
      return isCompEmpty(comp_name, _);
    }
  }

  //! Get if config values are stored for a component
  /*!
   * @param comp_name       Name of the component to retrieve the data from
   * @param comp_available  true if the specified component is available, otherwise false
   *
   * @return                True: No config values are stored, otherwise: False
   */
  bool isCompEmpty(const std::string &comp_name, bool &comp_available) const;

  //! check if a config value exists for a key in config database
  /*!
   * @param key   name of the key to check
   *
   * @return      true if config value was found for this key, else false
   */
  inline bool configValueExists(const std::string &key) const {
    {
      bool _;
      return configValueExists(key, _);
    }
  }

  //! check if a config value exists for a key in config database
  /*!
   * @param key             name of the key to check
   * @param comp_available  true if the component specified in the key is available, otherwise false
   *
   * @return                true if config value was found for this key, else false
   */
  bool configValueExists(const std::string &key, bool &comp_available) const;

  //! get infos about the config key
  /*!
   * @param key           name of the key to get information for
   * @param final_leaf    true: final leaf, false: has child nodes
   * @param array_size    size of the array.
   *                      0: Key doesn't exist
   *                      1: Single element
   *                      >1: Size of the array
   *
   * @return      true if key was found, else false
   */
  inline bool keyInfo(const std::string &key, bool &final_leaf, size_t &array_size) const {
    {
      bool _;
      return keyInfo(key, final_leaf, array_size, _);
    }
  }

  //! get infos about the config key
  /*!
   * @param key             name of the key to get information for
   * @param final_leaf      true: final leaf, false: has child nodes
   * @param array_size      size of the array.
   *                        0: Key doesn't exist
   *                        1: Single element
   *                        >1: Size of the array
   * @param comp_available  true if the component specified in the key is available, otherwise false
   *
   * @return                true if key was found, else false
   */
  bool keyInfo(const std::string &key, bool &final_leaf, size_t &array_size, bool &comp_available) const;

  //! Get configurations from config tree as JSON string not formatted using the json fast writer
  /*!
   * @param comp_name     component name from which the configurations shall be extracted
   * @param key           Extract all config values which are stored under this key name preserving the config tree
   *                      structure, e.g. component:subNode:configElem:value. For an empty key, the root node is used.
   *
   * @return              json string representing config data. In case key doesn't exists, an empty json
   *                      structure is returned
   */
  inline std::string getOneCompConfigAsJsonString(const std::string &comp_name, std::string key = "") {
    {
      bool _;
      return getOneCompConfigAsJsonString(comp_name, key, _);
    }
  }

  //! Get configurations from config tree as JSON string
  /*!
   * @param comp_name       component name from which the configurations shall be extracted
   * @param key             Extract all config values which are stored under this key name preserving the config tree
   *                        structure, e.g. component:subNode:configElem:value. For an empty key, the root node is used.
   * @param comp_available  true if the specified component is available, otherwise false
   * @param formatted       true: provide json string formatted. false: use json fast writer
   *
   * @return                json string representing config data. In case key doesn't exists, an empty json
   *                        structure is returned
   */
  std::string getOneCompConfigAsJsonString(const std::string &comp_name, const std::string &key, bool &comp_available,
                                           bool formatted = false);

  //! Get configuration from list of config trees as JSON string
  /*!
   * @param key                     Extract all config values which are stored under this key name preserving the config
   *                                tree structure, e.g. component:subNode:configElem:value. For an empty key, the root
   *                                node is used.
   * @param formatted               true: provide json string formatted. false: use json fast writer
   *
   * @return                        json string representing config data from all matching components. In case key
   *                                doesn't exists, an empty json structure is returned
   */
  std::string getAllCompConfigsAsJsonString(std::string key = "", bool formatted = false);

  //! Compares the input json to the stored config tree and returns the differences as a json string
  /*!
   * @param json_data               input json to be compared with the stored information
   */
  std::string getDifferences(std::string json_data);

  //! returns the name of config database manager
  /*!
   * @return std::string  return name
   */
  std::string getRootName() const;

  //! returns the names of all components which are stored in this configuration
  /*!
   * @return std::vector<std::string>  return component names
   */
  std::vector<std::string> getConfigCompNames() const;

  //! Checks if the component exists in the configuration tree
  /*!
   * @param comp_name     component name to be checked
   *
   * @return boolean      value with the check result
   */
  bool isComponentInConfig(const std::string &comp_name);

  //! Add a new component to the configuration tree
  /*!
   * @param comp_name     component to be added to configuration true
   *
   * @return boolean      true: component was added.
                          false: component exists already. Nothing added.
   */
  bool addNewComponentConfig(const std::string &comp_name);

  //! Remove all stored configurations
  void clear();

  //! Filter input data to add only config values of components which exists already
  void enableFilterByComponent();

  //! Disable filtering of input data
  void disableFilterByComponent();

  //! Get status of filtering of input data
  /*!
  * @return boolean      true: filtering of input data is active
                         false: no filtering
  */
  bool getFilterByComponent();

  //! Set filtering of input data
  /*!
   * @param filter_by_comp     set if input data shall be filtered
   */
  void setFilterByComponent(bool filter_by_comp);

private:
  ConfigDatabaseManager_Impl *impl_ = nullptr;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_DATABASE_MANAGER_H_
