/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_project_directory_handler.cpp
 * @brief    implementation of small module to handle open command
 **/

#include "open_project_directory_handler.h"

#include <map>
#include <thread>

#include <next/sdk/logger/logger.hpp>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {
void OpenProjectDirectoryHandler::open_project_directory(
    std::function<void(command_handler::Response)> response_callback) {
  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;

  res.cmd = gui_command_parser::GuiCommand::DIRECTORYLOADED;
  res.success = true;
  res.payload = payload;
  response_callback(res);
}
} // namespace controlbridge
} // namespace next
