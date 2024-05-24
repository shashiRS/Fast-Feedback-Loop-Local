/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     tpobjects.cpp
 * @brief    DO NOT USE. Still work in progress.
 *
 * See tpobjects.h.
 *
 **/

#include <cmath>
#include <limits>
#include <regex>
#include <string>

#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>
#include <next/plugins_utils/helper_functions.h>
#include <next/plugins_utils/plugins_types.h>
#include <next/plugins_utils/plugin_3d_view_utils.hpp>
#include <next/plugins_utils/process_traffic_participants.hpp>

#include "tpobjects.h"
#include "url_mapper.h"

namespace next {
namespace plugins {
using namespace next::plugins_utils;

TpObjects::TpObjects(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  this->setVersion(std::string("0.0.1"));
  this->setName(std::string("TPObjects_plugin"));
  // set path to *.fbs file which describes the plugin interface as flatbuffer schema file
  fbs_path_ = path + std::string("/trafficparticipant.fbs");
}

TpObjects::~TpObjects() {}

void TpObjects::ProcessTPObjectsCallback(const std::string topic_name,
                                         const next::bridgesdk::ChannelMessagePointers *datablock,
                                         const next::plugins_utils::SensorConfig sensor_config,
                                         std::map<std::string, std::string> naming_map) {

  // get number of traffic objects
  next::bridgesdk::plugin_addons::dataType iNumOfUsedObjects;
  std::string extract_name = sensor_config.sensor_url + naming_map["NumUsedObjects"];
  if (false == plugin_signal_extractor_->GetValue(
                   extract_name, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32, iNumOfUsedObjects)) {
    return;
  }

  if (std::get<int32_t>(iNumOfUsedObjects) == 0) {
    counter++;
    if ((counter % 101) == 0) {
      debug(__FILE__, "100 clicks ");
    }
  }

  // create flatbuffer container with a starting size of 1024 bytes for exchanging the traffic participants with the GUI
  // Hint: Container size is automatically growing if needed
  flatbuffers::FlatBufferBuilder builder(1024);
  // create all fields of the flatbuffer schema, which are set, even if the value is to the default value
  builder.ForceDefaults(true);

  // vector contains the flatbuffer offset of all traffic participants
  std::vector<flatbuffers::Offset<GuiSchema::TrafficParticipant>> tp_list;

  for (size_t i = 0; i < tpobjects::kTrafficParticipantsCount; ++i) {
    std::string object_search_string = sensor_config.sensor_url + ".aObject[" + std::to_string(i) + "].";

    next::bridgesdk::plugin_addons::dataType lifeTime = 0;
    // create signal url
    extract_name = object_search_string + naming_map["LifeTime"];
    // get signal value of type float and store it a variable
    if (false == plugin_signal_extractor_->GetValue(
                     extract_name, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, lifeTime)) {
      warn(__FILE__, "can not GetValue lifeTime");
      break;
    }
    if (std::get<float>(lifeTime) <= (float)0.01) {
      continue;
    }

    next::bridgesdk::plugin_addons::dataType fDistX = 0;
    // create signal url
    extract_name = object_search_string + naming_map["DistX"];
    // get signal value of type float and store it a variable
    if (false == plugin_signal_extractor_->GetValue(
                     extract_name, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, fDistX)) {
      warn(__FILE__, "can not GetValue fDistX");
      break;
    }
    next::bridgesdk::plugin_addons::dataType fDistY = 0;
    // create signal url
    extract_name = object_search_string + naming_map["DistY"];
    // get signal value of type float and store it a variable
    if (false == plugin_signal_extractor_->GetValue(
                     extract_name, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, fDistY)) {
      warn(__FILE__, "can not GetValue fDistY");
      break;
    }
    next::bridgesdk::plugin_addons::dataType fVabsX = 0;
    // create signal url
    extract_name = object_search_string + naming_map["fVabsX"];
    // get signal value of type float and store it a variable
    if (false == plugin_signal_extractor_->GetValue(
                     extract_name, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, fVabsX)) {
      warn(__FILE__, "can not GetValue fVabsX");
      break;
    }
    next::bridgesdk::plugin_addons::dataType fVabsY = 0;
    // create signal url
    extract_name = object_search_string + naming_map["fVabsY"];
    // get signal value of type float and store it a variable
    if (false == plugin_signal_extractor_->GetValue(
                     extract_name, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, fVabsY)) {
      warn(__FILE__, "can not GetValue fVabsY");
      break;
    }
    next::bridgesdk::plugin_addons::dataType unified_yaw = 0;
    extract_name = object_search_string + naming_map["fYaw"];
    if (false == plugin_signal_extractor_->GetValue(
                     extract_name, next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT, unified_yaw)) {
      warn(__FILE__, "can not GetValue unified_yaw");
      break;
    }
    next::bridgesdk::plugin_addons::dataType classification_value = 0;
    // create signal url
    extract_name = object_search_string + naming_map["classification"];
    // get signal value of type float and store it a variable
    if (false == plugin_signal_extractor_->GetValue(extract_name,
                                                    next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8,
                                                    classification_value)) {
      warn(__FILE__, "can not GetValue classification_value");
      break;
    }

    GuiSchema::TrafficParticipantClass classification_enum;
    classification_enum =
        tpobjects::flatbuffer_classification_participant_ars(std::get<unsigned char>(classification_value));
    struct shapeCoordinates {
      next::bridgesdk::plugin_addons::dataType fPosX;
      next::bridgesdk::plugin_addons::dataType fPosY;
      next::bridgesdk::plugin_addons::dataType fPosXStd;
      next::bridgesdk::plugin_addons::dataType fPosYStd;
    };

    const int SHAPE_CORNERS = 4;
    shapeCoordinates allShapeCoordinates[SHAPE_CORNERS];
    // getting x and y coordinates from the recording
    for (int k = 0; k < 4; k++) {
      extract_name = object_search_string + naming_map["shapePointCoordinates[" + std::to_string(k) + "]"] + ".fPosX";
      if (false == plugin_signal_extractor_->GetValue(extract_name,
                                                      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT,
                                                      allShapeCoordinates[k].fPosX)) {
        warn(__FILE__, "can not GetValue allShapeCoordinates[k].fPosX");
        break;
      }
      extract_name = object_search_string + naming_map["shapePointCoordinates[" + std::to_string(k) + "]"] + ".fPosY";
      if (false == plugin_signal_extractor_->GetValue(extract_name,
                                                      next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT,
                                                      allShapeCoordinates[k].fPosY)) {
        warn(__FILE__, "can not GetValue allShapeCoordinates[k].fPosY");
        break;
      }
    }

    float len_x, len_y, width_x, width_y;
    len_x = std::get<float>(allShapeCoordinates[1].fPosX) - std::get<float>(allShapeCoordinates[2].fPosX);
    len_y = std::get<float>(allShapeCoordinates[1].fPosY) - std::get<float>(allShapeCoordinates[2].fPosY);
    width_x = std::get<float>(allShapeCoordinates[0].fPosX) - std::get<float>(allShapeCoordinates[1].fPosX);
    width_y = std::get<float>(allShapeCoordinates[0].fPosY) - std::get<float>(allShapeCoordinates[1].fPosY);
    float length = std::sqrt(len_x * len_x + len_y * len_y);
    float width = std::sqrt(width_x * width_x + width_y * width_y);

    if (length == std::numeric_limits<float>::infinity()) {
      length = 0;
    }

    if (width == std::numeric_limits<float>::infinity()) {
      width = 0;
    }

    float times = 0.0f, opacity = 1.0f;
    int32_t uniqueId = i, unifiedRefPoint = 0, id = i;
    auto color_string = builder.CreateString("#FF6600");
    auto sensor_string = builder.CreateString("ARS510");
    auto traffic_participant = GuiSchema::CreateTrafficParticipant(
        builder, id, std::get<float>(fDistX), std::get<float>(fDistY), std::get<float>(fDistX), std::get<float>(fDistY),
        std::get<float>(unified_yaw), length, width, std::get<float>(fVabsX), std::get<float>(fVabsY), times, uniqueId,
        unifiedRefPoint, classification_enum, GuiSchema::TrafficParticipantState::TrafficParticipantState_static_);

    tp_list.push_back(traffic_participant);
  }

  if (!tp_list.empty()) {
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
      fbs_config.sensor_id = sensor_config.sensor_uid;

      // publish the json data to guis
      plugin_publisher_->sendFlatbufferData(fbs_path_, flatbuffer, fbs_config, {"TrafficParticipant", "0.0.1"});
    }
  }
}

bool TpObjects::init() {
  debug(__FILE__, "FdpTpObjects::init()");

  /**************************************************/
  /***** create objects of plugin utils classes ****/
  /*************************************************/
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(getReference());
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(getReference());
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(getReference());
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(getReference());
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(getReference());

  // sanity check if internal data structure of these sensors have the same size
  assert(csb_coding.size() == base_coding.size());
  assert(cem200_config.size() == base_coding.size());

  debug(__FILE__, "TpObjects::init() finished");
  return true;
}

bool TpObjects::getDescription(next::bridgesdk::plugin_config_t &config_fields) const {
  /****************************************************************************/
  /*** provide configuration options of the plugin including default values ***/
  /****************************************************************************/
  // create editable configuration fields
  config_fields.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "Traffic Participants"}});
  config_fields.insert({"datacache", {next::bridgesdk::ConfigType::EDITABLE, "fdp_meas"}});
  config_fields.insert({"color", {next::bridgesdk::ConfigType::EDITABLE, "#00ffff"}});

  // create an editable configuration field with a list of suggested urls as data source
  config_fields.insert({"url", {next::bridgesdk::ConfigType::SELECTABLE, "ADC4xx.FDP_CSB.m_tpfOutput"}});
  config_fields.insert({"url", {next::bridgesdk::ConfigType::SELECTABLE, "ADC4xx.FDP_Base.p_TpfOutputIf"}});
  config_fields.insert({"url", {next::bridgesdk::ConfigType::SELECTABLE, "SIM VFB.CEM200_FDP_SYS100.m_tpfOutput"}});

  // create a configuration field with a fixed list of selectable options
  config_fields.insert({"type", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "CSB"}});
  config_fields.insert({"type", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "BASE"}});
  config_fields.insert({"type", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "CEM200"}});

  return true;
}

