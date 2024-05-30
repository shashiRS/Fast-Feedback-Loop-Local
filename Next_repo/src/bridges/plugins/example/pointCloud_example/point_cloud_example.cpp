/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     point_cloud_example.cpp
 * @brief    creating a point cloud with parameters and sending the data via flatbuffer
 *
 * See point_cloud_example.h.
 *
 **/

#include <any>
#include <chrono>
#include <map>
#include <regex>
#include <thread>

#include <next/bridgesdk/datatypes.h>
#include <next/bridgesdk/plugin_addons/plugin_helper_data_extractor.hpp>

#include <next/plugins_utils/helper_functions.h>

#include "point_cloud_example.h"

namespace next {
namespace plugins {

using namespace next::plugins_utils;

constexpr const float kMaxHeight{15.f};
constexpr const float kMinHeight{-10.f};

std::atomic<bool> PointCloud::s_disconnect{false};

PointCloud::PointCloud(const char *path) : next::bridgesdk::plugin::Plugin(path), path_to_fbs_{path} {
  debug(__FILE__, "Hello view3D_PointCloud plugin");
  this->setVersion(std::string("1.0.0"));
  this->setName(std::string("view3D_PointCloud"));
  path_to_fbs_ += "/pointcloud.fbs";
}

PointCloud::~PointCloud() {}

bool PointCloud::getDescription(next::bridgesdk::plugin_config_t &config_fields) const {
  config_fields.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "Point Cloud Example"}});

  config_fields.insert({"size", {next::bridgesdk::ConfigType::EDITABLE, "10"}});
  config_fields.insert({"point_density", {next::bridgesdk::ConfigType::EDITABLE, "1000"}});

  config_fields.insert({"number_of_messages", {next::bridgesdk::ConfigType::EDITABLE, "50"}});
  config_fields.insert({"delay_in_ms", {next::bridgesdk::ConfigType::EDITABLE, "0"}});

  config_fields.insert({"point_direction", {next::bridgesdk::ConfigType::SELECTABLE, "+x, +y"}});
  config_fields.insert({"point_direction", {next::bridgesdk::ConfigType::SELECTABLE, "+x, -y"}});
  config_fields.insert({"point_direction", {next::bridgesdk::ConfigType::SELECTABLE, "-x, +y"}});
  config_fields.insert({"point_direction", {next::bridgesdk::ConfigType::SELECTABLE, "-x, -y"}});

  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#FFD67F"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#006666"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#00CC00"}});
  config_fields.insert({"point_colour", {next::bridgesdk::ConfigType::SELECTABLE, "#B266FF"}});

  return true;
}

template <class T>
bool IsFloatValid(const T value) {
  return std::isfinite(value) && !std::isnan(value);
}

void PointCloud::generateExamplePointCloud(uint16_t size, uint16_t point_density, time_t time,
                                           next::plugins_utils::SensorConfig sensor_config,
                                           std::string point_direction) {
  std::vector<float> points{};
  std::vector<float> points_colour{};
  // What is this used for?
  std::string colour{"#FF6600"};

  uint16_t dimension = (point_density / size) + 1;
  std::vector<std::vector<float>> examplePointCloud(dimension, std::vector<float>(dimension, 0.0));
  for (int x = 0; x < dimension; x++) {
    for (int y = 0; y < dimension; y++) {
      examplePointCloud[x][y] = (float)y * ((size * size) / (float)point_density);
    }
  }
  for (int y = 0; y < (point_density / size) + 1; y++) {
    for (int x = 0; x < (point_density / size) + 1; x++) {

      float red = 0.f, green = 1.f, blue = 0.f;

      points_colour.push_back(red);
      points_colour.push_back(green);
      points_colour.push_back(blue);

      // NEXT GUI needs the points in the format (x, z -y)
      if (point_direction.compare("+x, +y") == 0) {
        points.push_back(examplePointCloud[y][x]);
        points.push_back(0.0);
        points.push_back(-examplePointCloud[x][y]);
      } else if (point_direction.compare("+x, -y") == 0) {
        points.push_back(examplePointCloud[y][x]);
        points.push_back(0.0);
        points.push_back(examplePointCloud[x][y]);
      } else if (point_direction.compare("-x, +y") == 0) {
        points.push_back(-examplePointCloud[y][x]);
        points.push_back(0.0);
        points.push_back(-examplePointCloud[x][y]);
      } else if (point_direction.compare("-x, -y") == 0) {
        points.push_back(-examplePointCloud[y][x]);
        points.push_back(0.0);
        points.push_back(examplePointCloud[x][y]);
      }
    }
  }

  if (points.empty()) {
    return;
  }

  // TODO to keep point available on gui for each frame. Should be removed maybe.
  std::string cache_name{sensor_config.data_cache_name};
  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

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
  fbs_config.timestamp_microsecond = time;

  plugin_publisher_->sendFlatbufferData(path_to_fbs_, flatbuffer, fbs_config, {"PointCloud", "0.0.1"});

  builder.Clear();
}

