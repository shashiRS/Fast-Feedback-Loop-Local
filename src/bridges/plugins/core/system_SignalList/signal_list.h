/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     signal_list.h
 * @brief    Signal List Plugin
 *
 * This plugin gets a tree structure of the signals available from the backend
 * and puts it into a json
 *
 **/

#ifndef NEXT_PLUGINS_SIGNAL_LIST_PLUGIN_H_
#define NEXT_PLUGINS_SIGNAL_LIST_PLUGIN_H_

#include <memory>
#include <string>
#include <vector>

#include <json/json.h>

#include <next/sdk/sdk_macros.h>

#include <next/bridgesdk/plugin.h>

#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>

namespace next {
namespace plugins {

NEXT_DISABLE_DEPRECATED_WARNING
class SignalList : public next::bridgesdk::plugin::Plugin {
public:
  SignalList(const char *path);
  virtual ~SignalList();

  bool init() override;
  bool update(std::vector<std::string> user_args) override;

  bool enterReset() override;
  bool exitReset() override;

private:
  Json::Value
  createPayloadJSONByURL(const std::string &current_URL,
                         const std::vector<bridgesdk::plugin_addons::signal_child_info> &vector_childs_URL) const;
  Json::Value createPayloadJsonBySearchKeyword(const std::string &keyword) const;

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;

  std::vector<std::string> update_user_args_;

public:
  friend class SignalListTester;
};
NEXT_RESTORE_WARNINGS

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_SIGNAL_LIST_PLUGIN_H_
