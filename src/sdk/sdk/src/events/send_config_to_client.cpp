/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_to_client.cpp
 * @brief    event emmitted by nodes to send configuration info to Next Control
 *
 **/

#include <json/json.h>

#include <next/sdk/events/send_config_to_client.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

payload_type SendConfigToClient::serializeEventPayload(const SendConfigToClientInformationMessage &message) const {
  Json::Value jsonVal;
  jsonVal["config_json"] = message.config_json;
  jsonVal["receiver_name"] = message.receiver_name;
  jsonVal["sender_name"] = message.sender_name;
  if (message.reset_active) {
    jsonVal["reset_active"] = "true";
  } else {
    jsonVal["reset_active"] = "false";
  }
  Json::FastWriter fastWriter;
  return StringToPayload(fastWriter.write(jsonVal));
}

SendConfigToClientInformationMessage SendConfigToClient::deserializeEventPayload(const payload_type &strbuf) const {
  SendConfigToClientInformationMessage message{};
  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(strbuf), jsonVal)) {
    if (jsonVal.isMember("config_json")) {
      message.config_json = jsonVal["config_json"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [config_json]");
    }
    if (jsonVal.isMember("receiver_name")) {
      message.receiver_name = jsonVal["receiver_name"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [receiver_name]");
    }
    if (jsonVal.isMember("sender_name")) {
      message.sender_name = jsonVal["sender_name"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [sender_name]");
    }
    if (jsonVal.isMember("reset_active")) {
      if (jsonVal["reset_active"].asString().compare("true") == 0)
        message.reset_active = true;
      else
        message.reset_active = false;
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [reset_active]");
    }
  } else {
    logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to parse input as json");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
