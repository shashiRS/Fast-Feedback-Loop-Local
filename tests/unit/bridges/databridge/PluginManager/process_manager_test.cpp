/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     process_manager_test.cpp
 *  @brief    ProcessManager unit tests
 */

#include <chrono>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>

#include <next/databridge/plugin_manager.h>

#include "plugin_concurrency_mock.hpp"
#include "plugin_config_handler.h"
#include "plugin_sdl_mock.hpp"
#include "process_manager.h"
#include "unittest_mock_plugin_manager.hpp"
#include "unittest_plugin_manager_helper.hpp"

using namespace next;
using namespace next::databridge::plugin_util;
using next::test::MockPluginManager;

TYPED_TEST_CASE(MockPluginManager, next::plugins::MockSdlPlugin);

std::mutex mtx;
std::condition_variable cv;
bool dataReady = false;
std::chrono::time_point<std::chrono::steady_clock> start_time_invoke_call1;
std::chrono::time_point<std::chrono::steady_clock> start_time_invoke_call2;
std::chrono::time_point<std::chrono::steady_clock> start_time_invoke_call3;
std::chrono::time_point<std::chrono::steady_clock> start_time_invoke_call4;
void InvokFunc1() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [] { return dataReady; });
  start_time_invoke_call1 = std::chrono::steady_clock::now();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  cv.notify_one();
}
void InvokFunc2() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [] { return dataReady; });
  start_time_invoke_call2 = std::chrono::steady_clock::now();
}

void InvokFunc3() { start_time_invoke_call3 = std::chrono::steady_clock::now(); }
void InvokFunc4() { start_time_invoke_call4 = std::chrono::steady_clock::now(); }

// Test for invoke call for same process ,calls should not block another and execute sequentially
TEST(MockPluginManager, DISABLED_invokecall_test) {
  next::databridge::plugin_util::ProcessManager process_manager{};

  std::string process_name = "test_process";
  process_manager.invokeCall(std::bind(&InvokFunc1), process_name);
  dataReady = true;
  cv.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  process_manager.invokeCall(std::bind(&InvokFunc2), process_name);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  auto time_diff = std::chrono::duration<double, std::milli>(start_time_invoke_call2 - start_time_invoke_call1).count();
  EXPECT_LE(time_diff, 150);
}

TEST(MockPluginManager, invokecall_test_no_delay) {
  next::databridge::plugin_util::ProcessManager process_manager{};

  std::string process_name = "test_process";
  process_manager.invokeCall(std::bind(&InvokFunc3), process_name);
  process_manager.invokeCall(std::bind(&InvokFunc4), process_name);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  auto time_diff = std::chrono::duration<double, std::milli>(start_time_invoke_call4 - start_time_invoke_call3).count();
  EXPECT_LE(time_diff, 100);
}
