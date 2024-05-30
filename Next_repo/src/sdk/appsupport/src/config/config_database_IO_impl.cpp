/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_database_IO_impl.cpp
 * @brief    Functions for handling input and output in the config database
 *
 **/

#include "config_database_IO_impl.hpp"

#include <exception>
#include <map>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <next/sdk/logger/logger.hpp>

#include "config_comp_data.hpp"
#include "config_data_datatypes.hpp"
#include "config_utilities.hpp"

namespace pt = boost::property_tree;

namespace next {
namespace appsupport {

void ConfigDatabaseIO_Impl::putCfg(const std::string &file_path,
                                   std::map<std::string, ConfigCompData> &map_config_comp_data,
                                   bool filter_by_comp_name) {
  boost::filesystem::path config_path(file_path);

  if (!boost::filesystem::exists(config_path)) {
    return;
  }

  try {
    inputDataType loaded_data;
    if (config_path.extension() == ".xml") {
      pt::read_xml(file_path, loaded_data, pt::xml_parser::trim_whitespace);
    } else if (config_path.extension() == ".json") {
      pt::read_json(file_path, loaded_data);
    } else {
      next::sdk::logger::warn(__FILE__, "Unsupported config extension : {0}", config_path.extension().string());
    }
    // update the currently loaded data in the ConfigCompData
    insertInputData(loaded_data, map_config_comp_data, filter_by_comp_name);
  } catch (const pt::ptree_error &e) {
    next::sdk::logger::warn(__FILE__, "Error reading config file {0} : {1}", config_path.filename().string(), e.what());
  }
}

void ConfigDatabaseIO_Impl::insertJsonString(const std::string &json_str,
                                             std::map<std::string, ConfigCompData> &map_config_comp_data,
                                             bool filter_by_comp_name) {
  if (!json_str.empty()) {
    std::stringstream ss;
    ss << json_str;
    inputDataType tempInputData;
    try {
      // json_str needs to contain the component names on the first level
      pt::read_json(ss, tempInputData);
    } catch (const std::exception &e) {
      next::sdk::logger::warn(__FILE__, "Error parsing json string {0} : {1}", json_str, e.what());
    }
    insertInputData(tempInputData, map_config_comp_data, filter_by_comp_name);
  }
}

void ConfigDatabaseIO_Impl::insertConfigCompData(const std::map<std::string, ConfigCompData> &source,
                                                 std::map<std::string, ConfigCompData> &destination,
                                                 bool filter_by_comp_name) {
  // iterate over component data
  for (auto &[comp_name, comp_data] : source) {
    // check if component shall be considered
    auto it_config_comp_data = destination.find(comp_name);
    if (filter_by_comp_name && it_config_comp_data == destination.end()) {
      debug(__FILE__, "Component '{}' is skipped because component is not existing in destination.", comp_name);
      continue;
    }
    if (it_config_comp_data == destination.end()) {
      // check if component name is valid
      if (!ConfigUtilities::checkComponentName(comp_name)) {
        continue;
      }
      // create new ConfigCompData
      auto ret = destination.try_emplace(comp_name, comp_name);
      it_config_comp_data = ret.first;
    }

    // add config values from source to destination
    auto &comp_data_destination = it_config_comp_data->second;
    comp_data.traverseTree("", [&comp_data_destination](const std::string &key, const std::string &value) {
      comp_data_destination.put(key, value);
    });
  }
}

std::string ConfigDatabaseIO_Impl::getCompConfigAsJsonString(ConfigCompData &config_comp_data, std::string key,
                                                             bool formatted) {
  Json::Value config_tree;
  getConfigTreeAsJson(config_comp_data, key, &config_tree);

  return createJsonString(config_tree, formatted);
}

std::string
ConfigDatabaseIO_Impl::getCompConfigAsJsonString(std::map<std::string, ConfigCompData> &map_config_comp_data,
                                                 std::string key, bool formatted) {
  Json::Value combined_config_tree;
  for (const auto &it_comp_data : map_config_comp_data) {
    getConfigTreeAsJson(it_comp_data.second, key, &combined_config_tree);
  }
  return createJsonString(combined_config_tree, formatted);
}

std::string ConfigDatabaseIO_Impl::getDifferences(std::string json_data,
                                                  std::map<std::string, ConfigCompData> &map_config_comp_data) {

  std::map<std::string, ConfigCompData> map_config_comp_data_json;
  // write json string into comp_data for easier comparision afterwards
  insertJsonString(json_data, map_config_comp_data_json, false);

  // extract database from comp_data json
  std::map<std::string, std::map<std::string, std::string>> map_extracted_comp_database_json;
  extractCompDatabase(map_config_comp_data_json, map_extracted_comp_database_json);

  Json::Value diff_json_values;

  // iterate over json values and check which one exists in current database
  for (const auto &[comp_name_json, comp_database_json] : map_extracted_comp_database_json) {
    // check if component exist in current database
    if (const auto &it_comp_data_current = map_config_comp_data.find(comp_name_json);
        it_comp_data_current != map_config_comp_data.end()) {
      // iterate over all config values of the json component
      for (const auto &[config_key, config_value] : comp_database_json) {
        bool value_exist = false;
        std::string config_value_current = it_comp_data_current->second.getString(config_key, "", value_exist);
        // check if config values inside json doesn't exists in database of this component or is different
        if (!value_exist || config_value_current != config_value) {
          // create json value
          Json::Value *diff_json_values_updated = addJsonValueFromKey(config_key, &diff_json_values);
          // add non existing config value to json
          *diff_json_values_updated = config_value;
        }
      }
    } else {
      // add all config values of this component
      for (const auto &[config_key, config_value] : comp_database_json) {
        // create json value
        Json::Value *diff_json_values_updated = addJsonValueFromKey(config_key, &diff_json_values);
        // add non existing config value to json
        *diff_json_values_updated = config_value;
      }
    }
  }

  return createJsonString(diff_json_values, false);
}

void ConfigDatabaseIO_Impl::insertInputData(const inputDataType &input_data,
                                            std::map<std::string, ConfigCompData> &map_config_comp_data,
                                            bool filter_by_comp_name) {
  // split input_data on component level
  std::map<std::string, inputDataType> map_comp_data = splitInputDataByComp(input_data);
  for (const auto &[comp_name, comp_data] : map_comp_data) {
    // check if component shall be considered
    if (!filter_by_comp_name || map_config_comp_data.find(comp_name) != map_config_comp_data.end()) {
      // check if component name is valid
      if (!ConfigUtilities::checkComponentName(comp_name)) {
        continue;
      }
      // create new ConfigCompData if none exists with this name and return ConfigCompData (existing or newly created)
      const auto &it_config_comp_data = map_config_comp_data.emplace(make_pair(comp_name, comp_name));
      // use comp_name as root name
      walkInputDataAndInsertValues(comp_data, comp_name, it_config_comp_data.first->second);
    } else {
      debug(__FILE__, "Component '{}' is skipped because component is not existing in destination.", comp_name);
    }
  }
}

void ConfigDatabaseIO_Impl::walkInputDataAndInsertValues(const inputDataType &input_data, const std::string &key,
                                                         ConfigCompData &config_comp_data) {
  // empty keys are not allowed
  // write config value if it's a final leaf -> json and xml can only have values at the final leaf
  // Hint: input_data.data().empty() returns as well empty for an empty string
  if (!key.empty() && input_data.empty()) {
    config_comp_data.put(key, input_data.data());
  } else if (key.empty()) {
    warn(__FILE__, "Empty config keys are ignored during insert.");
  } else if (!input_data.data().empty()) {
    warn(__FILE__, "Data of key '{}' is ignored because it's not at a final leaf.", key);
  }

  // final leaf. No child nodes
  if (input_data.empty()) {
    return;
  }

  size_t array_index = 0;
  for (auto &it_child : input_data) {
    std::string child_key = key;
    // check if child contains an array
    if (!it_child.first.empty()) {
      if (it_child.first.find(":") != std::string::npos) {
        warn(__FILE__,
             "':' is not supported in the config key name '{}' because it's used as separator for the config "
             "hierarchie.",
             it_child.first);
        continue;
      }
      // add a separator between key segments if it's not the root node
      if (!key.empty()) {
        child_key += kConfigurationKeyDelimiter;
      }
      child_key += it_child.first;
    } else {
      child_key += "[" + std::to_string(array_index) + "]";
      ++array_index;
    }

    // walk child nodes
    walkInputDataAndInsertValues(it_child.second, child_key, config_comp_data);
  }
}

std::map<std::string, inputDataType> ConfigDatabaseIO_Impl::splitInputDataByComp(const inputDataType &input_data) {
  std::map<std::string, inputDataType> map_input_data_by_comp;
  if (input_data.empty()) {
    return map_input_data_by_comp;
  }
  // iterate over all child nodes to get configs for each component
  // component name is only on the first level
  for (const auto &it_child : input_data) {
    // check if component name is valid
    if (!ConfigUtilities::checkComponentName(it_child.first)) {
      continue;
    }
    map_input_data_by_comp[it_child.first] = it_child.second;
  }
  return map_input_data_by_comp;
}

void ConfigDatabaseIO_Impl::getConfigTreeAsJson(const ConfigCompData &config_comp_data, const std::string &key,
                                                Json::Value *config_tree) {
  std::vector<std::string> children_key_list = config_comp_data.getChildrenKeys(key, true);
  // check for final leaf
  if (children_key_list.empty()) {
    std::string key_path{""};
    size_t array_index{0};
    // remove array index from path if present
    if (!ConfigUtilities::extractKeyPath(key, key_path, array_index)) {
      return;
    }
    bool final_leaf{false};
    size_t array_size{0};
    // check if value exist for this key
    if (config_comp_data.keyInfo(key, final_leaf, array_size)) {
      Json::Value *updated_config_tree = addJsonValueFromKey(key_path, config_tree);
      addValueToJson(config_comp_data, key_path, array_size, updated_config_tree);
    }
  } else {
    // check if config value exists which is not at a final leaf
    if (config_comp_data.configValueExists(key)) {
      warn(__FILE__,
           "Value(s) for config key '{}' isn't exported because values which aren't at a final node are not "
           "supported by json.",
           key);
    }

    // get config from child nodes
    for (auto const &children_key : children_key_list) {
      getConfigTreeAsJson(config_comp_data, children_key, config_tree);
    }
  }
}

Json::Value *ConfigDatabaseIO_Impl::addJsonValueFromKey(const std::string &key, Json::Value *config_tree) {
  std::vector<std::string> node_list;
  // get all nodes
  // convert kConfigurationKeyDelimiter to string to fit input arguments of boost::is_any_of.
  // Hint: Converting kConfigurationKeyDelimiter to a char pointer doesn't work correctly
  boost::split(node_list, key, boost::is_any_of(std::string{kConfigurationKeyDelimiter}));

  // keep position of the original config_tree
  Json::Value *updated_config_tree = config_tree;

  // add nodes to json
  for (auto &node : node_list) {
    updated_config_tree = &(*updated_config_tree)[node];
  }
  return updated_config_tree;
}

void ConfigDatabaseIO_Impl::addValueToJson(const ConfigCompData &config_comp_data, const std::string &key,
                                           size_t array_size, Json::Value *config_tree) {
  // add all elements which are presented for this key
  // check if key contains an array
  if (1 == array_size) {
    *config_tree = config_comp_data.getString(key, "");
  } else {
    Json::Value arr = Json::Value(Json::arrayValue);
    for (size_t i = 0; i < array_size; ++i) {
      std::string key_array = ConfigUtilities::createArrayUrl(key, i);
      arr.append(config_comp_data.getString(key_array, ""));
    }
    *config_tree = arr;
  }
}

void ConfigDatabaseIO_Impl::extractCompDatabase(
    const std::map<std::string, ConfigCompData> &map_config_comp_data,
    std::map<std::string, std::map<std::string, std::string>> &map_extracted_config_comp_database) {

  for (auto const &[comp_name, config_comp_data] : map_config_comp_data) {
    // no check needed for valid component names -> only used by getDifferences function
    map_extracted_config_comp_database.emplace(make_pair(comp_name, config_comp_data.getCompData()));
  }
}

std::string ConfigDatabaseIO_Impl::createJsonString(Json::Value &value, bool formatted) {
  if (formatted) {
    Json::StyledWriter styled_writer;
    return styled_writer.write(value);
  } else {
    Json::FastWriter fast_writer;
    return fast_writer.write(value);
  }
}

} // namespace appsupport
} // namespace next
