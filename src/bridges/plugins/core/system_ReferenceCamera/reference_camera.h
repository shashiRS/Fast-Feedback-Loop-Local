/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     reference_camera.h
 * @brief    Reference Camera Plugin
 *
 * This plugin is responsible for providing images from reference camera data sources.
 *
 **/

#ifndef NEXT_PLUGINS_REFERENCE_CAMERA_PLUGIN_H_
#define NEXT_PLUGINS_REFERENCE_CAMERA_PLUGIN_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

namespace next {
namespace plugins {

class ReferenceCamera : public next::bridgesdk::plugin::Plugin {
public:
  ReferenceCamera(const char *path);
  virtual ~ReferenceCamera();

  bool init() override;

  bool enterReset() override;
  bool exitReset() override;

  void Callback(std::string const topic, const bridgesdk::ChannelMessagePointers *data, std::string video_stream_name);
  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

private:
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);

  mutable std::vector<std::string> reference_camera_device_names_;
  std::unordered_map<std::string /*url*/, std::tuple<size_t /*offset for format*/, size_t /*offset for size*/,
                                                     size_t /*offset for raw image*/>>
      offset_info_;

  // Pair of subscription id and source.
  using subscription_id = std::pair<size_t, std::string>;

  // Map with data view as key and subscription id as value.
  std::unordered_map<std::string, subscription_id> subscription_info_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};
};

} // namespace plugins
} // namespace next
//
extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_REFERENCE_CAMERA_PLUGIN_H_
