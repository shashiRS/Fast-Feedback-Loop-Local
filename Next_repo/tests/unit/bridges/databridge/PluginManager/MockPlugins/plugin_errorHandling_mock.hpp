/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_errorHandling_mock.hpp
 * @brief    A mock plugin for testing the error handling.
 *
 * This is an rudimentary plugin like class which can return a false value for
 * the functions addConfig and removeConfig.
 *
 * Note: The create_plugin function is not provided! Using via the plugin manager is not possible!
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

#ifndef NEXT_PLUGIN_MOCK_ERROR_HANDLING_H_
#define NEXT_PLUGIN_MOCK_ERROR_HANDLING_H_

namespace next {
namespace plugins {

class MockErrorHandlingPlugin : public next::bridgesdk::plugin::Plugin {

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

public:
  MockErrorHandlingPlugin(const char *path) : bridgesdk::plugin::Plugin(path) {
    this->setVersion(std::string("0.0.1"));
    this->setName(std::string("Error Handling Plugin"));
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
  bool addConfig([[maybe_unused]] const next::bridgesdk::plugin_config_t &config) override {
    add_config_counter++;
    return add_config_retValue;
  } // simulate if it returns different return values

  //! Set the config send by GUI to be used in plugin's code to subscribe to topic or extracting data from payload.
  bool update([[maybe_unused]] const std::vector<std::string> user_parameter) override {
    add_update_counter++;
    return update_config_retValue;
  } // simulate if it returns different return values

  //! Remove current active config used in plugin.
  bool removeConfig([[maybe_unused]] const next::bridgesdk::plugin_config_t &config) override {
    remove_config_counter++;

    return remove_config_retValue;
  } // simulate if it returns different return values

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
    return true;
  }

  void resetMockPlugin() {
    add_config_retValue = true;
    add_config_counter = 0;

    remove_config_retValue = true;
    remove_config_counter = 0;
  }

  bool enterReset() {
    ++enter_reset_counter;
    return true;
  }

  bool exitReset() {
    if (enter_reset_counter == exit_reset_counter + 1) {
      exit_reset_called_after_enter_reset = true;
    }
    ++exit_reset_counter;
    return true;
  }

  // member variables:
public:
  bool add_config_retValue{true};
  bool update_config_retValue{true};
  int add_config_counter{0};
  int add_update_counter{0};

  bool remove_config_retValue{true};
  int remove_config_counter{0};

  int enter_reset_counter{0};
  int exit_reset_counter{0};
  bool exit_reset_called_after_enter_reset = false;
};

} // namespace plugins
} // namespace next

#endif // NEXT_PLUGIN_MOCK_ERROR_HANDLING_H_
