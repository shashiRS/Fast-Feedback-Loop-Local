/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_server.cpp
 * @brief    implementation of class for EventServer which subscribe and receive events over ecal
 *
 **/

#include "event_server.h"

#include <ecal/ecal.h>

#include "event_publisher.h" //for createPublisherName()

namespace next {
namespace sdk {
namespace events {

void EventServer::DestroyEcalType(std::shared_ptr<eCAL::CServiceServer> ecalInstance) { ecalInstance->Destroy(); }

EventServer::EventServer() {}

EventServer::~EventServer() {
  shutdown_ = true;
  reset();
}

int EventServer::onMethodCallback([[maybe_unused]] const std::string &method_,
                                  [[maybe_unused]] const std::string &req_type_,
                                  [[maybe_unused]] const std::string &resp_type_, const std::string &request_,
                                  std::string &response) {
  if (shutdown_) {
    return 0;
  }

  // get request data;
  std::vector<uint8_t> buf = StringToPayload(request_);

  std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};
  // call registered callback with received message
  for (auto hook : GetEventReceivedHooks()) {
    hook.second(buf, 0, 0);
  }

  for (auto &hook : GetSyncEventReceivedHooks()) {
    hook.second(buf, response);
  }

  return 0;
}

void EventServer::onListenerAddedEvent(const char *name, [[maybe_unused]] const eCAL::SServerEventCallbackData *data) {
  if (shutdown_) {
    return;
  }
  std::lock_guard<std::recursive_mutex> guard{protect_hook_maps};
  for (auto hook : GetListenersAddedHooks()) {
    hook.second(name);
  }
}

bool EventServer::AddServer(const std::string &event_class, const std::string &topic_name) {
  bool ret = false;

  std::shared_ptr<eCAL::CServiceServer> server = std::make_shared<eCAL::CServiceServer>();

  ret = server->Create(EventHelper::createName(event_class, topic_name));
  auto callb = std::bind(&EventServer::onMethodCallback, this, std::placeholders::_1, std::placeholders::_2,
                         std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
  auto event_callback =
      std::bind(&EventServer::onListenerAddedEvent, this, std::placeholders::_1, std::placeholders::_2);

  ret = ret && server->AddMethodCallback(topic_name, "EventRequestType", "EventResponseType", callb);
  ret = ret && server->AddEventCallback(eCAL_Server_Event::server_event_connected, event_callback);
  if (ret) {
    EmplaceEcalType(server);
  }

  return ret;
}

bool EventServer::initialize(const std::string &event_class, const std::string &event_name,
                             const RegisteredEvent &event) {
  bool ret = true;
  std::shared_ptr<eCAL::CServiceServer> server = lookUpEcalType();

  if (!server) { // check if server is a nullptr
    ret = AddServer(event_class, event_name);
  }

  if (!CheckHooksAdded(event)) {
    AddEventReceived(event);
    AddSubscriberAddedHook(event);
  }

  return ret;
}
bool EventServer::listen(const std::string &event_class, const std::string &topic_name, const RegisteredEvent &event) {
  bool ret = true;
  std::shared_ptr<eCAL::CServiceServer> server = lookUpEcalType();

  if (!server) { // check if server is a nullptr
    ret = AddServer(event_class, topic_name);
  }

  if (!CheckHooksAdded(event)) {
    AddEventReceived(event);
    AddSubscriberAddedHook(event);
  }

  return ret;
}

} // namespace events
} // namespace sdk
} // namespace next
