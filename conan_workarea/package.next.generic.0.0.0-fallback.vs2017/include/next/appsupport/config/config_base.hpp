/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_base.hpp
 * @brief    Including funcctions for configuration handling
 *
 *
 */

#ifndef CONFIG_BASE_H_
#define CONFIG_BASE_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <next/sdk/sdk_macros.h>
#include <next/appsupport/appsupport_config.hpp>
#include <next/appsupport/config/config_tags.hpp>
#include <next/appsupport/config/config_types.hpp>

namespace next {
namespace appsupport {
enum class ConfigSource : uint8_t { Component, CmdOptionsConfig, CmdOptionsParameters, ConfigServer };

class ConfigBase_Impl;

/*!
 * @brief  Base class to implement the configuration handling
 * @brief Implementing some basic configuration handling and enabling derivation to add to it.
 *
 * The configuration handling is providing a space to store all the information, needed by the application,
 * to run. After initialization, the config handler can be called from everywhere in the process,
 * to store and retreive data.
 * The data is organized in a tree like structure and can be accessed by a 'URL' (dot separated string)
 */

class DECLSPEC_appsupport ConfigBase {
public:
  DISABLE_COPY_MOVE(ConfigBase)
  ConfigBase(const std::string &name, bool filter_by_comp_name);
  virtual ~ConfigBase();

  // TODO Key map for config
public: // configuration public interface
  // TODO usecase request: Store more than one item per key

  //! Store integer data into the config
  /*!
   * The value will be stored into the local config under the key and propagated to the global cfonfig, if connected.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The integer value, to be stored
   * @param source   Source under which the data shall be stored
   */
  virtual void put(const std::string &key, const int &value, const ConfigSource &source = ConfigSource::Component);

  //! Store float data into the config
  /*!
   * The value will be stored into the local config under the key and propagated to the global cfonfig, if connected.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The float value, to be stored
   * @param source   Source under which the data shall be stored
   */
  virtual void put(const std::string &key, const float &value, const ConfigSource &source = ConfigSource::Component);

  //! Store string data into the config
  /*!
   * The value will be stored into the local config under the key and propagated to the global cfonfig, if connected.
   *
   * @param key      The key / name the value will be stored under
   * @param value    The string value, to be stored
   * @param source   Source under which the data shall be stored
   */
  virtual void put(const std::string &key, const std::string &value,
                   const ConfigSource &source = ConfigSource::Component);

  //! Store boolean data into the config
  /*!
   * The value will be stored into the local config under the key and propagated to the global cfonfig, if connected.
   * Option is handled slightly different, as boolean is not overloadable (it will result into any other data format)
   *
   * @param key      The key / name the value will be stored under
   * @param value    The boolean value, to be stored
   * @param source   Source under which the data shall be stored
   */
  virtual void put_option(const std::string &key, const bool &value,
                          const ConfigSource &source = ConfigSource::Component);

  //! Load a configuration file into the config
  /*!
   * The loading is determining the file format automatically based on the trailing text (after the dot)
   * Valid fileformats are .json; .ini; .xml
   * The files have to be formated according to the definition of the key structure of the configuration
   *
   * @param filePath  The filepath, including the filename of the config file to be loaded
   * @param source   Source under which the data shall be stored
   */
  virtual void putCfg(const std::string &filePath, const ConfigSource &source = ConfigSource::Component);

  // TODO fetch multiple values per key

  //! Retreive integer data from the config
  /*!
   * Get the integer value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   *
   * @return        The value reteived from the config storage, or the default, if key not found
   */
  virtual int get_int(const std::string &key, const int &default_value, bool block_active, bool &value_available) const;

  inline virtual int get_int(const std::string &key, const int &default_value, bool block_active = false) const {
    bool _;
    return get_int(key, default_value, block_active, _);
  }

  //! Retreive float data from the config
  /*!
   * Get the float value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   *
   * @return        The value reteived from the config storage, or the default, if key not found
   */
  virtual float get_float(const std::string &key, const float &default_value, bool block_active,
                          bool &value_available) const;

  inline virtual float get_float(const std::string &key, const float &default_value, bool block_active = false) const {
    bool _;
    return get_float(key, default_value, block_active, _);
  }

  //! Retreive string data from the config
  /*!
   * Get the string value stored under the key before.
   * If the key is not found, the default value will be returned
   *
   * @param key              The key / name the value should be reteived from
   * @param default_value    The value that will be returned, if the key is not found
   *
   * @return        The value reteived from the config storage, or the default, if key not found
   */
  virtual std::string get_string(const std::string &key, const std::string &default_value, bool block_active,
                                 bool &value_available) const;
  inline virtual std::string get_string(const std::string &key, const std::string &default_value,
                                        bool block_active = false) const {
    bool _;
    return get_string(key, default_value, block_active, _);
  }

