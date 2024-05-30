/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_SDK
 * ========================== NEXT Project ==================================
 */
/**
 * @file     session_event.hpp
 * @brief    session event is the event which sends and receives session information
 *
 **/
#ifndef NEXT_SDK_LOG_EVENT_H_
#define NEXT_SDK_LOG_EVENT_H_

#include <map>
#include <string>
#include <vector>

#include "../event_base/base_event_binary.h"
#include "../sdk_macros.h"

namespace next {
namespace sdk {
namespace events {

struct SessionClientInformation {
  std::string component_name;
  int heartbeat;
  std::string version;
  int64_t startup_time;
  int64_t current_time;
  bool shutdown;
};

struct SessionInformation {
  std::string session_id;
  std::string user_hash;
  std::string platform;
  std::string version;
  int heartbeat;
  std::unordered_map<std::string, SessionClientInformation> component_map;
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk SessionEvent : public BaseEventBinary<SessionClientInformation> {
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  SessionEvent(std::string name) : BaseEventBinary<SessionClientInformation>(name) { initialize(); }
  void publishLogEvent(const SessionClientInformation &session_information);

protected:
  payload_type serializeEventPayload(const SessionClientInformation &message) const override;
  SessionClientInformation deserializeEventPayload(const payload_type &buf) const override;
};
} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_SDK_LOG_EVENT_H_
