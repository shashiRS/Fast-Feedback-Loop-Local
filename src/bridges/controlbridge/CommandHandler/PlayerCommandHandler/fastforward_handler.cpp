/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     fastforward_handler.cpp
 * @brief    implementation of small module to handle fastforward command
 **/

#include <map>
#include <thread>

#include "fastforward_handler.h"
#include "player_control_manager.h"

namespace next {
namespace controlbridge {
void FastForwardHandler::FastForwardRecording(std::function<void(command_handler::Response)> response_callback) {

  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::STEPFORWARD; /// how to handle this
  message.id = 0;

  if (simul_type == command_handler::SimulationType::OpenLoop) {
    command_handler_client->publish(message);
  } else if (simul_type == command_handler::SimulationType::CloseLoop) {
    command_handler_client->publish(message);
  }

  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;
  res.cmd = gui_command_parser::GuiCommand::STEPFORWARD;
  res.success = true;
  res.payload = payload;
  response_callback(res);
}
} // namespace controlbridge
} // namespace next
