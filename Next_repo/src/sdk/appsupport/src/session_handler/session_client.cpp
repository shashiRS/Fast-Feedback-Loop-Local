/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     session_client.cpp
 * @brief    functions for session information propagation
 *
 **/

#include <next/appsupport/session_handler/session_client.hpp>

#include "session_client_impl.hpp"

namespace next {
namespace appsupport {
namespace session {

SessionClient::SessionClient(const std::string &component_name) {
  this->impl_ = new SessionClient_Impl(component_name);
}

SessionClient::~SessionClient() {
  if (impl_) {
    delete this->impl_;
  }
}

bool SessionClient::StartSessionClient() { return this->impl_->StartSessionClient(); }
bool SessionClient::StopSessionClient() { return this->impl_->StopSessionClient(); }

} // namespace session
} // namespace appsupport
} // namespace next
