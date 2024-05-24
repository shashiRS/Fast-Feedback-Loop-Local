/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config_value_from_server.cpp
 * @brief    event to request a specific value from the configuration server
 *
 **/

#include <json/json.h>

#include <next/sdk/events/request_config_value_from_server.h>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace sdk {
namespace events {

payload_type RequestConfigValueFromServer::serializeEventPayload(
    const RequestConfigValueFromServerInformationMessage &message) const {
  Json::Value jsonVal;
  jsonVal["request_key"] = message.request_key;
  jsonVal["request_type"] = message.request_type;
  jsonVal["request_default_value"] = message.request_default_value;
  jsonVal["requester_name"] = message.requester_name;
  Json::FastWriter fastWriter;
  return StringToPayload(fastWriter.write(jsonVal));
}

RequestConfigValueFromServerInformationMessage
RequestConfigValueFromServer::deserializeEventPayload(const payload_type &buf) const {
  RequestConfigValueFromServerInformationMessage message{};
  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(buf), jsonVal)) {
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
    if (jsonVal.isMember("request_default_value")) {
      message.request_default_value = jsonVal["request_default_value"].asString();
    } else {
      logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to find key [request_default_value]");
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
