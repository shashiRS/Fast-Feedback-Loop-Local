/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     step_handler.cpp
 * @brief
 **/

#include "step_handler.h"

#include <next/sdk/logger/logger.hpp>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {
void StepHandler::step(std::function<void(command_handler::Response)> response_callback, int32_t step) {
  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::STEPFORWARD;
  message.id = 0;
  message.play_forward = true;
  message.requested_steps = step;

  if (simul_type == command_handler::SimulationType::OpenLoop) {
    command_handler_client->publish(message);
  } else if (simul_type == command_handler::SimulationType::CloseLoop) {
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
