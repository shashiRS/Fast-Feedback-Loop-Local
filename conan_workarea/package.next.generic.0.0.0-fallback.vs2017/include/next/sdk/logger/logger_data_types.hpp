/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file  logger_data_types.hpp
 * @brief data structures used by logger.h
 */

#ifndef NEXTSDK_LOGGER_DATA_TYPES_H_
#define NEXTSDK_LOGGER_DATA_TYPES_H_

#include <unordered_map>

namespace next {
namespace sdk {
namespace logger {

/**
 * @brief enum for different available loglevel
 *
 */
enum class LOGLEVEL {
  DEBUG,
  INFO,
  WARN,
  ERR,
  OFF,
};

const std::unordered_map<LOGLEVEL, std::string> kLogLevel = {{LOGLEVEL::DEBUG, "DEBUG"},
                                                             {LOGLEVEL::INFO, "INFO"},
                                                             {LOGLEVEL::WARN, "WARN"},
                                                             {LOGLEVEL::ERR, "ERR"},
                                                             {LOGLEVEL::OFF, "OFF"}};

const std::unordered_map<LOGLEVEL, std::string> kLogLevelLowerCase = {{LOGLEVEL::DEBUG, "debug"},
                                                                      {LOGLEVEL::INFO, "info"},
                                                                      {LOGLEVEL::WARN, "warn"},
                                                                      {LOGLEVEL::ERR, "error"},
                                                                      {LOGLEVEL::OFF, "off"}};

const std::unordered_map<std::string, LOGLEVEL> kLogLevelStrToEnum = {{"DEBUG", LOGLEVEL::DEBUG},
                                                                      {"INFO", LOGLEVEL::INFO},
                                                                      {"WARN", LOGLEVEL::WARN},
                                                                      {"ERR", LOGLEVEL::ERR},
                                                                      {"OFF", LOGLEVEL::OFF}};

} // namespace logger
} // namespace sdk
} // namespace next

#endif // NEXTSDK_LOGGER_DATA_TYPES_H_
