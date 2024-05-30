/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datasubscriber.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_DATASUBSCRIBER_H_
#define NEXT_BRIDGESDK_PLUGIN_DATASUBSCRIBER_H_

#include <map>
#include <memory>
#include <string>

#include <next/sdk/sdk_macros.h>

#include <next/bridgesdk/plugin.h>

namespace next {
namespace udex {
namespace subscriber {
struct SubscriptionInfo;
} // namespace subscriber
} // namespace udex
} // namespace next

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class PluginDataSubscriberImpl;

class DECLSPEC_plugin_config PluginDataSubscriber {
private:
  PluginDataSubscriber() = delete;

  NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
  std::unique_ptr<PluginDataSubscriberImpl> impl_;
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  PluginDataSubscriber(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginDataSubscriber();

  //! Adds a new URL subscription
  /*!
   * Forwards the URL subscription request to the backend. When new data for the URL is available
   * the sub_callback is called. If URL is split up into multiple ports and signals there can be
   * multiple subscription_callbacks for different data packages.
   *
   * Internally the ids of the subscribed urls are stored in the hidden implementation. On destruction
   * for all of these the unsubscribe function is called.
   *
   * @param url_name
   * @param description_type
   * @param sub_callback
   * @return id of the subscription, needed for unsubscribing
   */
  virtual size_t subscribeUrl(const std::string &url_name, bridgesdk::SubscriptionType subscription_type,
                              bridgesdk::pluginCallbackT sub_callback);

  //! Removes a URL subscription, referenced by the id
  /*!
   * @param id id of the subscription
   */
  virtual void unsubscribe(size_t id) const;

  std::map<size_t, next::udex::subscriber::SubscriptionInfo> getSubscriptionsInfo();
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_DATASUBSCRIBER_H_
