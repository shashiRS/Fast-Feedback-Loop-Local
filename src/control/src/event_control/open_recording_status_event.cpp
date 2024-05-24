/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_recording_status_event.cpp
 * @brief    meta event for Next Player Recording loading status
 *
 **/

#include <json/json.h>
#include <next/control/event_types/open_recording_status_event.h>
#include "next/sdk/logger/logger.hpp"

namespace next {
namespace control {
namespace events {

next::sdk::events::payload_type
OpenRecordingStatusEvent::serializeEventPayload(const OpenRecordingStatusInformationMessage &message) const {

  std::string jsonised_string;
  try {
    Json::Value json_val;
    json_val["executed_function"] = message.executed_function;
    json_val["description"] = message.description;
    json_val["areIndexesUsed"] = message.areIndexesUsed;
    json_val["maxIndex"] = message.maxIndex;
    json_val["currentIndex"] = message.currentIndex;

    Json::FastWriter fastWriter;
    jsonised_string = fastWriter.write(json_val);
  } catch (std::exception &e) {
    next::sdk::logger::warn(__FILE__, "StatusCode::serializeEventPayload :: failed to serializ: {}", e.what());
  }
  return next::sdk::events::StringToPayload(jsonised_string);
}

OpenRecordingStatusInformationMessage
OpenRecordingStatusEvent::deserializeEventPayload(const next::sdk::events::payload_type &strbuf) const {
  OpenRecordingStatusInformationMessage message{};
  Json::Reader reader;
  Json::Value json_val;
  std::string json_str = next::sdk::events::PayloadToString(strbuf);

  try {
    if (reader.parse(json_str, json_val)) {
      if (json_val.isMember("executed_function")) {
        message.executed_function = json_val["executed_function"].asString();
      }
      if (json_val.isMember("description")) {
        message.description = json_val["description"].asString();
      }
      if (json_val.isMember("areIndexesUsed")) {
        message.areIndexesUsed = json_val["areIndexesUsed"].asBool();
      }
      if (json_val.isMember("maxIndex")) {
        message.maxIndex = json_val["maxIndex"].asUInt64();
      }
      if (json_val.isMember("currentIndex")) {
        message.currentIndex = json_val["currentIndex"].asUInt64();
      }

    } else {
      next::sdk::logger::warn(__FILE__, "VersionInfo::deserializeEventPayload");
    }
  } catch (std::exception &e) {
    next::sdk::logger::warn(__FILE__, " failed to deserialize the message: {}", e.what());
    return {};
  }
  return message;
}

void OpenRecordingStatusEvent::publish(const OpenRecordingStatusInformationMessage &message) {
  std::list<std::string> res;
  next::sdk::events::BaseEventBinary<OpenRecordingStatusInformationMessage>::publish(message, res);
}
void OpenRecordingStatusEvent::send() { publish(message_); }

} // namespace events
} // namespace control
} // namespace next
