/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     udex_dynamic_subscriber.h
 * @brief
 */

#ifndef NEXT_UDEX_UDEX_DYNAMIC_SUBSCRIBER_H
#define NEXT_UDEX_UDEX_DYNAMIC_SUBSCRIBER_H

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include <ecal/ecal.h>
#include <ecal/msg/subscriber.h>

#include "signal_description_handler.h"
#include "signal_storage_types.h"
#include "signal_watcher.h"

namespace next {
namespace udex {
namespace ecal_util {

struct TopicReceiveMessage {
  MetaInfo meta;
  std::vector<char> binary_blob;
  std::vector<char> package_info;
};

class DynamicSubscriber : private eCAL::CMsgSubscriber<TopicReceiveMessage> {
public:
  DynamicSubscriber(bool intra_process);
  virtual ~DynamicSubscriber();

  DynamicSubscriber(const DynamicSubscriber &) = delete;
  DynamicSubscriber &operator=(const DynamicSubscriber &) = delete;
  DynamicSubscriber(DynamicSubscriber &&) = default;
  DynamicSubscriber &operator=(DynamicSubscriber &&) = default;

  bool SubscribeToUrl(std::string url, MsgReceiveCallbackT callback_);
  bool Reset(void);

  bool checkSubscriptionActive(void);
  SignalDescription GetSignalDescription(bool &found_url) const;
  std::string getSubscriptionUrl(void) const;
  size_t GetPublisherCount() const;

protected:
  virtual eCAL::SDataTypeInformation GetDataTypeInformation() const override;

private:
  std::string subscription_url_ = "";
  SignalDescriptionHandler desc_handler_;
  MsgReceiveCallbackT url_callback_ = {};
  std::atomic<bool> subscription_active_ = false;
  std::atomic<bool> dummy_subscription_active_ = false;
  SignalWatcher &signal_watcher_ = SignalWatcher::GetInstance();
  SignalDescription signal_description_ = {};
  mutable std::mutex subscription_url_mtx_;
  bool Deserialize(TopicReceiveMessage &obj, const void *buffer_, size_t size_) const override;
  void SubscribeToTopic(std::string topic_name, SignalDescriptionHandler topic_description_handler,
                        MsgReceiveCallbackT callback_, bool dummy_subscription);
  mutable std::recursive_mutex my_request_ids__mtx_;
  std::vector<size_t> my_request_ids_;
};

} // namespace ecal_util
} // namespace udex
} // namespace next
#endif // NEXT_UDEX_UDEX_DYNAMIC_SUBSCRIBER_H
