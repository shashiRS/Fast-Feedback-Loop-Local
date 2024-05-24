/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_types_hook_helper.h
 * @brief    Helper functions for event types.
 *
 **/
#ifndef NEXTSDK_CONTROL_EVENT_TYPES_HOOK_HELPER_H_
#define NEXTSDK_CONTROL_EVENT_TYPES_HOOK_HELPER_H_

#include <assert.h>
#include <atomic>
#include <map>
#include <mutex>
#include <string>

#include <next/sdk/event_base/event_types.h>
#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace events {

class EventHelper {
public:
  static std::string createName(const std::string &event_class, const std::string &event_name,
                                bool is_pull_request = false) {
    if (is_pull_request)
      return event_class + "_" + event_name + "_pullrequest";

    return event_class + "_" + event_name;
  }
};
template <typename eCALType>
class EventTypesHookHelper {

protected:
  virtual void DestroyEcalType(std::shared_ptr<eCALType> ecalInstance) = 0;

public:
  bool unregisterHooks(const RegisteredEvent &event) {
    std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};

    if (listeners_added_hooks.find(event.unique_id) != listeners_added_hooks.end()) {
      listeners_added_hooks.erase(event.unique_id);
    }

    if (event_received_hooks_.find(event.unique_id) != event_received_hooks_.end()) {
      event_received_hooks_.erase(event.unique_id);
    }

    if (sync_event_received_hooks_.find(event.unique_id) != sync_event_received_hooks_.end()) {
      sync_event_received_hooks_.erase(event.unique_id);
    }
    return true;
  }

protected:
  EventTypesHookHelper(){};
  virtual ~EventTypesHookHelper(){};

  //! the current milliseconds are used as unique topic ID
  static long long getCurrentMilliSeconds() {
    using namespace std::chrono;
    long long ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return ms;
  }

  bool CheckHooksAdded(const RegisteredEvent &event) {
    std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};

    if (listeners_added_hooks.find(event.unique_id) == listeners_added_hooks.end()) {
      return false;
    }

    if (event_received_hooks_.find(event.unique_id) == event_received_hooks_.end()) {
      return false;
    }

    if (sync_event_received_hooks_.find(event.unique_id) == sync_event_received_hooks_.end()) {
      return false;
    }
    return true;
  }

  std::shared_ptr<eCALType> lookUpEcalType() {
    std::lock_guard<std::mutex> guard{protect_ecal_types};
    if (shutdown_) {
      return {};
    }
    return ecal_types;
  }

  void EmplaceEcalType(const std::shared_ptr<eCALType> ecal_type) {
    std::lock_guard<std::mutex> guard{protect_ecal_types};
    ecal_types = ecal_type;
  }

  void InsertOrAssignEcalType(const std::shared_ptr<eCALType> ecal_type) {
    std::lock_guard<std::mutex> guard{protect_ecal_types};
    ecal_types = ecal_type;
  }

  void AddSubscriberAddedHook(const RegisteredEvent &event) {
    std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};

    if (event.subsciber_added_callback) {
      listeners_added_hooks.insert({event.unique_id, event.subsciber_added_callback});
    }
  }

  void AddEventReceived(const RegisteredEvent &event) {
    std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};

    if (event.event_callback) {
      event_received_hooks_.insert({event.unique_id, event.event_callback});
    }

    if (event.sync_event_callback) {
      sync_event_received_hooks_.insert({event.unique_id, event.sync_event_callback});
    }
  }

  inline std::map<size_t, subscribe_event_callback> GetListenersAddedHooks() { return listeners_added_hooks; }
  inline std::map<size_t, event_callback_type> GetEventReceivedHooks() { return event_received_hooks_; }
  inline std::map<size_t, sync_event_callback_type> GetSyncEventReceivedHooks() { return sync_event_received_hooks_; }

  void reset() {
    shutdown_ = true;
    {
      std::lock_guard<std::recursive_mutex> guardHookMaps{protect_hook_maps};
      event_received_hooks_.clear();
      sync_event_received_hooks_.clear();
      listeners_added_hooks.clear();
    }
    {
      std::lock_guard<std::mutex> guardEcalTypes{protect_ecal_types};
      if (ecal_types.get() != nullptr) {
        DestroyEcalType(ecal_types);
        ecal_types.reset();
      }
    }
  }

protected:
  std::recursive_mutex protect_hook_maps{};

  std::atomic<bool> shutdown_ = false;

private:
  std::shared_ptr<eCALType> ecal_types;
  std::mutex protect_ecal_types{};
  std::map<size_t, event_callback_type> event_received_hooks_;
  std::map<size_t, sync_event_callback_type> sync_event_received_hooks_;
  std::map<size_t, subscribe_event_callback> listeners_added_hooks;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_CONTROL_EVENT_TYPES_HOOK_HELPER_H_
