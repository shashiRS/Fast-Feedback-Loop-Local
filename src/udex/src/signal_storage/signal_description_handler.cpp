/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * TODO This is just a first idea - the actual implementation has to be done
 * @file     signal_description_handler.cpp
 * @brief    class to handle the serialization of the signal descripton together
 * together with some meta informatoin
 */

#include "signal_description_handler.h"

#include <json/json.h>

#include <next/sdk/profiler/profile_tags.hpp>

namespace next {
namespace udex {
namespace ecal_util {

bool SignalDescriptionHandler::Reset() {
  description_container_ = {};
  return true;
}

SignalDescription SignalDescriptionHandler::GetDescription() const { return description_container_; }

std::vector<char> SignalDescriptionHandler::GetSignalDescription() const {
  return description_container_.binary_data_description;
}

std::string SignalDescriptionHandler::MakeStringFromVec(const std::vector<char> &_vec) const {
  std::string result = "";
  result.append(_vec.data(), _vec.size());
  return result;
}

bool SignalDescriptionHandler::SetBinarySignalDescription(const std::vector<char> &signal_description) {
  description_container_.binary_data_description = signal_description;
  return true;
}

bool SignalDescriptionHandler::SetBasicInformation(const BasicInformation &_basic_info) {
  description_container_.basic_info = _basic_info;
  return true;
}

std::string SignalDescriptionHandler::ToString(void) const {
  ProfileUdex_DTL;
  Json::Value data;
  data["basic_info"]["parent_url"] = description_container_.basic_info.parent_url;
  data["basic_info"]["dummy"] = description_container_.basic_info.dummy;
  data["basic_info"]["mode"] = description_container_.basic_info.mode;

  data["basic_info"]["data_source_name"] = description_container_.basic_info.data_source_name;
  data["basic_info"]["view_name"] = description_container_.basic_info.view_name;
  data["basic_info"]["group_name"] = description_container_.basic_info.group_name;
  data["basic_info"]["format_type"] =
      next::sdk::types::getPackageFormatTypeString(description_container_.basic_info.format_type);
  data["basic_info"]["cycle_id"] = description_container_.basic_info.cycle_id;
  data["basic_info"]["instance_id"] = description_container_.basic_info.instance_id;
  data["basic_info"]["source_id"] = description_container_.basic_info.source_id;
  data["basic_info"]["vaddr"] = description_container_.basic_info.vaddr;

  std::string serialized_string = MakeStringFromVec(description_container_.binary_data_description);
  data["binary_data_description"] = serialized_string;

  Json::FastWriter writer;

  return writer.write(data);
}

PackageMode SignalDescriptionHandler::TranslateIntToPackageMode(const int &mode_msg_) const {
  switch (mode_msg_) {
  case 0:
    return PACKAGE_MODE_SDL;
  case 1:
    return PACKAGE_MODE_CAN;
  case 2:
    return PACKAGE_MODE_FIBEX;
  case 3:
    return PACKAGE_MODE_BINARY;
  case 4:
    return PACKAGE_MODE_DEBUG;
  case -1:
  default:
    return PACKAGE_MODE_NOTSET;
  }
}

bool SignalDescriptionHandler::ParseSignalDescriptionFromString(const std::string &serialized_string) {
  ProfileUdex_DTL;
  Json::Reader reader;
  Json::Value description_json;

  bool parseSuccess = reader.parse(serialized_string, description_json, false);
  if (!parseSuccess) {
    return false;
  }
  if (!description_json.isMember("basic_info")) {
    return false;
  }

  if (!description_json["basic_info"].isMember("parent_url") || !description_json["basic_info"].isMember("dummy") ||
      !description_json["basic_info"].isMember("mode") ||
      !description_json["basic_info"].isMember("data_source_name") ||
      !description_json["basic_info"].isMember("view_name") || !description_json["basic_info"].isMember("group_name") ||
      !description_json["basic_info"].isMember("format_type") || !description_json["basic_info"].isMember("cycle_id") ||
      !description_json["basic_info"].isMember("vaddr") || !description_json["basic_info"].isMember("instance_id") ||
      !description_json["basic_info"].isMember("source_id")) {
    return false;
  }
  description_container_.basic_info.parent_url = description_json["basic_info"]["parent_url"].asString();
  description_container_.basic_info.dummy = description_json["basic_info"]["dummy"].asInt();
  description_container_.basic_info.mode = TranslateIntToPackageMode(description_json["basic_info"]["mode"].asInt());

  description_container_.basic_info.data_source_name = description_json["basic_info"]["data_source_name"].asString();
  description_container_.basic_info.view_name = description_json["basic_info"]["view_name"].asString();
  description_container_.basic_info.group_name = description_json["basic_info"]["group_name"].asString();
  description_container_.basic_info.format_type =
      next::sdk::types::getPackageFormatTypeEnum(description_json["basic_info"]["format_type"].asString());

  description_container_.basic_info.cycle_id = description_json["basic_info"]["cycle_id"].asUInt();
  description_container_.basic_info.vaddr = description_json["basic_info"]["vaddr"].asUInt64();
  description_container_.basic_info.instance_id = description_json["basic_info"]["instance_id"].asUInt();
  description_container_.basic_info.source_id = (uint16_t)description_json["basic_info"]["source_id"].asUInt();

  if (!description_json.isMember("binary_data_description")) {
    return false;
  }

  std::string desc_string = description_json["binary_data_description"].asString();
  if (desc_string.size() != 0) {
    description_container_.binary_data_description.assign(desc_string.begin(), desc_string.end());
  } else {
    description_container_.binary_data_description.clear();
  }

  return true;
}

std::string SignalDescriptionHandler::GetParentUrl() const { return description_container_.basic_info.parent_url; }
BasicInformation SignalDescriptionHandler::GetBasicInformation() const { return description_container_.basic_info; }

} // namespace ecal_util
} // namespace udex
} // namespace next
