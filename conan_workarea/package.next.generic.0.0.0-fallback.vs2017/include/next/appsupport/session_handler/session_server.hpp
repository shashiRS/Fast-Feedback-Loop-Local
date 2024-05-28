/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     session_server.hpp
 * @brief    Session handling for collecting and distributing information about a NEXT session
 *
 * Include this file if you want to use the Next session handling.
 *
 *
 */

#ifndef SESSION_SERVER_H_
#define SESSION_SERVER_H_

#include <next/appsupport/appsupport_config.hpp>
#include <string>

namespace next {
namespace appsupport {
namespace session {

class SessionServer_Impl;

class DECLSPEC_appsupport SessionServer {
public:
  SessionServer(const std::string &name, const size_t &user_hash, const std::string &version);
  virtual ~SessionServer();
  // TODO remove and put functionality in Constructor
  bool StartSession();
  bool StopSession();
  std::string getSessionInformation();

private:
  SessionServer_Impl *impl_ = nullptr;
};

} // namespace session
} // namespace appsupport
} // namespace next

#endif // !SESSION_SERVER_H_
