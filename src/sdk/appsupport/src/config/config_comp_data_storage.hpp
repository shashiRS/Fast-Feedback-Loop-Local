/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_comp_data_storage.hpp
 * @brief    Handles the storage of config database values
 *
 * Implementation of the functions storing the config database values for one component
 *
 */

#ifndef CONFIG_COMP_DATA_STORAGE_H_
#define CONFIG_COMP_DATA_STORAGE_H_

#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>

#include <next/sdk/logger/logger.hpp>

#include "config_data_datatypes.hpp"

namespace next {
namespace appsupport {
namespace ConfigCompDataStorage {

// helper class to convert boost variant datatypes to string
class StringVisitor : public boost::static_visitor<std::string> {
public:
  std::string operator()(bool b) const { return std::to_string(b); }
  std::string operator()(int i) const { return std::to_string(i); }
  std::string operator()(float f) const { return std::to_string(f); }
  std::string operator()(std::string s) const { return s; }
  // not defined boost variant value
  std::string operator()(boost::blank) const { return ""; }
};

class ConfigCompDataStorage {
public:
  ConfigCompDataStorage(const std::string &comp_name) { comp_name_ = comp_name; }
  virtual ~ConfigCompDataStorage() {}

  // Hint: If template functions are not defined in the header file, each used template type needs to be declared in the
  // cpp-file
  template <typename T>
  void putDataInDatabaseTemplate(const std::string &database_key, size_t array_index, const T &value) {
    static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, std::string> ||
                      std::is_same_v<T, bool>,
                  "get function only supports int, bool, float and std::string types");
    const auto &it_config_value_list = config_data_storage_.find(database_key);
    if (it_config_value_list != config_data_storage_.end()) {
      if (it_config_value_list->second.data.size() <= array_index) {
        it_config_value_list->second.data.resize(array_index + 1);
      }
      it_config_value_list->second.data.at(array_index) = ConfigValue{value};
    } else if (0 == array_index) {
      config_data_storage_[database_key] = {std::vector<ConfigValue>{ConfigValue{value}}};
    } else {
      ConfigValueList config_value_list = {std::vector<ConfigValue>{}};
      config_value_list.data.resize(array_index + 1);
      config_value_list.data.at(array_index) = ConfigValue{value};
      config_data_storage_.emplace(database_key, std::move(config_value_list));
    }
  }

  template <typename T>
  T getDataFromDatabase(const std::string &database_key, size_t array_index, const T &default_value,
                        const bool &default_value_defined, bool &value_available) const {
    static_assert(std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, std::string> ||
                      std::is_same_v<T, bool>,
                  "get function only supports int, bool, float and std::string types");

    value_available = false;

    ConfigValue config_value;
    if (!getValueFromDatabase(database_key, array_index, config_value)) {
      if (!default_value_defined) {
        if (0 == array_index) {
          warn(__FILE__, "Key '{0}' respectively '{0}[{1}]' not defined in config database. Default value returned.",
               database_key, array_index);
        } else {
          warn(__FILE__, "Key '{0}[{1}]' not defined in config database. Default value returned.", database_key,
               array_index);
        }
      }

      return default_value;
    }
    try {
      value_available = true;
      return boost::lexical_cast<T>(config_value.value);
    } catch (...) {
      try {
        // check if value is a boolean value specified as string (true/false)
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, bool>) {
          if (boost::iequals(boost::lexical_cast<std::string>(config_value.value), "true")) {
            return boost::lexical_cast<T>(1);
          } else if (boost::iequals(boost::lexical_cast<std::string>(config_value.value), "false")) {
            return boost::lexical_cast<T>(0);
          }
        }
      } catch (...) {
      }

      try {
        // couldn't convert loss free to target type. return false for value_available
        value_available = false;
        warn(__FILE__,
             "No valid or loss free conversion available for key '{0}' to type '{1}' for value '{2}'. Default value "
             "returned.",
             database_key, typeid(T).name(), boost::lexical_cast<std::string>(config_value.value));
      } catch (...) {
        warn(__FILE__,
             "No valid or loss free conversion available for key '{0}' to type '{1}' (Hint: Value can't be converted "
             "to a string). Default value "
             "returned.",
             database_key, typeid(T).name());
      }

      return default_value;
    }
  }

