/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_SDK
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cycle_information_event.h
 * @brief    cycle event information is the event which would send and receive cycle information
 *
 **/
#ifndef NEXT_SDK_CYCLE_EVENT_INFORMATION_H_
#define NEXT_SDK_CYCLE_EVENT_INFORMATION_H_

#include <string>
#include <vector>

#include "../event_base/base_event_binary.h"
#include "../sdk_config.hpp"
#include "../sdk_macros.h"

namespace next {
namespace sdk {
namespace events {

typedef std::vector<std::pair<std::string, uint32_t>> CycleInformation;

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk CycleInformationEvent : public BaseEventBinary<CycleInformation> {
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  CycleInformationEvent(std::string name) : BaseEventBinary<CycleInformation>(name) { initialize(); }

protected:
  payload_type serializeEventPayload(const CycleInformation &message) const override;
  CycleInformation deserializeEventPayload(const payload_type &vector_buf) const override;
};
} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_SDK_CYCLE_EVENT_INFORMATION_H_
