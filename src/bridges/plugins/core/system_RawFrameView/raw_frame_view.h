/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     raw_frame_view.h
 * @brief    Raw Frame View Plug-in
 *
 * This plug-in gets a tree structure of the signals available from the back end
 * and puts it into a JSON.
 *
 **/

#ifndef NEXT_PLUGINS_RAW_FRAME_VIEW_PLUGIN_H_
#define NEXT_PLUGINS_RAW_FRAME_VIEW_PLUGIN_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <opencv2/core/mat.hpp>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

namespace next {
namespace plugins {

class RawFrameView : public next::bridgesdk::plugin::Plugin {
public:
  RawFrameView(const char *path);
  virtual ~RawFrameView();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;

  enum class image_format_type : uint8_t { RGB_24bit = 0, Grayscale_8bit = 1 };
  enum class image_rotation_type : uint8_t { Rot_0deg = 0, Rot_180deg = 1 };

  void Callback(std::string const topic, const bridgesdk::ChannelMessagePointers *data, std::string video_stream_name,
                image_format_type image_type, const image_rotation_type image_rotation);

  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

private:
  bool parse_ini_file(std::string filepath) const;
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);
  mutable std::unordered_map<std::string, std::string> device_url_map_;
  next::bridgesdk::plugin_addons::Signals return_signal_isolated_;

  enum class image_source_type : uint8_t { CAM_FC = 0, CAM_RC = 1, CAM_LSC = 2, CAM_RSC = 3 };
  std::unordered_map<std::string, image_source_type> const image_source_to_enum_ = {
      {"CAM_FC", image_source_type::CAM_FC},
      {"CAM_RC", image_source_type::CAM_RC},
      {"CAM_RSC", image_source_type::CAM_RSC},
      {"CAM_LSC", image_source_type::CAM_LSC}};

  std::unordered_map<std::string, image_format_type> const image_format_to_enum_ = {
      {"RGB_24bit", image_format_type::RGB_24bit}, {"Grayscale_8bit", image_format_type::Grayscale_8bit}};

  std::unordered_map<std::string, image_rotation_type> const image_rotation_to_enum_ = {
      {"0_deg", image_rotation_type::Rot_0deg}, {"180_deg", image_rotation_type::Rot_180deg}};

  struct sensor_config {
    image_source_type image_source;
    image_format_type image_format;
    image_rotation_type image_rotation;
    size_t subscription_id;
  };

  struct signal_offset {
    size_t image_height;
    size_t image_width;
    size_t image_data_offset;
    size_t image_data;
  };

  std::unordered_map<std::string /*Data view*/, sensor_config> subscription_info_;
  std::unordered_map<std::string /*Signal URL*/, signal_offset> offset_info_;
  std::string ini_file_path_;
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

#endif // NEXT_PLUGINS_RAW_FRAME_VIEW_PLUGIN_H_
