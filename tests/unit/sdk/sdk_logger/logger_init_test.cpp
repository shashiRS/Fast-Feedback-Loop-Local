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
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;
namespace logger = next::sdk::logger;

class LoggerInitializationTest : public ::testing::Test {
public:
  LoggerInitializationTest() {
    logger::register_to_init_chain();
    // disable the logrouter as we do not add the event system properly
    initmap_.emplace(next::sdk::logger::getLogRouterDisableCfgKey(), true);
    std::cout << "constructor()" << std::endl;
  }

  ~LoggerInitializationTest() {
    std::cout << "destructor()" << std::endl;
    InitChain::Reset();
    std::cout << "Reset() finished" << std::endl;
  }

protected:
  InitChain::ConfigMap initmap_;
};

TEST_F(LoggerInitializationTest, StringAsLogfileValueType_InitSuccessfull) {
  initmap_.emplace(logger::getFilenameCfgKey(), (std::string) "MyLogfile.log");

  EXPECT_TRUE(InitChain::Run(initmap_));
}

TEST_F(LoggerInitializationTest, CharPointerAsLogfileValueType_InitSuccessfull) {
  initmap_.emplace(logger::getFilenameCfgKey(), "MyLogfile.log");

  EXPECT_TRUE(InitChain::Run(initmap_));
}

TEST_F(LoggerInitializationTest, StringAsComponentNameValueType_InitSuccessfull) {
  initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), (std::string) "LoggerInitializationTest");

  EXPECT_TRUE(InitChain::Run(initmap_));
}

TEST_F(LoggerInitializationTest, CharPointerAsComponentNameValueType_InitSuccessfull) {
  initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), "LoggerInitializationTest");

  EXPECT_TRUE(InitChain::Run(initmap_));
}

TEST_F(LoggerInitializationTest, StringAsSantinelValueType_InitSuccessfull) {
  initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), (std::string) "LoggerInitializationTest");
  initmap_.emplace(logger::getSentinelProductNameCfgKey(), (std::string) "LoggerInitializationTest");

  EXPECT_TRUE(InitChain::Run(initmap_));
}

TEST_F(LoggerInitializationTest, CharPointerAsSantinelValueType_InitSuccessfull) {
  initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), (std::string) "LoggerInitializationTest");
  initmap_.emplace(logger::getSentinelProductNameCfgKey(), "LoggerInitializationTest");

  EXPECT_TRUE(InitChain::Run(initmap_));
}
