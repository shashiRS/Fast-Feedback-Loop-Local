/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     main.cpp
 * @brief
 **/

#include <string>
#include <thread>

#include <next/sdk/events/lifecycle_trigger.h>
#include <next/sdk/sdk.hpp>

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_change_hook_collection.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/log_connector/log_connector.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>

#include <next/control/log_collector/log_collector.hpp>

#include <next/bridgesdk/web_server.hpp>

#include "../LifeCycleOrchestrator/lifecycle_orchestrator.h"
#include "CommandHandler/player_control_manager.h"
#include "ConfigManager/config_manager.h"
#include "EventHandler/version_info_event_distributor.h"
#include "TelemetryHandler/telemetry_handler.h"
#include "bin_version.h"
#include "log_distributor/log_distributor.hpp"
#include "status_code_manager/status_code_manager.hpp"

using next::sdk::InitChain;
namespace logger = next::sdk::logger;
/**
 * @brief    Starting function for the dummy code.
 * @param[in]     argc    Number of arguments (filled by operating system).
 * @param[in]     argv    String of cmdline (filled by operating system).
 */
int main(int argc, const char *argv[]) {
  // ------------------------ INIT START
#ifdef _DEBUG
  auto profiling_level = next::sdk::profiler::ProfilingLevel::DETAIL;
#else
  auto profiling_level = next::sdk::profiler::ProfilingLevel::OFF;
#endif
  auto comp_name = next::sdk::version_manager::getControlbridgeName();

  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next::sdk::profiler::register_to_init_chain();
  next::sdk::version_manager::register_to_init_chain();

  size_t current_connection_count{0u};
  InitChain::ConfigMap initmap;
  bool init_finished_published{false};

  std::string version_string = next::sdk::version_manager::MakeVersionString(VERSIONINFO_PROJECTVERSION_STR);
  std::string doc_link = next::sdk::version_manager::CreateDocumentationLink(
      next::sdk::version_manager::ComponentName::NEXT_CONTROL_BRIDGE);
  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(), comp_name);
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), doc_link);
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), version_string);
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), comp_name);
  initmap.emplace(logger::getFilenameCfgKey(), comp_name + ".log");
  initmap.emplace(logger::getSentinelProductNameCfgKey(), comp_name);
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), profiling_level);

  if (!next::sdk::InitChain::Run(initmap)) {
    next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                        "Failed to initialize the ControlBridge");
    return 1;
  }

  info(__FILE__, "Controlbridge start up initialized");
  // make sure that eCAL is initalized by faking using it
  next::sdk::EcalSingleton::get_instance();

  std::shared_ptr<next::appsupport::ConfigServer> config_server =
      std::make_shared<next::appsupport::ConfigServer>(comp_name + "_server");
  next::appsupport::ConfigClient::do_init(comp_name);
  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getMajorVersionKey(comp_name),
                                                   VERSIONINFO_MAJOR);
  // TODO: comment in set tags after implementation in config class
  /*next::appsupport::ConfigClient::getConfig()->setTag(next::appsupport::configkey::getMajorVersionKey(comp_name),
                                                      next::appsupport::configtags::TAG::READONLY);*/
  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getMinorVersionKey(comp_name),
                                                   VERSIONINFO_MINOR);
  /*next::appsupport::ConfigClient::getConfig()->setTag(next::appsupport::configkey::getMinorVersionKey(comp_name),
                                                      next::appsupport::configtags::TAG::READONLY);*/
  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getPatchVersionKey(comp_name),
                                                   VERSIONINFO_PATCH);
  /*next::appsupport::ConfigClient::getConfig()->setTag(next::appsupport::configkey::getPatchVersionKey(comp_name),
                                                      next::appsupport::configtags::TAG::READONLY);*/

  try {
    next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getProfilingLevelKey(comp_name),
                                                     next::sdk::profiler::kProfilingLevel.at(profiling_level));
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  auto profiling_level_hook = std::bind(&next::appsupport::updateProfilerLevel, std::placeholders::_1,
                                        next::appsupport::configkey::getProfilingLevelKey(comp_name));
  next::appsupport::ConfigClient::getConfig()->addChangeHook(profiling_level_hook);

  auto cmd_options = next::appsupport::CmdOptions(comp_name); // c option but single
  LogConnector::connectLoggerToArgs(comp_name, cmd_options);

  cmd_options.addParameterString("websocket_ip,wi", "Configure IP of websocket",
                                 next::appsupport::configkey::getControlIpKey(comp_name));
  cmd_options.addParameterString("websocket_port,wp", "Configure port of websocket",
                                 next::appsupport::configkey::getControlPortKey(comp_name));
  cmd_options.addParameterString("global_config,gc", "Configuration server global config",
                                 next::appsupport::configkey::getGlobalConfigurationKey(comp_name));
  bool ret = cmd_options.parseCmd(argc, argv);
  if (!ret) {
    next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                        "Init failed. Parsing of command line arguments unsuccessful.");
    return ret;
  }
  next::appsupport::ConfigClient::finish_init();
  LogConnector::connectLoggerToSystem(comp_name);

  // TODO: use life cycle management instead of initchain

  next::sdk::shutdown_handler::register_shutdown_handler();

  next::controlbridge::LogDistributor log_distributor_to_gui(next::sdk::logger::LOGLEVEL::ERR,
                                                             std::chrono::milliseconds(1000));

  auto binded_callback =
      std::bind(&next::controlbridge::LogDistributor::SendLogEventData, &log_distributor_to_gui, std::placeholders::_1);
  next::control::log_collector::LogCollector combined_log("Combined_log", comp_name, binded_callback);
  combined_log.Setup();

  next::controlbridge::StatusCodeManager status_code_manager(comp_name);
  status_code_manager.Setup();

  // ------------------------ INIT END
