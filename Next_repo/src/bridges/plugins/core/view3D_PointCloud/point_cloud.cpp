/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     point_cloud.cpp
 * @brief    Parsing network data of AEye LiDAR sensor, providing point clouds
 *
 * See point_cloud_creator.h.
 *
 **/

#include <any>
#include <boost/regex.hpp>
#include <map>
#include <regex>
#include <unordered_set>

#include <boost/algorithm/string.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <json/json.h>

#include <next/bridgesdk/schema/3d_view/pointcloud_generated.h>

#include <next/bridgesdk/datatypes.h>
#include <next/bridgesdk/plugin_addons/plugin_helper_data_extractor.hpp>

#include <next/plugins_utils/helper_functions.h>

#include <next/sdk/version_manager/version_manager.hpp>
#include "next/appsupport/config/config_client.hpp"
#include "next/appsupport/config/config_key_generator.hpp"

#include "point_cloud.h"
#include "url_mapper.h"

namespace next {
namespace plugins {

using namespace boost::numeric;
using namespace next::plugins_utils;

constexpr const float kMaxHeight{15.f};
constexpr const float kMinHeight{-10.f};

PointCloud::PointCloud(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello view3D_PointCloud plugin");
  this->setVersion(std::string("1.0.0"));
  this->setName(std::string("view3D_PointCloud"));
  fbs_path_ = path + std::string("/pointcloud.fbs");
  bfbs_path_ = path + std::string("/pointcloud.bfbs");

  InitSearchRequests();
  SetRegexValuesInConfig();
}

PointCloud::~PointCloud() {}

bool PointCloud::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  config_fields.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "Parking Point Cloud"}});

  auto results = FindResultsByUrl("");
  if (!results.empty()) {
    for (const auto &result : results) {
      config_fields.insert({"url_point", {next::bridgesdk::ConfigType::SELECTABLE, result.root_url}});
    }
  }

  config_fields.insert(
      {"url_veh", {next::bridgesdk::ConfigType::SELECTABLE, "MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort"}});
  config_fields.insert(
      {"url_veh", {next::bridgesdk::ConfigType::SELECTABLE, "ADC5xx_Device.VD_DATA.IuOdoEstimationPort"}});
  config_fields.insert({"url_veh", {next::bridgesdk::ConfigType::SELECTABLE, "M7board.CAN_Thread.OdoEstimationPort"}});

  for (const auto coordinate_system_map_entry : coordinate_system_to_enum_) {
    config_fields.insert(
        {"coordinate_system", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, coordinate_system_map_entry.first}});
  }

  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "height"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#FFD67F"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#006666"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#00CC00"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#B266FF"}});

  const std::string wheelbase_name{"url_wheelbase (if coordinate_system \"front-axle\" is selected)"};
  config_fields.insert({wheelbase_name, {next::bridgesdk::ConfigType::SELECTABLE, ""}});
  config_fields.insert(
      {wheelbase_name,
       {next::bridgesdk::ConfigType::SELECTABLE, "M7board.ConfigurationTrace.MF_VEHICLE_Parameter.AP_V_WHEELBASE_M"}});
  config_fields.insert(
      {wheelbase_name,
       {next::bridgesdk::ConfigType::SELECTABLE, "ADC5xx_Device.CFG_DATA.MF_VEHICLE_Parameter.AP_V_WHEELBASE_M"}});

  return true;
}

bool FindAndReplace(std::string &str, const std::string &to_find, const std::string &replace_with) {
  std::size_t found = str.find(to_find);
  if (found != std::string::npos) {
    str.replace(found, to_find.length(), replace_with);
    return true;
  }
  return false;
}

