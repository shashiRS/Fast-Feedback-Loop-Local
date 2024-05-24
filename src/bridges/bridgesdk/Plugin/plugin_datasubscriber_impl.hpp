/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datasubscriber_impl.hpp
 * @brief    implements the datapublisher for the plugin
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_DATASUBSCRIBER_IMPL_H_
#define NEXT_BRIDGESDK_PLUGIN_DATASUBSCRIBER_IMPL_H_

#include <next/udex/subscriber/data_subscriber.hpp>

#include <next/bridgesdk/plugin.h>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class PluginDataSubscriberImpl {

private:
  PluginDataSubscriberImpl() = delete;

public:
  PluginDataSubscriberImpl(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginDataSubscriberImpl() = default;

  size_t subscribeUrl(const std::string &url_name, const bridgesdk::SubscriptionType subscription_type,
                      bridgesdk::pluginCallbackT &sub_callback);

  void unsubscribe(const size_t id);

  std::map<size_t, next::udex::subscriber::SubscriptionInfo> getSubscriptionsInfo();
  bool checkConnectionWait(const std::string &url_name, size_t timeout_ms) const;
  next::bridgesdk::plugin::Plugin *base_plugin_{};
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_DATASUBSCRIBER_IMPL_H_
