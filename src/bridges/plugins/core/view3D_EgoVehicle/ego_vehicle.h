/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     ego_vehicle.h
 * @brief    Parsing network data to provide ego vehicle
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_EGO_VEHICLE_H_
#define NEXT_PLUGINS_EGO_VEHICLE_H_

#include <boost/regex.hpp>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

#include <next/plugins_utils/parking_types.h>
#include <next/plugins_utils/plugins_types.h>

#include "ego_vehicle.hpp"

struct EgoVehicleSensorConfig {
  std::atomic<float> ego_y_distance;
  std::atomic<float> ego_x_distance;
  std::atomic<float> ego_yaw_angle;
  std::atomic<uint64_t> cache_frame_counter_;
  EgoVehicleSensorConfig() : ego_y_distance(0.0f), ego_x_distance(0.0f), ego_yaw_angle(0.0f), cache_frame_counter_(0) {}

  EgoVehicleSensorConfig &operator=(const EgoVehicleSensorConfig &other) {
    if (this == &other) {
      return *this;
    }

    this->ego_y_distance.store(other.ego_y_distance.load());
    this->ego_x_distance.store(other.ego_x_distance.load());
    this->ego_yaw_angle.store(other.ego_yaw_angle.load());
    this->cache_frame_counter_.store(other.cache_frame_counter_.load());

    return *this;
  }
};

namespace next {
namespace plugins {

class EgoVehiclePlugin : public next::bridgesdk::plugin::Plugin {
public:
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

  EgoVehiclePlugin(const char *);
  virtual ~EgoVehiclePlugin();

  std::vector<SearchResult> GetSearchResults(const std::string &url,
                                             const std::unordered_set<std::string> &unique_urls) const;

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;

  void ProcessDataCallbackOffset(const std::string topic_name, const next::bridgesdk::ChannelMessagePointers *datablock,
                                 std::map<std::string, std::string> parameters);
  void ProcessEgoVehicleCallback(const std::string topic_name, const next::bridgesdk::ChannelMessagePointers *datablock,
                                 const next::plugins_utils::SensorConfig sensor_config_ego_position,
                                 const next::plugins_utils::SensorConfig sensor_config_driver_trajectory,
                                 std::shared_ptr<EgoVehicle> ego_vehicle, uint64_t n_veh_update_rate);

  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;
  void createAndSendLine(float x_value, float y_val, next::plugins_utils::SensorConfig sensor_config,
                         uint64_t timestamp, uint64_t timestamp_ego);

private:
  std::vector<std::string> keywords_;
  mutable std::mutex config_regex_protector_;
  std::vector<SearchRequest> search_requests_;

  std::vector<SearchResult> FindResultsByUrl(const std::string &url);
  void InitSearchRequests();
  void UpdateRegexValuesFromConfig();
  void SetRegexValuesInConfig();
  bool GetNamingMap(const std::string &url, std::unordered_map<std::string, std::string> &naming_map);
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);
  void RemoveSubgroupResults(std::vector<SearchResult> &results, const std::unordered_set<std::string> &urls);

  enum class type_config { ADC5xx, CarPC, M7 };
  std::unordered_map<std::string, type_config> const type_config_to_enum_ = {
      {"ADC5xx_Device", type_config::ADC5xx}, {"CarPC", type_config::CarPC}, {"M7", type_config::M7}};

  bool isNumber(const std::string &str);

  std::unordered_map<std::string, next::plugins_utils::SensorConfig> active_config_to_sensors_trajectory_,
      active_config_to_sensors_ego_;
  std::string path_to_fbs_{};
  std::string path_to_bfbs_{};
  std::string path_to_fbs_line_{};
  std::string path_to_bfbs_line_{};
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginRegexSearcher> plugin_regex_searcher_;

  std::map<uint32_t, next::plugins_utils::parking::types::TrajectoryInfo> trajectory_info_;
  std::map<uint32_t, EgoVehicleSensorConfig> veh_sensor_config_;

  std::map<std::string, std::shared_ptr<EgoVehicle>> ego_vehicle_lists_;

  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};

  enum class offset_config { No_offset, Offset_by_value, Offset_by_URL };
  std::unordered_map<std::string, offset_config> const offset_config_to_enum_ = {
      {"No offset", offset_config::No_offset},
      {"Offset by value", offset_config::Offset_by_value},
      {"Offset by URL", offset_config::Offset_by_URL}};
  struct {
    std::atomic<offset_config> offset_cfg;
    std::atomic<float> offset_x{0};
    std::atomic<float> offset_y{0};
  } offset_;
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_EGO_VEHICLE_H_
