/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     raw_frame_view.cpp
 * @brief    raw frame view plugin.
 *
 * See raw_frame_view.h
 *
 **/
#include <fstream>
#include <future>

#include <json/json.h>

#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <next/plugins_utils/helper_functions.h>

#include "base64.h"
#include "raw_frame_view.h"

namespace next {
namespace plugins {
using namespace next::plugins_utils;

RawFrameView::RawFrameView(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello system_RawFrameView plugin");
  this->setVersion(std::string("0.0.1"));
  this->setName(std::string("system_RawFrameView"));
  ini_file_path_ = std::string(path) + "/raw_frame_plugin.ini";
}

RawFrameView::~RawFrameView() {}

void RawFrameView::Callback(std::string const topic, const bridgesdk::ChannelMessagePointers *data,
                            std::string video_stream_name, image_format_type image_type,
                            const image_rotation_type image_rotation) {
  uint8_t *data_array = (uint8_t *)data->data;
  auto offset_info_iterator = offset_info_.find(topic);

  if (offset_info_iterator == offset_info_.end()) {

    // Get the image height signal offset.
    auto return_signal_isolated = plugin_signal_extractor_->GetInfo(topic + ".sImgHeader.uiHeight");
    auto image_height_offset = return_signal_isolated.offset;

    // Get the image width signal offset.
    return_signal_isolated = plugin_signal_extractor_->GetInfo(topic + ".sImgHeader.uiWidth");
    auto image_width_offset = return_signal_isolated.offset;

    // Get the image data offset signal offset.
    return_signal_isolated = plugin_signal_extractor_->GetInfo(topic + ".imageDataOffset");
    auto image_data_offset = return_signal_isolated.offset;

    // Get the image data signal offset.
    return_signal_isolated = plugin_signal_extractor_->GetInfo(topic + ".aImageData");
    auto image_offset = return_signal_isolated.offset;

    signal_offset offset = {};
    offset.image_height = image_height_offset;
    offset.image_width = image_width_offset;
    offset.image_data_offset = image_data_offset;
    offset.image_data = image_offset;

    auto result = offset_info_.try_emplace(topic, offset);

    if (result.second) {
      offset_info_iterator = result.first;
    } else {
      return;
    }
  }

  // Validate the data size required to get the raw image.
  if (data->size < offset_info_iterator->second.image_data) {
    //    warn(__FILE__, "Size overflow! Raw image size: {0}, Data size: {1}", offset_info_iterator->second.image_data,
    //         data->size);
    return;
  }

  // Get the image parameters.
  uint16_t image_height = ((uint16_t *)(data_array + offset_info_iterator->second.image_height))[0];
  uint16_t image_width = ((uint16_t *)(data_array + offset_info_iterator->second.image_width))[0];
  uint16_t image_offset = ((uint16_t *)(data_array + offset_info_iterator->second.image_data_offset))[0];
  uint8_t *raw_image = data_array + image_offset;

  uint32_t in_size = (uint32_t)(image_height * image_width) * (uint32_t)2u + (uint32_t)image_offset;
  if (in_size > uint32_t(data->size)) {
    //    warn(__FILE__, "The image size is {} bytes, but as input only {} bytes are provided.", in_size, data->size);
    return;
  }

  cv::Mat imageBayer16Bits(image_height, image_width, CV_16UC1, raw_image);
  imageBayer16Bits.convertTo(imageBayer16Bits, CV_8UC1, 0.0625);
  if (image_rotation == image_rotation_type::Rot_180deg) {
    // rotate image by 180 degrees
    cv::flip(imageBayer16Bits, imageBayer16Bits, -1);
  }
  std::vector<uchar> image_buffer;
  if (image_type == image_format_type::Grayscale_8bit) {
    // Gray scale 8 bits
    cv::Mat imageGray8Bits(image_height, image_width, CV_8UC1);
    cv::cvtColor(imageBayer16Bits, imageGray8Bits, CV_BayerBG2GRAY);

    // Convert to JPEG.
    cv::imencode(".jpeg", imageGray8Bits, image_buffer);
  } else if (image_type == image_format_type::RGB_24bit) {
    // RGB24
    cv::Mat imageRGB24Bits(image_height, image_width, CV_8UC3);
    cv::cvtColor(imageBayer16Bits, imageRGB24Bits, cv::COLOR_BayerGB2BGR);

    // Convert to JPEG.
    cv::imencode(".jpeg", imageRGB24Bits, image_buffer);
  } else {
    //    warn(__FILE__, "Invalid image format: {0}", static_cast<int>(image_type));
    return;
  }
  Json::Value video_stream;
  video_stream["type"] = "jpeg";
  video_stream["data"] = base64_encode(image_buffer.data(), image_buffer.size());

  Json::Value final_message_json;
  final_message_json["dataStream"][video_stream_name] = video_stream;
  final_message_json["dataStream"]["timestamp"] = static_cast<Json::Value::UInt64>(data->time);
  Json::FastWriter writer;
  std::string response = writer.write(final_message_json);
  plugin_publisher_->sendResultData((uint8_t *)response.data(), response.size());
}

bool RawFrameView::init() {
  //  debug(__FILE__, "Init : {0}", __FILE__);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  return true;
}

bool RawFrameView::enterReset() {
  // so far we are doing nothing here
  return true;
}

bool RawFrameView::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool RawFrameView::parse_ini_file(std::string filepath) const {
  bool config_file_read = false;
  device_url_map_.clear();
  try {
    std::ifstream iniFile(filepath);
    if (iniFile.is_open()) {
      std::string line;
      bool trigger_section = false;
      while (std::getline(iniFile, line)) {
        std::size_t trigger_section_reached = line.find("[url]"); // searching correct area in file
        if (trigger_section_reached != std::string::npos)
          trigger_section = true;

        if (trigger_section) {
          // Search for an = as indicator that it's a key-value pair line
          std::size_t found = line.find("=");
          if (found != std::string::npos) {
            device_url_map_.insert(
                std::pair(line.substr(0, found), line.substr(found + 1, line.length() - (found + 1))));
          }
        }
      }
      iniFile.close();
      config_file_read = true;
    }
  } catch ([[maybe_unused]] std::exception &e) {

    //    warn(__FILE__, "Error while reading file: {0}. Error: {1}", filepath, e.what());
  }
  return config_file_read;
}

bool RawFrameView::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool RawFrameView::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {

  std::string image_source, data_view, image_type, image_rotation;

  config_ = config;

  if (GetValueFromPluginConfig(config, "Image Source", image_source) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "DATA_VIEW", data_view) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "Image Format", image_type) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "Image Rotation", image_rotation) != true) {
    return false;
  }

  const auto image_source_enum = image_source_to_enum_.at(image_source);
  const auto image_type_enum = image_format_to_enum_.at(image_type);
  const auto image_rotation_enum = image_rotation_to_enum_.at(image_rotation);

  // Check if data view already exists for new config request.
  auto subscription_info_iterator = subscription_info_.find(data_view);
  if (subscription_info_iterator != subscription_info_.end()) {
    if (subscription_info_iterator->second.image_source == image_source_enum &&
        subscription_info_iterator->second.image_format == image_type_enum &&
        subscription_info_iterator->second.image_rotation == image_rotation_enum) {
      // New config request is similar to existing one. No further action required.
      return true;
    } else {
      // Unique config request received for same data view.
      // Hence, unsubscribe the existing one and go ahead for fresh subscription.
      plugin_data_subscriber_->unsubscribe(subscription_info_iterator->second.subscription_id);
      subscription_info_.erase(data_view);
    }
  }

  if (device_url_map_.empty()) {
    if (!parse_ini_file(ini_file_path_)) {
      //      warn(__FILE__, " RawFrameViewer error in parsing file: {0}", ini_file_path_);
    }
  }
  std::string image_source_url;
  auto device_url_map_iterator = device_url_map_.find(image_source);
  if (device_url_map_iterator != device_url_map_.end()) {
    image_source_url = device_url_map_iterator->second;
  } else {
    //    warn(__FILE__, " RawFrameViewer could not find URL for image source: {0}", image_source);
    return false;
  }

  auto topics = plugin_signal_explorer_->getTopicsByUrl(image_source_url);
  if (topics.size() != 1 || topics[0].size() == 0) {
    //    warn(__FILE__, " RawFrameViewer could not find valid topic for URL: {0}", image_source_url);
    return false;
  }

  auto binded_callback = std::bind(&RawFrameView::Callback, this, std::placeholders::_1, std::placeholders::_2,
                                   data_view, image_format_to_enum_.at(image_type), image_rotation_enum);

  // Subscribe URL.
  auto subscription_id =
      plugin_data_subscriber_->subscribeUrl(topics[0], bridgesdk::SubscriptionType::DIRECT, binded_callback);

  if (subscription_id <= 0) {
    //    warn(__FILE__, "RawFrameView::addConfig::subscription to url: {0} failed", topics[0]);
    return false;
  }

  //  debug(__FILE__, "RawFrameView::addConfig::subscription to url: {0} is successful", topics[0]);

  sensor_config sensor_config_info = {};
  sensor_config_info.image_format = image_type_enum;
  sensor_config_info.image_source = image_source_enum;
  sensor_config_info.image_rotation = image_rotation_enum;
  sensor_config_info.subscription_id = subscription_id;

  subscription_info_.insert(std::make_pair(data_view, sensor_config_info));

  init_succesfull_ = true;

  return true;
}

