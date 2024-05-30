/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     main.cpp
 *  @brief    main.cpp of the data bridge itself
 */

#include <iostream>
#include <iterator>
#include <mutex>
#include <string>
#include <thread>

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_change_hook_collection.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/log_connector/log_connector.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/appsupport/session_handler/session_client.hpp>
#include <next/sdk/sdk.hpp>
#include <next/sdk/version_manager/version_manager.hpp>

#include <map>
#include <next/bridgesdk/data_manager.hpp>
#include "bin_version.h"
#include "data_class_manager.h"
#include "plugin_manager.h"

using next::sdk::InitChain;
namespace logger = next::sdk::logger;

/**
 * @brief    Starting function for the dummy code.
 * @param[in]     argc    Number of arguments (filled by operating system).
 * @param[in]     argv    String of cmdline (filled by operating system).
 */
int main(int argc, const char *argv[]) {

#ifdef _DEBUG
  auto profiling_level = next::sdk::profiler::ProfilingLevel::DETAIL;
#else
  auto profiling_level = next::sdk::profiler::ProfilingLevel::OFF;
#endif

  auto comp_name = next::sdk::version_manager::getDatabridgeName();

  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next::sdk::profiler::register_to_init_chain();
  next::sdk::version_manager::register_to_init_chain();

  InitChain::ConfigMap initmap;
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), comp_name);
  initmap.emplace(logger::getFilenameCfgKey(), comp_name + ".log");
  initmap.emplace(logger::getSentinelProductNameCfgKey(), comp_name);

  std::string version_string = next::sdk::version_manager::MakeVersionString(VERSIONINFO_PROJECTVERSION_STR);
  std::string doc_link =
      next::sdk::version_manager::CreateDocumentationLink(next::sdk::version_manager::ComponentName::NEXT_DATA_BRIDGE);
  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(), comp_name);
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), doc_link);
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), version_string);
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), profiling_level);

  if (!next::sdk::InitChain::Run(initmap)) {
    next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                        "Failed to initialize the DataBridge");
    return 1;
  }

  next::appsupport::ConfigClient::do_init(next::sdk::version_manager::getDatabridgeName());

  auto cmd_options = next::appsupport::CmdOptions(comp_name);
  LogConnector::connectLoggerToArgs(comp_name, cmd_options);

  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getMajorVersionKey(comp_name),
                                                   VERSIONINFO_MAJOR);
  // TODO comment in set tags after implementation in config class
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

  bool ret = cmd_options.parseCmd(argc, argv);
  if (!ret) {
    next::appsupport::NextStatusCode::PublishStatusCode(
        next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
        "[DataBridge] Init failed. Parsing of command line arguments unsuccessful.");
    return 1;
  }

  next::appsupport::ConfigClient::finish_init();
  LogConnector::connectLoggerToSystem(comp_name);

  // trying to read network setting from config client
  std::string foxglove_ip;
  std::string data_broadcast_ip;
  std::string data_request_ip;
  uint16_t data_broadcast_port{0u};
  uint16_t data_request_port{0u};

  foxglove_ip = next::appsupport::ConfigClient::getConfig()->get_string(
      next::appsupport::configkey::databridge::getFoxgloveIpKey(comp_name), "not_set");
  if (foxglove_ip != "not_set") {
    uint16_t foxglove_port{0u};
    if (const int tmp = next::appsupport::ConfigClient::getConfig()->get_int(
            next::appsupport::configkey::databridge::getFoxglovePortKey(comp_name), 8080),
        min = int(std::numeric_limits<uint16_t>::min()), max = int(std::numeric_limits<uint16_t>::max());
        min < tmp && tmp < max) {
      foxglove_port = static_cast<uint16_t>(tmp);
    } else {
      std::string warningrMessage =
          fmt::format("Invalid foxglove port given. Given port {} is not in range of {} to {}.", tmp, min, max);
      next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                          warningrMessage);
      return 1;
    }
    data_broadcast_ip = foxglove_ip;
    data_broadcast_port = foxglove_port;
  } else {
    data_broadcast_ip = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::databridge::getDataBroadcastIpKey(comp_name), "127.0.0.1");

    data_request_ip = next::appsupport::ConfigClient::getConfig()->get_string(
        next::appsupport::configkey::databridge::getDataRequestIpKey(comp_name), "127.0.0.1");
    if (const int tmp = next::appsupport::ConfigClient::getConfig()->get_int(
            next::appsupport::configkey::databridge::getDataBroadcastPortKey(comp_name), 8080),
        min = int(std::numeric_limits<uint16_t>::min()), max = int(std::numeric_limits<uint16_t>::max());
        min < tmp && tmp < max) {
      data_broadcast_port = static_cast<uint16_t>(tmp);
    } else {
      std::string warningMessage =
          fmt::format("Invalid data broadcast port given. Given port {} is not in range of {} to {}.", tmp, min, max);
      next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                          warningMessage);
      return 1;
    }
    if (const int tmp = next::appsupport::ConfigClient::getConfig()->get_int(
            next::appsupport::configkey::databridge::getDataRequestPortKey(comp_name), 8082),
        min = int(std::numeric_limits<uint16_t>::min()), max = int(std::numeric_limits<uint16_t>::max());
        min < tmp && tmp < max) {
      data_request_port = static_cast<uint16_t>(tmp);
    } else {
      std::string warningMessage =
          fmt::format("Invalid data request port given. Given port {} is not in range of {} to {}.", tmp, min, max);
      next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                          warningMessage);
      return 1;
    }
  }

  // ------------------------ INIT START

  // TODO: use life cycle management instead of initchain

  next::sdk::shutdown_handler::register_shutdown_handler();
  // make sure that eCAL is initalized by faking using it
  next::sdk::EcalSingleton::get_instance();
  // ------------------------ INIT END

  info(__FILE__, "Hello World! I am the DATA BRIDGE");

  // Start Information Client
  auto session_client = next::appsupport::session::SessionClient(comp_name);

  // create output connection to webserver
  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager =
      next::databridge::data_manager::CreateDataManager();
  output_data_manager->StartWebServer(data_broadcast_ip, data_broadcast_port, 1);

  // create GUI input
  std::shared_ptr<next::databridge::DataClassManager> data_class_manager =
      std::make_shared<next::databridge::DataClassManager>();

  // TODO do not only use the same webserver when it is for foxglove, but also when the IP and port are the same
  if (foxglove_ip != "not_set") {
    data_class_manager->SetWebServer(output_data_manager->GetWebsocketServer());
  } else {
    data_class_manager->InitBackgroundMode(data_request_ip, data_request_port);
  }

  // wait until all background threads are ready
  debug(__FILE__, "Waiting for Webservers to boot up");
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  // create the PluginManager
  debug(__FILE__, "Creating plugin manager");
  const std::string plugins_dir("plugins");
  next::databridge::plugin_manager::PluginManager plugin_manager;
  if (!plugin_manager.init(plugins_dir)) {
    next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_COMP_INIT_FAILED,
                                                        "Initializing the plugin manager failed, exiting");
    return -1;
  }
  plugin_manager.connectToOutputDataManager(output_data_manager);

  debug(__FILE__, "Available plugins:");
  for (const std::string &p : plugin_manager.getAvailablePlugins()) {
    debug(__FILE__, p.c_str());
  }

  next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_COMP_INIT_FINISHED,
                                                      "[DataBridge] Init finished");
  bool shutdown = false;
  while (false == next::sdk::shutdown_handler::shutdown_requested() && false == shutdown) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    using next::databridge::DataClassManager;
    DataClassManager::DataRequestItem dataRequestClass;
    if (data_class_manager->GetDataClassRequestNonBlocking(dataRequestClass)) {
      if (0 == dataRequestClass.name.compare("Shutdown")) {
        debug(__FILE__, "Shutdown request received");
        shutdown = true;
      } else {
        debug(__FILE__, "Request received for {0}", dataRequestClass.name);
        plugin_manager.updatePluginConfigs(dataRequestClass.name, dataRequestClass.version,
                                           dataRequestClass.source_name, dataRequestClass.arguments);
      }
    }

    DataClassManager::BackendConfigRequestItem backend_request;
    if (data_class_manager->GetBackendRequestNonBlocking(backend_request)) {
      bool success = true;
      success = plugin_manager.SetDataPackageDropMode(backend_request.package_drop_enabled);
      if (!success) {
        warn(__FILE__, "Setting PackageDrop Mode for plugins failed");
      }
      success = plugin_manager.SetSynchronizationMode(backend_request.synchronization_enabled);
      if (!success) {
        warn(__FILE__, "Setting Synchronization Mode for plugins failed");
      } else {
        debug(__FILE__, "Setting Synchronization Mode for plugins succeeded");
      }
      next::appsupport::NextStatusCode::PublishStatusCode(
          next::appsupport::StatusCode::SUCC_SETUP_BACKEND_CONFIG_FINISHED,
          "[DataBridge] Setup backend config finished");
    }

    DataClassManager::ConfigDataRequestItem config_request;
    while (data_class_manager->GetConfigDataClassRequestNonBlocking(config_request)) {
      if (DataClassManager::ConfigDataRequestItem::ITEM_TYPE::GET_DESCRIPTION == config_request.item_type) {
        plugin_manager.findAndSendDescription(config_request.name, config_request.version);
      } else if (DataClassManager::ConfigDataRequestItem::ITEM_TYPE::SET_CONFIG == config_request.item_type) {
        plugin_manager.findAndSetConfig(config_request.configs, config_request.plugin_name, config_request.source_name);
      }
    }
  }
  info(__FILE__, "done, destroying DataBridge. Goodbye.");
  plugin_manager.shutdown();
  session_client.StopSessionClient();
  InitChain::Reset();

  next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_COMP_SHUTDOWN_FINISHED,
                                                      "[DataBridge] Shutdown finished");

  return 0;
}
