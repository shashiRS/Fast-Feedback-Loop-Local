/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     close_handler.h
 * @brief    small module to handle close command
 **/

#ifndef CONTROLBRIDGE_CLOSEHANDLER_H
#define CONTROLBRIDGE_CLOSEHANDLER_H

#include <next/control/event_types/player_command.h>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

class CloseHandler {
public:
  CloseHandler() = default;
  virtual ~CloseHandler() = default;

  CloseHandler(command_handler::SimulationType simul_, std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : simul_type(simul_), command_handler_client(ecal_) {}
  //! Close the recording
  /*!
   *
   * @param response_callback On receiving response from the NextPlayer pass it to the callback method
   */
  void CloseRecording(std::function<void(command_handler::Response)> response_callback, std::string exe_name);

private:
  command_handler::SimulationType simul_type; //! Simulation type
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client; //! client to send requests to ecal server
};
} // namespace controlbridge
} // namespace next
#endif
