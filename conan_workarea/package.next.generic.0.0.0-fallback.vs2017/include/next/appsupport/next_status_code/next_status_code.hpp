/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_Appsupport
 * ========================== NEXT Project ==================================
 */
/**
 * @file    next_status_code.hpp
 * @brief   NextStatusCode is the class with a static function PublishStatusCode
 **/

#ifndef NEXT_STATUS_CODE_H_
#define NEXT_STATUS_CODE_H_
#include <optional>
#include <string>

#include <next/appsupport/appsupport_config.hpp>
#include <next/sdk/events/status_code_event.hpp>

namespace next {
namespace appsupport {

// enum for status codes inside next framework
// *** STATUS CODE NUMBER***
// the status code consists of 3 digits:  <status_severity><status_source><status_description>
// 1. digit "status_severity": 1 = INFO, 2 = SUCCESSFUL, 3 = WARNING, 4 = ERROR
// 2. digit "status source": grouping of status codes belonging to each other,
//                           e.g. Lifecycle + Components in the range 1- 2 at the 2. digit
// 3. digit "status description": unique number to describe status code of this source
//
// Example: status code 241 means: <succesful><exporter><started>
//
// *** STATUS CODE NAME ***
// status code naming convention: <status_severity>_<status_source>_<status_description>
// status severities are the following strings: 1 = INFO, 2 = SUCC, 3 = WARN, 4 = ERR
// e.g. SUCC_EXPORTER_STARTED
//
// Hints:
// 1. The status code publisher event is intended to send the ResultStatusCode and will add the component type in front
//    of the status code value, e.g. 1241 for exporter started (1 == databridge)
// 2. One status code should only report one specific status and shouldn't be reused at other sources inside one
//    executable/component (e.g. databridge). That means e.g. status codes of the Lifecycle + Components must not used
//    inside the plugins of databridge.
// clang-format off
enum class StatusCode {
  /****** Generic 0-9 ******/

  /******* Lifecycle + Components Range 10-29 *****/
  SUCC_COMP_INIT_FINISHED = 211,
  SUCC_COMP_CONFIG_FINISHED = 212,
  SUCC_COMP_SHUTDOWN_FINISHED = 213,
  SUCC_COMP_RESET_FINISHED = 214,
  SUCC_COMP_CONFIG_OUTPUT_FINISHED = 215,
  SUCC_COMP_CONFIG_INPUT_FINISHED = 216,

  WARN_COMP_EXEC = 312,

  ERR_COMP_INIT_FAILED = 411,
  //ERR_COMP_CONFIG_FAILED = 412,
  //ERR_COMP_SHUTDOWN_FAILED = 413,

  /******** Plugins Range 30-49 ********/
  //ERR_PLUGIN_FAILED = 431,

  /*****  Special Plugin e.g EXPORTER 40 *****/
  SUCC_FULL_SUBSCRIPTION = 241,
  SUCC_EXPORTER_STOPPED = 242,

  WARN_EXPORTER_PARTIAL_SUBSCRIPTION = 341,

  ERR_EXPORTER_TRIGGER_NOT_FOUND = 441,
  ERR_EXPORTER_FILE_FORMAT_NOT_SUPPORTED = 442,
  ERR_EXPORTER_CONFIGURATION_NOT_SUPPORTED = 443,
  ERR_EXPORTER_NO_SUBSCRIPTION = 444,
  /****** Player 50-69 ********/
  SUCC_RECORDING_LOADED = 251,
  ERR_RECORDING_LOADED = 451,

  SUCC_PLAYER_TRIGGER_FINISHED = 252,
  ERR_PLAYER_TRIGGER_FINISHED = 452,
  WARN_PLAYER_TRIGGER_NO_RECEIVER_FOUND = 352,
  WARN_PLAYER_TRIGGER_EXEC_WARN = 353,

  INFO_START_LOAD_RECORDING = 153,

  /***** Bridges 70-89 *******/
  SUCC_SETUP_BACKEND_CONFIG_FINISHED = 271,
  SUCC_PROCESS_QUEUE_FINISHED = 272,
  SUCC_RECEIVED_CHECK_WEBSOCKET_CONNECTION = 273,

  WARN_PLUGIN_NOT_FOUND = 371,

  /******* Reserved 90-99  *****/
  //ERR_MODULE_NOT_FOUND = 490,
  //ERR_CRASH = 491
};
// clang-format on

class DECLSPEC_appsupport NextStatusCode {
public:
  static std::optional<next::sdk::events::StatusCodeMessage>
  PublishStatusCode(next::appsupport::StatusCode next_status_code, const std::string &status_code_msg);

  static std::optional<next::sdk::events::StatusCodeMessage>
  PublishStatusCode(std::string component_name, next::appsupport::StatusCode next_status_code,
                    const std::string &status_code_msg);

private:
  static std::string GetComponentNameFromConfig(const std::string &default_component_name);
  static sdk::events::StatusOrigin GetOrigin(const std::string &component_name,
                                             const sdk::events::StatusOrigin &default_origin);

  static const std::optional<next::sdk::events::StatusCodeMessage>
  SendStatusCode(const next::sdk::events::StatusOrigin &origin, const std::string &component_name,
                 const next::appsupport::StatusCode &next_status_code, const std::string &status_code_msg);
};
} // namespace appsupport
} // namespace next

#endif // NEXT_STATUS_CODE_H_
