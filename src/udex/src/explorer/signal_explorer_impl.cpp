/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     signal_explorer_impl.cpp

 * @brief    matches the old Udex implementation to the new interface with
 *           regards to the signal_explorer interface
 */

#include "signal_explorer_impl.h"

#include <cctype>
#include <chrono>
#include <iostream>

#include <next/sdk/logger/logger.hpp>

namespace next {
namespace udex {
namespace explorer {

SignalExplorerImpl::SignalExplorerImpl() {}

SignalExplorerImpl::~SignalExplorerImpl() {}

std::vector<ChildInfo> SignalExplorerImpl::GetChildByUrl(std::string parent_url) {
  ProfileUdex_FLL;
  return signal_watcher_.getChildrenByUrl(parent_url);
}

std::vector<std::string> SignalExplorerImpl::GetPackageUrl(std::string signal_url) {
  ProfileUdex_FLL;
  return signal_watcher_.getTopicByUrl(signal_url);
}

std::unordered_map<std::string, std::vector<std::string>> SignalExplorerImpl::GetFullUrlTree() {
  ProfileUdex_FLL;
  return signal_watcher_.GetFullUrlTree();
}

SignalDescription SignalExplorerImpl::getSignalDescription(const std::string &url, bool &available) {
  ProfileUdex_FLL;
  return signal_watcher_.getSignalDescription(url, available);
}

std::pair<bool, std::string> SignalExplorerImpl::generateSdl(const std::string topic) {
  ProfileUdex_FLL;
  return signal_watcher_.generateSdl(topic);
}

std::vector<std::string> SignalExplorerImpl::getDeviceByFormatType(const std::string &format) {
  ProfileUdex_FLL;
  return signal_watcher_.getDevicesByFormat(format);
}

std::vector<std::string> SignalExplorerImpl::searchSignalTree(std::string keyword) {

  std::vector<std::string> result;
  auto start = std::chrono::high_resolution_clock::now();
  result = signal_watcher_.searchSignalTree(keyword);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms_double = end - start;
  debug(__FILE__, "Execution search time for keyword {0} : {1} ms\n", keyword, ms_double.count());
  return result;
}

void SignalExplorerImpl::EnableFastSearch() { signal_watcher_.EnableFastSearch(); }

} // namespace explorer
} // namespace udex
} // namespace next
