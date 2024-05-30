/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     udex_dynamic_publisher.h
 * @brief    ínternal ecal publisher adapted to also propagate signal descriptions
 * as well as meta information on a single copy action
 */

#ifndef NEXT_UDEX_UDEX_DYNAMIC_PUBLISHER_H
#define NEXT_UDEX_UDEX_DYNAMIC_PUBLISHER_H

#include <vector>

#include <ecal/ecal.h>
#include <ecal/msg/publisher.h>

#include "signal_description_handler.h"
#include "signal_storage_types.h"

#include <next/sdk/events/update_topic_cache.h>

namespace next {
namespace udex {
namespace ecal_util {

struct TopicSendMessage {
  MetaInfo meta;
  const void *binary_blob_in;
  size_t binary_blob_size;
  const void *package_info_in;
  size_t package_info_size;
};

/*!
 * @brief Publishes Data into the NEXT Simulation Network with help of binary data description
 * 1) Create Publisher with a node name and specify if intra process communication is required
 * 2) Create a topic with "Create(std::string base_url)" to create a topic for a specific url
 *   the url is the parent url of the whole binary blob. Only a single Create per Publisher is possible
 * 3) add the respective Description before sending a package
 * 4) Send a package
 */
class DynamicPublisher : public eCAL::CMsgPublisher<TopicSendMessage> {
public:
  DynamicPublisher(bool intra_process);
  virtual ~DynamicPublisher();

  DynamicPublisher(const DynamicPublisher &) = delete;
  DynamicPublisher &operator=(const DynamicPublisher &) = delete;
  DynamicPublisher(DynamicPublisher &&) = default;
  DynamicPublisher &operator=(DynamicPublisher &&) = default;

  bool CreateTopic(const std::string &topic_name, const SignalDescription &description);
  bool CreateTopic(const std::string &topic_name, const SignalDescription &description, bool registrationUpdate);

  bool SendTopic(const TopicSendMessage &msg);

  bool ResetTopic(void);

  SignalDescription GetSignalDescription(void) const;

  int getCurrentMsgNumber();

  /*!@brief set the synchronized timeout
   *
   * WATCH OUT: this is a global parameter
   *
   * @param timout_ms timout in milliseconds
   */
  void SetTimeout(int timout_ms) const;

  /*!@brief enable/disable the synchronized sending of messages
   *
   * WATCH OUT: this is a global parameter
   *
   * @param send_sync true enabling synchronized sending (true by default)
   */
  void SetSyncSend(bool send_sync) const;

protected:
  eCAL::SDataTypeInformation GetDataTypeInformation() const override;

private:
  SignalDescriptionHandler description_handler;

  size_t SendSynchronizedCopied(const TopicSendMessage &msg, long long acknowledge_timeout_ms_);
  std::vector<char> buffer_sync_send_;
  int msg_number_ = 0;

  size_t GetSize(const TopicSendMessage &obj) const override;
  bool Serialize(const TopicSendMessage &obj, char *buffer, size_t size) const override;
  next::sdk::events::UpdateTopicCache update_topic_cache_event_{"update topic cache event"};
  bool forced_registration_update_{false};
};

} // namespace ecal_util
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_UDEX_DYNAMIC_PUBLISHER_H
