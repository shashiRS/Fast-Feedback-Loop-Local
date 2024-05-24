/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_data_class_parser.h
 * @brief    Transforms Json messages from Plugin to match expected format
 */

#include "plugin_data_class_parser.h"

#include <next/sdk/sdk_macros.h>
#include <next/sdk/logger/logger.hpp>
#include <next/sdk/string_helper/strings.hpp>

namespace next {
namespace databridge {
namespace outputmanager {

constexpr uint64_t kMicSecToMilliSec = 1000;

bool replaceString(const std::string &from, const std::string &to, std::string &str) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

std::map<std::string, SupportedFbsTypes::supportedFBS> fbs_type_name_to_enum{
    {"CPU_Profiler", SupportedFbsTypes::supportedFBS::CPU_Profiler},
    {"EgoVehicle", SupportedFbsTypes::supportedFBS::EgoVehicle},
    {"FOV", SupportedFbsTypes::supportedFBS::FOV},
    {"FreeGeometry", SupportedFbsTypes::supportedFBS::FreeGeometry},
    {"GroundLines", SupportedFbsTypes::supportedFBS::GroundLines},
    {"NetworkGraph", SupportedFbsTypes::supportedFBS::NetworkGraph},
    {"Path", SupportedFbsTypes::supportedFBS::Path},
    {"PointCloud", SupportedFbsTypes::supportedFBS::PointCloud},
    {"Primitives", SupportedFbsTypes::supportedFBS::Primitive},
    {"RoadSign", SupportedFbsTypes::supportedFBS::RoadSign},
    {"TrafficParticipant", SupportedFbsTypes::supportedFBS::TrafficParticipant},
    {"Tunnel", SupportedFbsTypes::supportedFBS::Tunnel},
    {"SignalList", SupportedFbsTypes::supportedFBS::SignalList}};

std::map<std::string, std::string> fbs_type_name_to_gui_type_name{
    {"CPU_Profiler", "cpuProfiler"},  {"EgoVehicle", "egoVehicles"},
    {"FreeGeometry", "freeGeometry"}, {"GroundLines", "groundLines"},
    {"NetworkGraph", "dataTransfer"}, {"PointCloud", "pointClouds"},
    {"Primitives", "primitives"},     {"TrafficParticipant", "trafficParticipants"},
};

PluginDataClassParser::PluginDataClassParser() : meta_event_("") { meta_event_.subscribe(); }

bool PluginDataClassParser::parseDataClass(std::string &wrapped_message, const std::string &path_to_fbs,
                                           const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                           const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                           const std::pair<std::string, std::string> &structure_version_pair) {
  auto conversion_res = FbsToJson(path_to_fbs, flatbuffer, sensor_info, structure_version_pair, wrapped_message);
  if (!conversion_res) {
    // if fbs type name is unkown by GUI, send only flatbuffer content without wrapper around
    conversion_res = fbs_to_json_.toJsonFromFBSPlugin(path_to_fbs, flatbuffer.fbs_binary, wrapped_message);
  }

  return conversion_res;
}

namespace foxglove {

enum class BinaryOpcode : uint8_t {
  MESSAGE_DATA = 1,
  TIME_DATA = 2,
  SERVICE_CALL_RESPONSE = 3,
};

#pragma pack(push, 1)
#pragma pack(1)
//! Structure for sending data to foxglove, see
//! https://github.com/foxglove/ws-protocol/blob/main/docs/spec.md#message-data
struct MessageData {
  BinaryOpcode mode;
  uint32_t channel_id;
  uint64_t timestamp;
  NEXT_DISABLE_SPECIFIC_WARNING(4200, "-Wpedantic")
  char raw_buffer[0];
  NEXT_RESTORE_WARNINGS
};
#pragma pack(pop)

} // namespace foxglove

flatbuffers::Offset<Core::MetaInformation>
PluginDataClassParser::GetMetaInformation(flatbuffers::FlatBufferBuilder &builder,
                                          const bridgesdk::plugin::SensorInfoForFbs &sensor_info) const {

  std::string sensor_id_string = std::to_string(sensor_info.sensor_id).c_str();
  auto sensor_id = builder.CreateString(sensor_id_string);
  auto data_view = builder.CreateString(sensor_info.data_view);
  auto cache_name = builder.CreateString(sensor_info.cache_name.c_str());

  auto meta_info =
      Core::CreateMetaInformation(builder, sensor_info.timestamp_microsecond, sensor_id, cache_name, 100, data_view);

  return meta_info;
}

void PluginDataClassParser::parseDataClassToFlatbuffer(
    std::string &wrapped_message, const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
    const unsigned int subscription_id, const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
    const std::pair<std::string, std::string> &structure_version_pair) const {

  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  auto meta_info = GetMetaInformation(builder, sensor_info);

  auto fbs_type = mapFBStoEnum(structure_version_pair);
  switch (fbs_type) {
  case SupportedFbsTypes::supportedFBS::TrafficParticipant:
  case SupportedFbsTypes::supportedFBS::PointCloud:
  case SupportedFbsTypes::supportedFBS::Primitive:
  case SupportedFbsTypes::supportedFBS::GroundLines:
  case SupportedFbsTypes::supportedFBS::EgoVehicle: {
    auto view_update = AddObjectsToViewUpdate(flatbuffer, builder, fbs_type, meta_info);
    builder.Finish(view_update);
    break;
  }
  case SupportedFbsTypes::supportedFBS::SignalList: {

    auto data_update = AddObjectsToSignalUpdate(flatbuffer, builder, fbs_type, meta_info);
    builder.Finish(data_update);
    break;
  }
  default:
    break;
  }

  size_t msg_size = sizeof(foxglove::MessageData) + builder.GetSize();
  wrapped_message.resize(msg_size);

  foxglove::MessageData *msg = (foxglove::MessageData *)wrapped_message.data();
  msg->mode = foxglove::BinaryOpcode::MESSAGE_DATA;
  msg->channel_id = subscription_id;
  msg->timestamp = sensor_info.timestamp_microsecond;
  std::copy(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize(), msg->raw_buffer);
}

flatbuffers::Offset<GuiSchema::ViewUpdate> PluginDataClassParser::AddObjectsToViewUpdate(
    const bridgesdk::plugin::FlatbufferPackage &flatbuffer, flatbuffers::FlatBufferBuilder &builder,
    SupportedFbsTypes::supportedFBS fbs_type, const flatbuffers::Offset<Core::MetaInformation> &meta_info) const {

  flatbuffers::Offset<GuiSchema::ViewUpdate> view_update{};
  flatbuffers::Offset<GuiSchema::TrafficParticipantList> traffic_participant_list{};
  flatbuffers::Offset<GuiSchema::PrimitiveList> primitive_list{};
  flatbuffers::Offset<GuiSchema::PointCloudList> point_cloud_list{};
  flatbuffers::Offset<GuiSchema::GroundLineList> ground_line_list{};
  flatbuffers::Offset<GuiSchema::EgoVehicleList> ego_vehicle_list{};

  switch (fbs_type) {
  case SupportedFbsTypes::supportedFBS::TrafficParticipant: {
    auto traffic_participant_list_fb = GuiSchema::GetTrafficParticipantList(flatbuffer.fbs_binary);
    GuiSchema::TrafficParticipantListT list;
    traffic_participant_list_fb->UnPackTo(&list);
    traffic_participant_list = GuiSchema::TrafficParticipantList::Pack(builder, &list);
    break;
  }
  case SupportedFbsTypes::supportedFBS::PointCloud: {
    auto point_cloud_list_fb = GuiSchema::GetPointCloudList(flatbuffer.fbs_binary);
    GuiSchema::PointCloudListT list;
    point_cloud_list_fb->UnPackTo(&list);
    point_cloud_list = GuiSchema::PointCloudList::Pack(builder, &list);
    break;
  }
  case SupportedFbsTypes::supportedFBS::Primitive: {
    auto primitive_list_fb = GuiSchema::GetPrimitiveList(flatbuffer.fbs_binary);
    GuiSchema::PrimitiveListT list;
    primitive_list_fb->UnPackTo(&list);
    primitive_list = GuiSchema::PrimitiveList::Pack(builder, &list);
    break;
  }
  case SupportedFbsTypes::supportedFBS::GroundLines: {
    auto ground_line_list_fb = GuiSchema::GetGroundLineList(flatbuffer.fbs_binary);
    GuiSchema::GroundLineListT list;
    ground_line_list_fb->UnPackTo(&list);
    ground_line_list = GuiSchema::GroundLineList::Pack(builder, &list);
    break;
  }
  case SupportedFbsTypes::supportedFBS::EgoVehicle: {
    auto ego_vehicle_list_fb = GuiSchema::GetEgoVehicleList(flatbuffer.fbs_binary);
    GuiSchema::EgoVehicleListT list;
    ego_vehicle_list_fb->UnPackTo(&list);
    ego_vehicle_list = GuiSchema::EgoVehicleList::Pack(builder, &list);
    break;
  }
  default:
    break;
  }
  view_update = GuiSchema::CreateViewUpdate(builder, meta_info, primitive_list, traffic_participant_list,
                                            point_cloud_list, ground_line_list, ego_vehicle_list);

  return view_update;
}
flatbuffers::Offset<Core::SignalUpdate> PluginDataClassParser::AddObjectsToSignalUpdate(
    const bridgesdk::plugin::FlatbufferPackage &flatbuffer, flatbuffers::FlatBufferBuilder &builder,
    SupportedFbsTypes::supportedFBS fbs_type, const flatbuffers::Offset<Core::MetaInformation> &meta_info) const {

  flatbuffers::Offset<Core::SignalUpdate> data_update{};
  flatbuffers::Offset<Core::SignalList> signal_list{};

  switch (fbs_type) {
  case SupportedFbsTypes::supportedFBS::SignalList: {
    auto signal_list_fb = Core::GetSignalList(flatbuffer.fbs_binary);
    Core::SignalListT list;
    signal_list_fb->UnPackTo(&list);
    signal_list = Core::SignalList::Pack(builder, &list);
    break;
  }
  default:
    break;
  }

  data_update = Core::CreateSignalUpdate(builder, meta_info, signal_list);
  return data_update;
}

SupportedFbsTypes::supportedFBS
PluginDataClassParser::mapFBStoEnum(const std::pair<std::string, std::string> &structure_version_pair) const {
  auto index = fbs_type_name_to_enum.find(structure_version_pair.first);
  if (index == fbs_type_name_to_enum.end()) {
    // we did not find the supported fbs -> return custom if any is given
    if (structure_version_pair.first != "") {
      return SupportedFbsTypes::supportedFBS::Custom;
    }
    // no structure given at all -> not supported format for g -> refuse
    return SupportedFbsTypes::supportedFBS::UNIDENTIFIED;
  } else {
    return fbs_type_name_to_enum[structure_version_pair.first];
  }
}

uint64_t
PluginDataClassParser::GetCurrentTimestampMicroseconds(const bridgesdk::plugin::SensorInfoForFbs &sensor_info) {
  uint64_t result = 0;

  if (sensor_info.timestamp_microsecond != 0) {
    result = sensor_info.timestamp_microsecond;
  } else {
    result = meta_event_.getEventData().timestamp;
  }
  return result;
}

void PluginDataClassParser::makeJsonFromSignalInfo3dView(
    const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
    const std::pair<std::string, std::string> &structure_version_pair, Json::Value &output_json) {
  Json::Value meta_data{};
  meta_data["sensorUid"] = sensor_info.sensor_id;
  auto it_fbs_type_name_to_gui_type_name = fbs_type_name_to_gui_type_name.find(structure_version_pair.first);
  std::string gui_type_name{""};
  if (it_fbs_type_name_to_gui_type_name != std::end(fbs_type_name_to_gui_type_name)) {
    gui_type_name = it_fbs_type_name_to_gui_type_name->second;
  } else {
    debug(__FILE__, "Could not find related fbs type name '{0}' for gui.", structure_version_pair.first);
  }

  output_json["dataStream"][sensor_info.data_view][gui_type_name][sensor_info.cache_name]["metaData"] = meta_data;
  output_json["dataStream"][sensor_info.data_view][gui_type_name][sensor_info.cache_name]["data"] =
      kDataValuePlaceholder;
}

void PluginDataClassParser::makeJsonFromCustomType(const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                                   const std::pair<std::string, std::string> &structure_version_pair,
                                                   Json::Value &output_json) {
  debug(__FILE__, "Custom FBS used {}", structure_version_pair.first);
  if (sensor_info.cache_name.empty()) {
    output_json["dataStream"][sensor_info.data_view][structure_version_pair.first] = kDataValuePlaceholder;
  } else {
    output_json["dataStream"][sensor_info.data_view][structure_version_pair.first][sensor_info.cache_name] =
        kDataValuePlaceholder;
  }
}

void PluginDataClassParser::makeJsonFromSignalList(
    const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
    [[maybe_unused]] const std::pair<std::string, std::string> &structure_version_pair, Json::Value &output_json) {

  if (sensor_info.cache_name.empty()) {
    output_json["dataStream"][sensor_info.data_view] = kDataValuePlaceholder;
  } else {
    output_json["dataStream"][sensor_info.data_view][sensor_info.cache_name] = kDataValuePlaceholder;
  }
}

bool PluginDataClassParser::toJsonGeneralInfo(const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                              const std::pair<std::string, std::string> &structure_version_pair,
                                              std::string &output_general_info) {
  Json::Value output_json{};
  output_json["dataStream"]["timestamp"] = GetCurrentTimestampMicroseconds(sensor_info);

  switch (mapFBStoEnum(structure_version_pair)) {
  case SupportedFbsTypes::supportedFBS::CPU_Profiler:
    output_json["dataStream"][sensor_info.data_view]["cpuProfiler"] = kDataValuePlaceholder;
    break;
  case SupportedFbsTypes::supportedFBS::NetworkGraph:
    output_json["dataStream"][sensor_info.data_view]["dataTransfer"] = kDataValuePlaceholder;
    break;
  case SupportedFbsTypes::supportedFBS::PointCloud:
  case SupportedFbsTypes::supportedFBS::GroundLines:
  case SupportedFbsTypes::supportedFBS::TrafficParticipant:
  case SupportedFbsTypes::supportedFBS::EgoVehicle:
  case SupportedFbsTypes::supportedFBS::Primitive:
  case SupportedFbsTypes::supportedFBS::FreeGeometry:
    makeJsonFromSignalInfo3dView(sensor_info, structure_version_pair, output_json);
    break;

  case SupportedFbsTypes::supportedFBS::Custom:
    makeJsonFromCustomType(sensor_info, structure_version_pair, output_json);
    break;
  case SupportedFbsTypes::supportedFBS::SignalList:
    makeJsonFromSignalList(sensor_info, structure_version_pair, output_json);
    break;
  default:
    // raise error
    throw std::invalid_argument(structure_version_pair.first + " is not a supported FBS!");
    return false;
  }

  Json::FastWriter writer{};
  output_general_info = writer.write(output_json);
  return (!output_general_info.empty());
}

void removeFbsTypeNameFromJson(std::string &fbs_json) {
  auto pos = fbs_json.find("[");
  if (pos != fbs_json.npos) {
    fbs_json.replace(0, pos, "");
  }

  pos = fbs_json.rfind("}");
  if (pos != fbs_json.npos) {
    fbs_json.replace(pos, fbs_json.length(), "");
  }
}

bool PluginDataClassParser::FbsToJson(const std::string &path_to_fbs,
                                      const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                      const bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                      const std::pair<std::string, std::string> &structure_version_pair,
                                      std::string &output_message) {
  bool is_valid{true};
  std::string json_plugin_output{};
  is_valid &= fbs_to_json_.toJsonFromFBSPlugin(path_to_fbs, flatbuffer.fbs_binary, json_plugin_output);
  is_valid &= toJsonGeneralInfo(sensor_info, structure_version_pair, output_message);
  auto current_fbs_type = mapFBStoEnum(structure_version_pair);
  if (current_fbs_type != SupportedFbsTypes::supportedFBS::Primitive &&
      current_fbs_type != SupportedFbsTypes::supportedFBS::CPU_Profiler &&
      current_fbs_type != SupportedFbsTypes::supportedFBS::NetworkGraph &&
      current_fbs_type != SupportedFbsTypes::supportedFBS::SignalList) {
    removeFbsTypeNameFromJson(json_plugin_output);
  }

  if (current_fbs_type == SupportedFbsTypes::supportedFBS::SignalList) {
    json_plugin_output = UpdateJsonSignals(json_plugin_output);
  }

  // Adding plugin related signal info to the final message output.
  is_valid &= replaceString("\"" + kDataValuePlaceholder + "\"", json_plugin_output, output_message);
  return is_valid;
}

void PluginDataClassParser::AddNewSignalInfoToCache(const Json::Value &content) {
  if (!content.isMember("new_signal_info")) {
    // no signal info received, assume it's already in cache
  } else {
    // get signals information and store in cache
    auto new_signal_info = content["new_signal_info"];
    if (!new_signal_info.empty()) {
      for (const auto &signal_information : new_signal_info) {
        if (signal_information.isMember("signal_name") && signal_information.isMember("hash")) {
          signals_information_cache_.emplace(static_cast<size_t>(signal_information["hash"].asUInt64()),
                                             signal_information["signal_name"].asString());
        }
      }
    } else {
      // empty signal info object received, assume info is already in cache
    }
  }
}

std::string PluginDataClassParser::FormatSignalListToJson(const Json::Value &content) {
  Json::Value output;
  if (!content.isMember("signals")) {
    // just abort
    debug(__FILE__, "No 'signals' object received");
    return R"("")";
  } else {
    auto signals = content["signals"];
    if (signals.empty()) {
      debug(__FILE__, "Received empty 'signals' object");
      return R"("")";
    }

    size_t signals_found = signals.size();
    for (const auto signal : signals) {
      Json::Value *current = &output;

      size_t hash = signal["hash"].asUInt64();
      [[maybe_unused]] const auto &value_type = signal["value_type"].asString(); // not sure if needed here yet
      const auto &value = signal["value"];

      // if hash is not in cache, continue to next signal
      if (signals_information_cache_.find(hash) == signals_information_cache_.end()) {
        warn(__FILE__, "Hash {0} does not have associated signal name in cache!", hash);
        signals_found--;
        continue;
      }

      const std::string signal_name = signals_information_cache_.at(hash);

      // split signal on dots
      const auto &sections = next::sdk::string::split(signal_name, ".");

      // append to output json
      for (size_t i = 0; i < sections.size(); ++i) {
        // navigate or create a new node if it doesn't exist
        if (!(*current).isMember(sections[i])) {
          (*current)[sections[i]] = Json::Value();
        }
        current = &(*current)[sections[i]]; // move to the next level in the JSON tree
      }

      // set the signal value
      *current = value["value"];
    }

    if (signals_found == 0) {
      return R"("")";
    }
  }

  return output.toStyledString();
}

std::string PluginDataClassParser::UpdateJsonSignals(const std::string &input) {
  Json::Value content;
  Json::Reader reader;

  if (!reader.parse(input, content, false)) {
    return R"("")";
  }

  AddNewSignalInfoToCache(content);

  return FormatSignalListToJson(content);
}

} // namespace outputmanager
} // namespace databridge
} // namespace next
