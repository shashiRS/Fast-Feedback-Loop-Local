/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     canTpObjects.cpp
 * @brief    Parsing CAN data, providing the objects
 *
 * See canTpObjects.h
 *
 **/

#include <algorithm>
#include <utility>

#include <next/plugins_utils/helper_functions.h>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/bridgesdk/plugin_addons/plugin_helper_data_extractor.hpp>
#include <next/plugins_utils/process_traffic_participants.hpp>

#include "canTpObjects.h"
#include "canTpObjects_data_type.hpp"
#include "canTpObjects_struct.hpp"

namespace next {
namespace plugins {
using namespace next::plugins_utils;
CanTpObjectsPlugin::CanTpObjectsPlugin(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello view3D_CanTpObjects plugin");
  this->setVersion(std::string("0.0.2"));
  this->setName(std::string("view3D_CanTpObjects"));
  fbs_path_ = path + std::string("/trafficparticipant.fbs");
  bfbs_path_ = path + std::string("/trafficparticipant.bfbs");
  builder_["indentation"] = "";
}

CanTpObjectsPlugin::~CanTpObjectsPlugin() {}

void CanTpObjectsPlugin::ProcessDataCallback(const std::string topic_name,
                                             const next::bridgesdk::ChannelMessagePointers *datablock,
                                             const next::plugins_utils::SensorConfig sensor_config,
                                             std::shared_ptr<CanTpObjects> &can_tp_objects) {
  can_tp_objects->SetAddress(reinterpret_cast<const char *>(datablock->data), datablock->size);

  flatbuffers::FlatBufferBuilder builder(1024);
  builder.ForceDefaults(true);

  auto cantp_objects = can_tp_objects->GetCanTpOjects();
  std::vector<flatbuffers::Offset<GuiSchema::TrafficParticipant>> tp_list{};
  auto traffic_participant_1 =
      CreateTrafficParticipant(builder, sensor_config.sensor_name, cantp_objects[topic_name].object_1);
  tp_list.push_back(traffic_participant_1);

  auto traffic_participant_2 =
      CreateTrafficParticipant(builder, sensor_config.sensor_name, cantp_objects[topic_name].object_2);
  tp_list.push_back(traffic_participant_2);

  auto serialized_vector = builder.CreateVector(tp_list);
  auto traffic_participants = CreateTrafficParticipantList(builder, serialized_vector);

  builder.Finish(traffic_participants);

  bridgesdk::plugin::FlatbufferPackage flatbuffer;
  flatbuffer.fbs_size = builder.GetSize();
  if (flatbuffer.fbs_size != 0U) {
    flatbuffer.fbs_binary = builder.GetBufferPointer();

    bridgesdk::plugin::SensorInfoForFbs fbs_config;
    fbs_config.data_view = sensor_config.data_view_name;
    std::string first_id{""}, second_id{""};
    can_tp_objects->GetIds(topic_name, first_id, second_id);
    fbs_config.cache_name = sensor_config.data_cache_name + first_id + second_id;
    fbs_config.sensor_id = sensor_config.sensor_id;
    fbs_config.timestamp_microsecond = datablock->time;

    plugin_publisher_->sendFlatbufferData(fbs_path_, flatbuffer, fbs_config, {"TrafficParticipant", "0.0.1"});
  }
}

void CanTpObjectsPlugin::SetRegexValuesInConfig() {
  auto config_client = next::appsupport::ConfigClient::getConfig();
  if (!config_client) {
    debug(__FILE__, "{0}::SetRegexValuesInConfig()::couldn't access configuration to update regex", getName());
    return;
  }

  auto setRegexInConfig = [&config_client](const std::string &cfg_keyword, std::vector<std::string> regex_list) {
    for (size_t i = 0; i < regex_list.size(); i++) {
      config_client->put(fmt::format("[{}]", cfg_keyword, i), regex_list[i]);
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

bool CanTpObjectsPlugin::init() {
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
  plugin_regex_searcher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginRegexSearcher>();

  InitSearchRequests();
  SetRegexValuesInConfig();
  return true;
}

bool CanTpObjectsPlugin::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  config_fields.insert({"url", {next::bridgesdk::ConfigType::FIXED, "Local_ARS_CANFD.ARS510HA22_Private"}});
  config_fields.insert({"name", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "ARSCAN2"}});
  config_fields.insert({"name", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "ARSCAN1"}});
  config_fields.insert({"cachename", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "cache2"}});
  config_fields.insert({"cachename", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, "cache1"}});
  config_fields.insert({"dataview", {next::bridgesdk::ConfigType::FIXED, "objectData"}});
  return true;
}

