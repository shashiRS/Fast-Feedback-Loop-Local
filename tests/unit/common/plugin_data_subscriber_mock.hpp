/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_data_subscriber_mock.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGES_PLUGIN_DATASUBSCRIBER_MOCK_H
#define NEXT_BRIDGES_PLUGIN_DATASUBSCRIBER_MOCK_H

#include <map>

#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>

namespace next {
namespace test {

class PluginDataSubscriberMock : public next::bridgesdk::plugin_addons::PluginDataSubscriber {

public:
  PluginDataSubscriberMock(next::bridgesdk::plugin::Plugin *plugin) : PluginDataSubscriber(plugin) {}
  virtual ~PluginDataSubscriberMock() = default;

  size_t subscribeUrl(const std::string &url_name, bridgesdk::SubscriptionType,
                      bridgesdk::pluginCallbackT sub_callback) override {

    callbacks_[url_name] = sub_callback;
    return ++ids_;
  }

  void unsubscribe(size_t) const override {}

  std::map<std::string, bridgesdk::pluginCallbackT> callbacks_{};
  size_t ids_{0U};
};

} // namespace test
} // namespace next

#endif // NEXT_BRIDGES_PLUGIN_DATASUBSCRIBER_MOCK_H
