/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */

#if defined(_WIN32) && defined(_MSC_VER)
// nothing to do so far
#elif defined(__linux__) && defined(__GNUG__)
#include <sys/resource.h>
#else
#error "not supported OS (either Windows (MSVC) or Linux (GCC))"
#endif

#include <chrono>
#include <memory>
#include <sstream>
#include <thread>

#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/sdk.hpp>
namespace logger = next::sdk::logger;

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_change_hook_collection.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/config/config_server.hpp>
#include <next/appsupport/log_connector/log_connector.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/appsupport/session_handler/session_client.hpp>

#include "bin_version.h"
#include "play_manager.hpp"

int main(int argc, const char *argv[]) {
  // ---- init the application
  std::string instance_name = next::sdk::version_manager::getPlayerName();
  next::sdk::InitChain::ConfigMap initmap;
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), instance_name);
  initmap.emplace(logger::getFilenameCfgKey(), instance_name + ".log");
  initmap.emplace(logger::getSentinelProductNameCfgKey(), instance_name);

  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next::sdk::profiler::register_to_init_chain();
  next::sdk::version_manager::register_to_init_chain();

  std::string version_string = next::sdk::version_manager::MakeVersionString(VERSIONINFO_PROJECTVERSION_STR);
  std::string doc_link =
      next::sdk::version_manager::CreateDocumentationLink(next::sdk::version_manager::ComponentName::NEXT_PLAYER);
  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(), instance_name);
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), doc_link);
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), version_string);

#ifdef _DEBUG
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::DETAIL);
#else
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::OFF);
#endif
  if (!next::sdk::InitChain::Run(initmap)) {
    std::cerr << "Failed to initialize the Player" << std::endl;
    return 1;
  }
#if defined(__linux__)
  // For linux big measurements require more file descriptors than the default value
  // code is taken from https://github.com/eclipse-ecal/ecal/issues/592

  struct rlimit limit;
  bool readSucceeded = true;
  if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
    logger::error(__FILE__, "getrlimit() failed with errno {}", errno);
    readSucceeded = false;
  }

  if (readSucceeded) {
    limit.rlim_cur = limit.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
      logger::error(__FILE__, "setrlimit() failed with errno {0}", errno);
    }
  }
#endif

  next::sdk::shutdown_handler::register_shutdown_handler();
  // ---- init done
  auto session_client = next::appsupport::session::SessionClient(instance_name);

  auto profiling_level_hook = std::bind(&next::appsupport::updateProfilerLevel, std::placeholders::_1,
                                        next::appsupport::configkey::getProfilingLevelKey(instance_name));

  next::appsupport::ConfigClient::do_init(instance_name);
  auto cmd_options = next::appsupport::CmdOptions(instance_name);
  LogConnector::connectLoggerToArgs(instance_name, cmd_options);

  cmd_options.addParameterString("recording,r", "path to a recording",
                                 next::appsupport::configkey::player::getRecordingPathKey());

  next::appsupport::ConfigClient::getConfig()->putCfg("playerdefaultconfig.json");
  next::appsupport::ConfigClient::getConfig()->addChangeHook(profiling_level_hook);

#ifdef _DEBUG
  auto profiling_level = next::sdk::profiler::ProfilingLevel::DETAIL;
#else
  auto profiling_level = next::sdk::profiler::ProfilingLevel::OFF;
#endif
  try {
    next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getProfilingLevelKey(instance_name),
                                                     next::sdk::profiler::kProfilingLevel.at(profiling_level));
  } catch (const std::exception &e) {
    logger::error(__FILE__, e.what());
  }
  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getMajorVersionKey(instance_name),
                                                   VERSIONINFO_MAJOR);
  // TODO comment in set tags after implementation in config class
  /*next::appsupport::ConfigClient::getConfig()->setTag(next::appsupport::configkey::getMajorVersionKey(instance_name),
                                                      next::appsupport::configtags::TAG::READONLY);*/
  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getMinorVersionKey(instance_name),
                                                   VERSIONINFO_MINOR);
  /*next::appsupport::ConfigClient::getConfig()->setTag(next::appsupport::configkey::getMinorVersionKey(instance_name),
                                                      next::appsupport::configtags::TAG::READONLY);*/
  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getPatchVersionKey(instance_name),
                                                   VERSIONINFO_PATCH);
  /*next::appsupport::ConfigClient::getConfig()->setTag(next::appsupport::configkey::getPatchVersionKey(instance_name),
                                                      next::appsupport::configtags::TAG::READONLY);*/
  next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::player::getBufferMaxReadAheadKey(),
                                                   MAX_READ_AHEAD_BUFFER_SIZE_MB);
  bool ret = cmd_options.parseCmd(argc, argv);
  if (!ret) {
    return ret;
  }
  next::appsupport::ConfigClient::finish_init();

  LogConnector::connectLoggerToSystem(instance_name);

  std::shared_ptr<PlayManager> player_(new PlayManager);

  next::sdk::shutdown_handler::register_shutdown_handler([&player_]() {
    if (player_) {
      player_->Shutdown();
    }
  });
  next::appsupport::ConfigClient::getConfig()->addChangeHook([&player_]([[maybe_unused]] std::string property) {
    bool value_available{false};
    auto sil_factor = next::appsupport::ConfigClient::getConfig()->get_float(
        next::appsupport::configkey::player::getRealTimeFactorKey(), -1.0f, false, value_available);
    if (value_available)
      player_->UpdateSilFactorFromConfig(sil_factor);
    return true;
  });

  player_->Initialize();

  if (cmd_options.OptionUsed("recording")) {
    player_->RunCommandLine();
  } else {
    while (!next::sdk::shutdown_handler::shutdown_requested()) {
      player_->Execute();
    }
  }
  player_->Shutdown();

  // Manage shared pointer to the play_manager
  player_.reset();
  session_client.StopSessionClient();
  next::sdk::InitChain::Reset();

  next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_COMP_SHUTDOWN_FINISHED,
                                                      "[Player] Shutdown finished");

  return 0;
}
