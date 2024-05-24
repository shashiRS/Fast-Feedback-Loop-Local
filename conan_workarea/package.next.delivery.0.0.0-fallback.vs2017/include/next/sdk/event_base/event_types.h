/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_types.h
 * @brief    Implementation for event publisher
 *
 **/
#ifndef NEXTSDK_EVENT_TYPES_H_
#define NEXTSDK_EVENT_TYPES_H_

#include <functional>
#include <sstream> // this makes payload_type available

namespace next {
namespace sdk {
namespace events {

using payload_type = std::vector<uint8_t>;

//! Type used for event connection. Parameters are:
//! the message payload, publisher send time in µs, publisher id (SetId())
using event_callback_type = std::function<void(const payload_type &, long long, long long)>;
using subscribe_event_callback = std::function<void(std::string)>;
using sync_event_callback_type = std::function<void(const payload_type &, std::string &)>;

//! type used for event Hooks
using notify_hook = std::function<void(void)>;
using subs_event_hook = std::function<void(const std::string &)>;
using notify_sync_hook = std::function<void(const payload_type &, std::string &)>;

struct RegisteredEvent {
  size_t unique_id;
  event_callback_type event_callback{nullptr};
  subscribe_event_callback subsciber_added_callback{nullptr};
  sync_event_callback_type sync_event_callback{nullptr};
  bool is_pull_requested{false};
};

inline payload_type StringToPayload(const std::string &string) { return payload_type(string.begin(), string.end()); }

inline std::string PayloadToString(const payload_type &payload_in) {
  return std::string(payload_in.begin(), payload_in.end());
}

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_EVENT_TYPES_H_
