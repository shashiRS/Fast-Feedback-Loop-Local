/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     update_topic_cache.h
 * @brief    event emmitted by nodes when publishers created/deleted
 *
 **/
#ifndef NEXT_CORE_SDK_UDAPTE_TOPIC_CACHE_H_
#define NEXT_CORE_SDK_UDAPTE_TOPIC_CACHE_H_

#include <memory>
#include "../event_base/base_event_binary.h"
#include "../sdk_config.hpp"
#include "../sdk_macros.h"

namespace next {
namespace sdk {
namespace events {
/*!
 * @brief data the send event contains
 * @brief Structure of the NEXT Update Topic Cache messages for SignalWacther
 *
 */
struct UpdateTopicCacheMessage {
  // the list is filled when we want to remove topics
  std::vector<std::string> topics;
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class DECLSPEC_nextsdk UpdateTopicCache : public next::sdk::events::BaseEventBinary<UpdateTopicCacheMessage> {

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  UpdateTopicCache(std::string name) : BaseEventBinary<UpdateTopicCacheMessage>(name) {}

protected:
  payload_type serializeEventPayload(const UpdateTopicCacheMessage &message) const override;
  UpdateTopicCacheMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_CORE_SDK_UDAPTE_TOPIC_CACHE_H_
