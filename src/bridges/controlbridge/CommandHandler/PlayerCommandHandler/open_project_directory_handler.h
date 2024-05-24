/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_project_directory_handler.h
 * @brief    small module to handle open project directory command
 **/

#ifndef CONTROLBRIDGE_OPENPROJECTDIRECTORYHANDLER_H
#define CONTROLBRIDGE_OPENPROJECTDIRECTORYHANDLER_H

#include <string>
#include "player_control_manager.h"

namespace next {
namespace controlbridge {
class OpenProjectDirectoryHandler {
public:
  OpenProjectDirectoryHandler() = default;
  virtual ~OpenProjectDirectoryHandler() = default;

  OpenProjectDirectoryHandler(std::string fp, command_handler::SimulationType simul_,
                              std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : conf_path(fp), simul_type(simul_), command_handler_client(ecal_) {}

  void open_project_directory(std::function<void(command_handler::Response)> response_callback);

private:
  std::string conf_path;                      //! conf path
  command_handler::SimulationType simul_type; //! Simulation type
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client; //! client to send requests to ecal server
};
} // namespace controlbridge
} // namespace next
#endif
