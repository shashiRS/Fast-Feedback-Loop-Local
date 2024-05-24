/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     configuration_test.cpp
 *  @brief    Testing the configuration interface
 */

#include <cip_test_support/gtest_reporting.h>
#include <nextapp/Config_Base.hpp>

TEST(configuration_test, first_test) {
  next::coreApp::ConfigBase::do_init("test", nullptr);

  next::coreApp::ConfigBase::getConfig()->put("local.test.int", 10);
  EXPECT_EQ(next::coreApp::ConfigBase::getConfig()->get_int("local.test.int"), 10);
  EXPECT_NE(next::coreApp::ConfigBase::getConfig()->get_int("local.test.int"), 1);
}
