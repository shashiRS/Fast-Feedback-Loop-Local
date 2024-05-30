/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_command.h
 * @brief    meta event for Next Player Control
 *
 **/
#ifndef NEXT_CONTROL_PLAYER_COMMAND_H_
#define NEXT_CONTROL_PLAYER_COMMAND_H_

#include <sstream>
#include <string>

#include <next/sdk/event_base/base_event_binary_blocking.h>
#include <next/sdk/sdk_macros.h>

#include "../control_config.h"

namespace next {
namespace control {
namespace commands {

enum class PlayerCommand : uint8_t {
  NONE,
  OPEN,
  PAUSE,
  CLOSE,
  JUMP,
  STEPFORWARD,
  STEPFORWARD_UNTIL
}; //!< Enum of commands
} // namespace commands

namespace message {
struct request {
  int32_t id = 0;
  commands::PlayerCommand command = commands::PlayerCommand::NONE;
  std::vector<std::string> file_path;
  bool play_forward = true;
  int32_t requested_steps = 0;
  uint64_t requested_timestamp = 0;
  uint64_t play_until_timestamp = 0;
  std::string timestamp_type = "";
  float sil = 0.0;
};
struct response {
  std::string status;
};

inline bool operator==(const request &lhs, const request &rhs) {
  return lhs.id == rhs.id && lhs.command == rhs.command && lhs.file_path == rhs.file_path &&
         lhs.play_forward == rhs.play_forward && lhs.requested_steps == rhs.requested_steps &&
         lhs.requested_timestamp == rhs.requested_timestamp && lhs.timestamp_type == rhs.timestamp_type &&
         lhs.sil == rhs.sil && lhs.play_until_timestamp == rhs.play_until_timestamp;
}

} // namespace message
} // namespace control
} // namespace next

namespace next {
namespace control {
namespace events {

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class CONTROL_LIBRARY_EXPORT PlayerCommandEvent
    : public next::sdk::events::BaseEventBinaryBlocking<next::control::message::request> {

public:
  PlayerCommandEvent(std::string name)
      : next::sdk::events::BaseEventBinaryBlocking<next::control::message::request>(name) {}
  void publish(const next::control::message::request &message);

protected:
  // fills internal payload struct from protobuf -> updates header automatically
  next::sdk::events::payload_type serializeEventPayload(const next::control::message::request &message) const override;
  next::control::message::request deserializeEventPayload(const next::sdk::events::payload_type &buf) const override;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace events
} // namespace control
} // namespace next

#endif // NEXT_CONTROL_PLAYER_COMMAND_H_
