/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     rec_exporter.cpp
 * @brief    Rec Exporter Plugin
 *
 * See rec_exporter.h.
 *
 **/

#include "rec_exporter.h"

#include "pack_memory_to_corepackage.h"

#include <mts/toolbox/ecu/ecu.hpp>
#include <mts/toolbox/swc/packager.hpp> // For SWC generation

#include <boost/filesystem.hpp>
#include <chrono>
#include <ctime>
#include <thread>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>

#include <next/plugins_utils/helper_functions.h>

namespace next {
namespace plugins {

RecExporter::RecExporter(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello system_RecExporter plugin");

  this->setVersion(std::string("0.0.1"));
  this->setName(std::string("system_RecExporter"));
}

RecExporter::~RecExporter() { info(__FILE__, "Goodbye RecExporter"); }

bool RecExporter::init() {
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);

  return true;
}

bool RecExporter::enterReset() {
  if (!configured_ && !configuration_.empty()) {
    configure();
  }
  return configured_;
}

bool RecExporter::exitReset() {
  // so far we are doing nothing here
  return true;
}

bool RecExporter::update(std::vector<std::string> user_input) {
  if (user_input.empty())
    return true;

  save_config(user_input);

  // try to configure with lates received config
  configure();

  return configured_;
}

bool RecExporter::addConfig(const next::bridgesdk::plugin_config_t &config) {
  if (!next::plugins_utils::GetValueFromPluginConfig(config, "DATA_VIEW", exporter_name_)) {
    warn(__FILE__, "SignalExporter wrong data view mapping name: DATA_VIEW");
    return false;
  }

  return true;
}

void RecExporter::configure() {
  debug(__FILE__, "configure() ...");

  NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4996)
  Json::Value root;
  Json::Reader reader;
  if (configuration_.size() == 1) {
    reader.parse(configuration_[0], root);
  } else {
    for (auto const &us_in : configuration_) {
      Json::Value v_tmp;
      reader.parse(us_in, v_tmp);
      auto mem_name{v_tmp.getMemberNames()[0]};
      root[mem_name] = v_tmp[mem_name];
    }
  }
  NEXT_RESTORE_WARNINGS_WINDOWS

  if (root["event"] == "UserPressedStopSignalExportingButton") {
    handleStopExport();
    configured_ = false;
  } else if (root["event"] == "UserPressedStartSignalExportingButton") {
    configured_ = handleStartExport(root);
  } else {
    next::appsupport::NextStatusCode::PublishStatusCode(
        exporter_name_, next::appsupport::StatusCode::ERR_EXPORTER_CONFIGURATION_NOT_SUPPORTED,
        "Configuration event not supported");
  }
}

void RecExporter::save_config(const std::vector<std::string> &user_input) {
  // TODO instead of saving all the config, only adapt relevant info or properties that are different
  configuration_ = user_input;
}

void RecExporter::callBackInputPort(std::string const topic, const bridgesdk::ChannelMessagePointers *data) {
  std::lock_guard<std::mutex> lock_guard(file_access_lock_);

  if (topic_description_map_.find(topic) == topic_description_map_.end()) {
    return;
  }

  auto basic_info = topic_description_map_[topic];
  auto package = next::plugins::rec_exporter::TransferCoreLibPackage::CreatePackageFromData(basic_info, data->data,
                                                                                            data->size, data->time);

  try {
    next::plugins::rec_exporter::TransferCoreLibPackage::WritePackage(exporter_proxy_, package);

  } catch (const std::exception &e) {
    error(__FILE__, e.what());
    export_exception_counter_++;

    next::plugins::rec_exporter::TransferCoreLibPackage::FreePackage();

    return;
  }

  next::plugins::rec_exporter::TransferCoreLibPackage::FreePackage();
}

void RecExporter::subscribeToTopic(const std::string &topic) {
  auto bf = std::bind(&RecExporter::callBackInputPort, this, std::placeholders::_1, std::placeholders::_2);
  size_t subscribtion_id = plugin_data_subscriber_->subscribeUrl(topic, bridgesdk::SubscriptionType::DIRECT, bf);
  subscription_ids_.push_back(subscribtion_id);
  if (!plugin_data_subscriber_->checkConnectionWait(topic, 2000)) {
    error(__FILE__, "found topic could not be connected {0}: ", topic);
    next::appsupport::NextStatusCode::PublishStatusCode(
        exporter_name_, next::appsupport::StatusCode::WARN_EXPORTER_PARTIAL_SUBSCRIPTION,
        "No subscription found for topic: " + topic);
  }
}

