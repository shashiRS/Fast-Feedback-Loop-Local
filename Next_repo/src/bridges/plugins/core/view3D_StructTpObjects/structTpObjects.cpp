/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     structTpObjects.cpp
 * @brief    Parsing network data of struct with array of object attributes to provide Traffic Participants
 *
 * See structTpObjects.h.
 *
 **/

#include "structTpObjects.h"

#include <cmath>
#include <fstream>
#include <limits>
#include <string>

#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>

#include <next/plugins_utils/helper_functions.h>
#include <next/plugins_utils/plugins_types.h>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <next/plugins_utils/plugin_3d_view_utils.hpp>
#include <next/plugins_utils/process_traffic_participants.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/version_manager/version_manager.hpp>

#include "url_mapper.h"

namespace next {
namespace plugins {
using namespace next::plugins_utils;
using namespace boost::numeric;

StructTpObjects::StructTpObjects(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello view3D_StructTpObjects plugin");
  this->setVersion(std::string("0.0.1"));
  this->setName(std::string("view3D_StructTpObjects"));
  fbs_path_ = path + std::string("/trafficparticipant.fbs");
  bfbs_path_ = path + std::string("/view_update.bfbs");

  InitSearchRequests();
  SetRegexValuesInConfig();
  trans_rotate_matrix_ = GetTransformMatrix(0, 0, 0);
}

StructTpObjects::~StructTpObjects() {}

void StructTpObjects::ProcessTPObjectsCallback([[maybe_unused]] const std::string topic_name,
                                               const next::bridgesdk::ChannelMessagePointers *datablock,
                                               const next::plugins_utils::SensorConfig sensor_config,
                                               std::shared_ptr<TrafficList> traffic_list,
                                               const ObjectValidation object_validation) {
  std::lock_guard<std::mutex> lock(traffic_list_protector_);

  traffic_list->SetAddress((char *)datablock->data, datablock->size);
  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);
  std::vector<flatbuffers::Offset<GuiSchema::TrafficParticipant>> tp_list;

  if (traffic_list->num_objects_->Get() > (int)traffic_list->GetMaxSizeTrafficList()) {
    warn(__FILE__,
         "{0}::ProcessTPObjectsCallback::NumObjects is {1}, but object list size in data description is only {2}",
         getName(), traffic_list->num_objects_->Get(), traffic_list->GetMaxSizeTrafficList());
  }

