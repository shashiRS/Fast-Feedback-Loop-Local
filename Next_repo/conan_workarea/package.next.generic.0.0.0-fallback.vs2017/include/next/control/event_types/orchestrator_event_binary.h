/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_event_binary.h
 * @brief    meta event for Next Player Control
 *
 **/
#ifndef NEXT_ORCHESTRATOR_EVENT_BINARY_H_
#define NEXT_ORCHESTRATOR_EVENT_BINARY_H_

#include <memory>

#include <next/sdk/event_base/base_event_binary_blocking.h>
#include <next/sdk/sdk_macros.h>

#include <next/control/control_config.h>

namespace next {
namespace control {
namespace events {

/*!
 * @brief not yet used
 */
struct OrchestratorTrigger {
  std::string trigger_timestamp = "0";
  std::string flow_id = "";
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class CONTROL_LIBRARY_EXPORT OrchestratorEventBinary
    : public next::sdk::events::BaseEventBinaryBlocking<OrchestratorTrigger> {

public:
  OrchestratorEventBinary(std::string name) : next::sdk::events::BaseEventBinaryBlocking<OrchestratorTrigger>(name) {}

protected:
  // fills internal payload struct from protobuf -> updates header automatically
  next::sdk::events::payload_type serializeEventPayload(const OrchestratorTrigger &message) const override;

  OrchestratorTrigger deserializeEventPayload(const next::sdk::events::payload_type &buf) const override;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace events
} // namespace control
} // namespace next

#endif // NEXT_ORCHESTRATOR_EVENT_BINARY_H_
