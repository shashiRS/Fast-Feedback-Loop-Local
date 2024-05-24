/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_explorer_mock.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGES_PLUGIN_SIGNALEXPLORER_MOCK_H
#define NEXT_BRIDGES_PLUGIN_SIGNALEXPLORER_MOCK_H

#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>

namespace next {
namespace test {

class PluginSignalExplorerMock : public next::bridgesdk::plugin_addons::PluginSignalExplorer {

public:
  PluginSignalExplorerMock(next::bridgesdk::plugin::Plugin *plugin) : PluginSignalExplorer(plugin) {}
  ~PluginSignalExplorerMock() {}

  std::vector<next::bridgesdk::plugin_addons::signal_child_info> getChildsbyUrl(const std::string &) const override {
    return {};
  }

  void getDataSourceNames(const std::string &, std::vector<std::string> &) const override {}

  std::unordered_map<std::string, std::vector<std::string>> getUrlTree(void) const override { return {{"", {}}}; }

  std::vector<std::string> getTopicsByUrl(const std::string &signal_url) const override {
    auto url = sig_url_.find(signal_url);
    if (url != std::end(sig_url_)) {
      return url->second;
    } else {
      return {};
    }
  }

  void setURLs(const std::string &url, std::vector<std::string> topic) { sig_url_[url] = topic; }

  void setPackageDetailsMock(const std::string &url,
                             const next::bridgesdk::plugin_addons::PluginSignalDescription &desc) {
    packageDetailsMocks[url] = desc;
  }

  next::bridgesdk::plugin_addons::PluginSignalDescription
  getPackageDetailsByUrl(const std::string &signal_url) const override {
    next::bridgesdk::plugin_addons::PluginSignalDescription info;
    for (const auto &entry : packageDetailsMocks) {
      if (entry.first.find(signal_url) == 0) {
        info.data_source_name = entry.second.data_source_name;
        info.format_type = entry.second.format_type;
        info.source_id = entry.second.source_id;
        info.instance_id = entry.second.instance_id;
        info.cycle_id = entry.second.cycle_id;
        info.vaddr = entry.second.vaddr;

        break;
      }
    }

    return info;
  }

private:
  std::map<std::string, std::vector<std::string>> sig_url_{};
  std::unordered_map<std::string, next::bridgesdk::plugin_addons::PluginSignalDescription> packageDetailsMocks;
};

} // namespace test
} // namespace next

#endif // NEXT_BRIDGES_PLUGIN_SIGNALEXPLORER_MOCK_H
