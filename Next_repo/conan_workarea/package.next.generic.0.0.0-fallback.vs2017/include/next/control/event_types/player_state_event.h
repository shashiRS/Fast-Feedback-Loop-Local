/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_state_event.h
 * @brief    meta event for Next Player Control
 *
 **/
#ifndef NEXT_CONTROL_PLAYER_STATE_EVENT_H_
#define NEXT_CONTROL_PLAYER_STATE_EVENT_H_

#include <string>

#include <next/sdk/event_base/base_event_binary.h>
#include <next/sdk/sdk_macros.h>

#include "../control_config.h"

namespace next {
namespace control {
namespace events {
/*!
 * @brief possible states of a Player information message
 *
 */
enum class PlayerState : int {
  UNSPECIFIED = 0,
  ENTER_NEW = 1,
  ON_NEW = 2,
  EXIT_NEW = 3,
  ENTER_OPENING = 4,
  ON_OPENING = 5,
  EXIT_OPENING = 6,
  ENTER_READY = 7,
  ON_READY = 8,
  EXIT_READY = 9,
  ENTER_PLAY = 10,
  ON_PLAY = 11,
  EXIT_PLAY = 12,
  ENTER_UNLOAD_RECORDING = 13,
  ON_UNLOAD_RECORDING = 14,
  EXIT_UNLOAD_RECORDING = 15,
  ENTER_SKIP_TO = 16,
  ON_SKIP_TO = 17,
  EXIT_SKIP_TO = 18,
  ENTER_ERROR = 19,
  ON_ERROR = 20,
  EXIT_ERROR = 21,
  ENTER_SHUTDOWN = 22,
  ON_SHUTDOWN = 23,
  EXIT_SHUTDOWN = 24
};

/*!
 * @brief data the meta event contains
 * @brief Structure of the NEXT Meta Information messages for NEXT Player
 *
 * recording_name  Name of the recording that is loaded
 * max_timestamp   Maximal timestamp (length) of the recording
 * description     Current timestamp of the recording
 * status          Current status of the Player
 */
struct PlayerMetaInformationMessage {
  uint64_t min_timestamp = 0;
  uint64_t max_timestamp = 0;
  uint64_t timestamp = 0;
  float speed_factor = 0.0;
  PlayerState status = PlayerState::UNSPECIFIED;
  std::string recording_name = "";
  std::string stepping_type = "";
  uint64_t stepping_value = 0;
};

bool operator==(const PlayerMetaInformationMessage &lhs, const PlayerMetaInformationMessage &rhs);

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class CONTROL_LIBRARY_EXPORT PlayerStateEvent
    : public next::sdk::events::BaseEventBinary<PlayerMetaInformationMessage> {

public:
  PlayerStateEvent(std::string name) : next::sdk::events::BaseEventBinary<PlayerMetaInformationMessage>(name) {
    initialize();
  }

  PlayerMetaInformationMessage _message;

  //! updates only the status in the message
  void updateStatus(PlayerState new_status);

  void publish(const PlayerMetaInformationMessage &message);

  //! calls BaseEventBinary::publish with _message
  void send();

  // protected:
  next::sdk::events::payload_type serializeEventPayload(const PlayerMetaInformationMessage &message)
      const override; // fills internal payload -> updates header automatically
  PlayerMetaInformationMessage deserializeEventPayload(const next::sdk::events::payload_type &buf) const override;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace events
} // namespace control
} // namespace next

#endif // NEXT_CONTROL_PLAYER_STATE_EVENT_H_
