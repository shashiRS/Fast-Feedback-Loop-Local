/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     canTpObjects.h
 * @brief    Parsing CAN data, providing the objects
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_CAN_TPOBJECTS_H_
#define NEXT_PLUGINS_CAN_TPOBJECTS_H_

#include <json/json.h>
#include <memory>
#include <string>
#include <unordered_map>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/schema/3d_view/trafficparticipant_generated.h>
#include <next/plugins_utils/plugins_types.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_regex_searcher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/plugins_utils/plugin_3d_view_utils.hpp>

#include "canTpObjects_struct.hpp"

namespace next {
namespace plugins {
constexpr const char *default_color{"#FF6600"};
constexpr const char *default_sensor{"ARS510"};
constexpr const float default_opacity{0.5f};

class CanTpObjectsPlugin : public next::bridgesdk::plugin::Plugin {
public:
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;

  CanTpObjectsPlugin(const char *);
  virtual ~CanTpObjectsPlugin();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;
  /*! provides plugin description to allow for selection in GUI
   *
   * @param config selection of keys and values
   * @return true if successfull
   */
  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config);

  void ProcessDataCallback(const std::string url_name, const next::bridgesdk::ChannelMessagePointers *datablock,
                           const next::plugins_utils::SensorConfig sensor_config,
                           std::shared_ptr<CanTpObjects> &can_tp_objects);
  std::vector<SearchResult> GetSearchResults(const std::string &url,
                                             const std::unordered_set<std::string> &unique_urls) const;

private:
  bool init_flag = false;
  std::unordered_map<std::string, next::plugins_utils::SensorConfig> active_config_to_sensors_;

  bool cycle_finished_ = false;
  int cur_msg_counter_ = -1;
  Json::Value object_list_;

  mutable std::mutex config_regex_protector_;

  Json::StreamWriterBuilder builder_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::string fbs_path_;
  std::string bfbs_path_;

  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};

  std::vector<std::string> keywords_;
  std::vector<SearchRequest> search_requests_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginRegexSearcher> plugin_regex_searcher_;

  bool AddConfigInternal(const next::bridgesdk::plugin_config_t &config);
  std::unordered_map<std::string, std::string> GetNamingMap(const std::string &url);
  void InitSearchRequests();
  std::vector<SearchResult> FindResultsByUrl(const std::string &url);
  flatbuffers::Offset<GuiSchema::TrafficParticipant>
  CreateTrafficParticipant(flatbuffers::FlatBufferBuilder &builder, const std::string &sensor_name,
                           std::shared_ptr<TrafficObject> traffic_object);
  void SetRegexValuesInConfig();
  bool GetAvailableTPLists(const std::string &url, std::vector<SearchResult> &search_results);
  void UpdateRegexValuesFromConfig();
};
} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_CAN_TPOBJECTS_H_