  for (uint32_t i_tp = 0; i_tp < traffic_list->GetMaxSizeTrafficList(); ++i_tp) {
    float length{0.0f};
    float width{0.0f};

    if (object_validation == ObjectValidation::lifetime) {
      if (traffic_list->tp_objects[i_tp]->GetLifeTime() <= 0.01f) {
        continue;
      }
    }

    std::shared_ptr<TrafficObject> tp_general = std::static_pointer_cast<TrafficObject>(traffic_list->tp_objects[i_tp]);
    length = tp_general->GetLength();
    width = tp_general->GetWidth();

    if (length == std::numeric_limits<float>::infinity()) {
      length = 0;
    }
    if (width == std::numeric_limits<float>::infinity()) {
      width = 0;
    }

    if (object_validation == ObjectValidation::dimensions) {
      if ((length <= 0.01f) || (width <= 0.01f)) {
        continue;
      }
    }

    float posx = traffic_list->tp_objects[i_tp]->GetPosX();
    float posy = traffic_list->tp_objects[i_tp]->GetPosY();
    float velx = traffic_list->tp_objects[i_tp]->GetVelX();
    float vely = traffic_list->tp_objects[i_tp]->GetVelY();
    float yaw = traffic_list->tp_objects[i_tp]->GetYaw();
    float times = 0.0f;
    unsigned char classification = traffic_list->tp_objects[i_tp]->GetClassification();
    int32_t uniqueId = static_cast<int32_t>(i_tp), unifiedRefPoint = 0, id = static_cast<int32_t>(i_tp);
    GuiSchema::TrafficParticipantClass classification_enum;
    GuiSchema::TrafficParticipantState classification_type;
    if (ecu_type_ == ECU_type::ARS) {
      classification_enum = tpobjects::flatbuffer_classification_participant_ars(classification);
      classification_type = GuiSchema::TrafficParticipantState::static_;
    } else {
      classification_enum = tpobjects::flatbuffer_classification_participant_parking(classification);
      classification_type = GuiSchema::TrafficParticipantState::dyn;
    }

    if (coordinate_system_ == CoordinateSystem::kEgoVehicleRearAxle ||
        coordinate_system_ == CoordinateSystem::kEgoVehicleFrontAxle) {
      {
        std::lock_guard<std::mutex>{config_ego_vehicle_protector_};
        UpdateTransformMatrix(trans_rotate_matrix_, ego_yaw_angle_.load(), ego_x_distance_.load(),
                              ego_y_distance_.load());
      }
      transformPoint(trans_rotate_matrix_, posx, posy, posx, posy);
    }

    auto traffic_participant =
        GuiSchema::CreateTrafficParticipant(builder, id, posx, posy, posx, posy, yaw, length, width, velx, vely, times,
                                            uniqueId, unifiedRefPoint, classification_enum, classification_type);
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
      fbs_config.sensor_id = sensor_config.sensor_id;
      fbs_config.timestamp_microsecond = datablock->time;

      // publish the json data to guis
      plugin_publisher_->sendFlatbufferData(fbs_path_, flatbuffer, fbs_config, {"TrafficParticipant", "0.0.1"});
    }
  }
}

void StructTpObjects::ProcessDataCallbackEgoVehicle(const std::string topic_name,
                                                    const next::bridgesdk::ChannelMessagePointers *datablock,
                                                    next::bridgesdk::plugin_config_t config,
                                                    next::plugins_utils::SensorConfig sensor_config,
                                                    std::shared_ptr<EgoVehicle> ego_vehicle) {
  (void)topic_name;
  (void)config;
  (void)sensor_config;
  std::lock_guard<std::mutex> lock(config_ego_vehicle_protector_);

  ego_vehicle->SetAddress((char *)datablock->data, datablock->size);

  ego_y_distance_.store(ego_vehicle->ego_y_->Get());
  ego_x_distance_.store(ego_vehicle->ego_x_->Get());
  ego_yaw_angle_.store(ego_vehicle->ego_yaw_->Get());
}

bool StructTpObjects::init() {
  // init("StructTpObjects.log");
  debug(__FILE__, "StructTpObjects::init()");

  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  plugin_regex_searcher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginRegexSearcher>();

  assert(csb_coding.size() == base_coding.size());
  debug(__FILE__, "StructTpObjects::init() finished");
  return true;
}

