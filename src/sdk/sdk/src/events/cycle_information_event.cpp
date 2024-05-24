/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cycle_information_event.cpp
 * @brief    implementation of CycleInformationEvent class for send and receive CycleInformation
 *
 **/

#include "next/sdk/events/cycle_information_event.hpp"

#include <json/json.h>

#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace events {

payload_type CycleInformationEvent::serializeEventPayload(const CycleInformation &message) const {
  Json::Value json_val;

  for (const auto &pair : message) {
    Json::Value jp;
    jp["device_name"] = pair.first;
    jp["id"] = pair.second;
    json_val.append(jp);
  }

  Json::FastWriter fastWriter;
  std::string jsonised_string = fastWriter.write(json_val);
  return StringToPayload(jsonised_string);
}

CycleInformation CycleInformationEvent::deserializeEventPayload(const payload_type &vector_buf) const {
  CycleInformation message;

  Json::Value json_val;
  Json::Reader json_reader;
  if (json_reader.parse(PayloadToString(vector_buf), json_val) && json_val.isArray()) {
    for (auto it = json_val.begin(); it != json_val.end(); ++it) {
      const auto &elem = *it;
      if (elem.isMember("device_name") && elem.isMember("id")) {
        message.push_back(std::make_pair<std::string, uint32_t>(elem["device_name"].asString(), elem["id"].asUInt()));
      }
    }
  } else {
    logger::warn(__FILE__, "CycleInformation::deserializeEventPayload :: failed to deserialize");
  }

  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
