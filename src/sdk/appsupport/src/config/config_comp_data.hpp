/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_comp_data.hpp
 * @brief    Handles the config database for one component to store the configuration
 *
 * The configuration database is providing a space to store all the information, needed by the application,
 * to run.
 * The data is organized in a tree like structure and can be accessed by a 'URL' (colon separated string)
 */

#ifndef CONFIG_COMP_DATA_H_
#define CONFIG_COMP_DATA_H_

#include <functional>
#include <map>
#include <string>
#include <vector>

#include <next/sdk/sdk_macros.h>
#include <next/appsupport/appsupport_config.hpp>

namespace next {
namespace appsupport {

class ConfigCompData_Impl;

class DECLSPEC_appsupport ConfigCompData {
public:
  ConfigCompData(const std::string &comp_name);
  virtual ~ConfigCompData();

  // implement appropriate copy and move constructor if needed, e.g. for inserting object into map
  DISABLE_COPY_MOVE(ConfigCompData)

public: // config data public interface
  //! Store integer data into the config database
  /*!
   * @param key      The key / name the value will be stored under
   * @param value    The integer value, to be stored
   */
  virtual void put(const std::string &key, const int &value);

  //! Store float data into the config database
  /*!
   * @param key      The key / name the value will be stored under
   * @param value    The float value, to be stored
   */
  virtual void put(const std::string &key, const float &value);

  //! Store string data into the config database
  /*!
   * @param key      The key / name the value will be stored under
   * @param value    The string value, to be stored
   */
  virtual void put(const std::string &key, const std::string &value);

  //! Store char array into the config database
  /*!
   * Prevents casting of char array like "<text>" to boolean value
   *
   * Will be internally stored as string and can be retrieved with the getString function
   *
   * @param key      The key / name the value will be stored under
   * @param value    The char array value, to be stored
   */
  virtual void put(const std::string &key, const char *value);

  //! Store boolean data into the config database
  /*!
   * @param key      The key / name the value will be stored under
   * @param value    The boolean value, to be stored
   */
  virtual void put(const std::string &key, const bool &value);

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
  inline virtual int getInt(const std::string &key, const int &default_value) const {
    bool _;
    return getInt(key, default_value, _);
  }

  //! Retrieve integer data from the config database
  /*!
   * Get the integer value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key                     The key / name the value should be reteived from
   * @param default_value           The value that will be returned, if the key is not found
   * @param value_available         Returns false if value doesn't exist or value can't be converted to the target type.
   *                                Otherwise true.
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
  inline virtual float getFloat(const std::string &key, const float &default_value) const {
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
  inline virtual std::string getString(const std::string &key, const std::string &default_value) const {
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
  inline virtual bool getBool(const std::string &key, const bool &default_value) const {
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
  virtual std::vector<std::string> getChildrenKeys(const std::string &key, bool full_path = false) const;

  //! Access the config using lists as results for more flexible request values. Will convert all values to a string
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   * Will return all config values for this key.
   * In case an array is stored under this key, all config values are returned.
   * If the key contains an array index, only this specific config value is returned, if existent.
   *
   * @param key             The key / part of a value key for which the config values shall be returned
   * @param default_value   Will be returned, if the key is not found
   *
   * @return        List of config values stored for this key.
   */
  inline virtual std::vector<std::string> getValuesAsStringList(const std::string &key,
                                                                const std::vector<std::string> defaults) const {
    bool _;
    return getValuesAsStringList(key, defaults, _);
  }

  //! Access the config using lists as results for more flexible request values. Will convert all values to a string
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   * Will return all config values for this key.
   * In case an array is stored under this key, all config values are returned.
   * If the key contains an array index, only this specific config value is returned, if existent.
   *
   * @param key             The key / part of a value key for which the config values shall be returned
   * @param default_value   Will be returned, if the key is not found
   * @param value_available  Returns false if value doesn't exist or value can't be converted to the target type.
   *                         Otherwise true.
   *
   * @return        List of config values stored for this key.
   */
  virtual std::vector<std::string>
  getValuesAsStringList(const std::string &key, const std::vector<std::string> defaults, bool &value_available) const;

  //! Access the config using maps as results for more flexible request values. Will convert all values to a string
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   * Will return pairs of config key and config values for this key.
   * If the key is not a final leaf, the default values will be returned.
   * In case an array is stored under this key, all config values of this key are returned. The returned keys will
   * contain the respective array index in this case. If the key contains an array index, only this specific config
   * value is returned, if existent.
   *
   * @param key              The key / part of a value key for which the config values shall be returned
   * @param default_value    Will be returned, if the key is not found or the key is not a final leaf.
   *
   * @return                List of config values and their keys stored for this key.
   */

  inline virtual std::map<std::string, std::string>
  getValueKeyPairsAsStringList(const std::string &key, const std::map<std::string, std::string> defaults) const {
    bool _;
    return getValueKeyPairsAsStringList(key, defaults, _);
  }

  //! Access the config using maps as results for more flexible request values. Will convert all values to a string
  /*!
   * The keys are structured in a tree. The different levels are divided by a colon in the key.
   * Will return pairs of config key and config values for this key.
   * If the key is not a final leaf, the default values will be returned.
   * In case an array is stored under this key, all config values of this key are returned. The returned keys will
   * contain the respective array index in this case. If the key contains an array index, only this specific config
   * value is returned, if existent.
   *
   * @param key              The key / part of a value key for which the config values shall be returned
   * @param default_value    Will be returned, if the key is not found or the key is not a final leaf.
   * @param value_available  Returns false if value doesn't exist or value can't be converted to the target type.
   *                         Otherwise true.
   *
   * @return                List of config values and their keys stored for this key.
   */
  virtual std::map<std::string, std::string>
  getValueKeyPairsAsStringList(const std::string &key, const std::map<std::string, std::string> defaults,
                               bool &value_available) const;

  //! Get all config keys and values as string stored in this class
  /*!
   * @return        Map containing as key the configs keys and as value the config values as string
   */
  std::map<std::string, std::string> getCompData() const;

  //! traverse config tree and call function for each config value
  /*!
   *
   * @param key                     Starting point for traversing the config tree. Use empty string "" to start
   *                                from root node.
   * @param call_for_config_values  Function which will be called for each config value which is stored in this tree.
   *                                The function gets as first parameter the config key (full path) and as second the
   *                                config value.
   */
  void traverseTree(const std::string &key,
                    std::function<void(const std::string &, const std::string &)> call_for_config_values) const;

  //! Get if config values are stored
  /*!
   * @return        True: No config values are stored, otherwise: False
   */
  bool isEmpty() const;

  //! check if a config value exists for a key in config database
  /*!
   * @param key   name of the key to check
   *
   * @return      true if config value was found for this key, else false
   */
  bool configValueExists(const std::string &key) const;

  //! return compononent name
  /*!
   * @return     Component name
   */
  std::string getCompName() const;

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
  bool keyInfo(const std::string &key, bool &final_leaf, size_t &array_size) const;

private:
  ConfigCompData_Impl *impl_ = nullptr;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_COMP_DATA_H_
