/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     rotating_file_sink_log_event_data.hpp
 * @brief    rotating file sink for log event data
 *
 *           based on of rotating_file_sink.h from spdlog library:
 *           Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
 *           Distributed under the MIT License (http://opensource.org/licenses/MIT)
 **/

#ifndef NEXT_CONTROL_ROTATING_FILE_SINK_LOG_EVENT_DATA_H
#define NEXT_CONTROL_ROTATING_FILE_SINK_LOG_EVENT_DATA_H

#include <chrono>
#include <ctime>
#include <mutex>
#include <string>

#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/fmt_helper.h>
#include <spdlog/details/os.h>

#include <next/sdk/events/log_event.hpp>

namespace spdlog {
namespace sinks {

//
// Rotating file sink based on size
//
class RotatingFileSinkLogEventData {
public:
  RotatingFileSinkLogEventData(filename_t base_filename, std::size_t max_size, std::size_t max_files,
                               bool rotate_on_open = false,
                               next::sdk::logger::LOGLEVEL flush_level = next::sdk::logger::LOGLEVEL::ERR);
  virtual ~RotatingFileSinkLogEventData() {}
  static filename_t calc_filename(const filename_t &filename, std::size_t index);
  filename_t filename();
  void log(const std::string &component_name, const std::string &log_message,
           const next::sdk::logger::LOGLEVEL &log_level = next::sdk::logger::LOGLEVEL::INFO);
  void log(const next::sdk::events::LogEventData &log_event_data);

  void update_timestamp(const long long &timestamp);
  void flush();
  void flush_on(next::sdk::logger::LOGLEVEL log_level) { flush_level_ = log_level; }

private:
  // Rotate files:
  // log.txt -> log.1.txt
  // log.1.txt -> log.2.txt
  // log.2.txt -> log.3.txt
  // log.3.txt -> delete
  void rotate_();

  // delete the target if exists, and rename the src file  to target
  // return true on success, false otherwise.
  bool rename_file_(const filename_t &src_filename, const filename_t &target_filename);

  void format(spdlog::memory_buf_t &formatted, const next::sdk::events::LogEventData &log_event_data);
  void format(spdlog::memory_buf_t &formatted, const next::sdk::logger::LOGLEVEL &log_level, const size_t &thread_id,
              const std::string &component_name, const std::string &log_message);

  void sink_it(const next::sdk::logger::LOGLEVEL &log_level, spdlog::memory_buf_t &formatted_message);

  filename_t base_filename_;
  std::size_t max_size_;
  std::size_t max_files_;
  std::size_t current_size_;
  details::file_helper file_helper_;

  std::string eol_{spdlog::details::os::default_eol};
  std::tm cached_tm_;
  uint32_t cached_milliseconds_;
  long long last_log_timestamp_{0};

  std::mutex flush_mutex_;
  next::sdk::logger::LOGLEVEL flush_level_{level::err};
};

} // namespace sinks
} // namespace spdlog

#endif // NEXT_CONTROL_ROTATING_FILE_SINK_LOG_EVENT_DATA_H
