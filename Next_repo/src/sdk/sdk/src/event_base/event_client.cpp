/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_client.cpp
 * @brief    implementation of class for EventClient which publish and send events over ecal
 *
 **/

#include "event_client.h"

#include <chrono>
#include <thread>

#include <ecal/ecal.h>

#include "event_publisher.h" // for createclientName()

namespace next {
namespace sdk {
namespace events {

static constexpr int kEcalConnectionWaitMs = 1000;
static constexpr int kClientTime = -1;
void EventClient::DestroyEcalType(std::shared_ptr<eCAL::CServiceClient> ecalInstance) {
  ecalInstance->RemResponseCallback();
  ecalInstance->Destroy();
}
EventClient::EventClient() {}

EventClient::~EventClient() {
  shutdown_ = true;
  reset();
}

void EventClient::onClientConnectionCallback(const char *name, const eCAL::SClientEventCallbackData * /*data*/) {
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};
  for (auto hook : GetListenersAddedHooks()) {
    hook.second(name);
  }
}

std::shared_ptr<eCAL::CServiceClient> EventClient::AddClient(const std::string &event_class,
                                                             const std::string &event_name) {
  bool ret;

  std::shared_ptr<eCAL::CServiceClient> client = std::make_shared<eCAL::CServiceClient>();

  ret = client->Create(EventHelper::createName(event_class, event_name));
  auto event_callback =
      std::bind(&EventClient::onClientConnectionCallback, this, std::placeholders::_1, std::placeholders::_2);
  ret = ret && client->AddEventCallback(eCAL_Client_Event::client_event_connected, event_callback);
  if (ret) {
    EmplaceEcalType(client);
  } else {
    return {};
  }
  return client;
}

/*!
 * @param event_class will be used as eCal service_name_
 * @param event_name will be used as eCal method_name_
 * @param payload weill be used as eCal request_ string
 */
bool EventClient::call(const std::string &event_class, const std::string &event_name, const payload_type &payload,
                       const RegisteredEvent &event, std::list<std::string> &list_of_responses) {
  bool ret = true;
  std::shared_ptr<eCAL::CServiceClient> client = lookUpEcalType();

  if (!client) { // check if client is a nullptr
    client = AddClient(event_class, event_name);
    // wait until creation is proopgated through network
    std::this_thread::sleep_for(std::chrono::milliseconds(kEcalConnectionWaitMs));
  }
  if (!client) {
    return false;
  }
  if (!CheckHooksAdded(event)) {
    AddSubscriberAddedHook(event);
  }
  eCAL::ServiceResponseVecT responses;
  std::string payload_string = PayloadToString(payload);
  ret = ret && client->Call(event_name, payload_string, kClientTime, &responses); // this is a blocking call
  for (auto resp : responses) {
    list_of_responses.push_back(resp.response);
    // check for responses, were all executed, possible states are:
    // enum eCallState
    // {
    //   call_state_none = 0,    //!< undefined
    //   call_state_executed,    //!< executed (successfully)
    //   call_state_failed       //!< failed
    // };
    ret = ret && (resp.call_state == call_state_executed);
  }
  return ret;
}

bool EventClient::initialize(const std::string &event_class, const std::string &event_name,
                             const RegisteredEvent &event) {
  std::shared_ptr<eCAL::CServiceClient> client = lookUpEcalType();
  if (!client) { // check if client is a nullptr
    client = AddClient(event_class, event_name);
  }
  if (!client) {
    return false;
  }
  if (!CheckHooksAdded(event)) {
    AddSubscriberAddedHook(event);
  }
  return true;
}
} // namespace events
} // namespace sdk
} // namespace next
