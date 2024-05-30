/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     session_server.cpp
 * @brief    functions for session information collection and forwarding
 *
 **/

#include <next/appsupport/session_handler/session_server.hpp>

#include "session_server_impl.hpp"

namespace next {
namespace appsupport {
namespace session {
SessionServer::SessionServer(const std::string &name, const size_t &user_hash, const std::string &version) {
  this->impl_ = new SessionServer_Impl(name, user_hash, version);
}
SessionServer::~SessionServer() {
  if (impl_) {
    delete this->impl_;
  }

} // namespace session
bool SessionServer::StartSession() { return this->impl_->StartSession(); }
bool SessionServer::StopSession() { return this->impl_->StopSession(); }
std::string SessionServer::getSessionInformation() { return this->impl_->getSessionInformation(); }

} // namespace session
} // namespace appsupport
} // namespace next
