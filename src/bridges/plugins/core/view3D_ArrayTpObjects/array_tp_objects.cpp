/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     array_tp_objects.cpp
 * @brief    Parsing network data of array of objects to provide Traffic Participants
 *
 * See array_tp_objects.h.
 *
 **/
#include "array_tp_objects.h"

#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>

#include <next/plugins_utils/helper_functions.h>
#include <next/bridgesdk/plugin_addons/plugin_helper_data_extractor.hpp>
#include <next/plugins_utils/process_traffic_participants.hpp>

#include "array_tp_objects_data_type.hpp"

namespace next {
namespace plugins {
using namespace next::plugins_utils;

ArrayTpObjects::ArrayTpObjects(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello view3D_ArrayTpObjects");
  this->setVersion(std::string("0.0.1"));
  this->setName(std::string("view3D_ArrayTpObjects"));
  fbs_path_ = path + std::string("/trafficparticipant.fbs");
  bfbs_path_ = path + std::string("/trafficparticipant.bfbs");
}

ArrayTpObjects::~ArrayTpObjects() {}

bool ArrayTpObjects::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  config_fields.insert({"url", {next::bridgesdk::ConfigType::EDITABLE, "ADC4xx.FDP_21P.FDP_ObjectData"}});
  config_fields.insert({"color", {next::bridgesdk::ConfigType::EDITABLE, "#ff0000"}});
  config_fields.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "Tpf 21p Traffic Participants"}});
  return true;
}

