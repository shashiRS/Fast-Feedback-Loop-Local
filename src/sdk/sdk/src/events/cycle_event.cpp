/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cycle_event.cpp
 * @brief    implementation of CycleEvent class for send and receive Cycles
 *
 **/

#include "next/sdk/events/cycle_event.hpp"

#include <json/json.h>

#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace events {

CycleEvent::CycleEvent(std::string name) : BaseEventBinaryBlocking<Cycle>(name) {}

payload_type CycleEvent::serializeEventPayload(const Cycle &message) const {
  Json::Value json_val;
  json_val["cycle_id"] = message.cycle_id;
  json_val["cycle_state"] = kCycleStatus.at(message.cycle_state);

  Json::FastWriter fastWriter;
  std::string jsonised_string = fastWriter.write(json_val);
  return StringToPayload(jsonised_string);
}

Cycle CycleEvent::deserializeEventPayload(const payload_type &buf) const {
  Cycle message;

  Json::Value json_val;
  Json::Reader json_reader;
  std::string string_in = PayloadToString(buf);
  if (json_reader.parse(string_in, json_val) && json_val.isMember("cycle_id") && json_val.isMember("cycle_state")) {
    message.cycle_id = json_val["cycle_id"].asUInt();
    message.cycle_state = kCycleStatusStrToEnum.at(json_val["cycle_state"].asString());
  } else {
    logger::warn(__FILE__, "CycleEvent::deserializeEventPayload :: failed to deserialize");
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
