/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_trigger.cpp
 * @brief    implementation of LifecyleTrigger class for send and receive lifecycle trigger commands
 *
 **/

#include <json/json.h>

#include <next/sdk/logger/logger.hpp>
#include "next/sdk/events/lifecycle_trigger.h"

namespace next {
namespace sdk {
namespace events {

void LifecycleTrigger::publishTrigger(const std::string &trigger_event, const std::string &component_name) {
  LifecycleTriggerMessage message{};
  message.trigger_event = trigger_event;
  message.component_name = component_name;
  message.timestamp = getCurrentNanoSeconds();
  publish(message);
}

payload_type LifecycleTrigger::serializeEventPayload(const LifecycleTriggerMessage &message) const {
  Json::Value json_val;
  json_val["timestamp"] = std::to_string(message.timestamp);
  json_val["component_name"] = message.component_name;
  json_val["trigger_event"] = message.trigger_event;

  Json::FastWriter fastWriter;
  std::string jsonised_string = fastWriter.write(json_val);
  return StringToPayload(jsonised_string);
}

LifecycleTriggerMessage LifecycleTrigger::deserializeEventPayload(const payload_type &strbuf) const {
  LifecycleTriggerMessage message{};

  Json::Value jsonVal;
  Json::Reader jsonReader;
  if (jsonReader.parse(PayloadToString(strbuf), jsonVal)) {
    if (jsonVal.isMember("timestamp")) {
      message.timestamp = std::stoull(jsonVal["timestamp"].asCString());
    } else {
      logger::warn(__FILE__, "LifecycleTrigger::deserializeEventPayload :: failed to find key [timestamp]");
    }

    if (jsonVal.isMember("component_name")) {
      message.component_name = jsonVal["component_name"].asString();
    } else {
      logger::warn(__FILE__, "LifecycleTrigger::deserializeEventPayload :: failed to find key [component_name]");
    }

    if (jsonVal.isMember("trigger_event")) {
      message.trigger_event = jsonVal["trigger_event"].asString();
    } else {
      logger::warn(__FILE__, "LifecycleTrigger::deserializeEventPayload :: failed to find key [trigger_event]");
    }
  } else {
    logger::warn(__FILE__, "LifecycleTrigger::deserializeEventPayload :: failed to parse input as json");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
