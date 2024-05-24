/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_collector.cpp
 * @brief    collect log messages and combine them in one log
 **/

#include <next/control/log_collector/log_collector.hpp>
#include "log_collector_impl.hpp"

namespace next {
namespace control {
namespace log_collector {

LogCollector::LogCollector(const std::string &path_combined_log_file, const std::string &component_name,
                           next::sdk::logger::LOGLEVEL flush_level, std::chrono::seconds flushing_interval_Log_file) {
  p_impl_ = new LogCollectorImpl(path_combined_log_file, component_name, flush_level, flushing_interval_Log_file);
}

LogCollector::LogCollector(const std::string &path_combined_log_file, const std::string &component_name,
                           std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call,
                           next::sdk::logger::LOGLEVEL flush_level, std::chrono::seconds flushing_interval_Log_file) {
  p_impl_ =
      new LogCollectorImpl(path_combined_log_file, component_name, f_call, flush_level, flushing_interval_Log_file);
}

LogCollector::~LogCollector() { delete p_impl_; }

bool LogCollector::Setup() { return p_impl_->Setup(); }

bool LogCollector::Shutdown() { return p_impl_->Shutdown(); }

void LogCollector::AddLogEventDataProcessingHook(
    std::function<void(const std::vector<next::sdk::events::LogEventData> &)> f_call) {
  p_impl_->AddLogEventDataProcessingHook(f_call);
}

} // namespace log_collector
} // namespace control
} // namespace next
