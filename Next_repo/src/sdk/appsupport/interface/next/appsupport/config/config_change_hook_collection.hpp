/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_change_hook_collection.hpp
 * @brief    hook function for configClient
 *
 **/

#ifndef NEXT_SDK_CONFIG_CHANGE_HOOK_COLLECTION_H
#define NEXT_SDK_CONFIG_CHANGE_HOOK_COLLECTION_H

#include <functional>
#include <string>

#include <next/appsupport/appsupport_config.hpp>

namespace next {
namespace appsupport {
/**
 * @brief Change the profiler level according to the value set in the Config
 *
 * @param key The key value which changed
 * @param profiling_level_key The profiling level key in config
 * @return true if a valid value is set in the config
 * @return false if a wrong value is set in the config
 */
bool DECLSPEC_appsupport updateProfilerLevel(std::string key, std::string profiling_level_key);
/**
 * @brief Change the logger level according to the value set in the Config
 *
 * @param key The key value which changed
 * @param log_type_level_key The logger level key in config
 * @return true if a valid value is set in the config
 * @return false if a wrong value is set in the config
 */
bool DECLSPEC_appsupport updateLoggerTypeLevel(const std::string &key, const std::string &log_type_level_key);
} // namespace appsupport
} // namespace next

#endif
