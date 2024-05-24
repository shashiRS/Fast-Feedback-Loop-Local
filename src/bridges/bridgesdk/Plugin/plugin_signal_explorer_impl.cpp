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

#include "plugin_signal_explorer_impl.hpp"
#include "plugin_private.h"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginSignalExplorerImpl::PluginSignalExplorerImpl(next::bridgesdk::plugin::Plugin *base_plugin) {
  base_plugin_ = base_plugin;
}

std::vector<signal_child_info> PluginSignalExplorerImpl::getChildsByUrl(const std::string &url) const {
  return base_plugin_->impl_->getChildsByUrl(url);
}

void PluginSignalExplorerImpl::getDataSourceNames(const std::string &format_id,
                                                  std::vector<std::string> &data_source_names) {
  base_plugin_->impl_->getDataSourceNames(format_id, data_source_names);
}

std::unordered_map<std::string, std::vector<std::string>> PluginSignalExplorerImpl::getURLTree(void) const {
  return base_plugin_->impl_->getURLTree();
}

std::vector<std::string> PluginSignalExplorerImpl::getTopicByUrl(const std::string &url) const {
  return base_plugin_->impl_->getTopicByUrl(url);
}

std::vector<std::string> PluginSignalExplorerImpl::searchSignalTree(const std::string &keyword) const {
  return base_plugin_->impl_->searchSignalTree(keyword);
}

PluginSignalDescription PluginSignalExplorerImpl::getPackageDetailsByUrl(const std::string &signal_url) const {
  PluginSignalDescription desc;
  auto details = base_plugin_->impl_->getPackageDetailsByUrl(signal_url);
  desc.vaddr = details.basic_info.vaddr;
  desc.source_id = details.basic_info.source_id;
  desc.instance_id = details.basic_info.instance_id;
  desc.data_source_name = details.basic_info.data_source_name;
  desc.format_type = details.basic_info.format_type;
  desc.cycle_id = details.basic_info.cycle_id;

  return desc;
}

std::pair<bool, std::string> PluginSignalExplorerImpl::generateSdl(const std::string topic) const {
  return base_plugin_->impl_->generateSdl(topic);
}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next
