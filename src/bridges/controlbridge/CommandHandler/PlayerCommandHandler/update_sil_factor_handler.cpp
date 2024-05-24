/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     update_sil_factor_handler.cpp
 * @brief
 **/

#include "update_sil_factor_handler.h"

#include <next/appsupport/config/config_key_generator_player.hpp>

#include <next/sdk/logger/logger.hpp>

#include "player_control_manager.h"

namespace next {
namespace controlbridge {
void UpdateSilFactorHandler::updateSilFactor(std::function<void(command_handler::Response)> response_callback,
                                             float updatedSilFactor,
                                             std::shared_ptr<next::appsupport::ConfigServer> configServer) {
  // It is set to the Config server independent to the simulation type
  configServer->put(next::appsupport::configkey::player::getRealTimeFactorKey(), updatedSilFactor);
  configServer->pushGlobalConfig();

  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;
  res.cmd = gui_command_parser::GuiCommand::UPDATESILFACTOR; // UPDATESILFACTOR;
  res.success = true;
  res.payload = payload;
  response_callback(res);
}

} // namespace controlbridge
} // namespace next
