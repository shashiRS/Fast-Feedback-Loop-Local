/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     ego_vehicle.cpp
 * @brief    Parsing network data to provide ego vehicle
 *
 *
 **/
#include "ego_vehicle.h"

#include <json/json.h>
#include <math.h>
#include <unordered_set>
#include "url_mapper.h"

#include <next/bridgesdk/schema/3d_view/egovehicle_generated.h>
#include <next/bridgesdk/schema/3d_view/primitive_generated.h>
#include <next/bridgesdk/plugin_addons/plugin_helper_data_extractor.hpp>

#include <next/plugins_utils/helper_functions.h>
#include <next/plugins_utils/plugin_3d_view_utils.hpp>

namespace next {
namespace plugins {
using namespace next::plugins_utils;

EgoVehiclePlugin::EgoVehiclePlugin(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello view3D_EgoVehicle plugin");
  this->setVersion(std::string("1.0.0"));
  this->setName(std::string("view3D_EgoVehicle"));
  path_to_fbs_line_ = path + std::string("/primitive.fbs");
  path_to_bfbs_line_ = path + std::string("/primitive.bfbs");
  path_to_fbs_ = path + std::string("/egovehicle.fbs");
  path_to_bfbs_ = path + std::string("/egovehicle.bfbs");

  InitSearchRequests();
  SetRegexValuesInConfig();
}

EgoVehiclePlugin::~EgoVehiclePlugin() {}

void EgoVehiclePlugin::InitSearchRequests() {
  search_requests_.clear();

  bool optional = true;

  keywords_ = {"OdoEstimationPort"};

  std::vector<std::tuple<std::string, std::vector<std::string>, bool>> requests = {
      {"ego_pos_x", {R"(\.(xposition_m))"}, !optional},
      {"ego_pos_y", {R"(\.(yposition_m))"}, !optional},
      {"ego_yaw_angle", {R"(\.(yawangle_rad))"}, !optional},
      {"ego_vel_x", {R"(\.(xvelocity_mps))"}, !optional},
      {"ego_yaw_rate", {R"(\.(yawrate_radps))"}, !optional},
      {"timestamp", {R"(\.(timestamp_us|ssigheader\.uitimestamp))"}, !optional}};

  for (const auto &[key, patterns, isOptional] : requests) {
    search_requests_.emplace_back(SearchRequest{key, patterns, isOptional});
  }

  if (!plugin_regex_searcher_) {
    return;
  }

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

void EgoVehiclePlugin::SetRegexValuesInConfig() {
  auto config_client = next::appsupport::ConfigClient::getConfig();
  if (!config_client) {
    debug(__FILE__, "{0}::SetRegexValuesInConfig()::couldn't access configuration to update regex", getName());
    return;
  }

  auto setRegexInConfig = [&config_client](const std::string &cfg_keyword, std::vector<std::string> regex_list) {
    for (size_t i = 0; i < regex_list.size(); i++) {
      config_client->put(cfg_keyword + fmt::format("[{}]", i), regex_list[i]);
    }
  };

  std::lock_guard<std::mutex> lock(config_regex_protector_);
  const std::string regex_root_config = next::appsupport::configkey::databridge::getPluginRegexConfigRootKey(getName());

  auto root_children = config_client->getChildren(regex_root_config); // check if root doesn't already exist
  if (root_children.empty()) {
    setRegexInConfig(next::appsupport::configkey::databridge::getPluginRegexConfigEntryKey(getName(), "keywords"),
                     keywords_);

    for (const auto &search_request : search_requests_) {
      setRegexInConfig(next::appsupport::configkey::databridge::getPluginRegexConfigEntryKey(
                           getName(), search_request.data_type_name),
                       search_request.search_expression);
    }
  }
}

void EgoVehiclePlugin::UpdateRegexValuesFromConfig() {
  auto config_client = next::appsupport::ConfigClient::getConfig();
  if (!config_client) {
    warn(__FILE__, "{0}::UpdateRegexValuesFromConfig::couldn't access configuration to read regex values", getName());
    return;
  }

  if (!plugin_regex_searcher_) {
    warn(__FILE__, "{0}::UpdateRegexValuesFromConfig::cannot access regex searcher", getName());
    return;
  }

  std::lock_guard<std::mutex> lock(config_regex_protector_);
  const std::string regex_root_config = next::appsupport::configkey::databridge::getPluginRegexConfigRootKey(getName());

  auto root_children = config_client->getChildren(regex_root_config);
  if (root_children.empty()) {
    return;
  }
  keywords_ = config_client->getStringList(
      next::appsupport::configkey::databridge::getPluginRegexConfigEntryKey(getName(), "keywords"), {});

  for (auto &search_request : search_requests_) {
    search_request.search_expression = config_client->getStringList(
        next::appsupport::configkey::databridge::getPluginRegexConfigEntryKey(getName(), search_request.data_type_name),
        {});
  }

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

std::vector<EgoVehiclePlugin::SearchResult>
EgoVehiclePlugin::GetSearchResults(const std::string &url, const std::unordered_set<std::string> &unique_urls) const {
  if (!plugin_regex_searcher_) {
    return {};
  }

  if (url.empty())
    return plugin_regex_searcher_->GetSearchResults(unique_urls);
  else
    return {plugin_regex_searcher_->GetSearchResultForUrl(url, unique_urls)};
}

std::vector<EgoVehiclePlugin::SearchResult> EgoVehiclePlugin::FindResultsByUrl(const std::string &url) {
  std::vector<SearchResult> result{};
  if (!plugin_signal_explorer_) {
    return result;
  }

  if (!plugin_regex_searcher_) {
    return result;
  }

  UpdateRegexValuesFromConfig();

  std::unordered_set<std::string> unique_urls_to_scan{};
  if (url.empty()) {
    for (auto const &keyword : keywords_) {
      // if group url is empty, scan all signals containing the keywords
      auto results = plugin_signal_explorer_->searchSignalTree(keyword);
      unique_urls_to_scan.reserve(unique_urls_to_scan.size() + results.size());
      std::move(results.begin(), results.end(), std::inserter(unique_urls_to_scan, unique_urls_to_scan.end()));
    }
  } else {
    // if group is already selected, scan only the children of that group
    auto results = plugin_signal_explorer_->searchSignalTree(url);
    unique_urls_to_scan.reserve(results.size());
    std::move(results.begin(), results.end(), std::inserter(unique_urls_to_scan, unique_urls_to_scan.end()));
  }

  result = GetSearchResults(url, unique_urls_to_scan);

  // exclude results which are in subgroups
  RemoveSubgroupResults(result, unique_urls_to_scan);
  return result;
}

void EgoVehiclePlugin::RemoveSubgroupResults(std::vector<EgoVehiclePlugin::SearchResult> &results,
                                             const std::unordered_set<std::string> &urls) {
  if (results.size() == 0) {
    return;
  }

  // reconstruct the string results and check if they match the urls scanned
  for (auto it = results.begin(); it != results.end();) {
    bool remove_result = false;

    for (const auto &mapping : it->data_type_to_result) {
      std::string tmp = it->root_url;
      tmp += mapping.second.result_url;
      if (urls.count(tmp) == 0) {
        remove_result = true;
        break;
      }
    }

    if (remove_result) {
      it = results.erase(it);
    } else {
      it++;
    }
  }
}

bool EgoVehiclePlugin::GetNamingMap(const std::string &url, std::unordered_map<std::string, std::string> &naming_map) {
  if (url.empty())
    return false;

  auto result = FindResultsByUrl(url);
  if (result.empty()) {
    warn(__FILE__, "{0}::GetNamingMap::Unable to create naming map for {1}: URLs not found", url);
    return false;
  }

  for (const auto &res : result[0].data_type_to_result)
    naming_map[res.first] = res.second.result_url;

  return true;
}

bool EgoVehiclePlugin::getDescription(next::bridgesdk::plugin_config_t &config_fields) {

  config_fields.insert({"sensor_name", {next::bridgesdk::ConfigType::EDITABLE, "Ego Vehicle Signal"}});
  config_fields.insert({"color", {next::bridgesdk::ConfigType::EDITABLE, "#FFFFFF"}});

  auto ego_vehicle_data = FindResultsByUrl("");
  if (!ego_vehicle_data.empty()) {
    for (const auto &ego_vehicle : ego_vehicle_data) {
      config_fields.insert({"base url", {next::bridgesdk::ConfigType::SELECTABLE, ego_vehicle.root_url}});
    }
  } else {

    std::string keyword_list_string;
    for (auto &keystr : keywords_) {
      keyword_list_string += keystr + ":";
    }
    warn(__FILE__, "{0}::getDescription::Unable to find valid EgoVehicle Urls for keywords: {1}", getName(),
         keyword_list_string);
    return false;
  }

  config_fields.insert({"veh_update_rate", {next::bridgesdk::ConfigType::EDITABLE, "60"}});

  config_fields.insert({"enable_trajectory", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "ENABLE"}});
  config_fields.insert({"enable_trajectory", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "DISABLE"}});
  config_fields.insert({"sensor_name_tr", {next::bridgesdk::ConfigType::EDITABLE, "Ego Vehicle Trajectory"}});

  for (const auto offset_config_map_entry : offset_config_to_enum_) {
    config_fields.insert(
        {"offset configuration", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, offset_config_map_entry.first}});
  }

