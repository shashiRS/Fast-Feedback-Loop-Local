/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_sdl_mock.hpp
 * @brief    A mock plugin for testing mocked ...
 *
 * This is an rudimentary plugin class which is used for testing addConfig and removeConfig function calls.
 * It get topics and subscribe to urls (from a SDL file) to emulate the functionality of
 * configuring a usual plugin. It's not configured to receive/send data.
 *
 * Note: The ap_pdo_tool.sdl file has to be published first (see e.g. SetUp() in unittest_plugin_manager_helper.hpp
 *        because this mock plugin get topics and subscribe to urls from this sdl file.
 *
 **/

#include <algorithm> // std::for_each
#ifdef _DEBUG
#include <iostream>
#endif

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#ifndef NEXT_PLUGIN_MOCK_SDL_H_
#define NEXT_PLUGIN_MOCK_SDL_H_

namespace next {
namespace plugins {

class MockSdlPlugin : public next::bridgesdk::plugin::Plugin {

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

public:
  MockSdlPlugin(const char *path) : bridgesdk::plugin::Plugin(path) {
    this->setVersion(std::string("0.0.1"));
    this->setName(std::string("Basic Mock Plugin"));
  }

  bool init() override {
    plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
    plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
    plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
    plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
    plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);

    return true;
  }

  //! Set the config send by GUI to be used in plugin's code to subscribe to topic or extracting data from payload.
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override {
    lastAddedConfig_.clear();
    lastAddedConfig_ = config;
    add_config_counter++;
    return added_config = true;
  } // simulate if it was successfull

  //! Remove current active config used in plugin.
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override {
    lastRemovedConfig_.clear();
    lastRemovedConfig_ = config;

    remove_config_counter++;
    return removed_config = true;
  }

  //! Get current active config used in plugin.
  bool getConfig(next::bridgesdk::plugin_config_t &config_fields) const override {
    if (configs.size() > 0) {
      config_fields = configs.front();
    }
    return true;
  }

  //! Get possible configuration description
  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) const override {

    config_fields.insert({"url", {next::bridgesdk::ConfigType::EDITABLE, "videoURL"}});
    config_fields.insert({"resolution", {next::bridgesdk::ConfigType::SELECTABLE, "1080p"}});
    config_fields.insert({"resolution", {next::bridgesdk::ConfigType::SELECTABLE, "960p"}});
    config_fields.insert({"resolution", {next::bridgesdk::ConfigType::SELECTABLE, "480p"}});
    config_fields.insert({"format", {next::bridgesdk::ConfigType::SELECTABLE, "png"}});
    config_fields.insert({"format", {next::bridgesdk::ConfigType::SELECTABLE, "jpeg"}});
    config_fields.insert({"format", {next::bridgesdk::ConfigType::SELECTABLE, "svg"}});
    return true;
  }

  void resetCounter() {
    add_config_counter = 0;
    remove_config_counter = 0;
  }

  // member variables:
public:
  // only used for unittests
  bool added_config = false;
  std::atomic<int> add_config_counter{0};
  next::bridgesdk::plugin_config_t lastAddedConfig_;

  // only used for unittests
  bool removed_config = false;
  std::atomic<int> remove_config_counter{0};
  next::bridgesdk::plugin_config_t lastRemovedConfig_;

  std::string strDataSrc = "";

  std::unordered_map<std::string, size_t>::iterator itMapTopicsStart;
  std::unordered_map<std::string, size_t>::iterator itMapTopicsEnd;

  int iStartIDObject = 0;
  int iNumOfObject = 0;
};

} // namespace plugins
} // namespace next

#endif // NEXT_PLUGIN_MOCK_SDL_H_
