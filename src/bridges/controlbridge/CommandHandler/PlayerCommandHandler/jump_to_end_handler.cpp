#include "jump_to_end_handler.h"

#include "player_control_manager.h"

namespace next {
namespace controlbridge {
JumpToEndHandler::JumpToEndHandler(command_handler::SimulationType simul_,
                                   std::shared_ptr<next::control::events::PlayerCommandEvent> ecal_)
    : command_handler_client(ecal_), simul_type(simul_) {
  // nothing to do
}

void JumpToEndHandler::JumpToEnd(std::function<void(command_handler::Response)> response_callback) {
  next::control::message::request message{};
  message.command = next::control::commands::PlayerCommand::JUMP; // handle properlu jump to end
  message.id = 0;
  message.requested_timestamp = UINT64_MAX;

  if (simul_type == command_handler::SimulationType::OpenLoop) {
    command_handler_client->publish(message);
  } else if (simul_type == command_handler::SimulationType::CloseLoop) {
    // command_handler_client->HandleCommand(commands::Command::REWIND);
  }

  next::controlbridge::command_handler::Response res;
  std::map<std::string, std::string> payload;
  res.cmd = gui_command_parser::GuiCommand::JUMPTOEND;
  res.success = true;
  res.payload = payload;
  response_callback(res);
}
} // namespace controlbridge
} // namespace next