void PointCloud::ProcessDataCallbackEgoVehicle([[maybe_unused]] const std::string topic_name,
                                               const next::bridgesdk::ChannelMessagePointers *datablock,
                                               [[maybe_unused]] next::bridgesdk::plugin_config_t config,
                                               [[maybe_unused]] next::plugins_utils::SensorConfig sensor_config,
                                               std::shared_ptr<EgoVehicle> ego_vehicle) {
  std::lock_guard<std::mutex> lock(config_ego_vehicle_protector_);

  ego_vehicle->SetAddress((char *)datablock->data, datablock->size);

  ego_y_distance_.store(ego_vehicle->ego_y_->Get());
  ego_x_distance_.store(ego_vehicle->ego_x_->Get());
  ego_yaw_angle_.store(ego_vehicle->ego_yaw_->Get());
}

void PointCloud::ProcessWheelbaseCallback([[maybe_unused]] const std::string topic_name,
                                          [[maybe_unused]] const next::bridgesdk::ChannelMessagePointers *datablock,
                                          const std::string &url_wheelbase) {
  next::bridgesdk::plugin_addons::dataType wheelbase{0.0f};
  if (!plugin_signal_extractor_->GetValue(url_wheelbase, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT,
                                          wheelbase)) {
    warn(__FILE__, " Point Cloud: Could not extract value for: {0}", url_wheelbase);
    return;
  }
  ego_wheelbase_.store(std::get<float>(wheelbase));
}

template <class T>
bool IsFloatValid(const T value) {
  return std::isfinite(value) && !std::isnan(value);
}

inline ublas::matrix<float> GetTransformMatrix(float yaw, float x_pos, float y_pos) {
  ublas::matrix<float> trans_rotate_matrix(4, 4);

  trans_rotate_matrix(0, 0) = cosf(yaw);
  trans_rotate_matrix(0, 1) = -sinf(yaw);
  trans_rotate_matrix(0, 2) = 0.F;
  trans_rotate_matrix(0, 3) = x_pos;

  trans_rotate_matrix(1, 0) = sinf(yaw);
  trans_rotate_matrix(1, 1) = cosf(yaw);
  trans_rotate_matrix(1, 2) = 0.F;
  trans_rotate_matrix(1, 3) = y_pos;

  trans_rotate_matrix(2, 0) = 0.F;
  trans_rotate_matrix(2, 1) = 0.F;
  trans_rotate_matrix(2, 2) = 1.F;
  trans_rotate_matrix(2, 3) = 0.F;

  trans_rotate_matrix(3, 0) = 0.F;
  trans_rotate_matrix(3, 1) = 0.F;
  trans_rotate_matrix(3, 2) = 0.F;
  trans_rotate_matrix(3, 3) = 1.F;
  return trans_rotate_matrix;
}

// make to vector3 structs instead of floats
inline void transformPoint(ublas::matrix<float> transform_matrix, float pos_x_in, float pos_y_in, float pos_z_in,
                           float &pos_x_out, float &pos_y_out, float &pos_z_out) {
  ublas::matrix<float> point_value(4, 1);

  point_value(0, 0) = pos_x_in;
  point_value(1, 0) = pos_y_in;
  point_value(2, 0) = pos_z_in;
  point_value(3, 0) = 1.F;

  auto result = ublas::prod(transform_matrix, point_value);

  pos_x_out = result(0, 0);
  pos_y_out = result(1, 0);
  pos_z_out = pos_z_in;
}

// make to vector3 structs instead of floats
inline void setColorByHeight(float pos_z_in, float &red, float &green, float &blue) {
  if (fabs(pos_z_in) < 0.1f) {
    red = (pos_z_in - kMinHeight) / kMaxHeight;
    red = (red > 1.F) ? 1.F : red;
    green = 0;
    blue = 0;
  } else if ((pos_z_in - 1.0f) < 0.1f) {
    green = (pos_z_in - kMinHeight) / kMaxHeight;
    red = 0;
    green = (green > 1.F) ? 1.F : green;
    blue = 0;
  } else if ((pos_z_in - 2.0f) < 0.1f) {
    red = 0;
    green = 0;
    blue = (pos_z_in - kMinHeight) / kMaxHeight;
    blue = (blue > 1.F) ? 1.F : blue;
  }
}

