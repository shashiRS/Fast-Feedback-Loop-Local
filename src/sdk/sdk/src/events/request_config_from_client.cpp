/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config_from_client.cpp
 * @brief    event to request nodes to send their config files to Next Control
 *
 **/

#include <json/json.h>

#include <next/sdk/events/request_config_from_client.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

payload_type
RequestConfigFromClient::serializeEventPayload(const RequestConfigFromClientInformationMessage &message) const {

  Json::Value jsonVal;
  jsonVal["request_key"] = message.request_key;
  jsonVal["requested_componet"] = message.requested_componet;
  jsonVal["requester_name"] = message.requester_name;
  Json::FastWriter fastWriter;
  std::string jsonString = fastWriter.write(jsonVal);
  return StringToPayload(jsonString);
}

RequestConfigFromClientInformationMessage
RequestConfigFromClient::deserializeEventPayload(const payload_type &strbuf) const {
  RequestConfigFromClientInformationMessage message{};
  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(strbuf), jsonVal)) {
    if (jsonVal.isMember("request_key")) {
      message.request_key = jsonVal["request_key"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [request_key]");
    }
    if (jsonVal.isMember("requested_componet")) {
      message.requested_componet = jsonVal["requested_componet"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [requested_componet]");
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
