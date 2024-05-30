/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_SDK
 * ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_state.h
 * @brief    Implementation for event system
 *
 **/
#ifndef NEXT_SDK_LIFECYCLE_STATE_EVENT_H_
#define NEXT_SDK_LIFECYCLE_STATE_EVENT_H_

#include <string.h>
#include <atomic>
#include <map>
#include <memory>

#include "../event_base/base_event_binary.h"
#include "../sdk_config.hpp"
#include "../sdk_macros.h"

namespace next {
namespace sdk {
namespace events {

struct LifecycleStateMessage {
  // current timestamp as nanoseconds
  uint64_t timestamp;
  // from next::appsupport::lifecyclestatemachine:EState as string
  std::string state;
  std::string component_name;
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk LifecycleState : public BaseEventBinary<LifecycleStateMessage> {
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  LifecycleState(std::string name) : BaseEventBinary<LifecycleStateMessage>(name) { initialize(); }

  // create a new LifecycleStateMessage with current timestamp and the given state string and publish it
  void publishState(const std::string &state, const std::string &component_name);

protected:
  payload_type serializeEventPayload(const LifecycleStateMessage &message) const override;
  LifecycleStateMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_SDK_LIFECYCLE_STATE_EVENT_H_