void ArrayTpObjects::ProcessDataCallback([[maybe_unused]] const std::string topic_name,
                                         const next::bridgesdk::ChannelMessagePointers *datablock,
                                         const next::plugins_utils::SensorConfig sensor_config,
                                         std::shared_ptr<ArrayTpList> array_tp_list, size_t max_arraytp_size) {

  std::lock_guard<std::mutex> processDataLock{array_tp_list->tp_protector_};
  array_tp_list->SetAddress((char *)datablock->data, datablock->size);

  std::vector<uint32_t> id;
  std::vector<uint8_t> reference_point;
  std::vector<float> position_x;
  std::vector<float> position_y;
  std::vector<float> box_yaw_angle;
  std::vector<float> box_width;
  std::vector<float> box_length;
  std::vector<uint8_t> classification;
  std::vector<float> velocity_x;
  std::vector<float> velocity_y;

  id.reserve(array_tp_list->list_id_.size());
  for (const auto &value : array_tp_list->list_id_) {
    id.push_back(value->Get());
  }

  reference_point.reserve(array_tp_list->list_reference_point_.size());
  for (const auto &value : array_tp_list->list_reference_point_) {
    reference_point.push_back(value->Get());
  }

  position_x.reserve(array_tp_list->list_position_x_.size());
  for (const auto &value : array_tp_list->list_position_x_) {
    position_x.push_back(value->Get());
  }

  position_y.reserve(array_tp_list->list_position_y_.size());
  for (const auto &value : array_tp_list->list_position_y_) {
    position_y.push_back(value->Get());
  }

  box_yaw_angle.reserve(array_tp_list->list_bow_yaw_angle_.size());
  for (const auto &value : array_tp_list->list_bow_yaw_angle_) {
    box_yaw_angle.push_back(value->Get());
  }

  box_width.reserve(array_tp_list->list_box_width_.size());
  for (const auto &value : array_tp_list->list_box_width_) {
    box_width.push_back(value->Get());
  }

  box_length.reserve(array_tp_list->list_box_length_.size());
  for (const auto &value : array_tp_list->list_box_length_) {
    box_length.push_back(value->Get());
  }

  classification.reserve(array_tp_list->list_classification_.size());
  for (const auto &value : array_tp_list->list_classification_) {
    classification.push_back(value->Get());
  }

  velocity_x.reserve(array_tp_list->list_velocity_x_.size());
  for (const auto &value : array_tp_list->list_velocity_x_) {
    velocity_x.push_back(value->Get());
  }

  velocity_y.reserve(array_tp_list->list_velocity_y_.size());
  for (const auto &value : array_tp_list->list_velocity_y_) {
    velocity_y.push_back(value->Get());
  }

  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  float x = 0;
  float y = 0;
  float unified_yaw = 0;
  float length = 0;
  float width = 0;

  size_t object_size = static_cast<size_t>(sizeof(databridge::Fdp21POjectData));
  if (datablock->size < object_size) {
    warn(__FILE__, "datablock not large enough");
  }

  // check if there is a sensor registered
  // it must be, otherwise this callback would not be called
  if (active_config_to_sensors_.empty()) {
    return;
  }

  std::vector<flatbuffers::Offset<GuiSchema::TrafficParticipant>> tp_list;

  for (size_t i = 0; i < id.size() && i < max_arraytp_size; ++i) {

    if (id[i] != 0u) {

      const int unified_referece_pt =
          tpobjects::fdp_21_reference_point_to_unified(static_cast<int>(reference_point[i]));

      if (unified_referece_pt != -1) {
        tpobjects::calculate_unified_data(unified_referece_pt, position_x[i], position_y[i], box_yaw_angle[i],
                                          box_length[i], box_width[i], x, y, unified_yaw, length, width);

        float times = 0.0f, opacity = 0.5f;
        int32_t uniqueId = 0;
        auto color_string = builder.CreateString("#ff6600");

        const uint32_t tmp_id = id[i];
        auto traffic_participant = GuiSchema::CreateTrafficParticipant(
            builder,
            tmp_id < static_cast<uint32_t>(std::numeric_limits<int32_t>::max()) ? static_cast<int>(tmp_id)
                                                                                : std::numeric_limits<int>::max(),
            position_x[i], position_y[i], x, y, unified_yaw, length, width, velocity_x[i], velocity_y[i], times,
            uniqueId, unified_referece_pt, GuiSchema::TrafficParticipantClass::car,
            GuiSchema::TrafficParticipantState::static_, builder.CreateString(sensor_config.sensor_name), color_string,
            opacity, false, false, false, false, color_string);

        tp_list.push_back(traffic_participant);
      }
    }
  }

  auto vectortest = builder.CreateVector(tp_list);
  auto traffic_participants = CreateTrafficParticipantList(builder, vectortest);

  builder.Finish(traffic_participants);

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_size = builder.GetSize();
  if (flatbuffer.fbs_size != 0U) {

    flatbuffer.fbs_binary = builder.GetBufferPointer();

    bridgesdk::plugin::SensorInfoForFbs fbs_config;
    fbs_config.data_view = sensor_config.data_view_name;
    fbs_config.cache_name = sensor_config.data_cache_name;
    fbs_config.sensor_id = sensor_config.sensor_id;
    fbs_config.timestamp_microsecond = datablock->time;

    // publish the json data to guis
    plugin_publisher_->sendFlatbufferData(fbs_path_, flatbuffer, fbs_config, {"TrafficParticipant", "0.0.1"});
  }
}

bool ArrayTpObjects::init() {

  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
  return true;
}

bool ArrayTpObjects::removeConfig(const next::bridgesdk::plugin_config_t &config) {
  debug(__FILE__, "{0}::removeConfig was called", getName());

  auto hash_key = CreateKeyOutOfPluginConfig(config);

  auto it_active_config_to_sensors = active_config_to_sensors_.find(hash_key);

  if (it_active_config_to_sensors == active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}::removeConfig::config to remove was not found. removeConfig failed", getName());
    return false;
  }

  auto &sensor = it_active_config_to_sensors->second;
  plugin_3D_view_->unregisterAndPublishSensor(sensor.sensor_id);

  for (auto id : sensor.subscription_id)
    plugin_data_subscriber_->unsubscribe(id);

  auto array_tp_list = array_tp_lists_.find(hash_key);
  if (array_tp_list != array_tp_lists_.end()) {
    std::lock_guard<std::mutex> arrayTpListLock{array_tp_list->second->tp_protector_};
    array_tp_lists_.erase(hash_key);
  } else {
    warn(__FILE__, "{0}::removeConfig::sensor to remove array_tp_list was not found. removeConfig failed", getName());
    return false;
  }

  active_config_to_sensors_.erase(hash_key);

  return true;
}

bool ArrayTpObjects::enterReset() { return true; }

