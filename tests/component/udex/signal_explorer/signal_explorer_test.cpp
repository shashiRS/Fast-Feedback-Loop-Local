/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     signal_explorer_test.cpp
 *  @brief    testing high-level functionality of udex
 */

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>

using next::udex::explorer::SignalExplorer;

namespace {

TEST(signal_explorer_component, SignalExplorer) { auto signal_explorer = std::make_unique<SignalExplorer>(); }

TEST(signal_explorer_component, GetChildByUrl) {
  auto signal_explorer = std::make_unique<SignalExplorer>();
  std::vector<next::udex::ChildInfo> childs_info =
      signal_explorer->GetChildByUrl("UnitTest.AlgoVehCycle.VehDyn.sSigHeader");
}

TEST(signal_explorer_component, GetPackageUrl) {
  auto signal_explorer = std::make_unique<SignalExplorer>();
  std::vector<std::string> package_url = signal_explorer->GetPackageUrl("UnitTest.AlgoVehCycle.VehDyn.sSigHeader");
}

TEST(signal_explorer_component, GetFullUrlTree) {
  auto signal_explorer = std::make_unique<SignalExplorer>();
  std::unordered_map<std::string, std::vector<std::string>> url_tree = signal_explorer->GetFullUrlTree();
}

TEST(signal_explorer_component, getSignalDescription) {
  std::string url = "url";
  bool available = false;
  auto signal_explorer = std::make_unique<SignalExplorer>();
  auto _ = signal_explorer->getSignalDescription(url, available);
}

TEST(signal_explorer_component, generateSdl) {
  auto signal_explorer = std::make_unique<SignalExplorer>();
  auto _ = signal_explorer->generateSdl("UnitTest.AlgoVehCycle.VehDyn.sSigHeader");
}

} // namespace
