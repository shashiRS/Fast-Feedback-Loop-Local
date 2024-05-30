/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     point_cloud.h
 * @brief    Parsing network data of AEye LiDAR sensor, providing point clouds
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_POINT_CLOUD_H_
#define NEXT_PLUGINS_POINT_CLOUD_H_
#include <atomic>
#include <map>
#include <string>
#include <unordered_map>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/schema/3d_view/pointcloud_generated.h>
#include <next/plugins_utils/plugins_types.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_regex_searcher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include "ego_vehicle_struct.hpp"
#include "point_cloud_struct.hpp"

namespace next {
namespace plugins {

class PointCloud : public next::bridgesdk::plugin::Plugin {
  enum class CoordinateSystem : uint8_t { kGlobal = 0U, kEgoVehicleRearAxle = 1U, kEgoVehicleFrontAxle = 2U };

  struct ConfigData {
    std::shared_ptr<Cloud> point_cloud_{nullptr};
    std::shared_ptr<EgoVehicle> ego_vehicle_{nullptr};
    next::plugins_utils::SensorConfig sensor_config_;
  };

public:
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

  PointCloud(const char *);
  virtual ~PointCloud();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;

  void ProcessPointCloudCallback([[maybe_unused]] const std::string topic_name,
                                 const next::bridgesdk::ChannelMessagePointers *datablock,
                                 [[maybe_unused]] next::bridgesdk::plugin_config_t config,
                                 [[maybe_unused]] next::plugins_utils::SensorConfig sensor_config,
                                 std::shared_ptr<Cloud> point_cloud_struct, const std::string point_colour,
                                 const CoordinateSystem coordinate_system, bool spherical_coordinate_system);

  void ProcessDataCallbackEgoVehicle([[maybe_unused]] const std::string topic_name,
                                     const next::bridgesdk::ChannelMessagePointers *datablock,
                                     [[maybe_unused]] next::bridgesdk::plugin_config_t config,
                                     [[maybe_unused]] next::plugins_utils::SensorConfig sensor_config,
                                     std::shared_ptr<EgoVehicle> ego_vehicle);

  void ProcessWheelbaseCallback([[maybe_unused]] const std::string topic_name,
                                [[maybe_unused]] const next::bridgesdk::ChannelMessagePointers *datablock,
                                const std::string &url_wheelbase);

  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

  void setDependencies(std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> sig_publisher,
                       std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> sig_extractor,
                       std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> sig_subscriber,
                       std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> sig_explorer) {
    plugin_publisher_ = sig_publisher;
    plugin_signal_extractor_ = sig_extractor;
    plugin_data_subscriber_ = sig_subscriber;
    plugin_signal_explorer_ = sig_explorer;
  }

  void setSignalSubscriber(std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> sig_subscriber) {
    plugin_data_subscriber_ = sig_subscriber;
  }

  std::vector<SearchResult> GetSearchResults(const std::string &url,
                                             const std::unordered_set<std::string> &unique_urls) const;

private:
  std::unordered_map<std::string, CoordinateSystem> const coordinate_system_to_enum_ = {
      {"global", CoordinateSystem::kGlobal},
      {"ego_vehicle_rear_axle", CoordinateSystem::kEgoVehicleRearAxle},
      {"ego_vehicle_front_axle", CoordinateSystem::kEgoVehicleFrontAxle}};

  int counter;

  std::unordered_map<std::string, ConfigData> active_config_to_sensors_;
  std::mutex config_pc_protector_;
  std::mutex config_ego_vehicle_protector_;

  next::bridgesdk::plugin_addons::SignalInfo general_signals_info_;
  std::atomic<float> ego_y_distance_{0.F};
  std::atomic<float> ego_x_distance_{0.F};
  std::atomic<float> ego_yaw_angle_{0.F};
  std::atomic<float> ego_wheelbase_{0.F};
  std::string fbs_path_{};
  std::string bfbs_path_{};
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginRegexSearcher> plugin_regex_searcher_;

  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};

  mutable std::mutex config_regex_protector_;

  std::vector<std::string> keywords_;
  std::vector<SearchRequest> search_requests_;

  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);
  std::vector<SearchResult> FindResultsByUrl(const std::string &url);
  bool GetArraySizeStruct(const std::string &url, size_t &array_size) const;
  bool GetNamingMap(const std::string &url, std::map<std::string, std::string> &naming_map,
                    bool &spherical_coordinate_system);
  bool IsSphericalCoordinateSystem(const SearchResult &result);
  void InitSearchRequests();

  /**
   * @brief Sanitizes the coordinate results, removing non-Cartesian or non-spherical results.
   * @param results A vector of SearchResult objects representing the search results.
   * @return void
   */
  void SanitizeCoordinateResults(std::vector<SearchResult> &results);
  void SetRegexValuesInConfig();
  void UpdateRegexValuesFromConfig();
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_POINT_CLOUD_H_
