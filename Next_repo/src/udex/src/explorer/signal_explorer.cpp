/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * @file     data_explorer.cpp
 * @brief    class to handle signal tree exploring
 */

#include <next/udex/explorer/signal_explorer.hpp>

#include "signal_explorer_impl.h"

namespace next {
namespace udex {
namespace explorer {

SignalExplorer::SignalExplorer() { impl_ = std::make_unique<SignalExplorerImpl>(); }

SignalExplorer::~SignalExplorer() { impl_.reset(); }

std::vector<ChildInfo> SignalExplorer::GetChildByUrl(std::string parent_url) {
  return impl_->GetChildByUrl(parent_url);
}

std::vector<std::string> SignalExplorer::GetPackageUrl(std::string signal_url) {
  return impl_->GetPackageUrl(signal_url);
}

std::unordered_map<std::string, std::vector<std::string>> SignalExplorer::GetFullUrlTree() {
  return impl_->GetFullUrlTree();
}

SignalDescription SignalExplorer::getSignalDescription(const std::string &url, bool &available) {
  return impl_->getSignalDescription(url, available);
}

std::pair<bool, std::string> SignalExplorer::generateSdl(const std::string topic) { return impl_->generateSdl(topic); }

std::vector<std::string> SignalExplorer::getDeviceByFormatType(const std::string &format) {
  return impl_->getDeviceByFormatType(format);
}

std::vector<std::string> SignalExplorer::searchSignalTree(std::string keyword) {
  return impl_->searchSignalTree(keyword);
}

void SignalExplorer::EnableFastSearch() { return impl_->EnableFastSearch(); }

} // namespace explorer
} // namespace udex
} // namespace next
