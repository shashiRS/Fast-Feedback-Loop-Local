/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     fastforward_handler.h
 * @brief    small module to handle fastforward command
 **/

#ifndef CONTROLBRIDGE_FASTFORWARDHANDLER_H
#define CONTROLBRIDGE_FASTFORWARDHANDLER_H
#include "player_control_manager.h"

namespace next {
namespace controlbridge {

class FastForwardHandler {
public:
  FastForwardHandler() = default;
  virtual ~FastForwardHandler() = default;
  FastForwardHandler(command_handler::SimulationType simul_,
                     std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : simul_type(simul_), command_handler_client(ecal_) {}
  //! Rewind the recording
  /*!
   *
   * @param response_callback On receiving response from the NextPlayer pass it to the callback method
   */
  void FastForwardRecording(std::function<void(command_handler::Response)> response_callback);

private:
  command_handler::SimulationType simul_type; //! Simulation type
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client; //! client to send requests to ecal server
};
} // namespace controlbridge
} // namespace next
#endif
