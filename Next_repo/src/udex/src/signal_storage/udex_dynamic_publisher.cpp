/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     udex_dynamic_publisher.cpp
 * @brief    ínternal ecal publisher adapted to also propagate signal descriptions
 * as well as meta information on a single copy action
 */

#include "udex_dynamic_publisher.h"

#include <next/sdk/sdk.hpp>
#include <sstream>

namespace next {
namespace udex {
namespace ecal_util {

static int g_timeout_ms = 5000;
static int safe_last_timeout_ms = 5000;

void DynamicPublisher::SetTimeout(int timeout_ms) const {
  debug(__FILE__, "timeout changed to {}", timeout_ms);
  g_timeout_ms = timeout_ms;
}
void DynamicPublisher::SetSyncSend(bool send_sync) const {
  debug(__FILE__, "SyncMode changed to {} (true being enabled)", send_sync);
  if (send_sync) {
    g_timeout_ms = safe_last_timeout_ms;
  } else {
    safe_last_timeout_ms = g_timeout_ms;
    g_timeout_ms = 0;
  }
}

DynamicPublisher::DynamicPublisher(bool intra_process) {
  if (intra_process) {
    eCAL::Util::EnableLoopback(intra_process);
  }
}

DynamicPublisher::~DynamicPublisher() {
  if (forced_registration_update_) {
    std::string topic_name = CMsgPublisher::GetTopicName();
    if (topic_name != "") {
      next::sdk::events::UpdateTopicCacheMessage update_msg;
      update_msg.topics.push_back(topic_name);
      update_topic_cache_event_.publish(update_msg);
    }
  }
}

bool DynamicPublisher::CreateTopic(const std::string &topic_name, const SignalDescription &description,
                                   bool registrationUpdate) {
  forced_registration_update_ = registrationUpdate;
  bool ret = CreateTopic(topic_name, description);
  if (registrationUpdate) {
    next::sdk::events::UpdateTopicCacheMessage update_msg;
    update_topic_cache_event_.publish(update_msg);
  }
  return ret;
}

bool DynamicPublisher::CreateTopic(const std::string &topic_name, const SignalDescription &description) {
  description_handler.SetBinarySignalDescription(description.binary_data_description);
  description_handler.SetBasicInformation(description.basic_info);
  eCAL::SDataTypeInformation topic_info = GetDataTypeInformation();

  return (CMsgPublisher<TopicSendMessage>::Create(topic_name, topic_info));
}

size_t DynamicPublisher::SendSynchronizedCopied(const TopicSendMessage &msg, long long acknowledge_timeout_ms_) {
  return Send(msg, (long long)msg.meta.mts_timestamp, acknowledge_timeout_ms_);
}

bool DynamicPublisher::SendTopic(const TopicSendMessage &msg) {
  auto ecal_sent_size = SendSynchronizedCopied(msg, g_timeout_ms);
  auto message_size = msg.package_info_size + msg.binary_blob_size + sizeof(msg);
  if (ecal_sent_size < message_size && this->IsSubscribed()) {
    error(__FILE__, " eCAL Publishing failed");
    return false;
  }
  return true;
}

bool DynamicPublisher::ResetTopic(void) {
  if (forced_registration_update_) {
    next::sdk::events::UpdateTopicCacheMessage update_msg;
    update_msg.topics.push_back(CMsgPublisher::GetTopicName());
    update_topic_cache_event_.publish(update_msg);
  }
  if (!CMsgPublisher<TopicSendMessage>::Destroy()) {
    return false;
  }
  forced_registration_update_ = false;
  return description_handler.Reset();
}

int DynamicPublisher::getCurrentMsgNumber() { return ++msg_number_; }

SignalDescription DynamicPublisher::GetSignalDescription(void) const { return description_handler.GetDescription(); }

size_t DynamicPublisher::GetSize(const TopicSendMessage &obj) const {
  return obj.binary_blob_size + obj.package_info_size + sizeof(InternalMessageStruct);
}

bool DynamicPublisher::Serialize(const TopicSendMessage &obj, char *buffer, size_t size) const {
  TopicHeader header;
  header.magic_value = topic_header_magic_value;
  header.meta = obj.meta;
  header.blob_size = obj.binary_blob_size;
  header.package_info_size = obj.package_info_size;
  header.package_info_offset = 0;
  header.blob_offset = header.package_info_offset + obj.package_info_size;

  InternalMessageStruct *msg = (InternalMessageStruct *)buffer;
  msg->header = header;

  if (msg->blob - (char *)msg + header.package_info_offset + obj.package_info_size > size) {
    return false;
  }
  if (obj.package_info_size > 0) {
    memcpy(msg->blob + header.package_info_offset, obj.package_info_in, obj.package_info_size);
  }
  if (msg->blob - (char *)msg + header.blob_offset + obj.binary_blob_size > size) {
    return false;
  }
  if (obj.binary_blob_size > 0) {
    memcpy(msg->blob + header.blob_offset, obj.binary_blob_in, obj.binary_blob_size);
  }
  return true;
}

eCAL::SDataTypeInformation DynamicPublisher::GetDataTypeInformation() const {
  eCAL::SDataTypeInformation info;
  auto type_name = description_handler.GetParentUrl();
  if (type_name == "") {
    info.name = "UdexDynamicTopic_NotDefined";
  } else {
    const auto now = std::chrono::system_clock::now();
    std::stringstream ss;
    ss << "next:" << type_name << "<"
       << std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() << ">";
    info.name = ss.str();
  }
  info.descriptor = description_handler.ToString();
  info.encoding = "";
  return info;
}

} // namespace ecal_util
} // namespace udex
} // namespace next
