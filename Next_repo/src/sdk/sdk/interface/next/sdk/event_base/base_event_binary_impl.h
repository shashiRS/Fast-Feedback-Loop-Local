/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     base_event_binary_impl.h
 * @brief    Implementation of binary event
 *
 **/
#ifndef NEXTSDK_BASE_EVENT_BINARY_IMPL_H_
#define NEXTSDK_BASE_EVENT_BINARY_IMPL_H_

#include <list>
#include <string>

#include "../sdk_config.hpp"
#include "event_types.h"

namespace next {
namespace sdk {
namespace events {

class DECLSPEC_nextsdk BaseEventBinaryImpl {
private:
public:
  bool initializeBinaryEvent(const std::string &event_class, const std::string &event_name,
                             const RegisteredEvent &event);
  bool initializeBinaryEventBlocking(const std::string &event_class, const std::string &event_name,
                                     const RegisteredEvent &event);
  bool subscribe(const std::string &event_class, const std::string &event_name, const RegisteredEvent &event);
  bool publish(const std::string &event_class, const std::string &event_name, const payload_type &payload,
               const RegisteredEvent &event, std::list<std::string> &list_of_responses);
  bool listen(const std::string event_class, const std::string event_name, const RegisteredEvent &event);
  bool call(const std::string event_class, const std::string event_name, const payload_type &payload,
            const RegisteredEvent &event, std::list<std::string> &list_of_responses);
  int getNumberOfSubscribers(const std::string event_class, const std::string event_name);
  bool unregister(const std::string event_class, const std::string event_name, const RegisteredEvent &event);
  void requestEventUpdate(const std::string &event_class, const std::string &event_name, const payload_type &payload,
                          const RegisteredEvent &event, std::list<std::string> &list_of_responses);
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_BASE_EVENT_BINARY_IMPL_H_