inline bool checkValidColorInput(const std::string &color_string) {
  const int expected_color_string_size = 6;
  if (color_string.size() != expected_color_string_size) {
    return false;
  }
  // regex matches Hex values 0-9 and A-F (non case senisitive)
  if (!std::regex_match(color_string, std::regex("[0-9a-fA-F]+"))) {
    return false;
  }
  return true;
}

inline void setColorByValue(const std::string &point_color, float &red, float &green, float &blue) {
  // Remove leading # of color string
  float divider_for_colors = 255.F;
  std::string clean_color_string;
  if (point_color.at(0) == '#') {
    clean_color_string = point_color.substr(1, 6);
  } else {
    clean_color_string = point_color;
  }

  if (!checkValidColorInput(clean_color_string)) {
    warn(__FILE__, "Unsupported colour provided. Please use a valid Hexadezimal Value!");
    warn(__FILE__, "Setting PointCloud Color to default value: (#FFFFFF)");
    clean_color_string = "FFFFFF";
  }

  // Transform String input into Int Values
  int tmp_red = 0, tmp_green = 0, tmp_blue = 0;
  tmp_red = std::stoi(clean_color_string.substr(0, 2), nullptr, 16);
  tmp_green = std::stoi(clean_color_string.substr(2, 2), nullptr, 16);
  tmp_blue = std::stoi(clean_color_string.substr(4, 2), nullptr, 16);

  // Output must be between 1 and 0. Dividing by 255 so that FF would be 1
  red = (float)tmp_red / divider_for_colors;
  green = (float)tmp_green / divider_for_colors;
  blue = (float)tmp_blue / divider_for_colors;
}

inline void setColor(const std::string &point_colour, const float &pos_z, float &red, float &green, float &blue) {
  if (point_colour == "height") {
    setColorByHeight(pos_z, red, green, blue);
  } else {
    setColorByValue(point_colour, red, green, blue);
  }
}

