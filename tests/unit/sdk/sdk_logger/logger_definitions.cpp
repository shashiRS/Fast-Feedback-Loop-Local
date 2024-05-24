/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     logger_definitions.cpp
 *  @brief    Testing the availability of the logger definitions
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/logger/logger.hpp>

TEST(LoggerDefinitions, definitions_available) {
  // just call the functions to check that they are available

  debug(__FILE__, "test");
  info(__FILE__, "test");
  warn(__FILE__, "test");
  error(__FILE__, "test");

  EXPECT_TRUE(true); // dummy test
}