void StructTpObjects::InitSearchRequests() {
  search_requests_.clear();

  bool optional = true;
  keywords_ = {"object"};

  std::vector<std::tuple<std::string, std::vector<std::string>, bool>> requests = {
      {"num_objects_", {R"(([objlist]*?header.*?num.*objects?|num.*objects?|numberOfObjects)$)"}, !optional},

      {"aObject_", {R"(\.(aObject|obj|objects)\[)"}, !optional},

      {"pos_x_",
       {R"(((kinematic\.)?[f.]+distx*)$)", R"((objectDynamics\.position\.pos[x]+))", R"((shape\.referencePoint\.x)+$)",
        R"(\.(centerPoint_x)$)"},
       !optional},

      {"pos_y_",
       {R"(((kinematic\.)?[f.]+disty*)$)", R"((objectDynamics\.position\.pos[y]+))", R"((shape\.referencePoint\.y)+$)",
        R"(\.(centerPoint_y)$)"},
       !optional},

      {"life_time_", {R"(([general]*[\.][f]?LifeTime|age)$)"}, optional},

      {"vel_x_",
       {R"(((\.kinematic\.)fVabsX|absVelX))", R"((objectDynamics\.dynamics\.VelX))", R"((velocity\.+x))"},
       optional},

      {"vel_y_",
       {R"(((\.kinematic\.)fVabsY|absVelY))", R"((objectDynamics\.dynamics\.VelY))", R"((velocity\.+y))"},
       optional},

      {"yaw_",
       {R"(\.(kinematic\.fYaw|addKinematic\.fYaw|heading|orientation|objectDynamics\.orientation\.yaw|yawrate|objectYaw)$)"},
       !optional},

      {"classification_", {R"(\.([classification]*\.*[ec]*lassification)$)", R"(\.(classType))"}, optional},

      {"shapepoint_0_pos_x_",
       {R"(\.(shapePoints\..+\[0\]\.fPosX|length|geometry\.length|objectSize\.length)$)",
        R"(\.(shape\.points\[0\]\.position\.x)$)"},
       !optional},

      {"shapepoint_0_pos_y_",
       {R"(\.(shapePoints\..+\[0\]\.fPosY|width|geometry\.width|objectSize\.width)$)",
        R"(\.(shape\.points\[0\]\.position\.y)$)"},
       !optional},

      {"shapepoint_1_pos_x_",
       {R"(\.(shapePoints\..+\[1\]\.fPosX|length|geometry\.length|objectSize\.length)$)",
        R"(\.(shape\.points\[1\]\.position\.x)$)"},
       !optional},

      {"shapepoint_1_pos_y_",
       {R"(\.(shapePoints\..+\[1\]\.fPosY)$)", R"(\.(shape\.points\[1\]\.position\.y)$)"},
       optional},

      {"shapepoint_2_pos_x_",
       {R"(\.(shapePoints\..+\[2\]\.fPosX)$)", R"(\.(shape\.points\[2\]\.position\.x)$)"},
       optional},

      {"shapepoint_2_pos_y_",
       {R"(\.(shapePoints\..+\[2\]\.fPosY)$)", R"(\.(shape\.points\[2\]\.position\.y)$)"},
       optional}};

  for (const auto &[key, patterns, isOptional] : requests) {
    search_requests_.emplace_back(SearchRequest{key, patterns, isOptional});
  }
}

void StructTpObjects::SetRegexValuesInConfig() {
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

bool StructTpObjects::GetAvailableTPLists(const std::string &url, std::vector<SearchResult> &search_results) {
  search_results.clear();
  std::unordered_set<std::string> urls_to_scan{};

  if (!plugin_signal_explorer_)
    return false;

  if (!plugin_regex_searcher_)
    return false;

  UpdateRegexValuesFromConfig();

  if (url.empty()) {
    // default search
    for (const auto &keyword : keywords_) {
      auto results = plugin_signal_explorer_->searchSignalTree(keyword);

      urls_to_scan.reserve(urls_to_scan.size() + results.size());
      std::move(results.begin(), results.end(), std::inserter(urls_to_scan, urls_to_scan.end()));
    }
  } else {
    // tplist url already selected
    auto results = plugin_signal_explorer_->searchSignalTree(url);
    urls_to_scan.reserve(results.size());
    std::move(results.begin(), results.end(), std::inserter(urls_to_scan, urls_to_scan.end()));
  }

  if (url.empty())
    search_results = plugin_regex_searcher_->GetSearchResults(urls_to_scan);
  else
    search_results.emplace_back(plugin_regex_searcher_->GetSearchResultForUrl(url, urls_to_scan));

  if (search_results.size() == 0)
    return false;

  return true;
}

void StructTpObjects::UpdateRegexValuesFromConfig() {
  auto config_client = next::appsupport::ConfigClient::getConfig();
  if (!config_client) {
    warn(__FILE__, "{0}::UpdateRegexValuesFromConfig::couldn't access configuration to read regex values", getName());
    return;
  }

  if (!plugin_regex_searcher_) {
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

bool StructTpObjects::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  config_fields.insert({"sensor name", {next::bridgesdk::ConfigType::EDITABLE, "Traffic Participants"}});
  config_fields.insert({"color", {next::bridgesdk::ConfigType::EDITABLE, "#00ffff"}});
  config_fields.insert({"datacache", {next::bridgesdk::ConfigType::EDITABLE, ""}});
  config_fields.insert({"Object validation", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "lifetime"}});
  config_fields.insert({"Object validation", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "dimensions"}});

  config_fields.insert(
      {"url_veh", {next::bridgesdk::ConfigType::SELECTABLE, "MTA_ADC5.MF_VEDODO_DATA.OdoEstimationPort"}});
  config_fields.insert(
      {"url_veh", {next::bridgesdk::ConfigType::SELECTABLE, "ADC5xx_Device.VD_DATA.IuOdoEstimationPort"}});
  config_fields.insert({"url_veh", {next::bridgesdk::ConfigType::SELECTABLE, "M7board.CAN_Thread.OdoEstimationPort"}});

  for (const auto coordinate_system_map_entry : coordinate_system_to_enum_) {
    config_fields.insert(
        {"coordinate_system", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, coordinate_system_map_entry.first}});
  }

  for (const auto ecu_type_map_entry : ECU_type_to_enum_) {
    config_fields.insert(
        {"ECU type (default ARS)", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, ecu_type_map_entry.first}});
  }

  std::vector<SearchResult> search_results;
  if (GetAvailableTPLists("", search_results)) {
    for (const auto &search_result : search_results) {
      config_fields.insert({"url", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, search_result.root_url}});
    }
  }

  if (search_results.size() == 0) {
    warn(__FILE__, "{0}::getDescription:: No traffic participant list found.", getName());
    config_fields.insert({"url", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, ""}});
  }
  return true;
}