void PointCloud::ProcessPointCloudCallback([[maybe_unused]] const std::string topic_name,
                                           const next::bridgesdk::ChannelMessagePointers *datablock,
                                           [[maybe_unused]] next::bridgesdk::plugin_config_t config,
                                           next::plugins_utils::SensorConfig sensor_config,
                                           std::shared_ptr<Cloud> point_cloud_struct, const std::string point_colour,
                                           const CoordinateSystem coordinate_system, bool spherical_coordinate_system) {
  std::lock_guard<std::mutex> lock(config_pc_protector_);
  point_cloud_struct->SetAddress((char *)datablock->data, datablock->size);
  ublas::matrix<float> point_value(4, 1);

  // TODO: this is confusing(number_of_points, num_points)
  uint32_t number_of_points;
  if (spherical_coordinate_system) {
    number_of_points = static_cast<uint32_t>(point_cloud_struct->GetNumberOfPointsFromCloudPointAsArray());
  } else {
    number_of_points = point_cloud_struct->number_of_points_->Get();
  }

  size_t num_points = 0;
  if (spherical_coordinate_system) {
    num_points = point_cloud_struct->GetNumberOfPointsFromCloudPointAsArray();
  } else {
    num_points = point_cloud_struct->points_.size();
  }

  if (number_of_points > num_points) {
    warn(__FILE__, " Point Cloud number_of_points ({:d}) exceeds point array size ({:d})", number_of_points,
         num_points);
  }

  std::vector<float> points{};
  std::vector<float> points_colour{};
  for (size_t ind{0U}; ind < std::min(static_cast<size_t>(number_of_points), num_points); ++ind) {

    float pos_x{point_cloud_struct->GetPosX(ind)};
    float pos_y{point_cloud_struct->GetPosY(ind)};
    float pos_z{point_cloud_struct->GetPosZ(ind)};

    // if point is not valid skip it
    if (!IsFloatValid(pos_x) || !IsFloatValid(pos_y) || !IsFloatValid(pos_z)) {
      continue;
    }

    // SFM uses a coordinate system that is rotated by 180 degrees compared to the ISO coordinate system and uses the
    // front axle as origin see https://confluence-adas.zone2.agileci.conti.de/display/CEM200/CEM_LSM+Coordinate+Systems
    if (coordinate_system == CoordinateSystem::kEgoVehicleFrontAxle) {
      pos_x = -(pos_x - ego_wheelbase_.load());
      pos_y = -pos_y;
    }

    if (coordinate_system == CoordinateSystem::kEgoVehicleRearAxle ||
        coordinate_system == CoordinateSystem::kEgoVehicleFrontAxle) {
      auto ego_trans_matrix = GetTransformMatrix(ego_yaw_angle_.load(), ego_x_distance_.load(), ego_y_distance_.load());
      transformPoint(ego_trans_matrix, pos_x, pos_y, pos_z, pos_x, pos_y, pos_z);
    }

    float red = 0.f, green = 0.f, blue = 0.f;
    setColor(point_colour, pos_z, red, green, blue);

    points_colour.push_back(red);
    points_colour.push_back(green);
    points_colour.push_back(blue);

    // NEXT GUI needs the points in the format (x, z -y)
    points.push_back(pos_x);
    points.push_back(pos_z);
    points.push_back(-pos_y);
  }

  if (points.empty()) {
    return;
  }

  // TODO to keep point available on gui for each frame. Should be removed maybe.
  static uint64_t cache_counter = 0;
  std::string cache_name{sensor_config.data_cache_name + std::to_string(cache_counter++)};
  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  std::string colour{"#FF6600"};
  auto point_cloud = GuiSchema::CreatePointCloudDirect(builder, &points, &points_colour, colour.c_str());

  std::vector<flatbuffers::Offset<GuiSchema::PointCloud>> point_cloud_list;
  point_cloud_list.push_back(point_cloud);

  auto fbs_pointcloud_list = GuiSchema::CreatePointCloudListDirect(builder, &point_cloud_list);

  builder.Finish(point_cloud);
  builder.Finish(fbs_pointcloud_list);

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_binary = builder.GetBufferPointer();
  flatbuffer.fbs_size = builder.GetSize();

  bridgesdk::plugin::SensorInfoForFbs fbs_config;
  fbs_config.data_view = sensor_config.data_view_name;
  fbs_config.cache_name = cache_name;
  fbs_config.sensor_id = sensor_config.sensor_id;
  fbs_config.timestamp_microsecond = datablock->time;

  plugin_publisher_->sendFlatbufferData(fbs_path_, flatbuffer, fbs_config, {"PointCloud", "0.0.1"});

  builder.Clear();
}

bool PointCloud::init() {

  debug(__FILE__, "init {0}", __FILE__);
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  plugin_regex_searcher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginRegexSearcher>();

  InitSearchRequests();
  return true;
}

bool PointCloud::removeConfig(const next::bridgesdk::plugin_config_t &config) {
  debug(__FILE__, "{0}::removeConfig was called", getName());

  auto master_key = CreateKeyOutOfPluginConfig(config);

  auto it_active_config_to_sensors = active_config_to_sensors_.find(master_key);

  if (it_active_config_to_sensors == active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}::removeConfig::config to remove was not found. removeConfig failed", getName());
    return false;
  }

  auto &config_data = it_active_config_to_sensors->second;

  plugin_3D_view_->unregisterAndPublishSensor(config_data.sensor_config_.sensor_id);
  for (auto id : config_data.sensor_config_.subscription_id) {
    plugin_data_subscriber_->unsubscribe(id);
  }

  if (nullptr != config_data.point_cloud_) {
    std::lock_guard<std::mutex> lock(config_pc_protector_);
    config_data.point_cloud_.reset();
  }

  if (nullptr != config_data.ego_vehicle_) {
    std::lock_guard<std::mutex> lock(config_ego_vehicle_protector_);
    config_data.ego_vehicle_.reset();
  }

  active_config_to_sensors_.erase(master_key);

  return true;
}