bool RawFrameView::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  if (!parse_ini_file(ini_file_path_)) {
    //    warn(__FILE__, "Error in parsing ini file: {0}", ini_file_path_);
    return false;
  }

  // Add image source.
  for (auto device_url_map_iterator = device_url_map_.begin(); device_url_map_iterator != device_url_map_.end();
       device_url_map_iterator++) {
    std::string data_view_name = device_url_map_iterator->first;
    config_fields.insert({"Image Source", {next::bridgesdk::ConfigType::SELECTABLE, data_view_name}});
  }

  // Add image format.
  for (auto image_format_to_enum_iterator = image_format_to_enum_.begin();
       image_format_to_enum_iterator != image_format_to_enum_.end(); image_format_to_enum_iterator++) {
    std::string image_format = image_format_to_enum_iterator->first;
    config_fields.insert({"Image Format", {next::bridgesdk::ConfigType::SELECTABLE, image_format}});
  }

  // Add image rotation
  for (const auto image_rotation_map_entry : image_rotation_to_enum_) {
    config_fields.insert(
        {"Image Rotation", {next::bridgesdk::ConfigType::SELECTABLE_FIXED, image_rotation_map_entry.first}});
  }

  return true;
}

bool RawFrameView::removeConfig(const next::bridgesdk::plugin_config_t &config) {
  std::string image_source, data_view, image_type, image_rotation;

  if (GetValueFromPluginConfig(config, "Image Source", image_source) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "DATA_VIEW", data_view) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "Image Format", image_type) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "Image Rotation", image_rotation) != true) {
    return false;
  }

  const auto image_source_enum = image_source_to_enum_.at(image_source);
  const auto image_type_enum = image_format_to_enum_.at(image_type);
  const auto image_rotation_enum = image_rotation_to_enum_.at(image_rotation);

  // Check if received config request matches with the existing configurations.
  auto subscription_info_iterator = subscription_info_.find(data_view);
  if (subscription_info_iterator != subscription_info_.end()) {
    if (subscription_info_iterator->second.image_source == image_source_enum &&
        subscription_info_iterator->second.image_format == image_type_enum &&
        subscription_info_iterator->second.image_rotation == image_rotation_enum) {
      // Configuration exists. Hence, unsubscribe and remove.
      plugin_data_subscriber_->unsubscribe(subscription_info_iterator->second.subscription_id);
      subscription_info_.erase(data_view);
      return true;
    } else {
      //      warn(__FILE__, "Invalid configuration for data view: {0}", data_view);
      return false;
    }
  }

  //  warn(__FILE__, "No configuration found for data view: {0}", data_view);
  return false;
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::RawFrameView(path); }
