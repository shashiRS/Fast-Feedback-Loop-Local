/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_SDK
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_event.hpp
 * @brief    log event is the event which would send and receive logs
 *
 **/
#ifndef NEXT_SDK_LOG_EVENT_H_
#define NEXT_SDK_LOG_EVENT_H_

#include <string>
#include <vector>

#include "../event_base/base_event_binary.h"
#include "../logger/logger_data_types.hpp"
#include "../sdk_config.hpp"
#include "../sdk_macros.h"

namespace next {
namespace sdk {
namespace events {

struct LogEventData {
  long long time_stamp = 0;
  size_t thread_id = 0;
  std::string component_name;
  std::string channel_name; // Grouping of log messages. Can left empty.
  next::sdk::logger::LOGLEVEL log_level;
  std::string log_message;
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk LogEvent : public BaseEventBinary<std::vector<LogEventData>> {
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  LogEvent(const std::string &name = "") : BaseEventBinary<std::vector<LogEventData>>(name) {}
  void publishLogEvent(const std::vector<LogEventData> &queue_log_data);

protected:
  payload_type serializeEventPayload(const std::vector<LogEventData> &message) const override;
  std::vector<LogEventData> deserializeEventPayload(const payload_type &buf) const override;
};
} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_SDK_LOG_EVENT_H_