bool PointCloud::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool PointCloud::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {
  config_ = config;
  std::map<std::string, std::string> ego_naming_map_;

  next::plugins_utils::SensorConfig sensor_config;
  if (!GetValueFromPluginConfig(config, "DATA_VIEW", sensor_config.data_view_name)) {
    warn(__FILE__, "Point Cloud missing config value: DATA_VIEW");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "sensor name", sensor_config.sensor_name)) {
    warn(__FILE__, "Point Cloud missing config value: sensor name");
    return false;
  }

  std::string url_point;
  std::string url_veh;
  if (GetValueFromPluginConfig(config, "url_point", url_point) != true) {
    warn(__FILE__, "Point Cloud missing config value: url_point");
    return false;
  }
  if (GetValueFromPluginConfig(config, "url_veh", url_veh) != true) {
    warn(__FILE__, "Point Cloud missing config value: url_veh");
    return false;
  }

  std::string point_colour;
  if (GetValueFromPluginConfig(config, "point_colour", point_colour) != true) {
    warn(__FILE__, "Point Cloud missing config value: point_colour");
    return false;
  }

  std::string coordinate_type{};
  if (GetValueFromPluginConfig(config, "coordinate_system", coordinate_type) != true) {
    warn(__FILE__, "Point Cloud missing config value: point_colour");
    return false;
  }
  const CoordinateSystem coordinate_system{coordinate_system_to_enum_.at(coordinate_type)};

  info(__FILE__, "Starting up point cloud plugin for {0}", url_point);
  auto topic_points = plugin_signal_explorer_->getTopicsByUrl(url_point);
  if (topic_points.size() != 1 || topic_points[0].size() == 0) {
    warn(__FILE__, "point cloud could not find valid topic for URL: {0}", url_point);
    return false;
  }

  bridgesdk::plugin::SensorInfoForGUI sensor_info_gui;
  sensor_info_gui.name = sensor_config.sensor_name;
  sensor_info_gui.color = point_colour;
  sensor_info_gui.stream = sensor_config.data_view_name;
  sensor_info_gui.type = "pc";
  sensor_info_gui.referenceCoordinateSystem = "vehicle";
  sensor_info_gui.coordinateSystem.translationX = 0.0F;
  sensor_info_gui.coordinateSystem.translationY = 0.0F;
  sensor_info_gui.coordinateSystem.translationZ = 0.0F;
  sensor_info_gui.coordinateSystem.rotationX = 0.0F;
  sensor_info_gui.coordinateSystem.rotationY = 0.0F;
  sensor_info_gui.coordinateSystem.rotationZ = 0.0F;
  sensor_info_gui.flatbuffer_bfbs_path = bfbs_path_;
  sensor_info_gui.schemaName = "GuiSchema.PointCloudList";

  sensor_config.sensor_id = plugin_3D_view_->registerAndPublishSensor(sensor_info_gui);
  if (sensor_config.sensor_id <= 0) {
    warn(__FILE__, "{0}::addConfig::registerAndPublishSensor to sensor name: {1} failed", getName(),
         sensor_info_gui.name);
    return false;
  }

  sensor_config.data_cache_name = "point-cloud-" + std::to_string(sensor_config.sensor_id) + "-";

  // store sensor for this config
  auto master_key = CreateKeyOutOfPluginConfig(config);
  if (active_config_to_sensors_.find(master_key) != active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}::addConfig::Key already exists and is overwritten", getName());
  }

  active_config_to_sensors_[master_key].sensor_config_ = sensor_config;

  if (coordinate_system == CoordinateSystem::kEgoVehicleRearAxle ||
      coordinate_system == CoordinateSystem::kEgoVehicleFrontAxle) {
    auto ego_topic = plugin_signal_explorer_->getTopicsByUrl(url_veh);
    if (ego_topic.empty() || ego_topic[0].size() == 0) {
      warn(__FILE__, "point cloud (Ego vehicle topic) could not find valid topic for URL: {0}", url_veh);
      removeConfig(config);
      return false;
    }

    auto ego_vehicle_struct = std::make_shared<EgoVehicle>(next::udex::extractor::emptyRequestBasic);
    active_config_to_sensors_[master_key].ego_vehicle_ = ego_vehicle_struct;

    ego_vehicle_struct->urls_.clear();
    ego_vehicle_struct->urls_ = parking_odo_estimation;
    if (!ego_vehicle_struct->SetUrls(url_veh)) {
      warn(__FILE__, "{0}::addConfig::SetUrls of struct extractor failed.", getName());
      removeConfig(config);
      return false;
    }

    auto bound_callback_ego = std::bind(&PointCloud::ProcessDataCallbackEgoVehicle, this, std::placeholders::_1,
                                        std::placeholders::_2, config, sensor_config, ego_vehicle_struct);

    auto subs_id =
        plugin_data_subscriber_->subscribeUrl(ego_topic[0], bridgesdk::SubscriptionType::DIRECT, bound_callback_ego);

    if (subs_id <= 0) {
      warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), ego_topic[0]);
      removeConfig(config);
      return false;
    }

    sensor_config.subscription_id.push_back(subs_id);
    active_config_to_sensors_[master_key].sensor_config_ = sensor_config;
  }

  // For front-axle coordinate system, extract the ego-vehicle wheelbase from the recording
  if (coordinate_system == CoordinateSystem::kEgoVehicleFrontAxle) {
    std::string url_wheelbase;
    if (!GetValueFromPluginConfig(config, "url_wheelbase (if coordinate_system \"front-axle\" is selected)",
                                  url_wheelbase) ||
        url_wheelbase.empty()) {
      warn(__FILE__, "Point Cloud missing config value: url_wheelbase");
      removeConfig(config);
      return false;
    }
    const auto wheelbase_topic = plugin_signal_explorer_->getTopicsByUrl(url_wheelbase);
    if (wheelbase_topic.empty() || wheelbase_topic[0].empty()) {
      warn(__FILE__, "Point Cloud (wheelbase topic) could not find valid topic for URL: {0}", url_wheelbase);
      removeConfig(config);
      return false;
    }
    // check if wheelbase url is valid
    bridgesdk::plugin_addons::SignalInfo sig_info_wheelbase = plugin_signal_extractor_->GetInfo(url_wheelbase);
    if (0 == sig_info_wheelbase.signal_size) {
      warn(__FILE__, "Point Cloud could not find url_wheelbase: {0}", url_wheelbase);
      removeConfig(config);
      return false;
    }

    auto binded_callback_wheelbase = std::bind(&PointCloud::ProcessWheelbaseCallback, this, std::placeholders::_1,
                                               std::placeholders::_2, url_wheelbase);

    auto subs_id_wheelbase = plugin_data_subscriber_->subscribeUrl(
        wheelbase_topic[0], bridgesdk::SubscriptionType::DIRECT, binded_callback_wheelbase);

    if (subs_id_wheelbase <= 0) {
      warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), wheelbase_topic[0]);
      removeConfig(config);
      return false;
    }
    sensor_config.subscription_id.push_back(subs_id_wheelbase);
    active_config_to_sensors_[master_key].sensor_config_ = sensor_config;
  }

  std::map<std::string, std::string> naming_map;
  bool spherical_coordinate_system = false;

  if (!GetNamingMap(url_point, naming_map, spherical_coordinate_system)) {
    warn(__FILE__, "{0}::addConfig::failed to get naming_map for url: {1}", getName(), url_point);
    return false;
  }

  size_t max_num_points{0};
  if (!spherical_coordinate_system) {
    if (!GetArraySizeStruct(url_point + naming_map["point_"], max_num_points)) {
      removeConfig(config);
      warn(__FILE__, "{0}::addConfig::Unable to find url: {1}", getName(), url_point + naming_map["point_"]);
      return false;
    }
  }

  auto point_cloud_struct = std::make_shared<Cloud>(next::udex::extractor::emptyRequestBasic);
  active_config_to_sensors_[master_key].point_cloud_ = point_cloud_struct;

  point_cloud_struct->SetNumberOfPoints(max_num_points);

  if (spherical_coordinate_system) {
    point_cloud_struct->array_of_struct_ = false;
  } else {
    point_cloud_struct->array_of_struct_ = true;
  }
  point_cloud_struct->urls_.clear();
  point_cloud_struct->urls_ = naming_map;
  if (!point_cloud_struct->SetUrls(url_point)) {
    warn(__FILE__, "{0}::addConfig::setting up urls for point cloud extractor failed", getName());
    removeConfig(config);
    return false;
  }

  auto bound_callback =
      std::bind(&PointCloud::ProcessPointCloudCallback, this, std::placeholders::_1, std::placeholders::_2, config,
                sensor_config, point_cloud_struct, point_colour, coordinate_system, spherical_coordinate_system);

  auto subs_id_points =
      plugin_data_subscriber_->subscribeUrl(topic_points[0], bridgesdk::SubscriptionType::DIRECT, bound_callback);

  if (subs_id_points <= 0) {
    warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), topic_points[0]);
    removeConfig(config);
    return false;
  }

  sensor_config.subscription_id.push_back(subs_id_points);
  active_config_to_sensors_[master_key].sensor_config_ = sensor_config;

  debug(__FILE__, "{0}::addConfig::subscription to is successful", getName());
  return true;
}

