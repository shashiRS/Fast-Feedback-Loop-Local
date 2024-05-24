/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     testlifecycleapp.cpp
 * @brief    small application for testing and demonstrating the Next Core SDK with life cycle management
 *
 **/

// ! Just a demonstration app. Proper testing will be done in unit tests and in NEXT Repo

#include <next/appsupport/lifecycle/lifecycle_base.hpp>
#include <next/sdk/sdk.hpp>

namespace logger = next::sdk::logger; // make it more handy
using next::sdk::InitChain;

class TestApp_LifeCycle : public next::appsupport::LifecycleBase {
public:
  TestApp_LifeCycle(const std::string &name, uint32_t timeout = 1000)
      : next::appsupport::LifecycleBase(name, timeout) {}

  virtual bool enterConfig() {
    logger::debug(__FILE__, "enterConfig");
    return true;
  }
  virtual bool enterInit() {
    logger::debug(__FILE__, "enterInit");
    return true;
  }
  virtual bool enterReset() {
    logger::debug(__FILE__, "enterReset");
    return true;
  }
  virtual bool enterShutdown() {
    logger::debug(__FILE__, "enterShutdown");
    return true;
  }

  virtual bool onError(std::string error, [[maybe_unused]] std::exception *e = nullptr) {
    logger::debug(__FILE__, "onError: {}", error);
    return true;
  }
};

std::unique_ptr<TestApp_LifeCycle> lifecycle;

void shutdownCallBack() { lifecycle->shutdown(); }

int main([[maybe_unused]] int argc, [[maybe_unused]] const char *argv[]) {
  std::string app_name = "testlifcycleapp";
  lifecycle = std::unique_ptr<TestApp_LifeCycle>(new TestApp_LifeCycle(app_name));

  // ------------------------ init start
  // start of the initial lines of the application
  // first configure the initialization, then init the stuff and then the main application can continue
  logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next_profiling::register_to_init_chain();
  next::sdk::version_manager::register_to_init_chain();

  InitChain::ConfigMap initmap;
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), app_name);
  initmap.emplace(logger::getFilenameCfgKey(), "my_logfile.txt");
  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(), app_name);
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), "docu");
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), "0.0.0.0");
  initmap.emplace(next::sdk::logger::getLogRouterDisableCfgKey(), true);
#ifndef _DEBUG
  InitChain::Run(initmap);
#else
  initmap.emplace(logger::getLogRouterDisableCfgKey(), true);
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::FULL);

  bool res = InitChain::Run(initmap);
  assert(res);
#endif
  next::sdk::shutdown_handler::register_shutdown_handler(&shutdownCallBack);
  // TODO: if the EcalSingleton is not used, it is also not initialized, must be fixed
  next::sdk::EcalSingleton::get_instance();
  // ------------------------ init end

  // start the lifecycle statemachine, this methods blocks until a shutdown happens
  int ret = 0; // lifecycle->run();

  InitChain::Reset(); // this must be the last / second last line of the application, it destroys the singletons
  logger::debug(__FILE__, "shutdown App: {} with return value: {}", app_name, ret);
  return ret;
}
