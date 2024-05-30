/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     roadmodel.h
 * @brief    Parsing network data to provide road model
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_ROAD_MODEL_H_
#define NEXT_PLUGINS_ROAD_MODEL_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_regex_searcher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/version_manager/version_manager.hpp>

#include <next/plugins_utils/plugins_types.h>
#include "base_roadmodel.hpp"
#include "url_mapper.h"

namespace next {
namespace plugins {

class VisuRoadModel : public next::bridgesdk::plugin::Plugin {
  struct ConfigData {
    std::shared_ptr<BaseRoadModelList> base_road_model_list_{nullptr};
    next::plugins_utils::SensorConfig sensor_config_{};
  };
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

public:
  VisuRoadModel(const char *);
  virtual ~VisuRoadModel();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;

  void ProcessDataCallback(const std::string url_name, const next::bridgesdk::ChannelMessagePointers *datablock,
                           const next::plugins_utils::SensorConfig sensor_config,
                           std::shared_ptr<BaseRoadModelList> base_roadmodel, RoadModelMaxSize roadModelMaxSize);

  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

  void sendFlatBufferDataToParentPlugin(const bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                        const std::pair<std::string, std::string> &structure_version_pair,
                                        const bridgesdk::plugin::SensorInfoForFbs &sensor_info);

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> getRefToExtractor() {
    return plugin_signal_extractor_;
  }

  std::vector<SearchResult> GetSearchResults(const std::string &url,
                                             const std::unordered_set<std::string> &unique_urls);

private:
  bool GetArraySizeStruct(const std::string &url, size_t &array_size) const;
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);

  std::unordered_map<std::string, ConfigData> active_config_to_sensors_;
  std::mutex base_roadmodel_protector_;

  std::string fbs_path_;
  std::string bfbs_path_;

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginRegexSearcher> plugin_regex_searcher_;

  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};

  std::vector<SearchRequest> search_requests_;
  std::vector<std::string> search_keywords_;
  std::mutex config_regex_protector_;

  void InitSearchRequests();
  void SetRegexValuesInConfig();
  void UpdateRegexValuesFromConfig();
  bool GetNamingMap(const std::string &url, std::unordered_map<std::string, std::string> &naming_map);
  std::vector<SearchResult> FindResultsByUrl(const std::string &url);
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_ROAD_MODEL_H_