bool ArrayTpObjects::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool ArrayTpObjects::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool ArrayTpObjects::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {

  debug(__FILE__, "addConfig was called");
  std::string url, sensor_name, data_view_name, color;

  config_ = config;

  if (GetValueFromPluginConfig(config, "url", url) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "sensor name", sensor_name) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "DATA_VIEW", data_view_name) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "color", color) != true) {
    return false;
  }
  next::plugins_utils::SensorConfig sensor_config;
  sensor_config.sensor_url = url;
  sensor_config.sensor_name = sensor_name;
  sensor_config.data_view_name = data_view_name;
  sensor_config.data_cache_name = sensor_name;

  debug(__FILE__, "Sensor arguments received:");

  debug(__FILE__, "addConfig was called");

  debug(__FILE__, "Starting up ArrayTpObjects for {0},{1},{2}", url, sensor_name, data_view_name);

  auto topics = plugin_signal_explorer_->getTopicsByUrl(url);
  if (topics.size() != 1 || topics[0].size() == 0) {
    warn(__FILE__, " ArrayTpObjects could not find valid topic for URL: {0}", sensor_config.sensor_url);
    return false;
  }

  bridgesdk::plugin::SensorInfoForGUI sensor_info_gui;
  sensor_info_gui.name = sensor_config.sensor_name;
  sensor_info_gui.color = color;
  sensor_info_gui.stream = sensor_config.data_view_name;
  sensor_info_gui.type = "TP";
  sensor_info_gui.flatbuffer_bfbs_path = bfbs_path_;
  sensor_info_gui.schemaName = "GuiSchema.TrafficParticipantList";
  sensor_config.sensor_id = plugin_3D_view_->registerAndPublishSensor(sensor_info_gui);

  if (sensor_config.sensor_id <= 0) {
    warn(__FILE__, "{0}::addConfig::registerAndPublishSensor to sensor name: {1} failed", getName(),
         sensor_info_gui.name);
    return false;
  }

  // store sensor for this config
  auto hash_key = CreateKeyOutOfPluginConfig(config);
  if (active_config_to_sensors_.find(hash_key) != active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}::addConfig::Key already exists and is overwritten", getName());
  }
  active_config_to_sensors_[hash_key] = sensor_config;

  size_t max_arraytp_size{0};
  std::string url_polygen_vertex = sensor_config.sensor_url + fdp_21p.at("id_");
  if (!GetArraySizeStruct(url_polygen_vertex, max_arraytp_size)) {
    removeConfig(config);
    return false;
  }

  // create array_tp_list for getting the traffic participant attributes from the topic
  auto array_tp_list =
      std::make_shared<ArrayTpList>(next::udex::extractor::emptyRequestBasic, static_cast<uint32_t>(max_arraytp_size));
  array_tp_list->urls_ = fdp_21p;
  array_tp_list->SetUrls(sensor_config.sensor_url);

  array_tp_lists_.insert({hash_key, array_tp_list});

  auto binded_callback = std::bind(&ArrayTpObjects::ProcessDataCallback, this, std::placeholders::_1,
                                   std::placeholders::_2, sensor_config, array_tp_list, max_arraytp_size);

  auto subs_id = plugin_data_subscriber_->subscribeUrl(topics[0], bridgesdk::SubscriptionType::DIRECT, binded_callback);

  if (subs_id <= 0) {
    warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), topics[0]);
    removeConfig(config);
    return false;
  }

  sensor_config.subscription_id.push_back(subs_id);
  active_config_to_sensors_[hash_key] = sensor_config;
  debug(__FILE__, "{0}::addConfig::subscription to url: {1} is successful", getName(), topics[0]);

  debug(__FILE__, "finish setting up {0} objects", getName());

  return true;
}

bool ArrayTpObjects::GetArraySizeStruct(const std::string &url, size_t &array_size) const {
  bridgesdk::plugin_addons::SignalInfo sig_array_size = plugin_signal_extractor_->GetInfo(url);
  if (0 == sig_array_size.signal_size) {
    warn(__FILE__, "ArrayTp could not find any objects URL:{0}", url);
    return false;
  }
  array_size = sig_array_size.array_size;
  return true;
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::ArrayTpObjects(path); }
