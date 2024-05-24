/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_handler.h
 *  @brief    stub
 */

#ifndef NEXT_DATABRIDGE_PLUGIN_CONFIG_HANDLER_H_
#define NEXT_DATABRIDGE_PLUGIN_CONFIG_HANDLER_H_

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <next/bridgesdk/datatypes.h>
#include <next/bridgesdk/plugin.h>

namespace next {
namespace databridge {
namespace plugin_util {

// key: unique key for the uniq name of config , value: plugin config
typedef std::map<std::string, bridgesdk::plugin_config_t> map_plugin_configs;

struct PluginConfigUpdates {
  map_plugin_configs configs_to_add;
  map_plugin_configs configs_to_remove;
};

/*!
 * The data request server enables a data request
 * from multiple connected clients. Its API allows
 * to activate or deactivate data classes provided
 * by the data bridge.
 */
class PluginConfigHandler {
public:
  PluginConfigHandler() = default;
  PluginConfigHandler(const PluginConfigHandler &cpy);

  void updateConfigs(const std::vector<next::bridgesdk::plugin_config_t> &configs_to_set,
                     const std::string &plugin_name, const std::string &source_name,
                     PluginConfigUpdates &plugin_config_updates);
  void removeConfig(const std::string &plugin_name, const std::string &source_name, const std::string &config_name);

  std::map<std::string, std::map<std::string, map_plugin_configs>> getAvailableConfigs();

private:
  void addNewConfigCall(const plugin_util::map_plugin_configs &configs_to_set,
                        plugin_util::map_plugin_configs &plugin_configs_cpy, map_plugin_configs &configs_to_add);

  void removeConfigIfCfgChangedAndCall(const plugin_util::map_plugin_configs &configs_to_set,
                                       plugin_util::map_plugin_configs &plugin_configs_cpy,
                                       map_plugin_configs &configs_to_remove);

  // map containing the plugins and their configurations
  // contains: [string]: plugin name -> [string]: plugin source, [vector<plugin_config_t>]: list of plugin configs
  std::map<std::string, std::map<std::string, map_plugin_configs>> available_configs_{};
  std::mutex config_protection_{};

  friend class PluginConfigHandlerTest;
};

} // namespace plugin_util
} // namespace databridge
} // namespace next

#endif // NEXT_DATABRIDGE_PLUGIN_CONFIG_HANDLER_H_