  config_fields.insert({"offset X (value or URL)", {next::bridgesdk::ConfigType::EDITABLE, "0"}});
  config_fields.insert({"offset Y (value or URL)", {next::bridgesdk::ConfigType::EDITABLE, "0"}});

  return true;
}

void EgoVehiclePlugin::ProcessDataCallbackOffset(
    [[maybe_unused]] const std::string topic_name,
    [[maybe_unused]] const next::bridgesdk::ChannelMessagePointers *datablock,
    std::map<std::string, std::string> parameters) {

  const std::string url_offset_x{parameters["offset_x"]};
  next::bridgesdk::plugin_addons::dataType offset_x{0.0f};
  if (!plugin_signal_extractor_->GetValue(url_offset_x, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT,
                                          offset_x)) {
    warn(__FILE__, "Could not extract value for: {0}", url_offset_x);
    return;
  }
  offset_.offset_x = std::get<float>(offset_x);

  const std::string url_offset_y{parameters["offset_y"]};
  next::bridgesdk::plugin_addons::dataType offset_y{0.0f};
  if (!plugin_signal_extractor_->GetValue(url_offset_y, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT,
                                          offset_y)) {
    warn(__FILE__, "Could not extract value for: {0}", url_offset_y);
    return;
  }
  offset_.offset_y = std::get<float>(offset_y);
}

void EgoVehiclePlugin::ProcessEgoVehicleCallback(
    [[maybe_unused]] const std::string topic_name, const next::bridgesdk::ChannelMessagePointers *datablock,
    const next::plugins_utils::SensorConfig sensor_config_ego_position,
    const next::plugins_utils::SensorConfig sensor_config_driver_trajectory, std::shared_ptr<EgoVehicle> ego_vehicle,
    uint64_t n_veh_update_rate) {

  veh_sensor_config_[sensor_config_ego_position.sensor_id].cache_frame_counter_++;
  if (veh_sensor_config_[sensor_config_ego_position.sensor_id].cache_frame_counter_ % n_veh_update_rate != 0) {
    return;
  }

  std::lock_guard<std::mutex> egoVehicleLock{ego_vehicle->ego_vehicle_protector_};

  ego_vehicle->SetAddress((char *)datablock->data, datablock->size);

  float f_posx = ego_vehicle->pos_x_->Get();
  float f_posy = ego_vehicle->pos_y_->Get();
  float f_yaw_angle = ego_vehicle->yaw_angle_->Get();

  float f_vel_x = ego_vehicle->vel_x_->Get();
  float f_yaw_rate = ego_vehicle->yaw_rate_->Get();
  uint64_t timestamp_ego_vehicle = ego_vehicle->timestamp_->Get();

  /*
    If there is an offset to be added, we start rotating the coordinate system clockwise through the angle "yaw_angle"
    while the origin is kept fixed, to obtain an intermediate point P(x,y), which will be located in the usual
    Cartesian coordinate system.
    The transformation matrix is [ x ]     [  cos(angle)  sin(angle) ] [ f_posx ]
                                 |   |  =  |                         |*|        |
                                 [ y ]     [ -sin(angle)  cos(angle) ] [ f_posy ]

    Point P will be then used to apply the offset for x and y.
    After the offset is applied, we rotate the coordinate system back to the original state.

    The inverse transformation matrix is [ f_posx ]     [ cos(angle)  -sin(angle) ] [ x ]
                                         |        |  =  |                         |*|   |
                                         [ f_posy ]     [ sin(angle)   cos(angle) ] [ y ]
   */
  if (offset_.offset_cfg != offset_config::No_offset) {
    // Rotating the coordinate system clockwise
    auto x = f_posx * cos(f_yaw_angle) + f_posy * sin(f_yaw_angle);
    auto y = -f_posx * sin(f_yaw_angle) + f_posy * cos(f_yaw_angle);

    // Applying the offset for x and y
    x = x + offset_.offset_x;
    y = y + offset_.offset_y;

    // Rotating the coordinate system counterclockwise, back to the original state
    f_posx = x * cos(f_yaw_angle) - y * sin(f_yaw_angle);
    f_posy = x * sin(f_yaw_angle) + y * cos(f_yaw_angle);
  }

  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  auto fbs_ego_vehicle =
      GuiSchema::CreateEgoVehicle(builder, 2.5F, 6.5F, f_posx, f_posy, f_yaw_angle, f_vel_x, f_yaw_rate, 1.0f);
  std::vector<flatbuffers::Offset<GuiSchema::EgoVehicle>> ego_vector;
  ego_vector.push_back(fbs_ego_vehicle);
  auto fbs_vector = GuiSchema::CreateEgoVehicleListDirect(builder, &ego_vector);
  builder.Finish(fbs_vector);

  bridgesdk::plugin::FlatbufferPackage flatbuffer_ego_vehicle;
  flatbuffer_ego_vehicle.fbs_binary = builder.GetBufferPointer();
  flatbuffer_ego_vehicle.fbs_size = builder.GetSize();

  std::pair<std::string, std::string> structure_version_pair = {"EgoVehicle", "0.0.1"};

  bridgesdk::plugin::SensorInfoForFbs ego_veh_info;
  ego_veh_info.data_view = sensor_config_ego_position.data_view_name;
  ego_veh_info.sensor_id = sensor_config_ego_position.sensor_id;
  ego_veh_info.cache_name =
      sensor_config_ego_position.data_cache_name + std::to_string(sensor_config_ego_position.sensor_id);
  ego_veh_info.timestamp_microsecond = datablock->time;

  plugin_publisher_->sendFlatbufferData(path_to_fbs_, flatbuffer_ego_vehicle, ego_veh_info, structure_version_pair);

  if (!trajectory_info_[sensor_config_driver_trajectory.sensor_id].enable_trajectory) {
    return;
  }

  // use ego position without offset for the trajectory, since the offset is only meant to change the pivot point of the
  // ego vehicle 3D model
  createAndSendLine(ego_vehicle->pos_x_->Get(), ego_vehicle->pos_y_->Get(), sensor_config_driver_trajectory,
                    datablock->time, timestamp_ego_vehicle);
  // builder.Clear();
}

void EgoVehiclePlugin::createAndSendLine(float x_value, float y_val, next::plugins_utils::SensorConfig sensor_config,
                                         uint64_t timestamp, uint64_t timestamp_ego) {
  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);
  auto line_base =
      GuiSchema::CreatePrimitiveBasic(builder, GuiSchema::PrimitiveType::line, 1,
                                      flatbuffers::Offset<flatbuffers::String>{builder.CreateString("#FFFFFF")}, 1.0F);
  GuiSchema::XYZ old_coordinate{veh_sensor_config_[sensor_config.sensor_id].ego_x_distance,
                                veh_sensor_config_[sensor_config.sensor_id].ego_y_distance, 1.6F};
  GuiSchema::XYZ new_coordinate{x_value, y_val, 1.6F};

