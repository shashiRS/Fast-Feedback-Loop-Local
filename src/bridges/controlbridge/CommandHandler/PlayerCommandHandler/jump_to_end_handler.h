
#ifndef CONTROLBRIDGE_JUMPTOENDHANDLER_H
#define CONTROLBRIDGE_JUMPTOENDHANDLER_H

#include "player_control_manager.h"
namespace next {
namespace controlbridge {
class JumpToEndHandler {
public:
  JumpToEndHandler() = default;
  virtual ~JumpToEndHandler() = default;

  JumpToEndHandler(command_handler::SimulationType simul_,
                   std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_);

  void JumpToEnd(std::function<void(command_handler::Response)> response_callback);

private:
  std::shared_ptr<next::control::events::PlayerCommandEvent>
      command_handler_client;                 //! client to send requests to ecal server
  command_handler::SimulationType simul_type; //! Simulation type
};
} // namespace controlbridge
} // namespace next

#endif
