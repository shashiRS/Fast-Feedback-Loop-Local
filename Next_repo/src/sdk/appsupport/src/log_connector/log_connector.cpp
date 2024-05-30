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

#include <iostream>
#include <next/appsupport/appsupport_config.hpp>
#include <next/appsupport/config/config_change_hook_collection.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/config/config_server.hpp>
#include <next/appsupport/log_connector/log_connector.hpp>
#include <next/sdk/sdk.hpp>

void LogConnector::connectLoggerToArgs(const std::string comp_name, next::appsupport::CmdOptions &command_options) {
  command_options.addParameterString("log_level_console,lc", "log level console",
                                     next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "console"));
  command_options.addParameterString("log_level_router,lr", "log level logrouter",
                                     next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "router"));
  command_options.addParameterString("log_level_file,lf", "log level file",
                                     next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "file"));
}

void LogConnector::connectLoggerToSystem(const std::string &comp_name) {

  if (next::appsupport::ConfigClient::getConfig() == nullptr) {
    return;
  }
  readLogLevelFromConfig(comp_name, "file");
  readLogLevelFromConfig(comp_name, "console");
  readLogLevelFromConfig(comp_name, "router");

  auto log_file_level_hook = std::bind(&next::appsupport::updateLoggerTypeLevel, std::placeholders::_1,
                                       next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "file"));
  auto log_console_level_hook =
      std::bind(&next::appsupport::updateLoggerTypeLevel, std::placeholders::_1,
                next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "console"));
  auto log_router_hook = std::bind(&next::appsupport::updateLoggerTypeLevel, std::placeholders::_1,
                                   next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, "router"));

  next::appsupport::ConfigClient::getConfig()->addChangeHook(log_file_level_hook);
  next::appsupport::ConfigClient::getConfig()->addChangeHook(log_console_level_hook);
  next::appsupport::ConfigClient::getConfig()->addChangeHook(log_router_hook);
}

void putLogLevelToConfig(const std::string &comp_name, const std::string &sink_type,
                         const next::sdk::logger::LOGLEVEL &loglevel) {
  auto log_level_string = next::sdk::logger::kLogLevel.find(loglevel);
  if (log_level_string != next::sdk::logger::kLogLevel.end()) {
    next::appsupport::ConfigClient::getConfig()->put(
        next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, sink_type), log_level_string->second);
  }
}

next::sdk::logger::LOGLEVEL getLogLevelFromLogger(const std::string &sink_type) {
  next::sdk::logger::LOGLEVEL loglevel = next::sdk::logger::LOGLEVEL::OFF;
  // no file sink set -> default to current setup
  if (sink_type == "file") {
    loglevel = next::sdk::logger::getLogLevelFile();
  }
  if (sink_type == "console") {
    loglevel = next::sdk::logger::getLogLevelConsole();
  }
  if (sink_type == "router") {
    loglevel = next::sdk::logger::getLogLevelLogRouter();
  }
  return loglevel;
}

void setLogLevel(const std::string &sink_type, next::sdk::logger::LOGLEVEL &loglevel) {
  if (sink_type == "file") {
    next::sdk::logger::setLoglevelFile(loglevel);
  }
  if (sink_type == "console") {
    next::sdk::logger::setLoglevelConsole(loglevel);
  }
  if (sink_type == "router") {
    next::sdk::logger::setLoglevelLogRouter(loglevel);
  }
}

void LogConnector::readLogLevelFromConfig(const std::string &comp_name, const std::string &sink_type) {
  try {
    // file log_level in order from component to system to default level
    auto sink_conf_key = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::logger::getSinkLoglevelKey(comp_name, sink_type), "");
    if (sink_conf_key == "") {
      sink_conf_key = next::appsupport::ConfigClient::getConfig()->get_string(
          next::appsupport::configkey::logger::getSystemSinkLoglevelKey(comp_name), "");
    }
    next::sdk::logger::LOGLEVEL loglevel;
    if (sink_conf_key == "") {
      loglevel = getLogLevelFromLogger(sink_type);
    } else {
      loglevel = next::sdk::logger::getLogLevelFromString(sink_conf_key);
      setLogLevel(sink_type, loglevel);
    }
    putLogLevelToConfig(comp_name, sink_type, loglevel);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}