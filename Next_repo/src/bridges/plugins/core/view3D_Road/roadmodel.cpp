/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     roadmodel.cpp
 * @brief    Parsing network data to provide road model
 *
 * See roadmodel.h.
 *
 **/
#include "roadmodel.h"

#include <next/plugins_utils/helper_functions.h>
#include "roadmodel_flatbuffer_converter.h"

//#define TEST_DATA
#ifdef TEST_DATA
#include "plugins_utils/fdp_dummy_data_creator.h"
#endif

namespace next {
namespace plugins {
using namespace next::plugins_utils;

VisuRoadModel::VisuRoadModel(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello view3D_Roadmodel plugin");
  this->setVersion(std::string("0.0.1-alpha1"));
  this->setName(std::string("view3D_Roadmodel"));
  fbs_path_ = path + std::string("/groundline.fbs");
  bfbs_path_ = path + std::string("/groundline.bfbs");

  InitSearchRequests();
}

bool VisuRoadModel::init() {
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  plugin_regex_searcher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginRegexSearcher>();

  InitSearchRequests();
  SetRegexValuesInConfig();

  return true;
}

void VisuRoadModel::InitSearchRequests() {
  search_requests_.clear();

  bool optional = true;
  search_keywords_ = {"rmfoutput"};

  std::vector<std::tuple<std::string, std::vector<std::string>, bool>> requests = {
      {"ego_lane_", {R"((\.laneTopology\.egoLane)$)"}, !optional},

      {"ego_position_", {R"((\.laneTopology\.egoPosition)$)"}, !optional},

      {"num_connections_", {R"((\.laneTopology\.numConnections)$)"}, !optional},

      {"num_lane_segments_", {R"((\.laneTopology\.numLaneSegments)$)"}, !optional},

      {"laneTopology_laneSegments_", {R"((\.laneTopology\.laneSegments)\[\d\])"}, !optional},

      {"line_start_index_", {R"((\.centerline\.startIndex)$)"}, !optional},

      {"line_end_index_", {R"((\.centerline\.endIndex)$)"}, !optional},

      {"id_", {R"((\.id)$)"}, !optional},

      {"left_lane_", {R"((\.leftLane)$)"}, !optional},

      {"right_lane_", {R"((\.rightLane)$)"}, !optional},

      {"num_left_boundary_", {R"((\.numLeftBoundaryParts)$)"}, !optional},

      {"num_right_boundary_", {R"((\.numRightBoundaryParts)$)"}, !optional},

      {"leftBoundaryParts_", {R"((\.leftBoundaryParts)\[\d\])"}, !optional},

      {"rightBoundaryParts_", {R"((\.rightBoundaryParts)\[\d\])"}, !optional},

      {"num_points_", {R"((\.numPoints)$)"}, !optional},

      {"points_", {R"((\.points)\[\d\])"}, !optional},

      {"point_x_", {R"((\.x)$)"}, !optional},
      {"point_y_", {R"((\.y)$)"}, !optional},
      {"num_linear_obj_", {R"((\.numLinearObjects)$)"}, !optional},
      {"linearObjects_", {R"((\.linearObjects)\[\d\])"}, !optional},
      {"geo_start_index_", {R"((\.geometry\.startIndex)$)"}, !optional},
      {"geo_end_index_", {R"((\.geometry\.endIndex)$)"}, !optional},
  };

  for (const auto &[key, patterns, isOptional] : requests) {
    search_requests_.emplace_back(SearchRequest{key, patterns, isOptional});
  }

  if (!plugin_regex_searcher_) {
    return;
  }

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

void VisuRoadModel::SetRegexValuesInConfig() {
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
                     search_keywords_);

    for (const auto &search_request : search_requests_) {
      setRegexInConfig(next::appsupport::configkey::databridge::getPluginRegexConfigEntryKey(
                           getName(), search_request.data_type_name),
                       search_request.search_expression);
    }
  }
}

void VisuRoadModel::UpdateRegexValuesFromConfig() {
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

  search_keywords_ = config_client->getStringList(
      next::appsupport::configkey::databridge::getPluginRegexConfigEntryKey(getName(), "keywords"), {});

  for (auto &search_request : search_requests_) {
    search_request.search_expression = config_client->getStringList(
        next::appsupport::configkey::databridge::getPluginRegexConfigEntryKey(getName(), search_request.data_type_name),
        {});
  }

  plugin_regex_searcher_->InitializeSearchRequests(search_requests_);
}

std::vector<VisuRoadModel::SearchResult> VisuRoadModel::FindResultsByUrl(const std::string &url) {
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
    for (auto const &keyword : search_keywords_) {
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

bool VisuRoadModel::GetNamingMap(const std::string &url, std::unordered_map<std::string, std::string> &naming_map) {
  const auto results = FindResultsByUrl(url);

  if (results.size() != 1)
    return false;

  try {
    naming_map["ego_lane_"] = results[0].data_type_to_result.at("ego_lane_").result_url;
    naming_map["ego_position_"] = results[0].data_type_to_result.at("ego_position_").result_url;
    naming_map["num_connections_"] = results[0].data_type_to_result.at("num_connections_").result_url;
    naming_map["num_lane_segments_"] = results[0].data_type_to_result.at("num_lane_segments_").result_url;
    naming_map["laneTopology_laneSegments_"] =
        results[0].data_type_to_result.at("laneTopology_laneSegments_").result_url;
    naming_map["line_start_index_"] = results[0].data_type_to_result.at("line_start_index_").result_url;
    naming_map["line_end_index_"] = results[0].data_type_to_result.at("line_end_index_").result_url;
    naming_map["id_"] = results[0].data_type_to_result.at("id_").result_url;
    naming_map["left_lane_"] = results[0].data_type_to_result.at("left_lane_").result_url;
    naming_map["right_lane_"] = results[0].data_type_to_result.at("right_lane_").result_url;
    naming_map["num_left_boundary_"] = results[0].data_type_to_result.at("num_left_boundary_").result_url;
    naming_map["num_right_boundary_"] = results[0].data_type_to_result.at("num_right_boundary_").result_url;
    naming_map["leftBoundaryParts_"] = results[0].data_type_to_result.at("leftBoundaryParts_").result_url;
    naming_map["rightBoundaryParts_"] = results[0].data_type_to_result.at("rightBoundaryParts_").result_url;
    naming_map["num_points_"] = results[0].data_type_to_result.at("num_points_").result_url;
    naming_map["points_"] = results[0].data_type_to_result.at("points_").result_url;
    naming_map["point_x_"] = results[0].data_type_to_result.at("point_x_").result_url;
    naming_map["point_y_"] = results[0].data_type_to_result.at("point_y_").result_url;
    naming_map["num_linear_obj_"] = results[0].data_type_to_result.at("num_linear_obj_").result_url;
    naming_map["linearObjects_"] = results[0].data_type_to_result.at("linearObjects_").result_url;
    naming_map["geo_start_index_"] = results[0].data_type_to_result.at("geo_start_index_").result_url;
    naming_map["geo_end_index_"] = results[0].data_type_to_result.at("geo_end_index_").result_url;
  } catch (std::out_of_range &e) {
    debug(__FILE__, "Failed to create naming map, key not available in map: {0}", e.what());
    return false;
  }

  return true;
}
std::vector<VisuRoadModel::SearchResult>
VisuRoadModel::GetSearchResults(const std::string &url, const std::unordered_set<std::string> &unique_urls) {
  if (!plugin_regex_searcher_) {
    return {};
  }

  if (url.empty())
    return plugin_regex_searcher_->GetSearchResults(unique_urls);
  else
    return {plugin_regex_searcher_->GetSearchResultForUrl(url, unique_urls)};
}

VisuRoadModel::~VisuRoadModel() {}

void VisuRoadModel::sendFlatBufferDataToParentPlugin(const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                                     const std::pair<std::string, std::string> &structure_version_pair,
                                                     const bridgesdk::plugin::SensorInfoForFbs &sensor_info) {

  plugin_publisher_->sendFlatbufferData(fbs_path_, flatbuffer, sensor_info, structure_version_pair);
}

void VisuRoadModel::ProcessDataCallback([[maybe_unused]] const std::string url_name,
                                        const next::bridgesdk::ChannelMessagePointers *datablock,
                                        const next::plugins_utils::SensorConfig sensor_config,
                                        std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                        RoadModelMaxSize roadModelMaxSize) {

#ifdef TEST_DATA
  // create general_info_file_dump file and thenrecreate general_info_ back
  next::plugins_utils::createGeneralInfoFile(general_signals_info_, "general_info_file_dump.txt");
  bridgesdk::plugin_addons::Signals l_general_signals_info;

  next::plugins_utils::createGeneralInfoOutofDumpedFile("general_info_file_dump.txt", l_general_signals_info);

  // create dump file from payload and recreate payload from dumpedfile
  next::plugins_utils::dumpPayLoadAsFile(datablock, "payload_data_file_dump.txt");
  std::vector<int> res;
  next::plugins_utils::createVectorOutOfDumpedPayloadFile("payload_data_file_dump.txt", res);

#endif

  std::lock_guard<std::mutex> lock(base_roadmodel_protector_);
  base_roadmodel->SetAddress((char *)datablock->data, datablock->size);

  RoadModelFlatBufferConverter roadmodel_flatbuffer_converter(*this);
  roadmodel_flatbuffer_converter.convertRawDataToRoadModelAndSend(sensor_config, base_roadmodel, roadModelMaxSize,
                                                                  datablock->time);
}

bool VisuRoadModel::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  const auto search_results = FindResultsByUrl("");
  if (!search_results.empty()) {
    for (const auto &result : search_results) {
      config_fields.insert({"url", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, result.root_url}});
    }
  } else {
    debug(__FILE__, "No road data found for keywords");
    return false;
  }

  // config_fields.insert({"url", {next::bridgesdk::ConfigType::EDITABLE, "ADC4xx.FDP_Base.p_RmfOutputIf"}});
  config_fields.insert({"sensor", {next::bridgesdk::ConfigType::EDITABLE, "FDP base road model measfreeze"}});
  config_fields.insert({"datacache", {next::bridgesdk::ConfigType::EDITABLE, "Dummy1"}});
  return true;
}

bool VisuRoadModel::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool VisuRoadModel::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {
  config_ = config;
  std::string url, sensor, datacache, dataview;

  if (!GetValueFromPluginConfig(config, "url", url)) {
    return false;
  }

  if (!GetValueFromPluginConfig(config, "sensor", sensor)) {
    return false;
  }

  if (!GetValueFromPluginConfig(config, "datacache", datacache)) {
    return false;
  }

  if (!GetValueFromPluginConfig(config, "DATA_VIEW", dataview)) {
    return false;
  }

  std::unordered_map<std::string, std::string> naming_map{};
  if (!GetNamingMap(url, naming_map)) {
    return false;
  }

  next::plugins_utils::SensorConfig sensor_config;
  sensor_config.data_cache_name = datacache;
  sensor_config.data_view_name = dataview;
  sensor_config.sensor_url = url;
  sensor_config.sensor_name = sensor;

  debug(__FILE__, "Starting up view3D_Roadmodel for {0},{1},{2},{3}", sensor_config.sensor_url,
        sensor_config.sensor_name, sensor_config.data_view_name, sensor_config.data_cache_name);

  auto topics = plugin_signal_explorer_->getTopicsByUrl(sensor_config.sensor_url);

  if (topics.size() != 1 || topics[0].size() == 0) {
    warn(__FILE__, " view3D_Roadmodel could not find valid topic for URL: {0}", sensor_config.sensor_url);
    return false;
  }

  bridgesdk::plugin::SensorInfoForGUI sensor_info_gui;
  sensor_info_gui.name = sensor_config.sensor_name;
  sensor_info_gui.color = "#ff0000";
  sensor_info_gui.stream = sensor_config.data_view_name;
  sensor_info_gui.type = "line";
  sensor_info_gui.flatbuffer_bfbs_path = bfbs_path_;
  sensor_info_gui.schemaName = "GuiSchema.GroundLineList";
  sensor_config.sensor_id = plugin_3D_view_->registerAndPublishSensor(sensor_info_gui);

  if (sensor_config.sensor_id <= 0) {
    warn(__FILE__, "{0}::addConfig:: registerAndPublishSensor to sensor name: {1} failed", getName(),
         sensor_info_gui.name);
    return false;
  }

  // store sensor for this config
  auto hash_key = CreateKeyOutOfPluginConfig(config);
  if (active_config_to_sensors_.find(hash_key) != active_config_to_sensors_.end()) {
    warn(__FILE__, "{0}:: Key already exists and is overwritten", getName());
  }
  active_config_to_sensors_[hash_key].sensor_config_ = sensor_config;

  RoadModelMaxSize roadModelMaxSize;
  RoadModel roadModel;
  std::string url_points = sensor_config.sensor_url + naming_map.at("points_");
  if (!GetArraySizeStruct(url_points, roadModelMaxSize.max_size_points_)) {
    removeConfig(config);
    return false;
  }
  uint32_t points_road_model = (uint32_t)(sizeof(roadModel.points) / sizeof(roadModel.points[0]));
  if (roadModelMaxSize.max_size_points_ > points_road_model) {
    warn(__FILE__,
         "{0}:: Mismatch in size of points between data description ({1}) and defined ones size in roadModel.points "
         "({2}). Only the first {2} points are used.",
         getName(), roadModelMaxSize.max_size_points_, points_road_model);
    // reduce max size to the max size which can be used for storing the data
    roadModelMaxSize.max_size_points_ = points_road_model;
  }

  std::string url_linear_obj = sensor_config.sensor_url + naming_map.at("linearObjects_");
  if (!GetArraySizeStruct(url_linear_obj, roadModelMaxSize.max_size_linearObj_)) {
    removeConfig(config);
    return false;
  }
  uint32_t linearObj_road_model = (uint32_t)(sizeof(roadModel.linearObjects) / sizeof(roadModel.linearObjects[0]));
  if (roadModelMaxSize.max_size_linearObj_ > linearObj_road_model) {
    warn(__FILE__,
         "{0}:: Mismatch in size of linearObjects between data description ({1}) and defined ones size in "
         "roadModel.linearObjects ({2}). Only the first {2} points are used.",
         getName(), roadModelMaxSize.max_size_linearObj_, linearObj_road_model);
    roadModelMaxSize.max_size_linearObj_ = linearObj_road_model;
  }

  std::string url_laneTopology_laneSegments = sensor_config.sensor_url + naming_map.at("laneTopology_laneSegments_");
  if (!GetArraySizeStruct(url_laneTopology_laneSegments, roadModelMaxSize.max_size_laneSegments_)) {
    removeConfig(config);
    return false;
  }
  uint32_t laneSegments_laneTopology =
      (uint32_t)(sizeof(roadModel.laneTopology.laneSegments) / sizeof(roadModel.laneTopology.laneSegments[0]));
  if (roadModelMaxSize.max_size_laneSegments_ > laneSegments_laneTopology) {
    warn(__FILE__,
         "{0}:: Mismatch in size of laneSegments between data description ({1}) and defined ones size in "
         "roadModel.max_size_laneSegments_ ({2}). Only the first {2} points are used.",
         getName(), roadModelMaxSize.max_size_laneSegments_, laneSegments_laneTopology);
    roadModelMaxSize.max_size_laneSegments_ = laneSegments_laneTopology;
  }

  std::string url_BoundaryParts_left = sensor_config.sensor_url + naming_map.at("laneTopology_laneSegments_") + "[0]" +
                                       naming_map.at("leftBoundaryParts_");
  if (!GetArraySizeStruct(url_BoundaryParts_left, roadModelMaxSize.max_size_laneBoundary.left_)) {
    removeConfig(config);
    return false;
  }
  uint32_t leftBoundaryParts_laneSegments =
      (uint32_t)(sizeof(roadModel.laneTopology.laneSegments[0].leftBoundaryParts) /
                 sizeof(roadModel.laneTopology.laneSegments[0].leftBoundaryParts[0]));
  if (roadModelMaxSize.max_size_laneBoundary.left_ > leftBoundaryParts_laneSegments) {
    warn(__FILE__,
         "{0}:: Mismatch in size of left lane segment BoundaryParts between data description ({1}) and defined ones "
         "size in roadModelMaxSize.max_size_laneBoundary.left_ ({2}). Only the first {2} points are used.",
         getName(), roadModelMaxSize.max_size_laneBoundary.left_, leftBoundaryParts_laneSegments);
    roadModelMaxSize.max_size_laneBoundary.left_ = leftBoundaryParts_laneSegments;
  }

  std::string url_BoundaryParts_right = sensor_config.sensor_url + naming_map.at("laneTopology_laneSegments_") + "[0]" +
                                        naming_map.at("rightBoundaryParts_");
  if (!GetArraySizeStruct(url_BoundaryParts_right, roadModelMaxSize.max_size_laneBoundary.right_)) {
    removeConfig(config);
    return false;
  }
  uint32_t rightBoundaryParts_laneSegments =
      (uint32_t)(sizeof(roadModel.laneTopology.laneSegments[0].rightBoundaryParts) /
                 sizeof(roadModel.laneTopology.laneSegments[0].rightBoundaryParts[0]));
  if (roadModelMaxSize.max_size_laneBoundary.right_ > rightBoundaryParts_laneSegments) {
    warn(__FILE__,
         "{0}:: Mismatch in size of right lane segment BoundaryParts between data description ({1}) and defined ones "
         "size in roadModelMaxSize.max_size_laneBoundary.right_ ({2}). Only the first {2} points are used.",
         getName(), roadModelMaxSize.max_size_laneBoundary.right_, rightBoundaryParts_laneSegments);
    roadModelMaxSize.max_size_laneBoundary.right_ = rightBoundaryParts_laneSegments;
  }

  std::shared_ptr<BaseRoadModelList> base_object_list =
      std::make_shared<BaseRoadModelList>(next::udex::extractor::emptyRequestBasic, roadModelMaxSize);
  active_config_to_sensors_[hash_key].base_road_model_list_ = base_object_list;

  base_object_list->urls_ = naming_map;
  if (!base_object_list->SetUrls(sensor_config.sensor_url)) {
    warn(__FILE__, "{0}::addConfig::SetUrls of struct extractor BaseRoadModelList failed.", getName());
    removeConfig(config);
    return false;
  }

  auto binded_callback = std::bind(&VisuRoadModel::ProcessDataCallback, this, std::placeholders::_1,
                                   std::placeholders::_2, sensor_config, base_object_list, roadModelMaxSize);

  auto subs_id = plugin_data_subscriber_->subscribeUrl(topics[0], bridgesdk::SubscriptionType::DIRECT, binded_callback);

  if (subs_id <= 0) {
    warn(__FILE__, "{0}::addConfig:: subscription to url: {1} failed", getName(), topics[0]);
    removeConfig(config);
    return false;
  }

  sensor_config.subscription_id.push_back(subs_id);
  active_config_to_sensors_[hash_key].sensor_config_ = sensor_config;
  debug(__FILE__, "{0}::addConfig:: subscription to url: {1} is successful", getName(), topics[0]);
  return true;
}

bool VisuRoadModel::removeConfig(const next::bridgesdk::plugin_config_t &config) {

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

  if (nullptr != config_data.base_road_model_list_) {
    std::lock_guard<std::mutex> lock(base_roadmodel_protector_);
    config_data.base_road_model_list_.reset();
  }

  active_config_to_sensors_.erase(hash_key);

  return true;
}

bool VisuRoadModel::enterReset() { return true; }

bool VisuRoadModel::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool VisuRoadModel::GetArraySizeStruct(const std::string &url, size_t &array_size) const {
  bridgesdk::plugin_addons::SignalInfo sig_array_size = plugin_signal_extractor_->GetInfo(url);
  if (0 == sig_array_size.signal_size) {
    warn(__FILE__, " fdp base road could not find fdp base URL:{0}", url);
    return false;
  }
  array_size = sig_array_size.array_size;
  return true;
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::VisuRoadModel(path); }
