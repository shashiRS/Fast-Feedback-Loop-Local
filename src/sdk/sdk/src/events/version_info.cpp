/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     version_info.cpp
 * @brief    implementation of VersionInfo class for send and receive version info from different components
 *
 **/

#include <json/json.h>
#include <next/sdk/events/version_info.h>
#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace events {

payload_type VersionInfo::serializeEventPayload(const VersionInfoPackage &message) const {

  std::string jsonised_string;
  try {
    Json::Value json_val;
    json_val["component_name"] = message.component_name;
    json_val["component_version"] = message.component_version;
    json_val["documentation_link"] = message.documentation_link;

    Json::FastWriter fastWriter;
    jsonised_string = fastWriter.write(json_val);
  } catch (std::exception &e) {
    logger::warn(__FILE__, "StatusCode::serializeEventPayload :: failed to serializ: {}", e.what());
  }
  return StringToPayload(jsonised_string);
}

VersionInfoPackage VersionInfo::deserializeEventPayload(const payload_type &strbuf) const {
  VersionInfoPackage message{};
  Json::Reader reader;
  Json::Value json_val;
  std::string json_str = PayloadToString(strbuf);

  try {
    if (reader.parse(json_str, json_val)) {
      if (json_val.isMember("component_name")) {
        message.component_name = json_val["component_name"].asString();
      }
      if (json_val.isMember("component_version")) {
        message.component_version = json_val["component_version"].asString();
      }
      if (json_val.isMember("documentation_link")) {
        message.documentation_link = json_val["documentation_link"].asString();
      }
    } else {
      logger::warn(__FILE__, "VersionInfo::deserializeEventPayload");
    }
  } catch (std::exception &e) {
    logger::warn(__FILE__, " failed to deserialize the message: {}", e.what());
    return {};
  }
  return message;
}

} // namespace events
} // namespace sdk
} // namespace next