void PointCloud::runExamplePointCloud(uint16_t size, uint16_t point_density, time_t time,
                                      next::plugins_utils::SensorConfig sensor_config, uint16_t number_of_messages,
                                      uint16_t delay_in_ms, std::string point_direction) {

  for (int i = 0; i < number_of_messages; i++) {
    generateExamplePointCloud(size, point_density, time, sensor_config, point_direction);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_in_ms));
  }
}

void PointCloud::enableBackgroundRunning() { this->background_running_ = true; }

void PointCloud::disableBackgroundRunning() { this->background_running_ = false; }

void PointCloud::stopExamplePointCloud() { s_disconnect.store(true, std::memory_order_relaxed); }

bool PointCloud::init() {

  debug(__FILE__, "PointCloud init");
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);

  return true;
}

bool PointCloud::removeConfig(const next::bridgesdk::plugin_config_t &config) {
  stopExamplePointCloud();

  disableBackgroundRunning();
  if (background_thread_) {
    if (background_thread_->joinable()) {
      background_thread_->join();
    }
    background_thread_.reset();
  }

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

  return true;
}

bool PointCloud::addConfig(const next::bridgesdk::plugin_config_t &config) {
  uint16_t point_density, number_of_messages, size, delay_in_ms;
  std::string config_size, config_point_density, point_direction, config_number_of_messages, config_delay_in_ms;

  next::plugins_utils::SensorConfig sensor_config;

  if (!GetValueFromPluginConfig(config, "DATA_VIEW", sensor_config.data_view_name)) {
    warn(__FILE__, "Point Cloud missing config value: DATA_VIEW");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "sensor name", sensor_config.sensor_name)) {
    warn(__FILE__, "Point Cloud missing config value: sensor name");
    return false;
  }

  if (!GetValueFromPluginConfig(config, "size", config_size)) {
    warn(__FILE__, "Point Cloud missing config value: size");
    return false;
  }
  size = (uint16_t)stoi(config_size);

  if (!GetValueFromPluginConfig(config, "point_density", config_point_density)) {
    warn(__FILE__, "Point Cloud missing config value: point_density");
    return false;
  }
  point_density = (uint16_t)stoi(config_point_density);

  if (!GetValueFromPluginConfig(config, "point_direction", point_direction)) {
    warn(__FILE__, "Point Cloud missing config value: point_direction");
    return false;
  }
  if (!GetValueFromPluginConfig(config, "number_of_messages", config_number_of_messages)) {
    warn(__FILE__, "Point Cloud missing config value: number_of_messages");
    return false;
  }
  number_of_messages = (uint16_t)stoi(config_number_of_messages);

  if (!GetValueFromPluginConfig(config, "delay_in_ms", config_delay_in_ms)) {
    warn(__FILE__, "Point Cloud missing config value: delay_in_ms");
    return false;
  }
  delay_in_ms = (uint16_t)stoi(config_delay_in_ms);

  std::string point_colour;
  if (GetValueFromPluginConfig(config, "point_colour", point_colour) != true) {
    warn(__FILE__, "Point Cloud missing config value: point_colour");
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

  debug(__FILE__, "{0}::addConfig::subscription to is successful", getName());

  background_thread_ =
      std::make_shared<std::thread>(std::bind(&PointCloud::runExamplePointCloud, this, size, point_density, 6320000,
                                              sensor_config, number_of_messages, delay_in_ms, point_direction));

  return true;
}
bool PointCloud::enterReset() {

  ego_y_distance_.store(0.0F);
  ego_x_distance_.store(0.0F);
  ego_yaw_angle_.store(0.0F);
  ego_wheelbase_.store(0.0F);

  return true;
}

bool PointCloud::exitReset() { return true; }

bool PointCloud::GetArraySizeStruct(std::string url, size_t &array_size) const {
  bridgesdk::plugin_addons::SignalInfo sig_array_size = plugin_signal_extractor_->GetInfo(url);
  if (0 == sig_array_size.signal_size) {
    warn(__FILE__, "PointCloud could not find traffic objects URL:{0}", url);
    return false;
  }
  array_size = sig_array_size.array_size;
  return true;
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) {
  debug(__FILE__, "Point Cloud creator called");
  return new next::plugins::PointCloud(path);
}
