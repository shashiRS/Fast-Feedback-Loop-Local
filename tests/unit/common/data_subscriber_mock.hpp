/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     data_subscriber_mock.hpp
 * @brief    mock of the Data Subscriber
 *
 **/

#ifndef DATA_SUBSCRIBER__MOCK_H_
#define DATA_SUBSCRIBER__MOCK_H_

#include <map>
#include <thread>

#include <next/sdk/sdk_macros.h>

#include <next/udex/subscriber/data_subscriber.hpp>

namespace next {
namespace test {

NEXT_DISABLE_DEPRECATED_WARNING
class DataSubscriberMock : public next::udex::subscriber::DataSubscriber {
private:
  size_t subscriptions_ = 0; // counter of created subscriptions (not current active subscriptions!)
  std::map<size_t, next::udex::subscriber::SubscriptionInfo>
      active_subscriptions_; // the active subscriptions, identified by its id
  // std::map<size_t, next::udex::SubscriptionInfo> subInfo_{};
  std::vector<std::string> topicByUrl_{};

public:
  //! implementation of the interface function
  DataSubscriberMock(const std::string &node_name_) : DataSubscriber(node_name_) {}
  ~DataSubscriberMock() {}

  size_t Subscribe(const std::string &user_url, std::shared_ptr<next::udex::extractor::DataExtractor> dataExtractor,
                   const bool = true) override {

    next::udex::subscriber::SubscriptionInfo subscription_info;
    subscription_info.data_extractor = dataExtractor;
    subscription_info.ecal_subscriber_id = 0;
    subscription_info.topic = user_url;
    subscription_info.url = user_url;

    active_subscriptions_[++subscriptions_] = subscription_info;

    // subInfo_[++subscriptions_] = {user_url, SubscriptionInfo};
    return subscriptions_;
  }

  void Unsubscribe(const size_t id) override { active_subscriptions_.erase(id); }

  std::map<size_t, next::udex::subscriber::SubscriptionInfo> getSubscriptionsInfo() override {
    return active_subscriptions_;
  }

  size_t getNumberOfSubscriptionsForUrl(std::string url) {
    size_t count = 0;
    for (const auto &subscription : active_subscriptions_) {
      if (url == subscription.second.url) {
        count++;
      }
    }
    return count;
  }

  //! faking that some data was received, forwards the data to the plugin
  /*!
   * This funciton is for faking that the data source, e.g. a player, is providing data. The function simply forwards
   * the data to the subscribed data receivers.
   * Note: The mock function is not having a thread doing this, the caller thread of this function is calling the
   * callback functions.
   */
  void forwardData(const std::string &topic, const bridgesdk::ChannelMessagePointers &data) {
    for (const auto &subscription : active_subscriptions_) {
      if (subscription.second.topic == topic) {

        next::udex::UdexDataPackage udex_data_package;
        udex_data_package.Data = data.data;
        udex_data_package.Size = data.size;
        udex_data_package.info.PackageUrl = data.channel_name;
        udex_data_package.info.Time = 0;

        subscription.second.data_extractor->PushData(topic, &udex_data_package);
        while (!subscription.second.data_extractor->isDataAvailable(topic)) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
      }
    }
    forwarded_messages_[topic]++;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  void forwardData(const std::string &topic, const std::vector<std::byte> &data, next::bridgesdk::time_t time = 0) {
    const bridgesdk::ChannelMessagePointers message_pointers{topic.c_str(), data.data(), data.size(), time};
    forwardData(topic, message_pointers);
  }

  //! helper function to get the total cound of forwarded messages (sum over all topics)
  unsigned int getTotalForwardedMessagesCount() const {
    unsigned int count = 0;
    std::for_each(forwarded_messages_.cbegin(), forwarded_messages_.cend(),
                  [&count](auto &entry) { count += entry.second; });
    return count;
  }

  std::map<std::string, unsigned int> forwarded_messages_; // counter for each topic of forwarded messages
};
NEXT_RESTORE_WARNINGS

} // namespace test
} // namespace next

#endif // DATA_SUBSCRIBER__MOCK_H_