bool StructTpObjects::removeConfig(const next::bridgesdk::plugin_config_t &config) {
  debug(__FILE__, "{0}::removeConfig was called", getName());

  auto hash_key = CreateKeyOutOfPluginConfig(config);

  auto it_active_config_to_sensors = active_config_to_sensors_.find(hash_key);

  if (it_active_config_to_sensors == active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}::removeConfig::config to remove was not found. removeConfig failed", getName());
    return false;
  }

  auto &config_data = it_active_config_to_sensors->second;

  plugin_3D_view_->unregisterAndPublishSensor(config_data.sensor_config_.sensor_id);
  for (auto id : config_data.sensor_config_.subscription_id) {
    plugin_data_subscriber_->unsubscribe(id);
  }

  if (nullptr != config_data.traffic_list_) {
    std::lock_guard<std::mutex> lock(traffic_list_protector_);
    config_data.traffic_list_.reset();
  }

  if (nullptr != config_data.ego_vehicle_) {
    std::lock_guard<std::mutex> lock(config_ego_vehicle_protector_);
    config_data.ego_vehicle_.reset();
  }

  ego_x_distance_ = 0;
  ego_y_distance_ = 0;
  ego_yaw_angle_ = 0;
  coordinate_system_ = CoordinateSystem::kGlobal;
  trans_rotate_matrix_.clear();
  active_config_to_sensors_.erase(hash_key);

  return true;
}

bool StructTpObjects::GetNamingMap(const std::string &url, std::unordered_map<std::string, std::string> &naming_map) {
  std::vector<SearchResult> search_results;
  GetAvailableTPLists(url, search_results);

  if (search_results.size() != 1)
    return false;

  for (const auto &r : search_results[0].data_type_to_result) {
    naming_map[r.first] = r.second.result_url;
  }

  return true;
}

bool StructTpObjects::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

StructTpObjects::ObjectValidation
StructTpObjects::stringToObjectValidation(const std::string &object_validation_string) {
  if (object_validation_string == "dimensions") {
    return ObjectValidation::dimensions;
  } else {
    return ObjectValidation::lifetime;
  }
}