  //! Retreive boolean data from the config
  /*!
   * Get the boolean value stored under the key before.
   * If the key is not found, it will return false. So this can also be used to query, if a key is available.
   *
   * @param key              The key / name the value should be reteived from
   *
   * @return        The value reteived from the config storage, or false, if key not found
   */
  virtual bool get_option(const std::string &key, const bool &default_value, bool block_active,
                          bool &value_available) const;
  inline virtual bool get_option(const std::string &key, const bool &default_value, bool block_active = false) const {
    bool _;
    return get_option(key, default_value, block_active, _);
  }

  //! Get all child nodes for a given key
  /*!
   * The keys are structured in a tree. The different levels are divided by a dot in the key.
   *
   * @param key         The key / part of a value key all childs should be given
   * @param full_path   true: provides the full path to the child node(s)
   *                    false: return only the child node name(s)
   *
   * @return        A list of names, that can be appended to the key to get to next level of valid keys
   */
  virtual std::vector<std::string> getChildren(const std::string &key, bool full_path = false);

  //! Access the config using lists as results for more flexible request values
  virtual std::vector<std::string> getStringList(const std::string &key, const std::vector<std::string> &defaults,
                                                 bool block_active, bool &value_available) const;

  inline virtual std::vector<std::string>
  getStringList(const std::string &key, const std::vector<std::string> &defaults, bool block_active = false) const {
    bool _;
    return getStringList(key, defaults, block_active, _);
  }

  //! Get all Configurations from property tree as JSON string
  /*!
   *
   * @param filterOutPrivateKeys    flag used to decide if filtering using the private values is used
   * @param formatted               true: provide json string formatted. false: use json fast writer
   *
   * @return json string representing entire property data
   */
  std::string getAllConfigurationsAsJson(bool filterOutPrivateKeys = false, bool filterOutGenericKeys = false,
                                         bool filterOutReadOnly = false, bool formatted = false) const;

  //! Get component Configuration from property tree as JSON string
  /*!
   *
   * @param component   The name of the requested component
   * @param filterOutPrivateKeys    flag used to decide if filtering using the private values is used
   * @param formatted               true: provide json string formatted. false: use json fast writer
   *
   * @return            Json string representing the component property data
   *                    or empty string if component is not available
   */
  std::string getComponentConfigurationsAsJson(const std::string &component, bool filterOutPrivateKeys = false,
                                               bool formatted = false) const;

  //! inserts  JSON string into property tree
  /*!
   * @param json_str is the string in json format, which would be inserted into the ptree
   * @param filterOutPrivateKeys   flag used to decide if filtering using the private values is used
   * @param resetActiveTag   flag used to decide if the current active Tag should be reset
   * @param source   Source under which the data shall be stored
   */
  virtual void insert(const std::string &json_str, bool filterOutPrivateKeys = false, bool resetActiveTag = false,
                      const ConfigSource &source = ConfigSource::Component);

  //! checks if the configuration received from the server contains any Generic data
  /*!
   * @param json_str is the string in json format, which would be inserted into the ptree
   */
  // void checkGenericConfig(const std::string &json_str);

  //! adds the callback function which would be invoked upon each write operation in the config tree
  /*!
   * @param hook callable object which would be called
   */
  size_t addChangeHook(configtypes::change_hook hook);

  //! removes the callback function which would be invoked upon each write operation in the config tree
  /*!
   * @param hook callable object which would be removed
   */
  void removeChangeHook(size_t id);

  //! returns the component name
  /*!
   * @return std::string return type, which is component name
   */
  std::vector<std::string> componentName() const;

  //! returns the name of the first component initialized
  /*!
   * @return std::string return type, which is component name
   */
  std::string getRootName() const;

  //! Stores the received config value from the server
  /*!
   * @param value - the received string from the server
   */
  void setConfigValueFromServer(const std::string &value);

  //! Returns the received config value from the server
  /*!
   * @param
   */
  std::string getConfigValueFromServer();

  //! Stores a list of private config keys
  /*!
   * @param configKeys - string array of the defined private configuration keys
   */
  // void definePrivateConfigKeys(std::vector<std::string> configKeys);

  //! Adds a sinkle key to the PrivateKey list
  /*!
   * @param configKey - string of the defined private configuration key
   */
  // void putPrivateKey(std::string configKey);

  //! Compares the input json to the stored property tree and returns the differences as a json string
  /*!
   * @param inputJson - input json to be compared with the stored information
   * @return Json string representing the differences
   */
  std::string getDifferences(std::string inputJson);

