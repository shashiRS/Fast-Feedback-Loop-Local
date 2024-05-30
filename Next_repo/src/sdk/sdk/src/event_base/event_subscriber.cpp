/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_subscriber.cpp
 * @brief    implementation of class for EventSubscriber which subscribe and receive events over ecal
 *
 **/
#include "event_subscriber.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <ecal/ecal.h>

#include <next/sdk/logger/logger.hpp>

#include "event_manager.h"
#include "event_publisher.h" //for createPublisherName()

#ifdef NEXTSDK_LOGGER_H_
// #error logger cannot be used here. This is a singleton which might be destroyed after the logger is closed.
#endif

namespace next {
namespace sdk {
namespace events {

void EventSubscriber::DestroyEcalType(std::shared_ptr<eCAL::CSubscriber> ecalInstance) {
  ecalInstance->RemReceiveCallback();
  ecalInstance->Destroy();
}

EventSubscriber::EventSubscriber() { EventManager::GetInstance(); }

EventSubscriber::~EventSubscriber() {
  shutdown_ = true;
  std::lock_guard<std::mutex> shutdown_protect{protect_shutdown_};
  reset();
}

void EventSubscriber::onDataReceive(const char *, const struct eCAL::SReceiveCallbackData *data) {
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::mutex> shutdown_protect{protect_shutdown_};

  std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};

  // get received data;
  payload_type buf((uint8_t *)data->buf, ((uint8_t *)data->buf) + data->size);

  for (const auto &hook : GetEventReceivedHooks()) {
    hook.second(buf, data->time, data->id);
  }
}

void EventSubscriber::onListenerAddedEvent(const char *topic_name, const eCAL::SSubEventCallbackData * /*data*/) {
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};
  for (auto &hook : GetListenersAddedHooks()) {
    hook.second(topic_name);
  }
}

bool EventSubscriber::AddSubscriber(const std::string &event_class, const std::string &topic_name,
                                    bool is_pull_request) {
  // need to create a new subscriber
  eCAL::SDataTypeInformation topic_info;
  topic_info.descriptor = "Next simulation framework event type";
  topic_info.name = "EventType";
  topic_info.encoding = "";
  auto subscriber = std::make_shared<eCAL::CSubscriber>(
      EventHelper::createName(event_class, topic_name, is_pull_request), topic_info);
  bool ret = subscriber->IsCreated();
  if (ret) {
    auto callb = std::bind(&EventSubscriber::onDataReceive, this, std::placeholders::_1, std::placeholders::_2);
    auto event_callback =
        std::bind(&EventSubscriber::onListenerAddedEvent, this, std::placeholders::_1, std::placeholders::_2);

    ret &= subscriber->AddReceiveCallback(callb);
    ret &= subscriber->AddEventCallback(eCAL_Subscriber_Event::sub_event_connected, event_callback);
    InsertOrAssignEcalType(subscriber);
  }
  return ret;
}

bool EventSubscriber::initialize(const std::string &event_class, const std::string &event_name,
                                 const RegisteredEvent &event) {
  bool ret = false;
  std::shared_ptr<eCAL::CSubscriber> subscriber = lookUpEcalType();
  if (!subscriber) { // check if subscriber is a nullptr
    ret = AddSubscriber(event_class, event_name, event.is_pull_requested);
  } else {
    // don't do anything, topic is already subscribed
    ret = true;
  }

  if (ret && !CheckHooksAdded(event)) {
    AddEventReceived(event);
    AddSubscriberAddedHook(event);
  }
  return ret;
}
bool EventSubscriber::subscribe(const std::string &event_class, const std::string &topic_name,
                                const RegisteredEvent &event) {
  bool ret = false;
  std::shared_ptr<eCAL::CSubscriber> subscriber = lookUpEcalType();
  if (!subscriber) { // check if subscriber is a nullptr
    ret = AddSubscriber(event_class, topic_name, event.is_pull_requested);
  } else {
    // don't do anything, topic is already subscribed
    ret = true;
  }
  if (ret && !CheckHooksAdded(event)) {
    AddEventReceived(event);
    AddSubscriberAddedHook(event);
  }
  return ret;
}

} // namespace events
} // namespace sdk
} // namespace next
