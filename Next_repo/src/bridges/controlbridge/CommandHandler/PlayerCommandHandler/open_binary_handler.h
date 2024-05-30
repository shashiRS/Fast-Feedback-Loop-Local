/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_binary_handler.h
 * @brief    small module to handle open binary command
 **/

#ifndef CONTROLBRIDGE_OPENBINARYHANDLER_H
#define CONTROLBRIDGE_OPENBINARYHANDLER_H

#include <string>
#include "player_control_manager.h"

namespace next {
namespace controlbridge {
class OpenBinaryHandler {
public:
  OpenBinaryHandler() = default;
  virtual ~OpenBinaryHandler() = default;

  OpenBinaryHandler(std::string fp, command_handler::SimulationType simul_,
                    std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : binary_path(fp), simul_type(simul_), command_handler_client(ecal_) {}

  void open_binary(std::function<void(command_handler::Response)> response_callback);

private:
  std::string binary_path;                    //! binary path
  command_handler::SimulationType simul_type; //! Simulation type
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client; //! client to send requests to ecal server
};
} // namespace controlbridge
} // namespace next
#endif
