/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_concurrency_mock.hpp
 * @brief    A mock plugin for testing mocked ...
 *
 * This is an rudimentary plugin like class to check if addConfig and removeConfig is only
 * called sequential and not concurrent. It's not configured to receive/send data.
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

#ifndef NEXT_PLUGIN_MOCK_CONCURRENCY_H_
#define NEXT_PLUGIN_MOCK_CONCURRENCY_H_

namespace next {
namespace plugins {
// adding the helper_functions.h causes linker error, so created a test function here
// TODO fix the link error while including the helper_functions.h by adding the lib in cmake
bool GetValueFromPluginConfigTest(const next::bridgesdk::plugin_config_t &config, const std::string &key,
                                  std::string &value) {
  auto url_find_it = config.find(key);
  if (url_find_it == config.end()) {
    value = "";
    return false;
  }
  value = url_find_it->second.value;
  return true;
}

size_t k_WaitTimeMs = 250;

class MockConcurrencyPlugin : public next::bridgesdk::plugin::Plugin {

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

public:
  MockConcurrencyPlugin(const char *path) : bridgesdk::plugin::Plugin(path) {
    this->setVersion(std::string("0.0.1"));
    this->setName(std::string("Basic Concurrency Plugin"));
  }

  bool init() override {
    plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
    plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
    plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
    plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
    plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);

    return true;
  }

  bool enterReset() override {

    if (0 != reset_config_status) {
      concurrencyRemoveCall = true;
    }
    reset_config_status = 1;

    // wait to check if plugin is called concurrent
    std::this_thread::sleep_for(std::chrono::milliseconds(k_WaitTimeMs));

    reset_config_status = 0;

    reset_config_counter++;
    return reset_config = true;
  }

  //! Set the config send by GUI to be used in plugin's code to subscribe to topic or extracting data from payload.
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override {
    start_time_.push_back(std::chrono::system_clock::now());

    check_thread_safety_++;
    if (0 != add_config_status) {
      concurrencyAddCall = true;
    }
    add_config_status = 1;

    std::string str_timeout_in_ms{};
    if (GetValueFromPluginConfigTest(config, "timeoutInMs", str_timeout_in_ms) == true) {
      // increase wait time to check if plugin is called concurrent
      uint64_t i_timeout_in_ms = stoull(str_timeout_in_ms);
      std::this_thread::sleep_for(std::chrono::milliseconds(i_timeout_in_ms));
    } else {
      // wait to check if plugin is called concurrent
      std::this_thread::sleep_for(std::chrono::milliseconds(k_WaitTimeMs));
    }

    add_config_status = 0;
    add_config_counter++;
    return (added_config = true);
  } // simulate if it was successfull

  //! Remove current active config used in plugin.
  bool removeConfig([[maybe_unused]] const next::bridgesdk::plugin_config_t &config) override {
    check_thread_safety_--;
    if (0 != remove_config_status) {
      concurrencyRemoveCall = true;
    }
    remove_config_status = 1;

    // wait to check if plugin is called concurrent
    std::this_thread::sleep_for(std::chrono::milliseconds(k_WaitTimeMs));

    remove_config_status = 0;

    remove_config_counter++;
    return (removed_config = true);
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
    return true;
  }

  void reset() {
    add_config_counter = 0;
    remove_config_counter = 0;
    add_config_status = 0;
    remove_config_status = 0;
    concurrencyAddCall = false;
    concurrencyRemoveCall = false;
  }

  // member variables:
public:
  // only used for unittests
  std::atomic<bool> concurrencyAddCall = false;
  std::atomic<bool> concurrencyRemoveCall = false;
  std::atomic<bool> concurrencyResetCall = false;

  bool added_config = false;
  std::atomic<int> add_config_counter{0};
  std::atomic<int> add_config_status{0};

  bool reset_config = false;
  std::atomic<int> reset_config_counter{0};
  std::atomic<int> reset_config_status{0};

  // only used for unittests
  bool removed_config = false;
  std::atomic<int> remove_config_counter{0};
  std::atomic<int> remove_config_status{0};

  std::uint32_t check_thread_safety_{0};
  std::vector<std::chrono::time_point<std::chrono::system_clock>> start_time_{};
};

} // namespace plugins
} // namespace next

#endif // NEXT_PLUGIN_MOCK_CONCURRENCY_H_
