/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     jump_handler.h
 * @brief
 **/

#ifndef CONTROLBRIDGE_JUMPHANDLER_H
#define CONTROLBRIDGE_JUMPHANDLER_H

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

class JumpHandler {
public:
  JumpHandler() = default;
  virtual ~JumpHandler() = default;

  JumpHandler(uint64_t timestamp, command_handler::SimulationType simul_,
              std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : timestamp_(timestamp), command_handler_client(ecal_), simul_type(simul_) {}

  void JumpToTimestamp(std::function<void(command_handler::Response)> response_callback, uint64_t timestamp,
                       std::string timestamp_type);

private:
  uint64_t timestamp_;
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client;                 //! client to send requests to ecal server
  command_handler::SimulationType simul_type; //! Simulation type
};

} // namespace controlbridge
} // namespace next

#endif
