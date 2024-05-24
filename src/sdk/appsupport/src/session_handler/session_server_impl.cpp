/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     session_server_impl.hpp
 * @brief    Implementation of the session information distribution.
 *
 * Implementation of the functions for gathering and distributing session information.
 *
 */

#include "session_server_impl.hpp"
#include <boost/filesystem.hpp>
#include <chrono>
#include "../license_checker/license_checker.hpp"

namespace next {
namespace appsupport {
namespace session {
SessionServer_Impl::SessionServer_Impl(const std::string &name, const size_t &user_hash, const std::string &version)
    : name_(name), session_event_("") {

// Set Platform
#ifdef __linux__
  session_info_.platform = "Linux";
#elif _WIN32
  session_info_.platform = "Windows";
#endif

  session_info_.session_id =
      std::to_string(user_hash) + std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                     std::chrono::system_clock::now().time_since_epoch())
                                                     .count());
  session_info_.user_hash = std::to_string(user_hash);
  session_info_.version = version;
  session_info_.heartbeat = 1;
  StartSession();
}
SessionServer_Impl::~SessionServer_Impl() { StopSession(); }
bool SessionServer_Impl::StartSession() {
  auto hook = std::bind(&SessionServer_Impl::updateSessionInformation, this);
  session_event_.addEventHook(hook);
  session_event_.subscribe();
  close_threads_ = false;
  server_thread_ = std::thread([this] { RunSessionServer(); });
  return true;
}

bool SessionServer_Impl::StopSession() {
  close_threads_ = true;
  cond_publisher_thread_.notify_all();
  if (server_thread_.joinable()) {
    server_thread_.join();
  }
  return true;
}

void SessionServer_Impl::RunSessionServer() {
  while (!close_threads_) {
    std::unique_lock<std::mutex> locker(mut_publisher_thread_);
    cond_publisher_thread_.wait_for(locker, std::chrono::minutes(kServerUpdateMin),
                                    [this]() { return close_threads_ == true; });
    session_info_.heartbeat += 1;
  }
}

std::string SessionServer_Impl::getSessionInformation() {

  boost::filesystem::path home_dir;

  if (session_info_.platform == "Windows") {
    home_dir = std::getenv("USERPROFILE");
  } else {
    home_dir = std::getenv("HOME");
  }
  boost::filesystem::path json_path = home_dir / ".next" / "license.json";

  next::appsupport::license::LicenseChecker checker;

  auto json_content = checker.OpenJson(json_path.string());
  auto checker_result = checker.ParseJson(json_content);

  if (checker_result) {
    // telemetry enabled
    return writer_.write(createTelemetryData());
  } else {
    // telemetry is disabled return empty string
    return "";
  }
}

bool SessionServer_Impl::updateSessionInformation() {
  next::sdk::events::SessionClientInformation received_msg;
  received_msg = session_event_.getEventData();
  if (received_msg.shutdown) {
    session_info_.component_map.erase(received_msg.component_name);
  } else {
    session_info_.component_map[received_msg.component_name] = received_msg;
  }
  return true;
}

Json::Value SessionServer_Impl::createTelemetryData() {
  Json::Value telemetry_data;
  telemetry_data["resourceLogs"].append(fillResourceLogs());
  return telemetry_data;
}

Json::Value SessionServer_Impl::fillResourceLogs() {
  Json::Value resource_logs;
  Json::Value resource_attribute;
  resource_attribute["key"] = "service.name";
  resource_attribute["value"]["stringValue"] = "Next_telemetry_logs";
  resource_logs["resource"]["attributes"].append(resource_attribute);
  resource_logs["scopeLogs"].append(fillScopeLogs());
  return resource_logs;
}

Json::Value SessionServer_Impl::fillScopeLogs() {
  Json::Value scope_attribute;
  scope_attribute["key"] = "my.scope.attribute";
  scope_attribute["value"]["stringValue"] = "Next Telemetry Scope";

  Json::Value scope_logs;
  scope_logs["scope"]["name"] = "my.Library";
  scope_logs["scope"]["version"] = "1.0.0.";
  scope_logs["scope"]["attributes"].append(scope_attribute);

  scope_logs["logRecords"].append(fillLogAttributes());
  return scope_logs;
}

Json::Value SessionServer_Impl::fillLogAttributes() {
  Json::Value log_records;
  log_records["timeUnixNano"] =
      std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  log_records["severityText"] = "Information";
  log_records["attributes"].append(getLogEntry("SessionID", session_info_.session_id));
  log_records["attributes"].append(getLogEntry("UserHash", session_info_.user_hash));
  log_records["attributes"].append(getLogEntry("Platform", session_info_.platform));
  log_records["attributes"].append(getLogEntry("ErrorLog", "All Good"));
  log_records["attributes"].append(getLogEntry("Version", session_info_.version));
  log_records["attributes"].append(getLogEntry("Heartbeat", session_info_.heartbeat));
  log_records["attributes"].append(getLogEntry("RunningNodes", (int)session_info_.component_map.size() + 1));
  return log_records;
}

Json::Value SessionServer_Impl::getLogEntry(const std::string &key, const std::string &value) {
  Json::Value entry;
  entry["key"] = key;
  entry["value"]["stringValue"] = value;
  return entry;
}
Json::Value SessionServer_Impl::getLogEntry(const std::string &key, const int &value) {
  Json::Value entry;
  entry["key"] = key;
  entry["value"]["intValue"] = value;
  return entry;
}

} // namespace session
} // namespace appsupport
} // namespace next
