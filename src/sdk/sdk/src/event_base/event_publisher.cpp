/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_publisher.cpp
 * @brief    implementation of class for EventPublisher which publish and send events over ecal
 *
 **/

#include "event_publisher.h"
#include "event_manager.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <ecal/ecal.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

static constexpr int kEcalConnectionWaitMs = 2000;
static constexpr int kEcalPublishTimoutMs = 5000;
static std::unordered_map<size_t, long long> publisher_hash_creation_time_map;

void EventPublisher::DestroyEcalType(std::shared_ptr<eCAL::CPublisher> ecalInstance) { ecalInstance->Destroy(); }

EventPublisher::EventPublisher() {
  // create EventManager singelton if not available
  EventManager::GetInstance();
}

EventPublisher::~EventPublisher() {
  shutdown_ = true;
  reset();
  //  for (auto &pub : ecal_types) {
  //    pub.second->RemEventCallback(eCAL_Publisher_Event::pub_event_connected);
  //  }
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void EventPublisher::onListenerAdded(const char *topic_name, const eCAL::SPubEventCallbackData * /*data*/) {
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};
  for (auto hook : GetListenersAddedHooks()) {
    hook.second(topic_name);
  }
}

std::shared_ptr<eCAL::CPublisher> EventPublisher::AddPublisher(const std::string &event_class,
                                                               const std::string &event_name, bool is_pull_requested) {
  bool ret = false;

  // need to create a new publisher
  auto publisher = std::make_shared<eCAL::CPublisher>();
  eCAL::STopicInformation topic_info;
  topic_info.topic_type.descriptor = "Next simulation framework event type";
  topic_info.topic_type.name = "EventType";
  topic_info.topic_type.encoding = "";
  auto topic_name_desc = EventHelper::createName(event_class, event_name, is_pull_requested);
  ret = publisher->Create(topic_name_desc, topic_info.topic_type);
  auto hash = std::hash<std::string>{}(topic_name_desc);
  publisher_hash_creation_time_map[hash] =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();

  publisher->SetID(getCurrentMilliSeconds()); // use current milliseconds as unique ID

  //  TODO FIX WITH SIMEN-9093
  //  auto event_callback = std::bind(&EventPublisher::onListenerAdded, this, std::placeholders::_1,
  //  std::placeholders::_2);
  //  ret = ret && publisher->AddEventCallback(eCAL_Publisher_Event::pub_event_connected, event_callback);
  if (ret) {
    EmplaceEcalType(publisher);
  }
  return publisher;
}

bool EventPublisher::initialize(const std::string &event_class, const std::string &event_name,
                                const RegisteredEvent &event) {
  std::shared_ptr<eCAL::CPublisher> publisher = lookUpEcalType();
  if (!publisher) { // check if subscriber is a nullptr
    publisher = AddPublisher(event_class, event_name, event.is_pull_requested);
  }

  if (!publisher) {
    return false;
  }

  if (!CheckHooksAdded(event)) {
    AddSubscriberAddedHook(event);
  }
  return true;
}

bool EventPublisher::publish(const std::string &event_class, const std::string &event_name, const payload_type &payload,
                             const RegisteredEvent &event, [[maybe_unused]] std::list<std::string> &list_of_responses) {
  std::shared_ptr<eCAL::CPublisher> publisher = lookUpEcalType();

  if (!publisher) { // check if subscriber is a nullptr
    publisher = AddPublisher(event_class, event_name, event.is_pull_requested);
    // wait for eCAL to enable all connections so first publish is valid
    logger::debug(__FILE__, "EventPublisher::AddPublisher waiting for eCAL connection {0} event: {1}",
                  kEcalConnectionWaitMs, event_name);
    std::this_thread::sleep_for(std::chrono::milliseconds(kEcalConnectionWaitMs));
  } else {

    auto topic_name_desc = EventHelper::createName(event_class, event_name, event.is_pull_requested);
    auto hash = std::hash<std::string>{}(topic_name_desc);

    if (const auto it = publisher_hash_creation_time_map.find(hash); it != publisher_hash_creation_time_map.end()) {
      while ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                  .count() -
              it->second) < kEcalConnectionWaitMs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  }

  if (!publisher) {
    return false;
  }

  if (!CheckHooksAdded(event)) {
    AddSubscriberAddedHook(event);
  }

  size_t send = publisher->Send(payload.data(), payload.size(), -1, kEcalPublishTimoutMs);
  return (send == payload.size());
}

int EventPublisher::getNumberOfSubscribers(const std::string &event_class, const std::string &event_name) {
  std::shared_ptr<eCAL::CPublisher> publisher = lookUpEcalType();
  if (!publisher) {
    return 0;
  }

  auto topic_name_desc = EventHelper::createName(event_class, event_name);
  auto hash = std::hash<std::string>{}(topic_name_desc);

  if (const auto it = publisher_hash_creation_time_map.find(hash); it != publisher_hash_creation_time_map.end()) {
    while ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count() -
            it->second) < kEcalConnectionWaitMs) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  return (int)publisher->GetSubscriberCount();
}

} // namespace events
} // namespace sdk
} // namespace next