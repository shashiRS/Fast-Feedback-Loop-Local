/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     rewind_handler.cpp
 * @brief    implementation of small module to handle rewind command
 **/

#include "rewind_handler.h"

#include <chrono>
#include <map>
#include <thread>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

void RewindHandler::RewindRecording([[maybe_unused]] std::function<void(command_handler::Response)> response_callback) {
  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::JUMP; // REWIND;
  message.id = 0;
  message.requested_timestamp = 0;

  if (simul_type == command_handler::SimulationType::OpenLoop) {
    command_handler_client->publish(message);
  } else if (simul_type == command_handler::SimulationType::CloseLoop) {
    // command_handler_client->HandleCommand(commands::Command::REWIND);
  }

  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;
  res.cmd = gui_command_parser::GuiCommand::REWIND;
  res.success = true;
  res.payload = payload;
  // response_callback(res);
}

} // namespace controlbridge
} // namespace next
