/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_explorer_impl.cpp
 * @brief    implements the signal explorer for the plugin
 **/

#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>

#include "plugin_signal_explorer_impl.hpp"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

std::vector<signal_child_info> PluginSignalExplorer::getChildsbyUrl(const std::string &parent_url) const {
  return impl_->getChildsByUrl(parent_url);
}

void PluginSignalExplorer::getDataSourceNames(const std::string &format_id,
                                              std::vector<std::string> &data_source_names) const {
  impl_->getDataSourceNames(format_id, data_source_names);
}

std::unordered_map<std::string, std::vector<std::string>> PluginSignalExplorer::getUrlTree(void) const {
  return impl_->getURLTree();
}

std::vector<std::string> PluginSignalExplorer::getTopicsByUrl(const std::string &signal_url) const {
  return impl_->getTopicByUrl(signal_url);
}

PluginSignalDescription PluginSignalExplorer::getPackageDetailsByUrl(const std::string &signal_url) const {
  return impl_->getPackageDetailsByUrl(signal_url);
}

std::pair<bool, std::string> PluginSignalExplorer::generateSdl(const std::string topic) const {
  return impl_->generateSdl(topic);
}

PluginSignalExplorer::PluginSignalExplorer(next::bridgesdk::plugin::Plugin *base_plugin) {
  impl_ = std::make_unique<PluginSignalExplorerImpl>(base_plugin);
}

std::vector<std::string> PluginSignalExplorer::searchSignalTree(const std::string &signal_url) const {
  return impl_->searchSignalTree(signal_url);
}

PluginSignalExplorer::~PluginSignalExplorer() {}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next
