/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_explorer_impl.hpp
 * @brief    implements the signal explorer for the plugin
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_SIGNAL_EXPLORER_IMPL_H_
#define NEXT_BRIDGESDK_PLUGIN_SIGNAL_EXPLORER_IMPL_H_

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class PluginSignalExplorerImpl {

private:
  PluginSignalExplorerImpl() = delete;

public:
  PluginSignalExplorerImpl(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginSignalExplorerImpl() = default;

  std::vector<signal_child_info> getChildsByUrl(const std::string &url) const;

  void getDataSourceNames(const std::string &format_id, std::vector<std::string> &data_source_names);

  std::unordered_map<std::string, std::vector<std::string>> getURLTree(void) const;

  std::vector<std::string> getTopicByUrl(const std::string &url) const;

  std::vector<std::string> searchSignalTree(const std::string &keyword) const;

  PluginSignalDescription getPackageDetailsByUrl(const std::string &signal_url) const;

  std::pair<bool, std::string> generateSdl(const std::string topic) const;

  next::bridgesdk::plugin::Plugin *base_plugin_{};
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_SIGNAL_EXPLORER_IMPL_H_
