/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     player_control_manager.cpp
 * @brief    implementation of player_control_manager module
 *
 */

#include "player_control_manager.h"

#include <chrono>
#include <functional>
#include <future>

#include <boost/filesystem.hpp>

#include <next/sdk/logger/logger.hpp>

#include <next/appsupport/next_status_code/next_status_code.hpp>
#include "PlayerCommandHandler/close_handler.h"
#include "PlayerCommandHandler/fastforward_handler.h"
#include "PlayerCommandHandler/jump_handler.h"
#include "PlayerCommandHandler/jump_to_end_handler.h"
#include "PlayerCommandHandler/open_binary_handler.h"
#include "PlayerCommandHandler/open_file_handler.h"
#include "PlayerCommandHandler/open_project_directory_handler.h"
#include "PlayerCommandHandler/pause_handler.h"
#include "PlayerCommandHandler/play_handler.h"
#include "PlayerCommandHandler/rewind_handler.h"
#include "PlayerCommandHandler/step_handler.h"
#include "PlayerCommandHandler/update_sil_factor_handler.h"
#include "lifecycle_orchestrator.h"

namespace next {
namespace controlbridge {
namespace command_handler {

PlayerControlManager::PlayerControlManager() {}

std::string PlayerControlManager::GetStringParam(const std::map<std::string, std::vector<std::string>> &params,
                                                 const std::string &key) {
  auto it = params.find(key);
  if (it != params.end() && !it->second.empty()) {
    return it->second.front();
  }
  return ""; // Return an empty string if the key is not found or the vector is empty
}

std::vector<std::string>
PlayerControlManager::GetStringListParam(const std::map<std::string, std::vector<std::string>> &params,
                                         const std::string &key) {
  auto it = params.find(key);
  if (it != params.end() && !it->second.empty()) {
    return it->second;
  }
  return {}; // Return an empty string if the key is not found or the vector is empty
}

void PlayerControlManager::HandleOpenFile() {
  //  check player status here
  debug(__FILE__, "Open file: {0}", current_filepath.front());
  threads.push_back(std::thread([this] {
    OpenFileHandler f(this->current_filepath /*vector*/, this->simul_type, this->player_command_handler_);
    f.openFile(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

void PlayerControlManager::HandlePlay(bool forward) {
  debug(__FILE__, "Handle Play");
  threads.push_back(std::thread([this, forward] {
    PlayHandler ph(forward, this->current_filepath, this->simul_type, this->player_command_handler_);
    ph.PlayRecording(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

void PlayerControlManager::HandlePlayUntil(bool forward, const std::string &play_until_timestamp) {
  debug(__FILE__, "Handle Play");
  threads.push_back(std::thread([this, forward, play_until_timestamp] {
    PlayHandler ph(forward, this->current_filepath, this->simul_type, this->player_command_handler_);
    ph.PlayRecordingUntil(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1),
                          play_until_timestamp);
  }));
}

void PlayerControlManager::HandlePause() {
  debug(__FILE__, "Handle Pause");
  threads.push_back(std::thread([this] {
    PauseHandler ph(this->simul_type, this->player_command_handler_);
    ph.PauseRecording(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

void PlayerControlManager::HandleRewind() {
  debug(__FILE__, "Handle Rewind");
  threads.push_back(std::thread([this] {
    RewindHandler rh(this->simul_type, this->player_command_handler_);
    rh.RewindRecording(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

void PlayerControlManager::HandleJump(uint64_t timestamp, std::string timestamp_type) {
  debug(__FILE__, "Handle JumpToTimestamp");
  threads.push_back(std::thread([timestamp, timestamp_type, this] {
    JumpHandler ch(timestamp, this->simul_type, this->player_command_handler_);
    ch.JumpToTimestamp(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1), timestamp,
                       timestamp_type);
  }));
}

void PlayerControlManager::HandleStepForward(int32_t step) {
  debug(__FILE__, "Handle StepForward");
  threads.push_back(std::thread([step, this] {
    StepHandler ch(step, this->simul_type, this->player_command_handler_);
    ch.step(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1), step);
  }));
}

void PlayerControlManager::HandleUpdateSilFactor(float updatedSilFactor) {
  debug(__FILE__, "Handle UpdatedSilFactor");
  threads.push_back(std::thread([updatedSilFactor, this] {
    UpdateSilFactorHandler ch(updatedSilFactor, this->simul_type, this->player_command_handler_);
    ch.updateSilFactor(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1),
                       updatedSilFactor, config_server_);
  }));
}

void PlayerControlManager::HandleJumpToEnd() {
  debug(__FILE__, "Handle Jump To End");
  threads.push_back(std::thread([this] {
    JumpToEndHandler jteH(this->simul_type, this->player_command_handler_);
    jteH.JumpToEnd(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

void PlayerControlManager::HandleFastForward() {
  debug(__FILE__, "Handle FastForward");
  threads.push_back(std::thread([this] {
    FastForwardHandler ffh(this->simul_type, this->player_command_handler_);
    ffh.FastForwardRecording(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

void PlayerControlManager::HandleClose() {
  debug(__FILE__, "Handle Close");
  threads.push_back(std::thread([this] {
    CloseHandler ch(this->simul_type, this->player_command_handler_);
    ch.CloseRecording(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1), exe_path);
  }));
}

void PlayerControlManager::HandleOpenProjectDirectory() {
  debug(__FILE__, "Open project directory: {0}", conf_path);
  debug(__FILE__, "Handle Open Project Directory");
  threads.push_back(std::thread([this] {
    OpenProjectDirectoryHandler opdh(this->conf_path, this->simul_type, this->player_command_handler_);
    opdh.open_project_directory(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

void PlayerControlManager::HandleOpenBinary() {
  debug(__FILE__, "Open binary: {0}", exe_path);
  debug(__FILE__, "Handle Open Binary");
  threads.push_back(std::thread([this] {
    OpenBinaryHandler obh(this->exe_path, this->simul_type, this->player_command_handler_);
    obh.open_binary(std::bind(&PlayerControlManager::ResponseCallback, this, std::placeholders::_1));
  }));
}

bool PlayerControlManager::HandleCommand(const next::controlbridge::gui_command_parser::GuiCommand cmd,
                                         std::map<std::string, std::vector<std::string>> params) {
  last_cmd = next::controlbridge::gui_command_parser::GuiCommand::NONE;
  switch (cmd) {
  case next::controlbridge::gui_command_parser::GuiCommand::OPEN: {

    current_filepath = GetStringListParam(params, "fileName");
    current_filename.clear();
    for (const auto &filepath : current_filepath) {
      boost::filesystem::path path(filepath);
      if (boost::filesystem::is_directory(path)) {
        for (auto &entry : boost::filesystem::directory_iterator(path)) {
          if (boost::filesystem::is_regular_file(entry)) {
            current_filename.push_back(entry.path().filename().string());
          }
        }
      } else if (boost::filesystem::is_regular_file(path)) {
        current_filename.push_back(path.filename().string());
      }
    }

    std::string simul_mode;
    simul_mode = params["mode"].front();
    if (simul_mode == "OLS") {
      simul_type = SimulationType::OpenLoop;
    } else if (simul_mode == "CLS") {
      simul_type = SimulationType::CloseLoop;
    } else {
      simul_type = SimulationType::InvalidMode;
    }

    HandleOpenFile();
    player_running = true;
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::PLAY: {
    bool playForward = GetStringParam(params, "forward") == "true";
    HandlePlay(playForward);
    player_paused = false;
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::PLAY_UNTIL: {
    bool playForward = GetStringParam(params, "forward") == "true";
    std::string play_until_timestamp_string = GetStringParam(params, "untilTimestamp");
    HandlePlayUntil(playForward, play_until_timestamp_string);
    player_paused = false;
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::PAUSE: {
    HandlePause();
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::CLOSE: {
    HandleClose();
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::REWIND: {
    HandleRewind();
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::FASTFORWARD: {
    HandleFastForward();
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::JUMP: {
    bool streamFramesInBetween = GetStringParam(params, "streamFramesInBetween") == "true";
    requested_timestamp_string = GetStringParam(params, "requestedTimeStamp");
    requested_timestamp_type = GetStringParam(params, "type");
    if (!requested_timestamp_string.empty() && !requested_timestamp_type.empty()) {
      uint64_t requested_timestamp = std::stoull(requested_timestamp_string);
      if (streamFramesInBetween) {
        // play only forward. To Do to play also backwards
        HandlePlayUntil(true, requested_timestamp_string);
        player_paused = false;
        break;
      }
      HandleJump(requested_timestamp, requested_timestamp_type);
    }
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::STEPFORWARD: {
    requested_steps_string = GetStringParam(params, "requestedSteps");
    if (!requested_steps_string.empty()) {
      int32_t requested_steps = 0;
      try {
        requested_steps = std::stoi(requested_steps_string);
      } catch (std::exception &e) {
        error(__FILE__, "STEPFORWARD command - Conversion failure: {0}", e.what());
      }
      HandleStepForward(requested_steps);
    }
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::UPDATESILFACTOR: {
    updated_sil_factor_string = GetStringParam(params, "sil");
    if (!updated_sil_factor_string.empty()) {
      float updated_sil_factor = 0.0;
      try {
        updated_sil_factor = std::stof(updated_sil_factor_string);
      } catch (std::exception &e) {
        error(__FILE__, "UPDATEDSILFACTOR command - Conversion failure: {0}", e.what());
      }
      HandleUpdateSilFactor(updated_sil_factor);
    }
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::JUMPTOEND: {
    HandleJumpToEnd();
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::OPENDIRECTORY: {
    conf_path = GetStringParam(params, "directory");
    conf_name = boost::filesystem::path(conf_path).filename().string();

    HandleOpenProjectDirectory();

    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::OPENBINARY: {
    exe_path = GetStringParam(params, "fileName");
    exe_name = boost::filesystem::path(exe_path).filename().string();

    HandleOpenBinary();

    break;
  }

  case next::controlbridge::gui_command_parser::GuiCommand::SOFTRESET: {
    debug(__FILE__, "SW RESET: ");
    next::controlbridge::lifecycle_orchestrator::LifecycleOrchestrator::GetInstance()->publishReset();
    break;
  }
  case next::controlbridge::gui_command_parser::GuiCommand::CHECKWEBSOCKETCONNECTION: {
    debug(__FILE__, "CHECK WEBSOCKET CONNECTION: ");
    next::appsupport::NextStatusCode::PublishStatusCode(
        next::appsupport::StatusCode::SUCC_RECEIVED_CHECK_WEBSOCKET_CONNECTION,
        "[ControlBridge] Check Websocket Connection received");
    break;
  }
  default:
    return false;
  }
  last_cmd = cmd;
  return true;
}

void PlayerControlManager::SendResponse(size_t id, const std::vector<std::uint8_t> &payload,
                                        std::vector<size_t> session_ids) {
  if (webserver != nullptr) {
    webserver->BroadcastMessageWithNotification(
        payload, id, session_ids, std::bind(&PlayerControlManager::CommandCompleted, this, std::placeholders::_1));
  }
}

void PlayerControlManager::CommandCompleted([[maybe_unused]] size_t id) { debug(__FILE__, "Response sent to GUI"); }

void PlayerControlManager::StartJsonConsumerLoop() {
  std::string jsonMessage;
  while (!close_threads) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kThreadSleep1Ms));
    if (payload_queue.try_and_pop(jsonMessage)) {
      HandleJsonPayload(jsonMessage);
    }
  }
}

void PlayerControlManager::HandleResponse(Response res) {
  response_payload = jsonCommandParser.CreateResponseString(res.cmd, res.payload);
  debug(__FILE__, "Response will be sent to GUI");
  SendResponse(0, response_payload, {});
}

void PlayerControlManager::StartJsonProducerLoop() {
  Response res;
  while (!close_threads) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kThreadSleep1Ms));
    if (response_queue.try_and_pop(res)) {
      HandleResponse(res);
    }
  }
}

//! Setup the webserver and start the json_consumer, json_producer loop in a separate thread
void PlayerControlManager::StartUp(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in,
                                   std::shared_ptr<next::appsupport::ConfigServer> configServer,
                                   [[maybe_unused]] std::string trigger_node_name,
                                   [[maybe_unused]] std::string trigger_name) {
  debug(__FILE__, "Startup initiated");

  // external_trigger = std::make_shared<EcalHandler>(trigger_node_name, trigger_name);
  webserver = webserver_in;
  webserver->AddForwardMessageCallback(std::bind(&PlayerControlManager::PayloadCallback, this, std::placeholders::_1));
  config_server_ = configServer;
  player_command_handler_ = std::make_shared<next::control::events::PlayerCommandEvent>("command handler event");
  meta_event_distributor_ =
      std::make_shared<next::controlbridge::event_handler::PlayerEventDistributor>(webserver, "", config_server_);

  json_consumer = std::thread([this] { StartJsonConsumerLoop(); });
  json_producer = std::thread([this] { StartJsonProducerLoop(); });
  debug(__FILE__, "Startup complete");
}

void PlayerControlManager::HandleJsonPayload(std::string jsonMessage) {
  bool parseSuccesful = jsonCommandParser.parseJson(jsonMessage);
  if (!parseSuccesful) {
    return;
  }
  bool result = HandleCommand(jsonCommandParser.GetCommand(), jsonCommandParser.GetParams());
  if (!result) {
    response_payload = jsonCommandParser.CreateErrorResponseString(jsonCommandParser.GetCommand());
    SendResponse(0, response_payload, {});
  }
}

//! Push messages into the queue
void PlayerControlManager::PayloadCallback(std::vector<uint8_t> data) {
  debug(__FILE__, "Message received");
  const std::string jsonMessage(data.begin(), data.end());
  payload_queue.push(jsonMessage);
}

void PlayerControlManager::ResponseCallback(Response res) {
  debug(__FILE__, "Response received");
  response_queue.push(res);
}

void PlayerControlManager::ShutDown() {
  debug(__FILE__, "Shutdown initiated");
  close_threads = true;
  if (player_running) {
    debug(__FILE__, "Shutting down player");
    HandleClose();
  }
  for (std::thread &th : threads) {
    // If thread Object is Joinable then Join that thread.
    if (th.joinable())
      th.join();
  }
  threads.clear();
  if (json_consumer.joinable())
    json_consumer.join();
  if (json_producer.joinable())
    json_producer.join();
  webserver.reset();
  // external_trigger.reset();
  debug(__FILE__, "Shutdown complete");
}

PlayerControlManager::~PlayerControlManager() { ShutDown(); }

} // namespace command_handler
} // namespace controlbridge
} // namespace next
