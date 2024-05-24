/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_event.cpp
 * @brief    implementation of LogEvent class for send and receive LogEventData
 *
 **/

#include "next/sdk/events/log_event.hpp"

#include <json/json.h>

#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace events {

void LogEvent::publishLogEvent(const std::vector<LogEventData> &log_data) { publish(log_data); }

payload_type LogEvent::serializeEventPayload(const std::vector<LogEventData> &msgs) const {
  Json::Value root(Json::arrayValue);
  for (const auto &message : msgs) {
    Json::Value json_val;
    json_val["channel_name"] = message.channel_name;
    json_val["thread_id"] = std::to_string(message.thread_id);
    json_val["time_stamp"] = std::to_string(message.time_stamp);
    json_val["component_name"] = message.component_name;
    json_val["log_message"] = message.log_message;
    json_val["log_level"] = next::sdk::logger::kLogLevel.at(message.log_level);
    root.append(json_val);
  }

  Json::FastWriter fastWriter;
  std::string jsonised_string = fastWriter.write(root);
  return StringToPayload(jsonised_string);
}

std::vector<LogEventData> LogEvent::deserializeEventPayload(const payload_type &str_buf) const {
  std::vector<LogEventData> all_messages;
  Json::Reader reader;
  Json::Value value;
  std::string json_str = PayloadToString(str_buf);
  if (reader.parse(json_str, value)) {
    for (Json::ArrayIndex i = 0; i < value.size(); i++) {
      LogEventData message{};
      auto json_val = value[i];
      message.thread_id = std::stoul(json_val["thread_id"].asString());
      message.channel_name = json_val["channel_name"].asString();
      message.time_stamp = std::stoll(json_val["time_stamp"].asString());
      message.component_name = json_val["component_name"].asString();
      message.log_message = json_val["log_message"].asString();
      message.log_level = next::sdk::logger::kLogLevelStrToEnum.at(json_val["log_level"].asString());

      all_messages.push_back(message);
    }
  } else {
    logger::warn(__FILE__, "LogEvent::deserializeEventPayload :: failed to deserialize");
  }
  return all_messages;
}

} // namespace events
} // namespace sdk
} // namespace next
