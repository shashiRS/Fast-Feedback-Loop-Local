/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     lfc_trigger_app.cpp
 * @brief    small application for triggering lifecycle events over eCAL
 *
 **/

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <thread>

#include <boost/program_options.hpp>

#include <next/sdk/events/lifecycle_state.h>
#include <next/sdk/events/lifecycle_trigger.h>
#include <next/sdk/sdk.hpp>

namespace po = boost::program_options;

using next::sdk::InitChain;

// use next::sdk::shutdown_handler
std::atomic<bool> shutdown_flag = false;
void shutdownCallBack() { shutdown_flag = true; }

// if channel is "*" than the eCAL topic name will be not extended with channel name
std::string getChannelName(const std::string &channel) {
  if (channel == "*") {
    return "";
  } else {
    return channel;
  }
}

void lfc_trigger_Event(const std::string &channel, const std::string &payload) {
  next::sdk::events::LifecycleTrigger lfc_trigger("");
  lfc_trigger.publishTrigger(payload, getChannelName(channel));
}

void lfc_state_Event(const std::string &channel, const std::string &payload) {
  next::sdk::events::LifecycleState lfc_state("");
  lfc_state.publishState(payload, getChannelName(channel));
}

// lookup map for supported events
using event_method = std::function<void(const std::string &, const std::string &)>;
std::map<std::string, event_method> event_methods = {
    {"lfc_trigger", std::bind(&lfc_trigger_Event, std::placeholders::_1, std::placeholders::_2)},
    {"lfc_state", std::bind(&lfc_state_Event, std::placeholders::_1, std::placeholders::_2)},
};

void parseEvent(const std::string &channel, const std::string &event, const std::string &payload) {
  next::sdk::logger::debug(__FILE__, "channel={} ; event={} ; payload={}", channel, event, payload);
  if (auto event_method = event_methods.find(event); event_method != event_methods.end()) {
    next::sdk::logger::debug(__FILE__, "sending payload {} to event {}", payload, event);
    event_method->second(channel, payload);
  } else {
    next::sdk::logger::debug(__FILE__, "event \"{}\" not supported yet", event);
  }
}

void repeatMode(const std::string &channel, const std::string &event, const std::string &payload, long long timeout) {
  std::cout << "repeat mode: press CTRL+C to exit\n";
  while (!shutdown_flag) {
    parseEvent(channel, event, payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
  }
}

int main(int argc, const char *argv[]) {
  po::options_description desc("Allowed options");
  // clang-format off
  desc.add_options()
    ("help,h", "produce help message")
    ("channel,c", po::value<std::string>()->default_value("*"), "which channel should be addressed, use * to address all channels")
    ("event,e", po::value<std::string>()->default_value(""), "which event should be used, e.g. \"lfc_trigger\" or \"SHUTDOWN\"")
    ("payload,p", po::value<std::string>()->default_value(""), "which payload should be used, e.g. \"RESET\" or \"SHUTDOWN\" for lfc_trigger event")
    ("interactive,i", "starts the interactive mode")
    ("repeat,r", "repeat event every second")
    ("verbose", "set console log level to DEBUG")
  ;
  // clang-format on
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  std::string app_name = "lfc_trigger_app";
  // ------------------------ init start
  // start of the initial lines of the application
  // first configure the initialization, then init the stuff and then the main application can continue
  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next_profiling::register_to_init_chain();
  next::sdk::version_manager::register_to_init_chain();

  InitChain::ConfigMap initmap;
  if (vm.count("verbose")) {
  } else {
  }

  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), app_name);
  initmap.emplace(next::sdk::logger::getFilenameCfgKey(), app_name + "_log.txt");
  initmap.emplace(next::sdk::version_manager::getVersionManagerComponentNameCfgKey(), app_name);
  initmap.emplace(next::sdk::version_manager::getVersionManagerDocumentationCfgKey(), "docu");
  initmap.emplace(next::sdk::version_manager::getVersionManagerVersionStringCfgKey(), "0.0.0.0");
  initmap.emplace(next::sdk::logger::getLogRouterDisableCfgKey(), true);
#ifndef _DEBUG
  InitChain::Run(initmap);
#else
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::FULL);
  bool res = InitChain::Run(initmap);
  assert(res);
#endif
  next::sdk::shutdown_handler::register_shutdown_handler(&shutdownCallBack);
  // TODO: if the EcalSingleton is not used, it is also not initialized, must be fixed
  next::sdk::EcalSingleton::get_instance();
  // ------------------------ init end

  if (vm.count("repeat")) {
    repeatMode(vm["channel"].as<std::string>(), vm["event"].as<std::string>(), vm["payload"].as<std::string>(), 1000);
  } else {
    parseEvent(vm["channel"].as<std::string>(), vm["event"].as<std::string>(), vm["payload"].as<std::string>());
  }

  InitChain::Reset(); // this must be the last / second last line of the application, it destroys the singletons
  return 0;
}
