/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * LIBRARY: Next-UDex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     profiler_test.cpp
 * @brief    simulating / faking using the SDK as library within another library, which will then be used within an
 * application
 */
#include <fstream>
#include <iostream>
#include <regex>

#include <boost/filesystem.hpp>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include "profiling_library.hpp"

// we will not use everything of the logger namespace
#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/profiler/profile_tags.hpp>
#include <next/sdk/sdk.hpp>

namespace logger = next::sdk::logger; // make it more handy
using next::sdk::InitChain;

const char *logfilename = "test_file.log";

class TestEnvironment : public ::testing::Environment {
public:
  // Global SetUp
  void SetUp() override {
    if (boost::filesystem::exists(logfilename)) {
      boost::filesystem::remove(logfilename);
    }
  }
};

TestEnvironment *const testEnvironment =
    dynamic_cast<TestEnvironment *>(::testing::AddGlobalTestEnvironment(new TestEnvironment));

class MyTestFixture : public ::testing::Test {
public:
  MyTestFixture() : sbuf{nullptr}, ebuf{nullptr} {
    next::sdk::logger::register_to_init_chain();
    next::sdk::profiler::register_to_init_chain();

    InitChain::ConfigMap initmap;
    initmap.emplace(logger::getFilenameCfgKey(), "test_file.log");
#ifdef _DEBUG
    initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::FULL);
#else
    initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::OVERVIEW);
#endif
    initmap.emplace(logger::getLogRouterDisableCfgKey(), true);

    next::sdk::InitChain::Run(initmap);

    next::sdk::shutdown_handler::register_shutdown_handler();
  }

protected:
  // Called before each unit test
  void SetUp() override {
    // Save cout's buffer...
    sbuf = std::cout.rdbuf();
    ebuf = std::cerr.rdbuf();

    // Redirect cout to our stringstream buffer or any other ostream
    std::cout.rdbuf(buffer.rdbuf());
    std::cerr.rdbuf(ebuffer.rdbuf());
  }

  // Called after each unit test
  void TearDown() override {
    // When done redirect cout to its old self
    std::cout.rdbuf(sbuf);
    std::cerr.rdbuf(ebuf);
    sbuf = nullptr;
    ebuf = nullptr;
  }

  // This can be an ofstream as well or any other ostream
  std::stringstream buffer{};
  std::stringstream ebuffer{};

  // Save cout's buffer here
  std::streambuf *sbuf;
  std::streambuf *ebuf;
};

TEST_F(MyTestFixture, Square_4_16) {
  ProfileBridges_FLL;
  EXPECT_DOUBLE_EQ(square(4.0), 16.0);
}

TEST_F(MyTestFixture, sdk_library_initializing_test) {
  std::regex regex_expression_logger_info{"^\\[\\d\\d-\\d\\d-\\d\\d\\]\\[\\d\\d:\\d\\d:\\d\\d\\.\\d\\d\\d\\]\\[thread "
                                          "\\d+\\]\\[.*\\.(cpp|hpp)\\]\\[(info|warning|error|debug)\\].*"};

  logger::info(__FILE__, "a {0} message", "info");

  std::ifstream file(logfilename);
  std::string logfileline;

  while (std::getline(file, logfileline)) {
    EXPECT_TRUE(std::regex_match(logfileline, regex_expression_logger_info));
  }
}
