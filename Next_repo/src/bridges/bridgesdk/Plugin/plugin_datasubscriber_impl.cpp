/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datasubscriber_impl.cpp
 * @brief    implements the datapublisher for the plugin
 **/

#include "plugin_datasubscriber_impl.hpp"
#include "plugin_private.h"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginDataSubscriberImpl::PluginDataSubscriberImpl(next::bridgesdk::plugin::Plugin *base_plugin) {
  base_plugin_ = base_plugin;
}

size_t PluginDataSubscriberImpl::subscribeUrl(const std::string &url_name,
                                              [[maybe_unused]] const bridgesdk::SubscriptionType subscription_type,
                                              bridgesdk::pluginCallbackT &sub_callback) {
  return base_plugin_->impl_->subscribeUrl(url_name, sub_callback);
}

void PluginDataSubscriberImpl::unsubscribe(const size_t id) { return base_plugin_->impl_->unsubscribe(id); }

bool PluginDataSubscriberImpl::checkConnectionWait(const std::string &url_name, size_t timeout_ms) const {
  return base_plugin_->impl_->checkConnectionWait(url_name, timeout_ms);
}

std::map<size_t, next::udex::subscriber::SubscriptionInfo> PluginDataSubscriberImpl::getSubscriptionsInfo() {
  return base_plugin_->impl_->getSubscriptionsInfo();
}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next
