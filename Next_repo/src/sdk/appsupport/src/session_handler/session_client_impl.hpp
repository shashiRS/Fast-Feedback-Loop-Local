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

#ifndef SESSION_CLIENT_IMPL_H_
#define SESSION_CLIENT_IMPL_H_
#include <condition_variable>
#include <mutex>
#include <thread>

#include <next/sdk/events/session_event.hpp>

namespace next {
namespace appsupport {
namespace session {

constexpr const static int kClientUpdateMin = 2;

class SessionClient_Impl {
public:
  SessionClient_Impl() = delete;
  SessionClient_Impl(const std::string &component_name);
  virtual ~SessionClient_Impl();
  bool StartSessionClient();
  bool StopSessionClient();

protected:
  next::sdk::events::SessionClientInformation session_client_info_;

private:
  void RunSessionClient();
  bool SendSessionInformation();
  std::string name_;
  next::sdk::events::SessionEvent session_event_;
  std::atomic<bool> close_threads{false};
  std::thread client_thread_;
  std::mutex mut_publisher_thread_;
  std::condition_variable cond_publisher_thread_;
};
} // namespace session
} // namespace appsupport
} // namespace next

#endif // !session_client_impl_H_
