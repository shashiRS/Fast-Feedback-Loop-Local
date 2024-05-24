/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     play_manager.cpp
 * @brief    Player Manager Class
 *
 **/

#include "play_manager.hpp"

#include <limits>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>

using namespace mts::extensibility;

PlayManager::PlayManager()
    : publisher_manager_ptr_(new PublisherManager()), file_reader_ptr_(new FileReader()),
      resource_manager_ptr_(new ResourceManager()) {
  // start the state machine
  player_state_machine.start();
  ESTATES temp = (ESTATES)player_state_machine.current_state()[0];
  if (temp == ESTATES::E_NEW) {
    next::sdk::logger::debug(__FILE__, "State machine initialized ok");
  }
}

PlayManager::~PlayManager() {
  next::sdk::logger::debug(__FILE__, "PlayManager destructor called");

  if (player_state_machine.current_state_ == ESTATES::E_PLAY) {
    player_state_machine.process_event(command_PAUSE());
    player_state_machine.process_event(command_CLOSE());
  }
  if (player_state_machine.current_state_ == ESTATES::E_READY) {
    player_state_machine.process_event(command_CLOSE());
  }

  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
  // give the state machine time to close
  ESTATES currentState = (ESTATES)player_state_machine.current_state_;
  while (currentState != ESTATES::E_SHUTDOWN) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    currentState = (ESTATES)player_state_machine.current_state_;
  }
}

void PlayManager::HandleRequestMessage() {
  auto req_message = command_handler_server_->getEventData();
  message_queue_.push(req_message);
}

bool PlayManager::Initialize() {

  if (is_initialized_ == true) {
    return true;
  }
  resource_manager_ptr_->Initialize();
  auto openFileStatusCallback =
      std::bind(&PlayerStateMachine::UpdateOpenRecordingStatus, player_state_machine, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
  resource_manager_ptr_->GetUdexService()->SetLoadRecordingInfoCallback(openFileStatusCallback);
  file_reader_ptr_->SetLoadRecordingInfoCallback(openFileStatusCallback);

  publisher_manager_ptr_->Initialize(resource_manager_ptr_);
  file_reader_ptr_->SetResourceManager(resource_manager_ptr_);

  // init states
  StateOpening &stateOpening = player_state_machine.get_state<StateOpening &>();
  stateOpening.Initialize(resource_manager_ptr_, publisher_manager_ptr_, file_reader_ptr_);

  StatePlay &statePLAY = player_state_machine.get_state<StatePlay &>();
  statePLAY.Initialize(publisher_manager_ptr_, resource_manager_ptr_);

  StateUnloadRecording &stateUnloadRecording = player_state_machine.get_state<StateUnloadRecording &>();
  stateUnloadRecording.Initialize(resource_manager_ptr_, file_reader_ptr_);

  StateSkipTo &stateSkipTo = player_state_machine.get_state<StateSkipTo &>();
  stateSkipTo.Initialize(file_reader_ptr_);

  auto hook = std::bind(&PlayManager::HandleRequestMessage, this);
  command_handler_server_ = std::make_shared<next::control::events::PlayerCommandEvent>("command handler event");
  command_handler_server_->addEventHook(hook);
  command_handler_server_->subscribe();
  is_initialized_ = true;
  return true;
}

bool PlayManager::Execute() {
  // get commands
  next::control::message::request request{};
  message_queue_.wait_and_pop(request);

  next::control::commands::PlayerCommand cmd = request.command;

  bool status = true;
  // wait for any other process_event calls to finish
  std::lock_guard<std::mutex> guard(*player_state_machine.process_event_mutex_);

  switch (cmd) {
  case next::control::commands::PlayerCommand::OPEN: {
    if (player_state_machine.current_state_ == ESTATES::E_PLAY) {
      player_state_machine.process_event(command_PAUSE());
      player_state_machine.process_event(command_CLOSE());
    }
    if (player_state_machine.current_state_ == ESTATES::E_READY) {
      player_state_machine.process_event(command_CLOSE());
    }
    player_state_machine.process_event(command_OPEN(request.file_path)); /*adapt this to handle the vector*/
    break;
  }
  case next::control::commands::PlayerCommand::STEPFORWARD: {
    player_state_machine.process_event(command_STEP_FORWARD(request.requested_steps));
    break;
  }
  case next::control::commands::PlayerCommand::STEPFORWARD_UNTIL: {
    player_state_machine.process_event(command_STEP_FORWARD(request.requested_steps, request.play_until_timestamp));
    break;
  }
  case next::control::commands::PlayerCommand::PAUSE: {
    player_state_machine.process_event(command_PAUSE());
    break;
  }
  case next::control::commands::PlayerCommand::CLOSE: {
    player_state_machine.process_event(command_CLOSE());
    break;
  }
  case next::control::commands::PlayerCommand::JUMP: {
    player_state_machine.process_event(command_JUMP(request.requested_timestamp, request.timestamp_type));
    break;
  }
  default:
    status = false;
  }

  return status;
}

void PlayManager::RunCommandLine() {
  std::string recording_path = next::appsupport::ConfigClient::getConfig()->get_string(
      next::appsupport::configkey::player::getRecordingPathKey(), "");

  if (!recording_path.empty()) {
    player_state_machine.process_event(command_OPEN({recording_path})); // adapt to a vector here
    player_state_machine.process_event(command_STEP_FORWARD(INT32_MAX));

    // wait untill back in ready
    ESTATES currentState = (ESTATES)player_state_machine.current_state_;
    while (currentState != ESTATES::E_READY) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      currentState = (ESTATES)player_state_machine.current_state_;
    }
    player_state_machine.process_event(command_CLOSE());

    // give the state machine time to close
    currentState = (ESTATES)player_state_machine.current_state_;
    while (currentState != ESTATES::E_NEW) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      currentState = (ESTATES)player_state_machine.current_state_;
    }
  }

  return;
}
void PlayManager::UpdateSilFactorFromConfig(const float silFactor) {
  publisher_manager_ptr_->UpdateSpeedFactor(silFactor);
}
void PlayManager::Shutdown() { message_queue_.Shutdown(); }
