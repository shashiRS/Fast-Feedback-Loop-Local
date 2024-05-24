/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_file_handler.h
 * @brief    small module to handle open command
 **/

#ifndef CONTROLBRIDGE_OPENFILEHANDLER_H
#define CONTROLBRIDGE_OPENFILEHANDLER_H

#define DEFAULT_TESTRUN "CL_eCAL_CCRM.testrun"

#include <string>
#include <string_view>
#include "player_control_manager.h"

namespace next {
namespace controlbridge {
class OpenFileHandler {
public:
  OpenFileHandler() = default;
  virtual ~OpenFileHandler() = default;

  OpenFileHandler(std::vector<std::string> fp, command_handler::SimulationType simul_,
                  std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
      : file_path(fp), simul_type(simul_), command_handler_client(ecal_) {}
  //! Depending on the file extension call the NextPlayer open method or do nothing
  /*!
   *
   * @param response_callback On receiving response from the NextPlayer pass it to the callback method
   */
  void openFile(std::function<void(command_handler::Response)> response_callback);

private:
  std::vector<std::string> file_path;         //! recording path
  command_handler::SimulationType simul_type; //! Simulation type
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client; //! client to send requests to ecal server
};
} // namespace controlbridge
} // namespace next
#endif
