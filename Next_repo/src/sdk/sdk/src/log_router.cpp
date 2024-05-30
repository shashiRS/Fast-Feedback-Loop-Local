/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     log_router.cpp
 * @brief    routes the logs from spd logs using ecal publisher
 */

#include "log_router.hpp"
#include "next/sdk/logger/logger.hpp"

namespace next {
namespace sdk {
namespace logger {

LogRouter::LogRouter(const std::string &component_name) : component_name_(component_name) {

  using namespace std::chrono_literals;
  publisher_thread_ = std::move(std::thread(&LogRouter::publishLogData, this));
  if (!log_event_publisher_) {
    log_event_publisher_ = std::make_unique<next::sdk::events::LogEvent>("");
  }

  read_buffer_ = true;

  /*
  TODO: That this needs to be implemented later.
  Observation: CPublishers addCallback function is called everytime when publisher sends payload
  and somehow callback of subscriber isn't, which is weird and we need to discuss this scenario if
  the observed behaviour is correct or not.*/

  /*
  log_event_publisher_->addListenerAddedHook([this](const std::string &topic_name) {
    (void)topic_name;
    read_buffer_ = true;
    cond_buffer_.notify_all();
  });
  */
}

LogRouter::~LogRouter() {
  exit_from_reader_thread_ = true;
  publisher_exit_ = true;
  read_buffer_ = true;
  cond_buffer_.notify_all();
  publisher_thread_.join();
  flush();
}

// writer thread
void LogRouter::log(const spdlog::details::log_msg &msg) {
  next::sdk::events::LogEventData log_event_data;
  log_event_data.log_level = next::sdk::logger::getLogLevelFromSpd(msg.level);
  log_event_data.log_message.assign(msg.payload.begin(), msg.payload.end());
  log_event_data.thread_id = msg.thread_id;
  log_event_data.time_stamp = msg.time.time_since_epoch().count();
  log_event_data.component_name = component_name_;

  std::lock_guard<std::mutex> u_lock(mut_buffer_);
  buffer_.push_back(log_event_data);
  cond_buffer_.notify_all();
}

void LogRouter::flush() {

  std::vector<next::sdk::events::LogEventData> tmp;
  {
    std::lock_guard<std::mutex> lock(mut_buffer_);
    if (buffer_.size() == 0)
      return;
    buffer_.swap(tmp);
  }

  {
    std::lock_guard lock_pub(mut_publisher_);
    if (log_event_publisher_) {
      log_event_publisher_->publishLogEvent(tmp);
    }
  }
  cond_buffer_.notify_all();
}

void LogRouter::set_pattern(const std::string &pattern) { (void)pattern; }

void LogRouter::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) { (void)sink_formatter; }

// reader thread
void LogRouter::publishLogData() {
  while (!publisher_exit_) {
    std::vector<next::sdk::events::LogEventData> tmp;
    std::unique_lock<std::mutex> locker(mut_buffer_);
    cond_buffer_.wait(locker, [this]() { return (buffer_.size() != 0 && read_buffer_) || exit_from_reader_thread_; });
    buffer_.swap(tmp);
    locker.unlock();

    std::lock_guard lock_pub(mut_publisher_);
    log_event_publisher_->publishLogEvent(tmp);
  }
}

} // namespace logger
} // namespace sdk
} // namespace next
