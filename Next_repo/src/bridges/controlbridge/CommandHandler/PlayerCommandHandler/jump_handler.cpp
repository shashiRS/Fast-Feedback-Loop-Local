/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     jump_handler.cpp
 * @brief
 **/

#include "jump_handler.h"

#include <next/sdk/logger/logger.hpp>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

void JumpHandler::JumpToTimestamp(std::function<void(command_handler::Response)> response_callback, uint64_t timestamp,
                                  std::string timestamp_type) {
  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::JUMP;
  message.id = 0;
  message.play_forward = true;
  message.requested_timestamp = timestamp;
  message.timestamp_type = timestamp_type;
  message.requested_steps = 0;

  if (simul_type == command_handler::SimulationType::OpenLoop) {
    command_handler_client->publish(message);
  } else if (simul_type == command_handler::SimulationType::CloseLoop) {
  }

  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;
  res.cmd = gui_command_parser::GuiCommand::JUMP;
  res.success = true;
  res.payload = payload;
  response_callback(res);
}

} // namespace controlbridge
} // namespace next