bool TpObjects::removeConfig(const next::bridgesdk::plugin_config_t &config) {
  // deactivate current settings with respect to the url
  std::string url;
  GetValueFromPluginConfig(config, "url", url);

  // remove sensor configuration used by this plugin configuration
  for (auto sensor : active_sensors) {

    if (sensor.sensor_url != url) {
      continue;
    }
    // unregister sensor at GUI
    plugin_3D_view_->unregisterAndPublishSensor(sensor.sensor_uid);
    // unsubscribe sensor at UDex
    for (auto sub_id : sensor.subscription_id) {
      plugin_data_subscriber_->unsubscribe(sub_id);
    }
    // remove sensor from internal sensor configuration list
    active_sensors.remove(sensor);
    break;
  }
  return true;
}

bool TpObjects::addConfig(const next::bridgesdk::plugin_config_t &config) {

  std::string url, type, sensor_name, data_view_name, data_cache_name, color;

  /*****************************************************************************/
  /*** get config field values from GUI to create a new plugin configuration ***/
  /*****************************************************************************/
  if (GetValueFromPluginConfig(config, "url", url) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "type", type) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "color", color) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "sensor name", sensor_name) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "DATA_VIEW", data_view_name) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "datacache", data_cache_name) != true) {
    return false;
  }

  // set sensor configuration based on the config field values in the widget configuration in the GUI
  next::plugins_utils::SensorConfig sensor_config;
  sensor_config.sensor_url = url;
  sensor_config.sensor_name = sensor_name;
  sensor_config.data_view_name = data_view_name;
  sensor_config.data_cache_name = data_cache_name;

  debug(__FILE__, "Sensor arguments received:");
  debug(__FILE__, "Starting up TPobjects for {0}, {1}, {2}. {3}", sensor_config.sensor_url, sensor_config.sensor_name,
        sensor_config.data_view_name, sensor_config.data_cache_name);

  std::string object_search_string_zero = sensor_config.sensor_url + ".aObject[0].";
  std::string extract_name_0 = object_search_string_zero + naming_map["LifeTime"];
  std::string extract_name;

  std::string new_current_signal_name_1;
  for (size_t i = 0; i < tpobjects::kTrafficParticipantsCount; ++i) {
    /*Signal description general beginning*/
    extract_name = object_search_string_zero + naming_map["LifeTime"];
    auto replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]");
    auto signal_info = plugin_signal_extractor_->GetInfo(replace_string);

    extract_name = object_search_string_zero + naming_map["DistX"];
    replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]");
    signal_info = plugin_signal_extractor_->GetInfo(replace_string);

    extract_name = object_search_string_zero + naming_map["DistY"];
    replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]");
    signal_info = plugin_signal_extractor_->GetInfo(replace_string);

    extract_name = object_search_string_zero + naming_map["fVabsX"];
    replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]");
    signal_info = plugin_signal_extractor_->GetInfo(replace_string);

    extract_name = object_search_string_zero + naming_map["fVabsY"];
    replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]");
    signal_info = plugin_signal_extractor_->GetInfo(replace_string);

    extract_name = object_search_string_zero + naming_map["classification"];
    replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]");
    signal_info = plugin_signal_extractor_->GetInfo(replace_string);

    extract_name = object_search_string_zero + naming_map["fYaw"];
    replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]");
    signal_info = plugin_signal_extractor_->GetInfo(replace_string);

    for (int k = 0; k < 4; k++) {
      extract_name =
          object_search_string_zero + naming_map["shapePointCoordinates[" + std::to_string(k) + "]"] + ".fPosX";
      replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]",
                                     std::regex_constants::format_first_only);
      signal_info = plugin_signal_extractor_->GetInfo(replace_string);

      extract_name =
          object_search_string_zero + naming_map["shapePointCoordinates[" + std::to_string(k) + "]"] + ".fPosY";
      replace_string = regex_replace(extract_name, std::regex("[\\[0-9]+\\]"), "[" + std::to_string(i) + "]",
                                     std::regex_constants::format_first_only);
      signal_info = plugin_signal_extractor_->GetInfo(replace_string);
    }
  }
  * /

      debug(sensor_config.sensor_url.c_str());

  // set sensor configuration for registration at GUI
  bridgesdk::plugin::SensorInfoForGUI sensor_info_gui;
  sensor_info_gui.name = sensor_config.sensor_name;
  sensor_info_gui.color = color;
  sensor_info_gui.stream = sensor_config.data_view_name;
  sensor_info_gui.type = "TP";
  void *dummy = NULL;
  size_t dummy_size = 0;

  // register sensor at GUI
  sensor_config.sensor_uid = plugin_3D_view_->registerAndPublishSensor(sensor_info_gui);

  // get topic name of the sensor data for subscription at UDex
  auto topics = plugin_signal_explorer_->getTopicsByUrl(sensor_config.sensor_url + ".HeaderObjList");

  // check if topic exists
  if (topics.size() != 1 || topics[0].size() == 0) {
    debug(__FILE__, " TPobjects could not find valid topic for URL: {}", sensor_config.sensor_url);
    return false;
  }

  auto topicSensorData = topics[0];

  // store list of object attribute names based on a predefined list for each of these three sensors
  // to access these signal values later on in the ProcessTPObjectsCallback
  // this approach makes it possible to handle these three sensors together if they
  // have the same data structure but different attribute names
  std::map<std::string, std::vector<std::string, std::string>> naming_map;

  if (type == "CSB") {
    naming_map.clear();
    for (auto url_name_temp : csb_coding) {
      naming_map.insert(url_name_temp);
    }
  } else if (type == "BASE") {
    naming_map.clear();
    for (auto url_name_temp : base_coding) {
      naming_map.insert(url_name_temp);
    }
  } else if (type == "CEM200") {
    naming_map.clear();
    for (auto url_name_temp : cem200_config) {
      naming_map.insert(url_name_temp);
    }
  }

  // create callback which shall be called when new data are available for the sensor data topic
  auto binded_callback = std::bind(&TpObjects::ProcessTPObjectsCallback, this, std::placeholders::_1,
                                   std::placeholders::_2, sensor_config, naming_map);

  // subscribe callback for topicSensorData at UDex
  sensor_config.subscription_id.push_back(
      plugin_data_subscriber_->subscribeUrl(topicSensorData, bridgesdk::SubscriptionType::DIRECT, binded_callback));

  // store active sensor configuration internally
  active_sensors.push_back(sensor_config);

  debug(__FILE__, "finish setting up TP objects");
  return true;
}

bool TpObjects::enterReset() { return true; }

bool TpObjects::exitReset() { return true; }

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::TpObjects(path); }
