/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_UDEX_DATA_SUBSCRIBER_IMPL_H
#define NEXT_UDEX_DATA_SUBSCRIBER_IMPL_H

#include <ecal/ecal.h>

#include <atomic>
#include <chrono>
#include <map>
#include <thread>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

#include "../signal_storage/signal_watcher.h"
#include "../signal_storage/udex_dynamic_subscriber.h"

namespace next {
namespace udex {
namespace subscriber {

class DataSubscriberImpl {
public:
  DataSubscriberImpl(const std::string &data_subscriber_name);
  virtual ~DataSubscriberImpl();

  bool Reset(void);

  std::string getName();

  void OnReceiveCallback(const char *topic_name, const ecal_util::TopicReceiveMessage &msg, long long time,
                         long long id, std::string url, std::shared_ptr<extractor::DataExtractor> dataExtractor);
  size_t Subscribe(const std::string &url, std::shared_ptr<extractor::DataExtractor> dataExtractor,
                   const bool force_subscription);
  void Unsubscribe(const size_t id);
  std::map<size_t, SubscriptionInfo> getSubscriptionsInfo();
  bool IsConnected(const std::string &url);

private:
  void deleteSubscribers(void);

  std::string data_subscriber_name_;

  std::map<size_t, std::unique_ptr<ecal_util::DynamicSubscriber>> subscribers_;
  std::map<std::string, size_t> topic_to_ecal_subscriber_;

  std::atomic<size_t> ecal_subscriber_id_ = 0;
  std::atomic<size_t> url_subscribtion_id_ = 0;

  // map of extractors interested in the topic, and count the number of URL subscriptions for each topic
  std::map<std::string, std::map<std::shared_ptr<extractor::DataExtractor>, size_t>> topic_to_extractors_counter_;

  std::map<size_t, SubscriptionInfo> subscriptions_info_;

  // std::mutex subscription_mtx_;
  TracyLockable(std::mutex, subscription_mtx_);
  std::unique_ptr<explorer::SignalExplorer> signal_explorer_ = std::make_unique<explorer::SignalExplorer>();
};

} // namespace subscriber
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_DATA_SUBSCRIBER_IMPL_H
