/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file  log_collector_sink.hpp
 * @brief Class wraps LogEvent to publish datalog and consumes logs from logger
 */

#ifndef NEXTSDK_LOG_COLLECTOR_SINK_H_
#define NEXTSDK_LOG_COLLECTOR_SINK_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/sink.h>

#include <next/sdk/events/log_event.hpp>

// todo: dont send logs when LogCollector at controlbridge( as of now) is down, buffer the logs as muchas queue can
// handle and once signal/cb is triggered from logcollector via controlbridge, that its alive, send allllllll
// todo: find out how to hnadle shutdown/close event subscription or notification use case

namespace next {
namespace sdk {
namespace logger {

class LogRouter : public spdlog::sinks::sink {
public:
  LogRouter(const std::string &component_name = "");
  virtual ~LogRouter();

  virtual void log(const spdlog::details::log_msg &msg) override;
  virtual void flush() override;
  virtual void set_pattern(const std::string &pattern) override;
  virtual void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;

protected:
  void publishLogData();

  std::atomic<bool> read_buffer_ = false;
  std::atomic<bool> publisher_exit_ = false, exit_from_reader_thread_ = false;

private:
  std::vector<next::sdk::events::LogEventData> buffer_;
  mutable std::mutex mut_buffer_, mut_publisher_;
  std::condition_variable cond_buffer_;
  std::thread publisher_thread_;
  std::unique_ptr<next::sdk::events::LogEvent> log_event_publisher_;
  std::string component_name_;
};

} // namespace logger
} // namespace sdk
} // namespace next

#endif //
