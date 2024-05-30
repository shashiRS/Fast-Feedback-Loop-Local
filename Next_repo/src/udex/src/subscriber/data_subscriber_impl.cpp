/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     data_subscriber_impl.cpp
 * @brief    matches the old Udex implementation to the new interface with
 *           regards to the ECAl middleware
 */

#include "data_subscriber_impl.h"

#include <mutex>

#include <next/sdk/logger/logger.hpp>

#include <next/udex/data_types.hpp>

namespace next {
namespace udex {
namespace subscriber {

DataSubscriberImpl::DataSubscriberImpl(const std::string &data_subscriber_name)
    : data_subscriber_name_(data_subscriber_name) {
  ProfileUdex_OV;
}

DataSubscriberImpl::~DataSubscriberImpl() {
  ProfileUdex_OV;
  deleteSubscribers();
}

bool DataSubscriberImpl::Reset(void) {
  ProfileUdex_OV;
  deleteSubscribers();

  topic_to_extractors_counter_.clear();
  subscriptions_info_.clear();

  ecal_subscriber_id_ = 0;
  url_subscribtion_id_ = 0;

  return true;
}

std::string DataSubscriberImpl::getName() { return data_subscriber_name_; }

void DataSubscriberImpl::OnReceiveCallback(const char *topic_name, const ecal_util::TopicReceiveMessage &msg,
                                           long long time, [[maybe_unused]] long long id, std::string url,
                                           [[maybe_unused]] std::shared_ptr<extractor::DataExtractor> dataExtractor) {
  ProfileUdex_FLL;

  std ::lock_guard<LockableBase(std::mutex)> lock_receive_callback(subscription_mtx_);

  UdexDataPackage udexDataPackage;

  // no dynamic package send -> fallback solution for raw packages
  if (!msg.meta.dynamic_package) {
    udexDataPackage.Data = msg.binary_blob.data();
    udexDataPackage.Size = msg.binary_blob.size();
    udexDataPackage.info.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN;
    udexDataPackage.info.Time = (size_t)time;
    udexDataPackage.info.processor_required = false;
    udexDataPackage.info.PackageUrl = url;
  } else {
    udexDataPackage.Data = msg.binary_blob.data();
    udexDataPackage.Size = msg.binary_blob.size();
    udexDataPackage.info.format_type = msg.meta.format_type;
    udexDataPackage.info.Time = msg.meta.mts_timestamp;
    udexDataPackage.info.processor_required = msg.meta.processor_required;
    udexDataPackage.info.PackageUrl = url;
  }

  for (auto &topic_to_extractor : topic_to_extractors_counter_[topic_name]) {
    topic_to_extractor.first->PushData(topic_name, &udexDataPackage);
  }
}

size_t DataSubscriberImpl::Subscribe(const std::string &url, std::shared_ptr<extractor::DataExtractor> dataExtractor,
                                     const bool force_subscription) {
  ProfileUdex_OV;
  std::lock_guard<LockableBase(std::mutex)> lock_subscribe(subscription_mtx_);

  auto real_topic = signal_explorer_->GetPackageUrl(url);
  if (real_topic.size() == 0) {
    if (force_subscription) {
      real_topic.push_back(url);
    }
  } else {
    if (real_topic[0] == "" &&
        force_subscription) { // getTopicByUrl failed, topic not yet in the database due to other issues,
      // trust that the publisher exists and subscibe anyway
      real_topic[0] = url;
    }
    // check if there is an exact match and set it to first pos
    if (real_topic.size() > 1) {
      for (const auto &topic : real_topic) {
        if (topic == url) {
          real_topic[0] = topic;
          break;
        }
      }
    }
  }

  // only subscribe to topics that dont exist in the map already
  if (topic_to_extractors_counter_.count(real_topic[0]) == 0) {
    // Create a subscriber (to the topic 'real_topic')
    debug(__FILE__, " Subscribe to url: {0}", real_topic[0]);
    std::unique_ptr<ecal_util::DynamicSubscriber> subscriber_handle =
        std::make_unique<ecal_util::DynamicSubscriber>(true);

    auto callb = std::bind(&DataSubscriberImpl::OnReceiveCallback, this, std::placeholders::_1, std::placeholders::_2,
                           std::placeholders::_3, std::placeholders::_4, url, dataExtractor);
    auto sub_active_direct = subscriber_handle->SubscribeToUrl(real_topic[0], callb);

    if (sub_active_direct) {
      debug(__FILE__, " Adding succeeded and connected directly.");
    } else {
      debug(__FILE__, " Adding succeeded and waiting for topic.");
    }

    ecal_subscriber_id_++;
    subscribers_[ecal_subscriber_id_] = std::move(subscriber_handle);
    topic_to_ecal_subscriber_[real_topic[0]] = ecal_subscriber_id_;
  }

  // find ecalID for respective topic

  url_subscribtion_id_++;

  SubscriptionInfo subscription_info;
  subscription_info.ecal_subscriber_id = topic_to_ecal_subscriber_[real_topic[0]];
  subscription_info.data_extractor = dataExtractor;
  subscription_info.url = url;
  subscription_info.topic = real_topic[0];

  subscriptions_info_[url_subscribtion_id_] = subscription_info;

  topic_to_extractors_counter_[real_topic[0]][dataExtractor]++;

  dataExtractor->setDefaultSync(url);

  return url_subscribtion_id_;
}

void DataSubscriberImpl::Unsubscribe(const size_t id) {
  ProfileUdex_OV;
  std::lock_guard<LockableBase(std::mutex)> lock_unsubscribe(subscription_mtx_);

  if (subscriptions_info_.find(id) != subscriptions_info_.end()) {
    auto subscription_to_delete = subscriptions_info_[id];

    if (topic_to_extractors_counter_.find(subscription_to_delete.topic) != topic_to_extractors_counter_.end() &&
        topic_to_extractors_counter_[subscription_to_delete.topic].find(subscription_to_delete.data_extractor) !=
            topic_to_extractors_counter_[subscription_to_delete.topic].end()) {
      auto no_of_subscriptions =
          --topic_to_extractors_counter_[subscription_to_delete.topic][subscription_to_delete.data_extractor];
      if (no_of_subscriptions ==
          0) { // if there are no more URL subscription for this topic, for this extractor,  remove the extractor
        topic_to_extractors_counter_[subscription_to_delete.topic].erase(subscription_to_delete.data_extractor);
        // if there are no more extractors for this topic, remove the topic
        if (topic_to_extractors_counter_[subscription_to_delete.topic].empty()) {
          topic_to_extractors_counter_.erase(subscription_to_delete.topic);

          auto it = subscribers_.find(subscription_to_delete.ecal_subscriber_id);
          if (it != subscribers_.end()) {
            it->second->Reset(); // destroy subscriber
            subscribers_.erase(it);
          }
        }
      }
      subscriptions_info_.erase(id);
    }
  }
}

std::map<size_t, SubscriptionInfo> DataSubscriberImpl::getSubscriptionsInfo() {
  std::lock_guard<LockableBase(std::mutex)> lock_subscription_info(subscription_mtx_);
  return subscriptions_info_;
}

bool DataSubscriberImpl::IsConnected(const std::string &url) {
  for (const auto &[_, subscriber] : subscribers_) {
    static_cast<void>(_); // Ignore the key, suppress linux warning
    if (subscriber->getSubscriptionUrl() == url) {
      return subscriber->checkSubscriptionActive() && subscriber->GetPublisherCount() > 0;
    }
  }
  return false;
}

void DataSubscriberImpl::deleteSubscribers(void) {
  std::lock_guard<LockableBase(std::mutex)> lock_subscription_info(subscription_mtx_);
  if (!subscribers_.empty()) {
    for (auto &subscriber : subscribers_) {
      subscriber.second->Reset();
    }
    subscribers_.clear();
  }
}

} // namespace subscriber
} // namespace udex
} // namespace next