  std::vector<GuiSchema::XYZ> points;
  points.push_back(old_coordinate);
  points.push_back(new_coordinate);

  veh_sensor_config_[sensor_config.sensor_id].ego_x_distance = x_value;
  veh_sensor_config_[sensor_config.sensor_id].ego_y_distance = y_val;
  std::vector<flatbuffers::Offset<GuiSchema::Line>> line_vector;
  flatbuffers::Offset<GuiSchema::Line> val = GuiSchema::CreateLineDirect(builder, line_base, &points);
  builder.Finish(val);

  line_vector.push_back(val);
  auto fbs_vector = GuiSchema::CreatePrimitiveListDirect(builder, nullptr, nullptr, nullptr, nullptr, nullptr,
                                                         &line_vector, nullptr, nullptr, nullptr);
  builder.Finish(fbs_vector);

  bridgesdk::plugin::FlatbufferPackage flatbuffer_driver_trajectory;
  flatbuffer_driver_trajectory.fbs_binary = builder.GetBufferPointer();
  flatbuffer_driver_trajectory.fbs_size = builder.GetSize();

  bridgesdk::plugin::SensorInfoForFbs trajectory_info_fbs;
  trajectory_info_fbs.data_view = sensor_config.data_view_name;
  trajectory_info_fbs.sensor_id = sensor_config.sensor_id;
  trajectory_info_fbs.cache_name =
      "VehTraj-" + std::to_string(sensor_config.sensor_id) + "-frame-nr-" + std::to_string(timestamp_ego);
  trajectory_info_fbs.timestamp_microsecond = timestamp;

