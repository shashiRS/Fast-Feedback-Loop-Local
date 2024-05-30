/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_binary_handler.cpp
 * @brief    implementation of small module to handle open binary
 **/

#include "open_binary_handler.h"

#include <map>
#include <thread>

#include <next/sdk/logger/logger.hpp>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {
void OpenBinaryHandler::open_binary(std::function<void(command_handler::Response)> response_callback) {
  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;

  res.cmd = gui_command_parser::GuiCommand::BINARYLOADED;
  res.success = true;
  res.payload = payload;
  response_callback(res);
}
} // namespace controlbridge
} // namespace next
