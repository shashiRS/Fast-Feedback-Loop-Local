/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datasubscriber.cpp
 * @brief    implements the datapublisher for the plugin
 **/

#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>

#include "plugin_datasubscriber_impl.hpp"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginDataSubscriber::PluginDataSubscriber(next::bridgesdk::plugin::Plugin *base_plugin) {
  impl_ = std::make_unique<PluginDataSubscriberImpl>(base_plugin);
}

size_t PluginDataSubscriber::subscribeUrl(const std::string &url_name, bridgesdk::SubscriptionType subscription_type,
                                          bridgesdk::pluginCallbackT sub_callback) {
  return impl_->subscribeUrl(url_name, subscription_type, sub_callback);
}

void PluginDataSubscriber::unsubscribe(const size_t id) const { impl_->unsubscribe(id); }

bool PluginDataSubscriber::checkConnectionWait(const std::string &url_name, size_t timeout_ms) const {
  return impl_->checkConnectionWait(url_name, timeout_ms);
}

std::map<size_t, next::udex::subscriber::SubscriptionInfo> PluginDataSubscriber::getSubscriptionsInfo() {
  return impl_->getSubscriptionsInfo();
}

PluginDataSubscriber::~PluginDataSubscriber() {}
} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next
