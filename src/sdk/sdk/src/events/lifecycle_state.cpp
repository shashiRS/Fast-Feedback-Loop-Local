/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_state.cpp
 * @brief    implementation of LifecyleState class for send and receive lifecycle state changes
 *
 **/

#include <json/json.h>

#include <json/json.h>
#include <next/sdk/logger/logger.hpp>
#include "next/sdk/events/lifecycle_state.h"
#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace events {

void LifecycleState::publishState(const std::string &state, const std::string &component_name) {
  LifecycleStateMessage message{};
  message.state = state;
  message.component_name = component_name;
  message.timestamp = getCurrentNanoSeconds();
  publish(message);
}

payload_type LifecycleState::serializeEventPayload(const LifecycleStateMessage &message) const {
  Json::Value json_val;
  json_val["timestamp"] = std::to_string(message.timestamp);
  json_val["component_name"] = message.component_name;
  json_val["state"] = message.state;

  Json::FastWriter fastWriter;
  std::string jsonised_string = fastWriter.write(json_val);
  return StringToPayload(jsonised_string);
}

LifecycleStateMessage LifecycleState::deserializeEventPayload(const payload_type &strbuf) const {
  LifecycleStateMessage message{};

  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(strbuf), jsonVal)) {
    if (jsonVal.isMember("timestamp")) {
      message.timestamp = std::stoull(jsonVal["timestamp"].asCString());
    } else {
      logger::warn(__FILE__, "LifecycleState::deserializeEventPayload :: failed to find key [timestamp]");
    }

    if (jsonVal.isMember("component_name")) {
      message.component_name = jsonVal["component_name"].asString();
    } else {
      logger::warn(__FILE__, "LifecycleState::deserializeEventPayload :: failed to find key [component_name]");
    }

    if (jsonVal.isMember("state")) {
      message.state = jsonVal["state"].asString();
    } else {
      logger::warn(__FILE__, "LifecycleState::deserializeEventPayload :: failed to find key [state]");
    }
  } else {
    logger::warn(__FILE__, "LifecycleState::deserializeEventPayload :: failed to parse input as json");
  }

  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
