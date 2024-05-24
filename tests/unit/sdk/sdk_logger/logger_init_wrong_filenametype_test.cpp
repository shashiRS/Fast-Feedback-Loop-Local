/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     logger_initialization_wrong_filenametype_test.cpp
 *  @brief    Test that on wrong given filename type the init shall fail
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/init_chain/simple_init_chain.hpp>
#include <next/sdk/logger/logger.hpp>

using next::sdk::InitChain;
namespace logger = next::sdk::logger;

TEST(LoggerInitializationTest, InvalidLogfileValueType_InitFails) {
  logger::register_to_init_chain();
  InitChain::ConfigMap initmap;
  initmap.emplace(logger::getFilenameCfgKey(), int(5));
  EXPECT_FALSE(InitChain::Run(initmap)) << "use default file name failed";
}
