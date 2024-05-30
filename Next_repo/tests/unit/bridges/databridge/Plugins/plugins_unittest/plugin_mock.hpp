/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_mock.hpp
 * @brief    A mock plugin for testing mocked ...
 *
 * This is an rudimentary plugin like class. It is used for testing the mock functionalities for unit tests.
 * The class is expecting to get ints as input data and will add 1 to every int provided. If the amount of data is not
 * devidable by the size of int an exception is thrown.
 *
 * Note: The create_plugin function is not provided! Using via the plugin manager is not possible!
 *
 **/

#include <algorithm> // std::for_each
#ifdef _DEBUG
#include <iostream>
#endif

#include <next/sdk/sdk_macros.h>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>

#ifndef NEXT_PLUGIN_MOCK_H_
#define NEXT_PLUGIN_MOCK_H_

namespace next {
namespace plugins {

NEXT_DISABLE_DEPRECATED_WARNING
class MockPlugin : public next::bridgesdk::plugin::Plugin {

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;

public:
  MockPlugin(const char *path) : bridgesdk::plugin::Plugin(path) {
    this->setVersion(std::string("0.0.2"));
    this->setName(std::string("Mock Plugin"));
  }

  bool init() override {
    plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
    plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
    plugin_3D_view_ = std::make_shared<next::bridgesdk::plugin_addons::Plugin3dView>(this);
    auto b = std::bind(&MockPlugin::processData, this, std::placeholders::_1, std::placeholders::_2);
    if (0 != plugin_data_subscriber_->subscribeUrl(mock_url_, bridgesdk::SubscriptionType::DIRECT, b)) {
      return true;
    } else {
      return false;
    }
  }

  void callDummySend(int testnumber) {
    auto data_publisher = next::bridgesdk::plugin_addons::PluginDataPublisher(this);
    data_publisher.sendResultData((const uint8_t *)&testnumber, sizeof(int));
  }

  //! we expect a list of sensor names here
  bool update(std::vector<std::string> user_input) override {
    std::vector<bridgesdk::plugin_addons::uid_t> new_ids;
    for (const auto &input : user_input) {
      if (input.size() != 0) {
        bridgesdk::plugin::SensorInfoForGUI info;
        info.name = input;
        info.type = "TP";
        info.color = "#00FF00";
        info.stream = "objectData";
        info.referenceCoordinateSystem = "CS";
        info.coordinateSystem.translationX = -1.1f;
        info.coordinateSystem.translationY = -1.2f;
        info.coordinateSystem.translationZ = -1.3f;
        info.coordinateSystem.rotationX = -1.4f;
        info.coordinateSystem.rotationY = -1.5f;
        info.coordinateSystem.rotationZ = -1.6f;

        const auto id = plugin_3D_view_->registerAndPublishSensor(info);
        if (0 != id) {
          new_ids.push_back(id);
        }
      }
    }
    std::cout << std::endl;
    for (auto old_id_it = sensor_ids_.begin(); sensor_ids_.end() != old_id_it;) {
      if (std::find(new_ids.begin(), new_ids.end(), *old_id_it) == new_ids.end()) {
        plugin_3D_view_->unregisterAndPublishSensor(*old_id_it);
        old_id_it = sensor_ids_.erase(old_id_it);
      } else {
        old_id_it++;
      }
    }
    for (const auto &new_id : new_ids) {
      if (std::find(sensor_ids_.begin(), sensor_ids_.end(), new_id) == sensor_ids_.end()) {
        sensor_ids_.push_back(new_id);
      }
    }
    return true;
  }

  void processData(const std::string &topic, const bridgesdk::ChannelMessagePointers *data) {
    if (topic != "") {
#ifdef _DEBUG
      std::cout << "data received for topic " << topic << std::endl;
#endif
      if (data->size % sizeof(int) != 0) {
#ifdef _DEBUG
        std::cout << "received " << data->size << " bytes, wich cannot be devided by " << sizeof(int) << std::endl;
#endif
        throw std::invalid_argument("Incorrect amount of bytes provided");
      }
      received_messages_count[topic]++;
      const size_t count = data->size / sizeof(int);
      const int *in = reinterpret_cast<const int *>(data->data);
      std::vector<int> ints(in, in + count);
      std::for_each(ints.begin(), ints.end(), [](int &i) { ++i; });
      plugin_publisher_->sendResultData((uint8_t *)ints.data(), ints.size() * sizeof(ints[0]));
    }
  }

  //! Get the count of received messages
  /*
   * Just a helper function which provides the sum of received messages of all topics.
   */
  unsigned int getTotalReceivedMessagesCount() const {
    unsigned int count = 0;
    std::for_each(received_messages_count.cbegin(), received_messages_count.cend(),
                  [&count](auto &entry) { count += entry.second; });
    return count;
  }

  std::map<std::string, unsigned int> received_messages_count; // count of received messages per topic
  const std::string mock_url_ = "MockUrl";                     // the url this mock plugin is subscribing to

  std::vector<bridgesdk::plugin_addons::uid_t> sensor_ids_;

  // only used for unittests
  bool added_config = false;

  std::atomic<int> add_config_counter{0};
  //! Set the config send by GUI to be used in plugin's code to subscribe to topic or extracting data from payload.
  bool addConfig(const next::bridgesdk::plugin_config_t &) override {
    add_config_counter++;
    return added_config = true;
  } // simulate if it was successfull

  std::atomic<int> remove_config_counter{0};

  // only used for unittests
  bool removed_config = false;
  //! Remove current active config used in plugin.
  bool removeConfig(const next::bridgesdk::plugin_config_t &) override {
    remove_config_counter++;
    return removed_config = true;
  }

  //! Get possible configuration description
  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) override {

    config_fields.insert({"url", {next::bridgesdk::ConfigType::EDITABLE, "videoURL"}});
    config_fields.insert({"resolution", {next::bridgesdk::ConfigType::SELECTABLE, "1080p"}});
    config_fields.insert({"resolution", {next::bridgesdk::ConfigType::SELECTABLE, "960p"}});
    config_fields.insert({"resolution", {next::bridgesdk::ConfigType::SELECTABLE, "480p"}});
    config_fields.insert({"format", {next::bridgesdk::ConfigType::SELECTABLE, "png"}});
    config_fields.insert({"format", {next::bridgesdk::ConfigType::SELECTABLE, "jpeg"}});
    config_fields.insert({"format", {next::bridgesdk::ConfigType::SELECTABLE, "svg"}});
    return true;
  }
};
NEXT_RESTORE_WARNINGS

} // namespace plugins
} // namespace next

#endif // NEXT_PLUGIN_MOCK_H_
