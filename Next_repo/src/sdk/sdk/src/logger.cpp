/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     logger.cpp
 * @brief    wrapper for spdlog lib
 */

#include <next/sdk/logger/logger.hpp>

#include <string>

#include <boost/dll.hpp>
#include <boost/log/attributes/current_process_name.hpp>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <mts/sentinel/sentinel.hpp>
#include <next/sdk/profiler/profile_tags.hpp>
#include "log_router.hpp"
#include "next/sdk/ecal/ecal_singleton.hpp"
#include "next/sdk/version_manager/version_manager.hpp"

static constexpr const char *kLoggerFilenameConfigKey = "logger-filename";
static constexpr const char *kLoggerRouterDisableConfigKey = "logger-router-disable";
static constexpr const char *SENTINEL_PRODUCT_CFG_KEY = "sentinel-product-name";
static constexpr const char *kSentinelDisableConfigKey = "sentinel-disabled";

namespace next {
namespace sdk {
namespace logger {

std::shared_ptr<spdlog::logger> nextsdk_logger{spdlog::default_logger()};

const char *getFilenameCfgKey() { return kLoggerFilenameConfigKey; }

const char *getSentinelProductNameCfgKey() { return SENTINEL_PRODUCT_CFG_KEY; }

const char *getSentinelDisableCfgKey() { return kSentinelDisableConfigKey; }

const char *getLogRouterDisableCfgKey() { return kLoggerRouterDisableConfigKey; }

next::sdk::logger::LOGLEVEL getLogLevelFromSpd(spdlog::level::level_enum spd_loglevel) {
  switch (spd_loglevel) {
  case spdlog::level::debug:
    return next::sdk::logger::LOGLEVEL::DEBUG;
  case spdlog::level::info:
    return next::sdk::logger::LOGLEVEL::INFO;
  case spdlog::level::warn:
    return next::sdk::logger::LOGLEVEL::WARN;
  case spdlog::level::err:
    return next::sdk::logger::LOGLEVEL::ERR;
  case spdlog::level::off:
    return next::sdk::logger::LOGLEVEL::OFF;
  default:
    return next::sdk::logger::LOGLEVEL::OFF;
  }
}

constexpr spdlog::level::level_enum get_spd_loglevel(LOGLEVEL loglevel) {
  switch (loglevel) {
  case LOGLEVEL::DEBUG:
    return spdlog::level::debug;
  case LOGLEVEL::INFO:
    return spdlog::level::info;
  case LOGLEVEL::WARN:
    return spdlog::level::warn;
  case LOGLEVEL::ERR:
    return spdlog::level::err;
  case LOGLEVEL::OFF:
    return spdlog::level::off;
  default:
    return spdlog::level::debug;
  }
}

LOGLEVEL getLogLevelFromString(const std::string &log_level_string) {
  if (log_level_string == "off" || log_level_string == "OFF") {
    return LOGLEVEL::OFF;
  }
  if (log_level_string == "err" || log_level_string == "ERR") {
    return LOGLEVEL::ERR;
  }
  if (log_level_string == "warn" || log_level_string == "WARN") {
    return LOGLEVEL::WARN;
  }
  if (log_level_string == "info" || log_level_string == "INFO") {
    return LOGLEVEL::INFO;
  }
  return LOGLEVEL::DEBUG;
}

static bool init_already_done = false;
static bool init_log_router_already_done = false;
static bool mts_sentinel_init_already_done = false;
std::unique_ptr<mts::sentinel::sentinel> sentinel_ptr{};
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink = nullptr;
std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> file_sink = nullptr;
std::shared_ptr<next::sdk::logger::LogRouter> log_router = nullptr;

static LOGLEVEL loglevel_file = LOGLEVEL::DEBUG;
static LOGLEVEL loglevel_console = LOGLEVEL::DEBUG;
static LOGLEVEL loglevel_router = LOGLEVEL::DEBUG;
static std::string filename = "logfile.txt";

bool init_log_router(const InitChain::ConfigMap &config_map, int) {
  auto ret = config_map.find(getLogRouterDisableCfgKey());

  bool disable_logrouter = true;
  // set logrouter active as default
  if (ret == config_map.end()) {
    disable_logrouter = false;
  } else {
    disable_logrouter = std::any_cast<bool>(ret->second);
  }

  if (disable_logrouter) {
    return true;
  }

  if (init_log_router_already_done)
    return true;

  if (nextsdk_logger) {
    init_log_router_already_done = true;
    auto key = config_map.find(next::sdk::EcalSingleton::getInstancenameCfgKey());
    auto name = boost::log::attributes::current_process_name();
    std::string component_name = name.get();
    if (key != config_map.end()) {
      if (key->second.type() == typeid(std::string)) {
        // const std::string is provided as std::string, no special case needed
        component_name = std::any_cast<std::string>(key->second);
      } else if (key->second.type() == typeid(char *) || key->second.type() == typeid(const char *)) {
        // treating a char* as const char* is fine
        component_name = std::any_cast<const char *>(key->second);
      } else {
        return false;
      }
    }
    log_router = std::make_shared<next::sdk::logger::LogRouter>(component_name);
    log_router->set_level(get_spd_loglevel(loglevel_router));
    log_router->set_pattern("[%C-%m-%d][%H:%M:%S.%e][thread %t][%s][%l] %v");
    nextsdk_logger->sinks().push_back(log_router);
  }
  return true;
}

void SetupDefaultLogLevels() {
#ifdef _DEBUG
  auto logger_level = logger::LOGLEVEL::DEBUG;
#else
  auto logger_level = LOGLEVEL::INFO;
#endif
  loglevel_console = logger_level;
  loglevel_file = logger_level;
  loglevel_router = logger_level;
}
bool ReadFileName(const InitChain::ConfigMap &map) {
  auto cit = map.find(kLoggerFilenameConfigKey);
  if (map.end() != cit) {
    if (cit->second.type() == typeid(std::string)) {
      // const std::string is provided as std::string, no special case needed
      filename = std::any_cast<std::string>(cit->second);
    } else if (cit->second.type() == typeid(char *) || cit->second.type() == typeid(const char *)) {
      // treating a char* as const char* is fine
      filename = std::any_cast<const char *>(cit->second);
    } else {
      return false;
    }
  }
  return true;
}

void SetupSpdlogSinks() {
  std::vector<spdlog::sink_ptr> sinks;
  console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  setLoglevelConsole(loglevel_console);
  // set pattern with color
  console_sink->set_pattern("%^[%C-%m-%d][%H:%M:%S.%e][thread %t][%s][%l]%$ %v");
  sinks.push_back(console_sink);

  if (LOGLEVEL::OFF != loglevel_file) {
    file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 10 * 1024 * 1024, 10, false);
    setLoglevelFile(loglevel_file);
    file_sink->set_pattern("[%C-%m-%d][%H:%M:%S.%e][thread %t][%s][%l] %v");
    sinks.push_back(file_sink);
  }

