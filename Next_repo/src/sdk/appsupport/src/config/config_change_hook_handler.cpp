/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_change_hook_handler.cpp
 * @brief    class for adding function hooks if config value(s) changed
 *
 **/

#include <chrono>

#include "config_change_hook_handler.hpp"

namespace next {
namespace appsupport {

ConfigChangeHookHandler::ConfigChangeHookHandler() {}

ConfigChangeHookHandler::~ConfigChangeHookHandler() { hash_map_for_hooks_.clear(); }

uint64_t ConfigChangeHookHandler::getCurrentNanoSeconds() {
  using namespace std::chrono;
  return static_cast<uint64_t>(uint64_t(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count()));
}

size_t ConfigChangeHookHandler::addChangeHook(configtypes::change_hook hook) {
  auto id = getCurrentNanoSeconds();
  hash_map_for_hooks_.insert(std::make_pair(id, hook));
  return id;
}

bool ConfigChangeHookHandler::callChangeHooks(std::string key) {
  if (change_hooks_activated_) {
    for (auto &[id, callback] : hash_map_for_hooks_) {
      callback(key);
      std::ignore = id;
    }
  }

  return true;
}

void ConfigChangeHookHandler::removeChangeHook(size_t id) {
  // if "id" doesn't exist, nothing is erased and erase returns that 0 elements were removed
  hash_map_for_hooks_.erase(id);
}

bool ConfigChangeHookHandler::getChangeHooksActivationStatus() { return change_hooks_activated_; }

void ConfigChangeHookHandler::setChangeHooksActivationStatus(bool change_hook_activation_status) {
  change_hooks_activated_ = change_hook_activation_status;
}

} // namespace appsupport
} // namespace next