  plugin_publisher_->sendFlatbufferData(path_to_fbs_line_, flatbuffer_driver_trajectory, trajectory_info_fbs,
                                        {"Primitives", "0.0.1"});
}

bool EgoVehiclePlugin::init() {
  debug(__FILE__, "init  {0}", __FILE__);
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  plugin_regex_searcher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginRegexSearcher>();

  InitSearchRequests();
  return true;
}

bool EgoVehiclePlugin::removeConfig(const next::bridgesdk::plugin_config_t &config) {

  debug(__FILE__, "{0}::removeConfig was called", getName());
  bool retValue = true;

  auto hash_key = CreateKeyOutOfPluginConfig(config);

  // remove sensor ego
  auto it_active_config_to_sensors_ego = active_config_to_sensors_ego_.find(hash_key);

  if (it_active_config_to_sensors_ego == active_config_to_sensors_ego_.end()) {
    warn(__FILE__, "{0}::removeConfig::config to remove for sensor ego was not found. removeConfig failed", getName());
    retValue = false;
  } else {
    auto &sensor_ego = it_active_config_to_sensors_ego->second;
    plugin_3D_view_->unregisterAndPublishSensor(sensor_ego.sensor_id);

    for (auto id : sensor_ego.subscription_id)
      plugin_data_subscriber_->unsubscribe(id);

    auto it_ego_vehicle = ego_vehicle_lists_.find(hash_key);

    if (it_ego_vehicle != ego_vehicle_lists_.end()) {
      ego_vehicle_lists_.erase(hash_key);
    } else {
      warn(__FILE__, "{0}::removeConfig::sensor to remove traffic_list was not found. removeConfig failed", getName());
      retValue = false;
    }

    veh_sensor_config_.erase(sensor_ego.sensor_id);

    active_config_to_sensors_ego_.erase(hash_key);
  }

  // remove sensor trajectory
  auto it_active_config_to_sensors_tr = active_config_to_sensors_trajectory_.find(hash_key);

  if (it_active_config_to_sensors_tr == active_config_to_sensors_trajectory_.end()) {
    warn(__FILE__, "{0}::removeConfig::config to remove for sensor trajectory was not found. removeConfig failed",
         getName());
    retValue = false;
  } else {
    auto &sensor_tr = it_active_config_to_sensors_tr->second;
    plugin_3D_view_->unregisterAndPublishSensor(sensor_tr.sensor_id);

    for (auto id : sensor_tr.subscription_id)
      plugin_data_subscriber_->unsubscribe(id);

    trajectory_info_.erase(sensor_tr.sensor_id);

    active_config_to_sensors_trajectory_.erase(hash_key);
  }

  return retValue;
}