  // set multi sink logger as default
  nextsdk_logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
  spdlog::set_default_logger(nextsdk_logger);
  spdlog::set_level(spdlog::level::debug);

  // flush messages every second to logfile
  spdlog::flush_every(std::chrono::seconds(1));
  nextsdk_logger->flush_on(spdlog::level::warn);
}

void SetUpMtsSentinal(const InitChain::ConfigMap &map) {
  if (map.find(kSentinelDisableConfigKey) != map.end()) {
    debug(__FILE__, "MTS-Sentinel will not be initialized (requested by configuration)");
  } else {
    try {
      auto cit = map.find(SENTINEL_PRODUCT_CFG_KEY);
      std::string productName = "";
      if (map.end() != cit) {
        if (cit->second.type() == typeid(std::string)) {
          // const std::string is provided as std::string, no special case needed
          productName = std::any_cast<std::string>(cit->second);
        } else if (cit->second.type() == typeid(char *) || cit->second.type() == typeid(const char *)) {
          // treating a char* as const char* is fine
          productName = std::any_cast<const char *>(cit->second);
        }
      }
      std::string crash_report_handler_application_path = boost::dll::program_location().parent_path().string();
#ifdef _WIN32
      crash_report_handler_application_path.append("/mtssentinel.exe");
#else
      crash_report_handler_application_path.append("/mtssentinel");
#endif
      // Setup Sentinel options
      mts::sentinel::sentinel_options options;
      options.set_report_handler(crash_report_handler_application_path)
          .set_database_path(".sentinel/reports")       // Reports relative to the location of this application
          .set_symbol_cache(".sentinel/symbol-cache")   // Symbol cache relative to the location of this application
          .enable_symbolization()                       // Enable crash symbolization, defaults to `true`
          .enable_gather_indirectly_referenced_memory() // Enable capturing indirectly referenced memory in the
                                                        // minidump, defaults to `true`
          .add_annotation("product", productName);
      // Create the Sentinel instance
      sentinel_ptr = std::make_unique<mts::sentinel::sentinel>();
      // Initialize the Sentinel feature
      sentinel_ptr->init(std::move(options));

      debug(__FILE__, "MTS-Sentinel initialized");
      mts_sentinel_init_already_done = true;
    } catch (const std::exception &error) {
      warn(__FILE__, "MTS-Sentinel : Sentinel crash reporting will not be available: {0}", error.what());
    }
  }
}

bool init(const InitChain::ConfigMap &map, int) {
  ProfileCore_OV;
  if (false == init_already_done) {
    if (ReadFileName(map) != true) {
      return false;
    }
    SetupDefaultLogLevels();
    SetupSpdlogSinks();
    debug(__FILE__, ">>> Logger initialized <<< ");
    init_already_done = true;
  } else {
    debug(__FILE__, "Logger already initialized");
  }

  if (false == mts_sentinel_init_already_done) {
    SetUpMtsSentinal(map);
  } else {
    debug(__FILE__, "mts-sentinel already initialized");
  }
  return true;
}

bool setLoglevelLogRouter(LOGLEVEL level) {
  loglevel_router = level;
  if (nullptr != log_router) {
    log_router->set_level(get_spd_loglevel(level));
    return true;
  }
  return false;
}

void setLoglevelFile(LOGLEVEL level) {
  loglevel_file = level;
  if (nullptr != file_sink) {
    file_sink->set_level(get_spd_loglevel(level));
  }
}

void setLoglevelConsole(LOGLEVEL level) {
  loglevel_console = level;
  if (nullptr != console_sink) {
    console_sink->set_level(get_spd_loglevel(level));
  }
}

LOGLEVEL DECLSPEC_nextsdk getLogLevelFile() { return loglevel_file; }

LOGLEVEL DECLSPEC_nextsdk getLogLevelConsole() { return loglevel_console; }

LOGLEVEL DECLSPEC_nextsdk getLogLevelLogRouter() { return loglevel_router; }

void close(const InitChain::ConfigMap &, int) {
  if (nullptr != console_sink) {
    console_sink->flush();
    console_sink.reset();
  }

  if (nullptr != file_sink) {
    file_sink->flush();
    file_sink.reset();
  }

  // todo : we need to close the logger, its an open point
  // nextsdk_logger.reset();
  // spdlog::default_logger()->sinks().clear();
  auto size = spdlog::default_logger()->sinks().size();
  logger::debug(__FILE__, "the size of the logger sinks is ", size);
  init_already_done = false;
}

void close_log_router(const InitChain::ConfigMap &, int) {
  init_log_router_already_done = false;
  if (log_router) {
    if (nextsdk_logger) {
      auto &vec_loggers = nextsdk_logger->sinks();
      if (auto res = std::find(vec_loggers.begin(), vec_loggers.end(), log_router); res != vec_loggers.end()) {
        vec_loggers.erase(res);
      }
    }
    log_router.reset();
    log_router = nullptr;
  }
}

bool isInitialized() { return init_already_done; }

void register_to_init_chain() {
  static InitChain::El init_log_router_var(InitPriority::LOG_ROUTER, init_log_router, close_log_router);
  static InitChain::El init_el(InitPriority::LOGGER, init, close);
}

} // namespace logger
} // namespace sdk
} // namespace next
