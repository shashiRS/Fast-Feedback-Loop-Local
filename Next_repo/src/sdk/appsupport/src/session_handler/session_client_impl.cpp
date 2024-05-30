/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     session_client_impl.hpp
 * @brief    Implementation of the session information distribution.
 *
 * Implementation of the functions for gathering and distributing session information.
 *
 */

#include "session_client_impl.hpp"
#include <chrono>

namespace next {
namespace appsupport {
namespace session {
SessionClient_Impl::SessionClient_Impl(const std::string &component_name) : name_(component_name), session_event_("") {
  session_client_info_.component_name = component_name;
  session_client_info_.heartbeat = 1;
  session_client_info_.version = "1.0";
  session_client_info_.startup_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  session_client_info_.current_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  session_client_info_.shutdown = false;
  StartSessionClient();
}
SessionClient_Impl::~SessionClient_Impl() { StopSessionClient(); }

bool SessionClient_Impl::StartSessionClient() {
  close_threads = false;
  client_thread_ = std::thread([this] { RunSessionClient(); });
  return true;
}

bool SessionClient_Impl::StopSessionClient() {
  session_client_info_.shutdown = true;
  session_client_info_.current_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  SendSessionInformation();
  close_threads = true;
  cond_publisher_thread_.notify_all();
  if (client_thread_.joinable()) {
    client_thread_.join();
  }
  return true;
}

bool SessionClient_Impl::SendSessionInformation() {
  session_event_.publish(session_client_info_);
  return true;
}

void SessionClient_Impl::RunSessionClient() {
  session_client_info_.shutdown = false;
  while (!close_threads) {
    session_client_info_.heartbeat += 1;
    session_client_info_.current_time =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    SendSessionInformation();
    std::unique_lock<std::mutex> locker(mut_publisher_thread_);
    cond_publisher_thread_.wait_for(locker, std::chrono::minutes(kClientUpdateMin),
                                    [this]() { return close_threads == true; });
  }
  session_client_info_.shutdown = true;
}
} // namespace session
} // namespace appsupport
} // namespace next