bool StructTpObjects::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {

  std::string url, sensor_name, data_view_name, data_cache_name, color, object_validation_cfg;
  std::string url_veh;
  config_ = config;
  std::string coordinate_type{};
  std::string ecu_type{};
  if (GetValueFromPluginConfig(config, "coordinate_system", coordinate_type) != true) {
    warn(__FILE__, "StructTP plugin missing config value: coordinate_system");
    return false;
  }
  coordinate_system_ = coordinate_system_to_enum_.at(coordinate_type);
  if (GetValueFromPluginConfig(config, "url", url) != true) {
    return false;
  }
  if ((GetValueFromPluginConfig(config, "url_veh", url_veh) != true) &&
      (coordinate_system_ != CoordinateSystem::kGlobal)) {
    warn(__FILE__, "StructTP plugin missing config value: url_veh");
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
  if (GetValueFromPluginConfig(config, "ECU type (default ARS)", ecu_type) != true) {
    ecu_type_ = ECU_type::ARS;
  }
  ecu_type_ = ECU_type_to_enum_.at(ecu_type);
  if (GetValueFromPluginConfig(config, "Object validation", object_validation_cfg) != true) {
    object_validation_cfg = "lifetime";
  }

  std::unordered_map<std::string, std::string> naming_map;
  if (!GetNamingMap(url, naming_map)) {
    return false;
  }

  ObjectValidation object_validation = stringToObjectValidation(object_validation_cfg);
  next::plugins_utils::SensorConfig sensor_config;
  sensor_config.sensor_url = url;
  sensor_config.sensor_name = sensor_name;
  sensor_config.data_view_name = data_view_name;
  sensor_config.data_cache_name = data_cache_name;

  debug(__FILE__, "Sensor arguments received:");
  debug(__FILE__, "Starting up StructTpObjects for {0}, {1}, {2}. {3}", sensor_config.sensor_url,
        sensor_config.sensor_name, sensor_config.data_view_name, sensor_config.data_cache_name);

  auto topics =
      plugin_signal_explorer_->getTopicsByUrl(sensor_config.sensor_url + ".HeaderObjList.contributingSensors");
  if (topics.size() != 1 || topics[0].size() == 0) {
    debug(__FILE__, "StructTpObjects could not find valid topic for URL: {}", sensor_config.sensor_url);
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
  active_config_to_sensors_[hash_key].sensor_config_ = sensor_config;

  if (naming_map.find("aObject_") == naming_map.end()) {
    warn(__FILE__, "{0}::addConfig::object not found in url map", getName());
    removeConfig(config);
    return false;
  }
  std::string url_struct_tp = sensor_config.sensor_url + naming_map["aObject_"];
  size_t max_number_elements{0};
  if (!GetArraySizeStruct(url_struct_tp, max_number_elements)) {
    removeConfig(config);
    return false;
  }

  auto traffic_list = std::make_shared<TrafficList>(next::udex::extractor::emptyRequestBasic,
                                                    static_cast<uint32_t>(max_number_elements));
  active_config_to_sensors_[hash_key].traffic_list_ = traffic_list;

  traffic_list->urls_.clear();
  traffic_list->urls_ = naming_map;
  if (!traffic_list->SetUrls(sensor_config.sensor_url)) {
    warn(__FILE__, "{0}::addConfig::SetUrls failed for TrafficList", getName());
    removeConfig(config);
    return false;
  }

  if (data_cache_name == "") {
    sensor_config.data_cache_name = "cache_name_auto_" + std::to_string(sensor_config.sensor_id);
  }

  auto binded_callback = std::bind(&StructTpObjects::ProcessTPObjectsCallback, this, std::placeholders::_1,
                                   std::placeholders::_2, sensor_config, traffic_list, object_validation);

  auto subs_id = plugin_data_subscriber_->subscribeUrl(topics[0], bridgesdk::SubscriptionType::DIRECT, binded_callback);

  if (subs_id <= 0) {
    warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), topics[0]);
    removeConfig(config);
    return false;
  }

  sensor_config.subscription_id.push_back(subs_id);
  active_config_to_sensors_[hash_key].sensor_config_ = sensor_config;
  debug(__FILE__, "{0}::addConfig::subscription to url: {1} is successful", getName(), topics[0]);

  if (coordinate_system_ == CoordinateSystem::kEgoVehicleRearAxle ||
      coordinate_system_ == CoordinateSystem::kEgoVehicleFrontAxle) {
    auto ego_topic = plugin_signal_explorer_->getTopicsByUrl(url_veh);
    if (ego_topic.empty() || ego_topic[0].size() == 0) {
      warn(__FILE__, "StructTP plugin (Ego vehicle topic) could not find valid topic for URL: {0}", url_veh);
      removeConfig(config);
      return false;
    }

    auto ego_vehicle_struct = std::make_shared<EgoVehicle>(next::udex::extractor::emptyRequestBasic);
    active_config_to_sensors_[hash_key].ego_vehicle_ = ego_vehicle_struct;

    ego_vehicle_struct->urls_.clear();
    ego_vehicle_struct->urls_ = parking_odo_estimation;
    if (!ego_vehicle_struct->SetUrls(url_veh)) {
      warn(__FILE__, "{0}::addConfig::SetUrls of struct extractor 'ego_vehicle_struct' failed.", getName());
      removeConfig(config);
      return false;
    }

    auto bound_callback_ego = std::bind(&StructTpObjects::ProcessDataCallbackEgoVehicle, this, std::placeholders::_1,
                                        std::placeholders::_2, config, sensor_config, ego_vehicle_struct);

    subs_id =
        plugin_data_subscriber_->subscribeUrl(ego_topic[0], bridgesdk::SubscriptionType::DIRECT, bound_callback_ego);

    if (subs_id <= 0) {
      warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), ego_topic[0]);
      removeConfig(config);
      return false;
    }

    sensor_config.subscription_id.push_back(subs_id);
    active_config_to_sensors_[hash_key].sensor_config_ = sensor_config;
  }

  info(__FILE__, "finish setting up StructTpObjects for config: url {0}, name {1}", sensor_config.sensor_url,
       sensor_config.sensor_name);
  return true;
}