bool PointCloud::enterReset() {
  ego_y_distance_.store(0.0F);
  ego_x_distance_.store(0.0F);
  ego_yaw_angle_.store(0.0F);
  ego_wheelbase_.store(0.0F);

  return true;
}

bool PointCloud::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool PointCloud::GetArraySizeStruct(const std::string &url, size_t &array_size) const {
  bridgesdk::plugin_addons::SignalInfo sig_array_size = plugin_signal_extractor_->GetInfo(url);
  if (0 == sig_array_size.signal_size) {
    warn(__FILE__, "PointCloud could not find traffic objects URL:{0}", url);
    return false;
  }
  array_size = sig_array_size.array_size;
  return true;
}

bool PointCloud::IsSphericalCoordinateSystem(const PointCloud::SearchResult &result) {
  bool spherical_fields_available = true;

  auto isResultUrlEmpty = [&result](const std::string key) {
    auto it = result.data_type_to_result.find(key);

    if (it != result.data_type_to_result.end()) {
      return it->second.result_url.empty();
    }

    return true;
  };

  spherical_fields_available &= !isResultUrlEmpty("theta_");
  spherical_fields_available &= !isResultUrlEmpty("phi_");
  spherical_fields_available &= !isResultUrlEmpty("radius_");
  spherical_fields_available &= !isResultUrlEmpty("arrays");

  return spherical_fields_available;
}

