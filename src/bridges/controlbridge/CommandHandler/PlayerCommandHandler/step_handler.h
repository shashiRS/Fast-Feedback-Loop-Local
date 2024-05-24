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

#ifndef CONTROLBRIDGE_STEPHANDLER_H
#define CONTROLBRIDGE_STEPHANDLER_H

#include "player_control_manager.h"

namespace next {
namespace controlbridge {
class StepHandler {
public:
  StepHandler() = default;
  virtual ~StepHandler() = default;

  StepHandler(int32_t step, command_handler::SimulationType simul_,
              std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : step_(step), command_handler_client(ecal_), simul_type(simul_) {}

  void step(std::function<void(command_handler::Response)> response_callback, int32_t step);

private:
  int32_t step_;
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client;                 //! client to send requests to ecal server
  command_handler::SimulationType simul_type; //! Simulation type
};
} // namespace controlbridge
} // namespace next

#endif // !1
