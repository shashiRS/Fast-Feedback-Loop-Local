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

#include "udex_dynamic_subscriber.h"
#include "signal_description_handler.h"
#include "signal_storage_types.h"

namespace next {
namespace udex {
namespace ecal_util {

DynamicSubscriber::DynamicSubscriber(bool intra_process) {

  if (intra_process) {
    eCAL::Util::EnableLoopback(intra_process);
  }
}

DynamicSubscriber::~DynamicSubscriber() {
  std::lock_guard<std::recursive_mutex> lock(my_request_ids__mtx_);
  signal_watcher_.removeRequests(my_request_ids_);
  Reset();
}

SignalDescription DynamicSubscriber::GetSignalDescription(bool &found_url_) const {
  found_url_ = this->subscription_active_;
  if (!found_url_) {
    return {};
  }

  return desc_handler_.GetDescription();
}

void DynamicSubscriber::SubscribeToTopic(std::string topic_name, SignalDescriptionHandler topic_description_handler,
                                         MsgReceiveCallbackT callback_, bool dummy_subscription) {
  std::lock_guard<std::recursive_mutex> req_idlock(my_request_ids__mtx_);
  desc_handler_ = topic_description_handler;

  // if a subscriber is already created handle the proper cleanup here.
  if (this->dummy_subscription_active_) {
    if (topic_name == subscription_url_) {
      // adapt topic description only as the name is matching already
      eCAL::SDataTypeInformation topic_info = GetDataTypeInformation();
      this->eCAL::CMsgSubscriber<TopicReceiveMessage>::SetAttribute("Connection", "valid");
      this->dummy_subscription_active_ = false;
      return;
    } else {
      // Subscriber is not matching actual topic name -> destroy it and create proper subscriber
      this->eCAL::CMsgSubscriber<TopicReceiveMessage>::Destroy();
    }
  }

  if (dummy_subscription) {
    eCAL::SDataTypeInformation topic_type_data;
    topic_type_data.descriptor = "";
    topic_type_data.name = "DummyDynamicSubscriber";
    topic_type_data.encoding = "";
    this->eCAL::CMsgSubscriber<TopicReceiveMessage>::Create(topic_name, topic_type_data);
    dummy_subscription_active_ = true;
  } else {
    eCAL::SDataTypeInformation topic_info = GetDataTypeInformation();
    this->eCAL::CMsgSubscriber<TopicReceiveMessage>::Create(topic_name, topic_info);
    this->dummy_subscription_active_ = false;
  }
  this->subscription_active_ = true;

  this->eCAL::CMsgSubscriber<TopicReceiveMessage>::AddReceiveCallback(callback_);
}

bool DynamicSubscriber::Reset(void) {
  std::lock_guard<std::mutex> lock(subscription_url_mtx_);
  this->subscription_active_ = false;
  this->subscription_url_ = "";
  this->url_callback_ = {};
  if (this->eCAL::CMsgSubscriber<TopicReceiveMessage>::IsCreated()) {
    return this->eCAL::CMsgSubscriber<TopicReceiveMessage>::Destroy();
  } else {
    return true;
  }
}

std::string DynamicSubscriber::getSubscriptionUrl(void) const {
  std::lock_guard<std::mutex> lock(subscription_url_mtx_);
  return this->subscription_url_;
}

size_t DynamicSubscriber::GetPublisherCount() const {
  return this->eCAL::CMsgSubscriber<TopicReceiveMessage>::GetPublisherCount();
}

bool DynamicSubscriber::SubscribeToUrl(std::string url, MsgReceiveCallbackT callback_) {

  // do not subscribe twice
  std::lock_guard<std::mutex> lock(subscription_url_mtx_);
  if (subscription_url_ != "") {
    return false;
  }

  this->url_callback_ = callback_;
  this->subscription_url_ = url;
  bool signal_available = false;
  auto topic_name = this->signal_watcher_.getTopicName(url, signal_available);
  auto topic_desc = this->signal_watcher_.getTopicDescriptionHandler(url, signal_available);

  if (topic_name == "" || topic_name.empty()) {
    signal_available = false;
  }

  if (signal_available) {
    DynamicSubscriber::SubscribeToTopic(topic_name, topic_desc, callback_, false);
    return true;
  } else {
    SignalDescriptionHandler desc_handler;
    DynamicSubscriber::SubscribeToTopic(url, desc_handler, callback_, true);
    auto bindcallback = std::bind(&DynamicSubscriber::SubscribeToTopic, this, std::placeholders::_1,
                                  std::placeholders::_2, callback_, false);
    std::lock_guard<std::recursive_mutex> req_idlock(my_request_ids__mtx_);
    my_request_ids_.push_back(this->signal_watcher_.requestUrl(url, bindcallback));
    return false;
  }
}

bool DynamicSubscriber::checkSubscriptionActive() { return !dummy_subscription_active_ && subscription_active_; }

bool SetRawPackageIntoTopicReceiveMessage(TopicReceiveMessage &obj, const void *buffer_, size_t size_) {
  obj.meta.package_id = PACKAGE_MODE_NOTSET;
  obj.meta.mts_timestamp = 0;
  obj.meta.dynamic_package = false;
  obj.meta.package_id = 0;

  // insert raw data into binary blob
  obj.binary_blob.insert(obj.binary_blob.begin(), (char *)buffer_, (char *)buffer_ + size_);

  return true;
}

bool DynamicSubscriber::Deserialize(TopicReceiveMessage &obj, const void *buffer_, size_t size_) const {

  InternalMessageStruct *msg = (InternalMessageStruct *)buffer_;

  // special check if the message is not even containing the magic number -> copy data
  if (size_ < sizeof(int)) {
    return SetRawPackageIntoTopicReceiveMessage(obj, buffer_, size_);
  }
  // magic number not recognized -> set buffer to be compatible with any message
  if (msg->header.magic_value != topic_header_magic_value) {
    return SetRawPackageIntoTopicReceiveMessage(obj, buffer_, size_);
  }

  // write data back to blob
  obj.meta = msg->header.meta;
  if (msg->blob - (char *)msg + msg->header.blob_offset + msg->header.blob_size > size_) {
    return false;
  }
  obj.binary_blob.insert(obj.binary_blob.begin(), msg->blob + msg->header.blob_offset,
                         msg->blob + msg->header.blob_offset + msg->header.blob_size);
  if (msg->blob - (char *)msg + msg->header.package_info_offset + msg->header.package_info_size > size_) {
    return false;
  }
  obj.package_info.insert(obj.package_info.begin(), msg->blob + msg->header.package_info_offset,
                          msg->blob + msg->header.package_info_offset + msg->header.package_info_size);

  return true;
}

eCAL::SDataTypeInformation DynamicSubscriber::GetDataTypeInformation() const {
  eCAL::SDataTypeInformation info;
  if (desc_handler_.GetDescription().binary_data_description.size() > 0) {
    info.name = "UdexDynamicTopic_" + desc_handler_.GetParentUrl();
  } else {
    info.name = "UdexDynamicTopic_NotDefined";
  }
  info.descriptor = "";
  info.encoding = "";
  return info;
}

} // namespace ecal_util
} // namespace udex
} // namespace next
