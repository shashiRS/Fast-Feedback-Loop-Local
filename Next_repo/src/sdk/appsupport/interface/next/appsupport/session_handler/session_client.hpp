/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     session_client.hpp
 * @brief    Session handling for distributing information about a NEXT session to the session_server
 *
 * Include this file if you want to provide session information.
 *
 *
 */

#ifndef SESSION_CLIENT_H_
#define SESSION_CLIENT_H_

#include <string>

#include <next/appsupport/appsupport_config.hpp>

namespace next {
namespace appsupport {
namespace session {

class SessionClient_Impl;

class DECLSPEC_appsupport SessionClient {
public:
  SessionClient() = delete;
  SessionClient(const std::string &component_name);
  virtual ~SessionClient();
  bool StartSessionClient();
  bool StopSessionClient();

private:
  SessionClient_Impl *impl_ = nullptr;
};

} // namespace session
} // namespace appsupport
} // namespace next

#endif // !SESSION_CLIENT_H_
