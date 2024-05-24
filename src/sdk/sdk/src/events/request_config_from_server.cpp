/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config_from_server.cpp
 * @brief    event to request nodes to send their config files to Next Control
 *
 **/

#include <json/json.h>

#include <next/sdk/events/request_config_from_server.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

payload_type
RequestConfigFromServer::serializeEventPayload(const RequestConfigFromServerInformationMessage &message) const {
  Json::Value jsonVal;
  jsonVal["request_key"] = message.request_key;
  jsonVal["requester_name"] = message.requester_name;
  Json::FastWriter fastWriter;
  std::string jsonString = fastWriter.write(jsonVal);
  return StringToPayload(jsonString);
}

RequestConfigFromServerInformationMessage
RequestConfigFromServer::deserializeEventPayload(const payload_type &buf) const {
  RequestConfigFromServerInformationMessage message{};
  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(buf), jsonVal)) {
    if (jsonVal.isMember("request_key")) {
      message.request_key = jsonVal["request_key"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [request_key]");
    }
    if (jsonVal.isMember("requester_name")) {
      message.requester_name = jsonVal["requester_name"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [requester_name]");
    }
  } else {
    logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to parse input as json");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
