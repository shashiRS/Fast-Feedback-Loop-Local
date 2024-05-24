/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     telemetry_handler.cpp
 * @brief    Implementation to collect telemetry data and share it to our OpenTelemetry Server
 **/

#include "telemetry_handler.h"
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace controlbridge {
namespace telmetry {

// add constant to save update rate

TelemetryHandler::TelemetryHandler(const std::string &component_name, const std::string &version)
    : http_poster_("next-telemetry-dev-coll.cmo.conti.de", "/v1/logs") {
  name_ = component_name;
  version_ = version;
  hash_generator_.CreateHash(user_hash_);
  StartTelemetryDistribution();
}

TelemetryHandler::~TelemetryHandler() { StopTelemetryDistribution(); }

bool TelemetryHandler::StartTelemetryDistribution() {
  close_threads_ = false;
  telemetry_collector_ = std::thread([this] { SendTelemetryServer(); });
  return true;
}

bool TelemetryHandler::StopTelemetryDistribution() {
  close_threads_ = true;
  cond_publisher_thread_.notify_all();
  if (telemetry_collector_.joinable()) {
    telemetry_collector_.join();
  }
  return true;
}

void TelemetryHandler::SendTelemetryServer() {
  next::appsupport::session::SessionServer session_server(name_, user_hash_, version_);
  while (!close_threads_) {
    auto msg = session_server.getSessionInformation();

    if (!msg.empty()) {
      // publish results
      http_poster_.PostJsonString(msg);
    }
    std::unique_lock<std::mutex> locker(mut_publisher_thread_);
    cond_publisher_thread_.wait_for(locker, std::chrono::minutes(kTelemetryUpdateMin),
                                    [this]() { return close_threads_ == true; });
  }
}

} // namespace telmetry
} // namespace controlbridge
} // namespace next