  //! Checks if the component is valid in the current configuration tree
  /*!
   * @param value - component name to be checked
   * @return boolean value with the check result
   */
  bool isComponentInCurrentTree(const std::string &componentName);

  //! Adds a tag to a config key
  /*!
   * @param key - the configuration key
   * @param tag - tag value from the next::appsupport::configtags::TAG structure
   */
  // virtual void setTag(const std::string &key, next::appsupport::configtags::TAG tag);

  //! Adds a tag to a config key
  /*!
   * @param key - the configuration key
   * @param tag - tag string
   */
  // virtual void setTag(const std::string &key, const std::string &tag);

  //! Removes a tag from the config key tag list
  /*!
   * @param key - the configuration key
   * @param tag - tag to be removed
   */
  // virtual void removeTag(const std::string &key, next::appsupport::configtags::TAG tag);

  //! Removes a tag from the config key tag list
  /*!
   * @param key - the configuration key
   * @param tag - tag to be removed
   */
  // virtual void removeTag(const std::string &key, const std::string &tag);

  //! Removes all tags from a config key
  /*!
   * @param key - the configuration key
   * @param tag - tag to be removed
   */
  // virtual void removeAllTagsForKey(const std::string &key);

  //! Get all tags for a given key
  /*!
   *
   * @param key     The key for which we reguested the tags
   *
   * @return        A list of tags
   */
  // std::vector<std::string> getTags(const std::string &key);

  //! Get all keys from the tree that have the provided tag
  /*!
   *
   * @param tag     The tag used for filtering
   *
   * @return        the component tree filtered by tag as a json string
   */
  // std::string getTreeFilteredByTagAsJson(std::string tag);

  //! Get all keys from the tree that have all provided tags
  /*!
   *
   * @param tah     The tags used for filtering
   *
   * @return        the component tree filtered by the tags as a json string
   */
  // std::string getTreeFilteredByTagsIntersectionAsJson(std::vector<std::string> tagList);

  //! Get all keys from the tree that have at least one of the provided tags
  /*!
   *
   * @param tah     The tags used for filtering
   *
   * @return        the component tree filtered by the tags as a json string
   */
  // std::string getTreeFilteredByTagsUnionAsJson(std::vector<std::string> tagList);

  //! Get all tags from the configuration tree
  /*!
   * @return        the component tags as a json string  {component {tag [url list]}}
   */
  // std::string getTagsAsJsonStr();

  //! Get all tags from the configuration tree
  /*!
   * @return        the component tags as a json string  {component {tag [url list]}}
   */
  // std::string getTagsAsJsonStrForGUI();

  //! Updates the curent tags by a provided json string
  /*!
   * @param jsonStr -  the component tags as a json string  {component {tag [url list]}}
   */
  // void updateTagsFromJsonString(std::string jsonStr);

  //! Checks if the given key has a list value
  /*!
   * @param  key - the key that should be checked
   * @return   boolean value with the result
   */
  bool isList(const std::string &key);

  //! Adds the ACTIVE tag to the elemenst of the list specified by value
  /*!
   * @param key - the configuration key for the list
   * @param value - list value to set to ACTIVE
   * @param reset - if true will remove all other ACTIVE values from the list
   */
  // void setActive(const std::string &key, const std::string &value, const bool &reset = true);

  //! Removes the ACTIVE tag to the elemenst of the list specified by value
  /*!
   * @param key - the configuration key for the list
   * @param value - list value to remove the ACTIVE tag from
   */
  // void resetActive(const std::string &key, const std::string &value);

  /*! Enabels filtering by the component name when loading a configuration
   */
  void enableFilterByComponent();

  /*! Disabels filtering by the component name when loading a configuration
   */
  void disableFilterByComponent();

  //! Get status of filtering of input data for currently active ConfigCompDatabase
  /*!
   * @return boolean    filter status of currently active ConfigCompDatabase
   */
  bool getCurrentFilterByComponentStatus();

  //! Set filtering of input data for currently active ConfigCompDatabase
  /*!
   * @param filter_by_comp    set filter status of currently active ConfigCompDatabase
   */
  void setCurrentFilterByComponentStatus(bool filter_by_comp);

protected:
  //! Concatenates all previously loaded configs in the main one
  void finishInitialization();
  /*! Clears the PTree
   */
  void clearTree() const;

  void waitForResponse();
  void setWaitForResponseFlag(bool value);
  bool addNewComponentToLocalTree(const std::string &componentName);

  bool isInitFinished() const;

private:
  ConfigBase_Impl *_impl = nullptr;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_BASE_H_