void RecExporter::addSignalToSignalsList(const std::string &topic) {
  next::bridgesdk::plugin_addons::SignalInfo sig_info;
  sig_info = plugin_signal_extractor_->GetInfo(topic);
  map_topic_signals_name_and_info_[topic].push_back(std::make_pair(topic, sig_info));
}

bool RecExporter::handleStartExport(const Json::Value &root) {
  setSynchronizationMode(true);
  setPackageDropMode(false);
  std::string file_format = root["fileFormat"].asCString();
  if (file_format != ".rec") {
    return false;
  }

  info(__FILE__, "== RecExporter starting ==");

  export_exception_counter_ = 0;

  std::map<std::string, DeviceInfo> device_map;

  // go though the URL list and subscribe to the according topic
  for (auto &url : root["listofURLs"]) {
    auto signalURL = url.asString();
    auto topic = plugin_signal_explorer_->getTopicsByUrl(signalURL);
    if (topic.empty()) {
      continue;
    }

    if (map_topic_signals_name_and_info_.count(topic[0]) == 0) {
      auto description = plugin_signal_explorer_->getPackageDetailsByUrl(topic[0]);
      topic_description_map_.insert(make_pair(topic[0], description));

      if (auto sdl = plugin_signal_explorer_->generateSdl(topic[0]); sdl.first == true) {
        addDescriptionToDevice(device_map, description, sdl.second);
        subscribeToTopic(topic[0]);
        addSignalToSignalsList(topic[0]);
      } else {
        continue;
      }
    }
  }

  if (subscription_ids_.empty()) {
    next::appsupport::NextStatusCode::PublishStatusCode(exporter_name_,
                                                        next::appsupport::StatusCode::ERR_EXPORTER_NO_SUBSCRIPTION,
                                                        "No subscription found. Exporting stopped");

    // nothing was subscribed, cleanup everything to avoid memory leaks
    handleStopExport();
    return false;
  }

  std::string output_folder = root["outputFolder"].asCString();
  std::string formated_file_path;
  prepareOutputFile(output_folder, file_format, formated_file_path);

  formated_file_path += ".rec";

  // MTS uses the Service Locator pattern to access interfaces between different components
  auto service_container_ptr = mts::runtime::get_default_service_container();

  // Create an exporter proxy
  exporter_proxy_ =
      std::make_unique<mts::runtime::xport::recording_exporter_proxy>(service_container_ptr->get_service_locator());
  // Request our export format
  auto format = exporter_proxy_->get_export_format("REC");

  if (!format.has_value()) {
    error(__FILE__, "Missing export format");
    return false;
  }

  try {
    // Start exporting
    exporter_proxy_->start_export(format.value().name, formated_file_path);
    rec_file_open_ = true;
    // Export source for each available device
    exportSource(device_map);

  } catch (const std::exception &e) {
    error(__FILE__, e.what());
    return false;
  }

  info(__FILE__, "== Subscribe, {0} signals ==", subscription_ids_.size());
  // ecal issue with connections not being synchronizied -> remove with eCAL 6
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  next::appsupport::NextStatusCode::PublishStatusCode(
      exporter_name_, next::appsupport::StatusCode::SUCC_FULL_SUBSCRIPTION, "Exporting started");

  return true;
}

void RecExporter::handleStopExport() {
  for (auto subscribtion_id : subscription_ids_) {
    plugin_data_subscriber_->unsubscribe(subscribtion_id);
  }
  subscription_ids_.clear();
  map_topic_signals_name_and_info_.clear();
  topic_description_map_.clear();

  if (exporter_proxy_ && rec_file_open_) {
    exporter_proxy_->end_export();
    rec_file_open_ = false;
  }

  if (export_exception_counter_ > 0) {
    export_exception_counter_ = 0;
  }

  info(__FILE__, "== RecExporter stopped ==");
  next::appsupport::NextStatusCode::PublishStatusCode(
      exporter_name_, next::appsupport::StatusCode::SUCC_EXPORTER_STOPPED, "Exporting stopped");
}

void RecExporter::parseFolderAndFile(const std::string &file_path, std::string &folder_name, std::string &file_name) {
  if (file_path.empty()) {
    warn(__FILE__, "Output folder/file not set, current folder will be used, default file name");
    return;
  }

  auto temp_path = file_path;
  std::replace(temp_path.begin(), temp_path.end(), '\\', '/');
  boost::filesystem::path dir_path = temp_path;

  if (dir_path.has_extension()) {
    folder_name = dir_path.parent_path().string();
    file_name = dir_path.stem().string();
  } else {
    folder_name = dir_path.string();
  }
}

