/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_connector.hpp
 * @brief    LogConnector class exposes static function
 * connectLoggerToSystem which adds hooks to the log functions
 *
 **/

#ifndef LOG_CONNECTOR_H_
#define LOG_CONNECTOR_H_
#include <string>

#include "next/appsupport/cmd_options/cmd_options.hpp"

class DECLSPEC_appsupport LogConnector {
public:
  static void connectLoggerToSystem(const std::string &comp_name);

  static void connectLoggerToArgs(const std::string comp_name, next::appsupport::CmdOptions &command_options);

private:
  static void readLogLevelFromConfig(const std::string &comp_name, const std::string &sink_type);
};
#endif // LOG_CONNECTOR_H_