bool StructTpObjects::enterReset() { return true; }

bool StructTpObjects::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool StructTpObjects::GetArraySizeStruct(const std::string &url, size_t &array_size) const {
  bridgesdk::plugin_addons::SignalInfo sig_array_size = plugin_signal_extractor_->GetInfo(url);
  if (0 == sig_array_size.signal_size) {
    warn(__FILE__, " Struct TP could not find traffic objects URL:{0}", url);
    return false;
  }
  array_size = sig_array_size.array_size;
  return true;
}

ublas::matrix<float> StructTpObjects::GetTransformMatrix(float yaw, float x_pos, float y_pos) {
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

void StructTpObjects::transformPoint(const ublas::matrix<float> &transform_matrix, float pos_x_in, float pos_y_in,
                                     float &pos_x_out, float &pos_y_out) {
  ublas::matrix<float> point_value(4, 1);

  point_value(0, 0) = pos_x_in;
  point_value(1, 0) = pos_y_in;
  point_value(2, 0) = .0F;
  point_value(3, 0) = 1.F;

  auto result = ublas::prod(transform_matrix, point_value);

  pos_x_out = result(0, 0);
  pos_y_out = result(1, 0);
}

void StructTpObjects::UpdateTransformMatrix(boost::numeric::ublas::matrix<float> &matrix, float yaw, float x_pos,
                                            float y_pos) {

  matrix(0, 0) = cosf(yaw);
  matrix(0, 1) = -sinf(yaw);
  matrix(0, 3) = x_pos;
  matrix(1, 0) = sinf(yaw);
  matrix(1, 1) = cosf(yaw);
  matrix(1, 3) = y_pos;
  matrix(2, 2) = 1.F;
  matrix(3, 3) = 1.F;
}
} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::StructTpObjects(path); }