void RecExporter::createFolder(const std::string &folder_name) {
  if (!boost::filesystem::exists(folder_name)) {
    boost::filesystem::create_directories(folder_name);
  }
}

void RecExporter::replaceVariables(std::string &file_name) {
  auto now = std::chrono::system_clock::now();
  auto current_time = std::chrono::system_clock::to_time_t(now);
  NEXT_DISABLE_DEPRECATED_WARNING
  auto gmt_time = std::gmtime(&current_time);
  NEXT_RESTORE_WARNINGS

  if (size_t datePos = file_name.find("$DATE$"); datePos != std::string::npos) {
    char dateBuffer[12]; // DYYYY_MM_DD
    std::strftime(dateBuffer, sizeof(dateBuffer), "D%Y_%m_%d", gmt_time);
    file_name.replace(datePos, 6, dateBuffer);
  }

  if (size_t timePos = file_name.find("$TIME$"); timePos != std::string::npos) {
    char timeBuffer[10]; // THH_MM_SS
    std::strftime(timeBuffer, sizeof(timeBuffer), "T%H_%M_%S", gmt_time);
    file_name.replace(timePos, 6, timeBuffer);
  }

  if (size_t inputPos = file_name.find("$INPUT$"); inputPos != std::string::npos) {
    auto config_client = next::appsupport::ConfigClient::getConfig();

    std::string recording_path =
        config_client->get_string(next::appsupport::configkey::player::getRecordingPathKey(), "");
    std::string rec_folder_name = "";
    std::string rec_file_name = "";
    parseFolderAndFile(recording_path, rec_folder_name, rec_file_name);

    file_name.replace(inputPos, 7, rec_file_name);
  }
}

void RecExporter::replaceFileIndex(std::string &file_name, const std::string &file_format) {
  if (size_t fileIndexPos = file_name.find("$FILE_INDEX$"); fileIndexPos != std::string::npos) {
    int index = 0;
    do {
      std::string temp_str = file_name;
      temp_str.replace(fileIndexPos, 12, std::to_string(index));

      // add extension
      std::string fileExtended = temp_str + file_format;

      if (!boost::filesystem::exists(fileExtended)) {
        // found first file + index that does not exist
        file_name = temp_str;
        break;
      }

      index++;
    } while (true);
  }
}

void RecExporter::prepareOutputFile(const std::string &file_path, const std::string &file_format,
                                    std::string &formated_file_path) {
  std::string folder_name = ".";
  std::string file_name = "exported_file__$DATE$_$TIME$";

  parseFolderAndFile(file_path, folder_name, file_name);

  createFolder(folder_name);

  replaceVariables(file_name);

  formated_file_path = folder_name + "/" + file_name;

  replaceFileIndex(formated_file_path, file_format);
}

void RecExporter::addDescriptionToDevice(std::map<std::string, DeviceInfo> &device_map,
                                         const PluginSignalDescription description, const std::string sdl) {
  std::string device_name = description.data_source_name;

  if (device_map.find(device_name) == device_map.end()) {
    device_map[device_name].source_id = description.source_id;
    device_map[device_name].instance_id = description.instance_id;
  }

  device_map[device_name].sdls.push_back(sdl);
}

void RecExporter::exportSource(const std::map<std::string, DeviceInfo> device_map) {
  for (auto it : device_map) {
    // The software container manifest is a mandatory part of the container
    // Create and fill a manifest
    mts::toolbox::swc::manifest manifest;
    manifest.set_version_major(3U);
    manifest.set_version_minor(21U);
    manifest.set_version_patch_level(0U);
    manifest.set_build_type(2U);
    manifest.set_project_id("next");
    manifest.set_crc(0U);

    // Create a SWC packager object
    mts::toolbox::swc::packager packager;
    // Build an SWC in the memory
    // At least the manifest and the list of SDLs have to be specified
    // The result is a byte array representing the SWC (in the memory)
    std::vector<uint8_t> swc_buffer = packager.build(manifest, it.second.sdls);

    // Prepare a data source object to export
    mts::extensibility::data_source source{it.second.source_id,
                                           it.second.instance_id,
                                           0,
                                           1,
                                           mts::toolbox::ecu::sensor_software_package_format,
                                           it.first, // Device name
                                           ""};

    // Export to the data source channel
    // Pass the SWC buffer from the previous steps as data description content
    std::string swc_name = it.first + ".swc";
    exporter_proxy_->export_source(source, "swc", swc_name, reinterpret_cast<const uint8_t *>(swc_buffer.data()),
                                   swc_buffer.size());
  }
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::RecExporter(path); }
