/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     unittest_plugin_manager_helper.hpp
 *  @brief    Helper class for unit testing of the plugin manager
 */

#ifndef NEXT_UNITTEST_PLUGIN_MANAGER_HELPER_H_
#define NEXT_UNITTEST_PLUGIN_MANAGER_HELPER_H_

#include <gtest/gtest.h>
#include <memory>

#include <next/databridge/plugin_manager.h>
#include "plugin_config_handler.h"

namespace next {
namespace test {

//! A class helping for the plugin manager unit test to access private members and functions
/*!
 * This class is used for unit testing using GoogleTest.
 * What you have to do in your code:
 *   #include "unittest_plugin_manager_helper.hpp"
 */

using namespace next::databridge::plugin_manager;
class PluginManagerTestHelper {
public:
  PluginManagerTestHelper(PluginManager &oPluginManager) : oPluginManager_(oPluginManager) {}
  ~PluginManagerTestHelper() {}

  void SetUpPluginsOfPluginManager(const std::map<std::string, plugin_information> &plugins) {
    oPluginManager_.plugins_ = plugins;
  }

  bool checkForPluginReset(const next::control::events::PlayerMetaInformationMessage &message) {
    return oPluginManager_.checkForPluginReset(message);
  }

  bool updatePluginConfigs(const std::string data_class, const std::string version, std::string source_name,
                           const std::vector<std::string> user_parameter) {
    return oPluginManager_.updatePluginConfigs(data_class, version, source_name, user_parameter);
  }
  void resetAllPlugins() { return oPluginManager_.resetAllPlugins(); }

  bool findAndSetConfig(const std::vector<bridgesdk::plugin_config_t> &config, std::string plugin_name,
                        std::string source_name) {
    return oPluginManager_.findAndSetConfig(config, plugin_name, source_name);
  }

  bool addPluginInstance(const std::string &plugin_name, const std::string &source_name,
                         const std::string &instance_name) {
    return oPluginManager_.addPluginInstance(plugin_name, source_name, instance_name);
  }

  bool getPluginVersion(const std::string &plugin_name, const std::string &source_name,
                        const std::string &instance_name, std::string &plugin_version) {
    return oPluginManager_.getPluginVersion(plugin_name, source_name, instance_name, plugin_version);
  }

  bool initPlugin(const std::string &plugin_name, const std::string &source_name, const std::string &instance_name) {
    return oPluginManager_.initPlugin(plugin_name, source_name, instance_name);
  }

  bool updatePlugin(const std::string &plugin_name, const std::string &source_name, const std::string &instance_name,
                    std::vector<std::string> user_arg) {
    return oPluginManager_.updatePlugin(plugin_name, source_name, instance_name, user_arg);
  }
  void addAndRemoveConfigs(const std::vector<bridgesdk::plugin_config_t> &config, std::string plugin_name,
                           std::string source_name) {
    return oPluginManager_.addAndRemoveConfigs(config, plugin_name, source_name);
  }
  std::map<std::string, std::map<std::string, next::databridge::plugin_util::map_plugin_configs>>
  getAvailableConfigs() {
    return plugin_config_handler_->getAvailableConfigs();
  }
  void
  setPluginConfigHanlder(std::shared_ptr<next::databridge::plugin_util::PluginConfigHandler> plugin_config_handler) {
    plugin_config_handler_ = plugin_config_handler;
  }

private:
  PluginManager &oPluginManager_;
  std::shared_ptr<next::databridge::plugin_util::PluginConfigHandler> plugin_config_handler_{nullptr};
};

} // namespace test
} // namespace next

#endif // NEXT_UNITTEST_PLUGIN_MANAGER_HELPER_H_