#ifdef DELIVERY_RELEASE
#else
  auto telemetry = next::controlbridge::telmetry::TelemetryHandler(comp_name, version_string);
#endif
  auto handler = next::controlbridge::command_handler::PlayerControlManager();

  auto control_ip = next::appsupport::ConfigClient::getConfig()->get_string(
      next::appsupport::configkey::getControlIpKey(comp_name), "127.0.0.1");
  uint16_t control_port{0u};
  if (const int tmp = next::appsupport::ConfigClient::getConfig()->get_int(
          next::appsupport::configkey::getControlPortKey(comp_name), 8200),
      min = int(std::numeric_limits<uint16_t>::min()), max = int(std::numeric_limits<uint16_t>::max());
      min < tmp && tmp < max) {
    control_port = static_cast<uint16_t>(tmp);
  } else {
    std::string warningMessage = fmt::format(
        "Init failed. Invalid control port given. Given port {} is not in range of {} to {}.", tmp, min, max);
    next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                        warningMessage);
    return 1;
  }

  auto webserver = next::bridgesdk::webserver::CreateWebServer();
  webserver->SetIpAddress(control_ip);
  webserver->SetPortNumber(control_port);
  webserver->SetNumOfConnections(1);

  try {
    webserver->ActivateListingToSocket();
  } catch (std::exception &e) {
    error(__FILE__, " Error in setting up Webserver");
    throw e;
  }

  handler.StartUp(webserver, config_server, "ControlBridgeTriggering", "blockPlayer");

  log_distributor_to_gui.SetWebserver(webserver);
  debug(__FILE__, " Log distributor is set up to send logs to GUI");
  status_code_manager.SetWebserver(webserver);

  next::bridges::ConfigManager conf_manager(webserver, config_server,
                                            next::appsupport::ConfigClient::getConfig()->get_string(
                                                next::appsupport::configkey::getGlobalConfigurationKey(comp_name), ""));
  next::controlbridge::lifecycle_orchestrator::LifecycleOrchestrator::GetInstance()->addOnResetHook(
      [&]() { conf_manager.pushDataToNodes(); });
  next::controlbridge::lifecycle_orchestrator::LifecycleOrchestrator::start();

  next::controlbridge::event_handler::VersionEventDistributor version_event_distributor(webserver);

  version_event_distributor.startVersionThread();
  while (!next::sdk::shutdown_handler::shutdown_requested()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if ((!init_finished_published) && (webserver != nullptr)) {
      next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_COMP_INIT_FINISHED,
                                                          "[Controlbridge] Init finished");
      init_finished_published = true;
    }

    if ((webserver != nullptr) && (current_connection_count != (webserver->GetNumOfActiveConnections()))) {
      debug(__FILE__, " Run Meta Information Distribution | number of active connections: {0}",
            webserver->GetNumOfActiveConnections());
      version_event_distributor.requestVersions();
      current_connection_count = webserver->GetNumOfActiveConnections();
    }
  }
  handler.ShutDown();
  info(__FILE__, " Handler shutdown");
  InitChain::Reset();

  next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_COMP_SHUTDOWN_FINISHED,
                                                      "Shutdown finished");
  return 0;
}
