/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_recording_status_event.h
 * @brief    meta event for Next Player open recording status
 *
 **/
#ifndef OPEN_RECORDING_STATUS_EVENT_H_
#define OPEN_RECORDING_STATUS_EVENT_H_

#include <string>

#include <next/sdk/event_base/base_event_binary.h>
#include <next/sdk/sdk_macros.h>

#include "../control_config.h"

namespace next {
namespace control {
namespace events {

/*!
 * @brief data the meta event contains
 * @brief Structure of the NEXT Meta Information messages for NEXT Player
 *
 * executed_function  Name of the current executed function
 * description        Name of the current processed item
 * areIndexesUsed     Flag to mark if the max and current index values are used
 * maxIndex           Total number of items to process in the current function
 * currentIndex       The index of the current processed item
 */
struct OpenRecordingStatusInformationMessage {
  std::string executed_function = "";
  std::string description = "";
  bool areIndexesUsed = true;
  size_t maxIndex = 0;
  size_t currentIndex = 0;
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class CONTROL_LIBRARY_EXPORT OpenRecordingStatusEvent
    : public next::sdk::events::BaseEventBinary<OpenRecordingStatusInformationMessage> {

public:
  OpenRecordingStatusEvent(std::string name)
      : next::sdk::events::BaseEventBinary<OpenRecordingStatusInformationMessage>(name) {
    initialize();
  }

  void publish(const OpenRecordingStatusInformationMessage &message);
  //! calls BaseEventBinary::publish with message_
  void send();

  // protected:
  next::sdk::events::payload_type serializeEventPayload(const OpenRecordingStatusInformationMessage &message)
      const override; // fills internal payload -> updates header automatically
  OpenRecordingStatusInformationMessage
  deserializeEventPayload(const next::sdk::events::payload_type &buf) const override; // returns the struct payload

  OpenRecordingStatusInformationMessage message_;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace events
} // namespace control
} // namespace next

#endif // OPEN_RECORDING_STATUS_EVENT_H_
