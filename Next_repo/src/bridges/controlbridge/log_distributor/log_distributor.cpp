/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     log_distributor.cpp
 * @brief    sends out log message via websocket
 **/

#include "log_distributor.hpp"

#include <json/json.h>

namespace next {
namespace controlbridge {

next::sdk::logger::LOGLEVEL MaxLogLevel(const std::vector<next::sdk::events::LogEventData> &log_event_data_list) {

  next::sdk::logger::LOGLEVEL max_log_level = next::sdk::logger::LOGLEVEL::DEBUG;
  for (const auto &log_event : log_event_data_list) {
    if (log_event.log_level > max_log_level) {
      max_log_level = log_event.log_level;
    }
  }
  return max_log_level;
}

LogDistributor::LogDistributor(next::sdk::logger::LOGLEVEL flush_level, std::chrono::milliseconds wait_time) {
  flush_level_ = flush_level;
  wait_time_.store(wait_time);
  send_json_to_gui_thread_ = std::thread([&] { SendJsonToGUI(); });
}

LogDistributor::LogDistributor(std::shared_ptr<next::bridgesdk::webserver::IWebServer> webserver_in,
                               next::sdk::logger::LOGLEVEL flush_level, std::chrono::milliseconds wait_time) {
  webserver_ = webserver_in;
  flush_level_ = flush_level;

  wait_time_.store(wait_time);
  send_json_to_gui_thread_ = std::thread([&] { SendJsonToGUI(); });
}

LogDistributor::~LogDistributor() {
  send_data_.store(false);
  cond_wait_send_data_.notify_one();
  send_json_to_gui_thread_.join();
}

void LogDistributor::SendLogEventData(const std::vector<next::sdk::events::LogEventData> &log_event_data_list) {

  std::lock_guard<std::mutex> lock(mutex_log_event_data_);
  log_event_data_.insert(log_event_data_.end(), log_event_data_list.begin(), log_event_data_list.end());

  if (MaxLogLevel(log_event_data_list) >= flush_level_) {
    abort_wait_ = true;
    cond_wait_send_data_.notify_one();
  }
}

void LogDistributor::CreateJsonMessagePayload(const std::vector<next::sdk::events::LogEventData> &log_event_data_list,
                                              std::vector<uint8_t> &json_payload) {
  Json::Value root;
  Json::Value payload;

  root["channel"] = "logger";
  root["source"] = "NextBridge";
  root["event"] = "UpdateLogInfo";

  for (const auto &log_event : log_event_data_list) {
    Json::Value log_event_data;
    int64_t seconds =
        log_event.time_stamp * std::chrono::system_clock::period::num / (std::chrono::system_clock::period::den);
    // convert back to get ticks in seconds without decimal places
    auto ticksOnlySec = seconds * std::chrono::system_clock::period::den / std::chrono::system_clock::period::num;
    uint32_t nanoseconds = static_cast<uint32_t>(log_event.time_stamp - ticksOnlySec);

    Json::Value json_timestamp;
    json_timestamp["sec"] = Json::Value(seconds);
    json_timestamp["nsec"] = Json::Value(nanoseconds);

    log_event_data["timestamp"] = json_timestamp;
    int k_fix_offset_foxglove = 1;
    log_event_data["level"] = Json::Value(static_cast<int>(log_event.log_level) + k_fix_offset_foxglove);
    log_event_data["message"] = log_event.log_message;
    log_event_data["name"] = log_event.component_name;
    log_event_data["file"] = "";
    log_event_data["line"] = 0;
    log_event_data["thread_id"] = log_event.thread_id;
    log_event_data["channel_name"] = log_event.channel_name;

    root["payload"]["LogEventData"].append(log_event_data);
  }

  Json::FastWriter writer;
  std::string response = writer.write(root);

  json_payload.insert(json_payload.end(), response.begin(), response.end());
}

void LogDistributor::NotificationCallback(std::vector<uint8_t> *memory) {
  if (memory) {
    delete memory;
  }
}

void LogDistributor::SendJsonToGUI() {
  while (send_data_) {
    if (webserver_) {
      std::vector<next::sdk::events::LogEventData> log_event_data_to_send;
      {
        std::lock_guard<std::mutex> lock(mutex_log_event_data_);
        log_event_data_.swap(log_event_data_to_send);
      }
      if (log_event_data_to_send.size() > 0) {
        std::vector<uint8_t> *json_payload = new std::vector<uint8_t>();
        CreateJsonMessagePayload(log_event_data_to_send, *json_payload);

        webserver_->BroadcastMessageWithNotification(
            *json_payload, 0, {}, std::bind(&LogDistributor::NotificationCallback, this, json_payload));
      }
      std::unique_lock<std::mutex> lock_wait(mutex_wait_send_data_);
      cond_wait_send_data_.wait_for(lock_wait, wait_time_.load(),
                                    [this]() { return !send_data_ || abort_wait_.load(); });
      abort_wait_ = false;
    }
  }
}
} // namespace controlbridge
} // namespace next
