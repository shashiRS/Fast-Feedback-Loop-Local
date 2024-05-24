/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_collector_impl.hpp
 * @brief    collect log messages and combine them in one log
 *
 **/

#ifndef NEXT_CONTROL_LOG_COLLECTOR_IMPL_H
#define NEXT_CONTROL_LOG_COLLECTOR_IMPL_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>

#include <spdlog/details/periodic_worker.h>

#include <next/sdk/event_base/event_types.h>
#include <next/sdk/events/log_event.hpp>

#include "rotating_file_sink_log_event_data.hpp"

namespace next {
namespace sdk {
namespace events {
class LogEvent;
} // namespace events
} // namespace sdk
} // namespace next

namespace next {
namespace control {
namespace log_collector {

//! This class to receive log message from LogEvents and combine them in one log
class LogCollectorImpl {
public:
  LogCollectorImpl(const std::string &path_combined_log_file, const std::string &component_name,
                   next::sdk::logger::LOGLEVEL flush_level, std::chrono::seconds cycle_time_flush_log_file);
  LogCollectorImpl(const std::string &path_combined_log_file, const std::string &component_name,
                   std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call,
                   next::sdk::logger::LOGLEVEL flush_level, std::chrono::seconds cycle_time_flush_log_file);
  virtual ~LogCollectorImpl();

  //! startup and register everything needed
  bool Setup();
  bool Shutdown();

  void AddLogEventDataProcessingHook(std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call);

private:
  void LogStartLogCollector();
  void LogEndLogCollector();
  void StartPeriodicFlushFileSink(std::chrono::seconds cycle_time_flush_log_file);

  void CollectLogMessagesEventHook();

  std::string component_name_{""};

  next::sdk::events::LogEvent log_event_{""};

  std::mutex mutex_collect_log_messages_;

  enum class status { INIT, RUNNING };
  status log_collector_status_{status::INIT};

  std::mutex flusher_mutex_;
  std::unique_ptr<spdlog::details::periodic_worker> periodic_flusher_;
  std::chrono::seconds cycle_time_flush_log_file_;

  spdlog::sinks::RotatingFileSinkLogEventData file_sink_;

  std::vector<std::function<void(const std::vector<next::sdk::events::LogEventData> &)>>
      log_event_data_processing_hooks_;
};

} // namespace log_collector
} // namespace control
} // namespace next

#endif // NEXT_CONTROL_LOG_COLLECTOR_IMPL_H