bool PointCloud::GetNamingMap(const std::string &url, std::map<std::string, std::string> &naming_map,
                              bool &spherical_coordinate_system) {
  if (url.empty()) {
    return false;
  }
  auto result = FindResultsByUrl(url);
  if (result.size() != 1) {
    return false; // must be only a search result for selected url
  }

  if (IsSphericalCoordinateSystem(result[0])) {
    spherical_coordinate_system = true;
    naming_map["radius_"] = result[0].data_type_to_result["radius_"].result_url;
    naming_map["phi_"] = result[0].data_type_to_result["phi_"].result_url;
    naming_map["theta_"] = result[0].data_type_to_result["theta_"].result_url;
    naming_map["arrays"] = result[0].data_type_to_result["arrays"].result_url;
  } else {
    // cartesian coordinate system
    naming_map["point_"] = result[0].data_type_to_result["point_"].result_url;
    naming_map["num_points"] = result[0].data_type_to_result["num_points"].result_url;
    naming_map["xpos"] = result[0].data_type_to_result["xpos"].result_url;
    naming_map["ypos"] = result[0].data_type_to_result["ypos"].result_url;
    naming_map["zpos"] = result[0].data_type_to_result["zpos"].result_url;
  }

  return true;
}

void PointCloud::InitSearchRequests() {
  search_requests_.clear();

  bool optional = true;
  keywords_ = {"Output", "ClusterListArrays", "Point"};

  std::vector<std::tuple<std::string, std::vector<std::string>, bool>> requests = {
      {"point_", {R"(((\.ussPointList)?\.((uss)?points?(List|Array|_as)?))\[)"}, optional},
      {"xpos", {R"((\.[x]?position.*(?:_m|x|_m_f32)$))", R"(\.(x_m$))", R"(\.(positionX)$)"}, optional},
      {"ypos", {R"((\.[y]?position.*(?:_m|y|_m_f32)$))", R"(\.(y_m$))", R"(\.(positionY)$)"}, optional},
      {"zpos", {R"((\.[z]?position.*(?:_m|z|_m_f32)$))", R"(\.(z_m$))", R"(\.(positionZ)$)"}, optional},
      {"radius_", {R"((\.a_RangeRad)\[\d\]$)"}, optional},
      {"phi_", {R"((\.a_azang)\[\d\]$)"}, optional},
      {"theta_", {R"((\.a_elevang)\[\d\]$)"}, optional},
      {"num_points", {R"((\.(ussPointList\.)?num.*point.*)$)"}, optional},
      {"arrays", {R"((\.clusterlistarrays)\.)", R"(\.(ussPointList))"}, optional}

  };
  for (const auto &[key, patterns, isOptional] : requests) {
    search_requests_.emplace_back(SearchRequest{key, patterns, isOptional});
  }

  if (!plugin_regex_searcher_) {
    return;
  }

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

void PointCloud::SetRegexValuesInConfig() {
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

  if (!plugin_regex_searcher_) {
    return;
  }
  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

std::vector<PointCloud::SearchResult> PointCloud::FindResultsByUrl(const std::string &url) {
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
  SanitizeCoordinateResults(result);

  return result;
}

void PointCloud::SanitizeCoordinateResults(std::vector<PointCloud::SearchResult> &results) {
  if (results.empty()) {
    return;
  }

  for (auto it = results.begin(); it != results.end();) {
    bool cartesian_pos_available = true;
    bool spherical_pos_available = true;

    cartesian_pos_available &= !it->data_type_to_result["xpos"].result_url.empty();
    cartesian_pos_available &= !it->data_type_to_result["ypos"].result_url.empty();
    cartesian_pos_available &= !it->data_type_to_result["num_points"].result_url.empty();
    cartesian_pos_available &= !it->data_type_to_result["point_"].result_url.empty();

    spherical_pos_available &= !it->data_type_to_result["theta_"].result_url.empty();
    spherical_pos_available &= !it->data_type_to_result["phi_"].result_url.empty();
    spherical_pos_available &= !it->data_type_to_result["radius_"].result_url.empty();
    spherical_pos_available &= !it->data_type_to_result["arrays"].result_url.empty();

    if (!cartesian_pos_available && !spherical_pos_available) {
      it = results.erase(it);

      continue;
    }
    it++;
  }
}

std::vector<PointCloud::SearchResult>
PointCloud::GetSearchResults(const std::string &url, const std::unordered_set<std::string> &unique_urls) const {
  if (!plugin_regex_searcher_) {
    return {};
  }

  if (url.empty())
    return plugin_regex_searcher_->GetSearchResults(unique_urls);
  else
    return {plugin_regex_searcher_->GetSearchResultForUrl(url, unique_urls)};
}

void PointCloud::UpdateRegexValuesFromConfig() {
  auto config_client = next::appsupport::ConfigClient::getConfig();
  if (!config_client) {
    warn(__FILE__, "{0}::UpdateRegexValuesFromConfig::couldn't access configuration to read regex values", getName());
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

  if (!plugin_regex_searcher_) {
    return;
  }

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) {
  debug(__FILE__, "Point Cloud creator called");
  return new next::plugins::PointCloud(path);
}
