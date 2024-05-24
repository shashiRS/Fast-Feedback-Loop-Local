/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     array_tp_objects.h
 * @brief    Parsing network data of array of objects to provide Traffic Participants
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_ARRAY_TPOBJECTS_H_
#define NEXT_PLUGINS_ARRAY_TPOBJECTS_H_

#include <string>
#include <unordered_map>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include <next/plugins_utils/plugins_types.h>
#include "tp_objects.hpp"
#include "url_mapper.h"

namespace next {
namespace plugins {

class ArrayTpObjects : public next::bridgesdk::plugin::Plugin {
public:
  ArrayTpObjects(const char *);
  virtual ~ArrayTpObjects();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;

  void ProcessDataCallback(const std::string topic_name, const next::bridgesdk::ChannelMessagePointers *datablock,
                           const next::plugins_utils::SensorConfig sensor_config,
                           std::shared_ptr<ArrayTpList> array_tp_list, size_t max_arraytp_size);

  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

  inline size_t numOfActiveConfig() { return active_config_to_sensors_.size(); }

private:
  bool addConfigInternal(const next::bridgesdk::plugin_config_t &config);

  std::unordered_map<std::string, next::plugins_utils::SensorConfig> active_config_to_sensors_;
  bridgesdk::plugin_addons::Signals all_signals_info_;
  std::string fbs_path_;
  std::string bfbs_path_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::map<std::string, std::shared_ptr<ArrayTpList>> array_tp_lists_;
  bool GetArraySizeStruct(const std::string &url, size_t &array_size) const;
  next::bridgesdk::plugin_config_t config_;
  bool init_succesfull_{false};
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_ARRAY_TPOBJECTS_H_
