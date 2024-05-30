/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_utilities.hpp
 * @brief    Utilitiy functions for the config data
 *
 * Implementation of the functions which are useful for working with the config database.
 *
 **/
#ifndef CONFIG_DATA_UTILITIES_H_
#define CONFIG_DATA_UTILITIES_H_

#include <string>

#include <boost/algorithm/string.hpp>

#include <next/sdk/logger/logger.hpp>

#include "config_data_datatypes.hpp"

namespace next {
namespace appsupport {

class ConfigUtilities {
public:
  inline static bool extractKeyPath(const std::string &key, std::string &path, size_t &array_index) {
    bool _;
    return extractKeyPath(key, path, array_index, _);
  }

  static bool extractKeyPath(const std::string &key, std::string &path, size_t &array_index, bool &array_in_key_path) {
    array_in_key_path = false;
    array_index = 0;
    path = key;
    // remove white spaces from the beginning and the end of the key
    boost::trim_left(path);
    boost::trim_right(path);

    // check for array in path.  If there is no closing bracket, it is assumend that key doesn't contain an array.
    // -> for performance reason don't search for open bracket as well.
    //    -> If there is only an opening bracket it will considered as part of the url
    size_t q = path.find_last_of("]");
    if (q == std::string::npos) {
      return true;
    }

    if (q != path.length() - 1) {
      warn(__FILE__, " A key '{}' where ']' which is not at the end of the key isn't supported.", key);
      path = key; // revert to default values
      return false;
    }

    size_t p = path.find_last_of("[");
    if (p == std::string::npos) {
      warn(__FILE__, " A key '{}' where there is no opening bracket to a closing bracket isn't supported.", key);
      path = key; // revert to default values
      return false;
    }

    if (p != std::string::npos && p == q - 1) {
      warn(__FILE__, "A key '{}' whith an empty array index isn't supported.");
      path = key; // revert to default values
      return false;
    }

    try {
      array_index = std::stoul(path.substr(p + 1, q - p - 1));

      // check for additional opening and closing brackets starting from the position
      // where it was found previously
      size_t p_additional = path.substr(0, p).find_last_of("[");
      size_t q_additional = path.substr(0, q).find_last_of("]");
      if (p_additional != std::string::npos || q_additional != std::string::npos) {
        // revert to default values
        array_index = 0;
        path = key;
        warn(__FILE__,
             "Additional closing and opening bracket in the key '{}' besides the one at the end are not supported in "
             "the key.",
             key);
        return false;
      }

      size_t pos_final_node = key.find_last_of(":");
      if (pos_final_node + 1 == p) {
        // revert to default values
        array_index = 0;
        path = key;
        warn(__FILE__, "Array with an empty node name isn't supported in the key '{}'.", key);
        return false;
      }

      // extracting key was possible
      path = path.substr(0, p); // get key part without array index
      array_in_key_path = true;
      return true;
    } catch (std::exception &e) {
      error(__FILE__, "Extracting the array index from the key '{0}' failed with exception '{1}'. Key isn't supported.",
            key, e.what());
      array_index = 0;
      path = key;
      return false;
    }
  }

  inline static bool checkKeyPath(const std::string &key) {
    size_t array_index;
    bool array_in_path;
    std::string path;

    return checkKeyPath(key, path, array_index, array_in_path);
  }

  static bool checkKeyPath(const std::string &key, std::string &path, size_t &array_index, bool &array_in_key_path) {
    array_in_key_path = false;
    array_index = 0;
    path = key;

    if (path.empty()) {
      warn(__FILE__, "An empty key isn't supported.", key);
      return false;
    }

    // check for empty component name
    if (path.substr(0, 1) == ":") {
      warn(__FILE__, " A key '{}' with an empty component name isn't supported.", key);
      return false;
    }

    // check for empty node name
    if (path.find("::") != std::string::npos) {
      warn(__FILE__, " A key '{}' with an empty node name, which means it contains '::', isn't supported.", key);
      return false;
    }

    if (!extractKeyPath(key, path, array_index, array_in_key_path)) {
      return false;
    }

    // continue with additional check in case an array is in the path
    if (array_in_key_path) {
      // arrays at component level are not supported
      size_t pos_comp_node = key.find_first_of(":");
      // 1. if "array_in_key_path" is true -> an array is present at the end of the key
      // 2. if the previous check for additional brackets passed -> there can be only one array index in the path
      // => if the key doesn't contain the separator ":", the array must be on the component level which is not
      // supported
      if (pos_comp_node == std::string::npos) {
        // revert to default values
        array_in_key_path = false;
        array_index = 0;
        path = key;
        warn(__FILE__, "Arrays at component level (top level) are not supported in the key '{}'", key);
        return false;
      }
    }

    return true;
  }

  static bool checkComponentName(const std::string &comp_name) {
    if (comp_name.empty()) {
      warn(__FILE__, "An empty component name isn't supported.", comp_name);
      return false;
    }

    if (comp_name.find(":") != std::string::npos) {
      warn(__FILE__,
           "':' is not supported in the component name '{}' because it's used as separator for the config "
           "hierarchie.",
           comp_name);
      return false;
    }

    size_t q = comp_name.find_last_of("]");
    size_t p = comp_name.find_last_of("[");
    if (q != std::string::npos || p != std::string::npos) {
      warn(__FILE__, "Square brackets are not supported in the component name '{}'.", comp_name);
      return false;
    }

    return true;
  }

  static std::string createArrayUrl(const std::string &key, size_t array_index) {
    return key + "[" + std::to_string(array_index) + "]";
  }

  static std::string getComponentNameFromKey(const std::string &key) {
    std::string retVal = "";
    std::size_t delimiterPos = key.find(kConfigurationKeyDelimiter);
    if (delimiterPos != std::string::npos) {
      retVal = key.substr(0, delimiterPos);
    } else {
      retVal = key;
    }
    return retVal;
  }
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_DATA_UTILITIES_H_
