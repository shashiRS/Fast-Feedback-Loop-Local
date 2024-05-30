/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     reference_camera.cpp
 * @brief    Reference Camera Plugin
 *
 * See reference_camera.h.
 *
 **/
#include "reference_camera.h"
#include <json/json.h>
#include <next/plugins_utils/helper_functions.h>
#include "base64.h"
namespace next {
namespace plugins {
using namespace next::plugins_utils;

ReferenceCamera::ReferenceCamera(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello system_ReferenceCamera plugin");
  this->setVersion(std::string("0.0.1"));
  this->setName(std::string("system_ReferenceCamera"));
}

ReferenceCamera::~ReferenceCamera() {}

const std::string format_type[] = {"dib", "jpeg", "tiff", "bmp", "png", "gif", "pcx", "tga", "jpeglossless", "raw"};
bool ReferenceCamera::getDescription(next::bridgesdk::plugin_config_t &config_fields) {
  reference_camera_device_names_.clear();
  plugin_signal_explorer_->getDataSourceNames("mts.refcam", reference_camera_device_names_);
  for (auto &ref_camera_device : reference_camera_device_names_) {
    config_fields.insert({"source", {next::bridgesdk::ConfigType::SELECTABLE, ref_camera_device}});
  }
  return true;
}

bool ReferenceCamera::addConfig(const next::bridgesdk::plugin_config_t &config) {
  init_succesfull_ = addConfigInternal(config);
  return init_succesfull_;
}

bool ReferenceCamera::addConfigInternal(const next::bridgesdk::plugin_config_t &config) {
  std::string source, data_view_name;

  config_ = config;

  if (GetValueFromPluginConfig(config, "source", source) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "DATA_VIEW", data_view_name) != true) {
    return false;
  }

  // Check if data view already exists for new config request.
  auto subscription_info_iterator = subscription_info_.find(data_view_name);
  if (subscription_info_iterator != subscription_info_.end()) {
    if (subscription_info_iterator->second.second == source) {
      // New config request is similar to existing one. No further action required.
      return true;
    } else {
      // Unique config request received for same data view.
      // Hence, unsubscribe the existing one. Fresh subscription required.
      plugin_data_subscriber_->unsubscribe(subscription_info_iterator->second.first);
      subscription_info_.erase(data_view_name);
    }
  }

  std::string url = source + ".Video.Frame";
  auto topics = plugin_signal_explorer_->getTopicsByUrl(url);
  if (topics.size() != 1 || topics[0].size() == 0) {
    warn(__FILE__, "Could not find URL for image source: {0}", url);
    return false;
  }

  auto binded_callback =
      std::bind(&ReferenceCamera::Callback, this, std::placeholders::_1, std::placeholders::_2, data_view_name);
  auto subscription_id =
      plugin_data_subscriber_->subscribeUrl(topics[0], bridgesdk::SubscriptionType::DIRECT, binded_callback);

  if (subscription_id <= 0) {
    warn(__FILE__, "ReferenceCamera::addConfig::subscription to url: {0} failed", url);
    return false;
  }

  debug(__FILE__, "ReferenceCamera::addConfig::subscription to url: {0} is successful", url);

  subscription_info_.insert(std::make_pair(data_view_name, std::make_pair(subscription_id, source)));

  return true;
}
void ReferenceCamera::Callback(std::string const topic, const bridgesdk::ChannelMessagePointers *data,
                               std::string video_stream_name) {
  uint8_t *data_array = (uint8_t *)data->data;
  auto it_offset_info = offset_info_.find(topic);

  if (it_offset_info == offset_info_.end()) {
    /*Get the format offset*/
    auto return_signal_isolated = plugin_signal_extractor_->GetInfo(topic + ".FrameHeader.Format");
    auto format_offset = return_signal_isolated.offset;

    /*Get the size offset*/
    return_signal_isolated = plugin_signal_extractor_->GetInfo(topic + ".FrameHeader.DataSize");
    auto size_offset = return_signal_isolated.offset;

    /*Get the raw image offset*/

    return_signal_isolated = plugin_signal_extractor_->GetInfo(topic + ".Raw_image");
    auto raw_image_offset = return_signal_isolated.offset;

    auto result = offset_info_.try_emplace(topic, format_offset, size_offset, raw_image_offset);
    if (result.second) {
      it_offset_info = result.first;
    } else {
      return;
    }
  }

  /* Validate the data size required to get the raw image */
  if (data->size < std::get<2>(it_offset_info->second)) {
    error(__FILE__, "Size overflow!");
    return;
  }

  /* Get format*/
  auto format = ((uint8_t *)(data_array + std::get<0>(it_offset_info->second)))[0];
  if (format >= 10) {
    error(__FILE__, "Invalid frame format.");
    return;
  }
  /*Get size*/
  auto size = ((int32_t *)(data_array + std::get<1>(it_offset_info->second)))[0];
  /*Get raw image*/
  uint8_t *raw_image = data_array + std::get<2>(it_offset_info->second);
  /* Validate image size*/
  if (long long img_size = raw_image - data_array;
      0 > size || 0 > img_size || data->size < static_cast<size_t>(img_size) ||
      static_cast<size_t>((data->size - static_cast<size_t>(img_size))) < static_cast<size_t>(size)) {
    error(__FILE__, "Size overflow!");
    return;
  }

  Json::Value video_stream;
  video_stream["type"] = format_type[format];
  video_stream["data"] = base64_encode(raw_image, static_cast<size_t>(size));
  Json::Value final_message_json;
  final_message_json["dataStream"][video_stream_name] = video_stream;
  final_message_json["dataStream"]["timestamp"] = static_cast<Json::Value::UInt64>(data->time);

  Json::FastWriter writer;
  std::string response = writer.write(final_message_json);
  plugin_publisher_->sendResultData((uint8_t *)response.data(), response.size());
}

bool ReferenceCamera::init() {
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  return true;
}

bool ReferenceCamera::enterReset() {
  // so far we are doing nothing here
  return true;
}

bool ReferenceCamera::exitReset() {
  if (init_succesfull_) {
    removeConfig(config_);
  }
  init_succesfull_ = addConfig(config_);
  return init_succesfull_;
}

bool ReferenceCamera::removeConfig(const next::bridgesdk::plugin_config_t &config) {
  std::string source, data_view_name;
  if (GetValueFromPluginConfig(config, "source", source) != true) {
    return false;
  }

  if (GetValueFromPluginConfig(config, "DATA_VIEW", data_view_name) != true) {
    return false;
  }

  // Check if received config request matches with the existing configurations.
  auto subscription_info_iterator = subscription_info_.find(data_view_name);
  if (subscription_info_iterator != subscription_info_.end()) {
    if (subscription_info_iterator->second.second == source) {
      // Configuration exists. Hence, unsubscribe and remove.
      plugin_data_subscriber_->unsubscribe(subscription_info_iterator->second.first);
      subscription_info_.erase(data_view_name);
      return true;
    } else {
      error(__FILE__, "Invalid configuration for data view: {0}", data_view_name);
      return false;
    }
  }
  error(__FILE__, "No configuration found for data view: {0}", data_view_name);
  return false;
}
} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::ReferenceCamera(path); }
