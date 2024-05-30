/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     play_handler.h
 * @brief    small module to handle play command
 **/

#ifndef CONTROLBRIDGE_PLAYHANDLER_H
#define CONTROLBRIDGE_PLAYHANDLER_H

#include "player_control_manager.h"

namespace next {
namespace controlbridge {

class PlayHandler {
public:
  PlayHandler() = default;
  virtual ~PlayHandler() = default;
  PlayHandler(bool f, std::vector<std::string> fp, command_handler::SimulationType simul,
              std::shared_ptr<next::control::events::PlayerCommandEvent> ecal)
      : forward_(f), file_path_(fp), command_handler_client_(ecal), simul_type_(simul) {}
  //! Depending on the file extension call the NextPlayer Play method or start the Carmaker in headless mode
  /*!
   *
   * @param response_callback On receiving response from the NextPlayer pass it to the callback method
   */
  void PlayRecording([[maybe_unused]] std::function<void(command_handler::Response)> response_callback);

  //! Depending on the file extension call the NextPlayer Play method or start the Carmaker in headless mode
  /*!
   *
   * @param response_callback On receiving response from the NextPlayer pass it to the callback method
   */
  void PlayRecordingUntil([[maybe_unused]] std::function<void(command_handler::Response)> response_callback,
                          const std::string &play_until_timestamp);

  //! String split helper function
  /*!
   *
   * @param s string to split
   * @param delim delimiter to split the string
   */
  std::vector<std::string> split(const std::string &s, char delim);

private:
  void Play(const next::control::message::request &message);
  bool forward_;
  std::vector<std::string> file_path_; //!< Full path of the file
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client_;                 //! client to send requests to ecal server
  command_handler::SimulationType simul_type_; //! Simulation type
};

} // namespace controlbridge
} // namespace next

#endif
