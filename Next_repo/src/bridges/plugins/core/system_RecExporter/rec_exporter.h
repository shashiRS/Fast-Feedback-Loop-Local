/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     rec_exporter.h
 * @brief    Rec Exporter Plugin
 *
 * This plugin exports selected groups to a .rec file
 *
 **/

#ifndef NEXT_PLUGINS_REC_EXPORTER_H_
#define NEXT_PLUGINS_REC_EXPORTER_H_

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <json/json.h>

#include <next/sdk/sdk_macros.h>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/extensibility/sources.h>
#include <mts/extensibility/package.hpp>
#include <mts/introspection/conversion.hpp>
#include <mts/introspection/xtypes/dynamic_type.hpp>
#include <mts/introspection/xtypes/generator/sdl.hpp>
#include <mts/introspection/xtypes/parser/json.hpp>
#include <mts/runtime/memory_allocator.hpp>
#include <mts/runtime/offline/reader_proxy.hpp>
#include <mts/runtime/service_container.hpp>
#include <mts/runtime/xport/recording_exporter_proxy.hpp>
NEXT_RESTORE_WARNINGS

namespace next {
namespace plugins {

using namespace next::bridgesdk::plugin_addons;

typedef struct {
  uint16_t source_id = 0;
  uint32_t instance_id = 0;
  std::vector<std::string> sdls;
} DeviceInfo;

class RecExporter : public next::bridgesdk::plugin::Plugin {
public:
  RecExporter(const char *path);
  virtual ~RecExporter();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;
  bool update(std::vector<std::string> user_input) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;

private:
  void configure();
  void save_config(const std::vector<std::string> &user_input);

  void callBackInputPort(std::string const topic, const bridgesdk::ChannelMessagePointers *data);

  void subscribeToTopic(const std::string &topic);
  void addSignalToSignalsList(const std::string &topic);

  bool handleStartExport(const Json::Value &root);
  void handleStopExport();

  void parseFolderAndFile(const std::string &file_path, std::string &folder_name, std::string &file_name);
  void createFolder(const std::string &folder_name);
  void replaceVariables(std::string &file_name);
  void replaceFileIndex(std::string &file_name, const std::string &file_format);
  void prepareOutputFile(const std::string &file_path, const std::string &file_format, std::string &formated_file_path);
  void addDescriptionToDevice(std::map<std::string, DeviceInfo> &device_map, const PluginSignalDescription description,
                              const std::string sdl);

  void exportSource(const std::map<std::string, DeviceInfo> device_map);

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

  std::vector<std::string> configuration_;
  bool configured_ = false;

  std::map<std::string, std::vector<std::pair<std::string, next::bridgesdk::plugin_addons::SignalInfo>>>
      map_topic_signals_name_and_info_;

  std::vector<size_t> subscription_ids_;

  std::unique_ptr<mts::runtime::xport::recording_exporter_proxy> exporter_proxy_;
  bool rec_file_open_ = false;

  std::map<std::string, PluginSignalDescription> topic_description_map_;

  std::mutex file_access_lock_;

  uint64_t export_exception_counter_ = 0;

  std::string exporter_name_;
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_REC_EXPORTER_H_