bool CanTpObjectsPlugin::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = AddConfigInternal(config);
  return init_succesfull_;
}

bool CanTpObjectsPlugin::AddConfigInternal(const next::bridgesdk::plugin_config_t &config) {
  std::string url, name, cachename, dataview, data_cache_name, color;
  next::plugins_utils::SensorConfig sensor_config;
  config_ = config;

  if (GetValueFromPluginConfig(config, "url", url) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "name", name) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "cachename", cachename) != true) {
    return false;
  }
  if (GetValueFromPluginConfig(config, "dataview", dataview) != true) {
    return false;
  }

  sensor_config.sensor_url = url;
  sensor_config.sensor_name = name;
  sensor_config.data_cache_name = cachename;
  sensor_config.data_view_name = dataview;

  bridgesdk::plugin::SensorInfoForGUI sensor_info_gui;

  sensor_info_gui.name = sensor_config.sensor_name;
  sensor_info_gui.color = "#ffff00";
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
  auto master_key = CreateKeyOutOfPluginConfig(config);
  if (active_config_to_sensors_.find(master_key) != active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}::addConfig::Key already exists and is overwritten", getName());
  }
  active_config_to_sensors_[master_key] = sensor_config;

  auto can_tp_objects = std::make_shared<CanTpObjects>(next::udex::extractor::emptyRequestBasic);
  if (auto naming_map = GetNamingMap(url); naming_map.size() != 0) {
    can_tp_objects->urls_ = naming_map;
  }

  auto results = FindResultsByUrl("");
  for (const auto &result : results) {
    auto topics = plugin_signal_explorer_->getTopicsByUrl(result.root_url);
    // skip non existing can topics
    if (topics.size() != 1 || topics[0].size() == 0) {
      warn(__FILE__, "[Plugin] {0}:addConfig::Could not find valid topic for url: {1}", getName(), result.root_url);
      continue;
    }

    debug(__FILE__, "subscribing topic {0}", topics[0]);

    auto binded_callback = std::bind(&CanTpObjectsPlugin::ProcessDataCallback, this, std::placeholders::_1,
                                     std::placeholders::_2, sensor_config, can_tp_objects);

    auto subs_id =
        plugin_data_subscriber_->subscribeUrl(topics[0], bridgesdk::SubscriptionType::DIRECT, binded_callback);

    if (subs_id <= 0) {
      warn(__FILE__, "{0}::addConfig::subscription to url: {1} failed", getName(), topics[0]);
      removeConfig(config);
      return false;
    }

    sensor_config.subscription_id.push_back(subs_id);
    active_config_to_sensors_[master_key] = sensor_config;
    debug(__FILE__, "{0}::addConfig::subscription to url: {1} is successful", getName(), topics[0]);

    can_tp_objects->SetUrls(result.root_url);
  }

  if (0 == sensor_config.subscription_id.size()) {
    warn(__FILE__, "{0}::addConfig::No topics found. addConfig failed", getName());
    removeConfig(config);
    return false;
  }

  return true;
}

