/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_command.cpp
 * @brief    meta event for Next Player Control
 *
 **/

#include <next/control/event_types/player_command.h>

#include <type_traits>

#include <json/json.h>

namespace next {
namespace control {

namespace events {
bool stringToBool(std::string in) {
  bool return_value = false;
  std::transform(in.begin(), in.end(), in.begin(), [](unsigned char c) { return static_cast<char>(::tolower(c)); });
  if (in == "true") {
    return_value = true;
  } else if (in == "false") {
    return_value = false;
  } else if (in == "1") {
    return_value = true;
  } else if (in == "0") {
    return_value = false;
  }
  return return_value;
}

const std::unordered_map<commands::PlayerCommand, std::string> commandToString{
    {commands::PlayerCommand::NONE, "NONE"},
    {commands::PlayerCommand::OPEN, "OPEN"},
    {commands::PlayerCommand::PAUSE, "PAUSE"},
    {commands::PlayerCommand::CLOSE, "CLOSE"},
    {commands::PlayerCommand::JUMP, "JUMP"},
    {commands::PlayerCommand::STEPFORWARD, "STEPFORWARD"},
    {commands::PlayerCommand::STEPFORWARD_UNTIL, "STEPFORWARD_UNTIL"}};

const std::unordered_map<std::string, commands::PlayerCommand> stringToCommand{
    {"NONE", commands::PlayerCommand::NONE},
    {"OPEN", commands::PlayerCommand::OPEN},
    {"PAUSE", commands::PlayerCommand::PAUSE},
    {"CLOSE", commands::PlayerCommand::CLOSE},
    {"JUMP", commands::PlayerCommand::JUMP},
    {"STEPFORWARD", commands::PlayerCommand::STEPFORWARD},
    {"STEPFORWARD_UNTIL", commands::PlayerCommand::STEPFORWARD_UNTIL}};

next::sdk::events::payload_type
PlayerCommandEvent::serializeEventPayload(const next::control::message::request &message) const {

  Json::Value json_val;
  json_val["id"] = message.id;
  Json::Value json_file_paths(Json::arrayValue);
  for (const auto &path : message.file_path) {
    json_file_paths.append(path);
  }
  json_val["file_path"] = json_file_paths;
  json_val["play_forward"] = std::to_string(message.play_forward);
  json_val["requested_steps"] = message.requested_steps;
  json_val["requested_timestamp"] = message.requested_timestamp;
  json_val["timestamp_type"] = message.timestamp_type;
  json_val["play_until_timestamp"] = message.play_until_timestamp;
  json_val["sil"] = message.sil;
  auto command = message.command;
  json_val["command"] =
      (commandToString.find(command) != commandToString.end()) ? commandToString.find(command)->second : "NONE";

  Json::FastWriter fastWriter;
  return next::sdk::events::StringToPayload(fastWriter.write(json_val));
}

next::control::message::request
PlayerCommandEvent::deserializeEventPayload(const next::sdk::events::payload_type &strbuf) const {
  next::control::message::request message{};
  Json::Reader reader;
  Json::Value json_val;
  std::string json_str = next::sdk::events::PayloadToString(strbuf);

  if (reader.parse(json_str, json_val)) {
    message.id = std::stoi(json_val["id"].asString().c_str());
    if (json_val["file_path"].isArray()) {
      for (const auto &json_path : json_val["file_path"]) {
        message.file_path.push_back(json_path.asString());
      }
    }
    auto play_forward = json_val["play_forward"].asString();
    message.play_forward = stringToBool(play_forward);
    message.requested_steps = std::stoi(json_val["requested_steps"].asString().c_str());
    message.requested_timestamp = std::stoull(json_val["requested_timestamp"].asString().c_str());
    message.play_until_timestamp = std::stoull(json_val["play_until_timestamp"].asString().c_str());
    message.timestamp_type = json_val["timestamp_type"].asString();
    message.sil = std::stof(json_val["sil"].asString().c_str());
    auto command = json_val["command"].asString();
    message.command = (stringToCommand.find(command) != stringToCommand.end()) ? stringToCommand.find(command)->second
                                                                               : commands::PlayerCommand::NONE;
  }
  return message;
}
void PlayerCommandEvent::publish(const next::control::message::request &message) {
  std::list<std::string> res;
  next::sdk::events::BaseEventBinaryBlocking<next::control::message::request>::publish(message, res);
}
} // namespace events
} // namespace control
} // namespace next
