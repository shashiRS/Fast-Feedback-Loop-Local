/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     rotating_file_sink_log_event_data.cpp
 * @brief    rotating file sink for log event data
 *
 *           based on of rotating_file_sink-inl.h from spdlog library:
 *           Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
 *           Distributed under the MIT License (http://opensource.org/licenses/MIT)
 **/

#include "rotating_file_sink_log_event_data.hpp"

namespace spdlog {
namespace sinks {

RotatingFileSinkLogEventData::RotatingFileSinkLogEventData(filename_t base_filename, std::size_t max_size,
                                                           std::size_t max_files, bool rotate_on_open,
                                                           next::sdk::logger::LOGLEVEL flush_level)
    : base_filename_(std::move(base_filename)), max_size_(max_size), max_files_(max_files), flush_level_(flush_level) {
  file_helper_.open(calc_filename(base_filename_, 0));
  current_size_ = file_helper_.size(); // expensive. called only once
  if (rotate_on_open && current_size_ > 0) {
    rotate_();
  }

  std::memset(&cached_tm_, 0, sizeof(cached_tm_));
}

// calc filename according to index and file extension if exists.
// e.g. calc_filename("logs/mylog.txt, 3) => "logs/mylog.3.txt".
filename_t RotatingFileSinkLogEventData::calc_filename(const filename_t &filename, std::size_t index) {
  if (index == 0u) {
    return filename;
  }

  filename_t basename, ext;
  std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
  return fmt::format(SPDLOG_FILENAME_T("{}.{}{}"), basename, index, ext);
}

filename_t RotatingFileSinkLogEventData::filename() { return file_helper_.filename(); }

void RotatingFileSinkLogEventData::log(const std::string &component_name, const std::string &log_message,
                                       const next::sdk::logger::LOGLEVEL &log_level) {
  auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
  update_timestamp(timestamp);

  memory_buf_t formatted;
  format(formatted, log_level, details::os::thread_id(), component_name, log_message);

  sink_it(log_level, formatted);
}

void RotatingFileSinkLogEventData::log(const next::sdk::events::LogEventData &log_event_data) {

  update_timestamp(log_event_data.time_stamp);

  memory_buf_t formatted;
  format(formatted, log_event_data);

  sink_it(log_event_data.log_level, formatted);
}

void RotatingFileSinkLogEventData::sink_it(const next::sdk::logger::LOGLEVEL &log_level,
                                           spdlog::memory_buf_t &formatted_message) {
  current_size_ += formatted_message.size();

  if (current_size_ > max_size_) {
    rotate_();
    current_size_ = formatted_message.size();
  }
  file_helper_.write(formatted_message);

  if (log_level >= flush_level_) {
    flush();
  }
}

void RotatingFileSinkLogEventData::format(spdlog::memory_buf_t &formatted,
                                          const next::sdk::events::LogEventData &log_event_data) {
  format(formatted, log_event_data.log_level, log_event_data.thread_id, log_event_data.component_name,
         log_event_data.log_message);
}

void RotatingFileSinkLogEventData::format(spdlog::memory_buf_t &formatted, const next::sdk::logger::LOGLEVEL &log_level,
                                          const size_t &thread_id, const std::string &component_name,
                                          const std::string &log_message) {
  // set date
  formatted.push_back('[');
  details::fmt_helper::pad2(cached_tm_.tm_year % 100, formatted);
  formatted.push_back('-');
  details::fmt_helper::pad2(cached_tm_.tm_mon + 1, formatted);
  formatted.push_back('-');
  details::fmt_helper::pad2(cached_tm_.tm_mday, formatted);

  details::fmt_helper::append_string_view("][", formatted);

  // set time
  details::fmt_helper::pad2(cached_tm_.tm_hour, formatted);
  formatted.push_back(':');
  details::fmt_helper::pad2(cached_tm_.tm_min, formatted);
  formatted.push_back(':');
  details::fmt_helper::pad2(cached_tm_.tm_sec, formatted);
  formatted.push_back('.');
  details::fmt_helper::pad3(static_cast<uint32_t>(cached_milliseconds_), formatted);

  // set thread id
  details::fmt_helper::append_string_view("][thread ", formatted);
  details::fmt_helper::append_int(thread_id, formatted);

  // set log level
  std::string log_level_formatted = "";
  auto it_log_level = next::sdk::logger::kLogLevelLowerCase.find(log_level);
  if (it_log_level != next::sdk::logger::kLogLevelLowerCase.end()) {
    log_level_formatted = "][" + it_log_level->second + "][";
  } else {
    log_level_formatted = "][unkown level][";
  }
  auto *buf_ptr_log_level = log_level_formatted.data();
  formatted.append(buf_ptr_log_level, buf_ptr_log_level + log_level_formatted.size());

  // set component name
  auto *buf_ptr_comp_name = component_name.data();
  formatted.append(buf_ptr_comp_name, buf_ptr_comp_name + component_name.size());
  details::fmt_helper::append_string_view("] ", formatted);

  // set log message
  auto *buf_ptr_msg = log_message.data();
  formatted.append(buf_ptr_msg, buf_ptr_msg + log_message.size());

  // write eol
  details::fmt_helper::append_string_view(eol_, formatted);
}

void RotatingFileSinkLogEventData::update_timestamp(const long long &timestamp) {

  auto timestamp_milliseconds =
      timestamp * std::chrono::system_clock::period::num / (std::chrono::system_clock::period::den / 1000);
  if (timestamp_milliseconds != last_log_timestamp_) {
    auto seconds = timestamp_milliseconds / 1000;
    // convert back to get ticks in seconds without decimal places
    auto ticksOnlySec = seconds * std::chrono::system_clock::period::den / std::chrono::system_clock::period::num;
    // extract milliseconds
    cached_milliseconds_ = static_cast<uint32_t>((timestamp - ticksOnlySec) * std::chrono::system_clock::period::num /
                                                 (std::chrono::system_clock::period::den / 1000));

    cached_tm_ = details::os::localtime(seconds);
    last_log_timestamp_ = timestamp_milliseconds;
  }
}

void RotatingFileSinkLogEventData::flush() {
  std::lock_guard<std::mutex> lock(flush_mutex_);
  file_helper_.flush();
}

// Rotate files:
// log.txt -> log.1.txt
// log.1.txt -> log.2.txt
// log.2.txt -> log.3.txt
// log.3.txt -> delete
void RotatingFileSinkLogEventData::rotate_() {
  using details::os::filename_to_str;
  using details::os::path_exists;
  file_helper_.close();
  for (auto i = max_files_; i > 0; --i) {
    filename_t src = calc_filename(base_filename_, i - 1);
    if (!path_exists(src)) {
      continue;
    }
    filename_t target = calc_filename(base_filename_, i);

    if (!rename_file_(src, target)) {
      // if failed try again after a small delay.
      // this is a workaround to a windows issue, where very high rotation
      // rates can cause the rename to fail with permission denied (because of antivirus?).
      details::os::sleep_for_millis(100);
      if (!rename_file_(src, target)) {
        file_helper_.reopen(true); // truncate the log file anyway to prevent it to grow beyond its limit!
        current_size_ = 0;
        throw_spdlog_ex(
            "rotating_file_sink: failed renaming " + filename_to_str(src) + " to " + filename_to_str(target), errno);
      }
    }
  }
  file_helper_.reopen(true);
}

// delete the target if exists, and rename the src file  to target
// return true on success, false otherwise.
bool RotatingFileSinkLogEventData::rename_file_(const filename_t &src_filename, const filename_t &target_filename) {
  // try to delete the target file in case it already exists.
  (void)details::os::remove(target_filename);
  return details::os::rename(src_filename, target_filename) == 0;
}

} // namespace sinks
} // namespace spdlog
