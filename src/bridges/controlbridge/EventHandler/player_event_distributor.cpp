/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     meta_event_distributor.cpp
 * @brief    implementation of small module to handle meta event messages
 **/

#include <thread>

#include <json/json.h>

#include <next/appsupport/config/config_key_generator_player.hpp>
#include <next/sdk/logger/logger.hpp>

#include "player_event_distributor.h"

std::vector<uint8_t> global_sil_factor_payload;

namespace next {
namespace controlbridge {
namespace event_handler {

void MetaInfoCommandCompleted(std::vector<uint8_t> *memory) {
  if (memory) {
    delete memory;
  }

  debug(__FILE__, "[ControlBridge] MetaInfo sent to GUI");
}
void SilFactorUpdateCompleted() { debug(__FILE__, "[ControlBridge] Sil Factor Update sent to GUI"); }

PlayerEventDistributor::PlayerEventDistributor(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in,
                                               const std::string &name,
                                               std::shared_ptr<next::appsupport::ConfigServer> configServer)
    : player_event_subscriber_(name), webserver_(webserver_in), name_(name) {
  config_server_ = configServer;
  auto hook = std::bind(&PlayerEventDistributor::runMetaInformationDistribution, this);
  player_event_subscriber_.addEventHook(hook);
  player_event_subscriber_.subscribe();
}

PlayerEventDistributor::~PlayerEventDistributor() {}

void PlayerEventDistributor::runMetaInformationDistribution() {
  next::control::events::PlayerMetaInformationMessage received_msg;
  received_msg = this->getMessage();
  if (connectionChanged) {
    if (last_opened_recording_.size()) {
      info(__FILE__, "Connection status changed - resending last OpenRecording message : {0}", last_opened_recording_);
      sendJsonToGUI(last_opened_recording_);
    }
  }

  this->sendJsonToGUI(this->createJson(received_msg));
  SendSilFactor();
  if (ConnectionStatusChanged()) {
    webserver_connection_count_ = webserver_->GetNumOfActiveConnections();
    connectionChanged = true;
  } else {
    connectionChanged = false;
  }
}

next::control::events::PlayerMetaInformationMessage PlayerEventDistributor::getMessage() {
  next::control::events::PlayerMetaInformationMessage received_msg;
  received_msg = player_event_subscriber_.getEventData();
  return received_msg;
}

std::string PlayerEventDistributor::createSilFactorJson() {
  std::string response = "";
  if (config_server_ != nullptr) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool value_available{false};
    auto sil_factor = config_server_->get_float(next::appsupport::configkey::player::getRealTimeFactorKey(), -1.0f,
                                                false, value_available);
    if (value_available) {
      Json::Value root;
      Json::Value info;
      Json::Value stepping;

      root["channel"] = "player";
      root["source"] = "NextBridge";
      root["event"] = "OverwriteSilFactor";
      info["expectedSpeedMultiple"] = sil_factor;
      root["payload"] = info;
      Json::FastWriter writer;
      response = writer.write(root);
    } else {
      warn(__FILE__, "[{0}] is not defined", next::appsupport::configkey::player::getRealTimeFactorKey());
    }
  }
  return response;
}

std::string PlayerEventDistributor::createJson(next::control::events::PlayerMetaInformationMessage received_msg) {

  bool shouldSaveRecordingState_ = false;

  Json::Value root;
  Json::Value info;
  Json::Value stepping;

  root["channel"] = "player";
  root["source"] = "NextBridge";
  skip_GUI_update_ = false;
  switch (received_msg.status) {
  // Player initialized
  case next::control::events::PlayerState::ON_NEW:
    last_opened_recording_ = ""; // recorded unloaded
    if (current_player_state_ != next::control::events::PlayerState::UNSPECIFIED) {
      root["event"] = "SimulationStateIsIdle";
    } else {
      skip_GUI_update_ = true;
    }
    break;
  // Recording loaded
  case next::control::events::PlayerState::ON_READY:
    if (current_player_state_ == next::control::events::PlayerState::EXIT_OPENING) {
      next::sdk::logger::info(__FILE__, "RecordingIsLoaded : {0}", received_msg.recording_name);
      shouldSaveRecordingState_ = true;
      root["event"] = "RecordingIsLoaded";
      info["startTimeStamp"] = received_msg.min_timestamp;
      info["endTimeStamp"] = received_msg.max_timestamp;
      info["currentTimestamp"] = received_msg.timestamp;
      stepping["stepType"] = received_msg.stepping_type;
      stepping["stepValue"] = received_msg.stepping_value;
      info["metaData"] = stepping;
    } else {
      root["event"] = "PlaybackIsPaused";
      info["currentTimestamp"] = received_msg.timestamp;
    }

    break;
  // Playing recording
  case next::control::events::PlayerState::ON_PLAY:
    root["event"] = "PlaybackIsPlaying";
    info["startTimeStamp"] = received_msg.min_timestamp;
    info["endTimeStamp"] = received_msg.max_timestamp;
    info["currentTimestamp"] = received_msg.timestamp;
    info["speedMultiple"] = received_msg.speed_factor;
    info["unit"] = "us";
    stepping["stepType"] = received_msg.stepping_type;
    stepping["stepValue"] = received_msg.stepping_value;
    info["metaData"] = stepping;
    break;
  case next::control::events::PlayerState::ON_ERROR:
    root["event"] = "FailedToLoadRecording";
    break;
  default:
    skip_GUI_update_ = true;
    break;
  }
  root["payload"] = info;
  Json::FastWriter writer;
  std::string response = writer.write(root);
  if (current_player_state_ != received_msg.status) {
    state_changed_ = true;
    current_player_state_ = received_msg.status;
  } else {
    state_changed_ = false;
  }
  if (shouldSaveRecordingState_) {
    last_opened_recording_ = response;
  }

  return response;
}

bool PlayerEventDistributor::sendJsonToGUI(std::string msg) {
  std::vector<size_t> session_ids;
  if (skip_GUI_update_)
    return false;
  if (state_changed_ || current_player_state_ == next::control::events::PlayerState::ON_PLAY || connectionChanged) {
    session_ids.clear();

    std::vector<uint8_t> *payload = new std::vector<uint8_t>();
    payload->assign(msg.begin(), msg.end());

    webserver_->BroadcastMessageWithNotification(*payload, 0, session_ids,
                                                 std::bind(MetaInfoCommandCompleted, payload));
    return true;
  }
  return false;
}
void PlayerEventDistributor::SendSilFactor() {

  if (!skip_GUI_update_ &&
      ((state_changed_ && current_player_state_ == next::control::events::PlayerState::ON_NEW) || connectionChanged)) {
    std::vector<size_t> session_ids;
    std::string silFactorMessage = createSilFactorJson();
    if (silFactorMessage.size()) {
      global_sil_factor_payload.assign(silFactorMessage.begin(), silFactorMessage.end());
      webserver_->BroadcastMessageWithNotification(global_sil_factor_payload, 0, session_ids,
                                                   std::bind(SilFactorUpdateCompleted));
    }
  }
}

bool PlayerEventDistributor::ConnectionStatusChanged() {
  return (webserver_connection_count_ != webserver_->GetNumOfActiveConnections());
}
} // namespace event_handler
} // namespace controlbridge
} // namespace next
