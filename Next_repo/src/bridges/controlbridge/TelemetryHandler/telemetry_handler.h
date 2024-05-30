/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     telemetry_handler.h
 * @brief    Module to collect telemetry data and share it to our OpenTelemetry Server
 **/

#ifndef NEXT_TELMETRY_HANDLER_H_
#define NEXT_TELMETRY_HANDLER_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <next/appsupport/session_handler/session_server.hpp>
#include <next/sdk/user_hash/user_hash.hpp>
#include "../http_post/http_post.hpp"

namespace next {
namespace controlbridge {
namespace telmetry {

constexpr const static int kTelemetryUpdateMin = 5;

class TelemetryHandler {
public:
  TelemetryHandler(const std::string &component_name, const std::string &version);
  virtual ~TelemetryHandler();
  bool StartTelemetryDistribution();
  bool StopTelemetryDistribution();

protected:
  void SendTelemetryServer();

private:
  std::string name_;
  std::string version_;
  size_t user_hash_;
  next::controlbridge::HttpPost http_poster_;
  std::atomic<bool> close_threads_{false};
  std::thread telemetry_collector_;
  next::sdk::UserHash hash_generator_;
  std::mutex mut_publisher_thread_;
  std::condition_variable cond_publisher_thread_;
};

} // namespace telmetry
} // namespace controlbridge
} // namespace next

#endif // !NEXT_TELMETRY_HANDLER_H_