bool EgoVehiclePlugin::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool EgoVehiclePlugin::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {

  config_ = config;

  next::plugins_utils::SensorConfig sensor_config_ego_position;
  std::string url{};
  std::string color;
  std::string type;

  if (!GetValueFromPluginConfig(config, "base url", url)) {
    warn(__FILE__, " ego vehicle wrong url mapping name: base url");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "sensor_name", sensor_config_ego_position.sensor_name)) {
    warn(__FILE__, " Ego vehicle plugin unable to find sensor_name in plugin config");
    return false;
  }

  std::string sensor_tr{""};
  if (!GetValueFromPluginConfig(config, "sensor_name_tr", sensor_tr)) {
    warn(__FILE__, " Ego vehicle plugin unable to find sensor_name_tr in plugin config");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "DATA_VIEW", sensor_config_ego_position.data_view_name)) {
    warn(__FILE__, " ego vehicle wrong data view mapping name: DATA_VIEW");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "color", color)) {
    warn(__FILE__, " Ego vehicle plugin unable to find color in plugin config");
    return false;
  }

  std::string update_rate;
  if (!GetValueFromPluginConfig(config, "veh_update_rate", update_rate)) {
    warn(__FILE__, " Ego vehicle plugin unable to find veh_update_rate in plugin config");
    return false;
  }

  std::string offset_configuration;
  if (!GetValueFromPluginConfig(config, "offset configuration", offset_configuration)) {
    warn(__FILE__, " Ego vehicle plugin unable to find offset configuration in plugin config");
    return false;
  }
  offset_.offset_cfg = offset_config_to_enum_.at(offset_configuration);

  std::string offset_x;
  if (!GetValueFromPluginConfig(config, "offset X (value or URL)", offset_x)) {
    warn(__FILE__, " Ego vehicle plugin unable to find offset X in plugin config");
    return false;
  }

  std::string offset_y;
  if (!GetValueFromPluginConfig(config, "offset Y (value or URL)", offset_y)) {
    warn(__FILE__, " Ego vehicle plugin unable to find offset Y in plugin config");
    return false;
  }

  std::string enable_traj{};
  if (!GetValueFromPluginConfig(config, "enable_trajectory", enable_traj)) {
    warn(__FILE__, " ego vehicle wrong data view mapping name: enable_trajectory");
    return false;
  }

  if (!isNumber(update_rate)) {
    warn(__FILE__, " ego vehicle plugin bad parameter: veh_update_rate. Int value required");
    return false;
  }

  if (std::stoi(update_rate) < 0) {
    warn(__FILE__, " ego vehicle plugin bad parameter: veh_update_rate. Positive required");
    return false;
  }

  std::unordered_map<std::string, std::string> naming_map;
  if (!GetNamingMap(url, naming_map)) {
    warn(__FILE__, " Cannot create naming map for selected url: {0}", url);
    return false;
  }

  std::map<std::string, std::string> offset_url_list{};

  switch (offset_.offset_cfg) {
  case offset_config::Offset_by_URL:
    offset_url_list["offset_x"] = offset_x;
    offset_url_list["offset_y"] = offset_y;
    break;
  case offset_config::Offset_by_value:
    try {
      offset_.offset_x = std::stof(offset_x);
    } catch (const std::invalid_argument &) {
      warn(__FILE__, " Ego vehicle plugin bad parameter: offset_x. Float value required");
      removeConfig(config);
      return false;
    } catch (const std::out_of_range &) {
      warn(__FILE__, " Ego vehicle plugin bad parameter: offset_x. Value out of range");
      removeConfig(config);
      return false;
    }
    try {
      offset_.offset_y = std::stof(offset_y);
    } catch (const std::invalid_argument &) {
      warn(__FILE__, " Ego vehicle plugin bad parameter: offset_y. Float value required");
      removeConfig(config);
      return false;
    } catch (const std::out_of_range &) {
      warn(__FILE__, " Ego vehicle plugin bad parameter: offset_y. Value out of range");
      removeConfig(config);
      return false;
    }
    break;
  default:
    break;
  }

  info(__FILE__, "Starting up ego vehicle for {0}", url);

  auto topics = plugin_signal_explorer_->getTopicsByUrl(url + naming_map["ego_pos_x"]);
  if (topics.size() != 1 || topics[0].size() == 0) {
    warn(__FILE__, " ego vehicle could not find valid topic for URL:{0}", url);
    return false;
  }

  plugin_signal_extractor_->GetInfo(topics[0]);

  // register ego sensor
  bridgesdk::plugin::SensorInfoForGUI sensor_info_gui;
  sensor_info_gui.name = sensor_config_ego_position.sensor_name;
  sensor_info_gui.color = color;
  sensor_info_gui.stream = sensor_config_ego_position.data_view_name;
  sensor_info_gui.type = "line";
  sensor_info_gui.referenceCoordinateSystem = "vehicle";
  bridgesdk::plugin::CoordinateSystemTransform coordinate_system;
  sensor_info_gui.coordinateSystem.translationX = 0.0F;
  sensor_info_gui.coordinateSystem.translationY = 0.0F;
  sensor_info_gui.coordinateSystem.translationZ = 0.0F;
  sensor_info_gui.coordinateSystem.rotationX = 0.0F;
  sensor_info_gui.coordinateSystem.rotationY = 0.0F;
  sensor_info_gui.coordinateSystem.rotationZ = 0.0F;
  sensor_info_gui.flatbuffer_bfbs_path = path_to_bfbs_;
  sensor_info_gui.schemaName = "GuiSchema.EgoVehicleList";

  sensor_config_ego_position.data_cache_name = "ego-veh-position-";
  sensor_config_ego_position.sensor_id = plugin_3D_view_->registerAndPublishSensor(sensor_info_gui);

  // check sensor registration
  if (sensor_config_ego_position.sensor_id <= 0) {
    warn(__FILE__, "{0}::addConfig::registerAndPublishSensor to sensor name: {1} failed", getName(),
         sensor_info_gui.name);
    return false;
  }

  // register trajectory sensor
  next::plugins_utils::SensorConfig sensor_config_driver_trajectory{sensor_config_ego_position};
  sensor_config_driver_trajectory.sensor_name = sensor_tr;
  bridgesdk::plugin::SensorInfoForGUI sensor_info_gui_tr;
  sensor_info_gui_tr.name = sensor_tr;
  sensor_info_gui_tr.color = color;
  sensor_info_gui_tr.stream = sensor_config_driver_trajectory.data_view_name;
  sensor_info_gui_tr.type = "line";
  sensor_info_gui_tr.referenceCoordinateSystem = "vehicle";
  sensor_info_gui_tr.coordinateSystem.translationX = 0.0F;
  sensor_info_gui_tr.coordinateSystem.translationY = 0.0F;
  sensor_info_gui_tr.coordinateSystem.translationZ = 0.0F;
  sensor_info_gui_tr.coordinateSystem.rotationX = 0.0F;
  sensor_info_gui_tr.coordinateSystem.rotationY = 0.0F;
  sensor_info_gui_tr.coordinateSystem.rotationZ = 0.0F;
  sensor_info_gui_tr.flatbuffer_bfbs_path = path_to_bfbs_line_;
  sensor_info_gui_tr.schemaName = "GuiSchema.PrimitiveList";

  sensor_config_driver_trajectory.sensor_id = plugin_3D_view_->registerAndPublishSensor(sensor_info_gui_tr);

  // check sensor registration
  if (sensor_config_driver_trajectory.sensor_id <= 0) {
    warn(__FILE__, "{0}::addConfig::registerAndPublishSensor to sensor name: {1} failed", getName(),
         sensor_info_gui_tr.name);
    // unregister the previously successful registered ego sensor
    plugin_3D_view_->unregisterAndPublishSensor(sensor_config_ego_position.sensor_id);
    return false;
  }

  // store sensor for this config
  // Hint: Do it after the check of the sensor registration to be sure that both sensors are valid
  // and can be stored. The function removeConfig expects two valid sensors to be stored.
  auto hash_key = CreateKeyOutOfPluginConfig(config);
  // store ego sensor
  if (active_config_to_sensors_ego_.find(hash_key) != active_config_to_sensors_ego_.end()) {
    warn(__FILE__, "{0}::addConfig::Key already exists in active_config_to_sensors_ego_ and is overwritten", getName());
  }
  active_config_to_sensors_ego_[hash_key] = sensor_config_ego_position;
  veh_sensor_config_[sensor_config_ego_position.sensor_id] = EgoVehicleSensorConfig();

  // store trajectory sensor
  if (active_config_to_sensors_trajectory_.find(hash_key) != active_config_to_sensors_trajectory_.end()) {
    warn(__FILE__, "{0}::addConfig::Key already exists in active_config_to_sensors_trajectory_ and is overwritten",
         getName());
  }
  active_config_to_sensors_trajectory_[hash_key] = sensor_config_driver_trajectory;
  trajectory_info_.insert({sensor_config_driver_trajectory.sensor_id, {enable_traj == "DISABLE" ? false : true}});

  // create ego_vehicle_struct for getting the ego vehicle attributes from the topic
  auto ego_vehicle_struct = std::make_shared<EgoVehicle>(next::udex::extractor::emptyRequestBasic);
  ego_vehicle_struct->urls_.clear();
  ego_vehicle_struct->urls_ = naming_map;
  if (!ego_vehicle_struct->SetUrls(topics[0])) {
    warn(__FILE__, "{0}::addConfig::set urls for ego_vehicle_struct failed", getName());
    removeConfig(config);
    return false;
  }

  ego_vehicle_lists_.insert({hash_key, ego_vehicle_struct});

  // subscribe to ego topic
  auto binded_callback = std::bind(&EgoVehiclePlugin::ProcessEgoVehicleCallback, this, std::placeholders::_1,
                                   std::placeholders::_2, sensor_config_ego_position, sensor_config_driver_trajectory,
                                   ego_vehicle_struct, (uint64_t)std::stoi(update_rate));

  auto subs_id = plugin_data_subscriber_->subscribeUrl(topics[0], bridgesdk::SubscriptionType::DIRECT, binded_callback);

  if (subs_id <= 0) {
    warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), topics[0]);
    removeConfig(config);
    return false;
  }

  sensor_config_ego_position.subscription_id.push_back(subs_id);
  // update active_config_to_sensors_ego_ with subscribed urls for this sensor
  active_config_to_sensors_ego_[hash_key] = sensor_config_ego_position;
  debug(__FILE__, "{0}::addConfig::subscription to url: {1} is successful", getName(), topics[0]);

  if (offset_.offset_cfg == offset_config::Offset_by_URL) {
    const auto offset_topic{plugin_signal_explorer_->getTopicsByUrl(offset_url_list["offset_x"])};

    if (offset_topic.size() != 1 || offset_topic[0].size() == 0) {
      warn(__FILE__, " ego vehicle could not find valid topic for URL:{0}", url);
      removeConfig(config);
      return false;
    }

    // check if urls are valid
    bridgesdk::plugin_addons::SignalInfo sig_info_offset_x =
        plugin_signal_extractor_->GetInfo(offset_url_list["offset_x"]);
    if (0 == sig_info_offset_x.signal_size) {
      warn(__FILE__, " ego vehicle could not find offset_x URL:{0}", offset_url_list["offset_x"]);
      removeConfig(config);
      return false;
    }
    bridgesdk::plugin_addons::SignalInfo sig_info_offset_y =
        plugin_signal_extractor_->GetInfo(offset_url_list["offset_y"]);
    if (0 == sig_info_offset_y.signal_size) {
      warn(__FILE__, " ego vehicle could not find offset_y URL:{0}", offset_url_list["offset_y"]);
      removeConfig(config);
      return false;
    }

    auto binded_callback_offset = std::bind(&EgoVehiclePlugin::ProcessDataCallbackOffset, this, std::placeholders::_1,
                                            std::placeholders::_2, offset_url_list);

    auto subs_id_offset = plugin_data_subscriber_->subscribeUrl(offset_topic[0], bridgesdk::SubscriptionType::DIRECT,
                                                                binded_callback_offset);

    if (subs_id_offset <= 0) {
      warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), offset_topic[0]);
      removeConfig(config);
      return false;
    }

    sensor_config_driver_trajectory.subscription_id.push_back(subs_id_offset);
    // update active_config_to_sensors_trajectory_ with subscribed urls for this sensor
    active_config_to_sensors_trajectory_[hash_key] = sensor_config_driver_trajectory;
    debug(__FILE__, "{0}::addConfig::subscription to url: {1} is successful", getName(), offset_topic[0]);
  }

  return true;
}

bool EgoVehiclePlugin::enterReset() {
  for (auto &pair : veh_sensor_config_) {
    pair.second.ego_y_distance.store(0.0f);
    pair.second.ego_x_distance.store(0.0f);
    pair.second.ego_yaw_angle.store(0.0f);
    pair.second.cache_frame_counter_.store(0);
  }
  return true;
}

bool EgoVehiclePlugin::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool EgoVehiclePlugin::isNumber(const std::string &str) {
  for (char const &c : str) {
    if (isdigit(c) == 0)
      return false;
  }
  return true;
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::EgoVehiclePlugin(path); }
