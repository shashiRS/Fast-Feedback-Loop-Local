/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     point_cloud_example.h
 * @brief    creating a point cloud with parameters and sending the data via flatbuffer
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_POINT_CLOUD_H_
#define NEXT_PLUGINS_POINT_CLOUD_H_
#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/schema/3d_view/pointcloud_generated.h>
#include <next/plugins_utils/plugins_types.h>

#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

namespace next {
namespace plugins {

class PointCloud : public next::bridgesdk::plugin::Plugin {

  struct ConfigData {
    next::plugins_utils::SensorConfig sensor_config_;
  };

public:
  PointCloud(const char *);
  virtual ~PointCloud();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;

  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) const;
  bool addConfig(const next::bridgesdk::plugin_config_t &config);
  bool removeConfig(const next::bridgesdk::plugin_config_t &config);

  void generateExamplePointCloud(uint16_t size, uint16_t point_density, time_t time,
                                 next::plugins_utils::SensorConfig sensor_config, std::string point_direction);

  void enableBackgroundRunning();

  void disableBackgroundRunning();
  void runExamplePointCloud(uint16_t size, uint16_t point_density, time_t time,
                            next::plugins_utils::SensorConfig sensor_config, uint16_t number_of_messages,
                            uint16_t delay_in_ms, std::string point_direction);

private:
  bool GetArraySizeStruct(std::string url, size_t &array_size) const;

  int counter;

  std::unordered_map<std::string, ConfigData> active_config_to_sensors_;
  std::mutex config_pc_protector_;
  std::mutex config_ego_vehicle_protector_;

  next::bridgesdk::plugin_addons::SignalInfo general_signals_info_;
  std::atomic<float> ego_y_distance_{0.F};
  std::atomic<float> ego_x_distance_{0.F};
  std::atomic<float> ego_yaw_angle_{0.F};
  std::atomic<float> ego_wheelbase_{0.F};
  std::string path_to_fbs_{};
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

  void stopExamplePointCloud();

  static std::atomic<bool> s_disconnect;
  std::shared_ptr<std::thread> background_thread_;
  std::atomic_bool background_running_ = false;
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_POINT_CLOUD_H_
