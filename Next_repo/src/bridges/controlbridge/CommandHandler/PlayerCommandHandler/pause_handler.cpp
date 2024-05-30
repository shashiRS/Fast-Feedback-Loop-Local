/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     pause_handler.cpp
 * @brief    implementation of small module to handle pause command
 **/

#include "pause_handler.h"

#include <chrono>
#include <map>
#include <thread>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

void PauseHandler::PauseRecording([[maybe_unused]] std::function<void(command_handler::Response)> response_callback) {
  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::PAUSE;
  message.id = 0;

  if (simul_type == command_handler::SimulationType::OpenLoop) {
    command_handler_client->publish(message);
  } else if (simul_type == command_handler::SimulationType::CloseLoop) {
    command_handler_client->publish(message);
  }
}

} // namespace controlbridge
} // namespace next
