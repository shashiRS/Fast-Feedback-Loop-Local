/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_change_hook_collection.cpp
 * @brief    config hook function
 */

#include <next/appsupport/config/config_change_hook_collection.hpp>

#include <boost/algorithm/string.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/sdk/profiler/profiler.hpp>

namespace next {
namespace appsupport {

bool updateProfilerLevel(std::string key, std::string profiling_level_key) {

  if (key != profiling_level_key && key != "full_cfg")
    return true;

  std::string default_profiling_level = next_profiling::kProfilingLevel.at(next_profiling::ProfilingLevel::OFF);
  std::string profiling_level =
      next::appsupport::ConfigClient::getConfig()->get_string(profiling_level_key, default_profiling_level);
  std::string upper_case_profiling_level = boost::to_upper_copy<std::string>(profiling_level);

  try {
    if (profiling_level != next_profiling::kProfilingLevel.at(next_profiling::getProfilingLevel())) {
      std::string available_value_str = "";
      auto level_it = next_profiling::kProfilingLevel.begin();
      for (; level_it != next_profiling::kProfilingLevel.end(); level_it++) {
        if (level_it->second == upper_case_profiling_level) {
          next_profiling::setProfilingLevel(level_it->first);
          break;
        }
        available_value_str += " \"" + level_it->second + "\",";
      }
      if (level_it == next_profiling::kProfilingLevel.end()) {
        available_value_str[available_value_str.length() - 1] = ' ';
        next::sdk::logger::warn(__FILE__,
                                "Profiling level: {0} is not a valid value.\n\tCurrent Profiling level is : "
                                "{1}.\n\tAvailable values are : [{2}]",
                                profiling_level,
                                next_profiling::kProfilingLevel.at(next_profiling::getProfilingLevel()),
                                available_value_str);
        next::appsupport::ConfigClient::getConfig()->put(
            profiling_level_key, next_profiling::kProfilingLevel.at(next_profiling::getProfilingLevel()));
        return false;
      }
    }
    next::sdk::logger::debug(__FILE__, "Profiling level is now set to {}", upper_case_profiling_level);
    return true;
  } catch (const std::exception &e) {
    next::sdk::logger::warn(__FILE__, e.what());
    return false;
  }
}

bool updateLoggerTypeLevel(const std::string &key, const std::string &log_type_level_key) {

  if (key != log_type_level_key && key != "full_cfg")
    return true;

  std::string logger_level = next::appsupport::ConfigClient::getConfig()->get_string(log_type_level_key, "");

  std::string upper_case_logger_level = boost::to_upper_copy<std::string>(logger_level);
  std::string log_level = "";

  if (log_type_level_key.find("console") != std::string::npos) {
    log_level = next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelConsole());
  } else if (log_type_level_key.find("file") != std::string::npos) {
    log_level = next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelFile());
  } else if (log_type_level_key.find("router") != std::string::npos) {
    log_level = next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelLogRouter());
  }

  if (log_level == "")
    return false;

  try {
    if (upper_case_logger_level != log_level) {
      std::string available_value_str = "";
      auto level_it = next::sdk::logger::kLogLevel.begin();
      for (; level_it != next::sdk::logger::kLogLevel.end(); level_it++) {
        if (level_it->second == upper_case_logger_level) {
          if (log_type_level_key.find("console") != std::string::npos) {
            next::sdk::logger::setLoglevelConsole(level_it->first);
          } else if (log_type_level_key.find("file") != std::string::npos) {
            next::sdk::logger::setLoglevelFile(level_it->first);
          } else if (log_type_level_key.find("router") != std::string::npos) {
            next::sdk::logger::setLoglevelLogRouter(level_it->first);
          }
          break;
        }
        available_value_str += " \"" + level_it->second + "\",";
      }
      if (level_it == next::sdk::logger::kLogLevel.end()) {
        available_value_str[available_value_str.length() - 1] = ' ';
        if (log_type_level_key.find("console") != std::string::npos) {
          next::sdk::logger::error(
              "Logger Console level: {0} is not a valid value.\n\tCurrent Logger Console level is : "
              "{1}.\n\tAvailable values are : [{2}]",
              logger_level, next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelConsole()),
              available_value_str);
          next::appsupport::ConfigClient::getConfig()->put(
              log_type_level_key, next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelConsole()));
        } else if (log_type_level_key.find("file") != std::string::npos) {
          next::sdk::logger::error(
              "Logger Console level: {0} is not a valid value.\n\tCurrent Logger Console level is : "
              "{1}.\n\tAvailable values are : [{2}]",
              logger_level, next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelFile()), available_value_str);
          next::appsupport::ConfigClient::getConfig()->put(
              log_type_level_key, next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelFile()));
        } else if (log_type_level_key.find("router") != std::string::npos) {
          next::sdk::logger::error(
              "Logger Console level: {0} is not a valid value.\n\tCurrent Logger Console level is : "
              "{1}.\n\tAvailable values are : [{2}]",
              logger_level, next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelLogRouter()),
              available_value_str);
          next::appsupport::ConfigClient::getConfig()->put(
              log_type_level_key, next::sdk::logger::kLogLevel.at(next::sdk::logger::getLogLevelLogRouter()));
        }
        return false;
      }
    }
    next::sdk::logger::debug(__FILE__, "Logger Console level is now set to {}", upper_case_logger_level);
    return true;
  } catch (const std::exception &e) {
    next::sdk::logger::warn(__FILE__, e.what());
    return false;
  }
}

} // namespace appsupport
} // namespace next
