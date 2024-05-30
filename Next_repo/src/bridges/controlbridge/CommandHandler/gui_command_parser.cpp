/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     gui_command_parser.cpp
 * @brief    implementation of small module to parse json commands
 *
 * Parse commands received from GUI and return a map of commands and parameters
 **/
#include <chrono>

#include <json/json.h>

#include <next/sdk/logger/logger.hpp>
#include "gui_command_parser.h"

namespace next {
namespace controlbridge {
namespace gui_command_parser {

GuiCommand CommandParser::GetCommand() { return command_; }

std::map<std::string, std::vector<std::string>> CommandParser::GetParams() { return params_; }

//! If json can't be parsed or doesn't contain command, then default command is used
bool CommandParser::parseJson(const std::string &jsonMessage) {
  Json::Value parsedFromString;
  Json::Reader reader;
  debug(__FILE__, "Json received: {0}", jsonMessage);
  bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
  if (!parsingSuccessful) {
    error(__FILE__, " Parsing Unsuccessful");
    return false;
  }
  command_ = GuiCommand::NONE;
  if (!parsedFromString["event"] || parsedFromString["event"].isNull() || parsedFromString["payload"].isNull() ||
      !parsedFromString["event"].isString()) {
    return false;
  }

  params_.clear(); // clean the params before filling them
  if (auto cmd = stringToCommand.find(parsedFromString["event"].asString()); cmd != stringToCommand.end()) {
    command_ = cmd->second;
    debug(__FILE__, " Event received: {0}", parsedFromString["event"].asString());
    debug(__FILE__, " Channel: {0}", parsedFromString["channel"].asString());
    debug(__FILE__, " Source: {0}", parsedFromString["source"].asString());
    // Skip if no payload
    if (parsedFromString["payload"].size() != 0) {
      for (auto it = parsedFromString["payload"].begin(); it != parsedFromString["payload"].end(); ++it) {
        if (it->isArray()) {
          for (unsigned int i = 0; i < it->size(); ++i) {
            const Json::Value &item = (*it)[i];
            if (item.isString()) {
              params_[it.key().asString()].push_back(item.asString());
            }
          }
        } else {
          params_[it.key().asString()].push_back(it->asString());
        }
      }
    }
  } else {
    return false;
  }
  return true;
}

//! In case of error respond with the cmd and response false
std::vector<uint8_t> CommandParser::CreateErrorResponseString(GuiCommand command) {
  Json::Value root;
  root["channel"] = "player";
  root["event"] = CommandToString.find(command)->second;
  root["source"] = "ControlBridge";
  root["response"] = false;
  Json::FastWriter writer;
  std::string response = writer.write(root);
  return std::vector<uint8_t>(response.begin(), response.end());
}

//! @todo Using dummy response now change once connection to player is done
std::vector<uint8_t> CommandParser::CreateResponseString(GuiCommand command,
                                                         std::map<std::string, std::string> payload) {
  Json::Value root;
  Json::Value info;
  root["channel"] = "player";
  root["event"] = CommandToString.find(command)->second;
  root["source"] = "ControlBridge";
  for (auto it = payload.begin(); it != payload.end(); ++it) {
    info[it->first] = it->second;
  }
  root["payload"] = info;
  Json::FastWriter writer;
  std::string response = writer.write(root);
  return std::vector<uint8_t>(response.begin(), response.end());
}

} // namespace gui_command_parser
} // namespace controlbridge
} // namespace next
