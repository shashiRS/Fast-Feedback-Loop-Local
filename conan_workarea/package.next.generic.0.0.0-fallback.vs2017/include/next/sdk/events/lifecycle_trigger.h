/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_SDK
 * ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_trigger.h
 * @brief    Implementation for event system
 *
 **/
#ifndef NEXT_SDK_LIFECYCLE_TRIGGER_EVENT_H_
#define NEXT_SDK_LIFECYCLE_TRIGGER_EVENT_H_

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

struct LifecycleTriggerMessage {
  // current timestamp as nanoseconds
  uint64_t timestamp;
  // see void LifecycleBase::callEvent(..) in src\appsupport\src\Lifecycle_Base.cpp e.g. RESET or SHUTDOWN
  std::string trigger_event;
  // if this is empty than this trigger message is addressing everyone otherwise the addressed component_name is
  // mentioned
  std::string component_name;
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk LifecycleTrigger : public BaseEventBinary<LifecycleTriggerMessage> {
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  LifecycleTrigger(std::string name) : BaseEventBinary<LifecycleTriggerMessage>(name) { initialize(); }

  // create a new LifecycleTriggerMessage with current timestamp and the given state string and publish it
  void publishTrigger(const std::string &trigger_event, const std::string &component_name = "");

protected:
  payload_type serializeEventPayload(const LifecycleTriggerMessage &message) const override;
  LifecycleTriggerMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_SDK_LIFECYCLE_TRIGGER_EVENT_H_
