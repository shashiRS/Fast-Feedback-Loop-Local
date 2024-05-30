/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     base_event_binary_impl.cpp
 * @brief    Implementation of binary event
 *
 **/

#include <next/sdk/event_base/base_event_binary_impl.h>

#include "event_manager.h"

namespace next {
namespace sdk {
namespace events {

bool BaseEventBinaryImpl::subscribe(const std::string &event_class, const std::string &event_name,
                                    const RegisteredEvent &event) {
  return EventManager::GetInstance()->subscribe(event_class, event_name, event);
}
bool BaseEventBinaryImpl::publish(const std::string &event_class, const std::string &event_name,
                                  const payload_type &payload, const RegisteredEvent &event,
                                  std::list<std::string> &list_of_responses) {
  return EventManager::GetInstance()->publish(event_class, event_name, payload, event, list_of_responses);
}

bool BaseEventBinaryImpl::initializeBinaryEvent(const std::string &event_class, const std::string &event_name,
                                                const RegisteredEvent &event) {
  return EventManager::GetInstance()->initializeEventBinary(event_class, event_name, event);
}
bool BaseEventBinaryImpl::initializeBinaryEventBlocking(const std::string &event_class, const std::string &event_name,
                                                        const RegisteredEvent &event) {
  return EventManager::GetInstance()->initializeEventBinaryBlocking(event_class, event_name, event);
}
bool BaseEventBinaryImpl::listen(const std::string event_class, const std::string event_name,
                                 const RegisteredEvent &event) {
  return EventManager::GetInstance()->listen(event_class, event_name, event);
}
bool BaseEventBinaryImpl::call(const std::string event_class, const std::string event_name, const payload_type &payload,
                               const RegisteredEvent &event, std::list<std::string> &list_of_responses) {
  return EventManager::GetInstance()->call(event_class, event_name, payload, event, list_of_responses);
}

bool BaseEventBinaryImpl::unregister(const std::string event_class, const std::string event_name,
                                     const RegisteredEvent &event) {
  return EventManager::GetInstance()->unregister(event, event_class, event_name);
}

int BaseEventBinaryImpl::getNumberOfSubscribers(const std::string event_class, const std::string event_name) {
  return EventManager::GetInstance()->getNumberOfSubscribers(event_class, event_name);
}

void BaseEventBinaryImpl::requestEventUpdate(const std::string &event_class, const std::string &event_name,
                                             const payload_type &payload, const RegisteredEvent &event,
                                             std::list<std::string> &list_of_responses) {
  EventManager::GetInstance()->publish(event_class, event_name, payload, event, list_of_responses);
}

} // namespace events
} // namespace sdk
} // namespace next
