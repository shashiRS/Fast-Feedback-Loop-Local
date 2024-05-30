/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_to_server.cpp
 * @brief    event emmited by Next Control to overwright nodes' configurations
 *
 **/
#include <json/json.h>

#include <next/sdk/events/send_config_to_server.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

payload_type SendConfigToServer::serializeEventPayload(const SendConfigToServerInformationMessage &message) const {
  Json::Value jsonVal;
  jsonVal["config_json"] = message.config_json;
  jsonVal["sender_componet_name"] = message.sender_componet_name;
  Json::FastWriter fastWriter;
  return StringToPayload(fastWriter.write(jsonVal));
}

SendConfigToServerInformationMessage SendConfigToServer::deserializeEventPayload(const payload_type &strbuf) const {
  SendConfigToServerInformationMessage message{};
  Json::Value jsonVal;
  Json::Reader jsonReader;

  if (jsonReader.parse(PayloadToString(strbuf), jsonVal)) {
    if (jsonVal.isMember("config_json")) {
      message.config_json = jsonVal["config_json"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [config_json]");
    }
    if (jsonVal.isMember("sender_componet_name")) {
      message.sender_componet_name = jsonVal["sender_componet_name"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [sender_componet_name]");
    }
  } else {
    logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to parse input as json");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
