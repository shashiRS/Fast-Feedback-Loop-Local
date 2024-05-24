/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     testapp.cpp
 * @brief    small application for testing and demonstrating the Next SDK
 *
 **/

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

// we will not use everything of the logger namespace
#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/profiler/profile_tags.hpp>
#include <next/sdk/sdk.hpp>

namespace logger = next::sdk::logger; // make it more handy
using next::sdk::InitChain;

int main() {
  // ------------------------ init start
  // start of the initial lines of the application
  // first configure the initialization, then init the stuff and then the main application can continue
  logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next_profiling::register_to_init_chain();
  next::sdk::version_manager::register_to_init_chain();

  InitChain::ConfigMap initmap;
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), "testapp");
  initmap.emplace(logger::getFilenameCfgKey(), "my_logfile.txt");
  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(), "name");
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), "docu");
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), "0.0.0.0");
  initmap.emplace(logger::getLogRouterDisableCfgKey(), true);
#ifndef _DEBUG
  InitChain::Run(initmap);
#else
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::FULL);

  bool res = InitChain::Run(initmap);
  assert(res);
#endif
  next::sdk::shutdown_handler::register_shutdown_handler();
  // TODO: if the EcalSingleton is not used, it is also not initialized, must be fixed
  next::sdk::EcalSingleton::get_instance();
  // ------------------------ init end

  // debug, info, warn and error can be used with char*
  logger::debug(__FILE__, "a {0} message", "debug"); // arguments are possible
  logger::info(__FILE__, "an {0} message", "info");
  logger::warn(__FILE__, "a warning message"); // or without arguments
  logger::error(__FILE__, "an error message");
  // but std::string is also possible
  const std::string msg_fmt("{0} message");
  logger::debug(__FILE__, msg_fmt, "a debug");
  logger::info(__FILE__, msg_fmt, "an info");
  logger::warn(__FILE__, std::string("a warning message"));
  logger::error(__FILE__, std::string("an error message"));

  std::chrono::time_point start = std::chrono::high_resolution_clock::now();
  while (
      !next::sdk::shutdown_handler::shutdown_requested() &&
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() <
          5000) {
    {
      ProfileCoreN_OV("cout");
      std::cout << ".";
    }
    {
      ProfileCoreN_FLL("sleep");
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  std::cout << std::endl;

  InitChain::Reset(); // this must be the last / second last line of the application, it destroys the singletons
  return 0;
}
