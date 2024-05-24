/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_state_event.cpp
 * @brief    meta event for Next Player Control
 *
 **/

#include <type_traits>
#include <unordered_map>

#include <next/control/event_types/player_state_event.h>

#include <json/json.h>

namespace next {
namespace control {
namespace events {

bool operator==(const PlayerMetaInformationMessage &lhs, const PlayerMetaInformationMessage &rhs) {

  return lhs.max_timestamp == rhs.max_timestamp && lhs.min_timestamp == rhs.min_timestamp &&
         lhs.recording_name == rhs.recording_name && lhs.speed_factor == rhs.speed_factor && lhs.status == rhs.status &&
         lhs.stepping_type == rhs.stepping_type && lhs.stepping_value == rhs.stepping_value &&
         lhs.timestamp == rhs.timestamp;
}

const std::unordered_map<PlayerState, std::string> stateToString{
    {PlayerState::UNSPECIFIED, "UNSPECIFIED"},
    {PlayerState::ENTER_NEW, "ENTER_NEW"},
    {PlayerState::ON_NEW, "ON_NEW"},
    {PlayerState::EXIT_NEW, "EXIT_NEW"},
    {PlayerState::ENTER_OPENING, "ENTER_OPENING"},
    {PlayerState::ON_OPENING, "ON_OPENING"},
    {PlayerState::EXIT_OPENING, "EXIT_OPENING"},
    {PlayerState::ENTER_READY, "ENTER_READY"},
    {PlayerState::ON_READY, "ON_READY"},
    {PlayerState::EXIT_READY, "EXIT_READY"},
    {PlayerState::ENTER_PLAY, "ENTER_PLAY"},
    {PlayerState::ON_PLAY, "ON_PLAY"},
    {PlayerState::EXIT_PLAY, "EXIT_PLAY"},
    {PlayerState::ENTER_UNLOAD_RECORDING, "ENTER_UNLOAD_RECORDING"},
    {PlayerState::ON_UNLOAD_RECORDING, "ON_UNLOAD_RECORDING"},
    {PlayerState::EXIT_UNLOAD_RECORDING, "EXIT_UNLOAD_RECORDING"},
    {PlayerState::ENTER_SKIP_TO, "ENTER_SKIP_TO"},
    {PlayerState::ON_SKIP_TO, "ON_SKIP_TO"},
    {PlayerState::EXIT_SKIP_TO, "EXIT_SKIP_TO"},
    {PlayerState::ENTER_ERROR, "ENTER_ERROR"},
    {PlayerState::ON_ERROR, "ON_ERROR"},
    {PlayerState::EXIT_ERROR, "EXIT_ERROR"},
    {PlayerState::ENTER_SHUTDOWN, "ENTER_SHUTDOWN"},
    {PlayerState::ON_SHUTDOWN, "ON_SHUTDOWN"},
    {PlayerState::EXIT_SHUTDOWN, "EXIT_SHUTDOWN"}};

const std::unordered_map<std::string, PlayerState> stringToState{
    {"UNSPECIFIED", PlayerState::UNSPECIFIED},
    {"ENTER_NEW", PlayerState::ENTER_NEW},
    {"ON_NEW", PlayerState::ON_NEW},
    {"EXIT_NEW", PlayerState::EXIT_NEW},
    {"ENTER_OPENING", PlayerState::ENTER_OPENING},
    {"ON_OPENING", PlayerState::ON_OPENING},
    {"EXIT_OPENING", PlayerState::EXIT_OPENING},
    {"ENTER_READY", PlayerState::ENTER_READY},
    {"ON_READY", PlayerState::ON_READY},
    {"EXIT_READY", PlayerState::EXIT_READY},
    {"ENTER_PLAY", PlayerState::ENTER_PLAY},
    {"ON_PLAY", PlayerState::ON_PLAY},
    {"EXIT_PLAY", PlayerState::EXIT_PLAY},
    {"ENTER_UNLOAD_RECORDING", PlayerState::ENTER_UNLOAD_RECORDING},
    {"ON_UNLOAD_RECORDING", PlayerState::ON_UNLOAD_RECORDING},
    {"EXIT_UNLOAD_RECORDING", PlayerState::EXIT_UNLOAD_RECORDING},
    {"ENTER_SKIP_TO", PlayerState::ENTER_SKIP_TO},
    {"ON_SKIP_TO", PlayerState::ON_SKIP_TO},
    {"EXIT_SKIP_TO", PlayerState::EXIT_SKIP_TO},
    {"ENTER_ERROR", PlayerState::ENTER_ERROR},
    {"ON_ERROR", PlayerState::ON_ERROR},
    {"EXIT_ERROR", PlayerState::EXIT_ERROR},
    {"ENTER_SHUTDOWN", PlayerState::ENTER_SHUTDOWN},
    {"ON_SHUTDOWN", PlayerState::ON_SHUTDOWN},
    {"EXIT_SHUTDOWN", PlayerState::EXIT_SHUTDOWN}};

next::sdk::events::payload_type
PlayerStateEvent::serializeEventPayload(const PlayerMetaInformationMessage &message) const {
  Json::Value json_val;
  json_val["min_timestamp"] = std::to_string(message.min_timestamp);
  json_val["max_timestamp"] = std::to_string(message.max_timestamp);
  json_val["timestamp"] = std::to_string(message.timestamp);
  json_val["speed_factor"] = std::to_string(message.speed_factor);
  json_val["recording_name"] = message.recording_name;
  json_val["stepping_type"] = message.stepping_type;
  json_val["stepping_value"] = std::to_string(message.stepping_value);
  auto state = message.status;
  json_val["status"] = stateToString.at(state);

  Json::FastWriter fastWriter;
  return next::sdk::events::StringToPayload(fastWriter.write(json_val));
}

PlayerMetaInformationMessage
PlayerStateEvent::deserializeEventPayload(const next::sdk::events::payload_type &strbuf) const {
  PlayerMetaInformationMessage message{};
  Json::Reader reader;
  Json::Value json_val;
  std::string json_str = next::sdk::events::PayloadToString(strbuf);
  if (reader.parse(json_str, json_val)) {
    message.min_timestamp = std::stoull(json_val["min_timestamp"].asString().c_str());
    message.max_timestamp = std::stoull(json_val["max_timestamp"].asString().c_str());
    message.timestamp = std::stoull(json_val["timestamp"].asString().c_str());
    message.speed_factor = std::stof(json_val["speed_factor"].asString().c_str());
    message.recording_name = json_val["recording_name"].asString();
    message.stepping_type = json_val["stepping_type"].asString();
    message.stepping_value = std::stoull(json_val["stepping_value"].asString().c_str());
    auto state_str = json_val["status"].asString();
    message.status = stringToState.at(state_str);
  }
  return message;
}

void PlayerStateEvent::updateStatus(PlayerState new_status) { _message.status = new_status; }

void PlayerStateEvent::publish(const PlayerMetaInformationMessage &message) {
  std::list<std::string> res;
  next::sdk::events::BaseEventBinary<PlayerMetaInformationMessage>::publish(message, res);
}
void PlayerStateEvent::send() { publish(_message); }

} // namespace events
} // namespace control
} // namespace next
