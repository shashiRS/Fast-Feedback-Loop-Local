/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_callbacks.hpp
 *
 * @brief    Provide all overwritable functions for the Next Component Interface
 *
 */

#include <next/next_callbacks.hpp>

#include <next/appsupport/config/config_base.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/log_connector/log_connector.hpp>
#include <next/sdk/ecal/ecal_singleton.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/sdk/profiler/profiler.hpp>

namespace next {

NextCallbacks::NextCallbacks(const std::string &component_name) {
  next::sdk::InitChain::ConfigMap initmap;
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), component_name);
  initmap.emplace(next::sdk::logger::getFilenameCfgKey(), component_name + ".log");
  initmap.emplace(next::sdk::logger::getSentinelProductNameCfgKey(), component_name);
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::OVERVIEW);
  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();

  bool in_container;
  if (next::sdk::InitChain::IsInitialized()) {
    std::string container_name = "";
    if (next::appsupport::ConfigClient::getConfig()) {
      container_name = next::appsupport::ConfigClient::getConfig()->getRootName();
    }
    debug(__FILE__, "component {0} running in container {1}", component_name, container_name);
    in_container = true;
  } else {
    in_container = false;
    if (!next::sdk::InitChain::Run(initmap)) {
      error(__FILE__, "init failed", component_name);
    }
  }
  next::appsupport::ConfigClient::do_init(component_name);

  auto hook = std::bind(&NextCallbacks::onConfigChanged, this, std::placeholders::_1);
  next::appsupport::ConfigClient::getConfig()->addChangeHook(hook);
  LogConnector::connectLoggerToSystem(component_name);

  if (!in_container) {
    next::appsupport::ConfigClient::finish_init();
  }
}

NextCallbacks::~NextCallbacks() { next::sdk::InitChain::Reset(); }

bool NextCallbacks::onConfigChanged([[maybe_unused]] std::string changed_config_key) { return true; }

} // namespace next
