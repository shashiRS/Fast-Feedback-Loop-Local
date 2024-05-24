/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_event_binary.cpp
 * @brief    meta event for Next Player Control
 *
 **/

#include <next/control/event_types/orchestrator_event_binary.h>

#include <cstring>

namespace next {
namespace control {
namespace events {

struct OrchestratorTriggerMsg {
  uint64_t timestamp;
  char triggerid[256];
};

OrchestratorTriggerMsg CreateMsgFromOrchestratorTrigger(const OrchestratorTrigger &message) {
  OrchestratorTriggerMsg msg;
  msg.timestamp = std::stoull(message.trigger_timestamp);
  size_t max_size = message.flow_id.size() > 256 ? 256 : message.flow_id.size();
  memset(msg.triggerid, 0, 256);
  memcpy(msg.triggerid, message.flow_id.c_str(), max_size);
  return msg;
}

next::sdk::events::payload_type
OrchestratorEventBinary::serializeEventPayload(const OrchestratorTrigger &message) const {
  next::sdk::events::payload_type payload;
  payload.resize(sizeof(OrchestratorTriggerMsg));

  OrchestratorTriggerMsg msg = CreateMsgFromOrchestratorTrigger(message);
  if (sizeof(OrchestratorTriggerMsg) == payload.size()) {
    memcpy(payload.data(), &msg, sizeof(OrchestratorTriggerMsg));
  }
  return payload;
}

OrchestratorTrigger
OrchestratorEventBinary::deserializeEventPayload(const next::sdk::events::payload_type &strbuf) const {
  OrchestratorTriggerMsg msg;

  if (sizeof(OrchestratorTriggerMsg) == strbuf.size()) {
    memcpy(&msg, strbuf.data(), strbuf.size());
  }

  OrchestratorTrigger message{};
  message.flow_id = std::string(msg.triggerid);
  message.trigger_timestamp = std::to_string(msg.timestamp);

  return message;
}

} // namespace events
} // namespace control
} // namespace next
