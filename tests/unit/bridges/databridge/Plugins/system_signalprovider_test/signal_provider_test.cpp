/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     signal_list_test.cpp
 *  @brief    Testing the system_SignalList plugin
 */

#include <chrono>
#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
//#include "signal_list.h"

namespace next {
namespace plugins {

TEST(SignalProvider, dummy_test) { EXPECT_EQ(true, true); }

} // namespace plugins
} // namespace next
