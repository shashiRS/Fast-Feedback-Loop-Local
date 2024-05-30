/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_value_to_client.cpp
 * @brief    event emmitted by the config server with the single value os a config key
 *
 **/

#include <json/json.h>

#include <next/sdk/events/send_config_value_to_client.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

payload_type
SendConfigValueToClient::serializeEventPayload(const SendConfigValueToClientInformationMessage &message) const {
  Json::Value jsonVal;
  jsonVal["config_value"] = message.config_value;
  jsonVal["receiver_name"] = message.receiver_name;
  jsonVal["sender_name"] = message.sender_name;
  jsonVal["request_key"] = message.request_key;
  jsonVal["request_type"] = message.request_type;
  Json::FastWriter fastWriter;
  return StringToPayload(fastWriter.write(jsonVal));
}

SendConfigValueToClientInformationMessage
SendConfigValueToClient::deserializeEventPayload(const payload_type &strbuf) const {
  SendConfigValueToClientInformationMessage message{};
  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(strbuf), jsonVal)) {
    if (jsonVal.isMember("config_value")) {
      message.config_value = jsonVal["config_value"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [config_json]");
    }
    if (jsonVal.isMember("sender_name")) {
      message.sender_name = jsonVal["sender_name"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [sender_name]");
    }
    if (jsonVal.isMember("receiver_name")) {
      message.receiver_name = jsonVal["receiver_name"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [receiver_name]");
    }
    if (jsonVal.isMember("request_key")) {
      message.request_key = jsonVal["request_key"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [request_key]");
    }
    if (jsonVal.isMember("request_type")) {
      message.request_type = jsonVal["request_type"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [request_type]");
    }
  } else {
    logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to parse input as json");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