  std::vector<std::string> getValueListFromDatabase(const std::string &database_key) const {
    std::vector<std::string> data_list;

    const auto &it_config_value_list = config_data_storage_.find(database_key);
    if (it_config_value_list == config_data_storage_.end()) {
      return data_list;
    }

    for (const auto &config_value : it_config_value_list->second.data) {
      if (config_value.value.which() != 0) {
        data_list.push_back(boost::apply_visitor(StringVisitor(), config_value.value));
      }
    }

    return data_list;
  }

  std::map<std::string, std::string> getValueKeyPairListFromDatabase(const std::string &database_key) const {
    std::map<std::string, std::string> data_list;

    const auto &it_config_value_list = config_data_storage_.find(database_key);
    if (it_config_value_list == config_data_storage_.end()) {
      return data_list;
    }

    size_t config_value_size = it_config_value_list->second.data.size();
    if (1 == config_value_size) {
      const auto &config_value = it_config_value_list->second.data.at(0);
      if (config_value.value.which() != 0) {
        data_list.emplace(database_key, boost::apply_visitor(StringVisitor(), config_value.value));
      }
    } else if (1 < config_value_size) {
      size_t index = 0;
      for (const auto &config_value : it_config_value_list->second.data) {
        if (config_value.value.which() != 0) {
          std::string database_key_index = database_key + "[" + std::to_string(index) + "]";
          data_list.emplace(database_key_index, boost::apply_visitor(StringVisitor(), config_value.value));
        }
        ++index;
      }
    }

    return data_list;
  }

  std::map<std::string, std::string> getDatabase() const {
    std::map<std::string, std::string> map_database;

    for (const auto &[config_key, config_value_list] : config_data_storage_) {
      bool array_data = false;
      if (config_value_list.data.size() > 1) {
        array_data = true;
      }

      size_t array_index = 0;
      for (const auto &config_value : config_value_list.data) {
        if (config_value.value.which() != 0) {
          std::string url = config_key;
          if (array_data) {
            url += "[" + std::to_string(array_index) + "]";
          }
          // add value as string to output
          map_database.emplace_hint(map_database.end(),
                                    make_pair(url, boost::apply_visitor(StringVisitor(), config_value.value)));
        }
        ++array_index;
      }
    }
    return map_database;
  }

  bool dataInfo(const std::string &database_key, size_t &array_size) const {
    array_size = 0;
    const auto &it_config_value_list = config_data_storage_.find(database_key);
    if (it_config_value_list == config_data_storage_.end()) {
      return false;
    }
    array_size = it_config_value_list->second.data.size();
    return true;
  }

  bool configValueExists(const std::string &database_key, size_t array_index = 0) const {
    ConfigValue config_value;
    bool key_found = getValueFromDatabase(database_key, array_index, config_value);
    return key_found;
  }

  bool isEmpty() { return config_data_storage_.empty(); }

private:
  bool getValueFromDatabase(const std::string &database_key, size_t array_index, ConfigValue &config_value) const {

    const auto &it_config_value_list = config_data_storage_.find(database_key);
    if (it_config_value_list == config_data_storage_.end()) {
      return false;
    }

    if (array_index >= it_config_value_list->second.data.size()) {
      return false;
    }

    config_value = it_config_value_list->second.data[array_index];
    // check if boost:variant contains a default value
    if (config_value.value.which() == 0) {
      return false;
    }
    return true;
  }

  std::string comp_name_;
  std::unordered_map<std::string, ConfigValueList> config_data_storage_;
};

} // namespace ConfigCompDataStorage
} // namespace appsupport
} // namespace next

#endif // CONFIG_COMP_DATA_STORAGE_H_
