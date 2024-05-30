/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */

#ifndef CONTROLBRIDGE_UPDATEDSILFACTORHANDLER_H
#define CONTROLBRIDGE_UPDATEDSILFACTORHANDLER_H

#include <next/appsupport/config/config_server.hpp>
#include "player_control_manager.h"

namespace next {
namespace controlbridge {
class UpdateSilFactorHandler {
public:
  UpdateSilFactorHandler() = default;
  virtual ~UpdateSilFactorHandler() = default;

  UpdateSilFactorHandler(float updatedSilFactor, command_handler::SimulationType simul_,
                         std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : updatedSilFactor_(updatedSilFactor), command_handler_client(ecal_), simul_type(simul_) {}

  void updateSilFactor(std::function<void(command_handler::Response)> response_callback, float updatedSilFactor,
                       std::shared_ptr<next::appsupport::ConfigServer> configServer);

private:
  float updatedSilFactor_;
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client;                 //! client to send requests to ecal server
  command_handler::SimulationType simul_type; //! Simulation type
};
} // namespace controlbridge
} // namespace next

#endif
