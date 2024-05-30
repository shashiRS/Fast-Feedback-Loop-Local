/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     session_event.cpp
 * @brief    implementation of SessionEvent class for sending and receiving SessionInformation
 *
 **/

#include "next/sdk/events/session_event.hpp"

#include <json/json.h>

#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace events {

void SessionEvent::publishLogEvent(const SessionClientInformation &log_data) { publish(log_data); }

payload_type SessionEvent::serializeEventPayload(const SessionClientInformation &message) const {
  Json::Value json_val;
  json_val["component_name"] = message.component_name;
  json_val["heartbeat"] = message.heartbeat;
  json_val["version"] = message.version;
  json_val["startup_time"] = message.startup_time;
  json_val["current_time"] = message.current_time;
  json_val["shutdown"] = message.shutdown;

  Json::FastWriter fastWriter;
  return StringToPayload(fastWriter.write(json_val));
}

SessionClientInformation SessionEvent::deserializeEventPayload(const payload_type &str_buf) const {
  SessionClientInformation message{};
  Json::Reader reader;
  Json::Value json_val;
  std::string json_str = PayloadToString(str_buf);
  if (reader.parse(json_str, json_val)) {
    if (json_val.isMember("component_name")) {
      message.component_name = json_val["component_name"].asString();
    } else {
      logger::warn(__FILE__, "SessionEvent:deserializeEventPayloar: Issue while deserializing: component_name");
    }
    if (json_val.isMember("heartbeat")) {
      message.heartbeat = json_val["heartbeat"].asInt();
    } else {
      logger::warn(__FILE__, "SessionEvent:deserializeEventPayloar: Issue while deserializing: component_name");
    }
    if (json_val.isMember("version")) {
      message.version = json_val["version"].asString();
    } else {
      logger::warn(__FILE__, "SessionEvent:deserializeEventPayloar: Issue while deserializing: component_name");
    }
    if (json_val.isMember("startup_time")) {
      message.startup_time = json_val["startup_time"].asInt64();
    } else {
      logger::warn(__FILE__, "SessionEvent:deserializeEventPayloar: Issue while deserializing: component_name");
    }
    if (json_val.isMember("current_time")) {
      message.current_time = json_val["current_time"].asInt64();
    } else {
      logger::warn(__FILE__, "SessionEvent:deserializeEventPayloar: Issue while deserializing: component_name");
    }
    if (json_val.isMember("shutdown")) {
      message.shutdown = json_val["shutdown"].asBool();
    } else {
      logger::warn(__FILE__, "SessionEvent:deserializeEventPayloar: Issue while deserializing: component_name");
    }
  } else {
    logger::warn(__FILE__, "SessionEvent::deserializeEventPayload :: failed to deserialize");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
