/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_file_handler.cpp
 * @brief    implementation of small module to handle open command
 **/

#include "open_file_handler.h"

#include <thread>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

//! Start the Next Player or Carmaker executable depending on the Simulation type
//! @todo Implement the same for linux
//! @todo Remove hard-coded NextPlayer path
//! @todo Sending PlaybackIsPaused on RecordingLoad Finished to make GUI work. Correct it
void OpenFileHandler::openFile([[maybe_unused]] std::function<void(command_handler::Response)> response_callback) {

  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::OPEN;
  message.file_path = file_path;
  message.id = 0;
  command_handler_client->publish(message);
}
} // namespace controlbridge
} // namespace next
