/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_collector_impl.cpp
 * @brief    collect log messages and combine them in one log
 **/

#include "log_collector_impl.hpp"

namespace next {
namespace control {
namespace log_collector {

constexpr int kMaxFileSize = 10 * 1024 * 1024; // 10MB files size
constexpr int kMaxFileNumbers = 10;

LogCollectorImpl::LogCollectorImpl(const std::string &path_combined_log_file, const std::string &component_name,
                                   next::sdk::logger::LOGLEVEL flush_level,
                                   std::chrono::seconds cycle_time_flush_log_file)
    : component_name_(component_name), cycle_time_flush_log_file_(cycle_time_flush_log_file),
      file_sink_(path_combined_log_file, kMaxFileSize, kMaxFileNumbers, true, flush_level) {
  LogStartLogCollector();
}

LogCollectorImpl::LogCollectorImpl(const std::string &path_combined_log_file, const std::string &component_name,
                                   std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call,
                                   next::sdk::logger::LOGLEVEL flush_level,
                                   std::chrono::seconds cycle_time_flush_log_file)
    : component_name_(component_name), cycle_time_flush_log_file_(cycle_time_flush_log_file),
      file_sink_(path_combined_log_file, kMaxFileSize, kMaxFileNumbers, true, flush_level) {
  LogStartLogCollector();
  AddLogEventDataProcessingHook(f_call);
}

LogCollectorImpl::~LogCollectorImpl() {
  Shutdown();

  LogEndLogCollector();
}

void LogCollectorImpl::LogStartLogCollector() {
  file_sink_.log(component_name_, "---------------------------------------------");
  file_sink_.log(component_name_, "############# Combined log file #############");
  file_sink_.log(component_name_, "############### START/CONTINUE ##############");
  file_sink_.log(component_name_, "#############################################");
  file_sink_.flush(); // trigger flush once immediately at the beginning
}

void LogCollectorImpl::LogEndLogCollector() {
  file_sink_.log(component_name_, "#############################################");
  file_sink_.log(component_name_, "############# Combined log file #############");
  file_sink_.log(component_name_, "#################### END ####################");
  file_sink_.log(component_name_, "---------------------------------------------");
  file_sink_.flush(); // trigger flush once at the end
}

void LogCollectorImpl::StartPeriodicFlushFileSink(std::chrono::seconds cycle_time_flush_log_file) {
  file_sink_.log(component_name_, "[LogCollector] Start periodic flushing file sink");

  std::lock_guard<std::mutex> lock(flusher_mutex_);
  auto clbk = [this]() { file_sink_.flush(); };
  periodic_flusher_ = std::make_unique<spdlog::details::periodic_worker>(clbk, cycle_time_flush_log_file);
}

bool LogCollectorImpl::Setup() {
  if (status::RUNNING == log_collector_status_) {
    file_sink_.log(component_name_, "[LogCollector] ERROR: Setup already running without calling shutdown");
    return false;
  }
  log_collector_status_ = status::RUNNING;

  file_sink_.log(component_name_, "[LogCollector] Setup start");

  StartPeriodicFlushFileSink(cycle_time_flush_log_file_);

  auto event_hook = std::bind(&LogCollectorImpl::CollectLogMessagesEventHook, this);
  log_event_.subscribe();
  log_event_.addEventHook(event_hook);

  file_sink_.log(component_name_, "[LogCollector] Setup finished");

  return true;
}

bool LogCollectorImpl::Shutdown() {
  file_sink_.log(component_name_, "[LogCollector] Shutdown start");

  log_collector_status_ = status::INIT;

  file_sink_.log(component_name_, "[LogCollector] Stopp periodic flushing file sink");
  std::lock_guard<std::mutex> lock(flusher_mutex_);
  periodic_flusher_.reset();

  file_sink_.log(component_name_, "[LogCollector] Shutdown finished");

  return true;
}

void LogCollectorImpl::CollectLogMessagesEventHook() {
  std::vector<next::sdk::events::LogEventData> message_list = log_event_.getEventData();
  if (message_list.size() > 0) {
    std::lock_guard<std::mutex> lock(mutex_collect_log_messages_);
    for (const auto &message : message_list) {
      file_sink_.log(message);
    }
    for (auto const &hook : log_event_data_processing_hooks_) {
      hook(message_list);
    }
  }
}

void LogCollectorImpl::AddLogEventDataProcessingHook(
    std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call) {
  log_event_data_processing_hooks_.push_back(f_call);
}

} // namespace log_collector
} // namespace control
} // namespace next
