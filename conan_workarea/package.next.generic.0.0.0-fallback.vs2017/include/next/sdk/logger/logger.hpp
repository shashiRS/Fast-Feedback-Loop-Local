/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     logger.hpp
 * @brief    wrapper for spdlog lib
 */

#ifndef NEXTSDK_LOGGER_H_
#define NEXTSDK_LOGGER_H_

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <memory>

#include "logger_data_types.hpp"

#include <spdlog/spdlog.h>

#include "../init_chain/simple_init_chain.hpp"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace logger {

extern DECLSPEC_nextsdk std::shared_ptr<spdlog::logger> nextsdk_logger;

/**
 * @brief initialize the logger
 *
 */
bool DECLSPEC_nextsdk init(const InitChain::ConfigMap &configMap, int = 0);

/**
 * @brief initialize the log_router, which extends logger sink
 * and uses ecal for publishing logs across the Next
 */
bool DECLSPEC_nextsdk init_log_router(const InitChain::ConfigMap &, int = 0);

/**
 * @brief close the logger
 *
 */
void DECLSPEC_nextsdk close(const InitChain::ConfigMap & = InitChain::ConfigMap(), int = 0);

/**
 * @brief close the log router
 *
 */
void DECLSPEC_nextsdk close_log_router(const InitChain::ConfigMap &, int = 0);

/**
 * @brief return status of initialization
 *
 */
bool DECLSPEC_nextsdk isInitialized();

/**
 * @brief register the logger for initialization chain
 */
void DECLSPEC_nextsdk register_to_init_chain();

/**
 * @brief get the config key for the filename for the init chain
 *
 * As values [const] char* and [const] std::string are allowed. Otherwise the init function will fail.
 */
const char DECLSPEC_nextsdk *getFilenameCfgKey();

/**
 * @brief get the config key for the mts sentinel product name for the init chain
 *
 * As values [const] char* and [const] std::string are allowed.
 */
const char DECLSPEC_nextsdk *getSentinelProductNameCfgKey();

/**
 * @brief get the config key for disabling sentinel
 *
 * The value does not matter, as soon as the key is present sentinel will not be enabled.
 */
const char DECLSPEC_nextsdk *getSentinelDisableCfgKey();

/**
 * @brief get the config key for disabling the log router
 *
 * As values, true means logger is disabled and false is enabled.
 */
const char DECLSPEC_nextsdk *getLogRouterDisableCfgKey();

/**
 * @brief Set the log level For logrouter sink
 *
 * @param level e.g. @LOGLEVEL::DEBUG
 * @return true if set successfully otherwise false
 */
bool DECLSPEC_nextsdk setLoglevelLogRouter(LOGLEVEL level);

/**
 * @brief Set the log level For logfile sink
 *
 * @param level e.g. @LOGLEVEL::DEBUG
 */
void DECLSPEC_nextsdk setLoglevelFile(LOGLEVEL level);

/**
 * @brief Set the log level For console sink
 *
 * @param level e.g. @LOGLEVEL::DEBUG
 */
void DECLSPEC_nextsdk setLoglevelConsole(LOGLEVEL level);

/**
 * @brief get the log level For logfile sink
 *
 */
LOGLEVEL DECLSPEC_nextsdk getLogLevelFile();

/**
 * @brief get the log level For console sink
 *
 */
LOGLEVEL DECLSPEC_nextsdk getLogLevelConsole();

/**
 * @brief get the log level For LogRouter sink
 *
 */
LOGLEVEL DECLSPEC_nextsdk getLogLevelLogRouter();

/**
 * @brief logs a debug message to all sinks at once
 * for formatting see here: https://fmt.dev/latest/index.html
 *
 * @param fmt the message string
 * @param args parameters to format
 */
template <typename... Args>
inline void debug(const std::string &module_name, char const *fmt, Args const &...args) {
  nextsdk_logger->log(spdlog::source_loc{module_name.c_str(), __LINE__, SPDLOG_FUNCTION},
                      spdlog::level::level_enum::debug, fmt, args...);
}

/**
 * \overload
 */
template <typename... Args>
inline void debug(const std::string &module_name, std::string const &fmt, Args const &...args) {
  debug(module_name, fmt.c_str(), args...);
}

/**
 * @brief logs an info message to all sinks at once
 * for formating see here: https://fmt.dev/latest/index.html
 *
 * @param fmt the message string
 * @param args paramters to format
 */
template <typename... Args>
inline void info(const std::string &module_name, char const *fmt, Args const &...args) {
  nextsdk_logger->log(spdlog::source_loc{module_name.c_str(), __LINE__, SPDLOG_FUNCTION},
                      spdlog::level::level_enum::info, fmt, args...);
}

/**
 * \overload
 */
template <typename... Args>
inline void info(const std::string &module_name, std::string const &fmt, Args const &...args) {
  info(module_name, fmt.c_str(), args...);
}

/**
 * @brief logs a warning message to all sinks at once
 * for formating see here: https://fmt.dev/latest/index.html
 *
 * @param fmt the message string
 * @param args paramters to format
 */
template <typename... Args>
inline void warn(const std::string &module_name, char const *fmt, Args const &...args) {
  nextsdk_logger->log(spdlog::source_loc{module_name.c_str(), __LINE__, SPDLOG_FUNCTION},
                      spdlog::level::level_enum::warn, fmt, args...);
}

/**
 * \overload
 */
template <typename... Args>
inline void warn(const std::string &module_name, std::string const &fmt, Args const &...args) {
  warn(module_name, fmt.c_str(), args...);
}
/**
 * @brief logs an error message to all sinks at once
 * for formating see here: https://fmt.dev/latest/index.html
 *
 * @param fmt the message string
 * @param args paramters to format
 */
template <typename... Args>
inline void error(const std::string &module_name, char const *fmt, Args const &...args) {
  nextsdk_logger->log(spdlog::source_loc{module_name.c_str(), __LINE__, SPDLOG_FUNCTION},
                      spdlog::level::level_enum::err, fmt, args...);
}

/**
 * \overload
 */
template <typename... Args>
inline void error(const std::string &module_name, std::string const &fmt, Args const &...args) {
  error(module_name, fmt.c_str(), args...);
}

/**
 * @brief get_loglevel_from_spd function converts spd log_level to next::sdk::loglevel
 *
 * @param spd_loglevel the log level to map
 * @return the mapped log level
 */
next::sdk::logger::LOGLEVEL DECLSPEC_nextsdk getLogLevelFromSpd(spdlog::level::level_enum spd_loglevel);

/**
 * @brief getLogLevelFromString function converts string to spdlog loglevel
 *
 * @param string to map
 * @return the mapped log level
 */
LOGLEVEL DECLSPEC_nextsdk getLogLevelFromString(const std::string &log_level_string);

} // namespace logger
} // namespace sdk
} // namespace next

#endif // NEXTSDK_LOGGER_H_

#ifndef NO_USE_LOGGER_NAMESPACE
using next::sdk::logger::debug;
using next::sdk::logger::error;
using next::sdk::logger::info;
using next::sdk::logger::warn;
#endif
