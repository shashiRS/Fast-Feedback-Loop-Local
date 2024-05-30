/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     ecal_singleton.cpp
 * @brief    eCAL singleton, used for initalization and finalizing eCAL
 *
 **/

#include <next/sdk/ecal/ecal_singleton.hpp>

#include <cassert>
#include <iostream>
#include <thread>

#include <ecal/ecal.h>

#include <next/sdk/logger/logger.hpp>
#include <next/sdk/profiler/profile_tags.hpp>

static constexpr const char *ECALSINGLETON_INSTANCENAME_CFG_KEY = "ecalsingleton-instancename";

constexpr const int kRegistrationTimeoutMs = 3600000;
constexpr const bool kShmMonitoringEnabled = true;
constexpr const bool kNetworkMonitoringDisabled = true;

namespace next {
namespace sdk {

EcalSingleton *EcalSingleton::self_;
bool EcalSingleton::init_done_;

EcalSingleton::EcalSingleton() {
  ProfileCore_OV;
  self_ = this;
  if (eCAL::IsInitialized()) {
    if (eCAL::Config::GetRegistrationTimeoutMs() == kRegistrationTimeoutMs &&
        eCAL::Config::Experimental::IsShmMonitoringEnabled() == kShmMonitoringEnabled &&
        eCAL::Config::Experimental::IsNetworkMonitoringDisabled() == kNetworkMonitoringDisabled) {
      logger::debug(__FILE__, "EcalSingleton() called, singleton initialized using the default ecal.ini configuration");
    } else {
      logger::error(__FILE__,
                    "EcalSingleton() called, singleton initialized without using the default ecal.ini configuration");
    }
  } else {
    logger::error(__FILE__, "EcalSingleton() called: eCAL not initialized");
  }
}

EcalSingleton::~EcalSingleton() {
  // this is a workaround to allow ecal to close all samples. Currently they don't have a mutex there.
  // Finalizing ecal during a sample leads to a crash in Finalize()
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  logger::debug(__FILE__, "~EcalSingleton() called, calling eCAL::Finalize()");
  eCAL::Finalize();
}

EcalSingleton &EcalSingleton::get_instance() {
  ProfileCore_DTL;
  if (self_ != nullptr) {
    return *self_;
  }
  throw std::runtime_error("Pointer to the singleton instance is null, this is not expected.");
}

const char *EcalSingleton::getInstancenameCfgKey() { return ECALSINGLETON_INSTANCENAME_CFG_KEY; }

bool EcalSingleton::init_done() { return nullptr != self_; }

void EcalSingleton::clear() { self_ = nullptr; }

bool EcalSingleton::do_init(const InitChain::ConfigMap &map, int) {
  logger::debug(__FILE__, "EcalSingleton::do_init()");

  if (init_done()) {
    return true;
  }
  // we can only init once
  assert(!init_done_);
  // should not be initialized, this is the job of this function
  assert(eCAL::IsInitialized() == 0);

  // getting the instance name
  std::string instance_name;
  auto cit = map.find(ECALSINGLETON_INSTANCENAME_CFG_KEY);
  if (map.end() == cit) {
    logger::warn(__FILE__, "EcalSingleton::init(): No instance name given.");
    return false;
  }
  if (cit->second.type() == typeid(std::string)) {
    instance_name = std::any_cast<std::string>(cit->second);
  } else if (cit->second.type() == typeid(char *) || cit->second.type() == typeid(const char *)) {
    instance_name = std::any_cast<const char *>(cit->second);
  } else {
    logger::error(__FILE__, "EcalSingleton::init(): Value type of the instance name name is invalid.");
    return false;
  }
  logger::debug(__FILE__, "EcalSingleton::init(): Initializing eCAL with instance name {0}", instance_name);

  // TODO add possibility to give per config what to initialize (All, Publisher...)
  int ret = eCAL::Initialize(0, nullptr, instance_name.c_str(), eCAL::Init::All);
  if (0 > ret) {
    logger::error(__FILE__, "EcalSingleton::init(): Failed to initialize eCAL.");
    return false;
  } else if (ret == 0) {
    logger::debug(__FILE__, "EcalSingleton::init() eCAL sucessfully initalized.");
  } else if (ret == 1) {
    logger::debug(__FILE__, "EcalSingleton::init(): eCAL was already initalized.");
  }

  // this is not producing a memory leak, it is assigning in the constructor the pointer to itself
  new EcalSingleton();
  init_done_ = true;

  return true;
}

void EcalSingleton::do_reset(const InitChain::ConfigMap &, int) {
  if (!init_done_) {
    return;
  }
  // no public destructor, so we have to cast to something that is accessable
  EcalSingleton *s = dynamic_cast<EcalSingleton *>(&get_instance());
  assert(nullptr != s);
  clear();
  delete s;
  init_done_ = false;
}

void EcalSingleton::register_to_init_chain() { static InitChain::El init_el(InitPriority::ECAL, do_init, do_reset); }

} // namespace sdk
} // namespace next
