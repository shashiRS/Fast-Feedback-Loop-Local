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
#ifndef SESSION_SERVER_IMPL_H_
#define SESSION_SERVER_IMPL_H_
#include <json/json.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <next/sdk/events/session_event.hpp>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace appsupport {
namespace session {

constexpr const static int kServerUpdateMin = 5;

class SessionServer_Impl {
public:
  SessionServer_Impl() = delete;
  SessionServer_Impl(const std::string &name, const size_t &user_hash, const std::string &version);
  virtual ~SessionServer_Impl();
  bool StartSession();
  bool StopSession();
  std::string getSessionInformation();

protected:
  bool updateSessionInformation();
  void RunSessionServer();

  Json::Value createTelemetryData();
  Json::Value fillLogAttributes();
  Json::Value fillScopeLogs();
  Json::Value fillResourceLogs();
  Json::FastWriter writer_;
  next::sdk::events::SessionInformation session_info_;
  Json::Value getLogEntry(const std::string &key, const std::string &value);
  Json::Value getLogEntry(const std::string &key, const int &value);

private:
  std::string name_;
  next::sdk::events::SessionEvent session_event_;
  std::atomic<bool> close_threads_{false};
  std::thread server_thread_;
  std::mutex mut_publisher_thread_;
  std::condition_variable cond_publisher_thread_;
};
} // namespace session
} // namespace appsupport
} // namespace next

#endif // !SESSION_SERVER_IMPL_H_
