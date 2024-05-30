/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     plugin_config_handler.cpp
 *  @brief    see plugin_config_handler.h
 */

#include "plugin_config_handler.h"
#include <next/plugins_utils/helper_functions.h>

namespace next {
namespace databridge {
namespace plugin_util {

void handle_eptr(std::exception_ptr eptr, const std::string &plg_name) {
  try {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  } catch (const std::exception &e) {
    warn(__FILE__, "Caught exception in plugin config handler: {0} and plugin: {1} ", e.what(), plg_name);
  }
}

PluginConfigHandler::PluginConfigHandler(const PluginConfigHandler &cpy) {
  this->available_configs_ = cpy.available_configs_;
}

void PluginConfigHandler::addNewConfigCall(const plugin_util::map_plugin_configs &configs_to_set,
                                           plugin_util::map_plugin_configs &plugin_configs_cpy,
                                           map_plugin_configs &configs_to_add) {
  for (auto it_config : configs_to_set) {
    auto it = plugin_configs_cpy.find(it_config.first);
    if (it == plugin_configs_cpy.end()) {
      configs_to_add.emplace(it_config);
      plugin_configs_cpy.emplace(it_config);
    }
  }
}

void PluginConfigHandler::removeConfigIfCfgChangedAndCall(const plugin_util::map_plugin_configs &configs_to_set,
                                                          plugin_util::map_plugin_configs &plugin_configs_cpy,
                                                          map_plugin_configs &configs_to_remove) {
  for (auto config_it = plugin_configs_cpy.begin(); config_it != plugin_configs_cpy.end();) {
    auto it = configs_to_set.find(config_it->first);
    if (it != configs_to_set.end()) {
      // config is already there do nothing
      std::advance(config_it, 1);
    } else {
      // config is not present in the new config list from GUI, so remove it.
      configs_to_remove.emplace(*config_it);
      config_it = plugin_configs_cpy.erase(config_it);
      if (plugin_configs_cpy.empty()) {
        return;
      }
    }
  }
}

void PluginConfigHandler::removeConfig(const std::string &plugin_name, const std::string &source_name,
                                       const std::string &config_name) {
  std::lock_guard<std::mutex> lck(config_protection_);
  {
    // contains: [string]: plugin name -> [string]: plugin source, [vector<plugin_config_t>]: list of plugin configs
    // std::map<std::string, std::map<std::string, map_plugin_configs>> available_configs_{};
    // key: unique key for the uniq name of config , value: plugin config
    // typedef std::map<std::string, bridgesdk::plugin_config_t> map_plugin_configs;
    auto *it_plugin_configs_all = &(available_configs_[plugin_name][source_name]);
    auto it_instance_remove = it_plugin_configs_all->find(config_name);
    if (it_instance_remove != it_plugin_configs_all->end()) {
      it_plugin_configs_all->erase(it_instance_remove);
    }
  }
}

void PluginConfigHandler::updateConfigs(const std::vector<next::bridgesdk::plugin_config_t> &configs_to_set,
                                        const std::string &plugin_name, const std::string &source_name,
                                        PluginConfigUpdates &plugin_config_updates) {
  // compare given list of configs with the plugin internal list of configs
  // mark all known configs as "to be removed"than compare the new config with them,
  // if they are the same, remove from "to be removed list"
  // other wise keep it in the "to be removed"list and add the new config

  plugin_config_updates.configs_to_add.clear();
  plugin_config_updates.configs_to_remove.clear();

  plugin_util::map_plugin_configs plugin_configs_cpy{};
  {
    std::lock_guard<std::mutex> lck{config_protection_};
    {
      auto it_plugin_configs_all = available_configs_.find(plugin_name);
      if (it_plugin_configs_all != available_configs_.end()) {
        auto itPlugin_configs_source = it_plugin_configs_all->second.find(source_name);
        if (itPlugin_configs_source != it_plugin_configs_all->second.end()) {
          plugin_configs_cpy = itPlugin_configs_source->second;
        }
      }
    }
  }
  plugin_util::map_plugin_configs configs_map;
  for (const auto &conf : configs_to_set) {
    std::string config_key = next::plugins_utils::CreateKeyOutOfPluginConfig(conf);
    auto ret = configs_map.insert({config_key, conf});
    if (false == ret.second) {
      warn(__FILE__, "Generated key {0} is not unique. Config will be ignored.", config_key);
    }
  }
  removeConfigIfCfgChangedAndCall(configs_map, plugin_configs_cpy, plugin_config_updates.configs_to_remove);
  addNewConfigCall(configs_map, plugin_configs_cpy, plugin_config_updates.configs_to_add);

  std::lock_guard<std::mutex> lck(config_protection_);
  {
    if (!plugin_configs_cpy.empty()) {
      available_configs_[plugin_name][source_name] = plugin_configs_cpy;
    } else {
      if (auto pluginConfig = available_configs_.find(plugin_name); pluginConfig != available_configs_.end()) {
        pluginConfig->second.erase(source_name);
      }
    }
  }
}

std::map<std::string, std::map<std::string, map_plugin_configs>> PluginConfigHandler::getAvailableConfigs() {
  return available_configs_;
}

} // namespace plugin_util
} // namespace databridge
} // namespace next
