/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     logger_initialization_test.cpp
 *  @brief    Testing the logger initalization
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/init_chain/simple_init_chain.hpp>
#include <next/sdk/logger/logger.hpp>

using next::sdk::InitChain;
namespace logger = next::sdk::logger;

class LoggerInitializationTest : public ::testing::Test {
public:
  LoggerInitializationTest() { std::cout << "constructor()" << std::endl; }
  ~LoggerInitializationTest() {
    std::cout << "destructor()" << std::endl;
    InitChain::Reset();
    std::cout << "Reset() finished" << std::endl;
  }

protected:
  InitChain::ConfigMap initmap_;
};

TEST_F(LoggerInitializationTest, StringAsLogfileValueType_InitSuccessfull) {
  std::string tmp("MyLogfile.log");
  initmap_.emplace(logger::getFilenameCfgKey(), tmp);
  EXPECT_TRUE(InitChain::Run(initmap_));
}

TEST_F(LoggerInitializationTest, CharPointerAsLogfileValueType_InitSuccessfull) {
  initmap_.emplace(logger::getFilenameCfgKey(), "MyLogfile.log");
  EXPECT_TRUE(InitChain::Run(initmap_));
}