void CanTpObjectsPlugin::InitSearchRequests() {
  search_requests_.clear();

  bool optional = true;
  keywords_ = {"ARS_OBJ"};

  std::vector<std::tuple<std::string, std::vector<std::string>, bool>> requests = {
      {"pos_x_", {R"((fDistX_OBJ))"}, !optional},
      {"pos_y_", {R"((fDistY_OBJ))"}, !optional},
      {"yaw_", {R"((fOrientationStd_OBJ))"}, !optional},
      {"width_", {R"((fWidth_OBJ))"}, !optional},
      {"length_", {R"((flength_OBJ))"}, !optional},
      {"classification_ars_", {R"((eClassification_OBJ))"}, !optional},
      {"vel_x_", {R"((fVrelX_OBJ))"}, !optional},
      {"vel_y_", {R"((fVrelY_OBJ))"}, !optional},
      {"ref_tmp_", {R"((eRefPointPos_OBJ))"}, !optional},
      {"id_", {R"((uiID_OBJ))"}, !optional}};

  for (const auto &[key, patterns, isOptional] : requests) {
    search_requests_.emplace_back(SearchRequest{key, patterns, isOptional});
  }

  if (!plugin_regex_searcher_) {
    return;
  }

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

std::vector<CanTpObjectsPlugin::SearchResult> CanTpObjectsPlugin::FindResultsByUrl(const std::string &url) {
  std::vector<SearchResult> result{};
  if (!plugin_signal_explorer_ || !plugin_regex_searcher_) {
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

  return result;
}

std::vector<CanTpObjectsPlugin::SearchResult>
CanTpObjectsPlugin::GetSearchResults(const std::string &url, const std::unordered_set<std::string> &unique_urls) const {
  if (!plugin_regex_searcher_) {
    return {};
  }

  if (url.empty()) {
    return plugin_regex_searcher_->GetSearchResults(unique_urls);
  } else {
    return {plugin_regex_searcher_->GetSearchResultForUrl(url, unique_urls)};
  }
}

flatbuffers::Offset<GuiSchema::TrafficParticipant>
CanTpObjectsPlugin::CreateTrafficParticipant(flatbuffers::FlatBufferBuilder &builder, const std::string &sensor_name,
                                             std::shared_ptr<TrafficObject> traffic_object) {
  auto color = builder.CreateString(default_color);
  auto sensor = builder.CreateString(default_sensor);
  int unified_referece_pt = tpobjects::ars_reference_point_to_unified(traffic_object->GetRefTmp());

  auto traffic_participant = GuiSchema::CreateTrafficParticipant(
      builder, traffic_object->GetId(), traffic_object->GetPosX(), traffic_object->GetPosY(), traffic_object->GetPosX(),
      traffic_object->GetPosY(), traffic_object->GetYaw(), traffic_object->GetLength(), traffic_object->GetWidth(), {},
      {}, {}, {}, unified_referece_pt, GuiSchema::TrafficParticipantClass::car,
      GuiSchema::TrafficParticipantState::static_, builder.CreateString(sensor_name), sensor, default_opacity, false,
      false, false, false, color);

  return traffic_participant;
}

std::unordered_map<std::string, std::string> CanTpObjectsPlugin::GetNamingMap(const std::string &url) {

  std::vector<SearchResult> search_results;
  GetAvailableTPLists(url, search_results);

  std::unordered_map<std::string, std::string> naming_map;
  auto data_type = search_results[0].data_type_to_result;
  std::transform(data_type.begin(), data_type.end(), std::inserter(naming_map, naming_map.end()),
                 [](const auto &pair) { return std::make_pair(pair.first, std::move(pair.second.result_url)); });

  return naming_map;
}

bool CanTpObjectsPlugin::GetAvailableTPLists(const std::string &url, std::vector<SearchResult> &search_results) {
  std::unordered_set<std::string> urls_to_scan{};

  if (!plugin_signal_explorer_ || !plugin_regex_searcher_) {
    return false;
  }

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

void CanTpObjectsPlugin::UpdateRegexValuesFromConfig() {
  if (!plugin_regex_searcher_) {
    return;
  }

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

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

bool CanTpObjectsPlugin::removeConfig(const next::bridgesdk::plugin_config_t &config) {

  debug(__FILE__, "{0}::removeConfig was called", getName());

  auto master_key = CreateKeyOutOfPluginConfig(config);

  auto it_active_config_to_sensors = active_config_to_sensors_.find(master_key);

  if (it_active_config_to_sensors == active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}::removeConfig::config to remove was not found. removeConfig failed", getName());
    return false;
  }

  auto &sensor = it_active_config_to_sensors->second;
  plugin_3D_view_->unregisterAndPublishSensor(sensor.sensor_id);

  for (auto id : sensor.subscription_id)
    plugin_data_subscriber_->unsubscribe(id);

  active_config_to_sensors_.erase(master_key);

  return true;
}

bool CanTpObjectsPlugin::enterReset() { return true; }

bool CanTpObjectsPlugin::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) {
  debug(__FILE__, "CanTpObjectsPlugin creator called");
  return new next::plugins::CanTpObjectsPlugin(path);
}
