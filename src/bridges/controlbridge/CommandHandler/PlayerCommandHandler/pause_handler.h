/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     pause_handler.h
 * @brief    small module to handle pause command
 **/

#ifndef CONTROLBRIDGE_PAUSEHANDLER_H
#define CONTROLBRIDGE_PAUSEHANDLER_H

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

class PauseHandler {
public:
  PauseHandler() = default;
  virtual ~PauseHandler() = default;
  PauseHandler(command_handler::SimulationType simul_, std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : command_handler_client(ecal_), simul_type(simul_) {}
  //! Pause the recording
  /*!
   *
   * @param response_callback On receiving response from the NextPlayer pass it to the callback method
   */
  void PauseRecording(std::function<void(command_handler::Response)> response_callback);

private:
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client;                 //! client to send requests to ecal server
  command_handler::SimulationType simul_type; //! Simulation type
};

} // namespace controlbridge
} // namespace next

#endif
