/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     rewind_handler.h
 * @brief    small module to handle rewind command
 **/

#ifndef CONTROLBRIDGE_REWINDHANDLER_H
#define CONTROLBRIDGE_REWINDHANDLER_H
#include "player_control_manager.h"
namespace next {
namespace controlbridge {

class RewindHandler {
public:
  RewindHandler() = default;
  virtual ~RewindHandler() = default;
  RewindHandler(command_handler::SimulationType simul_,
                std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : simul_type(simul_), command_handler_client(ecal_) {}

  //! Rewind the recording
  /*!
   *
   * @param response_callback On receiving response from the NextPlayer pass it to the callback method
   */
  void RewindRecording(std::function<void(command_handler::Response)> response_callback);

private:
  command_handler::SimulationType simul_type; //! Simulation type
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client; //! client to send requests to ecal server
};
} // namespace controlbridge
} // namespace next
#endif
