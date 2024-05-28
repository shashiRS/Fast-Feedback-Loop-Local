/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     helper_functions.h
 * @brief    some generic helper functions
 *
 **/

#ifndef NEXT_PLUGINS_UTILS_HELPER_FUNCTIONS_H_
#define NEXT_PLUGINS_UTILS_HELPER_FUNCTIONS_H_

#include <next/bridgesdk/datatypes.h>
#include <next/sdk/logger/logger.hpp>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace next {
namespace plugins_utils {

//! splits a string into a vector of strings, separation based on delimiter d
/**
 * If the given delimiter is empty a invalid argument exception will be thrown.
 * Examples:
 *   split_string("hello.world", ".") -> {"hello", "world")
 *   split_string("hello world", ".") -> {"hello world"}
 *   split_string("", ".") -> {""}
 *   split_string("hello..world", ".") -> {"hello", "", "world"}
 */
std::vector<std::string> split_string(const std::string &s, const std::string &d) {
  std::vector<std::string> r;
  size_t p1 = 0;
  size_t p2 = 0;
  const size_t dl = d.length();
  if (0 == dl)
    throw std::invalid_argument("The delimiter cannot be empty.");
  while ((p2 = s.find(d, p1)) != std::string::npos) {
    r.push_back(s.substr(p1, p2 - p1));
    p1 = p2 + dl;
  }
  r.push_back(s.substr(p1));
  return r;
}

//! removes unwanted leading or trailing chars from a string
/**
 * The given string will be manipulated, removing unwanted chars.
 * Example: ' foo bar ' -> 'foo bar'
 * Example: '"Hello World!"' -> 'Hello World'
 *
 * @param str the string to manipulate
 * @param w a string of unwanted chars, by default double quote ('"') and blank (' ')
 */
void trim(std::string &str, const std::string &w = "\" ") {
  const size_t s = str.find_first_not_of(w);
  const size_t e = str.find_last_not_of(w);
  if (std::string::npos == s || std::string::npos == e) {
    str.clear();
  } else {
    str = str.substr(s, e + 1 - s);
  }
}

//! removes on every string of the vector unwanted leading or trailing chars
/**
 * @param v the vector of strings to manipulate
 * @param w a string of unwanted chars, by default double quote ('"') and blank (' ')
 */
void trim(std::vector<std::string> &v, const std::string &w = "\" ") {
  for (auto &s : v) {
    trim(s, w);
  }
}

//! Gets the value of a config parameter from the plugin configuration using a key
/**
 * @param config is a multimap container class containing the plugin configuration
 * @param key is the string used to get a parameter value from the plugin configuration
 * @param value is the string data value retrieved out of plugin configuration
 */

bool GetValueFromPluginConfig(const next::bridgesdk::plugin_config_t &config, const std::string &key,
                              std::string &value) {
  auto url_find_it = config.find(key);
  if (url_find_it == config.end()) {
    value = "";
    return false;
  }
  value = url_find_it->second.value;
  return true;
}

//! Creates unique key out of a config
/**
 * @param config is a multimap container class containing the plugin configuration
 * @return final_key is a string containing a unique key for the config
 */
std::string CreateKeyOutOfPluginConfig(const next::bridgesdk::plugin_config_t &config) {
  std::string final_key = "";
  for (const auto &[key, values] : config) {
    final_key += key + "[" + std::to_string(static_cast<uint32_t>(values.config_type)) + "]:" + values.value + ";";
  }
  return final_key;
}

} // namespace plugins_utils
} // namespace next
#endif // NEXT_PLUGINS_UTILS_HELPER_FUNCTIONS_H_
