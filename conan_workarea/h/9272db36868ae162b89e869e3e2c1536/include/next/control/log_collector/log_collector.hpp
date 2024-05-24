/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_collector.hpp
 * @brief    collect log messages and combine them in one log
 *
 **/

#ifndef NEXT_CONTROL_LOG_COLLECTOR_H
#define NEXT_CONTROL_LOG_COLLECTOR_H

#include <next/sdk/events/log_event.hpp>

#include "../control_config.h"

namespace next {
namespace sdk {
namespace events {
class LogEvent;
}
} // namespace sdk
} // namespace next

namespace next {
namespace control {
namespace log_collector {

class LogCollectorImpl;
//! This class to receive log message from LogEvents and combine them in one log
class CONTROL_LIBRARY_EXPORT LogCollector {
public:
  LogCollector(const std::string &path_combined_log_file, const std::string &component_name,
               next::sdk::logger::LOGLEVEL flush_level = next::sdk::logger::LOGLEVEL::ERR,
               std::chrono::seconds cycle_time_flush_log_file = std::chrono::seconds(1));
  LogCollector(const std::string &path_combined_log_file, const std::string &component_name,
               std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call,
               next::sdk::logger::LOGLEVEL flush_level = next::sdk::logger::LOGLEVEL::ERR,
               std::chrono::seconds cycle_time_flush_log_file = std::chrono::seconds(1));
  virtual ~LogCollector();

  //! startup and register everything needed
  bool Setup();
  bool Shutdown();

  //! Add custom hook for processing received log message from LogEvents
  void AddLogEventDataProcessingHook(std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call);

private:
  LogCollectorImpl *p_impl_ = nullptr;
};

} // namespace log_collector
} // namespace control
} // namespace next

#endif // NEXT_CONTROL_LOG_COLLECTOR_H
