/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     close_handler.cpp
 * @brief    implementation of small module to handle close command
 **/

#include "close_handler.h"

#include <chrono>
#include <map>
#include <thread>

#include <boost/filesystem/path.hpp>

#include <next/sdk/sdk_macros.h>
#include <next/sdk/logger/logger.hpp>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {
//! Kill the Next Player or Carmaker executable
//! @todo Implement the same for linux
void CloseHandler::CloseRecording([[maybe_unused]] std::function<void(command_handler::Response)> response_callback,
                                  std::string exe_name) {

  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;
  res.cmd = gui_command_parser::GuiCommand::CLOSE;
  std::string kill_command;

  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::CLOSE;
  message.id = 0;

  if (simul_type == command_handler::SimulationType::OpenLoop) {
    command_handler_client->publish(message);
  } else if (simul_type == command_handler::SimulationType::CloseLoop) {
    command_handler_client->publish(message);
    // kill_command = "taskkill /IM \"Next_Player_Player.exe\" /F";

    boost::filesystem::path p(exe_name);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    kill_command = "taskkill /IM \"" + p.filename().generic_string() + "\" /F";
    NEXT_DISABLE_SPECIFIC_WARNING_LINUX("-Wunused-result")
    std::system(kill_command.c_str());
    NEXT_RESTORE_WARNINGS_LINUX
    info(__FILE__, "Closing " + p.filename().generic_string());
  } else {
    warn(__FILE__, "Simulation type is not set; Cannot handle \"Close\" command!");
  }
}
} // namespace controlbridge
} // namespace next
