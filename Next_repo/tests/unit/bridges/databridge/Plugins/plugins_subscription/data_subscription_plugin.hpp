/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     data_subscription_plugin.hpp
 * @brief    A mock plugin for testing data subscriptions
 *
 * Note: The create_plugin function is not provided! Using via the plugin manager is not possible!
 *
 **/

#include <atomic>
#include <iostream>
#include <vector>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>

#ifndef NEXT_PLUGIN_MOCK_H_
#define NEXT_PLUGIN_MOCK_H_

namespace next {
namespace plugins {

class DataSubscriptionTestPlugin : public next::bridgesdk::plugin::Plugin {

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;

public:
  DataSubscriptionTestPlugin(const char *path) : bridgesdk::plugin::Plugin(path) {

    this->setVersion(std::string("0.0.2"));
    this->setName(std::string("Mock Plugin"));
  }

  std::vector<std::string> data_received;

  bool init() {
    plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
    return true;
  }

  size_t subscriberManualy(std::string url, std::atomic<int> &testflag) {
    auto b = std::bind(&DataSubscriptionTestPlugin::processData, this, std::placeholders::_1, std::placeholders::_2,
                       std::ref(testflag));
    auto id = plugin_data_subscriber_->subscribeUrl(url, bridgesdk::SubscriptionType::DIRECT, b);
    return id;
  }

  void removeSubscription(size_t id) { plugin_data_subscriber_->unsubscribe(id); }

  void processData(const std::string &, const bridgesdk::ChannelMessagePointers *data, std::atomic<int> &testflag) {
    testflag++;
    data_received.push_back(std::string(static_cast<const char *>(data->data), data->size));
  }
};

} // namespace plugins
} // namespace next

#endif // NEXT_PLUGIN_MOCK_H_
