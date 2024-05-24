/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     play_handler.cpp
 * @brief    implementation of small module to handle play command
 **/

#include "play_handler.h"

#include <chrono>
#include <map>
#include <next/sdk/logger/logger.hpp>
#include <sstream>
#include <thread>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

std::vector<std::string> PlayHandler::split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

//! Start the Next_Player or Carmaker executable depending on the simulation type
//! @todo Implement the same for linux
//! @todo Get the executable path
//! @todo Get the conf file
void PlayHandler::PlayRecording([[maybe_unused]] std::function<void(command_handler::Response)> response_callback) {
  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::STEPFORWARD;
  message.file_path = file_path_;
  message.requested_steps = INT32_MAX;
  message.id = 0;
  message.play_forward = true;
  Play(message);
}

//! Start the Next_Player or Carmaker executable depending on the simulation type
//! @todo Implement the same for linux
//! @todo Get the executable path
//! @todo Get the conf file
void PlayHandler::PlayRecordingUntil([[maybe_unused]] std::function<void(command_handler::Response)> response_callback,
                                     const std::string &play_until_timestamp) {
  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::STEPFORWARD_UNTIL;
  message.play_until_timestamp = std::stoull(play_until_timestamp);
  message.file_path = file_path_;
  message.requested_steps = INT32_MAX;
  message.id = 0;
  message.play_forward = true;
  Play(message);
}

void PlayHandler::Play(const next::control::message::request &message) {

  std::string log_path = "";
  for (auto path : file_path_) {
    log_path += path;
    log_path += "+";
  }

  if (simul_type_ == command_handler::SimulationType::OpenLoop) {
    info(__FILE__, "Request playing recording in Open Loop Simulation {0}", log_path);
    command_handler_client_->publish(message);
  } else if (simul_type_ == command_handler::SimulationType::CloseLoop) {
    info(__FILE__, "Request playing Testrun in Close Loop Simulation {0}", log_path);
    command_handler_client_->publish(message);
  }
}
} // namespace controlbridge
} // namespace next
